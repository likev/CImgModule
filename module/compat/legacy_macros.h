#ifndef CIMG_MODULE_COMPAT_LEGACY_MACROS_H
#define CIMG_MODULE_COMPAT_LEGACY_MACROS_H

//! Short alias name.
namespace cil = cimg_library;

#ifdef _cimg_redefine_False
#define False 0
#endif
#ifdef _cimg_redefine_True
#define True 1
#endif
#ifdef _cimg_redefine_Status
#define Status int
#endif
#ifdef _cimg_redefine_Success
#define Success 0
#endif
#ifdef _cimg_redefine_min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifdef _cimg_redefine_max
#define max(a,b) (((a)>(b))?(a):(b))
#endif
#ifdef _cimg_redefine_PI
#define PI 3.141592653589793238462643383
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif
