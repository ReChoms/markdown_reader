//! HTML, CSS, KaTeX, and Mermaid template.
//! Layer 0 (Leaf): Pure template with zero project dependencies.

/// Wraps the generated Markdown HTML body in a complete HTML document.
pub fn build_html_page(body_content: &str) -> String {
    format!(
        r#"<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>mdreader</title>
    <!-- 1. Obsidian & GitHub Typography & Auto Dark/Light CSS -->
    <style>
        :root {{
            --bg-color: #ffffff; --text-color: #1f2328; --border-color: #d0d7de;
            --code-bg: #f6f8fa; --blockquote-color: #57606a; --link-color: #0969da;
        }}
        @media (prefers-color-scheme: dark) {{
            :root {{
                --bg-color: #0d1117; --text-color: #e6edf3; --border-color: #30363d;
                --code-bg: #161b22; --blockquote-color: #8b949e; --link-color: #2f81f7;
            }}
        }}
        body {{
            background-color: var(--bg-color); color: var(--text-color);
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            line-height: 1.6; margin: 0; padding: 2rem 1.5rem;
        }}
        .markdown-body {{ max-width: 860px; margin: 0 auto; word-wrap: break-word; }}
        pre, code {{
            font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace;
            background-color: var(--code-bg); border-radius: 6px;
        }}
        code {{ padding: 0.2em 0.4em; font-size: 85%; }}
        pre code {{ padding: 0; font-size: 100%; background: transparent; }}
        pre {{ padding: 1rem; overflow-x: auto; border: 1px solid var(--border-color); }}
        blockquote {{
            margin: 0; padding: 0 1em; color: var(--blockquote-color);
            border-left: 0.25em solid var(--border-color);
        }}
        table {{ border-collapse: collapse; width: 100%; margin: 1rem 0; }}
        table th, table td {{ padding: 6px 13px; border: 1px solid var(--border-color); }}
        table tr:nth-child(2n) {{ background-color: var(--code-bg); }}
        img {{ max-width: 100%; height: auto; }}
        a {{ color: var(--link-color); text-decoration: none; }}
        a:hover {{ text-decoration: underline; }}
        .mermaid {{ display: flex; justify-content: center; margin: 1.5rem 0; }}
    </style>
    <!-- 2. KaTeX Math Rendering ($...$ and $$...$$) -->
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css">
    <script defer src="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.js"></script>
    <script>
        document.addEventListener("DOMContentLoaded", () => {{
            document.querySelectorAll('.math-inline').forEach(el => {{
                katex.render(el.textContent, el, {{ throwOnError: false, displayMode: false }});
            }});
            document.querySelectorAll('.math-display').forEach(el => {{
                katex.render(el.textContent, el, {{ throwOnError: false, displayMode: true }});
            }});
        }});
    </script>
    <!-- 3. Mermaid.js Diagram Engine (```mermaid blocks) -->
    <script type="module">
        import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.esm.min.mjs';
        mermaid.initialize({{ startOnLoad: false, theme: 'neutral' }});
        document.addEventListener("DOMContentLoaded", async () => {{
            document.querySelectorAll('pre code.language-mermaid').forEach(el => {{
                const pre = el.parentElement;
                const div = document.createElement('div');
                div.className = 'mermaid';
                div.textContent = el.textContent;
                pre.parentNode.replaceChild(div, pre);
            }});
            await mermaid.run();
        }});
    </script>
</head>
<body>
    <article class="markdown-body">
{}
    </article>
</body>
</html>"#,
        body_content
    )
}
