#include "window.hpp"
#include "render.hpp"
#include "state.hpp"

#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace mdreader {

// ==============================================================================
// PRIVATE IMPLEMENTATION (PIMPL) STRUCT
// ==============================================================================
// Holds pointers to the internal GTK4 and WebKit C objects.
// Because GTK is a C library, it manages objects dynamically on the heap.
// ==============================================================================
struct NativeWindow::Impl {
    // 1. The Picture Frame: The outer OS window (title bar, borders, close button)
    GtkWidget* window{nullptr};

    // 2. The Canvas: The WebKit rendering surface (HTML, KaTeX math, Mermaid diagrams)
    GtkWidget* web_view{nullptr};

    // 3. The Heartbeat: The desktop event loop that keeps the window alive at 0% CPU
    GMainLoop* main_loop{nullptr};

    // 4. Live file watching state
    std::string watched_path{};
    std::string base_dir{};
    std::filesystem::file_time_type last_mtime{};
    guint timer_id{0};
    int save_count{0};
    int current_scroll{0};
    int max_history{DEFAULT_MAX_HISTORY};
};

// ==============================================================================
// CONSTRUCTOR: Assembles the Native Desktop Window
// ==============================================================================
NativeWindow::NativeWindow(int width, int height)
    : pimpl_(new Impl()) {
    // 1. Handshake with the Linux display server (Wayland or X11)
    gtk_init();

    // 2. Create the outer OS window frame and set initial size and default title
    pimpl_->window = gtk_window_new();
    gtk_window_set_default_size(GTK_WINDOW(pimpl_->window), width, height);
    gtk_window_set_title(GTK_WINDOW(pimpl_->window), "mdreader");

    // 3. Create the WebKit web view and insert it inside the window frame
    // In GTK4, gtk_window_set_child attaches the view so it fills the entire window
    pimpl_->web_view = webkit_web_view_new();
    gtk_window_set_child(GTK_WINDOW(pimpl_->window), pimpl_->web_view);

    // Restore saved scroll position when WebKit finishes loading document
    g_signal_connect(pimpl_->web_view, "load-changed", G_CALLBACK(+[](WebKitWebView*, WebKitLoadEvent event, gpointer user_data) {
        if (event == WEBKIT_LOAD_FINISHED) {
            auto* impl = static_cast<Impl*>(user_data);
            if (!impl->watched_path.empty()) {
                auto state = load_state(impl->watched_path);
                if (state.zoom > 0.0) {
                    webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(impl->web_view), state.zoom);
                }
                if (state.scroll_y > 0) {
                    std::string js = "window.scrollTo(0, " + std::to_string(state.scroll_y) + ");";
                    webkit_web_view_evaluate_javascript(WEBKIT_WEB_VIEW(impl->web_view), js.c_str(), -1, nullptr, nullptr, nullptr, nullptr, nullptr);
                }
            }
        }
    }), pimpl_);

    // 4. Create the desktop event loop (FALSE = starts in stopped state until run() is called)
    pimpl_->main_loop = g_main_loop_new(nullptr, FALSE);

    // 5. Track scroll position (simplest way to get it from WebKit synchronously for close-request)
    auto* manager = webkit_web_view_get_user_content_manager(WEBKIT_WEB_VIEW(pimpl_->web_view));
    g_signal_connect(manager, "script-message-received::state", G_CALLBACK(+[](WebKitUserContentManager*, JSCValue* value, gpointer user_data) {
        auto* impl = static_cast<Impl*>(user_data);
        if (jsc_value_is_number(value)) impl->current_scroll = jsc_value_to_int32(value);
    }), pimpl_);
    webkit_user_content_manager_register_script_message_handler(manager, "state", nullptr);
    g_signal_connect(manager, "script-message-received::quit", G_CALLBACK(+[](WebKitUserContentManager*, JSCValue*, gpointer user_data) {
        auto* impl = static_cast<Impl*>(user_data);
        if (impl && impl->window) {
            gtk_window_close(GTK_WINDOW(impl->window));
        }
    }), pimpl_);
    webkit_user_content_manager_register_script_message_handler(manager, "quit", nullptr);
    g_signal_connect(manager, "script-message-received::zoom", G_CALLBACK(+[](WebKitUserContentManager*, JSCValue* value, gpointer user_data) {
        auto* impl = static_cast<Impl*>(user_data);
        if (impl && impl->web_view && jsc_value_is_string(value)) {
            char* act = jsc_value_to_string(value);
            double z = webkit_web_view_get_zoom_level(WEBKIT_WEB_VIEW(impl->web_view));
            if (std::string_view(act) == "in") {
                webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(impl->web_view), z + 0.1 > 3.0 ? 3.0 : z + 0.1);
            } else if (std::string_view(act) == "out") {
                webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(impl->web_view), z - 0.1 < 0.3 ? 0.3 : z - 0.1);
            } else if (std::string_view(act) == "reset") {
                webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(impl->web_view), 1.0);
            }
            g_free(act);
        }
    }), pimpl_);
    webkit_user_content_manager_register_script_message_handler(manager, "zoom", nullptr);

    // 6. Connect the window's close button (X) to stop the event loop cleanly
    // - "close-request" : Signal emitted by GTK when the user clicks the X button
    // - G_CALLBACK(...) : Wraps our C++ lambda into a C function pointer
    // - static_cast     : Unpacks the generic 'user_data' (void*) back into our GMainLoop*
    g_signal_connect(pimpl_->window, "close-request", G_CALLBACK(+[](GtkWindow*, gpointer user_data) -> gboolean {
        auto* impl = static_cast<Impl*>(user_data);
        if (impl) {
            if (!impl->watched_path.empty()) {
                double zoom = impl->web_view ? webkit_web_view_get_zoom_level(WEBKIT_WEB_VIEW(impl->web_view)) : 1.0;
                save_state(impl->watched_path, impl->current_scroll, zoom, impl->max_history);
            }
            if (impl->main_loop && g_main_loop_is_running(impl->main_loop)) {
                g_main_loop_quit(impl->main_loop);
            }
        }
        return FALSE; // Return FALSE to allow GTK to destroy the window
    }), pimpl_);
}

// ==============================================================================
// DESTRUCTOR: RAII Resource Cleanup
// ==============================================================================
NativeWindow::~NativeWindow() {
    if (pimpl_) {
        // Stop the file watching timer if active
        if (pimpl_->timer_id != 0) {
            g_source_remove(pimpl_->timer_id);
            pimpl_->timer_id = 0;
        }
        // Release the reference to the GTK event loop
        if (pimpl_->main_loop) {
            g_main_loop_unref(pimpl_->main_loop);
        }
        // Free our heap-allocated implementation struct
        delete pimpl_;
        pimpl_ = nullptr;
    }
}

// ==============================================================================
// SET_TITLE: Update Title Bar
// ==============================================================================
void NativeWindow::set_title(const std::string& title) {
    if (pimpl_ && pimpl_->window) {
        // .c_str() converts std::string into the const char* that GTK expects
        gtk_window_set_title(GTK_WINDOW(pimpl_->window), title.c_str());
    }
}

// ==============================================================================
// LOAD_HTML: Zero-Localhost In-Memory Rendering
// ==============================================================================
void NativeWindow::load_html(const std::string& html_content, const std::string& base_dir) {
    if (!pimpl_ || !pimpl_->web_view) return;

    // Convert the local folder path to a "file://" URI
    // This allows WebKit to resolve relative images like <img src="diagram.png">
    // directly from the markdown file's directory on your disk!
    std::string base_uri;
    if (!base_dir.empty()) {
        base_uri = "file://" + base_dir;
        if (base_uri.back() != '/') {
            base_uri += '/'; // Ensure trailing slash so filenames append correctly
        }
    }

    // Load the HTML string directly from RAM into the WebKit view
    // (Zero sockets, zero ports, zero web servers!)
    webkit_web_view_load_html(
        WEBKIT_WEB_VIEW(pimpl_->web_view),
        html_content.c_str(),
        base_uri.empty() ? nullptr : base_uri.c_str()
    );
}

// ==============================================================================
// WATCH_FILE: Live File Watching for ':w' Saves
// ==============================================================================
void NativeWindow::watch_file(const std::string& file_path, int max_history) {
    std::error_code ec;
    if (!std::filesystem::exists(file_path, ec)) {
        return; // File doesn't exist on disk (e.g. piped from stdin)
    }

    pimpl_->watched_path = file_path;
    pimpl_->max_history = max_history;
    pimpl_->base_dir = std::filesystem::path(file_path).parent_path().string();
    pimpl_->last_mtime = std::filesystem::last_write_time(file_path, ec);

    // Poll every 250ms (4 times a second) inside the GTK event loop
    pimpl_->timer_id = g_timeout_add(250, G_SOURCE_FUNC(+[](gpointer user_data) -> gboolean {
        auto* self = static_cast<NativeWindow*>(user_data);
        if (!self || !self->pimpl_ || self->pimpl_->watched_path.empty()) {
            return G_SOURCE_CONTINUE;
        }

        std::error_code ec;
        auto current_mtime = std::filesystem::last_write_time(self->pimpl_->watched_path, ec);
        if (ec) {
            return G_SOURCE_CONTINUE; // Neovim might be mid-write (atomic swap)
        }

        // Check if file modification timestamp changed
        if (current_mtime != self->pimpl_->last_mtime) {
            self->pimpl_->last_mtime = current_mtime;
            self->pimpl_->save_count++;

            // 1. Re-read the file from disk
            std::ifstream file(self->pimpl_->watched_path);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());

                // 2. Parse markdown into HTML fragment
                auto fragment = markdown_to_html(content);

                // 3. Every 100th save: perform a clean full reload (Safety Valve)
                if (self->pimpl_->save_count % 100 == 0) {
                    auto full_html = wrap_html_document(fragment, self->pimpl_->watched_path);
                    self->load_html(full_html, self->pimpl_->base_dir);
                    std::cout << "[Live Reload] Full safety reload (Save #"
                              << self->pimpl_->save_count << ")\n";
                } else {
                    // Saves 1-99: Tier 3 Hot DOM Swap (instant, anchored scroll)
                    gchar* b64 = g_base64_encode(
                        reinterpret_cast<const guchar*>(fragment.data()),
                        fragment.size()
                    );
                    std::string js = "window.updateContent(new TextDecoder().decode("
                                     "Uint8Array.from(atob('" + std::string(b64) +
                                     "'), c => c.charCodeAt(0))));";
                    g_free(b64);

                    webkit_web_view_evaluate_javascript(
                        WEBKIT_WEB_VIEW(self->pimpl_->web_view),
                        js.c_str(),
                        -1,
                        nullptr, nullptr, nullptr, nullptr, nullptr
                    );
                    std::cout << "[Live Reload] Hot swap (Save #"
                              << self->pimpl_->save_count << ")\n";
                }
            }
        }

        return G_SOURCE_CONTINUE; // Keep repeating timer
    }), this);
}

// ==============================================================================
// RUN: Display Window and Start Event Loop
// ==============================================================================
void NativeWindow::run() {
    if (pimpl_ && pimpl_->main_loop) {
        // 1. Present the window on your screen
        gtk_window_present(GTK_WINDOW(pimpl_->window));

        // 2. Start the event loop (pauses here, handling mouse wheel, zoom, and redraws)
        // Execution will only resume here when the user closes the window!
        g_main_loop_run(pimpl_->main_loop);
    }
}

} // namespace mdreader
