//! Markdown parsing and HTML generation.
//! Layer 1: Depends only on Layer 0 (template.rs).

mod template;

use pulldown_cmark::{Options, Parser};

/// Converts raw Markdown text into a complete HTML document with math, diagrams, and typography.
pub fn render_markdown_to_html(markdown_content: &str) -> String {
    // Full Obsidian & GitHub Flavored Markdown support
    let mut options = Options::empty();
    options.insert(Options::ENABLE_MATH);
    options.insert(Options::ENABLE_TABLES);
    options.insert(Options::ENABLE_TASKLISTS);
    options.insert(Options::ENABLE_STRIKETHROUGH);
    options.insert(Options::ENABLE_FOOTNOTES);
    options.insert(Options::ENABLE_GFM);
    options.insert(Options::ENABLE_SMART_PUNCTUATION);
    options.insert(Options::ENABLE_HEADING_ATTRIBUTES);
    options.insert(Options::ENABLE_YAML_STYLE_METADATA_BLOCKS);
    options.insert(Options::ENABLE_SUPERSCRIPT);
    options.insert(Options::ENABLE_SUBSCRIPT);
    options.insert(Options::ENABLE_WIKILINKS);

    let parser = Parser::new_ext(markdown_content, options);

    let mut body_html = String::new();
    pulldown_cmark::html::push_html(&mut body_html, parser);

    // Flow down to Layer 0 (template)
    template::build_html_page(&body_html)
}
