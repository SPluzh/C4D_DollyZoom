# C4D Dolly Zoom Plugin

A professional tool for Cinema 4D that implements the classic "Dolly Zoom" (Vertigo) effect. It dynamically adjusts the camera's Field of View (FOV) as you move the camera, keeping the subject at a constant size while the background perspective warps.


https://github.com/user-attachments/assets/178d0cdf-09ab-4c23-8c07-784dcd73fa96


## Features
*   Flexible target selection:
    *   **DollyTarget** null object (highest priority).
    *   Selected **geometric components** (Points, Edges, Polygons).
    *   Selected **Object** (fallback).
*   Focal length snapping (Ctrl key).
*   Precision speed control (Shift key).
*   Support for Cinema 4D 2025 and 2026.

## How to Use (Target Selection Logic)

The plugin intelligently determines the focal point of the effect based on the following priority:

1.  **DollyTarget Object**: If you create a Null object and name it exactly `DollyTarget`, the plugin will always use its position.
2.  **Selected Component**: If no `DollyTarget` exists, the plugin uses the center of your selected **points, edges, or polygons**.
3.  **Selected Object**: If no component is selected, it uses the position of the currently selected object.
4.  **DOF Auto Mode**: If nothing is selected and no `DollyTarget` is found, it falls back to the camera's **Focus Distance**.

## Modifier Keys (During Drag)

*   **No key**: Standard speed.
*   **Shift**: Precision mode (much slower movement) for fine-tuning.
*   **Control (Ctrl)**: Snapping mode. The focal length snaps to the nearest **integer millimeter** (e.g., 24mm, 35mm, 50mm). The dragging speed remains the same as standard mode.

## Download

You can download the latest version from the **[Releases](https://github.com/SPluzh/C4D_DollyZoom/releases)** page. 
The release ZIP contains pre-built binaries for different Cinema 4D versions:
*   `2025/C4D_DollyZoom` — for Cinema 4D 2025.
*   `2026/C4D_DollyZoom` — for Cinema 4D 2026.

## Changelog

See **[CHANGELOG.md](CHANGELOG.md)** for a full list of changes.

## Installation

1.  Download and extract the `C4D_DollyZoom_vX.X.X.zip`.
2.  Open your Cinema 4D version folder (2025 or 2026).
3.  Copy the entire `C4D_DollyZoom` folder.
4.  Paste it into your Cinema 4D **plugins** directory:
    *   *Path example:* `C:\Program Files\Maxon Cinema 4D 202X\plugins\`
    *   *Alternative path:* `C:\Users\<User>\AppData\Roaming\Maxon\Maxon Cinema 4D 202X_XXXXXXXX\plugins\`
5.  Restart Cinema 4D.

## Building from Source

If you want to compile the plugin yourself or modify the code, please refer to the **[BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)** for detailed environment setup and compilation steps.

## License
MIT License - Created for the Cinema 4D community.
