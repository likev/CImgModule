#ifndef CIMG_MODULE_IMAGE_BODY_FILTERS_EXTRA_H
#define CIMG_MODULE_IMAGE_BODY_FILTERS_EXTRA_H

    //! Blur image.
    /**
       \param sigma_x Standard deviation of the blur, along the X-axis.
       \param sigma_y Standard deviation of the blur, along the Y-axis.
       \param sigma_z Standard deviation of the blur, along the Z-axis.
       \param boundary_conditions Boundary conditions.
         Can be <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
       \param is_gaussian Tells if the blur uses a gaussian (\c true) or quasi-gaussian (\c false) kernel.
       \note
       - The blur is computed as a 0-order Vanvliet (gaussian) or Deriche filter (quasi-gaussian).
       - This is a recursive algorithm, not depending on the values of the standard deviations.
       \see deriche(), vanvliet().
    **/
    CImg<T>& blur(const float sigma_x, const float sigma_y, const float sigma_z,
                  const unsigned int boundary_conditions=1, const bool is_gaussian=true) {
      if (is_empty()) return *this;
      if (is_gaussian) {
        if (_width>1) vanvliet(sigma_x,0,'x',boundary_conditions);
        if (_height>1) vanvliet(sigma_y,0,'y',boundary_conditions);
        if (_depth>1) vanvliet(sigma_z,0,'z',boundary_conditions);
      } else {
        if (_width>1) deriche(sigma_x,0,'x',boundary_conditions);
        if (_height>1) deriche(sigma_y,0,'y',boundary_conditions);
        if (_depth>1) deriche(sigma_z,0,'z',boundary_conditions);
      }
      return *this;
    }

    //! Blur image \newinstance.
    CImg<Tfloat> get_blur(const float sigma_x, const float sigma_y, const float sigma_z,
                          const unsigned int boundary_conditions=1, const bool is_gaussian=true) const {
      return CImg<Tfloat>(*this,false).blur(sigma_x,sigma_y,sigma_z,boundary_conditions,is_gaussian);
    }

    //! Blur image isotropically.
    /**
       \param sigma Standard deviation of the blur.
       \param boundary_conditions Boundary conditions.
         Can be <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.a
       \param is_gaussian Use a gaussian kernel (VanVliet) is set, a quasi-gaussian (Deriche) otherwise.
       \see deriche(), vanvliet().
    **/
    CImg<T>& blur(const float sigma, const unsigned int boundary_conditions=1, const bool is_gaussian=true) {
      const float nsigma = sigma>=0?sigma:-sigma*cimg::max(_width,_height,_depth)/100;
      return blur(nsigma,nsigma,nsigma,boundary_conditions,is_gaussian);
    }

    //! Blur image isotropically \newinstance.
    CImg<Tfloat> get_blur(const float sigma, const unsigned int boundary_conditions=1,
                          const bool is_gaussian=true) const {
      return CImg<Tfloat>(*this,false).blur(sigma,boundary_conditions,is_gaussian);
    }

    //! Blur image anisotropically, directed by a field of diffusion tensors.
    /**
       \param G Field of square roots of diffusion tensors/vectors used to drive the smoothing.
       \param amplitude Amplitude of the smoothing.
       \param dl Spatial discretization.
       \param da Angular discretization.
       \param gauss_prec Precision of the diffusion process.
       \param interpolation_type Interpolation scheme.
         Can be <tt>{ 0=nearest-neighbor | 1=linear | 2=Runge-Kutta }</tt>.
       \param is_fast_approx Tells if a fast approximation of the gaussian function is used or not.
    **/
    template<typename t>
    CImg<T>& blur_anisotropic(const CImg<t>& G,
                              const float amplitude=60, const float dl=0.8f, const float da=30,
                              const float gauss_prec=2, const unsigned int interpolation_type=0,
                              const bool is_fast_approx=1) {

      // Check arguments and init variables.
      if (!is_sameXYZ(G) || (G._spectrum!=3 && G._spectrum!=6))
        throw CImgArgumentException(_cimg_instance
                                    "blur_anisotropic(): Invalid specified diffusion tensor field (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    G._width,G._height,G._depth,G._spectrum,G._data);
      if (is_empty() || dl<0) return *this;
      const float namplitude = amplitude>=0?amplitude:-amplitude*cimg::max(_width,_height,_depth)/100;
      unsigned int iamplitude = cimg::round(namplitude);
      const bool is_3d = (G._spectrum==6);
      T val_min, val_max = max_min(val_min);
      _cimg_abort_init_openmp;
      cimg_abort_init;

      if (da<=0) { // Iterated oriented Laplacians
        CImg<Tfloat> velocity(_width,_height,_depth,_spectrum);
        for (unsigned int iteration = 0; iteration<iamplitude; ++iteration) {
          Tfloat *ptrd = velocity._data, veloc_max = 0;
          if (is_3d) // 3D version
            cimg_forC(*this,c) {
              cimg_abort_test;
              CImg_3x3x3(I,Tfloat);
              cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
                const Tfloat
                  ixx = Incc + Ipcc - 2*Iccc,
                  ixy = (Innc + Ippc - Inpc - Ipnc)/4,
                  ixz = (Incn + Ipcp - Incp - Ipcn)/4,
                  iyy = Icnc + Icpc - 2*Iccc,
                  iyz = (Icnn + Icpp - Icnp - Icpn)/4,
                  izz = Iccn + Iccp - 2*Iccc,
                  veloc = (Tfloat)(G(x,y,z,0)*ixx + 2*G(x,y,z,1)*ixy + 2*G(x,y,z,2)*ixz +
                                   G(x,y,z,3)*iyy + 2*G(x,y,z,4)*iyz + G(x,y,z,5)*izz);
                *(ptrd++) = veloc;
                if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
              }
            }
          else // 2D version
            cimg_forZC(*this,z,c) {
              cimg_abort_test;
              CImg_3x3(I,Tfloat);
              cimg_for3x3(*this,x,y,z,c,I,Tfloat) {
                const Tfloat
                  ixx = Inc + Ipc - 2*Icc,
                  ixy = (Inn + Ipp - Inp - Ipn)/4,
                  iyy = Icn + Icp - 2*Icc,
                  veloc = (Tfloat)(G(x,y,0,0)*ixx + 2*G(x,y,0,1)*ixy + G(x,y,0,2)*iyy);
                *(ptrd++) = veloc;
                if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
              }
            }
          if (veloc_max>0) *this+=(velocity*=dl/veloc_max);
        }
      } else { // LIC-based smoothing
        const ulongT whd = (ulongT)_width*_height*_depth;
        const float sqrt2amplitude = (float)std::sqrt(2*namplitude);
        const int dx1 = width() - 1, dy1 = height() - 1, dz1 = depth() - 1;
        CImg<Tfloat> res(_width,_height,_depth,_spectrum,0), W(_width,_height,_depth,is_3d?4:3), val(_spectrum,1,1,1,0);
        int N = 0;
        if (is_3d) { // 3D version
          for (float phi = cimg::mod(180.f,da)/2.f; phi<=180; phi+=da) {
            const float phir = (float)(phi*cimg::PI/180), datmp = (float)(da/std::cos(phir)),
              da2 = datmp<1?360.f:datmp;
            for (float theta = 0; theta<360; (theta+=da2),++N) {
              const float
                thetar = (float)(theta*cimg::PI/180),
                vx = (float)(std::cos(thetar)*std::cos(phir)),
                vy = (float)(std::sin(thetar)*std::cos(phir)),
                vz = (float)std::sin(phir);
              const t
                *pa = G.data(0,0,0,0), *pb = G.data(0,0,0,1), *pc = G.data(0,0,0,2),
                *pd = G.data(0,0,0,3), *pe = G.data(0,0,0,4), *pf = G.data(0,0,0,5);
              Tfloat *pd0 = W.data(0,0,0,0), *pd1 = W.data(0,0,0,1), *pd2 = W.data(0,0,0,2), *pd3 = W.data(0,0,0,3);
              cimg_forXYZ(G,xg,yg,zg) {
                const t a = *(pa++), b = *(pb++), c = *(pc++), d = *(pd++), e = *(pe++), f = *(pf++);
                const float
                  u = (float)(a*vx + b*vy + c*vz),
                  v = (float)(b*vx + d*vy + e*vz),
                  w = (float)(c*vx + e*vy + f*vz),
                  n = 1e-5f + cimg::hypot(u,v,w),
                  dln = dl/n;
                *(pd0++) = (Tfloat)(u*dln);
                *(pd1++) = (Tfloat)(v*dln);
                *(pd2++) = (Tfloat)(w*dln);
                *(pd3++) = (Tfloat)n;
              }

              cimg_abort_test;
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                                 cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 && _height*_depth>=2)
                                 firstprivate(val))
              cimg_forYZ(*this,y,z) _cimg_abort_try_openmp2 {
                cimg_abort_test2;
                cimg_forX(*this,x) {
                  val.fill(0);
                  const float
                    n = (float)W(x,y,z,3),
                    fsigma = (float)(n*sqrt2amplitude),
                    fsigma2 = 2*fsigma*fsigma,
                    length = gauss_prec*fsigma;
                  float
                    S = 0,
                    X = (float)x,
                    Y = (float)y,
                    Z = (float)z;
                  switch (interpolation_type) {
                  case 0 : { // Nearest neighbor
                    for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1 && Z>=0 && Z<=dz1; l+=dl) {
                      const int
                        cx = (int)(X + 0.5f),
                        cy = (int)(Y + 0.5f),
                        cz = (int)(Z + 0.5f);
                      const float
                        u = (float)W(cx,cy,cz,0),
                        v = (float)W(cx,cy,cz,1),
                        w = (float)W(cx,cy,cz,2);
                      if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)(*this)(cx,cy,cz,c); ++S; }
                      else {
                        const float coef = (float)std::exp(-l*l/fsigma2);
                        cimg_forC(*this,c) val[c]+=(Tfloat)(coef*(*this)(cx,cy,cz,c));
                        S+=coef;
                      }
                      X+=u; Y+=v; Z+=w;
                    }
                  } break;
                  case 1 : { // Linear interpolation
                    for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1 && Z>=0 && Z<=dz1; l+=dl) {
                      const float
                        u = (float)(W._linear_atXYZ(X,Y,Z,0)),
                        v = (float)(W._linear_atXYZ(X,Y,Z,1)),
                        w = (float)(W._linear_atXYZ(X,Y,Z,2));
                      if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)_linear_atXYZ(X,Y,Z,c); ++S; }
                      else {
                        const float coef = (float)std::exp(-l*l/fsigma2);
                        cimg_forC(*this,c) val[c]+=(Tfloat)(coef*_linear_atXYZ(X,Y,Z,c));
                        S+=coef;
                      }
                      X+=u; Y+=v; Z+=w;
                    }
                  } break;
                  default : { // 2nd order Runge Kutta
                    for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1 && Z>=0 && Z<=dz1; l+=dl) {
                      const float
                        u0 = (float)(0.5f*W._linear_atXYZ(X,Y,Z,0)),
                        v0 = (float)(0.5f*W._linear_atXYZ(X,Y,Z,1)),
                        w0 = (float)(0.5f*W._linear_atXYZ(X,Y,Z,2)),
                        u = (float)(W._linear_atXYZ(X + u0,Y + v0,Z + w0,0)),
                        v = (float)(W._linear_atXYZ(X + u0,Y + v0,Z + w0,1)),
                        w = (float)(W._linear_atXYZ(X + u0,Y + v0,Z + w0,2));
                      if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)_linear_atXYZ(X,Y,Z,c); ++S; }
                      else {
                        const float coef = (float)std::exp(-l*l/fsigma2);
                        cimg_forC(*this,c) val[c]+=(Tfloat)(coef*_linear_atXYZ(X,Y,Z,c));
                        S+=coef;
                      }
                      X+=u; Y+=v; Z+=w;
                    }
                  } break;
                  }
                  Tfloat *ptrd = res.data(x,y,z);
                  if (S>0) cimg_forC(res,c) { *ptrd+=val[c]/S; ptrd+=whd; }
                  else cimg_forC(res,c) { *ptrd+=(Tfloat)((*this)(x,y,z,c)); ptrd+=whd; }
                }
              } _cimg_abort_catch_openmp2
            }
          }
        } else { // 2D LIC algorithm
          for (float theta = cimg::mod(360.f,da)/2.f; theta<360; (theta+=da),++N) {
            const float thetar = (float)(theta*cimg::PI/180),
              vx = (float)(std::cos(thetar)), vy = (float)(std::sin(thetar));
            const t *pa = G.data(0,0,0,0), *pb = G.data(0,0,0,1), *pc = G.data(0,0,0,2);
            Tfloat *pd0 = W.data(0,0,0,0), *pd1 = W.data(0,0,0,1), *pd2 = W.data(0,0,0,2);
            cimg_forXY(G,xg,yg) {
              const t a = *(pa++), b = *(pb++), c = *(pc++);
              const float
                u = (float)(a*vx + b*vy),
                v = (float)(b*vx + c*vy),
                n = std::max(1e-5f,cimg::hypot(u,v)),
                dln = dl/n;
              *(pd0++) = (Tfloat)(u*dln);
              *(pd1++) = (Tfloat)(v*dln);
              *(pd2++) = (Tfloat)n;
            }

            cimg_abort_test;
            cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 && _height>=2)
                               firstprivate(val))
            cimg_forY(*this,y) _cimg_abort_try_openmp2 {
              cimg_abort_test2;
              cimg_forX(*this,x) {
                val.fill(0);
                const float
                  n = (float)W(x,y,0,2),
                  fsigma = (float)(n*sqrt2amplitude),
                  fsigma2 = 2*fsigma*fsigma,
                  length = gauss_prec*fsigma;
                float
                  S = 0,
                  X = (float)x,
                  Y = (float)y;
                switch (interpolation_type) {
                case 0 : { // Nearest-neighbor
                  for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1; l+=dl) {
                    const int
                      cx = (int)(X + 0.5f),
                      cy = (int)(Y + 0.5f);
                    const float
                      u = (float)W(cx,cy,0,0),
                      v = (float)W(cx,cy,0,1);
                    if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)(*this)(cx,cy,0,c); ++S; }
                    else {
                      const float coef = (float)std::exp(-l*l/fsigma2);
                      cimg_forC(*this,c) val[c]+=(Tfloat)(coef*(*this)(cx,cy,0,c));
                      S+=coef;
                    }
                    X+=u; Y+=v;
                  }
                } break;
                case 1 : { // Linear interpolation
                  for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1; l+=dl) {
                    const float
                      u = (float)(W._linear_atXY(X,Y,0,0)),
                      v = (float)(W._linear_atXY(X,Y,0,1));
                    if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)_linear_atXY(X,Y,0,c); ++S; }
                    else {
                      const float coef = (float)std::exp(-l*l/fsigma2);
                      cimg_forC(*this,c) val[c]+=(Tfloat)(coef*_linear_atXY(X,Y,0,c));
                      S+=coef;
                    }
                    X+=u; Y+=v;
                  }
                } break;
                default : { // 2nd-order Runge-kutta interpolation
                  for (float l = 0; l<length && X>=0 && X<=dx1 && Y>=0 && Y<=dy1; l+=dl) {
                    const float
                      u0 = (float)(0.5f*W._linear_atXY(X,Y,0,0)),
                      v0 = (float)(0.5f*W._linear_atXY(X,Y,0,1)),
                      u = (float)(W._linear_atXY(X + u0,Y + v0,0,0)),
                      v = (float)(W._linear_atXY(X + u0,Y + v0,0,1));
                    if (is_fast_approx) { cimg_forC(*this,c) val[c]+=(Tfloat)_linear_atXY(X,Y,0,c); ++S; }
                    else {
                      const float coef = (float)std::exp(-l*l/fsigma2);
                      cimg_forC(*this,c) val[c]+=(Tfloat)(coef*_linear_atXY(X,Y,0,c));
                      S+=coef;
                    }
                    X+=u; Y+=v;
                  }
                }
                }
                Tfloat *ptrd = res.data(x,y);
                if (S>0) cimg_forC(res,c) { *ptrd+=val[c]/S; ptrd+=whd; }
                else cimg_forC(res,c) { *ptrd+=(Tfloat)((*this)(x,y,0,c)); ptrd+=whd; }
              }
            } _cimg_abort_catch_openmp2
          }
        }
        const Tfloat *ptrs = res._data;
        cimg_for(*this,ptrd,T) {
          const Tfloat _val = *(ptrs++)/N;
          *ptrd = _val<val_min?val_min:(_val>val_max?val_max:(T)_val);
        }
      }
      cimg_abort_test;
      return *this;
    }

    //! Blur image anisotropically, directed by a field of diffusion tensors \newinstance.
    template<typename t>
    CImg<Tfloat> get_blur_anisotropic(const CImg<t>& G,
                                      const float amplitude=60, const float dl=0.8f, const float da=30,
                                      const float gauss_prec=2, const unsigned int interpolation_type=0,
                                      const bool is_fast_approx=true) const {
      return CImg<Tfloat>(*this,false).blur_anisotropic(G,amplitude,dl,da,gauss_prec,interpolation_type,is_fast_approx);
    }

    //! Blur image anisotropically, in an edge-preserving way.
    /**
       \param amplitude Amplitude of the smoothing.
       \param sharpness Sharpness.
       \param anisotropy Anisotropy.
       \param alpha Standard deviation of the gradient blur.
       \param sigma Standard deviation of the structure tensor blur.
       \param dl Spatial discretization.
       \param da Angular discretization.
       \param gauss_prec Precision of the diffusion process.
       \param interpolation_type Interpolation scheme.
         Can be <tt>{ 0=nearest-neighbor | 1=linear | 2=Runge-Kutta }</tt>.
       \param is_fast_approx Tells if a fast approximation of the gaussian function is used or not.
     **/
    CImg<T>& blur_anisotropic(const float amplitude, const float sharpness=0.7f, const float anisotropy=0.6f,
                              const float alpha=0.6f, const float sigma=1.1f, const float dl=0.8f, const float da=30,
                              const float gauss_prec=2, const unsigned int interpolation_type=0,
                              const bool is_fast_approx=true) {
      const float nalpha = alpha>=0?alpha:-alpha*cimg::max(_width,_height,_depth)/100;
      const float nsigma = sigma>=0?sigma:-sigma*cimg::max(_width,_height,_depth)/100;
      return blur_anisotropic(get_diffusion_tensors(sharpness,anisotropy,nalpha,nsigma,interpolation_type!=3),
                              amplitude,dl,da,gauss_prec,interpolation_type,is_fast_approx);
    }

    //! Blur image anisotropically, in an edge-preserving way \newinstance.
    CImg<Tfloat> get_blur_anisotropic(const float amplitude, const float sharpness=0.7f, const float anisotropy=0.6f,
                                      const float alpha=0.6f, const float sigma=1.1f, const float dl=0.8f,
                                      const float da=30, const float gauss_prec=2,
                                      const unsigned int interpolation_type=0,
                                      const bool is_fast_approx=true) const {
      return CImg<Tfloat>(*this,false).blur_anisotropic(amplitude,sharpness,anisotropy,alpha,sigma,dl,da,gauss_prec,
                                                        interpolation_type,is_fast_approx);
    }

    //! Blur image, with the joint bilateral filter.
    /**
       \param guide Image used to model the smoothing weights.
       \param sigma_x Amount of blur along the X-axis.
       \param sigma_y Amount of blur along the Y-axis.
       \param sigma_z Amount of blur along the Z-axis.
       \param sigma_r Amount of blur along the value axis.
       \param sampling_x Amount of downsampling along the X-axis used for the approximation.
         Defaults (0) to sigma_x.
       \param sampling_y Amount of downsampling along the Y-axis used for the approximation.
         Defaults (0) to sigma_y.
       \param sampling_z Amount of downsampling along the Z-axis used for the approximation.
         Defaults (0) to sigma_z.
       \param sampling_r Amount of downsampling along the value axis used for the approximation.
         Defaults (0) to sigma_r.
       \note This algorithm uses the optimisation technique proposed by S. Paris and F. Durand, in ECCV'2006
       (extended for 3D volumetric images).
       It is based on the reference implementation http://people.csail.mit.edu/jiawen/software/bilateralFilter.m
    **/
    template<typename t>
    CImg<T>& blur_bilateral(const CImg<t>& guide,
                            const float sigma_x, const float sigma_y,
                            const float sigma_z, const float sigma_r,
                            const float sampling_x, const float sampling_y,
                            const float sampling_z, const float sampling_r) {
      if (!is_sameXYZ(guide))
        throw CImgArgumentException(_cimg_instance
                                    "blur_bilateral(): Invalid size for specified guide image (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data);
      if (is_empty() || (!sigma_x && !sigma_y && !sigma_z)) return *this;
      T edge_min, edge_max = guide.max_min(edge_min);
      if (edge_min==edge_max) return blur(sigma_x,sigma_y,sigma_z);
      const float
        edge_delta = (float)(edge_max - edge_min),
        _sigma_x = sigma_x>=0?sigma_x:-sigma_x*_width/100,
        _sigma_y = sigma_y>=0?sigma_y:-sigma_y*_height/100,
        _sigma_z = sigma_z>=0?sigma_z:-sigma_z*_depth/100,
        _sigma_r = sigma_r>=0?sigma_r:-sigma_r*edge_delta/100,
        _sampling_x = sampling_x?sampling_x:std::max(_sigma_x,1.f),
        _sampling_y = sampling_y?sampling_y:std::max(_sigma_y,1.f),
        _sampling_z = sampling_z?sampling_z:std::max(_sigma_z,1.f),
        _sampling_r = sampling_r?sampling_r:std::max(_sigma_r,edge_delta/256),
        derived_sigma_x = _sigma_x / _sampling_x,
        derived_sigma_y = _sigma_y / _sampling_y,
        derived_sigma_z = _sigma_z / _sampling_z,
        derived_sigma_r = _sigma_r / _sampling_r;
      const int
        padding_x = (int)(2*derived_sigma_x) + 1,
        padding_y = (int)(2*derived_sigma_y) + 1,
        padding_z = (int)(2*derived_sigma_z) + 1,
        padding_r = (int)(2*derived_sigma_r) + 1;
      const unsigned int
        bx = (unsigned int)((_width  - 1)/_sampling_x + 1 + 2*padding_x),
        by = (unsigned int)((_height - 1)/_sampling_y + 1 + 2*padding_y),
        bz = (unsigned int)((_depth  - 1)/_sampling_z + 1 + 2*padding_z),
        br = (unsigned int)(edge_delta/_sampling_r + 1 + 2*padding_r);
      if (bx>0 || by>0 || bz>0 || br>0) {
        const bool is_3d = (_depth>1);
        if (is_3d) { // 3D version of the algorithm
          CImg<floatT> bgrid(bx,by,bz,br), bgridw(bx,by,bz,br);
          cimg_forC(*this,c) {
            const CImg<t> _guide = guide.get_shared_channel(c%guide._spectrum);
            bgrid.fill(0); bgridw.fill(0);
            cimg_forXYZ(*this,x,y,z) {
              const T val = (*this)(x,y,z,c);
              const float edge = (float)_guide(x,y,z);
              const int
                X = (int)cimg::round(x/_sampling_x) + padding_x,
                Y = (int)cimg::round(y/_sampling_y) + padding_y,
                Z = (int)cimg::round(z/_sampling_z) + padding_z,
                R = (int)cimg::round((edge - edge_min)/_sampling_r) + padding_r;
              bgrid(X,Y,Z,R)+=(float)val;
              bgridw(X,Y,Z,R)+=1;
            }
            bgrid.blur(derived_sigma_x,derived_sigma_y,derived_sigma_z,true).deriche(derived_sigma_r,0,'c',false);
            bgridw.blur(derived_sigma_x,derived_sigma_y,derived_sigma_z,true).deriche(derived_sigma_r,0,'c',false);

            cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(size(),4096))
            cimg_forXYZ(*this,x,y,z) {
              const float edge = (float)_guide(x,y,z);
              const float
                X = x/_sampling_x + padding_x,
                Y = y/_sampling_y + padding_y,
                Z = z/_sampling_z + padding_z,
                R = (edge - edge_min)/_sampling_r + padding_r;
              const float bval0 = bgrid._linear_atXYZC(X,Y,Z,R), bval1 = bgridw._linear_atXYZC(X,Y,Z,R);
              (*this)(x,y,z,c) = (T)(bval0/bval1);
            }
          }
        } else { // 2D version of the algorithm
          CImg<floatT> bgrid(bx,by,br,2);
          cimg_forC(*this,c) {
            const CImg<t> _guide = guide.get_shared_channel(c%guide._spectrum);
            bgrid.fill(0);
            cimg_forXY(*this,x,y) {
              const T val = (*this)(x,y,c);
              const float edge = (float)_guide(x,y);
              const int
                X = (int)cimg::round(x/_sampling_x) + padding_x,
                Y = (int)cimg::round(y/_sampling_y) + padding_y,
                R = (int)cimg::round((edge - edge_min)/_sampling_r) + padding_r;
              bgrid(X,Y,R,0)+=(float)val;
              bgrid(X,Y,R,1)+=1;
            }
            bgrid.blur(derived_sigma_x,derived_sigma_y,0,true).blur(0,0,derived_sigma_r,false);

            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(size(),4096))
            cimg_forXY(*this,x,y) {
              const float edge = (float)_guide(x,y);
              const float
                X = x/_sampling_x + padding_x,
                Y = y/_sampling_y + padding_y,
                R = (edge - edge_min)/_sampling_r + padding_r;
              const float bval0 = bgrid._linear_atXYZ(X,Y,R,0), bval1 = bgrid._linear_atXYZ(X,Y,R,1);
              (*this)(x,y,c) = (T)(bval0/bval1);
            }
          }
        }
      }
      return *this;
    }

    //! Blur image, with the joint bilateral filter \newinstance.
    template<typename t>
    CImg<Tfloat> get_blur_bilateral(const CImg<t>& guide,
                                    const float sigma_x, const float sigma_y,
                                    const float sigma_z, const float sigma_r,
                                    const float sampling_x, const float sampling_y,
                                    const float sampling_z, const float sampling_r) const {
      return CImg<Tfloat>(*this,false).blur_bilateral(guide,sigma_x,sigma_y,sigma_z,sigma_r,
                                                      sampling_x,sampling_y,sampling_z,sampling_r);
    }

    //! Blur image using the joint bilateral filter.
    /**
       \param guide Image used to model the smoothing weights.
       \param sigma_s Amount of blur along the XYZ-axes.
       \param sigma_r Amount of blur along the value axis.
       \param sampling_s Amount of downsampling along the XYZ-axes used for the approximation. Defaults to sigma_s.
       \param sampling_r Amount of downsampling along the value axis used for the approximation. Defaults to sigma_r.
    **/
    template<typename t>
    CImg<T>& blur_bilateral(const CImg<t>& guide,
                            const float sigma_s, const float sigma_r,
                            const float sampling_s=0, const float sampling_r=0) {
      const float _sigma_s = sigma_s>=0?sigma_s:-sigma_s*cimg::max(_width,_height,_depth)/100;
      return blur_bilateral(guide,_sigma_s,_sigma_s,_sigma_s,sigma_r,sampling_s,sampling_s,sampling_s,sampling_r);
    }

    //! Blur image using the bilateral filter \newinstance.
    template<typename t>
    CImg<Tfloat> get_blur_bilateral(const CImg<t>& guide,
                                    const float sigma_s, const float sigma_r,
                                    const float sampling_s=0, const float sampling_r=0) const {
      return CImg<Tfloat>(*this,false).blur_bilateral(guide,sigma_s,sigma_r,sampling_s,sampling_r);
    }

    // [internal] Apply a box filter (used by CImg<T>::boxfilter() and CImg<T>::blur_box()).
    /*
      \param ptr the pointer of the data
      \param N size of the data
      \param boxsize Size of the box filter (can be subpixel).
      \param off the offset between two data point
      \param order the order of the filter 0 (smoothing), 1st derivative and 2nd derivative.
      \param boundary_conditions Boundary conditions.
        Can be <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
    */
    static void _cimg_blur_box_apply(T *ptr, const float boxsize, const int N, const ulongT off,
                                     const int order, const unsigned int boundary_conditions,
                                     const unsigned int nb_iter) {
      const int nboundary_conditions = boundary_conditions>1 && boxsize<=3?1:boundary_conditions;

      // Smooth.
      if (boxsize>1 && nb_iter) {
        const int w2 = (int)(boxsize - 1)/2;
        const unsigned int winsize = 2*w2 + 1U;
        const double frac = (boxsize - winsize)/2.;
        CImg<T> win(winsize);
        for (unsigned int iter = 0; iter<nb_iter; ++iter) {
          Tdouble sum = 0; // window sum
          for (int x = -w2; x<=w2; ++x) {
            win[x + w2] = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,x);
            sum+=win[x + w2];
          }
          int ifirst = 0, ilast = 2*w2;
          T
            prev = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,-w2 - 1),
            next = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,w2 + 1);
          for (int x = 0; x < N - 1; ++x) {
            const double sum2 = sum + frac * (prev + next);
            ptr[x*off] = (T)(sum2/boxsize);
            prev = win[ifirst];
            sum-=prev;
            ifirst = (int)((ifirst + 1)%winsize);
            ilast = (int)((ilast + 1)%winsize);
            win[ilast] = next;
            sum+=next;
            next = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,x + w2 + 2);
          }
          const double sum2 = sum + frac * (prev + next);
          ptr[(N - 1)*off] = (T)(sum2/boxsize);
        }
      }

      // Derive.
      switch (order) {
      case 0 :
        break;
      case 1 : {
        Tfloat
          p = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,-1),
          c = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,0),
          n = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,1);
        for (int x = 0; x<N - 1; ++x) {
          ptr[x*off] = (T)((n-p)/2.);
          p = c;
          c = n;
          n = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,x + 2);
        }
        ptr[(N - 1)*off] = (T)((n-p)/2.);
      } break;
      case 2: {
        Tfloat
          p = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,-1),
          c = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,0),
          n = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,1);
        for (int x = 0; x<N - 1; ++x) {
          ptr[x*off] = (T)(n - 2*c + p);
          p = c;
          c = n;
          n = __cimg_blur_box_apply(ptr,N,off,nboundary_conditions,x + 2);
        }
        ptr[(N - 1)*off] = (T)(n - 2*c + p);
      } break;
      }
    }

    static T __cimg_blur_box_apply(T *ptr, const int N, const ulongT off,
                                   const unsigned int boundary_conditions, const int x) {
      switch (boundary_conditions) {
      case 0 : // Dirichlet
        return x<0 || x>=N?(T)0:ptr[x*off];
      case 1 : { // Neumann
        const int nx = x<0?0:x>=N?N - 1:x;
        return ptr[nx*off];
      }
      case 2 : { // Periodic
        const int nx = cimg::mod(x,N);
        return ptr[nx*off];
      }
      default : { // Mirror
        const int
          N2 = 2*N,
          tx = cimg::mod(x,N2),
          nx = tx<N?tx:N2 - tx - 1;
        return ptr[nx*off];
      }
      }
      return (T)0;
    }

    // Apply box filter of order 0,1,2.
    /**
      \param boxsize Size of the box window (can be subpixel)
      \param order the order of the filter 0,1 or 2.
      \param axis  Axis along which the filter is computed. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
      \param boundary_conditions Boundary conditions.
        Can be <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
      \param nb_iter Number of filter iterations.
    **/
    CImg<T>& boxfilter(const float boxsize, const int order, const char axis='x',
                       const unsigned int boundary_conditions=1,
                       const unsigned int nb_iter=1) {
      const char naxis = cimg::lowercase(axis);
      const float nboxsize = boxsize>=0?boxsize:-boxsize*
        (naxis=='x'?_width:naxis=='y'?_height:naxis=='z'?_depth:_spectrum)/100;
      if (is_empty() || !nboxsize || (nboxsize<=1 && !order)) return *this;
      switch (naxis) {
      case 'x' : {
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forYZC(*this,y,z,c)
          _cimg_blur_box_apply(data(0,y,z,c),nboxsize,_width,1U,order,boundary_conditions,nb_iter);
      } break;
      case 'y' : {
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forXZC(*this,x,z,c)
          _cimg_blur_box_apply(data(x,0,z,c),nboxsize,_height,(ulongT)_width,order,boundary_conditions,nb_iter);
      } break;
      case 'z' : {
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forXYC(*this,x,y,c)
          _cimg_blur_box_apply(data(x,y,0,c),nboxsize,_depth,(ulongT)_width*_height,order,boundary_conditions,nb_iter);
      } break;
      default : {
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forXYZ(*this,x,y,z)
          _cimg_blur_box_apply(data(x,y,z,0),nboxsize,_spectrum,(ulongT)_width*_height*_depth,
                               order,boundary_conditions,nb_iter);
      }
      }
      return *this;
    }

    // Apply box filter of order 0,1 or 2 \newinstance.
    CImg<Tfloat> get_boxfilter(const float boxsize, const int order, const char axis='x',
                               const unsigned int boundary_conditions=1,
                               const unsigned int nb_iter=1) const {
      return CImg<Tfloat>(*this,false).boxfilter(boxsize,order,axis,boundary_conditions,nb_iter);
    }

    //! Blur image with a box filter.
    /**
       \param boxsize_x Size of the box window, along the X-axis (can be subpixel).
       \param boxsize_y Size of the box window, along the Y-axis (can be subpixel).
       \param boxsize_z Size of the box window, along the Z-axis (can be subpixel).
       \param boundary_conditions Boundary conditions.
         Can be <tt>{ false=dirichlet | true=neumann | 2=periodic | 3=mirror }</tt>.
       \param nb_iter Number of filter iterations.
       \note
       - This is a recursive algorithm, not depending on the values of the box kernel size.
       \see blur().
    **/
    CImg<T>& blur_box(const float boxsize_x, const float boxsize_y, const float boxsize_z,
                      const unsigned int boundary_conditions=1,
                      const unsigned int nb_iter=1) {
      if (is_empty()) return *this;
      if (_width>1) boxfilter(boxsize_x,0,'x',boundary_conditions,nb_iter);
      if (_height>1) boxfilter(boxsize_y,0,'y',boundary_conditions,nb_iter);
      if (_depth>1) boxfilter(boxsize_z,0,'z',boundary_conditions,nb_iter);
      return *this;
    }

    //! Blur image with a box filter \newinstance.
    CImg<Tfloat> get_blur_box(const float boxsize_x, const float boxsize_y, const float boxsize_z,
                              const unsigned int boundary_conditions=1) const {
      return CImg<Tfloat>(*this,false).blur_box(boxsize_x,boxsize_y,boxsize_z,boundary_conditions);
    }

    //! Blur image with a box filter.
    /**
       \param boxsize Size of the box window (can be subpixel).
       \param boundary_conditions Boundary conditions.
         Can be <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.a
       \see deriche(), vanvliet().
    **/
    CImg<T>& blur_box(const float boxsize, const unsigned int boundary_conditions=1) {
      const float nboxsize = boxsize>=0?boxsize:-boxsize*cimg::max(_width,_height,_depth)/100;
      return blur_box(nboxsize,nboxsize,nboxsize,boundary_conditions);
    }

    //! Blur image with a box filter \newinstance.
    CImg<Tfloat> get_blur_box(const float boxsize, const unsigned int boundary_conditions=1) const {
      return CImg<Tfloat>(*this,false).blur_box(boxsize,boundary_conditions);
    }

    //! Blur image, with the image guided filter.
    /**
       \param guide Image used to guide the smoothing process.
       \param radius Spatial radius. If negative, it is expressed as a percentage of the largest image size.
       \param regularization Regularization parameter.
                             If negative, it is expressed as a percentage of the guide value range.
       \note This method implements the filtering algorithm described in:
       He, Kaiming; Sun, Jian; Tang, Xiaoou, "Guided Image Filtering," Pattern Analysis and Machine Intelligence,
       IEEE Transactions on , vol.35, no.6, pp.1397,1409, June 2013
    **/
    template<typename t>
    CImg<T>& blur_guided(const CImg<t>& guide, const float radius, const float regularization) {
      return get_blur_guided(guide,radius,regularization).move_to(*this);
    }

    //! Blur image, with the image guided filter \newinstance.
    template<typename t>
    CImg<Tfloat> get_blur_guided(const CImg<t>& guide, const float radius, const float regularization) const {
      if (!is_sameXYZ(guide))
        throw CImgArgumentException(_cimg_instance
                                    "blur_guided(): Invalid size for specified guide image (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data);
      if (is_empty() || !radius) return *this;
      const int _radius = radius>=0?(int)radius:(int)(-radius*cimg::max(_width,_height,_depth)/100);
      float _regularization = regularization;
      if (regularization<0) {
        T edge_min, edge_max = guide.max_min(edge_min);
        if (edge_min==edge_max) return *this;
        _regularization = -regularization*(edge_max - edge_min)/100;
      }
      _regularization = std::max(_regularization,0.01f);
      const unsigned int psize = (unsigned int)(1 + 2*_radius);
      CImg<Tfloat>
        mean_p = get_blur_box(psize,true),
        mean_I = guide.get_blur_box(psize,true).resize(mean_p),
        cov_Ip = get_mul(guide).blur_box(psize,true)-=mean_p.get_mul(mean_I),
        var_I = guide.get_sqr().blur_box(psize,true)-=mean_I.get_sqr(),
        &a = cov_Ip.div(var_I+=_regularization),
        &b = mean_p-=a.get_mul(mean_I);
      a.blur_box(psize,true);
      b.blur_box(psize,true);
      return a.mul(guide)+=b;
    }

    //! Blur image using patch-based space.
    /**
       \param guide Image used to model the smoothing weights.
       \param sigma_s Amount of blur along the XYZ-axes.
       \param sigma_r Amount of blur along the value axis.
       \param patch_size Size of the patches.
       \param lookup_size Size of the window to search similar patches.
       \param smoothness Smoothness for the patch comparison.
       \param is_fast_approx Tells if a fast approximation of the gaussian function is used or not.
    **/
    template<typename t>
    CImg<T>& blur_patch(const CImg<t>& guide,
                        const float sigma_s, const float sigma_r, const unsigned int patch_size=3,
                        const unsigned int lookup_size=4, const float smoothness=0, const bool is_fast_approx=true) {
      if (is_empty() || !patch_size || !lookup_size) return *this;
      return get_blur_patch(guide,sigma_s,sigma_r,patch_size,lookup_size,smoothness,is_fast_approx).move_to(*this);
    }

    //! Blur image using patch-based space \newinstance.
    template<typename t>
    CImg<Tfloat> get_blur_patch(const CImg<t>& guide,
                                const float sigma_s, const float sigma_r, const unsigned int patch_size=3,
                                const unsigned int lookup_size=4, const float smoothness=0,
                                const bool is_fast_approx=true) const {

#define _cimg_blur_patch3d_fast(N) { \
      cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) \
                         cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*32 && res._height*res._depth>=4) \
                         firstprivate(P,Q)) \
      cimg_forXYZ(res,x,y,z) _cimg_abort_try_openmp2 { \
        cimg_abort_test2; \
        cimg_def##N##x##N##x##N(res,x,y,z); \
        tfloat *pP = P._data; cimg_forC(_guide,c) { cimg_get##N##x##N##x##N(_guide,x,y,z,c,pP,tfloat); pP+=N3; } \
        const int x0 = x - rsize1, y0 = y - rsize1, z0 = z - rsize1, \
                  x1 = x + rsize2, y1 = y + rsize2, z1 = z + rsize2; \
        tfloat sum_weights = 0; \
        cimg_for_in##N##XYZ(res,x0,y0,z0,x1,y1,z1,p,q,r) \
          if (cimg::abs(_guide(x,y,z,0) - _guide(p,q,r,0))<sigma_r3) { \
            tfloat *pQ = Q._data; cimg_forC(_guide,c) { cimg_get##N##x##N##x##N(_guide,p,q,r,c,pQ,tfloat); pQ+=N3; } \
            tfloat distance2 = 0; \
            pQ = Q._data; cimg_for(P,_pP,tfloat) { const tfloat dI = *_pP - *(pQ++); distance2+=dI*dI; } \
            distance2/=Pnorm; \
            const tfloat dx = (tfloat)p - x, dy = (tfloat)q - y, dz = (tfloat)r - z, \
              alldist = distance2 + (dx*dx + dy*dy + dz*dz)/sigma_s2, weight = alldist>3?0:1; \
            sum_weights+=weight; \
            cimg_forC(res,c) res(x,y,z,c)+=(Tfloat)weight*(*this)(p,q,r,c); \
          } \
        if (sum_weights>1e-10) cimg_forC(res,c) res(x,y,z,c)/=(Tfloat)sum_weights; \
        else cimg_forC(res,c) res(x,y,z,c) = (Tfloat)((*this)(x,y,z,c)); \
    } _cimg_abort_catch_openmp2 }

#define _cimg_blur_patch3d(N) { \
      cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) \
                         cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*32 && res._height*res._depth>=4) \
                         firstprivate(P,Q)) \
      cimg_forXYZ(res,x,y,z) _cimg_abort_try_openmp2 { \
        cimg_abort_test2; \
        cimg_def##N##x##N##x##N(res,x,y,z); \
        tfloat *pP = P._data; cimg_forC(_guide,c) { cimg_get##N##x##N##x##N(_guide,x,y,z,c,pP,tfloat); pP+=N3; } \
        const int x0 = x - rsize1, y0 = y - rsize1, z0 = z - rsize1, \
                  x1 = x + rsize2, y1 = y + rsize2, z1 = z + rsize2; \
        tfloat sum_weights = 0, weight_max = 0; \
        cimg_for_in##N##XYZ(res,x0,y0,z0,x1,y1,z1,p,q,r) if (p!=x || q!=y || r!=z) { \
          tfloat *pQ = Q._data; cimg_forC(_guide,c) { cimg_get##N##x##N##x##N(_guide,p,q,r,c,pQ,tfloat); pQ+=N3; } \
          tfloat distance2 = 0; \
          pQ = Q._data; cimg_for(P,_pP,tfloat) { const tfloat dI = *_pP - *(pQ++); distance2+=dI*dI; } \
          distance2/=Pnorm; \
          const tfloat dx = (tfloat)p - x, dy = (tfloat)q - y, dz = (tfloat)r - z, \
            alldist = distance2 + (dx*dx + dy*dy + dz*dz)/sigma_s2, weight = std::exp(-alldist); \
          if (weight>weight_max) weight_max = weight; \
          sum_weights+=weight; \
          cimg_forC(res,c) res(x,y,z,c)+=(Tfloat)weight*(*this)(p,q,r,c); \
        } \
        sum_weights+=weight_max; cimg_forC(res,c) res(x,y,z,c)+=(Tfloat)weight_max*(*this)(x,y,z,c); \
        if (sum_weights>1e-10) cimg_forC(res,c) res(x,y,z,c)/=(Tfloat)sum_weights; \
        else cimg_forC(res,c) res(x,y,z,c) = (Tfloat)((*this)(x,y,z,c)); \
      } _cimg_abort_catch_openmp2 }

#define _cimg_blur_patch2d_fast(N) { \
        cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*32 && res._height>=4) \
                           firstprivate(P,Q)) \
        cimg_forXY(res,x,y) _cimg_abort_try_openmp2 { \
          cimg_abort_test2; \
          cimg_def##N##x##N(res,x,y); \
          tfloat *pP = P._data; cimg_forC(_guide,c) { cimg_get##N##x##N(_guide,x,y,0,c,pP,tfloat); pP+=N2; } \
          const int x0 = x - rsize1, y0 = y - rsize1, x1 = x + rsize2, y1 = y + rsize2; \
          tfloat sum_weights = 0; \
          cimg_for_in##N##XY(res,x0,y0,x1,y1,p,q) \
            if (cimg::abs(_guide(x,y,0,0) - _guide(p,q,0,0))<sigma_r3) { \
              tfloat *pQ = Q._data; cimg_forC(_guide,c) { cimg_get##N##x##N(_guide,p,q,0,c,pQ,tfloat); pQ+=N2; } \
              tfloat distance2 = 0; \
              pQ = Q._data; cimg_for(P,_pP,tfloat) { const tfloat dI = *_pP - *(pQ++); distance2+=dI*dI; } \
              distance2/=Pnorm; \
              const tfloat dx = (tfloat)p - x, dy = (tfloat)q - y, \
                alldist = distance2 + (dx*dx+dy*dy)/sigma_s2, weight = alldist>3?0:1; \
              sum_weights+=weight; \
              cimg_forC(res,c) res(x,y,c)+=(Tfloat)weight*(*this)(p,q,c); \
            } \
          if (sum_weights>1e-10) cimg_forC(res,c) res(x,y,c)/=(Tfloat)sum_weights; \
          else cimg_forC(res,c) res(x,y,c) = (Tfloat)((*this)(x,y,c)); \
        } _cimg_abort_catch_openmp2 }

#define _cimg_blur_patch2d(N) { \
        cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*32 && res._height>=4) \
                           firstprivate(P,Q)) \
        cimg_forXY(res,x,y) _cimg_abort_try_openmp2 { \
          cimg_abort_test2; \
          cimg_def##N##x##N(res,x,y); \
          tfloat *pP = P._data; cimg_forC(_guide,c) { cimg_get##N##x##N(_guide,x,y,0,c,pP,tfloat); pP+=N2; } \
          const int x0 = x - rsize1, y0 = y - rsize1, x1 = x + rsize2, y1 = y + rsize2; \
          tfloat sum_weights = 0, weight_max = 0; \
          cimg_for_in##N##XY(res,x0,y0,x1,y1,p,q) if (p!=x || q!=y) { \
            tfloat *pQ = Q._data; cimg_forC(_guide,c) { cimg_get##N##x##N(_guide,p,q,0,c,pQ,tfloat); pQ+=N2; } \
            tfloat distance2 = 0; \
            pQ = Q._data; cimg_for(P,_pP,tfloat) { const tfloat dI = *_pP - *(pQ++); distance2+=dI*dI; } \
            distance2/=Pnorm; \
            const tfloat dx = (tfloat)p - x, dy = (tfloat)q - y, \
              alldist = distance2 + (dx*dx+dy*dy)/sigma_s2, weight = std::exp(-alldist); \
            if (weight>weight_max) weight_max = weight; \
            sum_weights+=weight; \
            cimg_forC(res,c) res(x,y,c)+=(Tfloat)weight*(*this)(p,q,c); \
          } \
          sum_weights+=weight_max; cimg_forC(res,c) res(x,y,c)+=(Tfloat)weight_max*(*this)(x,y,c); \
          if (sum_weights>1e-10) cimg_forC(res,c) res(x,y,c)/=(Tfloat)sum_weights; \
          else cimg_forC(res,c) res(x,y,c) = (Tfloat)((*this)(x,y,c)); \
    } _cimg_abort_catch_openmp2 }

      typedef _cimg_tfloat tfloat;
      if (!is_sameXYZ(guide))
        throw CImgArgumentException(_cimg_instance
                                    "blur_patch(): Invalid size for specified guide image (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data);
      if (is_empty() || !patch_size || !lookup_size) return +*this;
      Tfloat val_min, val_max = (Tfloat)max_min(val_min);
      _cimg_abort_init_openmp;
      cimg_abort_init;

      CImg<Tfloat> res(_width,_height,_depth,_spectrum,0);
      const CImg<tfloat>
        __guide = guide?CImg<tfloat>(guide,guide.pixel_type()==cimg::type<tfloat>::string()):
                        CImg<tfloat>(*this,pixel_type()==cimg::type<tfloat>::string()),
        _guide = smoothness>0?__guide.get_blur(smoothness):__guide.get_shared();
      CImg<tfloat> P(_guide._spectrum*patch_size*patch_size*(_depth>1?patch_size:1)), Q(P);

      t guide_min = (t)0, guide_max = (t)0;
      if (sigma_r<0) guide_max = guide.max_min(guide_min);
      const float
        guide_delta = (float)(guide_max - guide_min),
        _sigma_s = sigma_s>=0?sigma_s:-sigma_s*cimg::max(_width,_height,_depth)/100,
        _sigma_r = sigma_r>=0?sigma_r:-sigma_r*guide_delta/100,
        sigma_s2 = _sigma_s*_sigma_s,
        sigma_r2 = _sigma_r*_sigma_r,
        sigma_r3 = 3*_sigma_r,
        Pnorm = P.size()*sigma_r2;
      const int rsize2 = (int)lookup_size/2, rsize1 = (int)lookup_size - rsize2 - 1;
      const unsigned int N2 = patch_size*patch_size, N3 = N2*patch_size;
      cimg::unused(N2,N3);
      if (_depth>1) switch (patch_size) { // 3D
        case 2 : if (is_fast_approx) _cimg_blur_patch3d_fast(2) else _cimg_blur_patch3d(2) break;
        case 3 : if (is_fast_approx) _cimg_blur_patch3d_fast(3) else _cimg_blur_patch3d(3) break;
        default : {
          const int psize2 = (int)patch_size/2, psize1 = (int)patch_size - psize2 - 1;
          if (is_fast_approx) {
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                               cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*32 && res._height*res._depth>=4)
                               firstprivate(P,Q))
            cimg_forXYZ(res,x,y,z) _cimg_abort_try_openmp2 { // Fast
              cimg_abort_test2;
              P = _guide.get_crop(x - psize1,y - psize1,z - psize1,x + psize2,y + psize2,z + psize2,true);
              const int x0 = x - rsize1, y0 = y - rsize1, z0 = z - rsize1,
                x1 = x + rsize2, y1 = y + rsize2, z1 = z + rsize2;
              tfloat sum_weights = 0;
              cimg_for_inXYZ(res,x0,y0,z0,x1,y1,z1,p,q,r)
                if (cimg::abs(_guide(x,y,z,0) - _guide(p,q,r,0))<sigma_r3) {
                  (Q = _guide.get_crop(p - psize1,q - psize1,r - psize1,p + psize2,q + psize2,r + psize2,true))-=P;
                  const tfloat
                    dx = (tfloat)x - p, dy = (tfloat)y - q, dz = (tfloat)z - r,
                    distance2 = (tfloat)(Q.pow(2).sum()/Pnorm + (dx*dx + dy*dy + dz*dz)/sigma_s2),
                    weight = distance2>3?0:1;
                  sum_weights+=weight;
                  cimg_forC(res,c) res(x,y,z,c)+=(Tfloat)weight*(*this)(p,q,r,c);
                }
              if (sum_weights>1e-10) cimg_forC(res,c) res(x,y,z,c)/=(Tfloat)sum_weights;
              else cimg_forC(res,c) res(x,y,z,c) = (Tfloat)((*this)(x,y,z,c));
            } _cimg_abort_catch_openmp2
          } else {
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                               cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*32 && res._height*res._depth>=4)
                               firstprivate(P,Q))
            cimg_forXYZ(res,x,y,z) _cimg_abort_try_openmp2 { // Exact
              cimg_abort_test2;
              P = _guide.get_crop(x - psize1,y - psize1,z - psize1,x + psize2,y + psize2,z + psize2,true);
              const int x0 = x - rsize1, y0 = y - rsize1, z0 = z - rsize1,
                        x1 = x + rsize2, y1 = y + rsize2, z1 = z + rsize2;
              tfloat sum_weights = 0, weight_max = 0;
              cimg_for_inXYZ(res,x0,y0,z0,x1,y1,z1,p,q,r) if (p!=x || q!=y || r!=z) {
                (Q = _guide.get_crop(p - psize1,q - psize1,r - psize1,p + psize2,q + psize2,r + psize2,true))-=P;
                const tfloat
                  dx = (tfloat)x - p, dy = (tfloat)y - q, dz = (tfloat)z - r,
                  distance2 = (tfloat)(Q.pow(2).sum()/Pnorm + (dx*dx + dy*dy + dz*dz)/sigma_s2),
                  weight = std::exp(-distance2);
                if (weight>weight_max) weight_max = weight;
                sum_weights+=weight;
                cimg_forC(res,c) res(x,y,z,c)+=(Tfloat)weight*(*this)(p,q,r,c);
              }
              sum_weights+=weight_max; cimg_forC(res,c) res(x,y,z,c)+=(Tfloat)weight_max*(*this)(x,y,z,c);
              if (sum_weights>1e-10) cimg_forC(res,c) res(x,y,z,c)/=(Tfloat)sum_weights;
              else cimg_forC(res,c) res(x,y,z,c) = (Tfloat)((*this)(x,y,z,c));
            } _cimg_abort_catch_openmp2
          }
        }
        } else switch (patch_size) { // 2D
        case 2 : if (is_fast_approx) _cimg_blur_patch2d_fast(2) else _cimg_blur_patch2d(2) break;
        case 3 : if (is_fast_approx) _cimg_blur_patch2d_fast(3) else _cimg_blur_patch2d(3) break;
        case 4 : if (is_fast_approx) _cimg_blur_patch2d_fast(4) else _cimg_blur_patch2d(4) break;
        case 5 : if (is_fast_approx) _cimg_blur_patch2d_fast(5) else _cimg_blur_patch2d(5) break;
        case 6 : if (is_fast_approx) _cimg_blur_patch2d_fast(6) else _cimg_blur_patch2d(6) break;
        case 7 : if (is_fast_approx) _cimg_blur_patch2d_fast(7) else _cimg_blur_patch2d(7) break;
        case 8 : if (is_fast_approx) _cimg_blur_patch2d_fast(8) else _cimg_blur_patch2d(8) break;
        case 9 : if (is_fast_approx) _cimg_blur_patch2d_fast(9) else _cimg_blur_patch2d(9) break;
        default : { // Fast
          const int psize2 = (int)patch_size/2, psize1 = (int)patch_size - psize2 - 1;
          if (is_fast_approx) {
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*32 && res._height>=4)
                               firstprivate(P,Q))
            cimg_forXY(res,x,y) _cimg_abort_try_openmp2 { // Fast
              cimg_abort_test2;
              P = _guide.get_crop(x - psize1,y - psize1,x + psize2,y + psize2,true);
              const int x0 = x - rsize1, y0 = y - rsize1, x1 = x + rsize2, y1 = y + rsize2;
              tfloat sum_weights = 0;
              cimg_for_inXY(res,x0,y0,x1,y1,p,q)
                if (cimg::abs(_guide(x,y,0) - _guide(p,q,0))<sigma_r3) {
                  (Q = _guide.get_crop(p - psize1,q - psize1,p + psize2,q + psize2,true))-=P;
                  const tfloat
                    dx = (tfloat)x - p, dy = (tfloat)y - q,
                    distance2 = (tfloat)(Q.pow(2).sum()/Pnorm + (dx*dx + dy*dy)/sigma_s2),
                    weight = distance2>3?0:1;
                  sum_weights+=weight;
                  cimg_forC(res,c) res(x,y,c)+=(Tfloat)weight*(*this)(p,q,c);
                }
              if (sum_weights>1e-10) cimg_forC(res,c) res(x,y,c)/=(Tfloat)sum_weights;
              else cimg_forC(res,c) res(x,y,c) = (Tfloat)((*this)(x,y,c));
            } _cimg_abort_catch_openmp2
          } else {
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=(cimg_openmp_sizefactor)*32 && res._height>=4)
                               firstprivate(P,Q))
            cimg_forXY(res,x,y) _cimg_abort_try_openmp2 { // Exact
              cimg_abort_test2;
              P = _guide.get_crop(x - psize1,y - psize1,x + psize2,y + psize2,true);
              const int x0 = x - rsize1, y0 = y - rsize1, x1 = x + rsize2, y1 = y + rsize2;
              tfloat sum_weights = 0, weight_max = 0;
              cimg_for_inXY(res,x0,y0,x1,y1,p,q) if (p!=x || q!=y) {
                (Q = _guide.get_crop(p - psize1,q - psize1,p + psize2,q + psize2,true))-=P;
                const tfloat
                  dx = (tfloat)x - p, dy = (tfloat)y - q,
                  distance2 = (tfloat)(Q.pow(2).sum()/Pnorm + (dx*dx + dy*dy)/sigma_s2),
                  weight = std::exp(-distance2);
                if (weight>weight_max) weight_max = weight;
                sum_weights+=weight;
                cimg_forC(res,c) res(x,y,c)+=(Tfloat)weight*(*this)(p,q,c);
              }
              sum_weights+=weight_max; cimg_forC(res,c) res(x,y,c)+=(Tfloat)weight_max*(*this)(x,y,c);
              if (sum_weights>1e-10) cimg_forC(res,c) res(x,y,c)/=(Tfloat)sum_weights;
              else cimg_forC(res,c) res(x,y,c) = (Tfloat)((*this)(x,y,c));
            } _cimg_abort_catch_openmp2
          }
        }
        }
      cimg_abort_test;
      return res.cut(val_min,val_max);
    }

    //! Blur image using patch-based space \simplification.
    CImg<T>& blur_patch(const float sigma_s, const float sigma_r, const unsigned int patch_size=3,
                        const unsigned int lookup_size=4, const float smoothness=0, const bool is_fast_approx=true) {
      return blur_patch(*this,sigma_s,sigma_r,patch_size,lookup_size,smoothness,is_fast_approx);
    }

    //! Blur image using patch-based space \simplification \newinstance.
    CImg<Tfloat> get_blur_patch(const float sigma_s, const float sigma_r, const unsigned int patch_size=3,
                                const unsigned int lookup_size=4, const float smoothness=0,
                                const bool is_fast_approx=true) const {
      return get_blur_patch(*this,sigma_s,sigma_r,patch_size,lookup_size,smoothness,is_fast_approx);
    }

    //! Blur image with the median filter.
    /**
       \param n Size of the median filter.
       \param threshold Threshold used to discard pixels too far from the current pixel value in the median computation.
    **/
    CImg<T>& blur_median(const unsigned int n, const float threshold=0) {
      if (!n) return *this;
      return get_blur_median(n,threshold).move_to(*this);
    }

    //! Blur image with the median filter \newinstance.
    CImg<T> get_blur_median(const unsigned int n, const float threshold=0) const {
      if (is_empty() || n<=1) return +*this;

      // Manage special case of single-axis image.
      CImg<T> img = get_shared();
      const unsigned int whd = _width*_height*_depth;
      if (_width==whd || _height==whd || _depth==whd) { img._width = whd; img._height = img._depth = 1; }

      CImg<T> res(_width,_height,_depth,_spectrum);
      T *ptrd = res._data;
      cimg::unused(ptrd);
      const int hr = (int)n/2, hl = n - hr - 1;
      if (img._depth!=1) { // 3D
        if (threshold>0) // With threshold
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3)
                             cimg_openmp_if(img._width>=(cimg_openmp_sizefactor)*16 &&
                                            img._height*img._depth*img._spectrum>=4))
          cimg_forXYZC(img,x,y,z,c) {
            const int
              x0 = x - hl, y0 = y - hl, z0 = z - hl, x1 = x + hr, y1 = y + hr, z1 = z + hr,
              nx0 = x0<0?0:x0, ny0 = y0<0?0:y0, nz0 = z0<0?0:z0,
              nx1 = x1>=img.width()?img.width() - 1:x1,
              ny1 = y1>=img.height()?img.height() - 1:y1,
              nz1 = z1>=img.depth()?img.depth() - 1:z1;
            const Tfloat val0 = (Tfloat)img(x,y,z,c);
            CImg<T> values(n*n*n);
            unsigned int nb_values = 0;
            T *_ptrd = values.data();
            cimg_for_inXYZ(img,nx0,ny0,nz0,nx1,ny1,nz1,p,q,r)
              if (cimg::abs(img(p,q,r,c) - val0)<=threshold) { *(_ptrd++) = img(p,q,r,c); ++nb_values; }
            res(x,y,z,c) = nb_values?values.get_shared_points(0,nb_values - 1).median():img(x,y,z,c);
          }
        else // Without threshold
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3)
                             cimg_openmp_if(img._width>=(cimg_openmp_sizefactor)*16 &&
                                            img._height*img._depth*img._spectrum>=4))
          cimg_forXYZC(img,x,y,z,c) {
            const int
              x0 = x - hl, y0 = y - hl, z0 = z - hl, x1 = x + hr, y1 = y + hr, z1 = z + hr,
              nx0 = x0<0?0:x0, ny0 = y0<0?0:y0, nz0 = z0<0?0:z0,
              nx1 = x1>=img.width()?img.width() - 1:x1,
              ny1 = y1>=img.height()?img.height() - 1:y1,
              nz1 = z1>=img.depth()?img.depth() - 1:z1;
            res(x,y,z,c) = img.get_crop(nx0,ny0,nz0,c,nx1,ny1,nz1,c).median();
          }
      } else { // 2D or 1D
        if (threshold>0) // With threshold
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                             cimg_openmp_if(img._width>=(cimg_openmp_sizefactor)*16 &&
                                            img._height*img._spectrum>=4))
          cimg_forXYC(img,x,y,c) {
            const int
              x0 = x - hl, y0 = y - hl, x1 = x + hr, y1 = y + hr,
              nx0 = x0<0?0:x0, ny0 = y0<0?0:y0,
              nx1 = x1>=img.width()?img.width() - 1:x1,
              ny1 = y1>=img.height()?img.height() - 1:y1;
            const Tfloat val0 = (Tfloat)img(x,y,c);
            CImg<T> values(n*n);
            unsigned int nb_values = 0;
            T *_ptrd = values.data();
            cimg_for_inXY(img,nx0,ny0,nx1,ny1,p,q)
              if (cimg::abs(img(p,q,c) - val0)<=threshold) { *(_ptrd++) = img(p,q,c); ++nb_values; }
            res(x,y,c) = nb_values?values.get_shared_points(0,nb_values - 1).median():img(x,y,c);
          }
        else { // Without threshold
          const int
            w1 = img.width() - 1, h1 = img.height() - 1,
            w2 = img.width() - 2, h2 = img.height() - 2,
            w3 = img.width() - 3, h3 = img.height() - 3,
            w4 = img.width() - 4, h4 = img.height() - 4;

          if (n==3 && img._width>=2 && img._height==1) {
            cimg_pragma_openmp(parallel for cimg_openmp_if(img._spectrum>=2))
            cimg_forC(img,c) {
              const T *_ptrs = img.data(0,0,0,c);
              T *_ptrd = res.data(0,0,0,c);
              cimg_for_in3(img._width,1,w2,x)
                _ptrd[x] = cimg::median(_ptrs[_p1x],_ptrs[x],_ptrs[_n1x]);
              _ptrd[0] = cimg::median(_ptrs[0],_ptrs[1]);
              _ptrd[w1] = cimg::median(_ptrs[w2],_ptrs[w1]);
            }
          } else if (n==5 && img._width>=4 && img._height==1) {
            cimg_pragma_openmp(parallel for cimg_openmp_if(img._spectrum>=2))
            cimg_forC(img,c) {
              const T *_ptrs = img.data(0,0,0,c);
              T *_ptrd = res.data(0,0,0,c);
              cimg_for_in5(img._width,2,w3,x)
                _ptrd[x] = cimg::median(_ptrs[_p2x],_ptrs[_p1x],_ptrs[x],_ptrs[_n1x],_ptrs[_n2x]);
              _ptrd[0] = cimg::median(_ptrs[0],_ptrs[1],_ptrs[2]);
              _ptrd[1] = cimg::median(_ptrs[0],_ptrs[1],_ptrs[2],_ptrs[3]);
              _ptrd[w2] = cimg::median(_ptrs[w4],_ptrs[w3],_ptrs[w2],_ptrs[w1]);
              _ptrd[w1] = cimg::median(_ptrs[w3],_ptrs[w2],_ptrs[w1]);
            }
          } else if (n==3 && img._width>=n && img._height>=n) {
            cimg_pragma_openmp(parallel for cimg_openmp_if(img._spectrum>=2))
            cimg_forC(img,c) {
              CImg<T> I(9);
              cimg_for_in3x3(img,1,1,w2,h2,x,y,0,c,I,T)
                res(x,y,c) = cimg::median(I[0],I[1],I[2],I[3],I[4],I[5],I[6],I[7],I[8]);
              cimg_for_borderXY(img,x,y,1)
                res(x,y,c) = img.get_crop(std::max(0,x - 1),std::max(0,y - 1),0,c,
                                          std::min(w1,x + 1),std::min(h1,y + 1),0,c).median();
            }
          } else if (n==5 && img._width>=n && img._height>=n) {
            cimg_pragma_openmp(parallel for cimg_openmp_if(img._spectrum>=2))
            cimg_forC(img,c) {
              CImg<T> I(25);
              cimg_for_in5x5(img,2,2,w3,h3,x,y,0,c,I,T)
                res(x,y,c) = cimg::median(I[0],I[1],I[2],I[3],I[4],
                                          I[5],I[6],I[7],I[8],I[9],
                                          I[10],I[11],I[12],I[13],I[14],
                                          I[15],I[16],I[17],I[18],I[19],
                                          I[20],I[21],I[22],I[23],I[24]);
              cimg_for_borderXY(img,x,y,2)
                res(x,y,c) = img.get_crop(std::max(0,x - 2),std::max(0,y - 2),0,c,
                                          std::min(w1,x + 2),std::min(h1,y + 2),0,c).median();
            }
          } else if (n==7 && img._width>=n && img._height>=n) {
            cimg_pragma_openmp(parallel for cimg_openmp_if(img._spectrum>=2))
            cimg_forC(img,c) {
              CImg<T> I(49);
              cimg_for_in7x7(img,3,3,w4,h4,x,y,0,c,I,T)
                res(x,y,c) = cimg::median(I[0],I[1],I[2],I[3],I[4],I[5],I[6],
                                          I[7],I[8],I[9],I[10],I[11],I[12],I[13],
                                          I[14],I[15],I[16],I[17],I[18],I[19],I[20],
                                          I[21],I[22],I[23],I[24],I[25],I[26],I[27],
                                          I[28],I[29],I[30],I[31],I[32],I[33],I[34],
                                          I[35],I[36],I[37],I[38],I[39],I[40],I[41],
                                          I[42],I[43],I[44],I[45],I[46],I[47],I[48]);
              cimg_for_borderXY(img,x,y,3)
                res(x,y,c) = img.get_crop(std::max(0,x - 3),std::max(0,y - 3),0,c,
                                          std::min(w1,x + 3),std::min(h1,y + 3),0,c).median();
            }
          } else {
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                               cimg_openmp_if(img._width>=(cimg_openmp_sizefactor)*16 &&
                                              img._height*img._spectrum>=4))
            cimg_forXYC(img,x,y,c) {
              const int
                x0 = x - hl, y0 = y - hl, x1 = x + hr, y1 = y + hr,
                nx0 = x0<0?0:x0, ny0 = y0<0?0:y0,
                nx1 = x1>=img.width()?img.width() - 1:x1,
                ny1 = y1>=img.height()?img.height() - 1:y1;
              res(x,y,c) = img.get_crop(nx0,ny0,0,c,nx1,ny1,0,c).median();
            }
          }
        }
      }
      return res;
    }

    //! Sharpen image.
    /**
       \param amplitude Sharpening amplitude
       \param sharpen_type Select sharpening method. Can be <tt>{ false=inverse diffusion | true=shock filters }</tt>.
       \param edge Edge threshold (shock filters only).
       \param alpha Gradient smoothness (shock filters only).
       \param sigma Tensor smoothness (shock filters only).
    **/
    CImg<T>& sharpen(const float amplitude, const bool sharpen_type=false, const float edge=1,
                     const float alpha=0, const float sigma=0) {
      if (is_empty()) return *this;
      T val_min, val_max = max_min(val_min);
      const float nedge = edge/2;
      CImg<Tfloat> velocity(_width,_height,_depth,_spectrum), _veloc_max(_spectrum);

      if (_depth>1) { // 3D
        if (sharpen_type) { // Shock filters
          CImg<Tfloat> G = (alpha>0?get_blur(alpha).get_structure_tensors():get_structure_tensors());
          if (sigma>0) G.blur(sigma);
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*32 &&
                                                                     _height*_depth>=16))
          cimg_forYZ(G,y,z) {
            Tfloat *ptrG0 = G.data(0,y,z,0), *ptrG1 = G.data(0,y,z,1),
              *ptrG2 = G.data(0,y,z,2), *ptrG3 = G.data(0,y,z,3);
            CImg<Tfloat> val, vec;
            cimg_forX(G,x) {
              G.get_tensor_at(x,y,z).symmetric_eigen(val,vec);
              if (val[0]<0) val[0] = 0;
              if (val[1]<0) val[1] = 0;
              if (val[2]<0) val[2] = 0;
              *(ptrG0++) = vec(0,0);
              *(ptrG1++) = vec(0,1);
              *(ptrG2++) = vec(0,2);
              *(ptrG3++) = 1 - (Tfloat)std::pow(1 + val[0] + val[1] + val[2],-(Tfloat)nedge);
            }
          }
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*512 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd = velocity.data(0,0,0,c), veloc_max = 0;
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              const Tfloat
                u = G(x,y,z,0),
                v = G(x,y,z,1),
                w = G(x,y,z,2),
                amp = G(x,y,z,3),
                ixx = Incc + Ipcc - 2*Iccc,
                ixy = (Innc + Ippc - Inpc - Ipnc)/4,
                ixz = (Incn + Ipcp - Incp - Ipcn)/4,
                iyy = Icnc + Icpc - 2*Iccc,
                iyz = (Icnn + Icpp - Icnp - Icpn)/4,
                izz = Iccn + Iccp - 2*Iccc,
                ixf = Incc - Iccc,
                ixb = Iccc - Ipcc,
                iyf = Icnc - Iccc,
                iyb = Iccc - Icpc,
                izf = Iccn - Iccc,
                izb = Iccc - Iccp,
                itt = u*u*ixx + v*v*iyy + w*w*izz + 2*u*v*ixy + 2*u*w*ixz + 2*v*w*iyz,
                it = u*cimg::minmod(ixf,ixb) + v*cimg::minmod(iyf,iyb) + w*cimg::minmod(izf,izb),
                veloc = -amp*cimg::sign(itt)*cimg::abs(it);
              *(ptrd++) = veloc;
              if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
            }
            _veloc_max[c] = veloc_max;
          }
        } else // Inverse diffusion
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*512 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd = velocity.data(0,0,0,c), veloc_max = 0;
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              const Tfloat veloc = -Ipcc - Incc - Icpc - Icnc - Iccp - Iccn + 6*Iccc;
              *(ptrd++) = veloc;
              if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
            }
            _veloc_max[c] = veloc_max;
          }
      } else { // 2D
        if (sharpen_type) { // Shock filters
          CImg<Tfloat> G = (alpha>0?get_blur(alpha).get_structure_tensors():get_structure_tensors());
          if (sigma>0) G.blur(sigma);
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*32 &&
                                                         _height>=(cimg_openmp_sizefactor)*16))
          cimg_forY(G,y) {
            CImg<Tfloat> val, vec;
            Tfloat *ptrG0 = G.data(0,y,0,0), *ptrG1 = G.data(0,y,0,1), *ptrG2 = G.data(0,y,0,2);
            cimg_forX(G,x) {
              G.get_tensor_at(x,y).symmetric_eigen(val,vec);
              if (val[0]<0) val[0] = 0;
              if (val[1]<0) val[1] = 0;
              *(ptrG0++) = vec(0,0);
              *(ptrG1++) = vec(0,1);
              *(ptrG2++) = 1 - (Tfloat)std::pow(1 + val[0] + val[1],-(Tfloat)nedge);
            }
          }
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*512 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd = velocity.data(0,0,0,c), veloc_max = 0;
            CImg_3x3(I,Tfloat);
            cimg_for3x3(*this,x,y,0,c,I,Tfloat) {
              const Tfloat
                u = G(x,y,0),
                v = G(x,y,1),
                amp = G(x,y,2),
                ixx = Inc + Ipc - 2*Icc,
                ixy = (Inn + Ipp - Inp - Ipn)/4,
                iyy = Icn + Icp - 2*Icc,
                ixf = Inc - Icc,
                ixb = Icc - Ipc,
                iyf = Icn - Icc,
                iyb = Icc - Icp,
                itt = u*u*ixx + v*v*iyy + 2*u*v*ixy,
                it = u*cimg::minmod(ixf,ixb) + v*cimg::minmod(iyf,iyb),
                veloc = -amp*cimg::sign(itt)*cimg::abs(it);
              *(ptrd++) = veloc;
              if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
            }
            _veloc_max[c] = veloc_max;
          }
        } else // Inverse diffusion
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*512 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd = velocity.data(0,0,0,c), veloc_max = 0;
            CImg_3x3(I,Tfloat);
            cimg_for3x3(*this,x,y,0,c,I,Tfloat) {
              const Tfloat veloc = -Ipc - Inc - Icp - Icn + 4*Icc;
              *(ptrd++) = veloc;
              if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
            }
            _veloc_max[c] = veloc_max;
          }
      }
      const Tfloat veloc_max = _veloc_max.max();
      if (veloc_max<=0) return *this;
      return ((velocity*=amplitude/veloc_max)+=*this).cut(val_min,val_max).move_to(*this);
    }

    //! Sharpen image \newinstance.
    CImg<T> get_sharpen(const float amplitude, const bool sharpen_type=false, const float edge=1,
                        const float alpha=0, const float sigma=0) const {
      return (+*this).sharpen(amplitude,sharpen_type,edge,alpha,sigma);
    }

    //! Return image gradient.
    /**
       \param axes Axes considered for the gradient computation, as a C-string (e.g "xy").
       \param scheme = Numerical scheme used for the gradient computation:
       - -1 = Backward finite differences
       - 0 = Centered finite differences (default)
       - 1 = Forward finite differences
       - 2 = Using Sobel kernels
       - 3 = Using rotation invariant kernels
       - 4 = Using Deriche recursive filter.
       - 5 = Using Van Vliet recursive filter.
    **/
    CImgList<Tfloat> get_gradient(const char *const axes=0, const int scheme=0) const {
      CImgList<Tfloat> res;
      char __axes[4] = {};
      const char *_axes = axes?axes:__axes;
      if (!axes) {
        unsigned int k = 0;
        if (_width>1) __axes[k++] = 'x';
        if (_height>1) __axes[k++] = 'y';
        if (_depth>1) __axes[k++] = 'z';
      }

      CImg<Tfloat> grad;
      while (*_axes) {
        const char axis = cimg::lowercase(*(_axes++));
        if (axis!='x' && axis!='y' && axis!='z')
          throw CImgArgumentException(_cimg_instance
                                      "get_gradient(): Invalid specified axes '%s'.",
                                      cimg_instance,
                                      axes);
        const longT off = axis=='x'?1:axis=='y'?_width:_width*_height;
        if ((axis=='x' && _width==1) || (axis=='y' && _height==1) || (axis=='z' && _depth==1)) {
          grad.assign(_width,_height,_depth,_spectrum,0).move_to(res);
          continue;
        }

        const int _scheme = axis=='z' && (scheme==2 || scheme==3)?0:scheme;
        switch (_scheme) {
        case -1 : { // Backward finite differences
          grad.assign(_width,_height,_depth,_spectrum);
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(size(),16384))
          cimg_forXYZC(*this,x,y,z,c) {
            const ulongT pos = offset(x,y,z,c);
            if ((axis=='x' && !x) || (axis=='y' && !y) || (axis=='z' && !z))
              grad[pos] = 0;
            else
              grad[pos] = (Tfloat)_data[pos] - _data[pos - off];
          }
          grad.move_to(res);
        } break;
        case 1 : { // Forward finite differences
          grad.assign(_width,_height,_depth,_spectrum);
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(size(),16384))
          cimg_forXYZC(*this,x,y,z,c) {
            const ulongT pos = offset(x,y,z,c);
            if ((axis=='x' && x==width() - 1) || (axis=='y' && y==height() - 1) || (axis=='z' && z==depth() - 1))
              grad[pos] = 0;
            else
              grad[pos] = (Tfloat)_data[pos + off] - _data[pos];
          }
          grad.move_to(res);
        } break;
        case 2 : { // Sobel scheme
          grad.assign(_width,_height,_depth,_spectrum);
          if (axis=='x') // X-axis
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                               cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*16384 &&
                                              _depth*_spectrum>=2))
            cimg_forZC(*this,z,c) {
              CImg_3x3(I,Tfloat);
              cimg_for3x3(*this,x,y,z,c,I,Tfloat) grad(x,y,z,c) = - Ipp + Inp - 2*Ipc + 2*Inc - Ipn + Inn;
            }
          else // Y-axis
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                               cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*16384 &&
                                              _depth*_spectrum>=2))
            cimg_forZC(*this,z,c) {
              CImg_3x3(I,Tfloat);
              cimg_for3x3(*this,x,y,z,c,I,Tfloat) grad(x,y,z,c) = - Ipp - 2*Icp - Inp + Ipn + 2*Icn + Inn;
            }
          grad.move_to(res);
        } break;
        case 3 : { // Rotation invariant scheme
          const Tfloat a = (Tfloat)(0.25f*(2 - std::sqrt(2.f))), b = (Tfloat)(0.5f*(std::sqrt(2.f) - 1));
          grad.assign(_width,_height,_depth,_spectrum);
          if (axis=='x') // X-axis
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                               cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*16384 &&
                                              _depth*_spectrum>=2))
            cimg_forZC(*this,z,c) {
              CImg_3x3(I,Tfloat);
              cimg_for3x3(*this,x,y,z,c,I,Tfloat) grad(x,y,z,c) = -a*Ipp - b*Ipc - a*Ipn + a*Inp + b*Inc + a*Inn;
            }
          else // Y-axis
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                               cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*16384 &&
                                              _depth*_spectrum>=2))
            cimg_forZC(*this,z,c) {
              CImg_3x3(I,Tfloat);
              cimg_for3x3(*this,x,y,z,c,I,Tfloat) grad(x,y,z,c) = -a*Ipp - b*Icp - a*Inp + a*Ipn + b*Icn + a*Inn;
            }
          grad.move_to(res);
        } break;
        case 4 : // Deriche filter
          get_deriche(0,1,axis).move_to(res);
          break;
        case 5 : // Van Vliet filter
          get_vanvliet(0,1,axis).move_to(res);
          break;
        default : { // Central finite differences
          grad.assign(_width,_height,_depth,_spectrum);
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(size(),16384))
          cimg_forXYZC(*this,x,y,z,c) {
            const ulongT pos = offset(x,y,z,c);
            if ((axis=='x' && !x) || (axis=='y' && !y) || (axis=='z' && !z))
              grad[pos] = ((Tfloat)_data[pos + off] - _data[pos])/2;
            else if ((axis=='x' && x==width() - 1) || (axis=='y' && y==height() - 1) || (axis=='z' && z==depth() - 1))
              grad[pos] = ((Tfloat)_data[pos] - _data[pos - off])/2;
            else
              grad[pos] = ((Tfloat)_data[pos + off] - _data[pos - off])/2;
          }
          grad.move_to(res);
        } break;
        }
      }
      return res;
    }

    //! Return image hessian.
    /**
       \param axes Axes considered for the hessian computation, as a C-string (e.g "xy").
    **/
    CImgList<Tfloat> get_hessian(const char *const axes=0) const {
      CImgList<Tfloat> res;
      char __axes[12] = {};
      const char *_axes = axes?axes:__axes;
      if (!axes) {
        unsigned int k = 0;
        if (_width>1) { __axes[k++] = 'x'; __axes[k++] = 'x'; }
        if (_width>1 && _height>1) { __axes[k++] = 'x'; __axes[k++] = 'y'; }
        if (_width>1 && _depth>1) { __axes[k++] = 'x'; __axes[k++] = 'z'; }
        if (_height>1) { __axes[k++] = 'y'; __axes[k++] = 'y'; }
        if (_height>1 && _depth>1) { __axes[k++] = 'y'; __axes[k++] = 'z'; }
        if (_depth>1) { __axes[k++] = 'z'; __axes[k++] = 'z'; }
      }
      const unsigned int len = (unsigned int)std::strlen(_axes);
      if (len%2)
        throw CImgArgumentException(_cimg_instance
                                    "get_hessian(): Invalid specified axes '%s'.",
                                    cimg_instance,
                                    axes);
      CImg<Tfloat> hess;
      for (unsigned int k = 0; k<len; k+=2) {
        const char
          _axis1 = cimg::lowercase(_axes[k]),
          _axis2 = cimg::lowercase(_axes[k + 1]),
          axis1 = std::min(_axis1,_axis2),
          axis2 = std::max(_axis2,_axis2);
        if (axis1!='x' && axis1!='y' && axis1!='z' &&
            axis2!='x' && axis2!='y' && axis2!='z')
          throw CImgArgumentException(_cimg_instance
                                      "get_hessian(): Invalid specified axes '%s'.",
                                      cimg_instance,
                                      axes);
        const longT off = axis1=='x'?1:axis1=='y'?_width:_width*_height;

        hess.assign(_width,_height,_depth,_spectrum);

        if (((axis1=='x' || axis2=='x') && _width==1) ||
            ((axis1=='y' || axis2=='y') && _height==1) ||
            ((axis1=='z' || axis2=='z') && _depth==1)) {
          hess.fill(0).move_to(res);
          continue;
        }

        if (axis1==axis2) // Ixx, Iyy, Izz
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if_size(size(),16384))
          cimg_forXYZC(*this,x,y,z,c) {
            const ulongT pos = offset(x,y,z,c);
            if ((axis1=='x' && !x) || (axis1=='y' && !y) || (axis1=='z' && !z))
              hess[pos] = (Tfloat)_data[pos + off] - _data[pos];
            else if ((axis1=='x' && x==width() - 1) ||
                     (axis1=='y' && y==height() - 1) ||
                     (axis1=='z' && z==depth() - 1))
              hess[pos] = (Tfloat)_data[pos - off] - _data[pos];
            else
              hess[pos] = (Tfloat)_data[pos + off] + _data[pos - off] - 2*_data[pos];
          }
        else if (axis1=='x' && axis2=='y') // Ixy
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                             cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*16384 &&
                                            _depth*_spectrum>=2))
          cimg_forZC(*this,z,c) {
            CImg_3x3(I,Tfloat);
            cimg_for3x3(*this,x,y,z,c,I,Tfloat) hess(x,y,z,c) = (Inn + Ipp - Inp - Ipn)/4;
          }
        else if (axis1=='x' && axis2=='z') // Ixz
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*16384 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) hess(x,y,z,c) = (Incn + Ipcp - Incp - Ipcn)/4;
          }
        else // Iyz
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*16384 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) hess(x,y,z,c) = (Icnn + Icpp - Icnp - Icpn)/4;
          }
        hess.move_to(res);
      }
      return res;
    }

    //! Compute image Laplacian.
    CImg<T>& laplacian() {
      return get_laplacian().move_to(*this);
    }

    //! Compute image Laplacian \newinstance.
    CImg<Tfloat> get_laplacian() const {
      if (is_empty()) return CImg<Tfloat>();
      CImg<Tfloat> res(_width,_height,_depth,_spectrum);
      if (_depth>1) { // 3D
        cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*1048576 &&
                                                       _spectrum>=2))
        cimg_forC(*this,c) {
          Tfloat *ptrd = res.data(0,0,0,c);
          CImg_3x3x3(I,Tfloat);
          cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) *(ptrd++) = Incc + Ipcc + Icnc + Icpc + Iccn + Iccp - 6*Iccc;
        }
      } else if (_height>1) { // 2D
        cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*1048576 &&
                                                       _depth*_spectrum>=2))
        cimg_forC(*this,c) {
          Tfloat *ptrd = res.data(0,0,0,c);
          CImg_3x3(I,Tfloat);
          cimg_for3x3(*this,x,y,0,c,I,Tfloat) *(ptrd++) = Inc + Ipc + Icn + Icp - 4*Icc;
        }
      } else { // 1D
        cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*1048576 &&
                                                       _height*_depth*_spectrum>=2))
        cimg_forC(*this,c) {
          Tfloat *ptrd = res.data(0,0,0,c);
          CImg_3x3(I,Tfloat);
          cimg_for3x3(*this,x,y,0,c,I,Tfloat) *(ptrd++) = Inc + Ipc - 2*Icc;
        }
      }
      return res;
    }

    //! Compute the structure tensor field of an image.
    /**
       \param is_fwbw_scheme scheme. Can be <tt>{ false=centered | true=forward-backward }</tt>
    **/
    CImg<T>& structure_tensors(const bool is_fwbw_scheme=false) {
      return get_structure_tensors(is_fwbw_scheme).move_to(*this);
    }

    //! Compute the structure tensor field of an image \newinstance.
    CImg<Tfloat> get_structure_tensors(const bool is_fwbw_scheme=false) const {
      if (is_empty()) return *this;
      CImg<Tfloat> res;
      if (_depth>1) { // 3D
        res.assign(_width,_height,_depth,6,0);
        if (!is_fwbw_scheme) { // Classical central finite differences
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*1048576 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat
              *ptrd0 = res.data(0,0,0,0), *ptrd1 = res.data(0,0,0,1), *ptrd2 = res.data(0,0,0,2),
              *ptrd3 = res.data(0,0,0,3), *ptrd4 = res.data(0,0,0,4), *ptrd5 = res.data(0,0,0,5);
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              const Tfloat
                ix = (Incc - Ipcc)/2,
                iy = (Icnc - Icpc)/2,
                iz = (Iccn - Iccp)/2;
              cimg_pragma_openmp(atomic) *(ptrd0++)+=ix*ix;
              cimg_pragma_openmp(atomic) *(ptrd1++)+=ix*iy;
              cimg_pragma_openmp(atomic) *(ptrd2++)+=ix*iz;
              cimg_pragma_openmp(atomic) *(ptrd3++)+=iy*iy;
              cimg_pragma_openmp(atomic) *(ptrd4++)+=iy*iz;
              cimg_pragma_openmp(atomic) *(ptrd5++)+=iz*iz;
            }
          }
        } else { // Forward/backward finite differences
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*1048576 &&
                                                         _spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat
              *ptrd0 = res.data(0,0,0,0), *ptrd1 = res.data(0,0,0,1), *ptrd2 = res.data(0,0,0,2),
              *ptrd3 = res.data(0,0,0,3), *ptrd4 = res.data(0,0,0,4), *ptrd5 = res.data(0,0,0,5);
            CImg_3x3x3(I,Tfloat);
            cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) {
              const Tfloat
                ixf = Incc - Iccc, ixb = Iccc - Ipcc, ixc = (Incc - Ipcc)/2,
                iyf = Icnc - Iccc, iyb = Iccc - Icpc, iyc = (Icnc - Icpc)/2,
                izf = Iccn - Iccc, izb = Iccc - Iccp, izc = (Iccn - Iccp)/2;
              cimg_pragma_openmp(atomic) *(ptrd0++)+=(ixf*ixf + ixb*ixb)/2;
              cimg_pragma_openmp(atomic) *(ptrd1++)+=ixc*iyc;
              cimg_pragma_openmp(atomic) *(ptrd2++)+=ixc*izc;
              cimg_pragma_openmp(atomic) *(ptrd3++)+=(iyf*iyf + iyb*iyb)/2;
              cimg_pragma_openmp(atomic) *(ptrd4++)+=iyc*izc;
              cimg_pragma_openmp(atomic) *(ptrd5++)+=(izf*izf + izb*izb)/2;
            }
          }
        }
      } else { // 2D
        res.assign(_width,_height,_depth,3,0);
        if (!is_fwbw_scheme) { // Classical central finite differences
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*1048576 &&
                                                         _depth*_spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd0 = res.data(0,0,0,0), *ptrd1 = res.data(0,0,0,1), *ptrd2 = res.data(0,0,0,2);
            CImg_3x3(I,Tfloat);
            cimg_for3x3(*this,x,y,0,c,I,Tfloat) {
              const Tfloat
                ix = (Inc - Ipc)/2,
                iy = (Icn - Icp)/2;
              cimg_pragma_openmp(atomic) *(ptrd0++)+=ix*ix;
              cimg_pragma_openmp(atomic) *(ptrd1++)+=ix*iy;
              cimg_pragma_openmp(atomic) *(ptrd2++)+=iy*iy;
            }
          }
        } else { // Forward/backward finite differences (version 2)
          cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*1048576 &&
                                                         _depth*_spectrum>=2))
          cimg_forC(*this,c) {
            Tfloat *ptrd0 = res.data(0,0,0,0), *ptrd1 = res.data(0,0,0,1), *ptrd2 = res.data(0,0,0,2);
            CImg_3x3(I,Tfloat);
            cimg_for3x3(*this,x,y,0,c,I,Tfloat) {
              const Tfloat
                ixf = Inc - Icc, ixb = Icc - Ipc, ixc = (Inc - Ipc)/2,
                iyf = Icn - Icc, iyb = Icc - Icp, iyc = (Icn - Icp)/2;
              cimg_pragma_openmp(atomic) *(ptrd0++)+=(ixf*ixf + ixb*ixb)/2;
              cimg_pragma_openmp(atomic) *(ptrd1++)+=ixc*iyc;
              cimg_pragma_openmp(atomic) *(ptrd2++)+=(iyf*iyf + iyb*iyb)/2;
            }
          }
        }
      }
      return res;
    }

    //! Compute field of diffusion tensors for edge-preserving smoothing.
    /**
       \param sharpness Sharpness
       \param anisotropy Anisotropy
       \param alpha Standard deviation of the gradient blur.
       \param sigma Standard deviation of the structure tensor blur.
       \param is_sqrt Tells if the square root of the tensor field is computed instead.
    **/
    CImg<T>& diffusion_tensors(const float sharpness=0.7f, const float anisotropy=0.6f,
                               const float alpha=0.6f, const float sigma=1.1f, const bool is_sqrt=false) {
      CImg<Tfloat> res;
      const float
        nsharpness = std::max(sharpness,1e-5f),
        power1 = (is_sqrt?0.5f:1)*nsharpness,
        power2 = power1/(1e-7f + 1 - anisotropy);
      blur(alpha).normalize(0,(T)255);

      if (_depth>1) { // 3D
        get_structure_tensors().move_to(res).blur(sigma);
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth>=(cimg_openmp_sizefactor)*256))
        cimg_forYZ(*this,y,z) {
          Tfloat
            *ptrd0 = res.data(0,y,z,0), *ptrd1 = res.data(0,y,z,1), *ptrd2 = res.data(0,y,z,2),
            *ptrd3 = res.data(0,y,z,3), *ptrd4 = res.data(0,y,z,4), *ptrd5 = res.data(0,y,z,5);
          CImg<floatT> val(3), vec(3,3);
          cimg_forX(*this,x) {
            res.get_tensor_at(x,y,z).symmetric_eigen(val,vec);
            const float
              _l1 = val[2], _l2 = val[1], _l3 = val[0],
              l1 = _l1>0?_l1:0, l2 = _l2>0?_l2:0, l3 = _l3>0?_l3:0,
              ux = vec(0,0), uy = vec(0,1), uz = vec(0,2),
              vx = vec(1,0), vy = vec(1,1), vz = vec(1,2),
              wx = vec(2,0), wy = vec(2,1), wz = vec(2,2),
              n1 = (float)std::pow(1 + l1 + l2 + l3,-power1),
              n2 = (float)std::pow(1 + l1 + l2 + l3,-power2);
            *(ptrd0++) = n1*(ux*ux + vx*vx) + n2*wx*wx;
            *(ptrd1++) = n1*(ux*uy + vx*vy) + n2*wx*wy;
            *(ptrd2++) = n1*(ux*uz + vx*vz) + n2*wx*wz;
            *(ptrd3++) = n1*(uy*uy + vy*vy) + n2*wy*wy;
            *(ptrd4++) = n1*(uy*uz + vy*vz) + n2*wy*wz;
            *(ptrd5++) = n1*(uz*uz + vz*vz) + n2*wz*wz;
          }
        }
      } else { // for 2D images
        get_structure_tensors().move_to(res).blur(sigma);
        cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                       _height>=(cimg_openmp_sizefactor)*256))
        cimg_forY(*this,y) {
          Tfloat *ptrd0 = res.data(0,y,0,0), *ptrd1 = res.data(0,y,0,1), *ptrd2 = res.data(0,y,0,2);
          CImg<floatT> val(2), vec(2,2);
          cimg_forX(*this,x) {
            res.get_tensor_at(x,y).symmetric_eigen(val,vec);
            const float
              _l1 = val[1], _l2 = val[0],
              l1 = _l1>0?_l1:0, l2 = _l2>0?_l2:0,
              ux = vec(1,0), uy = vec(1,1),
              vx = vec(0,0), vy = vec(0,1),
              n1 = (float)std::pow(1 + l1 + l2,-power1),
              n2 = (float)std::pow(1 + l1 + l2,-power2);
            *(ptrd0++) = n1*ux*ux + n2*vx*vx;
            *(ptrd1++) = n1*ux*uy + n2*vx*vy;
            *(ptrd2++) = n1*uy*uy + n2*vy*vy;
          }
        }
      }
      return res.move_to(*this);
    }

    //! Compute field of diffusion tensors for edge-preserving smoothing \newinstance.
    CImg<Tfloat> get_diffusion_tensors(const float sharpness=0.7f, const float anisotropy=0.6f,
                                       const float alpha=0.6f, const float sigma=1.1f, const bool is_sqrt=false) const {
      return CImg<Tfloat>(*this,false).diffusion_tensors(sharpness,anisotropy,alpha,sigma,is_sqrt);
    }

    //! Estimate displacement field between two images.
    /**
       \param reference Reference image R.
       \param smoothness Smoothness of estimated displacement field.
       \param precision Precision required for algorithm convergence.
       \param nb_scales Number of scales used to estimate the displacement field.
       \param iteration_max Maximum number of iterations allowed for one scale.
       \param is_forward If true, optimize (I(X + U(X)) - R(X)). If false, optimize (I(X) - R(X - U(X))).
       \param guide Image used as the initial correspondence estimate for the algorithm.
       'guide' may have a last channel with boolean values (0=false | other=true) that
       tells for each pixel if its correspondence vector is constrained to its initial value (constraint mask).
    **/
    CImg<T>& displacement(const CImg<T>& reference, const float smoothness=0.1f, const float precision=7.f,
                          const unsigned int nb_scales=0, const unsigned int iteration_max=1000,
                          const bool is_forward=false,
                          const CImg<floatT>& guide=CImg<floatT>::const_empty()) {
      return get_displacement(reference,smoothness,precision,nb_scales,iteration_max,is_forward,guide).
        move_to(*this);
    }

    //! Estimate displacement field between two images \newinstance.
    CImg<floatT> get_displacement(const CImg<T>& reference,
                                  const float smoothness=0.1f, const float precision=7.f,
                                  const unsigned int nb_scales=0, const unsigned int iteration_max=1000,
                                  const bool is_forward=false,
                                  const CImg<floatT>& guide=CImg<floatT>::const_empty()) const {
      if (is_empty() || !reference) return +*this;
      if (!is_sameXYZC(reference))
        throw CImgArgumentException(_cimg_instance
                                    "displacement(): Instance and reference image (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    reference._width,reference._height,reference._depth,reference._spectrum,
                                    reference._data);
      if (precision<0)
        throw CImgArgumentException(_cimg_instance
                                    "displacement(): Invalid specified precision %g "
                                    "(should be >=0)",
                                    cimg_instance,
                                    precision);

      const bool is_3d = reference._depth>1;
      const unsigned int spectrum_U = is_3d?3:2;

      if (guide &&
          (guide._width!=_width || guide._height!=_height || guide._depth!=_depth || guide._spectrum<spectrum_U))
        throw CImgArgumentException(_cimg_instance
                                    "displacement(): Specified guide (%u,%u,%u,%u,%p) "
                                    "has invalid dimensions.",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data);
      const float
        scale_factor = 2.f,
        abs_smoothness = cimg::abs(smoothness),
        _precision = (float)std::pow(10.,-(double)precision);
      const unsigned int
        min_siz = is_3d?cimg::min(_width,_height,_depth):std::min(_width,_height),
        _nb_scales = nb_scales>0?nb_scales:(unsigned int)cimg::round(std::log(min_siz)/std::log(scale_factor)) - 1;

      CImg<floatT> U, C;  // U: vector field, C: constraints field (at current scale)
      for (int scale = (int)_nb_scales - 1; scale>=0; --scale) {
        const float
          fact = (float)std::pow(scale_factor,(double)scale);
        const unsigned int
          sw = std::max(1U,(unsigned int)cimg::round(_width/fact)),
          sh = std::max(1U,(unsigned int)cimg::round(_height/fact)),
          sd = std::max(1U,(unsigned int)cimg::round(_depth/fact));
        if (sw<4 && sh<4 && (!is_3d || sd<4)) continue; // Skip too small scales

        const float
          t = _nb_scales<2?1:(_nb_scales - 1 - scale)/(_nb_scales - 1.0f),
          omt = 1 - t,
          sigma_start = 1.25f,
          sigma_end = 0.5f,
          sigma = sigma_start*omt + sigma_end*t,
          __precision = _precision/fact;
        CImg<Tfloat>
          R = reference.get_resize(sw,sh,sd,-100,2).blur(sigma).normalize(0,1),
          I = get_resize(R,2).blur(sigma).normalize(0,1);

        if (guide._spectrum>spectrum_U) { // Guide has constraints
          guide.get_resize(I._width,I._height,I._depth,-100,2).move_to(C);
          C.get_shared_channels(0,spectrum_U - 1)/=fact;
        }

        if (U) { // Upscale U
          const float vfact = cimg::min((float)I._width/U._width,
                                        (float)I._height/U._height,
                                        is_3d?(float)I._depth/U._depth:cimg::type<float>::inf());
          // ^^ 'vfact' should be close to 'scale_factor', but slightly more precise.
          (U*=vfact).resize(I._width,I._height,I._depth,-100,3);
        }
        else { // Initialize U
          if (guide) {
            guide.get_shared_channels(0,spectrum_U - 1).get_resize(I._width,I._height,I._depth,-100,2).move_to(U);
            U/=fact;
          } else U.assign(I._width,I._height,I._depth,spectrum_U,0);
        }

        float dt = 0.25;
        double energy = cimg::type<float>::max();
        const CImgList<Tfloat> grad = is_forward?I.get_gradient():R.get_gradient();
        cimg_abort_init;

        const unsigned int _iteration_max = (unsigned int)(iteration_max*fact);
        for (unsigned int iteration = 0; iteration<_iteration_max; ++iteration) {
          cimg_abort_test;
          double _energy = 0;
          CImg<floatT> V(U._width,U._height,U._depth,U._spectrum);
          if (is_3d) { // 3D version
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                               cimg_openmp_if(_height*_depth>=(cimg_openmp_sizefactor)*8 &&
                                              _width>=(cimg_openmp_sizefactor)*16)
                               reduction(+:_energy))
            cimg_forYZ(U,y,z) {
              const int
                _p1y = y?y - 1:0, _n1y = y<U.height() - 1?y + 1:y,
                _p1z = z?z - 1:0, _n1z = z<U.depth() - 1?z + 1:z;
              cimg_for3X(U,x) {
                const float
                  X = is_forward?x + U(x,y,z,0):x - U(x,y,z,0),
                  Y = is_forward?y + U(x,y,z,1):y - U(x,y,z,1),
                  Z = is_forward?z + U(x,y,z,2):z - U(x,y,z,2);
                const bool not_constrained = C?C(x,y,z,3)==0:true;

                float veloc_u = 0, veloc_v = 0, veloc_w = 0;
                double _energy_data = 0, _energy_regul = 0;
                cimg_forC(I,c) {
                  const float delta = (float)(is_forward?R(x,y,z,c) - I._linear_atXYZ(X,Y,Z,c):
                                              R._linear_atXYZ(X,Y,Z,c) - I(x,y,z,c));
                  veloc_u+=delta*grad[0].linear_atXYZ(X,Y,Z,c,0);
                  veloc_v+=delta*grad[1].linear_atXYZ(X,Y,Z,c,0);
                  veloc_w+=delta*grad[2].linear_atXYZ(X,Y,Z,c,0);
                  _energy_data+=delta*delta;
                }

                if (smoothness==0) { // No regularization
                  V(x,y,z,0) = veloc_u;
                  V(x,y,z,1) = veloc_v;
                  V(x,y,z,2) = veloc_w;
                } else if (smoothness>=0) cimg_forC(U,c) { // Isotropic regularization
                    const float
                      uccc = U(x,y,z,c),
                      upcc = U(_p1x,y,z,c), uncc = U(_n1x,y,z,c),
                      ucpc = U(x,_p1y,z,c), ucnc = U(x,_n1y,z,c),
                      uccp = U(x,y,_p1z,c), uccn = U(x,y,_n1z,c),
                      ux = 0.5f*(uncc - upcc), uy = 0.5f*(ucnc - ucpc), uz = 0.5f*(uccn - uccp),
                      regul = uncc + upcc + ucnc + ucpc + uccn + uccp - 6*uccc,
                      veloc = c==0?veloc_u:c==1?veloc_v:veloc_w;
                    V(x,y,z,c) = veloc + smoothness*regul;
                    _energy_regul+=ux*ux + uy*uy + uz*uz;

                  } else cimg_forC(U,c) { // TV regularization
                    CImg_3x3x3(u,float);
                    cimg_get3x3x3(U,x,y,z,c,u,float);
                    const float
                      ux = 0.5f*(uncc - upcc), uy = 0.5f*(ucnc - ucpc), uz = 0.5f*(uccn - uccp),
                      N2 = ux*ux + uy*uy + uz*uz,
                      N = std::sqrt(N2),
                      N3 = 1e-5f + N2*N,
                      coef_a = (uy*uy + uz*uz)/N3,
                      coef_b = -2*ux*uy/N3,
                      coef_c = -2*ux*uz/N3,
                      coef_d = (ux*ux + uz*uz)/N3,
                      coef_e = -2*uy*uz/N3,
                      coef_f = (ux*ux + uy*uy)/N3,
                      uxx = uncc + upcc - 2*uccc,
                      uyy = ucnc + ucpc - 2*uccc,
                      uzz = uccn + uccp - 2*uccc,
                      uxy = 0.25f*(unnc + uppc - unpc - upnc),
                      uxz = 0.25f*(uncn + upcp - uncp - upcn),
                      uyz = 0.25f*(ucnn + ucpp - ucnp - ucpn),
                      regul = coef_a*uxx + coef_b*uxy + coef_c*uxz + coef_d*uyy + coef_e*uyz + coef_f*uzz,
                      veloc = c==0?veloc_u:c==1?veloc_v:veloc_w;
                    V(x,y,z,c) = veloc + abs_smoothness*regul;
                    _energy_regul+=N;
                  }
                if (not_constrained) _energy+=_energy_data + abs_smoothness*_energy_regul;
              }
            }

            // Update displacement field.
            float Vmin,Vmax = V.max_min(Vmin);
            const float _dt = dt/cimg::max(1e-8f,cimg::abs(Vmin),cimg::abs(Vmax));
            cimg_openmp_for(U,*ptr + _dt*V[ptr - U._data],32768,float);

            if (C) // Apply constraints
              cimg_forXYZ(C,x,y,z) {
                const float m = C(x,y,z,3), om = 1 - m;
                if (m>1e-5f) {
                  U(x,y,z,0) = m*C(x,y,z,0) + om*U(x,y,z,0);
                  U(x,y,z,1) = m*C(x,y,z,1) + om*U(x,y,z,1);
                  U(x,y,z,2) = m*C(x,y,z,2) + om*U(x,y,z,2);
                }
              }

          } else { // 2D version
            cimg_pragma_openmp(parallel for cimg_openmp_if(_height>=(cimg_openmp_sizefactor)*8 &&
                                                           _width>=(cimg_openmp_sizefactor)*16) reduction(+:_energy))
            cimg_forY(U,y) {
              const int _p1y = y?y - 1:0, _n1y = y<U.height() - 1?y + 1:y;
              cimg_for3X(U,x) {
                const float
                  X = is_forward?x + U(x,y,0):x - U(x,y,0),
                  Y = is_forward?y + U(x,y,1):y - U(x,y,1);
                const bool not_constrained = C?C(x,y,2)==0:true;
                float veloc_u = 0, veloc_v = 0;
                double _energy_data = 0, _energy_regul = 0;
#endif
