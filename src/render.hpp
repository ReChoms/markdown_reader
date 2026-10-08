#pragma once

#include <string>

namespace mdreader {

// Converts a Markdown string into an HTML fragment (tags only).
std::string markdown_to_html(const std::string& markdown);

// Wraps an HTML fragment inside a complete HTML5 document with CSS, KaTeX, and Mermaid.js.
std::string wrap_html_document(const std::string& body_html, const std::string& title = "mdreader");

} // namespace mdreader
