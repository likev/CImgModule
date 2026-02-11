#ifndef CIMG_MODULE_IMAGE_VALUE_OPS_H
#define CIMG_MODULE_IMAGE_VALUE_OPS_H

    CImg<T>& set_linear_atX(const T& value, const float fx, const int y=0, const int z=0, const int c=0,
                            const bool is_added=false) {
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

    //! Set pixel value, using linear interpolation for the X and Y-coordinates.
    /**
       Similar to set_linear_atX(const T&,float,int,int,int,bool), except that the linear interpolation
       is achieved both for X and Y-coordinates.
    **/
    CImg<T>& set_linear_atXY(const T& value, const float fx, const float fy=0, const int z=0, const int c=0,
                             const bool is_added=false) {
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

    //! Set pixel value, using linear interpolation for the X,Y and Z-coordinates.
    /**
       Similar to set_linear_atXY(const T&,float,float,int,int,bool), except that the linear interpolation
       is achieved both for X,Y and Z-coordinates.
    **/
    CImg<T>& set_linear_atXYZ(const T& value, const float fx, const float fy=0, const float fz=0, const int c=0,
                              const bool is_added=false) {
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

    //! Return a C-string containing a list of all values of the image instance.
    /**
       Return a new \c CImg<char> image whose buffer data() is a \c char* string describing the list of all pixel values
       of the image instance (written in base 10), separated by specified \c separator character.
       \param separator A \c char character which specifies the separator between values in the returned C-string.
       \param max_size Maximum size of the returned image (or \c 0 if no limits are set).
       \param format For float/double-values, tell the printf format used to generate the text representation
         of the numbers (or \c 0 for default representation).
       \note
       - The returned image is never empty.
       - For an empty image instance, the returned string is <tt>""</tt>.
       - If \c max_size is equal to \c 0, there are no limits on the size of the returned string.
       - Otherwise, if the maximum number of string characters is exceeded, the value string is cut off
         and terminated by character \c '\0'. In that case, the returned image size is <tt>max_size + 1</tt>.
    **/
    CImg<charT> value_string(const char separator=',', const unsigned int max_size=0,
                             const char *const format=0) const {
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
