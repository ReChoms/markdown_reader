# Research & Technical Reference — `mdreader`

A quick-reference knowledge base for Rust APIs, crate documentation, math rendering, and system integration used in this project.

---

## 1. Rust Standard Library Reference

### Command-Line Arguments (`std::env`)
- `std::env::args()` returns an iterator of Strings:
  - `.nth(0)`: Executable path.
  - `.nth(1)`: First argument passed by user (returns `Option<String>`).
  - `.skip(1)`: An iterator over all arguments except the program name.

### File & Path Handling (`std::fs`, `std::path`)
- `std::fs::read_to_string(&path)`: Reads entire file into a `Result<String, std::io::Error>`.
- `std::path::PathBuf`: Owned, mutable filesystem path.
- `path.parent()`: Returns an `Option<&Path>` pointing to the directory containing the file (used as `base_dir` for resolving images).
- `std::fs::metadata(&path)?.modified()`: Returns the file's last modified timestamp (`SystemTime`) to detect `:w` saves.

### Standard Streams & Terminal Detection (`std::io`)
- `std::io::stdin().is_terminal()`: Returns `true` if connected to an interactive terminal, `false` if connected to a pipe (e.g. `cat file.md | mdreader`).
- `eprintln!`: Writes formatted output directly to `stderr` (preserving `stdout` for clean Unix pipes).

---

## 2. Dependencies & Crate Notes

### `pulldown-cmark`
- Fast, pull-based CommonMark parser with optional extensions:
  ```rust
  let mut options = pulldown_cmark::Options::empty();
  options.insert(pulldown_cmark::Options::ENABLE_MATH);
  options.insert(pulldown_cmark::Options::ENABLE_TABLES);
  options.insert(pulldown_cmark::Options::ENABLE_TASKLISTS);
  options.insert(pulldown_cmark::Options::ENABLE_STRIKETHROUGH);
  options.insert(pulldown_cmark::Options::ENABLE_FOOTNOTES);
  ```
- Converts `$formula$` into `<span class="math math-inline">formula</span>`.
- Converts `$$formula$$` into `<span class="math math-display">formula</span>`.

### `tiny_http`
- Lightweight, synchronous HTTP server (zero async runtime overhead).
- Dynamic ephemeral port allocation:
  ```rust
  let server = tiny_http::Server::http("127.0.0.1:0").unwrap();
  let port = server.server_addr().to_ip().unwrap().port();
  ```

### `mime_guess`
- Automatically deduces MIME type from file extension:
  ```rust
  let mime = mime_guess::from_path(&file_path).first_or_octet_stream();
  ```

---

## 3. Visual Engines Reference

### KaTeX (LaTeX Math)
- Lightweight client-side math renderer:
  - CSS: `https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css`
  - JS: `https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.js`
- Auto-render script:
  ```javascript
  document.querySelectorAll('.math-inline').forEach(el => {
    katex.render(el.textContent, el, { throwOnError: false, displayMode: false });
  });
  document.querySelectorAll('.math-display').forEach(el => {
    katex.render(el.textContent, el, { throwOnError: false, displayMode: true });
  });
  ```

### Mermaid.js (Diagrams)
- Renders ` ```mermaid ` blocks:
  - JS: `https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.min.js`
- Init:
  ```javascript
  mermaid.initialize({ startOnLoad: true, theme: 'neutral' });
  ```

---

## 4. Linux & Window Integration

### Chromium App Mode
- Creates a dedicated, borderless companion window:
  ```bash
  chromium-browser --app=http://127.0.0.1:PORT
  ```

### KDE Desktop Entry Standard
- File path: `~/.local/share/applications/mdreader.desktop`
- Format:
  ```ini
  [Desktop Entry]
  Type=Application
  Name=Markdown Reader
  Exec=/home/siu/.local/bin/mdreader %f
  Terminal=false
  MimeType=text/markdown;text/x-markdown;
  Categories=Utility;Viewer;
  ```

---

## 5. Notes & Scratchpad
*(Add ad-hoc snippets, findings, and lookups here as needed)*
