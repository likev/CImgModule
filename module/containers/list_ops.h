#ifndef CIMG_MODULE_LIST_OPS_H
#define CIMG_MODULE_LIST_OPS_H

    //---------------------------
    //
    //! \name List Manipulation
    //@{
    //---------------------------

    //! Insert a copy of the image \c img into the current image list, at position \c pos.
    /**
        \param img Image to insert a copy to the list.
        \param pos Index of the insertion.
        \param is_shared Tells if the inserted image is a shared copy of \c img or not.
    **/
    template<typename t>
    CImgList<T>& insert(const CImg<t>& img, const unsigned int pos=~0U, const bool is_shared=false) {
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

    //! Insert a copy of the image \c img into the current image list, at position \c pos \specialization.
    CImgList<T>& insert(const CImg<T>& img, const unsigned int pos=~0U, const bool is_shared=false) {
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

    //! Insert a copy of the image \c img into the current image list, at position \c pos \newinstance.
    template<typename t>
    CImgList<T> get_insert(const CImg<t>& img, const unsigned int pos=~0U, const bool is_shared=false) const {
      return (+*this).insert(img,pos,is_shared);
    }

    //! Insert n empty images img into the current image list, at position \p pos.
    /**
       \param n Number of empty images to insert.
       \param pos Index of the insertion.
    **/
    CImgList<T>& insert(const unsigned int n, const unsigned int pos=~0U) {
      CImg<T> empty;
      if (!n) return *this;
      const unsigned int npos = pos==~0U?_width:pos;
      for (unsigned int i = 0; i<n; ++i) insert(empty,npos+i);
      return *this;
    }

    //! Insert n empty images img into the current image list, at position \p pos \newinstance.
    CImgList<T> get_insert(const unsigned int n, const unsigned int pos=~0U) const {
      return (+*this).insert(n,pos);
    }

    //! Insert \c n copies of the image \c img into the current image list, at position \c pos.
    /**
       \param n Number of image copies to insert.
       \param img Image to insert by copy.
       \param pos Index of the insertion.
       \param is_shared Tells if inserted images are shared copies of \c img or not.
    **/
    template<typename t>
    CImgList<T>& insert(const unsigned int n, const CImg<t>& img, const unsigned int pos=~0U,
                        const bool is_shared=false) {
      if (!n) return *this;
      const unsigned int npos = pos==~0U?_width:pos;
      insert(img,npos,is_shared);
      for (unsigned int i = 1; i<n; ++i) insert(_data[npos],npos + i,is_shared);
      return *this;
    }

    //! Insert \c n copies of the image \c img into the current image list, at position \c pos \newinstance.
    template<typename t>
    CImgList<T> get_insert(const unsigned int n, const CImg<t>& img, const unsigned int pos=~0U,
                           const bool is_shared=false) const {
      return (+*this).insert(n,img,pos,is_shared);
    }

    //! Insert a copy of the image list \c list into the current image list, starting from position \c pos.
    /**
      \param list Image list to insert.
      \param pos Index of the insertion.
      \param is_shared Tells if inserted images are shared copies of images of \c list or not.
    **/
    template<typename t>
    CImgList<T>& insert(const CImgList<t>& list, const unsigned int pos=~0U, const bool is_shared=false) {
      const unsigned int npos = pos==~0U?_width:pos;
      if ((void*)this!=(void*)&list) cimglist_for(list,l) insert(list[l],npos + l,is_shared);
      else insert(CImgList<T>(list),npos,is_shared);
      return *this;
    }

    //! Insert a copy of the image list \c list into the current image list, starting from position \c pos \newinstance.
    template<typename t>
    CImgList<T> get_insert(const CImgList<t>& list, const unsigned int pos=~0U, const bool is_shared=false) const {
      return (+*this).insert(list,pos,is_shared);
    }

    //! Insert n copies of the list \c list at position \c pos of the current list.
    /**
      \param n Number of list copies to insert.
      \param list Image list to insert.
      \param pos Index of the insertion.
      \param is_shared Tells if inserted images are shared copies of images of \c list or not.
    **/
    template<typename t>
    CImgList<T>& insert(const unsigned int n, const CImgList<t>& list, const unsigned int pos=~0U,
                        const bool is_shared=false) {
      if (!n) return *this;
      const unsigned int npos = pos==~0U?_width:pos;
      for (unsigned int i = 0; i<n; ++i) insert(list,npos,is_shared);
      return *this;
    }

    //! Insert n copies of the list \c list at position \c pos of the current list \newinstance.
    template<typename t>
    CImgList<T> get_insert(const unsigned int n, const CImgList<t>& list, const unsigned int pos=~0U,
                           const bool is_shared=false) const {
      return (+*this).insert(n,list,pos,is_shared);
    }

    //! Remove all images between from indexes.
    /**
      \param pos1 Starting index of the removal.
      \param pos2 Ending index of the removal.
    **/
    CImgList<T>& remove(const unsigned int pos1, const unsigned int pos2) {
      const unsigned int
        npos1 = pos1<pos2?pos1:pos2,
        tpos2 = pos1<pos2?pos2:pos1,
        npos2 = tpos2<_width?tpos2:_width - 1;
      if (npos1>=_width)
        throw CImgArgumentException(_cimglist_instance
                                    "remove(): Invalid remove request at positions %u->%u.",
                                    cimglist_instance,
                                    npos1,tpos2);
      else {
        if (tpos2>=_width)
          throw CImgArgumentException(_cimglist_instance
                                      "remove(): Invalid remove request at positions %u->%u.",
                                      cimglist_instance,
                                      npos1,tpos2);

        for (unsigned int k = npos1; k<=npos2; ++k) _data[k].assign();
        const unsigned int nb = 1 + npos2 - npos1;
        if (!(_width-=nb)) return assign();
        if (_width>(_allocated_width>>4) || _allocated_width<=16) { // Removing items without reallocation
          if (npos1!=_width)
            std::memmove((void*)(_data + npos1),(void*)(_data + npos2 + 1),sizeof(CImg<T>)*(_width - npos1));
          std::memset((void*)(_data + _width),0,sizeof(CImg<T>)*nb);
        } else { // Removing items with reallocation
          _allocated_width>>=4;
          while (_allocated_width>16 && _width<(_allocated_width>>1)) _allocated_width>>=1;
          CImg<T> *const new_data = new CImg<T>[_allocated_width];
          if (npos1) std::memcpy((void*)new_data,(void*)_data,sizeof(CImg<T>)*npos1);
          if (npos1!=_width)
            std::memcpy((void*)(new_data + npos1),(void*)(_data + npos2 + 1),sizeof(CImg<T>)*(_width - npos1));
          if (_width!=_allocated_width)
            std::memset((void*)(new_data + _width),0,sizeof(CImg<T>)*(_allocated_width - _width));
          std::memset((void*)_data,0,sizeof(CImg<T>)*(_width + nb));
          delete[] _data;
          _data = new_data;
        }
      }
      return *this;
    }

    //! Remove all images between from indexes \newinstance.
    CImgList<T> get_remove(const unsigned int pos1, const unsigned int pos2) const {
      return (+*this).remove(pos1,pos2);
    }

    //! Remove image at index \c pos from the image list.
    /**
      \param pos Index of the image to remove.
    **/
    CImgList<T>& remove(const unsigned int pos) {
      return remove(pos,pos);
    }

    //! Remove image at index \c pos from the image list \newinstance.
    CImgList<T> get_remove(const unsigned int pos) const {
      return (+*this).remove(pos);
    }

    //! Remove last image.
    /**
    **/
    CImgList<T>& remove() {
      return remove(_width - 1);
    }

    //! Remove last image \newinstance.
    CImgList<T> get_remove() const {
      return (+*this).remove();
    }

    //! Reverse list order.
    CImgList<T>& reverse() {
      for (unsigned int l = 0; l<_width/2; ++l) (*this)[l].swap((*this)[_width - 1 - l]);
      return *this;
    }

    //! Reverse list order \newinstance.
    CImgList<T> get_reverse() const {
      return (+*this).reverse();
    }

    //! Return a sublist.
    /**
      \param pos0 Starting index of the sublist.
      \param pos1 Ending index of the sublist.
    **/
    CImgList<T>& images(const unsigned int pos0, const unsigned int pos1) {
      return get_images(pos0,pos1).move_to(*this);
    }

    //! Return a sublist \newinstance.
    CImgList<T> get_images(const unsigned int pos0, const unsigned int pos1) const {
      if (pos0>pos1 || pos1>=_width)
        throw CImgArgumentException(_cimglist_instance
                                    "images(): Specified sub-list indices (%u->%u) are out of bounds.",
                                    cimglist_instance,
                                    pos0,pos1);
      CImgList<T> res(pos1 - pos0 + 1);
      cimglist_for(res,l) res[l].assign(_data[pos0 + l]);
      return res;
    }

    //! Return a shared sublist.
    /**
      \param pos0 Starting index of the sublist.
      \param pos1 Ending index of the sublist.
    **/
    CImgList<T> get_shared_images(const unsigned int pos0, const unsigned int pos1) {
      if (pos0>pos1 || pos1>=_width)
        throw CImgArgumentException(_cimglist_instance
                                    "get_shared_images(): Specified sub-list indices (%u->%u) are out of bounds.",
                                    cimglist_instance,
                                    pos0,pos1);
      CImgList<T> res(pos1 - pos0 + 1);
      cimglist_for(res,l) res[l].assign(_data[pos0 + l],_data[pos0 + l]?true:false);
      return res;
    }

    //! Return a shared sublist \newinstance.
    const CImgList<T> get_shared_images(const unsigned int pos0, const unsigned int pos1) const {
      if (pos0>pos1 || pos1>=_width)
        throw CImgArgumentException(_cimglist_instance
                                    "get_shared_images(): Specified sub-list indices (%u->%u) are out of bounds.",
                                    cimglist_instance,
                                    pos0,pos1);
      CImgList<T> res(pos1 - pos0 + 1);
      cimglist_for(res,l) res[l].assign(_data[pos0 + l],_data[pos0 + l]?true:false);
      return res;
    }

    //! Return a single image which is the appending of all images of the current CImgList instance.
    /**
       \param axis Appending axis. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
       \param align Appending alignment.
    **/
    CImg<T> get_append(const char axis, const float align=0) const {
      if (is_empty()) return CImg<T>();
      if (_width==1) return +_data[0];
      if (_width==2) return _data[0].get_append(_data[1],axis,align);

      unsigned int dx = 0, dy = 0, dz = 0, dc = 0, pos = 0;
      CImg<T> res;
      switch (cimg::lowercase(axis)) {
      case 'x' : { // Along the X-axis
        cimglist_for(*this,l) {
          const CImg<T>& img = (*this)[l];
          if (img) {
            dx+=img._width;
            dy = std::max(dy,img._height);
            dz = std::max(dz,img._depth);
            dc = std::max(dc,img._spectrum);
          }
        }
        res.assign(dx,dy,dz,dc,(T)0);
        if (res) cimglist_for(*this,l) {
            const CImg<T>& img = (*this)[l];
            if (img) {
              if (img._height==1 && img._depth==1 && img._spectrum==1 &&
                  res._height==1 && res._depth==1 && res._spectrum==1)
                std::memcpy(&res[pos],img._data,sizeof(T)*img._width);
              else
                res.draw_image(pos,
                               (int)(align*(dy - img._height)),
                               (int)(align*(dz - img._depth)),
                               (int)(align*(dc - img._spectrum)),
                               img);
            }
            pos+=img._width;
          }
      } break;
      case 'y' : { // Along the Y-axis
        cimglist_for(*this,l) {
          const CImg<T>& img = (*this)[l];
          if (img) {
            dx = std::max(dx,img._width);
            dy+=img._height;
            dz = std::max(dz,img._depth);
            dc = std::max(dc,img._spectrum);
          }
        }
        res.assign(dx,dy,dz,dc,(T)0);
        if (res) cimglist_for(*this,l) {
            const CImg<T>& img = (*this)[l];
            if (img) {
              if (img._width==1 && img._depth==1 && img._spectrum==1 &&
                  res._width==1 && res._depth==1 && res._spectrum==1)
                std::memcpy(&res[pos],img._data,sizeof(T)*img._height);
              else
                res.draw_image((int)(align*(dx - img._width)),
                               pos,
                               (int)(align*(dz - img._depth)),
                               (int)(align*(dc - img._spectrum)),
                               img);
            }
            pos+=img._height;
          }
      } break;
      case 'z' : { // Along the Z-axis
        cimglist_for(*this,l) {
          const CImg<T>& img = (*this)[l];
          if (img) {
            dx = std::max(dx,img._width);
            dy = std::max(dy,img._height);
            dz+=img._depth;
            dc = std::max(dc,img._spectrum);
          }
        }
        res.assign(dx,dy,dz,dc,(T)0);
        if (res) cimglist_for(*this,l) {
            const CImg<T>& img = (*this)[l];
            if (img) {
              if (img._width==1 && img._height==1 && img._spectrum==1 &&
                  res._width==1 && res._height==1 && res._spectrum==1)
                std::memcpy(&res[pos],img._data,sizeof(T)*img._depth);
              else
                res.draw_image((int)(align*(dx - img._width)),
                               (int)(align*(dy - img._height)),
                               pos,
                               (int)(align*(dc - img._spectrum)),
                               img);
            }
            pos+=img._depth;
          }
      } break;
      default : { // Along the C-axis
        cimglist_for(*this,l) {
          const CImg<T>& img = (*this)[l];
          if (img) {
            dx = std::max(dx,img._width);
            dy = std::max(dy,img._height);
            dz = std::max(dz,img._depth);
            dc+=img._spectrum;
          }
        }
        res.assign(dx,dy,dz,dc,(T)0);
        if (res) cimglist_for(*this,l) {
            const CImg<T>& img = (*this)[l];
            if (img) {
              if (img._width==1 && img._height==1 && img._depth==1 &&
                  res._width==1 && res._height==1 && res._depth==1)
                std::memcpy(&res[pos],img._data,sizeof(T)*img._spectrum);
              else
                res.draw_image((int)(align*(dx - img._width)),
                               (int)(align*(dy - img._height)),
                               (int)(align*(dz - img._depth)),
                               pos,
                               img);
            }
            pos+=img._spectrum;
          }
      }
      }
      return res;
    }

    //! Return a list where each image has been split along the specified axis.
    /**
        \param axis Axis to split images along.
        \param nb Number of split parts for each image.
    **/
    CImgList<T>& split(const char axis, const int nb=-1) {
      return get_split(axis,nb).move_to(*this);
    }

    //! Return a list where each image has been split along the specified axis \newinstance.
    CImgList<T> get_split(const char axis, const int nb=-1, const unsigned int max_parts=~0U) const {
      CImgList<T> res;
      cimglist_for(*this,l) _data[l].get_split(axis,nb,max_parts).move_to(res,~0U);
      return res;
    }

    //! Insert image at the end of the list.
    /**
      \param img Image to insert.
    **/
    template<typename t>
    CImgList<T>& push_back(const CImg<t>& img) {
      return insert(img);
    }

    //! Insert image at the front of the list.
    /**
      \param img Image to insert.
    **/
    template<typename t>
    CImgList<T>& push_front(const CImg<t>& img) {
      return insert(img,0);
    }

    //! Insert list at the end of the current list.
    /**
      \param list List to insert.
    **/
    template<typename t>
    CImgList<T>& push_back(const CImgList<t>& list) {
      return insert(list);
    }

    //! Insert list at the front of the current list.
    /**
      \param list List to insert.
    **/
    template<typename t>
    CImgList<T>& push_front(const CImgList<t>& list) {
      return insert(list,0);
    }

    //! Remove last image.
    /**
    **/
    CImgList<T>& pop_back() {
      return remove(_width - 1);
    }

    //! Remove first image.
    /**
    **/
    CImgList<T>& pop_front() {
      return remove(0);
    }

    //! Remove image pointed by iterator.
    /**
      \param iter Iterator pointing to the image to remove.
    **/
    CImgList<T>& erase(const iterator iter) {
      return remove(iter - _data);
    }

    //@}
    //----------------------------------
    //
    //! \name Data Input
    //@{
    //----------------------------------

    //! Display a simple interactive interface to select images or sublists.
    /**
       \param disp Window instance to display selection and user interface.
       \param feature_type Can be \c false to select a single image, or \c true to select a sublist.
       \param axis Axis along whom images are appended for visualization.
       \param align Alignment setting when images have not all the same size.
       \param exit_on_anykey Exit function when any key is pressed.
       \return A one-column vector containing the selected image indexes.
    **/
    CImg<intT> get_select(CImgDisplay &disp, const bool feature_type=true,
                          const char axis='x', const float align=0,
                          const bool exit_on_anykey=false) const {
      return _select(disp,0,feature_type,axis,align,exit_on_anykey,0,false,false,false);
    }

    //! Display a simple interactive interface to select images or sublists.
    /**
       \param title Title of a new window used to display selection and user interface.
       \param feature_type Can be \c false to select a single image, or \c true to select a sublist.
       \param axis Axis along whom images are appended for visualization.
       \param align Alignment setting when images have not all the same size.
       \param exit_on_anykey Exit function when any key is pressed.
       \return A one-column vector containing the selected image indexes.
    **/
    CImg<intT> get_select(const char *const title, const bool feature_type=true,
                          const char axis='x', const float align=0,
                          const bool exit_on_anykey=false) const {
      CImgDisplay disp;
      return _select(disp,title,feature_type,axis,align,exit_on_anykey,0,false,false,false);
    }

    CImg<intT> _select(CImgDisplay &disp, const char *const title, const bool feature_type,
                       const char axis, const float align, const bool exit_on_anykey,
                       const unsigned int orig, const bool resize_disp,
                       const bool exit_on_rightbutton, const bool exit_on_wheel) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "select(): Empty instance.",
                                    cimglist_instance);

      // Create image correspondence table and get list dimensions for visualization.
      CImgList<uintT> _indices;
      unsigned int max_width = 0, max_height = 0, sum_width = 0, sum_height = 0;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        const unsigned int
          w = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,false),
          h = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,true);
        if (w>max_width) max_width = w;
        if (h>max_height) max_height = h;
        sum_width+=w; sum_height+=h;
        if (axis=='x') CImg<uintT>(w,1,1,1,(unsigned int)l).move_to(_indices);
        else CImg<uintT>(h,1,1,1,(unsigned int)l).move_to(_indices);
      }
      const CImg<uintT> indices0 = _indices>'x';

      // Create display window.
      if (!disp) {
        if (axis=='x') disp.assign(cimg_fitscreen(sum_width,max_height,1),title?title:0,1);
        else disp.assign(cimg_fitscreen(max_width,sum_height,1),title?title:0,1);
        if (!title) disp.set_title("CImgList<%s> (%u)",pixel_type(),_width);
      } else {
        if (title) disp.set_title("%s",title);
        disp.move_inside_screen();
      }
      if (resize_disp) {
        if (axis=='x') disp.resize(cimg_fitscreen(sum_width,max_height,1),false);
        else disp.resize(cimg_fitscreen(max_width,sum_height,1),false);
      }

      const unsigned int old_normalization = disp.normalization();
      bool old_is_resized = disp.is_resized();
      disp._normalization = 0;
      disp.show().set_key(0).show_mouse();
      static const unsigned char foreground_color[] = { 255,255,255 }, background_color[] = { 0,0,0 };

      // Enter event loop.
      CImg<ucharT> visu0, visu;
      CImg<uintT> indices;
      CImg<intT> positions(_width,4,1,1,-1);
      int oindex0 = -1, oindex1 = -1, index0 = -1, index1 = -1;
      bool is_clicked = false, is_selected = false, text_down = false, update_display = true;
      unsigned int key = 0, font_size = 32;

      while (!is_selected && !disp.is_closed() && !key) {

        // Create background image.
        if (!visu0) {
          visu0.assign(disp._width,disp._height,1,3,0); visu.assign();
          (indices0.get_resize(axis=='x'?visu0._width:visu0._height,1)).move_to(indices);
          unsigned int _ind = 0;
          const CImg<T> onexone(1,1,1,1,(T)0);
          if (axis=='x')
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(_width,4))
            cimglist_for(*this,ind) {
              unsigned int x0 = 0;
              while (x0<visu0._width && indices[x0++]!=(unsigned int)ind) {}
              unsigned int x1 = x0;
              while (x1<visu0._width && indices[x1++]==(unsigned int)ind) {}
              const CImg<T> &src = _data[ind]?_data[ind]:onexone;
              CImg<ucharT> res;
              src._get_select(disp,old_normalization,src._width/2,src._height/2,src._depth/2).
                move_to(res);
              const unsigned int h = CImgDisplay::_fitscreen(res._width,res._height,1,128,-85,true);
              res.resize(x1 - x0,std::max(32U,h*disp._height/max_height),1,res._spectrum==1?3:-100);
              positions(ind,0) = positions(ind,2) = (int)x0;
              positions(ind,1) = positions(ind,3) = (int)(align*(visu0.height() - res.height()));
              positions(ind,2)+=res._width;
              positions(ind,3)+=res._height - 1;
              visu0.draw_image(positions(ind,0),positions(ind,1),res);
            }
          else
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(_width,4))
            cimglist_for(*this,ind) {
              unsigned int y0 = 0;
              while (y0<visu0._height && indices[y0++]!=(unsigned int)ind) {}
              unsigned int y1 = y0;
              while (y1<visu0._height && indices[y1++]==(unsigned int)ind) {}
              const CImg<T> &src = _data[ind]?_data[ind]:onexone;
              CImg<ucharT> res;
              src._get_select(disp,old_normalization,(src._width - 1)/2,(src._height - 1)/2,(src._depth - 1)/2).
                move_to(res);
              const unsigned int w = CImgDisplay::_fitscreen(res._width,res._height,1,128,-85,false);
              res.resize(std::max(32U,w*disp._width/max_width),y1 - y0,1,res._spectrum==1?3:-100);
              positions(ind,0) = positions(ind,2) = (int)(align*(visu0.width() - res.width()));
              positions(ind,1) = positions(ind,3) = (int)y0;
              positions(ind,2)+=res._width - 1;
              positions(ind,3)+=res._height;
              visu0.draw_image(positions(ind,0),positions(ind,1),res);
            }
          if (axis=='x') --positions(_ind,2); else --positions(_ind,3);
          update_display = true;
        }

        if (!visu || oindex0!=index0 || oindex1!=index1) {
          if (index0>=0 && index1>=0) {
            visu.assign(visu0,false);
            const int indm = std::min(index0,index1), indM = std::max(index0,index1);
            for (int ind = indm; ind<=indM; ++ind) if (positions(ind,0)>=0) {
                visu.draw_rectangle(positions(ind,0),positions(ind,1),positions(ind,2),positions(ind,3),
                                    background_color,0.2f);
                if ((axis=='x' && positions(ind,2) - positions(ind,0)>=8) ||
                    (axis!='x' && positions(ind,3) - positions(ind,1)>=8))
                  visu.draw_rectangle(positions(ind,0),positions(ind,1),positions(ind,2),positions(ind,3),
                                      foreground_color,0.9f,0xAAAAAAAA);
              }
            if (is_clicked) visu.__draw_text(" Images #%u - #%u, Size = %u ",font_size,(int)text_down,
                                             orig + indm,orig + indM,indM - indm + 1);
            else visu.__draw_text(" Image #%u (%u,%u,%u,%u) ",font_size,(int)text_down,
                                  orig + index0,
                                  _data[index0]._width,
                                  _data[index0]._height,
                                  _data[index0]._depth,
                                  _data[index0]._spectrum);
            update_display = true;
          } else visu.assign();
        }
        if (!visu) { visu.assign(visu0,true); update_display = true; }
        if (update_display) { visu.display(disp); update_display = false; }
        disp.wait();

        // Manage user events.
        const int xm = disp.mouse_x(), ym = disp.mouse_y();
        int index = -1;

        if (xm>=0) {
          index = (int)indices(axis=='x'?xm:ym);
          if (disp.button()&1) {
            if (!is_clicked) { is_clicked = true; oindex0 = index0; index0 = index; }
            oindex1 = index1; index1 = index;
            if (!feature_type) is_selected = true;
          } else {
            if (!is_clicked) { oindex0 = oindex1 = index0; index0 = index1 = index; }
            else is_selected = true;
          }
        } else {
          if (is_clicked) {
            if (!(disp.button()&1)) { is_clicked = is_selected = false; index0 = index1 = -1; }
            else index1 = -1;
          } else index0 = index1 = -1;
        }

        if (disp.button()&4) { is_clicked = is_selected = false; index0 = index1 = -1; }
        if (disp.button()&2 && exit_on_rightbutton) { is_selected = true; index1 = index0 = -1; }
        if (disp.wheel() && exit_on_wheel) is_selected = true;

        CImg<charT> filename(32);
        switch (key = disp.key()) {
#if cimg_OS!=2
        case cimg::keyCTRLRIGHT :
#endif
        case 0 : case cimg::keyCTRLLEFT : key = 0; break;
        case cimg::keyD : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).
              resize(CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,false),
                     CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,true),false).
              _is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyC : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).
              resize(cimg_fitscreen(2*disp.width()/3,2*disp.height()/3,1),false)._is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyR : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).
              resize(cimg_fitscreen(axis=='x'?sum_width:max_width,axis=='x'?max_height:sum_height,1),false).
              _is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyF : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.resize(disp.screen_width(),disp.screen_height(),false).toggle_fullscreen()._is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyS : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            static unsigned int snap_number = 0;
            do {
              cimg_snprintf(filename,filename._width,cimg_appname "_%.6u."
#ifdef cimg_use_png
                            "png",
#else
                            "bmp",
#endif
                            snap_number++);
            } while (cimg::path_exists(filename));
            if (visu0) {
              (+visu0).__draw_text(" Saving snapshot... ",font_size,(int)text_down).display(disp);
              visu0.save(filename);
              (+visu0).__draw_text(" Snapshot '%s' saved. ",font_size,(int)text_down,filename._data).display(disp);
            }
            disp.set_key(key,false).wait(); key = 0;
          } break;
        case cimg::keyO :
          if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            static unsigned int snap_number = 0;
            do {
#ifdef cimg_use_zlib
              cimg_snprintf(filename,filename._width,cimg_appname "_%.6u.cimgz",snap_number++);
#else
              cimg_snprintf(filename,filename._width,cimg_appname "_%.6u.cimg",snap_number++);
#endif
            } while (cimg::path_exists(filename));
            (+visu0).__draw_text(" Saving instance... ",font_size,(int)text_down).display(disp);
            save(filename);
            (+visu0).__draw_text(" Instance '%s' saved. ",font_size,(int)text_down,filename._data).display(disp);
            disp.set_key(key,false).wait(); key = 0;
          } break;
        }
        if (disp.is_resized()) { disp.resize(false); visu0.assign(); }
        if (ym>=0 && ym<13) { if (!text_down) { visu.assign(); text_down = true; }}
        else if (ym>=visu.height() - 13) { if (text_down) { visu.assign(); text_down = false; }}
        if (!exit_on_anykey && key && key!=cimg::keyESC &&
            (key!=cimg::keyW || (!disp.is_keyCTRLLEFT() && !disp.is_keyCTRLRIGHT()))) {
          key = 0;
        }
      }
      CImg<intT> res(1,2,1,1,-1);
      if (is_selected) {
        if (feature_type) res.fill(std::min(index0,index1),std::max(index0,index1));
        else res.fill(index0);
      }
      if (!(disp.button()&2)) disp.set_button();
      disp._normalization = old_normalization;
      disp._is_resized = old_is_resized;
      disp.set_key(key);
      return res;
    }

    //! Load a list from a file.
    /**
     \param filename Filename to read data from.
    **/
    CImgList<T>& load(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "load(): Specified filename is (null).",
                                    cimglist_instance);

      if (!cimg::strncasecmp(filename,"http://",7) || !cimg::strncasecmp(filename,"https://",8)) {
        CImg<charT> filename_local(256);
        load(cimg::load_network(filename,filename_local));
        std::remove(filename_local);
        return *this;
      }

      const bool is_stdin = *filename=='-' && (!filename[1] || filename[1]=='.');
      const char *const ext = cimg::split_filename(filename);
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      bool is_loaded = true;
      try {
#ifdef cimglist_load_plugin
        cimglist_load_plugin(filename);
#endif
#ifdef cimglist_load_plugin1
        cimglist_load_plugin1(filename);
#endif
#ifdef cimglist_load_plugin2
        cimglist_load_plugin2(filename);
#endif
#ifdef cimglist_load_plugin3
        cimglist_load_plugin3(filename);
#endif
#ifdef cimglist_load_plugin4
        cimglist_load_plugin4(filename);
#endif
#ifdef cimglist_load_plugin5
        cimglist_load_plugin5(filename);
#endif
#ifdef cimglist_load_plugin6
        cimglist_load_plugin6(filename);
#endif
#ifdef cimglist_load_plugin7
        cimglist_load_plugin7(filename);
#endif
#ifdef cimglist_load_plugin8
        cimglist_load_plugin8(filename);
#endif
        if (!cimg::strcasecmp(ext,"tif") ||
            !cimg::strcasecmp(ext,"tiff")) load_tiff(filename);
        else if (!cimg::strcasecmp(ext,"gif")) load_gif_external(filename);
        else if (!cimg::strcasecmp(ext,"cimg") ||
                 !cimg::strcasecmp(ext,"cimgz") ||
                 !*ext) load_cimg(filename);
        else if (!cimg::strcasecmp(ext,"rec") ||
                 !cimg::strcasecmp(ext,"par")) load_parrec(filename);
        else if (!cimg::strcasecmp(ext,"avi") ||
                 !cimg::strcasecmp(ext,"mov") ||
                 !cimg::strcasecmp(ext,"asf") ||
                 !cimg::strcasecmp(ext,"divx") ||
                 !cimg::strcasecmp(ext,"flv") ||
                 !cimg::strcasecmp(ext,"mpg") ||
                 !cimg::strcasecmp(ext,"m1v") ||
                 !cimg::strcasecmp(ext,"m2v") ||
                 !cimg::strcasecmp(ext,"m4v") ||
                 !cimg::strcasecmp(ext,"mjp") ||
                 !cimg::strcasecmp(ext,"mp4") ||
                 !cimg::strcasecmp(ext,"mkv") ||
                 !cimg::strcasecmp(ext,"mpe") ||
                 !cimg::strcasecmp(ext,"movie") ||
                 !cimg::strcasecmp(ext,"ogm") ||
                 !cimg::strcasecmp(ext,"ogg") ||
                 !cimg::strcasecmp(ext,"ogv") ||
                 !cimg::strcasecmp(ext,"qt") ||
                 !cimg::strcasecmp(ext,"rm") ||
                 !cimg::strcasecmp(ext,"vob") ||
                 !cimg::strcasecmp(ext,"webm") ||
                 !cimg::strcasecmp(ext,"wmv") ||
                 !cimg::strcasecmp(ext,"xvid") ||
                 !cimg::strcasecmp(ext,"mpeg")) load_video(filename);
        else if (!cimg::strcasecmp(ext,"gz")) load_gzip_external(filename);
        else is_loaded = false;
      } catch (CImgIOException&) { is_loaded = false; }

      // If nothing loaded, try to guess file format from magic number in file.
      if (!is_loaded && !is_stdin) {
        std::FILE *const file = cimg::std_fopen(filename,"rb");
        if (!file) {
          cimg::exception_mode(omode);
          throw CImgIOException(_cimglist_instance
                                "load(): Failed to open file '%s'.",
                                cimglist_instance,
                                filename);
        }

        const char *const f_type = cimg::ftype(file,filename);
        cimg::fclose(file);
        is_loaded = true;
        try {
          if (!cimg::strcasecmp(f_type,"gif")) load_gif_external(filename);
          else if (!cimg::strcasecmp(f_type,"tif") &&
                   cimg::strcasecmp(ext,"nef") &&
                   cimg::strcasecmp(ext,"dng")) load_tiff(filename);
          else is_loaded = false;
        } catch (CImgIOException&) { is_loaded = false; }
      }

      // If nothing loaded, try to load file as a single image.
      if (!is_loaded) {
        assign(1);
        try {
          _data->load(filename);
        } catch (CImgIOException&) {
          cimg::exception_mode(omode);
          throw CImgIOException(_cimglist_instance
                                "load(): Failed to recognize format of file '%s'.",
                                cimglist_instance,
                                filename);
        }
      }
      cimg::exception_mode(omode);
      return *this;
    }

    //! Load a list from a file \newinstance.
    static CImgList<T> get_load(const char *const filename) {
      return CImgList<T>().load(filename);
    }

    //! Load a list from a .cimg file.
    /**
      \param filename Filename to read data from.
    **/
    CImgList<T>& load_cimg(const char *const filename) {
      return _load_cimg(0,filename);
    }

    //! Load a list from a .cimg file \newinstance.
    static CImgList<T> get_load_cimg(const char *const filename) {
      return CImgList<T>().load_cimg(filename);
    }

    //! Load a list from a .cimg file.
    /**
      \param file File to read data from.
    **/
    CImgList<T>& load_cimg(std::FILE *const file) {
      return _load_cimg(file,0);
    }

    //! Load a list from a .cimg file \newinstance.
    static CImgList<T> get_load_cimg(std::FILE *const file) {
      return CImgList<T>().load_cimg(file);
    }

    CImgList<T>& _load_cimg(std::FILE *const file, const char *const filename) {
#ifdef cimg_use_zlib
#define _cimgz_load_cimg_case(Tss) { \
   Bytef *const cbuf = new Bytef[csiz]; \
   cimg::fread(cbuf,(size_t)csiz,nfile); \
   if (is_bool) { \
     CImg<ucharT> raw(W*H*D*C/8); \
     uLongf destlen = (uLongf)raw.size(); \
     uncompress((Bytef*)raw._data,&destlen,cbuf,(uLong)csiz); \
     img.assign(W,H,D,C); \
     img._uchar2bool(raw,raw.size(),false); \
   } else { \
     CImg<Tss> raw(W,H,D,C); \
     uLongf destlen = (uLongf)(raw.size()*sizeof(Tss)); \
     uncompress((Bytef*)raw._data,&destlen,cbuf,(uLong)csiz); \
     if (endian!=cimg::endianness()) cimg::invert_endianness(raw._data,raw.size()); \
     raw.move_to(img); \
   } \
   delete[] cbuf; \
}
#else
#define _cimgz_load_cimg_case(Tss) \
   throw CImgIOException(_cimglist_instance \
                         "load_cimg(): Unable to load compressed data from file '%s' unless zlib is enabled.", \
                         cimglist_instance, \
                         filename?filename:"(FILE*)");
#endif

#define _cimg_load_cimg_case(Ts1,Ts2,Ts3,Tss) \
      if (!loaded && ((Ts1 && !cimg::strcasecmp(Ts1,str_pixeltype)) || \
                      (Ts2 && !cimg::strcasecmp(Ts2,str_pixeltype)) || \
                      (Ts3 && !cimg::strcasecmp(Ts3,str_pixeltype)))) { \
        const bool is_bool = cimg::type<Tss>::string()==cimg::type<bool>::string(); \
        for (int l = 0; l<N; ++l) { \
          j = 0; while ((i=std::fgetc(nfile))!='\n' && i>=0 && j<255) tmp[j++] = (char)i; tmp[j] = 0; \
          W = H = D = C = 0; csiz = 0; \
          if ((err = cimg_sscanf(tmp,"%d %d %d %d #" cimg_fuint64,&W,&H,&D,&C,&csiz))<4 || \
              W<0 || H<0 || D<0 || C<0) \
            throw CImgIOException(_cimglist_instance \
                                  "load_cimg(): Invalid specified size (%d,%d,%d,%d) of image %u in file '%s'.", \
                                  cimglist_instance, \
                                  W,H,D,C,l,filename?filename:("(FILE*)")); \
          if (W*H*D*C>0) { \
            CImg<T> &img = _data[l]; \
            if (err==5) _cimgz_load_cimg_case(Tss) \
            else { \
              img.assign(W,H,D,C); \
              T *ptrd = img._data; \
              if (is_bool) { \
                CImg<ucharT> raw; \
                for (ulongT to_read = img.size(); to_read; ) { \
                  raw.assign((unsigned int)std::min(to_read,cimg_iobuffer)); \
                  cimg::fread(raw._data,raw._width,nfile); \
                  CImg<T>(ptrd,std::min(8*raw._width,(unsigned int)(img.end() - ptrd)),1,1,1,true).\
                    _uchar2bool(raw,raw._width,false); \
                  to_read-=raw._width; \
                } \
              } else { \
                CImg<Tss> raw; \
                for (ulongT to_read = img.size(); to_read; ) { \
                  raw.assign((unsigned int)std::min(to_read,cimg_iobuffer)); \
                  cimg::fread(raw._data,raw._width,nfile); \
                  if (endian!=cimg::endianness()) cimg::invert_endianness(raw._data,raw.size()); \
                  const Tss *ptrs = raw._data; \
                  for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++); \
                  to_read-=raw._width; \
                } \
              } \
            } \
          } \
        } \
        loaded = true; \
      }

      if (!filename && !file)
        throw CImgArgumentException(_cimglist_instance
                                    "load_cimg(): Specified filename is (null).",
                                    cimglist_instance);

      const ulongT cimg_iobuffer = (ulongT)24*1024*1024;
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      bool loaded = false, endian = cimg::endianness();
      CImg<charT> tmp(256), str_pixeltype(256), str_endian(256);
      *tmp = *str_pixeltype = *str_endian = 0;
      cimg_uint64 csiz;
      int N = 0, W, H, D, C, i, j, err;
      do {
        j = 0; while ((i=std::fgetc(nfile))!='\n' && i>=0 && j<255) tmp[j++] = (char)i; tmp[j] = 0;
      } while (*tmp=='#' && i>=0);
      err = cimg_sscanf(tmp,"%d%*c%255[A-Za-z123468_]%*c%255[sA-Za-z_ ]",
                        &N,str_pixeltype._data,str_endian._data);
      if (err<2 || N<0) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "load_cimg(): File or CImg header not found in file '%s'.",
                              cimglist_instance,
                              filename?filename:"(FILE*)");
      }
      const cimg_int64 fsiz = cimg::fsize(nfile);
      if (N>fsiz/8) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "load_cimg(): Invalid number of images (%d) specified in file '%s'.",
                              cimglist_instance,
                              N,filename?filename:"(FILE*)");
      }
      if (!cimg::strncasecmp("little",str_endian,6)) endian = false;
      else if (!cimg::strncasecmp("big",str_endian,3)) endian = true;
      assign(N);
      _cimg_load_cimg_case("bool",0,0,cimg_uint8);
      _cimg_load_cimg_case("uint8","unsigned_char","uchar",cimg_uint8);
      _cimg_load_cimg_case("int8",0,0,cimg_int8);
      _cimg_load_cimg_case("char",0,0,char);
      _cimg_load_cimg_case("uint16","unsigned_short","ushort",cimg_uint16);
      _cimg_load_cimg_case("int16","short",0,cimg_int16);
      _cimg_load_cimg_case("uint32","unsigned_int","uint",cimg_uint32);
      _cimg_load_cimg_case("int32","int",0,cimg_int32);
      _cimg_load_cimg_case("unsigned_long","ulong",0,cimg_ulong);
      _cimg_load_cimg_case("long",0,0,cimg_long);
      _cimg_load_cimg_case("uint64","unsigned_int64",0,cimg_uint64);
      _cimg_load_cimg_case("int64",0,0,cimg_int64);
      _cimg_load_cimg_case("float32","float",0,cimg_float32);
      _cimg_load_cimg_case("float64","double",0,cimg_float64);

      if (!loaded) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "load_cimg(): Unsupported pixel type '%s' for file '%s'.",
                              cimglist_instance,
                              str_pixeltype._data,filename?filename:"(FILE*)");
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load a sublist list from a (non compressed) .cimg file.
    /**
      \param filename Filename to read data from.
      \param n0 Starting index of images to read (~0U for max).
      \param n1 Ending index of images to read (~0U for max).
      \param x0 Starting X-coordinates of image regions to read.
      \param y0 Starting Y-coordinates of image regions to read.
      \param z0 Starting Z-coordinates of image regions to read.
      \param c0 Starting C-coordinates of image regions to read.
      \param x1 Ending X-coordinates of image regions to read (~0U for max).
      \param y1 Ending Y-coordinates of image regions to read (~0U for max).
      \param z1 Ending Z-coordinates of image regions to read (~0U for max).
      \param c1 Ending C-coordinates of image regions to read (~0U for max).
    **/
    CImgList<T>& load_cimg(const char *const filename,
                           const unsigned int n0, const unsigned int n1,
                           const unsigned int x0, const unsigned int y0,
                           const unsigned int z0, const unsigned int c0,
                           const unsigned int x1, const unsigned int y1,
                           const unsigned int z1, const unsigned int c1) {
      return _load_cimg(0,filename,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1);
    }

    //! Load a sublist list from a (non compressed) .cimg file \newinstance.
    static CImgList<T> get_load_cimg(const char *const filename,
                                     const unsigned int n0, const unsigned int n1,
                                     const unsigned int x0, const unsigned int y0,
                                     const unsigned int z0, const unsigned int c0,
                                     const unsigned int x1, const unsigned int y1,
                                     const unsigned int z1, const unsigned int c1) {
      return CImgList<T>().load_cimg(filename,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1);
    }

    //! Load a sub-image list from a (non compressed) .cimg file \overloading.
    CImgList<T>& load_cimg(std::FILE *const file,
                           const unsigned int n0, const unsigned int n1,
                           const unsigned int x0, const unsigned int y0,
                           const unsigned int z0, const unsigned int c0,
                           const unsigned int x1, const unsigned int y1,
                           const unsigned int z1, const unsigned int c1) {
      return _load_cimg(file,0,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1);
    }

    //! Load a sub-image list from a (non compressed) .cimg file \newinstance.
    static CImgList<T> get_load_cimg(std::FILE *const file,
                                     const unsigned int n0, const unsigned int n1,
                                     const unsigned int x0, const unsigned int y0,
                                     const unsigned int z0, const unsigned int c0,
                                     const unsigned int x1, const unsigned int y1,
                                     const unsigned int z1, const unsigned int c1) {
      return CImgList<T>().load_cimg(file,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1);
    }

    CImgList<T>& _load_cimg(std::FILE *const file, const char *const filename,
                            const unsigned int n0, const unsigned int n1,
                            const unsigned int x0, const unsigned int y0,
                            const unsigned int z0, const unsigned int c0,
                            const unsigned int x1, const unsigned int y1,
                            const unsigned int z1, const unsigned int c1) {
#define _cimg_load_cimg_case2(Ts1,Ts2,Ts3,Tss) \
      if (!loaded && ((Ts1 && !cimg::strcasecmp(Ts1,str_pixeltype)) || \
                      (Ts2 && !cimg::strcasecmp(Ts2,str_pixeltype)) || \
                      (Ts3 && !cimg::strcasecmp(Ts3,str_pixeltype)))) { \
        for (unsigned int l = 0; l<=nn1; ++l) { \
          j = 0; while ((i=std::fgetc(nfile))!='\n' && i>=0) tmp[j++] = (char)i; tmp[j] = 0; \
          W = H = D = C = 0; \
          if (cimg_sscanf(tmp,"%d %d %d %d",&W,&H,&D,&C)!=4) \
            throw CImgIOException(_cimglist_instance \
                                  "load_cimg(): Invalid specified size (%d,%d,%d,%d) of image %u in file '%s'", \
                                  cimglist_instance, \
                                  W,H,D,C,l,filename?filename:"(FILE*)"); \
          if (W*H*D*C>0) { \
            if (l<nn0 || nx0>=(unsigned int)W || ny0>=(unsigned int)H || nz0>=(unsigned int)D || nc0>=(unsigned int)C) \
              cimg::fseek(nfile,W*H*D*C*sizeof(Tss),SEEK_CUR); \
            else { \
              const int \
                _nx1 = nx1==~0U?W - 1:(int)nx1, \
                _ny1 = ny1==~0U?H - 1:(int)ny1, \
                _nz1 = nz1==~0U?D - 1:(int)nz1, \
                _nc1 = nc1==~0U?C - 1:(int)nc1; \
              if (_nx1>=W || _ny1>=H || _nz1>=D || _nc1>=C) \
                throw CImgArgumentException(_cimglist_instance \
                                            "load_cimg(): Invalid specified coordinates " \
                                            "[%u](%u,%u,%u,%u) -> [%u](%u,%u,%u,%u) " \
                                            "because image [%u] in file '%s' has size (%u,%u,%u,%u).", \
                                            cimglist_instance, \
                                            n0,x0,y0,z0,c0,n1,x1,y1,z1,c1,l,filename?filename:"(FILE*)",W,H,D,C); \
              CImg<Tss> raw(1 + _nx1 - nx0); \
              CImg<T> &img = _data[l - nn0]; \
              img.assign(1 + _nx1 - nx0,1 + _ny1 - ny0,1 + _nz1 - nz0,1 + _nc1 - nc0); \
              T *ptrd = img._data; \
              ulongT skipvb = nc0*W*H*D*sizeof(Tss); \
              if (skipvb) cimg::fseek(nfile,skipvb,SEEK_CUR); \
              for (unsigned int c = 1 + _nc1 - nc0; c; --c) { \
                const ulongT skipzb = nz0*W*H*sizeof(Tss); \
                if (skipzb) cimg::fseek(nfile,skipzb,SEEK_CUR); \
                for (unsigned int z = 1 + _nz1 - nz0; z; --z) { \
                  const ulongT skipyb = ny0*W*sizeof(Tss); \
                  if (skipyb) cimg::fseek(nfile,skipyb,SEEK_CUR); \
                  for (unsigned int y = 1 + _ny1 - ny0; y; --y) { \
                    const ulongT skipxb = nx0*sizeof(Tss); \
                    if (skipxb) cimg::fseek(nfile,skipxb,SEEK_CUR); \
                    cimg::fread(raw._data,raw._width,nfile); \
                    if (endian!=cimg::endianness()) cimg::invert_endianness(raw._data,raw._width); \
                    const Tss *ptrs = raw._data; \
                    for (unsigned int off = raw._width; off; --off) *(ptrd++) = (T)*(ptrs++); \
                    const ulongT skipxe = (W - 1 - _nx1)*sizeof(Tss); \
                    if (skipxe) cimg::fseek(nfile,skipxe,SEEK_CUR); \
                  } \
                  const ulongT skipye = (H - 1 - _ny1)*W*sizeof(Tss); \
                  if (skipye) cimg::fseek(nfile,skipye,SEEK_CUR); \
                } \
                const ulongT skipze = (D - 1 - _nz1)*W*H*sizeof(Tss); \
                if (skipze) cimg::fseek(nfile,skipze,SEEK_CUR); \
              } \
              const ulongT skipve = (C - 1 - _nc1)*W*H*D*sizeof(Tss); \
              if (skipve) cimg::fseek(nfile,skipve,SEEK_CUR); \
            } \
          } \
        } \
        loaded = true; \
      }

      if (!filename && !file)
        throw CImgArgumentException(_cimglist_instance
                                    "load_cimg(): Specified filename is (null).",
                                    cimglist_instance);
      unsigned int
        nn0 = std::min(n0,n1), nn1 = std::max(n0,n1),
        nx0 = std::min(x0,x1), nx1 = std::max(x0,x1),
        ny0 = std::min(y0,y1), ny1 = std::max(y0,y1),
        nz0 = std::min(z0,z1), nz1 = std::max(z0,z1),
        nc0 = std::min(c0,c1), nc1 = std::max(c0,c1);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      bool loaded = false, endian = cimg::endianness();
      CImg<charT> tmp(256), str_pixeltype(256), str_endian(256);
      *tmp = *str_pixeltype = *str_endian = 0;
      int N = 0, W, H, D, C, i, j, err;
      j = 0; while ((i=std::fgetc(nfile))!='\n' && i!=EOF && j<256) tmp[j++] = (char)i; tmp[j] = 0;
      err = cimg_sscanf(tmp,"%d%*c%255[A-Za-z123468_]%*c%255[sA-Za-z_ ]",
                        &N,str_pixeltype._data,str_endian._data);
      if (err<2 || N<0) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "load_cimg(): CImg header not found in file '%s'.",
                              cimglist_instance,
                              filename?filename:"(FILE*)");
      }
      const cimg_int64 fsiz = cimg::fsize(nfile);
      if (N>fsiz/8) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "load_cimg(): Invalid number of images (%d) specified in file '%s'.",
                              cimglist_instance,
                              N,filename?filename:"(FILE*)");
      }
      if (!cimg::strncasecmp("little",str_endian,6)) endian = false;
      else if (!cimg::strncasecmp("big",str_endian,3)) endian = true;
      nn1 = n1==~0U?(unsigned int)N - 1:n1;
      if (nn1>=(unsigned int)N)
        throw CImgArgumentException(_cimglist_instance
                                    "load_cimg(): Invalid specified coordinates [%u](%u,%u,%u,%u) -> [%u](%u,%u,%u,%u) "
                                    "because file '%s' contains only %d images.",
                                    cimglist_instance,
                                    n0,x0,y0,z0,c0,n1,x1,y1,z1,c1,filename?filename:"(FILE*)",N);
      assign(1 + nn1 - n0);
      _cimg_load_cimg_case2("bool",0,0,cimg_uint8);
      _cimg_load_cimg_case2("uint8","unsigned char","uchar",cimg_uint8);
      _cimg_load_cimg_case2("int8",0,0,cimg_int8);
      _cimg_load_cimg_case2("char",0,0,char);
      _cimg_load_cimg_case2("uint16","unsigned_short","ushort",cimg_uint16);
      _cimg_load_cimg_case2("int16","short",0,cimg_int16);
      _cimg_load_cimg_case2("uint32","unsigned_int","uint",cimg_uint32);
      _cimg_load_cimg_case2("int32","int",0,cimg_int32);
      _cimg_load_cimg_case2("unsigned_long","ulong",0,cimg_ulong);
      _cimg_load_cimg_case2("long",0,0,cimg_long);
      _cimg_load_cimg_case2("uint64","unsigned_int64",0,cimg_uint64);
      _cimg_load_cimg_case2("int64",0,0,cimg_int64);
      _cimg_load_cimg_case2("float32","float",0,cimg_float32);
      _cimg_load_cimg_case2("float64","double",0,cimg_float64);
      if (!loaded) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "load_cimg(): Unsupported pixel type '%s' for file '%s'.",
                              cimglist_instance,
                              str_pixeltype._data,filename?filename:"(FILE*)");
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load a list from a PAR/REC (Philips) file.
    /**
      \param filename Filename to read data from.
    **/
    CImgList<T>& load_parrec(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "load_parrec(): Specified filename is (null).",
                                    cimglist_instance);

      CImg<charT> body(1024), filenamepar(1024), filenamerec(1024);
      *body = *filenamepar = *filenamerec = 0;
      const char *const ext = cimg::split_filename(filename,body);
      if (!std::strcmp(ext,"par")) {
        std::strncpy(filenamepar,filename,filenamepar._width - 1);
        cimg_snprintf(filenamerec,filenamerec._width,"%s.rec",body._data);
      }
      if (!std::strcmp(ext,"PAR")) {
        std::strncpy(filenamepar,filename,filenamepar._width - 1);
        cimg_snprintf(filenamerec,filenamerec._width,"%s.REC",body._data);
      }
      if (!std::strcmp(ext,"rec")) {
        std::strncpy(filenamerec,filename,filenamerec._width - 1);
        cimg_snprintf(filenamepar,filenamepar._width,"%s.par",body._data);
      }
      if (!std::strcmp(ext,"REC")) {
        std::strncpy(filenamerec,filename,filenamerec._width - 1);
        cimg_snprintf(filenamepar,filenamepar._width,"%s.PAR",body._data);
      }
      std::FILE *file = cimg::fopen(filenamepar,"r");

      // Parse header file.
      CImgList<floatT> st_slices;
      CImgList<uintT> st_global;
      CImg<charT> line(256); *line = 0;
      int err;
      do { err = std::fscanf(file,"%255[^\n]%*c",line._data); } while (err!=EOF && (*line=='#' || *line=='.'));
      do {
        unsigned int sn,size_x,size_y,pixsize;
        float rs,ri,ss;
        err = std::fscanf(file,"%u%*u%*u%*u%*u%*u%*u%u%*u%u%u%g%g%g%*[^\n]",&sn,&pixsize,&size_x,&size_y,&ri,&rs,&ss);
        if (err==7) {
          CImg<floatT>::vector((float)sn,(float)pixsize,(float)size_x,(float)size_y,ri,rs,ss,0).move_to(st_slices);
          unsigned int i; for (i = 0; i<st_global._width && sn<=st_global[i][2]; ++i) {}
          if (i==st_global._width) CImg<uintT>::vector(size_x,size_y,sn).move_to(st_global);
          else {
            CImg<uintT> &vec = st_global[i];
            if (size_x>vec[0]) vec[0] = size_x;
            if (size_y>vec[1]) vec[1] = size_y;
            vec[2] = sn;
          }
          st_slices[st_slices._width - 1][7] = (float)i;
        }
      } while (err==7);

      // Read data.
      std::FILE *file2 = cimg::fopen(filenamerec,"rb");
      cimglist_for(st_global,l) {
        const CImg<uintT>& vec = st_global[l];
        CImg<T>(vec[0],vec[1],vec[2]).move_to(*this);
      }

      cimglist_for(st_slices,l) {
        const CImg<floatT>& vec = st_slices[l];
        const unsigned int
          sn = (unsigned int)vec[0] - 1,
          pixsize = (unsigned int)vec[1],
          size_x = (unsigned int)vec[2],
          size_y = (unsigned int)vec[3],
          imn = (unsigned int)vec[7];
        const float ri = vec[4], rs = vec[5], ss = vec[6];
        switch (pixsize) {
        case 8 : {
          CImg<ucharT> buf(size_x,size_y);
          cimg::fread(buf._data,size_x*size_y,file2);
          if (cimg::endianness()) cimg::invert_endianness(buf._data,size_x*size_y);
          CImg<T>& img = (*this)[imn];
          cimg_forXY(img,x,y) img(x,y,sn) = (T)(( buf(x,y)*rs + ri )/(rs*ss));
        } break;
        case 16 : {
          CImg<ushortT> buf(size_x,size_y);
          cimg::fread(buf._data,size_x*size_y,file2);
          if (cimg::endianness()) cimg::invert_endianness(buf._data,size_x*size_y);
          CImg<T>& img = (*this)[imn];
          cimg_forXY(img,x,y) img(x,y,sn) = (T)(( buf(x,y)*rs + ri )/(rs*ss));
        } break;
        case 32 : {
          CImg<uintT> buf(size_x,size_y);
          cimg::fread(buf._data,size_x*size_y,file2);
          if (cimg::endianness()) cimg::invert_endianness(buf._data,size_x*size_y);
          CImg<T>& img = (*this)[imn];
          cimg_forXY(img,x,y) img(x,y,sn) = (T)(( buf(x,y)*rs + ri )/(rs*ss));
        } break;
        default :
          cimg::fclose(file);
          cimg::fclose(file2);
          throw CImgIOException(_cimglist_instance
                                "load_parrec(): Unsupported %d-bits pixel type for file '%s'.",
                                cimglist_instance,
                                pixsize,filename);
        }
      }
      cimg::fclose(file);
      cimg::fclose(file2);
      if (!_width)
        throw CImgIOException(_cimglist_instance
                              "load_parrec(): Failed to recognize valid PAR-REC data in file '%s'.",
                              cimglist_instance,
                              filename);
      return *this;
    }

    //! Load a list from a PAR/REC (Philips) file \newinstance.
    static CImgList<T> get_load_parrec(const char *const filename) {
      return CImgList<T>().load_parrec(filename);
    }

    //! Load a list from a YUV image sequence file.
    /**
        \param filename Filename to read data from.
        \param size_x Width of the images.
        \param size_y Height of the images.
        \param chroma_subsampling Type of chroma subsampling. Can be <tt>{ 420 | 422 | 444 }</tt>.
        \param first_frame Index of first image frame to read.
        \param last_frame Index of last image frame to read.
        \param step_frame Step applied between each frame.
        \param yuv2rgb Apply YUV to RGB transformation during reading.
    **/
    CImgList<T>& load_yuv(const char *const filename,
                          const unsigned int size_x, const unsigned int size_y,
                          const unsigned int chroma_subsampling=444,
                          const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                          const unsigned int step_frame=1, const bool yuv2rgb=true) {
      return _load_yuv(0,filename,size_x,size_y,chroma_subsampling,
                       first_frame,last_frame,step_frame,yuv2rgb);
    }

    //! Load a list from a YUV image sequence file \newinstance.
    static CImgList<T> get_load_yuv(const char *const filename,
                                    const unsigned int size_x, const unsigned int size_y=1,
                                    const unsigned int chroma_subsampling=444,
                                    const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                                    const unsigned int step_frame=1, const bool yuv2rgb=true) {
      return CImgList<T>().load_yuv(filename,size_x,size_y,chroma_subsampling,
                                    first_frame,last_frame,step_frame,yuv2rgb);
    }

    //! Load a list from an image sequence YUV file \overloading.
    CImgList<T>& load_yuv(std::FILE *const file,
                          const unsigned int size_x, const unsigned int size_y,
                          const unsigned int chroma_subsampling=444,
                          const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                          const unsigned int step_frame=1, const bool yuv2rgb=true) {
      return _load_yuv(file,0,size_x,size_y,chroma_subsampling,
                       first_frame,last_frame,step_frame,yuv2rgb);
    }

    //! Load a list from an image sequence YUV file \newinstance.
    static CImgList<T> get_load_yuv(std::FILE *const file,
                                    const unsigned int size_x, const unsigned int size_y=1,
                                    const unsigned int chroma_subsampling=444,
                                    const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                                    const unsigned int step_frame=1, const bool yuv2rgb=true) {
      return CImgList<T>().load_yuv(file,size_x,size_y,chroma_subsampling,
                                    first_frame,last_frame,step_frame,yuv2rgb);
    }

    CImgList<T>& _load_yuv(std::FILE *const file, const char *const filename,
                           const unsigned int size_x, const unsigned int size_y,
                           const unsigned int chroma_subsampling,
                           const unsigned int first_frame, const unsigned int last_frame,
                           const unsigned int step_frame, const bool yuv2rgb) {
      if (!filename && !file)
        throw CImgArgumentException(_cimglist_instance
                                    "load_yuv(): Specified filename is (null).",
                                    cimglist_instance);
      if (chroma_subsampling!=420 && chroma_subsampling!=422 && chroma_subsampling!=444)
        throw CImgArgumentException(_cimglist_instance
                                    "load_yuv(): Specified chroma subsampling %u is invalid, for file '%s'.",
                                    cimglist_instance,
                                    chroma_subsampling,filename?filename:"(FILE*)");
      const unsigned int
        cfx = chroma_subsampling==420 || chroma_subsampling==422?2:1,
        cfy = chroma_subsampling==420?2:1,
        nfirst_frame = first_frame<last_frame?first_frame:last_frame,
        nlast_frame = first_frame<last_frame?last_frame:first_frame,
        nstep_frame = step_frame?step_frame:1;

      if (!size_x || !size_y || size_x%cfx || size_y%cfy)
        throw CImgArgumentException(_cimglist_instance
                                    "load_yuv(): Specified dimensions (%u,%u) are invalid, for file '%s'.",
                                    cimglist_instance,
                                    size_x,size_y,filename?filename:"(FILE*)");

      CImg<ucharT> YUV(size_x,size_y,1,3), UV(size_x/cfx,size_y/cfy,1,2);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      bool stop_flag = false;
      int err;
      if (nfirst_frame) {
        err = cimg::fseek(nfile,(uint64T)nfirst_frame*(YUV._width*YUV._height + 2*UV._width*UV._height),SEEK_CUR);
        if (err) {
          if (!file) cimg::fclose(nfile);
          throw CImgIOException(_cimglist_instance
                                "load_yuv(): File '%s' doesn't contain frame number %u.",
                                cimglist_instance,
                                filename?filename:"(FILE*)",nfirst_frame);
        }
      }
      unsigned int frame;
      for (frame = nfirst_frame; !stop_flag && frame<=nlast_frame; frame+=nstep_frame) {
        YUV.get_shared_channel(0).fill(0);
        // *TRY* to read the luminance part, do not replace by cimg::fread!
        err = (int)std::fread((void*)(YUV._data),1,(size_t)YUV._width*YUV._height,nfile);
        if (err!=(int)(YUV._width*YUV._height)) {
          stop_flag = true;
          if (err>0)
            cimg::warn(_cimglist_instance
                       "load_yuv(): File '%s' contains incomplete data or given image dimensions "
                       "(%u,%u) are incorrect.",
                       cimglist_instance,
                       filename?filename:"(FILE*)",size_x,size_y);
        } else {
          UV.fill(0);
          // *TRY* to read the luminance part, do not replace by cimg::fread!
          err = (int)std::fread((void*)(UV._data),1,(size_t)UV.size(),nfile);
          if (err!=(int)(UV.size())) {
            stop_flag = true;
            if (err>0)
              cimg::warn(_cimglist_instance
                         "load_yuv(): File '%s' contains incomplete data or given image dimensions "
                         "(%u,%u) are incorrect.",
                         cimglist_instance,
                         filename?filename:"(FILE*)",size_x,size_y);
          } else {
            const ucharT *ptrs1 = UV._data, *ptrs2 = UV.data(0,0,0,1);
            ucharT *ptrd1 = YUV.data(0,0,0,1), *ptrd2 = YUV.data(0,0,0,2);
            const unsigned int wd = YUV._width;
            switch (chroma_subsampling) {
            case 420 :
              cimg_forY(UV,y) {
                cimg_forX(UV,x) {
                  const ucharT U = *(ptrs1++), V = *(ptrs2++);
                  ptrd1[wd] = U; *(ptrd1)++ = U;
                  ptrd1[wd] = U; *(ptrd1)++ = U;
                  ptrd2[wd] = V; *(ptrd2)++ = V;
                  ptrd2[wd] = V; *(ptrd2)++ = V;
                }
                ptrd1+=wd; ptrd2+=wd;
              }
              break;
            case 422 :
              cimg_forXY(UV,x,y) {
                const ucharT U = *(ptrs1++), V = *(ptrs2++);
                *(ptrd1++) = U; *(ptrd1++) = U;
                *(ptrd2++) = V; *(ptrd2++) = V;
              }
              break;
            default :
              YUV.draw_image(0,0,0,1,UV);
            }
            if (yuv2rgb) YUV.YCbCrtoRGB();
            insert(YUV);
            if (nstep_frame>1) cimg::fseek(nfile,(uint64T)(nstep_frame - 1)*(size_x*size_y + size_x*size_y/2),SEEK_CUR);
          }
        }
      }
      if (is_empty())
        throw CImgIOException(_cimglist_instance
                              "load_yuv() : Missing data in file '%s'.",
                              cimglist_instance,
                              filename?filename:"(FILE*)");
      if (stop_flag && nlast_frame!=~0U && frame!=nlast_frame)
        cimg::warn(_cimglist_instance
                   "load_yuv(): Frame %d not reached since only %u frames were found in file '%s'.",
                   cimglist_instance,
                   nlast_frame,frame - 1,filename?filename:"(FILE*)");

      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load an image from a video file, using OpenCV library.
    /**
      \param filename Filename, as a C-string.
      \param first_frame Index of the first frame to read.
      \param last_frame Index of the last frame to read (can be higher than the actual number of frames, e.g. '~0U').
      \param step_frame Step value for frame reading.
      \note If step_frame==0, the current video stream is forced to be released (without any frames read).
    **/
    CImgList<T>& load_video(const char *const filename,
                            const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                            const unsigned int step_frame=1) {
#ifndef cimg_use_opencv
      if (first_frame || last_frame!=~0U || step_frame>1)
        throw CImgArgumentException(_cimglist_instance
                                    "load_video() : File '%s', arguments 'first_frame', 'last_frame' "
                                    "and 'step_frame' requires features from the OpenCV library "
                                    "('-Dcimg_use_opencv' must be defined).",
                                    cimglist_instance,filename);
      return load_ffmpeg_external(filename);
#else
      static cv::VideoCapture *captures[32] = {};
      static CImgList<charT> filenames(32);
      static CImg<uintT> positions(32,1,1,1,0);
      static int last_used_index = -1;

      // Detect if a video capture already exists for the specified filename.
      cimg::mutex(9);
      int index = -1;
      if (filename) {
        if (last_used_index>=0 && !std::strcmp(filename,filenames[last_used_index])) {
          index = last_used_index;
        } else cimglist_for(filenames,l) if (filenames[l] && !std::strcmp(filename,filenames[l])) {
            index = l; break;
          }
      } else index = last_used_index;
      cimg::mutex(9,0);

      // Release stream if needed.
      if (!step_frame || (index>=0 && positions[index]>first_frame)) {
        if (index>=0) {
          cimg::mutex(9);
          captures[index]->release();
          delete captures[index];
          captures[index] = 0;
          positions[index] = 0;
          filenames[index].assign();
          if (last_used_index==index) last_used_index = -1;
          index = -1;
          cimg::mutex(9,0);
        } else
          if (filename)
            cimg::warn(_cimglist_instance
                       "load_video() : File '%s', no opened video stream associated with filename found.",
                       cimglist_instance,filename);
          else
            cimg::warn(_cimglist_instance
                       "load_video() : No opened video stream found.",
                       cimglist_instance,filename);
        if (!step_frame) return *this;
      }

      // Find empty slot for capturing video stream.
      if (index<0) {
        if (!filename)
          throw CImgArgumentException(_cimglist_instance
                                      "load_video(): No already open video reader found. You must specify a "
                                      "non-(null) filename argument for the first call.",
                                      cimglist_instance);
        else { cimg::mutex(9); cimglist_for(filenames,l) if (!filenames[l]) { index = l; break; } cimg::mutex(9,0); }
        if (index<0)
          throw CImgIOException(_cimglist_instance
                                "load_video(): File '%s', no video reader slots available. "
                                "You have to release some of your previously opened videos.",
                                cimglist_instance,filename);
        cimg::mutex(9);
        captures[index] = new cv::VideoCapture(filename);
        positions[index] = 0;
        if (!captures[index]->isOpened()) {
          delete captures[index];
          captures[index] = 0;
          cimg::mutex(9,0);
          cimg::fclose(cimg::fopen(filename,"rb")); // Check file availability
          throw CImgIOException(_cimglist_instance
                                "load_video(): File '%s', unable to detect format of video file.",
                                cimglist_instance,filename);
        }
        CImg<charT>::string(filename).move_to(filenames[index]);
        cimg::mutex(9,0);
      }

      cimg::mutex(9);
      const unsigned int nb_frames = (unsigned int)std::max(0.,captures[index]->get(_cimg_cap_prop_frame_count));
      cimg::mutex(9,0);
      assign();

      // Skip frames if requested.
      bool go_on = true;
      unsigned int &pos = positions[index];
      while (pos<first_frame) {
        cimg::mutex(9);
        if (!captures[index]->grab()) { cimg::mutex(9,0); go_on = false; break; }
        cimg::mutex(9,0);
        ++pos;
      }

      // Read and convert frames.
      const unsigned int _last_frame = std::min(nb_frames?nb_frames - 1:~0U,last_frame);
      while (go_on && pos<=_last_frame) {
        cv::Mat cvimg;
        cimg::mutex(9);
        if (captures[index]->read(cvimg)) { CImg<T>::_cvmat2cimg(cvimg).move_to(*this); ++pos; }
        else go_on = false;
        cimg::mutex(9,0);
        if (go_on)
          for (unsigned int i = 1; go_on && i<step_frame && pos<=_last_frame; ++i, ++pos) {
            cimg::mutex(9);
            if (!captures[index]->grab()) go_on = false;
            cimg::mutex(9,0);
          }
      }

      if (!go_on || (nb_frames && pos>=nb_frames)) { // Close video stream when necessary
        cimg::mutex(9);
        captures[index]->release();
        delete captures[index];
        captures[index] = 0;
        filenames[index].assign();
        positions[index] = 0;
        index = -1;
        cimg::mutex(9,0);
      }

      cimg::mutex(9);
      last_used_index = index;
      cimg::mutex(9,0);

      if (is_empty())
        throw CImgIOException(_cimglist_instance
                              "load_video(): File '%s', unable to locate frame %u.",
                              cimglist_instance,filename,first_frame);
      return *this;
#endif
    }

    //! Load an image from a video file, using OpenCV library \newinstance.
    static CImgList<T> get_load_video(const char *const filename,
                           const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                           const unsigned int step_frame=1) {
      return CImgList<T>().load_video(filename,first_frame,last_frame,step_frame);
    }

    //! Load an image from a video file using the external tool 'ffmpeg'.
    /**
      \param filename Filename to read data from.
    **/
    CImgList<T>& load_ffmpeg_external(const char *const filename) {
      if (!filename || !cimg::is_file(filename))
        throw CImgArgumentException(_cimglist_instance
                                    "load_ffmpeg_external(): Specified filename is (null) or does not exist.",
                                    cimglist_instance);
      CImg<charT> command(1024), filename_tmp(256), filename_tmp2(256);
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_000001.ppm",filename_tmp._data);
      } while (cimg::path_exists(filename_tmp2));
      cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_%%6d.ppm",filename_tmp._data);
      cimg_snprintf(command,command._width,"\"%s\" -v -8 -i \"%s\" \"%s\"",
                    cimg::ffmpeg_path(),
                    CImg<charT>::string(filename)._system_strescape().data(),
                    CImg<charT>::string(filename_tmp2)._system_strescape().data());
      if (cimg::system(command,cimg::ffmpeg_path())!=0)
        throw CImgIOException(_cimglist_instance
                              "load_ffmpeg_external(): Failed to open file '%s' with external command 'ffmpeg'.",
                              cimglist_instance,
                              filename);
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      assign();
      unsigned int i = 1;
      for (bool stop_flag = false; !stop_flag; ++i) {
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_%.6u.ppm",filename_tmp._data,i);
        CImg<T> img;
        try { img.load_pnm(filename_tmp2); }
        catch (CImgException&) { stop_flag = true; }
        if (img) { img.move_to(*this); std::remove(filename_tmp2); }
      }
      cimg::exception_mode(omode);
      if (is_empty())
        throw CImgIOException(_cimglist_instance
                              "load_ffmpeg_external(): Failed to open file '%s' with external command 'ffmpeg'.",
                              cimglist_instance,
                              filename);
      return *this;
    }

    //! Load an image from a video file using the external tool 'ffmpeg' \newinstance.
    static CImgList<T> get_load_ffmpeg_external(const char *const filename) {
      return CImgList<T>().load_ffmpeg_external(filename);
    }

    //! Load gif file, using ImageMagick or GraphicsMagick's external tools.
    /**
      \param filename Filename to read data from.
    **/
    CImgList<T>& load_gif_external(const char *const filename) {
      if (!filename || !cimg::is_file(filename))
        throw CImgArgumentException(_cimglist_instance
                                    "load_gif_external(): Specified filename is (null) or does not exist.",
                                    cimglist_instance);
      if (!_load_gif_external(filename,false))
        if (!_load_gif_external(filename,true))
          try { assign(CImg<T>().load_other(filename)); } catch (CImgException&) { assign(); }
      if (is_empty())
        throw CImgIOException(_cimglist_instance
                              "load_gif_external(): Failed to open file '%s'.",
                              cimglist_instance,filename);
      return *this;
    }

    CImgList<T>& _load_gif_external(const char *const filename, const bool use_graphicsmagick=false) {
      CImg<charT> command(1024), filename_tmp(256), filename_tmp2(256);
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        if (use_graphicsmagick) cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s.png.0",filename_tmp._data);
        else cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s-0.png",filename_tmp._data);
      } while (cimg::path_exists(filename_tmp2));
      if (use_graphicsmagick) cimg_snprintf(command,command._width,"%s convert \"%s\" \"%s.png\"",
                                            cimg::graphicsmagick_path(),
                                            CImg<charT>::string(filename)._system_strescape().data(),
                                            CImg<charT>::string(filename_tmp)._system_strescape().data());
      else cimg_snprintf(command,command._width,"\"%s\" -coalesce \"%s\" \"%s.png\"",
                         cimg::imagemagick_path(),
                         CImg<charT>::string(filename)._system_strescape().data(),
                         CImg<charT>::string(filename_tmp)._system_strescape().data());
      cimg::system(command,cimg::imagemagick_path());
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      assign();

      // Try to read a single frame gif.
      cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s.png",filename_tmp._data);
      CImg<T> img;
      try { img.load_png(filename_tmp2); }
      catch (CImgException&) { }
      if (img) { img.move_to(*this); std::remove(filename_tmp2); }
      else { // Try to read animated gif
        unsigned int i = 0;
        for (bool stop_flag = false; !stop_flag; ++i) {
          if (use_graphicsmagick) cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s.png.%u",filename_tmp._data,i);
          else cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s-%u.png",filename_tmp._data,i);
          try { img.load_png(filename_tmp2); }
          catch (CImgException&) { stop_flag = true; }
          if (img) { img.move_to(*this); std::remove(filename_tmp2); }
        }
      }
      cimg::exception_mode(omode);
      return *this;
    }

    //! Load gif file, using ImageMagick or GraphicsMagick's external tools \newinstance.
    static CImgList<T> get_load_gif_external(const char *const filename) {
      return CImgList<T>().load_gif_external(filename);
    }

    //! Load a gzipped list, using external tool 'gunzip'.
    /**
      \param filename Filename to read data from.
    **/
    CImgList<T>& load_gzip_external(const char *const filename) {
      if (!filename || !cimg::is_file(filename))
        throw CImgIOException(_cimglist_instance
                              "load_gzip_external(): Specified filename is (null) or does not exist.",
                              cimglist_instance);
      CImg<charT> command(1024), filename_tmp(256), body(256);
      const char
        *ext = cimg::split_filename(filename,body),
        *ext2 = cimg::split_filename(body,0);
      do {
        if (!cimg::strcasecmp(ext,"gz")) {
          if (*ext2) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                   cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext2);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        } else {
          if (*ext) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                  cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        }
      } while (cimg::path_exists(filename_tmp));
      cimg_snprintf(command,command._width,"\"%s\" -c \"%s\" > \"%s\"",
                    cimg::gunzip_path(),
                    CImg<charT>::string(filename)._system_strescape().data(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data());
      if (cimg::system(command,cimg::gunzip_path())!=0)
        throw CImgIOException(_cimglist_instance
                              "load_gzip_external(): Failed to open file '%s'.",
                              cimglist_instance,
                              filename);
      if (!cimg::path_exists(filename_tmp)) {
        cimg::fclose(cimg::fopen(filename,"r"));
        throw CImgIOException(_cimglist_instance
                              "load_gzip_external(): Failed to open file '%s'.",
                              cimglist_instance,
                              filename);

      }
      load(filename_tmp);
      std::remove(filename_tmp);
      return *this;
    }

    //! Load a gzipped list, using external tool 'gunzip' \newinstance.
    static CImgList<T> get_load_gzip_external(const char *const filename) {
      return CImgList<T>().load_gzip_external(filename);
    }

    //! Load images from a TIFF file.
    /**
        \param filename Filename to read data from.
        \param first_frame Index of first image frame to read.
        \param last_frame Index of last image frame to read.
        \param step_frame Step applied between each frame.
        \param[out] bits_per_value Number of bits used to store a scalar value in the image file.
        \param[out] voxel_size Voxel size, as stored in the filename.
        \param[out] description Description, as stored in the filename.
    **/
    CImgList<T>& load_tiff(const char *const filename,
                           const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                           const unsigned int step_frame=1, unsigned int *const bits_per_value=0,
                           float *const voxel_size=0, CImg<charT> *const description=0) {
      const unsigned int
        nfirst_frame = first_frame<last_frame?first_frame:last_frame,
        nstep_frame = step_frame?step_frame:1;
      unsigned int nlast_frame = first_frame<last_frame?last_frame:first_frame;
#ifndef cimg_use_tiff
      cimg::unused(bits_per_value,voxel_size,description);
      if (nfirst_frame || nlast_frame!=~0U || nstep_frame!=1)
        throw CImgArgumentException(_cimglist_instance
                                    "load_tiff(): Unable to load sub-images from file '%s' unless libtiff is enabled.",
                                    cimglist_instance,
                                    filename);

      return assign(CImg<T>::get_load_tiff(filename));
#else
#if cimg_verbosity<3
        TIFFSetWarningHandler(0);
        TIFFSetErrorHandler(0);
#endif
      TIFF *tif = TIFFOpen(filename,"r");
      if (tif) {
        unsigned int nb_images = 0;
        do ++nb_images; while (TIFFReadDirectory(tif));
        if (nfirst_frame>=nb_images || (nlast_frame!=~0U && nlast_frame>=nb_images))
          cimg::warn(_cimglist_instance
                     "load_tiff(): Invalid specified frame range is [%u,%u] (step %u) since "
                     "file '%s' contains %u image(s).",
                     cimglist_instance,
                     nfirst_frame,nlast_frame,nstep_frame,filename,nb_images);

        if (nfirst_frame>=nb_images) return assign();
        if (nlast_frame>=nb_images) nlast_frame = nb_images - 1;
        assign(1 + (nlast_frame - nfirst_frame)/nstep_frame);
        TIFFSetDirectory(tif,0);
        cimglist_for(*this,l)
          _data[l]._load_tiff(tif,nfirst_frame + l*nstep_frame,bits_per_value,voxel_size,description);
        TIFFClose(tif);
      } else throw CImgIOException(_cimglist_instance
                                   "load_tiff(): Failed to open file '%s'.",
                                   cimglist_instance,
                                   filename);
      return *this;
#endif
    }

    //! Load a multi-page TIFF file \newinstance.
    static CImgList<T> get_load_tiff(const char *const filename,
                                     const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                                     const unsigned int step_frame=1, unsigned int *const bits_per_value=0,
                                     float *const voxel_size=0, CImg<charT> *const description=0) {
      return CImgList<T>().load_tiff(filename,first_frame,last_frame,step_frame,bits_per_value,voxel_size,description);
    }

    //@}
#endif
