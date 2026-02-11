#ifndef CIMG_MODULE_CORE_EXCEPTIONS_H
#define CIMG_MODULE_CORE_EXCEPTIONS_H

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

#endif
