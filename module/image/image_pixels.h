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
    iterator begin() {
      return _data;
    }

    //! Return a CImg<T>::iterator pointing to the first value of the pixel buffer \const.
    const_iterator begin() const {
      return _data;
    }

    //! Return a CImg<T>::iterator pointing next to the last pixel value.
    /**
       \note
       - Writing \c img.end() is equivalent to <tt>img.data() + img.size()</tt>.
       - It has been mainly defined for compatibility with STL naming conventions.
       \warning
       - The returned iterator actually points to a value located \e outside the acceptable bounds of the pixel buffer.
         Trying to read or write the content of the returned iterator will probably result in a crash.
         Use it mainly as a strict upper bound for a CImg<T>::iterator.
       \par Example
       \code
       CImg<float> img(100,100,1,3); // Define a 100x100 RGB color image
       // 'img.end()' used below as an upper bound for the iterator.
       for (CImg<float>::iterator it = img.begin(); it<img.end(); ++it)
         *it = 0;
       \endcode
    **/
    iterator end() {
      return _data + size();
    }

    //! Return a CImg<T>::iterator pointing next to the last pixel value \const.
    const_iterator end() const {
      return _data + size();
    }

    //! Return a reference to the first pixel value.
    /**
       \note
       - Writing \c img.front() is equivalent to <tt>img[0]</tt>, or <tt>img(0,0,0,0)</tt>.
       - It has been mainly defined for compatibility with STL naming conventions.
    **/
    T& front() {
      return *_data;
    }

    //! Return a reference to the first pixel value \const.
    const T& front() const {
      return *_data;
    }

    //! Return a reference to the last pixel value.
    /**
       \note
       - Writing \c img.back() is equivalent to <tt>img[img.size() - 1]</tt>, or
         <tt>img(img.width() - 1,img.height() - 1,img.depth() - 1,img.spectrum() - 1)</tt>.
       - It has been mainly defined for compatibility with STL naming conventions.
    **/
    T& back() {
      return *(_data + size() - 1);
    }

    //! Return a reference to the last pixel value \const.
    const T& back() const {
      return *(_data + size() - 1);
    }

    //! Access to a pixel value at a specified offset, using Dirichlet boundary conditions.
    /**
       Return a reference to the pixel value of the image instance located at a specified \c offset,
       or to a specified default value in case of out-of-bounds access.
       \param offset Offset to the desired pixel value.
       \param out_value Default value returned if \c offset is outside image bounds.
       \note
       - Writing \c img.at(offset,out_value) is similar to <tt>img[offset]</tt>, except that if \c offset
         is outside bounds (e.g. \c offset<0 or \c offset>=img.size()), a reference to a value \c out_value
         is safely returned instead.
       - Due to the additional boundary checking operation, this method is slower than operator()(). Use it when
         you are \e not sure about the validity of the specified pixel offset.
    **/
    T& at(const int offset, const T& out_value) {
      return (offset<0 || offset>=(int)size())?(cimg::temporary(out_value)=out_value):(*this)[offset];
    }

    //! Access to a pixel value at a specified offset, using Dirichlet boundary conditions \const.
    T at(const int offset, const T& out_value) const {
      return (offset<0 || offset>=(int)size())?out_value:(*this)[offset];
    }

    //! Access to a pixel value at a specified offset, using Neumann boundary conditions.
    /**
       Return a reference to the pixel value of the image instance located at a specified \c offset,
       or to the nearest pixel location in the image instance in case of out-of-bounds access.
       \param offset Offset to the desired pixel value.
       \note
       - Similar to at(int,const T), except that an out-of-bounds access returns the value of the
         nearest pixel in the image instance, regarding the specified offset, i.e.
         - If \c offset<0, then \c img[0] is returned.
         - If \c offset>=img.size(), then \c img[img.size() - 1] is returned.
       - Due to the additional boundary checking operation, this method is slower than operator()(). Use it when
         you are \e not sure about the validity of the specified pixel offset.
       - If you know your image instance is \e not empty, you may rather use the slightly faster method \c _at(int).
     **/
    T& at(const int offset) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "at(): Empty instance.",
                                    cimg_instance);
      return _at(offset);
    }

    T& _at(const int offset) {
      const unsigned int siz = (unsigned int)size();
      return (*this)[offset<0?0:(unsigned int)offset>=siz?siz - 1:offset];
    }

    //! Access to a pixel value at a specified offset, using Neumann boundary conditions \const.
    const T& at(const int offset) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "at(): Empty instance.",
                                    cimg_instance);
      return _at(offset);
    }

    const T& _at(const int offset) const {
      const unsigned int siz = (unsigned int)size();
      return (*this)[offset<0?0:(unsigned int)offset>=siz?siz - 1:offset];
    }

    //! Access to a pixel value, using Dirichlet boundary conditions for the X-coordinate.
    /**
       Return a reference to the pixel value of the image instance located at (\c x,\c y,\c z,\c c),
       or to a specified default value in case of out-of-bounds access along the X-axis.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param out_value Default value returned if \c (\c x,\c y,\c z,\c c) is outside image bounds.
       \note
       - Similar to operator()(), except that an out-of-bounds access along the X-axis returns the specified value
         \c out_value.
       - Due to the additional boundary checking operation, this method is slower than operator()(). Use it when
         you are \e not sure about the validity of the specified pixel coordinates.
       \warning
       - There is \e no boundary checking performed for the Y,Z and C-coordinates, so they must be inside image bounds.
    **/
    T& atX(const int x, const int y, const int z, const int c, const T& out_value) {
      return (x<0 || x>=width())?(cimg::temporary(out_value)=out_value):(*this)(x,y,z,c);
    }

    //! Access to a pixel value, using Dirichlet boundary conditions for the X-coordinate \const.
    T atX(const int x, const int y, const int z, const int c, const T& out_value) const {
      return (x<0 || x>=width())?out_value:(*this)(x,y,z,c);
    }

    //! Access to a pixel value, using Neumann boundary conditions for the X-coordinate.
    /**
       Return a reference to the pixel value of the image instance located at (\c x,\c y,\c z,\c c),
       or to the nearest pixel location in the image instance in case of out-of-bounds access along the X-axis.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note
       - Similar to at(int,int,int,int,const T), except that an out-of-bounds access returns the value of the
         nearest pixel in the image instance, regarding the specified X-coordinate.
       - Due to the additional boundary checking operation, this method is slower than operator()(). Use it when
         you are \e not sure about the validity of the specified pixel coordinates.
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _at(int,int,int,int).
       \warning
       - There is \e no boundary checking performed for the Y,Z and C-coordinates, so they must be inside image bounds.
     **/
    T& atX(const int x, const int y=0, const int z=0, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atX(): Empty instance.",
                                    cimg_instance);
      return _atX(x,y,z,c);
    }

    T& _atX(const int x, const int y=0, const int z=0, const int c=0) {
      return (*this)(x<0?0:(x>=width()?width() - 1:x),y,z,c);
    }

    //! Access to a pixel value, using Neumann boundary conditions for the X-coordinate \const.
    const T& atX(const int x, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atX(): Empty instance.",
                                    cimg_instance);
      return _atX(x,y,z,c);
    }

    const T& _atX(const int x, const int y=0, const int z=0, const int c=0) const {
      return (*this)(x<0?0:(x>=width()?width() - 1:x),y,z,c);
    }

    //! Access to a pixel value, using Dirichlet boundary conditions for the X and Y-coordinates.
    /**
       Similar to atX(int,int,int,int,const T), except that boundary checking is performed both on X and Y-coordinates.
    **/
    T& atXY(const int x, const int y, const int z, const int c, const T& out_value) {
      return (x<0 || y<0 || x>=width() || y>=height())?(cimg::temporary(out_value)=out_value):(*this)(x,y,z,c);
    }

    //! Access to a pixel value, using Dirichlet boundary conditions for the X and Y coordinates \const.
    T atXY(const int x, const int y, const int z, const int c, const T& out_value) const {
      return (x<0 || y<0 || x>=width() || y>=height())?out_value:(*this)(x,y,z,c);
    }

    //! Access to a pixel value, using Neumann boundary conditions for the X and Y-coordinates.
    /**
       Similar to atX(int,int,int,int), except that boundary checking is performed both on X and Y-coordinates.
       \note
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _atXY(int,int,int,int).
     **/
    T& atXY(const int x, const int y, const int z=0, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXY(): Empty instance.",
                                    cimg_instance);
      return _atXY(x,y,z,c);
    }

    T& _atXY(const int x, const int y, const int z=0, const int c=0) {
      return (*this)(cimg::cut(x,0,width() - 1),
                     cimg::cut(y,0,height() - 1),z,c);
    }

    //! Access to a pixel value, using Neumann boundary conditions for the X and Y-coordinates \const.
    const T& atXY(const int x, const int y, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXY(): Empty instance.",
                                    cimg_instance);
      return _atXY(x,y,z,c);
    }

    const T& _atXY(const int x, const int y, const int z=0, const int c=0) const {
      return (*this)(cimg::cut(x,0,width() - 1),
                     cimg::cut(y,0,height() - 1),z,c);
    }

    //! Access to a pixel value, using Dirichlet boundary conditions for the X,Y and Z-coordinates.
    /**
       Similar to atX(int,int,int,int,const T), except that boundary checking is performed both on
       X,Y and Z-coordinates.
    **/
    T& atXYZ(const int x, const int y, const int z, const int c, const T& out_value) {
      return (x<0 || y<0 || z<0 || x>=width() || y>=height() || z>=depth())?
        (cimg::temporary(out_value)=out_value):(*this)(x,y,z,c);
    }

    //! Access to a pixel value, using Dirichlet boundary conditions for the X,Y and Z-coordinates \const.
    T atXYZ(const int x, const int y, const int z, const int c, const T& out_value) const {
      return (x<0 || y<0 || z<0 || x>=width() || y>=height() || z>=depth())?out_value:(*this)(x,y,z,c);
    }

    //! Access to a pixel value, using Neumann boundary conditions for the X,Y and Z-coordinates.
    /**
       Similar to atX(int,int,int,int), except that boundary checking is performed both on X,Y and Z-coordinates.
       \note
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _atXYZ(int,int,int,int).
    **/
    T& atXYZ(const int x, const int y, const int z, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXYZ(): Empty instance.",
                                    cimg_instance);
      return _atXYZ(x,y,z,c);
    }

    T& _atXYZ(const int x, const int y, const int z, const int c=0) {
      return (*this)(cimg::cut(x,0,width() - 1),
                     cimg::cut(y,0,height() - 1),
                     cimg::cut(z,0,depth() - 1),c);
    }

    //! Access to a pixel value, using Neumann boundary conditions for the X,Y and Z-coordinates \const.
    const T& atXYZ(const int x, const int y, const int z, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXYZ(): Empty instance.",
                                    cimg_instance);
      return _atXYZ(x,y,z,c);
    }

    const T& _atXYZ(const int x, const int y, const int z, const int c=0) const {
      return (*this)(cimg::cut(x,0,width() - 1),
                     cimg::cut(y,0,height() - 1),
                     cimg::cut(z,0,depth() - 1),c);
    }

    //! Access to a pixel value, using Dirichlet boundary conditions.
    /**
       Similar to atX(int,int,int,int,const T), except that boundary checking is performed on all
       X,Y,Z and C-coordinates.
    **/
    T& atXYZC(const int x, const int y, const int z, const int c, const T& out_value) {
      return (x<0 || y<0 || z<0 || c<0 || x>=width() || y>=height() || z>=depth() || c>=spectrum())?
        (cimg::temporary(out_value)=out_value):(*this)(x,y,z,c);
    }

    //! Access to a pixel value, using Dirichlet boundary conditions \const.
    T atXYZC(const int x, const int y, const int z, const int c, const T& out_value) const {
      return (x<0 || y<0 || z<0 || c<0 || x>=width() || y>=height() || z>=depth() || c>=spectrum())?out_value:
        (*this)(x,y,z,c);
    }

    //! Access to a pixel value, using Neumann boundary conditions.
    /**
       Similar to atX(int,int,int,int), except that boundary checking is performed on all X,Y,Z and C-coordinates.
       \note
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _atXYZC(int,int,int,int).
    **/
    T& atXYZC(const int x, const int y, const int z, const int c) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXYZC(): Empty instance.",
                                    cimg_instance);
      return _atXYZC(x,y,z,c);
    }

    T& _atXYZC(const int x, const int y, const int z, const int c) {
      return (*this)(cimg::cut(x,0,width() - 1),
                     cimg::cut(y,0,height() - 1),
                     cimg::cut(z,0,depth() - 1),
                     cimg::cut(c,0,spectrum() - 1));
    }

    //! Access to a pixel value, using Neumann boundary conditions \const.
    const T& atXYZC(const int x, const int y, const int z, const int c) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "atXYZC(): Empty instance.",
                                    cimg_instance);
      return _atXYZC(x,y,z,c);
    }

    const T& _atXYZC(const int x, const int y, const int z, const int c) const {
      return (*this)(cimg::cut(x,0,width() - 1),
                     cimg::cut(y,0,height() - 1),
                     cimg::cut(z,0,depth() - 1),
                     cimg::cut(c,0,spectrum() - 1));
    }

    //! Return pixel value, using linear interpolation and Dirichlet boundary conditions for the X-coordinate.
    /**
       Return a linearly-interpolated pixel value of the image instance located at (\c fx,\c y,\c z,\c c),
       or a specified default value in case of out-of-bounds access along the X-axis.
       \param fx X-coordinate of the pixel value (float-valued).
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param out_value Default value returned if \c (\c fx,\c y,\c z,\c c) is outside image bounds.
       \note
       - Similar to atX(int,int,int,int,const T), except that the returned pixel value is approximated by
         a linear interpolation along the X-axis, if corresponding coordinates are not integers.
       - The type of the returned pixel value is extended to \c float, if the pixel type \c T is not float-valued.
       \warning
       - There is \e no boundary checking performed for the Y,Z and C-coordinates, so they must be inside image bounds.
    **/
    Tfloat linear_atX(const float fx, const int y, const int z, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1;
      const float
        dx = fx - x;
      const Tfloat
        Ic = (Tfloat)atX(x,y,z,c,out_value), In = (Tfloat)atXY(nx,y,z,c,out_value);
      return Ic + dx*(In - Ic);
    }

    //! Return pixel value, using linear interpolation and Neumann boundary conditions for the X-coordinate.
    /**
       Return a linearly-interpolated pixel value of the image instance located at (\c fx,\c y,\c z,\c c),
       or the value of the nearest pixel location in the image instance in case of out-of-bounds access along
       the X-axis.
       \param fx X-coordinate of the pixel value (float-valued).
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note
       - Similar to linear_atX(float,int,int,int,const T) const, except that an out-of-bounds access returns
         the value of the nearest pixel in the image instance, regarding the specified X-coordinate.
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _linear_atX(float,int,int,int).
       \warning
       - There is \e no boundary checking performed for the Y,Z and C-coordinates, so they must be inside image bounds.
    **/
    Tfloat linear_atX(const float fx, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atX(): Empty instance.",
                                    cimg_instance);

      return _linear_atX(fx,y,z,c);
    }

    Tfloat _linear_atX(const float fx, const int y=0, const int z=0, const int c=0) const {
      const float
        nfx = cimg::cut(fx,0.f,width() - 1.f);
      const unsigned int
        x = (unsigned int)nfx;
      const float
        dx = nfx - x;
      const unsigned int
        nx = dx>0?x + 1:x;
      const Tfloat
        Ic = (Tfloat)(*this)(x,y,z,c), In = (Tfloat)(*this)(nx,y,z,c);
      return Ic + dx*(In - Ic);
    }

    //! Return pixel value, using linear interpolation and periodic boundary conditions for the X-coordinate.
    Tfloat linear_atX_p(const float fx, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atX_p(): Empty instance.",
                                    cimg_instance);

      return _linear_atX_p(fx,y,z,c);
    }

    Tfloat _linear_atX_p(const float fx, const int y=0, const int z=0, const int c=0) const {
      const float
        nfx = cimg::mod(fx,_width - 0.5f);
      const unsigned int
        x = (unsigned int)nfx;
      const float
        dx = nfx - x;
      const unsigned int
        nx = cimg::mod(x + 1,_width);
      const Tfloat
        Ic = (Tfloat)(*this)(x,y,z,c), In = (Tfloat)(*this)(nx,y,z,c);
      return Ic + dx*(In - Ic);
    }

    //! Return pixel value, using linear interpolation and Dirichlet boundary conditions for the X and Y-coordinates.
    /**
       Similar to linear_atX(float,int,int,int,const T) const, except that the linear interpolation and the
       boundary checking are achieved both for X and Y-coordinates.
    **/
    Tfloat linear_atXY(const float fx, const float fy, const int z, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1;
      const float
        dx = fx - x,
        dy = fy - y;
      const Tfloat
        Icc = (Tfloat)atXY(x,y,z,c,out_value),  Inc = (Tfloat)atXY(nx,y,z,c,out_value),
        Icn = (Tfloat)atXY(x,ny,z,c,out_value), Inn = (Tfloat)atXY(nx,ny,z,c,out_value);
      return Icc + (Inc - Icc + (Icc + Inn - Icn - Inc)*dy)*dx + (Icn - Icc)*dy;
    }

    //! Return pixel value, using linear interpolation and Neumann boundary conditions for the X and Y-coordinates.
    /**
       Similar to linear_atX(float,int,int,int) const, except that the linear interpolation and the boundary checking
       are achieved both for X and Y-coordinates.
       \note
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _linear_atXY(float,float,int,int).
    **/
    Tfloat linear_atXY(const float fx, const float fy, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXY(): Empty instance.",
                                    cimg_instance);

      return _linear_atXY(fx,fy,z,c);
    }

    Tfloat _linear_atXY(const float fx, const float fy, const int z=0, const int c=0) const {
      const float
        nfx = cimg::cut(fx,0.f,width() - 1.f),
        nfy = cimg::cut(fy,0.f,height() - 1.f);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy;
      const float
        dx = nfx - x,
        dy = nfy - y;
      const unsigned int
        nx = dx>0?x + 1:x,
        ny = dy>0?y + 1:y;
      const Tfloat
        Icc = (Tfloat)(*this)(x,y,z,c),  Inc = (Tfloat)(*this)(nx,y,z,c),
        Icn = (Tfloat)(*this)(x,ny,z,c), Inn = (Tfloat)(*this)(nx,ny,z,c);
      return Icc + (Inc - Icc + (Icc + Inn - Icn - Inc)*dy)*dx + (Icn - Icc)*dy;
    }

    //! Return pixel value, using linear interpolation and periodic boundary conditions for the X and Y-coordinates.
    Tfloat linear_atXY_p(const float fx, const float fy, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXY_p(): Empty instance.",
                                    cimg_instance);

      return _linear_atXY_p(fx,fy,z,c);
    }

    Tfloat _linear_atXY_p(const float fx, const float fy, const int z=0, const int c=0) const {
      const float
        nfx = cimg::mod(fx,_width - 0.5f),
        nfy = cimg::mod(fy,_height - 0.5f);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy;
      const float
        dx = nfx - x,
        dy = nfy - y;
      const unsigned int
        nx = cimg::mod(x + 1,_width),
        ny = cimg::mod(y + 1,_height);
      const Tfloat
        Icc = (Tfloat)(*this)(x,y,z,c),  Inc = (Tfloat)(*this)(nx,y,z,c),
        Icn = (Tfloat)(*this)(x,ny,z,c), Inn = (Tfloat)(*this)(nx,ny,z,c);
      return Icc + (Inc - Icc + (Icc + Inn - Icn - Inc)*dy)*dx + (Icn - Icc)*dy;
    }

    //! Return pixel value, using linear interpolation and Dirichlet boundary conditions for the X,Y and Z-coordinates.
    /**
       Similar to linear_atX(float,int,int,int,const T) const, except that the linear interpolation and the
       boundary checking are achieved both for X,Y and Z-coordinates.
    **/
    Tfloat linear_atXYZ(const float fx, const float fy, const float fz, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1,
        z = (int)fz - (fz>=0?0:1), nz = z + 1;
      const float
        dx = fx - x,
        dy = fy - y,
        dz = fz - z;
      const Tfloat
        Iccc = (Tfloat)atXYZ(x,y,z,c,out_value), Incc = (Tfloat)atXYZ(nx,y,z,c,out_value),
        Icnc = (Tfloat)atXYZ(x,ny,z,c,out_value), Innc = (Tfloat)atXYZ(nx,ny,z,c,out_value),
        Iccn = (Tfloat)atXYZ(x,y,nz,c,out_value), Incn = (Tfloat)atXYZ(nx,y,nz,c,out_value),
        Icnn = (Tfloat)atXYZ(x,ny,nz,c,out_value), Innn = (Tfloat)atXYZ(nx,ny,nz,c,out_value);
      return Iccc +
        (Incc - Iccc +
         (Iccc + Innc - Icnc - Incc +
          (Iccn + Innn + Icnc + Incc - Icnn - Incn - Iccc - Innc)*dz)*dy +
         (Iccc + Incn - Iccn - Incc)*dz)*dx +
        (Icnc - Iccc +
         (Iccc + Icnn - Iccn - Icnc)*dz)*dy +
        (Iccn - Iccc)*dz;
    }

    //! Return pixel value, using linear interpolation and Neumann boundary conditions for the X,Y and Z-coordinates.
    /**
       Similar to linear_atX(float,int,int,int) const, except that the linear interpolation and the boundary checking
       are achieved both for X,Y and Z-coordinates.
       \note
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _linear_atXYZ(float,float,float,int).
    **/
    Tfloat linear_atXYZ(const float fx, const float fy=0, const float fz=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXYZ(): Empty instance.",
                                    cimg_instance);

      return _linear_atXYZ(fx,fy,fz,c);
    }

    Tfloat _linear_atXYZ(const float fx, const float fy=0, const float fz=0, const int c=0) const {
      const float
        nfx = cimg::cut(fx,0.f,width() - 1.f),
        nfy = cimg::cut(fy,0.f,height() - 1.f),
        nfz = cimg::cut(fz,0.f,depth() - 1.f);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy,
        z = (unsigned int)nfz;
      const float
        dx = nfx - x,
        dy = nfy - y,
        dz = nfz - z;
      const unsigned int
        nx = dx>0?x + 1:x,
        ny = dy>0?y + 1:y,
        nz = dz>0?z + 1:z;
      const Tfloat
        Iccc = (Tfloat)(*this)(x,y,z,c), Incc = (Tfloat)(*this)(nx,y,z,c),
        Icnc = (Tfloat)(*this)(x,ny,z,c), Innc = (Tfloat)(*this)(nx,ny,z,c),
        Iccn = (Tfloat)(*this)(x,y,nz,c), Incn = (Tfloat)(*this)(nx,y,nz,c),
        Icnn = (Tfloat)(*this)(x,ny,nz,c), Innn = (Tfloat)(*this)(nx,ny,nz,c);
      return Iccc +
        (Incc - Iccc +
         (Iccc + Innc - Icnc - Incc +
          (Iccn + Innn + Icnc + Incc - Icnn - Incn - Iccc - Innc)*dz)*dy +
         (Iccc + Incn - Iccn - Incc)*dz)*dx +
        (Icnc - Iccc +
         (Iccc + Icnn - Iccn - Icnc)*dz)*dy +
        (Iccn - Iccc)*dz;
    }

    //! Return pixel value, using linear interpolation and periodic boundary conditions for the X,Y and Z-coordinates.
    Tfloat linear_atXYZ_p(const float fx, const float fy=0, const float fz=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXYZ_p(): Empty instance.",
                                    cimg_instance);

      return _linear_atXYZ_p(fx,fy,fz,c);
    }

    Tfloat _linear_atXYZ_p(const float fx, const float fy=0, const float fz=0, const int c=0) const {
      const float
        nfx = cimg::mod(fx,_width - 0.5f),
        nfy = cimg::mod(fy,_height - 0.5f),
        nfz = cimg::mod(fz,_depth - 0.5f);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy,
        z = (unsigned int)nfz;
      const float
        dx = nfx - x,
        dy = nfy - y,
        dz = nfz - z;
      const unsigned int
        nx = cimg::mod(x + 1,_width),
        ny = cimg::mod(y + 1,_height),
        nz = cimg::mod(z + 1,_depth);
      const Tfloat
        Iccc = (Tfloat)(*this)(x,y,z,c), Incc = (Tfloat)(*this)(nx,y,z,c),
        Icnc = (Tfloat)(*this)(x,ny,z,c), Innc = (Tfloat)(*this)(nx,ny,z,c),
        Iccn = (Tfloat)(*this)(x,y,nz,c), Incn = (Tfloat)(*this)(nx,y,nz,c),
        Icnn = (Tfloat)(*this)(x,ny,nz,c), Innn = (Tfloat)(*this)(nx,ny,nz,c);
      return Iccc +
        (Incc - Iccc +
         (Iccc + Innc - Icnc - Incc +
          (Iccn + Innn + Icnc + Incc - Icnn - Incn - Iccc - Innc)*dz)*dy +
         (Iccc + Incn - Iccn - Incc)*dz)*dx +
        (Icnc - Iccc +
         (Iccc + Icnn - Iccn - Icnc)*dz)*dy +
        (Iccn - Iccc)*dz;
    }

    //! Return pixel value, using linear interpolation and Dirichlet boundary conditions for all X,Y,Z,C-coordinates.
    /**
       Similar to linear_atX(float,int,int,int,const T) const, except that the linear interpolation and the
       boundary checking are achieved for all X,Y,Z and C-coordinates.
    **/
    Tfloat linear_atXYZC(const float fx, const float fy, const float fz, const float fc, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1,
        z = (int)fz - (fz>=0?0:1), nz = z + 1,
        c = (int)fc - (fc>=0?0:1), nc = c + 1;
      const float
        dx = fx - x,
        dy = fy - y,
        dz = fz - z,
        dc = fc - c;
      const Tfloat
        Icccc = (Tfloat)atXYZC(x,y,z,c,out_value), Inccc = (Tfloat)atXYZC(nx,y,z,c,out_value),
        Icncc = (Tfloat)atXYZC(x,ny,z,c,out_value), Inncc = (Tfloat)atXYZC(nx,ny,z,c,out_value),
        Iccnc = (Tfloat)atXYZC(x,y,nz,c,out_value), Incnc = (Tfloat)atXYZC(nx,y,nz,c,out_value),
        Icnnc = (Tfloat)atXYZC(x,ny,nz,c,out_value), Innnc = (Tfloat)atXYZC(nx,ny,nz,c,out_value),
        Icccn = (Tfloat)atXYZC(x,y,z,nc,out_value), Inccn = (Tfloat)atXYZC(nx,y,z,nc,out_value),
        Icncn = (Tfloat)atXYZC(x,ny,z,nc,out_value), Inncn = (Tfloat)atXYZC(nx,ny,z,nc,out_value),
        Iccnn = (Tfloat)atXYZC(x,y,nz,nc,out_value), Incnn = (Tfloat)atXYZC(nx,y,nz,nc,out_value),
        Icnnn = (Tfloat)atXYZC(x,ny,nz,nc,out_value), Innnn = (Tfloat)atXYZC(nx,ny,nz,nc,out_value);
      return Icccc +
        dx*(Inccc - Icccc +
            dy*(Icccc + Inncc - Icncc - Inccc +
                dz*(Iccnc + Innnc + Icncc + Inccc - Icnnc - Incnc - Icccc - Inncc +
                    dc*(Iccnn + Innnn + Icncn + Inccn + Icnnc + Incnc + Icccc + Inncc -
                        Icnnn - Incnn - Icccn - Inncn - Iccnc - Innnc - Icncc - Inccc)) +
                dc*(Icccn + Inncn + Icncc + Inccc - Icncn - Inccn - Icccc - Inncc)) +
            dz*(Icccc + Incnc - Iccnc - Inccc +
                dc*(Icccn + Incnn + Iccnc + Inccc - Iccnn - Inccn - Icccc - Incnc)) +
            dc*(Icccc + Inccn - Inccc - Icccn)) +
        dy*(Icncc - Icccc +
            dz*(Icccc + Icnnc - Iccnc - Icncc +
                dc*(Icccn + Icnnn + Iccnc + Icncc - Iccnn - Icncn - Icccc - Icnnc)) +
            dc*(Icccc + Icncn - Icncc - Icccn)) +
        dz*(Iccnc - Icccc +
            dc*(Icccc + Iccnn - Iccnc - Icccn)) +
        dc*(Icccn  -Icccc);
    }

    //! Return pixel value, using linear interpolation and Neumann boundary conditions for all X,Y,Z and C-coordinates.
    /**
       Similar to linear_atX(float,int,int,int) const, except that the linear interpolation and the boundary checking
       are achieved for all X,Y,Z and C-coordinates.
       \note
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _linear_atXYZC(float,float,float,float).
    **/
    Tfloat linear_atXYZC(const float fx, const float fy=0, const float fz=0, const float fc=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXYZC(): Empty instance.",
                                    cimg_instance);

      return _linear_atXYZC(fx,fy,fz,fc);
    }

    Tfloat _linear_atXYZC(const float fx, const float fy=0, const float fz=0, const float fc=0) const {
      const float
        nfx = cimg::cut(fx,0.f,width() - 1.f),
        nfy = cimg::cut(fy,0.f,height() - 1.f),
        nfz = cimg::cut(fz,0.f,depth() - 1.f),
        nfc = cimg::cut(fc,0.f,spectrum() - 1.f);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy,
        z = (unsigned int)nfz,
        c = (unsigned int)nfc;
      const float
        dx = nfx - x,
        dy = nfy - y,
        dz = nfz - z,
        dc = nfc - c;
      const unsigned int
        nx = dx>0?x + 1:x,
        ny = dy>0?y + 1:y,
        nz = dz>0?z + 1:z,
        nc = dc>0?c + 1:c;
      const Tfloat
        Icccc = (Tfloat)(*this)(x,y,z,c), Inccc = (Tfloat)(*this)(nx,y,z,c),
        Icncc = (Tfloat)(*this)(x,ny,z,c), Inncc = (Tfloat)(*this)(nx,ny,z,c),
        Iccnc = (Tfloat)(*this)(x,y,nz,c), Incnc = (Tfloat)(*this)(nx,y,nz,c),
        Icnnc = (Tfloat)(*this)(x,ny,nz,c), Innnc = (Tfloat)(*this)(nx,ny,nz,c),
        Icccn = (Tfloat)(*this)(x,y,z,nc), Inccn = (Tfloat)(*this)(nx,y,z,nc),
        Icncn = (Tfloat)(*this)(x,ny,z,nc), Inncn = (Tfloat)(*this)(nx,ny,z,nc),
        Iccnn = (Tfloat)(*this)(x,y,nz,nc), Incnn = (Tfloat)(*this)(nx,y,nz,nc),
        Icnnn = (Tfloat)(*this)(x,ny,nz,nc), Innnn = (Tfloat)(*this)(nx,ny,nz,nc);
      return Icccc +
        dx*(Inccc - Icccc +
            dy*(Icccc + Inncc - Icncc - Inccc +
                dz*(Iccnc + Innnc + Icncc + Inccc - Icnnc - Incnc - Icccc - Inncc +
                    dc*(Iccnn + Innnn + Icncn + Inccn + Icnnc + Incnc + Icccc + Inncc -
                        Icnnn - Incnn - Icccn - Inncn - Iccnc - Innnc - Icncc - Inccc)) +
                dc*(Icccn + Inncn + Icncc + Inccc - Icncn - Inccn - Icccc - Inncc)) +
            dz*(Icccc + Incnc - Iccnc - Inccc +
                dc*(Icccn + Incnn + Iccnc + Inccc - Iccnn - Inccn - Icccc - Incnc)) +
            dc*(Icccc + Inccn - Inccc - Icccn)) +
        dy*(Icncc - Icccc +
            dz*(Icccc + Icnnc - Iccnc - Icncc +
                dc*(Icccn + Icnnn + Iccnc + Icncc - Iccnn - Icncn - Icccc - Icnnc)) +
            dc*(Icccc + Icncn - Icncc - Icccn)) +
        dz*(Iccnc - Icccc +
            dc*(Icccc + Iccnn - Iccnc - Icccn)) +
        dc*(Icccn - Icccc);
    }

    //! Return pixel value, using linear interpolation and periodic boundary conditions for all X,Y,Z and C-coordinates.
    Tfloat linear_atXYZC_p(const float fx, const float fy=0, const float fz=0, const float fc=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "linear_atXYZC_p(): Empty instance.",
                                    cimg_instance);

      return _linear_atXYZC_p(fx,fy,fz,fc);
    }

    Tfloat _linear_atXYZC_p(const float fx, const float fy=0, const float fz=0, const float fc=0) const {
      const float
        nfx = cimg::mod(fx,_width - 0.5f),
        nfy = cimg::mod(fy,_height - 0.5f),
        nfz = cimg::mod(fz,_depth - 0.5f),
        nfc = cimg::mod(fc,_spectrum - 0.5f);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy,
        z = (unsigned int)nfz,
        c = (unsigned int)nfc;
      const float
        dx = nfx - x,
        dy = nfy - y,
        dz = nfz - z,
        dc = nfc - c;
      const unsigned int
        nx = cimg::mod(x + 1,_width),
        ny = cimg::mod(y + 1,_height),
        nz = cimg::mod(z + 1,_depth),
        nc = cimg::mod(c + 1,_spectrum);
      const Tfloat
        Icccc = (Tfloat)(*this)(x,y,z,c), Inccc = (Tfloat)(*this)(nx,y,z,c),
        Icncc = (Tfloat)(*this)(x,ny,z,c), Inncc = (Tfloat)(*this)(nx,ny,z,c),
        Iccnc = (Tfloat)(*this)(x,y,nz,c), Incnc = (Tfloat)(*this)(nx,y,nz,c),
        Icnnc = (Tfloat)(*this)(x,ny,nz,c), Innnc = (Tfloat)(*this)(nx,ny,nz,c),
        Icccn = (Tfloat)(*this)(x,y,z,nc), Inccn = (Tfloat)(*this)(nx,y,z,nc),
        Icncn = (Tfloat)(*this)(x,ny,z,nc), Inncn = (Tfloat)(*this)(nx,ny,z,nc),
        Iccnn = (Tfloat)(*this)(x,y,nz,nc), Incnn = (Tfloat)(*this)(nx,y,nz,nc),
        Icnnn = (Tfloat)(*this)(x,ny,nz,nc), Innnn = (Tfloat)(*this)(nx,ny,nz,nc);
      return Icccc +
        dx*(Inccc - Icccc +
            dy*(Icccc + Inncc - Icncc - Inccc +
                dz*(Iccnc + Innnc + Icncc + Inccc - Icnnc - Incnc - Icccc - Inncc +
                    dc*(Iccnn + Innnn + Icncn + Inccn + Icnnc + Incnc + Icccc + Inncc -
                        Icnnn - Incnn - Icccn - Inncn - Iccnc - Innnc - Icncc - Inccc)) +
                dc*(Icccn + Inncn + Icncc + Inccc - Icncn - Inccn - Icccc - Inncc)) +
            dz*(Icccc + Incnc - Iccnc - Inccc +
                dc*(Icccn + Incnn + Iccnc + Inccc - Iccnn - Inccn - Icccc - Incnc)) +
            dc*(Icccc + Inccn - Inccc - Icccn)) +
        dy*(Icncc - Icccc +
            dz*(Icccc + Icnnc - Iccnc - Icncc +
                dc*(Icccn + Icnnn + Iccnc + Icncc - Iccnn - Icncn - Icccc - Icnnc)) +
            dc*(Icccc + Icncn - Icncc - Icccn)) +
        dz*(Iccnc - Icccc +
            dc*(Icccc + Iccnn - Iccnc - Icccn)) +
        dc*(Icccn - Icccc);
    }

    //! Return pixel value, using cubic interpolation and Dirichlet boundary conditions for the X-coordinate.
    /**
       Return a cubicly-interpolated pixel value of the image instance located at (\c fx,\c y,\c z,\c c),
       or a specified default value in case of out-of-bounds access along the X-axis.
       The cubic interpolation uses Hermite splines.
       \param fx d X-coordinate of the pixel value (float-valued).
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param out_value Default value returned if \c (\c fx,\c y,\c z,\c c) is outside image bounds.
       \note
       - Similar to linear_atX(float,int,int,int,const T) const, except that the returned pixel value is
         approximated by a \e cubic interpolation along the X-axis.
       - The type of the returned pixel value is extended to \c float, if the pixel type \c T is not float-valued.
       \warning
       - There is \e no boundary checking performed for the Y,Z and C-coordinates, so they must be inside image bounds.
    **/
    Tfloat cubic_atX(const float fx, const int y, const int z, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), px = x - 1, nx = x + 1, ax = x + 2;
      const float
        dx = fx - x;
      const Tfloat
        Ip = (Tfloat)atX(px,y,z,c,out_value), Ic = (Tfloat)atX(x,y,z,c,out_value),
        In = (Tfloat)atX(nx,y,z,c,out_value), Ia = (Tfloat)atX(ax,y,z,c,out_value);
      return Ic + 0.5f*(dx*(-Ip + In) + dx*dx*(2*Ip - 5*Ic + 4*In - Ia) + dx*dx*dx*(-Ip + 3*Ic - 3*In + Ia));
    }

    //! Return clamped pixel value, using cubic interpolation and Dirichlet boundary conditions for the X-coordinate.
    /**
       Similar to cubic_atX(float,int,int,int,const T) const, except that the return value is clamped to stay in the
       min/max range of the datatype \c T.
    **/
    T cubic_atX_c(const float fx, const int y, const int z, const int c, const T& out_value) const {
      return cimg::type<T>::cut(cubic_atX(fx,y,z,c,out_value));
    }

    //! Return pixel value, using cubic interpolation and Neumann boundary conditions for the X-coordinate.
    /**
       Return a cubicly-interpolated pixel value of the image instance located at (\c fx,\c y,\c z,\c c),
       or the value of the nearest pixel location in the image instance in case of out-of-bounds access
       along the X-axis. The cubic interpolation uses Hermite splines.
       \param fx X-coordinate of the pixel value (float-valued).
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note
       - Similar to cubic_atX(float,int,int,int,const T) const, except that the returned pixel value is
         approximated by a cubic interpolation along the X-axis.
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _cubic_atX(float,int,int,int).
       \warning
       - There is \e no boundary checking performed for the Y,Z and C-coordinates, so they must be inside image bounds.
    **/
    Tfloat cubic_atX(const float fx, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atX(): Empty instance.",
                                    cimg_instance);
      return _cubic_atX(fx,y,z,c);
    }

    Tfloat _cubic_atX(const float fx, const int y=0, const int z=0, const int c=0) const {
      const float
        nfx = cimg::type<float>::is_nan(fx)?0:cimg::cut(fx,0.f,width() - 1.f);
      const int
        x = (int)nfx;
      const float
        dx = nfx - x;
      const int
        px = x - 1<0?0:x - 1, nx = dx>0?x + 1:x, ax = x + 2>=width()?width() - 1:x + 2;
      const Tfloat
        Ip = (Tfloat)(*this)(px,y,z,c), Ic = (Tfloat)(*this)(x,y,z,c),
        In = (Tfloat)(*this)(nx,y,z,c), Ia = (Tfloat)(*this)(ax,y,z,c);
      return Ic + 0.5f*(dx*(-Ip + In) + dx*dx*(2*Ip - 5*Ic + 4*In - Ia) + dx*dx*dx*(-Ip + 3*Ic - 3*In + Ia));
    }

    //! Return clamped pixel value, using cubic interpolation and Neumann boundary conditions for the X-coordinate.
    /**
       Similar to cubic_atX(float,int,int,int) const, except that the return value is clamped to stay in the
       min/max range of the datatype \c T.
    **/
    T cubic_atX_c(const float fx, const int y, const int z, const int c) const {
      return cimg::type<T>::cut(cubic_atX(fx,y,z,c));
    }

    T _cubic_atX_c(const float fx, const int y, const int z, const int c) const {
      return cimg::type<T>::cut(_cubic_atX(fx,y,z,c));
    }

    //! Return pixel value, using cubic interpolation and periodic boundary conditions for the X-coordinate.
    Tfloat cubic_atX_p(const float fx, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atX_p(): Empty instance.",
                                    cimg_instance);
      return _cubic_atX_p(fx,y,z,c);
    }

    Tfloat _cubic_atX_p(const float fx, const int y=0, const int z=0, const int c=0) const {
      const float
        nfx = cimg::type<float>::is_nan(fx)?0:cimg::mod(fx,_width - 0.5f);
      const int
        x = (int)nfx;
      const float
        dx = nfx - x;
      const int
        px = cimg::mod(x - 1,width()), nx = cimg::mod(x + 1,width()), ax = cimg::mod(x + 2,width());
      const Tfloat
        Ip = (Tfloat)(*this)(px,y,z,c), Ic = (Tfloat)(*this)(x,y,z,c),
        In = (Tfloat)(*this)(nx,y,z,c), Ia = (Tfloat)(*this)(ax,y,z,c);
      return Ic + 0.5f*(dx*(-Ip + In) + dx*dx*(2*Ip - 5*Ic + 4*In - Ia) + dx*dx*dx*(-Ip + 3*Ic - 3*In + Ia));
    }

    T cubic_atX_pc(const float fx, const int y, const int z, const int c) const {
      return cimg::type<T>::cut(cubic_atX_p(fx,y,z,c));
    }

    T _cubic_atX_pc(const float fx, const int y, const int z, const int c) const {
      return cimg::type<T>::cut(_cubic_atX_p(fx,y,z,c));
    }

    //! Return pixel value, using cubic interpolation and Dirichlet boundary conditions for the X and Y-coordinates.
    /**
       Similar to cubic_atX(float,int,int,int,const T) const, except that the cubic interpolation and boundary checking
       are achieved both for X and Y-coordinates.
    **/
    Tfloat cubic_atXY(const float fx, const float fy, const int z, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), px = x - 1, nx = x + 1, ax = x + 2,
        y = (int)fy - (fy>=0?0:1), py = y - 1, ny = y + 1, ay = y + 2;
      const float dx = fx - x, dy = fy - y;
      const Tfloat
        Ipp = (Tfloat)atXY(px,py,z,c,out_value), Icp = (Tfloat)atXY(x,py,z,c,out_value),
        Inp = (Tfloat)atXY(nx,py,z,c,out_value), Iap = (Tfloat)atXY(ax,py,z,c,out_value),
        Ip = Icp + 0.5f*(dx*(-Ipp + Inp) + dx*dx*(2*Ipp - 5*Icp + 4*Inp - Iap) + dx*dx*dx*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ipc = (Tfloat)atXY(px,y,z,c,out_value),  Icc = (Tfloat)atXY(x, y,z,c,out_value),
        Inc = (Tfloat)atXY(nx,y,z,c,out_value),  Iac = (Tfloat)atXY(ax,y,z,c,out_value),
        Ic = Icc + 0.5f*(dx*(-Ipc + Inc) + dx*dx*(2*Ipc - 5*Icc + 4*Inc - Iac) + dx*dx*dx*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ipn = (Tfloat)atXY(px,ny,z,c,out_value), Icn = (Tfloat)atXY(x,ny,z,c,out_value),
        Inn = (Tfloat)atXY(nx,ny,z,c,out_value), Ian = (Tfloat)atXY(ax,ny,z,c,out_value),
        In = Icn + 0.5f*(dx*(-Ipn + Inn) + dx*dx*(2*Ipn - 5*Icn + 4*Inn - Ian) + dx*dx*dx*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ipa = (Tfloat)atXY(px,ay,z,c,out_value), Ica = (Tfloat)atXY(x,ay,z,c,out_value),
        Ina = (Tfloat)atXY(nx,ay,z,c,out_value), Iaa = (Tfloat)atXY(ax,ay,z,c,out_value),
        Ia = Ica + 0.5f*(dx*(-Ipa + Ina) + dx*dx*(2*Ipa - 5*Ica + 4*Ina - Iaa) + dx*dx*dx*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dy*(-Ip + In) + dy*dy*(2*Ip - 5*Ic + 4*In - Ia) + dy*dy*dy*(-Ip + 3*Ic - 3*In + Ia));
    }

    //! Return clamped pixel value, using cubic interpolation and Dirichlet boundary conditions for the X,Y-coordinates.
    /**
       Similar to cubic_atXY(float,float,int,int,const T) const, except that the return value is clamped to stay in the
       min/max range of the datatype \c T.
    **/
    T cubic_atXY_c(const float fx, const float fy, const int z, const int c, const T& out_value) const {
      return cimg::type<T>::cut(cubic_atXY(fx,fy,z,c,out_value));
    }

    //! Return pixel value, using cubic interpolation and Neumann boundary conditions for the X and Y-coordinates.
    /**
       Similar to cubic_atX(float,int,int,int) const, except that the cubic interpolation and boundary checking
       are achieved for both X and Y-coordinates.
       \note
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
       \c _cubic_atXY(float,float,int,int).
    **/
    Tfloat cubic_atXY(const float fx, const float fy, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atXY(): Empty instance.",
                                    cimg_instance);
      return _cubic_atXY(fx,fy,z,c);
    }

    Tfloat _cubic_atXY(const float fx, const float fy, const int z=0, const int c=0) const {
      const float
        nfx = cimg::type<float>::is_nan(fx)?0:cimg::cut(fx,0.f,width() - 1.f),
        nfy = cimg::type<float>::is_nan(fy)?0:cimg::cut(fy,0.f,height() - 1.f);
      const int x = (int)nfx, y = (int)nfy;
      const float dx = nfx - x, dy = nfy - y;
      const int
        px = x - 1<0?0:x - 1, nx = dx<=0?x:x + 1, ax = x + 2>=width()?width() - 1:x + 2,
        py = y - 1<0?0:y - 1, ny = dy<=0?y:y + 1, ay = y + 2>=height()?height() - 1:y + 2;
      const Tfloat
        Ipp = (Tfloat)(*this)(px,py,z,c), Icp = (Tfloat)(*this)(x,py,z,c), Inp = (Tfloat)(*this)(nx,py,z,c),
        Iap = (Tfloat)(*this)(ax,py,z,c),
        Ip = Icp + 0.5f*(dx*(-Ipp + Inp) + dx*dx*(2*Ipp - 5*Icp + 4*Inp - Iap) + dx*dx*dx*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ipc = (Tfloat)(*this)(px,y,z,c),  Icc = (Tfloat)(*this)(x, y,z,c), Inc = (Tfloat)(*this)(nx,y,z,c),
        Iac = (Tfloat)(*this)(ax,y,z,c),
        Ic = Icc + 0.5f*(dx*(-Ipc + Inc) + dx*dx*(2*Ipc - 5*Icc + 4*Inc - Iac) + dx*dx*dx*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ipn = (Tfloat)(*this)(px,ny,z,c), Icn = (Tfloat)(*this)(x,ny,z,c), Inn = (Tfloat)(*this)(nx,ny,z,c),
        Ian = (Tfloat)(*this)(ax,ny,z,c),
        In = Icn + 0.5f*(dx*(-Ipn + Inn) + dx*dx*(2*Ipn - 5*Icn + 4*Inn - Ian) + dx*dx*dx*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ipa = (Tfloat)(*this)(px,ay,z,c), Ica = (Tfloat)(*this)(x,ay,z,c), Ina = (Tfloat)(*this)(nx,ay,z,c),
        Iaa = (Tfloat)(*this)(ax,ay,z,c),
        Ia = Ica + 0.5f*(dx*(-Ipa + Ina) + dx*dx*(2*Ipa - 5*Ica + 4*Ina - Iaa) + dx*dx*dx*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dy*(-Ip + In) + dy*dy*(2*Ip - 5*Ic + 4*In - Ia) + dy*dy*dy*(-Ip + 3*Ic - 3*In + Ia));
    }

    //! Return clamped pixel value, using cubic interpolation and Neumann boundary conditions for the X,Y-coordinates.
    /**
       Similar to cubic_atXY(float,float,int,int) const, except that the return value is clamped to stay in the
       min/max range of the datatype \c T.
    **/
    T cubic_atXY_c(const float fx, const float fy, const int z, const int c) const {
      return cimg::type<T>::cut(cubic_atXY(fx,fy,z,c));
    }

    T _cubic_atXY_c(const float fx, const float fy, const int z, const int c) const {
      return cimg::type<T>::cut(_cubic_atXY(fx,fy,z,c));
    }

    //! Return pixel value, using cubic interpolation and periodic boundary conditions for the X and Y-coordinates.
    Tfloat cubic_atXY_p(const float fx, const float fy, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atXY_p(): Empty instance.",
                                    cimg_instance);
      return _cubic_atXY_p(fx,fy,z,c);
    }

    Tfloat _cubic_atXY_p(const float fx, const float fy, const int z=0, const int c=0) const {
      const float
        nfx = cimg::type<float>::is_nan(fx)?0:cimg::mod(fx,_width - 0.5f),
        nfy = cimg::type<float>::is_nan(fy)?0:cimg::mod(fy,_height - 0.5f);
      const int x = (int)nfx, y = (int)nfy;
      const float dx = nfx - x, dy = nfy - y;
      const int
        px = cimg::mod(x - 1,width()), nx = cimg::mod(x + 1,width()), ax = cimg::mod(x + 2,width()),
        py = cimg::mod(y - 1,height()), ny = cimg::mod(y + 1,height()), ay = cimg::mod(y + 2,height());
      const Tfloat
        Ipp = (Tfloat)(*this)(px,py,z,c), Icp = (Tfloat)(*this)(x,py,z,c), Inp = (Tfloat)(*this)(nx,py,z,c),
        Iap = (Tfloat)(*this)(ax,py,z,c),
        Ip = Icp + 0.5f*(dx*(-Ipp + Inp) + dx*dx*(2*Ipp - 5*Icp + 4*Inp - Iap) + dx*dx*dx*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ipc = (Tfloat)(*this)(px,y,z,c),  Icc = (Tfloat)(*this)(x, y,z,c), Inc = (Tfloat)(*this)(nx,y,z,c),
        Iac = (Tfloat)(*this)(ax,y,z,c),
        Ic = Icc + 0.5f*(dx*(-Ipc + Inc) + dx*dx*(2*Ipc - 5*Icc + 4*Inc - Iac) + dx*dx*dx*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ipn = (Tfloat)(*this)(px,ny,z,c), Icn = (Tfloat)(*this)(x,ny,z,c), Inn = (Tfloat)(*this)(nx,ny,z,c),
        Ian = (Tfloat)(*this)(ax,ny,z,c),
        In = Icn + 0.5f*(dx*(-Ipn + Inn) + dx*dx*(2*Ipn - 5*Icn + 4*Inn - Ian) + dx*dx*dx*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ipa = (Tfloat)(*this)(px,ay,z,c), Ica = (Tfloat)(*this)(x,ay,z,c), Ina = (Tfloat)(*this)(nx,ay,z,c),
        Iaa = (Tfloat)(*this)(ax,ay,z,c),
        Ia = Ica + 0.5f*(dx*(-Ipa + Ina) + dx*dx*(2*Ipa - 5*Ica + 4*Ina - Iaa) + dx*dx*dx*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dy*(-Ip + In) + dy*dy*(2*Ip - 5*Ic + 4*In - Ia) + dy*dy*dy*(-Ip + 3*Ic - 3*In + Ia));
    }

    T cubic_atXY_pc(const float fx, const float fy, const int z, const int c) const {
      return cimg::type<T>::cut(cubic_atXY_p(fx,fy,z,c));
    }

    T _cubic_atXY_pc(const float fx, const float fy, const int z, const int c) const {
      return cimg::type<T>::cut(_cubic_atXY_p(fx,fy,z,c));
    }

    //! Return pixel value, using cubic interpolation and Dirichlet boundary conditions for the X,Y and Z-coordinates.
    /**
       Similar to cubic_atX(float,int,int,int,const T) const, except that the cubic interpolation and boundary checking
       are achieved both for X,Y and Z-coordinates.
    **/
    Tfloat cubic_atXYZ(const float fx, const float fy, const float fz, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), px = x - 1, nx = x + 1, ax = x + 2,
        y = (int)fy - (fy>=0?0:1), py = y - 1, ny = y + 1, ay = y + 2,
        z = (int)fz - (fz>=0?0:1), pz = z - 1, nz = z + 1, az = z + 2;
      const float dx = fx - x, dy = fy - y, dz = fz - z;
      const Tfloat
        Ippp = (Tfloat)atXYZ(px,py,pz,c,out_value), Icpp = (Tfloat)atXYZ(x,py,pz,c,out_value),
        Inpp = (Tfloat)atXYZ(nx,py,pz,c,out_value), Iapp = (Tfloat)atXYZ(ax,py,pz,c,out_value),
        Ipp = Icpp + 0.5f*(dx*(-Ippp + Inpp) + dx*dx*(2*Ippp - 5*Icpp + 4*Inpp - Iapp) +
                           dx*dx*dx*(-Ippp + 3*Icpp - 3*Inpp + Iapp)),
        Ipcp = (Tfloat)atXYZ(px,y,pz,c,out_value),  Iccp = (Tfloat)atXYZ(x, y,pz,c,out_value),
        Incp = (Tfloat)atXYZ(nx,y,pz,c,out_value),  Iacp = (Tfloat)atXYZ(ax,y,pz,c,out_value),
        Icp = Iccp + 0.5f*(dx*(-Ipcp + Incp) + dx*dx*(2*Ipcp - 5*Iccp + 4*Incp - Iacp) +
                           dx*dx*dx*(-Ipcp + 3*Iccp - 3*Incp + Iacp)),
        Ipnp = (Tfloat)atXYZ(px,ny,pz,c,out_value), Icnp = (Tfloat)atXYZ(x,ny,pz,c,out_value),
        Innp = (Tfloat)atXYZ(nx,ny,pz,c,out_value), Ianp = (Tfloat)atXYZ(ax,ny,pz,c,out_value),
        Inp = Icnp + 0.5f*(dx*(-Ipnp + Innp) + dx*dx*(2*Ipnp - 5*Icnp + 4*Innp - Ianp) +
                           dx*dx*dx*(-Ipnp + 3*Icnp - 3*Innp + Ianp)),
        Ipap = (Tfloat)atXYZ(px,ay,pz,c,out_value), Icap = (Tfloat)atXYZ(x,ay,pz,c,out_value),
        Inap = (Tfloat)atXYZ(nx,ay,pz,c,out_value), Iaap = (Tfloat)atXYZ(ax,ay,pz,c,out_value),
        Iap = Icap + 0.5f*(dx*(-Ipap + Inap) + dx*dx*(2*Ipap - 5*Icap + 4*Inap - Iaap) +
                           dx*dx*dx*(-Ipap + 3*Icap - 3*Inap + Iaap)),
        Ip = Icp + 0.5f*(dy*(-Ipp + Inp) + dy*dy*(2*Ipp - 5*Icp + 4*Inp - Iap) +
                         dy*dy*dy*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ippc = (Tfloat)atXYZ(px,py,z,c,out_value), Icpc = (Tfloat)atXYZ(x,py,z,c,out_value),
        Inpc = (Tfloat)atXYZ(nx,py,z,c,out_value), Iapc = (Tfloat)atXYZ(ax,py,z,c,out_value),
        Ipc = Icpc + 0.5f*(dx*(-Ippc + Inpc) + dx*dx*(2*Ippc - 5*Icpc + 4*Inpc - Iapc) +
                           dx*dx*dx*(-Ippc + 3*Icpc - 3*Inpc + Iapc)),
        Ipcc = (Tfloat)atXYZ(px,y,z,c,out_value),  Iccc = (Tfloat)atXYZ(x, y,z,c,out_value),
        Incc = (Tfloat)atXYZ(nx,y,z,c,out_value),  Iacc = (Tfloat)atXYZ(ax,y,z,c,out_value),
        Icc = Iccc + 0.5f*(dx*(-Ipcc + Incc) + dx*dx*(2*Ipcc - 5*Iccc + 4*Incc - Iacc) +
                           dx*dx*dx*(-Ipcc + 3*Iccc - 3*Incc + Iacc)),
        Ipnc = (Tfloat)atXYZ(px,ny,z,c,out_value), Icnc = (Tfloat)atXYZ(x,ny,z,c,out_value),
        Innc = (Tfloat)atXYZ(nx,ny,z,c,out_value), Ianc = (Tfloat)atXYZ(ax,ny,z,c,out_value),
        Inc = Icnc + 0.5f*(dx*(-Ipnc + Innc) + dx*dx*(2*Ipnc - 5*Icnc + 4*Innc - Ianc) +
                           dx*dx*dx*(-Ipnc + 3*Icnc - 3*Innc + Ianc)),
        Ipac = (Tfloat)atXYZ(px,ay,z,c,out_value), Icac = (Tfloat)atXYZ(x,ay,z,c,out_value),
        Inac = (Tfloat)atXYZ(nx,ay,z,c,out_value), Iaac = (Tfloat)atXYZ(ax,ay,z,c,out_value),
        Iac = Icac + 0.5f*(dx*(-Ipac + Inac) + dx*dx*(2*Ipac - 5*Icac + 4*Inac - Iaac) +
                           dx*dx*dx*(-Ipac + 3*Icac - 3*Inac + Iaac)),
        Ic = Icc + 0.5f*(dy*(-Ipc + Inc) + dy*dy*(2*Ipc - 5*Icc + 4*Inc - Iac) +
                         dy*dy*dy*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ippn = (Tfloat)atXYZ(px,py,nz,c,out_value), Icpn = (Tfloat)atXYZ(x,py,nz,c,out_value),
        Inpn = (Tfloat)atXYZ(nx,py,nz,c,out_value), Iapn = (Tfloat)atXYZ(ax,py,nz,c,out_value),
        Ipn = Icpn + 0.5f*(dx*(-Ippn + Inpn) + dx*dx*(2*Ippn - 5*Icpn + 4*Inpn - Iapn) +
                           dx*dx*dx*(-Ippn + 3*Icpn - 3*Inpn + Iapn)),
        Ipcn = (Tfloat)atXYZ(px,y,nz,c,out_value),  Iccn = (Tfloat)atXYZ(x, y,nz,c,out_value),
        Incn = (Tfloat)atXYZ(nx,y,nz,c,out_value),  Iacn = (Tfloat)atXYZ(ax,y,nz,c,out_value),
        Icn = Iccn + 0.5f*(dx*(-Ipcn + Incn) + dx*dx*(2*Ipcn - 5*Iccn + 4*Incn - Iacn) +
                           dx*dx*dx*(-Ipcn + 3*Iccn - 3*Incn + Iacn)),
        Ipnn = (Tfloat)atXYZ(px,ny,nz,c,out_value), Icnn = (Tfloat)atXYZ(x,ny,nz,c,out_value),
        Innn = (Tfloat)atXYZ(nx,ny,nz,c,out_value), Iann = (Tfloat)atXYZ(ax,ny,nz,c,out_value),
        Inn = Icnn + 0.5f*(dx*(-Ipnn + Innn) + dx*dx*(2*Ipnn - 5*Icnn + 4*Innn - Iann) +
                           dx*dx*dx*(-Ipnn + 3*Icnn - 3*Innn + Iann)),
        Ipan = (Tfloat)atXYZ(px,ay,nz,c,out_value), Ican = (Tfloat)atXYZ(x,ay,nz,c,out_value),
        Inan = (Tfloat)atXYZ(nx,ay,nz,c,out_value), Iaan = (Tfloat)atXYZ(ax,ay,nz,c,out_value),
        Ian = Ican + 0.5f*(dx*(-Ipan + Inan) + dx*dx*(2*Ipan - 5*Ican + 4*Inan - Iaan) +
                           dx*dx*dx*(-Ipan + 3*Ican - 3*Inan + Iaan)),
        In = Icn + 0.5f*(dy*(-Ipn + Inn) + dy*dy*(2*Ipn - 5*Icn + 4*Inn - Ian) +
                         dy*dy*dy*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ippa = (Tfloat)atXYZ(px,py,az,c,out_value), Icpa = (Tfloat)atXYZ(x,py,az,c,out_value),
        Inpa = (Tfloat)atXYZ(nx,py,az,c,out_value), Iapa = (Tfloat)atXYZ(ax,py,az,c,out_value),
        Ipa = Icpa + 0.5f*(dx*(-Ippa + Inpa) + dx*dx*(2*Ippa - 5*Icpa + 4*Inpa - Iapa) +
                           dx*dx*dx*(-Ippa + 3*Icpa - 3*Inpa + Iapa)),
        Ipca = (Tfloat)atXYZ(px,y,az,c,out_value),  Icca = (Tfloat)atXYZ(x, y,az,c,out_value),
        Inca = (Tfloat)atXYZ(nx,y,az,c,out_value),  Iaca = (Tfloat)atXYZ(ax,y,az,c,out_value),
        Ica = Icca + 0.5f*(dx*(-Ipca + Inca) + dx*dx*(2*Ipca - 5*Icca + 4*Inca - Iaca) +
                           dx*dx*dx*(-Ipca + 3*Icca - 3*Inca + Iaca)),
        Ipna = (Tfloat)atXYZ(px,ny,az,c,out_value), Icna = (Tfloat)atXYZ(x,ny,az,c,out_value),
        Inna = (Tfloat)atXYZ(nx,ny,az,c,out_value), Iana = (Tfloat)atXYZ(ax,ny,az,c,out_value),
        Ina = Icna + 0.5f*(dx*(-Ipna + Inna) + dx*dx*(2*Ipna - 5*Icna + 4*Inna - Iana) +
                           dx*dx*dx*(-Ipna + 3*Icna - 3*Inna + Iana)),
        Ipaa = (Tfloat)atXYZ(px,ay,az,c,out_value), Icaa = (Tfloat)atXYZ(x,ay,az,c,out_value),
        Inaa = (Tfloat)atXYZ(nx,ay,az,c,out_value), Iaaa = (Tfloat)atXYZ(ax,ay,az,c,out_value),
        Iaa = Icaa + 0.5f*(dx*(-Ipaa + Inaa) + dx*dx*(2*Ipaa - 5*Icaa + 4*Inaa - Iaaa) +
                           dx*dx*dx*(-Ipaa + 3*Icaa - 3*Inaa + Iaaa)),
        Ia = Ica + 0.5f*(dy*(-Ipa + Ina) + dy*dy*(2*Ipa - 5*Ica + 4*Ina - Iaa) +
                         dy*dy*dy*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dz*(-Ip + In) + dz*dz*(2*Ip - 5*Ic + 4*In - Ia) + dz*dz*dz*(-Ip + 3*Ic - 3*In + Ia));
    }

    //! Return clamped pixel value, using cubic interpolation and Dirichlet boundary conditions for the XYZ-coordinates.
    /**
       Similar to cubic_atXYZ(float,float,float,int,const T) const, except that the return value is clamped to stay
       in the min/max range of the datatype \c T.
    **/
    T cubic_atXYZ_c(const float fx, const float fy, const float fz, const int c, const T& out_value) const {
      return cimg::type<T>::cut(cubic_atXYZ(fx,fy,fz,c,out_value));
    }

    //! Return pixel value, using cubic interpolation and Neumann boundary conditions for the X,Y and Z-coordinates.
    /**
       Similar to cubic_atX(float,int,int,int) const, except that the cubic interpolation and boundary checking
       are achieved both for X,Y and Z-coordinates.
       \note
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _cubic_atXYZ(float,float,float,int).
    **/
    Tfloat cubic_atXYZ(const float fx, const float fy, const float fz, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atXYZ(): Empty instance.",
                                    cimg_instance);
      return _cubic_atXYZ(fx,fy,fz,c);
    }

    Tfloat _cubic_atXYZ(const float fx, const float fy, const float fz, const int c=0) const {
      const float
        nfx = cimg::type<float>::is_nan(fx)?0:cimg::cut(fx,0.f,width() - 1.f),
        nfy = cimg::type<float>::is_nan(fy)?0:cimg::cut(fy,0.f,height() - 1.f),
        nfz = cimg::type<float>::is_nan(fz)?0:cimg::cut(fz,0.f,depth() - 1.f);
      const int x = (int)nfx, y = (int)nfy, z = (int)nfz;
      const float dx = nfx - x, dy = nfy - y, dz = nfz - z;
      const int
        px = x - 1<0?0:x - 1, nx = dx>0?x + 1:x, ax = x + 2>=width()?width() - 1:x + 2,
        py = y - 1<0?0:y - 1, ny = dy>0?y + 1:y, ay = y + 2>=height()?height() - 1:y + 2,
        pz = z - 1<0?0:z - 1, nz = dz>0?z + 1:z, az = z + 2>=depth()?depth() - 1:z + 2;
      const Tfloat
        Ippp = (Tfloat)(*this)(px,py,pz,c), Icpp = (Tfloat)(*this)(x,py,pz,c),
        Inpp = (Tfloat)(*this)(nx,py,pz,c), Iapp = (Tfloat)(*this)(ax,py,pz,c),
        Ipp = Icpp + 0.5f*(dx*(-Ippp + Inpp) + dx*dx*(2*Ippp - 5*Icpp + 4*Inpp - Iapp) +
                           dx*dx*dx*(-Ippp + 3*Icpp - 3*Inpp + Iapp)),
        Ipcp = (Tfloat)(*this)(px,y,pz,c),  Iccp = (Tfloat)(*this)(x, y,pz,c),
        Incp = (Tfloat)(*this)(nx,y,pz,c),  Iacp = (Tfloat)(*this)(ax,y,pz,c),
        Icp = Iccp + 0.5f*(dx*(-Ipcp + Incp) + dx*dx*(2*Ipcp - 5*Iccp + 4*Incp - Iacp) +
                           dx*dx*dx*(-Ipcp + 3*Iccp - 3*Incp + Iacp)),
        Ipnp = (Tfloat)(*this)(px,ny,pz,c), Icnp = (Tfloat)(*this)(x,ny,pz,c),
        Innp = (Tfloat)(*this)(nx,ny,pz,c), Ianp = (Tfloat)(*this)(ax,ny,pz,c),
        Inp = Icnp + 0.5f*(dx*(-Ipnp + Innp) + dx*dx*(2*Ipnp - 5*Icnp + 4*Innp - Ianp) +
                           dx*dx*dx*(-Ipnp + 3*Icnp - 3*Innp + Ianp)),
        Ipap = (Tfloat)(*this)(px,ay,pz,c), Icap = (Tfloat)(*this)(x,ay,pz,c),
        Inap = (Tfloat)(*this)(nx,ay,pz,c), Iaap = (Tfloat)(*this)(ax,ay,pz,c),
        Iap = Icap + 0.5f*(dx*(-Ipap + Inap) + dx*dx*(2*Ipap - 5*Icap + 4*Inap - Iaap) +
                           dx*dx*dx*(-Ipap + 3*Icap - 3*Inap + Iaap)),
        Ip = Icp + 0.5f*(dy*(-Ipp + Inp) + dy*dy*(2*Ipp - 5*Icp + 4*Inp - Iap) +
                         dy*dy*dy*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ippc = (Tfloat)(*this)(px,py,z,c), Icpc = (Tfloat)(*this)(x,py,z,c),
        Inpc = (Tfloat)(*this)(nx,py,z,c), Iapc = (Tfloat)(*this)(ax,py,z,c),
        Ipc = Icpc + 0.5f*(dx*(-Ippc + Inpc) + dx*dx*(2*Ippc - 5*Icpc + 4*Inpc - Iapc) +
                           dx*dx*dx*(-Ippc + 3*Icpc - 3*Inpc + Iapc)),
        Ipcc = (Tfloat)(*this)(px,y,z,c),  Iccc = (Tfloat)(*this)(x, y,z,c),
        Incc = (Tfloat)(*this)(nx,y,z,c),  Iacc = (Tfloat)(*this)(ax,y,z,c),
        Icc = Iccc + 0.5f*(dx*(-Ipcc + Incc) + dx*dx*(2*Ipcc - 5*Iccc + 4*Incc - Iacc) +
                           dx*dx*dx*(-Ipcc + 3*Iccc - 3*Incc + Iacc)),
        Ipnc = (Tfloat)(*this)(px,ny,z,c), Icnc = (Tfloat)(*this)(x,ny,z,c),
        Innc = (Tfloat)(*this)(nx,ny,z,c), Ianc = (Tfloat)(*this)(ax,ny,z,c),
        Inc = Icnc + 0.5f*(dx*(-Ipnc + Innc) + dx*dx*(2*Ipnc - 5*Icnc + 4*Innc - Ianc) +
                           dx*dx*dx*(-Ipnc + 3*Icnc - 3*Innc + Ianc)),
        Ipac = (Tfloat)(*this)(px,ay,z,c), Icac = (Tfloat)(*this)(x,ay,z,c),
        Inac = (Tfloat)(*this)(nx,ay,z,c), Iaac = (Tfloat)(*this)(ax,ay,z,c),
        Iac = Icac + 0.5f*(dx*(-Ipac + Inac) + dx*dx*(2*Ipac - 5*Icac + 4*Inac - Iaac) +
                           dx*dx*dx*(-Ipac + 3*Icac - 3*Inac + Iaac)),
        Ic = Icc + 0.5f*(dy*(-Ipc + Inc) + dy*dy*(2*Ipc - 5*Icc + 4*Inc - Iac) +
                         dy*dy*dy*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ippn = (Tfloat)(*this)(px,py,nz,c), Icpn = (Tfloat)(*this)(x,py,nz,c),
        Inpn = (Tfloat)(*this)(nx,py,nz,c), Iapn = (Tfloat)(*this)(ax,py,nz,c),
        Ipn = Icpn + 0.5f*(dx*(-Ippn + Inpn) + dx*dx*(2*Ippn - 5*Icpn + 4*Inpn - Iapn) +
                           dx*dx*dx*(-Ippn + 3*Icpn - 3*Inpn + Iapn)),
        Ipcn = (Tfloat)(*this)(px,y,nz,c),  Iccn = (Tfloat)(*this)(x, y,nz,c),
        Incn = (Tfloat)(*this)(nx,y,nz,c),  Iacn = (Tfloat)(*this)(ax,y,nz,c),
        Icn = Iccn + 0.5f*(dx*(-Ipcn + Incn) + dx*dx*(2*Ipcn - 5*Iccn + 4*Incn - Iacn) +
                           dx*dx*dx*(-Ipcn + 3*Iccn - 3*Incn + Iacn)),
        Ipnn = (Tfloat)(*this)(px,ny,nz,c), Icnn = (Tfloat)(*this)(x,ny,nz,c),
        Innn = (Tfloat)(*this)(nx,ny,nz,c), Iann = (Tfloat)(*this)(ax,ny,nz,c),
        Inn = Icnn + 0.5f*(dx*(-Ipnn + Innn) + dx*dx*(2*Ipnn - 5*Icnn + 4*Innn - Iann) +
                           dx*dx*dx*(-Ipnn + 3*Icnn - 3*Innn + Iann)),
        Ipan = (Tfloat)(*this)(px,ay,nz,c), Ican = (Tfloat)(*this)(x,ay,nz,c),
        Inan = (Tfloat)(*this)(nx,ay,nz,c), Iaan = (Tfloat)(*this)(ax,ay,nz,c),
        Ian = Ican + 0.5f*(dx*(-Ipan + Inan) + dx*dx*(2*Ipan - 5*Ican + 4*Inan - Iaan) +
                           dx*dx*dx*(-Ipan + 3*Ican - 3*Inan + Iaan)),
        In = Icn + 0.5f*(dy*(-Ipn + Inn) + dy*dy*(2*Ipn - 5*Icn + 4*Inn - Ian) +
                         dy*dy*dy*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ippa = (Tfloat)(*this)(px,py,az,c), Icpa = (Tfloat)(*this)(x,py,az,c),
        Inpa = (Tfloat)(*this)(nx,py,az,c), Iapa = (Tfloat)(*this)(ax,py,az,c),
        Ipa = Icpa + 0.5f*(dx*(-Ippa + Inpa) + dx*dx*(2*Ippa - 5*Icpa + 4*Inpa - Iapa) +
                           dx*dx*dx*(-Ippa + 3*Icpa - 3*Inpa + Iapa)),
        Ipca = (Tfloat)(*this)(px,y,az,c),  Icca = (Tfloat)(*this)(x, y,az,c),
        Inca = (Tfloat)(*this)(nx,y,az,c),  Iaca = (Tfloat)(*this)(ax,y,az,c),
        Ica = Icca + 0.5f*(dx*(-Ipca + Inca) + dx*dx*(2*Ipca - 5*Icca + 4*Inca - Iaca) +
                           dx*dx*dx*(-Ipca + 3*Icca - 3*Inca + Iaca)),
        Ipna = (Tfloat)(*this)(px,ny,az,c), Icna = (Tfloat)(*this)(x,ny,az,c),
        Inna = (Tfloat)(*this)(nx,ny,az,c), Iana = (Tfloat)(*this)(ax,ny,az,c),
        Ina = Icna + 0.5f*(dx*(-Ipna + Inna) + dx*dx*(2*Ipna - 5*Icna + 4*Inna - Iana) +
                           dx*dx*dx*(-Ipna + 3*Icna - 3*Inna + Iana)),
        Ipaa = (Tfloat)(*this)(px,ay,az,c), Icaa = (Tfloat)(*this)(x,ay,az,c),
        Inaa = (Tfloat)(*this)(nx,ay,az,c), Iaaa = (Tfloat)(*this)(ax,ay,az,c),
        Iaa = Icaa + 0.5f*(dx*(-Ipaa + Inaa) + dx*dx*(2*Ipaa - 5*Icaa + 4*Inaa - Iaaa) +
                           dx*dx*dx*(-Ipaa + 3*Icaa - 3*Inaa + Iaaa)),
        Ia = Ica + 0.5f*(dy*(-Ipa + Ina) + dy*dy*(2*Ipa - 5*Ica + 4*Ina - Iaa) +
                         dy*dy*dy*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dz*(-Ip + In) + dz*dz*(2*Ip - 5*Ic + 4*In - Ia) + dz*dz*dz*(-Ip + 3*Ic - 3*In + Ia));
    }

    //! Return clamped pixel value, using cubic interpolation and Neumann boundary conditions for the XYZ-coordinates.
    /**
       Similar to cubic_atXYZ(float,float,float,int) const, except that the return value is clamped to stay in the
       min/max range of the datatype \c T.
    **/
    T cubic_atXYZ_c(const float fx, const float fy, const float fz, const int c) const {
      return cimg::type<T>::cut(cubic_atXYZ(fx,fy,fz,c));
    }

    T _cubic_atXYZ_c(const float fx, const float fy, const float fz, const int c) const {
      return cimg::type<T>::cut(_cubic_atXYZ(fx,fy,fz,c));
    }

    //! Return pixel value, using cubic interpolation and Neumann boundary conditions for the X,Y and Z-coordinates.
    /**
       Similar to cubic_atX(float,int,int,int) const, except that the cubic interpolation and boundary checking
       are achieved both for X,Y and Z-coordinates.
       \note
       - If you know your image instance is \e not empty, you may rather use the slightly faster method
         \c _cubic_atXYZ(float,float,float,int).
    **/
    Tfloat cubic_atXYZ_p(const float fx, const float fy, const float fz, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "cubic_atXYZ_p(): Empty instance.",
                                    cimg_instance);
      return _cubic_atXYZ_p(fx,fy,fz,c);
    }

    Tfloat _cubic_atXYZ_p(const float fx, const float fy, const float fz, const int c=0) const {
      const float
        nfx = cimg::type<float>::is_nan(fx)?0:cimg::mod(fx,_width - 0.5f),
        nfy = cimg::type<float>::is_nan(fy)?0:cimg::mod(fy,_height - 0.5f),
        nfz = cimg::type<float>::is_nan(fz)?0:cimg::mod(fz,_depth - 0.5f);
      const int x = (int)nfx, y = (int)nfy, z = (int)nfz;
      const float dx = nfx - x, dy = nfy - y, dz = nfz - z;
      const int
        px = cimg::mod(x - 1,width()), nx = cimg::mod(x + 1,width()), ax = cimg::mod(x + 2,width()),
        py = cimg::mod(y - 1,height()), ny = cimg::mod(y + 1,height()), ay = cimg::mod(y + 2,height()),
        pz = cimg::mod(z - 1,depth()), nz = cimg::mod(z + 1,depth()), az = cimg::mod(z + 2,depth());
      const Tfloat
        Ippp = (Tfloat)(*this)(px,py,pz,c), Icpp = (Tfloat)(*this)(x,py,pz,c),
        Inpp = (Tfloat)(*this)(nx,py,pz,c), Iapp = (Tfloat)(*this)(ax,py,pz,c),
        Ipp = Icpp + 0.5f*(dx*(-Ippp + Inpp) + dx*dx*(2*Ippp - 5*Icpp + 4*Inpp - Iapp) +
                           dx*dx*dx*(-Ippp + 3*Icpp - 3*Inpp + Iapp)),
        Ipcp = (Tfloat)(*this)(px,y,pz,c),  Iccp = (Tfloat)(*this)(x, y,pz,c),
        Incp = (Tfloat)(*this)(nx,y,pz,c),  Iacp = (Tfloat)(*this)(ax,y,pz,c),
        Icp = Iccp + 0.5f*(dx*(-Ipcp + Incp) + dx*dx*(2*Ipcp - 5*Iccp + 4*Incp - Iacp) +
                           dx*dx*dx*(-Ipcp + 3*Iccp - 3*Incp + Iacp)),
        Ipnp = (Tfloat)(*this)(px,ny,pz,c), Icnp = (Tfloat)(*this)(x,ny,pz,c),
        Innp = (Tfloat)(*this)(nx,ny,pz,c), Ianp = (Tfloat)(*this)(ax,ny,pz,c),
        Inp = Icnp + 0.5f*(dx*(-Ipnp + Innp) + dx*dx*(2*Ipnp - 5*Icnp + 4*Innp - Ianp) +
                           dx*dx*dx*(-Ipnp + 3*Icnp - 3*Innp + Ianp)),
        Ipap = (Tfloat)(*this)(px,ay,pz,c), Icap = (Tfloat)(*this)(x,ay,pz,c),
        Inap = (Tfloat)(*this)(nx,ay,pz,c), Iaap = (Tfloat)(*this)(ax,ay,pz,c),
        Iap = Icap + 0.5f*(dx*(-Ipap + Inap) + dx*dx*(2*Ipap - 5*Icap + 4*Inap - Iaap) +
                           dx*dx*dx*(-Ipap + 3*Icap - 3*Inap + Iaap)),
        Ip = Icp + 0.5f*(dy*(-Ipp + Inp) + dy*dy*(2*Ipp - 5*Icp + 4*Inp - Iap) +
                         dy*dy*dy*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ippc = (Tfloat)(*this)(px,py,z,c), Icpc = (Tfloat)(*this)(x,py,z,c),
        Inpc = (Tfloat)(*this)(nx,py,z,c), Iapc = (Tfloat)(*this)(ax,py,z,c),
        Ipc = Icpc + 0.5f*(dx*(-Ippc + Inpc) + dx*dx*(2*Ippc - 5*Icpc + 4*Inpc - Iapc) +
                           dx*dx*dx*(-Ippc + 3*Icpc - 3*Inpc + Iapc)),
        Ipcc = (Tfloat)(*this)(px,y,z,c),  Iccc = (Tfloat)(*this)(x, y,z,c),
        Incc = (Tfloat)(*this)(nx,y,z,c),  Iacc = (Tfloat)(*this)(ax,y,z,c),
        Icc = Iccc + 0.5f*(dx*(-Ipcc + Incc) + dx*dx*(2*Ipcc - 5*Iccc + 4*Incc - Iacc) +
                           dx*dx*dx*(-Ipcc + 3*Iccc - 3*Incc + Iacc)),
        Ipnc = (Tfloat)(*this)(px,ny,z,c), Icnc = (Tfloat)(*this)(x,ny,z,c),
        Innc = (Tfloat)(*this)(nx,ny,z,c), Ianc = (Tfloat)(*this)(ax,ny,z,c),
        Inc = Icnc + 0.5f*(dx*(-Ipnc + Innc) + dx*dx*(2*Ipnc - 5*Icnc + 4*Innc - Ianc) +
                           dx*dx*dx*(-Ipnc + 3*Icnc - 3*Innc + Ianc)),
        Ipac = (Tfloat)(*this)(px,ay,z,c), Icac = (Tfloat)(*this)(x,ay,z,c),
        Inac = (Tfloat)(*this)(nx,ay,z,c), Iaac = (Tfloat)(*this)(ax,ay,z,c),
        Iac = Icac + 0.5f*(dx*(-Ipac + Inac) + dx*dx*(2*Ipac - 5*Icac + 4*Inac - Iaac) +
                           dx*dx*dx*(-Ipac + 3*Icac - 3*Inac + Iaac)),
        Ic = Icc + 0.5f*(dy*(-Ipc + Inc) + dy*dy*(2*Ipc - 5*Icc + 4*Inc - Iac) +
                         dy*dy*dy*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ippn = (Tfloat)(*this)(px,py,nz,c), Icpn = (Tfloat)(*this)(x,py,nz,c),
        Inpn = (Tfloat)(*this)(nx,py,nz,c), Iapn = (Tfloat)(*this)(ax,py,nz,c),
        Ipn = Icpn + 0.5f*(dx*(-Ippn + Inpn) + dx*dx*(2*Ippn - 5*Icpn + 4*Inpn - Iapn) +
                           dx*dx*dx*(-Ippn + 3*Icpn - 3*Inpn + Iapn)),
        Ipcn = (Tfloat)(*this)(px,y,nz,c),  Iccn = (Tfloat)(*this)(x, y,nz,c),
        Incn = (Tfloat)(*this)(nx,y,nz,c),  Iacn = (Tfloat)(*this)(ax,y,nz,c),
        Icn = Iccn + 0.5f*(dx*(-Ipcn + Incn) + dx*dx*(2*Ipcn - 5*Iccn + 4*Incn - Iacn) +
                           dx*dx*dx*(-Ipcn + 3*Iccn - 3*Incn + Iacn)),
        Ipnn = (Tfloat)(*this)(px,ny,nz,c), Icnn = (Tfloat)(*this)(x,ny,nz,c),
        Innn = (Tfloat)(*this)(nx,ny,nz,c), Iann = (Tfloat)(*this)(ax,ny,nz,c),
        Inn = Icnn + 0.5f*(dx*(-Ipnn + Innn) + dx*dx*(2*Ipnn - 5*Icnn + 4*Innn - Iann) +
                           dx*dx*dx*(-Ipnn + 3*Icnn - 3*Innn + Iann)),
        Ipan = (Tfloat)(*this)(px,ay,nz,c), Ican = (Tfloat)(*this)(x,ay,nz,c),
        Inan = (Tfloat)(*this)(nx,ay,nz,c), Iaan = (Tfloat)(*this)(ax,ay,nz,c),
        Ian = Ican + 0.5f*(dx*(-Ipan + Inan) + dx*dx*(2*Ipan - 5*Ican + 4*Inan - Iaan) +
                           dx*dx*dx*(-Ipan + 3*Ican - 3*Inan + Iaan)),
        In = Icn + 0.5f*(dy*(-Ipn + Inn) + dy*dy*(2*Ipn - 5*Icn + 4*Inn - Ian) +
                         dy*dy*dy*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ippa = (Tfloat)(*this)(px,py,az,c), Icpa = (Tfloat)(*this)(x,py,az,c),
        Inpa = (Tfloat)(*this)(nx,py,az,c), Iapa = (Tfloat)(*this)(ax,py,az,c),
        Ipa = Icpa + 0.5f*(dx*(-Ippa + Inpa) + dx*dx*(2*Ippa - 5*Icpa + 4*Inpa - Iapa) +
                           dx*dx*dx*(-Ippa + 3*Icpa - 3*Inpa + Iapa)),
        Ipca = (Tfloat)(*this)(px,y,az,c),  Icca = (Tfloat)(*this)(x, y,az,c),
        Inca = (Tfloat)(*this)(nx,y,az,c),  Iaca = (Tfloat)(*this)(ax,y,az,c),
        Ica = Icca + 0.5f*(dx*(-Ipca + Inca) + dx*dx*(2*Ipca - 5*Icca + 4*Inca - Iaca) +
                           dx*dx*dx*(-Ipca + 3*Icca - 3*Inca + Iaca)),
        Ipna = (Tfloat)(*this)(px,ny,az,c), Icna = (Tfloat)(*this)(x,ny,az,c),
        Inna = (Tfloat)(*this)(nx,ny,az,c), Iana = (Tfloat)(*this)(ax,ny,az,c),
        Ina = Icna + 0.5f*(dx*(-Ipna + Inna) + dx*dx*(2*Ipna - 5*Icna + 4*Inna - Iana) +
                           dx*dx*dx*(-Ipna + 3*Icna - 3*Inna + Iana)),
        Ipaa = (Tfloat)(*this)(px,ay,az,c), Icaa = (Tfloat)(*this)(x,ay,az,c),
        Inaa = (Tfloat)(*this)(nx,ay,az,c), Iaaa = (Tfloat)(*this)(ax,ay,az,c),
        Iaa = Icaa + 0.5f*(dx*(-Ipaa + Inaa) + dx*dx*(2*Ipaa - 5*Icaa + 4*Inaa - Iaaa) +
                           dx*dx*dx*(-Ipaa + 3*Icaa - 3*Inaa + Iaaa)),
        Ia = Ica + 0.5f*(dy*(-Ipa + Ina) + dy*dy*(2*Ipa - 5*Ica + 4*Ina - Iaa) +
                         dy*dy*dy*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dz*(-Ip + In) + dz*dz*(2*Ip - 5*Ic + 4*In - Ia) + dz*dz*dz*(-Ip + 3*Ic - 3*In + Ia));
    }

    T cubic_atXYZ_pc(const float fx, const float fy, const float fz, const int c) const {
      return cimg::type<T>::cut(cubic_atXYZ_p(fx,fy,fz,c));
    }

    T _cubic_atXYZ_pc(const float fx, const float fy, const float fz, const int c) const {
      return cimg::type<T>::cut(_cubic_atXYZ_p(fx,fy,fz,c));
    }

    //! Set pixel value, using linear interpolation for the X-coordinates.
    /**
       Set pixel value at specified coordinates (\c fx,\c y,\c z,\c c) in the image instance, in a way that
       the value is spread amongst several neighbors if the pixel coordinates are float-valued.
       \param value Pixel value to set.
       \param fx X-coordinate of the pixel value (float-valued).
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param is_added Tells if the pixel value is added to (\c true), or simply replace (\c false) the current image
         pixel(s).
       \return A reference to the current image instance.
       \note
       - Calling this method with out-of-bounds coordinates does nothing.
    **/
    CImg<T>& set_linear_atX(const T& value, const float fx, const int y=0, const int z=0, const int c=0,
                            const bool is_added=false) {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1;
      const float
        dx = fx - x;
      if (y>=0 && y<height() && z>=0 && z<depth() && c>=0 && c<spectrum()) {
        if (x>=0 && x<width()) {
          const float w1 = 1 - dx, w2 = is_added?1:(1 - w1);
          (*this)(x,y,z,c) = (T)(w1*value + w2*(*this)(x,y,z,c));
        }
        if (nx>=0 && nx<width()) {
          const float w1 = dx, w2 = is_added?1:(1 - w1);
          (*this)(nx,y,z,c) = (T)(w1*value + w2*(*this)(nx,y,z,c));
        }
      }
      return *this;
    }

    //! Set pixel value, using linear interpolation for the X and Y-coordinates.
    /**
       Similar to set_linear_atX(const T&,float,int,int,int,bool), except that the linear interpolation
       is achieved both for X and Y-coordinates.
    **/
    CImg<T>& set_linear_atXY(const T& value, const float fx, const float fy=0, const int z=0, const int c=0,
                             const bool is_added=false) {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1;
      const float
        dx = fx - x,
        dy = fy - y;
      if (z>=0 && z<depth() && c>=0 && c<spectrum()) {
        if (y>=0 && y<height()) {
          if (x>=0 && x<width()) {
            const float w1 = (1 - dx)*(1 - dy), w2 = is_added?1:(1 - w1);
            (*this)(x,y,z,c) = (T)(w1*value + w2*(*this)(x,y,z,c));
          }
          if (nx>=0 && nx<width()) {
            const float w1 = dx*(1 - dy), w2 = is_added?1:(1 - w1);
            (*this)(nx,y,z,c) = (T)(w1*value + w2*(*this)(nx,y,z,c));
          }
        }
        if (ny>=0 && ny<height()) {
          if (x>=0 && x<width()) {
            const float w1 = (1 - dx)*dy, w2 = is_added?1:(1 - w1);
            (*this)(x,ny,z,c) = (T)(w1*value + w2*(*this)(x,ny,z,c));
          }
          if (nx>=0 && nx<width()) {
            const float w1 = dx*dy, w2 = is_added?1:(1 - w1);
            (*this)(nx,ny,z,c) = (T)(w1*value + w2*(*this)(nx,ny,z,c));
          }
        }
      }
      return *this;
    }

    //! Set pixel value, using linear interpolation for the X,Y and Z-coordinates.
    /**
       Similar to set_linear_atXY(const T&,float,float,int,int,bool), except that the linear interpolation
       is achieved both for X,Y and Z-coordinates.
    **/
    CImg<T>& set_linear_atXYZ(const T& value, const float fx, const float fy=0, const float fz=0, const int c=0,
                              const bool is_added=false) {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1,
        z = (int)fz - (fz>=0?0:1), nz = z + 1;
      const float
        dx = fx - x,
        dy = fy - y,
        dz = fz - z;
      if (c>=0 && c<spectrum()) {
        if (z>=0 && z<depth()) {
          if (y>=0 && y<height()) {
            if (x>=0 && x<width()) {
              const float w1 = (1 - dx)*(1 - dy)*(1 - dz), w2 = is_added?1:(1 - w1);
              (*this)(x,y,z,c) = (T)(w1*value + w2*(*this)(x,y,z,c));
            }
            if (nx>=0 && nx<width()) {
              const float w1 = dx*(1 - dy)*(1 - dz), w2 = is_added?1:(1 - w1);
              (*this)(nx,y,z,c) = (T)(w1*value + w2*(*this)(nx,y,z,c));
            }
          }
          if (ny>=0 && ny<height()) {
            if (x>=0 && x<width()) {
              const float w1 = (1 - dx)*dy*(1 - dz), w2 = is_added?1:(1 - w1);
              (*this)(x,ny,z,c) = (T)(w1*value + w2*(*this)(x,ny,z,c));
            }
            if (nx>=0 && nx<width()) {
              const float w1 = dx*dy*(1 - dz), w2 = is_added?1:(1 - w1);
              (*this)(nx,ny,z,c) = (T)(w1*value + w2*(*this)(nx,ny,z,c));
            }
          }
        }
        if (nz>=0 && nz<depth()) {
          if (y>=0 && y<height()) {
            if (x>=0 && x<width()) {
              const float w1 = (1 - dx)*(1 - dy)*dz, w2 = is_added?1:(1 - w1);
              (*this)(x,y,nz,c) = (T)(w1*value + w2*(*this)(x,y,nz,c));
            }
            if (nx>=0 && nx<width()) {
              const float w1 = dx*(1 - dy)*dz, w2 = is_added?1:(1 - w1);
              (*this)(nx,y,nz,c) = (T)(w1*value + w2*(*this)(nx,y,nz,c));
            }
          }
          if (ny>=0 && ny<height()) {
            if (x>=0 && x<width()) {
              const float w1 = (1 - dx)*dy*dz, w2 = is_added?1:(1 - w1);
              (*this)(x,ny,nz,c) = (T)(w1*value + w2*(*this)(x,ny,nz,c));
            }
            if (nx>=0 && nx<width()) {
              const float w1 = dx*dy*dz, w2 = is_added?1:(1 - w1);
              (*this)(nx,ny,nz,c) = (T)(w1*value + w2*(*this)(nx,ny,nz,c));
            }
          }
        }
      }
      return *this;
    }

    //! Return a C-string containing a list of all values of the image instance.
    /**
       Return a new \c CImg<char> image whose buffer data() is a \c char* string describing the list of all pixel values
       of the image instance (written in base 10), separated by specified \c separator character.
       \param separator A \c char character which specifies the separator between values in the returned C-string.
       \param max_size Maximum size of the returned image (or \c 0 if no limits are set).
       \param format For float/double-values, tell the printf format used to generate the text representation
         of the numbers (or \c 0 for default representation).
       \note
       - The returned image is never empty.
       - For an empty image instance, the returned string is <tt>""</tt>.
       - If \c max_size is equal to \c 0, there are no limits on the size of the returned string.
       - Otherwise, if the maximum number of string characters is exceeded, the value string is cut off
         and terminated by character \c '\0'. In that case, the returned image size is <tt>max_size + 1</tt>.
    **/
    CImg<charT> value_string(const char separator=',', const unsigned int max_size=0,
                             const char *const format=0) const {
      if (is_empty() || max_size==1) return CImg<charT>(1,1,1,1,0);
      CImgList<charT> items;
      CImg<charT> s_item(256); *s_item = 0;
      const T *ptrs = _data;
      unsigned int string_size = 0;
      const char *const _format = format?format:cimg::type<T>::format();
      for (ulongT off = 0, siz = size(); off<siz && (!max_size || string_size<max_size); ++off) {
        const unsigned int printed_size = 1U + cimg_snprintf(s_item,s_item._width,_format,
                                                             cimg::type<T>::format(*(ptrs++)));
        CImg<charT> item(s_item._data,printed_size);
        item[printed_size - 1] = separator;
        item.move_to(items);
        if (max_size) string_size+=printed_size;
      }
      CImg<charT> res;
      (items>'x').move_to(res);
      if (max_size && res._width>=max_size) res.crop(0,max_size - 1);
      res.back() = 0;
      return res;
    }

    //@}

#endif
