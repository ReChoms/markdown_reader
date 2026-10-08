#include <fstream>
#include <iostream>
#include <string>
#include "state.hpp"

int main() {
    std::string path = mdreader::get_state_file_path();
    if (path.empty()) {
        std::cerr << "FAILED: expected non-empty state file path, got empty\n";
        return 1;
    }
    std::ofstream("/tmp/test_state.txt") << "/tmp/test_file.md\t250\t1.5\n";
    mdreader::FileState state = mdreader::load_state("/tmp/test_file.md", "/tmp/test_state.txt");
    if (state.scroll_y != 250) {
        std::cerr << "FAILED: expected scroll_y == 250, got " << state.scroll_y << "\n";
        return 1;
    }
    mdreader::save_state("/tmp/evict.md", 500, 1.2, 1, "/tmp/test_state.txt");
    if (mdreader::load_state("/tmp/test_file.md", "/tmp/test_state.txt").scroll_y != 0) {
        std::cerr << "FAILED: expected old entry evicted\n";
        return 1;
    }
    std::cout << "All state tests passed.\n";
    return 0;
}
