#include "input.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string_view>
#include <unistd.h> // POSIX header for isatty() and STDIN_FILENO

namespace mdreader {

std::optional<MarkdownInput> load_input(int argc, char* argv[]) {
    int max_history = DEFAULT_MAX_HISTORY;
    std::string file_arg;
    bool explicit_dash = false;

    // Parse options and positional file argument
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "--max-history" && i + 1 < argc) {
            try {
                max_history = std::max(0, std::stoi(argv[++i]));
            } catch (...) {
                std::cerr << "Warning: Invalid --max-history value, using " << DEFAULT_MAX_HISTORY << ".\n";
            }
        } else if (arg == "-") {
            explicit_dash = true;
        } else if (!arg.starts_with("-")) {
            file_arg = argv[i];
        }
    }

    // Detect if input is piped into the program
    bool piped_in = (file_arg.empty() && !explicit_dash && !isatty(STDIN_FILENO));
    bool read_from_stdin = explicit_dash || piped_in;

    if (file_arg.empty() && !read_from_stdin) {
        std::cerr << "Error: No file or pipe input specified.\n";
        std::cerr << "Usage: " << argv[0] << " [--max-history <N>] <file.md>   OR   cat <file.md> | " << argv[0] << "\n";
        return std::nullopt;
    }

    // 2. Read from stdin pipe
    if (read_from_stdin) {
        std::stringstream buffer;
        buffer << std::cin.rdbuf(); // Pours incoming stream into buffer

        return MarkdownInput{
            .content = buffer.str(),
            .base_dir = ".",
            .source_name = "<stdin>",
            .max_history = max_history
        };
    }

    // 3. Read from file on disk
    fs::path file_path = file_arg;

    if (!fs::exists(file_path)) {
        std::cerr << "Error: File does not exist: " << file_path << "\n";
        return std::nullopt;
    }

    fs::path base_dir = file_path.parent_path();
    if (base_dir.empty()) {
        base_dir = ".";
    }

    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << file_path << "\n";
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return MarkdownInput{
        .content = buffer.str(),
        .base_dir = base_dir,
        .source_name = file_path.string(),
        .max_history = max_history
    };
}

} // namespace mdreader
