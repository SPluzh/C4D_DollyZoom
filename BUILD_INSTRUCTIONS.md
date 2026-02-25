# Building Cinema 4D DollyZoom Plugin

The original plugin code was updated to be compatible with modern Cinema 4D versions (2025, 2026, and newer).

## Code Fixes and Updates
The following changes were made to the source code for modern compatibility:
1. **ToolData API**: Replaced legacy tool classes with the modern `cinema::ToolData`.
2. **Mouse Interaction**: Updated to the modern `MouseDragStart`, `MouseDrag`, and `MouseDragEnd` workflow.
3. **Camera Objects**: Updated to use `cinema::CameraObject`.
4. **Qualifiers**: Replaced old macros with modern `QSHIFT`, `QCTRL`, etc.
5. **Parameters**: Updated parameter access using `ConstDescIDLevel`. Resolved namespace conflicts (e.g., using `maxon::PI`).
6. **Entry Points**: Added standard initialization functions (`PluginStart`, `PluginEnd`, `PluginMessage`) inside the `cinema` namespace.

## Build Infrastructure
To support different Cinema 4D versions, the project uses a multi-SDK directory structure:
*   `sdk_2025`: Targeted for Cinema 4D 2025.
*   `sdk_2026`: Targeted for Cinema 4D 2026.

Each SDK folder contains its own `custom_paths.txt` file which points to the source code in the parent directory.

## Setup Instructions

### 1. Requirements
*   **Visual Studio 2022** with "Desktop development with C++" workload.
*   **Python 3** (required by Maxon Source Processor).

### 2. SDK Setup (Crucial)
Due to licensing and file size, the full Cinema 4D SDK frameworks are not included in this repository. You must provide them manually:

1.  **For 2025**: Copy the `frameworks` and `project` folders from the official Cinema 4D 2025 SDK into the `sdk_2025/` directory.
2.  **For 2026**: Copy the `frameworks` and `project` folders from the official Cinema 4D 2026 SDK into the `sdk_2026/` directory.

The final structure should look like this:
```
C4D_DollyZoom/
├── sdk_2025/
│   ├── frameworks/   <-- Copy from 2025 SDK
│   ├── project/      <-- Copy from 2025 SDK
│   ├── CMakeLists.txt
│   └── custom_paths.txt
└── sdk_2026/
    ├── frameworks/   <-- Copy from 2026 SDK
    ├── project/      <-- Copy from 2026 SDK
    ├── CMakeLists.txt
    └── custom_paths.txt
```

## How to Build

Use the provided batch files in the project root:

*   **For Cinema 4D 2025**: Run [build_2025.bat](file:///c:/Users/user/Desktop/cpp/C4D_DollyZoom/build_2025.bat)
*   **For Cinema 4D 2026**: Run [build_2026.bat](file:///c:/Users/user/Desktop/cpp/C4D_DollyZoom/build_2026.bat)

The scripts will:
1.  Configure the C++ project using CMake.
2.  Compile the plugin in **Release** mode.
3.  Clean up temporary `.pdb` files.

## Output Locations

The compiled plugin `C4D_DollyZoom.xdl64` and its resources (`res`) will be located in:
*   **2025**: `sdk_2025/build/bin/Release/plugins/C4D_DollyZoom/`
*   **2026**: `sdk_2026/build/bin/Release/plugins/C4D_DollyZoom/`

To install the plugin, copy the `C4D_DollyZoom.xdl64` file and the `res` folder into your Cinema 4D `plugins` directory.
