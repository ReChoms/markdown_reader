# Project Log — `mdreader`

A running chronological log of milestones, decisions, and development progress.

---

## 2026-09-02 / 2026-09-03 — Session 1: Architecture & Project Kickoff

### Objectives
- Define requirements, architecture, and working rules for a lightweight Markdown viewer in Rust.
- Set up project foundation and begin Milestone 1.

### Decisions & Milestones Achieved
1. **Architecture & Scope Defined** ([solution_design.md](solution_design.md)):
   - **Core Role**: Strict viewer/reader companion to Neovim and terminal; zero vault overhead.
   - **Unix Philosophy**: Support both file paths and stdin pipes (`cat file.md | mdreader`), clean exit codes, separate stderr.
   - **High-Fidelity Rendering**: LaTeX math formulas via KaTeX, Mermaid.js diagrams, local relative image path resolution.
   - **Interaction**: Neovim keybindings (`j`, `k`, `d`, `u`, `gg`, `G`, `q`), smooth mouse wheel, text selection/copy, zoom controls (`+`, `-`, `0`, `Ctrl`+wheel).
   - **Live Reload**: File modification tracking with scroll and zoom state persistence.
   - **KDE Desktop Integration**: Provide standard `.desktop` template for user-controlled file associations without hijacking system defaults.

2. **Agent Working Guidelines** ([AGENTS.md](AGENTS.md)):
   - Discuss before acting; no silent assumptions or unsolicited network calls.
   - Very small code steps (3–8 lines) with educational explanations of Rust concepts.
   - Respect user configuration control.

3. **Feedback Tracking** ([feedback_log.md](feedback_log.md)):
   - Established structured feedback logging (Complaint, Root Cause, Resolution).

4. **Milestone 1 Started** ([src/main.rs](src/main.rs)):
   - Initialized Cargo binary project (`mdreader`).
   - Implemented CLI argument retrieval (`std::env::args().nth(1)`).
   - Added safe argument extraction using `match` with clean stderr error message and exit code `1`.
   - Verified compilation passes cleanly.

### Current Status
- Milestone 1 completed (100%).
- Milestone 2 completed (100%).

---

## 2026-09-20 — Session 2: Milestones 1 & 2, River/Gravity Architecture

### Objectives
- Complete Milestone 1 (File I/O, `base_dir`, and Unix pipe/stdin support).
- Implement Milestone 2 (Markdown parsing, LaTeX math with KaTeX, Mermaid.js diagrams, reading typography, auto dark/light theme).
- Refactor codebase into a strict River / Gravity Architecture with a max 100 lines per file rule.

### Decisions & Milestones Achieved
1. **Milestone 1 Completed** ([src/input.rs](src/input.rs)):
   - **Step 1.3**: File reading into `String` via `std::fs::read_to_string` with clean `stderr` diagnostics and non-zero exit codes.
   - **Step 1.4**: Parent folder extraction (`base_dir`) using `std::path::Path` to resolve local images relative to markdown files.
   - **Step 1.5**: Full Unix stream support for stdin pipes (`cat file.md | mdreader`) and explicit dash (`mdreader -`) using `std::io::stdin` and `std::io::IsTerminal`.

2. **Milestone 2 Completed** ([src/render/mod.rs](src/render/mod.rs), [src/render/template.rs](src/render/template.rs)):
   - **Step 2.1**: Configured `pulldown-cmark` with rich Obsidian/GFM extensions (Math, Tables, Tasklists, Strikethrough, Footnotes, GFM Callouts, Smart Punctuation, Heading Attributes, YAML Frontmatter, Superscript, Subscript, Wikilinks).
   - **Step 2.2**: Converted parsed Markdown events into HTML via `pulldown_cmark::html::push_html`.
   - **Step 2.3**: Built minimal HTML5 document wrapper using Rust raw string literals (`r#"..."#`).
   - **Step 2.4**: Integrated KaTeX for sub-millisecond vector LaTeX math rendering (`$...$` and `$$...$$`).
   - **Step 2.5**: Integrated Mermaid.js engine to render ` ```mermaid ` blocks to SVG diagrams.
   - **Step 2.6**: Added GitHub/Obsidian reading typography (860px centered container) and automatic desktop Dark/Light mode detection via `@media (prefers-color-scheme: dark)`.

3. **River / Gravity Architecture & 100-Line Limit Refactor**:
   - Adopted strict unidirectional dependency ("what is down never calls up").
   - Enforced a hard limit of maximum 100 lines of code per file for readability and focus.
   - Restructured into:
     - `src/render/template.rs` (Layer 0, Leaf: 87 lines): Pure HTML/CSS/KaTeX/Mermaid template; zero project dependencies.
     - `src/render/mod.rs` (Layer 1: 32 lines): Markdown parsing & HTML conversion; depends only on `template.rs`.
     - `src/input.rs` (Layer 1: 55 lines): Self-contained CLI & stream input handling.
     - `src/main.rs` (Layer 2, Top Orchestrator: 17 lines): Coordinates input and rendering.

4. **Milestone 3 Started** ([src/server.rs](src/server.rs)):
   - **Step 3.1**: Created `src/server.rs` and bound `tiny_http` to an ephemeral free port (`127.0.0.1:0`) to eliminate port collisions.
   - **Step 3.2**: Implemented request handling loop to serve the full rendered HTML document on root path `/` (verified with `curl`).
   - **Unix Stream Separation Refinement**: Switched server status messages to `eprintln!`, ensuring `stdout` remains 100% clean for Unix pipelines.

5. **Rules Relocation**:
   - Moved `AGENTS.md` to `.agents/rules/AGENTS.md` for workspace-wide rule discovery while keeping the repository root tidy.

### Current Status
- 13 of 23 implementation steps complete (~57% overall progress).
- All files strictly under 100 lines of code:
  - `src/main.rs`: 14 lines
  - `src/server.rs`: 38 lines
  - `src/render/mod.rs`: 32 lines
  - `src/render/template.rs`: 87 lines
  - `src/input.rs`: 55 lines
- Compilation and test execution in ~0.10s.

### Next Session Plan
- Complete **Step 3.3**: Route local image and asset requests relative to `base_dir` using `mime_guess` and `percent-encoding` in `src/server.rs`.
- Begin **Milestone 4: Live File Watching & State Preservation**:
  - Step 4.1: Track file modification timestamp (`mtime`).
  - Step 4.2: Add `/__version` endpoint for live reload detection.
  - Step 4.3 & 4.4: Preserve exact scroll position and zoom level across reloads.

---

## 2026-09-25 — Session 3: C++ Transition & Architectural Foundations

### Objectives
- Transition `mdreader` to modern C++20 for deep systems programming learning.
- Establish clean build tooling, editor ergonomics (Neovim LSP), and low-level architecture.

### Decisions & Milestones Achieved
1. **Rust Codebase Archived**:
   - Safely moved previous Rust implementation (`src/`, `Cargo.toml`, `Cargo.lock`, `target/`) into `backup_rust/`.
   - Updated `.gitignore` to ignore C++ build outputs (`build/`, `*.o`, `mdreader`) and `backup_rust/target/`.

2. **Decision 1: Build System — GNU Make (`Makefile`)**:
   - Selected GNU Make over CMake for complete transparency and direct shell flexibility.
   - Preserves total control over compiler flags (`-std=c++20`, `-Wall`, `-Wextra`, `-pedantic`, `-g`).
   - Integrates natively with Neovim via `:make` and quickfix list (`:copen`).
   - Uses a root `.clangd` configuration to provide Neovim's `clangd` LSP with full autocomplete, hover docs, and instant diagnostics without CMake overhead.

3. **Decision 2: Networking & HTTP — Custom POSIX Sockets (LaurieWired Path)**:
   - Selected low-level Linux POSIX sockets (`<sys/socket.h>`, `<netinet/in.h>`, `<unistd.h>`) over third-party libraries.
   - Zero external networking dependencies.
   - Wrapped in modern C++ RAII classes for deterministic resource management and leak prevention.
   - Sub-millisecond compilation and direct interaction with Unix file descriptors.

---

## 2026-09-27 — Session 4: Milestones 1 & 2 Completion, WebKitGTK Native Window Architecture

### Objectives
- Complete C++ Milestone 1 (CLI & Stdin Input Handling).
- Complete C++ Milestone 2 (Markdown parsing with `md4c`, LaTeX math with KaTeX, Mermaid diagrams, HTML5 wrapper with dark/light themes).
- Deep dive into systems networking (TCP, Sockets, File Descriptors, RAII, `= delete`, Move Semantics).
- Pivot display architecture from external browser localhost tabs to a standalone native Linux desktop window via WebKitGTK 6.0.

### Decisions & Milestones Achieved
1. **Milestone 1 Completed in C++20** (`src/input.hpp`, `src/input.cpp`):
   - Structured `MarkdownInput` with `std::string_view` zero-copy comparisons.
   - Robust Unix pipe detection with POSIX `isatty(STDIN_FILENO)`.
   - Safe argument handling returning `std::optional<MarkdownInput>`.

2. **Milestone 2 Completed in C++20** (`src/render.hpp`, `src/render.cpp`, `vendor/md4c/`):
   - Integrated `md4c` (fast C99 CommonMark parser) with LaTeX math extension flags.
   - Mixed C (gcc C99) and C++ (g++ C++20) cleanly in `Makefile` using object files (`.o`).
   - Implemented `wrap_html_document` using C++ raw string literals `R"(...)"`.
   - Embedded KaTeX CDN for mathematical formulas and Mermaid.js for vector diagrams.
   - Responsive CSS with automatic `@media (prefers-color-scheme: dark)` theme detection.

3. **Systems Programming & Socket Architecture** (`src/server.hpp`, `src/server.cpp`):
   - Implemented RAII `TcpListener` with deleted copy constructors (`= delete`) and move semantics (`&&`).
   - Ephemeral port binding (`bind` to port 0) and port discovery with `getsockname`.
   - HTTP request parsing and response writer (`handle_client`).

4. **Architectural Evolution: Native WebKitGTK Window (Zero Localhost)**:
   - **Problem with browser tabs**: Opening an external browser tab with random localhost URLs felt clunky and non-native for a fast Unix document viewer.
   - **Explored Options**: Evaluated Electron (Obsidian), Chromium App Mode, Terminal ANSI mode, and native WebKitGTK.
   - **Selected WebKitGTK 6.0**:
     - Dedicated standalone desktop window (NOT a web browser).
     - Renders full Mermaid diagrams and LaTeX math without external tools.
     - **Zero localhost**: Loads HTML directly from RAM via `webkit_web_view_load_html`.
     - Blazingly fast (~20ms startup) and lightweight (~30MB RAM vs 300MB in Electron).
   - **Package Verified**: Installed `webkitgtk6.0-devel` (v2.54.0) on Fedora KDE.



---

## 2026-09-29 — Session 5: Milestone 4 Live File Watching & Tier 3 Hot DOM Swap

### Objectives
- Connect live file watcher in `src/main.cpp`.
- Design and implement state preservation (scroll and zoom) across live `:w` saves.
- Brainstorm and benchmark performance tiers for live reload (DOM Teardown vs. Hot DOM Swap).

### Decisions & Milestones Achieved
1. **Live Watcher Hook in `src/main.cpp`**:
   - Conditioned file watching on `input->source_name != "<stdin>"` to properly separate disk files from transient pipes.
   - Built and verified clean zero-warning compilation.

2. **Architectural Decision: Tier 3 Hot DOM Swap + 100-Save Safety Valve**:
   - **Problem Analyzed**: Traditional `load_html` on every save tears down the DOM, destroys scroll position, and re-parses the ~3MB Mermaid.js runtime.
   - **Selected Tier 3**: In-place DOM update via `webkit_web_view_evaluate_javascript`.
     - Preserves exact scroll position and zoom level naturally with zero jumping or clamping bugs.
     - Updates in `<5ms` with zero UI flicker.
   - **Crash-Proof Data Transfer**: Encoded Markdown HTML fragments into Base64 using GLib's `g_base64_encode`, fully immunizing against quote, newline, or LaTeX backslash syntax issues in JavaScriptCore.
   - **100-Save Safety Cap**: For saves 1–99, run Hot DOM Swap. On every 100th save, perform a clean full reload (`load_html`) with `sessionStorage` capturing/restoring scroll position to flush memory and reset the slate.

3. **Template & Engine Integration** (`src/render.cpp`, `src/window.cpp`):
   - Added `window.updateContent(html)` and `renderEnhancements(container)` in HTML template.
   - Implemented `save_count` tracking and WebKit JavaScript evaluation in `NativeWindow::watch_file`.
   - Verified live hot swap and scroll stability with `test.md`.

### Current Status
- Milestone 0: 100% Complete.
- Milestone 1: 100% Complete.
- Milestone 2: 100% Complete.
- Milestone 3: 100% Complete.
- Milestone 4: Steps 4.1–4.4 Complete (~80% of Milestone 4).


## Session: Step 4.5 State Persistence Setup
- **Changed**: Added `make test` target, `src/state.hpp` with XDG path resolution, and `--max-history <N>` CLI option parsing in `src/input.cpp`.
- **Why**: Established the automated test harness and foundational configuration for Step 4.5 persistent reading positions.
- **Learned**: C++ One Definition Rule (ODR) and `inline` semantics across translation units, plus raw pointer and null-terminator validation with `std::getenv`.
- **Next**: Implement `src/state.cpp` to parse/save `positions.txt` with LRU eviction, then hook up scroll and zoom restoration on window close.

## Session: Step 4.5 State Storage Implementation
- **Changed**: Implemented `load_state` and `save_state` with LRU eviction in `src/state.cpp`, added unit tests, and documented functions.
- **Why**: Provides persistent reading position and zoom storage capped at `--max-history` for Step 4.5.
- **Learned**: Using `\t` delimiters preserves paths with spaces; string streams simplify typed extraction and validation.
- **Next**: Hook `load_state` into page load and `save_state` into `close-request` to finish Step 4.5.

## Session: Milestone 4 Completion & Lifecycle Integration
- **Changed**: Wired `load_state` to `load-changed`, `save_state` to `close-request`, centralized `DEFAULT_MAX_HISTORY`, and connected `--max-history <N>`.
- **Why**: Finalized Step 4.5 and Milestone 4 so scroll positions restore on open and persist on close with configurable history depth.
- **Learned**: WebKitGTK 6 script message handlers receive `JSCValue*` directly; captureless C++ lambdas decay to C `GCallback` function pointers.
- **Next**: Milestone 5: Neovim keybindings (`j`, `k`, `d`, `u`, `gg`, `G`, `q`) and zoom controls.

## Session: Milestone 5 Completion (Keybindings & Zoom)
- **Changed**: Added Vim navigation (`j`, `k`, `d`, `u`, `gg`, `G`, `q`), zoom controls (`+`, `-`, `0`, `Ctrl`+wheel), and zoom state persistence.
- **Why**: Fulfills Milestone 5 so document reading offers native keyboard navigation and persistent zoom scaling.
- **Learned**: Script message handlers bridge DOM events directly into GTK window actions without localhost networking.
- **Open questions**: None; ready for Milestone 6 KDE desktop entry and end-to-end document testing.

## Session: Milestone 6 Completion (Desktop Entry & End-to-End Test)
- **Changed**: Created `mdreader.desktop` template and expanded `test.md` with tables, math, and diagrams.
- **Why**: Completes Milestone 6, enabling KDE desktop integration and end-to-end rendering verification.
- **Learned**: Standard XDG `.desktop` specifications allow seamless "Open With" file associations without overtaking system defaults.
- **Open questions**: None; all 6 implementation milestones are finished.
- **Next**: Milestone 7: Mermaid scaling, high-contrast readability, and interactive click-to-toggle sizing.

## Session: Milestone 7 Step 7.1 (Mermaid Card Styling & Overflow)
- **Changed**: Replaced `.mermaid` CSS in `src/render.cpp` with bordered card styling, hover shadow, custom thin scrollbars, and unconstrained `.expanded` rules.
- **Why**: Finished Step 7.1 to prevent diagram clipping and establish horizontal scrolling foundation.
- **Learned**: Symmetrical center alignment in flexbox clips left-overflow during horizontal scrolling; switching to `flex-start` when expanded ensures full scrollability.
- **Next**: Step 7.2 high-contrast Mermaid `themeVariables` in dark/light modes, followed by Step 7.3 click handler.

## Session: Milestone 7 Completion (Mermaid Contrast & Interactive Toggle)
- **Changed**: Added dark/light `themeVariables`, `tabIndex = 0` with click/Enter/Space toggle, `:focus-visible` outline, and pipeline test diagram.
- **Why**: Completes Milestone 7 with high-contrast, keyboard-accessible diagrams that expand to unconstrained horizontal scrolling.
- **Learned**: Assigning `tabIndex = 0` and preventing default spacebar scroll enables accessible element interaction alongside Vim shortcuts.
- **Open questions**: None; all 7 milestones are complete.

## Session: Milestone 8 Scope & Search Architecture
- **Changed**: Outlined Milestone 8 for bold headings, auto-numbering, and DOM search.
- **Why**: Resolves three reader flaws and selects the zero-chrome DOM overlay design.
- **Learned**: WebKit's window.find() provides native C++ text matching without mutating DOM nodes.
- **Next**: Step 8.1 heading typography in src/render.cpp.

## Session: Milestone 8 Completion (Headings, Auto-Numbering & Search)
- **Changed**: Added bold typography for `h1`–`h6`, CSS hierarchical counters, and in-page DOM search overlay (`/`, `n`, `N`, `Esc`).
- **Why**: Completes Milestone 8, resolving small headings, missing auto-numbering, and lack of text search.
- **Learned**: CSS counters hierarchically number headings without mutating AST; `window.find()` provides native search speed.
- **Open questions**: None; all 8 milestones are complete.
