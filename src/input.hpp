#pragma once

#include <string>
#include <filesystem>
#include <optional>
#include "state.hpp"

namespace fs = std::filesystem;

namespace mdreader {

// Data model: encapsulates the markdown content, parent folder, and source name
struct MarkdownInput {
    std::string content;
    fs::path base_dir;
    std::string source_name;
    int max_history{DEFAULT_MAX_HISTORY};
};

// Interface: loads markdown input from either stdin pipe or disk file
// Returns: MarkdownInput on success, or std::nullopt on error
std::optional<MarkdownInput> load_input(int argc, char* argv[]);

} // namespace mdreader
