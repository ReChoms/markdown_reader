#include <iostream>
#include <string>
#include "input.hpp"
#include "render.hpp"
#include "window.hpp"

int main(int argc, char* argv[]) {
    // 1. Load markdown input from file or pipe
    auto input = mdreader::load_input(argc, argv);
    if (!input) {
        return 1;
    }

    // 2. Render Markdown into full HTML5 document (KaTeX, Mermaid, dark/light CSS)
    auto fragment = mdreader::markdown_to_html(input->content);
    auto full_html = mdreader::wrap_html_document(fragment, input->source_name);

    // 3. Create the native desktop window and load HTML directly from RAM
    mdreader::NativeWindow window(860, 900);
    window.set_title(input->source_name + " — mdreader");
    window.load_html(full_html, input->base_dir);

    // 4. If loaded from a real file on disk (not piped stdin), start live watcher
    if (input->source_name != "<stdin>") {
        window.watch_file(input->source_name, input->max_history);
    }

    // 5. Run the desktop window (blocks until user closes the window)
    window.run();

    return 0;
}
