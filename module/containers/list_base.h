#ifndef CIMG_MODULE_LIST_BASE_H
#define CIMG_MODULE_LIST_BASE_H


/*
   #-----------------------------------------
   #
   #
   #
   # Definition of the CImgList<T> structure
   #
   #
   #
   #------------------------------------------
   */
  //! Represent a list of images CImg<T>.
  template<typename T>
  struct CImgList {
    unsigned int _width, _allocated_width;
    CImg<T> *_data;

    //! Simple iterator type, to loop through each image of a list.
    /**
       \note
       - The \c CImgList<T>::iterator type is defined as a <tt>CImg<T>*</tt>.
       - You may use it like this:
       \code
       CImgList<> list; // Assuming this image list is not empty
       for (CImgList<>::iterator it = list.begin(); it<list.end(); ++it) (*it).mirror('x');
       \endcode
       - Using the loop macro \c cimglist_for is another (more concise) alternative:
       \code
       cimglist_for(list,l) list[l].mirror('x');
       \endcode
    **/
    typedef CImg<T>* iterator;

    //! Simple const iterator type, to loop through each image of a \c const list instance.
    /**
       \note
       - The \c CImgList<T>::const_iterator type is defined to be a <tt>const CImg<T>*</tt>.
       - Similar to CImgList<T>::iterator, but for constant list instances.
    **/
    typedef const CImg<T>* const_iterator;

    //! Pixel value type.
    /**
       Refer to the pixels value type of the images in the list.
       \note
       - The \c CImgList<T>::value_type type of a \c CImgList<T> is defined to be a \c T.
         It is then similar to CImg<T>::value_type.
       - \c CImgList<T>::value_type is actually not used in %CImg methods. It has been mainly defined for
         compatibility with STL naming conventions.
    **/
    typedef T value_type;

    // Define common types related to template type T.
    typedef typename cimg::superset<T,bool>::type Tbool;
    typedef typename cimg::superset<T,unsigned char>::type Tuchar;
    typedef typename cimg::superset<T,char>::type Tchar;
    typedef typename cimg::superset<T,unsigned short>::type Tushort;
    typedef typename cimg::superset<T,short>::type Tshort;
    typedef typename cimg::superset<T,unsigned int>::type Tuint;
    typedef typename cimg::superset<T,int>::type Tint;
    typedef typename cimg::superset<T,cimg_ulong>::type Tulong;
    typedef typename cimg::superset<T,cimg_long>::type Tlong;
    typedef typename cimg::superset<T,float>::type Tfloat;
    typedef typename cimg::superset<T,double>::type Tdouble;
    typedef typename cimg::last<T,bool>::type boolT;
    typedef typename cimg::last<T,unsigned char>::type ucharT;
    typedef typename cimg::last<T,char>::type charT;
    typedef typename cimg::last<T,unsigned short>::type ushortT;
    typedef typename cimg::last<T,short>::type shortT;
    typedef typename cimg::last<T,unsigned int>::type uintT;
    typedef typename cimg::last<T,int>::type intT;
    typedef typename cimg::last<T,cimg_ulong>::type ulongT;
    typedef typename cimg::last<T,cimg_long>::type longT;
    typedef typename cimg::last<T,cimg_uint64>::type uint64T;
    typedef typename cimg::last<T,cimg_int64>::type int64T;
    typedef typename cimg::last<T,float>::type floatT;
    typedef typename cimg::last<T,double>::type doubleT;

    //@}
    //---------------------------
    //
    //! \name Plugins
    //@{
    //---------------------------
#ifdef cimglist_plugin
#include cimglist_plugin
#endif
#ifdef cimglist_plugin1
#include cimglist_plugin1
#endif
#ifdef cimglist_plugin2
#include cimglist_plugin2
#endif
#ifdef cimglist_plugin3
#include cimglist_plugin3
#endif
#ifdef cimglist_plugin4
#include cimglist_plugin4
#endif
#ifdef cimglist_plugin5
#include cimglist_plugin5
#endif
#ifdef cimglist_plugin6
#include cimglist_plugin6
#endif
#ifdef cimglist_plugin7
#include cimglist_plugin7
#endif
#ifdef cimglist_plugin8
#include cimglist_plugin8
#endif

    //@}
    //--------------------------------------------------------
    //
    //! \name Constructors / Destructor / Instance Management
    //@{
    //--------------------------------------------------------

    //! Destructor.
    /**
       Destroy current list instance.
       \note
       - Any allocated buffer is deallocated.
       - Destroying an empty list does nothing actually.
     **/
    ~CImgList() {
      delete[] _data;
    }

    //! Default constructor.
    /**
       Construct a new empty list instance.
       \note
       - An empty list has no pixel data and its dimension width() is set to \c 0, as well as its
         image buffer pointer data().
       - An empty list may be reassigned afterwards, with the family of the assign() methods.
         In all cases, the type of pixels stays \c T.
     **/
    CImgList():
      _width(0),_allocated_width(0),_data(0) {}

    //! Construct list containing empty images.
    /**
       \param n Number of empty images.
       \note Useful when you know by advance the number of images you want to manage, as
       it will allocate the right amount of memory for the list, without needs for reallocation
       (that may occur when starting from an empty list and inserting several images in it).
    **/
    explicit CImgList(const unsigned int n):_width(n) {
      if (n) _data = new CImg<T>[_allocated_width = std::max(16U,(unsigned int)cimg::nearest_pow2(n))];
      else { _allocated_width = 0; _data = 0; }
    }

    //! Construct list containing images of specified size.
    /**
       \param n Number of images.
       \param width Width of images.
       \param height Height of images.
       \param depth Depth of images.
       \param spectrum Number of channels of images.
       \note Pixel values are not initialized and may probably contain garbage.
    **/
    CImgList(const unsigned int n, const unsigned int width, const unsigned int height=1,
             const unsigned int depth=1, const unsigned int spectrum=1):
      _width(0),_allocated_width(0),_data(0) {
      assign(n);
      cimglist_apply(*this,assign)(width,height,depth,spectrum);
    }

    //! Construct list containing images of specified size, and initialize pixel values.
    /**
       \param n Number of images.
       \param width Width of images.
       \param height Height of images.
       \param depth Depth of images.
       \param spectrum Number of channels of images.
       \param val Initialization value for images pixels.
    **/
    CImgList(const unsigned int n, const unsigned int width, const unsigned int height,
             const unsigned int depth, const unsigned int spectrum, const T& val):
      _width(0),_allocated_width(0),_data(0) {
      assign(n);
      cimglist_apply(*this,assign)(width,height,depth,spectrum,val);
    }

    //! Construct list containing images of specified size, and initialize pixel values from a sequence of integers.
    /**
       \param n Number of images.
       \param width Width of images.
       \param height Height of images.
       \param depth Depth of images.
       \param spectrum Number of channels of images.
       \param val0 First value of the initializing integers sequence.
       \param val1 Second value of the initializing integers sequence.
       \warning You must specify at least <tt>width*height*depth*spectrum</tt> values in your argument list,
         or you will probably segfault.
    **/
    CImgList(const unsigned int n, const unsigned int width, const unsigned int height,
             const unsigned int depth, const unsigned int spectrum, const int val0, const int val1, ...):
      _width(0),_allocated_width(0),_data(0) {
#define _CImgList_stdarg(t) { \
        assign(n,width,height,depth,spectrum); \
        const ulongT siz = (ulongT)width*height*depth*spectrum, nsiz = siz*n; \
        T *ptrd = _data->_data; \
        va_list ap; \
        va_start(ap,val1); \
        for (ulongT l = 0, s = 0, i = 0; i<nsiz; ++i) { \
          *(ptrd++) = (T)(i==0?val0:(i==1?val1:va_arg(ap,t))); \
          if ((++s)==siz) { ptrd = _data[++l]._data; s = 0; } \
        } \
        va_end(ap); \
      }
      _CImgList_stdarg(int);
    }

    //! Construct list containing images of specified size, and initialize pixel values from a sequence of doubles.
    /**
       \param n Number of images.
       \param width Width of images.
       \param height Height of images.
       \param depth Depth of images.
       \param spectrum Number of channels of images.
       \param val0 First value of the initializing doubles sequence.
       \param val1 Second value of the initializing doubles sequence.
       \warning You must specify at least <tt>width*height*depth*spectrum</tt> values in your argument list,
         or you will probably segfault.
    **/
    CImgList(const unsigned int n, const unsigned int width, const unsigned int height,
             const unsigned int depth, const unsigned int spectrum, const double val0, const double val1, ...):
      _width(0),_allocated_width(0),_data(0) {
      _CImgList_stdarg(double);
    }

    //! Construct list containing copies of an input image.
    /**
       \param n Number of images.
       \param img Input image to copy in the constructed list.
       \param is_shared Tells if the elements of the list are shared or non-shared copies of \c img.
    **/
    template<typename t>
    CImgList(const unsigned int n, const CImg<t>& img, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(n);
      cimglist_apply(*this,assign)(img,is_shared);
    }

    //! Construct list from one image.
    /**
       \param img Input image to copy in the constructed list.
       \param is_shared Tells if the element of the list is a shared or non-shared copy of \c img.
     **/
    template<typename t>
    explicit CImgList(const CImg<t>& img, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(1);
      _data[0].assign(img,is_shared);
    }

    //! Construct list from two images.
    /**
       \param img1 First input image to copy in the constructed list.
       \param img2 Second input image to copy in the constructed list.
       \param is_shared Tells if the elements of the list are shared or non-shared copies of input images.
     **/
    template<typename t1, typename t2>
    CImgList(const CImg<t1>& img1, const CImg<t2>& img2, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(2);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared);
    }

    //! Construct list from three images.
    /**
       \param img1 First input image to copy in the constructed list.
       \param img2 Second input image to copy in the constructed list.
       \param img3 Third input image to copy in the constructed list.
       \param is_shared Tells if the elements of the list are shared or non-shared copies of input images.
    **/
    template<typename t1, typename t2, typename t3>
    CImgList(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(3);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
    }

    //! Construct list from four images.
    /**
       \param img1 First input image to copy in the constructed list.
       \param img2 Second input image to copy in the constructed list.
       \param img3 Third input image to copy in the constructed list.
       \param img4 Fourth input image to copy in the constructed list.
       \param is_shared Tells if the elements of the list are shared or non-shared copies of input images.
    **/
    template<typename t1, typename t2, typename t3, typename t4>
    CImgList(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
             const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(4);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared);
    }

    //! Construct list from five images.
    /**
       \param img1 First input image to copy in the constructed list.
       \param img2 Second input image to copy in the constructed list.
       \param img3 Third input image to copy in the constructed list.
       \param img4 Fourth input image to copy in the constructed list.
       \param img5 Fifth input image to copy in the constructed list.
       \param is_shared Tells if the elements of the list are shared or non-shared copies of input images.
    **/
    template<typename t1, typename t2, typename t3, typename t4, typename t5>
    CImgList(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
             const CImg<t5>& img5, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(5);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared); _data[4].assign(img5,is_shared);
    }

    //! Construct list from six images.
    /**
       \param img1 First input image to copy in the constructed list.
       \param img2 Second input image to copy in the constructed list.
       \param img3 Third input image to copy in the constructed list.
       \param img4 Fourth input image to copy in the constructed list.
       \param img5 Fifth input image to copy in the constructed list.
       \param img6 Sixth input image to copy in the constructed list.
       \param is_shared Tells if the elements of the list are shared or non-shared copies of input images.
    **/
    template<typename t1, typename t2, typename t3, typename t4, typename t5, typename t6>
    CImgList(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
             const CImg<t5>& img5, const CImg<t6>& img6, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(6);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared); _data[4].assign(img5,is_shared); _data[5].assign(img6,is_shared);
    }

    //! Construct list from seven images.
    /**
       \param img1 First input image to copy in the constructed list.
       \param img2 Second input image to copy in the constructed list.
       \param img3 Third input image to copy in the constructed list.
       \param img4 Fourth input image to copy in the constructed list.
       \param img5 Fifth input image to copy in the constructed list.
       \param img6 Sixth input image to copy in the constructed list.
       \param img7 Seventh input image to copy in the constructed list.
       \param is_shared Tells if the elements of the list are shared or non-shared copies of input images.
    **/
    template<typename t1, typename t2, typename t3, typename t4, typename t5, typename t6, typename t7>
    CImgList(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
             const CImg<t5>& img5, const CImg<t6>& img6, const CImg<t7>& img7, const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(7);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared); _data[4].assign(img5,is_shared); _data[5].assign(img6,is_shared);
      _data[6].assign(img7,is_shared);
    }

    //! Construct list from eight images.
    /**
       \param img1 First input image to copy in the constructed list.
       \param img2 Second input image to copy in the constructed list.
       \param img3 Third input image to copy in the constructed list.
       \param img4 Fourth input image to copy in the constructed list.
       \param img5 Fifth input image to copy in the constructed list.
       \param img6 Sixth input image to copy in the constructed list.
       \param img7 Seventh input image to copy in the constructed list.
       \param img8 Eighth input image to copy in the constructed list.
       \param is_shared Tells if the elements of the list are shared or non-shared copies of input images.
    **/
    template<typename t1, typename t2, typename t3, typename t4, typename t5, typename t6, typename t7, typename t8>
    CImgList(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
             const CImg<t5>& img5, const CImg<t6>& img6, const CImg<t7>& img7, const CImg<t8>& img8,
             const bool is_shared=false):
      _width(0),_allocated_width(0),_data(0) {
      assign(8);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared); _data[4].assign(img5,is_shared); _data[5].assign(img6,is_shared);
      _data[6].assign(img7,is_shared); _data[7].assign(img8,is_shared);
    }

    //! Construct list copy.
    /**
       \param list Input list to copy.
       \note The shared state of each element of the constructed list is kept the same as in \c list.
    **/
    template<typename t>
    CImgList(const CImgList<t>& list):_width(0),_allocated_width(0),_data(0) {
      assign(list._width);
      cimglist_for(*this,l) _data[l].assign(list[l],false);
    }

    //! Construct list copy \specialization.
    CImgList(const CImgList<T>& list):_width(0),_allocated_width(0),_data(0) {
      assign(list._width);
      cimglist_for(*this,l) _data[l].assign(list[l],list[l]._is_shared);
    }

    //! Construct list copy, and force the shared state of the list elements.
    /**
       \param list Input list to copy.
       \param is_shared Tells if the elements of the list are shared or non-shared copies of input images.
    **/
    template<typename t>
    CImgList(const CImgList<t>& list, const bool is_shared):_width(0),_allocated_width(0),_data(0) {
      assign(list._width);
      cimglist_for(*this,l) _data[l].assign(list[l],is_shared);
    }

    //! Construct list by reading the content of a file.
    /**
       \param filename Filename, as a C-string.
    **/
    explicit CImgList(const char *const filename):_width(0),_allocated_width(0),_data(0) {
      assign(filename);
    }

    //! Construct list from the content of a display window.
    /**
       \param disp Display window to get content from.
       \note Constructed list contains a single image only.
    **/
    explicit CImgList(const CImgDisplay& disp):_width(0),_allocated_width(0),_data(0) {
      assign(disp);
    }

    //! Return a list with elements being shared copies of images in the list instance.
    /**
      \note <tt>list2 = list1.get_shared()</tt> is equivalent to <tt>list2.assign(list1,true)</tt>.
    **/
    CImgList<T> get_shared() {
      CImgList<T> res(_width);
      cimglist_for(*this,l) res[l].assign(_data[l],true);
      return res;
    }

    //! Return a list with elements being shared copies of images in the list instance \const.
    const CImgList<T> get_shared() const {
      CImgList<T> res(_width);
      cimglist_for(*this,l) res[l].assign(_data[l],true);
      return res;
    }

    //! Destructor \inplace.
    /**
       \see CImgList().
    **/
    CImgList<T>& assign() {
      delete[] _data;
      _width = _allocated_width = 0;
      _data = 0;
      return *this;
    }

    //! Destructor \inplace.
    /**
       Equivalent to assign().
       \note Only here for compatibility with STL naming conventions.
    **/
    CImgList<T>& clear() {
      return assign();
    }

    //! Construct list containing empty images \inplace.
    /**
       \see CImgList(unsigned int).
    **/
    CImgList<T>& assign(const unsigned int n) {
      if (!n) return assign();
      if (_allocated_width<n || _allocated_width>(n<<2)) {
        delete[] _data;
        _data = new CImg<T>[_allocated_width = std::max(16U,(unsigned int)cimg::nearest_pow2(n))];
      }
      _width = n;
      return *this;
    }

    //! Construct list containing images of specified size \inplace.
    /**
       \see CImgList(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int).
    **/
    CImgList<T>& assign(const unsigned int n, const unsigned int width, const unsigned int height=1,
                        const unsigned int depth=1, const unsigned int spectrum=1) {
      assign(n);
      cimglist_apply(*this,assign)(width,height,depth,spectrum);
      return *this;
    }

    //! Construct list containing images of specified size, and initialize pixel values \inplace.
    /**
       \see CImgList(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, const T).
    **/
    CImgList<T>& assign(const unsigned int n, const unsigned int width, const unsigned int height,
                        const unsigned int depth, const unsigned int spectrum, const T& val) {
      assign(n);
      cimglist_apply(*this,assign)(width,height,depth,spectrum,val);
      return *this;
    }

    //! Construct list with images of specified size, and initialize pixel values from a sequence of integers \inplace.
    /**
       \see CImgList(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, const int, const int, ...).
    **/
    CImgList<T>& assign(const unsigned int n, const unsigned int width, const unsigned int height,
                        const unsigned int depth, const unsigned int spectrum, const int val0, const int val1, ...) {
      _CImgList_stdarg(int);
      return *this;
    }

    //! Construct list with images of specified size, and initialize pixel values from a sequence of doubles \inplace.
    /**
       \see CImgList(unsigned int,unsigned int,unsigned int,unsigned int,unsigned int,const double,const double,...).
    **/
    CImgList<T>& assign(const unsigned int n, const unsigned int width, const unsigned int height,
                        const unsigned int depth, const unsigned int spectrum,
                        const double val0, const double val1, ...) {
      _CImgList_stdarg(double);
      return *this;
    }

    //! Construct list containing copies of an input image \inplace.
    /**
       \see CImgList(unsigned int, const CImg<t>&, bool).
    **/
    template<typename t>
    CImgList<T>& assign(const unsigned int n, const CImg<t>& img, const bool is_shared=false) {
      assign(n);
      cimglist_apply(*this,assign)(img,is_shared);
      return *this;
    }

    //! Construct list from one image \inplace.
    /**
       \see CImgList(const CImg<t>&, bool).
    **/
    template<typename t>
    CImgList<T>& assign(const CImg<t>& img, const bool is_shared=false) {
      assign(1);
      _data[0].assign(img,is_shared);
      return *this;
    }

    //! Construct list from two images \inplace.
    /**
       \see CImgList(const CImg<t>&, const CImg<t>&, bool).
    **/
    template<typename t1, typename t2>
    CImgList<T>& assign(const CImg<t1>& img1, const CImg<t2>& img2, const bool is_shared=false) {
      assign(2);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared);
      return *this;
    }

    //! Construct list from three images \inplace.
    /**
       \see CImgList(const CImg<t>&, const CImg<t>&, const CImg<t>&, bool).
    **/
    template<typename t1, typename t2, typename t3>
    CImgList<T>& assign(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const bool is_shared=false) {
      assign(3);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      return *this;
    }

    //! Construct list from four images \inplace.
    /**
       \see CImgList(const CImg<t>&, const CImg<t>&, const CImg<t>&, const CImg<t>&, bool).
    **/
    template<typename t1, typename t2, typename t3, typename t4>
    CImgList<T>& assign(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
                        const bool is_shared=false) {
      assign(4);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared);
      return *this;
    }

    //! Construct list from five images \inplace.
    /**
       \see CImgList(const CImg<t>&, const CImg<t>&, const CImg<t>&, const CImg<t>&, const CImg<t>&, bool).
    **/
    template<typename t1, typename t2, typename t3, typename t4, typename t5>
    CImgList<T>& assign(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
                        const CImg<t5>& img5, const bool is_shared=false) {
      assign(5);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared); _data[4].assign(img5,is_shared);
      return *this;
    }

    //! Construct list from six images \inplace.
    /**
       \see CImgList(const CImg<t>&,const CImg<t>&,const CImg<t>&,const CImg<t>&,const CImg<t>&,const CImg<t>&, bool).
    **/
    template<typename t1, typename t2, typename t3, typename t4, typename t5, typename t6>
    CImgList<T>& assign(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
                        const CImg<t5>& img5, const CImg<t6>& img6, const bool is_shared=false) {
      assign(6);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared); _data[4].assign(img5,is_shared); _data[5].assign(img6,is_shared);
      return *this;
    }

    //! Construct list from seven images \inplace.
    /**
       \see CImgList(const CImg<t>&,const CImg<t>&,const CImg<t>&,const CImg<t>&,const CImg<t>&,const CImg<t>&,
       const CImg<t>&, bool).
    **/
    template<typename t1, typename t2, typename t3, typename t4, typename t5, typename t6, typename t7>
    CImgList<T>& assign(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
                        const CImg<t5>& img5, const CImg<t6>& img6, const CImg<t7>& img7, const bool is_shared=false) {
      assign(7);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared); _data[4].assign(img5,is_shared); _data[5].assign(img6,is_shared);
      _data[6].assign(img7,is_shared);
      return *this;
    }

    //! Construct list from eight images \inplace.
    /**
       \see CImgList(const CImg<t>&,const CImg<t>&,const CImg<t>&,const CImg<t>&,const CImg<t>&,const CImg<t>&,
       const CImg<t>&, const CImg<t>&, bool).
    **/
    template<typename t1, typename t2, typename t3, typename t4, typename t5, typename t6, typename t7, typename t8>
    CImgList<T>& assign(const CImg<t1>& img1, const CImg<t2>& img2, const CImg<t3>& img3, const CImg<t4>& img4,
                        const CImg<t5>& img5, const CImg<t6>& img6, const CImg<t7>& img7, const CImg<t8>& img8,
                        const bool is_shared=false) {
      assign(8);
      _data[0].assign(img1,is_shared); _data[1].assign(img2,is_shared); _data[2].assign(img3,is_shared);
      _data[3].assign(img4,is_shared); _data[4].assign(img5,is_shared); _data[5].assign(img6,is_shared);
      _data[6].assign(img7,is_shared); _data[7].assign(img8,is_shared);
      return *this;
    }

    //! Construct list as a copy of an existing list and force the shared state of the list elements \inplace.
    /**
      \see CImgList(const CImgList<t>&, bool is_shared).
    **/
    template<typename t>
    CImgList<T>& assign(const CImgList<t>& list, const bool is_shared=false) {
      cimg::unused(is_shared);
      assign(list._width);
      cimglist_for(*this,l) _data[l].assign(list[l],false);
      return *this;
    }

    //! Construct list as a copy of an existing list and force shared state of elements \inplace \specialization.
    CImgList<T>& assign(const CImgList<T>& list, const bool is_shared=false) {
      if (this==&list) return *this;
      CImgList<T> res(list._width);
      cimglist_for(res,l) res[l].assign(list[l],is_shared);
      return res.move_to(*this);
    }

    //! Construct list by reading the content of a file \inplace.
    /**
      \see CImgList(const char *const).
    **/
    CImgList<T>& assign(const char *const filename) {
      return load(filename);
    }

    //! Construct list from the content of a display window \inplace.
    /**
      \see CImgList(const CImgDisplay&).
    **/
    CImgList<T>& assign(const CImgDisplay &disp) {
      return assign(CImg<T>(disp));
    }

    //! Transfer the content of the list instance to another list.
    /**
       \param list Destination list.
       \note When returning, the current list instance is empty and the initial content of \c list is destroyed.
    **/
    template<typename t>
    CImgList<t>& move_to(CImgList<t>& list) {
      list.assign(_width);
      bool is_one_shared_element = false;
      cimglist_for(*this,l) is_one_shared_element|=_data[l]._is_shared;
      if (is_one_shared_element) cimglist_for(*this,l) list[l].assign(_data[l]);
      else cimglist_for(*this,l) _data[l].move_to(list[l]);
      assign();
      return list;
    }

    //! Transfer the content of the list instance at a specified position in another list.
    /**
       \param list Destination list.
       \param pos Index of the insertion in the list.
       \note When returning, the list instance is empty and the initial content of \c list is preserved
       (only images indexes may be modified).
     **/
    template<typename t>
    CImgList<t>& move_to(CImgList<t>& list, const unsigned int pos) {
      if (is_empty()) return list;
      const unsigned int npos = pos>list._width?list._width:pos;
      list.insert(_width,npos);
      bool is_one_shared_element = false;
      cimglist_for(*this,l) is_one_shared_element|=_data[l]._is_shared;
      if (is_one_shared_element) cimglist_for(*this,l) list[npos + l].assign(_data[l]);
      else cimglist_for(*this,l) _data[l].move_to(list[npos + l]);
      assign();
      return list;
    }

    //! Swap all fields between two list instances.
    /**
       \param list List to swap fields with.
       \note Can be used to exchange the content of two lists in a fast way.
    **/
    CImgList<T>& swap(CImgList<T>& list) {
      cimg::swap(_width,list._width,_allocated_width,list._allocated_width);
      cimg::swap(_data,list._data);
      return list;
    }

    //! Return a reference to an empty list.
    /**
      \note Can be used to define default values in a function taking a CImgList<T> as an argument.
      \code
      void f(const CImgList<char>& list=CImgList<char>::empty());
      \endcode
    **/
    static CImgList<T>& empty() {
      static CImgList<T> _empty;
      return _empty.assign();
    }

    //! Return a reference to an empty list \const.
    static const CImgList<T>& const_empty() {
      static const CImgList<T> _empty;
      return _empty;
    }

    //@}
    //------------------------------------------
    //
    //! \name Overloaded Operators
    //@{
    //------------------------------------------

    //! Return a reference to one image element of the list.
    /**
       \param pos Index of the image element.
    **/
    CImg<T>& operator()(const unsigned int pos) {
#if cimg_verbosity>=3
      if (pos>=_width) {
        cimg::warn(_cimglist_instance
                   "operator(): Invalid image request, at position [%u].",
                   cimglist_instance,
                   pos);
        return *_data;
      }
#endif
      return _data[pos];
    }

    //! Return a reference to one image of the list.
    /**
       \param pos Index of the image element.
    **/
    const CImg<T>& operator()(const unsigned int pos) const {
      return const_cast<CImgList<T>*>(this)->operator()(pos);
    }

    //! Return a reference to one pixel value of one image of the list.
    /**
       \param pos Index of the image element.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note <tt>list(n,x,y,z,c)</tt> is equivalent to <tt>list[n](x,y,z,c)</tt>.
    **/
    T& operator()(const unsigned int pos, const unsigned int x, const unsigned int y=0,
                  const unsigned int z=0, const unsigned int c=0) {
      return (*this)[pos](x,y,z,c);
    }

    //! Return a reference to one pixel value of one image of the list \const.
    const T& operator()(const unsigned int pos, const unsigned int x, const unsigned int y=0,
                        const unsigned int z=0, const unsigned int c=0) const {
      return (*this)[pos](x,y,z,c);
    }

    //! Return pointer to the first image of the list.
    /**
       \note Images in a list are stored as a buffer of \c CImg<T>.
    **/
    operator CImg<T>*() {
      return _data;
    }

    //! Return pointer to the first image of the list \const.
    operator const CImg<T>*() const {
      return _data;
    }

    //! Construct list from one image \inplace.
    /**
        \param img Input image to copy in the constructed list.
        \note <tt>list = img;</tt> is equivalent to <tt>list.assign(img);</tt>.
    **/
    template<typename t>
    CImgList<T>& operator=(const CImg<t>& img) {
      return assign(img);
    }

    //! Construct list from another list.
    /**
       \param list Input list to copy.
       \note <tt>list1 = list2</tt> is equivalent to <tt>list1.assign(list2);</tt>.
    **/
    template<typename t>
    CImgList<T>& operator=(const CImgList<t>& list) {
      return assign(list);
    }

    //! Construct list from another list \specialization.
    CImgList<T>& operator=(const CImgList<T>& list) {
      return assign(list);
    }

    //! Construct list by reading the content of a file \inplace.
    /**
       \see CImgList(const char *const).
    **/
    CImgList<T>& operator=(const char *const filename) {
      return assign(filename);
    }

    //! Construct list from the content of a display window \inplace.
    /**
        \see CImgList(const CImgDisplay&).
    **/
    CImgList<T>& operator=(const CImgDisplay& disp) {
      return assign(disp);
    }

    //! Return a non-shared copy of a list.
    /**
        \note <tt>+list</tt> is equivalent to <tt>CImgList<T>(list,false)</tt>.
          It forces the copy to have non-shared elements.
    **/
    CImgList<T> operator+() const {
      return CImgList<T>(*this,false);
    }

    //! Return a copy of the list instance, where image \c img has been inserted at the end.
    /**
       \param img Image inserted at the end of the instance copy.
       \note Define a convenient way to create temporary lists of images, as in the following code:
       \code
       (img1,img2,img3,img4).display("My four images");
       \endcode
    **/
    template<typename t>
    CImgList<T>& operator,(const CImg<t>& img) {
      return insert(img);
    }

    //! Return a copy of the list instance, where image \c img has been inserted at the end \const.
    template<typename t>
    CImgList<T> operator,(const CImg<t>& img) const {
      return (+*this).insert(img);
    }

    //! Return a copy of the list instance, where all elements of input list \c list have been inserted at the end.
    /**
       \param list List inserted at the end of the instance copy.
    **/
    template<typename t>
    CImgList<T>& operator,(const CImgList<t>& list) {
      return insert(list);
    }

    //! Return a copy of the list instance, where all elements of input \c list have been inserted at the end \const.
    template<typename t>
    CImgList<T>& operator,(const CImgList<t>& list) const {
      return (+*this).insert(list);
    }

    //! Return image corresponding to the appending of all images of the instance list along specified axis.
    /**
      \param axis Appending axis. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
      \note <tt>list>'x'</tt> is equivalent to <tt>list.get_append('x')</tt>.
    **/
    CImg<T> operator>(const char axis) const {
      return get_append(axis,0);
    }

    //! Return list corresponding to the splitting of all images of the instance list along specified axis.
    /**
      \param axis Axis used for image splitting.
      \note <tt>list<'x'</tt> is equivalent to <tt>list.get_split('x')</tt>.
    **/
    CImgList<T> operator<(const char axis) const {
      return get_split(axis);
    }

    //@}
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

    #include "module/containers/list_instance_decl.h"

    //! Access to pixel value with Dirichlet boundary conditions.
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param out_value Default value returned if \c offset is outside image bounds.
       \note <tt>list.atNXYZC(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZC(x,y,z,c);</tt>.
    **/
    T& atNXYZC(const int pos, const int x, const int y, const int z, const int c, const T& out_value) {
      return (pos<0 || pos>=width())?(cimg::temporary(out_value)=out_value):_data[pos].atXYZC(x,y,z,c,out_value);
    }

    //! Access to pixel value with Dirichlet boundary conditions \const.
    T atNXYZC(const int pos, const int x, const int y, const int z, const int c, const T& out_value) const {
      return (pos<0 || pos>=width())?out_value:_data[pos].atXYZC(x,y,z,c,out_value);
    }

    //! Access to pixel value with Neumann boundary conditions.
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note <tt>list.atNXYZC(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZC(x,y,z,c);</tt>.
    **/
    T& atNXYZC(const int pos, const int x, const int y, const int z, const int c) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXYZC(): Empty instance.",
                                    cimglist_instance);

      return _atNXYZC(pos,x,y,z,c);
    }

    //! Access to pixel value with Neumann boundary conditions \const.
    T atNXYZC(const int pos, const int x, const int y, const int z, const int c) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXYZC(): Empty instance.",
                                    cimglist_instance);

      return _atNXYZC(pos,x,y,z,c);
    }

    T& _atNXYZC(const int pos, const int x, const int y, const int z, const int c) {
      return _data[cimg::cut(pos,0,width() - 1)].atXYZC(x,y,z,c);
    }

    T _atNXYZC(const int pos, const int x, const int y, const int z, const int c) const {
      return _data[cimg::cut(pos,0,width() - 1)].atXYZC(x,y,z,c);
    }

    //! Access pixel value with Dirichlet boundary conditions for the 3 coordinates (\c pos, \c x,\c y,\c z).
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param out_value Default value returned if \c offset is outside image bounds.
       \note <tt>list.atNXYZ(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZ(x,y,z,c);</tt>.
    **/
    T& atNXYZ(const int pos, const int x, const int y, const int z, const int c, const T& out_value) {
      return (pos<0 || pos>=width())?(cimg::temporary(out_value)=out_value):_data[pos].atXYZ(x,y,z,c,out_value);
    }

    //! Access pixel value with Dirichlet boundary conditions for the 3 coordinates (\c pos, \c x,\c y,\c z) \const.
    T atNXYZ(const int pos, const int x, const int y, const int z, const int c, const T& out_value) const {
      return (pos<0 || pos>=width())?out_value:_data[pos].atXYZ(x,y,z,c,out_value);
    }

    //! Access to pixel value with Neumann boundary conditions for the 4 coordinates (\c pos, \c x,\c y,\c z).
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note <tt>list.atNXYZ(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZ(x,y,z,c);</tt>.
    **/
   T& atNXYZ(const int pos, const int x, const int y, const int z, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXYZ(): Empty instance.",
                                    cimglist_instance);

      return _atNXYZ(pos,x,y,z,c);
    }

    //! Access to pixel value with Neumann boundary conditions for the 4 coordinates (\c pos, \c x,\c y,\c z) \const.
    T atNXYZ(const int pos, const int x, const int y, const int z, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXYZ(): Empty instance.",
                                    cimglist_instance);

      return _atNXYZ(pos,x,y,z,c);
    }

    T& _atNXYZ(const int pos, const int x, const int y, const int z, const int c=0) {
      return _data[cimg::cut(pos,0,width() - 1)].atXYZ(x,y,z,c);
    }

    T _atNXYZ(const int pos, const int x, const int y, const int z, const int c=0) const {
      return _data[cimg::cut(pos,0,width() - 1)].atXYZ(x,y,z,c);
    }

    //! Access to pixel value with Dirichlet boundary conditions for the 3 coordinates (\c pos, \c x,\c y).
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param out_value Default value returned if \c offset is outside image bounds.
       \note <tt>list.atNXYZ(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZ(x,y,z,c);</tt>.
    **/
    T& atNXY(const int pos, const int x, const int y, const int z, const int c, const T& out_value) {
      return (pos<0 || pos>=width())?(cimg::temporary(out_value)=out_value):_data[pos].atXY(x,y,z,c,out_value);
    }

    //! Access to pixel value with Dirichlet boundary conditions for the 3 coordinates (\c pos, \c x,\c y) \const.
    T atNXY(const int pos, const int x, const int y, const int z, const int c, const T& out_value) const {
      return (pos<0 || pos>=width())?out_value:_data[pos].atXY(x,y,z,c,out_value);
    }

    //! Access to pixel value with Neumann boundary conditions for the 3 coordinates (\c pos, \c x,\c y).
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note <tt>list.atNXYZ(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZ(x,y,z,c);</tt>.
    **/
    T& atNXY(const int pos, const int x, const int y, const int z=0, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXY(): Empty instance.",
                                    cimglist_instance);

      return _atNXY(pos,x,y,z,c);
    }

    //! Access to pixel value with Neumann boundary conditions for the 3 coordinates (\c pos, \c x,\c y) \const.
    T atNXY(const int pos, const int x, const int y, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNXY(): Empty instance.",
                                    cimglist_instance);

      return _atNXY(pos,x,y,z,c);
    }

    T& _atNXY(const int pos, const int x, const int y, const int z=0, const int c=0) {
      return _data[cimg::cut(pos,0,width() - 1)].atXY(x,y,z,c);
    }

    T _atNXY(const int pos, const int x, const int y, const int z=0, const int c=0) const {
      return _data[cimg::cut(pos,0,width() - 1)].atXY(x,y,z,c);
    }

    //! Access to pixel value with Dirichlet boundary conditions for the 2 coordinates (\c pos,\c x).
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param out_value Default value returned if \c offset is outside image bounds.
       \note <tt>list.atNXYZ(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZ(x,y,z,c);</tt>.
    **/
    T& atNX(const int pos, const int x, const int y, const int z, const int c, const T& out_value) {
      return (pos<0 || pos>=width())?(cimg::temporary(out_value)=out_value):_data[pos].atX(x,y,z,c,out_value);
    }

    //! Access to pixel value with Dirichlet boundary conditions for the 2 coordinates (\c pos,\c x) \const.
    T atNX(const int pos, const int x, const int y, const int z, const int c, const T& out_value) const {
      return (pos<0 || pos>=width())?out_value:_data[pos].atX(x,y,z,c,out_value);
    }

    //! Access to pixel value with Neumann boundary conditions for the 2 coordinates (\c pos, \c x).
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note <tt>list.atNXYZ(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZ(x,y,z,c);</tt>.
    **/
    T& atNX(const int pos, const int x, const int y=0, const int z=0, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNX(): Empty instance.",
                                    cimglist_instance);

      return _atNX(pos,x,y,z,c);
    }

    //! Access to pixel value with Neumann boundary conditions for the 2 coordinates (\c pos, \c x) \const.
    T atNX(const int pos, const int x, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atNX(): Empty instance.",
                                    cimglist_instance);

      return _atNX(pos,x,y,z,c);
    }

    T& _atNX(const int pos, const int x, const int y=0, const int z=0, const int c=0) {
      return _data[cimg::cut(pos,0,width() - 1)].atX(x,y,z,c);
    }

    T _atNX(const int pos, const int x, const int y=0, const int z=0, const int c=0) const {
      return _data[cimg::cut(pos,0,width() - 1)].atX(x,y,z,c);
    }

    //! Access to pixel value with Dirichlet boundary conditions for the coordinate (\c pos).
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \param out_value Default value returned if \c offset is outside image bounds.
       \note <tt>list.atNXYZ(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZ(x,y,z,c);</tt>.
    **/
    T& atN(const int pos, const int x, const int y, const int z, const int c, const T& out_value) {
      return (pos<0 || pos>=width())?(cimg::temporary(out_value)=out_value):(*this)(pos,x,y,z,c);
    }

    //! Access to pixel value with Dirichlet boundary conditions for the coordinate (\c pos) \const.
    T atN(const int pos, const int x, const int y, const int z, const int c, const T& out_value) const {
      return (pos<0 || pos>=width())?out_value:(*this)(pos,x,y,z,c);
    }

    //! Return pixel value with Neumann boundary conditions for the coordinate (\c pos).
    /**
       \param pos Index of the image element to access.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \param c C-coordinate of the pixel value.
       \note <tt>list.atNXYZ(p,x,y,z,c);</tt> is equivalent to <tt>list[p].atXYZ(x,y,z,c);</tt>.
    **/
    T& atN(const int pos, const int x=0, const int y=0, const int z=0, const int c=0) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atN(): Empty instance.",
                                    cimglist_instance);
      return _atN(pos,x,y,z,c);
    }

    //! Return pixel value with Neumann boundary conditions for the coordinate (\c pos) \const.
    T atN(const int pos, const int x=0, const int y=0, const int z=0, const int c=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "atN(): Empty instance.",
                                    cimglist_instance);
      return _atN(pos,x,y,z,c);
    }

    T& _atN(const int pos, const int x=0, const int y=0, const int z=0, const int c=0) {
      return _data[cimg::cut(pos,0,width() - 1)](x,y,z,c);
    }

    T _atN(const int pos, const int x=0, const int y=0, const int z=0, const int c=0) const {
      return _data[cimg::cut(pos,0,width() - 1)](x,y,z,c);
    }

    //@}
    //-------------------------------------
    //
    //! \name Instance Checking
    //@{
    //-------------------------------------

    //! Return \c true if list is empty.
    /**
    **/
    bool is_empty() const {
      return (!_data || !_width);
    }

    //! Test if number of image elements is equal to specified value.
    /**
        \param size_n Number of image elements to test.
    **/
    bool is_sameN(const unsigned int size_n) const {
      return _width==size_n;
    }

    //! Test if number of image elements is equal between two images lists.
    /**
        \param list Input list to compare with.
    **/
    template<typename t>
    bool is_sameN(const CImgList<t>& list) const {
      return is_sameN(list._width);
    }

    // Define useful functions to check list dimensions.
    // (cannot be documented because macro-generated).
#define _cimglist_def_is_same1(axis) \
    bool is_same##axis(const unsigned int val) const { \
      bool res = true; \
      for (unsigned int l = 0; l<_width && res; ++l) res = _data[l].is_same##axis(val); \
      return res; \
    } \
    bool is_sameN##axis(const unsigned int n, const unsigned int val) const { \
      return is_sameN(n) && is_same##axis(val); \
    } \

#define _cimglist_def_is_same2(axis1,axis2) \
    bool is_same##axis1##axis2(const unsigned int val1, const unsigned int val2) const { \
      bool res = true; \
      for (unsigned int l = 0; l<_width && res; ++l) res = _data[l].is_same##axis1##axis2(val1,val2); \
      return res; \
    } \
    bool is_sameN##axis1##axis2(const unsigned int n, const unsigned int val1, const unsigned int val2) const { \
      return is_sameN(n) && is_same##axis1##axis2(val1,val2); \
    } \

#define _cimglist_def_is_same3(axis1,axis2,axis3) \
    bool is_same##axis1##axis2##axis3(const unsigned int val1, const unsigned int val2, \
                                      const unsigned int val3) const { \
      bool res = true; \
      for (unsigned int l = 0; l<_width && res; ++l) res = _data[l].is_same##axis1##axis2##axis3(val1,val2,val3); \
      return res; \
    } \
    bool is_sameN##axis1##axis2##axis3(const unsigned int n, const unsigned int val1, \
                                       const unsigned int val2, const unsigned int val3) const { \
      return is_sameN(n) && is_same##axis1##axis2##axis3(val1,val2,val3); \
    } \

#define _cimglist_def_is_same(axis) \
    template<typename t> bool is_same##axis(const CImg<t>& img) const { \
      bool res = true; \
      for (unsigned int l = 0; l<_width && res; ++l) res = _data[l].is_same##axis(img); \
      return res; \
    } \
    template<typename t> bool is_same##axis(const CImgList<t>& list) const { \
      const unsigned int lmin = std::min(_width,list._width); \
      bool res = true; \
      for (unsigned int l = 0; l<lmin && res; ++l) res = _data[l].is_same##axis(list[l]); \
      return res; \
    } \
    template<typename t> bool is_sameN##axis(const unsigned int n, const CImg<t>& img) const { \
      return (is_sameN(n) && is_same##axis(img)); \
    } \
    template<typename t> bool is_sameN##axis(const CImgList<t>& list) const { \
      return (is_sameN(list) && is_same##axis(list)); \
    }

    _cimglist_def_is_same(XY)
    _cimglist_def_is_same(XZ)
    _cimglist_def_is_same(XC)
    _cimglist_def_is_same(YZ)
    _cimglist_def_is_same(YC)
    _cimglist_def_is_same(XYZ)
    _cimglist_def_is_same(XYC)
    _cimglist_def_is_same(YZC)
    _cimglist_def_is_same(XYZC)
    _cimglist_def_is_same1(X)
    _cimglist_def_is_same1(Y)
    _cimglist_def_is_same1(Z)
    _cimglist_def_is_same1(C)
    _cimglist_def_is_same2(X,Y)
    _cimglist_def_is_same2(X,Z)
    _cimglist_def_is_same2(X,C)
    _cimglist_def_is_same2(Y,Z)
    _cimglist_def_is_same2(Y,C)
    _cimglist_def_is_same2(Z,C)
    _cimglist_def_is_same3(X,Y,Z)
    _cimglist_def_is_same3(X,Y,C)
    _cimglist_def_is_same3(X,Z,C)
    _cimglist_def_is_same3(Y,Z,C)

    //! Test if dimensions of each image of the list match specified arguments.
    /**
      \param dx Checked image width.
      \param dy Checked image height.
      \param dz Checked image depth.
      \param dc Checked image spectrum.
    **/
    bool is_sameXYZC(const unsigned int dx, const unsigned int dy,
                     const unsigned int dz, const unsigned int dc) const {
      bool res = true;
      for (unsigned int l = 0; l<_width && res; ++l) res = _data[l].is_sameXYZC(dx,dy,dz,dc);
      return res;
    }

    //! Test if list dimensions match specified arguments.
    /**
       \param n Number of images in the list.
       \param dx Checked image width.
       \param dy Checked image height.
       \param dz Checked image depth.
       \param dc Checked image spectrum.
    **/
    bool is_sameNXYZC(const unsigned int n,
                      const unsigned int dx, const unsigned int dy,
                      const unsigned int dz, const unsigned int dc) const {
      return is_sameN(n) && is_sameXYZC(dx,dy,dz,dc);
    }

    //! Test if list contains one particular pixel location.
    /**
       \param n Index of the image whom checked pixel value belong to.
       \param x X-coordinate of the checked pixel value.
       \param y Y-coordinate of the checked pixel value.
       \param z Z-coordinate of the checked pixel value.
       \param c C-coordinate of the checked pixel value.
    **/
    bool containsNXYZC(const int n, const int x=0, const int y=0, const int z=0, const int c=0) const {
      if (is_empty()) return false;
      return n>=0 && n<width() && x>=0 && x<_data[n].width() && y>=0 && y<_data[n].height() &&
        z>=0 && z<_data[n].depth() && c>=0 && c<_data[n].spectrum();
    }

    //! Test if list contains image with specified index.
    /**
       \param n Index of the checked image.
    **/
    bool containsN(const int n) const {
      if (is_empty()) return false;
      return n>=0 && n<width();
    }

    //! Test if one image of the list contains the specified referenced value.
    /**
       \param pixel Reference to pixel value to test.
       \param[out] n Index of image containing the pixel value, if test succeeds.
       \param[out] x X-coordinate of the pixel value, if test succeeds.
       \param[out] y Y-coordinate of the pixel value, if test succeeds.
       \param[out] z Z-coordinate of the pixel value, if test succeeds.
       \param[out] c C-coordinate of the pixel value, if test succeeds.
       \note If true, set coordinates (n,x,y,z,c).
    **/
    template<typename t>
    bool contains(const T& pixel, t& n, t& x, t&y, t& z, t& c) const {
      if (is_empty()) return false;
      cimglist_for(*this,l) if (_data[l].contains(pixel,x,y,z,c)) { n = (t)l; return true; }
      return false;
    }

    //! Test if one of the image list contains the specified referenced value.
    /**
       \param pixel Reference to pixel value to test.
       \param[out] n Index of image containing the pixel value, if test succeeds.
       \param[out] x X-coordinate of the pixel value, if test succeeds.
       \param[out] y Y-coordinate of the pixel value, if test succeeds.
       \param[out] z Z-coordinate of the pixel value, if test succeeds.
       \note If true, set coordinates (n,x,y,z).
    **/
    template<typename t>
    bool contains(const T& pixel, t& n, t& x, t&y, t& z) const {
      t c;
      return contains(pixel,n,x,y,z,c);
    }

    //! Test if one of the image list contains the specified referenced value.
    /**
       \param pixel Reference to pixel value to test.
       \param[out] n Index of image containing the pixel value, if test succeeds.
       \param[out] x X-coordinate of the pixel value, if test succeeds.
       \param[out] y Y-coordinate of the pixel value, if test succeeds.
       \note If true, set coordinates (n,x,y).
    **/
    template<typename t>
    bool contains(const T& pixel, t& n, t& x, t&y) const {
      t z, c;
      return contains(pixel,n,x,y,z,c);
    }

    //! Test if one of the image list contains the specified referenced value.
    /**
       \param pixel Reference to pixel value to test.
       \param[out] n Index of image containing the pixel value, if test succeeds.
       \param[out] x X-coordinate of the pixel value, if test succeeds.
       \note If true, set coordinates (n,x).
    **/
    template<typename t>
    bool contains(const T& pixel, t& n, t& x) const {
      t y, z, c;
      return contains(pixel,n,x,y,z,c);
    }

    //! Test if one of the image list contains the specified referenced value.
    /**
       \param pixel Reference to pixel value to test.
       \param[out] n Index of image containing the pixel value, if test succeeds.
       \note If true, set coordinates (n).
    **/
    template<typename t>
    bool contains(const T& pixel, t& n) const {
      t x, y, z, c;
      return contains(pixel,n,x,y,z,c);
    }

    //! Test if one of the image list contains the specified referenced value.
    /**
       \param pixel Reference to pixel value to test.
    **/
    bool contains(const T& pixel) const {
      unsigned int n, x, y, z, c;
      return contains(pixel,n,x,y,z,c);
    }

    //! Test if the list contains the image 'img'.
    /**
       \param img Reference to image to test.
       \param[out] n Index of image in the list, if test succeeds.
       \note If true, returns the position (n) of the image in the list.
    **/
    template<typename t>
    bool contains(const CImg<T>& img, t& n) const {
      if (is_empty()) return false;
      const CImg<T> *const ptr = &img;
      cimglist_for(*this,i) if (_data + i==ptr) { n = (t)i; return true; }
      return false;
    }

    //! Test if the list contains the image img.
    /**
       \param img Reference to image to test.
    **/
    bool contains(const CImg<T>& img) const {
      unsigned int n;
      return contains(img,n);
    }

    //@}
    //-------------------------------------
    //
    //! \name Mathematical Functions
    //@{
    //-------------------------------------

    //! Return a reference to the minimum pixel value of the instance list.
    /**
    **/
    T& min() {
      bool is_all_empty = true;
      T *ptr_min = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_min = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "min(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T min_value = *ptr_min;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) if (*ptrs<min_value) min_value = *(ptr_min=ptrs);
      }
      return *ptr_min;
    }

    //! Return a reference to the minimum pixel value of the instance list \const.
    const T& min() const {
      bool is_all_empty = true;
      T *ptr_min = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_min = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "min(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T min_value = *ptr_min;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) if (*ptrs<min_value) min_value = *(ptr_min=ptrs);
      }
      return *ptr_min;
    }

    //! Return a reference to the maximum pixel value of the instance list.
    /**
    **/
    T& max() {
      bool is_all_empty = true;
      T *ptr_max = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_max = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "max(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T max_value = *ptr_max;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) if (*ptrs>max_value) max_value = *(ptr_max=ptrs);
      }
      return *ptr_max;
    }

    //! Return a reference to the maximum pixel value of the instance list \const.
    const T& max() const {
      bool is_all_empty = true;
      T *ptr_max = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_max = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "max(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T max_value = *ptr_max;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) if (*ptrs>max_value) max_value = *(ptr_max=ptrs);
      }
      return *ptr_max;
    }

    //! Return a reference to the minimum pixel value of the instance list and return the maximum vvalue as well.
    /**
       \param[out] max_val Value of the maximum value found.
    **/
    template<typename t>
    T& min_max(t& max_val) {
      bool is_all_empty = true;
      T *ptr_min = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_min = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "min_max(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T min_value = *ptr_min, max_value = min_value;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) {
          const T val = *ptrs;
          if (val<min_value) { min_value = val; ptr_min = ptrs; }
          if (val>max_value) max_value = val;
        }
      }
      max_val = (t)max_value;
      return *ptr_min;
    }

    //! Return a reference to the minimum pixel value of the instance list and return the maximum vvalue as well \const.
    /**
       \param[out] max_val Value of the maximum value found.
    **/
    template<typename t>
    const T& min_max(t& max_val) const {
      bool is_all_empty = true;
      T *ptr_min = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_min = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "min_max(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T min_value = *ptr_min, max_value = min_value;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) {
          const T val = *ptrs;
          if (val<min_value) { min_value = val; ptr_min = ptrs; }
          if (val>max_value) max_value = val;
        }
      }
      max_val = (t)max_value;
      return *ptr_min;
    }

    //! Return a reference to the minimum pixel value of the instance list and return the minimum value as well.
    /**
       \param[out] min_val Value of the minimum value found.
    **/
    template<typename t>
    T& max_min(t& min_val) {
      bool is_all_empty = true;
      T *ptr_max = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_max = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "max_min(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T min_value = *ptr_max, max_value = min_value;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) {
          const T val = *ptrs;
          if (val>max_value) { max_value = val; ptr_max = ptrs; }
          if (val<min_value) min_value = val;
        }
      }
      min_val = (t)min_value;
      return *ptr_max;
    }

    //! Return a reference to the minimum pixel value of the instance list and return the minimum value as well \const.
    template<typename t>
    const T& max_min(t& min_val) const {
      bool is_all_empty = true;
      T *ptr_max = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_max = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "max_min(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T min_value = *ptr_max, max_value = min_value;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) {
          const T val = *ptrs;
          if (val>max_value) { max_value = val; ptr_max = ptrs; }
          if (val<min_value) min_value = val;
        }
      }
      min_val = (t)min_value;
      return *ptr_max;
    }

    //@}
#endif
