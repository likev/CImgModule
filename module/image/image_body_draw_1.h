#ifndef CIMG_MODULE_IMAGE_BODY_DRAW_1_H
#define CIMG_MODULE_IMAGE_BODY_DRAW_1_H

    //---------------------------
    //
    //! \name Drawing Functions
    //@{
    //---------------------------

#define cimg_init_scanline(opacity) \
    static const T _sc_maxval = (T)std::min(cimg::type<T>::max(),(T)cimg::type<tc>::max()); \
    const float _sc_nopacity = cimg::abs((float)opacity), _sc_copacity = 1 - std::max((float)opacity,0.f); \
    const ulongT _sc_whd = (ulongT)_width*_height*_depth; \
    cimg::unused(_sc_maxval);

#define cimg_draw_scanline(x0,x1,y,color,opacity,brightness) \
    _draw_scanline(x0,x1,y,color,opacity,brightness,_sc_nopacity,_sc_copacity,_sc_whd,_sc_maxval)

    // [internal] The following _draw_scanline() routines are *non user-friendly functions*,
    // used only for internal purpose.
    // Pre-requisites: x0<=x1, y-coordinate is valid, col is valid.
    template<typename tc>
    CImg<T>& _draw_scanline(const int x0, const int x1, const int y,
                            const tc *const color, const float opacity,
                            const float brightness,
                            const float nopacity, const float copacity, const ulongT whd, const T _sc_maxval) {
      const int nx0 = x0>0?x0:0, nx1 = x1<width()?x1:width() - 1, dx = nx1 - nx0;
      if (dx>=0) {
        const tc *col = color;
        const ulongT off = whd - dx - 1;
        T *ptrd = data(nx0,y);
        if (opacity>=1) { // ** Opaque drawing **
          if (brightness==1) { // Brightness==1
            if (sizeof(T)!=1) cimg_forC(*this,c) {
                const T val = (T)*(col++);
                for (int x = dx; x>=0; --x) *(ptrd++) = val;
                ptrd+=off;
              } else cimg_forC(*this,c) {
                const T val = (T)*(col++);
                std::memset(ptrd,(int)val,dx + 1);
                ptrd+=whd;
              }
          } else if (brightness<1) { // Brightness<1
            if (sizeof(T)!=1) cimg_forC(*this,c) {
                const T val = (T)(*(col++)*brightness);
                for (int x = dx; x>=0; --x) *(ptrd++) = val;
                ptrd+=off;
              } else cimg_forC(*this,c) {
                const T val = (T)(*(col++)*brightness);
                std::memset(ptrd,(int)val,dx + 1);
                ptrd+=whd;
              }
          } else { // Brightness>1
            if (sizeof(T)!=1) cimg_forC(*this,c) {
                const T val = (T)((2-brightness)**(col++) + (brightness - 1)*_sc_maxval);
                for (int x = dx; x>=0; --x) *(ptrd++) = val;
                ptrd+=off;
              } else cimg_forC(*this,c) {
                const T val = (T)((2-brightness)**(col++) + (brightness - 1)*_sc_maxval);
                std::memset(ptrd,(int)val,dx + 1);
                ptrd+=whd;
              }
          }
        } else { // ** Transparent drawing **
          if (brightness==1) { // Brightness==1
            cimg_forC(*this,c) {
              const Tfloat val = *(col++)*nopacity;
              for (int x = dx; x>=0; --x) { *ptrd = (T)(val + *ptrd*copacity); ++ptrd; }
              ptrd+=off;
            }
          } else if (brightness<=1) { // Brightness<1
            cimg_forC(*this,c) {
              const Tfloat val = *(col++)*brightness*nopacity;
              for (int x = dx; x>=0; --x) { *ptrd = (T)(val + *ptrd*copacity); ++ptrd; }
              ptrd+=off;
            }
          } else { // Brightness>1
            cimg_forC(*this,c) {
              const Tfloat val = ((2-brightness)**(col++) + (brightness - 1)*_sc_maxval)*nopacity;
              for (int x = dx; x>=0; --x) { *ptrd = (T)(val + *ptrd*copacity); ++ptrd; }
              ptrd+=off;
            }
          }
        }
      }
      return *this;
    }

    //! Draw a 3D point.
    /**
       \param x0 X-coordinate of the point.
       \param y0 Y-coordinate of the point.
       \param z0 Z-coordinate of the point.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \note
       - To set pixel values without clipping needs, you should use the faster CImg::operator()() function.
       \par Example:
       \code
       CImg<unsigned char> img(100,100,1,3,0);
       const unsigned char color[] = { 255,128,64 };
       img.draw_point(50,50,color);
       \endcode
    **/
    template<typename tc>
    CImg<T>& draw_point(const int x0, const int y0, const int z0,
                        const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_point(): Specified color is (null).",
                                    cimg_instance);
      if (x0>=0 && y0>=0 && z0>=0 && x0<width() && y0<height() && z0<depth()) {
        const ulongT whd = (ulongT)_width*_height*_depth;
        const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.f);
        T *ptrd = data(x0,y0,z0,0);
        const tc *col = color;
        if (opacity>=1) cimg_forC(*this,c) { *ptrd = (T)*(col++); ptrd+=whd; }
        else cimg_forC(*this,c) { *ptrd = (T)(*(col++)*nopacity + *ptrd*copacity); ptrd+=whd; }
      }
      return *this;
    }

    //! Draw a 2D point \simplification.
    template<typename tc>
    CImg<T>& draw_point(const int x0, const int y0,
                        const tc *const color, const float opacity=1) {
      return draw_point(x0,y0,0,color,opacity);
    }

    // Draw a points cloud.
    /**
       \param points Image of vertices coordinates.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename t, typename tc>
    CImg<T>& draw_point(const CImg<t>& points,
                        const tc *const color, const float opacity=1) {
      if (is_empty() || !points) return *this;
      switch (points._height) {
      case 0 : case 1 :
        throw CImgArgumentException(_cimg_instance
                                    "draw_point(): Invalid specified point set (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum,points._data);
      case 2 : {
        cimg_forX(points,i) draw_point((int)points(i,0),(int)points(i,1),color,opacity);
      } break;
      default : {
        cimg_forX(points,i) draw_point((int)points(i,0),(int)points(i,1),(int)points(i,2),color,opacity);
      }
      }
      return *this;
    }

    //! Draw a 2D line.
    /**
       \param x0 X-coordinate of the starting line point.
       \param y0 Y-coordinate of the starting line point.
       \param x1 X-coordinate of the ending line point.
       \param y1 Y-coordinate of the ending line point.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the line pattern.
       \param init_hatch Tells if a reinitialization of the hash state must be done.
       \note
       - Set \p init_hatch = false to draw consecutive hatched segments without breaking the line pattern.
       \par Example:
       \code
       CImg<unsigned char> img(100,100,1,3,0);
       const unsigned char color[] = { 255,128,64 };
        img.draw_line(40,40,80,70,color);
       \endcode
    **/
    template<typename tc>
    CImg<T>& draw_line(int x0, int y0,
                       int x1, int y1,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true,
                       const bool draw_last_pixel=true) {
      if (is_empty() || !opacity || !pattern ||
          std::min(y0,y1)>=height() || std::max(y0,y1)<0 || std::min(x0,x1)>=width() || std::max(x0,x1)<0)
        return *this;
      if (x0==x1 && y0==y1) return draw_point(x0,y0,0,color,opacity);
      int w1 = width() - 1, h1 = height() - 1, x01 = x1 - x0, y01 = y1 - y0;
      const bool is_horizontal = cimg::abs(x01)>cimg::abs(y01);
      if (is_horizontal) cimg::swap(x0,y0,x1,y1,w1,h1,x01,y01);

      const int dy = std::max(1,cimg::abs(y01)), hdy = dy/2;
      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);

      cimg_init_scanline(opacity);

      const int
        step = y0<=y1?1:-1,
        y0_cut = cimg::cut(y0,0,h1),
        y1_cut = cimg::cut(y1,0,h1) + (draw_last_pixel?step:0);

      for (int y = y0_cut; y!=y1_cut; y+=step) {
        const int
          t = cimg::abs(y - y0),
          x = x0 + cimg_rd(t*x01,dy);
        if (x>=0 && x<=w1 && pattern&hatch) {
          T *const ptrd = is_horizontal?data(y,x):data(x,y);
          cimg_forC(*this,c) {
            const T val = color[c];
            ptrd[c*_sc_whd] = opacity>=1?val:(T)(val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
          }
        }
        if (!(hatch>>=1)) hatch = ~0U - (~0U>>1);
      }
      return *this;
    }

    //! Draw a 2D line, with z-buffering.
    /**
       \param zbuffer Zbuffer image.
       \param x0 X-coordinate of the starting point.
       \param y0 Y-coordinate of the starting point.
       \param z0 Z-coordinate of the starting point
       \param x1 X-coordinate of the ending point.
       \param y1 Y-coordinate of the ending point.
       \param z1 Z-coordinate of the ending point.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the line pattern.
       \param init_hatch Tells if a reinitialization of the hash state must be done.
    **/
    template<typename tz, typename tc>
    CImg<T>& draw_line(CImg<tz>& zbuffer,
                       int x0, int y0, const float z0,
                       int x1, int y1, const float z1,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || z0<=0 || z1<=0 || !opacity || !pattern) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Specified color is (null).",
                                    cimg_instance);
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);

      if (std::min(y0,y1)>=height() || std::max(y0,y1)<0 || std::min(x0,x1)>=width() || std::max(x0,x1)<0)
        return *this;
      int
        w1 = width() - 1, h1 = height() - 1,
        dx01 = x1 - x0, dy01 = y1 - y0;
      float iz0 = 1/z0, iz1 = 1/z1, diz01 = iz1 - iz0;

      const bool is_horizontal = cimg::abs(dx01)>cimg::abs(dy01);
      if (is_horizontal) cimg::swap(x0,y0,x1,y1,w1,h1,dx01,dy01);
      if (pattern==~0U && y0>y1) {
        cimg::swap(x0,x1,y0,y1,iz0,iz1);
        dx01*=-1; dy01*=-1; diz01*=-1;
      }
      const float
        slope_x = dy01?(float)dx01/dy01:0,
        slope_iz = dy01?(float)diz01/dy01:0;

      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);
      cimg_init_scanline(opacity);

      const int step = y0<=y1?1:-1, cy0 = cimg::cut(y0,0,h1), cy1 = cimg::cut(y1,0,h1) + step;
      dy01+=dy01?0:1;

      for (int y = cy0; y!=cy1; y+=step) {
        const int yy0 = y - y0;
        const float
          fx = x0 + yy0*slope_x,
          iz = iz0 + yy0*slope_iz;
        if (fx>=0 && fx<=w1 && pattern&hatch) {
          const int x = (int)(fx + 0.5f);
          tz *const ptrz = is_horizontal?zbuffer.data(y,x):zbuffer.data(x,y);
          if (iz>=*ptrz) {
            *ptrz = (tz)iz;
            T *const ptrd = is_horizontal?data(y,x):data(x,y);
            cimg_forC(*this,c) {
              const T val = color[c];
              ptrd[c*_sc_whd] = opacity>=1?val:(T)(val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
          }
        }
        if (!(hatch>>=1)) hatch = ~0U - (~0U>>1);
      }
      return *this;
    }

    //! Draw a textured 2D line.
    /**
       \param x0 X-coordinate of the starting line point.
       \param y0 Y-coordinate of the starting line point.
       \param x1 X-coordinate of the ending line point.
       \param y1 Y-coordinate of the ending line point.
       \param texture Texture image defining the pixel colors.
       \param tx0 X-coordinate of the starting texture point.
       \param ty0 Y-coordinate of the starting texture point.
       \param tx1 X-coordinate of the ending texture point.
       \param ty1 Y-coordinate of the ending texture point.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the line pattern.
       \param init_hatch Tells if the hash variable must be reinitialized.
       \note
       - Line routine uses the well known Bresenham's algorithm.
       \par Example:
       \code
       CImg<unsigned char> img(100,100,1,3,0), texture("texture256x256.ppm");
       const unsigned char color[] = { 255,128,64 };
       img.draw_line(40,40,80,70,texture,0,0,255,255);
       \endcode
    **/
    template<typename tc>
    CImg<T>& draw_line(int x0, int y0,
                       int x1, int y1,
                       const CImg<tc>& texture,
                       int tx0, int ty0,
                       int tx1, int ty1,
                       const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {

      if (is_empty() || !opacity || !pattern) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture)) return draw_line(x0,y0,x1,y1,+texture,tx0,ty0,tx1,ty1,opacity,pattern,init_hatch);
      if (std::min(y0,y1)>=height() || std::max(y0,y1)<0 || std::min(x0,x1)>=width() || std::max(x0,x1)<0)
        return *this;
      int
        w1 = width() - 1, h1 = height() - 1,
        dx01 = x1 - x0, dy01 = y1 - y0,
        dtx01 = tx1 - tx0, dty01 = ty1 - ty0;

      const bool is_horizontal = cimg::abs(dx01)>cimg::abs(dy01);
      if (is_horizontal) cimg::swap(x0,y0,x1,y1,w1,h1,dx01,dy01);
      if (pattern==~0U && y0>y1) {
        cimg::swap(x0,x1,y0,y1,tx0,tx1,ty0,ty1);
        dx01*=-1; dy01*=-1; dtx01*=-1; dty01*=-1;
      }
      const float
        slope_x = dy01?(float)dx01/dy01:0,
        slope_tx = dy01?(float)dtx01/dy01:0,
        slope_ty = dy01?(float)dty01/dy01:0;

      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;
      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);
      cimg_init_scanline(opacity);

      const int step = y0<=y1?1:-1, cy0 = cimg::cut(y0,0,h1), cy1 = cimg::cut(y1,0,h1) + step;
      dy01+=dy01?0:1;

      for (int y = cy0; y!=cy1; y+=step) {
        const int yy0 = y - y0;
        const float
          fx = x0 + yy0*slope_x,
          ftx = tx0 + yy0*slope_tx,
          fty = ty0 + yy0*slope_ty;
        if (fx>=0 && fx<=w1 && pattern&hatch) {
          const int
            x = (int)(fx + 0.5f),
            tx = (int)ftx,
            ty = (int)fty;
          T *const ptrd = is_horizontal?data(y,x):data(x,y);
          const tc *const color = &texture._atXY(tx,ty);
          cimg_forC(*this,c) {
            const T val = color[c*twhd];
            ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
          }
        }
        if (!(hatch>>=1)) hatch = ~0U - (~0U>>1);
      }
      return *this;
    }

    //! Draw a textured 2D line, with perspective correction.
    /**
       \param x0 X-coordinate of the starting point.
       \param y0 Y-coordinate of the starting point.
       \param z0 Z-coordinate of the starting point
       \param x1 X-coordinate of the ending point.
       \param y1 Y-coordinate of the ending point.
       \param z1 Z-coordinate of the ending point.
       \param texture Texture image defining the pixel colors.
       \param tx0 X-coordinate of the starting texture point.
       \param ty0 Y-coordinate of the starting texture point.
       \param tx1 X-coordinate of the ending texture point.
       \param ty1 Y-coordinate of the ending texture point.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the line pattern.
       \param init_hatch Tells if the hash variable must be reinitialized.
    **/
    template<typename tc>
    CImg<T>& draw_line(int x0, int y0, const float z0,
                       int x1, int y1, const float z1,
                       const CImg<tc>& texture,
                       const int tx0, const int ty0,
                       const int tx1, const int ty1,
                       const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || z0<=0 || z1<=0 || !opacity || !pattern) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_line(x0,y0,z0,x1,y1,z1,+texture,tx0,ty0,tx1,ty1,opacity,pattern,init_hatch);
      if (std::min(y0,y1)>=height() || std::max(y0,y1)<0 || std::min(x0,x1)>=width() || std::max(x0,x1)<0)
        return *this;
      int
        w1 = width() - 1, h1 = height() - 1,
        dx01 = x1 - x0, dy01 = y1 - y0;
      float
        iz0 = 1/z0, iz1 = 1/z1,
        diz01 = iz1 - iz0,
        txz0 = tx0*iz0, txz1 = tx1*iz1,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1,
        dtxz01 = txz1 - txz0, dtyz01 = tyz1 - tyz0;

      const bool is_horizontal = cimg::abs(dx01)>cimg::abs(dy01);
      if (is_horizontal) cimg::swap(x0,y0,x1,y1,w1,h1,dx01,dy01);
      if (pattern==~0U && y0>y1) {
        cimg::swap(x0,x1,y0,y1,iz0,iz1,txz0,txz1,tyz0,tyz1);
        dx01*=-1; dy01*=-1; diz01*=-1; dtxz01*=-1; dtyz01*=-1;
      }
      const float
        slope_x = dy01?(float)dx01/dy01:0,
        slope_iz = dy01?(float)diz01/dy01:0,
        slope_txz = dy01?(float)dtxz01/dy01:0,
        slope_tyz = dy01?(float)dtyz01/dy01:0;

      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;
      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);
      cimg_init_scanline(opacity);

      const int step = y0<=y1?1:-1, cy0 = cimg::cut(y0,0,h1), cy1 = cimg::cut(y1,0,h1) + step;
      dy01+=dy01?0:1;

      for (int y = cy0; y!=cy1; y+=step) {
        const int yy0 = y - y0;
        const float
          fx = x0 + yy0*slope_x,
          iz = iz0 + yy0*slope_iz,
          ftxz = txz0 + yy0*slope_txz,
          ftyz = tyz0 + yy0*slope_tyz;
        if (fx>=0 && fx<=w1 && pattern&hatch) {
          const int
            x = (int)(fx + 0.5f),
            tx = (int)(ftxz/iz),
            ty = (int)(ftyz/iz);
          T *const ptrd = is_horizontal?data(y,x):data(x,y);
          const tc *const color = &texture._atXY(tx,ty);
          cimg_forC(*this,c) {
            const T val = color[c*twhd];
            ptrd[c*_sc_whd] = opacity>=1?val:(T)(val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
          }
        }
        if (!(hatch>>=1)) hatch = ~0U - (~0U>>1);
      }
      return *this;
    }

    //! Draw a textured 2D line, with perspective correction and z-buffering.
    /**
       \param zbuffer Z-buffer image.
       \param x0 X-coordinate of the starting point.
       \param y0 Y-coordinate of the starting point.
       \param z0 Z-coordinate of the starting point
       \param x1 X-coordinate of the ending point.
       \param y1 Y-coordinate of the ending point.
       \param z1 Z-coordinate of the ending point.
       \param texture Texture image defining the pixel colors.
       \param tx0 X-coordinate of the starting texture point.
       \param ty0 Y-coordinate of the starting texture point.
       \param tx1 X-coordinate of the ending texture point.
       \param ty1 Y-coordinate of the ending texture point.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the line pattern.
       \param init_hatch Tells if the hash variable must be reinitialized.
    **/
    template<typename tz, typename tc>
    CImg<T>& draw_line(CImg<tz>& zbuffer,
                       int x0, int y0, const float z0,
                       int x1, int y1, const float z1,
                       const CImg<tc>& texture,
                       const int tx0, const int ty0,
                       const int tx1, const int ty1,
                       const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || z0<=0 || z1<=0 || !opacity || !pattern) return *this;
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_line(zbuffer,x0,y0,z0,x1,y1,z1,+texture,tx0,ty0,tx1,ty1,opacity,pattern,init_hatch);
      if (std::min(y0,y1)>=height() || std::max(y0,y1)<0 || std::min(x0,x1)>=width() || std::max(x0,x1)<0)
        return *this;

      int
        w1 = width() - 1, h1 = height() - 1,
        dx01 = x1 - x0, dy01 = y1 - y0;
      float
        iz0 = 1/z0, iz1 = 1/z1,
        diz01 = iz1 - iz0,
        txz0 = tx0*iz0, txz1 = tx1*iz1,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1,
        dtxz01 = txz1 - txz0, dtyz01 = tyz1 - tyz0;

      const bool is_horizontal = cimg::abs(dx01)>cimg::abs(dy01);
      if (is_horizontal) cimg::swap(x0,y0,x1,y1,w1,h1,dx01,dy01);
      if (pattern==~0U && y0>y1) {
        cimg::swap(x0,x1,y0,y1,iz0,iz1,txz0,txz1,tyz0,tyz1);
        dx01*=-1; dy01*=-1; diz01*=-1; dtxz01*=-1; dtyz01*=-1;
      }
      const float
        slope_x = dy01?(float)dx01/dy01:0,
        slope_iz = dy01?(float)diz01/dy01:0,
        slope_txz = dy01?(float)dtxz01/dy01:0,
        slope_tyz = dy01?(float)dtyz01/dy01:0;

      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;
      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);
      cimg_init_scanline(opacity);

      const int step = y0<=y1?1:-1, cy0 = cimg::cut(y0,0,h1), cy1 = cimg::cut(y1,0,h1) + step;
      dy01+=dy01?0:1;

      for (int y = cy0; y!=cy1; y+=step) {
        const int yy0 = y - y0;
        const float
          fx = x0 + yy0*slope_x,
          iz = iz0 + yy0*slope_iz,
          ftxz = txz0 + yy0*slope_txz,
          ftyz = tyz0 + yy0*slope_tyz;
        if (fx>=0 && fx<=w1 && pattern&hatch) {
          const int x = (int)(fx + 0.5f);
          tz *const ptrz = is_horizontal?zbuffer.data(y,x):zbuffer.data(x,y);
          if (iz>=*ptrz) {
            *ptrz = (tz)iz;
            const int
              tx = (int)(ftxz/iz),
              ty = (int)(ftyz/iz);
            T *const ptrd = is_horizontal?data(y,x):data(x,y);
            const tc *const color = &texture._atXY(tx,ty);
            cimg_forC(*this,c) {
              const T val = color[c*twhd];
              ptrd[c*_sc_whd] = opacity>=1?val:(T)(val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
          }
        }
        if (!(hatch>>=1)) hatch = ~0U - (~0U>>1);
      }
      return *this;
    }

    //! Draw a set of consecutive lines.
    /**
       \param points Coordinates of vertices, stored as a list of vectors.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the line pattern.
       \param init_hatch If set to true, init hatch motif.
       \note
       - This function uses several call to the single CImg::draw_line() procedure,
       depending on the vectors size in \p points.
    **/
    template<typename tp, typename tc>
    CImg<T>& draw_line(const CImg<tp>& points,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || !points) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Specified color is (null).",
                                    cimg_instance);
      if (points.height()!=2)
        throw CImgArgumentException(_cimg_instance
                                    "draw_line(): Invalid specified point set (%u,%u,%u,%u).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum);
      CImg<intT> ipoints;
      if (cimg::type<tp>::is_float()) ipoints = points.get_round();
      else ipoints.assign(points,cimg::type<tp>::string()==cimg::type<int>::string());

      bool ninit_hatch = init_hatch;
      const int x0 = ipoints(0,0), y0 = ipoints(0,1);
      int ox = x0, oy = y0;
      for (unsigned int i = 1; i<ipoints._width; ++i) {
        const int x = ipoints(i,0), y = ipoints(i,1);
        draw_line(ox,oy,x,y,color,opacity,pattern,ninit_hatch);
        ninit_hatch = false;
        ox = x; oy = y;
      }
      return *this;
    }

    //! Draw a 2D arrow.
    /**
       \param x0 X-coordinate of the starting arrow point (tail).
       \param y0 Y-coordinate of the starting arrow point (tail).
       \param x1 X-coordinate of the ending arrow point (head).
       \param y1 Y-coordinate of the ending arrow point (head).
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param angle Aperture angle of the arrow head.
       \param length Length of the arrow head. If negative, describes a percentage of the arrow length.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the line pattern.
    **/
    template<typename tc>
    CImg<T>& draw_arrow(const int x0, const int y0,
                        const int x1, const int y1,
                        const tc *const color, const float opacity=1,
                        const float angle=30, const float length=-10,
                        const unsigned int pattern=~0U) {
      if (is_empty()) return *this;
      const float u = (float)(x0 - x1), v = (float)(y0 - y1), sq = u*u + v*v,
        deg = (float)(angle*cimg::PI/180), ang = (sq>0)?(float)std::atan2(v,u):0.f,
        l = (length>=0)?length:-length*(float)std::sqrt(sq)/100;
      if (sq>0) {
        const float
            cl = (float)std::cos(ang - deg), sl = (float)std::sin(ang - deg),
            cr = (float)std::cos(ang + deg), sr = (float)std::sin(ang + deg);
        const int
          xl = x1 + (int)(l*cl), yl = y1 + (int)(l*sl),
          xr = x1 + (int)(l*cr), yr = y1 + (int)(l*sr),
          xc = x1 + (int)((l + 1)*(cl + cr))/2, yc = y1 + (int)((l + 1)*(sl + sr))/2;
        draw_line(x0,y0,xc,yc,color,opacity,pattern).draw_triangle(x1,y1,xl,yl,xr,yr,color,opacity);
      } else draw_point(x0,y0,color,opacity);
      return *this;
    }

    //! Draw a 2D spline.
    /**
       \param x0 X-coordinate of the starting curve point
       \param y0 Y-coordinate of the starting curve point
       \param u0 X-coordinate of the starting velocity
       \param v0 Y-coordinate of the starting velocity
       \param x1 X-coordinate of the ending curve point
       \param y1 Y-coordinate of the ending curve point
       \param u1 X-coordinate of the ending velocity
       \param v1 Y-coordinate of the ending velocity
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param precision Curve drawing precision.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the line pattern.
       \param init_hatch If \c true, init hatch motif.
       \note
       - The curve is a 2D cubic Bezier spline, from the set of specified starting/ending points
       and corresponding velocity vectors.
       - The spline is drawn as a sequence of connected segments. The \p precision parameter sets the
       average number of pixels in each drawn segment.
       - A cubic Bezier curve is sometimes defined by a set of 4 points { (\p x0,\p y0), (\p xa,\p ya),
         (\p xb,\p yb), (\p x1,\p y1) } where (\p x0,\p y0) is the starting point, (\p x1,\p y1) is the ending point
         and (\p xa,\p ya), (\p xb,\p yb) are two
       \e control points.
       The starting and ending velocities (\p u0,\p v0) and (\p u1,\p v1) can be deduced easily from
       the control points as
       \p u0 = (\p xa - \p x0), \p v0 = (\p ya - \p y0), \p u1 = (\p x1 - \p xb) and \p v1 = (\p y1 - \p yb).
       \par Example:
       \code
       CImg<unsigned char> img(100,100,1,3,0);
       const unsigned char color[] = { 255,255,255 };
       img.draw_spline(30,30,0,100,90,40,0,-100,color);
       \endcode
    **/
    template<typename tc>
    CImg<T>& draw_spline(const int x0, const int y0, const float u0, const float v0,
                         const int x1, const int y1, const float u1, const float v1,
                         const tc *const color, const float opacity=1,
                         const float precision=0.25, const unsigned int pattern=~0U,
                         const bool init_hatch=true) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_spline(): Specified color is (null).",
                                    cimg_instance);
      if (x0==x1 && y0==y1) return draw_point(x0,y0,color,opacity);
      bool ninit_hatch = init_hatch;
      const float
        ax = u0 + u1 + 2*(x0 - x1),
        bx = 3*(x1 - x0) - 2*u0 - u1,
        ay = v0 + v1 + 2*(y0 - y1),
        by = 3*(y1 - y0) - 2*v0 - v1,
        _precision = 1/(cimg::hypot((float)x0 - x1,(float)y0 - y1)*(precision>0?precision:1));
      int ox = x0, oy = y0;
      for (float t = 0; t<1; t+=_precision) {
        const float t2 = t*t, t3 = t2*t;
        const int
          nx = (int)(ax*t3 + bx*t2 + u0*t + x0),
          ny = (int)(ay*t3 + by*t2 + v0*t + y0);
        draw_line(ox,oy,nx,ny,color,opacity,pattern,ninit_hatch);
        ninit_hatch = false;
        ox = nx; oy = ny;
      }
      return draw_line(ox,oy,x1,y1,color,opacity,pattern,false);
    }

    //! Draw a textured 2D spline.
    /**
       \param x0 X-coordinate of the starting curve point
       \param y0 Y-coordinate of the starting curve point
       \param u0 X-coordinate of the starting velocity
       \param v0 Y-coordinate of the starting velocity
       \param x1 X-coordinate of the ending curve point
       \param y1 Y-coordinate of the ending curve point
       \param u1 X-coordinate of the ending velocity
       \param v1 Y-coordinate of the ending velocity
       \param texture Texture image defining line pixel colors.
       \param tx0 X-coordinate of the starting texture point.
       \param ty0 Y-coordinate of the starting texture point.
       \param tx1 X-coordinate of the ending texture point.
       \param ty1 Y-coordinate of the ending texture point.
       \param precision Curve drawing precision.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the line pattern.
       \param init_hatch if \c true, reinit hatch motif.
    **/
    template<typename t>
    CImg<T>& draw_spline(const int x0, const int y0, const float u0, const float v0,
                         const int x1, const int y1, const float u1, const float v1,
                         const CImg<t>& texture,
                         const int tx0, const int ty0, const int tx1, const int ty1,
                         const float opacity=1,
                         const float precision=4, const unsigned int pattern=~0U,
                         const bool init_hatch=true) {
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_spline(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_empty()) return *this;
      if (is_overlapped(texture))
        return draw_spline(x0,y0,u0,v0,x1,y1,u1,v1,+texture,tx0,ty0,tx1,ty1,precision,opacity,pattern,init_hatch);
      if (x0==x1 && y0==y1)
        return draw_point(x0,y0,texture.get_vector_at(x0<=0?0:x0>=texture.width()?texture.width() - 1:x0,
                                                      y0<=0?0:y0>=texture.height()?texture.height() - 1:y0).data(),
                          opacity);
      bool ninit_hatch = init_hatch;
      const float
        ax = u0 + u1 + 2*(x0 - x1),
        bx = 3*(x1 - x0) - 2*u0 - u1,
        ay = v0 + v1 + 2*(y0 - y1),
        by = 3*(y1 - y0) - 2*v0 - v1,
        _precision = 1/(cimg::hypot((float)x0 - x1,(float)y0 - y1)*(precision>0?precision:1));
      int ox = x0, oy = y0, otx = tx0, oty = ty0;
      for (float t1 = 0; t1<1; t1+=_precision) {
        const float t2 = t1*t1, t3 = t2*t1;
        const int
          nx = (int)(ax*t3 + bx*t2 + u0*t1 + x0),
          ny = (int)(ay*t3 + by*t2 + v0*t1 + y0),
          ntx = tx0 + (int)((tx1 - tx0)*t1),
          nty = ty0 + (int)((ty1 - ty0)*t1);
        draw_line(ox,oy,nx,ny,texture,otx,oty,ntx,nty,opacity,pattern,ninit_hatch);
        ninit_hatch = false;
        ox = nx; oy = ny; otx = ntx; oty = nty;
      }
      return draw_line(ox,oy,x1,y1,texture,otx,oty,tx1,ty1,opacity,pattern,false);
    }

    //! Draw a set of consecutive splines.
    /**
       \param points Vertices data.
       \param tangents Tangents data.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
       \param is_closed_set Tells if the drawn spline set is closed.
       \param precision Precision of the drawing.
       \param pattern An integer whose bits describe the line pattern.
       \param init_hatch If \c true, init hatch motif.
    **/
    template<typename tp, typename tt, typename tc>
    CImg<T>& draw_spline(const CImg<tp>& points, const CImg<tt>& tangents,
                         const tc *const color, const float opacity=1,
                         const bool is_closed_set=false, const float precision=4,
                         const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || !points || !tangents || points._width<2 || tangents._width<2) return *this;
      bool ninit_hatch = init_hatch;
      switch (points._height) {
      case 0 : case 1 :
        throw CImgArgumentException(_cimg_instance
                                    "draw_spline(): Invalid specified point set (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum,points._data);

      default : {
        const int x0 = (int)points(0,0), y0 = (int)points(0,1);
        const float u0 = (float)tangents(0,0), v0 = (float)tangents(0,1);
        int ox = x0, oy = y0;
        float ou = u0, ov = v0;
        for (unsigned int i = 1; i<points._width; ++i) {
          const int x = (int)points(i,0), y = (int)points(i,1);
          const float u = (float)tangents(i,0), v = (float)tangents(i,1);
          draw_spline(ox,oy,ou,ov,x,y,u,v,color,precision,opacity,pattern,ninit_hatch);
          ninit_hatch = false;
          ox = x; oy = y; ou = u; ov = v;
        }
        if (is_closed_set) draw_spline(ox,oy,ou,ov,x0,y0,u0,v0,color,precision,opacity,pattern,false);
      }
      }
      return *this;
    }

    //! Draw a set of consecutive splines \overloading.
    /**
       Similar to previous function, with the point tangents automatically estimated from the given points set.
    **/
    template<typename tp, typename tc>
    CImg<T>& draw_spline(const CImg<tp>& points,
                         const tc *const color, const float opacity=1,
                         const bool is_closed_set=false, const float precision=4,
                         const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || !points || points._width<2) return *this;
      CImg<Tfloat> tangents;
      switch (points._height) {
      case 0 : case 1 :
        throw CImgArgumentException(_cimg_instance
                                    "draw_spline(): Invalid specified point set (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum,points._data);
      case 2 : {
        tangents.assign(points._width,points._height);
        cimg_forX(points,p) {
          const unsigned int
            p0 = is_closed_set?(p + points.width() - 1)%points.width():(p?p - 1:0),
            p1 = is_closed_set?(p + 1)%points.width():(p + 1<points.width()?p + 1:p);
          const float
            x = (float)points(p,0),
            y = (float)points(p,1),
            x0 = (float)points(p0,0),
            y0 = (float)points(p0,1),
            x1 = (float)points(p1,0),
            y1 = (float)points(p1,1),
            u0 = x - x0,
            v0 = y - y0,
            n0 = 1e-8f + cimg::hypot(u0,v0),
            u1 = x1 - x,
            v1 = y1 - y,
            n1 = 1e-8f + cimg::hypot(u1,v1),
            u = u0/n0 + u1/n1,
            v = v0/n0 + v1/n1,
            n = 1e-8f + cimg::hypot(u,v),
            fact = 0.5f*(n0 + n1);
          tangents(p,0) = (Tfloat)(fact*u/n);
          tangents(p,1) = (Tfloat)(fact*v/n);
        }
      } break;
      default : {
        tangents.assign(points._width,points._height);
        cimg_forX(points,p) {
          const unsigned int
            p0 = is_closed_set?(p + points.width() - 1)%points.width():(p?p - 1:0),
            p1 = is_closed_set?(p + 1)%points.width():(p + 1<points.width()?p + 1:p);
          const float
            x = (float)points(p,0),
            y = (float)points(p,1),
            z = (float)points(p,2),
            x0 = (float)points(p0,0),
            y0 = (float)points(p0,1),
            z0 = (float)points(p0,2),
            x1 = (float)points(p1,0),
            y1 = (float)points(p1,1),
            z1 = (float)points(p1,2),
            u0 = x - x0,
            v0 = y - y0,
            w0 = z - z0,
            n0 = 1e-8f + cimg::hypot(u0,v0,w0),
            u1 = x1 - x,
            v1 = y1 - y,
            w1 = z1 - z,
            n1 = 1e-8f + cimg::hypot(u1,v1,w1),
            u = u0/n0 + u1/n1,
            v = v0/n0 + v1/n1,
            w = w0/n0 + w1/n1,
            n = 1e-8f + cimg::hypot(u,v,w),
            fact = 0.5f*(n0 + n1);
          tangents(p,0) = (Tfloat)(fact*u/n);
          tangents(p,1) = (Tfloat)(fact*v/n);
          tangents(p,2) = (Tfloat)(fact*w/n);
        }
      }
      }
      return draw_spline(points,tangents,color,opacity,is_closed_set,precision,pattern,init_hatch);
    }

    // [internal] Draw a filled triangle.
    template<typename tc>
    CImg<T>& _draw_triangle(int x0, int y0,
                            int x1, int y1,
                            int x2, int y2,
                            const tc *const color, const float opacity,
                            const float brightness) {
      if (y0>y1) cimg::swap(x0,x1,y0,y1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0,
        dx02 = (longT)x2 - x0,
        dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float cbs = cimg::cut(brightness,0.f,2.f);

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        if (xm>xM) cimg::swap(xm,xM);
        cimg_draw_scanline(xm,xM,y,color,opacity,cbs);
      }
      return *this;
    }

    //! Draw a filled 2D triangle.
    /**
       \param x0 X-coordinate of the first vertex.
       \param y0 Y-coordinate of the first vertex.
       \param x1 X-coordinate of the second vertex.
       \param y1 Y-coordinate of the second vertex.
       \param x2 X-coordinate of the third vertex.
       \param y2 Y-coordinate of the third vertex.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
     **/
    template<typename tc>
    CImg<T>& draw_triangle(const int x0, const int y0,
                           const int x1, const int y1,
                           const int x2, const int y2,
                           const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      _draw_triangle(x0,y0,x1,y1,x2,y2,color,opacity,1);
      return *this;
    }

    //! Draw a outlined 2D triangle.
    /**
       \param x0 X-coordinate of the first vertex.
       \param y0 Y-coordinate of the first vertex.
       \param x1 X-coordinate of the second vertex.
       \param y1 Y-coordinate of the second vertex.
       \param x2 X-coordinate of the third vertex.
       \param y2 Y-coordinate of the third vertex.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the outline pattern.
     **/
    template<typename tc>
    CImg<T>& draw_triangle(const int x0, const int y0,
                           const int x1, const int y1,
                           const int x2, const int y2,
                           const tc *const color, const float opacity,
                           const unsigned int pattern) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      CImg<intT> points(3,2,1,1,x0,x1,x2,y0,y1,y2);
      return draw_polygon(points,color,opacity,pattern);
    }

    //! Draw a filled 2D triangle, with z-buffering.
    /**
       \param zbuffer Z-buffer image.
       \param x0 X-coordinate of the first vertex.
       \param y0 Y-coordinate of the first vertex.
       \param z0 Z-coordinate of the first vertex.
       \param x1 X-coordinate of the second vertex.
       \param y1 Y-coordinate of the second vertex.
       \param z1 Z-coordinate of the second vertex.
       \param x2 X-coordinate of the third vertex.
       \param y2 Y-coordinate of the third vertex.
       \param z2 Z-coordinate of the third vertex.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
       \param brightness Brightness factor.
    **/
    template<typename tz, typename tc>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const tc *const color, const float opacity=1,
                           const float brightness=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        cbs = cimg::cut(brightness,0.f,2.f);

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,izm,izM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float dizmM = izM - izm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              cimg_forC(*this,c) {
                const Tfloat val = cbs<=1?color[c]*cbs:(2 - cbs)*color[c] + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;
    }

    //! Draw a Gouraud-shaded 2D triangle.
    /**
       \param x0 X-coordinate of the first vertex in the image instance.
       \param y0 Y-coordinate of the first vertex in the image instance.
       \param x1 X-coordinate of the second vertex in the image instance.
       \param y1 Y-coordinate of the second vertex in the image instance.
       \param x2 X-coordinate of the third vertex in the image instance.
       \param y2 Y-coordinate of the third vertex in the image instance.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param bs0 Brightness factor of the first vertex (in [0,2]).
       \param bs1 brightness factor of the second vertex (in [0,2]).
       \param bs2 brightness factor of the third vertex (in [0,2]).
       \param opacity Drawing opacity.
    **/
    template<typename tc>
    CImg<T>& draw_triangle(int x0, int y0,
                           int x1, int y1,
                           int x2, int y2,
                           const tc *const color,
                           float bs0,
                           float bs1,
                           float bs2,
                           const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);

      if (y0>y1) cimg::swap(x0,x1,y0,y1,bs0,bs1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,bs0,bs2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,bs1,bs2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float dbs01 = bs1 - bs0, dbs02 = bs2 - bs0, dbs12 = bs2 - bs1;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          bsm = y<y1?(bs0 + dbs01*yy0/dy01):(bs1 + dbs12*yy1/dy12),
          bsM = bs0 + dbs02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,bsm,bsM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float dbsmM = bsM - bsm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float cbs = cimg::cut(bsm + dbsmM*xxm/dxmM,0.f,2.f);
            cimg_forC(*this,c) {
              const Tfloat val = cbs<=1?color[c]*cbs:(2 - cbs)*color[c] + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a Gouraud-shaded 2D triangle, with z-buffering \overloading.
    template<typename tz, typename tc>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const tc *const color,
                           float bs0,
                           float bs1,
                           float bs2,
                           float opacity=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,bs0,bs1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,bs0,bs2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,bs1,bs2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        dbs01 = bs1 - bs0, dbs02 = bs2 - bs0, dbs12 = bs2 - bs1;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          bsm = y<y1?(bs0 + dbs01*yy0/dy01):(bs1 + dbs12*yy1/dy12),
          bsM = bs0 + dbs02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,izm,izM,bsm,bsM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float dizmM = izM - izm, dbsmM = bsM - bsm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              const float cbs = cimg::cut(bsm + dbsmM*xxm/dxmM,0.f,2.f);
              cimg_forC(*this,c) {
                const Tfloat val = cbs<=1?color[c]*cbs:(2 - cbs)*color[c] + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;
    }

    //! Draw a color-interpolated 2D triangle.
    /**
       \param x0 X-coordinate of the first vertex in the image instance.
       \param y0 Y-coordinate of the first vertex in the image instance.
       \param x1 X-coordinate of the second vertex in the image instance.
       \param y1 Y-coordinate of the second vertex in the image instance.
       \param x2 X-coordinate of the third vertex in the image instance.
       \param y2 Y-coordinate of the third vertex in the image instance.
       \param color1 Pointer to \c spectrum() consecutive values of type \c T, defining the color of the first vertex.
       \param color2 Pointer to \c spectrum() consecutive values of type \c T, defining the color of the second vertex.
       \param color3 Pointer to \c spectrum() consecutive values of type \c T, defining the color of the third vertex.
       \param opacity Drawing opacity.
     **/
    template<typename tc>
    CImg<T>& draw_triangle(int x0, int y0,
                           int x1, int y1,
                           int x2, int y2,
                           const tc *color0,
                           const tc *color1,
                           const tc *color2,
                           const float opacity=1) {
      typedef typename cimg::superset<tc,int>::type stc;
      if (is_empty()) return *this;
      if (!color0 || !color1 || !color2)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): One of the specified color is (null).",
                                    cimg_instance);

      if (y0>y1) cimg::swap(x0,x1,y0,y1,color0,color1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,color0,color2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,color1,color2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;

      cimg_init_scanline(opacity);

      cimg_forC(*this,c) {
        const stc dcolor01 = color1[c] - color0[c], dcolor02 = color2[c] - color0[c], dcolor12 = color2[c] - color1[c];

        for (int y = cy0; y<=cy2; ++y) {
          const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
          longT
            xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
            xM = x0 + cimg_rd(dx02*yy0,dy02);
          stc
            colorm = y<y1?(color0[c] + dcolor01*yy0/dy01):(color1[c] + dcolor12*yy1/dy12),
            colorM = color0[c] + dcolor02*yy0/dy02;
          if (xm>xM) cimg::swap(xm,xM,colorm,colorM);
          if (xM>=0 && xm<=w1) {
            const int
              cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
              cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
            T *ptrd = data(cxm,y);
            const longT dxmM = std::max((longT)1,xM - xm);
            const stc dcolormM = colorM - colorm;

            for (int x = cxm; x<=cxM; ++x) {
              const longT xxm = (longT)x - xm;
              const stc col = colorm + dcolormM*xxm/dxmM;
              ptrd[c*_sc_whd] = (T)(opacity>=1?col:col*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              ++ptrd;
            }
          }
        }
      }
      return *this;
    }

    //! Draw a textured 2D triangle.
    /**
       \param x0 X-coordinate of the first vertex in the image instance.
       \param y0 Y-coordinate of the first vertex in the image instance.
       \param x1 X-coordinate of the second vertex in the image instance.
       \param y1 Y-coordinate of the second vertex in the image instance.
       \param x2 X-coordinate of the third vertex in the image instance.
       \param y2 Y-coordinate of the third vertex in the image instance.
       \param texture Texture image used to fill the triangle.
       \param tx0 X-coordinate of the first vertex in the texture image.
       \param ty0 Y-coordinate of the first vertex in the texture image.
       \param tx1 X-coordinate of the second vertex in the texture image.
       \param ty1 Y-coordinate of the second vertex in the texture image.
       \param tx2 X-coordinate of the third vertex in the texture image.
       \param ty2 Y-coordinate of the third vertex in the texture image.
       \param opacity Drawing opacity.
       \param brightness Brightness factor of the drawing (in [0,2]).
    **/
    template<typename tc>
    CImg<T>& draw_triangle(int x0, int y0,
                           int x1, int y1,
                           int x2, int y2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const float opacity=1,
                           const float brightness=1) {
      if (is_empty()) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,x1,y1,x2,y2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,opacity,brightness);

      if (y0>y1) cimg::swap(x0,x1,y0,y1,tx0,tx1,ty0,ty1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,tx0,tx2,ty0,ty2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,tx1,tx2,ty1,ty2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2,
        dtx01 = (longT)tx1 - tx0, dtx02 = (longT)tx2 - tx0, dtx12 = (longT)tx2 - tx1,
        dty01 = (longT)ty1 - ty0, dty02 = (longT)ty2 - ty0, dty12 = (longT)ty2 - ty1;
      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;
      const float cbs = cimg::cut(brightness,0.f,2.f);

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02),
          txm = y<y1?tx0 + cimg_rd(dtx01*yy0,dy01):tx1 + cimg_rd(dtx12*yy1,dy12),
          txM = tx0 + cimg_rd(dtx02*yy0,dy02),
          tym = y<y1?ty0 + cimg_rd(dty01*yy0,dy01):ty1 + cimg_rd(dty12*yy1,dy12),
          tyM = ty0 + cimg_rd(dty02*yy0,dy02);
        if (xm>xM) cimg::swap(xm,xM,txm,txM,tym,tyM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT
            dxmM = std::max((longT)1,xM - xm), hdxmM = dxmM/2,
            dtxmM = txM - txm, dtymM = tyM - tym;

          for (int x = cxm; x<=cxM; ++x) {
            const longT
              xxm = (longT)x - xm,
              tx = (txm*dxmM + dtxmM*xxm + hdxmM)/dxmM,
              ty = (tym*dxmM + dtymM*xxm + hdxmM)/dxmM;
            const tc *const color = &texture._atXY(tx,ty);
            cimg_forC(*this,c) {
              const Tfloat val = cbs<=1?color[c*twhd]*cbs:(2 - cbs)*color[c*twhd] + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a 2D textured triangle, with perspective correction.
    template<typename tc>
    CImg<T>& draw_triangle(int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const float opacity=1,
                           const float brightness=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,opacity,brightness);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1;
      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;
      const float cbs = cimg::cut(brightness,0.f,2.f);

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,txzm,txzM,tyzm,tyzM,izm,izM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float dizmM = izM - izm, dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float
              iz = izm + dizmM*xxm/dxmM,
              txz = txzm + dtxzmM*xxm/dxmM,
              tyz = tyzm + dtyzmM*xxm/dxmM;
            const int
              tx = (int)(txz/iz),
              ty = (int)(tyz/iz);
            const tc *const color = &texture._atXY(tx,ty);
            cimg_forC(*this,c) {
              const Tfloat val = cbs<=1?color[c*twhd]*cbs:(2 - cbs)*color[c*twhd] + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a textured 2D triangle, with perspective correction and z-buffering.
    template<typename tz, typename tc>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const float opacity=1,
                           const float brightness=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);

      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,opacity,brightness);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1;
      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;
      const float cbs = cimg::cut(brightness,0.f,2.f);

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,txzm,txzM,tyzm,tyzM,izm,izM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float dizmM = izM - izm, dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              const float
                txz = txzm + dtxzmM*xxm/dxmM,
                tyz = tyzm + dtyzmM*xxm/dxmM;
              const int
                tx = (int)(txz/iz),
                ty = (int)(tyz/iz);
              const tc *const color = &texture._atXY(tx,ty);
              cimg_forC(*this,c) {
                const Tfloat val = cbs<=1?color[c*twhd]*cbs:(2 - cbs)*color[c*twhd] + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;
    }

    //! Draw a Phong-shaded 2D triangle.
    /**
       \param x0 X-coordinate of the first vertex in the image instance.
       \param y0 Y-coordinate of the first vertex in the image instance.
       \param x1 X-coordinate of the second vertex in the image instance.
       \param y1 Y-coordinate of the second vertex in the image instance.
       \param x2 X-coordinate of the third vertex in the image instance.
       \param y2 Y-coordinate of the third vertex in the image instance.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param light Light image.
       \param lx0 X-coordinate of the first vertex in the light image.
       \param ly0 Y-coordinate of the first vertex in the light image.
       \param lx1 X-coordinate of the second vertex in the light image.
       \param ly1 Y-coordinate of the second vertex in the light image.
       \param lx2 X-coordinate of the third vertex in the light image.
       \param ly2 Y-coordinate of the third vertex in the light image.
       \param opacity Drawing opacity.
    **/
    template<typename tc, typename tl>
    CImg<T>& draw_triangle(int x0, int y0,
                           int x1, int y1,
                           int x2, int y2,
                           const tc *const color,
                           const CImg<tl>& light,
                           int lx0, int ly0,
                           int lx1, int ly1,
                           int lx2, int ly2,
                           const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);

      if (y0>y1) cimg::swap(x0,x1,y0,y1,lx0,lx1,ly0,ly1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,lx0,lx2,ly0,ly2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,lx1,lx2,ly1,ly2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2,
        dlx01 = (longT)lx1 - lx0, dlx02 = (longT)lx2 - lx0, dlx12 = (longT)lx2 - lx1,
        dly01 = (longT)ly1 - ly0, dly02 = (longT)ly2 - ly0, dly12 = (longT)ly2 - ly1;
      const ulongT lwhd = (ulongT)light._width*light._height*light._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02),
          lxm = y<y1?lx0 + cimg_rd(dlx01*yy0,dy01):lx1 + cimg_rd(dlx12*yy1,dy12),
          lxM = lx0 + cimg_rd(dlx02*yy0,dy02),
          lym = y<y1?ly0 + cimg_rd(dly01*yy0,dy01):ly1 + cimg_rd(dly12*yy1,dy12),
          lyM = ly0 + cimg_rd(dly02*yy0,dy02);
        if (xm>xM) cimg::swap(xm,xM,lxm,lxM,lym,lyM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT
            dxmM = std::max((longT)1,xM - xm), hdxmM = dxmM/2,
            dlxmM = lxM - lxm, dlymM = lyM - lym;

          for (int x = cxm; x<=cxM; ++x) {
            const longT
              xxm = (longT)x - xm,
              lx = (lxm*dxmM + dlxmM*xxm + hdxmM)/dxmM,
              ly = (lym*dxmM + dlymM*xxm + hdxmM)/dxmM;
            const tl *const lig = &light._atXY(lx,ly);
            cimg_forC(*this,c) {
              const tc col = color[c];
              const float cbs = cimg::cut((float)lig[c*lwhd],0.f,2.f);
              const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a Phong-shaded 2D triangle, with z-buffering.
    template<typename tz, typename tc, typename tl>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const tc *const color,
                           const CImg<tl>& light,
                           int lx0, int ly0,
                           int lx1, int ly1,
                           int lx2, int ly2,
                           const float opacity=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);
      if (is_overlapped(light)) return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,
                                                     +light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,lx0,lx1,ly0,ly1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,lx0,lx2,ly0,ly2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,lx1,lx2,ly1,ly2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2,
        dlx01 = (longT)lx1 - lx0, dlx02 = (longT)lx2 - lx0, dlx12 = (longT)lx2 - lx1,
        dly01 = (longT)ly1 - ly0, dly02 = (longT)ly2 - ly0, dly12 = (longT)ly2 - ly1;
      const float diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1;
      const ulongT lwhd = (ulongT)light._width*light._height*light._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02),
          lxm = y<y1?lx0 + cimg_rd(dlx01*yy0,dy01):lx1 + cimg_rd(dlx12*yy1,dy12),
          lxM = lx0 + cimg_rd(dlx02*yy0,dy02),
          lym = y<y1?ly0 + cimg_rd(dly01*yy0,dy01):ly1 + cimg_rd(dly12*yy1,dy12),
          lyM = ly0 + cimg_rd(dly02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02;

        if (xm>xM) cimg::swap(xm,xM,lxm,lxM,lym,lyM,izm,izM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const longT
            dxmM = std::max((longT)1,xM - xm), hdxmM = dxmM/2,
            dlxmM = lxM - lxm, dlymM = lyM - lym;
          const float dizmM = izM - izm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              const int
                lx = (int)((lxm*dxmM + dlxmM*xxm + hdxmM)/dxmM),
                ly = (int)((lym*dxmM + dlymM*xxm + hdxmM)/dxmM);
              const tl *const lig = &light._atXY(lx,ly);
              cimg_forC(*this,c) {
                const float cbs = cimg::cut((float)lig[c*lwhd],0.f,2.f);
                const tc col = color[c];
                const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;
    }

    //! Draw a textured Gouraud-shaded 2D triangle.
    /**
       \param x0 X-coordinate of the first vertex in the image instance.
       \param y0 Y-coordinate of the first vertex in the image instance.
       \param x1 X-coordinate of the second vertex in the image instance.
       \param y1 Y-coordinate of the second vertex in the image instance.
       \param x2 X-coordinate of the third vertex in the image instance.
       \param y2 Y-coordinate of the third vertex in the image instance.
       \param texture Texture image used to fill the triangle.
       \param tx0 X-coordinate of the first vertex in the texture image.
       \param ty0 Y-coordinate of the first vertex in the texture image.
       \param tx1 X-coordinate of the second vertex in the texture image.
       \param ty1 Y-coordinate of the second vertex in the texture image.
       \param tx2 X-coordinate of the third vertex in the texture image.
       \param ty2 Y-coordinate of the third vertex in the texture image.
       \param bs0 Brightness factor of the first vertex.
       \param bs1 Brightness factor of the second vertex.
       \param bs2 Brightness factor of the third vertex.
       \param opacity Drawing opacity.
    **/
    template<typename tc>
    CImg<T>& draw_triangle(int x0, int y0,
                           int x1, int y1,
                           int x2, int y2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           float bs0,
                           float bs1,
                           float bs2,
                           const float opacity=1) {
      if (is_empty()) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,x1,y1,x2,y2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,
                             bs0,bs1,bs2,opacity);

      if (y0>y1) cimg::swap(x0,x1,y0,y1,tx0,tx1,ty0,ty1,bs0,bs1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,tx0,tx2,ty0,ty2,bs0,bs2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,tx1,tx2,ty1,ty2,bs1,bs2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2,
        dtx01 = (longT)tx1 - tx0, dtx02 = (longT)tx2 - tx0, dtx12 = (longT)tx2 - tx1,
        dty01 = (longT)ty1 - ty0, dty02 = (longT)ty2 - ty0, dty12 = (longT)ty2 - ty1;
      const float dbs01 = bs1 - bs0, dbs02 = bs2 - bs0, dbs12 = bs2 - bs1;
      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02),
          txm = y<y1?tx0 + cimg_rd(dtx01*yy0,dy01):tx1 + cimg_rd(dtx12*yy1,dy12),
          txM = tx0 + cimg_rd(dtx02*yy0,dy02),
          tym = y<y1?ty0 + cimg_rd(dty01*yy0,dy01):ty1 + cimg_rd(dty12*yy1,dy12),
          tyM = ty0 + cimg_rd(dty02*yy0,dy02);
        float
          bsm = y<y1?(bs0 + dbs01*yy0/dy01):(bs1 + dbs12*yy1/dy12),
          bsM = bs0 + dbs02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,txm,txM,tym,tyM,bsm,bsM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT
            dxmM = std::max((longT)1,xM - xm), hdxmM = dxmM/2,
            dtxmM = txM - txm, dtymM = tyM - tym;
          const float dbsmM = bsM - bsm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT
              xxm = (longT)x - xm,
              tx = (txm*dxmM + dtxmM*xxm + hdxmM)/dxmM,
              ty = (tym*dxmM + dtymM*xxm + hdxmM)/dxmM;
            const float cbs = cimg::cut(bsm + dbsmM*xxm/dxmM,0.f,2.f);
            const tc *const color = &texture._atXY(tx,ty);
            cimg_forC(*this,c) {
              const tc col = color[c*twhd];
              const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a textured Gouraud-shaded 2D triangle, with perspective correction \overloading.
    template<typename tc>
    CImg<T>& draw_triangle(int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           float bs0,
                           float bs1,
                           float bs2,
                           const float opacity=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture)) return draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,
                                                       bs0,bs1,bs2,opacity);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1,bs0,bs1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2,bs0,bs2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2,bs1,bs2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1,
        dbs01 = bs1 - bs0, dbs02 = bs2 - bs0, dbs12 = bs2 - bs1;
      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02,
          bsm = y<y1?(bs0 + dbs01*yy0/dy01):(bs1 + dbs12*yy1/dy12),
          bsM = bs0 + dbs02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,txzm,txzM,tyzm,tyzM,izm,izM,bsm,bsM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float dizmM = izM - izm, dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm, dbsmM = bsM - bsm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float
              iz = izm + dizmM*xxm/dxmM,
              txz = txzm + dtxzmM*xxm/dxmM,
              tyz = tyzm + dtyzmM*xxm/dxmM,
              cbs = cimg::cut(bsm + dbsmM*xxm/dxmM,0.f,2.f);
            const int
              tx = (int)(txz/iz),
              ty = (int)(tyz/iz);
            const tc *const color = &texture._atXY(tx,ty);
            cimg_forC(*this,c) {
              const tc col = color[c*twhd];
              const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a textured Gouraud-shaded 2D triangle, with perspective correction and z-buffering \overloading.
    template<typename tz, typename tc>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           float bs0,
                           float bs1,
                           float bs2,
                           const float opacity=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,bs0,bs1,bs2,opacity);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;

      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1,bs0,bs1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2,bs0,bs2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2,bs1,bs2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1,
        dbs01 = bs1 - bs0, dbs02 = bs2 - bs0, dbs12 = bs2 - bs1;
      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02,
          bsm = y<y1?(bs0 + dbs01*yy0/dy01):(bs1 + dbs12*yy1/dy12),
          bsM = bs0 + dbs02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,txzm,txzM,tyzm,tyzM,izm,izM,bsm,bsM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float dizmM = izM - izm, dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm, dbsmM = bsM - bsm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              const float
                txz = txzm + dtxzmM*xxm/dxmM,
                tyz = tyzm + dtyzmM*xxm/dxmM,
                cbs = cimg::cut(bsm + dbsmM*xxm/dxmM,0.f,2.f);
              const int
                tx = (int)(txz/iz),
                ty = (int)(tyz/iz);
              const tc *const color = &texture._atXY(tx,ty);
              cimg_forC(*this,c) {
                const tc col = color[c*twhd];
                const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;
    }

    //! Draw a textured Phong-shaded 2D triangle.
    /**
       \param x0 X-coordinate of the first vertex in the image instance.
       \param y0 Y-coordinate of the first vertex in the image instance.
       \param x1 X-coordinate of the second vertex in the image instance.
       \param y1 Y-coordinate of the second vertex in the image instance.
       \param x2 X-coordinate of the third vertex in the image instance.
       \param y2 Y-coordinate of the third vertex in the image instance.
       \param texture Texture image used to fill the triangle.
       \param tx0 X-coordinate of the first vertex in the texture image.
       \param ty0 Y-coordinate of the first vertex in the texture image.
       \param tx1 X-coordinate of the second vertex in the texture image.
       \param ty1 Y-coordinate of the second vertex in the texture image.
       \param tx2 X-coordinate of the third vertex in the texture image.
       \param ty2 Y-coordinate of the third vertex in the texture image.
       \param light Light image.
       \param lx0 X-coordinate of the first vertex in the light image.
       \param ly0 Y-coordinate of the first vertex in the light image.
       \param lx1 X-coordinate of the second vertex in the light image.
       \param ly1 Y-coordinate of the second vertex in the light image.
       \param lx2 X-coordinate of the third vertex in the light image.
       \param ly2 Y-coordinate of the third vertex in the light image.
       \param opacity Drawing opacity.
    **/
    template<typename tc, typename tl>
    CImg<T>& draw_triangle(int x0, int y0,
                           int x1, int y1,
                           int x2, int y2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const CImg<tl>& light,
                           int lx0, int ly0,
                           int lx1, int ly1,
                           int lx2, int ly2,
                           const float opacity=1) {
      if (is_empty()) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,x1,y1,x2,y2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      if (is_overlapped(light))
        return draw_triangle(x0,y0,x1,y1,x2,y2,texture,tx0,ty0,tx1,ty1,tx2,ty2,+light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);

      if (y0>y1) cimg::swap(x0,x1,y0,y1,tx0,tx1,ty0,ty1,lx0,lx1,ly0,ly1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,tx0,tx2,ty0,ty2,lx0,lx2,ly0,ly2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,tx1,tx2,ty1,ty2,lx1,lx2,ly1,ly2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2,
        dtx01 = (longT)tx1 - tx0, dtx02 = (longT)tx2 - tx0, dtx12 = (longT)tx2 - tx1,
        dty01 = (longT)ty1 - ty0, dty02 = (longT)ty2 - ty0, dty12 = (longT)ty2 - ty1,
        dlx01 = (longT)lx1 - lx0, dlx02 = (longT)lx2 - lx0, dlx12 = (longT)lx2 - lx1,
        dly01 = (longT)ly1 - ly0, dly02 = (longT)ly2 - ly0, dly12 = (longT)ly2 - ly1;
      const ulongT
        twhd = (ulongT)texture._width*texture._height*texture._depth,
        lwhd = (ulongT)light._width*light._height*light._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02),
          txm = y<y1?tx0 + cimg_rd(dtx01*yy0,dy01):tx1 + cimg_rd(dtx12*yy1,dy12),
          txM = tx0 + cimg_rd(dtx02*yy0,dy02),
          tym = y<y1?ty0 + cimg_rd(dty01*yy0,dy01):ty1 + cimg_rd(dty12*yy1,dy12),
          tyM = ty0 + cimg_rd(dty02*yy0,dy02),
          lxm = y<y1?lx0 + cimg_rd(dlx01*yy0,dy01):lx1 + cimg_rd(dlx12*yy1,dy12),
          lxM = lx0 + cimg_rd(dlx02*yy0,dy02),
          lym = y<y1?ly0 + cimg_rd(dly01*yy0,dy01):ly1 + cimg_rd(dly12*yy1,dy12),
          lyM = ly0 + cimg_rd(dly02*yy0,dy02);
        if (xm>xM) cimg::swap(xm,xM,txm,txM,tym,tyM,lxm,lxM,lym,lyM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT
            dxmM = std::max((longT)1,xM - xm), hdxmM = dxmM/2,
            dtxmM = txM - txm, dtymM = tyM - tym,
            dlxmM = lxM - lxm, dlymM = lyM - lym;

          for (int x = cxm; x<=cxM; ++x) {
            const longT
              xxm = (longT)x - xm,
              tx = (txm*dxmM + dtxmM*xxm + hdxmM)/dxmM,
              ty = (tym*dxmM + dtymM*xxm + hdxmM)/dxmM,
              lx = (lxm*dxmM + dlxmM*xxm + hdxmM)/dxmM,
              ly = (lym*dxmM + dlymM*xxm + hdxmM)/dxmM;
            const tc *const color = &texture._atXY(tx,ty);
            const tl *const lig = &light._atXY(lx,ly);
            cimg_forC(*this,c) {
              const tc col = color[c*twhd];
              const float cbs = cimg::cut((float)lig[c*lwhd],0.f,2.f);
              const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a textured Phong-shaded 2D triangle, with perspective correction.
    template<typename tc, typename tl>
    CImg<T>& draw_triangle(int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const CImg<tl>& light,
                           int lx0, int ly0,
                           int lx1, int ly1,
                           int lx2, int ly2,
                           const float opacity=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,
                             light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      if (is_overlapped(light))
        return draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,texture,tx0,ty0,tx1,ty1,tx2,ty2,
                             +light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1,lx0,lx1,ly0,ly1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2,lx0,lx2,ly0,ly2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2,lx1,lx2,ly1,ly2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1,
        lxz0 = lx0*iz0, lxz1 = lx1*iz1, lxz2 = lx2*iz2,
        lyz0 = ly0*iz0, lyz1 = ly1*iz1, lyz2 = ly2*iz2,
        dlxz01 = lxz1 - lxz0, dlxz02 = lxz2 - lxz0, dlxz12 = lxz2 - lxz1,
        dlyz01 = lyz1 - lyz0, dlyz02 = lyz2 - lyz0, dlyz12 = lyz2 - lyz1;
      const ulongT
        twhd = (ulongT)texture._width*texture._height*texture._depth,
        lwhd = (ulongT)light._width*light._height*light._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02,
          lxzm = y<y1?(lxz0 + dlxz01*yy0/dy01):(lxz1 + dlxz12*yy1/dy12),
          lxzM = lxz0 + dlxz02*yy0/dy02,
          lyzm = y<y1?(lyz0 + dlyz01*yy0/dy01):(lyz1 + dlyz12*yy1/dy12),
          lyzM = lyz0 + dlyz02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,izm,izM,txzm,txzM,tyzm,tyzM,lxzm,lxzM,lyzm,lyzM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float
            dizmM = izM - izm,
            dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm,
            dlxzmM = lxzM - lxzm, dlyzmM = lyzM - lyzm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float
              iz = izm + dizmM*xxm/dxmM,
              txz = txzm + dtxzmM*xxm/dxmM,
              tyz = tyzm + dtyzmM*xxm/dxmM,
              lxz = lxzm + dlxzmM*xxm/dxmM,
              lyz = lyzm + dlyzmM*xxm/dxmM;
            const int
              tx = (int)(txz/iz),
              ty = (int)(tyz/iz),
              lx = (int)(lxz/iz),
              ly = (int)(lyz/iz);
            const tc *const color = &texture._atXY(tx,ty);
            const tl *const lig = &light._atXY(lx,ly);
            cimg_forC(*this,c) {
              const tc col = color[c*twhd];
              const float cbs = cimg::cut((float)lig[c*lwhd],0.f,2.f);
              const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a textured Phong-shaded 2D triangle, with perspective correction and z-buffering.
    template<typename tz, typename tc, typename tl>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const CImg<tl>& light,
                           int lx0, int ly0,
                           int lx1, int ly1,
                           int lx2, int ly2,
                           const float opacity=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (is_overlapped(texture))
        return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,
                             +texture,tx0,ty0,tx1,ty1,tx2,ty2,light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      if (is_overlapped(light))
        return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,
                             texture,tx0,ty0,tx1,ty1,tx2,ty2,+light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1,lx0,lx1,ly0,ly1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2,lx0,lx2,ly0,ly2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2,lx1,lx2,ly1,ly2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1,
        lxz0 = lx0*iz0, lxz1 = lx1*iz1, lxz2 = lx2*iz2,
        lyz0 = ly0*iz0, lyz1 = ly1*iz1, lyz2 = ly2*iz2,
        dlxz01 = lxz1 - lxz0, dlxz02 = lxz2 - lxz0, dlxz12 = lxz2 - lxz1,
        dlyz01 = lyz1 - lyz0, dlyz02 = lyz2 - lyz0, dlyz12 = lyz2 - lyz1;
      const ulongT
        twhd = (ulongT)texture._width*texture._height*texture._depth,
        lwhd = (ulongT)light._width*light._height*light._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02,
          lxzm = y<y1?(lxz0 + dlxz01*yy0/dy01):(lxz1 + dlxz12*yy1/dy12),
          lxzM = lxz0 + dlxz02*yy0/dy02,
          lyzm = y<y1?(lyz0 + dlyz01*yy0/dy01):(lyz1 + dlyz12*yy1/dy12),
          lyzM = lyz0 + dlyz02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,izm,izM,txzm,txzM,tyzm,tyzM,lxzm,lxzM,lyzm,lyzM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float
            dizmM = izM - izm,
            dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm,
            dlxzmM = lxzM - lxzm, dlyzmM = lyzM - lyzm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              const float
                txz = txzm + dtxzmM*xxm/dxmM,
                tyz = tyzm + dtyzmM*xxm/dxmM,
                lxz = lxzm + dlxzmM*xxm/dxmM,
                lyz = lyzm + dlyzmM*xxm/dxmM;
              const int
                tx = (int)(txz/iz),
                ty = (int)(tyz/iz),
                lx = (int)(lxz/iz),
                ly = (int)(lyz/iz);
              const tc *const color = &texture._atXY(tx,ty);
              const tl *const lig = &light._atXY(lx,ly);
              cimg_forC(*this,c) {
                const tc col = color[c*twhd];
                const float cbs = cimg::cut((float)lig[c*lwhd],0.f,2.f);
                const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;
    }

    //! Draw a filled 4D rectangle.
    /**
       \param x0 X-coordinate of the upper-left rectangle corner.
       \param y0 Y-coordinate of the upper-left rectangle corner.
       \param z0 Z-coordinate of the upper-left rectangle corner.
       \param c0 C-coordinate of the upper-left rectangle corner.
       \param x1 X-coordinate of the lower-right rectangle corner.
       \param y1 Y-coordinate of the lower-right rectangle corner.
       \param z1 Z-coordinate of the lower-right rectangle corner.
       \param c1 C-coordinate of the lower-right rectangle corner.
       \param val Scalar value used to fill the rectangle area.
       \param opacity Drawing opacity.
    **/
    CImg<T>& draw_rectangle(const int x0, const int y0, const int z0, const int c0,
                            const int x1, const int y1, const int z1, const int c1,
                            const T val, const float opacity=1) {
      if (is_empty()) return *this;
      const int
        nx0 = x0<x1?x0:x1, nx1 = x0^x1^nx0,
        ny0 = y0<y1?y0:y1, ny1 = y0^y1^ny0,
        nz0 = z0<z1?z0:z1, nz1 = z0^z1^nz0,
        nc0 = c0<c1?c0:c1, nc1 = c0^c1^nc0;
      const int
        lx = (1 + nx1 - nx0) + (nx1>=width()?width() - 1 - nx1:0) + (nx0<0?nx0:0),
        ly = (1 + ny1 - ny0) + (ny1>=height()?height() - 1 - ny1:0) + (ny0<0?ny0:0),
        lz = (1 + nz1 - nz0) + (nz1>=depth()?depth() - 1 - nz1:0) + (nz0<0?nz0:0),
        lc = (1 + nc1 - nc0) + (nc1>=spectrum()?spectrum() - 1 - nc1:0) + (nc0<0?nc0:0);
      const ulongT
        offX = (ulongT)_width - lx,
        offY = (ulongT)_width*(_height - ly),
        offZ = (ulongT)_width*_height*(_depth - lz);
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.f);
      T *ptrd = data(nx0<0?0:nx0,ny0<0?0:ny0,nz0<0?0:nz0,nc0<0?0:nc0);
      if (lx>0 && ly>0 && lz>0 && lc>0)
        for (int v = 0; v<lc; ++v) {
          for (int z = 0; z<lz; ++z) {
            for (int y = 0; y<ly; ++y) {
              if (opacity>=1) {
                if (sizeof(T)!=1) { for (int x = 0; x<lx; ++x) *(ptrd++) = val; ptrd+=offX; }
                else { std::memset(ptrd,(int)val,lx); ptrd+=_width; }
              } else { for (int x = 0; x<lx; ++x) { *ptrd = (T)(nopacity*val + *ptrd*copacity); ++ptrd; } ptrd+=offX; }
            }
            ptrd+=offY;
          }
          ptrd+=offZ;
        }
      return *this;
    }

    //! Draw a filled 3D rectangle.
    /**
       \param x0 X-coordinate of the upper-left rectangle corner.
       \param y0 Y-coordinate of the upper-left rectangle corner.
       \param z0 Z-coordinate of the upper-left rectangle corner.
       \param x1 X-coordinate of the lower-right rectangle corner.
       \param y1 Y-coordinate of the lower-right rectangle corner.
       \param z1 Z-coordinate of the lower-right rectangle corner.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0, const int z0,
                            const int x1, const int y1, const int z1,
                            const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_rectangle(): Specified color is (null).",
                                    cimg_instance);
      cimg_forC(*this,c) draw_rectangle(x0,y0,z0,c,x1,y1,z1,c,(T)color[c],opacity);
      return *this;
    }

    //! Draw a filled 2D rectangle.
    /**
       \param x0 X-coordinate of the upper-left rectangle corner.
       \param y0 Y-coordinate of the upper-left rectangle corner.
       \param x1 X-coordinate of the lower-right rectangle corner.
       \param y1 Y-coordinate of the lower-right rectangle corner.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0,
                            const int x1, const int y1,
                            const tc *const color, const float opacity=1) {
      return draw_rectangle(x0,y0,0,x1,y1,_depth - 1,color,opacity);
    }

    //! Draw a outlined 2D rectangle \overloading.
    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0,
                            const int x1, const int y1,
                            const tc *const color, const float opacity,
                            const unsigned int pattern) {
      if (is_empty()) return *this;
      if (y0==y1) return draw_line(x0,y0,x1,y0,color,opacity,pattern,true);
      if (x0==x1) return draw_line(x0,y0,x0,y1,color,opacity,pattern,true);
      const int
        nx0 = x0<x1?x0:x1, nx1 = x0^x1^nx0,
        ny0 = y0<y1?y0:y1, ny1 = y0^y1^ny0;
      if (ny1==ny0 + 1) return draw_line(nx0,ny0,nx1,ny0,color,opacity,pattern,true).
                      draw_line(nx1,ny1,nx0,ny1,color,opacity,pattern,false);
      return draw_line(nx0,ny0,nx1,ny0,color,opacity,pattern,true).
        draw_line(nx1,ny0 + 1,nx1,ny1 - 1,color,opacity,pattern,false).
        draw_line(nx1,ny1,nx0,ny1,color,opacity,pattern,false).
        draw_line(nx0,ny1 - 1,nx0,ny0 + 1,color,opacity,pattern,false);
    }

    //! Draw a filled 2D polygon.
    /**
       \param points Set of polygon vertices.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
     **/
    template<typename tp, typename tc>
    CImg<T>& draw_polygon(const CImg<tp>& points,
                          const tc *const color, const float opacity=1) {
      if (is_empty() || !points) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Specified color is (null).",
                                    cimg_instance);
      if (points.height()!=2)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Invalid specified point set (%u,%u,%u,%u).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum);
      CImg<intT> ipoints;
      if (cimg::type<tp>::is_float()) ipoints = points.get_round();
      else ipoints.assign(points,cimg::type<tp>::string()==cimg::type<int>::string());

      if (ipoints._width==1) return draw_point(ipoints(0,0),ipoints(0,1),color,opacity);
      if (ipoints._width==2) return draw_line(ipoints(0,0),ipoints(0,1),ipoints(1,0),ipoints(1,1),color,opacity);
      if (ipoints._width==3) return draw_triangle(ipoints(0,0),ipoints(0,1),ipoints(1,0),ipoints(1,1),
                                                  ipoints(2,0),ipoints(2,1),color,opacity);
      cimg_init_scanline(opacity);
      int
        xmin = 0, ymin = 0,
        xmax = ipoints.get_shared_row(0).max_min(xmin),
        ymax = ipoints.get_shared_row(1).max_min(ymin);
      if (xmax<0 || xmin>=width() || ymax<0 || ymin>=height()) return *this;
      if (ymin==ymax) return draw_line(xmin,ymin,xmax,ymax,color,opacity);
      ymin = std::max(0,ymin);
      ymax = std::min(height() - 1,ymax);

      CImg<intT> Xs(ipoints._width,ymax - ymin + 1);
      CImg<uintT> count(Xs._height,1,1,1,0);
      unsigned int n = 0, nn = 1;
      bool go_on = true;

      while (go_on) {
        unsigned int an = (nn + 1)%ipoints._width;
        const int x0 = ipoints(n,0), y0 = ipoints(n,1);
        if (ipoints(nn,1)==y0) while (ipoints(an,1)==y0) { nn = an; (an+=1)%=ipoints._width; }
        const int x1 = ipoints(nn,0), y1 = ipoints(nn,1);
        unsigned int tn = an;
        while (ipoints(tn,1)==y1) (tn+=1)%=ipoints._width;
        if (y0!=y1) {
          const int
            y2 = ipoints(tn,1),
            x01 = x1 - x0, y01 = y1 - y0, y12 = y2 - y1,
            step = cimg::sign(y01),
            tmax = std::max(1,cimg::abs(y01)),
            htmax = tmax*cimg::sign(x01)/2 + 1,
            tend = tmax - (step==cimg::sign(y12));
          unsigned int y = (unsigned int)y0 - ymin;
          for (int t = 0; t<=tend; ++t, y+=step)
            if (y<Xs._height) Xs(count[y]++,y) = x0 + cimg_rd(t*x01,tmax);
        }
        go_on = nn>n;
        n = nn;
        nn = an;
      }

      cimg_pragma_openmp(parallel for cimg_openmp_if(Xs._height>=(cimg_openmp_sizefactor)*512))
      cimg_forY(Xs,y) if (count[y]) {
        const CImg<intT> Xsy = Xs.get_shared_points(0,count[y] - 1,y).sort();
        int px = width();
        for (unsigned int k = 0; k<Xsy._width; k+=2) {
          int x0 = Xsy[k];
          const int x1 = Xsy[k + 1];
          x0+=x0==px;
          cimg_draw_scanline(x0,x1,y + ymin,color,opacity,1);
          px = x1;
        }
      }
      return *this;
    }

    //! Draw a outlined 2D or 3D polygon \overloading.
    template<typename tp, typename tc>
    CImg<T>& draw_polygon(const CImg<tp>& points,
                          const tc *const color, const float opacity, const unsigned int pattern,
                          const bool is_closed=true) {
      if (is_empty() || !points) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Specified color is (null).",
                                    cimg_instance);
      if (points.height()!=2)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Invalid specified point set (%u,%u,%u,%u).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum);
      CImg<intT> ipoints;
      if (cimg::type<tp>::is_float()) ipoints = points.get_round();
      else ipoints.assign(points,cimg::type<tp>::string()==cimg::type<int>::string());

      if (ipoints._width==1) return draw_point(ipoints(0,0),ipoints(0,1),color,opacity);
      if (ipoints._width==2) return draw_line(ipoints(0,0),ipoints(0,1),ipoints(1,0),ipoints(1,1),
                                              color,opacity,pattern);
      bool ninit_hatch = true, is_drawn = false;
      int x0 = ipoints(0,0), y0 = ipoints(0,1);
      const unsigned int N = ipoints._width - (is_closed?0:1);
      for (unsigned int i = 0; i<N; ++i) {
        const int
          ni = (i + 1)%ipoints.width(),
          x1 = ipoints(ni,0), y1 = ipoints(ni,1),
          x01 = x1 - x0, y01 = y1 - y0,
          l = std::max(cimg::abs(x01),cimg::abs(y01));
        if (l) {
          const bool draw_last_pixel = is_closed || i<N - 1?false:true;
          draw_line(x0,y0,x1,y1,color,opacity,pattern,ninit_hatch,draw_last_pixel);
          is_drawn = true;
        }
        ninit_hatch = false;
        x0 = x1; y0 = y1;
      }
      if (!is_drawn) draw_point(ipoints(0,0),ipoints(0,1),color,opacity); // All vertices were the same
      return *this;
    }

    //! Draw a filled 2D ellipse.
    /**
       \param x0 X-coordinate of the ellipse center.
       \param y0 Y-coordinate of the ellipse center.
       \param r1 First radius of the ellipse.
       \param r2 Second radius of the ellipse.
       \param angle Angle of the first radius.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const float r1, const float r2, const float angle,
                          const tc *const color, const float opacity=1) {
      return _draw_ellipse(x0,y0,r1,r2,angle,color,opacity,0U,true);
    }

    //! Draw a filled 2D ellipse \overloading.
    /**
       \param x0 X-coordinate of the ellipse center.
       \param y0 Y-coordinate of the ellipse center.
       \param tensor Diffusion tensor describing the ellipse.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename t, typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const CImg<t> &tensor,
                          const tc *const color, const float opacity=1) {
      CImgList<t> eig = tensor.get_symmetric_eigen();
      const CImg<t> &val = eig[0], &vec = eig[1];
      return draw_ellipse(x0,y0,std::sqrt(val(0)),std::sqrt(val(1)),
                          std::atan2(vec(0,1),vec(0,0))*180/cimg::PI,
                          color,opacity);
    }

    //! Draw an outlined 2D ellipse.
    /**
       \param x0 X-coordinate of the ellipse center.
       \param y0 Y-coordinate of the ellipse center.
       \param r1 First radius of the ellipse.
       \param r2 Second radius of the ellipse.
       \param angle Angle of the first radius.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the outline pattern.
    **/
    template<typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const float r1, const float r2, const float angle,
                          const tc *const color, const float opacity, const unsigned int pattern) {
      if (pattern) _draw_ellipse(x0,y0,r1,r2,angle,color,opacity,pattern,false);
      return *this;
    }

    //! Draw an outlined 2D ellipse \overloading.
    /**
       \param x0 X-coordinate of the ellipse center.
       \param y0 Y-coordinate of the ellipse center.
       \param tensor Diffusion tensor describing the ellipse.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the outline pattern.
    **/
    template<typename t, typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const CImg<t> &tensor,
                          const tc *const color, const float opacity,
                          const unsigned int pattern) {
      CImgList<t> eig = tensor.get_symmetric_eigen();
      const CImg<t> &val = eig[0], &vec = eig[1];
      return draw_ellipse(x0,y0,std::sqrt(val(0)),std::sqrt(val(1)),
                          std::atan2(vec(0,1),vec(0,0))*180/cimg::PI,
                          color,opacity,pattern);
    }

    template<typename tc>
    CImg<T>& _draw_ellipse(const int x0, const int y0, const float radius1, const float radius2, const float angle,
                           const tc *const color, const float opacity,
                           const unsigned int pattern, const bool is_filled) {
      if (is_empty() || (!is_filled && !pattern)) return *this;
      const float radiusM = std::max(radius1,radius2);
      if (radius1<0 || radius2<0 || x0 - radiusM>=width() || y0 + radiusM<0 || y0 - radiusM>=height()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_ellipse(): Specified color is (null).",
                                    cimg_instance);
      const int iradius1 = (int)cimg::round(radius1), iradius2 = (int)cimg::round(radius2);
      if (!iradius1 && !iradius2) return draw_point(x0,y0,color,opacity);
      if (iradius1==iradius2) {
        if (is_filled) return draw_circle(x0,y0,iradius1,color,opacity);
        else if (pattern==~0U) return draw_circle(x0,y0,iradius1,color,opacity,pattern);
      }
      const float ang = (float)(angle*cimg::PI/180);

      if (!is_filled) { // Outlined
        const float ca = std::cos(ang), sa = std::sin(ang);
        CImg<int> points((unsigned int)cimg::round(6*radiusM),2);
        cimg_forX(points,k) {
          const float
            _ang = (float)(2*cimg::PI*k/points._width),
            X = (float)(radius1*std::cos(_ang)),
            Y = (float)(radius2*std::sin(_ang));
          points(k,0) = (int)cimg::round(x0 + (X*ca - Y*sa));
          points(k,1) = (int)cimg::round(y0 + (X*sa + Y*ca));
        }
        draw_polygon(points,color,opacity,pattern);
      } else { // Filled
        cimg_init_scanline(opacity);
        const float
          ca = std::cos(ang),
          sa = -std::sin(ang),
          ca2 = ca*ca,
          sa2 = sa*sa,
          casa = ca*sa,
          i1 = 1/cimg::sqr(radius1),
          i2 = 1/cimg::sqr(radius2),
          t1 = i1*ca2 + i2*sa2,
          t2 = (i2 - i1)*casa,
          t3 = i2*ca2 + i1*sa2,
          t12 = t1*2;
        const int
          _ymin = (int)std::floor(y0 - radiusM),
          _ymax = (int)std::ceil(y0 + radiusM),
          ymin = _ymin<0?0:_ymin,
          ymax = _ymax>=height()?height() - 1:_ymax;
        for (int y = ymin; y<=ymax; ++y) {
          const float
            Y = y - y0 + 0.5f,
            B = 2*t2*Y,
            C = t3*Y*Y - 1,
            D = B*B - 4*t1*C;
          if (D>=0) {
            const float sD = std::sqrt(D);
            const int
              xmin = (int)(x0 + cimg::round((-B - sD)/t12)),
              xmax = (int)(x0 + cimg::round((-B + sD)/t12));
            cimg_draw_scanline(xmin,xmax,y,color,opacity,1);
          }
        }
      }
      return *this;
    }

    //! Draw a filled 2D circle.
    /**
       \param x0 X-coordinate of the circle center.
       \param y0 Y-coordinate of the circle center.
       \param radius  Circle radius.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \note
       - Circle version of the Bresenham's algorithm is used.
    **/
    template<typename tc>
    CImg<T>& draw_circle(const int x0, const int y0, int radius,
                         const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (radius<0 || x0 - radius>=width() || y0 + radius<0 || y0 - radius>=height()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_circle(): Specified color is (null).",
                                    cimg_instance);
      if (!radius) return draw_point(x0,y0,color,opacity);
      cimg_init_scanline(opacity);
      if (y0>=0 && y0<height()) cimg_draw_scanline(x0 - radius,x0 + radius,y0,color,opacity,1);
      for (int f = 1 - radius, ddFx = 0, ddFy = -(radius<<1), x = 0, y = radius; x<y; ) {
        if (f>=0) {
          const int x1 = x0 - x, x2 = x0 + x, y1 = y0 - y, y2 = y0 + y;
          if (y1>=0 && y1<height()) cimg_draw_scanline(x1,x2,y1,color,opacity,1);
          if (y2>=0 && y2<height()) cimg_draw_scanline(x1,x2,y2,color,opacity,1);
          f+=(ddFy+=2); --y;
        }
        const bool no_diag = y!=(x++);
        ++(f+=(ddFx+=2));
        const int x1 = x0 - y, x2 = x0 + y, y1 = y0 - x, y2 = y0 + x;
        if (no_diag) {
          if (y1>=0 && y1<height()) cimg_draw_scanline(x1,x2,y1,color,opacity,1);
          if (y2>=0 && y2<height()) cimg_draw_scanline(x1,x2,y2,color,opacity,1);
        }
      }
      return *this;
    }

    //! Draw an outlined 2D circle.
    /**
       \param x0 X-coordinate of the circle center.
       \param y0 Y-coordinate of the circle center.
       \param radius Circle radius.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the outline pattern.
    **/
    template<typename tc>
    CImg<T>& draw_circle(const int x0, const int y0, int radius,
                         const tc *const color, const float opacity,
                         const unsigned int pattern) {
      if (pattern!=~0U) return draw_ellipse(x0,y0,radius,radius,0,color,opacity,pattern);
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_circle(): Specified color is (null).",
                                    cimg_instance);
      if (radius<0 || x0 - radius>=width() || y0 + radius<0 || y0 - radius>=height()) return *this;
      if (!radius) return draw_point(x0,y0,color,opacity);

      draw_point(x0 - radius,y0,color,opacity).draw_point(x0 + radius,y0,color,opacity).
        draw_point(x0,y0 - radius,color,opacity).draw_point(x0,y0 + radius,color,opacity);
      if (radius==1) return *this;
      for (int f = 1 - radius, ddFx = 0, ddFy = -(radius<<1), x = 0, y = radius; x<y; ) {
        if (f>=0) { f+=(ddFy+=2); --y; }
        ++x; ++(f+=(ddFx+=2));
        if (x!=y + 1) {
          const int x1 = x0 - y, x2 = x0 + y, y1 = y0 - x, y2 = y0 + x,
            x3 = x0 - x, x4 = x0 + x, y3 = y0 - y, y4 = y0 + y;
          draw_point(x1,y1,color,opacity).draw_point(x1,y2,color,opacity).
            draw_point(x2,y1,color,opacity).draw_point(x2,y2,color,opacity);
          if (x!=y)
            draw_point(x3,y3,color,opacity).draw_point(x4,y4,color,opacity).
              draw_point(x4,y3,color,opacity).draw_point(x3,y4,color,opacity);
        }
      }
      return *this;
    }

#endif
