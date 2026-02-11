# PLAN-3: Separate Class Method Declarations and Definitions

## Objective
Refactor the three main classes of the CImg Library—`CImg<T>`, `CImgList<T>`, and `CImgDisplay`—to separate their member function declarations from their definitions. This follows the architectural pattern established in the `proof_of_concept`, moving method bodies out of the class definitions while maintaining the library's header-only (or header-mostly) nature.

## Strategy
For each class:
1.  **Declaration Header (`*_decl.h`):** Contains the class definition (`struct` or `class`) with only member function declarations, minimal inline getters, and member variables.
2.  **Implementation Headers (`*_impl.h` or existing `body/ops` files):** Contains the actual method bodies defined outside the class scope using the `template<typename T> ReturnType ClassName<T>::MethodName(...) { ... }` syntax.
3.  **Inclusion Pattern:**
    - The class definition includes the declaration fragments (if still split).
    - The implementation files are included *after* the class definition is complete.
    - This ensures all template definitions are visible to the compiler when the class header is included.

## Targeted Classes

### 1. `CImg<T>`
- **Current State:** `module/image/image_class.h` defines `struct CImg` and includes multiple `image_body_*.h` and `image_ops_*.h` files *inside* the struct body, leading to implicit inlining of thousands of methods.
- **Refactor:**
    - Move all method bodies from `module/image/image_body_*.h` and `module/image/image_ops_*.h` to out-of-class definitions.
    - Consolidate declarations into `module/image/image_class_decl.h` or include them into the struct body as signatures only.
    - Include implementation files at the end of `module/image/image_class.h` (after the `struct CImg` closing brace).

### 2. `CImgList<T>`
- **Current State:** `module/containers/list_base.h` defines `struct CImgList` with most methods defined in-place.
- **Refactor:**
    - Extract declarations to `module/containers/list_decl.h`.
    - Move implementations to `module/containers/list_impl.h` (and other logical splits).
    - Include implementations after the `struct CImgList` definition.

### 3. `CImgDisplay`
- **Current State:** `module/display/display_base.h` and backend-specific files (`display_x11.h`, etc.) contain in-class method definitions.
- **Refactor:**
    - Extract declarations to `module/display/display_decl.h`.
    - Move implementations to `module/display/display_impl.h`.

## Phased Execution Plan

### Phase 1: `CImg<T>` Declaration Extraction
1.  Identify all methods currently included in `CImg<T>`.
2.  Create a comprehensive declaration block in `module/image/image_class_decl.h`.
3.  Replace in-class inclusions of body files with this declaration block in small, incremental steps.
4.  **Verification:** After each small step (e.g., after moving declarations for a single category), run all automated tests in `tests/`. **Make compilation fast** by using parallel execution (e.g., `make -j`) and setting `-fmax-errors=1` to stop on the first error per file.

### Phase 2: `CImg<T>` Implementation Transformation
1.  Iteratively update each `module/image/image_body_*.h` and `module/image/image_ops_*.h` file:
    - Wrap code in `namespace cimg_library { ... }`.
    - Convert each method to `template<typename T> ... CImg<T>::method(...) { ... }`.
2.  Move these inclusions from inside the class to after the class definition in `module/image/image_class.h` in small, incremental steps.
3.  **Verification:** After each small step (e.g., after transforming a single implementation file), run all automated tests in `tests/`. **Make compilation fast** by using parallel execution and `-fmax-errors=1`.

### Phase 3: `CImgList<T>` Refactor
1.  Perform declaration extraction for `CImgList` in small increments.
2.  Transform `list_base.h`, `list_ops.h`, and `list_io.h` into out-of-class implementation files, one at a time.
3.  **Verification:** Run all automated tests in `tests/` after each incremental change. **Make compilation fast** using parallel execution and `-fmax-errors=1`.

### Phase 4: `CImgDisplay` Refactor
1.  Perform declaration extraction for `CImgDisplay` in small increments.
2.  Move implementations to out-of-class syntax, handling the `cimg_display` macro guards for different backends.
3.  **Verification:** Run all automated tests in `tests/` after each incremental change. **Make compilation fast** using parallel execution and `-fmax-errors=1`.

### Phase 5: Verification and Cleanup
1.  Run the full verification matrix (compilation parity, symbol parity).
2.  Ensure no regressions in example programs.
3.  Final run of all automated tests in `tests/` to confirm total system integrity.

## Definition of Done
- `CImg<T>`, `CImgList<T>`, and `CImgDisplay` have clean, declaration-only class definitions.
- All member function implementations are moved outside the class bodies.
- The project maintains full API and functional compatibility with the previous modular state.
- All automated tests in `tests/` pass.

## Progress tracking
- Update this section as work advances so status is visible in a single place.

### Phase status checklist
- [ ] Phase 1 — `CImg<T>` Declaration Extraction
- [ ] Phase 2 — `CImg<T>` Implementation Transformation
- [ ] Phase 3 — `CImgList<T>` Refactor
- [ ] Phase 4 — `CImgDisplay` Refactor
- [ ] Phase 5 — Verification and Cleanup

### Change log
- _2026-02-11_: Initialize PLAN-3 and define declaration/definition separation strategy.
