#pragma once

#include <cstdlib>
#include <filesystem>
#include <string>

namespace mdreader {

// Default capacity for persistent position history
inline constexpr int DEFAULT_MAX_HISTORY = 50;

// Represents the saved visual position for a document across sessions
struct FileState {
    int scroll_y{0};  // Vertical scroll position in pixels (0 = document top)
    double zoom{1.0}; // Viewport zoom scale (1.0 = standard 100% size)
};

// Looks up saved state in positions.txt; returns default (0, 1.0) if file is new
FileState load_state(const std::string& file_path, const std::string& state_file = "");

// Writes position to positions.txt, moves entry to front (MRU), and evicts oldest past max_history
void save_state(const std::string& file_path, int scroll_y, double zoom, int max_history, const std::string& state_file = "");

// 1. Find where positions.txt lives so mdreader can:
//    - Read saved scroll position when opening a file
//    - Save updated position when closing the window
inline std::string get_state_file_path() {
    // Check custom Linux XDG state folder first
    const char* xdg_state = std::getenv("XDG_STATE_HOME");
    if (xdg_state && *xdg_state != '\0') {
        return (std::filesystem::path(xdg_state) / "mdreader" / "positions.txt").string();
    }
    // Fall back to user home: ~/.local/state/mdreader/positions.txt
    const char* home = std::getenv("HOME");
    if (home && *home != '\0') {
        return (std::filesystem::path(home) / ".local" / "state" / "mdreader" / "positions.txt").string();
    }
    return "";
}

} // namespace mdreader
