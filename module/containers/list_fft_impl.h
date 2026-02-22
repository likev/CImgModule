#ifndef CIMG_MODULE_LIST_FFT_IMPL_H
#define CIMG_MODULE_LIST_FFT_IMPL_H

  template<typename T>
  CImgList<T>& CImgList<T>::FFT(const char axis, const bool invert) {
    if (is_empty()) return *this;
    if (_width==1) insert(1);
    if (_width>2)
      cimg::warn(_cimglist_instance
                 "FFT(): Instance has more than 2 images",
                 cimglist_instance);
    CImg<T>::FFT(_data[0],_data[1],axis,invert);
    return *this;
  }

  template<typename T>
  CImgList<typename CImgList<T>::Tfloat> CImgList<T>::get_FFT(const char axis, const bool invert) const {
    return CImgList<Tfloat>(*this,false).FFT(axis,invert);
  }

  template<typename T>
  CImgList<T>& CImgList<T>::FFT(const bool invert) {
    if (is_empty()) return *this;
    if (_width==1) insert(1);
    if (_width>2)
      cimg::warn(_cimglist_instance
                 "FFT(): Instance has more than 2 images",
                 cimglist_instance);

    CImg<T>::FFT(_data[0],_data[1],invert);
    return *this;
  }

  template<typename T>
  CImgList<typename CImgList<T>::Tfloat> CImgList<T>::get_FFT(const bool invert) const {
    return CImgList<Tfloat>(*this,false).FFT(invert);
  }

  template<typename T>
  CImgList<T>& CImgList<T>::reverse_object3d() {
    cimglist_for(*this,l) {
      CImg<T>& p = _data[l];
      switch (p.size()) {
      case 2 : case 3: cimg::swap(p[0],p[1]); break;
      case 6 : cimg::swap(p[0],p[1],p[2],p[4],p[3],p[5]); break;
      case 9 : cimg::swap(p[0],p[1],p[3],p[5],p[4],p[6]); break;
      case 4 : cimg::swap(p[0],p[1],p[2],p[3]); break;
      case 12 : cimg::swap(p[0],p[1],p[2],p[3],p[4],p[6],p[5],p[7],p[8],p[10],p[9],p[11]); break;
      }
    }
    return *this;
  }

  template<typename T>
  CImgList<T> CImgList<T>::get_reverse_object3d() const {
    return (+*this).reverse_object3d();
  }

#endif
