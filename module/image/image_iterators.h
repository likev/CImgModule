#ifndef CIMG_MODULE_IMAGE_ITERATORS_H
#define CIMG_MODULE_IMAGE_ITERATORS_H

  template<typename T>
  auto CImg<T>::begin() -> iterator {
    return _data;
  }

  template<typename T>
  auto CImg<T>::begin() const -> const_iterator {
    return _data;
  }

  template<typename T>
  auto CImg<T>::end() -> iterator {
    return _data + size();
  }

  template<typename T>
  auto CImg<T>::end() const -> const_iterator {
    return _data + size();
  }

  template<typename T>
  T& CImg<T>::front() {
    return *_data;
  }

  template<typename T>
  const T& CImg<T>::front() const {
    return *_data;
  }

  template<typename T>
  T& CImg<T>::back() {
    return *(_data + size() - 1);
  }

  template<typename T>
  const T& CImg<T>::back() const {
    return *(_data + size() - 1);
  }

#endif
