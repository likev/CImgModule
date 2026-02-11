#ifndef CIMG_MODULE_IMAGE_POINTWISE_H
#define CIMG_MODULE_IMAGE_POINTWISE_H
#define _cimg_create_pointwise_functions(name,func,min_size) \
    CImg<T>& name() { \
      if (is_empty()) return *this; \
      cimg_openmp_for(*this,func((typename cimg::superset<T,float>::type)*ptr),min_size,T); \
      return *this; \
    } \
    CImg<Tfloat> get_##name() const { \
      return CImg<Tfloat>(*this,false).name(); \
    }

    //! Compute the square value of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its square value \f$I_{(x,y,z,c)}^2\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
       \par Example
       \code
       const CImg<float> img("reference.jpg");
       (img,img.get_sqr().normalize(0,255)).display();
       \endcode
       \image html ref_sqr.jpg
    **/
    _cimg_create_pointwise_functions(sqr,cimg::sqr,524288)

    //! Compute the square root of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its square root \f$\sqrt{I_{(x,y,z,c)}}\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
       \par Example
       \code
       const CImg<float> img("reference.jpg");
       (img,img.get_sqrt().normalize(0,255)).display();
       \endcode
       \image html ref_sqrt.jpg
    **/
    _cimg_create_pointwise_functions(sqrt,std::sqrt,8192)

    //! Compute the exponential of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its exponential \f$e^{I_{(x,y,z,c)}}\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(exp,std::exp,4096)

    //! Compute the error function of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its error function.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
#if cimg_use_cpp11==1
    _cimg_create_pointwise_functions(erf,std::erf,4096)
#endif

    //! Compute the logarithm of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its logarithm
       \f$\mathrm{log}_{e}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(log,std::log,262144)

    //! Compute the base-2 logarithm of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its base-2 logarithm
       \f$\mathrm{log}_{2}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(log2,cimg::log2,4096)

    //! Compute the base-10 logarithm of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its base-10 logarithm
       \f$\mathrm{log}_{10}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(log10,std::log10,4096)

    //! Compute the absolute value of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its absolute value \f$|I_{(x,y,z,c)}|\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(abs,cimg::abs,524288)

    //! Compute the sign of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its sign
       \f$\mathrm{sign}(I_{(x,y,z,c)})\f$.
       \note
       - The sign is set to:
         - \c 1 if pixel value is strictly positive.
         - \c -1 if pixel value is strictly negative.
         - \c 0 if pixel value is equal to \c 0.
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(sign,cimg::sign,32768)

    //! Compute the cosine of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its cosine \f$\cos(I_{(x,y,z,c)})\f$.
       \note
       - Pixel values are regarded as being in \e radian.
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(cos,std::cos,8192)

    //! Compute the sine of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its sine \f$\sin(I_{(x,y,z,c)})\f$.
       \note
       - Pixel values are regarded as being in \e radian.
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(sin,std::sin,8192)

    //! Compute the sinc of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its sinc
       \f$\mathrm{sinc}(I_{(x,y,z,c)})\f$.
       \note
       - Pixel values are regarded as being exin \e radian.
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(sinc,cimg::sinc,2048)

    //! Compute the tangent of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its tangent \f$\tan(I_{(x,y,z,c)})\f$.
       \note
       - Pixel values are regarded as being exin \e radian.
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(tan,std::tan,2048)

    //! Compute the hyperbolic cosine of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its hyperbolic cosine
       \f$\mathrm{cosh}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(cosh,std::cosh,2048)

    //! Compute the hyperbolic sine of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its hyperbolic sine
       \f$\mathrm{sinh}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(sinh,std::sinh,2048)

    //! Compute the hyperbolic tangent of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its hyperbolic tangent
       \f$\mathrm{tanh}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(tanh,std::tanh,2048)

    //! Compute the arccosine of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its arccosine
       \f$\mathrm{acos}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(acos,std::acos,8192)

    //! Compute the arcsine of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its arcsine
       \f$\mathrm{asin}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(asin,std::asin,8192)

    //! Compute the arctangent of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its arctangent
       \f$\mathrm{atan}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(atan,std::atan,8192)

    //! Compute the arctangent2 of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its arctangent2
       \f$\mathrm{atan2}(I_{(x,y,z,c)})\f$.
       \param img Image whose pixel values specify the second argument of the \c atan2() function.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
       \par Example
       \code
       const CImg<float>
          img_x(100,100,1,1,"x-w/2",false), // Define an horizontal centered gradient, from '-width/2' to 'width/2'
          img_y(100,100,1,1,"y-h/2",false), // Define a vertical centered gradient, from '-height/2' to 'height/2'
          img_atan2 = img_y.get_atan2(img_x); // Compute atan2(y,x) for each pixel value
       (img_x,img_y,img_atan2).display();
       \endcode
    **/
    template<typename t>
    CImg<T>& atan2(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return atan2(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)std::atan2((double)*ptrd,(double)*(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)std::atan2((double)*ptrd,(double)*(ptrs++));
      }
      return *this;
    }

    //! Compute the arctangent2 of each pixel value \newinstance.
    template<typename t>
    CImg<Tfloat> get_atan2(const CImg<t>& img) const {
      return CImg<Tfloat>(*this,false).atan2(img);
    }

    //! Compute the hyperbolic arccosine of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its arccosineh
       \f$\mathrm{acosh}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(acosh,cimg::acosh,8192)

    //! Compute the hyperbolic arcsine of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its hyperbolic arcsine
       \f$\mathrm{asinh}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(asinh,cimg::asinh,8192)

    //! Compute the hyperbolic arctangent of each pixel value.
    /**
       Replace each pixel value \f$I_{(x,y,z,c)}\f$ of the image instance by its hyperbolic arctangent
       \f$\mathrm{atanh}(I_{(x,y,z,c)})\f$.
       \note
       - The \inplace of this method statically casts the computed values to the pixel type \c T.
       - The \newinstance returns a \c CImg<float> image, if the pixel type \c T is \e not float-valued.
    **/
    _cimg_create_pointwise_functions(atanh,cimg::atanh,8192)

#endif
