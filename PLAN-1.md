# PLAN-1: Split `CImg.h` into logical modules under `/module`

## Objective
Refactor the monolithic `CImg.h` header into a set of focused, composable headers under `/module`, while preserving API compatibility and single-include ergonomics.

## Scope and constraints
- Perform the refactor work on a dedicated feature branch (not directly on the main integration branch), because this is a major structural change.
- Keep `CImg.h` as the public umbrella header in phase 1.
- Preserve all existing public symbols, macros, include behavior, and binary/runtime semantics.
- Avoid functional changes during extraction; this is primarily a structural refactor.
- Ensure the split can be consumed either through `CImg.h` (default) or selective module includes (advanced users).
- Use version control for managing changes and recovery (e.g., `git commit`, `git stash`) instead of creating manual `.bak` files. This is a more robust and reliable approach.
- Perform extraction in very small increments: move code blocks section-by-section, step-by-step; avoid large batch moves in a single step.
- After each small move, compile a representative test file (e.g. `/examples/tutorial.cpp`) before continuing; use a compiler option that stops on the first error (such as `-fmax-errors=1`) to keep feedback fast.
- Keep section anchors in `module/MAP.md` stable by preferring unique symbol-based identifiers (macro/function/class names) in addition to indicative line ranges; review these anchors periodically as `CImg.h` evolves.

## Proposed target layout
Create a new directory:

- `/module/core/`
  - `version.h` (version/date/time macros)
  - `platform.h` (OS/compiler detection)
  - `config.h` (feature toggles, verbosity, OpenMP, display config)
  - `types.h` (portable integer/float typedefs)
  - `utils.h` (low-level helpers, macros, assertions)
- `/module/io/`
  - `io_common.h` (generic stream/file helpers)
  - `io_formats_*.h` (format-specific readers/writers split by family)
- `/module/math/`
  - `math_core.h` (constants, numeric helpers)
  - `math_algorithms.h` (filters/transforms/statistics primitives)
- `/module/image/`
  - `image_class_decl.h` (class declaration + lightweight inline members)
  - `image_pixels.h` (pixel access, iterators, addressing)
  - `image_ops_basic.h` (copy/fill/crop/resize)
  - `image_ops_advanced.h` (morphology, convolutions, spectral ops)
- `/module/display/`
  - `display_core.h` (display abstraction)
  - `display_backend_x11.h`
  - `display_backend_win32.h`
  - `display_backend_sdl.h`
- `/module/containers/`
  - `list.h` (`CImgList` and helpers)
- `/module/compat/`
  - `legacy_macros.h` (legacy aliases and compatibility shims)
- `/module/cimg_all.h`
  - canonical include order for all modules

## Phased execution plan

### Phase 0 — Inventory and dependency mapping
1. Identify top-level sections in `CImg.h` and annotate boundaries.
2. Build a dependency map:
   - macro dependencies
   - type dependencies
   - template/class dependencies
3. Define required include order constraints from this map.

Deliverable: `module/MAP.md` with section-to-module mapping and ordering rules.

### Phase 1 — Non-functional extraction
1. Create `/module` tree and placeholder headers with include guards.
2. Move code blocks section-by-section from `CImg.h` to target module files with minimal edits (one small step at a time).
3. After each small move, compile a representative test file (for example `/examples/tutorial.cpp`) before proceeding; compile with a first-error-only setting (e.g. `-fmax-errors=1`).
4. Replace moved regions in `CImg.h` with includes into `/module/cimg_all.h` incrementally as each section is validated.
5. Keep compatibility macros in `CImg.h` if downstream code relies on include location side effects.

Deliverable: Build passes with `#include "CImg.h"` unchanged for existing users.

### Phase 2 — Stabilize include graph
1. Remove accidental circular includes.
2. Centralize shared macros/types into `core/*`.
3. Ensure module headers are self-consistent (either self-contained or explicitly documented prerequisites).

Deliverable: deterministic, documented include order in `module/cimg_all.h`.

### Phase 3 — Verification matrix
1. Compile representative examples from existing repository assets.
2. Validate major feature flags:
   - display disabled/enabled
   - OpenMP on/off
   - strict warnings modes
3. Run format I/O smoke checks for common image formats.
4. Compare generated symbols and behavior against baseline where feasible.

Deliverable: `module/VERIFY.md` with pass/fail matrix and notes.

### Phase 4 — Optional selective include support
1. Document supported partial include entry points (e.g. image-only, math-only).
2. Add guidance for advanced users on required include combinations.
3. Keep this optional and secondary to full `CImg.h` compatibility.

Deliverable: documentation section in `README.md`.

## Progress tracking
- Update this section as work advances so status is visible in a single place.

### Phase status checklist
- [x] Phase 0 — Inventory and dependency mapping
- [ ] Phase 1 — Non-functional extraction
- [ ] Phase 2 — Stabilize include graph
- [ ] Phase 3 — Verification matrix
- [ ] Phase 4 — Optional selective include support

### Change log
- _YYYY-MM-DD_: Initialize PLAN-1 and define module split strategy.
- _2026-02-10_: Completed Phase 0 inventory/dependency mapping in `module/MAP.md`.
- _2026-02-10_: Created `/module` scaffolding headers and canonical include-order umbrella `module/cimg_all.h` (no functional code moves yet).
- _2026-02-10_: Added initial verification matrix in `module/VERIFY.md` and validated baseline tutorial compile with `-fmax-errors=1`.
- _2026-02-10_: Phase 1 extraction step 1: moved `cimg_version`/`cimg_date`/`cimg_time` defaults into `module/core/version.h` and included it from `CImg.h` without changing public macro semantics.
- _YYYY-MM-DD_: Added dedicated feature-branch requirement for this major refactor.
- _YYYY-MM-DD_: Added explicit progress-tracking checklist and log section.
- _YYYY-MM-DD_: Added small-step extraction + per-step compile gate (`/examples/tutorial.cpp`, first-error-only mode).
- _2026-02-10_: Phase 1 extraction step 2: moved OS detection and MSVC warning/CRT macro setup into `module/core/platform.h` and included it from `CImg.h` with no functional changes.
- _2026-02-10_: Added inventory-anchor guidance to prefer symbol-based anchors (with periodic line-range refresh) in `module/MAP.md`/PLAN-1 to keep mapping resilient as `CImg.h` shifts.
- _2026-02-10_: Phase 1 extraction step 3: moved portable datatype/64-bit format and `cimg_max_buf_size` macro definitions into `module/core/types.h` and included it from `CImg.h` with no functional changes.

- _2026-02-10_: Phase 1 extraction step 4: moved `cimg_file_separator` and `cimg_verbosity` configuration macros into `module/core/config.h` and included it from `CImg.h` with no functional changes.
- _2026-02-10_: Phase 1 extraction step 5: moved OpenMP configuration macros (`cimg_use_openmp`, `cimg_pragma_openmp`) into `module/core/config.h` and preserved include order via existing config include.
- _2026-02-10_: Phase 1 extraction step 6: moved abort-handler/OpenMP abort helper macro definitions into `module/core/config.h` with no functional changes.
- _2026-02-10_: Phase 1 extraction step 7: moved `cimg_display` framework selection and validation macros into `module/core/config.h` with unchanged defaults.
- _2026-02-10_: Phase 1 extraction step 8: moved display-framework conditional include block (X11/SDL headers) into `module/core/config.h` preserving macro guards.
- _2026-02-10_: Phase 1 extraction step 9: moved default `cimg_appname` macro definition into `module/core/config.h` without changing semantics.
- _2026-02-10_: Phase 1 extraction step 10: moved OpenCV support include/config block (`cimg_use_opencv`) into `module/io/io_common.h` and included it from `CImg.h` without functional changes.
- _2026-02-10_: Phase 1 extraction step 11: moved LibPNG support include/config block (`cimg_use_png`) into `module/io/io_common.h` with unchanged guards.
- _2026-02-10_: Phase 1 extraction step 12: moved LibJPEG support include/config block (`cimg_use_jpeg`) into `module/io/io_common.h` with unchanged guards.
- _2026-02-10_: Phase 1 extraction step 13: moved JPEG XL support include/config block (`cimg_use_jxl`) into `module/io/io_common.h` with unchanged guards.
- _2026-02-10_: Phase 1 extraction step 14: moved LibTIFF support include/config block (`cimg_use_tiff`) into `module/io/io_common.h` preserving the `uint64/int64` hack macros.
- _2026-02-10_: Phase 1 extraction step 15: moved HEIF support include/config block (`cimg_use_heif`) into `module/io/io_common.h` with unchanged guards.
- _2026-02-10_: Phase 1 extraction step 16: moved WebP support include/config block (`cimg_use_webp`) into `module/io/io_common.h` with unchanged guards.
- _2026-02-10_: Phase 1 extraction step 17: moved LibMINC2 support include/config block (`cimg_use_minc2`) into `module/io/io_common.h` with unchanged guards.
- _2026-02-10_: Phase 1 extraction step 18: moved Zlib support include/config block (`cimg_use_zlib`) into `module/io/io_common.h` with unchanged guards.
- _2026-02-10_: Phase 1 extraction step 19: moved libcurl support include/config block (`cimg_use_curl`) into `module/io/io_common.h` with unchanged guards.
- _2026-02-10_: Phase 1 extraction step 20: moved Magick++ support include/config block (`cimg_use_magick`) into `module/io/io_formats_image.h` and included it from `CImg.h` without functional changes.
- _2026-02-10_: Phase 1 extraction step 21: moved FFTW3 support include/config block (`cimg_use_fftw3`) into `module/io/io_formats_image.h` with unchanged guards.
- _2026-02-10_: Phase 1 extraction step 22: moved OpenEXR support include/config block (`cimg_use_openexr`) into `module/io/io_formats_image.h` preserving compiler diagnostic push/pop behavior.
- _2026-02-10_: Phase 1 extraction step 23: moved TinyEXR support include/config block (`cimg_use_tinyexr`) into `module/io/io_formats_image.h` preserving `TINYEXR_IMPLEMENTATION` define behavior.
- _2026-02-10_: `/module/io` phase-1 header extraction is now complete for top-level dependency/config include blocks (`io_common.h` + `io_formats_image.h`); remaining format implementation code stays in `CImg.h` for later phased moves.
- _2026-02-10_: Phase 1 extraction step 24: moved `cimg_float16`/`cimg_is_float16` setup, macro-conflict handling (`min`/`max`/`PI`), and user-friendly `cimg_*` macro utility block into `module/core/utils.h` and included it from `CImg.h` without functional changes.
- _2026-02-10_: `/module/core` phase-1 header extraction is now complete for top-level core blocks (`version.h`, `platform.h`, `types.h`, `config.h`, `utils.h`); deeper implementation refactors remain in `CImg.h` for later phased moves.
- _2026-02-10_: Phase 1 extraction step 25: moved VT100 color escape sequences (`cimg::t_*`) into `module/math/math_core.h` and included it from `CImg.h` without functional changes.
- _2026-02-10_: Phase 1 extraction step 26: moved the main `namespace cimg` helper block (type traits, constants, low-level helpers) into `module/math/math_core.h` and removed it from `CImg.h`.
- _2026-02-10_: Phase 1 extraction step 27: moved math operator overloads and pointwise external functions into `module/math/math_algorithms.h` and included it from `CImg.h`.
- _2026-02-10_: Phase 1 extraction step 28: split VT100 color sequences into `module/math/math_colors.h` and reordered `CImg.h` to include `module/math/math_core.h`/`module/math/math_algorithms.h` after exception definitions.


## Risk register and mitigations
- **Risk:** Macro ordering regressions.
  - **Mitigation:** Lock include order in `module/cimg_all.h`; add compile checks for key macros.
- **Risk:** Template definition visibility issues.
  - **Mitigation:** Keep all template definitions in headers; avoid moving declarations without definitions.
- **Risk:** Platform-specific breakage (X11/Win32/SDL).
  - **Mitigation:** Isolate backend headers and gate with existing feature macros.
- **Risk:** Hidden side effects from monolithic include.
  - **Mitigation:** Preserve umbrella behavior first; optimize only after parity.

## Definition of done (phase-1 baseline)
- `/module` exists with the agreed structure.
- `CImg.h` remains the default stable include for downstream users.
- No user-visible API breakage in examples/build checks.
- Extraction documented with mapping and verification notes.

## Suggested implementation order inside `CImg.h`
1. Header prologue, version, compiler/OS detection.
2. Core macros, config toggles, portable types.
3. Utility layer and math primitives.
4. Class declarations (`CImg`, `CImgList`, display types).
5. Image operations by complexity (basic first, advanced later).
6. Backend-specific display and I/O tails.

This ordering minimizes dependency churn early and reduces risk during incremental extraction.
