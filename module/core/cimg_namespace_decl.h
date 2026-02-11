#ifndef CIMG_MODULE_CORE_CIMG_NAMESPACE_DECL_H
#define CIMG_MODULE_CORE_CIMG_NAMESPACE_DECL_H

    inline std::FILE* output(std::FILE *file=0);
    inline void info();

    //! Avoid warning messages due to unused parameters. Do nothing actually.
    template<typename T>
    inline void unused(const T&, ...);

    // [internal] Lock/unlock a mutex for managing concurrent threads.
    // 'lock_mode' can be { 0=unlock | 1=lock | 2=trylock }.
    // 'n' can be in [0,31] but mutex range [0,15] is reserved by CImg.
    inline int mutex(const unsigned int n, const int lock_mode=1);

    inline unsigned int& exception_mode(const unsigned int value, const bool is_set);

    // Functions to return standard streams 'stdin', 'stdout' and 'stderr'.
    inline FILE* _stdin(const bool throw_exception=true);
    inline FILE* _stdout(const bool throw_exception=true);
    inline FILE* _stderr(const bool throw_exception=true);

    // Mandatory because Microsoft's _snprintf() and _vsnprintf() do not add the '\0' character
    // at the end of the string.
#if cimg_OS==2 && defined(_MSC_VER)
    inline int _snprintf(char *const s, const size_t size, const char *const format, ...);
    inline int _vsnprintf(char *const s, const size_t size, const char *const format, va_list ap);

    // Mutex-protected version of sscanf, snprintf and vnsprintf.
    // Used only MacOSX, as it seems those functions are not re-entrant on MacOSX.
#elif defined(__MACOSX__) || defined(__APPLE__)
    inline int _sscanf(const char *const s, const char *const format, ...);
    inline int _snprintf(char *const s, const size_t n, const char *const format, ...);
    inline int _vsnprintf(char *const s, const size_t size, const char* format, va_list ap);
#endif

    //! Set current \CImg exception mode.
    /**
       The way error messages are handled by \CImg can be changed dynamically, using this function.
       \param mode Desired exception mode. Possible values are:
       - \c 0: Hide library messages (quiet mode).
       - \c 1: Print library messages on the console.
       - \c 2: Display library messages on a dialog window.
       - \c 3: Do as \c 1 + add extra debug warnings (slow down the code!).
       - \c 4: Do as \c 2 + add extra debug warnings (slow down the code!).
     **/
    inline unsigned int& exception_mode(const unsigned int mode);

    //! Return current \CImg exception mode.
    /**
       \note By default, return the value of configuration macro \c cimg_verbosity
    **/
    inline unsigned int& exception_mode();

    inline unsigned int openmp_mode(const unsigned int value, const bool is_set);

    //! Set current \CImg openmp mode.
    /**
       The way openmp-based methods are handled by \CImg can be changed dynamically, using this function.
       \param mode Desired openmp mode. Possible values are:
       - \c 0: Never parallelize.
       - \c 1: Always parallelize.
       - \c 2: Adaptive parallelization mode (default behavior).
     **/
    inline unsigned int openmp_mode(const unsigned int mode);

    //! Return current \CImg openmp mode.
    inline unsigned int openmp_mode();

    #include "module/core/cimg_namespace_base.h"

#ifndef cimg_openmp_sizefactor
#define cimg_openmp_sizefactor 1
#endif
#define cimg_openmp_if(cond) if ((cimg::openmp_mode()==1 || (cimg::openmp_mode()>1 && (cond))))
#define cimg_openmp_if_size(size,min_size) cimg_openmp_if((size)>=(cimg_openmp_sizefactor)*(min_size))
#ifdef _MSC_VER
// Disable 'collapse()' directive for MSVC (supports only OpenMP 2.0).
#define cimg_openmp_collapse(k)
#else
#define cimg_openmp_collapse(k) collapse(k)
#endif

#if cimg_OS==2
// Disable parallelization of simple loops on Windows, due to noticed performance drop.
#define cimg_openmp_for(instance,expr,min_size,T) cimg_rof((instance),ptr,T) *ptr = (T)(expr);
#else
#define cimg_openmp_for(instance,expr,min_size,T) \
    cimg_pragma_openmp(parallel for cimg_openmp_if_size((instance).size(),min_size)) \
    cimg_rof((instance),ptr,T) *ptr = (T)(expr);
#endif

    // Display a simple dialog box, and wait for the user's response.
    inline int dialog(const char *const title, const char *const msg,
                      const char *const button1_label="OK", const char *const button2_label=0,
                      const char *const button3_label=0, const char *const button4_label=0,
                      const char *const button5_label=0, const char *const button6_label=0,
                      const bool centering=false);

    // Evaluate math expression.
    inline double eval(const char *const expression,
                       const double x=0, const double y=0, const double z=0, const double c=0);

    // Create a directory.
    inline void create_directory(const char *const dirname, const bool force_overwrite=true);

#endif
