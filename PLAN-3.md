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

## Constraints
- **Test Preservation:** Do not modify any files within the `tests/` directory. These are the source of truth for verification and must remain unaltered throughout the refactoring process.
- **Test Working Directory:** Tests must run from `tests/` so image assets resolve.

## Prerequisites
- **Dependencies:** The automated tests require `libjpeg`. Ensure `libjpeg-dev` is installed on the system. If not found, it must be installed using `sudo apt-get install -y libjpeg-dev`.

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

### Phase 0: Environment Setup
1.  Verify if `libjpeg-dev` is installed.
2.  If not installed, run `sudo apt-get update && sudo apt-get install -y libjpeg-dev` to ensure tests can handle JPEG images.

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

## Source-Informed Implementation Tips

### 1) Keep Fragment Boundaries Stable During Transition
- `module/image/image_class.h` currently injects `image_class_decl.h` and `image_class_body.h` inside `struct CImg`.
- `module/containers/list_base.h` opens `struct CImgList`, and `module/containers/list_io.h` closes it.
- `module/display/display_base.h` opens `struct CImgDisplay`, and `module/display/display_sdl.h` closes it.
- First transition step should be to establish a single declaration file per class that fully opens/closes the class, then include implementation headers after the class definition.

### 2) Preserve Plugin Injection Points
- Keep `cimg_plugin*`, `cimglist_plugin*`, and `cimgdisplay_plugin*` includes in class declaration scope.
- Do not move plugin includes to implementation files; plugin code expects class scope.

### 3) Signature Migration Rules (Critical)
- Keep default arguments only in declaration headers; remove them from out-of-class definitions.
- For member-function templates, use double template headers:
  - `template<typename T> template<typename t> ...`
- For methods that use class typedefs in signatures (`Tfloat`, `charT`, etc.), prefer trailing-return style in impl files to avoid dependent-type qualification churn:
  - `auto CImg<T>::linear_atXYZ(...) const -> Tfloat`
- For `CImgDisplay` (non-template), mark all out-of-class definitions in headers as `inline` to avoid ODR/multiple-definition link errors.

### 4) Handle Macro-Generated Methods as a Dedicated Track
- `module/image/image_pointwise.h` uses `_cimg_create_pointwise_functions(...)` to emit many in-class bodies.
- Add separate declaration and implementation macro variants so generated methods can be moved out-of-class without manual per-method rewriting.

### 5) Treat `_cimg_math_parser` as Its Own Migration Unit
- Parser code is currently injected via `module/math/math_parser*.h` in class scope.
- Move in two controlled steps:
  1. Forward-declare `struct _cimg_math_parser;` in the class declaration section.
  2. Define `template<typename T> struct CImg<T>::_cimg_math_parser { ... };` in implementation includes.
- Keep parser includes contiguous; parser macros (`_cimg_mp_*`) are tightly coupled.

### 6) Convert in Risk-Ordered Buckets
- `CImg<T>` recommended order:
  1. `image_ops_basic.h`
  2. `image_checks.h`, `image_object3d_ops.h`
  3. `image_pixels.h`, `image_iterators.h`, `image_value_ops.h`
  4. `image_accessors.h` (typedef-heavy signatures)
  5. Body leaves: `image_body_arithmetic.h`, `image_body_analysis.h`, `image_body_pointwise.h`, `image_body_colors.h`, `image_body_geometry_*.h`, `image_body_filters_*.h`, `image_body_object3d.h`, `image_body_draw_*.h`, `image_body_io.h`
- `CImgList<T>` recommended order:
  1. Declaration extraction from `list_base.h`
  2. `list_ops.h`
  3. `list_io.h`
- `CImgDisplay` recommended order:
  1. Common API declarations from `display_base.h`
  2. Backend impl conversion under existing guards: `display_x11.h`, `display_win32.h`, `display_sdl.h`

### 7) Verification Cadence to Control Blast Radius
- After each converted header, run targeted tests matching that header name first (e.g., `test_image_ops_basic_h_*.cpp`).
- After each phase, run the full test matrix from `tests/` (working directory requirement already captured in Constraints).
- For display changes, add compile-smoke checks with both `-Dcimg_display=0` and `-Dcimg_display=1` even if runtime display tests are environment-limited.

## Definition of Done
- `CImg<T>`, `CImgList<T>`, and `CImgDisplay` have clean, declaration-only class definitions.
- All member function implementations are moved outside the class bodies.
- The project maintains full API and functional compatibility with the previous modular state.
- All automated tests in `tests/` pass.

## Progress tracking
- Update this section as work advances so status is visible in a single place.

### Phase status checklist
- [x] Phase 0 — Environment Setup
- [ ] Phase 1 — `CImg<T>` Declaration Extraction
- [ ] Phase 2 — `CImg<T>` Implementation Transformation
- [ ] Phase 3 — `CImgList<T>` Refactor
- [ ] Phase 4 — `CImgDisplay` Refactor
- [ ] Phase 5 — Verification and Cleanup

### Change log
- _2026-02-11_: Initialize PLAN-3 and define declaration/definition separation strategy.
- _2026-02-12_: Add constraint to preserve `tests/` directory content.
- _2026-02-12_: Add prerequisite and Phase 0 for `libjpeg-dev` installation.
- _2026-02-22_: Add source-informed migration tips from direct audit of `module/image`, `module/containers`, and `module/display`.
