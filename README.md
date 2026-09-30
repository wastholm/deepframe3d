# Deepframe3D

A simple Plasma/KDE application for viewing MPO (Multi-Picture Object) stereo 3D images.

## Features

- **Viewing modes:**
  - Anaglyph (Red/Cyan) - for use with red-cyan 3D glasses
  - Side-by-Side - display both images horizontally
  - Wiggle - alternate between left and right images for depth perception

- **Slideshow support:** Load multiple files and navigate through them
- **Fullscreen mode** with F11 or via menu
- **Keyboard shortcuts:**
  - Ctrl+Q: Quit
  - F12: Toggle fullscreen
  - Left/Right arrows: Previous/Next file
  - Space: Play/Pause slideshow
  - +/=: Faster slideshow
  - -: Slower slideshow

- **File opening:**
  - Command line: `deepframe3d file1.mpo file2.mpo`
  - Drag and drop files onto the window
  - File > Open menu

- **Persistent settings:** Remembers window position, size, and viewing mode

## Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make
sudo make install
```

## Installation

After installation:
- The application will appear in your KDE menu
- `.mpo` files can be opened with Deepframe3D
- File association is handled via the `.desktop` file

## MIME Type

The application registers the `image/mpo` MIME type and associates `.mpo` files with it.

## Requirements

- Qt 6 (Core, Gui, Quick, Qml, QuickControls2)
- C++17 compiler
- CMake 3.16+

## Project Structure

- `src/` - Source code
- `resources/` - Desktop entry file
- `icons/` - Application icon
- `mimetypes/` - MIME type definition
