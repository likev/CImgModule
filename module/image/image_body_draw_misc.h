#ifndef CIMG_MODULE_IMAGE_BODY_DRAW_MISC_H
#define CIMG_MODULE_IMAGE_BODY_DRAW_MISC_H

            ref(xt,yt) = cimg::type<T>::cut(val);
          }

        for (int _delta = delta; _delta>1; _delta>>=1) {
          const int _delta2 = _delta>>1;
          r = alpha*_delta + beta;

          // Square step.
          for (int yt = _delta2; yt<h; yt+=_delta)
            for (int xt = _delta2; xt<w; xt+=_delta) {
              const int x0 = xt - _delta2, y0 = yt - _delta2, x1 = (xt + _delta2)%w, y1 = (yt + _delta2)%h;
              const Tfloat val = (Tfloat)(0.25f*(ref(x0,y0) + ref(x1,y0) + ref(x1,y1) + ref(x0,y1)) +
                                          r*cimg::rand(-1,1,&rng));
              ref(xt,yt) = cimg::type<T>::cut(val);
            }

          // Diamond steps.
          bool is_odd_y = false;
          for (int yt = 0; yt<h; yt+=_delta2) {
            for (int xt = is_odd_y?0:_delta2; xt<w; xt+=_delta) {
              const int
                x0 = cimg::mod(xt - _delta2,w), x1 = (xt + _delta2)%w,
                y0 = cimg::mod(yt - _delta2,h), y1 = (yt + _delta2)%h;
              const Tfloat val = (Tfloat)(0.25f*(ref(x0,yt) + ref(x1,yt) + ref(xt,y0) + ref(xt,y1)) +
                                          r*cimg::rand(-1,1,&rng));
              ref(xt,yt) = cimg::type<T>::cut(val);
            }
            is_odd_y = !is_odd_y;
          }
        }
      }
      cimg::srand(rng);
      return draw_image((w0 - w)/2,(h0 - h)/2,0,0,canvas);
    }

    //! Draw a quadratic Mandelbrot or Julia 2D fractal.
    /**
       \param x0 X-coordinate of the upper-left pixel.
       \param y0 Y-coordinate of the upper-left pixel.
       \param x1 X-coordinate of the lower-right pixel.
       \param y1 Y-coordinate of the lower-right pixel.
       \param palette Colormap.
       \param opacity Drawing opacity.
       \param z0r Real part of the upper-left fractal vertex.
       \param z0i Imaginary part of the upper-left fractal vertex.
       \param z1r Real part of the lower-right fractal vertex.
       \param z1i Imaginary part of the lower-right fractal vertex.
       \param iteration_max Maximum number of iterations for each estimated point.
       \param is_normalized_iteration Tells if iterations are normalized.
       \param is_julia_set Tells if the Mandelbrot or Julia set is rendered.
       \param param_r Real part of the Julia set parameter.
       \param param_i Imaginary part of the Julia set parameter.
       \note Fractal rendering is done by the Escape Time Algorithm.
    **/
    template<typename tc>
    CImg<T>& draw_mandelbrot(const int x0, const int y0, const int x1, const int y1,
                             const CImg<tc>& colormap, const float opacity=1,
                             const double z0r=-2, const double z0i=-2, const double z1r=2, const double z1i=2,
                             const unsigned int iteration_max=255,
                             const bool is_normalized_iteration=false,
                             const bool is_julia_set=false,
                             const double param_r=0, const double param_i=0) {
      if (is_empty()) return *this;
      CImg<tc> palette;
      if (colormap) palette.assign(colormap._data,colormap.size()/colormap._spectrum,1,1,colormap._spectrum,true);
      if (palette && palette._spectrum!=_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_mandelbrot(): Instance and specified colormap (%u,%u,%u,%u,%p) have "
                                    "incompatible dimensions.",
                                    cimg_instance,
                                    colormap._width,colormap._height,colormap._depth,colormap._spectrum,colormap._data);

      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.f), ln2 = (float)std::log(2.);
      const int
        _x0 = cimg::cut(x0,0,width() - 1),
        _y0 = cimg::cut(y0,0,height() - 1),
        _x1 = cimg::cut(x1,0,width() - 1),
        _y1 = cimg::cut(y1,0,height() - 1);

      cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                         cimg_openmp_if((1 + _x1 - _x0)*(1 + _y1 - _y0)>=(cimg_openmp_sizefactor)*2048))
      for (int q = _y0; q<=_y1; ++q)
        for (int p = _x0; p<=_x1; ++p) {
          unsigned int iteration = 0;
          const double x = z0r + p*(z1r-z0r)/_width, y = z0i + q*(z1i-z0i)/_height;
          double zr, zi, cr, ci;
          if (is_julia_set) { zr = x; zi = y; cr = param_r; ci = param_i; }
          else { zr = param_r; zi = param_i; cr = x; ci = y; }
          for (iteration=1; zr*zr + zi*zi<=4 && iteration<=iteration_max; ++iteration) {
            const double temp = zr*zr - zi*zi + cr;
            zi = 2*zr*zi + ci;
            zr = temp;
          }
          if (iteration>iteration_max) {
            if (palette) {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)palette(0,c);
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)(palette(0,c)*nopacity + (*this)(p,q,0,c)*copacity);
            } else {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)0;
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)((*this)(p,q,0,c)*copacity);
            }
          } else if (is_normalized_iteration) {
            const float
              normz = (float)cimg::abs(zr*zr + zi*zi),
              niteration = (float)(iteration + 1 - std::log(std::log(normz))/ln2);
            if (palette) {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)palette._linear_atX(niteration,c);
              else cimg_forC(*this,c)
                     (*this)(p,q,0,c) = (T)(palette._linear_atX(niteration,c)*nopacity + (*this)(p,q,0,c)*copacity);
            } else {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)niteration;
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)(niteration*nopacity + (*this)(p,q,0,c)*copacity);
            }
          } else {
            if (palette) {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)palette._atX(iteration,c);
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)(palette(iteration,c)*nopacity + (*this)(p,q,0,c)*copacity);
            } else {
              if (opacity>=1) cimg_forC(*this,c) (*this)(p,q,0,c) = (T)iteration;
              else cimg_forC(*this,c) (*this)(p,q,0,c) = (T)(iteration*nopacity + (*this)(p,q,0,c)*copacity);
            }
          }
        }
      return *this;
    }

    //! Draw a quadratic Mandelbrot or Julia 2D fractal \overloading.
    template<typename tc>
    CImg<T>& draw_mandelbrot(const CImg<tc>& colormap, const float opacity=1,
                             const double z0r=-2, const double z0i=-2, const double z1r=2, const double z1i=2,
                             const unsigned int iteration_max=255,
                             const bool is_normalized_iteration=false,
                             const bool is_julia_set=false,
                             const double param_r=0, const double param_i=0) {
      return draw_mandelbrot(0,0,_width - 1,_height - 1,colormap,opacity,
                             z0r,z0i,z1r,z1i,iteration_max,is_normalized_iteration,is_julia_set,param_r,param_i);
    }

    //! Draw a 1D gaussian function.
    /**
       \param xc X-coordinate of the gaussian center.
       \param sigma Standard variation of the gaussian distribution.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename tc>
    CImg<T>& draw_gaussian(const float xc, const float sigma,
                           const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_gaussian(): Specified color is (null).",
                                    cimg_instance);
      const float sigma2 = 2*sigma*sigma, nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.f);
      const ulongT whd = (ulongT)_width*_height*_depth;
      const tc *col = color;
      cimg_forX(*this,x) {
        const float dx = (x - xc), val = (float)std::exp(-dx*dx/sigma2);
        T *ptrd = data(x,0,0,0);
        if (opacity>=1) cimg_forC(*this,c) { *ptrd = (T)(val*(*col++)); ptrd+=whd; }
        else cimg_forC(*this,c) { *ptrd = (T)(nopacity*val*(*col++) + *ptrd*copacity); ptrd+=whd; }
        col-=_spectrum;
      }
      return *this;
    }

    //! Draw a 2D gaussian function.
    /**
       \param xc X-coordinate of the gaussian center.
       \param yc Y-coordinate of the gaussian center.
       \param tensor Covariance matrix (must be 2x2).
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename t, typename tc>
    CImg<T>& draw_gaussian(const float xc, const float yc, const CImg<t>& tensor,
                           const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (tensor._width!=2 || tensor._height!=2 || tensor._depth!=1 || tensor._spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "draw_gaussian(): Specified tensor (%u,%u,%u,%u,%p) is not a 2x2 matrix.",
                                    cimg_instance,
                                    tensor._width,tensor._height,tensor._depth,tensor._spectrum,tensor._data);
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_gaussian(): Specified color is (null).",
                                    cimg_instance);
      typedef typename CImg<t>::Tfloat tfloat;
      const CImg<tfloat> invT = tensor.get_invert(), invT2 = (invT*invT)/=-2.;
      const tfloat a = invT2(0,0), b = 2*invT2(1,0), c = invT2(1,1);
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.f);
      const ulongT whd = (ulongT)_width*_height*_depth;
      const tc *col = color;
      float dy = -yc;
      cimg_forY(*this,y) {
        float dx = -xc;
        cimg_forX(*this,x) {
          const float val = (float)std::exp(a*dx*dx + b*dx*dy + c*dy*dy);
          T *ptrd = data(x,y,0,0);
          if (opacity>=1) cimg_forC(*this,k) { *ptrd = (T)(val*(*col++)); ptrd+=whd; }
          else cimg_forC(*this,k) { *ptrd = (T)(nopacity*val*(*col++) + *ptrd*copacity); ptrd+=whd; }
          col-=_spectrum;
          ++dx;
        }
        ++dy;
      }
      return *this;
    }

    //! Draw a 2D gaussian function \overloading.
    template<typename tc>
    CImg<T>& draw_gaussian(const int xc, const int yc, const float r1, const float r2, const float ru, const float rv,
                           const tc *const color, const float opacity=1) {
      const double
        a = r1*ru*ru + r2*rv*rv,
        b = (r1-r2)*ru*rv,
        c = r1*rv*rv + r2*ru*ru;
      const CImg<Tfloat> tensor(2,2,1,1, a,b,b,c);
      return draw_gaussian(xc,yc,tensor,color,opacity);
    }

    //! Draw a 2D gaussian function \overloading.
    template<typename tc>
    CImg<T>& draw_gaussian(const float xc, const float yc, const float sigma,
                           const tc *const color, const float opacity=1) {
      return draw_gaussian(xc,yc,CImg<floatT>::diagonal(sigma,sigma),color,opacity);
    }

    //! Draw a 3D gaussian function \overloading.
    template<typename t, typename tc>
    CImg<T>& draw_gaussian(const float xc, const float yc, const float zc, const CImg<t>& tensor,
                           const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      typedef typename CImg<t>::Tfloat tfloat;
      if (tensor._width!=3 || tensor._height!=3 || tensor._depth!=1 || tensor._spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "draw_gaussian(): Specified tensor (%u,%u,%u,%u,%p) is not a 3x3 matrix.",
                                    cimg_instance,
                                    tensor._width,tensor._height,tensor._depth,tensor._spectrum,tensor._data);

      const CImg<tfloat> invT = tensor.get_invert(), invT2 = (invT*invT)/=-2.;
      const tfloat a = invT2(0,0), b = 2*invT2(1,0), c = 2*invT2(2,0), d = invT2(1,1), e = 2*invT2(2,1), f = invT2(2,2);
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.f);
      const ulongT whd = (ulongT)_width*_height*_depth;
      const tc *col = color;
      cimg_forXYZ(*this,x,y,z) {
        const float
          dx = (x - xc), dy = (y - yc), dz = (z - zc),
          val = (float)std::exp(a*dx*dx + b*dx*dy + c*dx*dz + d*dy*dy + e*dy*dz + f*dz*dz);
        T *ptrd = data(x,y,z,0);
        if (opacity>=1) cimg_forC(*this,k) { *ptrd = (T)(val*(*col++)); ptrd+=whd; }
        else cimg_forC(*this,k) { *ptrd = (T)(nopacity*val*(*col++) + *ptrd*copacity); ptrd+=whd; }
        col-=_spectrum;
      }
      return *this;
    }

    //! Draw a 3D gaussian function \overloading.
    template<typename tc>
    CImg<T>& draw_gaussian(const float xc, const float yc, const float zc, const float sigma,
                           const tc *const color, const float opacity=1) {
      return draw_gaussian(xc,yc,zc,CImg<floatT>::diagonal(sigma,sigma,sigma),color,opacity);
    }

    //! Draw a 3D object.
    /**
       \param x0 X-coordinate of the 3D object position
       \param y0 Y-coordinate of the 3D object position
       \param z0 Z-coordinate of the 3D object position
       \param vertices Image Nx3 describing 3D point coordinates
       \param primitives List of P primitives
       \param colors List of P color (or textures)
       \param opacities Image or list of P opacities
       \param render_type Render type (0=Points, 1=Lines, 2=Faces (no light), 3=Faces (flat), 4=Faces(Gouraud)
       \param is_double_sided Tells if object faces have two sides or are oriented.
       \param focale length of the focale (0 for parallel projection)
       \param lightx X-coordinate of the light
       \param lighty Y-coordinate of the light
       \param lightz Z-coordinate of the light
       \param specular_lightness Amount of specular light.
       \param specular_shininess Shininess of the object
       \param g_opacity Global opacity of the object.
       \param is_multithreaded_rendering Tells if mesh rendering is done with multiple threads
    **/
    template<typename tp, typename tf, typename tc, typename to>
    CImg<T>& draw_object3d(const float x0, const float y0, const float z0,
                           const CImg<tp>& vertices, const CImgList<tf>& primitives,
                           const CImgList<tc>& colors, const CImg<to>& opacities,
                           const unsigned int render_type=4,
                           const bool is_double_sided=false, const float focale=700,
                           const float lightx=0, const float lighty=0, const float lightz=-5e8,
                           const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                           const float g_opacity=1, const bool is_multithreaded_rendering=false) {
      return draw_object3d(x0,y0,z0,vertices,primitives,colors,opacities,render_type,
                           is_double_sided,focale,lightx,lighty,lightz,
                           specular_lightness,specular_shininess,g_opacity,CImg<floatT>::empty(),
                           is_multithreaded_rendering);
    }

    //! Draw a 3D object \simplification.
    template<typename tp, typename tf, typename tc, typename to, typename tz>
    CImg<T>& draw_object3d(const float x0, const float y0, const float z0,
                           const CImg<tp>& vertices, const CImgList<tf>& primitives,
                           const CImgList<tc>& colors, const CImg<to>& opacities,
                           const unsigned int render_type,
                           const bool is_double_sided, const float focale,
                           const float lightx, const float lighty, const float lightz,
                           const float specular_lightness, const float specular_shininess,
                           const float g_opacity, CImg<tz>& zbuffer,
                           const bool is_multithreaded_rendering=false) {
      return _draw_object3d(zbuffer,x0,y0,z0,vertices,primitives,colors,opacities,
                            render_type,is_double_sided,focale,lightx,lighty,lightz,
                            specular_lightness,specular_shininess,g_opacity,1,
                            is_multithreaded_rendering);
    }

    //! Draw a 3D object \simplification.
    template<typename tp, typename tf, typename tc, typename to>
    CImg<T>& draw_object3d(const float x0, const float y0, const float z0,
                           const CImg<tp>& vertices, const CImgList<tf>& primitives,
                           const CImgList<tc>& colors, const CImgList<to>& opacities,
                           const unsigned int render_type=4,
                           const bool is_double_sided=false, const float focale=700,
                           const float lightx=0, const float lighty=0, const float lightz=-5e8,
                           const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                           const float g_opacity=1, const bool is_multithreaded_rendering=false) {
      return draw_object3d(x0,y0,z0,vertices,primitives,colors,opacities,render_type,
                           is_double_sided,focale,lightx,lighty,lightz,
                           specular_lightness,specular_shininess,g_opacity,CImg<floatT>::empty(),
                           is_multithreaded_rendering);
    }

    //! Draw a 3D object \simplification.
    template<typename tp, typename tf, typename tc, typename to, typename tz>
    CImg<T>& draw_object3d(const float x0, const float y0, const float z0,
                           const CImg<tp>& vertices, const CImgList<tf>& primitives,
                           const CImgList<tc>& colors, const CImgList<to>& opacities,
                           const unsigned int render_type,
                           const bool is_double_sided, const float focale,
                           const float lightx, const float lighty, const float lightz,
                           const float specular_lightness, const float specular_shininess,
                           const float g_opacity, CImg<tz>& zbuffer,
                           const bool is_multithreaded_rendering=false) {
      return _draw_object3d(zbuffer,x0,y0,z0,vertices,primitives,colors,opacities,
                            render_type,is_double_sided,focale,lightx,lighty,lightz,
                            specular_lightness,specular_shininess,g_opacity,1,
                            is_multithreaded_rendering);
    }

    //! Draw a 3D object \simplification.
    template<typename tp, typename tf, typename tc>
    CImg<T>& draw_object3d(const float x0, const float y0, const float z0,
                           const CImg<tp>& vertices, const CImgList<tf>& primitives,
                           const CImgList<tc>& colors,
                           const unsigned int render_type=4,
                           const bool is_double_sided=false, const float focale=700,
                           const float lightx=0, const float lighty=0, const float lightz=-5e8,
                           const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                           const float g_opacity=1, const bool is_multithreaded_rendering=false) {
      return draw_object3d(x0,y0,z0,vertices,primitives,colors,CImg<floatT>::const_empty(),
                           render_type,is_double_sided,focale,lightx,lighty,lightz,
                           specular_lightness,specular_shininess,g_opacity,CImg<floatT>::empty(),
                           is_multithreaded_rendering);
    }

    //! Draw a 3D object \simplification.
    template<typename tp, typename tf, typename tc, typename tz>
    CImg<T>& draw_object3d(const float x0, const float y0, const float z0,
                           const CImg<tp>& vertices, const CImgList<tf>& primitives,
                           const CImgList<tc>& colors,
                           const unsigned int render_type,
                           const bool is_double_sided, const float focale,
                           const float lightx, const float lighty, const float lightz,
                           const float specular_lightness, const float specular_shininess,
                           const float g_opacity, CImg<tz>& zbuffer,
                           const bool is_multithreaded_rendering=false) {
      return draw_object3d(x0,y0,z0,vertices,primitives,colors,CImg<floatT>::const_empty(),
                           render_type,is_double_sided,focale,lightx,lighty,lightz,
                           specular_lightness,specular_shininess,g_opacity,zbuffer,
                           is_multithreaded_rendering);
    }

    template<typename t, typename to>
    static float __draw_object3d(const CImgList<t>& opacities, const unsigned int n_primitive, CImg<to>& opacity) {
      if (n_primitive>=opacities._width || opacities[n_primitive].is_empty()) { opacity.assign(); return 1; }
      if (opacities[n_primitive].size()==1) { opacity.assign(); return opacities(n_primitive,0); }
      opacity.assign(opacities[n_primitive],true);
      return 1.f;
    }

    template<typename t, typename to>
    static float __draw_object3d(const CImg<t>& opacities, const unsigned int n_primitive, CImg<to>& opacity) {
      opacity.assign();
      return n_primitive>=opacities._width?1.f:(float)opacities[n_primitive];
    }

    template<typename t>
    static float ___draw_object3d(const CImgList<t>& opacities, const unsigned int n_primitive) {
      return n_primitive<opacities._width && opacities[n_primitive].size()==1?(float)opacities(n_primitive,0):1.f;
    }

    template<typename t>
    static float ___draw_object3d(const CImg<t>& opacities, const unsigned int n_primitive) {
      return n_primitive<opacities._width?(float)opacities[n_primitive]:1.f;
    }

    // Draw flat-colored segment (with z-plane clipping).
    template<typename tz, typename tp, typename tc>
    CImg<T>& _draw_object3d_flat_colored_segment(CImg<tz>& zbuffer,
                                                 const float X, const float Y, const float Z,
                                                 int n0, int n1,
                                                 const CImg<tp>& vertices, const CImg<floatT>& projections,
                                                 const tc *const color, const float opacity,
                                                 float focale) {
      float z0 = vertices(n0,2) + Z + focale, z1 = vertices(n1,2) + Z + focale;
      if (z0>z1) cimg::swap(n0,n1,z0,z1);
      int
        x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
        x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1));
      const float zc = 1; // Clipping plane
      if (focale && z0<zc) {
        if (z1<zc) return *this; // Two vertices behind camera
        const float
          fx0 = vertices(n0,0), fy0 = vertices(n0,1),
          fx1 = vertices(n1,0), fy1 = vertices(n1,1),
          fact = (zc - z0)/(z1 - z0), nfx0 = fx0 + fact*(fx1 - fx0), nfy0 = fy0 + fact*(fy1 - fy0);
        z0 = zc;
        x0 = cimg::uiround(X + focale*nfx0/z0); y0 = cimg::uiround(Y + focale*nfy0/z0);
      }
      if (zbuffer) draw_line(zbuffer,x0,y0,z0,x1,y1,z1,color,opacity);
      else draw_line(x0,y0,x1,y1,color,opacity);
      return *this;
    }

    // Draw flat-textured segment (with z-plane clipping).
    template<typename tz, typename tp, typename tc>
    CImg<T>& _draw_object3d_flat_textured_segment(CImg<tz>& zbuffer,
                                                  const float X, const float Y, const float Z,
                                                  int n0, int n1,
                                                  const CImg<tp>& vertices, const CImg<floatT>& projections,
                                                  const CImg<tc>& texture,
                                                  int tx0, int ty0,
                                                  int tx1, int ty1,
                                                  const float opacity,
                                                  float focale) {
      float z0 = vertices(n0,2) + Z + focale, z1 = vertices(n1,2) + Z + focale;
      if (focale && z0>z1) cimg::swap(n0,n1,z0,z1,tx0,tx1,ty0,ty1);
      int
        x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
        x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1));
      const float zc = 1; // Clipping plane
      if (z0<zc) {
        if (z1<zc) return *this; // Two vertices behind camera
        const float
          fx0 = vertices(n0,0), fy0 = vertices(n0,1),
          fx1 = vertices(n1,0), fy1 = vertices(n1,1),
          fact = (zc - z0)/(z1 - z0), nfx0 = fx0 + fact*(fx1 - fx0), nfy0 = fy0 + fact*(fy1 - fy0);
        if (!focale) focale = zc;
        z0 = zc;
        x0 = cimg::uiround(X + focale*nfx0/z0); y0 = cimg::uiround(Y + focale*nfy0/z0);
        tx0 = cimg::uiround(tx0 + fact*(tx1 - tx0)); ty0 = cimg::uiround(ty0 + fact*(ty1 - ty0));
      }
      if (zbuffer) draw_line(zbuffer,x0,y0,z0,x1,y1,z1,texture,tx0,ty0,tx1,ty1,opacity);
      else draw_line(x0,y0,z0,x1,y1,z1,texture,tx0,ty0,tx1,ty1,opacity);
      return *this;
    }

    // Draw flat-colored triangle (with z-plane clipping).
    template<typename tz, typename tp, typename tc>
    CImg<T>& _draw_object3d_flat_colored_triangle(CImg<tz>& zbuffer,
                                                  const float X, const float Y, const float Z,
                                                  int n0, int n1, int n2,
                                                  const CImg<tp>& vertices, const CImg<floatT>& projections,
                                                  const tc *const color, const float opacity,
                                                  const float brightness, float focale) {
      float z0 = vertices(n0,2) + Z + focale, z1 = vertices(n1,2) + Z + focale, z2 = vertices(n2,2) + Z + focale;
      if (z0>z2) cimg::swap(n0,n2,z0,z2);
      if (z0>z1) cimg::swap(n0,n1,z0,z1);
      if (z1>z2) cimg::swap(n1,n2,z1,z2);
      int
        x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
        x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
        x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1));
      const float zc = 1; // Clipping plane
      if (focale && z0<zc) {
        if (z2<zc) return *this;
        const float
          fx0 = vertices(n0,0), fy0 = vertices(n0,1),
          fx1 = vertices(n1,0), fy1 = vertices(n1,1),
          fx2 = vertices(n2,0), fy2 = vertices(n2,1);
        if (z1<zc) { // Two vertices behind camera
          const float
            fact0 = (zc - z0)/(z2 - z0), nfx0 = fx0 + fact0*(fx2 - fx0), nfy0 = fy0 + fact0*(fy2 - fy0),
            fact1 = (zc - z1)/(z2 - z1), nfx1 = fx1 + fact1*(fx2 - fx1), nfy1 = fy1 + fact1*(fy2 - fy1);
          z0 = z1 = zc;
          x0 = cimg::uiround(X + focale*nfx0/z0); y0 = cimg::uiround(Y + focale*nfy0/z0);
          x1 = cimg::uiround(X + focale*nfx1/z1); y1 = cimg::uiround(Y + focale*nfy1/z1);
        } else { // One vertex behind camera
          const float
            fact0 = (zc - z0)/(z1 - z0), nfx0 = fx0 + fact0*(fx1 - fx0), nfy0 = fy0 + fact0*(fy1 - fy0),
            fact1 = (zc - z0)/(z2 - z0), nfx1 = fx0 + fact1*(fx2 - fx0), nfy1 = fy0 + fact1*(fy2 - fy0),
            nz0 = zc, nz1 = zc;
          const int
            nx0 = cimg::uiround(X + focale*nfx0/nz0), ny0 = cimg::uiround(Y + focale*nfy0/nz0),
            nx1 = cimg::uiround(X + focale*nfx1/nz1), ny1 = cimg::uiround(Y + focale*nfy1/nz1);
          if (brightness==1) {
            if (zbuffer) draw_triangle(zbuffer,nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,color,opacity).
                           draw_triangle(zbuffer,nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,color,opacity);
            else draw_triangle(nx0,ny0,x1,y1,x2,y2,color,opacity).
                   draw_triangle(nx0,ny0,nx1,ny1,x2,y2,color,opacity);
          } else {
            if (zbuffer) draw_triangle(zbuffer,nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,color,opacity,brightness).
                           draw_triangle(zbuffer,nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,color,opacity,brightness);
            else _draw_triangle(nx0,ny0,x1,y1,x2,y2,color,opacity,brightness).
                   _draw_triangle(nx0,ny0,nx1,ny1,x2,y2,color,opacity,brightness);
          }
          return *this;
        }
      }
      if (brightness==1) {
        if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,opacity);
        else draw_triangle(x0,y0,x1,y1,x2,y2,color,opacity);
      } else {
        if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,opacity,brightness);
        else _draw_triangle(x0,y0,x1,y1,x2,y2,color,opacity,brightness);
      }
      return *this;
    }

    // Draw flat-textured triangle (with z-plane clipping).
    template<typename tz, typename tp, typename tc>
    CImg<T>& _draw_object3d_flat_textured_triangle(CImg<tz>& zbuffer,
                                                   const float X, const float Y, const float Z,
                                                   int n0, int n1, int n2,
                                                   const CImg<tp>& vertices, const CImg<floatT>& projections,
                                                   const CImg<tc>& texture,
                                                   int tx0, int ty0,
                                                   int tx1, int ty1,
                                                   int tx2, int ty2,
                                                   const float opacity,
                                                   const float brightness, float focale) {
      float z0 = vertices(n0,2) + Z + focale, z1 = vertices(n1,2) + Z + focale, z2 = vertices(n2,2) + Z + focale;
      if (z0>z2) cimg::swap(n0,n2,z0,z2,tx0,tx2,ty0,ty2);
      if (z0>z1) cimg::swap(n0,n1,z0,z1,tx0,tx1,ty0,ty1);
      if (z1>z2) cimg::swap(n1,n2,z1,z2,tx1,tx2,ty1,ty2);
      int
        x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
        x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
        x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1));
      const float zc = 1; // Clipping plane
      if (focale && z0<zc) {
        if (z2<zc) return *this;
        const float
          fx0 = vertices(n0,0), fy0 = vertices(n0,1),
          fx1 = vertices(n1,0), fy1 = vertices(n1,1),
          fx2 = vertices(n2,0), fy2 = vertices(n2,1);
        if (z1<zc) { // Two vertices behind camera
          const float
            fact0 = (zc - z0)/(z2 - z0), nfx0 = fx0 + fact0*(fx2 - fx0), nfy0 = fy0 + fact0*(fy2 - fy0),
            fact1 = (zc - z1)/(z2 - z1), nfx1 = fx1 + fact1*(fx2 - fx1), nfy1 = fy1 + fact1*(fy2 - fy1);
          z0 = z1 = zc;
          x0 = cimg::uiround(X + focale*nfx0/z0); y0 = cimg::uiround(Y + focale*nfy0/z0);
          x1 = cimg::uiround(X + focale*nfx1/z1); y1 = cimg::uiround(Y + focale*nfy1/z1);
          tx0 = cimg::uiround(tx0 + fact0*(tx2 - tx0)); ty0 = cimg::uiround(ty0 + fact0*(ty2 - ty0));
          tx1 = cimg::uiround(tx1 + fact1*(tx2 - tx1)); ty1 = cimg::uiround(ty1 + fact1*(ty2 - ty1));
        } else { // One vertex behind camera
          const float
            fact0 = (zc - z0)/(z1 - z0), nfx0 = fx0 + fact0*(fx1 - fx0), nfy0 = fy0 + fact0*(fy1 - fy0),
            fact1 = (zc - z0)/(z2 - z0), nfx1 = fx0 + fact1*(fx2 - fx0), nfy1 = fy0 + fact1*(fy2 - fy0),
            nz0 = zc, nz1 = zc;
          const int
            nx0 = cimg::uiround(X + focale*nfx0/nz0), ny0 = cimg::uiround(Y + focale*nfy0/nz0),
            nx1 = cimg::uiround(X + focale*nfx1/nz1), ny1 = cimg::uiround(Y + focale*nfy1/nz1),
            ntx0 = cimg::uiround(tx0 + fact0*(tx1 - tx0)), nty0 = cimg::uiround(ty0 + fact0*(ty1 - ty0)),
            ntx1 = cimg::uiround(tx0 + fact1*(tx2 - tx0)), nty1 = cimg::uiround(ty0 + fact1*(ty2 - ty0));
          if (zbuffer) draw_triangle(zbuffer,nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,
                                     texture,ntx0,nty0,tx1,ty1,tx2,ty2,opacity,brightness).
                         draw_triangle(zbuffer,nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,
                                       texture,ntx0,nty0,ntx1,nty1,tx2,ty2,opacity,brightness);
          else draw_triangle(nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,
                             texture,ntx0,nty0,tx1,ty1,tx2,ty2,opacity,brightness).
                 draw_triangle(nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,
                               texture,ntx0,nty0,ntx1,nty1,tx2,ty2,opacity,brightness);
          return *this;
        }
      }
      if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,
                                 texture,tx0,ty0,tx1,ty1,tx2,ty2,opacity,brightness);
      else draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,
                         texture,tx0,ty0,tx1,ty1,tx2,ty2,opacity,brightness);
      return *this;
    }

    // Draw gouraud-colored triangle (with z-plane clipping).
    template<typename tz, typename tp, typename tc>
    CImg<T>& _draw_object3d_gouraud_colored_triangle(CImg<tz>& zbuffer,
                                                     const float X, const float Y, const float Z,
                                                     int n0, int n1, int n2,
                                                     const CImg<tp>& vertices, const CImg<floatT>& projections,
                                                     const tc *const color,
                                                     float bs0, float bs1, float bs2,
                                                     const float opacity, float focale) {
      float z0 = vertices(n0,2) + Z + focale, z1 = vertices(n1,2) + Z + focale, z2 = vertices(n2,2) + Z + focale;
      if (z0>z2) cimg::swap(n0,n2,z0,z2,bs0,bs2);
      if (z0>z1) cimg::swap(n0,n1,z0,z1,bs0,bs1);
      if (z1>z2) cimg::swap(n1,n2,z1,z2,bs1,bs2);
      int
        x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
        x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
        x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1));
      const float zc = 1; // Clipping plane
      if (focale && z0<zc) {
        if (z2<zc) return *this;
        const float
          fx0 = vertices(n0,0), fy0 = vertices(n0,1),
          fx1 = vertices(n1,0), fy1 = vertices(n1,1),
          fx2 = vertices(n2,0), fy2 = vertices(n2,1);
        if (z1<zc) { // Two vertices behind camera
          const float
            fact0 = (zc - z0)/(z2 - z0), nfx0 = fx0 + fact0*(fx2 - fx0), nfy0 = fy0 + fact0*(fy2 - fy0),
            fact1 = (zc - z1)/(z2 - z1), nfx1 = fx1 + fact1*(fx2 - fx1), nfy1 = fy1 + fact1*(fy2 - fy1);
          z0 = z1 = zc;
          x0 = cimg::uiround(X + focale*nfx0/z0); y0 = cimg::uiround(Y + focale*nfy0/z0);
          x1 = cimg::uiround(X + focale*nfx1/z1); y1 = cimg::uiround(Y + focale*nfy1/z1);
          bs0 = bs0 + fact0*(bs2 - bs0);
          bs1 = bs1 + fact1*(bs2 - bs1);
        } else { // One vertex behind camera
          const float
            fact0 = (zc - z0)/(z1 - z0), nfx0 = fx0 + fact0*(fx1 - fx0), nfy0 = fy0 + fact0*(fy1 - fy0),
            fact1 = (zc - z0)/(z2 - z0), nfx1 = fx0 + fact1*(fx2 - fx0), nfy1 = fy0 + fact1*(fy2 - fy0),
            nbs0 = bs0 + fact0*(bs1 - bs0),
            nbs1 = bs0 + fact1*(bs2 - bs0),
            nz0 = zc, nz1 = zc;
          const int
            nx0 = cimg::uiround(X + focale*nfx0/nz0), ny0 = cimg::uiround(Y + focale*nfy0/nz0),
            nx1 = cimg::uiround(X + focale*nfx1/nz1), ny1 = cimg::uiround(Y + focale*nfy1/nz1);
          if (zbuffer) draw_triangle(zbuffer,nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,color,nbs0,bs1,bs2,opacity).
                         draw_triangle(zbuffer,nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,color,nbs0,nbs1,bs2,opacity);
          else draw_triangle(nx0,ny0,x1,y1,x2,y2,color,nbs0,bs1,bs2,opacity).
                 draw_triangle(nx0,ny0,nx1,ny1,x2,y2,color,nbs0,nbs1,bs2,opacity);


          return *this;
        }
      }
      if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,bs0,bs1,bs2,opacity);
      else draw_triangle(x0,y0,x1,y1,x2,y2,color,bs0,bs1,bs2,opacity);
      return *this;
    }

    // Draw gouraud-textured triangle (with z-plane clipping).
    template<typename tz, typename tp, typename tc>
    CImg<T>& _draw_object3d_gouraud_textured_triangle(CImg<tz>& zbuffer,
                                                      const float X, const float Y, const float Z,
                                                      int n0, int n1, int n2,
                                                      const CImg<tp>& vertices, const CImg<floatT>& projections,
                                                      const CImg<tc>& texture,
                                                      int tx0, int ty0,
                                                      int tx1, int ty1,
                                                      int tx2, int ty2,
                                                      float bs0, float bs1, float bs2,
                                                      const float opacity, float focale) {
      float z0 = vertices(n0,2) + Z + focale, z1 = vertices(n1,2) + Z + focale, z2 = vertices(n2,2) + Z + focale;
      if (z0>z2) cimg::swap(n0,n2,z0,z2,tx0,tx2,ty0,ty2,bs0,bs2);
      if (z0>z1) cimg::swap(n0,n1,z0,z1,tx0,tx1,ty0,ty1,bs0,bs1);
      if (z1>z2) cimg::swap(n1,n2,z1,z2,tx1,tx2,ty1,ty2,bs1,bs2);
      int
        x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
        x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
        x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1));
      const float zc = 1; // Clipping plane
      if (focale && z0<zc) {
        if (z2<zc) return *this;
        const float
          fx0 = vertices(n0,0), fy0 = vertices(n0,1),
          fx1 = vertices(n1,0), fy1 = vertices(n1,1),
          fx2 = vertices(n2,0), fy2 = vertices(n2,1);
        if (z1<zc) { // Two vertices behind camera
          const float
            fact0 = (zc - z0)/(z2 - z0), nfx0 = fx0 + fact0*(fx2 - fx0), nfy0 = fy0 + fact0*(fy2 - fy0),
            fact1 = (zc - z1)/(z2 - z1), nfx1 = fx1 + fact1*(fx2 - fx1), nfy1 = fy1 + fact1*(fy2 - fy1);
          z0 = z1 = zc;
          x0 = cimg::uiround(X + focale*nfx0/z0); y0 = cimg::uiround(Y + focale*nfy0/z0);
          x1 = cimg::uiround(X + focale*nfx1/z1); y1 = cimg::uiround(Y + focale*nfy1/z1);
          tx0 = cimg::uiround(tx0 + fact0*(tx2 - tx0)); ty0 = cimg::uiround(ty0 + fact0*(ty2 - ty0));
          tx1 = cimg::uiround(tx1 + fact1*(tx2 - tx1)); ty1 = cimg::uiround(ty1 + fact1*(ty2 - ty1));
          bs0 = bs0 + fact0*(bs2 - bs0);
          bs1 = bs1 + fact1*(bs2 - bs1);
        } else { // One vertex behind camera
          const float
            fact0 = (zc - z0)/(z1 - z0), nfx0 = fx0 + fact0*(fx1 - fx0), nfy0 = fy0 + fact0*(fy1 - fy0),
            fact1 = (zc - z0)/(z2 - z0), nfx1 = fx0 + fact1*(fx2 - fx0), nfy1 = fy0 + fact1*(fy2 - fy0),
            nbs0 = bs0 + fact0*(bs1 - bs0),
            nbs1 = bs0 + fact1*(bs2 - bs0),
            nz0 = zc, nz1 = zc;
          const int
            nx0 = cimg::uiround(X + focale*nfx0/nz0), ny0 = cimg::uiround(Y + focale*nfy0/nz0),
            nx1 = cimg::uiround(X + focale*nfx1/nz1), ny1 = cimg::uiround(Y + focale*nfy1/nz1),
            ntx0 = cimg::uiround(tx0 + fact0*(tx1 - tx0)), nty0 = cimg::uiround(ty0 + fact0*(ty1 - ty0)),
            ntx1 = cimg::uiround(tx0 + fact1*(tx2 - tx0)), nty1 = cimg::uiround(ty0 + fact1*(ty2 - ty0));
          if (zbuffer) draw_triangle(zbuffer,nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,
                                     texture,ntx0,nty0,tx1,ty1,tx2,ty2,nbs0,bs1,bs2,opacity).
                         draw_triangle(zbuffer,nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,
                                       texture,ntx0,nty0,ntx1,nty1,tx2,ty2,nbs0,nbs1,bs2,opacity);
          else draw_triangle(nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,
                             texture,ntx0,nty0,tx1,ty1,tx2,ty2,nbs0,bs1,bs2,opacity).
                 draw_triangle(nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,
                               texture,ntx0,nty0,ntx1,nty1,tx2,ty2,nbs0,nbs1,bs2,opacity);
          return *this;
        }
      }
      if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,texture,tx0,ty0,tx1,ty1,tx2,ty2,
                                 bs0,bs1,bs2,opacity);
      else draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,texture,tx0,ty0,tx1,ty1,tx2,ty2,bs0,bs1,bs2,opacity);
      return *this;
    }

    // Draw phong-colored triangle (with z-plane clipping).
    template<typename tz, typename tp, typename tc>
    CImg<T>& _draw_object3d_phong_colored_triangle(CImg<tz>& zbuffer,
                                                   const float X, const float Y, const float Z,
                                                   int n0, int n1, int n2,
                                                   const CImg<tp>& vertices, const CImg<floatT>& projections,
                                                   const tc *const color,
                                                   const CImg<floatT>& light_texture,
                                                   int lx0, int ly0,
                                                   int lx1, int ly1,
                                                   int lx2, int ly2,
                                                   const float opacity, float focale) {
      float z0 = vertices(n0,2) + Z + focale, z1 = vertices(n1,2) + Z + focale, z2 = vertices(n2,2) + Z + focale;
      if (z0>z2) cimg::swap(n0,n2,z0,z2,lx0,lx2,ly0,ly2);
      if (z0>z1) cimg::swap(n0,n1,z0,z1,lx0,lx1,ly0,ly1);
      if (z1>z2) cimg::swap(n1,n2,z1,z2,lx1,lx2,ly1,ly2);
      int
        x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
        x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
        x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1));
      const float zc = 1; // Clipping plane
      if (focale && z0<zc) {
        if (z2<zc) return *this;
        const float
          fx0 = vertices(n0,0), fy0 = vertices(n0,1),
          fx1 = vertices(n1,0), fy1 = vertices(n1,1),
          fx2 = vertices(n2,0), fy2 = vertices(n2,1);
        if (z1<zc) { // Two vertices behind camera
          const float
            fact0 = (zc - z0)/(z2 - z0), nfx0 = fx0 + fact0*(fx2 - fx0), nfy0 = fy0 + fact0*(fy2 - fy0),
            fact1 = (zc - z1)/(z2 - z1), nfx1 = fx1 + fact1*(fx2 - fx1), nfy1 = fy1 + fact1*(fy2 - fy1);
          z0 = z1 = zc;
          x0 = cimg::uiround(X + focale*nfx0/z0); y0 = cimg::uiround(Y + focale*nfy0/z0);
          x1 = cimg::uiround(X + focale*nfx1/z1); y1 = cimg::uiround(Y + focale*nfy1/z1);
          lx0 = cimg::uiround(lx0 + fact0*(lx2 - lx0)); ly0 = cimg::uiround(ly0 + fact0*(ly2 - ly0));
          lx1 = cimg::uiround(lx1 + fact1*(lx2 - lx1)); ly1 = cimg::uiround(ly1 + fact1*(ly2 - ly1));
        } else { // One vertex behind camera
          const float
            fact0 = (zc - z0)/(z1 - z0), nfx0 = fx0 + fact0*(fx1 - fx0), nfy0 = fy0 + fact0*(fy1 - fy0),
            fact1 = (zc - z0)/(z2 - z0), nfx1 = fx0 + fact1*(fx2 - fx0), nfy1 = fy0 + fact1*(fy2 - fy0),
            nz0 = zc, nz1 = zc;
          const int
            nx0 = cimg::uiround(X + focale*nfx0/nz0), ny0 = cimg::uiround(Y + focale*nfy0/nz0),
            nx1 = cimg::uiround(X + focale*nfx1/nz1), ny1 = cimg::uiround(Y + focale*nfy1/nz1),
            nlx0 = cimg::uiround(lx0 + fact0*(lx1 - lx0)), nly0 = cimg::uiround(ly0 + fact0*(ly1 - ly0)),
            nlx1 = cimg::uiround(lx0 + fact1*(lx2 - lx0)), nly1 = cimg::uiround(ly0 + fact1*(ly2 - ly0));
          if (zbuffer) draw_triangle(zbuffer,nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,color,
                                     light_texture,nlx0,nly0,lx1,ly1,lx2,ly2,opacity).
                         draw_triangle(zbuffer,nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,color,
                                       light_texture,nlx0,nly0,nlx1,nly1,lx2,ly2,opacity);
          else draw_triangle(nx0,ny0,x1,y1,x2,y2,color,
                             light_texture,nlx0,nly0,lx1,ly1,lx2,ly2,opacity).
                 draw_triangle(nx0,ny0,nx1,ny1,x2,y2,color,
                               light_texture,nlx0,nly0,nlx1,nly1,lx2,ly2,opacity);
          return *this;
        }
      }
      if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,
                                 light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      else draw_triangle(x0,y0,x1,y1,x2,y2,color,
                         light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      return *this;
    }

    // Draw phong-textured triangle (with z-plane clipping).
    template<typename tz, typename tp, typename tc>
    CImg<T>& _draw_object3d_phong_textured_triangle(CImg<tz>& zbuffer,
                                                    const float X, const float Y, const float Z,
                                                    int n0, int n1, int n2,
                                                    const CImg<tp>& vertices, const CImg<floatT>& projections,
                                                    const CImg<tc>& texture,
                                                    int tx0, int ty0, int tx1, int ty1, int tx2, int ty2,
                                                    const CImg<floatT>& light_texture,
                                                    int lx0, int ly0, int lx1, int ly1, int lx2, int ly2,
                                                    const float opacity, float focale) {
      float z0 = vertices(n0,2) + Z + focale, z1 = vertices(n1,2) + Z + focale, z2 = vertices(n2,2) + Z + focale;
      if (z0>z2) cimg::swap(n0,n2,z0,z2,tx0,tx2,ty0,ty2,lx0,lx2,ly0,ly2);
      if (z0>z1) cimg::swap(n0,n1,z0,z1,tx0,tx1,ty0,ty1,lx0,lx1,ly0,ly1);
      if (z1>z2) cimg::swap(n1,n2,z1,z2,tx1,tx2,ty1,ty2,lx1,lx2,ly1,ly2);
      int
        x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
        x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
        x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1));
      const float zc = 1; // Clipping plane
      if (focale && z0<zc) {
        if (z2<zc) return *this;
        const float
          fx0 = vertices(n0,0), fy0 = vertices(n0,1),
          fx1 = vertices(n1,0), fy1 = vertices(n1,1),
          fx2 = vertices(n2,0), fy2 = vertices(n2,1);
        if (z1<zc) { // Two vertices behind camera
          const float
            fact0 = (zc - z0)/(z2 - z0), nfx0 = fx0 + fact0*(fx2 - fx0), nfy0 = fy0 + fact0*(fy2 - fy0),
            fact1 = (zc - z1)/(z2 - z1), nfx1 = fx1 + fact1*(fx2 - fx1), nfy1 = fy1 + fact1*(fy2 - fy1);
          z0 = z1 = zc;
          x0 = cimg::uiround(X + focale*nfx0/z0); y0 = cimg::uiround(Y + focale*nfy0/z0);
          x1 = cimg::uiround(X + focale*nfx1/z1); y1 = cimg::uiround(Y + focale*nfy1/z1);
          tx0 = cimg::uiround(tx0 + fact0*(tx2 - tx0)); ty0 = cimg::uiround(ty0 + fact0*(ty2 - ty0));
          tx1 = cimg::uiround(tx1 + fact1*(tx2 - tx1)); ty1 = cimg::uiround(ty1 + fact1*(ty2 - ty1));
          lx0 = cimg::uiround(lx0 + fact0*(lx2 - lx0)); ly0 = cimg::uiround(ly0 + fact0*(ly2 - ly0));
          lx1 = cimg::uiround(lx1 + fact1*(lx2 - lx1)); ly1 = cimg::uiround(ly1 + fact1*(ly2 - ly1));
        } else { // One vertex behind camera
          const float
            fact0 = (zc - z0)/(z1 - z0), nfx0 = fx0 + fact0*(fx1 - fx0), nfy0 = fy0 + fact0*(fy1 - fy0),
            fact1 = (zc - z0)/(z2 - z0), nfx1 = fx0 + fact1*(fx2 - fx0), nfy1 = fy0 + fact1*(fy2 - fy0),
            nz0 = zc, nz1 = zc;
          const int
            nx0 = cimg::uiround(X + focale*nfx0/nz0), ny0 = cimg::uiround(Y + focale*nfy0/nz0),
            nx1 = cimg::uiround(X + focale*nfx1/nz1), ny1 = cimg::uiround(Y + focale*nfy1/nz1),
            ntx0 = cimg::uiround(tx0 + fact0*(tx1 - tx0)), nty0 = cimg::uiround(ty0 + fact0*(ty1 - ty0)),
            ntx1 = cimg::uiround(tx0 + fact1*(tx2 - tx0)), nty1 = cimg::uiround(ty0 + fact1*(ty2 - ty0)),
            nlx0 = cimg::uiround(lx0 + fact0*(lx1 - lx0)), nly0 = cimg::uiround(ly0 + fact0*(ly1 - ly0)),
            nlx1 = cimg::uiround(lx0 + fact1*(lx2 - lx0)), nly1 = cimg::uiround(ly0 + fact1*(ly2 - ly0));
          if (zbuffer) draw_triangle(zbuffer,nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,
                                     texture,ntx0,nty0,tx1,ty1,tx2,ty2,
                                     light_texture,nlx0,nly0,lx1,ly1,lx2,ly2,opacity).
                         draw_triangle(zbuffer,nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,
                                       texture,ntx0,nty0,ntx1,nty1,tx2,ty2,
                                       light_texture,nlx0,nly0,nlx1,nly1,lx2,ly2,opacity);
          else draw_triangle(nx0,ny0,nz0,x1,y1,z1,x2,y2,z2,
                             texture,ntx0,nty0,tx1,ty1,tx2,ty2,
                             light_texture,nlx0,nly0,lx1,ly1,lx2,ly2,opacity).
                 draw_triangle(nx0,ny0,nz0,nx1,ny1,nz1,x2,y2,z2,
                               texture,ntx0,nty0,ntx1,nty1,tx2,ty2,
                               light_texture,nlx0,nly0,nlx1,nly1,lx2,ly2,opacity);
          return *this;
        }
      }
      if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,
                                 texture,tx0,ty0,tx1,ty1,tx2,ty2,
                                 light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      else draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,
                         texture,tx0,ty0,tx1,ty1,tx2,ty2,
                         light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      return *this;
    }

    template<typename tz, typename tp, typename tf, typename tc, typename to>
    CImg<T>& _draw_object3d(CImg<tz>& zbuffer,
                            const float X, const float Y, const float Z,
                            const CImg<tp>& vertices,
                            const CImgList<tf>& primitives,
                            const CImgList<tc>& colors,
                            const to& opacities,
                            const unsigned int render_type,
                            const bool is_double_sided, const float focale,
                            const float lightx, const float lighty, const float lightz,
                            const float specular_lightness, const float specular_shininess,
                            const float g_opacity, const float sprite_scale,
                            const bool is_multithreaded_rendering) {
      typedef typename to::value_type _to;
      cimg::unused(is_multithreaded_rendering);
      if (is_empty() || !vertices || !primitives) return *this;
      CImg<char> error_message(1024);
      if (!vertices.is_object3d(primitives,colors,opacities,false,error_message))
        throw CImgArgumentException(_cimg_instance
                                    "draw_object3d(): Invalid specified 3D object (%u,%u) (%s).",
                                    cimg_instance,vertices._width,primitives._width,error_message.data());
      if (render_type==5) cimg::mutex(10); // Static variable used in this case, breaks thread-safety

      const float
        _focale = focale?cimg::abs(focale):1e8f, // Parallel projection -> very large focale
        nspec = 1 - cimg::cut(specular_shininess,0.0f,1.0f),
        nspec2 = 1 + std::max(specular_lightness,0.0f),
        nsl1 = (nspec2 - 1)/cimg::sqr(nspec - 1),
        nsl2 = 1 - 2*nsl1*nspec,
        nsl3 = nspec2 - nsl1 - nsl2;

      // Create light texture for phong-like rendering.
      CImg<floatT> light_texture;
      if (render_type==5) {
        if (colors._width>primitives._width) {
          static CImg<floatT> default_light_texture;
          static const tc *lptr = 0;
          static tc ref_values[64] = {};
          const CImg<tc>& img = colors.back();
          bool is_same_texture = (lptr==img._data);
          if (is_same_texture)
            for (unsigned int r = 0, j = 0; j<8; ++j)
              for (unsigned int i = 0; i<8; ++i)
                if (ref_values[r++]!=img(i*img._width/9,j*img._height/9,0,(i + j)%img._spectrum)) {
                  is_same_texture = false; break;
                }
          if (!is_same_texture || default_light_texture._spectrum<_spectrum) {
            (default_light_texture.assign(img,false)/=255).resize(-100,-100,1,_spectrum);
            lptr = colors.back().data();
            for (unsigned int r = 0, j = 0; j<8; ++j)
              for (unsigned int i = 0; i<8; ++i)
                ref_values[r++] = img(i*img._width/9,j*img._height/9,0,(i + j)%img._spectrum);
          }
          light_texture.assign(default_light_texture,true);
        } else {
          static CImg<floatT> default_light_texture;
          static float olightx = 0, olighty = 0, olightz = 0, ospecular_shininess = 0;
          if (!default_light_texture ||
              lightx!=olightx || lighty!=olighty || lightz!=olightz ||
              specular_shininess!=ospecular_shininess || default_light_texture._spectrum<_spectrum) {
            default_light_texture.assign(512,512);
            const float
              dlx = lightx - X,
              dly = lighty - Y,
              dlz = lightz - Z,
              nl = cimg::hypot(dlx,dly,dlz),
              nlx = (default_light_texture._width - 1)/2*(1 + dlx/nl),
              nly = (default_light_texture._height - 1)/2*(1 + dly/nl),
              white[] = { 1 };
            default_light_texture.draw_gaussian(nlx,nly,default_light_texture._width/3.0f,white);
            cimg_forXY(default_light_texture,x,y) {
              const float factor = default_light_texture(x,y);
              if (factor>nspec) default_light_texture(x,y) = std::min(2.f,nsl1*factor*factor + nsl2*factor + nsl3);
            }
            default_light_texture.resize(-100,-100,1,_spectrum);
            olightx = lightx; olighty = lighty; olightz = lightz; ospecular_shininess = specular_shininess;
          }
          light_texture.assign(default_light_texture,true);
        }
      }

      // Compute normal 3D vectors.
      CImg<floatT> p_centers, p_normals, v_normals;
      if (render_type>=2) {

        // 3D normals to primitives.
        p_centers.assign(primitives._width,3);
        p_normals.assign(primitives._width,3);
        cimg_pragma_openmp(parallel for cimg_openmp_if(is_multithreaded_rendering &&
                                                       primitives.size()>=cimg_openmp_sizefactor*4096))
        cimglist_for(primitives,l) {
          const CImg<tf>& primitive = primitives[l];
          switch (primitive.size()) {
          case 1 : { // Point
            const unsigned int
              i0 = (unsigned int)primitive(0);
            const float
              x0 = (float)vertices(i0,0), y0 = (float)vertices(i0,1), z0 = (float)vertices(i0,2);
            p_centers(l,0) = X + x0; p_centers(l,1) = Y + y0; p_centers(l,2) = Z + z0;
            p_normals(l,0) = p_normals(l,1) = p_normals(l,2) = 0;
          } break;
          case 2 : case 5 : { // Segment and sphere
            const unsigned int
              i0 = (unsigned int)primitive(0),
              i1 = (unsigned int)primitive(1);
            const float
              x0 = (float)vertices(i0,0), y0 = (float)vertices(i0,1), z0 = (float)vertices(i0,2),
              x1 = (float)vertices(i1,0), y1 = (float)vertices(i1,1), z1 = (float)vertices(i1,2);
            p_centers(l,0) = X + (x0 + x1)/2; p_centers(l,1) = Y + (y0 + y1)/2; p_centers(l,2) = Z + (z0 + z1)/2;
            p_normals(l,0) = p_normals(l,1) = p_normals(l,2) = 0;
          } break;
          case 3 : case 9 : { // Triangle
            const unsigned int
              i0 = (unsigned int)primitive(0),
              i1 = (unsigned int)primitive(1),
              i2 = (unsigned int)primitive(2);
            const float
              x0 = (float)vertices(i0,0), y0 = (float)vertices(i0,1), z0 = (float)vertices(i0,2),
              x1 = (float)vertices(i1,0), y1 = (float)vertices(i1,1), z1 = (float)vertices(i1,2),
              x2 = (float)vertices(i2,0), y2 = (float)vertices(i2,1), z2 = (float)vertices(i2,2),
              dx01 = x1 - x0, dy01 = y1 - y0, dz01 = z1 - z0,
              dx02 = x2 - x0, dy02 = y2 - y0, dz02 = z2 - z0,
              u = dy01*dz02 - dz01*dy02,
              v = dz01*dx02 - dx01*dz02,
              w = dx01*dy02 - dy01*dx02,
              nn = 1e-5f + cimg::hypot(u,v,w),
              nu = u/nn, nv = v/nn, nw = w/nn;
            p_centers(l,0) = X + (x0 + x1 + x2)/3;
            p_centers(l,1) = Y + (y0 + y1 + y2)/3;
            p_centers(l,2) = Z + (z0 + z1 + z2)/3;
            p_normals(l,0) = nu; p_normals(l,1) = nv; p_normals(l,2) = nw;
          } break;
          case 4 : case 12 : { // Quadrangle
            const unsigned int
              i0 = (unsigned int)primitive(0),
              i1 = (unsigned int)primitive(1),
              i2 = (unsigned int)primitive(2),
              i3 = (unsigned int)primitive(3);
            const float
              x0 = (float)vertices(i0,0), y0 = (float)vertices(i0,1), z0 = (float)vertices(i0,2),
              x1 = (float)vertices(i1,0), y1 = (float)vertices(i1,1), z1 = (float)vertices(i1,2),
              x2 = (float)vertices(i2,0), y2 = (float)vertices(i2,1), z2 = (float)vertices(i2,2),
              x3 = (float)vertices(i3,0), y3 = (float)vertices(i3,1), z3 = (float)vertices(i3,2),
              dx01 = x1 - x0, dy01 = y1 - y0, dz01 = z1 - z0,
              dx02 = x2 - x0, dy02 = y2 - y0, dz02 = z2 - z0,
              dx03 = x3 - x0, dy03 = y3 - y0, dz03 = z3 - z0,
              u0 = dy01*dz02 - dz01*dy02,
              v0 = dz01*dx02 - dx01*dz02,
              w0 = dx01*dy02 - dy01*dx02,
              nn0 = 1e-5f + cimg::hypot(u0,v0,w0),
              nu0 = u0/nn0, nv0 = v0/nn0, nw0 = w0/nn0,
              u1 = dy02*dz03 - dz02*dy03,
              v1 = dz02*dx03 - dx02*dz03,
              w1 = dx02*dy03 - dy02*dx03,
              nn1 = 1e-5f + cimg::hypot(u1,v1,w1),
              nu1 = u1/nn1, nv1 = v1/nn1, nw1 = w1/nn1,
              u = nu0 + nu1, v = nv0 + nv1, w = nw0 + nw1,
              nn = 1e-5f + cimg::hypot(u,v,w),
              nu = u/nn, nv = v/nn, nw = w/nn;
            p_centers(l,0) = X + (x0 + x1 + x2 + x3)/4;
            p_centers(l,1) = Y + (y0 + y1 + y2 + y3)/4;
            p_centers(l,2) = Z + (z0 + z1 + z2 + z3)/4;
            p_normals(l,0) = nu; p_normals(l,1) = nv; p_normals(l,2) = nw;
          } break;
          default : // Other types of primitives (should never happen...)
            p_centers(l,0) = p_centers(l,1) = p_centers(l,2) =
              p_normals(l,0) = p_normals(l,1) = p_normals(l,2) = 0;
          }
        }

        // 3D normals to vertices.
        if (render_type>=4) {
          v_normals.assign(vertices._width,3,1,1,0);
          cimglist_for(primitives,l) {
            const CImg<tf>& primitive = primitives[l];
            const float u = p_normals(l,0), v = p_normals(l,1), w = p_normals(l,2);
            switch (primitive.size()) {
            case 3 : case 9 : { // Triangle
              const unsigned int
                i0 = (unsigned int)primitive(0),
                i1 = (unsigned int)primitive(1),
                i2 = (unsigned int)primitive(2);
              v_normals(i0,0)+=u; v_normals(i0,1)+=v; v_normals(i0,2)+=w;
              v_normals(i1,0)+=u; v_normals(i1,1)+=v; v_normals(i1,2)+=w;
              v_normals(i2,0)+=u; v_normals(i2,1)+=v; v_normals(i2,2)+=w;
            } break;
            case 4 : case 12 : { // Quadrangle
              const unsigned int
                i0 = (unsigned int)primitive(0),
                i1 = (unsigned int)primitive(1),
                i2 = (unsigned int)primitive(2),
                i3 = (unsigned int)primitive(3);
              v_normals(i0,0)+=u; v_normals(i0,1)+=v; v_normals(i0,2)+=w;
              v_normals(i1,0)+=u; v_normals(i1,1)+=v; v_normals(i1,2)+=w;
              v_normals(i2,0)+=u; v_normals(i2,1)+=v; v_normals(i2,2)+=w;
              v_normals(i3,0)+=u; v_normals(i3,1)+=v; v_normals(i3,2)+=w;
            } break;
            }
          }
          cimg_pragma_openmp(parallel for cimg_openmp_if(is_multithreaded_rendering &&
                                                         v_normals.width()>=cimg_openmp_sizefactor*4096))
          cimg_forX(v_normals,l) {
            const float
              u = v_normals(l,0), v = v_normals(l,1), w = v_normals(l,2),
              nn = 1e-5f + cimg::hypot(u,v,w),
              nu = u/nn, nv = v/nn, nw = w/nn;
            v_normals(l,0) = nu; v_normals(l,1) = nv; v_normals(l,2) = nw;
          }
        }
      }

      // Compute 3D to 2D projection.
      CImg<floatT> projections(vertices._width,2);
      if (focale) {
        cimg_pragma_openmp(parallel for cimg_openmp_if(is_multithreaded_rendering &&
                                                       projections.size()>=cimg_openmp_sizefactor*4096))
        cimg_forX(projections,l) { // Perspective projection
          const float
            x = (float)vertices(l,0), y = (float)vertices(l,1), z = (float)vertices(l,2),
            z_denom = z + Z + _focale;
          projections(l,1) = Y + _focale*y/z_denom;
          projections(l,0) = X + _focale*x/z_denom;
        }
      } else {
        cimg_pragma_openmp(parallel for cimg_openmp_if(is_multithreaded_rendering &&
                                                       projections.size()>=cimg_openmp_sizefactor*4096))
        cimg_forX(projections,l) { // Parallel projection
          const float x = (float)vertices(l,0), y = (float)vertices(l,1);
          projections(l,1) = Y + y;
          projections(l,0) = X + x;
        }
      }

      float zmax = 0;
      if (zbuffer) zmax = vertices.get_shared_row(2).max();

      // Compute visible primitives.
      CImg<uintT> visibles(primitives._width,1,1,1,~0U);
      CImg<floatT> zrange(primitives._width);
      const float zmin = focale?1 - _focale:cimg::type<float>::min();
      bool is_forward = zbuffer?true:false;
      cimg_pragma_openmp(parallel for cimg_openmp_if(is_multithreaded_rendering &&
                                                     primitives.size()>=cimg_openmp_sizefactor*4096))
      cimglist_for(primitives,l) {
        const CImg<tf>& primitive = primitives[l];
        switch (primitive.size()) {
        case 1 : { // Point
          CImg<_to> _opacity;
          __draw_object3d(opacities,l,_opacity);
          if (l<=colors.width() && (colors[l].size()!=_spectrum || _opacity)) is_forward = false;
          const unsigned int i0 = (unsigned int)primitive(0);
          const float z0 = Z + (float)vertices(i0,2);
          if (z0>=zmin) {
            visibles(l) = (unsigned int)l;
            zrange(l) = z0;
          }
        } break;
        case 5 : { // Sphere
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          const float
            Xc = 0.5f*((float)vertices(i0,0) + (float)vertices(i1,0)),
            Yc = 0.5f*((float)vertices(i0,1) + (float)vertices(i1,1)),
            Zc = 0.5f*((float)vertices(i0,2) + (float)vertices(i1,2)),
            _zc = Z + Zc,
            zc = _zc + _focale,
            xc = X + Xc*(focale?_focale/zc:1),
            yc = Y + Yc*(focale?_focale/zc:1),
            radius = 0.5f*cimg::hypot(vertices(i1,0) - vertices(i0,0),
                                      vertices(i1,1) - vertices(i0,1),
                                      vertices(i1,2) - vertices(i0,2))*(focale?_focale/zc:1),
            xm = xc - radius,
            ym = yc - radius,
            xM = xc + radius,
            yM = yc + radius;
          if (xM>=0 && xm<_width && yM>=0 && ym<_height && _zc>=zmin) {
            visibles(l) = (unsigned int)l;
            zrange(l) = _zc;
          }
          is_forward = false;
        } break;
        case 2 : case 6 : { // Segment
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          const float
            x0 = projections(i0,0), y0 = projections(i0,1), z0 = Z + (float)vertices(i0,2),
            x1 = projections(i1,0), y1 = projections(i1,1), z1 = Z + (float)vertices(i1,2);
          float xm, xM, ym, yM, zm, zM;
          if (x0<x1) { xm = x0; xM = x1; } else { xm = x1; xM = x0; }
          if (y0<y1) { ym = y0; yM = y1; } else { ym = y1; yM = y0; }
          if (z0<z1) { zm = z0; zM = z1; } else { zm = z1; zM = z0; }
          if ((zm>=zmin && xM>=0 && xm<_width && yM>=0 && ym<_height) || (zm<zmin && zM>=zmin)) {
            visibles(l) = (unsigned int)l;
            zrange(l) = (z0 + z1)/2;
          }
        } break;
        case 3 : case 9 : { // Triangle
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2);
          const float
            x0 = projections(i0,0), y0 = projections(i0,1), z0 = Z + (float)vertices(i0,2),
            x1 = projections(i1,0), y1 = projections(i1,1), z1 = Z + (float)vertices(i1,2),
            x2 = projections(i2,0), y2 = projections(i2,1), z2 = Z + (float)vertices(i2,2);
          float dot = -1;
          if (p_normals) {
            if (focale) {
              const float
                cam2obj_x = p_centers(l,0) - X,
                cam2obj_y = p_centers(l,1) - Y,
                cam2obj_z = p_centers(l,2) + _focale;
              dot = cam2obj_x*p_normals(l,0) + cam2obj_y*p_normals(l,1) + cam2obj_z*p_normals(l,2);
            } else dot = p_normals(l,2);
          }
          float xm, xM, ym, yM, zm, zM;
          if (x0<x1) { xm = x0; xM = x1; } else { xm = x1; xM = x0; }
          if (x2<xm) xm = x2;
          if (x2>xM) xM = x2;
          if (y0<y1) { ym = y0; yM = y1; } else { ym = y1; yM = y0; }
          if (y2<ym) ym = y2;
          if (y2>yM) yM = y2;
          if (z0<z1) { zm = z0; zM = z1; } else { zm = z1; zM = z0; }
          if (z2<zm) zm = z2;
          if (z2>zM) zM = z2;
          if (((zm>=zmin && xM>=0 && xm<_width && yM>=0 && ym<_height) || (zm<zmin && zM>=zmin)) &&
              (is_double_sided || dot<0)) {
            visibles(l) = (unsigned int)l;
            zrange(l) = (z0 + z1 + z2)/3;
          }
        } break;
        case 4 : case 12 : { // Quadrangle
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2),
            i3 = (unsigned int)primitive(3);
          const float
            x0 = projections(i0,0), y0 = projections(i0,1), z0 = Z + (float)vertices(i0,2),
            x1 = projections(i1,0), y1 = projections(i1,1), z1 = Z + (float)vertices(i1,2),
            x2 = projections(i2,0), y2 = projections(i2,1), z2 = Z + (float)vertices(i2,2),
            x3 = projections(i3,0), y3 = projections(i3,1), z3 = Z + (float)vertices(i3,2);
          float dot = -1;
          if (p_normals) {
            if (focale) {
              const float
                cam2obj_x = p_centers(l,0) - X,
                cam2obj_y = p_centers(l,1) - Y,
                cam2obj_z = p_centers(l,2) + _focale;
              dot = cam2obj_x*p_normals(l,0) + cam2obj_y*p_normals(l,1) + cam2obj_z*p_normals(l,2);
            } else dot = p_normals(l,2);
          }
          float xm, xM, ym, yM, zm, zM;
          if (x0<x1) { xm = x0; xM = x1; } else { xm = x1; xM = x0; }
          if (x2<xm) xm = x2;
          if (x2>xM) xM = x2;
          if (x3<xm) xm = x3;
          if (x3>xM) xM = x3;
          if (y0<y1) { ym = y0; yM = y1; } else { ym = y1; yM = y0; }
          if (y2<ym) ym = y2;
          if (y2>yM) yM = y2;
          if (y3<ym) ym = y3;
          if (y3>yM) yM = y3;
          if (z0<z1) { zm = z0; zM = z1; } else { zm = z1; zM = z0; }
          if (z2<zm) zm = z2;
          if (z2>zM) zM = z2;
          if (z3<zm) zm = z3;
          if (z3>zM) zM = z3;
          if (((zm>=zmin && xM>=0 && xm<_width && yM>=0 && ym<_height) || (zm<zmin && zM>=zmin)) &&
              (is_double_sided || dot<0)) {
            visibles(l) = (unsigned int)l;
            zrange(l) = (z0 + z1 + z2 + z3)/4;
          }
        } break;
        default :
          if (render_type==5) cimg::mutex(10,0);
          throw CImgArgumentException(_cimg_instance
                                      "draw_object3d(): Invalid primitive[%u] with size %u "
                                      "(should have size 1,2,3,4,5,6,9 or 12).",
                                      cimg_instance,
                                      l,primitive.size());
        }
      }

      // Force transparent primitives to be drawn last when zbuffer is activated
      // (and if object contains no spheres or sprites).
      if (is_forward)
        cimglist_for(primitives,l)
          if (___draw_object3d(opacities,l)!=1) zrange(l) = 2*zmax - zrange(l);

      // Sort only visibles primitives.
      unsigned int *p_visibles = visibles._data;
      float *p_zrange = zrange._data;
      const float *ptrz = p_zrange;
      cimg_for(visibles,ptr,unsigned int) {
        if (*ptr!=~0U) { *(p_visibles++) = *ptr; *(p_zrange++) = *ptrz; }
        ++ptrz;
      }
      const unsigned int nb_visibles = (unsigned int)(p_zrange - zrange._data);
      if (!nb_visibles) {
        if (render_type==5) cimg::mutex(10,0);
        return *this;
      }
      CImg<uintT> permutations;
      CImg<floatT>(zrange._data,nb_visibles,1,1,1,true).sort(permutations,is_forward);

      // Compute light properties.
      CImg<floatT> lightprops;
      switch (render_type) {
      case 3 : { // Flat Shading
        lightprops.assign(nb_visibles);
        cimg_pragma_openmp(parallel for cimg_openmp_if(is_multithreaded_rendering &&
                                                       nb_visibles>=cimg_openmp_sizefactor*4096))
        cimg_forX(lightprops,l) {
          const unsigned int p = visibles(permutations(l));
          const CImg<tf>& primitive = primitives[p];
          const unsigned int psize = (unsigned int)primitive.size();
          if (psize==3 || psize==4 || psize==9 || psize==12) {
            const float
              x = (float)p_centers(p,0), y = (float)p_centers(p,1), z = (float)p_centers(p,2),
              u = (float)p_normals(p,0), v = (float)p_normals(p,1), w = (float)p_normals(p,2),
              lx = lightx - x, ly = lighty - y, lz = lightz - z,
              nn = 1e-5f + cimg::hypot(lx,ly,lz),
              nlx = lx/nn, nly = ly/nn, nlz = lz/nn,
              dot = nlx*u + nly*v + nlz*w,
              factor = std::max(0.15f,is_double_sided?cimg::abs(dot):std::max(0.0f,dot));
            lightprops[l] = factor<=nspec?factor:(nsl1*factor*factor + nsl2*factor + nsl3);
          } else lightprops[l] = 1;
        }
      } break;
      case 4 : // Gouraud Shading
      case 5 : { // Phong-Shading
        if (render_type==4) {
          lightprops.assign(vertices._width);
          cimg_pragma_openmp(parallel for cimg_openmp_if(is_multithreaded_rendering &&
                                                         nb_visibles>=cimg_openmp_sizefactor*4096))
          cimg_forX(lightprops,l) {
            const float
              x = vertices(l,0), y = vertices(l,1), z = vertices(l,2),
              u = v_normals(l,0), v = v_normals(l,1), w = v_normals(l,2),
              lx = lightx - x - X, ly = lighty - y - Y, lz = lightz - z - Z,
              nn = 1e-5f + cimg::hypot(lx,ly,lz),
              nlx = lx/nn, nly = ly/nn, nlz = lz/nn,
              dot = nlx*u + nly*v + nlz*w,
              factor = std::max(0.15f,is_double_sided?cimg::abs(dot):std::max(0.0f,dot));
            lightprops[l] = factor<=nspec?factor:(nsl1*factor*factor + nsl2*factor + nsl3);
          }
        } else {
          const unsigned int
            lw2 = light_texture._width/2 - 1,
            lh2 = light_texture._height/2 - 1;
          lightprops.assign(vertices._width,2);
          cimg_pragma_openmp(parallel for cimg_openmp_if(is_multithreaded_rendering &&
                                                         nb_visibles>=cimg_openmp_sizefactor*4096))
          cimg_forX(lightprops,l) {
            const float u = v_normals(l,0), v = v_normals(l,1);
            lightprops(l,0) = lw2*(1 + u);
            lightprops(l,1) = lh2*(1 + v);
          }
        }
      } break;
      }

      // Draw visible primitives.
      const CImg<tc> default_color(1,_spectrum,1,1,(tc)200);
      cimg_pragma_openmp(parallel for cimg_openmp_if(is_multithreaded_rendering &&
                                                     nb_visibles>=cimg_openmp_sizefactor*256))
      for (int l = 0; l<(int)nb_visibles; ++l) {
        CImg<_to> _opacity;
        const unsigned int n_primitive = visibles(permutations(l));
        const CImg<tf>& primitive = primitives[n_primitive];
        const CImg<tc>
          &__color = n_primitive<colors._width?colors[n_primitive]:CImg<tc>(),
          _color = (__color && __color.size()!=_spectrum && __color._spectrum<_spectrum)?
            __color.get_resize(-100,-100,-100,_spectrum,0):CImg<tc>(),
          &color = _color?_color:(__color?__color:default_color);
        const tc *const pcolor = color._data;
        float opacity = __draw_object3d(opacities,n_primitive,_opacity);
        if (_opacity.is_empty()) opacity*=g_opacity;

        switch (primitive.size()) {
        case 1 : { // Colored point or sprite
          const unsigned int n0 = (unsigned int)primitive[0];
          const int x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1));

          if (_opacity.is_empty()) { // Scalar opacity
            if (color.size()==_spectrum) // Colored point
              draw_point(x0,y0,pcolor,opacity);
            else { // Sprite
              const float
                z = Z + (float)vertices(n0,2),
                factor = focale<0?1:sprite_scale*(focale?_focale/(z + _focale):1);
              const unsigned int
                _sw = (unsigned int)(color._width*factor),
                _sh = (unsigned int)(color._height*factor),
                sw = _sw?_sw:1, sh = _sh?_sh:1;
              const int nx0 = x0 - (int)sw/2, ny0 = y0 - (int)sh/2;
              if (sw<=3*_width/2 && sh<=3*_height/2 &&
                  (nx0 + (int)sw/2>=0 || nx0 - (int)sw/2<width() || ny0 + (int)sh/2>=0 || ny0 - (int)sh/2<height())) {
                const CImg<tc>
                  _sprite = (sw!=color._width || sh!=color._height)?
                    color.get_resize(sw,sh,1,-100,render_type<=3?1:3):CImg<tc>(),
                  &sprite = _sprite?_sprite:color;
                draw_image(nx0,ny0,sprite,opacity);
              }
            }
          } else { // Opacity mask
            const float
              z = Z + (float)vertices(n0,2),
              factor = focale<0?1:sprite_scale*(focale?_focale/(z + _focale):1);
            const unsigned int
              _sw = (unsigned int)(std::max(color._width,_opacity._width)*factor),
              _sh = (unsigned int)(std::max(color._height,_opacity._height)*factor),
              sw = _sw?_sw:1, sh = _sh?_sh:1;
            const int nx0 = x0 - (int)sw/2, ny0 = y0 - (int)sh/2;
            if (sw<=3*_width/2 && sh<=3*_height/2 &&
                (nx0 + (int)sw/2>=0 || nx0 - (int)sw/2<width() || ny0 + (int)sh/2>=0 || ny0 - (int)sh/2<height())) {
              const CImg<tc>
                _sprite = (sw!=color._width || sh!=color._height)?
                  color.get_resize(sw,sh,1,-100,render_type<=3?1:3):CImg<tc>(),
                &sprite = _sprite?_sprite:color;
              const CImg<_to>
                _nopacity = (sw!=_opacity._width || sh!=_opacity._height)?
                  _opacity.get_resize(sw,sh,1,-100,render_type<=3?1:3):CImg<_to>(),
                &nopacity = _nopacity?_nopacity:_opacity;
              draw_image(nx0,ny0,sprite,nopacity,g_opacity);
            }
          }
        } break;
        case 2 : { // Colored segment
          unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1];
          if (render_type)
            _draw_object3d_flat_colored_segment(zbuffer,X,Y,Z,n0,n1,vertices,projections,pcolor,opacity,_focale);
          else {
            const int
              x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
              x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1));
            draw_point(x0,y0,pcolor,opacity).draw_point(x1,y1,pcolor,opacity);
          }
        } break;
        case 5 : { // Colored sphere
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            is_wireframe = (unsigned int)primitive[2],
            is_radius = (unsigned int)primitive[3];
          float Xc,Yc,Zc,radius;
          if (is_radius) {
            Xc = (float)vertices(n0,0);
            Yc = (float)vertices(n0,1);
            Zc = (float)vertices(n0,2);
            radius = cimg::hypot(vertices(n1,0) - vertices(n0,0),
                                 vertices(n1,1) - vertices(n0,1),
                                 vertices(n1,2) - vertices(n0,2));
          } else {
            Xc = 0.5f*((float)vertices(n0,0) + (float)vertices(n1,0));
            Yc = 0.5f*((float)vertices(n0,1) + (float)vertices(n1,1));
            Zc = 0.5f*((float)vertices(n0,2) + (float)vertices(n1,2));
            radius = 0.5f*cimg::hypot(vertices(n1,0) - vertices(n0,0),
                                      vertices(n1,1) - vertices(n0,1),
                                      vertices(n1,2) - vertices(n0,2));
          }
          const float
            zc = Z + Zc + _focale,
            af = focale?_focale/zc:1,
            xc = X + Xc*af,
            yc = Y + Yc*af;
          radius*=af;

          switch (render_type) {
          case 0 :
            draw_point((int)xc,(int)yc,pcolor,opacity);
            break;
          case 1 :
            draw_circle((int)xc,(int)yc,(int)radius,pcolor,opacity,~0U);
            break;
          default :
            if (is_wireframe) draw_circle((int)xc,(int)yc,(int)radius,pcolor,opacity,~0U);
            else draw_circle((int)xc,(int)yc,(int)radius,pcolor,opacity);
            break;
          }
        } break;
        case 6 : { // Textured segment
          if (!__color) {
            if (render_type==5) cimg::mutex(10,0);
            throw CImgArgumentException(_cimg_instance
                                        "draw_object3d(): Undefined texture for segment primitive [%u].",
                                        cimg_instance,n_primitive);
          }
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1];
          const int
            tx0 = (int)primitive[2], ty0 = (int)primitive[3],
            tx1 = (int)primitive[4], ty1 = (int)primitive[5];
          if (render_type)
            _draw_object3d_flat_textured_segment(zbuffer,X,Y,Z,n0,n1,vertices,projections,
                                                 color,tx0,ty0,tx1,ty1,opacity,_focale);
          else {
            const int
              x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
              x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1));
            draw_point(x0,y0,color.get_vector_at(tx0<=0?0:tx0>=color.width()?color.width() - 1:tx0,
                                                 ty0<=0?0:ty0>=color.height()?color.height() - 1:ty0)._data,opacity).
              draw_point(x1,y1,color.get_vector_at(tx1<=0?0:tx1>=color.width()?color.width() - 1:tx1,
                                                   ty1<=0?0:ty1>=color.height()?color.height() - 1:ty1)._data,opacity);
          }
        } break;
        case 3 : { // Colored triangle
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            n2 = (unsigned int)primitive[2];
          switch (render_type) {
          case 0 : {
            const int
              x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
              x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
              x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1));
            draw_point(x0,y0,pcolor,opacity).draw_point(x1,y1,pcolor,opacity).draw_point(x2,y2,pcolor,opacity);
          } break;
          case 1 :
            _draw_object3d_flat_colored_segment(zbuffer,X,Y,Z,n0,n1,vertices,projections,pcolor,opacity,_focale).
              _draw_object3d_flat_colored_segment(zbuffer,X,Y,Z,n1,n2,vertices,projections,pcolor,opacity,_focale).
              _draw_object3d_flat_colored_segment(zbuffer,X,Y,Z,n2,n0,vertices,projections,pcolor,opacity,_focale);
            break;
          case 2 :
            _draw_object3d_flat_colored_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,pcolor,opacity,1,_focale);
            break;
          case 3 :
            _draw_object3d_flat_colored_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,pcolor,opacity,
                                                 lightprops(l),_focale);
            break;
          case 4 :
            _draw_object3d_gouraud_colored_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                    pcolor,lightprops(n0),lightprops(n1),lightprops(n2),opacity,
                                                    _focale);
            break;
          case 5 : {
            const int
              lx0 = cimg::uiround(lightprops(n0,0)), ly0 = cimg::uiround(lightprops(n0,1)),
              lx1 = cimg::uiround(lightprops(n1,0)), ly1 = cimg::uiround(lightprops(n1,1)),
              lx2 = cimg::uiround(lightprops(n2,0)), ly2 = cimg::uiround(lightprops(n2,1));
            _draw_object3d_phong_colored_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                  pcolor,light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity,_focale);
          } break;
          }
        } break;
        case 4 : { // Colored quadrangle
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            n2 = (unsigned int)primitive[2],
            n3 = (unsigned int)primitive[3];
          switch (render_type) {
          case 0 : {
            const int
              x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
              x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
              x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1)),
              x3 = cimg::uiround(projections(n3,0)), y3 = cimg::uiround(projections(n3,1));
            draw_point(x0,y0,pcolor,opacity).draw_point(x1,y1,pcolor,opacity).
              draw_point(x2,y2,pcolor,opacity).draw_point(x3,y3,pcolor,opacity);
          } break;
          case 1 :
            _draw_object3d_flat_colored_segment(zbuffer,X,Y,Z,n0,n1,vertices,projections,pcolor,opacity,_focale).
              _draw_object3d_flat_colored_segment(zbuffer,X,Y,Z,n1,n2,vertices,projections,pcolor,opacity,_focale).
              _draw_object3d_flat_colored_segment(zbuffer,X,Y,Z,n2,n3,vertices,projections,pcolor,opacity,_focale).
              _draw_object3d_flat_colored_segment(zbuffer,X,Y,Z,n3,n0,vertices,projections,pcolor,opacity,_focale);
            break;
          case 2 :
            _draw_object3d_flat_colored_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,pcolor,opacity,1,_focale).
              _draw_object3d_flat_colored_triangle(zbuffer,X,Y,Z,n0,n2,n3,vertices,projections,pcolor,opacity,1,
                                                   _focale);
            break;
          case 3 :
            _draw_object3d_flat_colored_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,pcolor,opacity,
                                                 lightprops(l),_focale).
              _draw_object3d_flat_colored_triangle(zbuffer,X,Y,Z,n0,n2,n3,vertices,projections,pcolor,opacity,
                                                   lightprops(l),_focale);
            break;
          case 4 :
            _draw_object3d_gouraud_colored_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                    pcolor,lightprops(n0),lightprops(n1),lightprops(n2),opacity,
                                                    _focale).
              _draw_object3d_gouraud_colored_triangle(zbuffer,X,Y,Z,n0,n2,n3,vertices,projections,
                                                      pcolor,lightprops(n0),lightprops(n2),lightprops(n3),opacity,
                                                      _focale);
            break;
          case 5 : {
            const int
              lx0 = cimg::uiround(lightprops(n0,0)), ly0 = cimg::uiround(lightprops(n0,1)),
              lx1 = cimg::uiround(lightprops(n1,0)), ly1 = cimg::uiround(lightprops(n1,1)),
              lx2 = cimg::uiround(lightprops(n2,0)), ly2 = cimg::uiround(lightprops(n2,1)),
              lx3 = cimg::uiround(lightprops(n3,0)), ly3 = cimg::uiround(lightprops(n3,1));
            _draw_object3d_phong_colored_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                  pcolor,light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity,_focale).
              _draw_object3d_phong_colored_triangle(zbuffer,X,Y,Z,n0,n2,n3,vertices,projections,
                                                    pcolor,light_texture,lx0,ly0,lx2,ly2,lx3,ly3,opacity,_focale);
          } break;
          }
        } break;
        case 9 : { // Textured triangle
          if (!__color) {
            if (render_type==5) cimg::mutex(10,0);
            throw CImgArgumentException(_cimg_instance
                                        "draw_object3d(): Undefined texture for triangle primitive [%u].",
                                        cimg_instance,n_primitive);
          }
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            n2 = (unsigned int)primitive[2];
          const int
            tx0 = (int)primitive[3], ty0 = (int)primitive[4],
            tx1 = (int)primitive[5], ty1 = (int)primitive[6],
            tx2 = (int)primitive[7], ty2 = (int)primitive[8];
          switch (render_type) {
          case 0 : {
            const int
              x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
              x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
              x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1));
            draw_point(x0,y0,color.get_vector_at(tx0<=0?0:tx0>=color.width()?color.width() - 1:tx0,
                                                 ty0<=0?0:ty0>=color.height()?color.height() - 1:ty0)._data,opacity).
              draw_point(x1,y1,color.get_vector_at(tx1<=0?0:tx1>=color.width()?color.width() - 1:tx1,
                                                   ty1<=0?0:ty1>=color.height()?color.height() - 1:ty1)._data,opacity).
              draw_point(x2,y2,color.get_vector_at(tx2<=0?0:tx2>=color.width()?color.width() - 1:tx2,
                                                   ty2<=0?0:ty2>=color.height()?color.height() - 1:ty2)._data,opacity);
          } break;
          case 1 :
            _draw_object3d_flat_textured_segment(zbuffer,X,Y,Z,n0,n1,vertices,projections,
                                                 color,tx0,ty0,tx1,ty1,opacity,_focale).
              _draw_object3d_flat_textured_segment(zbuffer,X,Y,Z,n1,n2,vertices,projections,
                                                   color,tx1,ty1,tx2,ty2,opacity,_focale).
              _draw_object3d_flat_textured_segment(zbuffer,X,Y,Z,n2,n0,vertices,projections,
                                                   color,tx2,ty2,tx0,ty0,opacity,_focale);
            break;
          case 2 :
            _draw_object3d_flat_textured_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                  color,tx0,ty0,tx1,ty1,tx2,ty2,opacity,1,_focale);
            break;
          case 3 :
            _draw_object3d_flat_textured_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                  color,tx0,ty0,tx1,ty1,tx2,ty2,opacity,lightprops(l),_focale);
            break;
          case 4 :
            _draw_object3d_gouraud_textured_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                     color,tx0,ty0,tx1,ty1,tx2,ty2,
                                                     lightprops(n0),lightprops(n1),lightprops(n2),opacity,_focale);
            break;
          case 5 :
            const int
              lx0 = cimg::uiround(lightprops(n0,0)), ly0 = cimg::uiround(lightprops(n0,1)),
              lx1 = cimg::uiround(lightprops(n1,0)), ly1 = cimg::uiround(lightprops(n1,1)),
              lx2 = cimg::uiround(lightprops(n2,0)), ly2 = cimg::uiround(lightprops(n2,1));
            _draw_object3d_phong_textured_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                   color,tx0,ty0,tx1,ty1,tx2,ty2,
                                                   light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity,_focale);
            break;
          }
        } break;
        case 12 : { // Textured quadrangle
          if (!__color) {
            if (render_type==5) cimg::mutex(10,0);
            throw CImgArgumentException(_cimg_instance
                                        "draw_object3d(): Undefined texture for quadrangle primitive [%u].",
                                        cimg_instance,n_primitive);
          }
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            n2 = (unsigned int)primitive[2],
            n3 = (unsigned int)primitive[3];
          const int
            tx0 = (int)primitive[4], ty0 = (int)primitive[5],
            tx1 = (int)primitive[6], ty1 = (int)primitive[7],
            tx2 = (int)primitive[8], ty2 = (int)primitive[9],
            tx3 = (int)primitive[10], ty3 = (int)primitive[11];
          switch (render_type) {
          case 0 : {
            const int
              x0 = cimg::uiround(projections(n0,0)), y0 = cimg::uiround(projections(n0,1)),
              x1 = cimg::uiround(projections(n1,0)), y1 = cimg::uiround(projections(n1,1)),
              x2 = cimg::uiround(projections(n2,0)), y2 = cimg::uiround(projections(n2,1)),
              x3 = cimg::uiround(projections(n3,0)), y3 = cimg::uiround(projections(n3,1));
            draw_point(x0,y0,color.get_vector_at(tx0<=0?0:tx0>=color.width()?color.width() - 1:tx0,
                                                 ty0<=0?0:ty0>=color.height()?color.height() - 1:ty0)._data,opacity).
              draw_point(x1,y1,color.get_vector_at(tx1<=0?0:tx1>=color.width()?color.width() - 1:tx1,
                                                   ty1<=0?0:ty1>=color.height()?color.height() - 1:ty1)._data,opacity).
              draw_point(x2,y2,color.get_vector_at(tx2<=0?0:tx2>=color.width()?color.width() - 1:tx2,
                                                   ty2<=0?0:ty2>=color.height()?color.height() - 1:ty2)._data,opacity).
              draw_point(x3,y3,color.get_vector_at(tx3<=0?0:tx3>=color.width()?color.width() - 1:tx3,
                                                   ty3<=0?0:ty3>=color.height()?color.height() - 1:ty3)._data,opacity);
          } break;
          case 1 :
            _draw_object3d_flat_textured_segment(zbuffer,X,Y,Z,n0,n1,vertices,projections,
                                                 color,tx0,ty0,tx1,ty1,opacity,_focale).
              _draw_object3d_flat_textured_segment(zbuffer,X,Y,Z,n1,n2,vertices,projections,
                                                   color,tx1,ty1,tx2,ty2,opacity,_focale).
              _draw_object3d_flat_textured_segment(zbuffer,X,Y,Z,n2,n3,vertices,projections,
                                                   color,tx2,ty2,tx3,ty3,opacity,_focale).
              _draw_object3d_flat_textured_segment(zbuffer,X,Y,Z,n3,n0,vertices,projections,
                                                   color,tx3,ty3,tx0,ty0,opacity,_focale);
            break;
          case 2 :
            _draw_object3d_flat_textured_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                  color,tx0,ty0,tx1,ty1,tx2,ty2,opacity,1,_focale).
              _draw_object3d_flat_textured_triangle(zbuffer,X,Y,Z,n0,n2,n3,vertices,projections,
                                                    color,tx0,ty0,tx2,ty2,tx3,ty3,opacity,1,_focale);
            break;
          case 3 :
            _draw_object3d_flat_textured_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                  color,tx0,ty0,tx1,ty1,tx2,ty2,opacity,lightprops(l),_focale).
              _draw_object3d_flat_textured_triangle(zbuffer,X,Y,Z,n0,n2,n3,vertices,projections,
                                                    color,tx0,ty0,tx2,ty2,tx3,ty3,opacity,lightprops(l),_focale);
            break;
          case 4 :
            _draw_object3d_gouraud_textured_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                     color,tx0,ty0,tx1,ty1,tx2,ty2,
                                                     lightprops(n0),lightprops(n1),lightprops(n2),opacity,_focale).
              _draw_object3d_gouraud_textured_triangle(zbuffer,X,Y,Z,n0,n2,n3,vertices,projections,
                                                       color,tx0,ty0,tx2,ty2,tx3,ty3,
                                                       lightprops(n0),lightprops(n2),lightprops(n3),opacity,_focale);
            break;
          case 5 : {
            const int
              lx0 = cimg::uiround(lightprops(n0,0)), ly0 = cimg::uiround(lightprops(n0,1)),
              lx1 = cimg::uiround(lightprops(n1,0)), ly1 = cimg::uiround(lightprops(n1,1)),
              lx2 = cimg::uiround(lightprops(n2,0)), ly2 = cimg::uiround(lightprops(n2,1)),
              lx3 = cimg::uiround(lightprops(n3,0)), ly3 = cimg::uiround(lightprops(n3,1));
            _draw_object3d_phong_textured_triangle(zbuffer,X,Y,Z,n0,n1,n2,vertices,projections,
                                                   color,tx0,ty0,tx1,ty1,tx2,ty2,
                                                   light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity,_focale).
              _draw_object3d_phong_textured_triangle(zbuffer,X,Y,Z,n0,n2,n3,vertices,projections,
                                                     color,tx0,ty0,tx2,ty2,tx3,ty3,
                                                     light_texture,lx0,ly0,lx2,ly2,lx3,ly3,opacity,_focale);
          } break;
          }
        } break;
        }
      }
      if (render_type==5) cimg::mutex(10,0);
      return *this;
    }

    //@}
    //---------------------------
#endif
