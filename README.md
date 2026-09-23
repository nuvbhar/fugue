# Fugue

Fugue is a modular, cross-platform terminal user interface (TUI) music player developed in C++23. It aims to provide a highly customizable and performant audio playback experience directly from the terminal, utilizing a purpose-built rendering engine.

## Features

- **Custom Terminal Engine**: A built-in rendering engine utilizing double-buffering and raw ANSI/VT escape sequences, bypassing the need for ncurses.
- **Cross-Platform Compatibility**: Native support for Windows and POSIX-based systems (Linux, macOS, BSD) through a dedicated Platform Abstraction Layer (PAL).
- **Format Support**: Broad audio format compatibility powered by FFmpeg and miniaudio.
- **Deep Customizability**: Flexible JSON-based configuration for dynamic UI layouts, themes, and customizable keybindings.
- **Extensible Architecture**: A robust runtime plugin system utilizing a stable C ABI.
- **Advanced Terminal Graphics**: Support for high-resolution album art rendering via Sixel, Kitty, and iTerm2 protocols, with graceful text-based fallbacks for older terminals.

## Architecture

Fugue is structured around self-contained modules that communicate through a typed event bus. This decoupled design ensures that the audio engine, decoding logic, user interface, and plugin system remain strictly independent. 

## Documentation

- **User Documentation**: Public-facing guides and installation instructions are available in the `/docs` directory.

## License

This project is licensed under the MIT License.
