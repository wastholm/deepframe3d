# Vibe Agent Instructions for Deepframe3D

## Build System
- Use CMake
- The `VERSION` file must always contain the current version number (and nothing else)
- A `build.sh` script is provided for convenience

## Project Structure
- `src/` - Source code
- `resources/` - Desktop entry file
- `icons/` - Application icon
- `mimetypes/` - MIME type definition

## Key Files
- `src/main.qml` - Main QML application
- `src/backend.cpp/h` - Backend logic and MPO parsing
- `src/managedimageprovider.cpp/h` - Image provider for QML
- `src/mpo.cpp/h` - MPO frame extraction
- `resources/deepframe3d.desktop` - Desktop entry
- `VERSION` - Current version number

## Coding Style
- Match existing style (indentation, naming, etc.)
- Minimal diff
- No unused imports
- Respect Qt conventions
