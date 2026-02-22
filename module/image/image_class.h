#ifndef CIMG_MODULE_IMAGE_CLASS_H
#define CIMG_MODULE_IMAGE_CLASS_H

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
  template<typename T>
  struct CImg {

    unsigned int _width, _height, _depth, _spectrum;
    bool _is_shared;
    T *_data;

    #include "module/image/image_class_decl.h"

    #include "module/image/image_class_body.h"
  }; // struct CImg { ...

  #include "module/image/image_iterators.h"
  #include "module/image/image_object3d_ops.h"
  #include "module/image/image_pointwise.h"
  #include "module/image/image_value_ops.h"

#endif
