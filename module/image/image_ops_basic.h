#ifndef CIMG_MODULE_IMAGE_OPS_BASIC_H
#define CIMG_MODULE_IMAGE_OPS_BASIC_H

    //---------------------------------------------------------
    //
    //! \name Constructors / Destructor / Instance Management
    //@{
    //---------------------------------------------------------

    //! Destroy image.
    /**
       \note
       - The pixel buffer data() is deallocated if necessary, e.g. for non-empty and non-shared image instances.
       - Destroying an empty or shared image does nothing actually.
       \warning
       - When destroying a non-shared image, make sure that you will \e not operate on a remaining shared image
         that shares its buffer with the destroyed instance, in order to avoid further invalid memory access
         (to a deallocated buffer).
    **/
    ~CImg() {
      if (!_is_shared) delete[] _data;
    }

    //! Construct empty image.
    /**
       \note
       - An empty image has no pixel data and all of its dimensions width(), height(), depth(), spectrum()
         are set to \c 0, as well as its pixel buffer pointer data().
       - An empty image may be re-assigned afterwards, e.g. with the family of
         assign(unsigned int,unsigned int,unsigned int,unsigned int) methods,
         or by operator=(const CImg<t>&). In all cases, the type of pixels stays \c T.
       - An empty image is never shared.
       \par Example
       \code
       CImg<float> img1, img2; // Construct two empty images
       img1.assign(256,256,1,3); // Re-assign 'img1' to be a 256x256x1x3 (color) image
       img2 = img1.get_rand(0,255); // Re-assign 'img2' to be a random-valued version of 'img1'
       img2.assign(); // Re-assign 'img2' to be an empty image again
       \endcode
    **/
    CImg():_width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {}

    //! Construct image with specified size.
    /**
       \param size_x Image width().
       \param size_y Image height().
       \param size_z Image depth().
       \param size_c Image spectrum() (number of channels).
       \note
       - It is able to create only \e non-shared images, and allocates thus a pixel buffer data()
         for each constructed image instance.
       - Setting one dimension \c size_x,\c size_y,\c size_z or \c size_c to \c 0 leads to the construction of
         an \e empty image.
       - A \c CImgInstanceException is thrown when the pixel buffer cannot be allocated
         (e.g. when requested size is too big for available memory).
       \warning
       - The allocated pixel buffer is \e not filled with a default value, and is likely to contain garbage values.
         In order to initialize pixel values during construction (e.g. with \c 0), use constructor
         CImg(unsigned int,unsigned int,unsigned int,unsigned int,T) instead.
       \par Example
       \code
       CImg<float> img1(256,256,1,3); // Construct a 256x256x1x3 (color) image, filled with garbage values
       CImg<float> img2(256,256,1,3,0); // Construct a 256x256x1x3 (color) image, filled with value '0'
       \endcode
    **/
    explicit CImg(const unsigned int size_x, const unsigned int size_y=1,
                  const unsigned int size_z=1, const unsigned int size_c=1):
      _is_shared(false) {
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (siz) {
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
        try { _data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                      size_x,size_y,size_z,size_c);
        }
      } else { _width = _height = _depth = _spectrum = 0; _data = 0; }
    }

    //! Construct image with specified size and initialize pixel values.
    /**
       \param size_x Image width().
       \param size_y Image height().
       \param size_z Image depth().
       \param size_c Image spectrum() (number of channels).
       \param value Initialization value.
       \note
       - Similar to CImg(unsigned int,unsigned int,unsigned int,unsigned int),
         but it also fills the pixel buffer with the specified \c value.
       \warning
       - It cannot be used to construct a vector-valued image and initialize it with \e vector-valued pixels
         (e.g. RGB vector, for color images).
         For this task, you may use fillC() after construction.
    **/
    CImg(const unsigned int size_x, const unsigned int size_y,
         const unsigned int size_z, const unsigned int size_c, const T& value):
      _is_shared(false) {
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (siz) {
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
        try { _data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                      size_x,size_y,size_z,size_c);
        }
        fill(value);
      } else { _width = _height = _depth = _spectrum = 0; _data = 0; }
    }

    //! Construct image with specified size and initialize pixel values from a sequence of integers.
    /**
       Construct a new image instance of size \c size_x x \c size_y x \c size_z x \c size_c,
       with pixels of type \c T, and initialize pixel
       values from the specified sequence of integers \c value0,\c value1,\c ...
       \param size_x Image width().
       \param size_y Image height().
       \param size_z Image depth().
       \param size_c Image spectrum() (number of channels).
       \param value0 First value of the initialization sequence (must be an \e integer).
       \param value1 Second value of the initialization sequence (must be an \e integer).
       \param ...
       \note
       - Similar to CImg(unsigned int,unsigned int,unsigned int,unsigned int), but it also fills
         the pixel buffer with a sequence of specified integer values.
       \warning
       - You must specify \e exactly \c size_x*\c size_y*\c size_z*\c size_c integers in the initialization sequence.
         Otherwise, the constructor may crash or fill your image pixels with garbage.
       \par Example
       \code
       const CImg<float> img(2,2,1,3, // Construct a 2x2 color (RGB) image
                             0,255,0,255, // Set the 4 values for the red component
                             0,0,255,255, // Set the 4 values for the green component
                             64,64,64,64); // Set the 4 values for the blue component
       img.resize(150,150).display();
       \endcode
       \image html ref_constructor1.jpg
     **/
    CImg(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z, const unsigned int size_c,
         const int value0, const int value1, ...):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
#define _CImg_stdarg(img,a0,a1,N,t) { \
        size_t _siz = (size_t)N; \
        if (_siz--) { \
          va_list ap; \
          va_start(ap,a1); \
          T *ptrd = (img)._data; \
          *(ptrd++) = (T)a0; \
          if (_siz--) { \
            *(ptrd++) = (T)a1; \
            for ( ; _siz; --_siz) *(ptrd++) = (T)va_arg(ap,t); \
          } \
          va_end(ap); \
        } \
      }
      assign(size_x,size_y,size_z,size_c);
      _CImg_stdarg(*this,value0,value1,safe_size(size_x,size_y,size_z,size_c),int);
    }

#if cimg_use_cpp11==1
    //! Construct image with specified size and initialize pixel values from an initializer list of integers.
    /**
       Construct a new image instance of size \c size_x x \c size_y x \c size_z x \c size_c,
       with pixels of type \c T, and initialize pixel
       values from the specified initializer list of integers { \c value0,\c value1,\c ... }
       \param size_x Image width().
       \param size_y Image height().
       \param size_z Image depth().
       \param size_c Image spectrum() (number of channels).
       \param { value0, value1, ... } Initialization list
       \param repeat_values Tells if the value filling process is repeated over the image.

       \note
       - Similar to CImg(unsigned int,unsigned int,unsigned int,unsigned int), but it also fills
         the pixel buffer with a sequence of specified integer values.
       \par Example
       \code
       const CImg<float> img(2,2,1,3, // Construct a 2x2 color (RGB) image
                             { 0,255,0,255, // Set the 4 values for the red component
                               0,0,255,255, // Set the 4 values for the green component
                               64,64,64,64 }); // Set the 4 values for the blue component
       img.resize(150,150).display();
       \endcode
       \image html ref_constructor1.jpg
    **/
    template<typename t>
    CImg(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z, const unsigned int size_c,
         const std::initializer_list<t> values,
         const bool repeat_values=true):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
#define _cimg_constructor_cpp11(repeat_values) \
  auto it = values.begin(); \
  size_t siz = size(); \
  if (repeat_values) for (T *ptrd = _data; siz--; ) { \
    *(ptrd++) = (T)(*(it++)); if (it==values.end()) it = values.begin(); } \
  else { siz = std::min(siz,values.size()); for (T *ptrd = _data; siz--; ) *(ptrd++) = (T)(*(it++)); }
      assign(size_x,size_y,size_z,size_c);
      _cimg_constructor_cpp11(repeat_values);
    }

    template<typename t>
    CImg(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z,
         std::initializer_list<t> values,
         const bool repeat_values=true):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(size_x,size_y,size_z);
      _cimg_constructor_cpp11(repeat_values);
    }

    template<typename t>
    CImg(const unsigned int size_x, const unsigned int size_y,
         std::initializer_list<t> values,
         const bool repeat_values=true):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(size_x,size_y);
      _cimg_constructor_cpp11(repeat_values);
    }

    template<typename t>
    CImg(const unsigned int size_x,
         std::initializer_list<t> values,
         const bool repeat_values=true):_width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(size_x);
      _cimg_constructor_cpp11(repeat_values);
    }

    //! Construct single channel 1D image with pixel values and width obtained from an initializer list of integers.
    /**
       Construct a new image instance of size \c width x \c 1 x \c 1 x \c 1,
       with pixels of type \c T, and initialize pixel
       values from the specified initializer list of integers { \c value0,\c value1,\c ... }. Image width is
       given by the size of the initializer list.
       \param { value0, value1, ... } Initialization list
       \note
       - Similar to CImg(unsigned int,unsigned int,unsigned int,unsigned int) with height=1, depth=1, and spectrum=1,
         but it also fills the pixel buffer with a sequence of specified integer values.
       \par Example
       \code
       const CImg<float> img = {10,20,30,20,10 }; // Construct a 5x1 image with one channel, and set its pixel values
       img.resize(150,150).display();
       \endcode
       \image html ref_constructor1.jpg
     **/
    template<typename t>
    CImg(const std::initializer_list<t> values):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(values.size(),1,1,1);
      auto it = values.begin();
      unsigned int siz = _width;
      for (T *ptrd = _data; siz--; ) *(ptrd++) = (T)(*(it++));
    }

    template<typename t>
    CImg<T>& operator=(std::initializer_list<t> values) {
      _cimg_constructor_cpp11(siz>values.size());
      return *this;
    }
#endif

    //! Construct image with specified size and initialize pixel values from a sequence of doubles.
    /**
       Construct a new image instance of size \c size_x x \c size_y x \c size_z x \c size_c, with pixels of type \c T,
       and initialize pixel values from the specified sequence of doubles \c value0,\c value1,\c ...
       \param size_x Image width().
       \param size_y Image height().
       \param size_z Image depth().
       \param size_c Image spectrum() (number of channels).
       \param value0 First value of the initialization sequence (must be a \e double).
       \param value1 Second value of the initialization sequence (must be a \e double).
       \param ...
       \note
       - Similar to CImg(unsigned int,unsigned int,unsigned int,unsigned int,int,int,...), but
         takes a sequence of double values instead of integers.
       \warning
       - You must specify \e exactly \c dx*\c dy*\c dz*\c dc doubles in the initialization sequence.
         Otherwise, the constructor may crash or fill your image with garbage.
         For instance, the code below will probably crash on most platforms:
         \code
         const CImg<float> img(2,2,1,1, 0.5,0.5,255,255); // FAIL: The two last arguments are 'int', not 'double'!
         \endcode
     **/
    CImg(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z, const unsigned int size_c,
         const double value0, const double value1, ...):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(size_x,size_y,size_z,size_c);
      _CImg_stdarg(*this,value0,value1,safe_size(size_x,size_y,size_z,size_c),double);
    }

    //! Construct image with specified size and initialize pixel values from a value string.
    /**
       Construct a new image instance of size \c size_x x \c size_y x \c size_z x \c size_c, with pixels of type \c T,
       and initializes pixel values from the specified string \c values.
       \param size_x Image width().
       \param size_y Image height().
       \param size_z Image depth().
       \param size_c Image spectrum() (number of channels).
       \param values Value string describing the way pixel values are set.
       \param repeat_values Tells if the value filling process is repeated over the image.
       \note
       - Similar to CImg(unsigned int,unsigned int,unsigned int,unsigned int), but it also fills
         the pixel buffer with values described in the value string \c values.
       - Value string \c values may describe two different filling processes:
         - Either \c values is a sequences of values assigned to the image pixels, as in <tt>"1,2,3,7,8,2"</tt>.
           In this case, set \c repeat_values to \c true to periodically fill the image with the value sequence.
         - Either, \c values is a formula, as in <tt>"cos(x/10)*sin(y/20)"</tt>.
           In this case, parameter \c repeat_values is pointless.
       - For both cases, specifying \c repeat_values is mandatory.
         It disambiguates the possible overloading of constructor
         CImg(unsigned int,unsigned int,unsigned int,unsigned int,T) with \c T being a <tt>const char*</tt>.
       - A \c CImgArgumentException is thrown when an invalid value string \c values is specified.
       \par Example
       \code
       const CImg<float> img1(129,129,1,3,"0,64,128,192,255",true), // Construct image from a value sequence
                         img2(129,129,1,3,"if(c==0,255*abs(cos(x/10)),1.8*y)",false); // Construct image from a formula
       (img1,img2).display();
       \endcode
       \image html ref_constructor2.jpg
     **/
    CImg(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z, const unsigned int size_c,
         const char *const values, const bool repeat_values):_is_shared(false) {
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (siz) {
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
        try { _data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                      size_x,size_y,size_z,size_c);
        }
        fill(values,repeat_values);
      } else { _width = _height = _depth = _spectrum = 0; _data = 0; }
    }

    //! Construct image with specified size and initialize pixel values from a memory buffer.
    /**
       Construct a new image instance of size \c size_x x \c size_y x \c size_z x \c size_c, with pixels of type \c T,
       and initializes pixel values from the specified \c t* memory buffer.
       \param values Pointer to the input memory buffer.
       \param size_x Image width().
       \param size_y Image height().
       \param size_z Image depth().
       \param size_c Image spectrum() (number of channels).
       \param is_shared Tells if input memory buffer must be shared by the current instance.
       \note
       - If \c is_shared is \c false, the image instance allocates its own pixel buffer,
         and values from the specified input buffer are copied to the instance buffer.
         If buffer types \c T and \c t are different, a regular static cast is performed during buffer copy.
       - Otherwise, the image instance does \e not allocate a new buffer, and uses the input memory buffer as its
         own pixel buffer. This case requires that types \c T and \c t are the same. Later, destroying such a shared
         image will not deallocate the pixel buffer, this task being obviously charged to the initial buffer allocator.
       - A \c CImgInstanceException is thrown when the pixel buffer cannot be allocated
         (e.g. when requested size is too big for available memory).
       \warning
       - You must take care when operating on a shared image, since it may have an invalid pixel buffer pointer data()
         (e.g. already deallocated).
       \par Example
       \code
       unsigned char tab[256*256] = {};
       CImg<unsigned char> img1(tab,256,256,1,1,false), // Construct new non-shared image from buffer 'tab'
                           img2(tab,256,256,1,1,true); // Construct new shared-image from buffer 'tab'
       tab[1024] = 255; // Here, 'img2' is indirectly modified, but not 'img1'
       \endcode
    **/
    template<typename t>
    CImg(const t *const values, const unsigned int size_x, const unsigned int size_y=1,
         const unsigned int size_z=1, const unsigned int size_c=1, const bool is_shared=false):_is_shared(false) {
      if (is_shared) {
        _width = _height = _depth = _spectrum = 0; _data = 0;
        throw CImgArgumentException(_cimg_instance
                                    "CImg(): Invalid construction request of a (%u,%u,%u,%u) shared instance "
                                    "from a (%s*) buffer (pixel types are different).",
                                    cimg_instance,
                                    size_x,size_y,size_z,size_c,CImg<t>::pixel_type());
      }
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (values && siz) {
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
        try { _data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                      size_x,size_y,size_z,size_c);

        }
        const t *ptrs = values; cimg_for(*this,ptrd,T) *ptrd = (T)*(ptrs++);
      } else { _width = _height = _depth = _spectrum = 0; _data = 0; }
    }

    //! Construct image with specified size and initialize pixel values from a memory buffer \specialization.
    CImg(const T *const values, const unsigned int size_x, const unsigned int size_y=1,
         const unsigned int size_z=1, const unsigned int size_c=1, const bool is_shared=false) {
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (values && siz) {
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c; _is_shared = is_shared;
        if (_is_shared) _data = const_cast<T*>(values);
        else {
          try { _data = new T[siz]; } catch (...) {
            _width = _height = _depth = _spectrum = 0; _data = 0;
            throw CImgInstanceException(_cimg_instance
                                        "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                        cimg_instance,
                                        cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                        size_x,size_y,size_z,size_c);
          }
          std::memcpy(_data,values,siz*sizeof(T));
        }
      } else { _width = _height = _depth = _spectrum = 0; _is_shared = false; _data = 0; }
    }

    //! Construct image from memory buffer with specified size and pixel ordering scheme.
    template<typename t>
    CImg(const t *const values, const unsigned int size_x, const unsigned int size_y,
         const unsigned int size_z, const unsigned int size_c,
         const char *const axes_order):_data(0),_is_shared(false) {
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (values && siz) {
        unsigned char s_code[4] = { 0,1,2,3 }, n_code[4] = {};
        for (unsigned int l = 0; axes_order[l]; ++l) {
          int c = cimg::lowercase(axes_order[l]);
          if (l>=4 || (c!='x' && c!='y' && c!='z' && c!='c')) { *s_code = 4; break; }
          else { ++n_code[c%=4]; s_code[l] = c; }
        }
        if (*axes_order && *s_code<4 && *n_code<=1 && n_code[1]<=1 && n_code[2]<=1 && n_code[3]<=1) {
          const unsigned int code = (s_code[0]<<12) | (s_code[1]<<8) | (s_code[2]<<4) | (s_code[3]);
          int s0 = 0, s1 = 0, s2 = 0, s3 = 0;
          const char *inv_order = 0;
          switch (code) {
            case 0x0123 : inv_order = "xyzc"; s0 = size_x; s1 = size_y; s2 = size_z; s3 = size_c; break; // xyzc
            case 0x0132 : inv_order = "xyzc"; s0 = size_x; s1 = size_y; s2 = size_c; s3 = size_z; break; // xycz
            case 0x0213 : inv_order = "xzyc"; s0 = size_x; s1 = size_z; s2 = size_y; s3 = size_c; break; // xzyc
            case 0x0231 : inv_order = "xcyz"; s0 = size_x; s1 = size_z; s2 = size_c; s3 = size_y; break; // xzcy
            case 0x0312 : inv_order = "xzcy"; s0 = size_x; s1 = size_c; s2 = size_y; s3 = size_z; break; // xcyz
            case 0x0321 : inv_order = "xczy"; s0 = size_x; s1 = size_c; s2 = size_z; s3 = size_y; break; // xczy
            case 0x1023 : inv_order = "yxzc"; s0 = size_y; s1 = size_x; s2 = size_z; s3 = size_c; break; // yxzc
            case 0x1032 : inv_order = "yxcz"; s0 = size_y; s1 = size_x; s2 = size_c; s3 = size_z; break; // yxcz
            case 0x1203 : inv_order = "zxyc"; s0 = size_y; s1 = size_z; s2 = size_x; s3 = size_c; break; // yzxc
            case 0x1230 : inv_order = "cxyz"; s0 = size_y; s1 = size_z; s2 = size_c; s3 = size_x; break; // yzcx
            case 0x1302 : inv_order = "zxcy"; s0 = size_y; s1 = size_c; s2 = size_x; s3 = size_z; break; // ycxz
            case 0x1320 : inv_order = "cxzy"; s0 = size_y; s1 = size_c; s2 = size_z; s3 = size_x; break; // yczx
            case 0x2013 : inv_order = "yzxc"; s0 = size_z; s1 = size_x; s2 = size_y; s3 = size_c; break; // zxyc
            case 0x2031 : inv_order = "ycxz"; s0 = size_z; s1 = size_x; s2 = size_c; s3 = size_y; break; // zxcy
            case 0x2103 : inv_order = "zyxc"; s0 = size_z; s1 = size_y; s2 = size_x; s3 = size_c; break; // zyxc
            case 0x2130 : inv_order = "cyxz"; s0 = size_z; s1 = size_y; s2 = size_c; s3 = size_x; break; // zycx
            case 0x2301 : inv_order = "zcxy"; s0 = size_z; s1 = size_c; s2 = size_x; s3 = size_y; break; // zcxy
            case 0x2310 : inv_order = "czxy"; s0 = size_z; s1 = size_c; s2 = size_y; s3 = size_x; break; // zcyx
            case 0x3012 : inv_order = "yzcx"; s0 = size_c; s1 = size_x; s2 = size_y; s3 = size_z; break; // cxyz
            case 0x3021 : inv_order = "yczx"; s0 = size_c; s1 = size_x; s2 = size_z; s3 = size_y; break; // cxzy
            case 0x3102 : inv_order = "zycx"; s0 = size_c; s1 = size_y; s2 = size_x; s3 = size_z; break; // cyxz
            case 0x3120 : inv_order = "cyzx"; s0 = size_c; s1 = size_y; s2 = size_z; s3 = size_x; break; // cyzx
            case 0x3201 : inv_order = "zcyx"; s0 = size_c; s1 = size_z; s2 = size_x; s3 = size_y; break; // czxy
            case 0x3210 : inv_order = "czyx"; s0 = size_c; s1 = size_z; s2 = size_y; s3 = size_x; break; // czyx
          }
          CImg<t>(values,s0,s1,s2,s3,true).get_permute_axes(inv_order).move_to(*this);
        } else {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgArgumentException(_cimg_instance
                                      "CImg(): Invalid specified axes order '%s'.",
                                      cimg_instance,
                                      axes_order);
        }
      } else { _width = _height = _depth = _spectrum = 0; _is_shared = false; _data = 0; }
    }

    //! Construct image from reading an image file.
    /**
       Construct a new image instance with pixels of type \c T, and initialize pixel values with the data read from
       an image file.
       \param filename Filename, as a C-string.
       \note
       - Similar to CImg(unsigned int,unsigned int,unsigned int,unsigned int), but it reads the image
         dimensions and pixel values from the specified image file.
       - The recognition of the image file format by %CImg higlhy depends on the tools installed on your system
         and on the external libraries you used to link your code against.
       - Considered pixel type \c T should better fit the file format specification, or data loss may occur during
         file load (e.g. constructing a \c CImg<unsigned char> from a float-valued image file).
       - A \c CImgIOException is thrown when the specified \c filename cannot be read, or if the file format is not
         recognized.
       \par Example
       \code
       const CImg<float> img("reference.jpg");
       img.display();
       \endcode
       \image html ref_image.jpg
    **/
    explicit CImg(const char *const filename):_width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(filename);
    }

    //! Construct image copy.
    /**
       Construct a new image instance with pixels of type \c T, as a copy of an existing \c CImg<t> instance.
       \param img Input image to copy.
       \note
       - Constructed copy has the same size width() x height() x depth() x spectrum() and pixel values as the
         input image \c img.
       - If input image \c img is \e shared and if types \c T and \c t are the same, the constructed copy is also
         \e shared, and shares its pixel buffer with \c img.
         Modifying a pixel value in the constructed copy will thus also modifies it in the input image \c img.
         This behavior is needful to allow functions to return shared images.
       - Otherwise, the constructed copy allocates its own pixel buffer, and copies pixel values from the input
         image \c img into its buffer. The copied pixel values may be eventually statically casted if types \c T and
         \c t are different.
       - Constructing a copy from an image \c img when types \c t and \c T are the same is significantly faster than
         with different types.
       - A \c CImgInstanceException is thrown when the pixel buffer cannot be allocated
         (e.g. not enough available memory).
    **/
    template<typename t>
    CImg(const CImg<t>& img):_is_shared(false) {
      const size_t siz = (size_t)img.size();
      if (img._data && siz) {
        _width = img._width; _height = img._height; _depth = img._depth; _spectrum = img._spectrum;
        try { _data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*img._width*img._height*img._depth*img._spectrum),
                                      img._width,img._height,img._depth,img._spectrum);
        }
        const t *ptrs = img._data; cimg_for(*this,ptrd,T) *ptrd = (T)*(ptrs++);
      } else { _width = _height = _depth = _spectrum = 0; _data = 0; }
    }

    //! Construct image copy \specialization.
    CImg(const CImg<T>& img) {
      const size_t siz = (size_t)img.size();
      if (img._data && siz) {
        _width = img._width; _height = img._height; _depth = img._depth; _spectrum = img._spectrum;
        _is_shared = img._is_shared;
        if (_is_shared) _data = const_cast<T*>(img._data);
        else {
          try { _data = new T[siz]; } catch (...) {
            _width = _height = _depth = _spectrum = 0; _data = 0;
            throw CImgInstanceException(_cimg_instance
                                        "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                        cimg_instance,
                                        cimg::strbuffersize(sizeof(T)*img._width*img._height*img._depth*img._spectrum),
                                        img._width,img._height,img._depth,img._spectrum);

          }
          std::memcpy(_data,img._data,siz*sizeof(T));
        }
      } else { _width = _height = _depth = _spectrum = 0; _is_shared = false; _data = 0; }
    }

    //! Advanced copy constructor.
    /**
       Construct a new image instance with pixels of type \c T, as a copy of an existing \c CImg<t> instance,
       while forcing the shared state of the constructed copy.
       \param img Input image to copy.
       \param is_shared Tells about the shared state of the constructed copy.
       \note
       - Similar to CImg(const CImg<t>&), except that it allows to decide the shared state of
         the constructed image, which does not depend anymore on the shared state of the input image \c img:
         - If \c is_shared is \c true, the constructed copy will share its pixel buffer with the input image \c img.
           For that case, the pixel types \c T and \c t \e must be the same.
         - If \c is_shared is \c false, the constructed copy will allocate its own pixel buffer, whether the input
           image \c img is shared or not.
       - A \c CImgArgumentException is thrown when a shared copy is requested with different pixel types \c T and \c t.
    **/
    template<typename t>
    CImg(const CImg<t>& img, const bool is_shared):_is_shared(false) {
      if (is_shared) {
        _width = _height = _depth = _spectrum = 0; _data = 0;
        throw CImgArgumentException(_cimg_instance
                                    "CImg(): Invalid construction request of a shared instance from a "
                                    "CImg<%s> image (%u,%u,%u,%u,%p) (pixel types are different).",
                                    cimg_instance,
                                    CImg<t>::pixel_type(),img._width,img._height,img._depth,img._spectrum,img._data);
      }
      const size_t siz = (size_t)img.size();
      if (img._data && siz) {
        _width = img._width; _height = img._height; _depth = img._depth; _spectrum = img._spectrum;
        try { _data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*img._width*img._height*img._depth*img._spectrum),
                                      img._width,img._height,img._depth,img._spectrum);
        }
        const t *ptrs = img._data; cimg_for(*this,ptrd,T) *ptrd = (T)*(ptrs++);
      } else { _width = _height = _depth = _spectrum = 0; _data = 0; }
    }

    //! Advanced copy constructor \specialization.
    CImg(const CImg<T>& img, const bool is_shared) {
      const size_t siz = (size_t)img.size();
      if (img._data && siz) {
        _width = img._width; _height = img._height; _depth = img._depth; _spectrum = img._spectrum;
        _is_shared = is_shared;
        if (_is_shared) _data = const_cast<T*>(img._data);
        else {
          try { _data = new T[siz]; } catch (...) {
            _width = _height = _depth = _spectrum = 0; _data = 0;
            throw CImgInstanceException(_cimg_instance
                                        "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                        cimg_instance,
                                        cimg::strbuffersize(sizeof(T)*img._width*img._height*img._depth*img._spectrum),
                                        img._width,img._height,img._depth,img._spectrum);
          }
          std::memcpy(_data,img._data,siz*sizeof(T));
        }
      } else { _width = _height = _depth = _spectrum = 0; _is_shared = false; _data = 0; }
    }

    //! Construct image with dimensions borrowed from another image.
    /**
       Construct a new image instance with pixels of type \c T, and size get from some dimensions of an existing
       \c CImg<t> instance.
       \param img Input image from which dimensions are borrowed.
       \param dimensions C-string describing the image size along the X,Y,Z and C-dimensions.
       \note
       - Similar to CImg(unsigned int,unsigned int,unsigned int,unsigned int), but it takes the image dimensions
         (\e not its pixel values) from an existing \c CImg<t> instance.
       - The allocated pixel buffer is \e not filled with a default value, and is likely to contain garbage values.
         In order to initialize pixel values (e.g. with \c 0), use constructor CImg(const CImg<t>&,const char*,T)
         instead.
       \par Example
       \code
       const CImg<float> img1(256,128,1,3), // 'img1' is a 256x128x1x3 image
                         img2(img1,"xyzc"), // 'img2' is a 256x128x1x3 image
                         img3(img1,"y,x,z,c"), // 'img3' is a 128x256x1x3 image
                         img4(img1,"c,x,y,3",0), // 'img4' is a 3x128x256x3 image (with pixels initialized to '0')
       \endcode
     **/
    template<typename t>
    CImg(const CImg<t>& img, const char *const dimensions):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(img,dimensions);
    }

    //! Construct image with dimensions borrowed from another image and initialize pixel values.
    /**
       Construct a new image instance with pixels of type \c T, and size get from the dimensions of an existing
       \c CImg<t> instance, and set all pixel values to specified \c value.
       \param img Input image from which dimensions are borrowed.
       \param dimensions String describing the image size along the X,Y,Z and V-dimensions.
       \param value Value used for initialization.
       \note
       - Similar to CImg(const CImg<t>&,const char*), but it also fills the pixel buffer with the specified \c value.
     **/
    template<typename t>
    CImg(const CImg<t>& img, const char *const dimensions, const T& value):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(img,dimensions).fill(value);
    }

    //! Construct image from a display window.
    /**
       Construct a new image instance with pixels of type \c T, as a snapshot of an existing \c CImgDisplay instance.
       \param disp Input display window.
       \note
       - The width() and height() of the constructed image instance are the same as the specified \c CImgDisplay.
       - The depth() and spectrum() of the constructed image instance are respectively set to \c 1 and \c 3
         (i.e. a 2D color image).
       - The image pixels are read as 8-bits RGB values.
     **/
    explicit CImg(const CImgDisplay &disp):_width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      disp.snapshot(*this);
    }

    // Constructor and assignment operator for rvalue references (c++11).
    // This avoids an additional image copy for methods returning new images. Can save RAM for big images!
#if cimg_use_cpp11==1
    CImg(CImg<T>&& img):_width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      swap(img);
    }

    CImg<T>& operator=(CImg<T>&& img) {
      if (_is_shared) return assign(img);
      return img.swap(*this);
    }
#endif

    //! Construct empty image \inplace.
    /**
       In-place version of the default constructor CImg(). It simply resets the instance to an empty image.
    **/
    CImg<T>& assign() {
      if (!_is_shared) delete[] _data;
      _width = _height = _depth = _spectrum = 0; _is_shared = false; _data = 0;
      return *this;
    }

    //! Construct image with specified size \inplace.
    /**
       In-place version of the constructor CImg(unsigned int,unsigned int,unsigned int,unsigned int).
    **/
    CImg<T>& assign(const unsigned int size_x, const unsigned int size_y=1,
                    const unsigned int size_z=1, const unsigned int size_c=1) {
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (!siz) return assign();
      const size_t curr_siz = (size_t)size();
      if (siz!=curr_siz) {
        if (_is_shared)
          throw CImgArgumentException(_cimg_instance
                                      "assign(): Invalid assignment request of shared instance from specified "
                                      "image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      size_x,size_y,size_z,size_c);
        if (siz>curr_siz || (curr_siz>4096 && siz<curr_siz/2)) {
          delete[] _data;
          try { _data = new T[siz]; } catch (...) {
            _width = _height = _depth = _spectrum = 0; _data = 0;
            throw CImgInstanceException(_cimg_instance
                                        "assign(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                        cimg_instance,
                                        cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                        size_x,size_y,size_z,size_c);
          }
        }
      }
      _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
      return *this;
    }

    //! Construct image with specified size and initialize pixel values \inplace.
    /**
       In-place version of the constructor CImg(unsigned int,unsigned int,unsigned int,unsigned int,T).
    **/
    CImg<T>& assign(const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c, const T& value) {
      return assign(size_x,size_y,size_z,size_c).fill(value);
    }

    //! Construct image with specified size and initialize pixel values from a sequence of integers \inplace.
    /**
       In-place version of the constructor CImg(unsigned int,unsigned int,unsigned int,unsigned int,int,int,...).
    **/
    CImg<T>& assign(const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c,
                    const int value0, const int value1, ...) {
      assign(size_x,size_y,size_z,size_c);
      _CImg_stdarg(*this,value0,value1,safe_size(size_x,size_y,size_z,size_c),int);
      return *this;
    }

    //! Construct image with specified size and initialize pixel values from a sequence of doubles \inplace.
    /**
       In-place version of the constructor CImg(unsigned int,unsigned int,unsigned int,unsigned int,double,double,...).
    **/
    CImg<T>& assign(const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c,
                    const double value0, const double value1, ...) {
      assign(size_x,size_y,size_z,size_c);
      _CImg_stdarg(*this,value0,value1,safe_size(size_x,size_y,size_z,size_c),double);
      return *this;
    }

    //! Construct image with specified size and initialize pixel values from a value string \inplace.
    /**
       In-place version of the constructor CImg(unsigned int,unsigned int,unsigned int,unsigned int,const char*,bool).
    **/
    CImg<T>& assign(const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c,
                    const char *const values, const bool repeat_values) {
      return assign(size_x,size_y,size_z,size_c).fill(values,repeat_values);
    }

    //! Construct image with specified size and initialize pixel values from a memory buffer \inplace.
    /**
       In-place version of the constructor CImg(const t*,unsigned int,unsigned int,unsigned int,unsigned int).
    **/
    template<typename t>
    CImg<T>& assign(const t *const values, const unsigned int size_x, const unsigned int size_y=1,
                    const unsigned int size_z=1, const unsigned int size_c=1) {
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (!values || !siz) return assign();
      assign(size_x,size_y,size_z,size_c);
      const t *ptrs = values; cimg_for(*this,ptrd,T) *ptrd = (T)*(ptrs++);
      return *this;
    }

    //! Construct image with specified size and initialize pixel values from a memory buffer \specialization.
    CImg<T>& assign(const T *const values, const unsigned int size_x, const unsigned int size_y=1,
                    const unsigned int size_z=1, const unsigned int size_c=1) {
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (!values || !siz) return assign();
      const size_t curr_siz = (size_t)size();
      if (values==_data && siz==curr_siz) return assign(size_x,size_y,size_z,size_c);
      if (_is_shared || values + siz<_data || values>=_data + size()) {
        assign(size_x,size_y,size_z,size_c);
        if (_is_shared) std::memmove((void*)_data,(void*)values,siz*sizeof(T));
        else std::memcpy((void*)_data,(void*)values,siz*sizeof(T));
      } else {
        T *new_data = 0;
        try { new_data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "assign(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                      size_x,size_y,size_z,size_c);
        }
        std::memcpy((void*)new_data,(void*)values,siz*sizeof(T));
        delete[] _data; _data = new_data; _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
      }
      return *this;
    }

    //! Construct image with specified size and initialize pixel values from a memory buffer \overloading.
    template<typename t>
    CImg<T>& assign(const t *const values, const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c, const bool is_shared) {
      if (is_shared)
        throw CImgArgumentException(_cimg_instance
                                    "assign(): Invalid assignment request of shared instance from (%s*) buffer "
                                    "(pixel types are different).",
                                    cimg_instance,
                                    CImg<t>::pixel_type());
      return assign(values,size_x,size_y,size_z,size_c);
    }

    //! Construct image with specified size and initialize pixel values from a memory buffer \overloading.
    CImg<T>& assign(const T *const values, const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c, const bool is_shared) {
      const size_t siz = safe_size(size_x,size_y,size_z,size_c);
      if (!values || !siz) return assign();
      if (!is_shared) { if (_is_shared) assign(); assign(values,size_x,size_y,size_z,size_c); }
      else {
        if (!_is_shared) {
          if (values + siz<_data || values>=_data + size()) assign();
          else cimg::warn(_cimg_instance
                          "assign(): Shared image instance has overlapping memory.",
                          cimg_instance);
        }
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c; _is_shared = true;
        _data = const_cast<T*>(values);
      }
      return *this;
    }

    //! Construct image from memory buffer with specified size and pixel ordering scheme.
    template<typename t>
    CImg<T>& assign(const t *const values, const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c,
                    const char *const axes_order) {
      CImg<T>(values,size_x,size_y,size_z,size_c,axes_order).move_to(*this);
    }

    //! Construct image from reading an image file \inplace.
    /**
       In-place version of the constructor CImg(const char*).
    **/
    CImg<T>& assign(const char *const filename) {
      return load(filename);
    }

    //! Construct image copy \inplace.
    /**
       In-place version of the constructor CImg(const CImg<t>&).
    **/
    template<typename t>
    CImg<T>& assign(const CImg<t>& img) {
      return assign(img._data,img._width,img._height,img._depth,img._spectrum);
    }

    //! In-place version of the advanced copy constructor.
    /**
       In-place version of the constructor CImg(const CImg<t>&,bool).
     **/
    template<typename t>
    CImg<T>& assign(const CImg<t>& img, const bool is_shared) {
      return assign(img._data,img._width,img._height,img._depth,img._spectrum,is_shared);
    }

    //! Construct image from a display window \inplace.
    /**
       In-place version of the constructor CImg(const CImgDisplay&).
    **/
    CImg<T>& assign(const CImgDisplay &disp) {
      disp.snapshot(*this);
      return *this;
    }

    //! Construct empty image \inplace.
    /**
       Equivalent to assign().
       \note
       - It has been defined for compatibility with STL naming conventions.
    **/
    CImg<T>& clear() {
      return assign();
    }

    //! Transfer content of an image instance into another one.
    /**
       Transfer the dimensions and the pixel buffer content of an image instance into another one,
       and replace instance by an empty image. It avoids the copy of the pixel buffer
       when possible.
       \param img Destination image.
       \note
       - Pixel types \c T and \c t of source and destination images can be different, though the process is
         designed to be instantaneous when \c T and \c t are the same.
       \par Example
       \code
       CImg<float> src(256,256,1,3,0), // Construct a 256x256x1x3 (color) image filled with value '0'
                   dest(16,16); // Construct a 16x16x1x1 (scalar) image
       src.move_to(dest); // Now, 'src' is empty and 'dest' is the 256x256x1x3 image
       \endcode
    **/
    template<typename t>
    CImg<t>& move_to(CImg<t>& img) {
      img.assign(*this);
      assign();
      return img;
    }

    //! Transfer content of an image instance into another one \specialization.
    CImg<T>& move_to(CImg<T>& img) {
      if (_is_shared || img._is_shared) img.assign(*this);
      else swap(img);
      assign();
      return img;
    }

    //! Transfer content of an image instance into a new image in an image list.
    /**
       Transfer the dimensions and the pixel buffer content of an image instance
       into a newly inserted image at position \c pos in specified \c CImgList<t> instance.
       \param list Destination list.
       \param pos Position of the newly inserted image in the list.
       \note
       - When optional parameter \c pos is omitted, the image instance is transferred as a new
         image at the end of the specified \c list.
       - It is convenient to sequentially insert new images into image lists, with no
         additional copies of memory buffer.
       \par Example
       \code
       CImgList<float> list; // Construct an empty image list
       CImg<float> img("reference.jpg"); // Read image from filename
       img.move_to(list); // Transfer image content as a new item in the list (no buffer copy)
       \endcode
    **/
    template<typename t>
    CImgList<t>& move_to(CImgList<t>& list, const unsigned int pos=~0U) {
      const unsigned int npos = pos>list._width?list._width:pos;
      move_to(list.insert(1,npos)[npos]);
      return list;
    }

    //! Swap fields of two image instances.
    /**
      \param img Image to swap fields with.
      \note
      - It can be used to interchange the content of two images in a very fast way. Can be convenient when dealing
        with algorithms requiring two swapping buffers.
      \par Example
      \code
      CImg<float> img1("lena.jpg"),
                  img2("milla.jpg");
      img1.swap(img2); // Now, 'img1' is 'milla' and 'img2' is 'lena'
      \endcode
    **/
    CImg<T>& swap(CImg<T>& img) {
      cimg::swap(_width,img._width,_height,img._height,_depth,img._depth,_spectrum,img._spectrum);
      cimg::swap(_data,img._data);
      cimg::swap(_is_shared,img._is_shared);
      return img;
    }

    //! Return a reference to an empty image.
    /**
       \note
       This function is useful mainly to declare optional parameters having type \c CImg<T> in functions prototypes,
       e.g.
       \code
       void f(const int x=0, const int y=0, const CImg<float>& img=CImg<float>::empty());
       \endcode
     **/
    static CImg<T>& empty() {
      static CImg<T> _empty;
      return _empty.assign();
    }

    //! Return a reference to an empty image \const.
    static const CImg<T>& const_empty() {
      static const CImg<T> _empty;
      return _empty;
    }

    //@}

#endif
