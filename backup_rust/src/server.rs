//! Local HTTP server for serving rendered Markdown and assets.
//! Layer 2: Depends on Layer 1 (render).

use crate::render;
use std::path::PathBuf;
use tiny_http::{Header, Response, Server};

/// Binds the HTTP server and serves rendered HTML on the root path `/`.
pub fn start(content: String, base_dir: PathBuf) {
    let server = Server::http("127.0.0.1:0").expect("Failed to bind HTTP server");
    let port = server.server_addr().to_ip().expect("Failed to get IP").port();

    eprintln!("Server running at http://127.0.0.1:{}", port);
    eprintln!("Base directory for assets: {}", base_dir.display());

    // Render HTML from Markdown (Layer 1)
    let html = render::render_markdown_to_html(&content);

    // Listen for incoming requests (thread sleeps here until a request arrives!)
    for request in server.incoming_requests() {
        match request.url() {
            "/" => {
                let header = "Content-Type: text/html; charset=utf-8"
                    .parse::<Header>()
                    .unwrap();
                let response = Response::from_string(&html).with_header(header);
                let _ = request.respond(response);
            }
            _ => {
                // In Step 3.3, we will route local images here!
                let response = Response::from_string("Not Found").with_status_code(404);
                let _ = request.respond(response);
            }
        }
    }
}
