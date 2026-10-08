//! Command-line argument and standard input stream handling.
//! Layer 1: Self-contained, zero dependencies on rendering or higher layers.

use std::io::{IsTerminal, Read};
use std::path::{Path, PathBuf};

/// Reads Markdown from either a file path or stdin, returning (content, base_dir).
pub fn load_input() -> (String, PathBuf) {
    let arg = std::env::args().nth(1);

    match arg.as_deref() {
        // Case 1: Explicit dash means read from stdin
        Some("-") => {
            let mut buffer = String::new();
            if let Err(err) = std::io::stdin().read_to_string(&mut buffer) {
                eprintln!("Error reading from stdin: {}", err);
                std::process::exit(1);
            }
            (buffer, PathBuf::from("."))
        }
        // Case 2: A file path was passed
        Some(path_str) => {
            let text = match std::fs::read_to_string(path_str) {
                Ok(t) => t,
                Err(err) => {
                    eprintln!("Error reading '{}': {}", path_str, err);
                    std::process::exit(1);
                }
            };
            let parent = Path::new(path_str)
                .parent()
                .unwrap_or_else(|| Path::new("."));
            let base = if parent.as_os_str().is_empty() {
                PathBuf::from(".")
            } else {
                parent.to_path_buf()
            };
            (text, base)
        }
        // Case 3: No argument passed — check if data is being piped in
        None => {
            if !std::io::stdin().is_terminal() {
                let mut buffer = String::new();
                if let Err(err) = std::io::stdin().read_to_string(&mut buffer) {
                    eprintln!("Error reading from stdin: {}", err);
                    std::process::exit(1);
                }
                (buffer, PathBuf::from("."))
            } else {
                eprintln!("Usage: mdreader <file.md> or cat <file.md> | mdreader");
                std::process::exit(1);
            }
        }
    }
}
