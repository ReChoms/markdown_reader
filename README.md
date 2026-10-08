# mdreader

A fast, lightweight, single-purpose Markdown companion viewer written in C++ and WebKitGTK.

Designed for terminal and Neovim users who need Obsidian-quality fidelity (LaTeX math, Mermaid diagrams, local images) without editor plugins, databases, or daemon bloat.

## Features

- **Unix Philosophy**: Single binary, reads files or piped stdin (`cat doc.md | mdreader`).
- **Rich Rendering**: KaTeX math (`$...$`, `$$...$$`), Mermaid diagrams, local image resolution.
- **Keyboard-First**: Vim keybindings (`j`/`k`, `d`/`u`, `gg`/`G`, `q`) and zoom controls (`+`, `-`, `0`).
- **Live Watcher**: Automatically reloads when file changes on disk, preserving scroll and zoom.
- **Dark & Light Mode**: Automatic system theme detection and high-contrast diagram themes.

## Prerequisites

- C++20 compiler (`g++` or `clang++`)
- `make`, `pkg-config`, `python3`
- WebKitGTK 6 development headers (`webkitgtk-6.0`)

## Build & Install

```bash
# Build binary
make -s mdreader

# Run test suite
make test

# Install to ~/.local/bin
install -Dm755 mdreader ~/.local/bin/mdreader
```

## Usage

```bash
# Open a markdown file
mdreader path/to/document.md

# Pipe via stdin
cat document.md | mdreader

# Configurable scroll history limit (default: 50)
mdreader --max-history 20 document.md
```

## Keybindings

- `j` / `k`: Scroll down / up
- `d` / `u`: Half-page scroll down / up
- `gg` / `G`: Jump to top / bottom
- `+` / `-` / `0`: Zoom in / zoom out / reset zoom (or `Ctrl` + scroll wheel)
- `q`: Quit

## Desktop Integration

```bash
# Register desktop application entry
install -Dm644 mdreader.desktop ~/.local/share/applications/mdreader.desktop
update-desktop-database ~/.local/share/applications
```
