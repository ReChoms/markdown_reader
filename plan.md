# Implementation Plan — `mdreader` (C++20 Edition)

A checklist of micro-steps across each milestone. Every step is implemented in very small, educational increments.

---

## Milestone 0: Build Tooling & Neovim LSP Setup
- [x] **Step 0.1**: Create `Makefile` with strict flags (`-std=c++20 -Wall -Wextra -pedantic -g`).
- [x] **Step 0.2**: Create `.clangd` configuration for Neovim LSP (`clangd`) autocompletion and diagnostics.
- [x] **Step 0.3**: Write minimal `src/main.cpp` and verify `make` compiles cleanly.

---

## Milestone 1: CLI Arguments, Stdin & Input Handling
- [x] **Step 1.1**: Handle `int argc, char* argv[]` safely in `main`.
- [x] **Step 1.2**: Unpack file path argument or exit with clean `std::cerr` usage message and exit code `1`.
- [x] **Step 1.3**: Read file contents into `std::string` using `std::ifstream` and `std::filesystem`.
- [x] **Step 1.4**: Extract parent folder (`base_dir`) with `std::filesystem::path` to resolve relative images.
- [x] **Step 1.5**: Support Unix pipes (`cat notes.md | mdreader` or `mdreader -`) via `std::cin`.

---

## Milestone 2: Markdown, LaTeX Math & Mermaid Translation
- [x] **Step 2.1**: Set up `md4c` (fast, lightweight C CommonMark parser with math extensions).
- [x] **Step 2.2**: Convert Markdown into HTML string.
- [x] **Step 2.3**: Build HTML5 template with KaTeX and Mermaid.js support.
- [x] **Step 2.4**: Add clean typography and automatic dark/light desktop theme detection.

---

## Milestone 3: Native Desktop Companion Window (WebKitGTK 6.0 — Zero Localhost)
- [x] **Step 3.1 (Educational Socket Architecture)**: Implemented RAII `TcpListener` socket listener on ephemeral port and `handle_client`.
- [x] **Step 3.2**: Create native C++ desktop window using WebKitGTK 6.0 (`src/window.hpp` / `src/window.cpp`).
- [x] **Step 3.3**: Load rendered HTML directly from memory into WebKit (`webkit_web_view_load_html`) with local image support (`base_dir`).

---

## Milestone 4: Live File Watching & State Preservation
- [x] **Step 4.1**: Check file modification timestamp (`mtime`) with `std::filesystem::last_write_time`.
- [x] **Step 4.2 (Superseded)**: Old `/__version` HTTP polling superseded by zero-localhost native WebKitGTK event loop.
- [x] **Step 4.3**: Preserve exact scroll position across live reloads (Tier 3 Hot DOM Swap via `evaluate_javascript` + `sessionStorage` fallback).
- [x] **Step 4.4**: Preserve zoom level across live reloads (Hot DOM Swap leaves viewport zoom scale intact).
- [x] **Step 4.5**: Persistent reading position via Unix XDG state file (`~/.local/state/mdreader/positions.txt`), capped at max 50 files (LRU cleanup), configurable via `--max-history <N>` (default 50).

---

## Milestone 5: Neovim Keybindings, Zoom & Window Launching
- [x] **Step 5.1**: Add Vim keyboard listeners (`j`, `k`, `d`, `u`, `gg`, `G`, `q`).
- [x] **Step 5.2**: Add zoom controls (<kbd>+</kbd>, <kbd>-</kbd>, <kbd>0</kbd>, and <kbd>Ctrl</kbd>+wheel).
- [x] **Step 5.3 (Superseded)**: Chromium `--app` mode superseded by native WebKitGTK 6.0 companion window.

---

## Milestone 6: KDE Desktop Entry & End-to-End Verification
- [x] **Step 6.1**: Create `mdreader.desktop` template for KDE File Associations without hijacking defaults.
- [x] **Step 6.2**: End-to-end verification with a sample `.md` file containing math, diagrams, images, and tables.

---

## Milestone 7: Mermaid Scaling, Interactive Sizing & High-Contrast Readability
- [x] **Step 7.1**: Configure Mermaid natural font scaling with horizontal scrolling (`overflow-x: auto`).
- [x] **Step 7.2**: Improve Mermaid color readability and dark/light contrast via `themeVariables` and CSS overrides.
- [x] **Step 7.3**: Add click-to-toggle between fit-to-width and 100% natural scale on `.mermaid` containers.
- [x] **Step 7.4**: Verify rendering with `make -s mdreader` and test visual diagrams.
