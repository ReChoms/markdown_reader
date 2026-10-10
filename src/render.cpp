#include "render.hpp"
#include "md4c-html.h"

namespace mdreader {

// Static callback invoked by md_html() for every chunk of generated HTML
static void append_html_chunk(const MD_CHAR* text, MD_SIZE size, void* userdata) {
    auto* out = static_cast<std::string*>(userdata);
    out->append(text, size);
}

std::string markdown_to_html(const std::string& markdown) {
    std::string html_output;

    // Enable GitHub Flavored Markdown (tables, tasklists, autolinks, strikethrough)
    // + LaTeX Math ($...$ and $$...$$) + Wikilinks + Highlight
    unsigned parser_flags = MD_DIALECT_GITHUB | MD_FLAG_LATEXMATHSPANS | MD_FLAG_WIKILINKS | MD_FLAG_HIGHLIGHT;
    unsigned renderer_flags = 0;

    int res = md_html(
        markdown.data(),
        static_cast<MD_SIZE>(markdown.size()),
        append_html_chunk,
        &html_output,
        parser_flags,
        renderer_flags
    );

    if (res != 0) {
        return "<p>Error: Failed to parse markdown</p>";
    }

    return html_output;
}

std::string wrap_html_document(const std::string& body_html, const std::string& title) {
    // 1. Modern CSS with automatic dark/light theme support
    constexpr const char* CSS_TEMPLATE = R"css(
        :root {
            --bg-color: #ffffff;
            --text-color: #1f2328;
            --code-bg: #f6f8fa;
            --border-color: #d0d7de;
            --link-color: #0969da;
            --blockquote-color: #59636e;
        }
        @media (prefers-color-scheme: dark) {
            :root {
                --bg-color: #0d1117;
                --text-color: #e6edf3;
                --code-bg: #161b22;
                --border-color: #30363d;
                --link-color: #4493f8;
                --blockquote-color: #8b949e;
            }
        }
        body {
            background-color: var(--bg-color);
            color: var(--text-color);
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif;
            font-size: 16px;
            line-height: 1.6;
            margin: 0;
            padding: 2rem;
            display: flex;
            justify-content: center;
        }
        .markdown-body {
            width: 100%;
            max-width: 860px;
        }
        h1, h2, h3, h4, h5, h6 {
            font-weight: 700;
            line-height: 1.25;
            margin-top: 1.5em;
            margin-bottom: 0.5em;
        }
        h1 { font-size: 2.0rem; border-bottom: 1px solid var(--border-color); padding-bottom: 0.3em; }
        h2 { font-size: 1.5rem; border-bottom: 1px solid var(--border-color); padding-bottom: 0.3em; }
        h3 { font-size: 1.25rem; }
        h4 { font-size: 1.15rem; }
        h5 { font-size: 1.08rem; }
        h6 { font-size: 1.02rem; }
        .markdown-body { counter-reset: h1; }
        .markdown-body h1 { counter-reset: h2; }
        .markdown-body h2 { counter-reset: h3; }
        .markdown-body h3 { counter-reset: h4; }
        .markdown-body h4 { counter-reset: h5; }
        .markdown-body h5 { counter-reset: h6; }
        .markdown-body h1::before { counter-increment: h1; content: counter(h1) ". "; }
        .markdown-body h2::before { counter-increment: h2; content: counter(h1) "." counter(h2) " "; }
        .markdown-body h3::before { counter-increment: h3; content: counter(h1) "." counter(h2) "." counter(h3) " "; }
        .markdown-body h4::before { counter-increment: h4; content: counter(h1) "." counter(h2) "." counter(h3) "." counter(h4) " "; }
        .markdown-body h5::before { counter-increment: h5; content: counter(h1) "." counter(h2) "." counter(h3) "." counter(h4) "." counter(h5) " "; }
        .markdown-body h6::before { counter-increment: h6; content: counter(h1) "." counter(h2) "." counter(h3) "." counter(h4) "." counter(h5) "." counter(h6) " "; }
        pre {
            background-color: var(--code-bg);
            padding: 1rem;
            border-radius: 6px;
            overflow-x: auto;
        }
        code {
            font-family: "SFMono-Regular", Consolas, "Liberation Mono", Menlo, monospace;
            font-size: 85%;
        }
        p code, li code {
            background-color: var(--code-bg);
            padding: 0.2em 0.4em;
            border-radius: 4px;
        }
        blockquote {
            margin: 0;
            padding: 0 1em;
            color: var(--blockquote-color);
            border-left: 0.25em solid var(--border-color);
        }
        table {
            border-collapse: collapse;
            width: 100%;
            margin: 1rem 0;
        }
        th, td {
            border: 1px solid var(--border-color);
            padding: 6px 13px;
        }
        img {
            max-width: 100%;
            height: auto;
        }
        a { color: var(--link-color); }
        .mermaid {
            display: flex;
            justify-content: center;
            margin: 1.5rem 0;
            padding: 1rem;
            background-color: var(--code-bg);
            border: 1px solid var(--border-color);
            border-radius: 8px;
            overflow-x: auto;
            cursor: zoom-in;
            scrollbar-width: thin;
            scrollbar-color: var(--border-color) transparent;
            transition: border-color 0.2s ease, box-shadow 0.2s ease;
        }
        .mermaid:hover {
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
            border-color: var(--link-color);
        }
        .mermaid:focus-visible {
            outline: 2px solid var(--link-color);
            outline-offset: 2px;
        }
        .mermaid svg { max-width: 100%; height: auto; }
        .mermaid.expanded { justify-content: flex-start; cursor: zoom-out; }
        .mermaid.expanded svg { max-width: none; }
        #search-bar {
            display: none;
            position: fixed;
            bottom: 1.25rem;
            right: 1.25rem;
            background: var(--bg-color);
            border: 1px solid var(--border-color);
            border-radius: 6px;
            padding: 0.35rem 0.65rem;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
            z-index: 1000;
        }
        #search-input {
            background: transparent;
            border: none;
            outline: none;
            color: var(--text-color);
            font-family: inherit;
            font-size: 14px;
            width: 180px;
        }
    )css";

    // 2. Client-side JS to render KaTeX equations and Mermaid diagrams
    constexpr const char* JS_SCRIPTS = R"js(
        function renderEnhancements(container) {
            // 1. Render KaTeX equations (<x-equation>)
            container.querySelectorAll("x-equation").forEach(el => {
                const isDisplay = el.getAttribute("type") === "display";
                if (window.katex) {
                    katex.render(el.textContent, el, {
                        displayMode: isDisplay,
                        throwOnError: false
                    });
                }
            });

            // 2. Render Mermaid diagrams (pre > code.language-mermaid)
            const mermaidBlocks = container.querySelectorAll("pre code.language-mermaid");
            if (mermaidBlocks.length > 0 && window.mermaid) {
                mermaidBlocks.forEach(block => {
                    const pre = block.parentElement;
                    const div = document.createElement("div");
                    div.className = "mermaid";
                    div.tabIndex = 0;
                    div.textContent = block.textContent;
                    const toggle = () => div.classList.toggle("expanded");
                    div.addEventListener("click", toggle);
                    div.addEventListener("keydown", (e) => {
                        if (e.key === "Enter" || e.key === " ") {
                            e.preventDefault();
                            toggle();
                        }
                    });
                    pre.replaceWith(div);
                });
                const isDark = window.matchMedia("(prefers-color-scheme: dark)").matches;
                mermaid.initialize({
                    startOnLoad: false,
                    theme: isDark ? "dark" : "default",
                    themeVariables: isDark ? {
                        darkMode: true,
                        background: "#161b22",
                        primaryColor: "#21262d",
                        primaryTextColor: "#e6edf3",
                        primaryBorderColor: "#30363d",
                        lineColor: "#8b949e"
                    } : {
                        darkMode: false,
                        background: "#f6f8fa",
                        primaryColor: "#ffffff",
                        primaryTextColor: "#1f2328",
                        primaryBorderColor: "#d0d7de",
                        lineColor: "#656d76"
                    }
                });
                mermaid.run();
            }
        }

        // Tier 3 Hot Swap API: called by C++ on saves 1-99
        window.updateContent = function(html) {
            const content = document.getElementById("content");
            if (content) {
                content.innerHTML = html;
                renderEnhancements(content);
            }
        };

        // Safety Valve: On the 100th save (full reload), capture scroll position
        window.addEventListener("beforeunload", () => {
            sessionStorage.setItem("scroll_pos", window.scrollY);
        });

        // Initial Load (and 100th-save restore)
        document.addEventListener("DOMContentLoaded", () => {
            const content = document.getElementById("content");
            if (content) {
                renderEnhancements(content);
            }
            const savedScroll = sessionStorage.getItem("scroll_pos");
            if (savedScroll !== null) {
                window.scrollTo(0, parseInt(savedScroll, 10));
            }
        });

        // Track scroll for C++ persistence
        window.addEventListener("scroll", () => {
            if (window.webkit && window.webkit.messageHandlers.state) {
                window.webkit.messageHandlers.state.postMessage(window.scrollY);
            }
        });

        // Neovim navigation keybindings
        let lastGTime = 0, lastQuery = "";
        const sBar = document.createElement("div");
        sBar.id = "search-bar";
        sBar.innerHTML = '<input id="search-input" placeholder="Search... (Enter/Esc)">';
        document.body.appendChild(sBar);
        const sInput = sBar.firstElementChild;
        sInput.addEventListener("keydown", (e) => {
            if (e.key === "Enter" && (lastQuery = sInput.value)) {
                window.find(lastQuery, false, e.shiftKey, true);
            } else if (e.key === "Escape") {
                sBar.style.display = "none";
            }
        });
        window.addEventListener("keydown", (e) => {
            if (e.target === sInput || e.altKey || e.metaKey) return;
            if (e.key === "/" || (e.ctrlKey && e.key === "f")) {
                e.preventDefault();
                sBar.style.display = "flex";
                sInput.focus(); sInput.select();
                return;
            }
            if (e.key === "n" && lastQuery) { window.find(lastQuery, false, false, true); return; }
            if (e.key === "N" && lastQuery) { window.find(lastQuery, false, true, true); return; }
            if (e.key === "Escape") { sBar.style.display = "none"; return; }
            const step = 60, half = window.innerHeight / 2;
            if (e.key === "g" && !e.ctrlKey) {
                const now = Date.now();
                if (now - lastGTime < 400) {
                    window.scrollTo({ top: 0, behavior: "smooth" });
                    lastGTime = 0;
                } else {
                    lastGTime = now;
                }
                return;
            }
            lastGTime = 0;
            if (e.key === "Home") {
                window.scrollTo({ top: 0, behavior: "smooth" });
            } else if (e.key === "j" || e.key === "ArrowDown") {
                window.scrollBy({ top: step, behavior: "smooth" });
            } else if (e.key === "k" || e.key === "ArrowUp") {
                window.scrollBy({ top: -step, behavior: "smooth" });
            } else if (e.key === "d" || (e.ctrlKey && e.key === "d")) {
                window.scrollBy({ top: half, behavior: "smooth" });
            } else if (e.key === "u" || (e.ctrlKey && e.key === "u")) {
                window.scrollBy({ top: -half, behavior: "smooth" });
            } else if (e.key === "G" || e.key === "End") {
                window.scrollTo({ top: document.body.scrollHeight, behavior: "smooth" });
            } else if (e.key === "q" && !e.ctrlKey) {
                if (window.webkit && window.webkit.messageHandlers.quit) {
                    window.webkit.messageHandlers.quit.postMessage("");
                }
            } else if (e.key === "+" || e.key === "=") {
                if (window.webkit && window.webkit.messageHandlers.zoom) window.webkit.messageHandlers.zoom.postMessage("in");
            } else if (e.key === "-") {
                if (window.webkit && window.webkit.messageHandlers.zoom) window.webkit.messageHandlers.zoom.postMessage("out");
            } else if (e.key === "0") {
                if (window.webkit && window.webkit.messageHandlers.zoom) window.webkit.messageHandlers.zoom.postMessage("reset");
            }
        });

        window.addEventListener("wheel", (e) => {
            if (e.ctrlKey && window.webkit && window.webkit.messageHandlers.zoom) {
                e.preventDefault();
                window.webkit.messageHandlers.zoom.postMessage(e.deltaY < 0 ? "in" : "out");
            }
        }, { passive: false });
    )js";

    // 3. Assemble the full HTML5 document
    std::string html;
    html += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html += "  <meta charset=\"UTF-8\">\n";
    html += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "  <title>" + title + "</title>\n";
    html += "  <style>" + std::string(CSS_TEMPLATE) + "</style>\n";
    html += "  <!-- KaTeX -->\n";
    html += "  <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/katex@0.16.9/dist/katex.min.css\">\n";
    html += "  <script defer src=\"https://cdn.jsdelivr.net/npm/katex@0.16.9/dist/katex.min.js\"></script>\n";
    html += "  <!-- Mermaid -->\n";
    html += "  <script src=\"https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.min.js\"></script>\n";
    html += "</head>\n<body>\n";
    html += "  <main id=\"content\" class=\"markdown-body\">\n";
    html += body_html;
    html += "  </main>\n";
    html += "  <script>" + std::string(JS_SCRIPTS) + "</script>\n";
    html += "</body>\n</html>\n";

    return html;
}

} // namespace mdreader
