# mypreprocessor

A modular C preprocessor tool that processes C source files by removing comments, expanding `#include` directives, and substituting `#define` macros to generate an extended `.i` source file.

## Features

- **Comment Removal**: Strips single-line (`//`) and multi-line (`/* ... */`) comments while preserving code structure, line breaks, and string/character literals.
- **File Inclusion**: Resolves `#include <filename>` and `#include "filename"` directives by replacing them with the contents of the referenced file (searches the current directory first, then `/usr/include/`).
- **Macro Substitution**: Parses `#define` directives and substitutes all subsequent usages, supporting:
  - Simple object-like macros (e.g., `#define PI 3.14`)
  - Function-like macros with arguments (e.g., `#define SQUARE(x) ((x)*(x))`)

## Project Structure

mypreprocessor/
├── Makefile
├── main.c
├── header.h
├── comment_removal.c
├── file_inclusion.c
└── macro_handler.c
