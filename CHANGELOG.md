# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0] - 2026-09-30

### Added
- Initial release of Deepframe3D
- MPO stereo image viewing with three modes:
  - Anaglyph (Red/Cyan) for use with 3D glasses (placeholder implementation)
  - Side-by-Side display
  - Wiggle mode (alternating between left/right images)
- Slideshow support for multiple files
- Fullscreen mode (F12)
- File loading via command line, drag-and-drop, or Open menu
- Keyboard shortcuts:
  - Ctrl+Q: Quit
  - F12: Toggle fullscreen
  - Left/Right: Previous/Next file
  - Space: Play/Pause slideshow
  - Up/Down: Faster/Slower slideshow
  - A: Anaglyph mode
  - S: Side-by-Side mode
  - W: Wiggle mode
- Toast notifications for play/pause, speed changes, and mode changes
- Status bar showing current file, dimensions, and mode
- System font size support for UI text
