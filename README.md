# C4D Dolly Zoom Plugin

A professional tool for Cinema 4D that implements the classic "Dolly Zoom" (Vertigo) effect. It dynamically adjusts the camera's Field of View (FOV) as you move the camera, keeping the subject at a constant size while the background perspective warps.

## Features
*   Smooth real-time Dolly Zoom interaction.
*   Automatic target detection.
*   Support for Cinema 4D 2025 and 2026.
*   Clean performance-optimized C++ implementation.

## How to Use (Target Selection Logic)

The plugin intelligently determines the focal point of the effect based on the following priority:

1.  **DollyTarget Object**: If you create a Null object and name it exactly `DollyTarget`, the plugin will always use its position as the center of the effect, regardless of your selection.
2.  **Selected Object**: If no `DollyTarget` exists, the plugin will use the position of any object you currently have selected in the Object Manager.
3.  **DOF Auto Mode**: If there are no objects selected and no `DollyTarget` is found, the plugin falls back to an automatic mode using the camera's current **Focus Distance**. This ensures the effect always has a valid pivot point.

## Download

You can download the latest version from the **[Releases](https://github.com/SPluzh/C4D_DollyZoom/releases)** page. 
The release ZIP contains pre-built binaries for different Cinema 4D versions:
*   `2025/C4D_DollyZoom` — for Cinema 4D 2025.
*   `2026/C4D_DollyZoom` — for Cinema 4D 2026.

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
