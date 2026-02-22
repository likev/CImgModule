#ifndef CIMG_MODULE_LIST_INSTANCE_IMPL_H
#define CIMG_MODULE_LIST_INSTANCE_IMPL_H

  template<typename T>
  int CImgList<T>::width() const {
    return (int)_width;
  }

  template<typename T>
  unsigned int CImgList<T>::size() const {
    return _width;
  }

  template<typename T>
  CImg<T>* CImgList<T>::data() {
    return _data;
  }

  template<typename T>
  const CImg<T>* CImgList<T>::data() const {
    return _data;
  }

#if cimg_verbosity>=3
  template<typename T>
  CImg<T>* CImgList<T>::data(const unsigned int pos) {
    if (pos>=size())
      cimg::warn(_cimglist_instance
                 "data(): Invalid pointer request, at position [%u].",
                 cimglist_instance,
                 pos);
    return _data + pos;
  }

  template<typename T>
  const CImg<T>* CImgList<T>::data(const unsigned int l) const {
    return const_cast<CImgList<T>*>(this)->data(l);
  }
#else
  template<typename T>
  CImg<T>* CImgList<T>::data(const unsigned int l) {
    return _data + l;
  }

  template<typename T>
  const CImg<T>* CImgList<T>::data(const unsigned int l) const {
    return _data + l;
  }
#endif

  template<typename T>
  auto CImgList<T>::begin() -> iterator {
    return _data;
  }

  template<typename T>
  auto CImgList<T>::begin() const -> const_iterator {
    return _data;
  }

  template<typename T>
  auto CImgList<T>::end() -> iterator {
    return _data + _width;
  }

  template<typename T>
  auto CImgList<T>::end() const -> const_iterator {
    return _data + _width;
  }

  template<typename T>
  CImg<T>& CImgList<T>::front() {
    return *_data;
  }

  template<typename T>
  const CImg<T>& CImgList<T>::front() const {
    return *_data;
  }

  template<typename T>
  const CImg<T>& CImgList<T>::back() const {
    return *(_data + _width - 1);
  }

  template<typename T>
  CImg<T>& CImgList<T>::back() {
    return *(_data + _width - 1);
  }

  template<typename T>
  CImg<T>& CImgList<T>::at(const int pos) {
    if (is_empty())
      throw CImgInstanceException(_cimglist_instance
                                  "at(): Empty instance.",
                                  cimglist_instance);

    return _data[cimg::cut(pos,0,width() - 1)];
  }

#endif
