# Aria

**A lightweight, digital painting application**

![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux%20%7C%20Windows-blue)
![License](https://img.shields.io/badge/license-Unlicense-green)
![Qt](https://img.shields.io/badge/Qt-6-41cd52)

Aria is a free, open-source digital painting application inspired by professional tools like Clip Studio Paint and Krita. Built with Qt 6 and C++, it provides a fast, somwwhat intuitive painting experience with powerful (in the future) features for artists.

## Features

### Mostly Planned but Half-Implemented Features
- **Brush System** - Drawing with customizable brushes (Custom brushes will be implemented sometime)
- **Layer Management** - Multiple layers with blend modes and opacity control
- **Selection Tools** - Rectangle, ellipse, and lasso selection
- **Fill Bucket** - Flood fill with tolerance control
- **Eyedropper** - Pick colors from your canvas
- **HSV Color Picker** - Professional-grade color selection (This is almost finished)
- **Keyboard Shortcuts** - Fully customizable shortcuts
- **File Support** - Open and save PNG, JPEG, and native .aria format (File export works but .aria needs to be implemented)

### More Planned Features
- Advanced brush dynamics and presets
- Layer groups and masks
- Text tool with typography controls
- Filters and adjustments (blur, sharpen, curves, levels)
- Symmetry painting
- Image reference viewer
- GPU acceleration (Metal/Vulkan)

## Screenshots

*Screenshots maybe coming soon*

## Getting Started

### Prerequisites
- **Qt 6.2 or later**
- **CMake 3.16 or later**
- **C++17 compatible compiler**

### Building from Source

#### macOS
```bash
# Install Qt via Homebrew
brew install qt@6 cmake

# Clone and build
git clone https://github.com/cogrow4/Aria.git
cd Aria
cmake -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
cmake --build build
./build/Aria
```

#### Linux
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt install qt6-base-dev cmake build-essential

# Clone and build
git clone https://github.com/cogrow4/Aria.git
cd Aria
cmake -B build
cmake --build build
./build/Aria
```

#### Windows
```bash
# Install Qt via official installer or vcpkg
# Then use CMake GUI or command line

git clone https://github.com/cogrow4/Aria.git
cd Aria
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
cmake --build build --config Release
```

## Usage

### Basic Workflow
1. **Create a new canvas**: File → New (or Ctrl+N)
2. **Select a tool**: Use toolbar or keyboard shortcuts (B for brush, E for eraser) (Currently all select tools are broken)
3. **Choose a color**: Click the color picker panel
4. **Draw**: Click and drag on the canvas
5. **Manage layers**: Use the Layers panel to add, delete, or organize layers
6. **Save your work**: File → Save (Ctrl+S)

### Keyboard Shortcuts
- `B` - Brush tool
- `E` - Eraser tool
- `I` - Eyedropper tool
- `M` - Rectangle select
- `Shift+M` - Ellipse select
- `O` - Lasso select
- `G` - Fill bucket
- `Ctrl+N` - New canvas
- `Ctrl+S` - Save
- `Ctrl+Z` - Undo (coming soon)

## 🏗️ Architecture

Aria is built with a modular architecture:

```
src/
  ├── core/          # Core painting engine (Canvas, Brush, Layers)
  ├── ui/            # Main window and UI components
  ├── tools/         # Tool implementations
  ├── rendering/     # Drawing and compositing
  ├── utils/         # Utilities and managers
  └── widgets/       # Custom Qt widgets
```

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed design documentation.

## Contributing

Contributions are welcome! Aria is released into the public domain under The Unlicense, making it easy for anyone to contribute.

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code style, development process, and how to submit pull requests.

## License

This software is released into the **public domain** under [The Unlicense](LICENSE).

You are free to use, modify, and distribute this software for any purpose, commercial or non-commercial, without any restrictions.

### Third-Party Software

Aria uses Qt Framework, which is licensed under LGPL v3. See [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md) for details.

## Acknowledgments

- Built with [Qt Framework](https://www.qt.io/)
- Inspired by Clip Studio Paint, Krita, and Fire Alpaca



## 🗺️ Roadmap

See our [project board](https://github.com/cogrow4/Aria/projects) for upcoming features and development status.

---

**Star ⭐ this repository if you find it useful!**
