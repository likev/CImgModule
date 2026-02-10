#ifndef CIMG_MODULE_MATH_ALGORITHMS_H
#define CIMG_MODULE_MATH_ALGORITHMS_H

namespace cimg_library {
  template<typename T> struct CImg;
}

namespace cimg_library {
/*------------------------------------------------
   #
   #
   #   Definition of mathematical operators and
   #   external functions.
   #
   #
   -------------------------------------------------*/

#define _cimg_create_operator(typ) \
  template<typename T> \
  inline CImg<typename cimg::superset<T,typ>::type> operator+(const typ val, const CImg<T>& img) { \
    return img.operator+(val); \
  } \
  template<typename T> \
  inline CImg<typename cimg::superset<T,typ>::type> operator-(const typ val, const CImg<T>& img) { \
    typedef typename cimg::superset<T,typ>::type Tt; \
    return CImg<Tt>(img._width,img._height,img._depth,img._spectrum,val)-=img; \
  } \
  template<typename T> \
  inline CImg<typename cimg::superset<T,typ>::type> operator*(const typ val, const CImg<T>& img) { \
    return img.operator*(val); \
  } \
  template<typename T> \
  inline CImg<typename cimg::superset<T,typ>::type> operator/(const typ val, const CImg<T>& img) { \
    return val*img.get_invert(); \
  } \
  template<typename T> \
  inline CImg<typename cimg::superset<T,typ>::type> operator&(const typ val, const CImg<T>& img) { \
    return img.operator&(val); \
  } \
  template<typename T> \
  inline CImg<typename cimg::superset<T,typ>::type> operator|(const typ val, const CImg<T>& img) { \
    return img.operator|(val); \
  } \
  template<typename T> \
  inline CImg<typename cimg::superset<T,typ>::type> operator^(const typ val, const CImg<T>& img) { \
    return img.operator^(val); \
  } \
  template<typename T> \
  inline bool operator==(const typ val, const CImg<T>& img) {   \
    return img.operator==(val); \
  } \
  template<typename T> \
  inline bool operator!=(const typ val, const CImg<T>& img) { \
    return img.operator!=(val); \
  }

  _cimg_create_operator(bool)
  _cimg_create_operator(unsigned char)
  _cimg_create_operator(char)
  _cimg_create_operator(signed char)
  _cimg_create_operator(unsigned short)
  _cimg_create_operator(short)
  _cimg_create_operator(unsigned int)
  _cimg_create_operator(int)
  _cimg_create_operator(cimg_uint64)
  _cimg_create_operator(cimg_int64)
  _cimg_create_operator(float)
  _cimg_create_operator(double)
  _cimg_create_operator(long double)

  template<typename T>
  inline CImg<_cimg_Tfloat> operator+(const char *const expression, const CImg<T>& img) {
    return img + expression;
  }

  template<typename T>
  inline CImg<_cimg_Tfloat> operator-(const char *const expression, const CImg<T>& img) {
    return CImg<_cimg_Tfloat>(img,false).fill(expression,true)-=img;
  }

  template<typename T>
  inline CImg<_cimg_Tfloat> operator*(const char *const expression, const CImg<T>& img) {
    return img*expression;
  }

  template<typename T>
  inline CImg<_cimg_Tfloat> operator/(const char *const expression, const CImg<T>& img) {
    return expression*img.get_invert();
  }

  template<typename T>
  inline CImg<T> operator&(const char *const expression, const CImg<T>& img) {
    return img & expression;
  }

  template<typename T>
  inline CImg<T> operator|(const char *const expression, const CImg<T>& img) {
    return img | expression;
  }

  template<typename T>
  inline CImg<T> operator^(const char *const expression, const CImg<T>& img) {
    return img ^ expression;
  }

  template<typename T>
  inline bool operator==(const char *const expression, const CImg<T>& img) {
    return img==expression;
  }

  template<typename T>
  inline bool operator!=(const char *const expression, const CImg<T>& img) {
    return img!=expression;
  }

  template<typename T>
  inline CImg<T> transpose(const CImg<T>& instance) {
    return instance.get_transpose();
  }

  template<typename T>
  inline CImg<_cimg_Tfloat> invert(const CImg<T>& instance) {
    return instance.get_invert();
  }

#define _cimg_create_pointwise_function(name) \
  template<typename T> \
  inline CImg<_cimg_Tfloat> name(const CImg<T>& instance) { \
    return instance.get_##name(); \
  }

  _cimg_create_pointwise_function(abs)
  _cimg_create_pointwise_function(acos)
  _cimg_create_pointwise_function(acosh)
  _cimg_create_pointwise_function(asin)
  _cimg_create_pointwise_function(asinh)
  _cimg_create_pointwise_function(atan)
  _cimg_create_pointwise_function(atanh)
  _cimg_create_pointwise_function(cos)
  _cimg_create_pointwise_function(cosh)
  _cimg_create_pointwise_function(erf)
  _cimg_create_pointwise_function(exp)
  _cimg_create_pointwise_function(log)
  _cimg_create_pointwise_function(log2)
  _cimg_create_pointwise_function(log10)
  _cimg_create_pointwise_function(sqr)
  _cimg_create_pointwise_function(sqrt)
  _cimg_create_pointwise_function(sign)
  _cimg_create_pointwise_function(sin)
  _cimg_create_pointwise_function(sinc)
  _cimg_create_pointwise_function(sinh)
  _cimg_create_pointwise_function(tan)
  _cimg_create_pointwise_function(tanh)
}

#endif
