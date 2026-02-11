#ifndef CIMG_MODULE_IMAGE_BODY_GEOMETRY_CROP_H
#define CIMG_MODULE_IMAGE_BODY_GEOMETRY_CROP_H

    //! Construct a 2D representation of a 3D image, with XY,XZ and YZ views \inplace.
    CImg<T>& projections2d(const unsigned int x0, const unsigned int y0, const unsigned int z0) {
      if (_depth<2) return *this;
      return get_projections2d(x0,y0,z0).move_to(*this);
    }

    //! Crop image region.
    /**
       \param x0 = X-coordinate of the upper-left crop rectangle corner.
       \param y0 = Y-coordinate of the upper-left crop rectangle corner.
       \param z0 = Z-coordinate of the upper-left crop rectangle corner.
       \param c0 = C-coordinate of the upper-left crop rectangle corner.
       \param x1 = X-coordinate of the lower-right crop rectangle corner.
       \param y1 = Y-coordinate of the lower-right crop rectangle corner.
       \param z1 = Z-coordinate of the lower-right crop rectangle corner.
       \param c1 = C-coordinate of the lower-right crop rectangle corner.
       \param boundary_conditions = Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
    **/
    CImg<T>& crop(const int x0, const int y0, const int z0, const int c0,
                  const int x1, const int y1, const int z1, const int c1,
                  const unsigned int boundary_conditions=0) {
      return get_crop(x0,y0,z0,c0,x1,y1,z1,c1,boundary_conditions).move_to(*this);
    }

    //! Crop image region \newinstance.
    CImg<T> get_crop(const int x0, const int y0, const int z0, const int c0,
                     const int x1, const int y1, const int z1, const int c1,
                     const unsigned int boundary_conditions=0) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "crop(): Empty instance.",
                                    cimg_instance);
      const int
        nx0 = x0<x1?x0:x1, nx1 = x0^x1^nx0,
        ny0 = y0<y1?y0:y1, ny1 = y0^y1^ny0,
        nz0 = z0<z1?z0:z1, nz1 = z0^z1^nz0,
        nc0 = c0<c1?c0:c1, nc1 = c0^c1^nc0;
      const unsigned int
        _boundary_conditions = nx0>=0 && nx1<width() &&
                               ny0>=0 && ny1<height() &&
                               nz0>=0 && nz1<depth() &&
                               nc0>=0 && nc1<spectrum()?0:boundary_conditions;
      CImg<T> res(1U + nx1 - nx0,1U + ny1 - ny0,1U + nz1 - nz0,1U + nc1 - nc0);
      if (nx0<0 || nx1>=width() || ny0<0 || ny1>=height() || nz0<0 || nz1>=depth() || nc0<0 || nc1>=spectrum())
        switch (_boundary_conditions) {
        case 3 : { // Mirror
          const int w2 = 2*width(), h2 = 2*height(), d2 = 2*depth(), s2 = 2*spectrum();
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*16 &&
                                                                     _height*_depth*_spectrum>=4))
          cimg_forXYZC(res,x,y,z,c) {
            const int
              mx = cimg::mod(nx0 + x,w2),
              my = cimg::mod(ny0 + y,h2),
              mz = cimg::mod(nz0 + z,d2),
              mc = cimg::mod(nc0 + c,s2);
            res(x,y,z,c) = (*this)(mx<width()?mx:w2 - mx - 1,
                                   my<height()?my:h2 - my - 1,
                                   mz<depth()?mz:d2 - mz - 1,
                                   mc<spectrum()?mc:s2 - mc - 1);
          }
        } break;
        case 2 : { // Periodic
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*16 &&
                                                                     _height*_depth*_spectrum>=4))
          cimg_forXYZC(res,x,y,z,c) {
            res(x,y,z,c) = (*this)(cimg::mod(nx0 + x,width()),cimg::mod(ny0 + y,height()),
                                   cimg::mod(nz0 + z,depth()),cimg::mod(nc0 + c,spectrum()));
          }
        } break;
        case 1 : // Neumann
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*16 &&
                                                                     _height*_depth*_spectrum>=4))
          cimg_forXYZC(res,x,y,z,c) res(x,y,z,c) = _atXYZC(nx0 + x,ny0 + y,nz0 + z,nc0 + c);
          break;
        default : // Dirichlet
          res.fill((T)0).draw_image(-nx0,-ny0,-nz0,-nc0,*this);
        }
      else res.draw_image(-nx0,-ny0,-nz0,-nc0,*this);
      return res;
    }

    //! Crop image region \overloading.
    CImg<T>& crop(const int x0, const int y0, const int z0,
                  const int x1, const int y1, const int z1,
                  const unsigned int boundary_conditions=0) {
      return crop(x0,y0,z0,0,x1,y1,z1,_spectrum - 1,boundary_conditions);
    }

    //! Crop image region \newinstance.
    CImg<T> get_crop(const int x0, const int y0, const int z0,
                     const int x1, const int y1, const int z1,
                     const unsigned int boundary_conditions=0) const {
      return get_crop(x0,y0,z0,0,x1,y1,z1,_spectrum - 1,boundary_conditions);
    }

    //! Crop image region \overloading.
    CImg<T>& crop(const int x0, const int y0,
                  const int x1, const int y1,
                  const unsigned int boundary_conditions=0) {
      return crop(x0,y0,0,0,x1,y1,_depth - 1,_spectrum - 1,boundary_conditions);
    }

    //! Crop image region \newinstance.
    CImg<T> get_crop(const int x0, const int y0,
                     const int x1, const int y1,
                     const unsigned int boundary_conditions=0) const {
      return get_crop(x0,y0,0,0,x1,y1,_depth - 1,_spectrum - 1,boundary_conditions);
    }

    //! Crop image region \overloading.
    CImg<T>& crop(const int x0, const int x1, const unsigned int boundary_conditions=0) {
      return crop(x0,0,0,0,x1,_height - 1,_depth - 1,_spectrum - 1,boundary_conditions);
    }

    //! Crop image region \newinstance.
    CImg<T> get_crop(const int x0, const int x1, const unsigned int boundary_conditions=0) const {
      return get_crop(x0,0,0,0,x1,_height - 1,_depth - 1,_spectrum - 1,boundary_conditions);
    }

    //! Autocrop image region, regarding the specified background value.
    CImg<T>& autocrop(const T& value, const char *const axes="czyx") {
      if (is_empty()) return *this;
      for (const char *s = axes; *s; ++s) {
        const char axis = cimg::lowercase(*s);
        const CImg<intT> coords = _autocrop(value,axis);
        if (coords[0]==-1 && coords[1]==-1) return assign(); // Image has only 'value' pixels
        else switch (axis) {
        case 'x' : {
          const int x0 = coords[0], x1 = coords[1];
          if (x0>=0 && x1>=0) crop(x0,x1);
        } break;
        case 'y' : {
          const int y0 = coords[0], y1 = coords[1];
          if (y0>=0 && y1>=0) crop(0,y0,_width - 1,y1);
        } break;
        case 'z' : {
          const int z0 = coords[0], z1 = coords[1];
          if (z0>=0 && z1>=0) crop(0,0,z0,_width - 1,_height - 1,z1);
        } break;
        default : {
          const int c0 = coords[0], c1 = coords[1];
          if (c0>=0 && c1>=0) crop(0,0,0,c0,_width - 1,_height - 1,_depth - 1,c1);
        }
        }
      }
      return *this;
    }

    //! Autocrop image region, regarding the specified background value \newinstance.
    CImg<T> get_autocrop(const T& value, const char *const axes="czyx") const {
      return (+*this).autocrop(value,axes);
    }

    //! Autocrop image region, regarding the specified background color.
    /**
       \param color Color used for the crop. If \c 0, color is guessed.
       \param axes Axes used for the crop.
    **/
    CImg<T>& autocrop(const T *const color=0, const char *const axes="zyx") {
      if (is_empty()) return *this;
      if (!color) { // Guess color
        const CImg<T> col1 = get_vector_at(0,0,0);
        const unsigned int w = _width, h = _height, d = _depth, s = _spectrum;
        autocrop(col1,axes);
        if (_width==w && _height==h && _depth==d && _spectrum==s) {
          const CImg<T> col2 = get_vector_at(w - 1,h - 1,d - 1);
          autocrop(col2,axes);
        }
        return *this;
      }
      for (const char *s = axes; *s; ++s) {
        const char axis = cimg::lowercase(*s);
        switch (axis) {
        case 'x' : {
          int x0 = width(), x1 = -1;
          cimg_forC(*this,c) {
            const CImg<intT> coords = get_shared_channel(c)._autocrop(color[c],'x');
            const int nx0 = coords[0], nx1 = coords[1];
            if (nx0>=0 && nx1>=0) { x0 = std::min(x0,nx0); x1 = std::max(x1,nx1); }
          }
          if (x0==width() && x1==-1) return assign(); else crop(x0,x1);
        } break;
        case 'y' : {
          int y0 = height(), y1 = -1;
          cimg_forC(*this,c) {
            const CImg<intT> coords = get_shared_channel(c)._autocrop(color[c],'y');
            const int ny0 = coords[0], ny1 = coords[1];
            if (ny0>=0 && ny1>=0) { y0 = std::min(y0,ny0); y1 = std::max(y1,ny1); }
          }
          if (y0==height() && y1==-1) return assign(); else crop(0,y0,_width - 1,y1);
        } break;
        default : {
          int z0 = depth(), z1 = -1;
          cimg_forC(*this,c) {
            const CImg<intT> coords = get_shared_channel(c)._autocrop(color[c],'z');
            const int nz0 = coords[0], nz1 = coords[1];
            if (nz0>=0 && nz1>=0) { z0 = std::min(z0,nz0); z1 = std::max(z1,nz1); }
          }
          if (z0==depth() && z1==-1) return assign(); else crop(0,0,z0,_width - 1,_height - 1,z1);
        }
        }
      }
      return *this;
    }

    //! Autocrop image region, regarding the specified background color \newinstance.
    CImg<T> get_autocrop(const T *const color=0, const char *const axes="zyx") const {
      return (+*this).autocrop(color,axes);
    }

    CImg<intT> _autocrop(const T& value, const char axis) const {
      CImg<intT> res;
      switch (cimg::lowercase(axis)) {
      case 'x' : {
        int x0 = -1, x1 = -1;
        cimg_forX(*this,x) cimg_forYZC(*this,y,z,c)
          if ((*this)(x,y,z,c)!=value) { x0 = x; x = width(); y = height(); z = depth(); c = spectrum(); }
        if (x0>=0) {
          for (int x = width() - 1; x>=0; --x) cimg_forYZC(*this,y,z,c)
            if ((*this)(x,y,z,c)!=value) { x1 = x; x = 0; y = height(); z = depth(); c = spectrum(); }
        }
        res = CImg<intT>::vector(x0,x1);
      } break;
      case 'y' : {
        int y0 = -1, y1 = -1;
        cimg_forY(*this,y) cimg_forXZC(*this,x,z,c)
          if ((*this)(x,y,z,c)!=value) { y0 = y; x = width(); y = height(); z = depth(); c = spectrum(); }
        if (y0>=0) {
          for (int y = height() - 1; y>=0; --y) cimg_forXZC(*this,x,z,c)
            if ((*this)(x,y,z,c)!=value) { y1 = y; x = width(); y = 0; z = depth(); c = spectrum(); }
        }
        res = CImg<intT>::vector(y0,y1);
      } break;
      case 'z' : {
        int z0 = -1, z1 = -1;
        cimg_forZ(*this,z) cimg_forXYC(*this,x,y,c)
          if ((*this)(x,y,z,c)!=value) { z0 = z; x = width(); y = height(); z = depth(); c = spectrum(); }
        if (z0>=0) {
          for (int z = depth() - 1; z>=0; --z) cimg_forXYC(*this,x,y,c)
            if ((*this)(x,y,z,c)!=value) { z1 = z; x = width(); y = height(); z = 0; c = spectrum(); }
        }
        res = CImg<intT>::vector(z0,z1);
      } break;
      default : {
        int c0 = -1, c1 = -1;
        cimg_forC(*this,c) cimg_forXYZ(*this,x,y,z)
          if ((*this)(x,y,z,c)!=value) { c0 = c; x = width(); y = height(); z = depth(); c = spectrum(); }
        if (c0>=0) {
          for (int c = spectrum() - 1; c>=0; --c) cimg_forXYZ(*this,x,y,z)
            if ((*this)(x,y,z,c)!=value) { c1 = c; x = width(); y = height(); z = depth(); c = 0; }
        }
        res = CImg<intT>::vector(c0,c1);
      }
      }
      return res;
    }

    //! Return specified image column.
    /**
       \param x0 Image column.
    **/
    CImg<T> get_column(const int x0) const {
      return get_columns(x0,x0);
    }

    //! Return specified image column \inplace.
    CImg<T>& column(const int x0) {
      return columns(x0,x0);
    }

    //! Return specified range of image columns.
    /**
       \param x0 Starting image column.
       \param x1 Ending image column.
    **/
    CImg<T>& columns(const int x0, const int x1) {
      return get_columns(x0,x1).move_to(*this);
    }

    //! Return specified range of image columns \inplace.
    CImg<T> get_columns(const int x0, const int x1) const {
      return get_crop(x0,0,0,0,x1,height() - 1,depth() - 1,spectrum() - 1);
    }

    //! Return specified image row.
    CImg<T> get_row(const int y0) const {
      return get_rows(y0,y0);
    }

    //! Return specified image row \inplace.
    /**
       \param y0 Image row.
    **/
    CImg<T>& row(const int y0) {
      return rows(y0,y0);
    }

    //! Return specified range of image rows.
    /**
       \param y0 Starting image row.
       \param y1 Ending image row.
    **/
    CImg<T> get_rows(const int y0, const int y1) const {
      return get_crop(0,y0,0,0,width() - 1,y1,depth() - 1,spectrum() - 1);
    }

    //! Return specified range of image rows \inplace.
    CImg<T>& rows(const int y0, const int y1) {
      return get_rows(y0,y1).move_to(*this);
    }

    //! Return specified image slice.
    /**
       \param z0 Image slice.
    **/
    CImg<T> get_slice(const int z0) const {
      return get_slices(z0,z0);
    }

    //! Return specified image slice \inplace.
    CImg<T>& slice(const int z0) {
      return slices(z0,z0);
    }

    //! Return specified range of image slices.
    /**
       \param z0 Starting image slice.
       \param z1 Ending image slice.
    **/
    CImg<T> get_slices(const int z0, const int z1) const {
      return get_crop(0,0,z0,0,width() - 1,height() - 1,z1,spectrum() - 1);
    }

    //! Return specified range of image slices \inplace.
    CImg<T>& slices(const int z0, const int z1) {
      return get_slices(z0,z1).move_to(*this);
    }

    //! Return specified image channel.
    /**
       \param c0 Image channel.
    **/
    CImg<T> get_channel(const int c0) const {
      return get_channels(c0,c0);
    }

    //! Return specified image channel \inplace.
    CImg<T>& channel(const int c0) {
      return channels(c0,c0);
    }

    //! Return specified range of image channels.
    /**
       \param c0 Starting image channel.
       \param c1 Ending image channel.
    **/
    CImg<T> get_channels(const int c0, const int c1) const {
      return get_crop(0,0,0,c0,width() - 1,height() - 1,depth() - 1,c1);
    }

    //! Return specified range of image channels \inplace.
    CImg<T>& channels(const int c0, const int c1) {
      return get_channels(c0,c1).move_to(*this);
    }

    //! Return stream line of a 2D or 3D vector field.
    CImg<floatT> get_streamline(const float x, const float y, const float z,
                                const float L=256, const float dl=0.1f,
                                const unsigned int interpolation_type=2, const bool is_backward_tracking=false,
                                const bool is_oriented_only=false) const {
      if (_spectrum!=2 && _spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "streamline(): Instance is not a 2D or 3D vector field.",
                                    cimg_instance);
      if (_spectrum==2) {
        if (is_oriented_only) {
          typename CImg<T>::_functor4d_streamline2d_oriented func(*this);
          return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,true,
                            0,0,0,_width - 1.f,_height - 1.f,0.f);
        } else {
          typename CImg<T>::_functor4d_streamline2d_directed func(*this);
          return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,false,
                            0,0,0,_width - 1.f,_height - 1.f,0.f);
        }
      }
      if (is_oriented_only) {
        typename CImg<T>::_functor4d_streamline3d_oriented func(*this);
        return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,true,
                          0,0,0,_width - 1.f,_height - 1.f,_depth - 1.f);
      }
      typename CImg<T>::_functor4d_streamline3d_directed func(*this);
      return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,false,
                        0,0,0,_width - 1.f,_height - 1.f,_depth - 1.f);
    }

    //! Return stream line of a 3D vector field.
    /**
       \param func Vector field function.
       \param x X-coordinate of the starting point of the streamline.
       \param y Y-coordinate of the starting point of the streamline.
       \param z Z-coordinate of the starting point of the streamline.
       \param L Streamline length.
       \param dl Streamline length increment.
       \param interpolation_type Type of interpolation.
         Can be <tt>{ 0=nearest int | 1=linear | 2=2nd-order RK | 3=4th-order RK. }</tt>.
       \param is_backward_tracking Tells if the streamline is estimated forward or backward.
       \param is_oriented_only Tells if the direction of the vectors must be ignored.
       \param x0 X-coordinate of the first bounding-box vertex.
       \param y0 Y-coordinate of the first bounding-box vertex.
       \param z0 Z-coordinate of the first bounding-box vertex.
       \param x1 X-coordinate of the second bounding-box vertex.
       \param y1 Y-coordinate of the second bounding-box vertex.
       \param z1 Z-coordinate of the second bounding-box vertex.
    **/
    template<typename tfunc>
    static CImg<floatT> streamline(const tfunc& func,
                                   const float x, const float y, const float z,
                                   const float L=256, const float dl=0.1f,
                                   const unsigned int interpolation_type=2, const bool is_backward_tracking=false,
                                   const bool is_oriented_only=false,
                                   const float x0=0, const float y0=0, const float z0=0,
                                   const float x1=0, const float y1=0, const float z1=0) {
      if (dl<=0)
        throw CImgArgumentException("CImg<%s>::streamline(): Invalid specified integration length %g "
                                    "(should be >0).",
                                    pixel_type(),
                                    dl);

      const bool is_bounded = (x0!=x1 || y0!=y1 || z0!=z1);
      if (L<=0 || (is_bounded && (x<x0 || x>x1 || y<y0 || y>y1 || z<z0 || z>z1))) return CImg<floatT>();
      const unsigned int size_L = (unsigned int)cimg::round(L/dl + 1);
      CImg<floatT> coordinates(size_L,3);
      const float dl2 = dl/2;
      float
        *ptr_x = coordinates.data(0,0),
        *ptr_y = coordinates.data(0,1),
        *ptr_z = coordinates.data(0,2),
        pu = (float)(dl*func(x,y,z,0)),
        pv = (float)(dl*func(x,y,z,1)),
        pw = (float)(dl*func(x,y,z,2)),
        X = x, Y = y, Z = z;

      switch (interpolation_type) {
      case 0 : { // Nearest integer interpolation
        cimg_forX(coordinates,l) {
          *(ptr_x++) = X; *(ptr_y++) = Y; *(ptr_z++) = Z;
          const int
            xi = (int)(X>0?X + 0.5f:X - 0.5f),
            yi = (int)(Y>0?Y + 0.5f:Y - 0.5f),
            zi = (int)(Z>0?Z + 0.5f:Z - 0.5f);
          float
            u = (float)(dl*func((float)xi,(float)yi,(float)zi,0)),
            v = (float)(dl*func((float)xi,(float)yi,(float)zi,1)),
            w = (float)(dl*func((float)xi,(float)yi,(float)zi,2));
          if (is_oriented_only && u*pu + v*pv + w*pw<0) { u = -u; v = -v; w = -w; }
          if (is_backward_tracking) { X-=(pu=u); Y-=(pv=v); Z-=(pw=w); } else { X+=(pu=u); Y+=(pv=v); Z+=(pw=w); }
          if (is_bounded && (X<x0 || X>x1 || Y<y0 || Y>y1 || Z<z0 || Z>z1)) break;
        }
      } break;
      case 1 : { // First-order interpolation
        cimg_forX(coordinates,l) {
          *(ptr_x++) = X; *(ptr_y++) = Y; *(ptr_z++) = Z;
          float
            u = (float)(dl*func(X,Y,Z,0)),
            v = (float)(dl*func(X,Y,Z,1)),
            w = (float)(dl*func(X,Y,Z,2));
          if (is_oriented_only && u*pu + v*pv + w*pw<0) { u = -u; v = -v; w = -w; }
          if (is_backward_tracking) { X-=(pu=u); Y-=(pv=v); Z-=(pw=w); } else { X+=(pu=u); Y+=(pv=v); Z+=(pw=w); }
          if (is_bounded && (X<x0 || X>x1 || Y<y0 || Y>y1 || Z<z0 || Z>z1)) break;
        }
      } break;
      case 2 : { // Second order interpolation
        cimg_forX(coordinates,l) {
          *(ptr_x++) = X; *(ptr_y++) = Y; *(ptr_z++) = Z;
          float
            u0 = (float)(dl2*func(X,Y,Z,0)),
            v0 = (float)(dl2*func(X,Y,Z,1)),
            w0 = (float)(dl2*func(X,Y,Z,2));
          if (is_oriented_only && u0*pu + v0*pv + w0*pw<0) { u0 = -u0; v0 = -v0; w0 = -w0; }
          float
            u = (float)(dl*func(X + u0,Y + v0,Z + w0,0)),
            v = (float)(dl*func(X + u0,Y + v0,Z + w0,1)),
            w = (float)(dl*func(X + u0,Y + v0,Z + w0,2));
          if (is_oriented_only && u*pu + v*pv + w*pw<0) { u = -u; v = -v; w = -w; }
          if (is_backward_tracking) { X-=(pu=u); Y-=(pv=v); Z-=(pw=w); } else { X+=(pu=u); Y+=(pv=v); Z+=(pw=w); }
          if (is_bounded && (X<x0 || X>x1 || Y<y0 || Y>y1 || Z<z0 || Z>z1)) break;
        }
      } break;
      default : { // Fourth order interpolation
        cimg_forX(coordinates,k) {
          *(ptr_x++) = X; *(ptr_y++) = Y; *(ptr_z++) = Z;
          float
            u0 = (float)(dl2*func(X,Y,Z,0)),
            v0 = (float)(dl2*func(X,Y,Z,1)),
            w0 = (float)(dl2*func(X,Y,Z,2));
          if (is_oriented_only && u0*pu + v0*pv + w0*pw<0) { u0 = -u0; v0 = -v0; w0 = -w0; }
          float
            u1 = (float)(dl2*func(X + u0,Y + v0,Z + w0,0)),
            v1 = (float)(dl2*func(X + u0,Y + v0,Z + w0,1)),
            w1 = (float)(dl2*func(X + u0,Y + v0,Z + w0,2));
          if (is_oriented_only && u1*pu + v1*pv + w1*pw<0) { u1 = -u1; v1 = -v1; w1 = -w1; }
          float
            u2 = (float)(dl2*func(X + u1,Y + v1,Z + w1,0)),
            v2 = (float)(dl2*func(X + u1,Y + v1,Z + w1,1)),
            w2 = (float)(dl2*func(X + u1,Y + v1,Z + w1,2));
          if (is_oriented_only && u2*pu + v2*pv + w2*pw<0) { u2 = -u2; v2 = -v2; w2 = -w2; }
          float
            u3 = (float)(dl2*func(X + u2,Y + v2,Z + w2,0)),
            v3 = (float)(dl2*func(X + u2,Y + v2,Z + w2,1)),
            w3 = (float)(dl2*func(X + u2,Y + v2,Z + w2,2));
          if (is_oriented_only && u2*pu + v2*pv + w2*pw<0) { u3 = -u3; v3 = -v3; w3 = -w3; }
          const float
            u = (u0 + u3)/3 + (u1 + u2)/1.5f,
            v = (v0 + v3)/3 + (v1 + v2)/1.5f,
            w = (w0 + w3)/3 + (w1 + w2)/1.5f;
          if (is_backward_tracking) { X-=(pu=u); Y-=(pv=v); Z-=(pw=w); } else { X+=(pu=u); Y+=(pv=v); Z+=(pw=w); }
          if (is_bounded && (X<x0 || X>x1 || Y<y0 || Y>y1 || Z<z0 || Z>z1)) break;
        }
      }
      }
      if (ptr_x!=coordinates.data(0,1)) coordinates.resize((int)(ptr_x-coordinates.data()),3,1,1,0);
      return coordinates;
    }

    //! Return stream line of a 3D vector field \overloading.
    static CImg<floatT> streamline(const char *const expression,
                                   const float x, const float y, const float z,
                                   const float L=256, const float dl=0.1f,
                                   const unsigned int interpolation_type=2, const bool is_backward_tracking=true,
                                   const bool is_oriented_only=false,
                                   const float x0=0, const float y0=0, const float z0=0,
                                   const float x1=0, const float y1=0, const float z1=0) {
      _functor4d_streamline_expr func(expression);
      return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,is_oriented_only,x0,y0,z0,x1,y1,z1);
    }

    struct _functor4d_streamline2d_directed {
      const CImg<T>& ref;
      _functor4d_streamline2d_directed(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
        return c<2?(float)ref._linear_atXY(x,y,(int)z,c):0;
      }
    };

    struct _functor4d_streamline3d_directed {
      const CImg<T>& ref;
      _functor4d_streamline3d_directed(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
        return (float)ref._linear_atXYZ(x,y,z,c);
      }
    };

    struct _functor4d_streamline2d_oriented {
      const CImg<T>& ref;
      CImg<floatT> *pI;
      _functor4d_streamline2d_oriented(const CImg<T>& pref):ref(pref),pI(0) { pI = new CImg<floatT>(2,2,1,2); }
      ~_functor4d_streamline2d_oriented() { delete pI; }
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
#define _cimg_vecalign2d(i,j) \
        if (I(i,j,0)*I(0,0,0) + I(i,j,1)*I(0,0,1)<0) { I(i,j,0) = -I(i,j,0); I(i,j,1) = -I(i,j,1); }
        int
          xi = (int)x - (x>=0?0:1), nxi = xi + 1,
          yi = (int)y - (y>=0?0:1), nyi = yi + 1,
          zi = (int)z;
        const float
          dx = x - xi,
          dy = y - yi;
        if (c==0) {
          CImg<floatT>& I = *pI;
          if (xi<0) xi = 0;
          if (nxi<0) nxi = 0;
          if (xi>=ref.width()) xi = ref.width() - 1;
          if (nxi>=ref.width()) nxi = ref.width() - 1;
          if (yi<0) yi = 0;
          if (nyi<0) nyi = 0;
          if (yi>=ref.height()) yi = ref.height() - 1;
          if (nyi>=ref.height()) nyi = ref.height() - 1;
          I(0,0,0) = (float)ref(xi,yi,zi,0);   I(0,0,1) = (float)ref(xi,yi,zi,1);
          I(1,0,0) = (float)ref(nxi,yi,zi,0);  I(1,0,1) = (float)ref(nxi,yi,zi,1);
          I(1,1,0) = (float)ref(nxi,nyi,zi,0); I(1,1,1) = (float)ref(nxi,nyi,zi,1);
          I(0,1,0) = (float)ref(xi,nyi,zi,0);  I(0,1,1) = (float)ref(xi,nyi,zi,1);
          _cimg_vecalign2d(1,0); _cimg_vecalign2d(1,1); _cimg_vecalign2d(0,1);
        }
        return c<2?(float)pI->_linear_atXY(dx,dy,0,c):0;
      }
    };

    struct _functor4d_streamline3d_oriented {
      const CImg<T>& ref;
      CImg<floatT> *pI;
      _functor4d_streamline3d_oriented(const CImg<T>& pref):ref(pref),pI(0) { pI = new CImg<floatT>(2,2,2,3); }
      ~_functor4d_streamline3d_oriented() { delete pI; }
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
#define _cimg_vecalign3d(i,j,k) if (I(i,j,k,0)*I(0,0,0,0) + I(i,j,k,1)*I(0,0,0,1) + I(i,j,k,2)*I(0,0,0,2)<0) { \
  I(i,j,k,0) = -I(i,j,k,0); I(i,j,k,1) = -I(i,j,k,1); I(i,j,k,2) = -I(i,j,k,2); }
        int
          xi = (int)x - (x>=0?0:1), nxi = xi + 1,
          yi = (int)y - (y>=0?0:1), nyi = yi + 1,
          zi = (int)z - (z>=0?0:1), nzi = zi + 1;
        const float
          dx = x - xi,
          dy = y - yi,
          dz = z - zi;
        if (c==0) {
          CImg<floatT>& I = *pI;
          if (xi<0) xi = 0;
          if (nxi<0) nxi = 0;
          if (xi>=ref.width()) xi = ref.width() - 1;
          if (nxi>=ref.width()) nxi = ref.width() - 1;
          if (yi<0) yi = 0;
          if (nyi<0) nyi = 0;
          if (yi>=ref.height()) yi = ref.height() - 1;
          if (nyi>=ref.height()) nyi = ref.height() - 1;
          if (zi<0) zi = 0;
          if (nzi<0) nzi = 0;
          if (zi>=ref.depth()) zi = ref.depth() - 1;
          if (nzi>=ref.depth()) nzi = ref.depth() - 1;
          I(0,0,0,0) = (float)ref(xi,yi,zi,0); I(0,0,0,1) = (float)ref(xi,yi,zi,1);
          I(0,0,0,2) = (float)ref(xi,yi,zi,2); I(1,0,0,0) = (float)ref(nxi,yi,zi,0);
          I(1,0,0,1) = (float)ref(nxi,yi,zi,1); I(1,0,0,2) = (float)ref(nxi,yi,zi,2);
          I(1,1,0,0) = (float)ref(nxi,nyi,zi,0); I(1,1,0,1) = (float)ref(nxi,nyi,zi,1);
          I(1,1,0,2) = (float)ref(nxi,nyi,zi,2); I(0,1,0,0) = (float)ref(xi,nyi,zi,0);
          I(0,1,0,1) = (float)ref(xi,nyi,zi,1); I(0,1,0,2) = (float)ref(xi,nyi,zi,2);
          I(0,0,1,0) = (float)ref(xi,yi,nzi,0); I(0,0,1,1) = (float)ref(xi,yi,nzi,1);
          I(0,0,1,2) = (float)ref(xi,yi,nzi,2); I(1,0,1,0) = (float)ref(nxi,yi,nzi,0);
          I(1,0,1,1) = (float)ref(nxi,yi,nzi,1);  I(1,0,1,2) = (float)ref(nxi,yi,nzi,2);
          I(1,1,1,0) = (float)ref(nxi,nyi,nzi,0); I(1,1,1,1) = (float)ref(nxi,nyi,nzi,1);
          I(1,1,1,2) = (float)ref(nxi,nyi,nzi,2); I(0,1,1,0) = (float)ref(xi,nyi,nzi,0);
          I(0,1,1,1) = (float)ref(xi,nyi,nzi,1);  I(0,1,1,2) = (float)ref(xi,nyi,nzi,2);
          _cimg_vecalign3d(1,0,0); _cimg_vecalign3d(1,1,0); _cimg_vecalign3d(0,1,0);
          _cimg_vecalign3d(0,0,1); _cimg_vecalign3d(1,0,1); _cimg_vecalign3d(1,1,1); _cimg_vecalign3d(0,1,1);
        }
        return (float)pI->_linear_atXYZ(dx,dy,dz,c);
      }
    };

    struct _functor4d_streamline_expr {
      _cimg_math_parser *mp;
      ~_functor4d_streamline_expr() { mp->end(); delete mp; }
      _functor4d_streamline_expr(const char *const expr):mp(0) {
        mp = new _cimg_math_parser(expr,"streamline",CImg<T>::const_empty(),0);
      }
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
        return (float)(*mp)(x,y,z,c);
      }
    };

    //! Return a shared-memory image referencing a range of pixels of the image instance.
    /**
       \param x0 X-coordinate of the starting pixel.
       \param x1 X-coordinate of the ending pixel.
       \param y0 Y-coordinate.
       \param z0 Z-coordinate.
       \param c0 C-coordinate.
     **/
    CImg<T> get_shared_points(const unsigned int x0, const unsigned int x1,
                              const unsigned int y0=0, const unsigned int z0=0, const unsigned int c0=0) {
      const ulongT
        beg = (ulongT)offset(x0,y0,z0,c0),
        end = (ulongT)offset(x1,y0,z0,c0);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_points(): Invalid request of a shared-memory subset (%u->%u,%u,%u,%u).",
                                    cimg_instance,
                                    x0,x1,y0,z0,c0);
      return CImg<T>(_data + beg,x1 - x0 + 1,1,1,1,true);
    }

    //! Return a shared-memory image referencing a range of pixels of the image instance \const.
    const CImg<T> get_shared_points(const unsigned int x0, const unsigned int x1,
                                    const unsigned int y0=0, const unsigned int z0=0, const unsigned int c0=0) const {
      const ulongT
        beg = (ulongT)offset(x0,y0,z0,c0),
        end = (ulongT)offset(x1,y0,z0,c0);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_points(): Invalid request of a shared-memory subset (%u->%u,%u,%u,%u).",
                                    cimg_instance,
                                    x0,x1,y0,z0,c0);
      return CImg<T>(_data + beg,x1 - x0 + 1,1,1,1,true);
    }

    //! Return a shared-memory image referencing a range of rows of the image instance.
    /**
       \param y0 Y-coordinate of the starting row.
       \param y1 Y-coordinate of the ending row.
       \param z0 Z-coordinate.
       \param c0 C-coordinate.
    **/
    CImg<T> get_shared_rows(const unsigned int y0, const unsigned int y1,
                             const unsigned int z0=0, const unsigned int c0=0) {
      const ulongT
        beg = (ulongT)offset(0,y0,z0,c0),
        end = (ulongT)offset(0,y1,z0,c0);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_rows(): Invalid request of a shared-memory subset "
                                    "(0->%u,%u->%u,%u,%u).",
                                    cimg_instance,
                                    _width - 1,y0,y1,z0,c0);
      return CImg<T>(_data + beg,_width,y1 - y0 + 1,1,1,true);
    }

    //! Return a shared-memory image referencing a range of rows of the image instance \const.
    const CImg<T> get_shared_rows(const unsigned int y0, const unsigned int y1,
                                   const unsigned int z0=0, const unsigned int c0=0) const {
      const ulongT
        beg = (ulongT)offset(0,y0,z0,c0),
        end = (ulongT)offset(0,y1,z0,c0);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_rows(): Invalid request of a shared-memory subset "
                                    "(0->%u,%u->%u,%u,%u).",
                                    cimg_instance,
                                    _width - 1,y0,y1,z0,c0);
      return CImg<T>(_data + beg,_width,y1 - y0 + 1,1,1,true);
    }

    //! Return a shared-memory image referencing one row of the image instance.
    /**
       \param y0 Y-coordinate.
       \param z0 Z-coordinate.
       \param c0 C-coordinate.
    **/
    CImg<T> get_shared_row(const unsigned int y0, const unsigned int z0=0, const unsigned int c0=0) {
      return get_shared_rows(y0,y0,z0,c0);
    }

    //! Return a shared-memory image referencing one row of the image instance \const.
    const CImg<T> get_shared_row(const unsigned int y0, const unsigned int z0=0, const unsigned int c0=0) const {
      return get_shared_rows(y0,y0,z0,c0);
    }

    //! Return a shared memory image referencing a range of slices of the image instance.
    /**
       \param z0 Z-coordinate of the starting slice.
       \param z1 Z-coordinate of the ending slice.
       \param c0 C-coordinate.
    **/
    CImg<T> get_shared_slices(const unsigned int z0, const unsigned int z1, const unsigned int c0=0) {
      const ulongT
        beg = (ulongT)offset(0,0,z0,c0),
        end = (ulongT)offset(0,0,z1,c0);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_slices(): Invalid request of a shared-memory subset "
                                    "(0->%u,0->%u,%u->%u,%u).",
                                    cimg_instance,
                                    _width - 1,_height - 1,z0,z1,c0);
      return CImg<T>(_data + beg,_width,_height,z1 - z0 + 1,1,true);
    }

    //! Return a shared memory image referencing a range of slices of the image instance \const.
    const CImg<T> get_shared_slices(const unsigned int z0, const unsigned int z1, const unsigned int c0=0) const {
      const ulongT
        beg = (ulongT)offset(0,0,z0,c0),
        end = (ulongT)offset(0,0,z1,c0);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_slices(): Invalid request of a shared-memory subset "
                                    "(0->%u,0->%u,%u->%u,%u).",
                                    cimg_instance,
                                    _width - 1,_height - 1,z0,z1,c0);
      return CImg<T>(_data + beg,_width,_height,z1 - z0 + 1,1,true);
    }

    //! Return a shared-memory image referencing one slice of the image instance.
    /**
       \param z0 Z-coordinate.
       \param c0 C-coordinate.
    **/
    CImg<T> get_shared_slice(const unsigned int z0, const unsigned int c0=0) {
      return get_shared_slices(z0,z0,c0);
    }

    //! Return a shared-memory image referencing one slice of the image instance \const.
    const CImg<T> get_shared_slice(const unsigned int z0, const unsigned int c0=0) const {
      return get_shared_slices(z0,z0,c0);
    }

    //! Return a shared-memory image referencing a range of channels of the image instance.
    /**
       \param c0 C-coordinate of the starting channel.
       \param c1 C-coordinate of the ending channel.
    **/
    CImg<T> get_shared_channels(const unsigned int c0, const unsigned int c1) {
      const ulongT
        beg = (ulongT)offset(0,0,0,c0),
        end = (ulongT)offset(0,0,0,c1);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_channels(): Invalid request of a shared-memory subset "
                                    "(0->%u,0->%u,0->%u,%u->%u).",
                                    cimg_instance,
                                    _width - 1,_height - 1,_depth - 1,c0,c1);
      return CImg<T>(_data + beg,_width,_height,_depth,c1 - c0 + 1,true);
    }

    //! Return a shared-memory image referencing a range of channels of the image instance \const.
    const CImg<T> get_shared_channels(const unsigned int c0, const unsigned int c1) const {
      const ulongT
        beg = (ulongT)offset(0,0,0,c0),
        end = (ulongT)offset(0,0,0,c1);
      if (beg>end || beg>=size() || end>=size())
        throw CImgArgumentException(_cimg_instance
                                    "get_shared_channels(): Invalid request of a shared-memory subset "
                                    "(0->%u,0->%u,0->%u,%u->%u).",
                                    cimg_instance,
                                    _width - 1,_height - 1,_depth - 1,c0,c1);
      return CImg<T>(_data + beg,_width,_height,_depth,c1 - c0 + 1,true);
    }

    //! Return a shared-memory image referencing one channel of the image instance.
    /**
       \param c0 C-coordinate.
    **/
    CImg<T> get_shared_channel(const unsigned int c0) {
      return get_shared_channels(c0,c0);
    }

    //! Return a shared-memory image referencing one channel of the image instance \const.
    const CImg<T> get_shared_channel(const unsigned int c0) const {
      return get_shared_channels(c0,c0);
    }

    //! Return a shared-memory version of the image instance.
    CImg<T> get_shared() {
      return CImg<T>(_data,_width,_height,_depth,_spectrum,true);
    }

    //! Return a shared-memory version of the image instance \const.
    const CImg<T> get_shared() const {
      return CImg<T>(_data,_width,_height,_depth,_spectrum,true);
    }

    //! Split image into a list along specified axis.
    /**
       \param axis Splitting axis. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
       \param nb Number of split parts.
       \param max_parts Number of max parts allowed for the split.
       \note
       - If \c nb==0, instance image is split into blocks of equal values along the specified axis.
       - If \c nb>0, instance image is split into \c nb blocks.
       - If \c nb<0, instance image is split into blocks of -\c nb pixel wide.
    **/
    CImgList<T> get_split(const char axis, const int nb=-1, const unsigned int max_parts=~0U) const {
      CImgList<T> res;
      if (is_empty() || !max_parts) return res;
      if (max_parts==1) return res.assign(*this);
      const char _axis = cimg::lowercase(axis);
      const unsigned int mp1 = max_parts - 1;

      if (nb<0) { // Split by block size
        const unsigned int dp = (unsigned int)-nb;
        switch (_axis) {
        case 'x': {
          if (max_parts==1 || dp>=_width) res.assign(*this);
          else {
            res.assign(std::min(max_parts,_width/dp + (_width%dp?1:0)));
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*128 &&
                                                           _height*_depth*_spectrum>=128))
            cimglist_for(res,p)
              if (p!=res.width() - 1) get_columns(p*dp,(p + 1)*dp - 1).move_to(res[p]);
              else get_columns(p*dp,_width - 1).move_to(res[p]);
          }
        } break;
        case 'y': {
          if (max_parts==1 || dp>=_height) res.assign(*this);
          else {
            res.assign(std::min(max_parts,_height/dp + (_height%dp?1:0)));
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*128 &&
                                                           _width*_depth*_spectrum>=128))
            cimglist_for(res,p)
              if (p!=res.width() - 1) get_rows(p*dp,(p + 1)*dp - 1).move_to(res[p]);
              else get_rows(p*dp,_height - 1).move_to(res[p]);
          }
        } break;
        case 'z': {
          if (max_parts==1 || dp>=_depth) res.assign(*this);
          else {
            res.assign(std::min(max_parts,_depth/dp + (_depth%dp?1:0)));
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*128 &&
                                                           _width*_height*_spectrum>=128))
            cimglist_for(res,p)
              if (p!=res.width() - 1) get_slices(p*dp,(p + 1)*dp - 1).move_to(res[p]);
              else get_slices(p*dp,_depth - 1).move_to(res[p]);
          }
        } break;
        case 'c' : {
          if (max_parts==1 || dp>=_spectrum) res.assign(*this);
          else {
            res.assign(std::min(max_parts,_spectrum/dp + (_spectrum%dp?1:0)));
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*128 &&
                                                           _width*_height*_depth>=128))
            cimglist_for(res,p)
              if (p!=res.width() - 1) get_channels(p*dp,(p + 1)*dp - 1).move_to(res[p]);
              else get_channels(p*dp,_spectrum - 1).move_to(res[p]);
          }
        }
        }
      } else if (nb>0) { // Split by number of (non-homogeneous) blocks
        const unsigned int siz = _axis=='x'?_width:_axis=='y'?_height:_axis=='z'?_depth:_axis=='c'?_spectrum:0;
        if ((unsigned int)nb>siz)
          throw CImgArgumentException(_cimg_instance
                                      "get_split(): Instance cannot be split along %c-axis into %u blocks.",
                                      cimg_instance,
                                      axis,nb);
        if (nb==1) res.assign(*this);
        else {
          int err = (int)siz;
          unsigned int _p = 0;
          switch (_axis) {
          case 'x' :
            cimg_forX(*this,p) if ((err-=nb)<=0) {
              get_columns(_p,p).move_to(res);
              if (res._width>=mp1) { get_columns(p,_width - 1).move_to(res); break; }
              err+=(int)siz; _p = p + 1U;
            }
            break;
          case 'y' :
            cimg_forY(*this,p) if ((err-=nb)<=0) {
              get_rows(_p,p).move_to(res);
              if (res._width>=mp1) { get_rows(p,_height - 1).move_to(res); break; }
              err+=(int)siz; _p = p + 1U;
            }
            break;
          case 'z' :
            cimg_forZ(*this,p) if ((err-=nb)<=0) {
              get_slices(_p,p).move_to(res);
              if (res._width>=mp1) { get_slices(p,_depth - 1).move_to(res); break; }
              err+=(int)siz; _p = p + 1U;
            }
            break;
          case 'c' :
            cimg_forC(*this,p) if ((err-=nb)<=0) {
              get_channels(_p,p).move_to(res);
              if (res._width>=mp1) { get_channels(p,_spectrum - 1).move_to(res); break; }
              err+=(int)siz; _p = p + 1U;
            }
          }
        }
      } else { // Split by equal values according to specified axis
        T current = *_data;
        switch (_axis) {
        case 'x' : {
          int i0 = 0;
          cimg_forX(*this,i)
            if ((*this)(i)!=current) {
              get_columns(i0,i - 1).move_to(res);
              if (res._width>=mp1) break;
              i0 = i; current = (*this)(i);
            }
          get_columns(i0,width() - 1).move_to(res);
        } break;
        case 'y' : {
          int i0 = 0;
          cimg_forY(*this,i)
            if ((*this)(0,i)!=current) {
              get_rows(i0,i - 1).move_to(res);
              if (res._width>=mp1) break;
              i0 = i; current = (*this)(0,i);
            }
          get_rows(i0,height() - 1).move_to(res);
        } break;
        case 'z' : {
          int i0 = 0;
          cimg_forZ(*this,i)
            if ((*this)(0,0,i)!=current) {
              get_slices(i0,i - 1).move_to(res);
              if (res._width>=mp1) break;
              i0 = i; current = (*this)(0,0,i);
            }
          get_slices(i0,depth() - 1).move_to(res);
        } break;
        case 'c' : {
          int i0 = 0;
          cimg_forC(*this,i)
            if ((*this)(0,0,0,i)!=current) {
              get_channels(i0,i - 1).move_to(res);
              if (res._width>=mp1) break;
              i0 = i; current = (*this)(0,0,0,i);
            }
          get_channels(i0,spectrum() - 1).move_to(res);
        } break;
        default : {
          longT i0 = 0;
          cimg_foroff(*this,i)
            if ((*this)[i]!=current) {
              CImg<T>(_data + i0,1,(unsigned int)(i - i0)).move_to(res);
              if (res._width>=mp1) break;
              i0 = (longT)i; current = (*this)[i];
            }
          CImg<T>(_data + i0,1,(unsigned int)(size() - i0)).move_to(res);
        }
        }
      }
      return res;
    }

    //! Split image into a list of sub-images, according to a specified splitting value sequence and optionally axis.
    /**
       \param values Splitting value sequence.
       \param axis Axis along which the splitting is performed. Can be '0' to ignore axis.
       \param keep_values Tells if the splitting sequence must be kept in the split blocks.
     **/
    template<typename t>
    CImgList<T> get_split(const CImg<t>& values, const char axis=0, const bool keep_values=true) const {
      typedef _cimg_Tt Tt;

      CImgList<T> res;
      if (is_empty()) return res;
      const ulongT vsiz = values.size();
      const char _axis = cimg::lowercase(axis);
      if (!vsiz) return CImgList<T>(*this);
      if (vsiz==1) { // Split according to a single value
        const T value = (T)*values;
        switch (_axis) {
        case 'x' : {
          unsigned int i0 = 0, i = 0;
          do {
            while (i<_width && (*this)(i)==value) ++i;
            if (i>i0) { if (keep_values) get_columns(i0,i - 1).move_to(res); i0 = i; }
            while (i<_width && (*this)(i)!=value) ++i;
            if (i>i0) { get_columns(i0,i - 1).move_to(res); i0 = i; }
          } while (i<_width);
        } break;
        case 'y' : {
          unsigned int i0 = 0, i = 0;
          do {
            while (i<_height && (*this)(0,i)==value) ++i;
            if (i>i0) { if (keep_values) get_rows(i0,i - 1).move_to(res); i0 = i; }
            while (i<_height && (*this)(0,i)!=value) ++i;
            if (i>i0) { get_rows(i0,i - 1).move_to(res); i0 = i; }
          } while (i<_height);
        } break;
        case 'z' : {
          unsigned int i0 = 0, i = 0;
          do {
            while (i<_depth && (*this)(0,0,i)==value) ++i;
            if (i>i0) { if (keep_values) get_slices(i0,i - 1).move_to(res); i0 = i; }
            while (i<_depth && (*this)(0,0,i)!=value) ++i;
            if (i>i0) { get_slices(i0,i - 1).move_to(res); i0 = i; }
          } while (i<_depth);
        } break;
        case 'c' : {
          unsigned int i0 = 0, i = 0;
          do {
            while (i<_spectrum && (*this)(0,0,0,i)==value) ++i;
            if (i>i0) { if (keep_values) get_channels(i0,i - 1).move_to(res); i0 = i; }
            while (i<_spectrum && (*this)(0,0,0,i)!=value) ++i;
            if (i>i0) { get_channels(i0,i - 1).move_to(res); i0 = i; }
          } while (i<_spectrum);
        } break;
        default : {
          const ulongT siz = size();
          ulongT i0 = 0, i = 0;
          do {
            while (i<siz && (*this)[i]==value) ++i;
            if (i>i0) {
              if (keep_values) CImg<T>(_data + i0,1,(unsigned int)(i - i0)).move_to(res);
              i0 = i;
            }
            while (i<siz && (*this)[i]!=value) ++i;
            if (i>i0) { CImg<T>(_data + i0,1,(unsigned int)(i - i0)).move_to(res); i0 = i; }
          } while (i<siz);
        }
        }
      } else { // Split according to multiple values
        ulongT j = 0;
        switch (_axis) {
        case 'x' : {
          unsigned int i0 = 0, i1 = 0, i = 0;
          do {
            if ((Tt)(*this)(i)==(Tt)*values) {
              i1 = i; j = 0;
              while (i<_width && (Tt)(*this)(i)==(Tt)values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=(unsigned int)j;
              if (i>i1) {
                if (i1>i0) get_columns(i0,i1 - 1).move_to(res);
                if (keep_values) get_columns(i1,i - 1).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<_width);
          if (i0<_width) get_columns(i0,width() - 1).move_to(res);
        } break;
        case 'y' : {
          unsigned int i0 = 0, i1 = 0, i = 0;
          do {
            if ((Tt)(*this)(0,i)==(Tt)*values) {
              i1 = i; j = 0;
              while (i<_height && (Tt)(*this)(0,i)==(Tt)values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=(unsigned int)j;
              if (i>i1) {
                if (i1>i0) get_rows(i0,i1 - 1).move_to(res);
                if (keep_values) get_rows(i1,i - 1).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<_height);
          if (i0<_height) get_rows(i0,height() - 1).move_to(res);
        } break;
        case 'z' : {
          unsigned int i0 = 0, i1 = 0, i = 0;
          do {
            if ((Tt)(*this)(0,0,i)==(Tt)*values) {
              i1 = i; j = 0;
              while (i<_depth && (Tt)(*this)(0,0,i)==(Tt)values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=(unsigned int)j;
              if (i>i1) {
                if (i1>i0) get_slices(i0,i1 - 1).move_to(res);
                if (keep_values) get_slices(i1,i - 1).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<_depth);
          if (i0<_depth) get_slices(i0,depth() - 1).move_to(res);
        } break;
        case 'c' : {
          unsigned int i0 = 0, i1 = 0, i = 0;
          do {
            if ((Tt)(*this)(0,0,0,i)==(Tt)*values) {
              i1 = i; j = 0;
              while (i<_spectrum && (Tt)(*this)(0,0,0,i)==(Tt)values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=(unsigned int)j;
              if (i>i1) {
                if (i1>i0) get_channels(i0,i1 - 1).move_to(res);
                if (keep_values) get_channels(i1,i - 1).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<_spectrum);
          if (i0<_spectrum) get_channels(i0,spectrum() - 1).move_to(res);
        } break;
        default : {
          const ulongT siz = size();
          ulongT i0 = 0, i1 = 0, i = 0;
          do {
            if ((Tt)(*this)[i]==(Tt)*values) {
              i1 = i; j = 0;
              while (i<siz && (Tt)(*this)[i]==(Tt)values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=(unsigned int)j;
              if (i>i1) {
                if (i1>i0) CImg<T>(_data + i0,1,(unsigned int)(i1 - i0)).move_to(res);
                if (keep_values) CImg<T>(_data + i1,1,(unsigned int)(i - i1)).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<siz);
          if (i0<siz) CImg<T>(_data + i0,1,(unsigned int)(siz - i0)).move_to(res);
        } break;
        }
      }
      return res;
    }

    //! Append two images along specified axis.
    /**
       \param img Image to append with instance image.
       \param axis Appending axis. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
       \param align Append alignment in \c [0,1].
    **/
    template<typename t>
    CImg<T>& append(const CImg<t>& img, const char axis='x', const float align=0) {
      if (is_empty()) return assign(img,false);
      if (!img) return *this;
      return get_append(img,axis,align).move_to(*this);
    }

    //! Append two images along specified axis \const.
    template<typename t>
    CImg<_cimg_Tt> get_append(const CImg<t>& img, const char axis='x', const float align=0) const {
      if (is_empty()) return CImg<_cimg_Tt>(img,false);
      if (!img) return CImg<_cimg_Tt>(*this,false);
      CImg<_cimg_Tt> res;
      switch (cimg::lowercase(axis)) {
        case 'x' : {
          res.assign(_width + img._width,std::max(_height,img._height),
                     std::max(_depth,img._depth),std::max(_spectrum,img._spectrum),0);
          return res.draw_image(0,
                                (int)(align*(res._height - _height)),
                                (int)(align*(res._depth - _depth)),
                                (int)(align*(res._spectrum - _spectrum)),
                                *this).
            draw_image(_width,
                       (int)(align*(res._height - img._height)),
                       (int)(align*(res._depth - img._depth)),
                       (int)(align*(res._spectrum - img._spectrum)),
                       img);
        } break;
        case 'y' : {
          res.assign(std::max(_width,img._width),_height + img._height,
                     std::max(_depth,img._depth),std::max(_spectrum,img._spectrum),0);
          return res.draw_image((int)(align*(res._width - _width)),
                                0,
                                (int)(align*(res._depth - _depth)),
                                (int)(align*(res._spectrum - _spectrum)),
                                *this).
            draw_image((int)(align*(res._width - img._width)),
                       _height,
                       (int)(align*(res._depth - img._depth)),
                       (int)(align*(res._spectrum - img._spectrum)),
                       img);
        } break;
        case 'z' : {
          res.assign(std::max(_width,img._width),std::max(_height,img._height),
                     _depth + img._depth,std::max(_spectrum,img._spectrum),0);
          return res.draw_image((int)(align*(res._width - _width)),
                                (int)(align*(res._height - _height)),
                                0,
                                (int)(align*(res._spectrum - _spectrum)),
                                *this).
            draw_image((int)(align*(res._width - img._width)),
                       (int)(align*(res._height - img._height)),
                       _depth,
                       (int)(align*(res._spectrum - img._spectrum)),
                       img);
        } break;
        default : {
          res.assign(std::max(_width,img._width),std::max(_height,img._height),
                     std::max(_depth,img._depth),_spectrum + img._spectrum,0);
          return res.draw_image((int)(align*(res._width - _width)),
                                (int)(align*(res._height - _height)),
                                (int)(align*(res._depth - _depth)),
                                0,
                                *this).
            draw_image((int)(align*(res._width - img._width)),
                       (int)(align*(res._height - img._height)),
                       (int)(align*(res._depth - img._depth)),
                       _spectrum,
                       img);
        } break;
      }
      return res;
    }

    //@}
    //---------------------------------------
#endif
