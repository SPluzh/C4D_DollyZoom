# About C4D_DollyZoom

## Purpose
C4D_DollyZoom is a Cinema 4D plugin that provides a custom tool for performing a "Dolly Zoom" (or Vertigo effect). It allows users to drag in the viewport to simultaneously change the camera's focal length and its distance from the target (center of interest), maintaining the same framing for the target object.

## Tech Stack
- **Language**: C++
- **SDK**: Cinema 4D C++ SDK
- **Platform**: Windows (as indicated by the development environment)

## Key Components
- `DollyWoodToolData`: Implements the `CToolData` interface for the custom tool logic.
- `DollyWoodTool`: A wrapper class inheriting from `CPluginToolObject`.
- `dollywood.str`: String table for the tool's name and tooltips.
