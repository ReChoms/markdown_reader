#pragma once

#include <string>

namespace mdreader {

// ==============================================================================
// NativeWindow Class
// ==============================================================================
// Manages a native Linux desktop application window using GTK4 and WebKitGTK 6.0.
// Unlike a web browser, this is an independent desktop window with:
// - No browser tabs, no address bars, and no bookmarks
// - Zero localhost network sockets (loads HTML directly from RAM)
// - Native window snapping (Super + Left / Super + Right on KDE/GNOME)
// ==============================================================================
class NativeWindow {
public:
    // CONSTRUCTOR:
    // Initializes the desktop window with default dimensions.
    // - width  : Window width in screen pixels (default 860px)
    // - height : Window height in screen pixels (default 900px)
    NativeWindow(int width = 860, int height = 900);

    // DESTRUCTOR:
    // Cleans up the event loop and frees internal GUI resources on exit.
    ~NativeWindow();

    // ==========================================================================
    // RESOURCE OWNERSHIP (Rule of 5 / RAII)
    // ==========================================================================
    // Delete copy constructor and copy assignment operator.
    // Because NativeWindow owns operating system GUI handles, copying is banned
    // to prevent two objects from closing or double-freeing the same window.
    NativeWindow(const NativeWindow&) = delete;
    NativeWindow& operator=(const NativeWindow&) = delete;

    // ==========================================================================
    // PUBLIC MEMBER FUNCTIONS
    // ==========================================================================

    // Sets the title text displayed in the desktop window's top title bar.
    void set_title(const std::string& title);

    // Loads the generated HTML string directly from computer memory (RAM)
    // into the WebKit rendering canvas (Zero localhost / zero network sockets!).
    // - html_content : The complete HTML5 document string with KaTeX and Mermaid
    // - base_dir     : Folder path of the markdown file (used to resolve relative images)
    void load_html(const std::string& html_content, const std::string& base_dir = "");

    // Watches a markdown file on disk and automatically re-renders the document
    // whenever the file modification timestamp changes (e.g. after a ':w' save in Neovim).
    // - file_path   : Path to the markdown file on disk
    // - max_history : Maximum persistent positions to retain
    void watch_file(const std::string& file_path, int max_history);

    // Enters the native desktop event loop.
    // This pauses/blocks main(), keeping the window visible and responsive to
    // mouse scrolling, keyboard events, and window resizing until the user closes it.
    void run();

private:
    // ==========================================================================
    // PIMPL IDIOM (Pointer to Implementation)
    // ==========================================================================
    // By forward-declaring 'struct Impl' here and defining it only inside window.cpp:
    // 1. Heavy GTK4 and WebKit C headers (<gtk/gtk.h>, <webkit/webkit.h>) stay
    //    completely out of this public header.
    // 2. Any file including window.hpp (like main.cpp) compiles in milliseconds.
    // 3. Prevents naming and macro collisions between GTK and standard C++.
    struct Impl;
    Impl* pimpl_{nullptr}; // Pointer to the heap-allocated private implementation
};

} // namespace mdreader
