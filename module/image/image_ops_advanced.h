#ifndef CIMG_MODULE_IMAGE_OPS_ADVANCED_H
#define CIMG_MODULE_IMAGE_OPS_ADVANCED_H

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

    //! Test if the set {\c *this,\c primitives,\c colors,\c opacities} defines a valid 3D object.
    /**
       Return \c true is the 3D object represented by the set {\c *this,\c primitives,\c colors,\c opacities} defines a
       valid 3D object, and \c false otherwise. The vertex coordinates are defined by the instance image.
       \param primitives List of primitives of the 3D object.
       \param colors List of colors of the 3D object.
       \param opacities List (or image) of opacities of the 3D object.
       \param full_check Tells if full checking of the 3D object must be performed.
       \param[out] error_message C-string to contain the error message, if the test does not succeed
                   (at least 256 bytes).
       \note
       - Set \c full_checking to \c false to speed-up the 3D object checking. In this case, only the size of
         each 3D object component is checked.
       - Size of the string \c error_message should be at least 128-bytes long, to be able to contain the error message.
    **/
    template<typename tp, typename tc, typename to>
    bool is_object3d(const CImgList<tp>& primitives,
                     const CImgList<tc>& colors,
                     const to& opacities,
                     const bool full_check=true,
                     char *const error_message=0) const {
      if (error_message) *error_message = 0;

      // Check consistency for the particular case of an empty 3D object.
      if (is_empty()) {
        if (primitives || colors || opacities) {
          if (error_message) cimg_snprintf(error_message,256,
                                          "3D object (%u,%u) defines no vertices but %u primitives, "
                                          "%u colors and %lu opacities",
                                          _width,primitives._width,primitives._width,
                                          colors._width,(unsigned long)opacities.size());
          return false;
        }
        return true;
      }

      // Check consistency of vertices.
      if (_height!=3 || _depth>1 || _spectrum>1) { // Check vertices dimensions
        if (error_message) cimg_snprintf(error_message,256,
                                        "3D object (%u,%u) has invalid vertex dimensions (%u,%u,%u,%u)",
                                        _width,primitives._width,_width,_height,_depth,_spectrum);
        return false;
      }
      if (colors._width>primitives._width + 1) {
        if (error_message) cimg_snprintf(error_message,256,
                                        "3D object (%u,%u) defines %u colors",
                                        _width,primitives._width,colors._width);
        return false;
      }
      if (opacities.size()>primitives._width) {
        if (error_message) cimg_snprintf(error_message,256,
                                        "3D object (%u,%u) defines %lu opacities",
                                        _width,primitives._width,(unsigned long)opacities.size());
        return false;
      }
      if (!full_check) return true;

      // Check consistency of primitives.
      cimglist_for(primitives,l) {
        const CImg<tp>& primitive = primitives[l];
        const unsigned int psiz = (unsigned int)primitive.size();
        switch (psiz) {
        case 1 : { // Point
          const unsigned int i0 = (unsigned int)primitive(0);
          if (i0>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex index %u in "
                                            "point primitive [%u]",
                                            _width,primitives._width,i0,l);
            return false;
          }
        } break;
        case 5 : { // Sphere
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          if (i0>=_width || i1>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                            "sphere primitive [%u]",
                                            _width,primitives._width,i0,i1,l);
            return false;
          }
        } break;
        case 2 : case 6 : { // Segment
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          if (i0>=_width || i1>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                            "segment primitive [%u]",
                                            _width,primitives._width,i0,i1,l);
            return false;
          }
        } break;
        case 3 : case 9 : { // Triangle
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2);
          if (i0>=_width || i1>=_width || i2>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex indices (%u,%u,%u) in "
                                            "triangle primitive [%u]",
                                            _width,primitives._width,i0,i1,i2,l);
            return false;
          }
        } break;
        case 4 : case 12 : { // Quadrangle
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2),
            i3 = (unsigned int)primitive(3);
          if (i0>=_width || i1>=_width || i2>=_width || i3>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex indices (%u,%u,%u,%u) in "
                                            "quadrangle primitive [%u]",
                                            _width,primitives._width,i0,i1,i2,i3,l);
            return false;
          }
        } break;
        default :
          if (error_message) cimg_snprintf(error_message,256,
                                          "3D object (%u,%u) defines an invalid primitive [%u] of size %u",
                                          _width,primitives._width,l,(unsigned int)psiz);
          return false;
        }
      }

      // Check consistency of colors.
      cimglist_for(colors,c) {
        const CImg<tc>& color = colors[c];
        if (!color) {
          if (error_message) cimg_snprintf(error_message,256,
                                          "3D object (%u,%u) defines no color for primitive [%u]",
                                          _width,primitives._width,c);
          return false;
        }
      }

      // Check consistency of light texture.
      if (colors._width>primitives._width) {
        const CImg<tc> &light = colors.back();
        if (!light || light._depth>1) {
          if (error_message) cimg_snprintf(error_message,256,
                                          "3D object (%u,%u) defines an invalid light texture (%u,%u,%u,%u)",
                                          _width,primitives._width,light._width,
                                          light._height,light._depth,light._spectrum);
          return false;
        }
      }

      return true;
    }

    //! Test if image instance represents a valid serialization of a 3D object.
    /**
       Return \c true if the image instance represents a valid serialization of a 3D object, and \c false otherwise.
       \param full_check Tells if full checking of the instance must be performed.
       \param[out] error_message C-string to contain the error message, if the test does not succeed.
       \note
       - Set \c full_check to \c false to speed-up the 3D object checking. In this case, only the size of
         each 3D object component is checked.
       - Size of the string \c error_message should be at least 256-bytes long, to be able to contain the error message.
    **/
    bool is_CImg3d(const bool full_check=true, char *const error_message=0) const {
      if (error_message) *error_message = 0;

      // Check instance dimension and header.
      if (_width!=1 || _height<8 || _depth!=1 || _spectrum!=1) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d has invalid dimensions (%u,%u,%u,%u)",
                                         _width,_height,_depth,_spectrum);
        return false;
      }
      const T *ptrs = _data, *const ptre = end();
      if (!_is_CImg3d(*(ptrs++),'C') || !_is_CImg3d(*(ptrs++),'I') || !_is_CImg3d(*(ptrs++),'m') ||
          !_is_CImg3d(*(ptrs++),'g') || !_is_CImg3d(*(ptrs++),'3') || !_is_CImg3d(*(ptrs++),'d')) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d header not found");
        return false;
      }
      if (!cimg::type<T>::is_finite(*ptrs) || !cimg::type<T>::is_finite(ptrs[1])) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "Specified numbers of vertices/primitives (%g/%g) are invalid",
                                         (double)*ptrs,(double)ptrs[1]);
        return false;
      }

      const unsigned int
        nb_points = cimg::float2uint(*(ptrs++)),
        nb_primitives = cimg::float2uint(*(ptrs++));

      // Check consistency of number of vertices / primitives.
      if (!full_check) {
        const ulongT minimal_size = 8UL + 3*nb_points + 6*nb_primitives;
        if (_data + minimal_size>ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) has only %lu values, "
                                           "while at least %lu values were expected",
                                           nb_points,nb_primitives,(unsigned long)size(),(unsigned long)minimal_size);
          return false;
        }
      }

      // Check consistency of vertex data.
      if (!nb_points) {
        if (nb_primitives) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) defines no vertices but %u primitives",
                                           nb_points,nb_primitives,nb_primitives);
          return false;
        }
        if (ptrs!=ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) is an empty object but contains %u value%s "
                                           "more than expected",
                                           nb_points,nb_primitives,(unsigned int)(ptre - ptrs),(ptre - ptrs)>1?"s":"");
          return false;
        }
        return true;
      }
      if (ptrs + 3*nb_points>ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) defines only %u vertices data",
                                         nb_points,nb_primitives,(unsigned int)(ptre - ptrs)/3);
        return false;
      }
      ptrs+=3*nb_points;

      // Check consistency of primitive data.
      if (ptrs==ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) defines %u vertices but no primitive",
                                         nb_points,nb_primitives,nb_points);
        return false;
      }

      if (!full_check) return true;

      for (unsigned int p = 0; p<nb_primitives; ++p) {
        const unsigned int nb_inds = (unsigned int)*(ptrs++);
        switch (nb_inds) {
        case 1 : { // Point
          const unsigned int i0 = cimg::float2uint(*(ptrs++));
          if (i0>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex index %u in point primitive [%u]",
                                             nb_points,nb_primitives,i0,p);
            return false;
          }
        } break;
        case 5 : { // Sphere
          const unsigned int
            i0 = cimg::float2uint(*(ptrs++)),
            i1 = cimg::float2uint(*(ptrs++));
          ptrs+=3;
          if (i0>=nb_points || i1>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                             "sphere primitive [%u]",
                                             nb_points,nb_primitives,i0,i1,p);
            return false;
          }
        } break;
        case 2 : case 6 : { // Segment
          const unsigned int
            i0 = cimg::float2uint(*(ptrs++)),
            i1 = cimg::float2uint(*(ptrs++));
          if (nb_inds==6) ptrs+=4;
          if (i0>=nb_points || i1>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                             "segment primitive [%u]",
                                             nb_points,nb_primitives,i0,i1,p);
            return false;
          }
        } break;
        case 3 : case 9 : { // Triangle
          const unsigned int
            i0 = cimg::float2uint(*(ptrs++)),
            i1 = cimg::float2uint(*(ptrs++)),
            i2 = cimg::float2uint(*(ptrs++));
          if (nb_inds==9) ptrs+=6;
          if (i0>=nb_points || i1>=nb_points || i2>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u,%u) in "
                                             "triangle primitive [%u]",
                                             nb_points,nb_primitives,i0,i1,i2,p);
            return false;
          }
        } break;
        case 4 : case 12 : { // Quadrangle
          const unsigned int
            i0 = cimg::float2uint(*(ptrs++)),
            i1 = cimg::float2uint(*(ptrs++)),
            i2 = cimg::float2uint(*(ptrs++)),
            i3 = cimg::float2uint(*(ptrs++));
          if (nb_inds==12) ptrs+=8;
          if (i0>=nb_points || i1>=nb_points || i2>=nb_points || i3>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u,%u,%u) in "
                                             "quadrangle primitive [%u]",
                                             nb_points,nb_primitives,i0,i1,i2,i3,p);
            return false;
          }
        } break;
        default :
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) defines an invalid primitive [%u] of size %u",
                                           nb_points,nb_primitives,p,nb_inds);
          return false;
        }
        if (ptrs>ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) has incomplete primitive data for primitive [%u], "
                                           "%u values missing",
                                           nb_points,nb_primitives,p,(unsigned int)(ptrs - ptre));
          return false;
        }
      }

      // Check consistency of color data.
      if (ptrs==ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) defines no color/texture data",
                                         nb_points,nb_primitives);
        return false;
      }
      for (unsigned int c = 0; c<nb_primitives; ++c) {
        if (*(ptrs++)!=(T)-128) ptrs+=2;
        else if ((ptrs+=3)<ptre) {
          const unsigned int
            w = (unsigned int)cimg::float2uint(*(ptrs - 3)),
            h = (unsigned int)*(ptrs - 2),
            s = (unsigned int)*(ptrs - 1);
          if (!h && !s) {
            if (w>=c) {
              if (error_message) cimg_snprintf(error_message,256,
                                               "CImg3d (%u,%u) refers to invalid shared sprite/texture index %u "
                                               "for primitive [%u]",
                                               nb_points,nb_primitives,w,c);
              return false;
            }
          } else ptrs+=w*h*s;
        }
        if (ptrs>ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) has incomplete color/texture data for primitive [%u], "
                                           "%u values missing",
                                           nb_points,nb_primitives,c,(unsigned int)(ptrs - ptre));
          return false;
        }
      }

      // Check consistency of opacity data.
      if (ptrs==ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) defines no opacity data",
                                         nb_points,nb_primitives);
        return false;
      }
      for (unsigned int o = 0; o<nb_primitives; ++o) {
        if (*(ptrs++)==(T)-128 && (ptrs+=3)<ptre) {
          const unsigned int
            w = (unsigned int)cimg::float2uint(*(ptrs - 3)),
            h = (unsigned int)*(ptrs - 2),
            s = (unsigned int)*(ptrs - 1);
          if (!h && !s) {
            if (w>=o) {
              if (error_message) cimg_snprintf(error_message,256,
                                               "CImg3d (%u,%u) refers to invalid shared opacity index %u "
                                               "for primitive [%u]",
                                               nb_points,nb_primitives,w,o);
              return false;
            }
          } else ptrs+=w*h*s;
        }
        if (ptrs>ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) has incomplete opacity data for primitive [%u]",
                                           nb_points,nb_primitives,o);
          return false;
        }
      }

      // Check end of data.
      if (ptrs<ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) contains %u value%s more than expected",
                                         nb_points,nb_primitives,(unsigned int)(ptre - ptrs),(ptre - ptrs)>1?"s":"");
        return false;
      }
      return true;
    }

    static bool _is_CImg3d(const T val, const char c) {
      return val>=(T)c && val<(T)(c + 1);
    }

    //@}
    //-------------------------------------

#endif
