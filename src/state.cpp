#include "state.hpp"
#include <fstream>
#include <sstream>
#include <vector>

namespace mdreader {
// Looks up saved scroll position and zoom scale for a file from positions.txt
FileState load_state(const std::string& file_path, const std::string& state_file) {
    // Open state file: use custom path (unit testing) or default XDG path (production)
    std::ifstream in(state_file.empty() ? get_state_file_path() : state_file);
    std::string line, path;
    int scroll = 0;
    double zoom = 1.0;
    // Read line-by-line: gracefully skips if 'in' failed to open (e.g. file not created yet)
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        // Delimit by '\t' so file paths containing spaces are safely preserved
        if (std::getline(ss, path, '\t') && (ss >> scroll >> zoom))
            // Return parsed state immediately if this record matches the requested file
            if (path == file_path) return {scroll, zoom};
    }
    return {}; // Default fallback: top of page (0) and standard 100% zoom (1.0)
}

void save_state(const std::string& file_path, int scroll_y, double zoom, int max_history, const std::string& state_file) {
    if (max_history <= 0 || file_path.empty() || file_path == "<stdin>") return;
    std::string s_path = state_file.empty() ? get_state_file_path() : state_file;
    std::vector<std::string> entries{file_path + "\t" + std::to_string(scroll_y) + "\t" + std::to_string(zoom)};
    std::ifstream in(s_path);
    std::string line, path;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        if (std::getline(ss, path, '\t') && path != file_path && (int)entries.size() < max_history)
            entries.push_back(line);
    }
    in.close();
    std::filesystem::create_directories(std::filesystem::path(s_path).parent_path());
    std::ofstream out(s_path);
    for (const auto& e : entries) out << e << "\n";
}
} // namespace mdreader
