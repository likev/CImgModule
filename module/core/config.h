#ifndef CIMG_MODULE_CORE_CONFIG_H
#define CIMG_MODULE_CORE_CONFIG_H

// Configure filename separator.
//
// Filename separator is set by default to '/', except for Windows where it is '\'.
#ifndef cimg_file_separator
#if cimg_OS==2
#define cimg_file_separator '\\'
#else
#define cimg_file_separator '/'
#endif
#endif

// Configure verbosity of output messages.
//
// Define 'cimg_verbosity' to: '0' to hide library messages (quiet mode).
//                             '1' to output library messages on the console.
//                             '2' to output library messages on a basic dialog window (default behavior).
//                             '3' to do as '1' + add extra warnings (may slow down the code!).
//                             '4' to do as '2' + add extra warnings (may slow down the code!).
//
// Define 'cimg_strict_warnings' to replace warning messages by exception throwns.
//
// Define 'cimg_use_vt100' to allow output of color messages on VT100-compatible terminals.
#ifndef cimg_verbosity
#if cimg_OS==2
#define cimg_verbosity 2
#else
#define cimg_verbosity 1
#endif
#elif !(cimg_verbosity==0 || cimg_verbosity==1 || cimg_verbosity==2 || cimg_verbosity==3 || cimg_verbosity==4)
#error CImg Library: Configuration variable 'cimg_verbosity' is badly defined.
#error (should be { 0=quiet | 1=console | 2=dialog | 3=console+warnings | 4=dialog+warnings }).
#endif

// Configure OpenMP support.
// (http://www.openmp.org)
//
// Define 'cimg_use_openmp' to enable OpenMP support (requires OpenMP 3.0+).
//
// OpenMP directives are used in many CImg functions to get
// advantages of multi-core CPUs.
#if !defined(cimg_use_openmp)
#ifdef _OPENMP
#define cimg_use_openmp 1
#else
#define cimg_use_openmp 0
#endif
#else
#undef cimg_use_openmp
#define cimg_use_openmp 1
#endif
#if cimg_use_openmp!=0
#include <omp.h>
#define cimg_pragma_openmp(p) cimg_pragma(omp p)
#else
#define cimg_pragma_openmp(p)
#endif

// Configure the 'abort' signal handler (does nothing by default).
// A typical signal handler can be defined in your own source like this:
// #define cimg_abort_test if (is_abort) throw CImgAbortException("")
//
// where 'is_abort' is a boolean variable defined somewhere in your code and reachable in the method.
// 'cimg_abort_test2' does the same but is called more often (in inner loops).
#if defined(cimg_abort_test) && cimg_use_openmp!=0

// Define abort macros to be used with OpenMP.
#ifndef _cimg_abort_init_openmp
#define _cimg_abort_init_openmp unsigned int _cimg_abort_go_openmp = 1; cimg::unused(_cimg_abort_go_openmp)
#endif
#ifndef _cimg_abort_try_openmp
#define _cimg_abort_try_openmp if (_cimg_abort_go_openmp) try
#endif
#ifndef _cimg_abort_catch_openmp
#define _cimg_abort_catch_openmp catch (CImgAbortException&) { \
  cimg_pragma_openmp(atomic) _cimg_abort_go_openmp&=0; \
}
#endif
#ifndef _cimg_abort_catch_fill_openmp
#define _cimg_abort_catch_fill_openmp \
  catch (CImgException& e) { cimg_pragma(omp critical(abort)) CImg<charT>::string(e._message).move_to(is_error_expr); \
                             cimg_pragma_openmp(atomic) _cimg_abort_go_openmp&=0; }
#endif
#ifdef cimg_abort_test2
#ifndef _cimg_abort_try_openmp2
#define _cimg_abort_try_openmp2 _cimg_abort_try_openmp
#endif
#ifndef _cimg_abort_catch_openmp2
#define _cimg_abort_catch_openmp2 _cimg_abort_catch_openmp
#endif
#endif
#endif

#ifndef _cimg_abort_init_openmp
#define _cimg_abort_init_openmp
#endif
#ifndef _cimg_abort_try_openmp
#define _cimg_abort_try_openmp
#endif
#ifndef _cimg_abort_catch_openmp
#define _cimg_abort_catch_openmp
#endif
#ifndef _cimg_abort_try_openmp2
#define _cimg_abort_try_openmp2
#endif
#ifndef _cimg_abort_catch_openmp2
#define _cimg_abort_catch_openmp2
#endif
#ifndef _cimg_abort_catch_fill_openmp
#define _cimg_abort_catch_fill_openmp
#endif
#ifndef cimg_abort_init
#define cimg_abort_init
#endif
#ifndef cimg_abort_test
#define cimg_abort_test
#endif
#ifndef cimg_abort_test2
#define cimg_abort_test2
#endif

// Configure display framework.
//
// Define 'cimg_display' to: '0' to disable display capabilities.
//                           '1' to use the X-Window framework (X11).
//                           '2' to use the Microsoft GDI32 framework.
//                           '3' to use SDL3 framework.
#ifndef cimg_display
#if cimg_OS==0
#define cimg_display 0
#elif cimg_OS==1
#define cimg_display 1
#elif cimg_OS==2
#define cimg_display 2
#endif
#elif !(cimg_display==0 || cimg_display==1 || cimg_display==2 || cimg_display==3)
#error CImg Library: Configuration variable 'cimg_display' is badly defined.
#error (should be { 0=none | 1=X-Window (X11) | 2=Microsoft GDI32 | 3=SDL3 }).
#endif

// Include display-specific headers.
#if cimg_display==1
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <pthread.h>
#ifdef cimg_use_xshm
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif
#ifdef cimg_use_xrandr
#include <X11/extensions/Xrandr.h>
#endif
#elif cimg_display==3
#include <SDL3/SDL.h>
#if cimg_OS==1
#include <csignal>
#endif
#endif

#ifndef cimg_appname
#define cimg_appname "CImg"
#endif

#endif
