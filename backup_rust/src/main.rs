//! mdreader — Fast, lightweight Markdown companion viewer.
//! Layer 3 (Top): Entry point that starts the river flow.

mod input;
mod render;
mod server;

fn main() {
    // 1. Get input from file or pipe (Layer 1)
    let (content, base_dir) = input::load_input();

    // 2. Flow down into the server (Layer 2)
    server::start(content, base_dir);
}
