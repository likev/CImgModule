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
- _YYYY-MM-DD_: Added dedicated feature-branch requirement for this major refactor.
- _YYYY-MM-DD_: Added explicit progress-tracking checklist and log section.
- _YYYY-MM-DD_: Added small-step extraction + per-step compile gate (`/examples/tutorial.cpp`, first-error-only mode).

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
