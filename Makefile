# ==============================================================================
# COMPILERS
# ==============================================================================
# CXX: The compiler used for modern C++ files (.cpp)
CXX := g++

# CC: The compiler used for pure C files (.c) from vendored libraries
CC  := gcc

# ==============================================================================
# COMPILER FLAGS
# ==============================================================================
# CXXFLAGS: Options passed to g++ when compiling C++ code
#   -std=c++20      : Use the C++20 standard (string_view, optional, modern syntax)
#   -Wall           : "Warn All" - enables standard compiler warnings to catch bugs
#   -Wextra         : Enables additional strict warnings beyond -Wall
#   -Isrc           : Search inside 'src/' directory for headers (e.g. #include "input.hpp")
#   -Ivendor/md4c   : Search inside 'vendor/md4c/' for headers (e.g. #include "md4c-html.h")
WEBKIT_CFLAGS := $(shell pkg-config --cflags webkitgtk-6.0)
WEBKIT_LIBS   := $(shell pkg-config --libs webkitgtk-6.0)

CXXFLAGS := -std=c++20 -Wall -Wextra -Isrc -Ivendor/md4c $(WEBKIT_CFLAGS)

# CFLAGS: Options passed to gcc when compiling pure C code
#   -std=c99        : Use the C99 standard required by the md4c library
#   -O2             : Optimize code for speed (Level 2 optimization)
#   -Ivendor/md4c   : Search inside 'vendor/md4c/' for C headers (e.g. entity.h)
CFLAGS   := -std=c99 -O2 -Ivendor/md4c

# ==============================================================================
# VENDORED OBJECT FILES
# ==============================================================================
# List of pre-compiled machine-code files (.o) for the md4c parser
MD4C_OBJS := vendor/md4c/md4c.o vendor/md4c/entity.o vendor/md4c/md4c-html.o

# ==============================================================================
# BUILD TARGETS
# ==============================================================================

# AUTOMATIC LSP DATABASE: Generate compile_commands.json for Neovim's clangd
compdb:
	@python3 -c 'import json, glob; \
	flags = """$(CXXFLAGS)"""; \
	cmds = [{"directory": "'"$$(pwd)"'", "command": f"g++ {flags} -c {f}", "file": f} for f in sorted(glob.glob("src/*.cpp"))]; \
	open("compile_commands.json", "w").write(json.dumps(cmds, indent=2))'

# MAIN TARGET: Build the 'mdreader' executable
# - Left of colon:  The target we want to produce ('mdreader')
# - Right of colon: The dependencies. If any file changes, rebuild.
# - $@              : Automatic variable meaning "the target name" ('mdreader')
mdreader: compdb src/main.cpp src/input.cpp src/input.hpp src/render.cpp src/render.hpp src/window.cpp src/window.hpp src/state.cpp src/state.hpp $(MD4C_OBJS)
	$(CXX) $(CXXFLAGS) src/main.cpp src/input.cpp src/render.cpp src/window.cpp src/state.cpp $(MD4C_OBJS) $(WEBKIT_LIBS) -o $@

# PATTERN RULE: How to compile any .c file in vendor/md4c/ into a .o file
# - %   : Matches any filename (e.g., md4c.c -> md4c.o)
# - $<  : Automatic variable meaning "the input file" (e.g. vendor/md4c/md4c.c)
# - $@  : Automatic variable meaning "the output file" (e.g. vendor/md4c/md4c.o)
# - -c  : Compile to machine code only; do NOT run the linker yet
vendor/md4c/%.o: vendor/md4c/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# SHORTCUT: 'make run' compiles (if needed) and runs the test markdown file
run: mdreader
	./mdreader test.md

# CLEANUP: 'make clean' removes generated binary and .o object files
clean:
	rm -f mdreader $(MD4C_OBJS) tests/test_state

# TESTS: Compile and run test suite
test: tests/test_state
	./tests/test_state

tests/test_state: tests/test_state.cpp src/state.cpp src/state.hpp
	$(CXX) -std=c++20 -Wall -Wextra -Isrc tests/test_state.cpp src/state.cpp -o $@

# .PHONY tells Make that 'run' and 'clean' are command shortcuts,
# not real files on disk. (Prevents Make from getting confused if a file named 'clean' exists)
.PHONY: run clean compdb test

