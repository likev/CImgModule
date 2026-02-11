# Verification matrix (PLAN-1)

Tracking compile/runtime parity checks during and after modular extraction.

## Baseline checks

| Check | Command | Status | Notes |
|---|---|---|---|
| Representative compile (umbrella include path) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms baseline build still works after first extraction move. |
| Macro override compatibility (\`cimg_date\`) | `g++ -std=c++11 -fmax-errors=1 -I. /tmp/test_cimg_date.cpp -o /tmp/test_cimg_date` | ✅ Pass | Confirms macro override remains honored after extracting version/date/time defaults. |
| Representative compile after platform extraction | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms `module/core/platform.h` extraction keeps umbrella include behavior unchanged. |
| Representative compile after types extraction | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms `module/core/types.h` extraction preserves baseline umbrella include compile. |
| Representative compile after config extraction | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms `module/core/config.h` extraction keeps umbrella include compile behavior unchanged. |
| Representative compile after config extraction steps 5-9 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving OpenMP/abort/display/appname config blocks into `module/core/config.h` preserves umbrella include compile behavior. |
| Representative compile after io extraction steps 10-19 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving OpenCV/PNG/JPEG/JXL/TIFF/HEIF/WebP/MINC2/Zlib/libcurl config-include blocks into `module/io/io_common.h` preserves umbrella include compile behavior. |
| Representative compile after io extraction steps 20-23 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving Magick++/FFTW3/OpenEXR/TinyEXR config-include blocks into `module/io/io_formats_image.h` preserves umbrella include compile behavior. |
| Representative compile after core utils extraction step 24 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving top-level utility macro block into `module/core/utils.h` preserves umbrella include compile behavior. |
| Representative compile after math core extraction step 25 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving VT100 color escape sequences into `module/math/math_core.h` preserves umbrella include compile behavior. |
| Representative compile after math extraction steps 26-28 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving `namespace cimg` helpers and math operator overloads into `module/math` (with `math_colors.h` split + include reorder) preserves umbrella include compile behavior. |
| Representative compile after math parser extraction step 29 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving `CImg<T>::_cimg_math_parser` into `module/math/math_parser.h` preserves umbrella include compile behavior. |
| Representative compile after containers extraction step 30 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving `CImgList<T>` into `module/containers/list.h` preserves umbrella include compile behavior. |
| Representative compile after display extraction step 31 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving `CImgDisplay` into `module/display/display_core.h` preserves umbrella include compile behavior. |
| Representative compile after compat extraction step 32 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving legacy aliases/macros into `module/compat/legacy_macros.h` preserves umbrella include compile behavior. |
| Representative compile after io extraction steps 33-35 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving `CImg<T>` load/save methods and `namespace cimg` I/O helpers into `module/io/*` preserves umbrella include compile behavior. |
| Representative compile after image extraction step 36 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving `CImg<T>` iterator/value/type alias block into `module/image/image_class_decl.h` preserves umbrella include compile behavior. |
| Representative compile after image extraction step 37 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving constructors/destructor/instance management block into `module/image/image_ops_basic.h` preserves umbrella include compile behavior. |
| Representative compile after image extraction step 38 | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms moving instance characteristics block into `module/image/image_pixels.h` preserves umbrella include compile behavior. |
| Representative compile after extraction steps 39-44 | `g++ -std=c++11 -fmax-errors=1 -I. -c examples/tutorial.cpp -o /tmp/p1_tutorial_default.o` | ✅ Pass | Confirms image advanced extraction, exception extraction, and remaining `namespace cimg` split keep umbrella include compile behavior. |
| Display explicit enabled compile | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=1 -I. -c examples/tutorial.cpp -o /tmp/p1_tutorial_display1.o` | ✅ Pass | Confirms display-enabled compile path remains valid in this environment. |
| OpenMP explicit off compile | `g++ -std=c++11 -fmax-errors=1 -Dcimg_use_openmp=0 -I. -c examples/tutorial.cpp -o /tmp/p1_tutorial_openmp_off.o` | ✅ Pass | Confirms explicit non-OpenMP config remains valid. |
| OpenMP explicit on compile | `g++ -std=c++11 -fopenmp -fmax-errors=1 -Dcimg_use_openmp=1 -I. -c examples/tutorial.cpp -o /tmp/p1_tutorial_openmp_on.o` | ✅ Pass | Confirms OpenMP compile path remains valid. |
| Strict warnings compile | `g++ -std=c++11 -Wall -Wextra -Werror -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/p1_tutorial_werror.o` | ✅ Pass | Confirms strict warning mode for representative umbrella compile. |
| Additional example compile (`image2ascii`) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/image2ascii.cpp -o /tmp/p1_image2ascii.o` | ✅ Pass | Additional non-tutorial compile sanity check. |
| Additional example compile (`use_draw_gradient`) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/use_draw_gradient.cpp -o /tmp/p1_use_draw_gradient.o` | ✅ Pass | Additional module-coverage sanity check. |
| Additional example compile (`use_RGBclass`) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/use_RGBclass.cpp -o /tmp/p3_use_RGBclass.o` | ✅ Pass | Phase 3 representative coverage expansion. |
| Additional example compile (`use_nlmeans`) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/use_nlmeans.cpp -o /tmp/p3_use_nlmeans.o` | ✅ Pass | Phase 3 representative coverage expansion. |
| Additional example compile (`captcha`) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/captcha.cpp -o /tmp/p3_captcha.o` | ✅ Pass | Phase 3 representative coverage expansion. |
| Additional example compile (`plotter1d`) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/plotter1d.cpp -o /tmp/p3_plotter1d.o` | ✅ Pass | Phase 3 representative coverage expansion. |
| Module umbrella compile (`module/cimg_all.h`) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c /tmp/test_cimg_all.cpp -o /tmp/test_cimg_all.o` | ✅ Pass | Confirms deterministic module umbrella include is valid after Phase 2 stabilization. |
| Symbol parity (`CImg.h` vs `module/cimg_all.h`) | `nm -C /tmp/p3_sym_cimg.o | sort` vs `nm -C /tmp/p3_sym_mod.o | sort` | ✅ Pass | Identical symbol listing for equivalent translation units (no diff). |
| Phase 4 selective profile compile (`CImg.h`, reduced dependencies) | `g++ -std=c++11 -fmax-errors=1 -I. -c /tmp/p4_profile_core.cpp -o /tmp/p4_profile_core.o` | ✅ Pass | Uses `cimg_display=0`, `cimg_use_openmp=0`; optional I/O macros left undefined (`#ifdef` semantics). |
| Phase 4 selective profile compile (`module/cimg_all.h` umbrella) | `g++ -std=c++11 -fmax-errors=1 -I. -c /tmp/p4_profile_mod_umbrella.cpp -o /tmp/p4_profile_mod_umbrella.o` | ✅ Pass | Confirms documented module umbrella entry point for selective-profile usage. |
| Phase 4 selective profile compile (`CImg.h` with OpenMP) | `g++ -std=c++11 -fopenmp -fmax-errors=1 -I. -c /tmp/p4_profile_openmp.cpp -o /tmp/p4_profile_openmp.o` | ✅ Pass | Confirms feature-enabled profile guidance (`cimg_use_openmp=1`) compiles. |
| JPEG I/O example compile (`use_jpeg_buffer`) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -Dcimg_use_jpeg=1 -I. -c examples/use_jpeg_buffer.cpp -o /tmp/p1_use_jpeg_buffer.o` | ⚠️ Blocked (env) | Fails due to missing `jpeglib.h` in current environment, not due to header split. |
| TIFF I/O example compile (`use_tiff_stream`) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/use_tiff_stream.cpp -o /tmp/p1_use_tiff_stream.o` | ⚠️ Blocked (env) | Fails due to missing `tiffio.h` in current environment, not due to header split. |


## Feature flag matrix (to fill during Phases 1-3)

| Scenario | Status | Notes |
|---|---|---|
| Display disabled (`cimg_display=0`) | ✅ Baseline pass | |
| Display enabled (X11/Win32/SDL as available) | ✅ Pass | Explicit `cimg_display=1` compile succeeds for representative source. |
| OpenMP off | ✅ Pass | Explicit `cimg_use_openmp=0` compile succeeds. |
| OpenMP on | ✅ Pass | Explicit `cimg_use_openmp=1` with `-fopenmp` compile succeeds. |
| Strict warnings mode | ✅ Pass | `-Wall -Wextra -Werror` representative compile succeeds. |
| PNG/JPEG/TIFF I/O smoke | ⚠️ Partial (env-limited) | JPEG/TIFF example compiles are blocked by missing dev headers (`jpeglib.h`, `tiffio.h`). |

## Notes

- This file is intentionally incremental and should be updated after every extraction milestone.

# Verification matrix (PLAN-2)

Tracking compile/runtime parity checks during and after splitting oversized module files.

## Baseline checks (PLAN-2)

| Check | Command | Status | Notes |
|---|---|---|---|
| Representative compile (umbrella include path) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | PLAN-2 baseline before splitting. |
| Symbol parity (PLAN-2 baseline) | `nm -C examples/tutorial.o | sort` | ✅ Pass | Recorded for parity checks after splits. |
| Line count threshold check | `./resources/check_line_counts.sh` | ⚠️ Fail | Current baseline has files over 3000 lines. |

## Phase status matrix

| Phase | Milestone | Status | Notes |
|---|---|---|---|
| Phase 0 | Baseline and guardrails | ✅ Done | Baseline recorded and line-count script added. |
| Phase 1 | Image module split | ✅ Done | `image_class_body.h` and others split. All `<3000` lines. |
| Phase 2 | Math split | ✅ Done | `math_parser.h` and `math_core.h` split. All `<3000` lines. |
| Phase 3 | IO, display, containers split | ✅ Done | `image_load.h`, `image_save.h`, `display_core.h`, `list.h` split. |
| Phase 4 | Threshold cleanup | ✅ Done | All `module/**` files are within 3000 lines. |
