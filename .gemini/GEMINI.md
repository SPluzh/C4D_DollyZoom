# Cinema 4D Plugin Project (C4D_DollyZoom)

This project is a Cinema 4D plugin.
When working on this project:
- *CRITICAL*: Always use the `cinema-docs` MCP server to search for documentation for any C++ SDK classes, functions, or symbols BEFORE building an implementation plan. The plan must be based on current SDK documentation.
- *CRITICAL*: Strictly use the `language="cpp"` parameter when calling `cinema-docs` MCP tools to ensure you get C++ documentation.
- Cinema 4D SDK uses custom types (e.g., `maxon::Int32`, `maxon::Float`, `cinema::String`, `cinema::BaseDocument`, `cinema::BaseDraw`).
- Do not make assumptions about standard C++ types; refer to C++ SDK documentation using the `cinema-docs` MCP tools.
- Modern Cinema 4D SDK (2024+) uses namespaces (`cinema` and `maxon`). Ensure types are properly qualified or namespaces are declared.

## Implementation Plan Rules
- **Highly Detailed**: The plan must be granular and comprehensive.
- **Specific Changes**: Clearly describe what is changing, which files are affected, and the exact locations of changes.
- **Code Examples**: Always include code examples for new logic or SDK interactions to ensure clarity and correctness before implementation.

## Execution Rules
- **Strict Adherence**: Follow the approved implementation plan exactly. Do not deviate or add unplanned logic during execution.
- **No Documentation Search**: Since all necessary research must be completed during the planning phase, documentation search should not be required during execution.

## SDK 2026+ Coding Standards (Strict Rules)
- **Const Correctness**: Most getters (e.g., `GetPointS()`, `GetEdgeS()`, `GetPointR()`) return `const` pointers or references. Always use `const` qualifiers when retrieving SDK data.
- **Error Handling (`iferr`)**: 
    - Methods that can fail (like `Resize()`, `CopyFrom()`, `Alloc()`) must handle errors using the `iferr` block or `iferr_ignore("description")`.
    - `iferr_ignore()` **MUST** include a string literal describing what is being ignored (e.g., `iferr_ignore("Memory allocation")`).
- **Memory Management**: Use `maxon::BaseArray` or other Maxon collections instead of raw C-style arrays for dynamic grouping/tracking to ensure safe memory handling and SDK compatibility.
- **Naming & Namespaces**: Always prefer `maxon::` and `cinema::` namespaces explicitly or via `using namespace`. Be aware that some symbols may exist in both, requiring explicit resolution.
