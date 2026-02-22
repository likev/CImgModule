#ifndef CIMG_MODULE_IMAGE_VALUE_OPS_H
#define CIMG_MODULE_IMAGE_VALUE_OPS_H

  template<typename T>
  CImg<T>& CImg<T>::set_linear_atX(const T& value, const float fx, const int y, const int z, const int c,
                                   const bool is_added) {
    const int
      x = (int)fx - (fx>=0?0:1), nx = x + 1;
    const float
      dx = fx - x;
    if (y>=0 && y<height() && z>=0 && z<depth() && c>=0 && c<spectrum()) {
      if (x>=0 && x<width()) {
        const float w1 = 1 - dx, w2 = is_added?1:(1 - w1);
        (*this)(x,y,z,c) = (T)(w1*value + w2*(*this)(x,y,z,c));
      }
      if (nx>=0 && nx<width()) {
        const float w1 = dx, w2 = is_added?1:(1 - w1);
        (*this)(nx,y,z,c) = (T)(w1*value + w2*(*this)(nx,y,z,c));
      }
    }
    return *this;
  }

  template<typename T>
  CImg<T>& CImg<T>::set_linear_atXY(const T& value, const float fx, const float fy, const int z, const int c,
                                    const bool is_added) {
    const int
      x = (int)fx - (fx>=0?0:1), nx = x + 1,
      y = (int)fy - (fy>=0?0:1), ny = y + 1;
    const float
      dx = fx - x,
      dy = fy - y;
    if (z>=0 && z<depth() && c>=0 && c<spectrum()) {
      if (y>=0 && y<height()) {
        if (x>=0 && x<width()) {
          const float w1 = (1 - dx)*(1 - dy), w2 = is_added?1:(1 - w1);
          (*this)(x,y,z,c) = (T)(w1*value + w2*(*this)(x,y,z,c));
        }
        if (nx>=0 && nx<width()) {
          const float w1 = dx*(1 - dy), w2 = is_added?1:(1 - w1);
          (*this)(nx,y,z,c) = (T)(w1*value + w2*(*this)(nx,y,z,c));
        }
      }
      if (ny>=0 && ny<height()) {
        if (x>=0 && x<width()) {
          const float w1 = (1 - dx)*dy, w2 = is_added?1:(1 - w1);
          (*this)(x,ny,z,c) = (T)(w1*value + w2*(*this)(x,ny,z,c));
        }
        if (nx>=0 && nx<width()) {
          const float w1 = dx*dy, w2 = is_added?1:(1 - w1);
          (*this)(nx,ny,z,c) = (T)(w1*value + w2*(*this)(nx,ny,z,c));
        }
      }
    }
    return *this;
  }

  template<typename T>
  CImg<T>& CImg<T>::set_linear_atXYZ(const T& value, const float fx, const float fy, const float fz, const int c,
                                     const bool is_added) {
    const int
      x = (int)fx - (fx>=0?0:1), nx = x + 1,
      y = (int)fy - (fy>=0?0:1), ny = y + 1,
      z = (int)fz - (fz>=0?0:1), nz = z + 1;
    const float
      dx = fx - x,
      dy = fy - y,
      dz = fz - z;
    if (c>=0 && c<spectrum()) {
      if (z>=0 && z<depth()) {
        if (y>=0 && y<height()) {
          if (x>=0 && x<width()) {
            const float w1 = (1 - dx)*(1 - dy)*(1 - dz), w2 = is_added?1:(1 - w1);
            (*this)(x,y,z,c) = (T)(w1*value + w2*(*this)(x,y,z,c));
          }
          if (nx>=0 && nx<width()) {
            const float w1 = dx*(1 - dy)*(1 - dz), w2 = is_added?1:(1 - w1);
            (*this)(nx,y,z,c) = (T)(w1*value + w2*(*this)(nx,y,z,c));
          }
        }
        if (ny>=0 && ny<height()) {
          if (x>=0 && x<width()) {
            const float w1 = (1 - dx)*dy*(1 - dz), w2 = is_added?1:(1 - w1);
            (*this)(x,ny,z,c) = (T)(w1*value + w2*(*this)(x,ny,z,c));
          }
          if (nx>=0 && nx<width()) {
            const float w1 = dx*dy*(1 - dz), w2 = is_added?1:(1 - w1);
            (*this)(nx,ny,z,c) = (T)(w1*value + w2*(*this)(nx,ny,z,c));
          }
        }
      }
      if (nz>=0 && nz<depth()) {
        if (y>=0 && y<height()) {
          if (x>=0 && x<width()) {
            const float w1 = (1 - dx)*(1 - dy)*dz, w2 = is_added?1:(1 - w1);
            (*this)(x,y,nz,c) = (T)(w1*value + w2*(*this)(x,y,nz,c));
          }
          if (nx>=0 && nx<width()) {
            const float w1 = dx*(1 - dy)*dz, w2 = is_added?1:(1 - w1);
            (*this)(nx,y,nz,c) = (T)(w1*value + w2*(*this)(nx,y,nz,c));
          }
        }
        if (ny>=0 && ny<height()) {
          if (x>=0 && x<width()) {
            const float w1 = (1 - dx)*dy*dz, w2 = is_added?1:(1 - w1);
            (*this)(x,ny,nz,c) = (T)(w1*value + w2*(*this)(x,ny,nz,c));
          }
          if (nx>=0 && nx<width()) {
            const float w1 = dx*dy*dz, w2 = is_added?1:(1 - w1);
            (*this)(nx,ny,nz,c) = (T)(w1*value + w2*(*this)(nx,ny,nz,c));
          }
        }
      }
    }
    return *this;
  }

  template<typename T>
  auto CImg<T>::value_string(const char separator, const unsigned int max_size, const char *const format) const
    -> CImg<charT> {
    if (is_empty() || max_size==1) return CImg<charT>(1,1,1,1,0);
    CImgList<charT> items;
    CImg<charT> s_item(256); *s_item = 0;
    const T *ptrs = _data;
    unsigned int string_size = 0;
    const char *const _format = format?format:cimg::type<T>::format();
    for (ulongT off = 0, siz = size(); off<siz && (!max_size || string_size<max_size); ++off) {
      const unsigned int printed_size = 1U + cimg_snprintf(s_item,s_item._width,_format,
                                                           cimg::type<T>::format(*(ptrs++)));
      CImg<charT> item(s_item._data,printed_size);
      item[printed_size - 1] = separator;
      item.move_to(items);
      if (max_size) string_size+=printed_size;
    }
    CImg<charT> res;
    (items>'x').move_to(res);
    if (max_size && res._width>=max_size) res.crop(0,max_size - 1);
    res.back() = 0;
    return res;
  }

#endif
