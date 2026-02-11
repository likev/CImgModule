#ifndef CIMG_MODULE_IMAGE_BODY_GEOMETRY_TRANSFORM_H
#define CIMG_MODULE_IMAGE_BODY_GEOMETRY_TRANSFORM_H

    }

    //! Mirror image content along specified axis.
    /**
       \param axis Mirror axis
    **/
    CImg<T>& mirror(const char axis) {
      if (is_empty()) return *this;
      T *pf, *pb, *buf = 0;
      switch (cimg::lowercase(axis)) {
      case 'x' : {
        pf = _data; pb = data(_width - 1);
        const unsigned int width2 = _width/2;
        for (unsigned int yzv = 0; yzv<_height*_depth*_spectrum; ++yzv) {
          for (unsigned int x = 0; x<width2; ++x) { const T val = *pf; *(pf++) = *pb; *(pb--) = val; }
          pf+=_width - width2;
          pb+=_width + width2;
        }
      } break;
      case 'y' : {
        buf = new T[_width];
        pf = _data; pb = data(0,_height - 1);
        const unsigned int height2 = _height/2;
        for (unsigned int zv = 0; zv<_depth*_spectrum; ++zv) {
          for (unsigned int y = 0; y<height2; ++y) {
            std::memcpy(buf,pf,_width*sizeof(T));
            std::memcpy(pf,pb,_width*sizeof(T));
            std::memcpy(pb,buf,_width*sizeof(T));
            pf+=_width;
            pb-=_width;
          }
          pf+=(ulongT)_width*(_height - height2);
          pb+=(ulongT)_width*(_height + height2);
        }
      } break;
      case 'z' : {
        buf = new T[(ulongT)_width*_height];
        pf = _data; pb = data(0,0,_depth - 1);
        const unsigned int depth2 = _depth/2;
        cimg_forC(*this,c) {
          for (unsigned int z = 0; z<depth2; ++z) {
            std::memcpy(buf,pf,_width*_height*sizeof(T));
            std::memcpy(pf,pb,_width*_height*sizeof(T));
            std::memcpy(pb,buf,_width*_height*sizeof(T));
            pf+=(ulongT)_width*_height;
            pb-=(ulongT)_width*_height;
          }
          pf+=(ulongT)_width*_height*(_depth - depth2);
          pb+=(ulongT)_width*_height*(_depth + depth2);
        }
      } break;
      case 'c' : {
        buf = new T[(ulongT)_width*_height*_depth];
        pf = _data; pb = data(0,0,0,_spectrum - 1);
        const unsigned int _spectrum2 = _spectrum/2;
        for (unsigned int v = 0; v<_spectrum2; ++v) {
          std::memcpy(buf,pf,_width*_height*_depth*sizeof(T));
          std::memcpy(pf,pb,_width*_height*_depth*sizeof(T));
          std::memcpy(pb,buf,_width*_height*_depth*sizeof(T));
          pf+=(ulongT)_width*_height*_depth;
          pb-=(ulongT)_width*_height*_depth;
        }
      } break;
      default :
        throw CImgArgumentException(_cimg_instance
                                    "mirror(): Invalid specified axis '%c'.",
                                    cimg_instance,
                                    axis);
      }
      delete[] buf;
      return *this;
    }

    //! Mirror image content along specified axis \newinstance.
    CImg<T> get_mirror(const char axis) const {
      return (+*this).mirror(axis);
    }

    //! Mirror image content along specified axes.
    /**
       \param axes Mirror axes, as a C-string.
       \note \c axes may contains multiple characters, e.g. \c "xyz"
    **/
    CImg<T>& mirror(const char *const axes) {
      for (const char *s = axes; *s; ++s) mirror(*s);
      return *this;
    }

    //! Mirror image content along specified axes \newinstance.
    CImg<T> get_mirror(const char *const axes) const {
      return (+*this).mirror(axes);
    }

    //! Shift image content.
    /**
       \param delta_x Amount of displacement along the X-axis.
       \param delta_y Amount of displacement along the Y-axis.
       \param delta_z Amount of displacement along the Z-axis.
       \param delta_c Amount of displacement along the C-axis.
       \param boundary_conditions Boundary conditions.
         Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
    **/
    CImg<T>& shift(const int delta_x, const int delta_y=0, const int delta_z=0, const int delta_c=0,
                   const unsigned int boundary_conditions=0) {
      if (is_empty()) return *this;
      if (boundary_conditions==3)
        return get_crop(-delta_x,-delta_y,-delta_z,-delta_c,
                        width() - delta_x - 1,
                        height() - delta_y - 1,
                        depth() - delta_z - 1,
                        spectrum() - delta_c - 1,3).move_to(*this);
      if (delta_x) // Shift along X-axis
        switch (boundary_conditions) {
        case 2 : { // Periodic
          const int ml = cimg::mod(-delta_x,width()), ndelta_x = (ml<=width()/2)?ml:(ml-width());
          if (!ndelta_x) return *this;
          CImg<T> buf(cimg::abs(ndelta_x));
          if (ndelta_x>0) cimg_forYZC(*this,y,z,c) {
              std::memcpy(buf,data(0,y,z,c),ndelta_x*sizeof(T));
              std::memmove(data(0,y,z,c),data(ndelta_x,y,z,c),(_width-ndelta_x)*sizeof(T));
              std::memcpy(data(_width-ndelta_x,y,z,c),buf,ndelta_x*sizeof(T));
            } else cimg_forYZC(*this,y,z,c) {
              std::memcpy(buf,data(_width + ndelta_x,y,z,c),-ndelta_x*sizeof(T));
              std::memmove(data(-ndelta_x,y,z,c),data(0,y,z,c),(_width + ndelta_x)*sizeof(T));
              std::memcpy(data(0,y,z,c),buf,-ndelta_x*sizeof(T));
            }
        } break;
        case 1 : // Neumann
          if (delta_x<0) {
            const int ndelta_x = (-delta_x>=width())?width() - 1:-delta_x;
            if (!ndelta_x) return *this;
            cimg_forYZC(*this,y,z,c) {
              std::memmove(data(0,y,z,c),data(ndelta_x,y,z,c),(_width-ndelta_x)*sizeof(T));
              T *ptrd = data(_width - 1,y,z,c);
              const T val = *ptrd;
              for (int l = 0; l<ndelta_x - 1; ++l) *(--ptrd) = val;
            }
          } else {
            const int ndelta_x = (delta_x>=width())?width() - 1:delta_x;
            if (!ndelta_x) return *this;
            cimg_forYZC(*this,y,z,c) {
              std::memmove(data(ndelta_x,y,z,c),data(0,y,z,c),(_width-ndelta_x)*sizeof(T));
              T *ptrd = data(0,y,z,c);
              const T val = *ptrd;
              for (int l = 0; l<ndelta_x - 1; ++l) *(++ptrd) = val;
            }
          }
          break;
        default : // Dirichlet
          if (delta_x<=-width() || delta_x>=width()) return fill((T)0);
          if (delta_x<0) cimg_forYZC(*this,y,z,c) {
              std::memmove(data(0,y,z,c),data(-delta_x,y,z,c),(_width + delta_x)*sizeof(T));
              std::memset(data(_width + delta_x,y,z,c),0,-delta_x*sizeof(T));
            } else cimg_forYZC(*this,y,z,c) {
              std::memmove(data(delta_x,y,z,c),data(0,y,z,c),(_width-delta_x)*sizeof(T));
              std::memset(data(0,y,z,c),0,delta_x*sizeof(T));
            }
        }

      if (delta_y) // Shift along Y-axis
        switch (boundary_conditions) {
        case 2 : { // Periodic
          const int ml = cimg::mod(-delta_y,height()), ndelta_y = (ml<=height()/2)?ml:(ml-height());
          if (!ndelta_y) return *this;
          CImg<T> buf(width(),cimg::abs(ndelta_y));
          if (ndelta_y>0) cimg_forZC(*this,z,c) {
              std::memcpy(buf,data(0,0,z,c),_width*ndelta_y*sizeof(T));
              std::memmove(data(0,0,z,c),data(0,ndelta_y,z,c),_width*(_height-ndelta_y)*sizeof(T));
              std::memcpy(data(0,_height-ndelta_y,z,c),buf,_width*ndelta_y*sizeof(T));
            } else cimg_forZC(*this,z,c) {
              std::memcpy(buf,data(0,_height + ndelta_y,z,c),-ndelta_y*_width*sizeof(T));
              std::memmove(data(0,-ndelta_y,z,c),data(0,0,z,c),_width*(_height + ndelta_y)*sizeof(T));
              std::memcpy(data(0,0,z,c),buf,-ndelta_y*_width*sizeof(T));
            }
        } break;
        case 1 : // Neumann
          if (delta_y<0) {
            const int ndelta_y = (-delta_y>=height())?height() - 1:-delta_y;
            if (!ndelta_y) return *this;
            cimg_forZC(*this,z,c) {
              std::memmove(data(0,0,z,c),data(0,ndelta_y,z,c),_width*(_height-ndelta_y)*sizeof(T));
              T *ptrd = data(0,_height-ndelta_y,z,c), *ptrs = data(0,_height - 1,z,c);
              for (int l = 0; l<ndelta_y - 1; ++l) { std::memcpy(ptrd,ptrs,_width*sizeof(T)); ptrd+=_width; }
            }
          } else {
            const int ndelta_y = (delta_y>=height())?height() - 1:delta_y;
            if (!ndelta_y) return *this;
            cimg_forZC(*this,z,c) {
              std::memmove(data(0,ndelta_y,z,c),data(0,0,z,c),_width*(_height-ndelta_y)*sizeof(T));
              T *ptrd = data(0,1,z,c), *ptrs = data(0,0,z,c);
              for (int l = 0; l<ndelta_y - 1; ++l) { std::memcpy(ptrd,ptrs,_width*sizeof(T)); ptrd+=_width; }
            }
          }
          break;
        default : // Dirichlet
          if (delta_y<=-height() || delta_y>=height()) return fill((T)0);
          if (delta_y<0) cimg_forZC(*this,z,c) {
              std::memmove(data(0,0,z,c),data(0,-delta_y,z,c),_width*(_height + delta_y)*sizeof(T));
              std::memset(data(0,_height + delta_y,z,c),0,-delta_y*_width*sizeof(T));
            } else cimg_forZC(*this,z,c) {
              std::memmove(data(0,delta_y,z,c),data(0,0,z,c),_width*(_height-delta_y)*sizeof(T));
              std::memset(data(0,0,z,c),0,delta_y*_width*sizeof(T));
            }
        }

      if (delta_z) // Shift along Z-axis
        switch (boundary_conditions) {
        case 2 : { // Periodic
          const int ml = cimg::mod(-delta_z,depth()), ndelta_z = (ml<=depth()/2)?ml:(ml-depth());
          if (!ndelta_z) return *this;
          CImg<T> buf(width(),height(),cimg::abs(ndelta_z));
          if (ndelta_z>0) cimg_forC(*this,c) {
              std::memcpy(buf,data(0,0,0,c),_width*_height*ndelta_z*sizeof(T));
              std::memmove(data(0,0,0,c),data(0,0,ndelta_z,c),_width*_height*(_depth-ndelta_z)*sizeof(T));
              std::memcpy(data(0,0,_depth-ndelta_z,c),buf,_width*_height*ndelta_z*sizeof(T));
            } else cimg_forC(*this,c) {
              std::memcpy(buf,data(0,0,_depth + ndelta_z,c),-ndelta_z*_width*_height*sizeof(T));
              std::memmove(data(0,0,-ndelta_z,c),data(0,0,0,c),_width*_height*(_depth + ndelta_z)*sizeof(T));
              std::memcpy(data(0,0,0,c),buf,-ndelta_z*_width*_height*sizeof(T));
            }
        } break;
        case 1 : // Neumann
          if (delta_z<0) {
            const int ndelta_z = (-delta_z>=depth())?depth() - 1:-delta_z;
            if (!ndelta_z) return *this;
            cimg_forC(*this,c) {
              std::memmove(data(0,0,0,c),data(0,0,ndelta_z,c),_width*_height*(_depth-ndelta_z)*sizeof(T));
              T *ptrd = data(0,0,_depth-ndelta_z,c), *ptrs = data(0,0,_depth - 1,c);
              for (int l = 0; l<ndelta_z - 1; ++l) {
                std::memcpy(ptrd,ptrs,_width*_height*sizeof(T)); ptrd+=(ulongT)_width*_height;
              }
            }
          } else {
            const int ndelta_z = (delta_z>=depth())?depth() - 1:delta_z;
            if (!ndelta_z) return *this;
            cimg_forC(*this,c) {
              std::memmove(data(0,0,ndelta_z,c),data(0,0,0,c),_width*_height*(_depth-ndelta_z)*sizeof(T));
              T *ptrd = data(0,0,1,c), *ptrs = data(0,0,0,c);
              for (int l = 0; l<ndelta_z - 1; ++l) {
                std::memcpy(ptrd,ptrs,_width*_height*sizeof(T)); ptrd+=(ulongT)_width*_height;
              }
            }
          }
          break;
        default : // Dirichlet
          if (delta_z<=-depth() || delta_z>=depth()) return fill((T)0);
          if (delta_z<0) cimg_forC(*this,c) {
              std::memmove(data(0,0,0,c),data(0,0,-delta_z,c),_width*_height*(_depth + delta_z)*sizeof(T));
              std::memset(data(0,0,_depth + delta_z,c),0,_width*_height*(-delta_z)*sizeof(T));
            } else cimg_forC(*this,c) {
              std::memmove(data(0,0,delta_z,c),data(0,0,0,c),_width*_height*(_depth-delta_z)*sizeof(T));
              std::memset(data(0,0,0,c),0,delta_z*_width*_height*sizeof(T));
            }
        }

      if (delta_c) // Shift along C-axis
        switch (boundary_conditions) {
        case 2 : { // Periodic
          const int ml = cimg::mod(-delta_c,spectrum()), ndelta_c = (ml<=spectrum()/2)?ml:(ml-spectrum());
          if (!ndelta_c) return *this;
          CImg<T> buf(width(),height(),depth(),cimg::abs(ndelta_c));
          if (ndelta_c>0) {
            std::memcpy(buf,_data,_width*_height*_depth*ndelta_c*sizeof(T));
            std::memmove(_data,data(0,0,0,ndelta_c),_width*_height*_depth*(_spectrum-ndelta_c)*sizeof(T));
            std::memcpy(data(0,0,0,_spectrum-ndelta_c),buf,_width*_height*_depth*ndelta_c*sizeof(T));
          } else {
            std::memcpy(buf,data(0,0,0,_spectrum + ndelta_c),-ndelta_c*_width*_height*_depth*sizeof(T));
            std::memmove(data(0,0,0,-ndelta_c),_data,_width*_height*_depth*(_spectrum + ndelta_c)*sizeof(T));
            std::memcpy(_data,buf,-ndelta_c*_width*_height*_depth*sizeof(T));
          }
        } break;
        case 1 : // Neumann
          if (delta_c<0) {
            const int ndelta_c = (-delta_c>=spectrum())?spectrum() - 1:-delta_c;
            if (!ndelta_c) return *this;
            std::memmove(_data,data(0,0,0,ndelta_c),_width*_height*_depth*(_spectrum-ndelta_c)*sizeof(T));
            T *ptrd = data(0,0,0,_spectrum-ndelta_c), *ptrs = data(0,0,0,_spectrum - 1);
            for (int l = 0; l<ndelta_c - 1; ++l) {
              std::memcpy(ptrd,ptrs,_width*_height*_depth*sizeof(T)); ptrd+=(ulongT)_width*_height*_depth;
            }
          } else {
            const int ndelta_c = (delta_c>=spectrum())?spectrum() - 1:delta_c;
            if (!ndelta_c) return *this;
            std::memmove(data(0,0,0,ndelta_c),_data,_width*_height*_depth*(_spectrum-ndelta_c)*sizeof(T));
            T *ptrd = data(0,0,0,1);
            for (int l = 0; l<ndelta_c - 1; ++l) {
              std::memcpy(ptrd,_data,_width*_height*_depth*sizeof(T)); ptrd+=(ulongT)_width*_height*_depth;
            }
          }
          break;
        default : // Dirichlet
          if (delta_c<=-spectrum() || delta_c>=spectrum()) return fill((T)0);
          if (delta_c<0) {
            std::memmove(_data,data(0,0,0,-delta_c),_width*_height*_depth*(_spectrum + delta_c)*sizeof(T));
            std::memset(data(0,0,0,_spectrum + delta_c),0,_width*_height*_depth*(-delta_c)*sizeof(T));
          } else {
            std::memmove(data(0,0,0,delta_c),_data,_width*_height*_depth*(_spectrum-delta_c)*sizeof(T));
            std::memset(_data,0,delta_c*_width*_height*_depth*sizeof(T));
          }
        }
      return *this;
    }

    //! Shift image content \newinstance.
    CImg<T> get_shift(const int delta_x, const int delta_y=0, const int delta_z=0, const int delta_c=0,
                      const unsigned int boundary_conditions=0) const {
      return (+*this).shift(delta_x,delta_y,delta_z,delta_c,boundary_conditions);
    }

    //! Permute axes order.
    /**
       \param axes_order Axes permutations, as a C-string of 4 characters.
       This function permutes image content regarding the specified axes permutation.
    **/
    CImg<T>& permute_axes(const char *const axes_order) {
      if (is_empty() || !axes_order) return *this;
      const unsigned uicase = _permute_axes_uicase(axes_order);
      if (_permute_axes_is_optim(uicase)) { // Data layout allow to do nothing but set the new dimensions
        CImg<T> res(*this,true);
        for (unsigned int i = 0; i<4; ++i) {
          const unsigned int
            axis = (uicase>>(4*(3 - i)))&15,
            value = !axis?_width:axis==1?_height:axis==2?_depth:_spectrum;
          if (!i) res._width = value; else if (i==1) res._height = value;
          else if (i==2) res._depth = value; else res._spectrum = value;
        }
        _width = res._width; _height = res._height; _depth = res._depth; _spectrum = res._spectrum;
        return *this;
      }
      return get_permute_axes(axes_order).move_to(*this);
    }

    //! Permute axes order \newinstance.
    CImg<T> get_permute_axes(const char *const axes_order) const {
      const T foo = (T)0;
      return _permute_axes(axes_order,foo);
    }

    unsigned int _permute_axes_uicase(const char *const axes_order) const { // Convert axes to integer case number
      unsigned char s_axes[4] = { 0,1,2,3 }, n_axes[4] = { };
      bool is_error = false;
      if (axes_order) for (unsigned int l = 0; axes_order[l]; ++l) {
          int c = cimg::lowercase(axes_order[l]);
          if (l>=4 || (c!='x' && c!='y' && c!='z' && c!='c')) { is_error = true; break; }
          else { ++n_axes[c%=4]; s_axes[l] = (unsigned char)c; }
        }
      is_error|=(*n_axes>1) || (n_axes[1]>1) || (n_axes[2]>1) || (n_axes[3]>1);
      if (is_error)
        throw CImgArgumentException(_cimg_instance
                                    "permute_axes(): Invalid specified axes order '%s'.",
                                    cimg_instance,
                                    axes_order);
      return (s_axes[0]<<12) | (s_axes[1]<<8) | (s_axes[2]<<4) | (s_axes[3]);
    }

    bool _permute_axes_is_optim(const unsigned int uicase) const { // Determine cases where nothing has to be done
      const unsigned int co = ((_width>1)<<3)|((_height>1)<<2)|((_depth>1)<<1)|(_spectrum>1);
      if (co<=2 || uicase==0x0123) return true;
      switch (uicase) {
      case (0x0132) : if ((co>=4 && co<=6) || (co>=8 && co<=10) || (co>=12 && co<=14)) return true; break;
      case (0x0213) : if ((co>=3 && co<=5) || (co>=8 && co<=13)) return true; break;
      case (0x0231) : if (co==3 || co==4 || (co>=8 && co<=12)) return true; break;
      case (0x0312) : if (co==4 || co==6 || co==8 || co==9 || co==10 || co==12 || co==14) return true; break;
      case (0x0321) : if (co==4 || (co>=8 && co<=10) || co==12) return true; break;
      case (0x1023) : if (co>=3 && co<=11) return true; break;
      case (0x1032) : if ((co>=4 && co<=6) || (co>=8 && co<=10)) return true; break;
      case (0x1203) : if (co>=3 && co<=9) return true; break;
      case (0x1230) : if (co>=3 && co<=8) return true; break;
      case (0x1302) : if ((co>=4 && co<=6) || co==8 || co==10) return true; break;
      case (0x1320) : if ((co>=4 && co<=6) || co==8) return true; break;
      case (0x2013) : if ((co>=3 && co<=5) || co==8 || co==9 || co==12 || co==13) return true; break;
      case (0x2031) : if (co==3 || co==4 || co==8 || co==9 || co==12) return true; break;
      case (0x2103) : if ((co>=3 && co<=5) || co==8 || co==9) return true; break;
      case (0x2130) : if ((co>=3 && co<=5) || co==8) return true; break;
      case (0x2301) : if (co==3 || co==4 || co==8 || co==12) return true; break;
      case (0x2310) : if (co==3 || co==4 || co==8) return true; break;
      case (0x3012) : if (co==4 || co==6 || co==8 || co==10 || co==12 || co==14) return true; break;
      case (0x3021) : if (co==4 || co==8 || co==10 || co==12) return true; break;
      case (0x3102) : if (co==4 || co==6 || co==8 || co==10) return true; break;
      case (0x3120) : if (co==4 || co==6 || co==8) return true; break;
      case (0x3201) : if (co==4 || co==8 || co==12) return true; break;
      case (0x3210) : if (co==4 || co==8) return true; break;
      }
      return false;
    }

    template<typename t>
    CImg<t> _permute_axes(const char *const axes_order, const t&) const {
      if (is_empty() || !axes_order) return CImg<t>(*this,false);
      CImg<t> res;
      const unsigned uicase = _permute_axes_uicase(axes_order);

      if (_permute_axes_is_optim(uicase)) { // Data layout allow to do nothing but set the new dimensions
        res.assign(*this,false);
        for (unsigned int i = 0; i<4; ++i) {
          const unsigned int
            axis = (uicase>>(4*(3 - i)))&15,
            value = !axis?_width:axis==1?_height:axis==2?_depth:_spectrum;
          if (!i) res._width = value; else if (i==1) res._height = value;
          else if (i==2) res._depth = value; else res._spectrum = value;
        }
        return res;
      }

      const T* ptrs = _data;
      ulongT wh, whd;

      switch (uicase) {
      case 0x0123 : // xyzc
        return +*this;
      case 0x0132 : // xycz
        res.assign(_width,_height,_spectrum,_depth);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(x,y,c,z,wh,whd) = (t)*(ptrs++);
        break;
      case 0x0213 : // xzyc
        res.assign(_width,_depth,_height,_spectrum);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(x,z,y,c,wh,whd) = (t)*(ptrs++);
        break;
      case 0x0231 : // xzcy
        res.assign(_width,_depth,_spectrum,_height);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(x,z,c,y,wh,whd) = (t)*(ptrs++);
        break;
      case 0x0312 : // xcyz
        res.assign(_width,_spectrum,_height,_depth);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(x,c,y,z,wh,whd) = (t)*(ptrs++);
        break;
      case 0x0321 : // xczy
        res.assign(_width,_spectrum,_depth,_height);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(x,c,z,y,wh,whd) = (t)*(ptrs++);
        break;
      case 0x1023 : // yxzc
        res.assign(_height,_width,_depth,_spectrum);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(y,x,z,c,wh,whd) = (t)*(ptrs++);
        break;
      case 0x1032 : // yxcz
        res.assign(_height,_width,_spectrum,_depth);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(y,x,c,z,wh,whd) = (t)*(ptrs++);
        break;
      case 0x1203 : // yzxc
        res.assign(_height,_depth,_width,_spectrum);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(y,z,x,c,wh,whd) = (t)*(ptrs++);
        break;
      case 0x1230 : // yzcx
        res.assign(_height,_depth,_spectrum,_width);
        switch (_width) {
        case 1 : {
          t *ptr_r = res.data(0,0,0,0);
          for (unsigned int siz = _height*_depth*_spectrum; siz; --siz) {
            *(ptr_r++) = (t)*(ptrs++);
          }
        } break;
        case 2 : {
          t *ptr_r = res.data(0,0,0,0), *ptr_g = res.data(0,0,0,1);
          for (unsigned int siz = _height*_depth*_spectrum; siz; --siz) {
            *(ptr_r++) = (t)ptrs[0];
            *(ptr_g++) = (t)ptrs[1];
            ptrs+=2;
          }
        } break;
        case 3 : { // Optimization for the classical conversion from interleaved RGB to planar RGB
          t *ptr_r = res.data(0,0,0,0), *ptr_g = res.data(0,0,0,1), *ptr_b = res.data(0,0,0,2);
          for (unsigned int siz = _height*_depth*_spectrum; siz; --siz) {
            *(ptr_r++) = (t)ptrs[0];
            *(ptr_g++) = (t)ptrs[1];
            *(ptr_b++) = (t)ptrs[2];
            ptrs+=3;
          }
        } break;
        case 4 : { // Optimization for the classical conversion from interleaved RGBA to planar RGBA
          t
            *ptr_r = res.data(0,0,0,0), *ptr_g = res.data(0,0,0,1),
            *ptr_b = res.data(0,0,0,2), *ptr_a = res.data(0,0,0,3);
          for (unsigned int siz = _height*_depth*_spectrum; siz; --siz) {
            *(ptr_r++) = (t)ptrs[0];
            *(ptr_g++) = (t)ptrs[1];
            *(ptr_b++) = (t)ptrs[2];
            *(ptr_a++) = (t)ptrs[3];
            ptrs+=4;
          }
        } break;
        default : {
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(y,z,c,x,wh,whd) = *(ptrs++);
          return res;
        }
        }
        break;
      case 0x1302 : // ycxz
        res.assign(_height,_spectrum,_width,_depth);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(y,c,x,z,wh,whd) = (t)*(ptrs++);
        break;
      case 0x1320 : // yczx
        res.assign(_height,_spectrum,_depth,_width);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(y,c,z,x,wh,whd) = (t)*(ptrs++);
        break;
      case 0x2013 : // zxyc
        res.assign(_depth,_width,_height,_spectrum);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(z,x,y,c,wh,whd) = (t)*(ptrs++);
        break;
      case 0x2031 : // zxcy
        res.assign(_depth,_width,_spectrum,_height);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(z,x,c,y,wh,whd) = (t)*(ptrs++);
        break;
      case 0x2103 : // zyxc
        res.assign(_depth,_height,_width,_spectrum);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(z,y,x,c,wh,whd) = (t)*(ptrs++);
        break;
      case 0x2130 : // zycx
        res.assign(_depth,_height,_spectrum,_width);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(z,y,c,x,wh,whd) = (t)*(ptrs++);
        break;
      case 0x2301 : // zcxy
        res.assign(_depth,_spectrum,_width,_height);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(z,c,x,y,wh,whd) = (t)*(ptrs++);
        break;
      case 0x2310 : // zcyx
        res.assign(_depth,_spectrum,_height,_width);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(z,c,y,x,wh,whd) = (t)*(ptrs++);
        break;
      case 0x3012 : // cxyz
        res.assign(_spectrum,_width,_height,_depth);
        switch (_spectrum) {
        case 1 : {
          const T *ptr_r = data(0,0,0,0);
          t *ptrd = res._data;
          for (ulongT siz = (ulongT)_width*_height*_depth; siz; --siz) *(ptrd++) = (t)*(ptr_r++);
        } break;
        case 2 : {
          const T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1);
          t *ptrd = res._data;
          for (ulongT siz = (ulongT)_width*_height*_depth; siz; --siz) {
            ptrd[0] = (t)*(ptr_r++);
            ptrd[1] = (t)*(ptr_g++);
            ptrd+=2;
          }
        } break;
        case 3 : { // Optimization for the classical conversion from planar RGB to interleaved RGB
          const T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2);
          t *ptrd = res._data;
          for (ulongT siz = (ulongT)_width*_height*_depth; siz; --siz) {
            ptrd[0] = (t)*(ptr_r++);
            ptrd[1] = (t)*(ptr_g++);
            ptrd[2] = (t)*(ptr_b++);
            ptrd+=3;
          }
        } break;
        case 4 : { // Optimization for the classical conversion from planar RGBA to interleaved RGBA
          const T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2), *ptr_a = data(0,0,0,3);
          t *ptrd = res._data;
          for (ulongT siz = (ulongT)_width*_height*_depth; siz; --siz) {
            ptrd[0] = (t)*(ptr_r++);
            ptrd[1] = (t)*(ptr_g++);
            ptrd[2] = (t)*(ptr_b++);
            ptrd[3] = (t)*(ptr_a++);
            ptrd+=4;
          }
        } break;
        default : {
          wh = (ulongT)res._width*res._height; whd = wh*res._depth;
          cimg_forXYZC(*this,x,y,z,c) res(c,x,y,z,wh,whd) = (t)*(ptrs++);
        }
        }
        break;
      case 0x3021 : // cxzy
        res.assign(_spectrum,_width,_depth,_height);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(c,x,z,y,wh,whd) = (t)*(ptrs++);
        break;
      case 0x3102 : // cyxz
        res.assign(_spectrum,_height,_width,_depth);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(c,y,x,z,wh,whd) = (t)*(ptrs++);
        break;
      case 0x3120 : // cyzx
        res.assign(_spectrum,_height,_depth,_width);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(c,y,z,x,wh,whd) = (t)*(ptrs++);
        break;
      case 0x3201 : // czxy
        res.assign(_spectrum,_depth,_width,_height);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(c,z,x,y,wh,whd) = (t)*(ptrs++);
        break;
      case 0x3210 : // czyx
        res.assign(_spectrum,_depth,_height,_width);
        wh = (ulongT)res._width*res._height; whd = wh*res._depth;
        cimg_forXYZC(*this,x,y,z,c) res(c,z,y,x,wh,whd) = (t)*(ptrs++);
        break;
      }
      return res;
    }

    //! Unroll pixel values along specified axis.
    /**
       \param axis Unroll axis (can be \c 'x', \c 'y', \c 'z' or c 'c').
    **/
    CImg<T>& unroll(const char axis) {
      const unsigned int siz = (unsigned int)size();
      if (siz) switch (cimg::lowercase(axis)) {
      case 'x' : _width = siz; _height = _depth = _spectrum = 1; break;
      case 'y' : _height = siz; _width = _depth = _spectrum = 1; break;
      case 'z' : _depth = siz; _width = _height = _spectrum = 1; break;
      case 'c' : _spectrum = siz; _width = _height = _depth = 1; break;
      }
      return *this;
    }

    //! Unroll pixel values along specified axis \newinstance.
    CImg<T> get_unroll(const char axis) const {
      return (+*this).unroll(axis);
    }

    //! Rotate image with arbitrary angle.
    /**
       \param angle Rotation angle, in degrees.
       \param interpolation Type of interpolation. Can be <tt>{ 0=nearest | 1=linear | 2=cubic }</tt>.
       \param boundary_conditions Boundary conditions.
         Can be <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
       \note The size of the image is modified.
    **/
    CImg<T>& rotate(const float angle, const unsigned int interpolation=1,
                    const unsigned int boundary_conditions=0) {
      const float nangle = cimg::mod(angle,360.f);
      if (nangle==0.f) return *this;
      return get_rotate(nangle,interpolation,boundary_conditions).move_to(*this);
    }

    //! Rotate image with arbitrary angle \newinstance.
    CImg<T> get_rotate(const float angle, const unsigned int interpolation=1,
                       const unsigned int boundary_conditions=0) const {
      if (is_empty()) return *this;
      CImg<T> res;
      const float nangle = cimg::mod(angle,360.f);
      if (boundary_conditions!=1 && cimg::mod(nangle,90.f)==0) { // Optimized version for orthogonal angles
        const int wm1 = width() - 1, hm1 = height() - 1;
        const int iangle = (int)nangle/90;
        switch (iangle) {
        case 1 : { // 90 deg
          res.assign(_height,_width,_depth,_spectrum);
          T *ptrd = res._data;
          cimg_forXYZC(res,x,y,z,c) *(ptrd++) = (*this)(y,hm1 - x,z,c);
        } break;
        case 2 : { // 180 deg
          res.assign(_width,_height,_depth,_spectrum);
          T *ptrd = res._data;
          cimg_forXYZC(res,x,y,z,c) *(ptrd++) = (*this)(wm1 - x,hm1 - y,z,c);
        } break;
        case 3 : { // 270 deg
          res.assign(_height,_width,_depth,_spectrum);
          T *ptrd = res._data;
          cimg_forXYZC(res,x,y,z,c) *(ptrd++) = (*this)(wm1 - y,x,z,c);
        } break;
        default : // 0 deg
          return *this;
        }
      } else { // Generic angle
        const float
          rad = (float)(nangle*cimg::PI/180.),
          ca = (float)std::cos(rad), sa = (float)std::sin(rad),
          ux = cimg::abs((_width - 1)*ca), uy = cimg::abs((_width - 1)*sa),
          vx = cimg::abs((_height - 1)*sa), vy = cimg::abs((_height - 1)*ca),
          w2 = 0.5f*(_width - 1), h2 = 0.5f*(_height - 1);
        res.assign((int)cimg::round(1 + ux + vx),(int)cimg::round(1 + uy + vy),_depth,_spectrum);
        const float rw2 = 0.5f*(res._width - 1), rh2 = 0.5f*(res._height - 1);
        _rotate(res,nangle,interpolation,boundary_conditions,w2,h2,rw2,rh2);
      }
      return res;
    }

    //! Rotate image with arbitrary angle, around a center point.
    /**
       \param angle Rotation angle, in degrees.
       \param cx X-coordinate of the rotation center.
       \param cy Y-coordinate of the rotation center.
       \param interpolation Type of interpolation, <tt>{ 0=nearest | 1=linear | 2=cubic | 3=mirror }</tt>.
       \param boundary_conditions Boundary conditions, <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
    **/
    CImg<T>& rotate(const float angle, const float cx, const float cy,
                    const unsigned int interpolation, const unsigned int boundary_conditions=0) {
      return get_rotate(angle,cx,cy,interpolation,boundary_conditions).move_to(*this);
    }

    //! Rotate image with arbitrary angle, around a center point \newinstance.
    CImg<T> get_rotate(const float angle, const float cx, const float cy,
                       const unsigned int interpolation, const unsigned int boundary_conditions=0) const {
      if (is_empty()) return *this;
      CImg<T> res(_width,_height,_depth,_spectrum);
      _rotate(res,angle,interpolation,boundary_conditions,cx,cy,cx,cy);
      return res;
    }

    // [internal] Perform 2D rotation with arbitrary angle.
    void _rotate(CImg<T>& res, const float angle,
                 const unsigned int interpolation, const unsigned int boundary_conditions,
                 const float w2, const float h2,
                 const float rw2, const float rh2) const {
      const float
        rad = (float)(angle*cimg::PI/180.),
        ca = (float)std::cos(rad), sa = (float)std::sin(rad);

      switch (boundary_conditions) {
      case 3 : { // Mirror

        switch (interpolation) {
        case 2 : { // Cubic interpolation
          const float ww = 2.f*width(), hh = 2.f*height();
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2,
              mx = cimg::mod(w2 + xc*ca + yc*sa,ww),
              my = cimg::mod(h2 - xc*sa + yc*ca,hh);
            res(x,y,z,c) = _cubic_atXY_c(mx<width()?mx:ww - mx - 1,my<height()?my:hh - my - 1,z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          const float ww = 2.f*width(), hh = 2.f*height();
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2,
              mx = cimg::mod(w2 + xc*ca + yc*sa,ww),
              my = cimg::mod(h2 - xc*sa + yc*ca,hh);
            res(x,y,z,c) = (T)_linear_atXY(mx<width()?mx:ww - mx - 1,my<height()?my:hh - my - 1,z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          const int ww = 2*width(), hh = 2*height();
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2,
              mx = cimg::mod((int)cimg::round(w2 + xc*ca + yc*sa),ww),
              my = cimg::mod((int)cimg::round(h2 - xc*sa + yc*ca),hh);
            res(x,y,z,c) = (*this)(mx<width()?mx:ww - mx - 1,my<height()?my:hh - my - 1,z,c);
          }
        }
        }
      } break;

      case 2 : // Periodic
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = _cubic_atXY_pc(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = (T)_linear_atXY_p(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = (*this)(cimg::mod((int)cimg::round(w2 + xc*ca + yc*sa),(float)width()),
                                   cimg::mod((int)cimg::round(h2 - xc*sa + yc*ca),(float)height()),z,c);
          }
        }
        } break;

      case 1 : // Neumann
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = _cubic_atXY_c(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = (T)_linear_atXY(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = _atXY((int)cimg::round(w2 + xc*ca + yc*sa),
                                 (int)cimg::round(h2 - xc*sa + yc*ca),z,c);
          }
        }
        } break;

      default : // Dirichlet
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = cubic_atXY_c(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c,(T)0);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = (T)linear_atXY(w2 + xc*ca + yc*sa,h2 - xc*sa + yc*ca,z,c,(T)0);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZC(res,x,y,z,c) {
            const float xc = x - rw2, yc = y - rh2;
            res(x,y,z,c) = atXY((int)cimg::round(w2 + xc*ca + yc*sa),
                                (int)cimg::round(h2 - xc*sa + yc*ca),z,c,(T)0);
          }
        }
        }
      }
    }

    //! Rotate volumetric image with arbitrary angle and axis.
    /**
       \param u X-coordinate of the 3D rotation axis.
       \param v Y-coordinate of the 3D rotation axis.
       \param w Z-coordinate of the 3D rotation axis.
       \param angle Rotation angle, in degrees.
       \param interpolation Type of interpolation. Can be <tt>{ 0=nearest | 1=linear | 2=cubic }</tt>.
       \param boundary_conditions Boundary conditions.
         Can be <tt>{  0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
       \note Most of the time, size of the image is modified.
    **/
    CImg<T> rotate(const float u, const float v, const float w, const float angle,
                   const unsigned int interpolation, const unsigned int boundary_conditions) {
      const float nangle = cimg::mod(angle,360.f);
      if (nangle==0.f) return *this;
      return get_rotate(u,v,w,nangle,interpolation,boundary_conditions).move_to(*this);
    }

    //! Rotate volumetric image with arbitrary angle and axis \newinstance.
    CImg<T> get_rotate(const float u, const float v, const float w, const float angle,
                       const unsigned int interpolation, const unsigned int boundary_conditions) const {
      if (is_empty()) return *this;
      CImg<T> res;
      const float
        w1 = _width - 1, h1 = _height - 1, d1 = _depth -1,
        w2 = 0.5f*w1, h2 = 0.5f*h1, d2 = 0.5f*d1;
      CImg<floatT> R = CImg<floatT>::rotation_matrix(u,v,w,angle);
      const CImg<Tfloat>
        X = R*CImg<Tfloat>(8,3,1,1,
                           0.f,w1,w1,0.f,0.f,w1,w1,0.f,
                           0.f,0.f,h1,h1,0.f,0.f,h1,h1,
                           0.f,0.f,0.f,0.f,d1,d1,d1,d1);
      float
        xm, xM = X.get_shared_row(0).max_min(xm),
        ym, yM = X.get_shared_row(1).max_min(ym),
        zm, zM = X.get_shared_row(2).max_min(zm);
      const int
        dx = (int)cimg::round(xM - xm),
        dy = (int)cimg::round(yM - ym),
        dz = (int)cimg::round(zM - zm);
      R.transpose();
      res.assign(1 + dx,1 + dy,1 + dz,_spectrum);
      const float rw2 = 0.5f*dx, rh2 = 0.5f*dy, rd2 = 0.5f*dz;
      _rotate(res,R,interpolation,boundary_conditions,w2,h2,d2,rw2,rh2,rd2);
      return res;
    }

    //! Rotate volumetric image with arbitrary angle and axis, around a center point.
    /**
       \param u X-coordinate of the 3D rotation axis.
       \param v Y-coordinate of the 3D rotation axis.
       \param w Z-coordinate of the 3D rotation axis.
       \param angle Rotation angle, in degrees.
       \param cx X-coordinate of the rotation center.
       \param cy Y-coordinate of the rotation center.
       \param cz Z-coordinate of the rotation center.
       \param interpolation Type of interpolation. Can be <tt>{ 0=nearest | 1=linear | 2=cubic | 3=mirror }</tt>.
       \param boundary_conditions Boundary conditions.
         Can be <tt>{  0=dirichlet | 1=neumann | 2=periodic }</tt>.
       \note Most of the time, size of the image is modified.
    **/
    CImg<T> rotate(const float u, const float v, const float w, const float angle,
                   const float cx, const float cy, const float cz,
                   const unsigned int interpolation=1, const unsigned int boundary_conditions=0) {
      const float nangle = cimg::mod(angle,360.f);
      if (nangle==0.f) return *this;
      return get_rotate(u,v,w,nangle,cx,cy,cz,interpolation,boundary_conditions).move_to(*this);
    }

    //! Rotate volumetric image with arbitrary angle and axis, around a center point \newinstance.
    CImg<T> get_rotate(const float u, const float v, const float w, const float angle,
                       const float cx, const float cy, const float cz,
                       const unsigned int interpolation=1, const unsigned int boundary_conditions=0) const {
      if (is_empty()) return *this;
      CImg<T> res(_width,_height,_depth,_spectrum);
      CImg<floatT> R = CImg<floatT>::rotation_matrix(u,v,w,-angle);
      _rotate(res,R,interpolation,boundary_conditions,cx,cy,cz,cx,cy,cz);
      return res;
    }

    // [internal] Perform 3D rotation with arbitrary axis and angle.
    void _rotate(CImg<T>& res, const CImg<Tfloat>& R,
                 const unsigned int interpolation, const unsigned int boundary_conditions,
                 const float w2, const float h2, const float d2,
                 const float rw2, const float rh2, const float rd2) const {
      switch (boundary_conditions) {
      case 3 : // Mirror
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          const float ww = 2.f*width(), hh = 2.f*height(), dd = 2.f*depth();
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = cimg::mod((float)(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),ww),
              Y = cimg::mod((float)(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),hh),
              Z = cimg::mod((float)(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),dd);
            cimg_forC(res,c) res(x,y,z,c) = _cubic_atXYZ_c(X<width()?X:ww - X - 1,
                                                           Y<height()?Y:hh - Y - 1,
                                                           Z<depth()?Z:dd - Z - z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          const float ww = 2.f*width(), hh = 2.f*height(), dd = 2.f*depth();
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = cimg::mod((float)(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),ww),
              Y = cimg::mod((float)(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),hh),
              Z = cimg::mod((float)(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),dd);
            cimg_forC(res,c) res(x,y,z,c) = (T)_linear_atXYZ(X<width()?X:ww - X - 1,
                                                             Y<height()?Y:hh - Y - 1,
                                                             Z<depth()?Z:dd - Z - 1,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          const int ww = 2*width(), hh = 2*height(), dd = 2*depth();
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float xc = x - rw2, yc = y - rh2, zc = z - rd2;
            const int
              X = cimg::mod((int)cimg::round(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),ww),
              Y = cimg::mod((int)cimg::round(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),hh),
              Z = cimg::mod((int)cimg::round(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),dd);
            cimg_forC(res,c) res(x,y,z,c) = (*this)(X<width()?X:ww - X - 1,
                                                    Y<height()?Y:hh - Y - 1,
                                                    Z<depth()?Z:dd - Z -  1,c);
          }
        }
        } break;

      case 2 : // Periodic
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = _cubic_atXYZ_pc(X,Y,Z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = (T)_linear_atXYZ_p(X,Y,Z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float xc = x - rw2, yc = y - rh2, zc = z - rd2;
            const int
              X = cimg::mod((int)cimg::round(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),width()),
              Y = cimg::mod((int)cimg::round(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),height()),
              Z = cimg::mod((int)cimg::round(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc),depth());
            cimg_forC(res,c) res(x,y,z,c) = (*this)(X,Y,Z,c);
          }
        }
        } break;

      case 1 : // Neumann
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = _cubic_atXYZ_c(X,Y,Z,c);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = _linear_atXYZ(X,Y,Z,c);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float xc = x - rw2, yc = y - rh2, zc = z - rd2;
            const int
              X = (int)cimg::round(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),
              Y = (int)cimg::round(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),
              Z = (int)cimg::round(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc);
            cimg_forC(res,c) res(x,y,z,c) = _atXYZ(X,Y,Z,c);
          }
        }
        } break;

      default : // Dirichlet
        switch (interpolation) {
        case 2 : { // Cubic interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = cubic_atXYZ_c(X,Y,Z,c,(T)0);
          }
        } break;
        case 1 : { // Linear interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float
              xc = x - rw2, yc = y - rh2, zc = z - rd2,
              X = w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc,
              Y = h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc,
              Z = d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc;
            cimg_forC(res,c) res(x,y,z,c) = linear_atXYZ(X,Y,Z,c,(T)0);
          }
        } break;
        default : { // Nearest-neighbor interpolation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(res.size(),2048))
          cimg_forXYZ(res,x,y,z) {
            const float xc = x - rw2, yc = y - rh2, zc = z - rd2;
            const int
              X = (int)cimg::round(w2 + R(0,0)*xc + R(1,0)*yc + R(2,0)*zc),
              Y = (int)cimg::round(h2 + R(0,1)*xc + R(1,1)*yc + R(2,1)*zc),
              Z = (int)cimg::round(d2 + R(0,2)*xc + R(1,2)*yc + R(2,2)*zc);
            cimg_forC(res,c) res(x,y,z,c) = atXYZ(X,Y,Z,c,(T)0);
          }
        }
        } break;
      }
    }

    //! Warp image content by a warping field.
    /**
       \param warp Warping field.
       \param mode Can be { 0=backward-absolute | 1=backward-relative | 2=forward-absolute | 3=foward-relative }
       \param interpolation Can be <tt>{ 0=nearest | 1=linear | 2=cubic }</tt>.
       \param boundary_conditions Boundary conditions <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
    **/
    template<typename t>
    CImg<T>& warp(const CImg<t>& p_warp, const unsigned int mode=0,
                  const unsigned int interpolation=1, const unsigned int boundary_conditions=0) {
      return get_warp(p_warp,mode,interpolation,boundary_conditions).move_to(*this);
    }

    //! Warp image content by a warping field \newinstance.
    template<typename t>
    CImg<T> get_warp(const CImg<t>& p_warp, const unsigned int mode=0,
                     const unsigned int interpolation=1, const unsigned int boundary_conditions=0) const {
      if (is_empty() || !p_warp) return *this;
      if (mode && !is_sameXYZ(p_warp))
        throw CImgArgumentException(_cimg_instance
                                    "warp(): Instance and specified relative warping field (%u,%u,%u,%u,%p) "
                                    "have different XYZ dimensions.",
                                    cimg_instance,
                                    p_warp._width,p_warp._height,p_warp._depth,p_warp._spectrum,p_warp._data);

      CImg<T> res(p_warp._width,p_warp._height,p_warp._depth,_spectrum);

      if (p_warp._spectrum==1) { // 1D warping
        if (mode>=3) { // Forward-relative warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atX(*(ptrs++),x + (float)*(ptrs0++),y,z,c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int X = x + (int)cimg::round(*(ptrs0++));
                if (X>=0 && X<width()) res(X,y,z,c) = *(ptrs++);
              }
            }
        } else if (mode==2) { // Forward-absolute warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atX(*(ptrs++),(float)*(ptrs0++),y,z,c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int X = (int)cimg::round(*(ptrs0++));
                if (X>=0 && X<width()) res(X,y,z,c) = *(ptrs++);
              }
            }
        } else if (mode==1) { // Backward-relative warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float mx = cimg::mod(x - (float)*(ptrs0++),w2);
                  *(ptrd++) = _cubic_atX_c(mx<width()?mx:w2 - mx - 1,y,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atX_pc(x - (float)*(ptrs0++),y,z,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atX_c(x - (float)*(ptrs0++),y,z,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atX_c(x - (float)*(ptrs0++),y,z,c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float mx = cimg::mod(x - (float)*(ptrs0++),w2);
                  *(ptrd++) = (T)_linear_atX(mx<width()?mx:w2 - mx - 1,y,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atX_p(x - (float)*(ptrs0++),y,z,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atX(x - (float)*(ptrs0++),y,z,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atX(x - (float)*(ptrs0++),y,z,c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int mx = cimg::mod(x - (int)cimg::round(*(ptrs0++)),w2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,y,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod(x - (int)cimg::round(*(ptrs0++)),width()),y,z,c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atX(x - (int)cimg::round(*(ptrs0++)),y,z,c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atX(x - (int)cimg::round(*(ptrs0++)),y,z,c,(T)0);
              }
            }
        }
        else { // Backward-absolute warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float mx = cimg::mod((float)*(ptrs0++),w2);
                  *(ptrd++) = _cubic_atX_c(mx<width()?mx:w2 - mx - 1,0,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atX_pc((float)*(ptrs0++),0,0,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atX_c((float)*(ptrs0++),0,0,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atX_c((float)*(ptrs0++),0,0,c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float mx = cimg::mod((float)*(ptrs0++),w2);
                  *(ptrd++) = (T)_linear_atX(mx<width()?mx:w2 - mx - 1,0,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atX_p((float)*(ptrs0++),0,0,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atX((float)*(ptrs0++),0,0,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atX((float)*(ptrs0++),0,0,c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int mx = cimg::mod((int)cimg::round(*(ptrs0++)),w2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,0,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod((int)cimg::round(*(ptrs0++)),width()),0,0,c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atX((int)cimg::round(*(ptrs0++)),0,0,c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atX((int)cimg::round(*(ptrs0++)),0,0,c,(T)0);
              }
            }
        }

      } else if (p_warp._spectrum==2) { // 2D warping
        if (mode>=3) { // Forward-relative warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atXY(*(ptrs++),x + (float)*(ptrs0++),y + (float)*(ptrs1++),z,c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int X = x + (int)cimg::round(*(ptrs0++)), Y = y + (int)cimg::round(*(ptrs1++));
                if (X>=0 && X<width() && Y>=0 && Y<height()) res(X,Y,z,c) = *(ptrs++);
              }
            }
        } else if (mode==2) { // Forward-absolute warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atXY(*(ptrs++),(float)*(ptrs0++),(float)*(ptrs1++),z,c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int X = (int)cimg::round(*(ptrs0++)), Y = (int)cimg::round(*(ptrs1++));
                if (X>=0 && X<width() && Y>=0 && Y<height()) res(X,Y,z,c) = *(ptrs++);
              }
            }
        } else if (mode==1) { // Backward-relative warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod(x - (float)*(ptrs0++),w2),
                    my = cimg::mod(y - (float)*(ptrs1++),h2);
                  *(ptrd++) = _cubic_atXY_c(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXY_pc(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXY_c(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atXY_c(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod(x - (float)*(ptrs0++),w2),
                    my = cimg::mod(y - (float)*(ptrs1++),h2);
                  *(ptrd++) = (T)_linear_atXY(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXY_p(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXY(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atXY(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z,c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width(), h2 = 2*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int
                    mx = cimg::mod(x - (int)cimg::round(*(ptrs0++)),w2),
                    my = cimg::mod(y - (int)cimg::round(*(ptrs1++)),h2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,z,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod(x - (int)cimg::round(*(ptrs0++)),width()),
                                                     cimg::mod(y - (int)cimg::round(*(ptrs1++)),height()),z,c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atXY(x - (int)cimg::round(*(ptrs0++)),
                                                   y - (int)cimg::round(*(ptrs1++)),z,c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atXY(x - (int)cimg::round(*(ptrs0++)),
                                                  y - (int)cimg::round(*(ptrs1++)),z,c,(T)0);
              }
            }
        } else { // Backward-absolute warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod((float)*(ptrs0++),w2),
                    my = cimg::mod((float)*(ptrs1++),h2);
                  *(ptrd++) = _cubic_atXY_c(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXY_pc((float)*(ptrs0++),(float)*(ptrs1++),0,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXY_c((float)*(ptrs0++),(float)*(ptrs1++),0,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atXY_c((float)*(ptrs0++),(float)*(ptrs1++),0,c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod((float)*(ptrs0++),w2),
                    my = cimg::mod((float)*(ptrs1++),h2);
                  *(ptrd++) = (T)_linear_atXY(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXY_p((float)*(ptrs0++),(float)*(ptrs1++),0,c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXY((float)*(ptrs0++),(float)*(ptrs1++),0,c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atXY((float)*(ptrs0++),(float)*(ptrs1++),0,c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width(), h2 = 2*height();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int
                    mx = cimg::mod((int)cimg::round(*(ptrs0++)),w2),
                    my = cimg::mod((int)cimg::round(*(ptrs1++)),h2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,my<height()?my:h2 - my - 1,0,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod((int)cimg::round(*(ptrs0++)),width()),
                                                     cimg::mod((int)cimg::round(*(ptrs1++)),height()),0,c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atXY((int)cimg::round(*(ptrs0++)),
                                                   (int)cimg::round(*(ptrs1++)),0,c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1); T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atXY((int)cimg::round(*(ptrs0++)),
                                                  (int)cimg::round(*(ptrs1++)),0,c,(T)0);
              }
            }
        }

      } else { // 3D warping
        if (mode>=3) { // Forward-relative warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
              const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atXYZ(*(ptrs++),x + (float)*(ptrs0++),y + (float)*(ptrs1++),
                                                    z + (float)*(ptrs2++),c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
              const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int
                  X = x + (int)cimg::round(*(ptrs0++)),
                  Y = y + (int)cimg::round(*(ptrs1++)),
                  Z = z + (int)cimg::round(*(ptrs2++));
                if (X>=0 && X<width() && Y>=0 && Y<height() && Z>=0 && Z<depth()) res(X,Y,Z,c) = *(ptrs++);
              }
            }
        } else if (mode==2) { // Forward-absolute warp
          res.fill((T)0);
          if (interpolation>=1) // Linear interpolation
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
              const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) res.set_linear_atXYZ(*(ptrs++),(float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),c);
            }
          else // Nearest-neighbor interpolation
            cimg_forYZC(res,y,z,c) {
              const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
              const T *ptrs = data(0,y,z,c);
              cimg_forX(res,x) {
                const int
                  X = (int)cimg::round(*(ptrs0++)),
                  Y = (int)cimg::round(*(ptrs1++)),
                  Z = (int)cimg::round(*(ptrs2++));
                if (X>=0 && X<width() && Y>=0 && Y<height() && Z>=0 && Z<depth()) res(X,Y,Z,c) = *(ptrs++);
              }
            }
        } else if (mode==1) { // Backward-relative warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height(), d2 = 2.f*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod(x - (float)*(ptrs0++),w2),
                    my = cimg::mod(y - (float)*(ptrs1++),h2),
                    mz = cimg::mod(z - (float)*(ptrs2++),d2);
                  *(ptrd++) = _cubic_atXYZ_c(mx<width()?mx:w2 - mx - 1,
                                             my<height()?my:h2 - my - 1,
                                             mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = _cubic_atXYZ_pc(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = _cubic_atXYZ_c(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = cubic_atXYZ_c(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height(), d2 = 2.f*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod(x - (float)*(ptrs0++),w2),
                    my = cimg::mod(y - (float)*(ptrs1++),h2),
                    mz = cimg::mod(z - (float)*(ptrs2++),d2);
                  *(ptrd++) = (T)_linear_atXYZ(mx<width()?mx:w2 - mx - 1,
                                               my<height()?my:h2 - my - 1,
                                               mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXYZ_p(x - (float)*(ptrs0++),y - (float)*(ptrs1++),
                                                                z - (float)*(ptrs2++),c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = (T)_linear_atXYZ(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x)
                  *(ptrd++) = (T)linear_atXYZ(x - (float)*(ptrs0++),y - (float)*(ptrs1++),z - (float)*(ptrs2++),c,(T)0);
              }
            }
          else // Nearest neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width(), h2 = 2*height(), d2 = 2*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int
                    mx = cimg::mod(x - (int)cimg::round(*(ptrs0++)),w2),
                    my = cimg::mod(y - (int)cimg::round(*(ptrs1++)),h2),
                    mz = cimg::mod(z - (int)cimg::round(*(ptrs2++)),d2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,
                                      my<height()?my:h2 - my - 1,
                                      mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod(x - (int)cimg::round(*(ptrs0++)),width()),
                                                     cimg::mod(y - (int)cimg::round(*(ptrs1++)),height()),
                                                     cimg::mod(z - (int)cimg::round(*(ptrs2++)),depth()),c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atXYZ(x - (int)cimg::round(*(ptrs0++)),
                                                    y - (int)cimg::round(*(ptrs1++)),
                                                    z - (int)cimg::round(*(ptrs2++)),c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atXYZ(x - (int)cimg::round(*(ptrs0++)),
                                                   y - (int)cimg::round(*(ptrs1++)),
                                                   z - (int)cimg::round(*(ptrs2++)),c,(T)0);
              }
            }
        } else { // Backward-absolute warp
          if (interpolation==2) // Cubic interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height(), d2 = 2.f*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod((float)*(ptrs0++),w2),
                    my = cimg::mod((float)*(ptrs1++),h2),
                    mz = cimg::mod((float)*(ptrs2++),d2);
                  *(ptrd++) = _cubic_atXYZ_c(mx<width()?mx:w2 - mx - 1,
                                             my<height()?my:h2 - my - 1,
                                             mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXYZ_pc((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _cubic_atXYZ_c((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = cubic_atXYZ_c((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),
                                                           c,(T)0);
              }
            }
          else if (interpolation==1) // Linear interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const float w2 = 2.f*width(), h2 = 2.f*height(), d2 = 2.f*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const float
                    mx = cimg::mod((float)*(ptrs0++),w2),
                    my = cimg::mod((float)*(ptrs1++),h2),
                    mz = cimg::mod((float)*(ptrs2++),d2);
                  *(ptrd++) = (T)_linear_atXYZ(mx<width()?mx:w2 - mx - 1,
                                               my<height()?my:h2 - my - 1,
                                               mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 :// Periodic
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXYZ_p((float)*(ptrs0++),(float)*(ptrs1++),
                                                                (float)*(ptrs2++),c);
              }
              break;
            case 1 : // Neumann
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)_linear_atXYZ((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),c);
              }
              break;
            default : // Dirichlet
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),1048576))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (T)linear_atXYZ((float)*(ptrs0++),(float)*(ptrs1++),(float)*(ptrs2++),
                                                             c,(T)0);
              }
            }
          else // Nearest-neighbor interpolation
            switch (boundary_conditions) {
            case 3 : { // Mirror
              const int w2 = 2*width(), h2 = 2*height(), d2 = 2*depth();
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(res.size(),4096))
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) {
                  const int
                    mx = cimg::mod((int)cimg::round(*(ptrs0++)),w2),
                    my = cimg::mod((int)cimg::round(*(ptrs1++)),h2),
                    mz = cimg::mod((int)cimg::round(*(ptrs2++)),d2);
                  *(ptrd++) = (*this)(mx<width()?mx:w2 - mx - 1,
                                      my<height()?my:h2 - my - 1,
                                      mz<depth()?mz:d2 - mz - 1,c);
                }
              }
            } break;
            case 2 : // Periodic
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = (*this)(cimg::mod((int)cimg::round(*(ptrs0++)),width()),
                                                     cimg::mod((int)cimg::round(*(ptrs1++)),height()),
                                                     cimg::mod((int)cimg::round(*(ptrs2++)),depth()),c);
              }
              break;
            case 1 : // Neumann
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = _atXYZ((int)cimg::round(*(ptrs0++)),
                                                    (int)cimg::round(*(ptrs1++)),
                                                    (int)cimg::round(*(ptrs2++)),c);
              }
              break;
            default : // Dirichlet
              cimg_forYZC(res,y,z,c) {
                const t *ptrs0 = p_warp.data(0,y,z,0), *ptrs1 = p_warp.data(0,y,z,1), *ptrs2 = p_warp.data(0,y,z,2);
                T *ptrd = res.data(0,y,z,c);
                cimg_forX(res,x) *(ptrd++) = atXYZ((int)cimg::round(*(ptrs0++)),
                                                   (int)cimg::round(*(ptrs1++)),
                                                   (int)cimg::round(*(ptrs2++)),c,(T)0);
              }
            }
        }
      }
      return res;
    }

    //! Generate a 2D representation of a 3D image, with XY,XZ and YZ views.
    /**
       \param x0 X-coordinate of the projection point.
       \param y0 Y-coordinate of the projection point.
       \param z0 Z-coordinate of the projection point.
    **/
    CImg<T> get_projections2d(const unsigned int x0, const unsigned int y0, const unsigned int z0) const {
      if (is_empty() || _depth<2) return +*this;
      const unsigned int
        _x0 = (x0>=_width)?_width - 1:x0,
        _y0 = (y0>=_height)?_height - 1:y0,
        _z0 = (z0>=_depth)?_depth - 1:z0;
      const CImg<T>
        img_xy = get_crop(0,0,_z0,0,_width - 1,_height - 1,_z0,_spectrum - 1),
        img_zy = get_crop(_x0,0,0,0,_x0,_height - 1,_depth - 1,_spectrum - 1).permute_axes("xzyc").
        resize(_depth,_height,1,-100,-1),
        img_xz = get_crop(0,_y0,0,0,_width - 1,_y0,_depth - 1,_spectrum - 1).resize(_width,_depth,1,-100,-1);
      return CImg<T>(_width + _depth,_height + _depth,1,_spectrum,cimg::min(img_xy.min(),img_zy.min(),img_xz.min())).
        draw_image(0,0,img_xy).draw_image(img_xy._width,0,img_zy).
        draw_image(0,img_xy._height,img_xz);
    }

#endif
