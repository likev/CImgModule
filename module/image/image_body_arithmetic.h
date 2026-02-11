#ifndef CIMG_MODULE_IMAGE_BODY_ARITHMETIC_H
#define CIMG_MODULE_IMAGE_BODY_ARITHMETIC_H

    //------------------------------------------
    //
    //! \name Overloaded Operators
    //@{
    //------------------------------------------

    //! Access to a pixel value.
    /**
       Return a reference to a located pixel value of the image instance,
       being possibly \e const, whether the image instance is \e const or not.
       This is the standard method to get/set pixel values in \c CImg<T> images.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note
       - Range of pixel coordinates start from <tt>(0,0,0,0)</tt> to
         <tt>(width() - 1,height() - 1,depth() - 1,spectrum() - 1)</tt>.
       - Due to the particular arrangement of the pixel buffers defined in %CImg, you can omit one coordinate if the
         corresponding dimension is equal to \c 1.
         For instance, pixels of a 2D image (depth() equal to \c 1) can be accessed by <tt>img(x,y,c)</tt> instead of
         <tt>img(x,y,0,c)</tt>.
       \warning
       - There is \e no boundary checking done in this operator, to make it as fast as possible.
         You \e must take care of out-of-bounds access by yourself, if necessary.
         For debugging purposes, you may want to define macro \c 'cimg_verbosity'>=3 to enable additional boundary
         checking operations in this operator. In that case, warning messages will be printed on the error output
         when accessing out-of-bounds pixels.
       \par Example
       \code
       CImg<float> img(100,100,1,3,0); // Construct a 100x100x1x3 (color) image with pixels set to '0'
       const float
          valR = img(10,10,0,0), // Read red value at coordinates (10,10)
          valG = img(10,10,0,1), // Read green value at coordinates (10,10)
          valB = img(10,10,2), // Read blue value at coordinates (10,10) (Z-coordinate can be omitted)
          avg = (valR + valG + valB)/3; // Compute average pixel value
       img(10,10,0) = img(10,10,1) = img(10,10,2) = avg; // Replace the color pixel (10,10) by the average grey value
       \endcode
    **/
#if cimg_verbosity>=3
    T& operator()(const unsigned int x, const unsigned int y=0,
                  const unsigned int z=0, const unsigned int c=0) {
      const ulongT off = (ulongT)offset(x,y,z,c);
      if (!_data || off>=size()) {
        cimg::warn(_cimg_instance
                   "operator(): Invalid pixel request, at coordinates (%d,%d,%d,%d) [offset=%u].",
                   cimg_instance,
                   (int)x,(int)y,(int)z,(int)c,off);
        return *_data;
      }
      else return _data[off];
    }

    //! Access to a pixel value \const.
    const T& operator()(const unsigned int x, const unsigned int y=0,
                        const unsigned int z=0, const unsigned int c=0) const {
      return const_cast<CImg<T>*>(this)->operator()(x,y,z,c);
    }

    //! Access to a pixel value.
    /**
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param wh Precomputed offset, must be equal to <tt>width()*\ref height()</tt>.
       \param whd Precomputed offset, must be equal to <tt>width()*\ref height()*\ref depth()</tt>.
       \note
       - Similar to (but faster than) operator()().
         It uses precomputed offsets to optimize memory access. You may use it to optimize
         the reading/writing of several pixel values in the same image (e.g. in a loop).
     **/
    T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c,
                  const ulongT wh, const ulongT whd=0) {
      cimg::unused(wh,whd);
      return (*this)(x,y,z,c);
    }

    //! Access to a pixel value \const.
    const T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c,
                        const ulongT wh, const ulongT whd=0) const {
      cimg::unused(wh,whd);
      return (*this)(x,y,z,c);
    }
#else
    T& operator()(const unsigned int x) {
      return _data[x];
    }

    const T& operator()(const unsigned int x) const {
      return _data[x];
    }

    T& operator()(const unsigned int x, const unsigned int y) {
      return _data[x + y*_width];
    }

    const T& operator()(const unsigned int x, const unsigned int y) const {
      return _data[x + y*_width];
    }

    T& operator()(const unsigned int x, const unsigned int y, const unsigned int z) {
      return _data[x + y*(ulongT)_width + z*(ulongT)_width*_height];
   }

    const T& operator()(const unsigned int x, const unsigned int y, const unsigned int z) const {
      return _data[x + y*(ulongT)_width + z*(ulongT)_width*_height];
    }

    T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c) {
      return _data[x + y*(ulongT)_width + z*(ulongT)_width*_height + c*(ulongT)_width*_height*_depth];
    }

    const T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c) const {
      return _data[x + y*(ulongT)_width + z*(ulongT)_width*_height + c*(ulongT)_width*_height*_depth];
    }

    T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int,
                  const ulongT wh) {
      return _data[x + y*_width + z*wh];
    }

    const T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int,
                        const ulongT wh) const {
      return _data[x + y*_width + z*wh];
    }

    T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c,
                  const ulongT wh, const ulongT whd) {
      return _data[x + y*_width + z*wh + c*whd];
    }

    const T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c,
                        const ulongT wh, const ulongT whd) const {
      return _data[x + y*_width + z*wh + c*whd];
    }
#endif

    //! Implicitly cast an image into a \c T*.
    /**
       Implicitly cast a \c CImg<T> instance into a \c T* or \c const \c T* pointer, whether the image instance
       is \e const or not. The returned pointer points on the first value of the image pixel buffer.
       \note
       - It simply returns the pointer data() to the pixel buffer.
       - This implicit conversion is convenient to test the empty state of images (data() being \c 0 in this case), e.g.
       \code
       CImg<float> img1(100,100), img2; // 'img1' is a 100x100 image, 'img2' is an empty image
       if (img1) { // Test succeeds, 'img1' is not an empty image
         if (!img2) { // Test succeeds, 'img2' is an empty image
           std::printf("'img1' is not empty, 'img2' is empty.");
         }
       }
       \endcode
       - It also allows to use brackets to access pixel values, without need for a \c CImg<T>::operator[](), e.g.
       \code
       CImg<float> img(100,100);
       const float value = img[99]; // Access to value of the last pixel on the first row
       img[510] = 255; // Set pixel value at (10,5)
       \endcode
    **/
    operator T*() {
      return _data;
    }

    //! Implicitly cast an image into a \c T* \const.
    operator const T*() const {
      return _data;
    }

    //! Assign a value to all image pixels.
    /**
       Assign specified \c value to each pixel value of the image instance.
       \param value Value that will be assigned to image pixels.
       \note
       - The image size is never modified.
       - The \c value may be casted to pixel type \c T if necessary.
       \par Example
       \code
       CImg<char> img(100,100); // Declare image (with garbage values)
       img = 0; // Set all pixel values to '0'
       img = 1.2; // Set all pixel values to '1' (cast of '1.2' as a 'char')
       \endcode
    **/
    CImg<T>& operator=(const T& value) {
      return fill(value);
    }

    //! Assign pixels values from a specified expression.
    /**
       Initialize all pixel values from the specified string \c expression.
       \param expression Value string describing the way pixel values are set.
       \note
       - String parameter \c expression may describe different things:
         - If \c expression is a list of values (as in \c "1,2,3,8,3,2"), or a formula (as in \c "(x*y)%255"),
           the pixel values are set from specified \c expression and the image size is not modified.
         - If \c expression is a filename (as in \c "reference.jpg"), the corresponding image file is loaded and
           replace the image instance. The image size is modified if necessary.
       \par Example
       \code
       CImg<float> img1(100,100), img2(img1), img3(img1); // Declare 3 scalar images 100x100 with uninitialized values
       img1 = "0,50,100,150,200,250,200,150,100,50"; // Set pixel values of 'img1' from a value sequence
       img2 = "10*((x*y)%25)"; // Set pixel values of 'img2' from a formula
       img3 = "reference.jpg"; // Set pixel values of 'img3' from a file (image size is modified)
       (img1,img2,img3).display();
       \endcode
       \image html ref_operator_eq.jpg
    **/
    CImg<T>& operator=(const char *const expression) {
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      try {
        _fill(expression,true,3,(CImgList<T>*)0,"operator=",(CImg<T>*)0,(CImg<doubleT>*)0);
      } catch (CImgException&) {
        cimg::exception_mode(omode);
        load(expression);
      }
      cimg::exception_mode(omode);
      return *this;
    }

    //! Copy an image into the current image instance.
    /**
       Similar to the in-place copy constructor assign(const CImg<t>&).
    **/
    template<typename t>
    CImg<T>& operator=(const CImg<t>& img) {
      return assign(img);
    }

    //! Copy an image into the current image instance \specialization.
    CImg<T>& operator=(const CImg<T>& img) {
      return assign(img);
    }

    //! Copy the content of a display window to the current image instance.
    /**
       Similar to assign(const CImgDisplay&).
    **/
    CImg<T>& operator=(const CImgDisplay& disp) {
      disp.snapshot(*this);
      return *this;
    }

    //! In-place addition operator.
    /**
       Add specified \c value to all pixels of an image instance.
       \param value Value to add.
       \note
       - Resulting pixel values are casted to fit the pixel type \c T.
         For instance, adding \c 0.2 to a \c CImg<char> is possible but does nothing indeed.
       - Overflow values are treated as with standard C++ numeric types. For instance,
       \code
       CImg<unsigned char> img(100,100,1,1,255); // Construct a 100x100 image with pixel values '255'
       img+=1; // Add '1' to each pixels -> Overflow
       // here all pixels of image 'img' are equal to '0'.
       \endcode
       - To prevent value overflow, you may want to consider pixel type \c T as \c float or \c double,
         and use cut() after addition.
       \par Example
       \code
       CImg<unsigned char> img1("reference.jpg"); // Load a 8-bits RGB image (values in [0,255])
       CImg<float> img2(img1); // Construct a float-valued copy of 'img1'
       img2+=100; // Add '100' to pixel values -> goes out of [0,255] but no problems with floats
       img2.cut(0,255); // Cut values in [0,255] to fit the 'unsigned char' constraint
       img1 = img2; // Rewrite safe result in 'unsigned char' version 'img1'
       const CImg<unsigned char> img3 = (img1 + 100).cut(0,255); // Do the same in a more simple and elegant way
       (img1,img2,img3).display();
       \endcode
       \image html ref_operator_plus.jpg
     **/
    template<typename t>
    CImg<T>& operator+=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,*ptr + value,524288,T);
      return *this;
    }

    //! In-place addition operator.
    /**
       Add values to image pixels, according to the specified string \c expression.
       \param expression Value string describing the way pixel values are added.
       \note
       - Similar to operator=(const char*), except that it adds values to the pixels of the current image instance,
         instead of assigning them.
    **/
    CImg<T>& operator+=(const char *const expression) {
      return *this+=(+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator+=",this,(CImg<doubleT>*)0);
    }

    //! In-place addition operator.
    /**
       Add values to image pixels, according to the values of the input image \c img.
       \param img Input image to add.
       \note
       - The size of the image instance is never modified.
       - It is not mandatory that input image \c img has the same size as the image instance.
         If less values are available in \c img, then the values are added periodically. For instance, adding one
         WxH scalar image (spectrum() equal to \c 1) to one WxH color image (spectrum() equal to \c 3)
         means each color channel will be incremented with the same values at the same locations.
       \par Example
       \code
       CImg<float> img1("reference.jpg"); // Load a RGB color image (img1.spectrum()==3)
       // Construct a scalar shading (img2.spectrum()==1).
       const CImg<float> img2(img1.width(),img.height(),1,1,"255*(x/w)^2");
       img1+=img2; // Add shading to each channel of 'img1'
       img1.cut(0,255); // Prevent [0,255] overflow
       (img2,img1).display();
       \endcode
       \image html ref_operator_plus1.jpg
    **/
    template<typename t>
    CImg<T>& operator+=(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return *this+=+img;
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)(*ptrd + *(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)(*ptrd + *(ptrs++));
      }
      return *this;
    }

    //! In-place increment operator (prefix).
    /**
       Add \c 1 to all image pixels, and return a reference to the current incremented image instance.
       \note
       - Writing \c ++img is equivalent to \c img+=1.
     **/
    CImg<T>& operator++() {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,*ptr + 1,524288,T);
      return *this;
    }

    //! In-place increment operator (postfix).
    /**
       Add \c 1 to all image pixels, and return a new copy of the initial (pre-incremented) image instance.
       \note
       - Use the prefixed version operator++() if you don't need a copy of the initial
         (pre-incremented) image instance, since a useless image copy may be expensive in terms of memory usage.
     **/
    CImg<T> operator++(int) {
      const CImg<T> copy(*this,false);
      ++*this;
      return copy;
    }

    //! Return a non-shared copy of the image instance.
    /**
       \note
       - Use this operator to ensure you get a non-shared copy of an image instance with same pixel type \c T.
         Indeed, the usual copy constructor CImg<T>(const CImg<T>&) returns a shared copy of a shared input image,
         and it may be not desirable to work on a regular copy (e.g. for a resize operation) if you have no
         information about the shared state of the input image.
       - Writing \c (+img) is equivalent to \c CImg<T>(img,false).
    **/
    CImg<T> operator+() const {
      return CImg<T>(*this,false);
    }

    //! Addition operator.
    /**
       Similar to operator+=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
     **/
    template<typename t>
    CImg<_cimg_Tt> operator+(const t value) const {
      return CImg<_cimg_Tt>(*this,false)+=value;
    }

    //! Addition operator.
    /**
       Similar to operator+=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
     **/
    CImg<Tfloat> operator+(const char *const expression) const {
      return CImg<Tfloat>(*this,false)+=expression;
    }

    //! Addition operator.
    /**
       Similar to operator+=(const CImg<t>&), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
     **/
    template<typename t>
    CImg<_cimg_Tt> operator+(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false)+=img;
    }

    //! In-place subtraction operator.
    /**
       Similar to operator+=(const t), except that it performs a subtraction instead of an addition.
     **/
    template<typename t>
    CImg<T>& operator-=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,*ptr - value,524288,T);
      return *this;
    }

    //! In-place subtraction operator.
    /**
       Similar to operator+=(const char*), except that it performs a subtraction instead of an addition.
     **/
    CImg<T>& operator-=(const char *const expression) {
      return *this-=(+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator-=",this,(CImg<doubleT>*)0);
    }

    //! In-place subtraction operator.
    /**
       Similar to operator+=(const CImg<t>&), except that it performs a subtraction instead of an addition.
     **/
    template<typename t>
    CImg<T>& operator-=(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return *this-=+img;
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)(*ptrd - *(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)(*ptrd - *(ptrs++));
      }
      return *this;
    }

    //! In-place decrement operator (prefix).
    /**
       Similar to operator++(), except that it performs a decrement instead of an increment.
    **/
    CImg<T>& operator--() {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,*ptr - 1,524288,T);
      return *this;
    }

    //! In-place decrement operator (postfix).
    /**
       Similar to operator++(int), except that it performs a decrement instead of an increment.
    **/
    CImg<T> operator--(int) {
      const CImg<T> copy(*this,false);
      --*this;
      return copy;
    }

    //! Replace each pixel by its opposite value.
    /**
       \note
       - If the computed opposite values are out-of-range, they are treated as with standard C++ numeric types.
         For instance, the \c unsigned \c char opposite of \c 1 is \c 255.
       \par Example
       \code
       const CImg<unsigned char>
         img1("reference.jpg"), // Load a RGB color image
         img2 = -img1; // Compute its opposite (in 'unsigned char')
       (img1,img2).display();
       \endcode
       \image html ref_operator_minus.jpg
     **/
    CImg<T> operator-() const {
      return CImg<T>(_width,_height,_depth,_spectrum,(T)0)-=*this;
    }

    //! Subtraction operator.
    /**
       Similar to operator-=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    template<typename t>
    CImg<_cimg_Tt> operator-(const t value) const {
      return CImg<_cimg_Tt>(*this,false)-=value;
    }

    //! Subtraction operator.
    /**
       Similar to operator-=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    CImg<Tfloat> operator-(const char *const expression) const {
      return CImg<Tfloat>(*this,false)-=expression;
    }

    //! Subtraction operator.
    /**
       Similar to operator-=(const CImg<t>&), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    template<typename t>
    CImg<_cimg_Tt> operator-(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false)-=img;
    }

    //! In-place multiplication operator.
    /**
       Similar to operator+=(const t), except that it performs a multiplication instead of an addition.
     **/
    template<typename t>
    CImg<T>& operator*=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,*ptr * value,262144,T);
      return *this;
    }

    //! In-place multiplication operator.
    /**
       Similar to operator+=(const char*), except that it performs a multiplication instead of an addition.
     **/
    CImg<T>& operator*=(const char *const expression) {
      return mul((+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator*=",this,(CImg<doubleT>*)0));
    }

    //! In-place multiplication operator.
    /**
       Replace the image instance by the matrix multiplication between the image instance and the specified matrix
       \c img.
       \param img Second operand of the matrix multiplication.
       \note
       - It does \e not compute a pointwise multiplication between two images. For this purpose, use
         mul(const CImg<t>&) instead.
       - The size of the image instance can be modified by this operator.
       \par Example
       \code
       CImg<float> A(2,2,1,1, 1,2,3,4); // Construct 2x2 matrix A = [1,2;3,4]
       const CImg<float> X(1,2,1,1, 1,2); // Construct 1x2 vector X = [1;2]
       A*=X; // Assign matrix multiplication A*X to 'A'
       // 'A' is now a 1x2 vector whose values are [5;11].
       \endcode
    **/
    template<typename t>
    CImg<T>& operator*=(const CImg<t>& img) {
      return ((*this)*img).move_to(*this);
    }

    //! Multiplication operator.
    /**
       Similar to operator*=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    template<typename t>
    CImg<_cimg_Tt> operator*(const t value) const {
      return CImg<_cimg_Tt>(*this,false)*=value;
    }

    //! Multiplication operator.
    /**
       Similar to operator*=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    CImg<Tfloat> operator*(const char *const expression) const {
      return CImg<Tfloat>(*this,false)*=expression;
    }

    //! Multiplication operator.
    /**
       Similar to operator*=(const CImg<t>&), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    template<typename t>
    CImg<_cimg_Tt> operator*(const CImg<t>& img) const {
      typedef _cimg_Ttdouble Ttdouble;
      typedef _cimg_Tt Tt;
      if (_width!=img._height || _depth!=1 || _spectrum!=1 || img._depth!=1 || img._spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "operator*(): Invalid multiplication of instance by specified "
                                    "matrix (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    img._width,img._height,img._depth,img._spectrum,img._data);
      CImg<Tt> res(img._width,_height);

      // Check for common cases to optimize.
      if (img._width==1) { // Matrix * Vector
        if (_height==1) switch (_width) { // Vector^T * Vector
          case 1 :
            res[0] = (Tt)((Ttdouble)_data[0]*img[0]);
            return res;
          case 2 :
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1]);
            return res;
          case 3 :
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1] +
                          (Ttdouble)_data[2]*img[2]);
            return res;
          case 4 :
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1] +
                          (Ttdouble)_data[2]*img[2] + (Ttdouble)_data[3]*img[3]);
            return res;
          default : {
            Ttdouble val = 0;
            cimg_pragma_openmp(parallel for reduction(+:val) cimg_openmp_if_size(size(),4096))
            cimg_forX(*this,i) val+=(Ttdouble)_data[i]*img[i];
            res[0] = val;
            return res;
          }
          } else if (_height==_width) switch (_width) { // Square_matrix * Vector
          case 2 : // 2x2_matrix * Vector
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1]);
            res[1] = (Tt)((Ttdouble)_data[2]*img[0] + (Ttdouble)_data[3]*img[1]);
            return res;
          case 3 : // 3x3_matrix * Vector
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1] +
                          (Ttdouble)_data[2]*img[2]);
            res[1] = (Tt)((Ttdouble)_data[3]*img[0] + (Ttdouble)_data[4]*img[1] +
                          (Ttdouble)_data[5]*img[2]);
            res[2] = (Tt)((Ttdouble)_data[6]*img[0] + (Ttdouble)_data[7]*img[1] +
                          (Ttdouble)_data[8]*img[2]);
            return res;
          case 4 : // 4x4_matrix * Vector
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[1] +
                          (Ttdouble)_data[2]*img[2] + (Ttdouble)_data[3]*img[3]);
            res[1] = (Tt)((Ttdouble)_data[4]*img[0] + (Ttdouble)_data[5]*img[1] +
                          (Ttdouble)_data[6]*img[2] + (Ttdouble)_data[7]*img[3]);
            res[2] = (Tt)((Ttdouble)_data[8]*img[0] + (Ttdouble)_data[9]*img[1] +
                          (Ttdouble)_data[10]*img[2] + (Ttdouble)_data[11]*img[3]);
            res[3] = (Tt)((Ttdouble)_data[12]*img[0] + (Ttdouble)_data[13]*img[1] +
                          (Ttdouble)_data[14]*img[2] + (Ttdouble)_data[15]*img[3]);
            return res;
          }
      } else if (_height==_width) {
        if (img._height==img._width) switch (_width) { // Square_matrix * Square_matrix
          case 2 : // 2x2_matrix * 2x2_matrix
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[2]);
            res[1] = (Tt)((Ttdouble)_data[0]*img[1] + (Ttdouble)_data[1]*img[3]);
            res[2] = (Tt)((Ttdouble)_data[2]*img[0] + (Ttdouble)_data[3]*img[2]);
            res[3] = (Tt)((Ttdouble)_data[2]*img[1] + (Ttdouble)_data[3]*img[3]);
            return res;
          case 3 : // 3x3_matrix * 3x3_matrix
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[3] +
                          (Ttdouble)_data[2]*img[6]);
            res[1] = (Tt)((Ttdouble)_data[0]*img[1] + (Ttdouble)_data[1]*img[4] +
                          (Ttdouble)_data[2]*img[7]);
            res[2] = (Tt)((Ttdouble)_data[0]*img[2] + (Ttdouble)_data[1]*img[5] +
                          (Ttdouble)_data[2]*img[8]);
            res[3] = (Tt)((Ttdouble)_data[3]*img[0] + (Ttdouble)_data[4]*img[3] +
                          (Ttdouble)_data[5]*img[6]);
            res[4] = (Tt)((Ttdouble)_data[3]*img[1] + (Ttdouble)_data[4]*img[4] +
                          (Ttdouble)_data[5]*img[7]);
            res[5] = (Tt)((Ttdouble)_data[3]*img[2] + (Ttdouble)_data[4]*img[5] +
                          (Ttdouble)_data[5]*img[8]);
            res[6] = (Tt)((Ttdouble)_data[6]*img[0] + (Ttdouble)_data[7]*img[3] +
                          (Ttdouble)_data[8]*img[6]);
            res[7] = (Tt)((Ttdouble)_data[6]*img[1] + (Ttdouble)_data[7]*img[4] +
                          (Ttdouble)_data[8]*img[7]);
            res[8] = (Tt)((Ttdouble)_data[6]*img[2] + (Ttdouble)_data[7]*img[5] +
                          (Ttdouble)_data[8]*img[8]);
            return res;
          case 4 : // 4x4_matrix * 4x4_matrix
            res[0] = (Tt)((Ttdouble)_data[0]*img[0] + (Ttdouble)_data[1]*img[4] +
                          (Ttdouble)_data[2]*img[8] + (Ttdouble)_data[3]*img[12]);
            res[1] = (Tt)((Ttdouble)_data[0]*img[1] + (Ttdouble)_data[1]*img[5] +
                          (Ttdouble)_data[2]*img[9] + (Ttdouble)_data[3]*img[13]);
            res[2] = (Tt)((Ttdouble)_data[0]*img[2] + (Ttdouble)_data[1]*img[6] +
                          (Ttdouble)_data[2]*img[10] + (Ttdouble)_data[3]*img[14]);
            res[3] = (Tt)((Ttdouble)_data[0]*img[3] + (Ttdouble)_data[1]*img[7] +
                          (Ttdouble)_data[2]*img[11] + (Ttdouble)_data[3]*img[15]);
            res[4] = (Tt)((Ttdouble)_data[4]*img[0] + (Ttdouble)_data[5]*img[4] +
                          (Ttdouble)_data[6]*img[8] + (Ttdouble)_data[7]*img[12]);
            res[5] = (Tt)((Ttdouble)_data[4]*img[1] + (Ttdouble)_data[5]*img[5] +
                          (Ttdouble)_data[6]*img[9] + (Ttdouble)_data[7]*img[13]);
            res[6] = (Tt)((Ttdouble)_data[4]*img[2] + (Ttdouble)_data[5]*img[6] +
                          (Ttdouble)_data[6]*img[10] + (Ttdouble)_data[7]*img[14]);
            res[7] = (Tt)((Ttdouble)_data[4]*img[3] + (Ttdouble)_data[5]*img[7] +
                          (Ttdouble)_data[6]*img[11] + (Ttdouble)_data[7]*img[15]);
            res[8] = (Tt)((Ttdouble)_data[8]*img[0] + (Ttdouble)_data[9]*img[4] +
                          (Ttdouble)_data[10]*img[8] + (Ttdouble)_data[11]*img[12]);
            res[9] = (Tt)((Ttdouble)_data[8]*img[1] + (Ttdouble)_data[9]*img[5] +
                          (Ttdouble)_data[10]*img[9] + (Ttdouble)_data[11]*img[13]);
            res[10] = (Tt)((Ttdouble)_data[8]*img[2] + (Ttdouble)_data[9]*img[6] +
                           (Ttdouble)_data[10]*img[10] + (Ttdouble)_data[11]*img[14]);
            res[11] = (Tt)((Ttdouble)_data[8]*img[3] + (Ttdouble)_data[9]*img[7] +
                           (Ttdouble)_data[10]*img[11] + (Ttdouble)_data[11]*img[15]);
            res[12] = (Tt)((Ttdouble)_data[12]*img[0] + (Ttdouble)_data[13]*img[4] +
                           (Ttdouble)_data[14]*img[8] + (Ttdouble)_data[15]*img[12]);
            res[13] = (Tt)((Ttdouble)_data[12]*img[1] + (Ttdouble)_data[13]*img[5] +
                           (Ttdouble)_data[14]*img[9] + (Ttdouble)_data[15]*img[13]);
            res[14] = (Tt)((Ttdouble)_data[12]*img[2] + (Ttdouble)_data[13]*img[6] +
                           (Ttdouble)_data[14]*img[10] + (Ttdouble)_data[15]*img[14]);
            res[15] = (Tt)((Ttdouble)_data[12]*img[3] + (Ttdouble)_data[13]*img[7] +
                           (Ttdouble)_data[14]*img[11] + (Ttdouble)_data[15]*img[15]);
            return res;
          } else switch (_width) { // Square_matrix * Matrix
          case 2 : { // 2x2_matrix * Matrix
            const t *const ps0 = img.data(), *const ps1 = img.data(0,1);
            Tt *const pd0 = res.data(), *const pd1 = res.data(0,1);
            const Ttdouble
              a0 = (Ttdouble)_data[0], a1 = (Ttdouble)_data[1],
              a2 = (Ttdouble)_data[2], a3 = (Ttdouble)_data[3];
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(img.width(),4096))
            cimg_forX(img,i) {
              const Ttdouble x = (Ttdouble)ps0[i], y = (Ttdouble)ps1[i];
              pd0[i] = (Tt)(a0*x + a1*y);
              pd1[i] = (Tt)(a2*x + a3*y);
            }
            return res;
          }
          case 3 : { // 3x3_matrix * Matrix
            const t *const ps0 = img.data(), *const ps1 = img.data(0,1), *const ps2 = img.data(0,2);
            Tt *const pd0 = res.data(), *const pd1 = res.data(0,1), *const pd2 = res.data(0,2);
            const Ttdouble
              a0 = (Ttdouble)_data[0], a1 = (Ttdouble)_data[1], a2 = (Ttdouble)_data[2],
              a3 = (Ttdouble)_data[3], a4 = (Ttdouble)_data[4], a5 = (Ttdouble)_data[5],
              a6 = (Ttdouble)_data[6], a7 = (Ttdouble)_data[7], a8 = (Ttdouble)_data[8];
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(img.width(),1024))
            cimg_forX(img,i) {
              const Ttdouble x = (Ttdouble)ps0[i], y = (Ttdouble)ps1[i], z = (Ttdouble)ps2[i];
              pd0[i] = (Tt)(a0*x + a1*y + a2*z);
              pd1[i] = (Tt)(a3*x + a4*y + a5*z);
              pd2[i] = (Tt)(a6*x + a7*y + a8*z);
            }
            return res;
          }
          case 4 : { // 4x4_matrix * Matrix
            const t
              *const ps0 = img.data(), *const ps1 = img.data(0,1),
              *const ps2 = img.data(0,2), *const ps3 = img.data(0,3);
            Tt
              *const pd0 = res.data(), *const pd1 = res.data(0,1),
              *const pd2 = res.data(0,2), *const pd3 = res.data(0,3);
            const Ttdouble
              a0 = (Ttdouble)_data[0], a1 = (Ttdouble)_data[1], a2 = (Ttdouble)_data[2], a3 = (Ttdouble)_data[3],
              a4 = (Ttdouble)_data[4], a5 = (Ttdouble)_data[5], a6 = (Ttdouble)_data[6], a7 = (Ttdouble)_data[7],
              a8 = (Ttdouble)_data[8], a9 = (Ttdouble)_data[9], a10 = (Ttdouble)_data[10], a11 = (Ttdouble)_data[11],
              a12 = (Ttdouble)_data[12], a13 = (Ttdouble)_data[13], a14 = (Ttdouble)_data[14],
              a15 = (Ttdouble)_data[15];
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(img.width(),512))
            cimg_forX(img,i) {
              const Ttdouble x = (Ttdouble)ps0[i], y = (Ttdouble)ps1[i], z = (Ttdouble)ps2[i], c = (Ttdouble)ps3[i];
              pd0[i] = (Tt)(a0*x + a1*y + a2*z + a3*c);
              pd1[i] = (Tt)(a4*x + a5*y + a6*z + a7*c);
              pd2[i] = (Tt)(a8*x + a9*y + a10*z + a11*c);
              pd3[i] = (Tt)(a12*x + a13*y + a14*z + a15*c);
            }
            return res;
          }
          }
      }

      // Fallback to generic version.
#if cimg_use_openmp!=0
      cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                         cimg_openmp_if(size()>(cimg_openmp_sizefactor)*1024 &&
                                        img.size()>(cimg_openmp_sizefactor)*1024))
      cimg_forXY(res,i,j) {
        Ttdouble value = 0;
        cimg_forX(*this,k) value+=(*this)(k,j)*img(i,k);
        res(i,j) = (Tt)value;
      }
#else
      Tt *ptrd = res._data;
      cimg_forXY(res,i,j) {
        Ttdouble value = 0;
        cimg_forX(*this,k) value+=(*this)(k,j)*img(i,k);
        *(ptrd++) = (Tt)value;
      }
#endif
      return res;
    }

    //! In-place division operator.
    /**
       Similar to operator+=(const t), except that it performs a division instead of an addition.
     **/
    template<typename t>
    CImg<T>& operator/=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,*ptr / value,32768,T);
      return *this;
    }

    //! In-place division operator.
    /**
       Similar to operator+=(const char*), except that it performs a division instead of an addition.
     **/
    CImg<T>& operator/=(const char *const expression) {
      return div((+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator/=",this,(CImg<doubleT>*)0));
    }

    //! In-place division operator.
    /**
       Replace the image instance by the (right) matrix division between the image instance and the specified
       matrix \c img.
       \param img Second operand of the matrix division.
       \note
       - It does \e not compute a pointwise division between two images. For this purpose, use
         div(const CImg<t>&) instead.
       - It returns the matrix operation \c A*inverse(img).
       - The size of the image instance can be modified by this operator.
     **/
    template<typename t>
    CImg<T>& operator/=(const CImg<t>& img) {
      return (*this*img.get_invert()).move_to(*this);
    }

    //! Division operator.
    /**
       Similar to operator/=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    template<typename t>
    CImg<_cimg_Tt> operator/(const t value) const {
      return CImg<_cimg_Tt>(*this,false)/=value;
    }

    //! Division operator.
    /**
       Similar to operator/=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    CImg<Tfloat> operator/(const char *const expression) const {
      return CImg<Tfloat>(*this,false)/=expression;
    }

    //! Division operator.
    /**
       Similar to operator/=(const CImg<t>&), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    template<typename t>
    CImg<_cimg_Tt> operator/(const CImg<t>& img) const {
      return (*this)*img.get_invert();
    }

    //! In-place modulo operator.
    /**
       Similar to operator+=(const t), except that it performs a modulo operation instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator%=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,cimg::mod(*ptr,(T)value),16384,T);
      return *this;
    }

    //! In-place modulo operator.
    /**
       Similar to operator+=(const char*), except that it performs a modulo operation instead of an addition.
    **/
    CImg<T>& operator%=(const char *const expression) {
      return *this%=(+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator%=",this,(CImg<doubleT>*)0);
    }

    //! In-place modulo operator.
    /**
       Similar to operator+=(const CImg<t>&), except that it performs a modulo operation instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator%=(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return *this%=+img;
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = cimg::mod(*ptrd,(T)*(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = cimg::mod(*ptrd,(T)*(ptrs++));
      }
      return *this;
    }

    //! Modulo operator.
    /**
       Similar to operator%=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    template<typename t>
    CImg<_cimg_Tt> operator%(const t value) const {
      return CImg<_cimg_Tt>(*this,false)%=value;
    }

    //! Modulo operator.
    /**
       Similar to operator%=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    CImg<Tfloat> operator%(const char *const expression) const {
      return CImg<Tfloat>(*this,false)%=expression;
    }

    //! Modulo operator.
    /**
       Similar to operator%=(const CImg<t>&), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image may be a superset of the initial pixel type \c T, if necessary.
    **/
    template<typename t>
    CImg<_cimg_Tt> operator%(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false)%=img;
    }

    //! In-place bitwise AND operator.
    /**
       Similar to operator+=(const t), except that it performs a bitwise AND operation instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator&=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,(longT)*ptr & (longT)value,32768,T);
      return *this;
    }

    //! In-place bitwise AND operator.
    /**
       Similar to operator+=(const char*), except that it performs a bitwise AND operation instead of an addition.
    **/
    CImg<T>& operator&=(const char *const expression) {
      return *this&=(+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator&=",this,(CImg<doubleT>*)0);
    }

    //! In-place bitwise AND operator.
    /**
       Similar to operator+=(const CImg<t>&), except that it performs a bitwise AND operation instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator&=(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return *this&=+img;
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)((longT)*ptrd & (longT)*(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)((longT)*ptrd & (longT)*(ptrs++));
      }
      return *this;
    }

    //! Bitwise AND operator.
    /**
       Similar to operator&=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator&(const t value) const {
      return (+*this)&=value;
    }

    //! Bitwise AND operator.
    /**
       Similar to operator&=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    CImg<T> operator&(const char *const expression) const {
      return (+*this)&=expression;
    }

    //! Bitwise AND operator.
    /**
       Similar to operator&=(const CImg<t>&), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator&(const CImg<t>& img) const {
      return (+*this)&=img;
    }

    //! In-place bitwise OR operator.
    /**
       Similar to operator+=(const t), except that it performs a bitwise OR operation instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator|=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,(longT)*ptr | (longT)value,32768,T);
      return *this;
    }

    //! In-place bitwise OR operator.
    /**
       Similar to operator+=(const char*), except that it performs a bitwise OR operation instead of an addition.
    **/
    CImg<T>& operator|=(const char *const expression) {
      return *this|=(+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator|=",this,(CImg<doubleT>*)0);
    }

    //! In-place bitwise OR operator.
    /**
       Similar to operator+=(const CImg<t>&), except that it performs a bitwise OR operation instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator|=(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return *this|=+img;
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)((longT)*ptrd | (longT)*(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)((longT)*ptrd | (longT)*(ptrs++));
      }
      return *this;
    }

    //! Bitwise OR operator.
    /**
       Similar to operator|=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator|(const t value) const {
      return (+*this)|=value;
    }

    //! Bitwise OR operator.
    /**
       Similar to operator|=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    CImg<T> operator|(const char *const expression) const {
      return (+*this)|=expression;
    }

    //! Bitwise OR operator.
    /**
       Similar to operator|=(const CImg<t>&), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator|(const CImg<t>& img) const {
      return (+*this)|=img;
    }

    //! In-place bitwise XOR operator.
    /**
       Similar to operator+=(const t), except that it performs a bitwise XOR operation instead of an addition.
       \warning
       - It does \e not compute the \e power of pixel values. For this purpose, use pow(const t) instead.
    **/
    template<typename t>
    CImg<T>& operator^=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,(longT)*ptr ^ (longT)value,32768,T);
      return *this;
    }

    //! In-place bitwise XOR operator.
    /**
       Similar to operator+=(const char*), except that it performs a bitwise XOR operation instead of an addition.
       \warning
       - It does \e not compute the \e power of pixel values. For this purpose, use pow(const char*) instead.
    **/
    CImg<T>& operator^=(const char *const expression) {
      return *this^=(+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator^=",this,(CImg<doubleT>*)0);
    }

    //! In-place bitwise XOR operator.
    /**
       Similar to operator+=(const CImg<t>&), except that it performs a bitwise XOR operation instead of an addition.
       \warning
       - It does \e not compute the \e power of pixel values. For this purpose, use pow(const CImg<t>&) instead.
    **/
    template<typename t>
    CImg<T>& operator^=(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return *this^=+img;
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)((longT)*ptrd ^ (longT)*(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)((longT)*ptrd ^ (longT)*(ptrs++));
      }
      return *this;
    }

    //! Bitwise XOR operator.
    /**
       Similar to operator^=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator^(const t value) const {
      return (+*this)^=value;
    }

    //! Bitwise XOR operator.
    /**
       Similar to operator^=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    CImg<T> operator^(const char *const expression) const {
      return (+*this)^=expression;
    }

    //! Bitwise XOR operator.
    /**
       Similar to operator^=(const CImg<t>&), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator^(const CImg<t>& img) const {
      return (+*this)^=img;
    }

    //! In-place bitwise left shift operator.
    /**
       Similar to operator+=(const t), except that it performs a bitwise left shift instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator<<=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,((longT)*ptr) << (int)value,65536,T);
      return *this;
    }

    //! In-place bitwise left shift operator.
    /**
       Similar to operator+=(const char*), except that it performs a bitwise left shift instead of an addition.
    **/
    CImg<T>& operator<<=(const char *const expression) {
      return *this<<=(+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator<<=",this,(CImg<doubleT>*)0);
    }

    //! In-place bitwise left shift operator.
    /**
       Similar to operator+=(const CImg<t>&), except that it performs a bitwise left shift instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator<<=(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return *this^=+img;
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)((longT)*ptrd << (int)*(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)((longT)*ptrd << (int)*(ptrs++));
      }
      return *this;
    }

    //! Bitwise left shift operator.
    /**
       Similar to operator<<=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator<<(const t value) const {
      return (+*this)<<=value;
    }

    //! Bitwise left shift operator.
    /**
       Similar to operator<<=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    CImg<T> operator<<(const char *const expression) const {
      return (+*this)<<=expression;
    }

    //! Bitwise left shift operator.
    /**
       Similar to operator<<=(const CImg<t>&), except that it returns a new image instance instead of
       operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator<<(const CImg<t>& img) const {
      return (+*this)<<=img;
    }

    //! In-place bitwise right shift operator.
    /**
       Similar to operator+=(const t), except that it performs a bitwise right shift instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator>>=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,((longT)*ptr) >> (int)value,65536,T);
      return *this;
    }

    //! In-place bitwise right shift operator.
    /**
       Similar to operator+=(const char*), except that it performs a bitwise right shift instead of an addition.
    **/
    CImg<T>& operator>>=(const char *const expression) {
      return *this>>=(+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator>>=",this,(CImg<doubleT>*)0);
    }

    //! In-place bitwise right shift operator.
    /**
       Similar to operator+=(const CImg<t>&), except that it performs a bitwise right shift instead of an addition.
    **/
    template<typename t>
    CImg<T>& operator>>=(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return *this^=+img;
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)((longT)*ptrd >> (int)*(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)((longT)*ptrd >> (int)*(ptrs++));
      }
      return *this;
    }

    //! Bitwise right shift operator.
    /**
       Similar to operator>>=(const t), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator>>(const t value) const {
      return (+*this)>>=value;
    }

    //! Bitwise right shift operator.
    /**
       Similar to operator>>=(const char*), except that it returns a new image instance instead of operating in-place.
       The pixel type of the returned image is \c T.
    **/
    CImg<T> operator>>(const char *const expression) const {
      return (+*this)>>=expression;
    }

    //! Bitwise right shift operator.
    /**
       Similar to operator>>=(const CImg<t>&), except that it returns a new image instance instead of
       operating in-place.
       The pixel type of the returned image is \c T.
    **/
    template<typename t>
    CImg<T> operator>>(const CImg<t>& img) const {
      return (+*this)>>=img;
    }

    //! Bitwise inversion operator.
    /**
       Similar to operator-(), except that it compute the bitwise inverse instead of the opposite value.
    **/
    CImg<T> operator~() const {
      CImg<T> res(_width,_height,_depth,_spectrum);
      const T *ptrs = _data;
      cimg_for(res,ptrd,T) { const ulongT value = (ulongT)*(ptrs++); *ptrd = (T)~value; }
      return res;
    }

    //! Test if all pixels of an image have the same value.
    /**
       Return \c true is all pixels of the image instance are equal to the specified \c value.
       \param value Reference value to compare with.
    **/
    template<typename t>
    bool operator==(const t value) const {
      if (is_empty()) return false;
      typedef _cimg_Tt Tt;
      bool is_equal = true;
      for (T *ptrd = _data + size(); is_equal && ptrd>_data; is_equal = ((Tt)*(--ptrd)==(Tt)value)) {}
      return is_equal;
    }

    //! Test if all pixel values of an image follow a specified expression.
    /**
       Return \c true is all pixels of the image instance are equal to the specified \c expression.
       \param expression Value string describing the way pixel values are compared.
    **/
    bool operator==(const char *const expression) const {
      return *this==(+*this)._fill(expression,true,3,(CImgList<T>*)0,"operator==",this,(CImg<doubleT>*)0);
    }

    //! Test if two images have the same size and values.
    /**
       Return \c true if the image instance and the input image \c img have the same pixel values,
       even if the dimensions of the two images do not match. It returns \c false otherwise.
       \param img Input image to compare with.
       \note
       - The pixel buffer pointers data() of the two compared images do not have to be the same for operator==()
         to return \c true.
         Only the dimensions and the pixel values matter. Thus, the comparison can be \c true even for different
         pixel types \c T and \c t.
       \par Example
       \code
       const CImg<float> img1(1,3,1,1, 0,1,2); // Construct a 1x3 vector [0;1;2] (with 'float' pixel values)
       const CImg<char> img2(1,3,1,1, 0,1,2); // Construct a 1x3 vector [0;1;2] (with 'char' pixel values)
       if (img1==img2) { // Test succeeds, image dimensions and values are the same
         std::printf("'img1' and 'img2' have same dimensions and values.");
       }
       \endcode
    **/
    template<typename t>
    bool operator==(const CImg<t>& img) const {
      typedef _cimg_Tt Tt;
      const ulongT siz = size();
      bool is_equal = true;
      if (siz!=img.size()) return false;
      t *ptrs = img._data + siz;
      for (T *ptrd = _data + siz; is_equal && ptrd>_data; is_equal = ((Tt)*(--ptrd)==(Tt)*(--ptrs))) {}
      return is_equal;
    }

    //! Test if pixels of an image are all different from a value.
    /**
       Return \c true is all pixels of the image instance are different than the specified \c value.
       \param value Reference value to compare with.
    **/
    template<typename t>
    bool operator!=(const t value) const {
      return !((*this)==value);
    }

    //! Test if all pixel values of an image are different from a specified expression.
    /**
       Return \c true is all pixels of the image instance are different to the specified \c expression.
       \param expression Value string describing the way pixel values are compared.
    **/
    bool operator!=(const char *const expression) const {
      return !((*this)==expression);
    }

    //! Test if two images have different sizes or values.
    /**
       Return \c true if the image instance and the input image \c img have different dimensions or pixel values,
       and \c false otherwise.
       \param img Input image to compare with.
       \note
       - Writing \c img1!=img2 is equivalent to \c !(img1==img2).
    **/
    template<typename t>
    bool operator!=(const CImg<t>& img) const {
      return !((*this)==img);
    }

    //! Construct an image list from two images.
    /**
       Return a new list of image (\c CImgList instance) containing exactly two elements:
         - A copy of the image instance, at position [\c 0].
         - A copy of the specified image \c img, at position [\c 1].

       \param img Input image that will be the second image of the resulting list.
       \note
       - The family of operator,() is convenient to easily create list of images, but it is also \e quite \e slow
         in practice (see warning below).
       - Constructed lists contain no shared images. If image instance or input image \c img are shared, they are
         inserted as new non-shared copies in the resulting list.
       - The pixel type of the returned list may be a superset of the initial pixel type \c T, if necessary.
       \warning
       - Pipelining operator,() \c N times will perform \c N copies of the entire content of a (growing) image list.
         This may become very expensive in terms of speed and used memory. You should avoid using this technique to
         build a new CImgList instance from several images, if you are seeking for performance.
         Fast insertions of images in an image list are possible with
         CImgList<T>::insert(const CImg<t>&,unsigned int,bool) or move_to(CImgList<t>&,unsigned int).
       \par Example
       \code
       const CImg<float>
          img1("reference.jpg"),
          img2 = img1.get_mirror('x'),
          img3 = img2.get_blur(5);
       const CImgList<float> list = (img1,img2); // Create list of two elements from 'img1' and 'img2'
       (list,img3).display(); // Display image list containing copies of 'img1','img2' and 'img3'
       \endcode
       \image html ref_operator_comma.jpg
    **/
    template<typename t>
    CImgList<_cimg_Tt> operator,(const CImg<t>& img) const {
      return CImgList<_cimg_Tt>(*this,img);
    }

    //! Construct an image list from image instance and an input image list.
    /**
       Return a new list of images (\c CImgList instance) containing exactly \c list.size() \c + \c 1 elements:
         - A copy of the image instance, at position [\c 0].
         - A copy of the specified image list \c list, from positions [\c 1] to [\c list.size()].

       \param list Input image list that will be appended to the image instance.
       \note
       - Similar to operator,(const CImg<t>&) const, except that it takes an image list as an argument.
    **/
    template<typename t>
    CImgList<_cimg_Tt> operator,(const CImgList<t>& list) const {
      return CImgList<_cimg_Tt>(list,false).insert(*this,0);
    }

    //! Split image along specified axis.
    /**
       Return a new list of images (\c CImgList instance) containing the split components
       of the instance image along the specified axis.
       \param axis Splitting axis (can be '\c x','\c y','\c z' or '\c c')
       \note
       - Similar to get_split(char,int) const, with default second argument.
       \par Example
       \code
       const CImg<unsigned char> img("reference.jpg"); // Load a RGB color image
       const CImgList<unsigned char> list = (img<'c'); // Get a list of its three R,G,B channels
       (img,list).display();
       \endcode
       \image html ref_operator_less.jpg
    **/
    CImgList<T> operator<(const char axis) const {
      return get_split(axis);
    }

    //@}


    //! In-place pointwise multiplication.
    /**
       Compute the pointwise multiplication between the image instance and the specified input image \c img.
       \param img Input image, as the second operand of the multiplication.
       \note
       - Similar to operator+=(const CImg<t>&), except that it performs a pointwise multiplication
         instead of an addition.
       - It does \e not perform a \e matrix multiplication. For this purpose, use operator*=(const CImg<t>&) instead.
       \par Example
       \code
       CImg<float>
         img("reference.jpg"),
         shade(img.width,img.height(),1,1,"-(x-w/2)^2-(y-h/2)^2",false);
       shade.normalize(0,1);
       (img,shade,img.get_mul(shade)).display();
       \endcode
    **/
    template<typename t>
    CImg<T>& mul(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return mul(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)(*ptrd * *(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)(*ptrd * *(ptrs++));
      }
      return *this;
    }

    //! In-place pointwise multiplication \newinstance.
    template<typename t>
    CImg<_cimg_Tt> get_mul(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).mul(img);
    }

    //! In-place pointwise division.
    /**
       Similar to mul(const CImg<t>&), except that it performs a pointwise division instead of a multiplication.
    **/
    template<typename t>
    CImg<T>& div(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return div(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)(*ptrd / *(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)(*ptrd / *(ptrs++));
      }
      return *this;
    }

    //! In-place pointwise division \newinstance.
    template<typename t>
    CImg<_cimg_Tt> get_div(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).div(img);
    }

    //! Raise each pixel value to a specified power.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its power \f$I_{(x,y,z,c)}^p\f$.
       \param p Exponent value.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
       \par Example
       \code
       const CImg<float>
         img0("reference.jpg"), // Load reference color image
         img1 = (img0/255).pow(1.8)*=255, // Compute gamma correction, with gamma = 1.8
         img2 = (img0/255).pow(0.5)*=255; // Compute gamma correction, with gamma = 0.5
       (img0,img1,img2).display();
       \endcode
    **/
    CImg<T>& pow(const double p) {
      if (is_empty()) return *this;
      if (p==-4) { cimg_openmp_for(*this,1/(Tfloat)cimg::pow4(*ptr),32768,T); return *this; }
      if (p==-3) { cimg_openmp_for(*this,1/(Tfloat)cimg::pow3(*ptr),32768,T); return *this; }
      if (p==-2) { cimg_openmp_for(*this,1/(Tfloat)cimg::sqr(*ptr),32768,T); return *this; }
      if (p==-1) { cimg_openmp_for(*this,1/(Tfloat)*ptr,32768,T); return *this; }
      if (p==-0.5) { cimg_openmp_for(*this,1/std::sqrt((Tfloat)*ptr),8192,T); return *this; }
      if (p==0) return fill((T)1);
      if (p==0.5) return sqrt();
      if (p==1) return *this;
      if (p==2) return sqr();
      if (p==3) { cimg_openmp_for(*this,cimg::pow3(*ptr),262144,T); return *this; }
      if (p==4) { cimg_openmp_for(*this,cimg::pow4(*ptr),131072,T); return *this; }
      cimg_openmp_for(*this,std::pow((Tfloat)*ptr,(Tfloat)p),1024,T);
      return *this;
    }

    //! Raise each pixel value to a specified power \newinstance.
    CImg<Tfloat> get_pow(const double p) const {
      return CImg<Tfloat>(*this,false).pow(p);
    }

    //! Raise each pixel value to a power, specified from an expression.
    /**
       Similar to operator+=(const char*), except it performs a pointwise exponentiation instead of an addition.
    **/
    CImg<T>& pow(const char *const expression) {
      return pow((+*this)._fill(expression,true,3,(CImgList<T>*)0,"pow",this,(CImg<doubleT>*)0));
    }

    //! Raise each pixel value to a power, specified from an expression \newinstance.
    CImg<Tfloat> get_pow(const char *const expression) const {
      return CImg<Tfloat>(*this,false).pow(expression);
    }

    //! Raise each pixel value to a power, pointwisely specified from another image.
    /**
       Similar to operator+=(const CImg<t>& img), except that it performs an exponentiation instead of an addition.
    **/
    template<typename t>
    CImg<T>& pow(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return pow(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)std::pow((double)*ptrd,(double)(*(ptrs++)));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)std::pow((double)*ptrd,(double)(*(ptrs++)));
      }
      return *this;
    }

    //! Raise each pixel value to a power, pointwisely specified from another image \newinstance.
    template<typename t>
    CImg<Tfloat> get_pow(const CImg<t>& img) const {
      return CImg<Tfloat>(*this,false).pow(img);
    }

    //! Compute the bitwise left rotation of each pixel value.
    /**
       Similar to operator<<=(unsigned int), except that it performs a left rotation instead of a left shift.
    **/
    CImg<T>& rol(const unsigned int n=1) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,cimg::rol(*ptr,n),32768,T);
      return *this;
    }

    //! Compute the bitwise left rotation of each pixel value \newinstance.
    CImg<T> get_rol(const unsigned int n=1) const {
      return (+*this).rol(n);
    }

    //! Compute the bitwise left rotation of each pixel value.
    /**
       Similar to operator<<=(const char*), except that it performs a left rotation instead of a left shift.
    **/
    CImg<T>& rol(const char *const expression) {
      return rol((+*this)._fill(expression,true,3,(CImgList<T>*)0,"rol",this,(CImg<doubleT>*)0));
    }

    //! Compute the bitwise left rotation of each pixel value \newinstance.
    CImg<T> get_rol(const char *const expression) const {
      return (+*this).rol(expression);
    }

    //! Compute the bitwise left rotation of each pixel value.
    /**
       Similar to operator<<=(const CImg<t>&), except that it performs a left rotation instead of a left shift.
    **/
    template<typename t>
    CImg<T>& rol(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return rol(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)cimg::rol(*ptrd,(unsigned int)(*(ptrs++)));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)cimg::rol(*ptrd,(unsigned int)(*(ptrs++)));
      }
      return *this;
    }

    //! Compute the bitwise left rotation of each pixel value \newinstance.
    template<typename t>
    CImg<T> get_rol(const CImg<t>& img) const {
      return (+*this).rol(img);
    }

    //! Compute the bitwise right rotation of each pixel value.
    /**
       Similar to operator>>=(unsigned int), except that it performs a right rotation instead of a right shift.
    **/
    CImg<T>& ror(const unsigned int n=1) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,cimg::ror(*ptr,n),32768,T);
      return *this;
    }

    //! Compute the bitwise right rotation of each pixel value \newinstance.
    CImg<T> get_ror(const unsigned int n=1) const {
      return (+*this).ror(n);
    }

    //! Compute the bitwise right rotation of each pixel value.
    /**
       Similar to operator>>=(const char*), except that it performs a right rotation instead of a right shift.
    **/
    CImg<T>& ror(const char *const expression) {
      return ror((+*this)._fill(expression,true,3,(CImgList<T>*)0,"ror",this,(CImg<doubleT>*)0));
    }

    //! Compute the bitwise right rotation of each pixel value \newinstance.
    CImg<T> get_ror(const char *const expression) const {
      return (+*this).ror(expression);
    }

    //! Compute the bitwise right rotation of each pixel value.
    /**
       Similar to operator>>=(const CImg<t>&), except that it performs a right rotation instead of a right shift.
    **/
    template<typename t>
    CImg<T>& ror(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return ror(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)cimg::ror(*ptrd,(unsigned int)(*(ptrs++)));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)cimg::ror(*ptrd,(unsigned int)(*(ptrs++)));
      }
      return *this;
    }

    //! Compute the bitwise right rotation of each pixel value \newinstance.
    template<typename t>
    CImg<T> get_ror(const CImg<t>& img) const {
      return (+*this).ror(img);
    }

    //! Softmax operator.
    CImg<T>& softmax(const float temperature=1) {
      return get_softmax(temperature).move_to(*this);
    }

    //! Softmax operator \newinstance.
    CImg<Tfloat> get_softmax(const float temperature=1) const {
      if (is_empty()) return CImg<Tfloat>();
      CImg<Tfloat> res(_width,_height,_depth,_spectrum);
      const T val_max = max();
      Tfloat sum = 0;
      cimg_pragma_openmp(parallel reduction(+:sum) cimg_openmp_if_size(size(),4096)) {
        cimg_pragma_openmp(for)
        cimg_rofoff(*this,off) {
          const Tfloat val = std::exp(((Tfloat)_data[off] - val_max)/temperature);
          res[off] = val;
          sum+=val;
        }
      }
      return res/=sum;
    }

    //! Softmin operator.
    CImg<T>& softmin(const float temperature=1) {
      return get_softmin(temperature).move_to(*this);
    }

    //! Softmin operator \newinstance.
    CImg<Tfloat> get_softmin(const float temperature=1) const {
      if (is_empty()) return CImg<Tfloat>();
      CImg<Tfloat> res(_width,_height,_depth,_spectrum);
      const T val_min = min();
      Tfloat sum = 0;
      cimg_pragma_openmp(parallel reduction(+:sum) cimg_openmp_if_size(size(),4096)) {
        cimg_pragma_openmp(for)
        cimg_rofoff(*this,off) {
          const Tfloat val = std::exp((-(Tfloat)_data[off] + val_min)/temperature);
          res[off] = val;
          sum+=val;
        }
      }
      return res/=sum;
    }

    //! Pointwise min operator between instance image and a value.
    /**
       \param val Value used as the reference argument of the min operator.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{min}(I_{(x,y,z,c)},\mathrm{val})\f$.
     **/
    CImg<T>& min(const T& value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,std::min(*ptr,value),65536,T);
      return *this;
    }

    //! Pointwise min operator between instance image and a value \newinstance.
    CImg<T> get_min(const T& value) const {
      return (+*this).min(value);
    }

    //! Pointwise min operator between two images.
    /**
       \param img Image used as the reference argument of the min operator.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{min}(I_{(x,y,z,c)},\mathrm{img}_{(x,y,z,c)})\f$.
     **/
    template<typename t>
    CImg<T>& min(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return min(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = std::min((T)*(ptrs++),*ptrd);
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = std::min((T)*(ptrs++),*ptrd);
      }
      return *this;
    }

    //! Pointwise min operator between two images \newinstance.
    template<typename t>
    CImg<_cimg_Tt> get_min(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).min(img);
    }

    //! Pointwise min operator between an image and an expression.
    /**
       \param expression Math formula as a C-string.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{min}(I_{(x,y,z,c)},\mathrm{expr}_{(x,y,z,c)})\f$.
    **/
    CImg<T>& min(const char *const expression) {
      return min((+*this)._fill(expression,true,3,(CImgList<T>*)0,"min",this,(CImg<doubleT>*)0));
    }

    //! Pointwise min operator between an image and an expression \newinstance.
    CImg<Tfloat> get_min(const char *const expression) const {
      return CImg<Tfloat>(*this,false).min(expression);
    }

    //! Pointwise max operator between instance image and a value.
    /**
       \param val Value used as the reference argument of the max operator.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{max}(I_{(x,y,z,c)},\mathrm{val})\f$.
     **/
    CImg<T>& max(const T& value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,std::max(*ptr,value),65536,T);
      return *this;
    }

    //! Pointwise max operator between instance image and a value \newinstance.
    CImg<T> get_max(const T& value) const {
      return (+*this).max(value);
    }

    //! Pointwise max operator between two images.
    /**
       \param img Image used as the reference argument of the max operator.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{max}(I_{(x,y,z,c)},\mathrm{img}_{(x,y,z,c)})\f$.
     **/
    template<typename t>
    CImg<T>& max(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return max(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = std::max((T)*(ptrs++),*ptrd);
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = std::max((T)*(ptrs++),*ptrd);
      }
      return *this;
    }

    //! Pointwise max operator between two images \newinstance.
    template<typename t>
    CImg<_cimg_Tt> get_max(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).max(img);
    }

    //! Pointwise max operator between an image and an expression.
    /**
       \param expression Math formula as a C-string.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{max}(I_{(x,y,z,c)},\mathrm{expr}_{(x,y,z,c)})\f$.
    **/
    CImg<T>& max(const char *const expression) {
      return max((+*this)._fill(expression,true,3,(CImgList<T>*)0,"max",this,(CImg<doubleT>*)0));
    }

    //! Pointwise max operator between an image and an expression \newinstance.
    CImg<Tfloat> get_max(const char *const expression) const {
      return CImg<Tfloat>(*this,false).max(expression);
    }

    //! Pointwise minabs operator between instance image and a value.
    /**
       \param val Value used as the reference argument of the minabs operator.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{minabs}(I_{(x,y,z,c)},\mathrm{val})\f$.
     **/
    CImg<T>& minabs(const T& value) {
      if (is_empty()) return *this;
      const T absvalue = cimg::abs(value);
      cimg_openmp_for(*this,cimg::minabs(*ptr,value,absvalue),65536,T);
      return *this;
    }

    //! Pointwise minabs operator between instance image and a value \newinstance.
    CImg<T> get_minabs(const T& value) const {
      return (+*this).minabs(value);
    }

    //! Pointwise minabs operator between two images.
    /**
       \param img Image used as the reference argument of the minabs operator.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{minabs}(I_{(x,y,z,c)},\mathrm{img}_{(x,y,z,c)})\f$.
     **/
    template<typename t>
    CImg<T>& minabs(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return minabs(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = cimg::minabs((T)*(ptrs++),*ptrd);
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = cimg::minabs((T)*(ptrs++),*ptrd);
      }
      return *this;
    }

    //! Pointwise minabs operator between two images \newinstance.
    template<typename t>
    CImg<_cimg_Tt> get_minabs(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).minabs(img);
    }

    //! Pointwise minabs operator between an image and an expression.
    /**
       \param expression Math formula as a C-string.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{minabs}(I_{(x,y,z,c)},\mathrm{expr}_{(x,y,z,c)})\f$.
    **/
    CImg<T>& minabs(const char *const expression) {
      return minabs((+*this)._fill(expression,true,3,(CImgList<T>*)0,"minabs",this,(CImg<doubleT>*)0));
    }

    //! Pointwise minabs operator between an image and an expression \newinstance.
    CImg<Tfloat> get_minabs(const char *const expression) const {
      return CImg<Tfloat>(*this,false).minabs(expression);
    }

    //! Pointwise maxabs operator between instance image and a value.
    /**
       \param val Value used as the reference argument of the maxabs operator.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{maxabs}(I_{(x,y,z,c)},\mathrm{val})\f$.
     **/
    CImg<T>& maxabs(const T& value) {
      if (is_empty()) return *this;
      const T absvalue = cimg::abs(value);
      cimg_openmp_for(*this,cimg::maxabs(*ptr,value,absvalue),65536,T);
      return *this;
    }

    //! Pointwise maxabs operator between instance image and a value \newinstance.
    CImg<T> get_maxabs(const T& value) const {
      return (+*this).maxabs(value);
    }

    //! Pointwise maxabs operator between two images.
    /**
       \param img Image used as the reference argument of the maxabs operator.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{maxabs}(I_{(x,y,z,c)},\mathrm{img}_{(x,y,z,c)})\f$.
     **/
    template<typename t>
    CImg<T>& maxabs(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return maxabs(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = cimg::maxabs((T)*(ptrs++),*ptrd);
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = cimg::maxabs((T)*(ptrs++),*ptrd);
      }
      return *this;
    }

    //! Pointwise maxabs operator between two images \newinstance.
    template<typename t>
    CImg<_cimg_Tt> get_maxabs(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this,false).maxabs(img);
    }

    //! Pointwise maxabs operator between an image and an expression.
    /**
       \param expression Math formula as a C-string.
       \note Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by
       \f$\mathrm{maxabs}(I_{(x,y,z,c)},\mathrm{expr}_{(x,y,z,c)})\f$.
    **/
    CImg<T>& maxabs(const char *const expression) {
      return maxabs((+*this)._fill(expression,true,3,(CImgList<T>*)0,"maxabs",this,(CImg<doubleT>*)0));
    }

    //! Pointwise maxabs operator between an image and an expression \newinstance.
    CImg<Tfloat> get_maxabs(const char *const expression) const {
      return CImg<Tfloat>(*this,false).maxabs(expression);
    }

    //! Return a reference to the minimum pixel value.
    /**
     **/
    T& min() {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "min(): Empty instance.",
                                    cimg_instance);
      T *ptr_min = _data;
      T min_value = *ptr_min;
      cimg_for(*this,ptrs,T) if (*ptrs<min_value) min_value = *(ptr_min=ptrs);
      return *ptr_min;
    }

    //! Return a reference to the minimum pixel value \const.
    const T& min() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "min(): Empty instance.",
                                    cimg_instance);
      const T *ptr_min = _data;
      T min_value = *ptr_min;
      cimg_for(*this,ptrs,T) if (*ptrs<min_value) min_value = *(ptr_min=ptrs);
      return *ptr_min;
    }

    //! Return a reference to the minimum pixel value in absolute value.
    /**
     **/
    T& minabs() {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "minabs(): Empty instance.",
                                    cimg_instance);
      T *ptr_minabs = _data;
      T minabs_value = *ptr_minabs;
      cimg_for(*this,ptrs,T) {
        const T ma = cimg::abs(*ptrs);
        if (ma<minabs_value) { minabs_value = ma; ptr_minabs = ptrs; }
      }
      return *ptr_minabs;
    }

    //! Return a reference to the minimum pixel value in absolute value \const.
    const T& minabs() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "minabs(): Empty instance.",
                                    cimg_instance);
      const T *ptr_minabs = _data;
      T minabs_value = *ptr_minabs;
      cimg_for(*this,ptrs,T) {
        const T ma = cimg::abs(*ptrs);
        if (ma<minabs_value) { minabs_value = ma; ptr_minabs = ptrs; }
      }
      return *ptr_minabs;
    }

    //! Return a reference to the maximum pixel value.
    /**
     **/
    T& max() {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "max(): Empty instance.",
                                    cimg_instance);
      T *ptr_max = _data;
      T max_value = *ptr_max;
      cimg_for(*this,ptrs,T) if (*ptrs>max_value) max_value = *(ptr_max=ptrs);
      return *ptr_max;
    }

    //! Return a reference to the maximum pixel value \const.
    const T& max() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "max(): Empty instance.",
                                    cimg_instance);
      const T *ptr_max = _data;
      T max_value = *ptr_max;
      cimg_for(*this,ptrs,T) if (*ptrs>max_value) max_value = *(ptr_max=ptrs);
      return *ptr_max;
    }

    //! Return a reference to the maximum pixel value in absolute value.
    /**
     **/
    T& maxabs() {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "maxabs(): Empty instance.",
                                    cimg_instance);
      T *ptr_maxabs = _data;
      T maxabs_value = *ptr_maxabs;
      cimg_for(*this,ptrs,T) {
        const T ma = cimg::abs(*ptrs);
        if (ma>maxabs_value) { maxabs_value = ma; ptr_maxabs = ptrs; }
      }
      return *ptr_maxabs;
    }

    //! Return a reference to the maximum pixel value in absolute value \const.
    const T& maxabs() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "maxabs(): Empty instance.",
                                    cimg_instance);
      const T *ptr_maxabs = _data;
      T maxabs_value = *ptr_maxabs;
      cimg_for(*this,ptrs,T) {
        const T ma = cimg::abs(*ptrs);
        if (ma>maxabs_value) { maxabs_value = ma; ptr_maxabs = ptrs; }
      }
      return *ptr_maxabs;
    }

    //! Return a reference to the minimum pixel value as well as the maximum pixel value.
    /**
       \param[out] max_val Maximum pixel value.
    **/
    template<typename t>
    T& min_max(t& max_val) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "min_max(): Empty instance.",
                                    cimg_instance);
      const T *ptr_min, *ptr_max;
      _min_max(ptr_min,ptr_max);
      max_val = (t)*ptr_max;
      return (T&)*ptr_min;
    }

    void _min_max(const T* &ptr_min, const T* &ptr_max) const {
      T val_min = *_data, val_max = val_min;
      ptr_min = ptr_max = _data;
      if (cimg_use_openmp && size()>(1LU<<24)*cimg_openmp_sizefactor) {
#if cimg_use_openmp
        cimg_pragma_openmp(parallel) {
          T l_val_min = *_data, l_val_max = l_val_min;
          const T *l_ptr_min = _data, *l_ptr_max = l_ptr_min;
          cimg_pragma_openmp(for)
          cimg_rofoff(*this,off) {
            const T *const ptr = _data + off;
            const T val = *ptr;
            if (val<l_val_min) { l_val_min = val; l_ptr_min = ptr; }
            if (val>l_val_max) { l_val_max = val; l_ptr_max = ptr; }
          }
          cimg_pragma_openmp(critical(_min_max)) {
            if (l_val_min<val_min || (l_val_min==val_min && l_ptr_min<ptr_min)) {
              val_min = l_val_min; ptr_min = l_ptr_min;
            }
            if (l_val_max>val_max || (l_val_max==val_max && l_ptr_max<ptr_max)) {
              val_max = l_val_max; ptr_max = l_ptr_max;
            }
          }
        }
#endif
      }

      // Single-threaded version.
      cimg_for(*this,ptrs,T) {
        const T val = *ptrs;
        if (val<val_min) { val_min = val; ptr_min = ptrs; }
        if (val>val_max) { val_max = val; ptr_max = ptrs; }
      }
    }

    //! Return a reference to the minimum pixel value as well as the maximum pixel value \const.
    template<typename t>
    const T& min_max(t& max_val) const {
      return ((CImg<T>*)this)->min_max(max_val);
    }

    //! Return a reference to the maximum pixel value as well as the minimum pixel value.
    /**
       \param[out] min_val Minimum pixel value.
    **/
    template<typename t>
    T& max_min(t& min_val) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "max_min(): Empty instance.",
                                    cimg_instance);
      const T *ptr_min, *ptr_max;
      _min_max(ptr_min,ptr_max);
      min_val = (t)*ptr_min;
      return (T&)*ptr_max;
    }

    //! Return a reference to the maximum pixel value as well as the minimum pixel value \const.
    template<typename t>
    const T& max_min(t& min_val) const {
      return ((CImg<T>*)this)->max_min(min_val);
    }

    //! Return the kth smallest pixel value.
    /**
       \param k Rank of the smallest element searched.
    **/
    T kth_smallest(const ulongT k) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "kth_smallest(): Empty instance.",
                                    cimg_instance);
      if (k>=size()) return max();
      CImg<T> arr(*this,false);
      ulongT l = 0, ir = size() - 1;
      for ( ; ; ) {
        if (ir<=l + 1) {
          if (ir==l + 1 && arr[ir]<arr[l]) cimg::swap(arr[l],arr[ir]);
          return arr[k];
        } else {
          const ulongT mid = (l + ir)>>1;
          cimg::swap(arr[mid],arr[l + 1]);
          if (arr[l]>arr[ir]) cimg::swap(arr[l],arr[ir]);
          if (arr[l + 1]>arr[ir]) cimg::swap(arr[l + 1],arr[ir]);
          if (arr[l]>arr[l + 1]) cimg::swap(arr[l],arr[l + 1]);
          ulongT i = l + 1, j = ir;
          const T pivot = arr[l + 1];
          for ( ; ; ) {
            do ++i; while (arr[i]<pivot);
            do --j; while (arr[j]>pivot);
            if (j<i) break;
            cimg::swap(arr[i],arr[j]);
          }
          arr[l + 1] = arr[j];
          arr[j] = pivot;
          if (j>=k) ir = j - 1;
          if (j<=k) l = i;
        }
      }
    }

    //! Return the median pixel value.
    /**
     **/
    T median() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "median(): Empty instance.",
                                    cimg_instance);
      const ulongT s = size();
      switch (s) {
      case 1 : return _data[0];
      case 2 : return cimg::median(_data[0],_data[1]);
      case 3 : return cimg::median(_data[0],_data[1],_data[2]);
      case 5 : return cimg::median(_data[0],_data[1],_data[2],_data[3],_data[4]);
      case 7 : return cimg::median(_data[0],_data[1],_data[2],_data[3],_data[4],_data[5],_data[6]);
      case 9 : return cimg::median(_data[0],_data[1],_data[2],_data[3],_data[4],_data[5],_data[6],_data[7],_data[8]);
      case 13 : return cimg::median(_data[0],_data[1],_data[2],_data[3],_data[4],_data[5],_data[6],_data[7],_data[8],
                                    _data[9],_data[10],_data[11],_data[12]);
      }
      const T res = kth_smallest(s>>1);
      return (s%2)?res:(T)((res + kth_smallest((s>>1) - 1))/2);
    }

    //! Return greatest common diviser of all image values.
    T gcd() const {
      if (is_empty()) return 0;
      const ulongT siz = size();
      longT res = (longT)*_data;
      for (ulongT k = 1; k<siz; ++k) res = cimg::gcd(res,(longT)_data[k]);
      return res;
    }

    //! Return least common multiplier of all image values.
    T lcm() const {
      if (is_empty()) return 0;
      const ulongT siz = size();
      longT res = (longT)*_data;
      for (ulongT k = 1; k<siz; ++k) res = cimg::lcm(res,(longT)_data[k]);
      return res;
    }

    //! Return the product of all the pixel values.
    /**
     **/
    double product() const {
      if (is_empty()) return 0;
      double res = 1;
      cimg_for(*this,ptrs,T) res*=(double)*ptrs;
      return res;
    }

    //! Return the sum of all the pixel values.
    /**
     **/
    double sum() const {
      double res = 0;
      cimg_for(*this,ptrs,T) res+=(double)*ptrs;
      return res;
    }

    //! Return the average pixel value.
    /**
     **/
    double mean() const {
      double res = 0;
      cimg_for(*this,ptrs,T) res+=(double)*ptrs;
      return res/size();
    }

    //! Return the variance of the pixel values.
    /**
       \param variance_method Method used to estimate the variance. Can be:
       - \c 0: Second moment, computed as
       \f$1/N \sum\limits_{k=1}^{N} (x_k - \bar x)^2 =
       1/N \left( \sum\limits_{k=1}^N x_k^2 - \left( \sum\limits_{k=1}^N x_k \right)^2 / N \right)\f$
       with \f$ \bar x = 1/N \sum\limits_{k=1}^N x_k \f$.
       - \c 1: Best unbiased estimator, computed as \f$\frac{1}{N - 1} \sum\limits_{k=1}^{N} (x_k - \bar x)^2 \f$.
       - \c 2: Least median of squares.
       - \c 3: Least trimmed of squares.
    **/
    double variance(const unsigned int variance_method=0) const {
      double foo;
      return variance_mean(variance_method,foo);
    }

    //! Return the variance as well as the average of the pixel values.
    /**
       \param variance_method Method used to estimate the variance (see variance(const unsigned int) const).
       \param[out] mean Average pixel value.
    **/
    template<typename t>
    double variance_mean(const unsigned int variance_method, t& mean) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "variance_mean(): Empty instance.",
                                    cimg_instance);
      double var = 0, sum = 0;
      const ulongT siz = size();
      if (!siz) { mean = (t)0; return 0; }
      if (siz==1) { mean = (t)*_data; return 0; }
      switch (variance_method) {
      case 0 : case 1 : { // Population/unbiased (using Welford method)
        double m = 0, S2 = 0;
        ulongT n = 0;
        cimg_for(*this,ptrs,T) {
          const double val = (double)*ptrs, delta = val - m;
          m+=delta/++n;
          const double delta2 = val - m;
          S2+=delta*delta2;
        }
        mean = (t)m;
        var = S2/(siz - variance_method);
      } break;
      case 2 : { // Least Median of Squares (MAD)
        CImg<Tfloat> buf(*this,false);
        buf.sort();
        const ulongT siz2 = siz>>1;
        const double med_i = (double)buf[siz2];
        cimg_for(buf,ptrs,Tfloat) {
          const double val = (double)*ptrs; *ptrs = (Tfloat)cimg::abs(val - med_i); sum+=val;
        }
        buf.sort();
        const double sig = (double)(1.4828*buf[siz2]);
        var = sig*sig;
      } break;
      default : { // Least trimmed of Squares
        CImg<Tfloat> buf(*this,false);
        const ulongT siz2 = siz>>1;
        cimg_for(buf,ptrs,Tfloat) {
          const double val = (double)*ptrs; (*ptrs)=(Tfloat)((*ptrs)*val); sum+=val;
        }
        buf.sort();
        double a = 0;
        const Tfloat *ptrs = buf._data;
        for (ulongT j = 0; j<siz2; ++j) a+=(double)*(ptrs++);
        const double sig = (double)(2.6477*std::sqrt(a/siz2));
        var = sig*sig;
      }
      }
      if (variance_method>1) mean = (t)(sum/siz);
      return std::max(var,0.);
    }

    //! Return estimated variance of the noise.
    /**
       \param variance_method Method used to compute the variance (see variance(const unsigned int) const).
       \note Because of structures such as edges in images it is
       recommended to use an unbiased variance estimation. The variance of the
       noise is estimated by computing the variance of the Laplacian \f$(\Delta
       I)^2 \f$ scaled by a factor \f$c\f$ insuring \f$ c E[(\Delta I)^2]=
       \sigma^2\f$ where \f$\sigma\f$ is the noise variance.
    **/
    double variance_noise(const unsigned int variance_method=2) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "variance_noise(): Empty instance.",
                                    cimg_instance);

      const ulongT siz = size();
      if (siz<2 || !_data) return 0;
      if (variance_method>1) { // Compute a scaled version of the Laplacian
        CImg<Tdouble> tmp(*this,false);
        if (_depth==1) {
          const double cste = 1./std::sqrt(20.); // Depends on how the Laplacian is computed
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*262144 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            CImg_3x3(I,T);
            cimg_for3x3(*this,x,y,0,c,I,T) {
              tmp(x,y,c) = cste*((double)Inc + (double)Ipc + (double)Icn +
                                 (double)Icp - 4*(double)Icc);
            }
          }
        } else {
          const double cste = 1./std::sqrt(42.); // Depends on how the Laplacian is computed
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*262144 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            CImg_3x3x3(I,T);
            cimg_for3x3x3(*this,x,y,z,c,I,T) {
              tmp(x,y,z,c) = cste*((double)Incc + (double)Ipcc + (double)Icnc + (double)Icpc +
                                   (double)Iccn + (double)Iccp - 6*(double)Iccc);
            }
          }
        }
        return tmp.variance(variance_method);
      }

      // Version that doesn't need intermediate images.
      double m = 0, S2 = 0;
      ulongT n = 0;
      if (_depth==1) {
        const double cste = 1./std::sqrt(20.);
        CImg_3x3(I,T);
        cimg_forC(*this,c) cimg_for3x3(*this,x,y,0,c,I,T) {
          const double
            val = cste*((double)Inc + (double)Ipc + (double)Icn + (double)Icp - 4*(double)Icc),
            delta = val - m;
          m+=delta/++n;
          const double delta2 = val - m;
          S2+=delta*delta2;
        }
      } else {
        const double cste = 1./std::sqrt(42.);
        CImg_3x3x3(I,T);
        cimg_forC(*this,c) cimg_for3x3x3(*this,x,y,z,c,I,T) {
          const double
            val = cste*((double)Incc + (double)Ipcc + (double)Icnc + (double)Icpc +
                        (double)Iccn + (double)Iccp - 6*(double)Iccc),
            delta = val - m;
          m+=delta/++n;
          const double delta2 = val - m;
          S2+=delta*delta2;
        }
      }
      return std::max(S2/(n - variance_method),0.);
    }

    //! Compute the MSE (Mean-Squared Error) between two images.
    /**
       \param img Image used as the second argument of the MSE operator.
    **/
    template<typename t>
    double MSE(const CImg<t>& img) const {
      if (img.size()!=size())
        throw CImgArgumentException(_cimg_instance
                                    "MSE(): Instance and specified image (%u,%u,%u,%u,%p) have different dimensions.",
                                    cimg_instance,
                                    img._width,img._height,img._depth,img._spectrum,img._data);
      double vMSE = 0;
      const t* ptr2 = img._data;
      cimg_for(*this,ptr1,T) {
        const double diff = (double)*ptr1 - (double)*(ptr2++);
        vMSE+=diff*diff;
      }
      const ulongT siz = img.size();
      if (siz) vMSE/=siz;
      return vMSE;
    }

    //! Compute the PSNR (Peak Signal-to-Noise Ratio) between two images.
    /**
       \param img Image used as the second argument of the PSNR operator.
       \param max_value Maximum theoretical value of the signal.
     **/
    template<typename t>
    double PSNR(const CImg<t>& img, const double max_value=255) const {
      const double vMSE = (double)std::sqrt(MSE(img));
      return (vMSE!=0)?(double)(20*std::log10(max_value/vMSE)):(double)(cimg::type<double>::max());
    }

    //! Evaluate math formula.
    /**
       \param expression Math formula, as a C-string.
       \param x Value of the pre-defined variable \c x.
       \param y Value of the pre-defined variable \c y.
       \param z Value of the pre-defined variable \c z.
       \param c Value of the pre-defined variable \c c.
       \param list_images A list of images attached to the specified math formula.
    **/
    double eval(const char *const expression,
                const double x=0, const double y=0, const double z=0, const double c=0,
                CImgList<T> *const list_images=0) {
      return _eval(this,expression,x,y,z,c,list_images);
    }

    //! Evaluate math formula \const.
    double eval(const char *const expression,
                const double x=0, const double y=0, const double z=0, const double c=0,
                CImgList<T> *const list_images=0) const {
      return _eval(0,expression,x,y,z,c,list_images);
    }

    // Fast function to pre-evaluate common expressions.
    // (return 'true' in case of success, and set value of 'res').
    template<typename t>
    bool __eval(const char *const expression, t &res) const {

#define __eval_op(op) if (__eval_get(++ptr,val2) && !*ptr) { res = (t)(op); return true; } else return false;

      double val1, val2;
      if (!expression || !*expression || *expression==';' || *expression=='[') return false;
      if (!expression[1]) switch (*expression) {
        case 'w' : res = (t)_width; return true;
        case 'h' : res = (t)_height; return true;
        case 'd' : res = (t)_depth; return true;
        case 's' : res = (t)_spectrum; return true;
        case 'r' : res = (t)_is_shared; return true;
        default : if (*expression>='0' && *expression<='9') { res = (t)(*expression - '0'); return true; }
        }
      if (*expression=='w' && expression[1]=='h') {
        if (!expression[2]) { res = (t)(_width*_height); return true; }
        if (expression[2]=='d') {
          if (!expression[3]) { res = (t)(_width*_height*_depth); return true; }
          if (expression[3]=='s' && !expression[4]) { res = (t)(_width*_height*_depth*_spectrum); return true; }
        }
        if (expression[2]=='s' && !expression[3]) { res = (t)(_width*_height*_spectrum); return true; }
      }
      const char *ptr = expression;
      while (*ptr && cimg::is_blank(*ptr)) ++ptr;
      if (*ptr=='\'' && *(++ptr)) { // Detect 'stringA' op 'stringB' (op='==' or '!=')
        const char *ptr2 = std::strchr(ptr,'\'');
        if (ptr2) {
          const char *ptr3 = ptr2 + 1;
          while (*ptr3 && cimg::is_blank(*ptr3)) ++ptr3;
          const char *ptr4 = ptr3;
          if ((*ptr3=='!' || *ptr3=='=') && *(++ptr4)=='=') {
            ++ptr4;
            while (*ptr4 && cimg::is_blank(*ptr4)) ++ptr4;
            if (*ptr4=='\'' && *(++ptr4)) {
              const char *const ptr5 = std::strchr(ptr4,'\'');
              if (ptr5) {
                const char *ptr6 = ptr5 + 1;
                while (*ptr6 && cimg::is_blank(*ptr6)) ++ptr6;
                if (!*ptr6) {
                  CImg<charT> str1(ptr,ptr2 - ptr,1,1,1,true), str2(ptr4,ptr5 - ptr4,1,1,1,true);
                  if (*ptr3=='!') res = (t)!(str1==str2); else res = (t)(str1==str2);
                  return true;
                }
              }
            }
          }
        }
        return false;
      }
      if (__eval_get(ptr,val1)) { // Detect 'value1' op 'value2'
        switch (*ptr) {
        case 0 : res = (t)val1; return true;
        case '+' : __eval_op(val1 + val2);
        case '-' : __eval_op(val1 - val2);
        case '*' : __eval_op(val1 * val2);
        case '/' : __eval_op(val1 / val2);
        case '%' : __eval_op(cimg::mod(val1,val2));
        case '&' : if (ptr[1]=='&') { ++ptr; __eval_op(val1 && val2); } else { __eval_op((long)val1 & (long)val2); }
        case '|' : if (ptr[1]=='|') { ++ptr; __eval_op(val1 || val2); } else { __eval_op((long)val1 | (long)val2); }
        case '>' : if (ptr[1]=='=') { ++ptr; __eval_op(val1>=val2); } else { __eval_op(val1>val2); }
        case '<' : if (ptr[1]=='=') { ++ptr; __eval_op(val1<=val2); } else { __eval_op(val1<val2); }
        case ';' : __eval_op(val2);
        case '^' : __eval_op(std::pow(val1,val2));
        case '=' : if (*++ptr=='=') { __eval_op(val1==val2); } else return false;
        case '!' : if (*++ptr=='=') { __eval_op(val1!=val2); } else return false;
        }
      }
      return false;
    }

    // Return 'true' is a single 'value' or '!value' has been succesfully read ('value' being a double or { w,h,d,s }).
    bool __eval_get(const char* &ptr, double &value) const {
      int n = 0;
      while (*ptr && cimg::is_blank(*ptr)) ++ptr;

      bool is_not = false; // Detect preceding '!' operator
      if (*ptr=='!') { is_not = true; ++ptr; while (*ptr && cimg::is_blank(*ptr)) ++ptr; }

      if ((*ptr=='w' || *ptr=='h' || *ptr=='d' || *ptr=='s' || *ptr=='r') || cimg_sscanf(ptr,"%lf %n",&value,&n)==1) {
        if (!n) {
          switch (*ptr) {
          case 'w': value = (double)_width; break;
          case 'h': value = (double)_height; break;
          case 'd': value = (double)_depth; break;
          case 's': value = (double)_spectrum; break;
          case 'r': value = (double)_is_shared; break;
          }
          ++ptr; while (*ptr && cimg::is_blank(*ptr)) ++ptr;
        } else ptr+=n;
        if (is_not) value = (double)!value;
        return true;
      }
      return false;
    }

    double _eval(CImg<T> *const img_output, const char *const expression,
                 const double x, const double y, const double z, const double c,
                 CImgList<T> *const list_images) const {
      if (!expression || !*expression) return 0;
      double _val = 0;
      if (__eval(expression,_val)) return _val;
      _cimg_math_parser mp(expression + (*expression=='>' || *expression=='<' || *expression=='+' ||
                                         *expression=='*' || *expression==':'),"eval",
                           *this,img_output,list_images,false);
      mp.begin_t();
      const double val = mp(x,y,z,c);
      mp.end_t();
      mp.end();
      return val;
    }

    //! Evaluate math formula.
    /**
       \param[out] output Contains values of output vector returned by the evaluated expression
         (or is empty if the returned type is scalar).
       \param expression Math formula, as a C-string.
       \param x Value of the pre-defined variable \c x.
       \param y Value of the pre-defined variable \c y.
       \param z Value of the pre-defined variable \c z.
       \param c Value of the pre-defined variable \c c.
       \param list_images A list of input images attached to the specified math formula.
    **/
    template<typename t>
    void eval(CImg<t> &output, const char *const expression,
              const double x=0, const double y=0, const double z=0, const double c=0,
              CImgList<T> *const list_images=0) {
      _eval(output,this,expression,x,y,z,c,list_images);
    }

    //! Evaluate math formula \const.
    template<typename t>
    void eval(CImg<t>& output, const char *const expression,
              const double x=0, const double y=0, const double z=0, const double c=0,
              CImgList<T> *const list_images=0) const {
      _eval(output,0,expression,x,y,z,c,list_images);
    }

    template<typename t>
    void _eval(CImg<t>& output, CImg<T> *const img_output, const char *const expression,
               const double x, const double y, const double z, const double c,
               CImgList<T> *const list_images) const {
      if (!expression || !*expression) { output.assign(1); *output = 0; return; }
      double _val = 0;
      if (__eval(expression,_val)) { output.assign(1); *output = _val; return; }
      _cimg_math_parser mp(expression + (*expression=='>' || *expression=='<' || *expression=='+' ||
                                         *expression=='*' || *expression==':'),"eval",
                           *this,img_output,list_images,false);
      output.assign(1,std::max(1U,mp.result_dim));
      mp.begin_t();
      mp(x,y,z,c,output._data);
      mp.end_t();
      mp.end();
    }

    //! Evaluate math formula on a set of variables.
    /**
       \param expression Math formula, as a C-string.
       \param xyzc Set of values (x,y,z,c) used for the evaluation.
       \param list_images A list of input images attached to the specified math formula.
    **/
    template<typename t>
    CImg<doubleT> eval(const char *const expression, const CImg<t>& xyzc,
                       CImgList<T> *const list_images=0) {
      return _eval(this,expression,xyzc,list_images);
    }

    //! Evaluate math formula on a set of variables \const.
    template<typename t>
    CImg<doubleT> eval(const char *const expression, const CImg<t>& xyzc,
                       CImgList<T> *const list_images=0) const {
      return _eval(0,expression,xyzc,list_images);
    }

    template<typename t>
    CImg<doubleT> _eval(CImg<T> *const output, const char *const expression, const CImg<t>& xyzc,
                        CImgList<T> *const list_images=0) const {
      CImg<doubleT> res(1,xyzc.size()/4);
      if (!expression || !*expression) return res.fill(0);
      _cimg_math_parser mp(expression,"eval",*this,output,list_images,false);

#if cimg_use_openmp!=0
      const int num_threads = (int)std::min(res.size(),(ulongT)omp_get_max_threads());
      cimg_pragma_openmp(parallel if (num_threads>0 && res._height>=512) num_threads(num_threads)) {
        _cimg_math_parser
          *const _mp = omp_get_thread_num()?new _cimg_math_parser(mp):&mp,
          &lmp = *_mp;
        cimg_pragma_openmp(barrier)
        lmp.begin_t();
        cimg_pragma_openmp(for)
        for (int i = 0; i<res.height(); ++i) {
          const unsigned int i4 = 4*i;
          const double
            x = (double)xyzc[i4], y = (double)xyzc[i4 + 1],
            z = (double)xyzc[i4 + 2], c = (double)xyzc[i4 + 3];
          res[i] = lmp(x,y,z,c);
        }
        lmp.end_t();
        cimg_pragma_openmp(barrier) cimg_pragma_openmp(critical) { lmp.merge(mp); }
        if (&lmp!=&mp) delete &lmp;
      }
#else
      mp.begin_t();
      const t *ps = xyzc._data;
      cimg_for(res,pd,double) {
        const double x = (double)*(ps++), y = (double)*(ps++), z = (double)*(ps++), c = (double)*(ps++);
        *pd = mp(x,y,z,c);
      }
      mp.end_t();
#endif
      mp.end();
      return res;
    }

    //! Compute statistics vector from the pixel values.
    /**
       \param variance_method Method used to compute the variance (see variance(const unsigned int) const).
       \return Statistics vector as
         <tt>[min, max, mean, variance, xmin, ymin, zmin, cmin, xmax, ymax, zmax, cmax, sum, product]</tt>.
    **/
    CImg<Tdouble> get_stats(const unsigned int variance_method=0) const {
      if (is_empty()) return CImg<doubleT>();
      const ulongT siz = size();
      const longT off_end = (longT)siz;
      double avg = 0, S = 0, S2 = 0, P = 1;
      longT offm = 0, offM = 0, n = 0;
      T m = *_data, M = m;
      cimg_pragma_openmp(parallel reduction(+:S,S2) reduction(*:P)
                         cimg_openmp_if(variance_method>1 && siz>=(cimg_openmp_sizefactor)*131072)) {
        longT loffm = 0, loffM = 0;
        T lm = *_data, lM = lm;
        cimg_pragma_openmp(for)
        for (longT off = 0; off<off_end; ++off) {
          const T val = _data[off];
          const double _val = (double)val;
          if (val<lm) { lm = val; loffm = off; }
          if (val>lM) { lM = val; loffM = off; }
          S+=_val;
          P*=_val;
          if (variance_method<1) {
           const double delta = _val - avg;
           avg+=delta/++n;
           const double delta2 = _val - avg;
           S2+=delta*delta2;
          }
        }
        cimg_pragma_openmp(critical(get_stats)) {
          if (lm<m || (lm==m && loffm<offm)) { m = lm; offm = loffm; }
          if (lM>M || (lM==M && loffM<offM)) { M = lM; offM = loffM; }
        }
      }

      const double
        mean_value = S/siz,
        variance_value = siz<2?0:std::max(variance_method<2?S2/(siz - variance_method):variance(variance_method),0.);
      int
        xm = 0, ym = 0, zm = 0, cm = 0,
        xM = 0, yM = 0, zM = 0, cM = 0;
      contains(_data[offm],xm,ym,zm,cm);
      contains(_data[offM],xM,yM,zM,cM);
      return CImg<Tdouble>(1,14).fill((double)m,(double)M,mean_value,variance_value,
                                      (double)xm,(double)ym,(double)zm,(double)cm,
                                      (double)xM,(double)yM,(double)zM,(double)cM,
                                      S,P);
    }

    //! Compute statistics vector from the pixel values \inplace.
    CImg<T>& stats(const unsigned int variance_method=0) {
      return get_stats(variance_method).move_to(*this);
    }

    //@}
#endif
