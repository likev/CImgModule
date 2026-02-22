#ifndef CIMG_MODULE_IMAGE_CHECKS_H
#define CIMG_MODULE_IMAGE_CHECKS_H

  template<typename T>
  bool CImg<T>::is_shared() const {
    return _is_shared;
  }

  template<typename T>
  bool CImg<T>::is_empty() const {
    return !(_data && _width && _height && _depth && _spectrum);
  }

  template<typename T>
  bool CImg<T>::is_inf() const {
    if (cimg::type<T>::is_float()) cimg_for(*this,p,T) if (cimg::type<T>::is_inf((float)*p)) return true;
    return false;
  }

  template<typename T>
  bool CImg<T>::is_nan() const {
    if (cimg::type<T>::is_float()) cimg_for(*this,p,T) if (cimg::type<T>::is_nan((float)*p)) return true;
    return false;
  }

  template<typename T>
  bool CImg<T>::is_sameX(const unsigned int size_x) const {
    return _width==size_x;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameX(const CImg<t>& img) const {
    return is_sameX(img._width);
  }

  template<typename T>
  bool CImg<T>::is_sameX(const CImgDisplay& disp) const {
    return is_sameX(disp._width);
  }

  template<typename T>
  bool CImg<T>::is_sameY(const unsigned int size_y) const {
    return _height==size_y;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameY(const CImg<t>& img) const {
    return is_sameY(img._height);
  }

  template<typename T>
  bool CImg<T>::is_sameY(const CImgDisplay& disp) const {
    return is_sameY(disp._height);
  }

  template<typename T>
  bool CImg<T>::is_sameZ(const unsigned int size_z) const {
    return _depth==size_z;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameZ(const CImg<t>& img) const {
    return is_sameZ(img._depth);
  }

  template<typename T>
  bool CImg<T>::is_sameC(const unsigned int size_c) const {
    return _spectrum==size_c;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameC(const CImg<t>& img) const {
    return is_sameC(img._spectrum);
  }

  template<typename T>
  bool CImg<T>::is_sameXY(const unsigned int size_x, const unsigned int size_y) const {
    return _width==size_x && _height==size_y;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameXY(const CImg<t>& img) const {
    return is_sameXY(img._width,img._height);
  }

  template<typename T>
  bool CImg<T>::is_sameXY(const CImgDisplay& disp) const {
    return is_sameXY(disp._width,disp._height);
  }

  template<typename T>
  bool CImg<T>::is_sameXZ(const unsigned int size_x, const unsigned int size_z) const {
    return _width==size_x && _depth==size_z;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameXZ(const CImg<t>& img) const {
    return is_sameXZ(img._width,img._depth);
  }

  template<typename T>
  bool CImg<T>::is_sameXC(const unsigned int size_x, const unsigned int size_c) const {
    return _width==size_x && _spectrum==size_c;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameXC(const CImg<t>& img) const {
    return is_sameXC(img._width,img._spectrum);
  }

  template<typename T>
  bool CImg<T>::is_sameYZ(const unsigned int size_y, const unsigned int size_z) const {
    return _height==size_y && _depth==size_z;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameYZ(const CImg<t>& img) const {
    return is_sameYZ(img._height,img._depth);
  }

  template<typename T>
  bool CImg<T>::is_sameYC(const unsigned int size_y, const unsigned int size_c) const {
    return _height==size_y && _spectrum==size_c;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameYC(const CImg<t>& img) const {
    return is_sameYC(img._height,img._spectrum);
  }

  template<typename T>
  bool CImg<T>::is_sameZC(const unsigned int size_z, const unsigned int size_c) const {
    return _depth==size_z && _spectrum==size_c;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameZC(const CImg<t>& img) const {
    return is_sameZC(img._depth,img._spectrum);
  }

  template<typename T>
  bool CImg<T>::is_sameXYZ(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z) const {
    return is_sameXY(size_x,size_y) && _depth==size_z;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameXYZ(const CImg<t>& img) const {
    return is_sameXYZ(img._width,img._height,img._depth);
  }

  template<typename T>
  bool CImg<T>::is_sameXYC(const unsigned int size_x, const unsigned int size_y, const unsigned int size_c) const {
    return is_sameXY(size_x,size_y) && _spectrum==size_c;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameXYC(const CImg<t>& img) const {
    return is_sameXYC(img._width,img._height,img._spectrum);
  }

  template<typename T>
  bool CImg<T>::is_sameXZC(const unsigned int size_x, const unsigned int size_z, const unsigned int size_c) const {
    return is_sameXZ(size_x,size_z) && _spectrum==size_c;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameXZC(const CImg<t>& img) const {
    return is_sameXZC(img._width,img._depth,img._spectrum);
  }

  template<typename T>
  bool CImg<T>::is_sameYZC(const unsigned int size_y, const unsigned int size_z, const unsigned int size_c) const {
    return is_sameYZ(size_y,size_z) && _spectrum==size_c;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameYZC(const CImg<t>& img) const {
    return is_sameYZC(img._height,img._depth,img._spectrum);
  }

  template<typename T>
  bool CImg<T>::is_sameXYZC(const unsigned int size_x, const unsigned int size_y,
                            const unsigned int size_z, const unsigned int size_c) const {
    return is_sameXYZ(size_x,size_y,size_z) && _spectrum==size_c;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_sameXYZC(const CImg<t>& img) const {
    return is_sameXYZC(img._width,img._height,img._depth,img._spectrum);
  }

  template<typename T>
  bool CImg<T>::containsXYZC(const int x, const int y, const int z, const int c) const {
    return x>=0 && x<width() && y>=0 && y<height() && z>=0 && z<depth() && c>=0 && c<spectrum();
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::contains(const T& pixel, t& x, t& y, t& z, t& c) const {
    const ulongT wh = (ulongT)_width*_height, whd = wh*_depth, siz = whd*_spectrum;
    const T *const ppixel = &pixel;
    if (is_empty() || ppixel<_data || ppixel>=_data + siz) return false;
    ulongT off = (ulongT)(ppixel - _data);
    const ulongT nc = off/whd;
    off%=whd;
    const ulongT nz = off/wh;
    off%=wh;
    const ulongT ny = off/_width, nx = off%_width;
    x = (t)nx; y = (t)ny; z = (t)nz; c = (t)nc;
    return true;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::contains(const T& pixel, t& x, t& y, t& z) const {
    const ulongT wh = (ulongT)_width*_height, whd = wh*_depth, siz = whd*_spectrum;
    const T *const ppixel = &pixel;
    if (is_empty() || ppixel<_data || ppixel>=_data + siz) return false;
    ulongT off = ((ulongT)(ppixel - _data))%whd;
    const ulongT nz = off/wh;
    off%=wh;
    const ulongT ny = off/_width, nx = off%_width;
    x = (t)nx; y = (t)ny; z = (t)nz;
    return true;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::contains(const T& pixel, t& x, t& y) const {
    const ulongT wh = (ulongT)_width*_height, siz = wh*_depth*_spectrum;
    const T *const ppixel = &pixel;
    if (is_empty() || ppixel<_data || ppixel>=_data + siz) return false;
    ulongT off = ((unsigned int)(ppixel - _data))%wh;
    const ulongT ny = off/_width, nx = off%_width;
    x = (t)nx; y = (t)ny;
    return true;
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::contains(const T& pixel, t& x) const {
    const T *const ppixel = &pixel;
    if (is_empty() || ppixel<_data || ppixel>=_data + size()) return false;
    x = (t)(((ulongT)(ppixel - _data))%_width);
    return true;
  }

  template<typename T>
  bool CImg<T>::contains(const T& pixel) const {
    const T *const ppixel = &pixel;
    return !is_empty() && ppixel>=_data && ppixel<_data + size();
  }

  template<typename T>
  template<typename t>
  bool CImg<T>::is_overlapped(const CImg<t>& img) const {
    const ulongT csiz = size(), isiz = img.size();
    return !((void*)(_data + csiz)<=(void*)img._data || (void*)_data>=(void*)(img._data + isiz));
  }

#endif
