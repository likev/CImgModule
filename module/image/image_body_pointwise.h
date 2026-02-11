#ifndef CIMG_MODULE_IMAGE_BODY_POINTWISE_H
#define CIMG_MODULE_IMAGE_BODY_POINTWISE_H

    //-----------------------------------
    //
    //! \name Value Manipulation
    //@{
    //-----------------------------------

    //! Fill all pixel values with specified value.
    /**
       \param val Fill value.
    **/
    CImg<T>& fill(const T& val) {
      if (is_empty()) return *this;
      if (val && sizeof(T)!=1) cimg_for(*this,ptrd,T) *ptrd = val;
      else std::memset(_data,(int)(ulongT)val,sizeof(T)*size()); // Double cast to allow val to be (void*)
      return *this;
    }

    //! Fill all pixel values with specified value \newinstance.
    CImg<T> get_fill(const T& val) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val);
    }

    //! Fill sequentially all pixel values with specified values.
    /**
       \param val0 First fill value.
       \param val1 Second fill value.
    **/
    CImg<T>& fill(const T& val0, const T& val1) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 1;
      for (ptrd = _data; ptrd<ptre; ) { *(ptrd++) = val0; *(ptrd++) = val1; }
      if (ptrd!=ptre + 1) *(ptrd++) = val0;
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 2;
      for (ptrd = _data; ptrd<ptre; ) { *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; }
      ptre+=2;
      switch (ptre - ptrd) {
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 3;
      for (ptrd = _data; ptrd<ptre; ) { *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; }
      ptre+=3;
      switch (ptre - ptrd) {
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 4;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4;
      }
      ptre+=4;
      switch (ptre - ptrd) {
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 5;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4; *(ptrd++) = val5;
      }
      ptre+=5;
      switch (ptre - ptrd) {
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 6;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4; *(ptrd++) = val5;
        *(ptrd++) = val6;
      }
      ptre+=6;
      switch (ptre - ptrd) {
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 7;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3;
        *(ptrd++) = val4; *(ptrd++) = val5; *(ptrd++) = val6; *(ptrd++) = val7;
      }
      ptre+=7;
      switch (ptre - ptrd) {
      case 7 : *(--ptre) = val6; // Fallthrough
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7, const T& val8) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 8;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2;
        *(ptrd++) = val3; *(ptrd++) = val4; *(ptrd++) = val5;
        *(ptrd++) = val6; *(ptrd++) = val7; *(ptrd++) = val8;
      }
      ptre+=8;
      switch (ptre - ptrd) {
      case 8 : *(--ptre) = val7; // Fallthrough
      case 7 : *(--ptre) = val6; // Fallthrough
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7, const T& val8) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7,val8);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7, const T& val8, const T& val9) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 9;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4;
        *(ptrd++) = val5; *(ptrd++) = val6; *(ptrd++) = val7; *(ptrd++) = val8; *(ptrd++) = val9;
      }
      ptre+=9;
      switch (ptre - ptrd) {
      case 9 : *(--ptre) = val8; // Fallthrough
      case 8 : *(--ptre) = val7; // Fallthrough
      case 7 : *(--ptre) = val6; // Fallthrough
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7, const T& val8, const T& val9) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7,val8,val9);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7, const T& val8, const T& val9, const T& val10) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 10;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4;
        *(ptrd++) = val5; *(ptrd++) = val6; *(ptrd++) = val7; *(ptrd++) = val8; *(ptrd++) = val9;
        *(ptrd++) = val10;
      }
      ptre+=10;
      switch (ptre - ptrd) {
      case 10 : *(--ptre) = val9; // Fallthrough
      case 9 : *(--ptre) = val8; // Fallthrough
      case 8 : *(--ptre) = val7; // Fallthrough
      case 7 : *(--ptre) = val6; // Fallthrough
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7, const T& val8, const T& val9, const T& val10) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7,val8,val9,val10);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 11;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4; *(ptrd++) = val5;
        *(ptrd++) = val6; *(ptrd++) = val7; *(ptrd++) = val8; *(ptrd++) = val9; *(ptrd++) = val10; *(ptrd++) = val11;
      }
      ptre+=11;
      switch (ptre - ptrd) {
      case 11 : *(--ptre) = val10; // Fallthrough
      case 10 : *(--ptre) = val9; // Fallthrough
      case 9 : *(--ptre) = val8; // Fallthrough
      case 8 : *(--ptre) = val7; // Fallthrough
      case 7 : *(--ptre) = val6; // Fallthrough
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,
                                                           val11);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11,
                  const T& val12) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 12;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4; *(ptrd++) = val5;
        *(ptrd++) = val6; *(ptrd++) = val7; *(ptrd++) = val8; *(ptrd++) = val9; *(ptrd++) = val10; *(ptrd++) = val11;
        *(ptrd++) = val12;
      }
      ptre+=12;
      switch (ptre - ptrd) {
      case 12 : *(--ptre) = val11; // Fallthrough
      case 11 : *(--ptre) = val10; // Fallthrough
      case 10 : *(--ptre) = val9; // Fallthrough
      case 9 : *(--ptre) = val8; // Fallthrough
      case 8 : *(--ptre) = val7; // Fallthrough
      case 7 : *(--ptre) = val6; // Fallthrough
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11,
                     const T& val12) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,
                                                           val11,val12);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11,
                  const T& val12, const T& val13) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 13;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4; *(ptrd++) = val5;
        *(ptrd++) = val6; *(ptrd++) = val7; *(ptrd++) = val8; *(ptrd++) = val9; *(ptrd++) = val10; *(ptrd++) = val11;
        *(ptrd++) = val12; *(ptrd++) = val13;
      }
      ptre+=13;
      switch (ptre - ptrd) {
      case 13 : *(--ptre) = val12; // Fallthrough
      case 12 : *(--ptre) = val11; // Fallthrough
      case 11 : *(--ptre) = val10; // Fallthrough
      case 10 : *(--ptre) = val9; // Fallthrough
      case 9 : *(--ptre) = val8; // Fallthrough
      case 8 : *(--ptre) = val7; // Fallthrough
      case 7 : *(--ptre) = val6; // Fallthrough
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11,
                     const T& val12, const T& val13) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,
                                                           val11,val12,val13);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11,
                  const T& val12, const T& val13, const T& val14) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 14;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4; *(ptrd++) = val5;
        *(ptrd++) = val6; *(ptrd++) = val7; *(ptrd++) = val8; *(ptrd++) = val9; *(ptrd++) = val10; *(ptrd++) = val11;
        *(ptrd++) = val12; *(ptrd++) = val13; *(ptrd++) = val14;
      }
      ptre+=14;
      switch (ptre - ptrd) {
      case 14 : *(--ptre) = val13; // Fallthrough
      case 13 : *(--ptre) = val12; // Fallthrough
      case 12 : *(--ptre) = val11; // Fallthrough
      case 11 : *(--ptre) = val10; // Fallthrough
      case 10 : *(--ptre) = val9; // Fallthrough
      case 9 : *(--ptre) = val8; // Fallthrough
      case 8 : *(--ptre) = val7; // Fallthrough
      case 7 : *(--ptre) = val6; // Fallthrough
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11,
                     const T& val12, const T& val13, const T& val14) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,
                                                           val11,val12,val13,val14);
    }

    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11,
                  const T& val12, const T& val13, const T& val14, const T& val15) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 15;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4; *(ptrd++) = val5;
        *(ptrd++) = val6; *(ptrd++) = val7; *(ptrd++) = val8; *(ptrd++) = val9; *(ptrd++) = val10; *(ptrd++) = val11;
        *(ptrd++) = val12; *(ptrd++) = val13; *(ptrd++) = val14; *(ptrd++) = val15;
      }
      ptre+=15;
      switch (ptre - ptrd) {
      case 15 : *(--ptre) = val14; // Fallthrough
      case 14 : *(--ptre) = val13; // Fallthrough
      case 13 : *(--ptre) = val12; // Fallthrough
      case 12 : *(--ptre) = val11; // Fallthrough
      case 11 : *(--ptre) = val10; // Fallthrough
      case 10 : *(--ptre) = val9; // Fallthrough
      case 9 : *(--ptre) = val8; // Fallthrough
      case 8 : *(--ptre) = val7; // Fallthrough
      case 7 : *(--ptre) = val6; // Fallthrough
      case 6 : *(--ptre) = val5; // Fallthrough
      case 5 : *(--ptre) = val4; // Fallthrough
      case 4 : *(--ptre) = val3; // Fallthrough
      case 3 : *(--ptre) = val2; // Fallthrough
      case 2 : *(--ptre) = val1; // Fallthrough
      case 1 : *(--ptre) = val0; // Fallthrough
      }
      return *this;
    }

    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11,
                     const T& val12, const T& val13, const T& val14, const T& val15) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,
                                                           val11,val12,val13,val14,val15);
    }

    //! Fill sequentially pixel values according to a given expression.
    /**
       \param expression C-string describing a math formula, or a sequence of values.
       \param repeat_values In case a list of values is provided, tells if this list must be repeated for the filling.
       \param allow_formula Tells that mathematical formulas are authorized for the filling.
       \param list_images In case of a mathematical expression, attach a list of images to the specified expression.
    **/
    CImg<T>& fill(const char *const expression, const bool repeat_values, const bool allow_formula=true,
                  CImgList<T> *const list_images=0) {
      return _fill(expression,repeat_values,allow_formula?3:1,list_images,"fill",0,0);
    }

    // bits of 'mode' can enable/disable these properties:
    // . 1 = Allow list of values.
    // . 2 = Allow formula.
    // . 4 = Evaluate but does not fill image values.
    template<typename t>
    CImg<T>& _fill(const char *const expression, const bool repeat_values, const unsigned int mode,
                   CImgList<t> *const list_images, const char *const calling_function,
                   const CImg<T> *provides_copy, CImg<doubleT> *const result_end) {
      if (!expression || !*expression) return *this; // But empty images be evaluated to allow side effects
      typedef typename CImg<t>::_cimg_math_parser _cimg_math_parser_t;
      const unsigned int excmode = cimg::exception_mode();
      cimg::exception_mode(0);
      CImg<charT> is_error_expr;
      bool is_done = false, is_value_sequence = false;
      cimg_abort_init;
      if (result_end) result_end->assign();

      // Detect value sequence.
      if (mode&1) {
        double value;
        char sep;
        const int err = cimg_sscanf(expression,"%lf %c",&value,&sep);
        if (err==1 || (err==2 && sep==',')) {
          if (err==1) { if (mode&4) return *this; return fill((T)value); }
          else is_value_sequence = true;
        }
      }

      // Try to fill values according to a formula.
      if (mode&2 && !is_value_sequence) {
        _cimg_abort_init_openmp;
        try {
          CImg<t> base;
          if (cimg::type<T>::string()==cimg::type<t>::string())
            base.assign(provides_copy?*provides_copy:*this,true);
          else
            base = provides_copy?*provides_copy:*this;

          _cimg_math_parser_t mp(expression + (*expression=='>' || *expression=='<' ||
                                               (*expression=='+' && expression[1]!='+') ||
                                               *expression=='*' || *expression==':'),
                                 calling_function,base,
                                 cimg::type<T>::string()==cimg::type<t>::string()?(CImg<t>*)this:&base,
                                 list_images,true);

          if (!provides_copy && expression &&
              *expression!='>' && *expression!='<' && *expression!=':' &&
              mp.need_input_copy)
            base.assign().assign(*this,false); // Needs input copy

          // Determine M2, smallest image dimension (used as axis for the most inner loop in parallelized iterations).
          // M1 is the total number of parallelized iterations.
          unsigned int M1 = 0, M2 = 0;
          cimg::unused(M1,M2);
          if (mp.result_dim) {
            M2 = cimg::min(_width,_height,_depth);
            M1 = M2==_width?_height*_depth:M2==_height?_width*_depth:_width*_height;
          } else {
            M2 = cimg::min(_width,_height,_depth,_spectrum);
            M1 = M2==_width?_height*_depth*_spectrum:M2==_height?_width*_depth*_spectrum:
              M2==_depth?_width*_height*_spectrum:_width*_height*_depth;
          }

          bool is_parallelizable = false;
          cimg_openmp_if(*expression=='*' || *expression==':' || (mp.is_parallelizable &&
                                                                  (M2>=2 || M1>=4096) && M1*M2>=32))
            is_parallelizable = true;

          if (mp.result_dim) { // Vector-valued expression
            const unsigned int N = std::min(mp.result_dim,_spectrum);
            const ulongT whd = (ulongT)_width*_height*_depth;
            T *ptrd = *expression=='<'?_data + _width*_height*_depth - 1:_data;

            if (*expression=='<') {
              CImg<doubleT> res(1,mp.result_dim);
              mp.begin_t();
              cimg_rofYZ(*this,y,z) {
                cimg_abort_test;
                if (mode&4) cimg_rofX(*this,x) mp(x,y,z,0);
                else cimg_rofX(*this,x) {
                    mp(x,y,z,0,res._data);
                    const double *ptrs = res._data;
                    T *_ptrd = ptrd--; for (unsigned int n = N; n>0; --n) { *_ptrd = (T)(*ptrs++); _ptrd+=whd; }
                  }
              }
              mp.end_t();

            } else if (*expression=='>' || *expression=='+' || !is_parallelizable) {
              CImg<doubleT> res(1,mp.result_dim);
              mp.begin_t();
              cimg_forYZ(*this,y,z) {
                cimg_abort_test;
                if (mode&4) cimg_forX(*this,x) mp(x,y,z,0);
                else cimg_forX(*this,x) {
                    mp(x,y,z,0,res._data);
                    const double *ptrs = res._data;
                    T *_ptrd = ptrd++; for (unsigned int n = N; n>0; --n) { *_ptrd = (T)(*ptrs++); _ptrd+=whd; }
                  }
              }
              mp.end_t();

            } else {

#if cimg_use_openmp!=0
              const int num_threads = (int)std::min(size(),(ulongT)omp_get_max_threads());
              cimg_pragma_openmp(parallel if (num_threads>0) num_threads(num_threads)) {
                _cimg_math_parser_t
                  *const _mp = omp_get_thread_num()?new _cimg_math_parser_t(mp):&mp,
                  &lmp = *_mp;
                lmp.is_fill = true;
                cimg_pragma_openmp(barrier)
                lmp.begin_t();

#define _cimg_fill_openmp_vector(_YZ,_y,_z,_X,_x,_sx,_sy,_sz,_off) \
  cimg_pragma_openmp(for cimg_openmp_collapse(2)) \
  cimg_for##_YZ(*this,_y,_z) _cimg_abort_try_openmp { \
    cimg_abort_test; \
    if (mode&4) cimg_for##_X(*this,_x) lmp(x,y,z,0); \
    else { \
      CImg<doubleT> res(1,lmp.result_dim); \
      T *__ptrd = data(_sx,_sy,_sz,0); \
      const ulongT off = (ulongT)_off; \
      cimg_for##_X(*this,_x) { \
        lmp(x,y,z,0,res._data); \
        const double *ptrs = res._data; \
        T *_ptrd = __ptrd; \
        for (unsigned int n = N; n>0; --n) { *_ptrd = (T)(*ptrs++); _ptrd+=whd; } \
        __ptrd+=off; \
      } \
    } \
  } _cimg_abort_catch_openmp _cimg_abort_catch_fill_openmp

                if (M2==_width) { _cimg_fill_openmp_vector(YZ,y,z,X,x,0,y,z,1) }
                else if (M2==_height) { _cimg_fill_openmp_vector(XZ,x,z,Y,y,x,0,z,_width) }
                else { _cimg_fill_openmp_vector(XY,x,y,Z,z,x,y,0,_width*_height) }

                lmp.end_t();
                cimg_pragma_openmp(barrier) cimg_pragma_openmp(critical) { lmp.merge(mp); }
                if (&lmp!=&mp) delete &lmp;
              }
#endif
            }

          } else { // Scalar-valued expression
            T *ptrd = *expression=='<'?end() - 1:_data;
            if (*expression=='<') {
              mp.begin_t();
              if (mode&4) cimg_rofYZC(*this,y,z,c) { cimg_abort_test; cimg_rofX(*this,x) mp(x,y,z,c); }
                else cimg_rofYZC(*this,y,z,c) { cimg_abort_test; cimg_rofX(*this,x) *(ptrd--) = (T)mp(x,y,z,c); }
              mp.end_t();

            } else if (*expression=='>' || *expression=='+' || !is_parallelizable) {
              mp.begin_t();
              if (mode&4) cimg_forYZC(*this,y,z,c) { cimg_abort_test; cimg_forX(*this,x) mp(x,y,z,c); }
              else cimg_forYZC(*this,y,z,c) { cimg_abort_test; cimg_forX(*this,x) *(ptrd++) = (T)mp(x,y,z,c); }
              mp.end_t();

            } else {

#if cimg_use_openmp!=0
              const int num_threads = (int)std::min(size(),(ulongT)omp_get_max_threads());
              cimg_pragma_openmp(parallel if (num_threads>0) num_threads(num_threads)) {
                _cimg_math_parser_t
                  *const _mp = omp_get_thread_num()?new _cimg_math_parser_t(mp):&mp,
                  &lmp = *_mp;
                lmp.is_fill = true;
                cimg_pragma_openmp(barrier)
                lmp.begin_t();

#define _cimg_fill_openmp_scalar(_YZC,_y,_z,_c,_X,_x,_sx,_sy,_sz,_sc,_off) \
  cimg_pragma_openmp(for cimg_openmp_collapse(3)) \
  cimg_for##_YZC(*this,_y,_z,_c) _cimg_abort_try_openmp { \
    cimg_abort_test; \
    if (mode&4) cimg_for##_X(*this,_x) lmp(x,y,z,c); \
    else { \
      T *_ptrd = data(_sx,_sy,_sz,_sc); \
      const ulongT off = (ulongT)_off; \
      cimg_for##_X(*this,_x) { *_ptrd = (T)lmp(x,y,z,c); _ptrd+=off; } \
    } \
  } _cimg_abort_catch_openmp _cimg_abort_catch_fill_openmp

                if (M2==_width) { _cimg_fill_openmp_scalar(YZC,y,z,c,X,x,0,y,z,c,1) }
                else if (M2==_height) { _cimg_fill_openmp_scalar(XZC,x,z,c,Y,y,x,0,z,c,_width) }
                else if (M2==_depth) { _cimg_fill_openmp_scalar(XYC,x,y,c,Z,z,x,y,0,c,_width*_height) }
                else { _cimg_fill_openmp_scalar(XYZ,x,y,z,C,c,x,y,z,0,_width*_height*_depth) }

                lmp.end_t();
                cimg_pragma_openmp(barrier) cimg_pragma_openmp(critical) { lmp.merge(mp); }
                if (&lmp!=&mp) delete &lmp;
              }
#endif
            }
          }
          mp.end();

          if (result_end && mp.result_end) // Transfer result of the end() block if requested
            result_end->assign(mp.result_end + (mp.result_end_dim?1:0),std::max(1U,mp.result_end_dim));

          is_done = true;
        } catch (CImgException& e) { CImg<charT>::string(e._message).move_to(is_error_expr); }
      }

      // Try to fill values according to a value sequence.
      if (!is_done && mode&1) is_done = !_fill_from_values(expression,repeat_values);

      if (!is_done) {
        cimg::exception_mode(excmode);
        if (is_error_expr) throw CImgArgumentException(_cimg_instance
                                                       "%s",
                                                       cimg_instance,is_error_expr._data);
        else throw CImgArgumentException(_cimg_instance
                                         "%s(): Invalid sequence of filling values '%s'.",
                                         cimg_instance,calling_function,expression);
      }
      cimg::exception_mode(excmode);
      cimg_abort_test;
      return *this;
    }

    //! Fill sequentially pixel values according to a given expression \newinstance.
    CImg<T> get_fill(const char *const expression, const bool repeat_values, const bool allow_formula=true,
                     CImgList<T> *const list_images=0) const {
      return (+*this)._fill(expression,repeat_values,allow_formula?3:1,list_images,"fill",this,0);
    }

    //! Fill sequentially pixel values according to a value sequence, given as a string.
    /**
       \param values C-string describing a sequence of values.
       \param repeat_values Tells if this sequence must be repeated when filling.
    **/
    CImg<T>& fill_from_values(const char *const values, const bool repeat_values) {
      if (_fill_from_values(values,repeat_values))
        throw CImgArgumentException(_cimg_instance
                                    "Invalid sequence of filling values '%s'.",
                                    cimg_instance,values);
      return *this;
    }

    //! Fill sequentially pixel values according to a value sequence, given as a string \newinstance.
    CImg<T> get_fill_from_values(const char *const values, const bool repeat_values) const {
      return (+*this).fill_from_values(values,repeat_values);
    }

    // Fill image according to a value sequence, given as a string.
    // Return 'true' if an error occured, 'false' otherwise.
    bool _fill_from_values(const char *const values, const bool repeat_values) {
      CImg<charT> item(256);
      const char *nvalues = values;
      const ulongT siz = size();
      T *ptrd = _data;
      ulongT nb = 0;
      char sep = 0;
      for (double val = 0; *nvalues && nb<siz; ++nb) {
        sep = 0;
        const int err = cimg_sscanf(nvalues,"%255[ \n\t0-9.eEinfa+-]%c",item._data,&sep);
        if (err>0 && cimg_sscanf(item,"%lf",&val)==1 && (sep==',' || sep==';' || err==1)) {
          nvalues+=std::strlen(item) + (err>1);
          *(ptrd++) = (T)val;
        } else break;
      }
      if (nb<siz && (sep || *nvalues)) return true;
      if (repeat_values && nb && nb<siz)
        for (T *ptrs = _data, *const ptre = _data + siz; ptrd<ptre; ++ptrs) *(ptrd++) = *ptrs;
      return false;
    }

    //! Fill sequentially pixel values according to the values found in another image.
    /**
       \param values Image containing the values used for the filling.
       \param repeat_values In case there are less values than necessary in \c values, tells if these values must be
         repeated for the filling.
    **/
    template<typename t>
    CImg<T>& fill(const CImg<t>& values, const bool repeat_values=true) {
      if (is_empty() || !values) return *this;
      T *ptrd = _data, *ptre = ptrd + size();
      for (t *ptrs = values._data, *ptrs_end = ptrs + values.size(); ptrs<ptrs_end && ptrd<ptre; ++ptrs)
        *(ptrd++) = (T)*ptrs;
      if (repeat_values && ptrd<ptre) for (T *ptrs = _data; ptrd<ptre; ++ptrs) *(ptrd++) = *ptrs;
      return *this;
    }

    //! Fill sequentially pixel values according to the values found in another image \newinstance.
    template<typename t>
    CImg<T> get_fill(const CImg<t>& values, const bool repeat_values=true) const {
      return repeat_values?CImg<T>(_width,_height,_depth,_spectrum).fill(values,repeat_values):
        (+*this).fill(values,repeat_values);
    }

    //! Fill pixel values along the X-axis at a specified pixel position.
    /**
       \param y Y-coordinate of the filled column.
       \param z Z-coordinate of the filled column.
       \param c C-coordinate of the filled column.
       \param a0 First fill value.
    **/
    CImg<T>& fillX(const unsigned int y, const unsigned int z, const unsigned int c, const int a0, ...) {
#define _cimg_fill1(x,y,z,c,off,siz,t) { \
    va_list ap; va_start(ap,a0); T *ptrd = data(x,y,z,c); *ptrd = (T)a0; \
    for (unsigned int k = 1; k<siz; ++k) { ptrd+=off; *ptrd = (T)va_arg(ap,t); } \
    va_end(ap); }
      if (y<_height && z<_depth && c<_spectrum) _cimg_fill1(0,y,z,c,1,_width,int);
      return *this;
    }

    //! Fill pixel values along the X-axis at a specified pixel position \overloading.
    CImg<T>& fillX(const unsigned int y, const unsigned int z, const unsigned int c, const double a0, ...) {
      if (y<_height && z<_depth && c<_spectrum) _cimg_fill1(0,y,z,c,1,_width,double);
      return *this;
    }

    //! Fill pixel values along the Y-axis at a specified pixel position.
    /**
       \param x X-coordinate of the filled row.
       \param z Z-coordinate of the filled row.
       \param c C-coordinate of the filled row.
       \param a0 First fill value.
    **/
    CImg<T>& fillY(const unsigned int x, const unsigned int z, const unsigned int c, const int a0, ...) {
      if (x<_width && z<_depth && c<_spectrum) _cimg_fill1(x,0,z,c,_width,_height,int);
      return *this;
    }

    //! Fill pixel values along the Y-axis at a specified pixel position \overloading.
    CImg<T>& fillY(const unsigned int x, const unsigned int z, const unsigned int c, const double a0, ...) {
      if (x<_width && z<_depth && c<_spectrum) _cimg_fill1(x,0,z,c,_width,_height,double);
      return *this;
    }

    //! Fill pixel values along the Z-axis at a specified pixel position.
    /**
       \param x X-coordinate of the filled slice.
       \param y Y-coordinate of the filled slice.
       \param c C-coordinate of the filled slice.
       \param a0 First fill value.
    **/
    CImg<T>& fillZ(const unsigned int x, const unsigned int y, const unsigned int c, const int a0, ...) {
      const ulongT wh = (ulongT)_width*_height;
      if (x<_width && y<_height && c<_spectrum) _cimg_fill1(x,y,0,c,wh,_depth,int);
      return *this;
    }

    //! Fill pixel values along the Z-axis at a specified pixel position \overloading.
    CImg<T>& fillZ(const unsigned int x, const unsigned int y, const unsigned int c, const double a0, ...) {
      const ulongT wh = (ulongT)_width*_height;
      if (x<_width && y<_height && c<_spectrum) _cimg_fill1(x,y,0,c,wh,_depth,double);
      return *this;
    }

    //! Fill pixel values along the C-axis at a specified pixel position.
    /**
       \param x X-coordinate of the filled channel.
       \param y Y-coordinate of the filled channel.
       \param z Z-coordinate of the filled channel.
       \param a0 First filling value.
    **/
    CImg<T>& fillC(const unsigned int x, const unsigned int y, const unsigned int z, const int a0, ...) {
      const ulongT whd = (ulongT)_width*_height*_depth;
      if (x<_width && y<_height && z<_depth) _cimg_fill1(x,y,z,0,whd,_spectrum,int);
      return *this;
    }

    //! Fill pixel values along the C-axis at a specified pixel position \overloading.
    CImg<T>& fillC(const unsigned int x, const unsigned int y, const unsigned int z, const double a0, ...) {
      const ulongT whd = (ulongT)_width*_height*_depth;
      if (x<_width && y<_height && z<_depth) _cimg_fill1(x,y,z,0,whd,_spectrum,double);
      return *this;
    }

    //! Discard specified sequence of values in the image buffer, along a specific axis.
    /**
       \param values Sequence of values to discard.
       \param axis Axis along which the values are discarded. If set to \c 0 (default value)
         the method does it for all the buffer values and returns a one-column vector.
       \note Discarded values will change the image geometry, so the resulting image
         is returned as a one-column vector.
    **/
    template<typename t>
    CImg<T>& discard(const CImg<t>& values, const char axis=0) {
      if (is_empty() || !values) return *this;
      return get_discard(values,axis).move_to(*this);
    }

    template<typename t>
    CImg<T> get_discard(const CImg<t>& values, const char axis=0) const {
      if (!values) return +*this;
      CImg<T> res;
      if (is_empty()) return res;
      const ulongT vsiz = values.size();
      const char _axis = cimg::lowercase(axis);
      ulongT j = 0;
      unsigned int k = 0;
      int i0 = 0;
      res.assign(width(),height(),depth(),spectrum());
      switch (_axis) {
      case 'x' : {
        cimg_forX(*this,i) {
          if ((*this)(i)!=(T)values[j]) {
            if (j) --i;
            res.draw_image(k,get_columns(i0,i));
            k+=i - i0 + 1; i0 = i + 1; j = 0;
          } else { ++j; if (j>=vsiz) { j = 0; i0 = i + 1; } }
        }
        if (i0<width()) { res.draw_image(k,get_columns(i0,width() - 1)); k+=width() - i0; }
        res.resize(k,-100,-100,-100,0);
      } break;
      case 'y' : {
        cimg_forY(*this,i) {
          if ((*this)(0,i)!=(T)values[j]) {
            if (j) --i;
            res.draw_image(0,k,get_rows(i0,i));
            k+=i - i0 + 1; i0 = i + 1; j = 0;
          } else { ++j; if (j>=vsiz) { j = 0; i0 = i + 1; } }
        }
        if (i0<height()) { res.draw_image(0,k,get_rows(i0,height() - 1)); k+=height() - i0; }
        res.resize(-100,k,-100,-100,0);
      } break;
      case 'z' : {
        cimg_forZ(*this,i) {
          if ((*this)(0,0,i)!=(T)values[j]) {
            if (j) --i;
            res.draw_image(0,0,k,get_slices(i0,i));
            k+=i - i0 + 1; i0 = i + 1; j = 0;
          } else { ++j; if (j>=vsiz) { j = 0; i0 = i + 1; } }
        }
        if (i0<depth()) { res.draw_image(0,0,k,get_slices(i0,height() - 1)); k+=depth() - i0; }
        res.resize(-100,-100,k,-100,0);
      } break;
      case 'c' : {
        cimg_forC(*this,i) {
          if ((*this)(0,0,0,i)!=(T)values[j]) {
            if (j) --i;
            res.draw_image(0,0,0,k,get_channels(i0,i));
            k+=i - i0 + 1; i0 = i + 1; j = 0;
          } else { ++j; if (j>=vsiz) { j = 0; i0 = i + 1; } }
        }
        if (i0<spectrum()) { res.draw_image(0,0,k,get_channels(i0,height() - 1)); k+=spectrum() - i0; }
        res.resize(-100,-100,-100,k,0);
      } break;
      default : {
        const ulongT siz = size();
        res.unroll('y');
        if (vsiz==1) { // Optimized version for a single discard value
          const T val = (T)values[0];
          cimg_foroff(*this,i) {
            const T _val = (T)_data[i];
            if (_val!=val) res[k++] = _val;
          }
        } else { // Generic version
          cimg_foroff(*this,i) {
            if ((*this)[i]!=(T)values[j]) {
              if (j) --i;
              std::memcpy(res._data + k,_data + i0,(i - i0 + 1)*sizeof(T));
              k+=i - i0 + 1; i0 = (int)i + 1; j = 0;
            } else { ++j; if (j>=vsiz) { j = 0; i0 = (int)i + 1; }}
          }
          if ((ulongT)i0<siz) { std::memcpy(res._data + k,_data + i0,(siz - i0)*sizeof(T)); k+=siz - i0; }
        }
        res.resize(1,k,1,1,0);
      }
      }
      return res;
    }

    //! Discard neighboring duplicates in the image buffer, along the specified axis.
    CImg<T>& discard(const char axis=0) {
      return get_discard(axis).move_to(*this);
    }

    //! Discard neighboring duplicates in the image buffer, along the specified axis \newinstance.
    CImg<T> get_discard(const char axis=0) const {
      CImg<T> res;
      if (is_empty()) return res;
      const char _axis = cimg::lowercase(axis);
      T current = *_data?(T)0:(T)1;
      int j = 0;
      res.assign(width(),height(),depth(),spectrum());
      switch (_axis) {
      case 'x' : {
        cimg_forX(*this,i)
          if ((*this)(i)!=current) { res.draw_image(j++,get_column(i)); current = (*this)(i); }
        res.resize(j,-100,-100,-100,0);
      } break;
      case 'y' : {
        cimg_forY(*this,i)
          if ((*this)(0,i)!=current) { res.draw_image(0,j++,get_row(i)); current = (*this)(0,i); }
        res.resize(-100,j,-100,-100,0);
      } break;
      case 'z' : {
        cimg_forZ(*this,i)
          if ((*this)(0,0,i)!=current) { res.draw_image(0,0,j++,get_slice(i)); current = (*this)(0,0,i); }
        res.resize(-100,-100,j,-100,0);
      } break;
      case 'c' : {
        cimg_forC(*this,i)
          if ((*this)(0,0,0,i)!=current) { res.draw_image(0,0,0,j++,get_channel(i)); current = (*this)(0,0,0,i); }
        res.resize(-100,-100,-100,j,0);
      } break;
      default : {
        res.unroll('y');
        cimg_foroff(*this,i) {
          const T val = (*this)[i];
          if (val!=current) res[j++] = current = val;
        }
        res.resize(-100,j,-100,-100,0);
      }
      }
      return res;
    }

    //! Invert endianness of all pixel values.
    /**
     **/
    CImg<T>& invert_endianness() {
      cimg::invert_endianness(_data,size());
      return *this;
    }

    //! Invert endianness of all pixel values \newinstance.
    CImg<T> get_invert_endianness() const {
      return (+*this).invert_endianness();
    }

    //! Fill image with random values in specified range.
    /**
       \param val_min Minimal authorized random value.
       \param val_max Maximal authorized random value.
       \note Random variables are uniformly distributed in [val_min,val_max].
     **/
    CImg<T>& rand(const T& val_min, const T& val_max) {
      const float delta = (float)val_max - (float)val_min + (cimg::type<T>::is_float()?0:1);
      if (cimg::type<T>::is_float()) cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),524288)) {
          cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
          cimg_rofoff(*this,off) _data[off] = (T)(val_min + delta*cimg::rand(1,&rng));
          cimg::srand(rng);
        } else cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),524288)) {
          cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
          cimg_rofoff(*this,off) _data[off] = std::min(val_max,(T)(val_min + delta*cimg::rand(1,&rng)));
          cimg::srand(rng);
        }
      return *this;
    }

    //! Fill image with random values in specified range \newinstance.
    CImg<T> get_rand(const T& val_min, const T& val_max) const {
      return (+*this).rand(val_min,val_max);
    }

    //! Fill image with random values following specified distribution and range.
    /**
       \param val_min Minimal authorized random value.
       \param val_max Maximal authorized random value.
       \param pdf Probability density function.
       \param precision Precision of generated values. Set to '0' for automatic precision.
         A negative value means 'percentage of the pdf size'.
     **/
    template<typename t>
    CImg<T>& rand(const T& val_min, const T& val_max, const CImg<t>& pdf, const int precision=65536) {
      typedef _cimg_tfloat tfloat;
      const unsigned int
        siz = (unsigned int)pdf.size(),
        prec = precision<0?(unsigned int)(-(float)siz*precision/100):(unsigned int)precision;
      if (siz<2 || precision<2) return fill(val_min);
      const tfloat
        delta = (tfloat)val_max - (tfloat)val_min,
        delta_over_siz1 = delta/(siz - 1);

      // Compute inverse cdf.
      CImg<tfloat> cdf = pdf.get_max((t)0).cumulate(), icdf(prec);
      unsigned int k = 0;
      tfloat p = 0;
      cdf*=(prec - 1)/cdf.back();
      cimg_forX(icdf,x) {
        while (k<siz && (!(p = cdf[k]) || p<x)) ++k;
        if (k>=siz) { while (x<icdf.width()) icdf[x++] = val_max; break; }
        icdf[x] = val_min + k*delta_over_siz1;
      }

      // Generate random numbers.
      cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),524288)) {
        cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
        rng+=omp_get_thread_num();
#endif
        cimg_pragma_openmp(for)
        cimg_rofoff(*this,off) {
          const unsigned int
            _ind = (unsigned int)cimg::rand(0,(double)prec,&rng),
            ind = _ind==prec?0:_ind;
          _data[off] = (T)icdf[ind];
        }
        cimg::srand(rng);
      }
      return *this;
    }

    //! Fill image with random values following specified distribution and range \newinstance.
    template<typename t>
    CImg<T> get_rand(const T& val_min, const T& val_max, const CImg<t>& pdf, const int precision=65536) const {
      return (+*this).rand(val_min,val_max,pdf,precision);
    }

    //! Round pixel values.
    /**
       \param y Rounding precision.
       \param rounding_type Rounding type. Can be:
       - \c -1: Backward.
       - \c 0: Nearest.
       - \c 1: Forward.
    **/
    CImg<T>& round(const double y=1, const int rounding_type=0) {
      if (y>0) cimg_openmp_for(*this,cimg::round(*ptr,y,rounding_type),8192,T);
      return *this;
    }

    //! Round pixel values \newinstance.
    CImg<T> get_round(const double y=1, const unsigned int rounding_type=0) const {
      return (+*this).round(y,rounding_type);
    }

    //! Add random noise to pixel values.
    /**
       \param sigma Amplitude of the random additive noise. If \p sigma<0, it stands for a percentage of the
         global value range.
       \param noise_type Type of additive noise (can be \p 0=gaussian, \p 1=uniform, \p 2=Salt and Pepper,
         \p 3=Poisson or \p 4=Rician).
       \return A reference to the modified image instance.
       \note
       - For Poisson noise (\p noise_type=3), parameter \p sigma is ignored, as Poisson noise only depends on
         the image value itself.
       - Function \p CImg<T>::get_noise() is also defined. It returns a non-shared modified copy of the image instance.
       \par Example
       \code
       const CImg<float> img("reference.jpg"), res = img.get_noise(40);
       (img,res.normalize(0,255)).display();
       \endcode
       \image html ref_noise.jpg
    **/
    CImg<T>& noise(const double amplitude, const unsigned int noise_type=0) {
      if (is_empty()) return *this;
      const Tfloat vmin = (Tfloat)cimg::type<T>::min(), vmax = (Tfloat)cimg::type<T>::max();
      Tfloat namplitude = (Tfloat)amplitude, m = 0, M = 0;
      if (namplitude==0 && noise_type!=3) return *this;
      if (namplitude<0 || noise_type==2) m = (Tfloat)min_max(M);
      if (namplitude<0) namplitude = (Tfloat)(-namplitude*(M-m)/100.);
      switch (noise_type) {
      case 0 : { // Gaussian noise
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
          cimg_rofoff(*this,off) {
            Tfloat val = (Tfloat)(_data[off] + namplitude*cimg::grand(&rng));
            if (val>vmax) val = vmax;
            if (val<vmin) val = vmin;
            _data[off] = (T)val;
          }
          cimg::srand(rng);
        }
      } break;
      case 1 : { // Uniform noise
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
          cimg_rofoff(*this,off) {
            Tfloat val = (Tfloat)(_data[off] + namplitude*cimg::rand(-1,1,&rng));
            if (val>vmax) val = vmax;
            if (val<vmin) val = vmin;
            _data[off] = (T)val;
          }
          cimg::srand(rng);
        }
      } break;
      case 2 : { // Salt & Pepper noise
        if (namplitude<0) namplitude = -namplitude;
        if (M==m) {
          if (cimg::type<T>::is_float()) { --m; ++M; }
          else { m = (Tfloat)cimg::type<T>::min(); M = (Tfloat)cimg::type<T>::max(); }
        }
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
          cimg_rofoff(*this,off) if (cimg::rand(100,&rng)<namplitude) _data[off] = (T)(cimg::rand(1,&rng)<0.5?M:m);
          cimg::srand(rng);
          }
      } break;
      case 3 : { // Poisson Noise
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
          cimg_rofoff(*this,off) _data[off] = (T)cimg::prand(_data[off],&rng);
          cimg::srand(rng);
        }
      } break;
      case 4 : { // Rice noise
        const Tfloat sqrt2 = (Tfloat)std::sqrt(2.);
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
          cimg_rofoff(*this,off) {
            const Tfloat
              val0 = (Tfloat)_data[off]/sqrt2,
              re = (Tfloat)(val0 + namplitude*cimg::grand(&rng)),
              im = (Tfloat)(val0 + namplitude*cimg::grand(&rng));
            Tfloat val = cimg::hypot(re,im);
            if (val>vmax) val = vmax;
            if (val<vmin) val = vmin;
            _data[off] = (T)val;
          }
          cimg::srand(rng);
        }
      } break;
      default :
        throw CImgArgumentException(_cimg_instance
                                    "noise(): Invalid specified noise type %d "
                                    "(should be { 0=gaussian | 1=uniform | 2=salt&Pepper | 3=poisson }).",
                                    cimg_instance,
                                    noise_type);
      }
      return *this;
    }

    //! Add random noise to pixel values \newinstance.
    CImg<T> get_noise(const double amplitude, const unsigned int noise_type=0) const {
      return (+*this).noise(amplitude,noise_type);
    }

    //! Linearly normalize pixel values.
    /**
       \param min_value Minimum desired value of the resulting image.
       \param max_value Maximum desired value of the resulting image.
       \param constant_case_ratio In case of instance image having a constant value, tell what ratio
              of [min_value,max_value] is used to fill the normalized image
              (=0 for min_value, =1 for max_value, =0.5 for (min_value + max_value)/2).
       \par Example
       \code
       const CImg<float> img("reference.jpg"), res = img.get_normalize(160,220);
       (img,res).display();
       \endcode
       \image html ref_normalize2.jpg
    **/
    CImg<T>& normalize(const T& min_value, const T& max_value,
                       const float constant_case_ratio=0) {
      if (is_empty()) return *this;
      const T a = min_value<max_value?min_value:max_value, b = min_value<max_value?max_value:min_value;
      T m, M = max_min(m);
      const Tfloat fm = (Tfloat)m, fM = (Tfloat)M;
      if (m==M)
        return fill(constant_case_ratio==0?a:
                    constant_case_ratio==1?b:
                    (T)((1 - constant_case_ratio)*a + constant_case_ratio*b));
      if (m!=a || M!=b) cimg_rof(*this,ptrd,T) *ptrd = (T)((*ptrd - fm)/(fM - fm)*(b - a) + a);
      return *this;
    }

    //! Linearly normalize pixel values \newinstance.
    CImg<Tfloat> get_normalize(const T& min_value, const T& max_value,
                               const float ratio_if_constant_image=0) const {
      return CImg<Tfloat>(*this,false).normalize((Tfloat)min_value,(Tfloat)max_value,ratio_if_constant_image);
    }

    //! Normalize multi-valued pixels of the image instance, with respect to their L2-norm.
    /**
       \par Example
       \code
       const CImg<float> img("reference.jpg"), res = img.get_normalize();
       (img,res.normalize(0,255)).display();
       \endcode
       \image html ref_normalize.jpg
    **/
    CImg<T>& normalize() {
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*512 &&
                                                                 _height*_depth>=16))
      cimg_forYZ(*this,y,z) {
        T *ptrd = data(0,y,z,0);
        cimg_forX(*this,x) {
          const T *ptrs = ptrd;
          float n = 0;
          cimg_forC(*this,c) { n+=cimg::sqr((float)*ptrs); ptrs+=whd; }
          n = (float)std::sqrt(n);
          T *_ptrd = ptrd++;
          if (n>0) cimg_forC(*this,c) { *_ptrd = (T)(*_ptrd/n); _ptrd+=whd; }
          else cimg_forC(*this,c) { *_ptrd = (T)0; _ptrd+=whd; }
        }
      }
      return *this;
    }

    //! Normalize multi-valued pixels of the image instance, with respect to their L2-norm \newinstance.
    CImg<Tfloat> get_normalize() const {
      return CImg<Tfloat>(*this,false).normalize();
    }

    //! Compute Lp-norm of each multi-valued pixel of the image instance.
    /**
       \param norm_type Type of computed vector norm (can be \p -1=Linf, or \p greater or equal than 0).
       \par Example
       \code
       const CImg<float> img("reference.jpg"), res = img.get_norm();
       (img,res.normalize(0,255)).display();
       \endcode
       \image html ref_norm.jpg
    **/
    CImg<T>& norm(const int norm_type=2) {
      if (_spectrum==1 && norm_type) return abs();
      return get_norm(norm_type).move_to(*this);
    }

    //! Compute L2-norm of each multi-valued pixel of the image instance \newinstance.
    CImg<Tfloat> get_norm(const int norm_type=2) const {
      if (is_empty()) return *this;
      if (_spectrum==1 && norm_type) return get_abs();
      const ulongT whd = (ulongT)_width*_height*_depth;
      CImg<Tfloat> res(_width,_height,_depth);
      switch (norm_type) {
      case -1 : { // Linf-norm
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*512 &&
                                                                   _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            Tfloat n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { const Tfloat val = (Tfloat)cimg::abs(*_ptrs); if (val>n) n = val; _ptrs+=whd; }
            *(ptrd++) = n;
          }
        }
      } break;
      case 0 : { // L0-norm
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*512 &&
                                                                   _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            unsigned int n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { n+=*_ptrs==0?0:1; _ptrs+=whd; }
            *(ptrd++) = (Tfloat)n;
          }
        }
      } break;
      case 1 : { // L1-norm
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*512 &&
                                                                   _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            Tfloat n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { n+=cimg::abs(*_ptrs); _ptrs+=whd; }
            *(ptrd++) = n;
          }
        }
      } break;
      case 2 : { // L2-norm
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*512 &&
                                                                   _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            Tfloat n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { n+=cimg::sqr((Tfloat)*_ptrs); _ptrs+=whd; }
            *(ptrd++) = (Tfloat)std::sqrt((Tfloat)n);
          }
        }
      } break;
      default : { // Linf-norm
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*512 &&
                                                                   _height*_depth>=16))
        cimg_forYZ(*this,y,z) {
          const ulongT off = (ulongT)offset(0,y,z);
          const T *ptrs = _data + off;
          Tfloat *ptrd = res._data + off;
          cimg_forX(*this,x) {
            Tfloat n = 0;
            const T *_ptrs = ptrs++;
            cimg_forC(*this,c) { n+=std::pow(cimg::abs((Tfloat)*_ptrs),(Tfloat)norm_type); _ptrs+=whd; }
            *(ptrd++) = (Tfloat)std::pow((Tfloat)n,1/(Tfloat)norm_type);
          }
        }
      }
      }
      return res;
    }

    //! Cut pixel values in specified range.
    /**
       \param min_value Minimum desired value of the resulting image.
       \param max_value Maximum desired value of the resulting image.
       \par Example
       \code
       const CImg<float> img("reference.jpg"), res = img.get_cut(160,220);
       (img,res).display();
       \endcode
       \image html ref_cut.jpg
    **/
    CImg<T>& cut(const T& min_value, const T& max_value) {
      if (is_empty()) return *this;
      const T
        a = min_value<max_value?min_value:max_value,
        b = min_value<max_value?max_value:min_value;
      cimg_openmp_for(*this,cimg::cut(*ptr,a,b),32768,T);
      return *this;
    }

    //! Cut pixel values in specified range \newinstance.
    CImg<T> get_cut(const T& min_value, const T& max_value) const {
      return (+*this).cut(min_value,max_value);
    }

    //! Cut pixel absolute values in specified range.
    /**
       \param min_value Minimum desired absolute value of the resulting image.
       \param max_value Maximum desired absolute value of the resulting image.
       \param offset Offset applied to absolute value of the resulting image.
    **/
    CImg<T>& abscut(const T& min_value, const T& max_value, const T& offset) {
      if (is_empty()) return *this;
      const T
        a = min_value<max_value?min_value:max_value,
        b = min_value<max_value?max_value:min_value;
      cimg_openmp_for(*this,cimg::abscut(*ptr,a,b,offset),32768,T);
      return *this;
    }

    //! Cut pixel values in specified range \newinstance.
    CImg<T> get_abscut(const T& min_value, const T& max_value, const T& offset) const {
      return (+*this).abscut(min_value,max_value,offset);
    }

    //! Uniformly quantize pixel values.
    /**
       \param nb_levels Number of quantization levels.
       \param keep_range Tells if resulting values keep the same range as the original ones.
       \par Example
       \code
       const CImg<float> img("reference.jpg"), res = img.get_quantize(4);
       (img,res).display();
       \endcode
       \image html ref_quantize.jpg
    **/
    CImg<T>& quantize(const unsigned int nb_levels, const bool keep_range=true) {
      if (!nb_levels)
        throw CImgArgumentException(_cimg_instance
                                    "quantize(): Invalid quantization request with 0 values.",
                                    cimg_instance);

      if (is_empty()) return *this;
      Tfloat m, M = (Tfloat)max_min(m), range = M - m;
      if (range>0) {
        if (keep_range)
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32768))
          cimg_rofoff(*this,off) {
            const unsigned int val = (unsigned int)((_data[off] - m)*nb_levels/range);
            _data[off] = (T)(m + std::min(val,nb_levels - 1)*range/nb_levels);
          } else
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32768))
          cimg_rofoff(*this,off) {
            const unsigned int val = (unsigned int)((_data[off] - m)*nb_levels/range);
            _data[off] = (T)std::min(val,nb_levels - 1);
          }
      }
      return *this;
    }

    //! Uniformly quantize pixel values \newinstance.
    CImg<T> get_quantize(const unsigned int n, const bool keep_range=true) const {
      return (+*this).quantize(n,keep_range);
    }

    //! Return the Otsu threshold.
    /**
       \param nb_levels Number of histogram levels used for the estimation.
    **/
    T otsu(const unsigned int nb_levels=256) const {
      T m,M = max_min(m);
      CImg<ulongT> hist = get_histogram(nb_levels,m,M);
      ulongT sum = 0, sumB = 0, wB = 0;
      double best_variance = 0;
      unsigned int best_t = 0;
      cimg_forX(hist,t) sum+=t*hist[t];
      cimg_forX(hist,t) {
        wB+=hist[t];
        if (wB) {
          const ulongT wF = size() - wB;
          if (!wF) break;
          sumB+=t*hist[t];
          const double
            mB = (double)sumB/wB,
            mF = (double)(sum - sumB)/wF,
            variance = wB*wF*cimg::sqr(mB - mF);
          if (variance>best_variance) { best_variance = variance; best_t = t; }
        }
      }
      return m + best_t*(M - m)/(hist.width() - 1);
    }

    //! Threshold pixel values.
    /**
       \param value Threshold value
       \param soft_threshold Tells if soft thresholding must be applied (instead of hard one).
       \param strict_threshold Tells if threshold value is strict.
       \par Example
       \code
       const CImg<float> img("reference.jpg"), res = img.get_threshold(128);
       (img,res.normalize(0,255)).display();
       \endcode
       \image html ref_threshold.jpg
    **/
    CImg<T>& threshold(const T& value, const bool soft_threshold=false, const bool strict_threshold=false) {
      if (is_empty()) return *this;
      if (strict_threshold) {
        if (soft_threshold)
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32768))
          cimg_rofoff(*this,off) {
            const T v = _data[off];
            _data[off] = v>value?(T)(v-value):v<-(float)value?(T)(v + value):(T)0;
          }
        else
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),65536))
          cimg_rofoff(*this,off) _data[off] = _data[off]>value?(T)1:(T)0;
      } else {
        if (soft_threshold)
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32768))
          cimg_rofoff(*this,off) {
            const T v = _data[off];
            _data[off] = v>=value?(T)(v-value):v<=-(float)value?(T)(v + value):(T)0;
          }
        else
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),65536))
          cimg_rofoff(*this,off) _data[off] = _data[off]>=value?(T)1:(T)0;
      }
      return *this;
    }

    //! Threshold pixel values \newinstance.
    CImg<T> get_threshold(const T& value, const bool soft_threshold=false, const bool strict_threshold=false) const {
      return (+*this).threshold(value,soft_threshold,strict_threshold);
    }

    //! Compute the histogram of pixel values.
    /**
       \param nb_levels Number of desired histogram levels.
       \param min_value Minimum pixel value considered for the histogram computation.
         All pixel values lower than \p min_value will not be counted.
       \param max_value Maximum pixel value considered for the histogram computation.
         All pixel values higher than \p max_value will not be counted.
       \note
       - The histogram H of an image I is the 1D function where H(x) counts the number of occurrences of the value x
         in the image I.
       - The resulting histogram is always defined in 1D. Histograms of multi-valued images are not multi-dimensional.
       \par Example
       \code
       const CImg<float> img = CImg<float>("reference.jpg").histogram(256);
       img.display_graph(0,3);
       \endcode
       \image html ref_histogram.jpg
    **/
    CImg<T>& histogram(const unsigned int nb_levels, const T& min_value, const T& max_value) {
      return get_histogram(nb_levels,min_value,max_value).move_to(*this);
    }

    //! Compute the histogram of pixel values \overloading.
    CImg<T>& histogram(const unsigned int nb_levels) {
      return get_histogram(nb_levels).move_to(*this);
    }

    //! Compute the histogram of pixel values \newinstance.
    CImg<ulongT> get_histogram(const unsigned int nb_levels, const T& min_value, const T& max_value) const {
      if (!nb_levels || is_empty()) return CImg<ulongT>();
      const double
        vmin = (double)(min_value<max_value?min_value:max_value),
        vmax = (double)(min_value<max_value?max_value:min_value);
      CImg<ulongT> res(nb_levels,1,1,1,0);
      cimg_rof(*this,ptrs,T) {
        const T val = *ptrs;
        if (val>=vmin && val<=vmax) ++res[val==vmax?nb_levels - 1:(unsigned int)((val - vmin)*nb_levels/(vmax - vmin))];
      }
      return res;
    }

    //! Compute the histogram of pixel values \newinstance.
    CImg<ulongT> get_histogram(const unsigned int nb_levels) const {
      if (!nb_levels || is_empty()) return CImg<ulongT>();
      T vmax = 0, vmin = min_max(vmax);
      return get_histogram(nb_levels,vmin,vmax);
    }

    //! Equalize histogram of pixel values.
    /**
       \param nb_levels Number of histogram levels used for the equalization.
       \param min_value Minimum pixel value considered for the histogram computation.
         All pixel values lower than \p min_value will not be counted.
       \param max_value Maximum pixel value considered for the histogram computation.
         All pixel values higher than \p max_value will not be counted.
       \par Example
       \code
       const CImg<float> img("reference.jpg"), res = img.get_equalize(256,0,255);
       (img,res).display();
       \endcode
       \image html ref_equalize.jpg
    **/
    CImg<T>& equalize(const unsigned int nb_levels, const T& min_value, const T& max_value) {
      if (!nb_levels || is_empty()) return *this;
      const T
        vmin = min_value<max_value?min_value:max_value,
        vmax = min_value<max_value?max_value:min_value;
      CImg<ulongT> hist = get_histogram(nb_levels,vmin,vmax);
      ulongT cumul = 0;
      cimg_forX(hist,pos) { cumul+=hist[pos]; hist[pos] = cumul; }
      if (!cumul) cumul = 1;
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),1048576))
      cimg_rofoff(*this,off) {
        const int pos = (int)((_data[off] - vmin)*(nb_levels - 1.)/(vmax - vmin));
        if (pos>=0 && pos<(int)nb_levels) _data[off] = (T)(vmin + (vmax - vmin)*hist[pos]/cumul);
      }
      return *this;
    }

    //! Equalize histogram of pixel values \overloading.
    CImg<T>& equalize(const unsigned int nb_levels) {
      if (!nb_levels || is_empty()) return *this;
      T vmax = 0, vmin = min_max(vmax);
      return equalize(nb_levels,vmin,vmax);
    }

    //! Equalize histogram of pixel values \newinstance.
    CImg<T> get_equalize(const unsigned int nblevels, const T& val_min, const T& val_max) const {
      return (+*this).equalize(nblevels,val_min,val_max);
    }

    //! Equalize histogram of pixel values \newinstance.
    CImg<T> get_equalize(const unsigned int nblevels) const {
      return (+*this).equalize(nblevels);
    }

    //! Index multi-valued pixels regarding to a specified palette.
    /**
       \param colormap Multi-valued colormap used as the basis for multi-valued pixel indexing.
       \param dithering Level of dithering (0=disable, 1=standard level).
       \param map_colors Tell if the values of the resulting image are the colormap indices or the colormap vectors.
       \note
       - \p img.index(colormap,dithering,1) is equivalent to <tt>img.index(colormap,dithering,0).map(colormap)</tt>.
       \par Example
       \code
       const CImg<float> img("reference.jpg"), colormap(3,1,1,3, 0,128,255, 0,128,255, 0,128,255);
       const CImg<float> res = img.get_index(colormap,1,true);
       (img,res).display();
       \endcode
       \image html ref_index.jpg
    **/
    template<typename t>
    CImg<T>& index(const CImg<t>& colormap, const float dithering=1, const bool map_colors=false) {
      return get_index(colormap,dithering,map_colors).move_to(*this);
    }

    //! Index multi-valued pixels regarding to a specified colormap \newinstance.
    template<typename t>
    CImg<typename CImg<t>::Tuint>
    get_index(const CImg<t>& colormap, const float dithering=1, const bool map_colors=true) const {
      if (colormap._spectrum!=_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "index(): Instance and specified colormap (%u,%u,%u,%u,%p) "
                                    "have incompatible dimensions.",
                                    cimg_instance,
                                    colormap._width,colormap._height,colormap._depth,colormap._spectrum,colormap._data);

      typedef typename CImg<t>::Tuint tuint;
      if (is_empty()) return CImg<tuint>();
      const ulongT
        whd = (ulongT)_width*_height*_depth,
        pwhd = (ulongT)colormap._width*colormap._height*colormap._depth;
      CImg<tuint> res(_width,_height,_depth,map_colors?_spectrum:1);
      if (dithering>0) { // Dithered versions
        tuint *ptrd = res._data;
        const float ndithering = cimg::cut(dithering,0.f,1.f)/16;
        Tfloat valm = 0, valM = (Tfloat)max_min(valm);
        if (valm==valM && valm>=0 && valM<=255) { valm = 0; valM = 255; }
        CImg<Tfloat> cache = get_crop(-1,0,0,0,_width,1,0,_spectrum - 1);
        Tfloat *cache_current = cache.data(1,0,0,0), *cache_next = cache.data(1,1,0,0);
        const ulongT cwhd = (ulongT)cache._width*cache._height*cache._depth;
        switch (_spectrum) {
        case 1 : { // Optimized for scalars
          cimg_forYZ(*this,y,z) {
            if (y<height() - 2) {
              Tfloat *ptrc0 = cache_next; const T *ptrs0 = data(0,y + 1,z,0);
              cimg_forX(*this,x) *(ptrc0++) = (Tfloat)*(ptrs0++);
            }
            Tfloat *ptrs0 = cache_current, *ptrsn0 = cache_next;
            cimg_forX(*this,x) {
              const Tfloat _val0 = (Tfloat)*ptrs0, val0 = _val0<valm?valm:_val0>valM?valM:_val0;
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp_end = ptrp0 + pwhd; ptrp0<ptrp_end; ) {
                const Tfloat pval0 = (Tfloat)*(ptrp0++) - val0, dist = pval0*pval0;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              const Tfloat err0 = ((*(ptrs0++)=val0) - (Tfloat)*ptrmin0)*ndithering;
              *ptrs0+=7*err0; *(ptrsn0 - 1)+=3*err0; *(ptrsn0++)+=5*err0; *ptrsn0+=err0;
              if (map_colors) *(ptrd++) = (tuint)*ptrmin0; else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
            cimg::swap(cache_current,cache_next);
          }
        } break;
        case 2 : { // Optimized for 2D vectors
          tuint *ptrd1 = ptrd + whd;
          cimg_forYZ(*this,y,z) {
            if (y<height() - 2) {
              Tfloat *ptrc0 = cache_next, *ptrc1 = ptrc0 + cwhd;
              const T *ptrs0 = data(0,y + 1,z,0), *ptrs1 = ptrs0 + whd;
              cimg_forX(*this,x) { *(ptrc0++) = (Tfloat)*(ptrs0++); *(ptrc1++) = (Tfloat)*(ptrs1++); }
            }
            Tfloat
              *ptrs0 = cache_current, *ptrs1 = ptrs0 + cwhd,
              *ptrsn0 = cache_next, *ptrsn1 = ptrsn0 + cwhd;
            cimg_forX(*this,x) {
              const Tfloat
                _val0 = (Tfloat)*ptrs0, val0 = _val0<valm?valm:_val0>valM?valM:_val0,
                _val1 = (Tfloat)*ptrs1, val1 = _val1<valm?valm:_val1>valM?valM:_val1;
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp1 = ptrp0 + pwhd, *ptrp_end = ptrp1; ptrp0<ptrp_end; ) {
                const Tfloat
                  pval0 = (Tfloat)*(ptrp0++) - val0, pval1 = (Tfloat)*(ptrp1++) - val1,
                  dist = pval0*pval0 + pval1*pval1;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              const t *const ptrmin1 = ptrmin0 + pwhd;
              const Tfloat
                err0 = ((*(ptrs0++)=val0) - (Tfloat)*ptrmin0)*ndithering,
                err1 = ((*(ptrs1++)=val1) - (Tfloat)*ptrmin1)*ndithering;
              *ptrs0+=7*err0; *ptrs1+=7*err1;
              *(ptrsn0 - 1)+=3*err0; *(ptrsn1 - 1)+=3*err1;
              *(ptrsn0++)+=5*err0; *(ptrsn1++)+=5*err1;
              *ptrsn0+=err0; *ptrsn1+=err1;
              if (map_colors) { *(ptrd++) = (tuint)*ptrmin0; *(ptrd1++) = (tuint)*ptrmin1; }
              else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
            cimg::swap(cache_current,cache_next);
          }
        } break;
        case 3 : { // Optimized for 3D vectors (colors)
          tuint *ptrd1 = ptrd + whd, *ptrd2 = ptrd1 + whd;
          cimg_forYZ(*this,y,z) {
            if (y<height() - 2) {
              Tfloat *ptrc0 = cache_next, *ptrc1 = ptrc0 + cwhd, *ptrc2 = ptrc1 + cwhd;
              const T *ptrs0 = data(0,y + 1,z,0), *ptrs1 = ptrs0 + whd, *ptrs2 = ptrs1 + whd;
              cimg_forX(*this,x) {
                *(ptrc0++) = (Tfloat)*(ptrs0++); *(ptrc1++) = (Tfloat)*(ptrs1++); *(ptrc2++) = (Tfloat)*(ptrs2++);
              }
            }
            Tfloat
              *ptrs0 = cache_current, *ptrs1 = ptrs0 + cwhd, *ptrs2 = ptrs1 + cwhd,
              *ptrsn0 = cache_next, *ptrsn1 = ptrsn0 + cwhd, *ptrsn2 = ptrsn1 + cwhd;
            cimg_forX(*this,x) {
              const Tfloat
                _val0 = (Tfloat)*ptrs0, val0 = _val0<valm?valm:_val0>valM?valM:_val0,
                _val1 = (Tfloat)*ptrs1, val1 = _val1<valm?valm:_val1>valM?valM:_val1,
                _val2 = (Tfloat)*ptrs2, val2 = _val2<valm?valm:_val2>valM?valM:_val2;
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp1 = ptrp0 + pwhd, *ptrp2 = ptrp1 + pwhd,
                     *ptrp_end = ptrp1; ptrp0<ptrp_end; ) {
                const Tfloat
                  pval0 = (Tfloat)*(ptrp0++) - val0,
                  pval1 = (Tfloat)*(ptrp1++) - val1,
                  pval2 = (Tfloat)*(ptrp2++) - val2,
                  dist = pval0*pval0 + pval1*pval1 + pval2*pval2;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              const t *const ptrmin1 = ptrmin0 + pwhd, *const ptrmin2 = ptrmin1 + pwhd;
              const Tfloat
                err0 = ((*(ptrs0++)=val0) - (Tfloat)*ptrmin0)*ndithering,
                err1 = ((*(ptrs1++)=val1) - (Tfloat)*ptrmin1)*ndithering,
                err2 = ((*(ptrs2++)=val2) - (Tfloat)*ptrmin2)*ndithering;

              *ptrs0+=7*err0; *ptrs1+=7*err1; *ptrs2+=7*err2;
              *(ptrsn0 - 1)+=3*err0; *(ptrsn1 - 1)+=3*err1; *(ptrsn2 - 1)+=3*err2;
              *(ptrsn0++)+=5*err0; *(ptrsn1++)+=5*err1; *(ptrsn2++)+=5*err2;
              *ptrsn0+=err0; *ptrsn1+=err1; *ptrsn2+=err2;

              if (map_colors) {
                *(ptrd++) = (tuint)*ptrmin0; *(ptrd1++) = (tuint)*ptrmin1; *(ptrd2++) = (tuint)*ptrmin2;
              } else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
            cimg::swap(cache_current,cache_next);
          }
        } break;
        default : // Generic version
          cimg_forYZ(*this,y,z) {
            if (y<height() - 2) {
              Tfloat *ptrc = cache_next;
              cimg_forC(*this,c) {
                Tfloat *_ptrc = ptrc; const T *_ptrs = data(0,y + 1,z,c);
                cimg_forX(*this,x) *(_ptrc++) = (Tfloat)*(_ptrs++);
                ptrc+=cwhd;
              }
            }
            Tfloat *ptrs = cache_current, *ptrsn = cache_next;
            cimg_forX(*this,x) {
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin = colormap._data;
              for (const t *ptrp = colormap._data, *ptrp_end = ptrp + pwhd; ptrp<ptrp_end; ++ptrp) {
                Tfloat dist = 0; Tfloat *_ptrs = ptrs; const t *_ptrp = ptrp;
                cimg_forC(*this,c) {
                  const Tfloat _val = *_ptrs, val = _val<valm?valm:_val>valM?valM:_val;
                  dist+=cimg::sqr((*_ptrs=val) - (Tfloat)*_ptrp); _ptrs+=cwhd; _ptrp+=pwhd;
                }
                if (dist<distmin) { ptrmin = ptrp; distmin = dist; }
              }
              const t *_ptrmin = ptrmin; Tfloat *_ptrs = ptrs++, *_ptrsn = (ptrsn++) - 1;
              cimg_forC(*this,c) {
                const Tfloat err = (*(_ptrs++) - (Tfloat)*_ptrmin)*ndithering;
                *_ptrs+=7*err; *(_ptrsn++)+=3*err; *(_ptrsn++)+=5*err; *_ptrsn+=err;
                _ptrmin+=pwhd; _ptrs+=cwhd - 1; _ptrsn+=cwhd - 2;
              }
              if (map_colors) {
                tuint *_ptrd = ptrd++;
                cimg_forC(*this,c) { *_ptrd = (tuint)*ptrmin; _ptrd+=whd; ptrmin+=pwhd; }
              }
              else *(ptrd++) = (tuint)(ptrmin - colormap._data);
            }
            cimg::swap(cache_current,cache_next);
          }
        }
      } else { // Non-dithered versions
        switch (_spectrum) {
        case 1 : { // Optimized for scalars
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64 &&
                                                                     _height*_depth>=16 && pwhd>=16))
          cimg_forYZ(*this,y,z) {
            tuint *ptrd = res.data(0,y,z);
            for (const T *ptrs0 = data(0,y,z), *ptrs_end = ptrs0 + _width; ptrs0<ptrs_end; ) {
              const Tfloat val0 = (Tfloat)*(ptrs0++);
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp_end = ptrp0 + pwhd; ptrp0<ptrp_end; ) {
                const Tfloat pval0 = (Tfloat)*(ptrp0++) - val0, dist = pval0*pval0;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              if (map_colors) *(ptrd++) = (tuint)*ptrmin0; else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
          }
        } break;
        case 2 : { // Optimized for 2D vectors
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64 &&
                                                                     _height*_depth>=16 && pwhd>=16))
          cimg_forYZ(*this,y,z) {
            tuint *ptrd = res.data(0,y,z), *ptrd1 = ptrd + whd;
            for (const T *ptrs0 = data(0,y,z), *ptrs1 = ptrs0 + whd, *ptrs_end = ptrs0 + _width; ptrs0<ptrs_end; ) {
              const Tfloat val0 = (Tfloat)*(ptrs0++), val1 = (Tfloat)*(ptrs1++);
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp1 = ptrp0 + pwhd, *ptrp_end = ptrp1; ptrp0<ptrp_end; ) {
                const Tfloat
                  pval0 = (Tfloat)*(ptrp0++) - val0, pval1 = (Tfloat)*(ptrp1++) - val1,
                  dist = pval0*pval0 + pval1*pval1;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              if (map_colors) { *(ptrd++) = (tuint)*ptrmin0; *(ptrd1++) = (tuint)*(ptrmin0 + pwhd); }
              else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
          }
        } break;
        case 3 : { // Optimized for 3D vectors (colors)
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64 &&
                                                                     _height*_depth>=16 && pwhd>=16))
          cimg_forYZ(*this,y,z) {
            tuint *ptrd = res.data(0,y,z), *ptrd1 = ptrd + whd, *ptrd2 = ptrd1 + whd;
            for (const T *ptrs0 = data(0,y,z), *ptrs1 = ptrs0 + whd, *ptrs2 = ptrs1 + whd,
                   *ptrs_end = ptrs0 + _width; ptrs0<ptrs_end; ) {
              const Tfloat val0 = (Tfloat)*(ptrs0++), val1 = (Tfloat)*(ptrs1++), val2 = (Tfloat)*(ptrs2++);
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin0 = colormap._data;
              for (const t *ptrp0 = colormap._data, *ptrp1 = ptrp0 + pwhd, *ptrp2 = ptrp1 + pwhd,
                     *ptrp_end = ptrp1; ptrp0<ptrp_end; ) {
                const Tfloat
                  pval0 = (Tfloat)*(ptrp0++) - val0,
                  pval1 = (Tfloat)*(ptrp1++) - val1,
                  pval2 = (Tfloat)*(ptrp2++) - val2,
                  dist = pval0*pval0 + pval1*pval1 + pval2*pval2;
                if (dist<distmin) { ptrmin0 = ptrp0 - 1; distmin = dist; }
              }
              if (map_colors) {
                *(ptrd++) = (tuint)*ptrmin0;
                *(ptrd1++) = (tuint)*(ptrmin0 + pwhd);
                *(ptrd2++) = (tuint)*(ptrmin0 + 2*pwhd);
              } else *(ptrd++) = (tuint)(ptrmin0 - colormap._data);
            }
          }
        } break;
        default : // Generic version
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64 &&
                                                                     _height*_depth>=16 && pwhd>=16))
          cimg_forYZ(*this,y,z) {
            tuint *ptrd = res.data(0,y,z);
            for (const T *ptrs = data(0,y,z), *ptrs_end = ptrs + _width; ptrs<ptrs_end; ++ptrs) {
              Tfloat distmin = cimg::type<Tfloat>::max(); const t *ptrmin = colormap._data;
              for (const t *ptrp = colormap._data, *ptrp_end = ptrp + pwhd; ptrp<ptrp_end; ++ptrp) {
                Tfloat dist = 0; const T *_ptrs = ptrs; const t *_ptrp = ptrp;
                cimg_forC(*this,c) { dist+=cimg::sqr((Tfloat)*_ptrs - (Tfloat)*_ptrp); _ptrs+=whd; _ptrp+=pwhd; }
                if (dist<distmin) { ptrmin = ptrp; distmin = dist; }
              }
              if (map_colors) {
                tuint *_ptrd = ptrd++;
                cimg_forC(*this,c) { *_ptrd = (tuint)*ptrmin; _ptrd+=whd; ptrmin+=pwhd; }
              }
              else *(ptrd++) = (tuint)(ptrmin - colormap._data);
            }
          }
        }
      }
      return res;
    }

    //! Map predefined palette on the scalar (indexed) image instance.
    /**
       \param palette Multi-valued palette used for mapping the indexes.
       \param boundary_conditions Boundary conditions.
         Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
       \par Example
       \code
       const CImg<float> img("reference.jpg"),
                         palette1(3,1,1,3, 0,128,255, 0,128,255, 0,128,255),
                         palette2(3,1,1,3, 255,0,0, 0,255,0, 0,0,255),
                         res = img.get_index(palette1,0).map(palette2);
       (img,res).display();
       \endcode
       \image html ref_map.jpg
    **/
    template<typename t>
    CImg<T>& map(const CImg<t>& palette, const unsigned int boundary_conditions=0) {
      return get_map(palette,boundary_conditions).move_to(*this);
    }

    //! Map predefined palette on the scalar (indexed) image instance \newinstance.
    template<typename t>
    CImg<t> get_map(const CImg<t>& palette, const unsigned int boundary_conditions=0) const {
      const ulongT
        whd = (ulongT)_width*_height*_depth, siz = size(),
        cwhd = (ulongT)palette._width*palette._height*palette._depth,
        cwhd2 = 2*cwhd;
      CImg<t> res(_width,_height,_depth,_spectrum*palette._spectrum);
      switch (palette._spectrum) {

      case 1 : { // Optimized for scalars
        switch (boundary_conditions) {
        case 3 : // Mirror
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
          for (longT off = 0; off<(longT)siz; ++off) {
            const ulongT ind = ((ulongT)_data[off])%cwhd2;
            res[off] = palette[ind<cwhd?ind:cwhd2 - ind - 1];
          }
          break;
        case 2 : // Periodic
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
          for (longT off = 0; off<(longT)siz; ++off) {
            const ulongT ind = (ulongT)_data[off];
            res[off] = palette[ind%cwhd];
          }
          break;
        case 1 : // Neumann
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
          for (longT off = 0; off<(longT)siz; ++off) {
            const longT ind = (longT)_data[off];
            res[off] = palette[cimg::cut(ind,(longT)0,(longT)cwhd - 1)];
          } break;
        default : // Dirichlet
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
          for (longT off = 0; off<(longT)siz; ++off) {
            const ulongT ind = (ulongT)_data[off];
            res[off] = ind<cwhd?palette[ind]:(t)0;
          }
        }
      } break;

      case 2 : { // Optimized for 2D vectors
        const t *const ptrp0 = palette._data, *const ptrp1 = ptrp0 + cwhd;
        switch (boundary_conditions) {
        case 3 : // Mirror
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,2*c), *const ptrd1 = ptrd0 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT
                _ind = ((ulongT)ptrs[off])%cwhd2,
                ind = _ind<cwhd?_ind:cwhd2 - _ind - 1;
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind];
            }
          } break;
        case 2 : // Periodic
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,2*c), *const ptrd1 = ptrd0 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = ((ulongT)ptrs[off])%cwhd;
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind];
            }
          } break;
        case 1 : // Neumann
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,2*c), *const ptrd1 = ptrd0 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const longT ind = cimg::cut((longT)ptrs[off],(longT)0,(longT)cwhd - 1);
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind];
            }
          } break;
        default : // Dirichlet
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,2*c), *const ptrd1 = ptrd0 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = (ulongT)ptrs[off];
              if (ind<cwhd) { ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; }
              else ptrd0[off] = ptrd1[off] = (t)0;
            }
          }
        }
      } break;

      case 3 : { // Optimized for 3D vectors (colors)
        const t *const ptrp0 = palette._data, *ptrp1 = ptrp0 + cwhd, *ptrp2 = ptrp0 + 2*cwhd;
        switch (boundary_conditions) {
        case 3 : // Mirror
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,3*c), *const ptrd1 = ptrd0 + whd, *const ptrd2 = ptrd1 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT
                _ind = ((ulongT)ptrs[off])%cwhd2,
                ind = _ind<cwhd?_ind:cwhd2 - _ind - 1;
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; ptrd2[off] = ptrp2[ind];
            }
          } break;
        case 2 : // Periodic
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,3*c), *const ptrd1 = ptrd0 + whd, *const ptrd2 = ptrd1 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = ((ulongT)ptrs[off])%cwhd;
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; ptrd2[off] = ptrp2[ind];
            }
          } break;
        case 1 : // Neumann
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,3*c), *const ptrd1 = ptrd0 + whd, *const ptrd2 = ptrd1 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const longT ind = cimg::cut((longT)ptrs[off],(longT)0,(longT)cwhd - 1);
              ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; ptrd2[off] = ptrp2[ind];
            }
          } break;
        default : // Dirichlet
          cimg_forC(*this,c) {
            t *const ptrd0 = res.data(0,0,0,3*c), *const ptrd1 = ptrd0 + whd, *const ptrd2 = ptrd1 + whd;
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = (ulongT)ptrs[off];
              if (ind<cwhd) { ptrd0[off] = ptrp0[ind]; ptrd1[off] = ptrp1[ind]; ptrd2[off] = ptrp2[ind]; }
              else ptrd0[off] = ptrd1[off] = ptrd2[off] = (t)0;
            }
          }
        }
      } break;

      default : { // Generic version
        switch (boundary_conditions) {
        case 3 : // Mirror
          cimg_forC(*this,c) {
            t *const ptrd = res.data(0,0,0,palette._spectrum*c);
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT
                _ind = ((ulongT)ptrs[off])%cwhd,
                ind = _ind<cwhd?_ind:cwhd2 - _ind - 1;
              t *const _ptrd = ptrd + off;
              const t *const ptrp = &palette[ind];
              cimg_forC(palette,k) _ptrd[k*whd] = ptrp[k*cwhd];
            }
          } break;
        case 2 : // Periodic
          cimg_forC(*this,c) {
            t *const ptrd = res.data(0,0,0,palette._spectrum*c);
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = ((ulongT)ptrs[off])%cwhd;
              t *const _ptrd = ptrd + off;
              const t *const ptrp = &palette[ind];
              cimg_forC(palette,k) _ptrd[k*whd] = ptrp[k*cwhd];
            }
          } break;
        case 1 : // Neumann
          cimg_forC(*this,c) {
            t *const ptrd = res.data(0,0,0,palette._spectrum*c);
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const longT ind = cimg::cut((longT)ptrs[off],(longT)0,(longT)cwhd - 1);
              t *const _ptrd = ptrd + off;
              const t *const ptrp = &palette[ind];
              cimg_forC(palette,k) _ptrd[k*whd] = ptrp[k*cwhd];
            }
          } break;
        default : // Dirichlet
          cimg_forC(*this,c) {
            t *const ptrd = res.data(0,0,0,palette._spectrum*c);
            const T *const ptrs = data(0,0,0,c);
            cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),256))
            for (longT off = 0; off<(longT)whd; ++off) {
              const ulongT ind = (ulongT)ptrs[off];
              t *const _ptrd = ptrd + off;
              if (ind<cwhd) {
                const t *const ptrp = &palette[ind];
                cimg_forC(palette,k) _ptrd[k*whd] = ptrp[k*cwhd];
              } else cimg_forC(palette,k) _ptrd[k*whd] = (t)0;
            }
          }
        }
      }
      }
      return res;
    }

    //! Label connected components.
    /**
       \param is_high_connectivity Boolean that choose between 4(false)- or 8(true)-connectivity
       in 2D case, and between 6(false)- or 26(true)-connectivity in 3D case.
       \param tolerance Tolerance used to determine if two neighboring pixels belong to the same region.
       \param is_L2_norm If true, tolerance is compared against L2 difference, otherwise L1 is used.
       \note The algorithm of connected components computation has been primarily done
       by A. Meijster, according to the publication:
       'W.H. Hesselink, A. Meijster, C. Bron, "Concurrent Determination of Connected Components.",
       In: Science of Computer Programming 41 (2001), pp. 173--194'.
       The submitted code has then been modified to fit CImg coding style and constraints.
    **/
    CImg<T>& label(const bool is_high_connectivity=false, const Tfloat tolerance=0,
                   const bool is_L2_norm=true) {
      if (is_empty()) return *this;
      return get_label(is_high_connectivity,tolerance,is_L2_norm).move_to(*this);
    }

    //! Label connected components \newinstance.
    CImg<ulongT> get_label(const bool is_high_connectivity=false, const Tfloat tolerance=0,
                           const bool is_L2_norm=true) const {
      if (is_empty()) return CImg<ulongT>();

      // Create neighborhood tables.
      int dx[13], dy[13], dz[13], nb = 0;
      dx[nb] = 1; dy[nb] = 0; dz[nb++] = 0;
      dx[nb] = 0; dy[nb] = 1; dz[nb++] = 0;
      if (is_high_connectivity) {
        dx[nb] = 1; dy[nb] = 1; dz[nb++] = 0;
        dx[nb] = 1; dy[nb] = -1; dz[nb++] = 0;
      }
      if (_depth>1) { // 3D version
        dx[nb] = 0; dy[nb] = 0; dz[nb++]=1;
        if (is_high_connectivity) {
          dx[nb] = 1; dy[nb] = 1; dz[nb++] = -1;
          dx[nb] = 1; dy[nb] = 0; dz[nb++] = -1;
          dx[nb] = 1; dy[nb] = -1; dz[nb++] = -1;
          dx[nb] = 0; dy[nb] = 1; dz[nb++] = -1;

          dx[nb] = 0; dy[nb] = 1; dz[nb++] = 1;
          dx[nb] = 1; dy[nb] = -1; dz[nb++] = 1;
          dx[nb] = 1; dy[nb] = 0; dz[nb++] = 1;
          dx[nb] = 1; dy[nb] = 1; dz[nb++] = 1;
        }
      }
      return _label(nb,dx,dy,dz,tolerance,is_L2_norm);
    }

    //! Label connected components \overloading.
    /**
       \param connectivity_mask Mask of the neighboring pixels.
       \param tolerance Tolerance used to determine if two neighboring pixels belong to the same region.
       \param is_L2_norm If true, tolerance is compared against L2 difference, otherwise L1 is used.
    **/
    template<typename t>
    CImg<T>& label(const CImg<t>& connectivity_mask, const Tfloat tolerance=0,
                   const bool is_L2_norm=true) {
      if (is_empty()) return *this;
      return get_label(connectivity_mask,tolerance,is_L2_norm).move_to(*this);
    }

    //! Label connected components \newinstance.
    template<typename t>
    CImg<ulongT> get_label(const CImg<t>& connectivity_mask, const Tfloat tolerance=0,
                           const bool is_L2_norm=true) const {
      if (is_empty()) return CImg<ulongT>();
      int nb = 0;
      cimg_for(connectivity_mask,ptr,t) if (*ptr) ++nb;
      CImg<intT> dx(nb,1,1,1,0), dy(nb,1,1,1,0), dz(nb,1,1,1,0);
      nb = 0;
      cimg_forXYZ(connectivity_mask,x,y,z) if ((x || y || z) &&
                                               connectivity_mask(x,y,z)) {
        dx[nb] = x; dy[nb] = y; dz[nb++] = z;
      }
      return _label(nb,dx,dy,dz,tolerance,is_L2_norm);
    }

    CImg<ulongT> _label(const unsigned int nb, const int *const dx,
                        const int *const dy, const int *const dz,
                        const Tfloat tolerance, const bool is_L2_norm) const {
      CImg<ulongT> res(_width,_height,_depth);
      const Tfloat _tolerance = _spectrum>1 && is_L2_norm?cimg::sqr(tolerance):tolerance;

      // Init label numbers.
      ulongT *ptr = res.data();
      cimg_foroff(res,p) *(ptr++) = p;

      // For each neighbour-direction, label.
      for (unsigned int n = 0; n<nb; ++n) {
        const int _dx = dx[n], _dy = dy[n], _dz = dz[n];
        if (_dx || _dy || _dz) {
          const int
            x0 = _dx<0?-_dx:0,
            x1 = _dx<0?width():width() - _dx,
            y0 = _dy<0?-_dy:0,
            y1 = _dy<0?height():height() - _dy,
            z0 = _dz<0?-_dz:0,
            z1 = _dz<0?depth():depth() - _dz;
          const longT
            wh = (longT)width()*height(),
            whd = (longT)width()*height()*depth(),
            offset = _dz*wh + _dy*width() + _dx;
          for (longT z = z0, nz = z0 + _dz, pz = z0*wh; z<z1; ++z, ++nz, pz+=wh) {
            for (longT y = y0, ny = y0 + _dy, py = y0*width() + pz; y<y1; ++y, ++ny, py+=width()) {
              for (longT x = x0, nx = x0 + _dx, p = x0 + py; x<x1; ++x, ++nx, ++p) {
                Tfloat diff;
                switch (_spectrum) {
                case 1 :
                  diff = cimg::abs((Tfloat)(*this)(x,y,z,0,wh,whd) - (Tfloat)(*this)(nx,ny,nz,0,wh,whd));
                  break;
                case 2 :
                  if (is_L2_norm)
                    diff = cimg::sqr((Tfloat)(*this)(x,y,z,0,wh,whd) - (Tfloat)(*this)(nx,ny,nz,0,wh,whd)) +
                      cimg::sqr((Tfloat)(*this)(x,y,z,1,wh,whd) - (Tfloat)(*this)(nx,ny,nz,1,wh,whd));
                  else
                    diff = cimg::abs((Tfloat)(*this)(x,y,z,0,wh,whd) - (Tfloat)(*this)(nx,ny,nz,0,wh,whd)) +
                      cimg::abs((Tfloat)(*this)(x,y,z,1,wh,whd) - (Tfloat)(*this)(nx,ny,nz,1,wh,whd));
                  break;
                case 3 :
                  if (is_L2_norm)
                    diff = cimg::sqr((Tfloat)(*this)(x,y,z,0,wh,whd) - (Tfloat)(*this)(nx,ny,nz,0,wh,whd)) +
                      cimg::sqr((Tfloat)(*this)(x,y,z,1,wh,whd) - (Tfloat)(*this)(nx,ny,nz,1,wh,whd)) +
                      cimg::sqr((Tfloat)(*this)(x,y,z,2,wh,whd) - (Tfloat)(*this)(nx,ny,nz,2,wh,whd));
                  else
                    diff = cimg::abs((Tfloat)(*this)(x,y,z,0,wh,whd) - (Tfloat)(*this)(nx,ny,nz,0,wh,whd)) +
                      cimg::abs((Tfloat)(*this)(x,y,z,1,wh,whd) - (Tfloat)(*this)(nx,ny,nz,1,wh,whd)) +
                      cimg::abs((Tfloat)(*this)(x,y,z,2,wh,whd) - (Tfloat)(*this)(nx,ny,nz,2,wh,whd));
                  break;
                case 4 :
                  if (is_L2_norm)
                    diff = cimg::sqr((Tfloat)(*this)(x,y,z,0,wh,whd) - (Tfloat)(*this)(nx,ny,nz,0,wh,whd)) +
                      cimg::sqr((Tfloat)(*this)(x,y,z,1,wh,whd) - (Tfloat)(*this)(nx,ny,nz,1,wh,whd)) +
                      cimg::sqr((Tfloat)(*this)(x,y,z,2,wh,whd) - (Tfloat)(*this)(nx,ny,nz,2,wh,whd)) +
                      cimg::sqr((Tfloat)(*this)(x,y,z,3,wh,whd) - (Tfloat)(*this)(nx,ny,nz,3,wh,whd));
                  else
                    diff = cimg::abs((Tfloat)(*this)(x,y,z,0,wh,whd) - (Tfloat)(*this)(nx,ny,nz,0,wh,whd)) +
                      cimg::abs((Tfloat)(*this)(x,y,z,1,wh,whd) - (Tfloat)(*this)(nx,ny,nz,1,wh,whd)) +
                      cimg::abs((Tfloat)(*this)(x,y,z,2,wh,whd) - (Tfloat)(*this)(nx,ny,nz,2,wh,whd)) +
                      cimg::abs((Tfloat)(*this)(x,y,z,3,wh,whd) - (Tfloat)(*this)(nx,ny,nz,3,wh,whd));
                  break;
                default :
                  diff = 0;
                  if (is_L2_norm)
                    cimg_forC(*this,c)
                      diff+=cimg::sqr((Tfloat)(*this)(x,y,z,c,wh,whd) - (Tfloat)(*this)(nx,ny,nz,c,wh,whd));
                  else
                    cimg_forC(*this,c)
                      diff+=cimg::abs((Tfloat)(*this)(x,y,z,c,wh,whd) - (Tfloat)(*this)(nx,ny,nz,c,wh,whd));
                }

                if (diff<=_tolerance) {
                  const longT q = p + offset;
                  ulongT xk, yk;
                  for (xk = (ulongT)(p<q?q:p), yk = (ulongT)(p<q?p:q); xk!=yk && res[xk]!=xk; ) {
                    xk = res[xk]; if (xk<yk) cimg::swap(xk,yk);
                  }
                  if (xk!=yk) res[xk] = (ulongT)yk;
                  for (ulongT _p = (ulongT)p; _p!=yk; ) {
                    const ulongT h = res[_p];
                    res[_p] = (ulongT)yk;
                    _p = h;
                  }
                  for (ulongT _q = (ulongT)q; _q!=yk; ) {
                    const ulongT h = res[_q];
                    res[_q] = (ulongT)yk;
                    _q = h;
                  }
                }
              }
            }
          }
        }
      }

      // Resolve equivalences.
      ulongT counter = 0;
      ptr = res.data();
      cimg_foroff(res,p) { *ptr = *ptr==p?counter++:res[*ptr]; ++ptr; }
      return res;
    }

    // [internal] Replace possibly malicious characters for commands to be called by system() by their escaped version.
    CImg<T>& _system_strescape() {
#define cimg_system_strescape(c,s) case c : if (p!=ptrs) CImg<T>(ptrs,(unsigned int)(p-ptrs),1,1,1,false).\
      move_to(list); \
      CImg<T>(s,(unsigned int)std::strlen(s),1,1,1,false).move_to(list); ptrs = p + 1; break
      CImgList<T> list;
      const T *ptrs = _data;
      cimg_for(*this,p,T) switch ((int)*p) {
        cimg_system_strescape('\\',"\\\\");
        cimg_system_strescape('\"',"\\\"");
        cimg_system_strescape('!',"\"\\!\"");
        cimg_system_strescape('`',"\\`");
        cimg_system_strescape('$',"\\$");
      }
      if (ptrs<end()) CImg<T>(ptrs,(unsigned int)(end()-ptrs),1,1,1,false).move_to(list);
      return (list>'x').move_to(*this);
    }

    //@}
#endif
