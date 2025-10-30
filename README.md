# Minimal OpenXR API Layer Template

A stripped-down, modern C + CMake template for building OpenXR API layers on Windows and Linux. Based on the C++ flavored [OpenXR-Layer-Template](https://github.com/mbucchia/OpenXR-Layer-Template) by Matthieu Bucchianeri.

## Features

- **Pure C11** - No C++ required
- **CMake-based** - Cross-platform build system
- **Multi-platform** - Works on Windows and Linux
- **Minimal dependencies** - Only requires OpenXR SDK headers

## Structure

```
openxr_layer_template/
├── src/
│   ├── layer.h                 # Layer context and state
│   ├── layer.c                 # Context management
│   ├── dispatch.h              # Dispatch function declarations
│   ├── dispatch.c              # Core dispatch implementation
│   ├── dispatch_core.c         # Core function interception
│   └── generated/
│       ├── layer_gen.h         # Generated layer boilerplate
│       ├── layer_gen.c         # Generated layer functions
│       └── dispatch_gen.h      # Generated dispatch declarations
├── scripts/
│   ├── generate_layer.py       # Code generation tool
│   ├── layer_config.py         # Layer configuration for code gen
│   ├── manifest.json.in        # Layer manifest template
│   ├── layer_config.h.in       # Generated #define header
│   ├── enable-layer.py.in      # Cross-platform layer installer
│   ├── disable-layer.py.in     # Layer uninstaller
│   ├── enable-layer.sh.in      # Linux quick install script
│   ├── enable-layer.bat.in     # Windows quick install script
│   └── README.md               # Scripts documentation
├── CMakeLists.txt              # Build configuration
└── README.md
```

## Building

```bash
# 1. Configure
mkdir build
cmake -B build

# 2. Build
cmake --build build

# Activate OpenXR layer (Linux)
./build/enable-layer.sh
# or (Windows)
build/enable-layer.bat
# or (either)
python3 enable-layer.py --implicit

# Generate new API stubs

# Update scripts/layer_config.py with desired functions
# Then call this to hook those functions up to the layer, and overwrite 
# src/dispatch.c with empty function stubs
python3 scripts/generate_layer.py
```

## Resources

- [OpenXR Specification](https://www.khronos.org/registry/OpenXR/specs/1.0/html/xrspec.html)
- [OpenXR Loader](https://www.khronos.org/registry/OpenXR/specs/1.0/loader.html)
- [API Layers](https://www.khronos.org/registry/OpenXR/specs/1.0/html/xrspec.html#api-layers)
