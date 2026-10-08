# Solution Design: Lightweight Markdown Reader (`mdreader`)

## 1. Problem Statement & Motivation
- **The Goal**: A simple, fast, read-only Markdown viewer written in Rust.
- **Why not Obsidian?** Obsidian requires a "vault", creates `.obsidian` configuration directories, indexes files into a database, and is heavy. We want to open **any single `.md` file** from anywhere on the filesystem without vaults or setup.
- **Why not Neovim plugins?** Rendering full graphical images, Mermaid diagrams, and complex LaTeX math formulas (fractions, matrices, square roots) inside Neovim requires a cascade of heavy plugins (`image.nvim`, LuaRocks, ImageMagick, etc.) that can bloat and destabilize your editor config.
- **The Ideal Workflow**:
  - Keep Neovim clean and lean for editing text.
  - Run `mdreader <file.md>` directly from the terminal or from Neovim (`:!mdreader % &`).
  - Read rendered text, LaTeX math, Mermaid diagrams, and images in a dedicated side-by-side reader window with full Vim keybindings, mouse scrolling, and live auto-reload on save.

---

## 2. Unix Philosophy Compliance

To make `mdreader` easy, predictable, and composable:

1. **Do One Thing Well**: Read and render Markdown with math, diagrams, and images cleanly. No editing, no note graph databases, no proprietary lock-in.
2. **Standard CLI & Composability**:
   - Accepts a file path: `mdreader notes.md`
   - Accepts standard input (piping): `cat notes.md | mdreader` or `curl -s https://example.com/doc.md | mdreader`
   - Integrates effortlessly with tools like `fzf`, `ranger`, `find`, and shell aliases:
     ```bash
     fzf | xargs mdreader
     ```
3. **Clean Stream Separation**:
   - Status / logs stay minimal.
   - Diagnostic and error messages go strictly to `stderr`.
4. **Standard Exit Codes & Signal Handling**:
   - Exits with `0` on clean close (`q` or window close).
   - Gracefully handles `SIGINT` (<kbd>Ctrl</kbd>+<kbd>c</kbd>) and `SIGTERM`.
5. **No Forced Defaults**:
   - Provides a standard `mdreader.desktop` template for manual configuration in KDE / desktop settings without hijacking system defaults.

---

## 3. Key Requirements & Features

| Category | Requirement | Details |
| :--- | :--- | :--- |
| **Simplicity** | Zero-configuration | Run `mdreader notes.md`. No accounts, no vaults, no config files. |
| **Math** | Full LaTeX Rendering | Inline math (`$E=mc^2$`) and block display math (`$$\int ...$$`) rendered crisply via KaTeX. |
| **Diagrams** | Mermaid.js Support | Render flowcharts, sequence diagrams, class diagrams from ` ```mermaid ` blocks. |
| **Images** | Local & Remote Images | `![label](images/diagram.png)` resolved relative to the markdown file's directory. |
| **Navigation** | Neovim Keybindings | `j`/`k` (scroll), `d`/`u` (half-page), `gg`/`G` (top/bottom), `q` (quit). |
| **Zoom Support** | Zoom In / Out / Reset | Adjust font & layout size via keyboard (`+`/`-`/`0`) or `Ctrl` + mouse wheel. Persisted across reloads. |
| **Mouse Support** | Mouse Wheel & Copy | Smooth mouse wheel scrolling, mouse text selection, and standard `Ctrl`+`c` copying. |
| **Live Sync** | Auto-reload on Save | When you `:w` in Neovim, the reader reloads instantly, keeping your exact scroll position and zoom level. |
| **Lightweight** | Fast Compile & Small Binary | Written in standard Rust with minimal dependencies, compiles in seconds. |

---

## 4. Workflow & How It Runs From the Terminal

### Terminal & Neovim Integration
You can launch the reader in multiple standard ways:

1. **Directly from Terminal**:
   ```bash
   mdreader notes.md
   ```
2. **From inside Neovim**:
   Run `:!mdreader % &` or bind it in your `init.lua`:
   ```lua
   vim.keymap.set("n", "<leader>mp", ":silent !mdreader % &<CR>", { desc = "Markdown Preview" })
   ```
3. **From a Unix Pipe**:
   ```bash
   cat notes.md | mdreader
   ```

### The "Second Window" Experience
- **Terminal limitations**: Standard terminal grid cells cannot render vector math fonts or graphics without specialized terminal protocols.
- **Dedicated Companion Window (Side-by-Side)**:
  `mdreader` launches a minimal, distraction-free companion window (no address bar, no tabs, no menus) positioned right next to your terminal/Neovim.
  - You edit in Neovim on one side.
  - You view the rendered output on the other side.
  - Both update in real-time as you edit and save.

---

## 5. Technical Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Terminal / Neovim                      │
│            $ mdreader file.md   OR   cat file.md | mdreader │
└──────────────────────────────┬──────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                 Modern C++20 Backend (`mdreader`)           │
│                                                             │
│  1. Input Handler: Read from file OR stdin                  │
│  2. Parse Markdown & Math (md4c C99 engine + Math flags)    │
│  3. Local File & Asset Resolver (images relative to .md)    │
│  4. File Watcher (detects :w / file modification)           │
│  5. Native Desktop Window Engine (WebKitGTK 6.0, 0 localhost)│
└──────────────────────────────┬──────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                 Dedicated Reader Window                     │
│                                                             │
│  • Obsidian-style Typography & Themes (Dark / Light)        │
│  • KaTeX Math Rendering ($...$ and $$...$$)                 │
│  • Mermaid.js Diagram Engine (flowcharts, sequence, etc.)   │
│  • Inline Local Images (PNG, JPG, SVG, WebP)                │
│  • Vim Keybindings Listener (j, k, d, u, gg, G, q)          │
│  • Zoom Controls (+, -, 0, Ctrl+wheel) & Persistence        │
│  • Mouse Wheel Scrolling & Text Selection / Copy            │
│  • Auto-Reload Listener (preserves scroll & zoom)           │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. Keyboard & Mouse Controls

| Input | Action |
| :--- | :--- |
| `j` / `Down` | Scroll down smoothly |
| `k` / `Up` | Scroll up smoothly |
| `d` / `Ctrl-d` | Jump half page down |
| `u` / `Ctrl-u` | Jump half page up |
| `gg` / `Home` | Jump to top of document |
| `G` / `End` | Jump to bottom of document |
| `+` / `=` / `Ctrl` + `+` | **Zoom In** |
| `-` / `Ctrl` + `-` | **Zoom Out** |
| `0` / `Ctrl` + `0` | **Reset Zoom (100%)** |
| `Ctrl` + **Mouse Wheel** | **Zoom In / Out** |
| `q` | Quit / close window |
| **Mouse Wheel** | Smooth vertical scrolling |
| **Mouse Drag** | Highlight and select text |
| `Ctrl` + `c` | Copy selected text |

---

## 7. Implementation Plan & Milestones

1. **Milestone 1: CLI Arguments, Unix Pipes & Stdin Support**
   - Accept file path or read from stdin (`-` or pipe).
   - Locate base directory for relative images.

2. **Milestone 2: Markdown, Math & Mermaid Translation Engine**
   - Configure `pulldown-cmark` with all formatting and math options.
   - Integrate KaTeX and Mermaid.js.
   - Build HTML layout with zoom scaling and clean dark/light typography.

3. **Milestone 3: Local Asset & Image Serving**
   - Serve rendered HTML at root `/`.
   - Handle local relative/absolute image paths with proper MIME types.

4. **Milestone 4: Live File Watching & State Preservation**
   - Check file modification timestamp.
   - Maintain scroll position and zoom level across auto-reloads.

5. **Milestone 5: Vim Keybindings, Zoom Controls & Window Integration**
   - Add keyboard navigation (`j`, `k`, `gg`, `G`, `d`, `u`, `+`, `-`, `0`, `q`).
   - Add Ctrl+wheel zoom.
   - Launch dedicated app window or browser fallback.

6. **Milestone 6: KDE Desktop Entry & End-to-End Verification**
   - Provide standard `mdreader.desktop` template for KDE.
   - Test with a sample markdown document containing equations, Mermaid diagrams, local images, zoom in/out, tables, and code blocks.
