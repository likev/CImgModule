#ifndef CIMG_MODULE_IMAGE_CHECKS_H
#define CIMG_MODULE_IMAGE_CHECKS_H

    //-------------------------------------
    //
    //! \name Instance Checking
    //@{
    //-------------------------------------

    //! Test shared state of the pixel buffer.
    /**
       Return \c true if image instance has a shared memory buffer, and \c false otherwise.
       \note
       - A shared image do not own his pixel buffer data() and will not deallocate it on destruction.
       - Most of the time, a \c CImg<T> image instance will \e not be shared.
       - A shared image can only be obtained by a limited set of constructors and methods (see list below).
    **/
    bool is_shared() const {
      return _is_shared;
    }

    //! Test if image instance is empty.
    /**
       Return \c true, if image instance is empty, i.e. does \e not contain any pixel values, has dimensions
       \c 0 x \c 0 x \c 0 x \c 0 and a pixel buffer pointer set to \c 0 (null pointer), and \c false otherwise.
    **/
    bool is_empty() const {
      return !(_data && _width && _height && _depth && _spectrum);
    }

    //! Test if image instance contains a 'inf' value.
    /**
       Return \c true, if image instance contains a 'inf' value, and \c false otherwise.
    **/
    bool is_inf() const {
      if (cimg::type<T>::is_float()) cimg_for(*this,p,T) if (cimg::type<T>::is_inf((float)*p)) return true;
      return false;
    }

    //! Test if image instance contains a NaN value.
    /**
       Return \c true, if image instance contains a NaN value, and \c false otherwise.
    **/
    bool is_nan() const {
      if (cimg::type<T>::is_float()) cimg_for(*this,p,T) if (cimg::type<T>::is_nan((float)*p)) return true;
      return false;
    }

    //! Test if image width is equal to specified value.
    bool is_sameX(const unsigned int size_x) const {
      return _width==size_x;
    }

    //! Test if image width is equal to specified value.
    template<typename t>
    bool is_sameX(const CImg<t>& img) const {
      return is_sameX(img._width);
    }

    //! Test if image width is equal to specified value.
    bool is_sameX(const CImgDisplay& disp) const {
      return is_sameX(disp._width);
    }

    //! Test if image height is equal to specified value.
    bool is_sameY(const unsigned int size_y) const {
      return _height==size_y;
    }

    //! Test if image height is equal to specified value.
    template<typename t>
    bool is_sameY(const CImg<t>& img) const {
      return is_sameY(img._height);
    }

    //! Test if image height is equal to specified value.
    bool is_sameY(const CImgDisplay& disp) const {
      return is_sameY(disp._height);
    }

    //! Test if image depth is equal to specified value.
    bool is_sameZ(const unsigned int size_z) const {
      return _depth==size_z;
    }

    //! Test if image depth is equal to specified value.
    template<typename t>
    bool is_sameZ(const CImg<t>& img) const {
      return is_sameZ(img._depth);
    }

    //! Test if image spectrum is equal to specified value.
    bool is_sameC(const unsigned int size_c) const {
      return _spectrum==size_c;
    }

    //! Test if image spectrum is equal to specified value.
    template<typename t>
    bool is_sameC(const CImg<t>& img) const {
      return is_sameC(img._spectrum);
    }

    //! Test if image width and height are equal to specified values.
    /**
       Test if is_sameX(unsigned int) const and is_sameY(unsigned int) const are both verified.
    **/
    bool is_sameXY(const unsigned int size_x, const unsigned int size_y) const {
      return _width==size_x && _height==size_y;
    }

    //! Test if image width and height are the same as that of another image.
    /**
       Test if is_sameX(const CImg<t>&) const and is_sameY(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameXY(const CImg<t>& img) const {
      return is_sameXY(img._width,img._height);
    }

    //! Test if image width and height are the same as that of an existing display window.
    /**
       Test if is_sameX(const CImgDisplay&) const and is_sameY(const CImgDisplay&) const are both verified.
    **/
    bool is_sameXY(const CImgDisplay& disp) const {
      return is_sameXY(disp._width,disp._height);
    }

    //! Test if image width and depth are equal to specified values.
    /**
       Test if is_sameX(unsigned int) const and is_sameZ(unsigned int) const are both verified.
    **/
    bool is_sameXZ(const unsigned int size_x, const unsigned int size_z) const {
      return _width==size_x && _depth==size_z;
    }

    //! Test if image width and depth are the same as that of another image.
    /**
       Test if is_sameX(const CImg<t>&) const and is_sameZ(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameXZ(const CImg<t>& img) const {
      return is_sameXZ(img._width,img._depth);
    }

    //! Test if image width and spectrum are equal to specified values.
    /**
       Test if is_sameX(unsigned int) const and is_sameC(unsigned int) const are both verified.
    **/
    bool is_sameXC(const unsigned int size_x, const unsigned int size_c) const {
      return _width==size_x && _spectrum==size_c;
    }

    //! Test if image width and spectrum are the same as that of another image.
    /**
       Test if is_sameX(const CImg<t>&) const and is_sameC(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameXC(const CImg<t>& img) const {
      return is_sameXC(img._width,img._spectrum);
    }

    //! Test if image height and depth are equal to specified values.
    /**
       Test if is_sameY(unsigned int) const and is_sameZ(unsigned int) const are both verified.
    **/
    bool is_sameYZ(const unsigned int size_y, const unsigned int size_z) const {
      return _height==size_y && _depth==size_z;
    }

    //! Test if image height and depth are the same as that of another image.
    /**
       Test if is_sameY(const CImg<t>&) const and is_sameZ(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameYZ(const CImg<t>& img) const {
      return is_sameYZ(img._height,img._depth);
    }

    //! Test if image height and spectrum are equal to specified values.
    /**
       Test if is_sameY(unsigned int) const and is_sameC(unsigned int) const are both verified.
    **/
    bool is_sameYC(const unsigned int size_y, const unsigned int size_c) const {
      return _height==size_y && _spectrum==size_c;
    }

    //! Test if image height and spectrum are the same as that of another image.
    /**
       Test if is_sameY(const CImg<t>&) const and is_sameC(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameYC(const CImg<t>& img) const {
      return is_sameYC(img._height,img._spectrum);
    }

    //! Test if image depth and spectrum are equal to specified values.
    /**
       Test if is_sameZ(unsigned int) const and is_sameC(unsigned int) const are both verified.
    **/
    bool is_sameZC(const unsigned int size_z, const unsigned int size_c) const {
      return _depth==size_z && _spectrum==size_c;
    }

    //! Test if image depth and spectrum are the same as that of another image.
    /**
       Test if is_sameZ(const CImg<t>&) const and is_sameC(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameZC(const CImg<t>& img) const {
      return is_sameZC(img._depth,img._spectrum);
    }

    //! Test if image width, height and depth are equal to specified values.
    /**
       Test if is_sameXY(unsigned int,unsigned int) const and is_sameZ(unsigned int) const are both verified.
    **/
    bool is_sameXYZ(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z) const {
      return is_sameXY(size_x,size_y) && _depth==size_z;
    }

    //! Test if image width, height and depth are the same as that of another image.
    /**
       Test if is_sameXY(const CImg<t>&) const and is_sameZ(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameXYZ(const CImg<t>& img) const {
      return is_sameXYZ(img._width,img._height,img._depth);
    }

    //! Test if image width, height and spectrum are equal to specified values.
    /**
       Test if is_sameXY(unsigned int,unsigned int) const and is_sameC(unsigned int) const are both verified.
    **/
    bool is_sameXYC(const unsigned int size_x, const unsigned int size_y, const unsigned int size_c) const {
      return is_sameXY(size_x,size_y) && _spectrum==size_c;
    }

    //! Test if image width, height and spectrum are the same as that of another image.
    /**
       Test if is_sameXY(const CImg<t>&) const and is_sameC(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameXYC(const CImg<t>& img) const {
      return is_sameXYC(img._width,img._height,img._spectrum);
    }

    //! Test if image width, depth and spectrum are equal to specified values.
    /**
       Test if is_sameXZ(unsigned int,unsigned int) const and is_sameC(unsigned int) const are both verified.
    **/
    bool is_sameXZC(const unsigned int size_x, const unsigned int size_z, const unsigned int size_c) const {
      return is_sameXZ(size_x,size_z) && _spectrum==size_c;
    }

    //! Test if image width, depth and spectrum are the same as that of another image.
    /**
       Test if is_sameXZ(const CImg<t>&) const and is_sameC(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameXZC(const CImg<t>& img) const {
      return is_sameXZC(img._width,img._depth,img._spectrum);
    }

    //! Test if image height, depth and spectrum are equal to specified values.
    /**
       Test if is_sameYZ(unsigned int,unsigned int) const and is_sameC(unsigned int) const are both verified.
    **/
    bool is_sameYZC(const unsigned int size_y, const unsigned int size_z, const unsigned int size_c) const {
      return is_sameYZ(size_y,size_z) && _spectrum==size_c;
    }

    //! Test if image height, depth and spectrum are the same as that of another image.
    /**
       Test if is_sameYZ(const CImg<t>&) const and is_sameC(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameYZC(const CImg<t>& img) const {
      return is_sameYZC(img._height,img._depth,img._spectrum);
    }

    //! Test if image width, height, depth and spectrum are equal to specified values.
    /**
       Test if is_sameXYZ(unsigned int,unsigned int,unsigned int) const and is_sameC(unsigned int) const are both
       verified.
    **/
    bool is_sameXYZC(const unsigned int size_x, const unsigned int size_y,
                     const unsigned int size_z, const unsigned int size_c) const {
      return is_sameXYZ(size_x,size_y,size_z) && _spectrum==size_c;
    }

    //! Test if image width, height, depth and spectrum are the same as that of another image.
    /**
       Test if is_sameXYZ(const CImg<t>&) const and is_sameC(const CImg<t>&) const are both verified.
    **/
    template<typename t>
    bool is_sameXYZC(const CImg<t>& img) const {
      return is_sameXYZC(img._width,img._height,img._depth,img._spectrum);
    }

    //! Test if specified coordinates are inside image bounds.
    /**
       Return \c true if pixel located at (\c x,\c y,\c z,\c c) is inside bounds of the image instance,
       and \c false otherwise.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note
       - Return \c true only if all these conditions are verified:
         - The image instance is \e not empty.
         - <tt>0<=x<=\ref width() - 1</tt>.
         - <tt>0<=y<=\ref height() - 1</tt>.
         - <tt>0<=z<=\ref depth() - 1</tt>.
         - <tt>0<=c<=\ref spectrum() - 1</tt>.
    **/
    bool containsXYZC(const int x, const int y=0, const int z=0, const int c=0) const {
      return x>=0 && x<width() && y>=0 && y<height() && z>=0 && z<depth() && c>=0 && c<spectrum();
    }

    //! Test if pixel value is inside image bounds and get its X,Y,Z and C-coordinates.
    /**
       Return \c true, if specified reference refers to a pixel value inside bounds of the image instance,
       and \c false otherwise.
       \param pixel Reference to pixel value to test.
       \param[out] x X-coordinate of the pixel value, if test succeeds.
       \param[out] y Y-coordinate of the pixel value, if test succeeds.
       \param[out] z Z-coordinate of the pixel value, if test succeeds.
       \param[out] c C-coordinate of the pixel value, if test succeeds.
       \note
       - Useful to convert an offset to a buffer value into pixel value coordinates:
       \code
       const CImg<float> img(100,100,1,3); // Construct a 100x100 RGB color image
       const unsigned long offset = 1249; // Offset to the pixel (49,12,0,0)
       unsigned int x,y,z,c;
       if (img.contains(img[offset],x,y,z,c)) { // Convert offset to (x,y,z,c) coordinates
         std::printf("Offset %u refers to pixel located at (%u,%u,%u,%u).\n",
                     offset,x,y,z,c);
       }
       \endcode
    **/
    template<typename t>
    bool contains(const T& pixel, t& x, t& y, t& z, t& c) const {
      const ulongT wh = (ulongT)_width*_height, whd = wh*_depth, siz = whd*_spectrum;
      const T *const ppixel = &pixel;
      if (is_empty() || ppixel<_data || ppixel>=_data + siz) return false;
      ulongT off = (ulongT)(ppixel - _data);
      const ulongT nc = off/whd;
      off%=whd;
      const ulongT nz = off/wh;
      off%=wh;
      const ulongT ny = off/_width, nx = off%_width;
      x = (t)nx; y = (t)ny; z = (t)nz; c = (t)nc;
      return true;
    }

    //! Test if pixel value is inside image bounds and get its X,Y and Z-coordinates.
    /**
       Similar to contains(const T&,t&,t&,t&,t&) const, except that only the X,Y and Z-coordinates are set.
    **/
    template<typename t>
    bool contains(const T& pixel, t& x, t& y, t& z) const {
      const ulongT wh = (ulongT)_width*_height, whd = wh*_depth, siz = whd*_spectrum;
      const T *const ppixel = &pixel;
      if (is_empty() || ppixel<_data || ppixel>=_data + siz) return false;
      ulongT off = ((ulongT)(ppixel - _data))%whd;
      const ulongT nz = off/wh;
      off%=wh;
      const ulongT ny = off/_width, nx = off%_width;
      x = (t)nx; y = (t)ny; z = (t)nz;
      return true;
    }

    //! Test if pixel value is inside image bounds and get its X and Y-coordinates.
    /**
       Similar to contains(const T&,t&,t&,t&,t&) const, except that only the X and Y-coordinates are set.
    **/
    template<typename t>
    bool contains(const T& pixel, t& x, t& y) const {
      const ulongT wh = (ulongT)_width*_height, siz = wh*_depth*_spectrum;
      const T *const ppixel = &pixel;
      if (is_empty() || ppixel<_data || ppixel>=_data + siz) return false;
      ulongT off = ((unsigned int)(ppixel - _data))%wh;
      const ulongT ny = off/_width, nx = off%_width;
      x = (t)nx; y = (t)ny;
      return true;
    }

    //! Test if pixel value is inside image bounds and get its X-coordinate.
    /**
       Similar to contains(const T&,t&,t&,t&,t&) const, except that only the X-coordinate is set.
    **/
    template<typename t>
    bool contains(const T& pixel, t& x) const {
      const T *const ppixel = &pixel;
      if (is_empty() || ppixel<_data || ppixel>=_data + size()) return false;
      x = (t)(((ulongT)(ppixel - _data))%_width);
      return true;
    }

    //! Test if pixel value is inside image bounds.
    /**
       Similar to contains(const T&,t&,t&,t&,t&) const, except that no pixel coordinates are set.
    **/
    bool contains(const T& pixel) const {
      const T *const ppixel = &pixel;
      return !is_empty() && ppixel>=_data && ppixel<_data + size();
    }

    //! Test if pixel buffers of instance and input images overlap.
    /**
       Return \c true, if pixel buffers attached to image instance and input image \c img overlap,
       and \c false otherwise.
       \param img Input image to compare with.
       \note
       - Buffer overlapping may happen when manipulating \e shared images.
       - If two image buffers overlap, operating on one of the image will probably modify the other one.
       - Most of the time, \c CImg<T> instances are \e non-shared and do not overlap between each others.
       \par Example
       \code
       const CImg<float>
         img1("reference.jpg"), // Load RGB-color image
         img2 = img1.get_shared_channel(1); // Get shared version of the green channel
       if (img1.is_overlapped(img2)) { // Test succeeds, 'img1' and 'img2' overlaps
         std::printf("Buffers overlap!\n");
       }
       \endcode
    **/
    template<typename t>
    bool is_overlapped(const CImg<t>& img) const {
      const ulongT csiz = size(), isiz = img.size();
      return !((void*)(_data + csiz)<=(void*)img._data || (void*)_data>=(void*)(img._data + isiz));
    }
#endif
