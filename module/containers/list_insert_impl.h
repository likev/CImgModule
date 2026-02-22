#ifndef CIMG_MODULE_LIST_INSERT_IMPL_H
#define CIMG_MODULE_LIST_INSERT_IMPL_H

  template<typename T>
  template<typename t>
  CImgList<T>& CImgList<T>::insert(const CImg<t>& img, const unsigned int pos, const bool is_shared) {
    const unsigned int npos = pos==~0U?_width:pos;
    if (npos>_width)
      throw CImgArgumentException(_cimglist_instance
                                  "insert(): Invalid insertion request of specified image (%u,%u,%u,%u,%p) "
                                  "at position %u.",
                                  cimglist_instance,
                                  img._width,img._height,img._depth,img._spectrum,img._data,npos);
    if (is_shared)
      throw CImgArgumentException(_cimglist_instance
                                  "insert(): Invalid insertion request of specified shared image "
                                  "CImg<%s>(%u,%u,%u,%u,%p) at position %u (pixel types are different).",
                                  cimglist_instance,
                                  img.pixel_type(),img._width,img._height,img._depth,img._spectrum,img._data,npos);

    CImg<T> *const new_data = (++_width>_allocated_width)?new CImg<T>[_allocated_width?(_allocated_width<<=1):
                                                                      (_allocated_width=16)]:0;
    if (!_data) { // Insert new element into empty list
      _data = new_data;
      *_data = img;
    } else {
      if (new_data) { // Insert with re-allocation
        if (npos) std::memcpy((void*)new_data,(void*)_data,sizeof(CImg<T>)*npos);
        if (npos!=_width - 1)
          std::memcpy((void*)(new_data + npos + 1),(void*)(_data + npos),sizeof(CImg<T>)*(_width - 1 - npos));
        std::memset((void*)_data,0,sizeof(CImg<T>)*(_width - 1));
        delete[] _data;
        _data = new_data;
      } else if (npos!=_width - 1) // Insert without re-allocation
        std::memmove((void*)(_data + npos + 1),(void*)(_data + npos),sizeof(CImg<T>)*(_width - 1 - npos));
      _data[npos]._width = _data[npos]._height = _data[npos]._depth = _data[npos]._spectrum = 0;
      _data[npos]._data = 0;
      _data[npos] = img;
    }
    return *this;
  }

  template<typename T>
  CImgList<T>& CImgList<T>::insert(const CImg<T>& img, const unsigned int pos, const bool is_shared) {
    const unsigned int npos = pos==~0U?_width:pos;
    if (npos>_width)
      throw CImgArgumentException(_cimglist_instance
                                  "insert(): Invalid insertion request of specified image (%u,%u,%u,%u,%p) "
                                  "at position %u.",
                                  cimglist_instance,
                                  img._width,img._height,img._depth,img._spectrum,img._data,npos);
    CImg<T> *const new_data = (++_width>_allocated_width)?new CImg<T>[_allocated_width?(_allocated_width<<=1):
                                                                      (_allocated_width=16)]:0;
    if (!_data) { // Insert new element into empty list
      _data = new_data;
      if (is_shared && img) {
        _data->_width = img._width;
        _data->_height = img._height;
        _data->_depth = img._depth;
        _data->_spectrum = img._spectrum;
        _data->_is_shared = true;
        _data->_data = img._data;
      } else *_data = img;
    }
    else {
      if (new_data) { // Insert with re-allocation
        if (npos) std::memcpy((void*)new_data,(void*)_data,sizeof(CImg<T>)*npos);
        if (npos!=_width - 1)
          std::memcpy((void*)(new_data + npos + 1),(void*)(_data + npos),sizeof(CImg<T>)*(_width - 1 - npos));
        if (is_shared && img) {
          new_data[npos]._width = img._width;
          new_data[npos]._height = img._height;
          new_data[npos]._depth = img._depth;
          new_data[npos]._spectrum = img._spectrum;
          new_data[npos]._is_shared = true;
          new_data[npos]._data = img._data;
        } else {
          new_data[npos]._width = new_data[npos]._height = new_data[npos]._depth = new_data[npos]._spectrum = 0;
          new_data[npos]._data = 0;
          new_data[npos] = img;
        }
        std::memset((void*)_data,0,sizeof(CImg<T>)*(_width - 1));
        delete[] _data;
        _data = new_data;
      } else { // Insert without re-allocation
        if (npos!=_width - 1)
          std::memmove((void*)(_data + npos + 1),(void*)(_data + npos),sizeof(CImg<T>)*(_width - 1 - npos));
        if (is_shared && img) {
          _data[npos]._width = img._width;
          _data[npos]._height = img._height;
          _data[npos]._depth = img._depth;
          _data[npos]._spectrum = img._spectrum;
          _data[npos]._is_shared = true;
          _data[npos]._data = img._data;
        } else {
          _data[npos]._width = _data[npos]._height = _data[npos]._depth = _data[npos]._spectrum = 0;
          _data[npos]._data = 0;
          _data[npos] = img;
        }
      }
    }
    return *this;
  }

  template<typename T>
  template<typename t>
  CImgList<T> CImgList<T>::get_insert(const CImg<t>& img, const unsigned int pos, const bool is_shared) const {
    return (+*this).insert(img,pos,is_shared);
  }

  template<typename T>
  CImgList<T>& CImgList<T>::insert(const unsigned int n, const unsigned int pos) {
    CImg<T> empty;
    if (!n) return *this;
    const unsigned int npos = pos==~0U?_width:pos;
    for (unsigned int i = 0; i<n; ++i) insert(empty,npos+i);
    return *this;
  }

  template<typename T>
  CImgList<T> CImgList<T>::get_insert(const unsigned int n, const unsigned int pos) const {
    return (+*this).insert(n,pos);
  }

  template<typename T>
  template<typename t>
  CImgList<T>& CImgList<T>::insert(const unsigned int n, const CImg<t>& img,
                                   const unsigned int pos, const bool is_shared) {
    if (!n) return *this;
    const unsigned int npos = pos==~0U?_width:pos;
    insert(img,npos,is_shared);
    for (unsigned int i = 1; i<n; ++i) insert(_data[npos],npos + i,is_shared);
    return *this;
  }

  template<typename T>
  template<typename t>
  CImgList<T> CImgList<T>::get_insert(const unsigned int n, const CImg<t>& img,
                                      const unsigned int pos, const bool is_shared) const {
    return (+*this).insert(n,img,pos,is_shared);
  }

  template<typename T>
  template<typename t>
  CImgList<T>& CImgList<T>::insert(const CImgList<t>& list, const unsigned int pos, const bool is_shared) {
    const unsigned int npos = pos==~0U?_width:pos;
    if ((void*)this!=(void*)&list) cimglist_for(list,l) insert(list[l],npos + l,is_shared);
    else insert(CImgList<T>(list),npos,is_shared);
    return *this;
  }

  template<typename T>
  template<typename t>
  CImgList<T> CImgList<T>::get_insert(const CImgList<t>& list, const unsigned int pos, const bool is_shared) const {
    return (+*this).insert(list,pos,is_shared);
  }

  template<typename T>
  template<typename t>
  CImgList<T>& CImgList<T>::insert(const unsigned int n, const CImgList<t>& list,
                                   const unsigned int pos, const bool is_shared) {
    if (!n) return *this;
    const unsigned int npos = pos==~0U?_width:pos;
    for (unsigned int i = 0; i<n; ++i) insert(list,npos,is_shared);
    return *this;
  }

  template<typename T>
  template<typename t>
  CImgList<T> CImgList<T>::get_insert(const unsigned int n, const CImgList<t>& list,
                                      const unsigned int pos, const bool is_shared) const {
    return (+*this).insert(n,list,pos,is_shared);
  }

#endif
