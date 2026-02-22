#ifndef CIMG_MODULE_LIST_FFT_DECL_H
#define CIMG_MODULE_LIST_FFT_DECL_H

    CImgList<T>& FFT(const char axis, const bool invert=false);

    CImgList<Tfloat> get_FFT(const char axis, const bool invert=false) const;

    CImgList<T>& FFT(const bool invert=false);

    CImgList<Tfloat> get_FFT(const bool invert=false) const;

    CImgList<T>& reverse_object3d();

    CImgList<T> get_reverse_object3d() const;

#endif
