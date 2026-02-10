# Verification matrix (PLAN-1)

Tracking compile/runtime parity checks during and after modular extraction.

## Baseline checks

| Check | Command | Status | Notes |
|---|---|---|---|
| Representative compile (umbrella include path) | `g++ -std=c++11 -fmax-errors=1 -Dcimg_display=0 -I. -c examples/tutorial.cpp -o /tmp/tutorial.o` | ✅ Pass | Confirms baseline build still works before extraction moves. |

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
