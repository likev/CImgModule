/*
 #
 #  File            : CImg.h
 #                    ( C++ header file )
 #
 #  Description     : C++ Template Image Processing Toolkit.
 #                    This file is the main component of the CImg Library project.
 #                    ( http://cimg.eu )
 #
 #  Project manager : David Tschumperlé
 #                    ( http://tschumperle.users.greyc.fr/ )
 #
 #                    A complete list of contributors is available in file 'README.txt'
 #                    distributed within the CImg package.
 #
 #  Licenses        : This file is 'dual-licensed', you have to choose one
 #                    of the two licenses below to apply.
 #
 #                    CeCILL-C
 #                    The CeCILL-C license is close to the GNU LGPL.
 #                    ( http://cecill.info/licences/Licence_CeCILL-C_V1-en.html )
 #
 #                or  CeCILL v2.1
 #                    The CeCILL license is compatible with the GNU GPL.
 #                    ( http://cecill.info/licences/Licence_CeCILL_V2.1-en.html )
 #
 #  This software is governed either by the CeCILL or the CeCILL-C license
 #  under French law and abiding by the rules of distribution of free software.
 #  You can  use, modify and or redistribute the software under the terms of
 #  the CeCILL or CeCILL-C licenses as circulated by CEA, CNRS and INRIA
 #  at the following URL: "http://cecill.info".
 #
 #  As a counterpart to the access to the source code and  rights to copy,
 #  modify and redistribute granted by the license, users are provided only
 #  with a limited warranty  and the software's author,  the holder of the
 #  economic rights,  and the successive licensors  have only  limited
 #  liability.
 #
 #  In this respect, the user's attention is drawn to the risks associated
 #  with loading,  using,  modifying and/or developing or reproducing the
 #  software by the user in light of its specific status of free software,
 #  that may mean  that it is complicated to manipulate,  and  that  also
 #  therefore means  that it is reserved for developers  and  experienced
 #  professionals having in-depth computer knowledge. Users are therefore
 #  encouraged to load and test the software's suitability as regards their
 #  requirements in conditions enabling the security of their systems and/or
 #  data to be ensured and,  more generally, to use and operate it in the
 #  same conditions as regards security.
 #
 #  The fact that you are presently reading this means that you have had
 #  knowledge of the CeCILL and CeCILL-C licenses and that you accept its terms.
 #
*/

// Set version number of the library.
#ifndef cimg_version
#include "module/core/version.h"

/*-----------------------------------------------------------
 #
 # Test and possibly auto-set CImg configuration variables
 # and include required headers.
 #
 # If you find that the default configuration variables are
 # not adapted to your system, you can override their values
 # before including the header file "CImg.h"
 # (use the #define directive).
 #
 ------------------------------------------------------------*/

// Include standard C++ headers.
// This is the minimal set of required headers to make CImg-based codes compile.
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <climits>
#include <ctime>
#include <exception>
#include <algorithm>
#define cimg_str(x) #x
#define cimg_str2(x) cimg_str(x)

#include "module/core/platform.h"

// Define correct string functions for each compiler and OS.
#if cimg_OS==2 && defined(_MSC_VER)
#define cimg_sscanf std::sscanf
#define cimg_snprintf cimg::_snprintf
#define cimg_vsnprintf cimg::_vsnprintf
#else
#include <stdio.h>
#if defined(__MACOSX__) || defined(__APPLE__)
#define cimg_sscanf cimg::_sscanf
#define cimg_snprintf cimg::_snprintf
#define cimg_vsnprintf cimg::_vsnprintf
#else
#define cimg_sscanf std::sscanf
#define cimg_snprintf snprintf
#define cimg_vsnprintf vsnprintf
#endif
#endif

// Include OS-specific headers.
#if cimg_OS==1
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>
#elif cimg_OS==2
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif
#include <shlobj.h>
#include <process.h>
#include <io.h>
enum {FALSE_WIN = 0};
#endif

// Look for C++11 features.
#ifndef cimg_use_cpp11
#if __cplusplus>201100
#define cimg_use_cpp11 1
#else
#define cimg_use_cpp11 0
#endif
#endif
#if cimg_use_cpp11==1
#include <initializer_list>
#include <utility>
#endif

// Convenient macro to define pragma.
#ifdef _MSC_VER
#define cimg_pragma(x) __pragma(x)
#else
#define cimg_pragma(x) _Pragma(#x)
#endif

#include "module/core/types.h"
#include "module/core/config.h"

#include "module/io/io_common.h"

#include "module/io/io_formats_image.h"

#include "module/core/utils.h"
#include "module/math/math_colors.h"

/*------------------------------------------------
 #
 #
 #  Define cimg_library:: namespace
 #
 #
 -------------------------------------------------*/
//! Contains <i>all classes and functions</i> of the \CImg library.
/**
   This namespace is defined to avoid functions and class names collisions
   that could happen with the inclusion of other C++ header files.
   Anyway, it should not happen often and you should reasonably start most of your
   \CImg-based programs with
   \code
   #include "CImg.h"
   using namespace cimg_library;
   \endcode
   to simplify the declaration of \CImg Library objects afterwards.
**/
namespace cimg_library {

  // Declare the four classes of the CImg Library.
  template<typename T=float> struct CImg;
  template<typename T=float> struct CImgList;
  struct CImgDisplay;
  struct CImgException;

  // Declare cimg:: namespace.
  // This is an incomplete namespace definition here. It only contains some stuff to ensure a correct declaration order
  // of the classes and functions defined afterwards.
  namespace cimg {


    inline std::FILE* output(std::FILE *file=0);
    inline void info();

    //! Avoid warning messages due to unused parameters. Do nothing actually.
    template<typename T>
    inline void unused(const T&, ...) {}

    // [internal] Lock/unlock a mutex for managing concurrent threads.
    // 'lock_mode' can be { 0=unlock | 1=lock | 2=trylock }.
    // 'n' can be in [0,31] but mutex range [0,15] is reserved by CImg.
    inline int mutex(const unsigned int n, const int lock_mode=1);

    inline unsigned int& exception_mode(const unsigned int value, const bool is_set) {
      static unsigned int mode = cimg_verbosity;
      if (is_set) { cimg::mutex(0); mode = value<4?value:4; cimg::mutex(0,0); }
      return mode;
    }

    // Functions to return standard streams 'stdin', 'stdout' and 'stderr'.
    inline FILE* _stdin(const bool throw_exception=true);
    inline FILE* _stdout(const bool throw_exception=true);
    inline FILE* _stderr(const bool throw_exception=true);

    // Mandatory because Microsoft's _snprintf() and _vsnprintf() do not add the '\0' character
    // at the end of the string.
#if cimg_OS==2 && defined(_MSC_VER)
    inline int _snprintf(char *const s, const size_t size, const char *const format, ...) {
      va_list ap;
      va_start(ap,format);
      const int result = _vsnprintf(s,size,format,ap);
      va_end(ap);
      return result;
    }

    inline int _vsnprintf(char *const s, const size_t size, const char *const format, va_list ap) {
      int result = -1;
      cimg::mutex(6);
      if (size) result = _vsnprintf_s(s,size,_TRUNCATE,format,ap);
      if (result==-1) result = _vscprintf(format,ap);
      cimg::mutex(6,0);
      return result;
    }

    // Mutex-protected version of sscanf, snprintf and vnsprintf.
    // Used only MacOSX, as it seems those functions are not re-entrant on MacOSX.
#elif defined(__MACOSX__) || defined(__APPLE__)
    inline int _sscanf(const char *const s, const char *const format, ...) {
      cimg::mutex(6);
      va_list args;
      va_start(args,format);
      const int result = std::vsscanf(s,format,args);
      va_end(args);
      cimg::mutex(6,0);
      return result;
    }

    inline int _snprintf(char *const s, const size_t n, const char *const format, ...) {
      cimg::mutex(6);
      va_list args;
      va_start(args,format);
      const int result = std::vsnprintf(s,n,format,args);
      va_end(args);
      cimg::mutex(6,0);
      return result;
    }

    inline int _vsnprintf(char *const s, const size_t size, const char* format, va_list ap) {
      cimg::mutex(6);
      const int result = std::vsnprintf(s,size,format,ap);
      cimg::mutex(6,0);
      return result;
    }
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
    inline unsigned int& exception_mode(const unsigned int mode) {
      return exception_mode(mode,true);
    }

    //! Return current \CImg exception mode.
    /**
       \note By default, return the value of configuration macro \c cimg_verbosity
    **/
    inline unsigned int& exception_mode() {
      return exception_mode(0,false);
    }

    inline unsigned int openmp_mode(const unsigned int value, const bool is_set) {
#if cimg_use_openmp!=0
      static unsigned int mode = 2;
      if (is_set)  { cimg::mutex(0); mode = value<2?value:2; cimg::mutex(0,0); }
      return mode;
#else
      cimg::unused(value,is_set);
      return 0;
#endif
    }

    //! Set current \CImg openmp mode.
    /**
       The way openmp-based methods are handled by \CImg can be changed dynamically, using this function.
       \param mode Desired openmp mode. Possible values are:
       - \c 0: Never parallelize.
       - \c 1: Always parallelize.
       - \c 2: Adaptive parallelization mode (default behavior).
     **/
    inline unsigned int openmp_mode(const unsigned int mode) {
      return openmp_mode(mode,true);
    }

    //! Return current \CImg openmp mode.
    inline unsigned int openmp_mode() {
      return openmp_mode(0,false);
    }

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

  } // namespace cimg { ...

  /*---------------------------------------
    #
    # Define the CImgException structures
    #
    --------------------------------------*/
  //! Instances of \c CImgException are thrown when errors are encountered in a \CImg function call.
  /**
     \par Overview

      CImgException is the base class of all exceptions thrown by \CImg (except \b CImgAbortException).
      CImgException is never thrown itself. Derived classes that specify the type of errord are thrown instead.
      These classes can be:

      - \b CImgAbortException: Thrown when a computationally-intensive function is aborted by an external signal.
        This is the only \c non-derived exception class.

      - \b CImgArgumentException: Thrown when one argument of a called \CImg function is invalid.
      This is probably one of the most thrown exception by \CImg.
      For instance, the following example throws a \c CImgArgumentException:
      \code
      CImg<float> img(100,100,1,3); // Define a 100x100 color image with float-valued pixels
      img.mirror('e'); // Try to mirror image along the (non-existing) 'e'-axis
      \endcode

      - \b CImgDisplayException: Thrown when something went wrong during the display of images in CImgDisplay instances.

      - \b CImgInstanceException: Thrown when an instance associated to a called \CImg method does not fit
      the function requirements. For instance, the following example throws a \c CImgInstanceException:
      \code
      const CImg<float> img; // Define an empty image
      const float value = img.at(0); // Try to read first pixel value (does not exist)
      \endcode

      - \b CImgIOException: Thrown when an error occurred when trying to load or save image files.
      This happens when trying to read files that do not exist or with invalid formats.
      For instance, the following example throws a \c CImgIOException:
      \code
      const CImg<float> img("missing_file.jpg"); // Try to load a file that does not exist
      \endcode

      - \b CImgWarningException: Thrown only if configuration macro \c cimg_strict_warnings is set, and
      when a \CImg function has to display a warning message (see cimg::warn()).

      It is not recommended to throw CImgException instances by yourself,
      since they are expected to be thrown only by \CImg.
      When an error occurs in a library function call, \CImg may display error messages on the screen or on the
      standard output, depending on the current \CImg exception mode.
      The \CImg exception mode can be get and set by functions cimg::exception_mode() and
      cimg::exception_mode(unsigned int).

      \par Exceptions handling

      In all cases, when an error occurs in \CImg, an instance of the corresponding exception class is thrown.
      This may lead the program to break (this is the default behavior), but you can bypass this behavior by
      handling the exceptions by yourself,
      using a usual <tt>try { ... } catch () { ... }</tt> block, as in the following example:
      \code
      #define "CImg.h"
      using namespace cimg_library;
      int main() {
        cimg::exception_mode(0); // Enable quiet exception mode
        try {
          ... // Here, do what you want to stress CImg
        } catch (CImgException& e) { // You succeeded: something went wrong!
          std::fprintf(stderr,"CImg Library Error: %s",e.what()); // Display your custom error message
          ... // Do what you want now to save the ship!
          }
        }
      \endcode
  **/
  struct CImgException : public std::exception {
#define _cimg_exception_err(etype,disp_flag) \
  std::va_list ap, ap2; \
  va_start(ap,format); va_start(ap2,format); \
  int size = cimg_vsnprintf(0,0,format,ap2); \
  if (size++>=0) { \
    delete[] _message; \
    _message = new char[(size_t)size]; \
    cimg_vsnprintf(_message,(size_t)size,format,ap); \
    if (cimg::exception_mode()) { \
      std::fprintf(cimg::output(),"\n%s[CImg] *** %s ***%s %s\n",cimg::t_red,etype,cimg::t_normal,_message); \
      if (cimg_display && disp_flag && !(cimg::exception_mode()%2)) try { cimg::dialog(etype,_message,"Abort"); } \
      catch (CImgException&) {} \
      if (cimg::exception_mode()>=3) cimg_library::cimg::info(); \
    } \
  } \
  va_end(ap); va_end(ap2);

    char *_message;
    CImgException() { _message = new char[1]; *_message = 0; }
    CImgException(const char *const format, ...):_message(0) { _cimg_exception_err("CImgException",true); }
    CImgException(const CImgException& e):std::exception(e) {
      const size_t size = std::strlen(e._message);
      _message = new char[size + 1];
      std::strncpy(_message,e._message,size);
      _message[size] = 0;
    }
    ~CImgException() throw() { delete[] _message; }
    CImgException& operator=(const CImgException& e) {
      const size_t size = std::strlen(e._message);
      _message = new char[size + 1];
      std::strncpy(_message,e._message,size);
      _message[size] = 0;
      return *this;
    }
    //! Return a C-string containing the error message associated to the thrown exception.
    const char *what() const throw() { return _message; }
  }; // struct CImgException { ...

  // The CImgAbortException class is used to throw an exception when
  // a computationally-intensive function has been aborted by an external signal.
  struct CImgAbortException : public std::exception {
    char *_message;
    CImgAbortException() { _message = new char[1]; *_message = 0; }
    CImgAbortException(const char *const format, ...):_message(0) { _cimg_exception_err("CImgAbortException",true); }
    CImgAbortException(const CImgAbortException& e):std::exception(e) {
      const size_t size = std::strlen(e._message);
      _message = new char[size + 1];
      std::strncpy(_message,e._message,size);
      _message[size] = 0;
    }
    ~CImgAbortException() throw() { delete[] _message; }
    CImgAbortException& operator=(const CImgAbortException& e) {
      const size_t size = std::strlen(e._message);
      _message = new char[size + 1];
      std::strncpy(_message,e._message,size);
      _message[size] = 0;
      return *this;
    }
    //! Return a C-string containing the error message associated to the thrown exception.
    const char *what() const throw() { return _message; }
  }; // struct CImgAbortException { ...

  // The CImgArgumentException class is used to throw an exception related
  // to invalid arguments encountered in a library function call.
  struct CImgArgumentException : public CImgException {
    CImgArgumentException(const char *const format, ...) { _cimg_exception_err("CImgArgumentException",true); }
  }; // struct CImgArgumentException { ...

  // The CImgDisplayException class is used to throw an exception related
  // to display problems encountered in a library function call.
  struct CImgDisplayException : public CImgException {
    CImgDisplayException(const char *const format, ...) { _cimg_exception_err("CImgDisplayException",false); }
  }; // struct CImgDisplayException { ...

  // The CImgInstanceException class is used to throw an exception related
  // to an invalid instance encountered in a library function call.
  struct CImgInstanceException : public CImgException {
    CImgInstanceException(const char *const format, ...) { _cimg_exception_err("CImgInstanceException",true); }
  }; // struct CImgInstanceException { ...

  // The CImgIOException class is used to throw an exception related
  // to input/output file problems encountered in a library function call.
  struct CImgIOException : public CImgException {
    CImgIOException(const char *const format, ...) { _cimg_exception_err("CImgIOException",true); }
  }; // struct CImgIOException { ...

  // The CImgWarningException class is used to throw an exception for warnings
  // encountered in a library function call.
  struct CImgWarningException : public CImgException {
    CImgWarningException(const char *const format, ...) { _cimg_exception_err("CImgWarningException",false); }
  }; // struct CImgWarningException { ...

} // namespace cimg_library

#include "module/math/math_core.h"
#include "module/math/math_algorithms.h"

namespace cimg_library {
#include "module/display/display_core.h"
  /*
   #--------------------------------------
   #
   #
   #
   # Definition of the CImg<T> structure
   #
   #
   #
   #--------------------------------------
   */

  //! Class representing an image (up to 4 dimensions wide), each pixel being of type \c T.
  /**
     This is the main class of the %CImg Library. It declares and constructs
     an image, allows access to its pixel values, and is able to perform various image operations.

     \par Image representation

     A %CImg image is defined as an instance of the container \c CImg<T>, which contains a regular grid of pixels,
     each pixel value being of type \c T. The image grid can have up to 4 dimensions: width, height, depth
     and number of channels.
     Usually, the three first dimensions are used to describe spatial coordinates <tt>(x,y,z)</tt>,
     while the number of channels is rather used as a vector-valued dimension
     (it may describe the R,G,B color channels for instance).
     If you need a fifth dimension, you can use image lists \c CImgList<T> rather than simple images \c CImg<T>.

     Thus, the \c CImg<T> class is able to represent volumetric images of vector-valued pixels,
     as well as images with less dimensions (1D scalar signal, 2D color images, ...).
     Most member functions of the class CImg<\c T> are designed to handle this maximum case of (3+1) dimensions.

     Concerning the pixel value type \c T:
     fully supported template types are the basic C++ types: <tt>unsigned char, char, short, unsigned int, int,
     unsigned long, long, float, double, ... </tt>.
     Typically, fast image display can be done using <tt>CImg<unsigned char></tt> images,
     while complex image processing algorithms may be rather coded using <tt>CImg<float></tt> or <tt>CImg<double></tt>
     images that have floating-point pixel values. The default value for the template T is \c float.
     Using your own template types may be possible. However, you will certainly have to define the complete set
     of arithmetic and logical operators for your class.

     \par Image structure

     The \c CImg<T> structure contains \e six fields:
     - \c _width defines the number of \a columns of the image (size along the X-axis).
     - \c _height defines the number of \a rows of the image (size along the Y-axis).
     - \c _depth defines the number of \a slices of the image (size along the Z-axis).
     - \c _spectrum defines the number of \a channels of the image (size along the C-axis).
     - \c _data defines a \a pointer to the \a pixel \a data (of type \c T).
     - \c _is_shared is a boolean that tells if the memory buffer \c data is shared with
       another image.

     You can access these fields publicly although it is recommended to use the dedicated functions
     width(), height(), depth(), spectrum() and ptr() to do so.
     Image dimensions are not limited to a specific range (as long as you got enough available memory).
     A value of \e 1 usually means that the corresponding dimension is \a flat.
     If one of the dimensions is \e 0, or if the data pointer is null, the image is considered as \e empty.
     Empty images should not contain any pixel data and thus, will not be processed by CImg member functions
     (a CImgInstanceException will be thrown instead).
     Pixel data are stored in memory, in a non interlaced mode (See \ref cimg_storage).

     \par Image declaration and construction

     Declaring an image can be done by using one of the several available constructors.
     Here is a list of the most used:

     - Construct images from arbitrary dimensions:
         - <tt>CImg<char> img;</tt> declares an empty image.
         - <tt>CImg<unsigned char> img(128,128);</tt> declares a 128x128 greyscale image with
         \c unsigned \c char pixel values.
         - <tt>CImg<double> img(3,3);</tt> declares a 3x3 matrix with \c double coefficients.
         - <tt>CImg<unsigned char> img(256,256,1,3);</tt> declares a 256x256x1x3 (color) image
         (colors are stored as an image with three channels).
         - <tt>CImg<double> img(128,128,128);</tt> declares a 128x128x128 volumetric and greyscale image
         (with \c double pixel values).
         - <tt>CImg<> img(128,128,128,3);</tt> declares a 128x128x128 volumetric color image
         (with \c float pixels, which is the default value of the template parameter \c T).
         - \b Note: images pixels are <b>not automatically initialized to 0</b>. You may use the function \c fill() to
         do it, or use the specific constructor taking 5 parameters like this:
         <tt>CImg<> img(128,128,128,3,0);</tt> declares a 128x128x128 volumetric color image with all pixel values to 0.

     - Construct images from filenames:
         - <tt>CImg<unsigned char> img("image.jpg");</tt> reads a JPEG color image from the file "image.jpg".
         - <tt>CImg<float> img("analyze.hdr");</tt> reads a volumetric image (ANALYZE7.5 format) from the
         file "analyze.hdr".
         - \b Note: You need to install <a href="http://www.imagemagick.org">ImageMagick</a>
         to be able to read common compressed image formats (JPG,PNG, ...) (See \ref cimg_files_io).

     - Construct images from C-style arrays:
         - <tt>CImg<int> img(data_buffer,256,256);</tt> constructs a 256x256 greyscale image from a \c int* buffer
         \c data_buffer (of size 256x256=65536).
         - <tt>CImg<unsigned char> img(data_buffer,256,256,1,3);</tt> constructs a 256x256 color image
         from a \c unsigned \c char* buffer \c data_buffer (where R,G,B channels follow each others).

         The complete list of constructors can be found <a href="#constructors">here</a>.

     \par Most useful functions

     The \c CImg<T> class contains a lot of functions that operates on images.
     Some of the most useful are:

     - operator()(): Read or write pixel values.
     - display(): displays the image in a new window.
  **/
  #include "module/image/image_class.h"

#include "module/containers/list.h"

  // Completion of previously declared functions.
  //---------------------------------------------
  namespace cimg {

    #include "module/io/io_helpers.h"

    // Implement a tic/toc mechanism to display elapsed time of algorithms.
    inline cimg_uint64 tictoc(const bool is_tic) {
      cimg::mutex(2);
      static CImg<cimg_uint64> times(64);
      static unsigned int pos = 0;
      const cimg_uint64 t1 = cimg::time();
      if (is_tic) {
        // Tic.
        times[pos++] = t1;
        if (pos>=times._width)
          throw CImgArgumentException("cimg::tic(): Too much calls to 'cimg::tic()' without calls to 'cimg::toc()'.");
        cimg::mutex(2,0);
        return t1;
      }

      // Toc.
      if (!pos)
        throw CImgArgumentException("cimg::toc(): No previous call to 'cimg::tic()' has been made.");
      const cimg_uint64
        t0 = times[--pos],
        dt = t1>=t0?(t1 - t0):cimg::type<cimg_uint64>::max();
      const unsigned int
        edays = (unsigned int)(dt/86400000.),
        ehours = (unsigned int)((dt - edays*86400000.)/3600000.),
        emin = (unsigned int)((dt - edays*86400000. - ehours*3600000.)/60000.),
        esec = (unsigned int)((dt - edays*86400000. - ehours*3600000. - emin*60000.)/1000.),
        ems = (unsigned int)(dt - edays*86400000. - ehours*3600000. - emin*60000. - esec*1000.);
      if (!edays && !ehours && !emin && !esec)
        std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u ms%s\n",
                     cimg::t_red,1 + 2*pos,"",ems,cimg::t_normal);
      else {
        if (!edays && !ehours && !emin)
          std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u sec %u ms%s\n",
                       cimg::t_red,1 + 2*pos,"",esec,ems,cimg::t_normal);
        else {
          if (!edays && !ehours)
            std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u min %u sec %u ms%s\n",
                         cimg::t_red,1 + 2*pos,"",emin,esec,ems,cimg::t_normal);
          else{
            if (!edays)
              std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u hours %u min %u sec %u ms%s\n",
                           cimg::t_red,1 + 2*pos,"",ehours,emin,esec,ems,cimg::t_normal);
            else{
              std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u days %u hours %u min %u sec %u ms%s\n",
                           cimg::t_red,1 + 2*pos,"",edays,ehours,emin,esec,ems,cimg::t_normal);
            }
          }
        }
      }
      cimg::mutex(2,0);
      return dt;
    }

    // Return a temporary string describing the size of a memory buffer.
    inline const char *strbuffersize(const cimg_ulong size) {
      static CImg<char> res(256);
      cimg::mutex(5);
      if (size<1024LU) cimg_snprintf(res,res._width,"%lu byte%s",(unsigned long)size,size>1?"s":"");
      else if (size<1024*1024LU) { const float nsize = size/1024.f; cimg_snprintf(res,res._width,"%.1f Kio",nsize); }
      else if (size<1024*1024*1024LU) {
        const float nsize = size/(1024*1024.f); cimg_snprintf(res,res._width,"%.1f Mio",nsize);
      } else { const float nsize = size/(1024*1024*1024.f); cimg_snprintf(res,res._width,"%.1f Gio",nsize); }
      cimg::mutex(5,0);
      return res;
    }

    //! Display a simple dialog box, and wait for the user's response.
    /**
       \param title Title of the dialog window.
       \param msg Main message displayed inside the dialog window.
       \param button1_label Label of the 1st button.
       \param button2_label Label of the 2nd button (\c 0 to hide button).
       \param button3_label Label of the 3rd button (\c 0 to hide button).
       \param button4_label Label of the 4th button (\c 0 to hide button).
       \param button5_label Label of the 5th button (\c 0 to hide button).
       \param button6_label Label of the 6th button (\c 0 to hide button).
       \param logo Image logo displayed at the left of the main message.
       \param is_centered Tells if the dialog window must be centered on the screen.
       \return Index of clicked button (from \c 0 to \c 5), or \c -1 if the dialog window has been closed by the user.
       \note
       - Up to 6 buttons can be defined in the dialog window.
       - The function returns when a user clicked one of the button or closed the dialog window.
       - If a button text is set to 0, the corresponding button (and the following) will not appear in the dialog box.
       At least one button must be specified.
    **/
    template<typename t>
    inline int dialog(const char *const title, const char *const msg,
                      const char *const button1_label, const char *const button2_label,
                      const char *const button3_label, const char *const button4_label,
                      const char *const button5_label, const char *const button6_label,
                      const CImg<t>& logo, const bool is_centered=false) {
#if cimg_display==0
      cimg::unused(title,msg,button1_label,button2_label,button3_label,button4_label,button5_label,button6_label,
                   logo._data,is_centered);
      throw CImgIOException("cimg::dialog(): No display available.");
#else
      static const unsigned char
        black[] = { 0,0,0 }, white[] = { 255,255,255 }, gray[] = { 200,200,200 }, gray2[] = { 150,150,150 };

      // Create buttons and canvas graphics.
      CImgList<unsigned char> buttons, cbuttons, sbuttons;
      if (button1_label) {
        CImg<unsigned char>().draw_text(0,0,button1_label,black,gray,1,13).move_to(buttons);
        if (button2_label) {
          CImg<unsigned char>().draw_text(0,0,button2_label,black,gray,1,13).move_to(buttons);
          if (button3_label) {
            CImg<unsigned char>().draw_text(0,0,button3_label,black,gray,1,13).move_to(buttons);
            if (button4_label) {
              CImg<unsigned char>().draw_text(0,0,button4_label,black,gray,1,13).move_to(buttons);
              if (button5_label) {
                CImg<unsigned char>().draw_text(0,0,button5_label,black,gray,1,13).move_to(buttons);
                if (button6_label) {
                  CImg<unsigned char>().draw_text(0,0,button6_label,black,gray,1,13).move_to(buttons);
                }}}}}}
      if (!buttons._width)
        throw CImgArgumentException("cimg::dialog(): No buttons have been defined.");
      cimglist_for(buttons,l) buttons[l].resize(-100,-100,1,3);

      unsigned int bw = 0, bh = 0;
      cimglist_for(buttons,l) { bw = std::max(bw,buttons[l]._width); bh = std::max(bh,buttons[l]._height); }
      bw+=8; bh+=8;
      if (bw<64) bw = 64;
      if (bw>128) bw = 128;
      if (bh<24) bh = 24;
      if (bh>48) bh = 48;

      CImg<unsigned char> button(bw,bh,1,3);
      button.draw_rectangle(0,0,bw - 1,bh - 1,gray);
      button.draw_line(0,0,bw - 1,0,white).draw_line(0,bh - 1,0,0,white);
      button.draw_line(bw - 1,0,bw - 1,bh - 1,black).draw_line(bw - 1,bh - 1,0,bh - 1,black);
      button.draw_line(1,bh - 2,bw - 2,bh - 2,gray2).draw_line(bw - 2,bh - 2,bw - 2,1,gray2);
      CImg<unsigned char> sbutton(bw,bh,1,3);
      sbutton.draw_rectangle(0,0,bw - 1,bh - 1,gray);
      sbutton.draw_line(0,0,bw - 1,0,black).draw_line(bw - 1,0,bw - 1,bh - 1,black);
      sbutton.draw_line(bw - 1,bh - 1,0,bh - 1,black).draw_line(0,bh - 1,0,0,black);
      sbutton.draw_line(1,1,bw - 2,1,white).draw_line(1,bh - 2,1,1,white);
      sbutton.draw_line(bw - 2,1,bw - 2,bh - 2,black).draw_line(bw - 2,bh - 2,1,bh - 2,black);
      sbutton.draw_line(2,bh - 3,bw - 3,bh - 3,gray2).draw_line(bw - 3,bh - 3,bw - 3,2,gray2);
      sbutton.draw_line(4,4,bw - 5,4,black,1,0xAAAAAAAA,true).
        draw_line(bw - 5,4,bw - 5,bh - 5,black,1,0xAAAAAAAA,false);
      sbutton.draw_line(bw - 5,bh - 5,4,bh - 5,black,1,0xAAAAAAAA,false).
        draw_line(4,bh - 5,4,4,black,1,0xAAAAAAAA,false);
      CImg<unsigned char> cbutton(bw,bh,1,3);
      cbutton.draw_rectangle(0,0,bw - 1,bh - 1,black).draw_rectangle(1,1,bw - 2,bh - 2,gray2).
        draw_rectangle(2,2,bw - 3,bh - 3,gray);
      cbutton.draw_line(4,4,bw - 5,4,black,1,0xAAAAAAAA,true).
        draw_line(bw - 5,4,bw - 5,bh - 5,black,1,0xAAAAAAAA,false);
      cbutton.draw_line(bw - 5,bh - 5,4,bh - 5,black,1,0xAAAAAAAA,false).
        draw_line(4,bh - 5,4,4,black,1,0xAAAAAAAA,false);

      cimglist_for(buttons,ll) {
        CImg<unsigned char>(cbutton).
          draw_image(1 + (bw  -buttons[ll].width())/2,1 + (bh - buttons[ll].height())/2,buttons[ll]).
          move_to(cbuttons);
        CImg<unsigned char>(sbutton).
          draw_image((bw - buttons[ll].width())/2,(bh - buttons[ll].height())/2,buttons[ll]).
          move_to(sbuttons);
        CImg<unsigned char>(button).
          draw_image((bw - buttons[ll].width())/2,(bh - buttons[ll].height())/2,buttons[ll]).
          move_to(buttons[ll]);
      }

      CImg<unsigned char> canvas;
      if (msg)
        ((CImg<unsigned char>().draw_text(0,0,"%s",gray,0,1,13,msg)*=-1)+=200).resize(-100,-100,1,3).move_to(canvas);

      const unsigned int
        bwall = (buttons._width - 1)*(12 + bw) + bw,
        w = cimg::max(196U,36 + logo._width + canvas._width,24 + bwall),
        h = cimg::max(96U,36 + canvas._height + bh,36 + logo._height + bh),
        lx = 12 + (canvas._data?0:((w - 24 - logo._width)/2)),
        ly = (h - 12 - bh - logo._height)/2,
        tx = lx + logo._width + 12,
        ty = (h - 12 - bh - canvas._height)/2,
        bx = (w - bwall)/2,
        by = h - 12 - bh;

      if (canvas._data)
        canvas = CImg<unsigned char>(w,h,1,3).
          draw_rectangle(0,0,w - 1,h - 1,gray).
          draw_line(0,0,w - 1,0,white).draw_line(0,h - 1,0,0,white).
          draw_line(w - 1,0,w - 1,h - 1,black).draw_line(w - 1,h - 1,0,h - 1,black).
          draw_image(tx,ty,canvas);
      else
        canvas = CImg<unsigned char>(w,h,1,3).
          draw_rectangle(0,0,w - 1,h - 1,gray).
          draw_line(0,0,w - 1,0,white).draw_line(0,h - 1,0,0,white).
          draw_line(w - 1,0,w - 1,h - 1,black).draw_line(w - 1,h - 1,0,h - 1,black);
      if (logo._data) canvas.draw_image(lx,ly,logo);

      unsigned int xbuttons[6] = {};
      cimglist_for(buttons,lll) {
        xbuttons[lll] = bx + (bw + 12)*lll;
        canvas.draw_image(xbuttons[lll],by,buttons[lll]);
      }

      // Open window and enter events loop.
      CImgDisplay disp(canvas,title?title:" ",0,false,is_centered?true:false);
      if (is_centered) disp.move((CImgDisplay::screen_width() - disp.width())/2,
                                 (CImgDisplay::screen_height() - disp.height())/2);
      bool stop_flag = false, refresh = false;
      int oselected = -1, oclicked = -1, selected = -1, clicked = -1;
      while (!disp.is_closed() && !stop_flag) {
        if (refresh) {
          if (clicked>=0)
            CImg<unsigned char>(canvas).draw_image(xbuttons[clicked],by,cbuttons[clicked]).display(disp);
          else {
            if (selected>=0)
              CImg<unsigned char>(canvas).draw_image(xbuttons[selected],by,sbuttons[selected]).display(disp);
            else canvas.display(disp);
          }
          refresh = false;
        }
        disp.wait(15);
        if (disp.is_resized()) disp.resize(disp,false);

        if (disp.button()&1)  {
          oclicked = clicked;
          clicked = -1;
          cimglist_for(buttons,l)
            if (disp.mouse_y()>=(int)by && disp.mouse_y()<(int)(by + bh) &&
                disp.mouse_x()>=(int)xbuttons[l] && disp.mouse_x()<(int)(xbuttons[l] + bw)) {
              clicked = selected = l;
              refresh = true;
            }
          if (clicked!=oclicked) refresh = true;
        } else if (clicked>=0) stop_flag = true;

        if (disp.key()) {
          oselected = selected;
          switch (disp.key()) {
          case cimg::keyESC : selected = -1; stop_flag = true; break;
          case cimg::keyRETURN : if (selected<0) selected = 0; stop_flag = true; break;
          case cimg::keyTAB :
          case cimg::keyARROWRIGHT :
          case cimg::keyARROWDOWN : selected = (selected + 1)%buttons.width(); break;
          case cimg::keyARROWLEFT :
          case cimg::keyARROWUP : selected = (selected + buttons.width() - 1)%buttons.width(); break;
          }
          disp.set_key();
          if (selected!=oselected) refresh = true;
        }
      }
      if (!disp) selected = -1;
      return selected;
#endif
    }

    //! Display a simple dialog box, and wait for the user's response \specialization.
    inline int dialog(const char *const title, const char *const msg,
                      const char *const button1_label, const char *const button2_label,
                      const char *const button3_label, const char *const button4_label,
                      const char *const button5_label, const char *const button6_label,
                      const bool is_centered) {
      return dialog(title,msg,button1_label,button2_label,button3_label,button4_label,button5_label,button6_label,
                    CImg<unsigned char>::_logo40x38(),is_centered);
    }

    //! Evaluate math expression.
    /**
       \param expression C-string describing the formula to evaluate.
       \param x Value of the pre-defined variable \c x.
       \param y Value of the pre-defined variable \c y.
       \param z Value of the pre-defined variable \c z.
       \param c Value of the pre-defined variable \c c.
       \return Result of the formula evaluation.
       \note Set \c expression to \c 0 to keep evaluating the last specified \c expression.
       \par Example
       \code
       const double
       res1 = cimg::eval("cos(x)^2 + sin(y)^2",2,2), // will return '1'
       res2 = cimg::eval(0,1,1); // will return '1' too
       \endcode
    **/
    inline double eval(const char *const expression, const double x, const double y, const double z, const double c) {
      static const CImg<float> empty;
      return empty.eval(expression,x,y,z,c);
    }

    template<typename t>
    inline CImg<typename cimg::superset<double,t>::type> eval(const char *const expression, const CImg<t>& xyzc) {
      static const CImg<float> empty;
      return empty.eval(expression,xyzc);
    }

  } // namespace cimg { ...
} // namespace cimg_library { ...

#include "module/compat/legacy_macros.h"

#endif

// Local Variables:
// mode: c++
// End:
