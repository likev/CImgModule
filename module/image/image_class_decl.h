#ifndef CIMG_MODULE_IMAGE_CLASS_DECL_H
#define CIMG_MODULE_IMAGE_CLASS_DECL_H

    //! Simple iterator type, to loop through each pixel value of an image instance.
    /**
       \note
       - The \c CImg<T>::iterator type is defined to be a <tt>T*</tt>.
       - You will seldom have to use iterators in %CImg, most classical operations
         being achieved (often in a faster way) using methods of \c CImg<T>.
       \par Example
       \code
       CImg<float> img("reference.jpg"); // Load image from file
       // Set all pixels to '0', with a CImg iterator.
       for (CImg<float>::iterator it = img.begin(), it<img.end(); ++it) *it = 0;
       img.fill(0); // Do the same with a built-in method
       \endcode
   **/
    typedef T* iterator;

    //! Simple const iterator type, to loop through each pixel value of a \c const image instance.
    /**
       \note
       - The \c CImg<T>::const_iterator type is defined to be a \c const \c T*.
       - You will seldom have to use iterators in %CImg, most classical operations
         being achieved (often in a faster way) using methods of \c CImg<T>.
       \par Example
       \code
       const CImg<float> img("reference.jpg"); // Load image from file
       float sum = 0;
       // Compute sum of all pixel values, with a CImg iterator.
       for (CImg<float>::iterator it = img.begin(), it<img.end(); ++it) sum+=*it;
       const float sum2 = img.sum(); // Do the same with a built-in method
       \endcode
    **/
    typedef const T* const_iterator;

    //! Pixel value type.
    /**
       Refer to the type of the pixel values of an image instance.
       \note
       - The \c CImg<T>::value_type type of a \c CImg<T> is defined to be a \c T.
       - \c CImg<T>::value_type is actually not used in %CImg methods. It has been mainly defined for
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

#endif
