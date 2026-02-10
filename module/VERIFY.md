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


## Feature flag matrix (to fill during Phases 1-3)

| Scenario | Status | Notes |
|---|---|---|
| Display disabled (`cimg_display=0`) | ✅ Baseline pass | |
| Display enabled (X11/Win32/SDL as available) | ⏳ Pending | |
| OpenMP off | ⏳ Pending | |
| OpenMP on | ⏳ Pending | |
| Strict warnings mode | ⏳ Pending | |
| PNG/JPEG/TIFF I/O smoke | ⏳ Pending | |

## Notes

- This file is intentionally incremental and should be updated after every extraction milestone.
