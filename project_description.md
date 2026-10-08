# Project Description — `mdreader`

A fast, lightweight, single-purpose Markdown companion viewer written in Rust.

Designed specifically for terminal and Neovim users who want Obsidian-quality visual fidelity (crisp LaTeX math formulas, Mermaid diagrams, local images) without the vault system, database overhead, or bloated editor plugins.

---

## 1. Project Philosophy & Core Features

- **Adheres to the Unix Philosophy**:
  - **Do one thing and do it well**: Strictly a fast, read-only Markdown viewer.
  - **Standard input & pipes**: Accepts both file paths (`mdreader file.md`) and piped stdin (`cat file.md | mdreader`).
  - **Clean stream separation**: Diagnostic and error messages strictly to `stderr`, clean exit `0` on close/`q`.
  - **No forced defaults**: Respects user choice; does not hijack system file associations.
- **High Visual Fidelity**:
  - Crisp LaTeX math formulas (`$...$` and `$$...$$`) via KaTeX.
  - Interactive diagrams via Mermaid.js (` ```mermaid `).
  - Proper resolution of local images relative to the opened markdown file's directory.
  - Automatic desktop dark/light mode detection.
- **Keyboard-First & Neovim-Friendly UX**:
  - Full Vim keybindings: `j`/`k` (scroll), `d`/`u` (half-page), `gg`/`G` (top/bottom), `q` (quit).
  - Smooth mouse wheel scrolling, mouse drag text selection, and standard `Ctrl`+`c` copy.
  - Zoom controls: `+`, `-`, `0` (reset), and `Ctrl` + mouse wheel.
  - Live auto-reload on save (`:w`) preserving exact scroll position and zoom level.

---

## 2. Documentation Structure & Agreed Guidelines

Every document in the repository has a distinct, agreed-upon role:

| Document | Purpose & What Belongs Here |
| :--- | :--- |
| **[`project_description.md`](project_description.md)** | **Project Overview & Standards**: High-level summary of what `mdreader` is, its Unix principles, feature set, and the documentation map. |
| **[`solution_design.md`](solution_design.md)** | **Architecture Blueprint & Requirements**: Comprehensive specification of the technical architecture, component breakdown (parser, watcher, server, window launcher), and milestone scope. |
| **[`plan.md`](plan.md)** | **Implementation Checklist**: Granular micro-steps broken down into checkboxes (`[ ]` / `[x]`) across all 6 milestones to track implementation progress. |
| **[`AGENTS.md`](AGENTS.md)** | **Working Guidelines for Collaboration**: Rules of engagement for AI agents. Enforces discussion before acting, very small code steps (3–8 lines) with educational explanations of Rust, Unix philosophy, and respect for user control. |
| **[`research.md`](research.md)** | **Technical Lookup & Knowledge Base**: Reference sheets for Rust standard library APIs (`std::env`, `std::fs`, `std::io`), crate documentation (`pulldown-cmark`, `tiny_http`, `mime_guess`), KaTeX/Mermaid setup snippets, and Linux desktop integration (`.desktop` format, Chromium app mode). |
| **[`feedback_log.md`](feedback_log.md)** | **Structured Quality Log**: Record of user complaints, root causes, and permanent resolutions/adjustments to prevent recurring mistakes. |
| **[`project_log.md`](project_log.md)** | **Chronological Session Log**: Running journal of session objectives, decisions made, milestones reached, and next session plans. |

---

## 3. Quick Start (Development)

```bash
# Check compilation
cargo check

# Run with a markdown file
cargo run -- path/to/document.md

# Run via pipe
cat path/to/document.md | cargo run
```
