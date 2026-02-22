#ifndef CIMG_MODULE_IMAGE_POINTWISE_DECL_H
#define CIMG_MODULE_IMAGE_POINTWISE_DECL_H

#define _cimg_create_pointwise_functions(name,func,min_size) \
    CImg<T>& name(); \
    CImg<Tfloat> get_##name() const;

    _cimg_create_pointwise_functions(sqr,cimg::sqr,524288)
    _cimg_create_pointwise_functions(sqrt,std::sqrt,8192)
    _cimg_create_pointwise_functions(exp,std::exp,4096)
#if cimg_use_cpp11==1
    _cimg_create_pointwise_functions(erf,std::erf,4096)
#endif
    _cimg_create_pointwise_functions(log,std::log,262144)
    _cimg_create_pointwise_functions(log2,cimg::log2,4096)
    _cimg_create_pointwise_functions(log10,std::log10,4096)
    _cimg_create_pointwise_functions(abs,cimg::abs,524288)
    _cimg_create_pointwise_functions(sign,cimg::sign,32768)
    _cimg_create_pointwise_functions(cos,std::cos,8192)
    _cimg_create_pointwise_functions(sin,std::sin,8192)
    _cimg_create_pointwise_functions(sinc,cimg::sinc,2048)
    _cimg_create_pointwise_functions(tan,std::tan,2048)
    _cimg_create_pointwise_functions(cosh,std::cosh,2048)
    _cimg_create_pointwise_functions(sinh,std::sinh,2048)
    _cimg_create_pointwise_functions(tanh,std::tanh,2048)
    _cimg_create_pointwise_functions(acos,std::acos,8192)
    _cimg_create_pointwise_functions(asin,std::asin,8192)
    _cimg_create_pointwise_functions(atan,std::atan,8192)
    _cimg_create_pointwise_functions(acosh,cimg::acosh,8192)
    _cimg_create_pointwise_functions(asinh,cimg::asinh,8192)
    _cimg_create_pointwise_functions(atanh,cimg::atanh,8192)

    template<typename t>
    CImg<T>& atan2(const CImg<t>& img);

    template<typename t>
    CImg<Tfloat> get_atan2(const CImg<t>& img) const;

#undef _cimg_create_pointwise_functions

#endif
