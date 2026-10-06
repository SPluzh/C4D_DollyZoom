# Completion Checklist

When completing a task or an edit in this project, ensure the following:

- [ ] **Redraw Viewport**: Call `doc->Redraw()` after making changes to camera parameters.
- [ ] **Undo Management**: Ensure `doc->StartUndo()` and `doc->EndUndo()` are called correctly around modifications.
- [ ] **Cleanup**: Reset any static or member camera pointers (like `activeCamera`) to `nullptr` when the tool interaction ends (e.g., in `MouseUp`).
- [ ] **Check Compatibility**: Verify that any changes to `PLUGIN_ID` or registration parameters are reflected in `dollywood.str` if necessary.
