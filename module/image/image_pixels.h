#ifndef CIMG_MODULE_IMAGE_PIXELS_H
#define CIMG_MODULE_IMAGE_PIXELS_H

    //-------------------------------------
    //
    //! \name Instance Characteristics
    //@{
    //-------------------------------------

    //! Return the type of image pixel values as a C string.
    /**
       Return a \c char* string containing the usual type name of the image pixel values
       (i.e. a stringified version of the template parameter \c T).
       \note
       - The returned string does not contain any spaces.
       - If the pixel type \c T does not correspond to a registered type, the string <tt>"unknown"</tt> is returned.
    **/
    static const char* pixel_type() {
      return cimg::type<T>::string();
    }

    //! Return the number of image columns.
    /**
       Return the image width, i.e. the image dimension along the X-axis.
       \note
       - The width() of an empty image is equal to \c 0.
       - width() is typically equal to \c 1 when considering images as \e vectors for matrix calculations.
       - width() returns an \c int, although the image width is internally stored as an \c unsigned \c int.
         Using an \c int is safer and prevents arithmetic traps possibly encountered when doing calculations involving
         \c unsigned \c int variables.
         Access to the initial \c unsigned \c int variable is possible (though not recommended) by
         <tt>(*this)._width</tt>.
    **/
    int width() const {
      return (int)_width;
    }

    //! Return the number of image rows.
    /**
       Return the image height, i.e. the image dimension along the Y-axis.
       \note
       - The height() of an empty image is equal to \c 0.
       - height() returns an \c int, although the image height is internally stored as an \c unsigned \c int.
         Using an \c int is safer and prevents arithmetic traps possibly encountered when doing calculations involving
         \c unsigned \c int variables.
         Access to the initial \c unsigned \c int variable is possible (though not recommended) by
         <tt>(*this)._height</tt>.
    **/
    int height() const {
      return (int)_height;
    }

    //! Return the number of image slices.
    /**
       Return the image depth, i.e. the image dimension along the Z-axis.
       \note
       - The depth() of an empty image is equal to \c 0.
       - depth() is typically equal to \c 1 when considering usual 2D images. When depth()\c > \c 1, the image
         is said to be \e volumetric.
       - depth() returns an \c int, although the image depth is internally stored as an \c unsigned \c int.
         Using an \c int is safer and prevents arithmetic traps possibly encountered when doing calculations involving
         \c unsigned \c int variables.
         Access to the initial \c unsigned \c int variable is possible (though not recommended) by
         <tt>(*this)._depth</tt>.
    **/
    int depth() const {
      return (int)_depth;
    }

    //! Return the number of image channels.
    /**
       Return the number of image channels, i.e. the image dimension along the C-axis.
       \note
       - The spectrum() of an empty image is equal to \c 0.
       - spectrum() is typically equal to \c 1 when considering scalar-valued images, to \c 3
         for RGB-coded color images, and to \c 4 for RGBA-coded color images (with alpha-channel).
         The number of channels of an image instance is not limited. The meaning of the pixel values is not linked
         up to the number of channels (e.g. a 4-channel image may indifferently stands for a RGBA or CMYK color image).
       - spectrum() returns an \c int, although the image spectrum is internally stored as an \c unsigned \c int.
         Using an \c int is safer and prevents arithmetic traps possibly encountered when doing calculations involving
         \c unsigned \c int variables.
         Access to the initial \c unsigned \c int variable is possible (though not recommended) by
         <tt>(*this)._spectrum</tt>.
    **/
    int spectrum() const {
      return (int)_spectrum;
    }

    //! Return the total number of pixel values.
    /**
       Return <tt>width()*\ref height()*\ref depth()*\ref spectrum()</tt>,
       i.e. the total number of values of type \c T in the pixel buffer of the image instance.
       \note
       - The size() of an empty image is equal to \c 0.
       - The allocated memory size for a pixel buffer of a non-shared \c CImg<T> instance is equal to
         <tt>size()*sizeof(T)</tt>.
       \par Example
       \code
       const CImg<float> img(100,100,1,3); // Construct new 100x100 color image
       if (img.size()==30000) // Test succeeds
         std::printf("Pixel buffer uses %lu bytes",
                     img.size()*sizeof(float));
       \endcode
    **/
    ulongT size() const {
      return (ulongT)_width*_height*_depth*_spectrum;
    }

    //! Return a pointer to the first pixel value.
    /**
       Return a \c T*, or a \c const \c T* pointer to the first value in the pixel buffer of the image instance,
       whether the instance is \c const or not.
       \note
       - The data() of an empty image is equal to \c 0 (null pointer).
       - The allocated pixel buffer for the image instance starts from \c data()
         and goes to <tt>data()+\ref size() - 1</tt> (included).
       - To get the pointer to one particular location of the pixel buffer, use
         data(unsigned int,unsigned int,unsigned int,unsigned int) instead.
    **/
    T* data() {
      return _data;
    }

    //! Return a pointer to the first pixel value \const.
    const T* data() const {
      return _data;
    }

    //! Return a pointer to a located pixel value.
    /**
       Return a \c T*, or a \c const \c T* pointer to the value located at (\c x,\c y,\c z,\c c) in the pixel buffer
       of the image instance,
       whether the instance is \c const or not.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note
       - Writing \c img.data(x,y,z,c) is equivalent to <tt>&(img(x,y,z,c))</tt>. Thus, this method has the same
         properties as operator()(unsigned int,unsigned int,unsigned int,unsigned int).
     **/
#if cimg_verbosity>=3
    T *data(const unsigned int x, const unsigned int y=0, const unsigned int z=0, const unsigned int c=0) {
      const ulongT off = (ulongT)offset(x,y,z,c);
      if (off>=size())
        cimg::warn(_cimg_instance
                   "data(): Invalid pointer request, at coordinates (%u,%u,%u,%u) [offset=%u].",
                   cimg_instance,
                   x,y,z,c,off);
      return _data + off;
    }

    //! Return a pointer to a located pixel value \const.
    const T* data(const unsigned int x, const unsigned int y=0, const unsigned int z=0, const unsigned int c=0) const {
      return const_cast<CImg<T>*>(this)->data(x,y,z,c);
    }
#else
    T* data(const unsigned int x, const unsigned int y=0, const unsigned int z=0, const unsigned int c=0) {
      return _data + x + (ulongT)y*_width + (ulongT)z*_width*_height + (ulongT)c*_width*_height*_depth;
    }

    const T* data(const unsigned int x, const unsigned int y=0, const unsigned int z=0, const unsigned int c=0) const {
      return _data + x + (ulongT)y*_width + (ulongT)z*_width*_height + (ulongT)c*_width*_height*_depth;
    }
#endif

    //! Return the offset to a located pixel value, with respect to the beginning of the pixel buffer.
    /**
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note
       - Writing \c img.data(x,y,z,c) is equivalent to <tt>&(img(x,y,z,c)) - img.data()</tt>.
         Thus, this method has the same properties as operator()(unsigned int,unsigned int,unsigned int,unsigned int).
       \par Example
       \code
       const CImg<float> img(100,100,1,3); // Define a 100x100 RGB-color image
       const long off = img.offset(10,10,0,2); // Get the offset of the blue value of the pixel located at (10,10)
       const float val = img[off]; // Get the blue value of this pixel
       \endcode
    **/
    longT offset(const int x, const int y=0, const int z=0, const int c=0) const {
      return x + (longT)y*_width + (longT)z*_width*_height + (longT)c*_width*_height*_depth;
    }

    //! Return a CImg<T>::iterator pointing to the first pixel value.
    /**
       \note
       - Equivalent to data().
       - It has been mainly defined for compatibility with STL naming conventions.
     **/
    #include "module/image/image_iterators.h"
    #include "module/image/image_accessors.h"
    #include "module/image/image_value_ops.h"


#endif
