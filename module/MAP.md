# CImg module split map (Phase 0)

This document maps major `CImg.h` regions into the target `/module` layout and captures include-order constraints for phased extraction.

## Source inventory (high-level anchors)

> Note: line numbers below are indicative snapshots of the current `CImg.h` state, not stable identifiers. Re-validate/update them periodically as extraction progresses.

- Version defaults anchor (`#ifndef cimg_version`) starts near line 56.
- Core build/environment config anchor (comment `Test and possibly auto-set CImg configuration variables`) starts near line 71 and runs through feature toggles (OpenMP/display/libs).
- Public API namespace anchor (`namespace cimg_library {`) opens near line 2315.
- Core helper namespace anchor (`namespace cimg {`) appears in two main blocks: ~2326-2514 and ~2692-8302.
- Display abstraction anchor (`struct CImgDisplay`) extracted to `module/display/display_core.h` (previously ~8472-12621).
- Image class anchor (`template<typename T> class CImg`) spans ~12727-63519.
- Container class anchor (`template<typename T> struct CImgList`) extracted to `module/containers/list.h` (previously ~63534-68691).
- Image I/O methods anchor (`CImg<T>::load*`/`save*`) extracted to `module/io/image_load.h` + `module/io/image_save.h`
  (previously ~31064-38535).
- Final helper tail anchor (`namespace cimg {` block after `CImgList`) now follows `#include "module/containers/list.h"` and closes near 68695-70137.
- I/O helper block (`stdin/stdout/stderr`, `std_fopen`, path utilities, network load) extracted to `module/io/io_helpers.h`
  (previously within the post-`CImgList` `namespace cimg` tail).

## Section-to-module mapping

| Current `CImg.h` area | Target module | Notes |
|---|---|---|
| `cimg_version`, `cimg_date`, `cimg_time` | `module/core/version.h` | Keep macro names and defaults identical. |
| OS/compiler detection and warning pragmas | `module/core/platform.h` | Preserve Windows/Unix detection and pragma behavior. |
| Feature toggles (`cimg_display`, `cimg_use_*`, OpenMP) | `module/core/config.h` | Must remain available before dependent includes. |
| Portable scalar/integer type aliases/macros | `module/core/types.h` | Must remain ABI-neutral and pre-class. |
| Common macros/assertions/helpers | `module/core/utils.h` | Shared substrate for all downstream modules. |
| Core exception hierarchy (`CImgException`, `CImg*Exception`) | `module/core/exceptions.h` | Extracted as a contiguous namespace-level exception block. |
| VT100 color escape sequences (`cimg::t_*`) | `module/math/math_colors.h` | Keep available before exception formatting. |
| `namespace cimg` helpers (math-ish/core utilities) | `module/math/math_core.h`, `module/math/math_algorithms.h` | Split low-level constants/utilities from algorithms. |
| `CImg<T>::_cimg_math_parser` expression compiler | `module/math/math_parser.h` | Included inside the `CImg<T>` definition to preserve class scope. |
| `CImgDisplay` + backend-specific branches | `module/display/*` | Keep backend macro guards exactly equivalent. |
| `CImg<T>` declaration + implementation body | `module/image/*` | Perform in smallest possible extraction chunks. |
| `CImg<T>` iterator/value/type alias block (`iterator`, `const_iterator`, `value_type`, `T*` aliases) | `module/image/image_class_decl.h` | Extracted first as a self-contained class-decl chunk. |
| `CImg<T>` constructors/destructor/instance management (constructors, assign/clear, move/swap, empty) | `module/image/image_ops_basic.h` | Constructor/destructor block extracted wholesale. |
| `CImg<T>` instance characteristics (dimensions/data/offset/iterators, linear interpolation setters, `value_string`) | `module/image/image_pixels.h` | Extracted as pixel-access and basic instance metadata. |
| `CImg<T>` instance checking (`is_*` dimension checks, bounds/overlap checks, `is_object3d`) | `module/image/image_ops_advanced.h` | Extracted as a contiguous validation/checking block. |
| `CImg<T>` pointwise math transforms (`sqr`/`sqrt`/`exp`/`log*`/`trig`/`atan2`/`acosh`/`asinh`/`atanh`) | `module/image/image_ops_advanced.h` | Extracted as a contiguous unary math-operation block (macro-generated methods + `atan2`). |
| Remaining `CImg<T>` in-class body (operators, algorithms, display helpers, serialization/logo, and in-class module includes) | `module/image/image_class_body.h` | Extracted as the class-body umbrella so `CImg.h` keeps only fields plus image-class includes. |
| `CImgList<T>` + list helpers | `module/containers/list.h` | Depends on `CImg<T>` and core utilities. |
| `CImg<T>::load*`/`save*` methods | `module/io/image_load.h`, `module/io/image_save.h` | Included inside the `CImg<T>` class body. |
| Format loaders/savers and stream helpers | `module/io/io_common.h`, `module/io/io_formats_*.h` | Maintain conditional-compile gates. |
| File/path/network helper functions (`namespace cimg`) | `module/io/io_helpers.h` | Included after `module/containers/list.h` inside `namespace cimg`. |
| Legacy aliases/shims (`cil`, `_cimg_redefine_*` macros, warning pop) | `module/compat/legacy_macros.h` | Keep opt-in compatibility semantics. |

## Required include-order constraints

1. `core/version.h` before modules that reference version/date/time macros.
2. `core/platform.h` before any OS-specific include and display backends.
3. `core/config.h` before optional dependency includes (`PNG/JPEG/TIFF/Zlib/OpenEXR/FFTW/OpenMP`).
4. `core/types.h` before templates and helper math utilities that rely on portable width assumptions.
5. `core/utils.h` before class/template implementations using helper macros.
6. `math/*` before algorithm-heavy portions of `image/*` that call `cimg::` primitives.
7. `image/image_class_decl.h` before `containers/list.h`.
8. `display/display_core.h` before backend headers.
9. `io/io_common.h` before `io_formats_*.h`.
10. `compat/legacy_macros.h` last so aliases see all final declarations.

## Extraction rules used for Phase 1

- Move code in very small contiguous blocks with zero intentional behavior changes.
- After each move, compile-check `examples/tutorial.cpp` with early-exit diagnostics (`-fmax-errors=1`).
- Keep `CImg.h` as umbrella include in phase 1; module headers are scaffolding until content migration is complete.
