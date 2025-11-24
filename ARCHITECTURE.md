# Aria Architecture

## Overview

Aria is built as a modular digital painting application using Qt 6 and C++17. The architecture follows a layered approach with clear separation of concerns between the rendering engine, UI, and tool systems.

## System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     User Interface Layer                 │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │  MainWindow  │  │    Panels    │  │   Dialogs    │  │
│  │              │  │  (Layers,    │  │  (Welcome,   │  │
│  │  - Menus     │  │   Brush,     │  │   Settings)  │  │
│  │  - Toolbars  │  │   Color)     │  │              │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────┴───────────────────────────────────┐
│                     Tool System Layer                    │
│  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐     │
│  │Brush │  │Eraser│  │Select│  │ Fill │  │ More │     │
│  │ Tool │  │ Tool │  │ Tool │  │ Tool │  │Tools │     │
│  └──────┘  └──────┘  └──────┘  └──────┘  └──────┘     │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────┴───────────────────────────────────┐
│                   Core Painting Engine                   │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │    Canvas    │  │LayerManager  │  │    Brush     │  │
│  │              │  │              │  │              │  │
│  │  - Events    │  │  - Layers    │  │  - Props     │  │
│  │  - Rendering │  │  - Composite │  │  - Dynamics  │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────────────────────────────────────────┘
```

## Directory Structure

```
src/
├── core/              # Core painting engine
│   ├── canvas         # Main canvas widget
│   ├── brush          # Brush system
│   ├── layer          # Layer & LayerManager
│   └── selection      # Selection management
│
├── ui/                # User interface
│   ├── mainwindow     # Main application window
│   ├── panels/        # Dockable panels
│   │   ├── layerpanel
│   │   └── brushpanel
│   └── dialogs/       # Modal dialogs
│       └── welcomedialog
│
├── tools/             # Tool implementations
│   ├── brushtool
│   ├── erasertool
│   ├── selecttool
│   └── filltool
│
├── rendering/         # Drawing & compositing
│   ├── compositor     # Layer compositing
│   └── strokerenderer # Brush stroke rendering
│
├── utils/             # Utilities
│   ├── shortcuts/     # Keyboard shortcut system
│   └── fileio/        # File I/O operations
│
└── widgets/           # Custom Qt widgets
    ├── hsvcolorpicker # HSV color picker
    └── ...
```

## Core Components

### Canvas (`core/canvas`)

The Canvas is the central widget where all drawing occurs. It manages:
- Mouse  and tablet input events
- Tool dispatching
- Layer composition caching
- Selection state
- Viewport transformations

**Key responsibilities:**
- Accept input events (mouse, tablet, keyboard)
- Route events to active tool
- Composite layers for display
- Manage selection region

```cpp
class Canvas : public QWidget {
    // Input handling
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    
    // Rendering
    void paintEvent(QPaintEvent *event) override;
    
    // Layer management
    LayerManager* layerManager();
    
    // Tool system
    void setTool(ToolType tool);
    
    // Selection
    QRegion selectionRegion() const;
};
```

### LayerManager (`core/layer`)

Manages the layer stack and compositing:
- Layer creation, deletion, reordering
- Compositing with blend modes
- Current layer tracking

```cpp
class LayerManager {
    // Layer operations
    void addLayer(const QString &name, int width, int height);
    void deleteLayer(int index);
    void moveLayer(int from, int to);
    
    // Compositing
    QImage composite(int width, int height);
    
    // Current layer
    Layer* currentLayer();
    void setCurrentLayer(int index);
};
```

### Brush (`core/brush`)

Encapsulates brush properties and behavior:
- Size, color, opacity, hardness
- Pressure sensitivity curves
- Brush presets

```cpp
class Brush {
    // Properties
    void setSize(int size);
    void setColor(const QColor &color);
    void setOpacity(int opacity);
    void setHardness(int hardness);
    
    // Dynamics
    void setPressureSizeEnabled(bool enabled);
    void setPressureOpacityEnabled(bool enabled);
};
```

## Data Flow

### Drawing a Brush Stroke

```
User Input (Mouse/Tablet)
        ↓
  Canvas::mousePressEvent()
        ↓
    Tool Router
        ↓
   Brush Tool
        ↓
Canvas::drawLineTo(point, pressure)
        ↓
  Paint on Current Layer
        ↓
LayerManager::composite()
        ↓
  Canvas::update()
        ↓
   Display Update
```

### Selection Creation

```
  Mouse Press
       ↓
  Start Selection
  (store start point)
       ↓
   Mouse Move
(update selection preview)
       ↓
  Mouse Release
(finalize selection region  )
       ↓
Store in m_selectionRegion
       ↓
  Constrain Drawing
```

## Event System

Aria uses Qt's signal/slot mechanism for loose coupling:

```cpp
// Layer changes
LayerManager::layerAdded(int index)
LayerManager::currentLayerChanged(int index)
LayerManager::canvasUpdateNeeded()

// Color picking
Canvas::colorPicked(QColor color)

// Brush changes
Brush::sizeChanged(int size)
Brush::colorChanged(QColor color)
```

## Rendering Pipeline

### Layer Compositing

1. Create blank QImage (canvas size)
2. Iterate through layers (bottom to top)
3. Apply blend mode and opacity
4. Composite onto result image
5. Cache result in Canvas::m_image

### Brush Rendering

1. Receive mouse/tablet input
2. Calculate intermediate points (for smooth strokes)
3. For each point:
   - Apply pressure to size/opacity
   - Draw with QPainter (round cap/join)
   - Respect selection region
4. Mark dirty region for update

## Threading Model

Currently single-threaded with future plans for:
- Background thread for large operations (compositing, filters)
- GPU offload for brush rendering
- Async file I/O

## Performance Optimizations

1. **Layer Composition Caching**
   - Cache composited result in Canvas
   - Only recomposite when layers change

2. **Dirty Rectangle Tracking**
   - Only repaint modified regions
   - Minimize full canvas redraws

3. **Lazy Evaluation**
   - Defer expensive operations
   - Composite on-demand

## Design Patterns Used

- **Observer Pattern**: Signal/slot for event notification
- **Strategy Pattern**: Tool system (different drawing behaviors)
- **Composite Pattern**: Layer hierarchy (future layer groups)
- **Command Pattern**: Future undo/redo system
- **Singleton Pattern**: ShortcutManager

## Future Architecture Improvements

1. **Plugin System**: Allow third-party tools and brushes
2. **Document Model**: Separate document data from view
3. **GPU Rendering**: Metal/Vulkan render backend
4. **Undo/Redo**: Command pattern with history stack
5. **Scripting**: Python/JavaScript API for automation

## Dependencies

### Required
- **Qt 6.2+**: Core, GUI, Widgets modules
- **C++17**: Standard library, filesystem

### Future
- **Vulkan/Metal**: GPU acceleration
- **FreeType**: Advanced text rendering
- **Python**: Scripting support

## Build System

CMake is used for cross-platform builds:
- Automatic Qt MOC, UIC, RCC invocation
- Platform-specific bundling (macOS .app, Windows installer)
- Dependency management

## Testing Strategy

(To be implemented)
- **Unit Tests**: Core components (Brush, LayerManager)
- **Integration Tests**: Tool workflows  
- **UI Tests**: Automated UI interaction
- **Performance Tests**: Rendering benchmarks

## Code Style

- Follow Qt naming conventions
- CamelCase for classes, camelCase for methods
- Private members prefixed with `m_`
- Document public APIs
- Keep functions small and focused

---

For contributing guidelines, see [CONTRIBUTING.md](CONTRIBUTING.md)
