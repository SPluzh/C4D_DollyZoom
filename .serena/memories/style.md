# Code Style and Conventions

## Naming Conventions
- **Classes**: `PascalCase` (e.g., `DollyWoodToolData`)
- **Functions/Methods**: `PascalCase` (e.g., `InitTool`, `MouseDown`)
- **Variables**: `camelCase` (e.g., `anchorPoint`, `activeCamera`)
- **Macros/Constants**: `SCREAMING_SNAKE_CASE` (e.g., `PLUGIN_ID_DOLLYWOOD`)

## Formatting
- **Braces**: Allman style (braces on a new line after the signature).
- **Indentation**: Standard indentation.

## Patterns
- Uses the standard Cinema 4D `CToolData` pattern for tool implementation.
- Uses `dynamic_cast` for safe casting to `BaseCameraObject*`.
- Includes Russian comments in some places (e.g., in `dollywood.cpp`).
