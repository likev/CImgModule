#ifndef CIMG_MODULE_IMAGE_BODY_FILTERS_1_H
#define CIMG_MODULE_IMAGE_BODY_FILTERS_1_H

    //---------------------------------------
    //
    //! \name Filtering / Transforms
    //@{
    //---------------------------------------

    //! Correlate image by a kernel.
    /**
       \param kernel = the correlation kernel.
       \param boundary_conditions Boundary condition. Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
       \param is_normalized = enable local normalization.
       \param channel_mode Channel processing mode.
                           Can be { 0=all | 1=one for one (default) | 2=partial sum | 3=full sum }.
       \param xcenter X-coordinate of the kernel center (~0U>>1 means 'centered').
       \param ycenter Y-coordinate of the kernel center (~0U>>1 means 'centered').
       \param zcenter Z-coordinate of the kernel center (~0U>>1 means 'centered').
       \param xstride Stride along the X-axis.
       \param ystride Stride along the Y-axis.
       \param zstride Stride along the Z-axis.
       \param xdilation Dilation along the X-axis.
       \param ydilation Dilation along the Y-axis.
       \param zdilation Dilation along the Z-axis.
       \param xoffset X-offset.
       \param yoffset Y-offset.
       \param zoffset Z-offset.
       \param xsize Width of the resulting image (~0U means 'instance_width/xstride').
       \param ysize Height of the resulting image (~0U means 'instance_height/ystride').
       \param zsize Depth of the resulting image (~0U means 'instance_depth/zstride').
       \note
       - The correlation of the image instance \p *this by the kernel \p kernel is defined to be:
       res(x,y,z) = sum_{i,j,k} (*this)(\alpha_x\;x + \beta_x\;(i - c_x),\alpha_y\;y + \beta_y\;(j -
                    c_y),\alpha_z\;z + \beta_z\;(k - c_z))*kernel(i,j,k).
    **/
    template<typename t>
    CImg<T>& correlate(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                       const bool is_normalized=false, const unsigned int channel_mode=1,
                       const int xcenter=(int)(~0U>>1),
                       const int ycenter=(int)(~0U>>1),
                       const int zcenter=(int)(~0U>>1),
                       const unsigned int xstride=1,
                       const unsigned int ystride=1,
                       const unsigned int zstride=1,
                       const int xdilation=1, const int ydilation=1, const int zdilation=1,
                       const int xoffset=0, const int yoffset=0, const int zoffset=0,
                       const unsigned int xsize=~0U,
                       const unsigned int ysize=~0U,
                       const unsigned int zsize=~0U) {
      if (is_empty() || !kernel) return *this;
      return get_correlate(kernel,boundary_conditions,is_normalized,channel_mode,
                           xcenter,ycenter,zcenter,xstride,ystride,zstride,xdilation,ydilation,zdilation,
                           xoffset,yoffset,zoffset,xsize,ysize,zsize).move_to(*this);
    }

    template<typename t>
    CImg<_cimg_Ttfloat> get_correlate(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                                      const bool is_normalized=false, const unsigned int channel_mode=1,
                                      const int xcenter=(int)(~0U>>1),
                                      const int ycenter=(int)(~0U>>1),
                                      const int zcenter=(int)(~0U>>1),
                                      const unsigned int xstride=1,
                                      const unsigned int ystride=1,
                                      const unsigned int zstride=1,
                                      const int xdilation=1, const int ydilation=1, const int zdilation=1,
                                      const int xoffset=0, const int yoffset=0, const int zoffset=0,
                                      const unsigned int xsize=~0U,
                                      const unsigned int ysize=~0U,
                                      const unsigned int zsize=~0U) const {
      return _correlate(kernel,boundary_conditions,is_normalized,channel_mode,
                        xcenter,ycenter,zcenter,xstride,ystride,zstride,xdilation,ydilation,zdilation,
                        xoffset,yoffset,zoffset,xsize,ysize,zsize,false);
    }

    //! Correlate image by a kernel \newinstance.
    template<typename t>
    CImg<_cimg_Ttfloat> _correlate(const CImg<t>& kernel, const unsigned int boundary_conditions,
                                   const bool is_normalized, const unsigned int channel_mode,
                                   const int xcenter, const int ycenter, const int zcenter,
                                   const unsigned int xstride,
                                   const unsigned int ystride,
                                   const unsigned int zstride,
                                   const int xdilation, const int ydilation, const int zdilation,
                                   const int xoffset, const int yoffset, const int zoffset,
                                   const unsigned int xsize, const unsigned int ysize, const unsigned int zsize,
                                   const bool is_convolve) const {
      typedef _cimg_Ttfloat Ttfloat;
      CImg<Ttfloat> res;
      _cimg_abort_init_openmp;
      cimg_abort_init;
      if (is_empty() || !kernel) return *this;

      const unsigned int
        _xsize = xsize==~0U?_width/xstride:xsize,
        _ysize = ysize==~0U?_height/ystride:ysize,
        _zsize = zsize==~0U?_depth/zstride:zsize;
      const ulongT
        res_wh = (ulongT)_xsize*_ysize,
        res_whd = (ulongT)_xsize*_ysize*_zsize;
      if (!xsize || !ysize || !zsize) return CImg<Ttfloat>();

      int
        _xcenter = xcenter==(int)(~0U>>1)?kernel.width()/2 - 1 + (kernel.width()%2):xcenter,
        _ycenter = ycenter==(int)(~0U>>1)?kernel.height()/2 - 1 + (kernel.height()%2):ycenter,
        _zcenter = zcenter==(int)(~0U>>1)?kernel.depth()/2 - 1 + (kernel.depth()%2):zcenter,
        _xdilation = xdilation, _ydilation = ydilation, _zdilation = zdilation;

      CImg<t> _kernel;
      if (is_convolve) { // If convolution, go back to correlation
        if (kernel.size()/kernel.spectrum()<=27) {
          _kernel = CImg<t>(kernel._data,kernel.size()/kernel._spectrum,1,1,kernel._spectrum,true).
            get_mirror('x').resize(kernel,-1);
          _xcenter = kernel.width() - 1 - _xcenter;
          _ycenter = kernel.height() - 1 - _ycenter;
          _zcenter = kernel.depth() - 1 - _zcenter;
        } else { _kernel = kernel.get_shared(); _xdilation*=-1; _ydilation*=-1; _zdilation*=-1; }
      } else _kernel = kernel.get_shared();

      const int
        smin = std::min(spectrum(),_kernel.spectrum()),
        smax = std::max(spectrum(),_kernel.spectrum()),
        cend = !channel_mode?spectrum()*_kernel.spectrum():smax;

      res.assign(_xsize,_ysize,_zsize,
                 !channel_mode?_spectrum*_kernel._spectrum:
                 channel_mode==1?smax:
                 channel_mode==2?(int)std::ceil((float)smax/smin):1);
      const ulongT res_siz = res_whd*res._spectrum;
      if (channel_mode>=2) res.fill(0);

      const bool
#if cimg_use_openmp==1
        is_master_thread = !omp_get_thread_num(),
#else
        is_master_thread = true,
#endif
        is_outer_parallel = is_master_thread &&
        (res._spectrum>=cimg::nb_cpus() || (res_siz<=(cimg_openmp_sizefactor)*32768 && res._spectrum>1)),
        is_inner_parallel = is_master_thread &&
        (!is_outer_parallel && res_whd>=(cimg_openmp_sizefactor)*32768);
      cimg::unused(is_inner_parallel,is_outer_parallel);
      const int
        w = width(), h = height(), d = depth(),
        w1 = w  - 1, h1 = h - 1, d1 = d - 1,
        w2 = 2*w, h2 = 2*h, d2 = 2*d;
      const ulongT wh = (ulongT)w*h, whd = wh*d;

      // Reshape kernel to enable optimizations for a few cases.
      if (boundary_conditions==1 &&
          _kernel._width>1 && _kernel._height>1 &&
          ((_kernel._depth==1 && _kernel._width<=5 && _kernel._height<=5) ||
           (_kernel._depth<=3 && _kernel._width<=3 && _kernel._height<=3)) &&
          xstride==1 && ystride==1 && zstride==1 &&
          xoffset>=0 && yoffset>=0 && zoffset>=0 &&
          xoffset + _xsize<=_width && yoffset + _ysize<=_height && zoffset + _zsize<=_depth) {
        const unsigned int M = cimg::max(_kernel._width,_kernel._height,_kernel._depth);
        _kernel.assign(_kernel.get_resize(M + 1 - (M%2),M + 1 - (M%2),_kernel._depth>1?M + 1 - (M%2):1,-100,
                                          0,0,1,1,1),false);
        _xcenter = _ycenter = (int)M/2;
        if (_kernel._depth>1) _zcenter = (int)M/2;
      }

      // Optimized version for a few particular cases (3x3, 5x5 and 3x3x3 kernels, with a few other conditions).
      if (boundary_conditions==1 &&
          _kernel._width==_kernel._height &&
          ((_kernel._depth==1 && (_kernel._width==3 || _kernel._width==5)) ||
           (_kernel._depth==_kernel._width && _kernel._width==3)) &&
          _xcenter==_kernel.width()/2 && _ycenter==_kernel.height()/2 && _zcenter==_kernel.depth()/2 &&
          xstride==1 && ystride==1 && zstride==1 &&
          xoffset>=0 && yoffset>=0 && zoffset>=0 &&
          xoffset + _xsize<=_width && yoffset + _ysize<=_height && zoffset + _zsize<=_depth) {

        switch (_kernel._depth) {
        case 3 : { // 3x3x3 centered kernel
          cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
          for (int c = 0; c<cend; ++c) _cimg_abort_try_openmp2 {
            cimg_abort_test2;
            const CImg<T> I = get_shared_channel(c%_spectrum);
            const CImg<t> K = _kernel.get_shared_channel(!channel_mode?c/_spectrum:c%_kernel._spectrum);
            CImg<Ttfloat> _resu = channel_mode<=1?res.get_shared_channel(c):
              CImg<Ttfloat>(res.width(),res.height(),res.depth(),1);
            if (is_normalized) {
              const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
              cimg_forXYZ(res,X,Y,Z) {
                const int
                  x = xoffset + X, y = yoffset + Y, z = zoffset + Z,
                  px = x - xdilation>0?x - xdilation:0, nx = x + xdilation<w1?x + xdilation:w1,
                  py = y - ydilation>0?y - ydilation:0, ny = y + ydilation<h1?y + ydilation:h1,
                  pz = z - zdilation>0?z - zdilation:0, nz = z + zdilation<d1?z + zdilation:d1;
                const Ttfloat N = M2*(cimg::sqr(I(px,py,pz)) + cimg::sqr(I(x,py,pz)) + cimg::sqr(I(nx,py,pz)) +
                                      cimg::sqr(I(px,y,pz)) + cimg::sqr(I(x,y,pz)) + cimg::sqr(I(nx,y,pz)) +
                                      cimg::sqr(I(px,ny,pz)) + cimg::sqr(I(x,ny,pz)) + cimg::sqr(I(nx,ny,pz)) +
                                      cimg::sqr(I(px,py,z)) + cimg::sqr(I(x,py,z)) + cimg::sqr(I(nx,py,z)) +
                                      cimg::sqr(I(px,y,z)) + cimg::sqr(I(x,y,z)) + cimg::sqr(I(nx,y,z)) +
                                      cimg::sqr(I(px,ny,z)) + cimg::sqr(I(x,ny,z)) + cimg::sqr(I(nx,ny,z)) +
                                      cimg::sqr(I(px,py,nz)) + cimg::sqr(I(x,py,nz)) + cimg::sqr(I(nx,py,nz)) +
                                      cimg::sqr(I(px,y,nz)) + cimg::sqr(I(x,y,nz)) + cimg::sqr(I(nx,y,nz)) +
                                      cimg::sqr(I(px,ny,nz)) + cimg::sqr(I(x,ny,nz)) + cimg::sqr(I(nx,ny,nz)));
                _resu(X,Y,Z) = (Ttfloat)(N?(K[0]*I(px,py,pz) + K[1]*I(x,py,pz) + K[2]*I(nx,py,pz) +
                                            K[3]*I(px,y,pz) + K[4]*I(x,y,pz) + K[5]*I(nx,y,pz) +
                                            K[6]*I(px,ny,pz) + K[7]*I(x,ny,pz) + K[8]*I(nx,ny,pz) +
                                            K[9]*I(px,py,z) + K[10]*I(x,py,z) + K[11]*I(nx,py,z) +
                                            K[12]*I(px,y,z) + K[13]*I(x,y,z) + K[14]*I(nx,y,z) +
                                            K[15]*I(px,ny,z) + K[16]*I(x,ny,z) + K[17]*I(nx,ny,z) +
                                            K[18]*I(px,py,nz) + K[19]*I(x,py,nz) + K[20]*I(nx,py,nz) +
                                            K[21]*I(px,y,nz) + K[22]*I(x,y,nz) + K[23]*I(nx,y,nz) +
                                            K[24]*I(px,ny,nz) + K[25]*I(x,ny,nz) + K[26]*I(nx,ny,nz))/std::sqrt(N):0);
              }
            } else {
              cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
              cimg_forXYZ(res,X,Y,Z) {
                const int
                  x = xoffset + X, y = yoffset + Y, z = zoffset + Z,
                  px = x - xdilation>0?x - xdilation:0, nx = x + xdilation<w1?x + xdilation:w1,
                  py = y - ydilation>0?y - ydilation:0, ny = y + ydilation<h1?y + ydilation:h1,
                  pz = z - zdilation>0?z - zdilation:0, nz = z + zdilation<d1?z + zdilation:d1;
                _resu(X,Y,Z) = (Ttfloat)(K[0]*I(px,py,pz) + K[1]*I(x,py,pz) + K[2]*I(nx,py,pz) +
                                         K[3]*I(px,y,pz) + K[4]*I(x,y,pz) + K[5]*I(nx,y,pz) +
                                         K[6]*I(px,ny,pz) + K[7]*I(x,ny,pz) + K[8]*I(nx,ny,pz) +
                                         K[9]*I(px,py,z) + K[10]*I(x,py,z) + K[11]*I(nx,py,z) +
                                         K[12]*I(px,y,z) + K[13]*I(x,y,z) + K[14]*I(nx,y,z) +
                                         K[15]*I(px,ny,z) + K[16]*I(x,ny,z) + K[17]*I(nx,ny,z) +
                                         K[18]*I(px,py,nz) + K[19]*I(x,py,nz) + K[20]*I(nx,py,nz) +
                                         K[21]*I(px,y,nz) + K[22]*I(x,y,nz) + K[23]*I(nx,y,nz) +
                                         K[24]*I(px,ny,nz) + K[25]*I(x,ny,nz) + K[26]*I(nx,ny,nz));
              }
            }
            if (channel_mode==2)
              cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(c/smin)+=_resu;
            else if (channel_mode==3)
              cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(0)+=_resu;
          } _cimg_abort_catch_openmp2
        } break;

        default :
        case 1 :
          switch (_kernel._width) {
          case 5 : { // 5x5 centered kernel
            cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
            for (int c = 0; c<cend; ++c) _cimg_abort_try_openmp2 {
              cimg_abort_test2;
              const CImg<T> I = get_shared_channel(c%_spectrum);
              const CImg<t> K = _kernel.get_shared_channel(!channel_mode?c/_spectrum:c%_kernel._spectrum);
              CImg<Ttfloat> _resu = channel_mode<=1?res.get_shared_channel(c):
                CImg<Ttfloat>(res.width(),res.height(),res.depth(),1);
              if (is_normalized) {
                const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
                cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
                cimg_forXYZ(res,X,Y,z) {
                  const int
                    x = xoffset + X, y = yoffset + Y,
                    px = x - xdilation>0?x - xdilation:0, bx = px - xdilation>0?px - xdilation:0,
                    nx = x + xdilation<w1?x + xdilation:w1, ax = nx + xdilation<w1?nx + xdilation:w1,
                    py = y - ydilation>0?y - ydilation:0, by = py - ydilation>0?py - ydilation:0,
                    ny = y + ydilation<h1?y + ydilation:h1, ay = ny + ydilation<h1?ny + ydilation:h1;
                  const Ttfloat N = M2*(cimg::sqr(I(bx,by,z)) + cimg::sqr(I(px,by,z)) + cimg::sqr(I(x,by,z)) +
                                        cimg::sqr(I(nx,by,z)) + cimg::sqr(I(ax,by,z)) +
                                        cimg::sqr(I(bx,py,z)) + cimg::sqr(I(px,py,z)) + cimg::sqr(I(x,py,z)) +
                                        cimg::sqr(I(nx,py,z)) + cimg::sqr(I(ax,py,z)) +
                                        cimg::sqr(I(bx,y,z)) + cimg::sqr(I(px,y,z)) + cimg::sqr(I(x,y,z)) +
                                        cimg::sqr(I(nx,y,z)) + cimg::sqr(I(ax,y,z)) +
                                        cimg::sqr(I(bx,ny,z)) + cimg::sqr(I(px,ny,z)) + cimg::sqr(I(x,ny,z)) +
                                        cimg::sqr(I(nx,ny,z)) + cimg::sqr(I(ax,ny,z)) +
                                        cimg::sqr(I(bx,ay,z)) + cimg::sqr(I(px,ay,z)) + cimg::sqr(I(x,ay,z)) +
                                        cimg::sqr(I(nx,ay,z)) + cimg::sqr(I(ax,ay,z)));
                  _resu(X,Y,z) = (Ttfloat)(N?(K[0]*I(bx,by,z) + K[1]*I(px,by,z) + K[2]*I(x,by,z) +
                                              K[3]*I(nx,by,z) + K[4]*I(ax,by,z) +
                                              K[5]*I(bx,py,z) + K[6]*I(px,py,z) + K[7]*I(x,py,z) +
                                              K[8]*I(nx,py,z) + K[9]*I(ax,py,z) +
                                              K[10]*I(bx,y,z) + K[11]*I(px,y,z) + K[12]*I(x,y,z) +
                                              K[13]*I(nx,y,z) + K[14]*I(ax,y,z) +
                                              K[15]*I(bx,ny,z) + K[16]*I(px,ny,z) + K[17]*I(x,ny,z) +
                                              K[18]*I(nx,ny,z) + K[19]*I(ax,ny,z) +
                                              K[20]*I(bx,ay,z) + K[21]*I(px,ay,z) + K[22]*I(x,ay,z) +
                                              K[23]*I(nx,ay,z) + K[24]*I(ax,ay,z))/std::sqrt(N):0);
                }
              } else {
                cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
                cimg_forXYZ(res,X,Y,z) {
                  const int
                    x = xoffset + X, y = yoffset + Y,
                    px = x - xdilation>0?x - xdilation:0, bx = px - xdilation>0?px - xdilation:0,
                    nx = x + xdilation<w1?x + xdilation:w1, ax = nx + xdilation<w1?nx + xdilation:w1,
                    py = y - ydilation>0?y - ydilation:0, by = py - ydilation>0?py - ydilation:0,
                    ny = y + ydilation<h1?y + ydilation:h1, ay = ny + ydilation<h1?ny + ydilation:h1;
                  _resu(X,Y,z) = (Ttfloat)(K[0]*I(bx,by,z) + K[1]*I(px,by,z) + K[2]*I(x,by,z) +
                                           K[3]*I(nx,by,z) + K[4]*I(ax,by,z) +
                                           K[5]*I(bx,py,z) + K[6]*I(px,py,z) + K[7]*I(x,py,z) +
                                           K[8]*I(nx,py,z) + K[9]*I(ax,py,z) +
                                           K[10]*I(bx,y,z) + K[11]*I(px,y,z) + K[12]*I(x,y,z) +
                                           K[13]*I(nx,y,z) + K[14]*I(ax,y,z) +
                                           K[15]*I(bx,ny,z) + K[16]*I(px,ny,z) + K[17]*I(x,ny,z) +
                                           K[18]*I(nx,ny,z) + K[19]*I(ax,ny,z) +
                                           K[20]*I(bx,ay,z) + K[21]*I(px,ay,z) + K[22]*I(x,ay,z) +
                                           K[23]*I(nx,ay,z) + K[24]*I(ax,ay,z));
                }
              }
            if (channel_mode==2)
              cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(c/smin)+=_resu;
            else if (channel_mode==3)
              cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(0)+=_resu;
            } _cimg_abort_catch_openmp2
          } break;

          case 3 : { // 3x3 centered kernel
            cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
            for (int c = 0; c<cend; ++c) _cimg_abort_try_openmp2 {
              cimg_abort_test2;
              const CImg<T> I = get_shared_channel(c%_spectrum);
              const CImg<t> K = _kernel.get_shared_channel(!channel_mode?c/_spectrum:c%_kernel._spectrum);
              CImg<Ttfloat> _resu = channel_mode<=1?res.get_shared_channel(c):
                CImg<Ttfloat>(res.width(),res.height(),res.depth(),1);
              if (is_normalized) {
                const Ttfloat M = (Ttfloat)K.magnitude(2), M2 = M*M;
                cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
                cimg_forXYZ(res,X,Y,z) {
                  const int
                    x = xoffset + X, y = yoffset + Y,
                    px = x - xdilation>0?x - xdilation:0, nx = x + xdilation<w1?x + xdilation:w1,
                    py = y - ydilation>0?y - ydilation:0, ny = y + ydilation<h1?y + ydilation:h1;
                  const Ttfloat N = M2*(cimg::sqr(I(px,py,z)) + cimg::sqr(I(x,py,z)) + cimg::sqr(I(nx,py,z)) +
                                        cimg::sqr(I(px,y,z)) + cimg::sqr(I(x,y,z)) + cimg::sqr(I(nx,y,z)) +
                                        cimg::sqr(I(px,ny,z)) + cimg::sqr(I(x,ny,z)) + cimg::sqr(I(nx,ny,z)));
                  _resu(X,Y,z) = (Ttfloat)(N?(K[0]*I(px,py,z) + K[1]*I(x,py,z) + K[2]*I(nx,py,z) +
                                              K[3]*I(px,y,z) + K[4]*I(x,y,z) + K[5]*I(nx,y,z) +
                                              K[6]*I(px,ny,z) + K[7]*I(x,ny,z) + K[8]*I(nx,ny,z))/std::sqrt(N):0);
                }
              } else {
                cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
                cimg_forXYZ(res,X,Y,z) {
                  const int
                    x = xoffset + X, y = yoffset + Y,
                    px = x - xdilation>0?x - xdilation:0, nx = x + xdilation<w1?x + xdilation:w1,
                    py = y - ydilation>0?y - ydilation:0, ny = y + ydilation<h1?y + ydilation:h1;
                  _resu(X,Y,z) = (Ttfloat)(K[0]*I(px,py,z) + K[1]*I(x,py,z) + K[2]*I(nx,py,z) +
                                           K[3]*I(px,y,z)  + K[4]*I(x,y,z)  + K[5]*I(nx,y,z) +
                                           K[6]*I(px,ny,z) + K[7]*I(x,ny,z) + K[8]*I(nx,ny,z));
                }
              }
            if (channel_mode==2)
              cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(c/smin)+=_resu;
            else if (channel_mode==3)
              cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(0)+=_resu;
            } _cimg_abort_catch_openmp2
          } break;
          }
        }
      } else if (_kernel._width==1 && _kernel._height==1 && _kernel._depth==1 &&
                 !_xcenter && !_ycenter && !_zcenter &&
                 xstride==1 && ystride==1 && zstride==1 &&
                 xoffset>=0 && yoffset>=0 && zoffset>=0 &&
                 xoffset + _xsize<=_width && yoffset + _ysize<=_height && zoffset + _zsize<=_depth) {

        // Special optimization for 1x1 kernel.
        cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
        for (int c = 0; c<cend; ++c) {
          const t valK = _kernel[!channel_mode?c/_spectrum:c%_kernel._spectrum];
          CImg<T> I = get_crop(xoffset,yoffset,zoffset,c%_spectrum,
                               xoffset + _xsize - 1,yoffset + _ysize - 1,zoffset + _zsize - 1,c%_spectrum);
          if (valK!=1) I*=valK;
          if (is_normalized) I.sign();
          switch (channel_mode) {
          case 0 : // All
          case 1 : // One for one
            res.get_shared_channel(c) = I;
            break;
          case 2 : // Partial sum
            cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(c/smin)+=I;
            break;
          case 3 : // Full sum
            cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(0)+=I;
            break;
          }
        }
      } else { // Generic version
        cimg_pragma_openmp(parallel for cimg_openmp_if(is_outer_parallel))
        for (int c = 0; c<cend; ++c) _cimg_abort_try_openmp2 {
          cimg_abort_test2;
          const CImg<T> I = get_shared_channel(c%_spectrum);
          const CImg<t> K = _kernel.get_shared_channel(!channel_mode?c/_spectrum:c%_kernel._spectrum);
          CImg<Ttfloat> _resu = channel_mode<=1?res.get_shared_channel(c):
            CImg<Ttfloat>(res.width(),res.height(),res.depth(),1);
          Ttfloat M = 0, M2 = 0;
          if (is_normalized) { M = (Ttfloat)K.magnitude(2); M2 = cimg::sqr(M); }

#define _cimg_correlate_x const int ix = xstride*x + xoffset + _xdilation*(p - _xcenter)
#define _cimg_correlate_y const int iy = ystride*y + yoffset + _ydilation*(q - _ycenter)
#define _cimg_correlate_z const int iz = zstride*z + zoffset + _zdilation*(r - _zcenter)

#define _cimg_correlate_x_dirichlet const bool is_in_x = ix>=0 && ix<w
#define _cimg_correlate_y_dirichlet const bool is_in_y = iy>=0 && iy<h
#define _cimg_correlate_z_dirichlet const bool is_in_z = iz>=0 && iz<d

#define _cimg_correlate_x_neumann const int nix = cimg::cut(ix,0,w1)
#define _cimg_correlate_y_neumann const int niy = cimg::cut(iy,0,h1)
#define _cimg_correlate_z_neumann const int niz = cimg::cut(iz,0,d1)

#define _cimg_correlate_x_periodic const int nix = cimg::mod(ix,w)
#define _cimg_correlate_y_periodic const int niy = cimg::mod(iy,h)
#define _cimg_correlate_z_periodic const int niz = cimg::mod(iz,d)

#define _cimg_correlate_x_mirror const int mx = cimg::mod(ix,w2), nix = mx<w?mx:w2 - mx - 1
#define _cimg_correlate_y_mirror const int my = cimg::mod(iy,h2), niy = my<h?my:h2 - my - 1
#define _cimg_correlate_z_mirror const int mz = cimg::mod(iz,d2), niz = mz<d?mz:d2 - mz - 1

#define _cimg_correlate(boundary,access) \
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel)) \
          cimg_forXYZ(res,x,y,z) { \
            Ttfloat val = 0; \
            const t *pK = K._data; \
            cimg_forZ(_kernel,r) { _cimg_correlate_z; _cimg_correlate_z_##boundary; \
              cimg_forY(_kernel,q) { _cimg_correlate_y; _cimg_correlate_y_##boundary; \
                cimg_forX(_kernel,p) { _cimg_correlate_x; _cimg_correlate_x_##boundary; \
                  val+=*(pK++)*(access); \
                } \
              } \
            } \
            _resu(x,y,z,0,res_wh,res_whd) = val; \
          }

#define _cimg_correlate_n(boundary,access) \
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel)) \
          cimg_forXYZ(res,x,y,z) { \
            Ttfloat val = 0, N = 0; \
            const t *pK = K._data; \
            cimg_forZ(_kernel,r) { _cimg_correlate_z; _cimg_correlate_z_##boundary; \
              cimg_forY(_kernel,q) { _cimg_correlate_y; _cimg_correlate_y_##boundary; \
                cimg_forX(_kernel,p) { _cimg_correlate_x; _cimg_correlate_x_##boundary; \
                  Ttfloat _val = access; \
                  val+=*(pK++)*_val; \
                  _val*=_val; N+=_val; \
                } \
              } \
            } \
            N*=M2; _resu(x,y,z,0,res_wh,res_whd) = N?val/std::sqrt(N):0; \
          }

          if (is_normalized) { // Normalized convolution/correlation
            switch (boundary_conditions) {
            case 0 : // Dirichlet
              _cimg_correlate_n(dirichlet,is_in_x && is_in_y && is_in_z?I(ix,iy,iz,0,wh,whd):(T)0);
              break;
            case 1 : // Neumann
              _cimg_correlate_n(neumann,I(nix,niy,niz,0,wh,whd));
              break;
            case 2 : // Periodic
              _cimg_correlate_n(periodic,I(nix,niy,niz,0,wh,whd));
              break;
            case 3 : // Mirror
              _cimg_correlate_n(mirror,I(nix,niy,niz,0,wh,whd));
              break;
            }
          } else { // Standard convolution/correlation
            switch (boundary_conditions) {
            case 0 : // Dirichlet
              _cimg_correlate(dirichlet,is_in_x && is_in_y && is_in_z?I(ix,iy,iz,0,wh,whd):(T)0);
              break;
            case 1 : // Neumann
              _cimg_correlate(neumann,I(nix,niy,niz,0,wh,whd));
              break;
            case 2 : // Periodic
              _cimg_correlate(periodic,I(nix,niy,niz,0,wh,whd));
              break;
            case 3 : // Mirror
              _cimg_correlate(mirror,I(nix,niy,niz,0,wh,whd));
              break;
            }
          }
          if (channel_mode==2)
            cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(c/smin)+=_resu;
          else if (channel_mode==3)
            cimg_pragma_openmp(critical(_correlate)) res.get_shared_channel(0)+=_resu;

        } _cimg_abort_catch_openmp2
      }
      cimg_abort_test;
      return res;
    }

    //! Convolve image by a kernel.
    /**
       \param kernel = the correlation kernel.
       \param boundary_conditions Boundary condition. Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
       \param is_normalized = enable local normalization.
       \param channel_mode Channel processing mode.
                           Can be { 0=all | 1=one for one (default) | 2=partial sum | 3=full sum }.
       \param xcenter X-coordinate of the kernel center (~0U>>1 means 'centered').
       \param ycenter Y-coordinate of the kernel center (~0U>>1 means 'centered').
       \param zcenter Z-coordinate of the kernel center (~0U>>1 means 'centered').
       \param xstride Stride along the X-axis.
       \param ystride Stride along the Y-axis.
       \param zstride Stride along the Z-axis.
       \param xdilation Dilation along the X-axis.
       \param ydilation Dilation along the Y-axis.
       \param zdilation Dilation along the Z-axis.
       \param xoffset X-offset.
       \param yoffset Y-offset.
       \param zoffset Z-offset.
       \param xsize Width of the resulting image (~0U means 'instance_width/xstride').
       \param ysize Height of the resulting image (~0U means 'instance_height/ystride').
       \param zsize Depth of the resulting image (~0U means 'instance_depth/zstride').
       \note
       - The convolution of the image instance \p *this by the kernel \p kernel is defined to be:
       res(x,y,z) = sum_{i,j,k} (*this)(\alpha_x\;x - \beta_x\;(i - c_x),\alpha_y\;y
                    - \beta_y\;(j - c_y),\alpha_z\;z - \beta_z\;(k - c_z))*kernel(i,j,k).
    **/
    template<typename t>
    CImg<T>& convolve(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                      const bool is_normalized=false, const unsigned int channel_mode=1,
                      const int xcenter=(int)(~0U>>1),
                      const int ycenter=(int)(~0U>>1),
                      const int zcenter=(int)(~0U>>1),
                      const unsigned int xstride=1,
                      const unsigned int ystride=1,
                      const unsigned int zstride=1,
                      const int xdilation=1, const int ydilation=1, const int zdilation=1,
                      const int xoffset=0, const int yoffset=0, const int zoffset=0,
                      const unsigned int xsize=~0U,
                      const unsigned int ysize=~0U,
                      const unsigned int zsize=~0U) {
      if (is_empty() || !kernel) return *this;
      return get_convolve(kernel,boundary_conditions,is_normalized,channel_mode,
                          xcenter,ycenter,zcenter,xstride,ystride,zstride,xdilation,ydilation,zdilation,
                          xoffset,yoffset,zoffset,xsize,ysize,zsize).move_to(*this);
    }

    //! Convolve image by a kernel \newinstance.
    template<typename t>
    CImg<_cimg_Ttfloat> get_convolve(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                                     const bool is_normalized=false, const unsigned int channel_mode=1,
                                     const int xcenter=(int)(~0U>>1),
                                     const int ycenter=(int)(~0U>>1),
                                     const int zcenter=(int)(~0U>>1),
                                     const unsigned int xstride=1,
                                     const unsigned int ystride=1,
                                     const unsigned int zstride=1,
                                     const int xdilation=1, const int ydilation=1, const int zdilation=1,
                                     const int xoffset=0, const int yoffset=0, const int zoffset=0,
                                     const unsigned int xsize=~0U,
                                     const unsigned int ysize=~0U,
                                     const unsigned int zsize=~0U) const {
      return _correlate(kernel,boundary_conditions,is_normalized,channel_mode,
                        xcenter,ycenter,zcenter,xstride,ystride,zstride,xdilation,ydilation,zdilation,
                        xoffset,yoffset,zoffset,xsize,ysize,zsize,true);
    }

    //! Cumulate image values, optionally along specified axis.
    /**
       \param axis Cumulation axis. Set it to 0 to cumulate all values globally without taking axes into account.
    **/
    CImg<T>& cumulate(const char axis=0) {
      switch (cimg::lowercase(axis)) {
      case 'x' :
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*512 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forYZC(*this,y,z,c) {
          T *ptrd = data(0,y,z,c);
          Tlong cumul = (Tlong)0;
          cimg_forX(*this,x) { cumul+=(Tlong)*ptrd; *(ptrd++) = (T)cumul; }
        }
        break;
      case 'y' : {
        const ulongT w = (ulongT)_width;
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_height>=(cimg_openmp_sizefactor)*512 &&
                                                                   _width*_depth*_spectrum>=16))
        cimg_forXZC(*this,x,z,c) {
          T *ptrd = data(x,0,z,c);
          Tlong cumul = (Tlong)0;
          cimg_forY(*this,y) { cumul+=(Tlong)*ptrd; *ptrd = (T)cumul; ptrd+=w; }
        }
      } break;
      case 'z' : {
        const ulongT wh = (ulongT)_width*_height;
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_depth>=(cimg_openmp_sizefactor)*512 &&
                                                                   _width*_depth*_spectrum>=16))
        cimg_forXYC(*this,x,y,c) {
          T *ptrd = data(x,y,0,c);
          Tlong cumul = (Tlong)0;
          cimg_forZ(*this,z) { cumul+=(Tlong)*ptrd; *ptrd = (T)cumul; ptrd+=wh; }
        }
      } break;
      case 'c' : {
        const ulongT whd = (ulongT)_width*_height*_depth;
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3)
                           cimg_openmp_if(_spectrum>=(cimg_openmp_sizefactor)*512 && _width*_height*_depth>=16))
        cimg_forXYZ(*this,x,y,z) {
          T *ptrd = data(x,y,z,0);
          Tlong cumul = (Tlong)0;
          cimg_forC(*this,c) { cumul+=(Tlong)*ptrd; *ptrd = (T)cumul; ptrd+=whd; }
        }
      } break;
      default : { // Global cumulation
        Tlong cumul = (Tlong)0;
        cimg_for(*this,ptrd,T) { cumul+=(Tlong)*ptrd; *ptrd = (T)cumul; }
      }
      }
      return *this;
    }

    //! Cumulate image values, optionally along specified axis \newinstance.
    CImg<Tlong> get_cumulate(const char axis=0) const {
      return CImg<Tlong>(*this,false).cumulate(axis);
    }

    //! Cumulate image values, along specified axes.
    /**
       \param axes Cumulation axes, as a C-string.
       \note \c axes may contains multiple characters, e.g. \c "xyz"
    **/
    CImg<T>& cumulate(const char *const axes) {
      if (!axes) return cumulate();
      for (const char *s = axes; *s; ++s) cumulate(*s);
      return *this;
    }

    //! Cumulate image values, along specified axes \newinstance.
    CImg<Tlong> get_cumulate(const char *const axes) const {
      return CImg<Tlong>(*this,false).cumulate(axes);
    }

    //! Erode image by a structuring element.
    /**
       \param kernel Structuring element.
       \param boundary_conditions Boundary conditions.
         Can be <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
       \param is_real Do the erosion in real (a.k.a 'non-flat') mode (\c true) rather than binary mode (\c false).
    **/
    template<typename t>
    CImg<T>& erode(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                   const bool is_real=false) {
      if (is_empty() || !kernel) return *this;
      return get_erode(kernel,boundary_conditions,is_real).move_to(*this);
    }

    //! Erode image by a structuring element \newinstance.
    template<typename t>
    CImg<_cimg_Tt> get_erode(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                             const bool is_real=false) const {
      if (is_empty() || !kernel) return *this;
      if (!is_real && kernel==0) return CImg<T>(width(),height(),depth(),spectrum(),0);
      typedef _cimg_Tt Tt;
      CImg<Tt> res(_width,_height,_depth,std::max(_spectrum,kernel._spectrum));
      const int
        mx2 = kernel.width()/2, my2 = kernel.height()/2, mz2 = kernel.depth()/2,
        mx1 = kernel.width() - mx2 - 1, my1 = kernel.height() - my2 - 1, mz1 = kernel.depth() - mz2 - 1,
        mxe = width() - mx2, mye = height() - my2, mze = depth() - mz2,
        w2 = 2*width(), h2 = 2*height(), d2 = 2*depth();
      const bool
        is_inner_parallel = _width*_height*_depth>=(cimg_openmp_sizefactor)*32768,
        is_outer_parallel = res.size()>=(cimg_openmp_sizefactor)*32768;
      cimg::unused(is_inner_parallel,is_outer_parallel);
      _cimg_abort_init_openmp;
      cimg_abort_init;
      cimg_pragma_openmp(parallel for cimg_openmp_if(!is_inner_parallel && is_outer_parallel))
      cimg_forC(res,c) _cimg_abort_try_openmp {
        cimg_abort_test;
        const CImg<T> img = get_shared_channel(c%_spectrum);
        const CImg<t> K = kernel.get_shared_channel(c%kernel._spectrum);
        if (is_real) { // Real erosion
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
          for (int z = mz1; z<mze; ++z)
            for (int y = my1; y<mye; ++y)
              for (int x = mx1; x<mxe; ++x) _cimg_abort_try_openmp2 {
                cimg_abort_test2;
                Tt min_val = cimg::type<Tt>::max();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm) {
                      const t mval = K(mx1 + xm,my1 + ym,mz1 + zm);
                      const Tt cval = (Tt)(img(x + xm,y + ym,z + zm) - mval);
                      if (cval<min_val) min_val = cval;
                    }
                res(x,y,z,c) = min_val;
              } _cimg_abort_catch_openmp2

          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
          cimg_forYZ(res,y,z) _cimg_abort_try_openmp2 {
            cimg_abort_test2;
            for (int x = 0; x<width(); (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
              Tt min_val = cimg::type<Tt>::max();
              for (int zm = -mz1; zm<=mz2; ++zm)
                for (int ym = -my1; ym<=my2; ++ym)
                  for (int xm = -mx1; xm<=mx2; ++xm) {
                    const t mval = K(mx1 + xm,my1 + ym,mz1 + zm);
                    Tt cval;
                    switch (boundary_conditions) {
                    case 0 : cval = (Tt)(img.atXYZ(x + xm,y + ym,z + zm,0,(T)0) - mval); break;
                    case 1 : cval = (Tt)(img._atXYZ(x + xm,y + ym,z + zm) - mval); break;
                    case 2 : {
                      const int
                        nx = cimg::mod(x + xm,width()),
                        ny = cimg::mod(y + ym,height()),
                        nz = cimg::mod(z + zm,depth());
                      cval = img(nx,ny,nz) - mval;
                    } break;
                    default : {
                      const int
                        tx = cimg::mod(x + xm,w2),
                        ty = cimg::mod(y + ym,h2),
                        tz = cimg::mod(z + zm,d2),
                        nx = tx<width()?tx:w2 - tx - 1,
                        ny = ty<height()?ty:h2 - ty - 1,
                        nz = tz<depth()?tz:d2 - tz - 1;
                      cval = img(nx,ny,nz) - mval;
                    }
                    }
                    if (cval<min_val) min_val = cval;
                  }
              res(x,y,z,c) = min_val;
            }
          } _cimg_abort_catch_openmp2

        } else { // Binary erosion
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
          for (int z = mz1; z<mze; ++z)
            for (int y = my1; y<mye; ++y)
              for (int x = mx1; x<mxe; ++x) _cimg_abort_try_openmp2 {
                cimg_abort_test2;
                Tt min_val = cimg::type<Tt>::max();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm)
                      if (K(mx1 + xm,my1 + ym,mz1 + zm)) {
                        const Tt cval = (Tt)img(x + xm,y + ym,z + zm);
                        if (cval<min_val) min_val = cval;
                      }
                res(x,y,z,c) = min_val;
              } _cimg_abort_catch_openmp2

          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
          cimg_forYZ(res,y,z) _cimg_abort_try_openmp2 {
            cimg_abort_test2;
            for (int x = 0; x<width(); (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
              Tt min_val = cimg::type<Tt>::max();
              for (int zm = -mz1; zm<=mz2; ++zm)
                for (int ym = -my1; ym<=my2; ++ym)
                  for (int xm = -mx1; xm<=mx2; ++xm) {
                    if (K(mx1 + xm,my1 + ym,mz1 + zm)) {
                      Tt cval;
                      switch (boundary_conditions) {
                      case 0 : cval = (Tt)img.atXYZ(x + xm,y + ym,z + zm,0,(T)0); break;
                      case 1 : cval = (Tt)img._atXYZ(x + xm,y + ym,z + zm); break;
                      case 2 : {
                        const int
                          nx = cimg::mod(x + xm,width()),
                          ny = cimg::mod(y + ym,height()),
                          nz = cimg::mod(z + zm,depth());
                        cval = img(nx,ny,nz);
                      } break;
                      default : {
                        const int
                          tx = cimg::mod(x + xm,w2),
                          ty = cimg::mod(y + ym,h2),
                          tz = cimg::mod(z + zm,d2),
                          nx = tx<width()?tx:w2 - tx - 1,
                          ny = ty<height()?ty:h2 - ty - 1,
                          nz = tz<depth()?tz:d2 - tz - 1;
                        cval = img(nx,ny,nz);
                      }
                      }
                      if (cval<min_val) min_val = cval;
                    }
                  }
              res(x,y,z,c) = min_val;
            }
          } _cimg_abort_catch_openmp2

        }
      } _cimg_abort_catch_openmp
      cimg_abort_test;
      return res;
    }

    //! Erode image by a rectangular structuring element of specified size.
    /**
       \param sx Width of the structuring element.
       \param sy Height of the structuring element.
       \param sz Depth of the structuring element.
    **/
    CImg<T>& erode(const unsigned int sx, const unsigned int sy, const unsigned int sz=1) {
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      if (sx>1 && _width>1) { // Along X-axis
        const int L = width(), off = 1, s = (int)sx, _s2 = s/2 + 1, _s1 = s - _s2, s1 = _s1>L?L:_s1, s2 = _s2>L?L:_s2;
        CImg<T> buf(L);
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) firstprivate(buf) if (size()>524288))
        cimg_forYZC(*this,y,z,c) {
          T *const ptrdb = buf._data, *ptrd = buf._data, *const ptrde = buf._data + L - 1;
          const T *const ptrsb = data(0,y,z,c), *ptrs = ptrsb, *const ptrse = ptrs + (ulongT)L*off - off;
          T cur = *ptrs; ptrs+=off; bool is_first = true;
          for (int p = s2 - 1; p>0 && ptrs<=ptrse; --p) {
            const T val = *ptrs; ptrs+=off; if (val<=cur) { cur = val; is_first = false; }}
          *(ptrd++) = cur;
          if (ptrs>=ptrse) {
            T *pd = data(0,y,z,c); cur = std::min(cur,*ptrse); cimg_forX(buf,k) { *pd = cur; pd+=off; }
          } else {
            for (int p = s1; p>0 && ptrd<=ptrde; --p) {
              const T val = *ptrs; if (ptrs<ptrse) ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
              *(ptrd++) = cur;
            }
            for (int p = L - s - 1; p>0; --p) {
              const T val = *ptrs; ptrs+=off;
              if (is_first) {
                const T *nptrs = ptrs - off; cur = val;
                for (int q = s - 2; q>0; --q) { nptrs-=off; const T nval = *nptrs; if (nval<cur) cur = nval; }
                nptrs-=off; const T nval = *nptrs; if (nval<cur) { cur = nval; is_first = true; } else is_first = false;
              } else { if (val<=cur) cur = val; else if (cur==*(ptrs-s*off)) is_first = true; }
              *(ptrd++) = cur;
            }
            ptrd = ptrde; ptrs = ptrse; cur = *ptrs; ptrs-=off;
            for (int p = s1; p>0 && ptrs>=ptrsb; --p) {
              const T val = *ptrs; ptrs-=off; if (val<cur) cur = val;
            }
            *(ptrd--) = cur;
            for (int p = s2 - 1; p>0 && ptrd>=ptrdb; --p) {
              const T val = *ptrs; if (ptrs>ptrsb) ptrs-=off; if (val<cur) cur = val; *(ptrd--) = cur;
            }
            T *pd = data(0,y,z,c); cimg_for(buf,ps,T) { *pd = *ps; pd+=off; }
          }
        }
      }

      if (sy>1 && _height>1) { // Along Y-axis
        const int L = height(), off = width(), s = (int)sy, _s2 = s/2 + 1, _s1 = s - _s2, s1 = _s1>L?L:_s1,
          s2 = _s2>L?L:_s2;
        CImg<T> buf(L);
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) firstprivate(buf) if (size()>524288))
        cimg_forXZC(*this,x,z,c) {
          T *const ptrdb = buf._data, *ptrd = ptrdb, *const ptrde = buf._data + L - 1;
          const T *const ptrsb = data(x,0,z,c), *ptrs = ptrsb, *const ptrse = ptrs + (ulongT)L*off - off;
          T cur = *ptrs; ptrs+=off; bool is_first = true;
          for (int p = s2 - 1; p>0 && ptrs<=ptrse; --p) {
            const T val = *ptrs; ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
          }
          *(ptrd++) = cur;
          if (ptrs>=ptrse) {
            T *pd = data(x,0,z,c); cur = std::min(cur,*ptrse); cimg_forX(buf,k) { *pd = cur; pd+=off; }
          } else {
            for (int p = s1; p>0 && ptrd<=ptrde; --p) {
              const T val = *ptrs; if (ptrs<ptrse) ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
              *(ptrd++) = cur;
            }
            for (int p = L - s - 1; p>0; --p) {
              const T val = *ptrs; ptrs+=off;
              if (is_first) {
                const T *nptrs = ptrs - off; cur = val;
                for (int q = s - 2; q>0; --q) { nptrs-=off; const T nval = *nptrs; if (nval<cur) cur = nval; }
                nptrs-=off; const T nval = *nptrs; if (nval<cur) { cur = nval; is_first = true; } else is_first = false;
              } else { if (val<=cur) cur = val; else if (cur==*(ptrs-s*off)) is_first = true; }
              *(ptrd++) = cur;
            }
            ptrd = ptrde; ptrs = ptrse; cur = *ptrs; ptrs-=off;
            for (int p = s1; p>0 && ptrs>=ptrsb; --p) {
              const T val = *ptrs; ptrs-=off; if (val<cur) cur = val;
            }
            *(ptrd--) = cur;
            for (int p = s2 - 1; p>0 && ptrd>=ptrdb; --p) {
              const T val = *ptrs; if (ptrs>ptrsb) ptrs-=off; if (val<cur) cur = val; *(ptrd--) = cur;
            }
            T *pd = data(x,0,z,c); cimg_for(buf,ps,T) { *pd = *ps; pd+=off; }
          }
        }
      }

      if (sz>1 && _depth>1) { // Along Z-axis
        const int L = depth(), off = width()*height(), s = (int)sz, _s2 = s/2 + 1, _s1 = s - _s2, s1 = _s1>L?L:_s1,
          s2 = _s2>L?L:_s2;
        CImg<T> buf(L);
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) firstprivate(buf) if (size()>524288))
        cimg_forXYC(*this,x,y,c) {
          T *const ptrdb = buf._data, *ptrd = ptrdb, *const ptrde = buf._data + L - 1;
          const T *const ptrsb = data(x,y,0,c), *ptrs = ptrsb, *const ptrse = ptrs + (ulongT)L*off - off;
          T cur = *ptrs; ptrs+=off; bool is_first = true;
          for (int p = s2 - 1; p>0 && ptrs<=ptrse; --p) {
            const T val = *ptrs; ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
          }
          *(ptrd++) = cur;
          if (ptrs>=ptrse) {
            T *pd = data(x,y,0,c); cur = std::min(cur,*ptrse); cimg_forX(buf,k) { *pd = cur; pd+=off; }
          } else {
            for (int p = s1; p>0 && ptrd<=ptrde; --p) {
              const T val = *ptrs; if (ptrs<ptrse) ptrs+=off; if (val<=cur) { cur = val; is_first = false; }
              *(ptrd++) = cur;
            }
            for (int p = L - s - 1; p>0; --p) {
              const T val = *ptrs; ptrs+=off;
              if (is_first) {
                const T *nptrs = ptrs - off; cur = val;
                for (int q = s - 2; q>0; --q) { nptrs-=off; const T nval = *nptrs; if (nval<cur) cur = nval; }
                nptrs-=off; const T nval = *nptrs; if (nval<cur) { cur = nval; is_first = true; } else is_first = false;
              } else { if (val<=cur) cur = val; else if (cur==*(ptrs-s*off)) is_first = true; }
              *(ptrd++) = cur;
            }
            ptrd = ptrde; ptrs = ptrse; cur = *ptrs; ptrs-=off;
            for (int p = s1; p>0 && ptrs>=ptrsb; --p) {
              const T val = *ptrs; ptrs-=off; if (val<cur) cur = val;
            }
            *(ptrd--) = cur;
            for (int p = s2 - 1; p>0 && ptrd>=ptrdb; --p) {
              const T val = *ptrs; if (ptrs>ptrsb) ptrs-=off; if (val<cur) cur = val; *(ptrd--) = cur;
            }
            T *pd = data(x,y,0,c); cimg_for(buf,ps,T) { *pd = *ps; pd+=off; }
          }
        }
      }
      return *this;
    }

    //! Erode image by a rectangular structuring element of specified size \newinstance.
    CImg<T> get_erode(const unsigned int sx, const unsigned int sy, const unsigned int sz=1) const {
      return (+*this).erode(sx,sy,sz);
    }

    //! Erode the image by a square structuring element of specified size.
    /**
       \param s Size of the structuring element.
    **/
    CImg<T>& erode(const unsigned int s) {
      return erode(s,s,s);
    }

    //! Erode the image by a square structuring element of specified size \newinstance.
    CImg<T> get_erode(const unsigned int s) const {
      return (+*this).erode(s);
    }

    //! Dilate image by a structuring element.
    /**
       \param kernel Structuring element.
       \param boundary_conditions Boundary conditions.
         Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
       \param is_real Do the dilation in real (a.k.a 'non-flat') mode (\c true) rather than binary mode (\c false).
    **/
    template<typename t>
    CImg<T>& dilate(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                    const bool is_real=false) {
      if (is_empty() || !kernel) return *this;
      return get_dilate(kernel,boundary_conditions,is_real).move_to(*this);
    }

    //! Dilate image by a structuring element \newinstance.
    template<typename t>
    CImg<_cimg_Tt> get_dilate(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                              const bool is_real=false) const {
      if (is_empty() || !kernel || (!is_real && kernel==0)) return *this;
      typedef _cimg_Tt Tt;
      CImg<Tt> res(_width,_height,_depth,std::max(_spectrum,kernel._spectrum));
      const int
        mx1 = kernel.width()/2, my1 = kernel.height()/2, mz1 = kernel.depth()/2,
        mx2 = kernel.width() - mx1 - 1, my2 = kernel.height() - my1 - 1, mz2 = kernel.depth() - mz1 - 1,
        mxe = width() - mx2, mye = height() - my2, mze = depth() - mz2,
        w2 = 2*width(), h2 = 2*height(), d2 = 2*depth();
      const bool
        is_inner_parallel = _width*_height*_depth>=(cimg_openmp_sizefactor)*32768,
        is_outer_parallel = res.size()>=(cimg_openmp_sizefactor)*32768;
      cimg::unused(is_inner_parallel,is_outer_parallel);
      _cimg_abort_init_openmp;
      cimg_abort_init;
      cimg_pragma_openmp(parallel for cimg_openmp_if(!is_inner_parallel && is_outer_parallel))
      cimg_forC(res,c) _cimg_abort_try_openmp {
        cimg_abort_test;
        const CImg<T> img = get_shared_channel(c%_spectrum);
        const CImg<t> K = kernel.get_shared_channel(c%kernel._spectrum);
        if (is_real) { // Real dilation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
          for (int z = mz1; z<mze; ++z)
            for (int y = my1; y<mye; ++y)
              for (int x = mx1; x<mxe; ++x) _cimg_abort_try_openmp2 {
                cimg_abort_test2;
                Tt max_val = cimg::type<Tt>::min();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm) {
                      const t mval = K(mx2 - xm,my2 - ym,mz2 - zm);
                      const Tt cval = (Tt)(img(x + xm,y + ym,z + zm) + mval);
                      if (cval>max_val) max_val = cval;
                    }
                res(x,y,z,c) = max_val;
              } _cimg_abort_catch_openmp2

          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
          cimg_forYZ(res,y,z) _cimg_abort_try_openmp2 {
            cimg_abort_test2;
            for (int x = 0; x<width(); (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
              Tt max_val = cimg::type<Tt>::min();
              for (int zm = -mz1; zm<=mz2; ++zm)
                for (int ym = -my1; ym<=my2; ++ym)
                  for (int xm = -mx1; xm<=mx2; ++xm) {
                    const t mval = K(mx2 - xm,my2 - ym,mz2 - zm);
                    Tt cval;
                    switch (boundary_conditions) {
                    case 0 : cval = (Tt)(img.atXYZ(x + xm,y + ym,z + zm,0,(T)0) + mval); break;
                    case 1 : cval = (Tt)(img._atXYZ(x + xm,y + ym,z + zm) + mval); break;
                    case 2 : {
                      const int
                        nx = cimg::mod(x + xm,width()),
                        ny = cimg::mod(y + ym,height()),
                        nz = cimg::mod(z + zm,depth());
                      cval = img(nx,ny,nz) + mval;
                    } break;
                    default : {
                      const int
                        tx = cimg::mod(x + xm,w2),
                        ty = cimg::mod(y + ym,h2),
                        tz = cimg::mod(z + zm,d2),
                        nx = tx<width()?tx:w2 - tx - 1,
                        ny = ty<height()?ty:h2 - ty - 1,
                        nz = tz<depth()?tz:d2 - tz - 1;
                      cval = img(nx,ny,nz) + mval;
                    }
                    }
                    if (cval>max_val) max_val = cval;
                  }
              res(x,y,z,c) = max_val;
            }
          } _cimg_abort_catch_openmp2

        } else { // Binary dilation
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(is_inner_parallel))
          for (int z = mz1; z<mze; ++z)
            for (int y = my1; y<mye; ++y)
              for (int x = mx1; x<mxe; ++x) _cimg_abort_try_openmp2 {
                cimg_abort_test2;
                Tt max_val = cimg::type<Tt>::min();
                for (int zm = -mz1; zm<=mz2; ++zm)
                  for (int ym = -my1; ym<=my2; ++ym)
                    for (int xm = -mx1; xm<=mx2; ++xm)
                      if (K(mx2 - xm,my2 - ym,mz2 - zm)) {
                        const Tt cval = (Tt)img(x + xm,y + ym,z + zm);
                        if (cval>max_val) max_val = cval;
                      }
                res(x,y,z,c) = max_val;
              } _cimg_abort_catch_openmp2

          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(is_inner_parallel))
          cimg_forYZ(res,y,z) _cimg_abort_try_openmp2 {
            cimg_abort_test2;
            for (int x = 0; x<width(); (y<my1 || y>=mye || z<mz1 || z>=mze)?++x:((x<mx1 - 1 || x>=mxe)?++x:(x=mxe))) {
              Tt max_val = cimg::type<Tt>::min();
              for (int zm = -mz1; zm<=mz2; ++zm)
                for (int ym = -my1; ym<=my2; ++ym)
                  for (int xm = -mx1; xm<=mx2; ++xm) {
                    if (K(mx2 - xm,my2 - ym,mz2 - zm)) {
                      Tt cval;
                      switch (boundary_conditions) {
                      case 0 : cval = (Tt)img.atXYZ(x + xm,y + ym,z + zm,0,(T)0); break;
                      case 1 : cval = (Tt)img._atXYZ(x + xm,y + ym,z + zm); break;
                      case 2 : {
                        const int
                          nx = cimg::mod(x + xm,width()),
                          ny = cimg::mod(y + ym,height()),
                          nz = cimg::mod(z + zm,depth());
                        cval = img(nx,ny,nz);
                      } break;
                      default : {
                        const int
                          tx = cimg::mod(x + xm,w2),
                          ty = cimg::mod(y + ym,h2),
                          tz = cimg::mod(z + zm,d2),
                          nx = tx<width()?tx:w2 - tx - 1,
                          ny = ty<height()?ty:h2 - ty - 1,
                          nz = tz<depth()?tz:d2 - tz - 1;
                        cval = img(nx,ny,nz);
                      }
                      }
                      if (cval>max_val) max_val = cval;
                    }
                  }
              res(x,y,z,c) = max_val;
            }
          } _cimg_abort_catch_openmp2

        }
      } _cimg_abort_catch_openmp
      cimg_abort_test;
      return res;
    }

    //! Dilate image by a rectangular structuring element of specified size.
    /**
       \param sx Width of the structuring element.
       \param sy Height of the structuring element.
       \param sz Depth of the structuring element.
    **/
    CImg<T>& dilate(const unsigned int sx, const unsigned int sy, const unsigned int sz=1) {
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      if (sx>1 && _width>1) { // Along X-axis
        const int L = width(), off = 1, s = (int)sx, _s1 = s/2, _s2 = s - _s1, s1 = _s1>L?L:_s1, s2 = _s2>L?L:_s2;
        CImg<T> buf(L);
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) firstprivate(buf) if (size()>524288))
        cimg_forYZC(*this,y,z,c) {
          T *const ptrdb = buf._data, *ptrd = ptrdb, *const ptrde = buf._data + L - 1;
          const T *const ptrsb = data(0,y,z,c), *ptrs = ptrsb, *const ptrse = ptrs + (ulongT)L*off - off;
          T cur = *ptrs; ptrs+=off; bool is_first = true;
          for (int p = s2 - 1; p>0 && ptrs<=ptrse; --p) {
            const T val = *ptrs; ptrs+=off; if (val>=cur) { cur = val; is_first = false; }
          }
          *(ptrd++) = cur;
          if (ptrs>=ptrse) {
            T *pd = data(0,y,z,c); cur = std::max(cur,*ptrse); cimg_forX(buf,k) { *pd = cur; pd+=off; }
          } else {
            for (int p = s1; p>0 && ptrd<=ptrde; --p) {
              const T val = *ptrs; if (ptrs<ptrse) ptrs+=off; if (val>=cur) { cur = val; is_first = false; }
              *(ptrd++) = cur;
            }
            for (int p = L - s - 1; p>0; --p) {
              const T val = *ptrs; ptrs+=off;
              if (is_first) {
                const T *nptrs = ptrs - off; cur = val;
                for (int q = s - 2; q>0; --q) { nptrs-=off; const T nval = *nptrs; if (nval>cur) cur = nval; }
                nptrs-=off; const T nval = *nptrs; if (nval>cur) { cur = nval; is_first = true; } else is_first = false;
              } else { if (val>=cur) cur = val; else if (cur==*(ptrs-s*off)) is_first = true; }
              *(ptrd++) = cur;
            }
            ptrd = ptrde; ptrs = ptrse; cur = *ptrs; ptrs-=off;
            for (int p = s1; p>0 && ptrs>=ptrsb; --p) {
              const T val = *ptrs; ptrs-=off; if (val>cur) cur = val;
            }
            *(ptrd--) = cur;
            for (int p = s2 - 1; p>0 && ptrd>=ptrdb; --p) {
              const T val = *ptrs; if (ptrs>ptrsb) ptrs-=off; if (val>cur) cur = val; *(ptrd--) = cur;
            }
            T *pd = data(0,y,z,c); cimg_for(buf,ps,T) { *pd = *ps; pd+=off; }
          }
        }
      }

      if (sy>1 && _height>1) { // Along Y-axis
        const int L = height(), off = width(), s = (int)sy, _s1 = s/2, _s2 = s - _s1, s1 = _s1>L?L:_s1,
          s2 = _s2>L?L:_s2;
        CImg<T> buf(L);
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) firstprivate(buf) if (size()>524288))
        cimg_forXZC(*this,x,z,c) {
          T *const ptrdb = buf._data, *ptrd = ptrdb, *const ptrde = buf._data + L - 1;
          const T *const ptrsb = data(x,0,z,c), *ptrs = ptrsb, *const ptrse = ptrs + (ulongT)L*off - off;
          T cur = *ptrs; ptrs+=off; bool is_first = true;
          for (int p = s2 - 1; p>0 && ptrs<=ptrse; --p) {
            const T val = *ptrs; ptrs+=off; if (val>=cur) { cur = val; is_first = false; }
          }
          *(ptrd++) = cur;
          if (ptrs>=ptrse) {
            T *pd = data(x,0,z,c); cur = std::max(cur,*ptrse); cimg_forX(buf,k) { *pd = cur; pd+=off; }
          } else {
            for (int p = s1; p>0 && ptrd<=ptrde; --p) {
              const T val = *ptrs; if (ptrs<ptrse) ptrs+=off; if (val>=cur) { cur = val; is_first = false; }
              *(ptrd++) = cur;
            }
            for (int p = L - s - 1; p>0; --p) {
              const T val = *ptrs; ptrs+=off;
              if (is_first) {
                const T *nptrs = ptrs - off; cur = val;
                for (int q = s - 2; q>0; --q) { nptrs-=off; const T nval = *nptrs; if (nval>cur) cur = nval; }
                nptrs-=off; const T nval = *nptrs; if (nval>cur) { cur = nval; is_first = true; } else is_first = false;
              } else { if (val>=cur) cur = val; else if (cur==*(ptrs-s*off)) is_first = true; }
              *(ptrd++) = cur;
            }
            ptrd = ptrde; ptrs = ptrse; cur = *ptrs; ptrs-=off;
            for (int p = s1; p>0 && ptrs>=ptrsb; --p) {
              const T val = *ptrs; ptrs-=off; if (val>cur) cur = val;
            }
            *(ptrd--) = cur;
            for (int p = s2 - 1; p>0 && ptrd>=ptrdb; --p) {
              const T val = *ptrs; if (ptrs>ptrsb) ptrs-=off; if (val>cur) cur = val; *(ptrd--) = cur;
            }
            T *pd = data(x,0,z,c); cimg_for(buf,ps,T) { *pd = *ps; pd+=off; }
          }
        }
      }

      if (sz>1 && _depth>1) { // Along Z-axis
        const int L = depth(), off = width()*height(), s = (int)sz, _s1 = s/2, _s2 = s - _s1, s1 = _s1>L?L:_s1,
          s2 = _s2>L?L:_s2;
        CImg<T> buf(L);
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) firstprivate(buf) if (size()>524288))
        cimg_forXYC(*this,x,y,c) {
          T *const ptrdb = buf._data, *ptrd = ptrdb, *const ptrde = buf._data + L - 1;
          const T *const ptrsb = data(x,y,0,c), *ptrs = ptrsb, *const ptrse = ptrs + (ulongT)L*off - off;
          T cur = *ptrs; ptrs+=off; bool is_first = true;
          for (int p = s2 - 1; p>0 && ptrs<=ptrse; --p) {
            const T val = *ptrs; ptrs+=off; if (val>=cur) { cur = val; is_first = false; }
          }
          *(ptrd++) = cur;
          if (ptrs>=ptrse) {
            T *pd = data(x,y,0,c); cur = std::max(cur,*ptrse); cimg_forX(buf,k) { *pd = cur; pd+=off; }
          } else {
            for (int p = s1; p>0 && ptrd<=ptrde; --p) {
              const T val = *ptrs; if (ptrs<ptrse) ptrs+=off; if (val>=cur) { cur = val; is_first = false; }
              *(ptrd++) = cur;
            }
            for (int p = L - s - 1; p>0; --p) {
              const T val = *ptrs; ptrs+=off;
              if (is_first) {
                const T *nptrs = ptrs - off; cur = val;
                for (int q = s - 2; q>0; --q) { nptrs-=off; const T nval = *nptrs; if (nval>cur) cur = nval; }
                nptrs-=off; const T nval = *nptrs; if (nval>cur) { cur = nval; is_first = true; } else is_first = false;
              } else { if (val>=cur) cur = val; else if (cur==*(ptrs-s*off)) is_first = true; }
              *(ptrd++) = cur;
            }
            ptrd = ptrde; ptrs = ptrse; cur = *ptrs; ptrs-=off;
            for (int p = s1; p>0 && ptrs>=ptrsb; --p) {
              const T val = *ptrs; ptrs-=off; if (val>cur) cur = val;
            }
            *(ptrd--) = cur;
            for (int p = s2 - 1; p>0 && ptrd>=ptrdb; --p) {
              const T val = *ptrs; if (ptrs>ptrsb) ptrs-=off; if (val>cur) cur = val; *(ptrd--) = cur;
            }
            T *pd = data(x,y,0,c); cimg_for(buf,ps,T) { *pd = *ps; pd+=off; }
          }
        }
      }
      return *this;
    }

    //! Dilate image by a rectangular structuring element of specified size \newinstance.
    CImg<T> get_dilate(const unsigned int sx, const unsigned int sy, const unsigned int sz=1) const {
      return (+*this).dilate(sx,sy,sz);
    }

    //! Dilate image by a square structuring element of specified size.
    /**
       \param s Size of the structuring element.
    **/
    CImg<T>& dilate(const unsigned int s) {
      return dilate(s,s,s);
    }

    //! Dilate image by a square structuring element of specified size \newinstance.
    CImg<T> get_dilate(const unsigned int s) const {
      return (+*this).dilate(s);
    }

    //! Apply morphological closing by a structuring element.
    /**
       \param kernel Structuring element.
       \param boundary_conditions Boundary conditions.
         Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
       \param is_real Do the closing in real (a.k.a 'non-flat') mode (\c true) rather than binary mode (\c false).
    **/
    template<typename t>
    CImg<T>& closing(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                     const bool is_real=false) {
      const int sx = kernel.width(), sy = kernel.height(), sz = kernel.depth();
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      return get_closing(kernel,boundary_conditions,is_real).move_to(*this);
    }

    //! Apply morphological closing by a structuring element \newinstance.
    template<typename t>
    CImg<T> get_closing(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                        const bool is_real=false) const {
      const int sx = kernel.width(), sy = kernel.height(), sz = kernel.depth();
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      const int sx1 = (int)(sx - 1)/2, sy1 = (int)(sy - 1)/2, sz1 = (int)(sz - 1)/2;
      CImg<T> res;
      if (_depth>1) { // 3D
        get_resize(width() + sx + 1,height() + sy + 1,depth() + sz + 1,spectrum(),0,boundary_conditions,0.5,0.5,0.5).
          dilate(kernel,1,is_real).erode(kernel,1,is_real).
          crop(sx1 + 1,sy1 + 1,sz1 + 1,sx1 + width(),sy1 + height(),sz1 + depth()).move_to(res);
      } else if (_height>1) { // 2D
        get_resize(width() + sx + 1,height() + sy + 1,1,spectrum(),0,boundary_conditions,0.5,0.5).
          dilate(kernel,1,is_real).erode(kernel,1,is_real).
          crop(sx1 + 1,sy1 + 1,sx1 + width(),sy1 + height()).move_to(res);
      } else if (_width>1) { // 1D
        get_resize(width() + sx + 1,1,1,spectrum(),0,boundary_conditions,0.5).
          dilate(kernel,1,is_real).erode(kernel,1,is_real).
          crop(sx1 + 1,sx1 + width()).move_to(res);
      }
      return res;
    }

    //! Apply morphological closing by a rectangular structuring element of specified size.
    CImg<T>& closing(const unsigned int sx, const unsigned int sy, const unsigned int sz=1) {
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      return get_closing(sx,sy,sz).move_to(*this);
    }

    //! Apply morphological closing by a rectangular structuring element of specified size \newinstance.
    CImg<T> get_closing(const unsigned int sx, const unsigned int sy, const unsigned int sz=1) const {
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      const int sx1 = (int)(sx - 1)/2, sy1 = (int)(sy - 1)/2, sz1 = (int)(sz - 1)/2;
      CImg<T> res;
      if (_depth>1) { // 3D
        get_resize(width() + sx + 1,height() + sy + 1,depth() + sz + 1,spectrum(),0,1,0.5,0.5,0.5).
          dilate(sx,sy,sz).erode(sx,sy,sz).
          crop(sx1 + 1,sy1 + 1,sz1 + 1,sx1 + width(),sy1 + height(),sz1 + depth()).move_to(res);
      } else if (_height>1) { // 2D
        get_resize(width() + sx + 1,height() + sy + 1,1,spectrum(),0,1,0.5,0.5).
          dilate(sx,sy).erode(sx,sy).
          crop(sx1 + 1,sy1 + 1,sx1 + width(),sy1 + height()).move_to(res);
      } else if (_width>1) { // 1D
        get_resize(width() + sx + 1,1,1,spectrum(),0,1,0.5).
          dilate(sx,1).erode(sx,1).
          crop(sx1 + 1,sx1 + width()).move_to(res);
      }
      return res;
    }

    //! Apply morphological closing by a square structuring element of specified size.
    /**
       \param s Size of the structuring element.
    **/
    CImg<T>& closing(const unsigned int s) {
      return closing(s,s,s);
    }

    //! Apply morphological closing by a square structuring element of specified size \newinstance.
    CImg<T> get_closing(const unsigned int s) const {
      return (+*this).closing(s);
    }

    //! Apply morphological opening by a structuring element.
    /**
       \param kernel Structuring element.
       \param boundary_conditions Boundary conditions.
         Can be { 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }.
       \param is_real Do the opening in real (a.k.a 'non-flat') mode (\c true) rather than binary mode (\c false).
    **/
    template<typename t>
    CImg<T>& opening(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                     const bool is_real=false) {
      const int sx = kernel.width(), sy = kernel.height(), sz = kernel.depth();
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      return get_opening(kernel,boundary_conditions,is_real).move_to(*this);
    }

    //! Apply morphological opening by a structuring element \newinstance.
    template<typename t>
    CImg<T> get_opening(const CImg<t>& kernel, const unsigned int boundary_conditions=1,
                        const bool is_real=false) const {
      const int sx = kernel.width(), sy = kernel.height(), sz = kernel.depth();
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      const int sx1 = (int)(sx - 1)/2, sy1 = (int)(sy - 1)/2, sz1 = (int)(sz - 1)/2;
      CImg<T> res;
      if (_depth>1) { // 3D
        get_resize(width() + sx + 1,height() + sy + 1,depth() + sz + 1,spectrum(),0,boundary_conditions,0.5,0.5,0.5).
          erode(kernel,1,is_real).dilate(kernel,1,is_real).
          crop(sx1 + 1,sy1 + 1,sz1 + 1,sx1 + width(),sy1 + height(),sz1 + depth()).move_to(res);
      } else if (_height>1) { // 2D
        get_resize(width() + sx + 1,height() + sy + 1,1,spectrum(),0,boundary_conditions,0.5,0.5).
          erode(kernel,1,is_real).dilate(kernel,1,is_real).
          crop(sx1 + 1,sy1 + 1,sx1 + width(),sy1 + height()).move_to(res);
      } else if (_width>1) { // 1D
        get_resize(width() + sx + 1,1,1,spectrum(),0,boundary_conditions,0.5).
          erode(kernel,1,is_real).dilate(kernel,1,is_real).
          crop(sx1 + 1,sx1 + width()).move_to(res);
      }
      return res;
    }

    //! Apply morphological opening by a rectangular structuring element of specified size.
    CImg<T>& opening(const unsigned int sx, const unsigned int sy, const unsigned int sz=1) {
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      return get_opening(sx,sy,sz).move_to(*this);
    }

    //! Apply morphological opening by a rectangular structuring element of specified size \newinstance.
    CImg<T> get_opening(const unsigned int sx, const unsigned int sy, const unsigned int sz=1) const {
      if (is_empty() || (sx<=1 && sy<=1 && sz<=1)) return *this;
      const int sx1 = (int)(sx - 1)/2, sy1 = (int)(sy - 1)/2, sz1 = (int)(sz - 1)/2;
      CImg<T> res;
      if (_depth>1) { // 3D
        get_resize(width() + sx + 1,height() + sy + 1,depth() + sz + 1,spectrum(),0,1,0.5,0.5,0.5).
          erode(sx,sy,sz).dilate(sx,sy,sz).
          crop(sx1 + 1,sy1 + 1,sz1 + 1,sx1 + width(),sy1 + height(),sz1 + depth()).move_to(res);
      } else if (_height>1) { // 2D
        get_resize(width() + sx + 1,height() + sy + 1,1,spectrum(),0,1,0.5,0.5).
          erode(sx,sy).dilate(sx,sy).
          crop(sx1 + 1,sy1 + 1,sx1 + width(),sy1 + height()).move_to(res);
      } else if (_width>1) { // 1D
        get_resize(width() + sx + 1,1,1,spectrum(),0,1,0.5).
          erode(sx,1).dilate(sx,1).
          crop(sx1 + 1,sx1 + width()).move_to(res);
      }
      return res;
    }

    //! Apply morphological opening by a square structuring element of specified size.
    /**
       \param s Size of the structuring element.
    **/
    CImg<T>& opening(const unsigned int s) {
      return opening(s,s,s);
    }

    //! Apply morphological opening by a square structuring element of specified size \newinstance.
    CImg<T> get_opening(const unsigned int s) const {
      return (+*this).opening(s);
    }

    //! Compute watershed transform.
    /**
       \param priority Priority map.
       \param is_high_connectivity Boolean that choose between 4(false)- or 8(true)-connectivity
       in 2D case, and between 6(false)- or 26(true)-connectivity in 3D case.
       \note Non-zero values of the instance instance are propagated to zero-valued ones according to
       specified the priority map.
    **/
    template<typename t>
    CImg<T>& watershed(const CImg<t>& priority, const bool is_high_connectivity=false) {
#define _cimg_watershed_init(cond,X,Y,Z) \
      if (cond && !(*this)(X,Y,Z)) Q._priority_queue_insert(labels,sizeQ,priority(X,Y,Z),X,Y,Z,nb_seeds)

#define _cimg_watershed_propagate(cond,X,Y,Z) \
      if (cond) { \
        if ((*this)(X,Y,Z)) { \
          ns = labels(X,Y,Z) - 1; xs = seeds(ns,0); ys = seeds(ns,1); zs = seeds(ns,2); \
          d = cimg::sqr((float)x - xs) + cimg::sqr((float)y - ys) + cimg::sqr((float)z - zs); \
          if (d<dmin) { dmin = d; nmin = ns; nlabel = (*this)(xs,ys,zs); } \
        } else Q._priority_queue_insert(labels,sizeQ,priority(X,Y,Z),X,Y,Z,n); \
      }

      if (is_empty()) return *this;
      if (!is_sameXYZ(priority))
        throw CImgArgumentException(_cimg_instance
                                    "watershed(): image instance and specified priority (%u,%u,%u,%u,%p) "
                                    "have different dimensions.",
                                    cimg_instance,
                                    priority._width,priority._height,priority._depth,priority._spectrum,priority._data);
      if (_spectrum!=1) {
        cimg_forC(*this,c)
          get_shared_channel(c).watershed(priority.get_shared_channel(c%priority._spectrum));
        return *this;
      }

      CImg<uintT> labels(_width,_height,_depth,1,0), seeds(64,3);
      CImg<typename cimg::superset2<T,t,int>::type> Q;
      unsigned int sizeQ = 0;
      int px, nx, py, ny, pz, nz;
      bool is_px, is_nx, is_py, is_ny, is_pz, is_nz;
      const bool is_3d = _depth>1;

      // Find seed points and insert them in priority queue.
      unsigned int nb_seeds = 0;
      const T *ptrs = _data;
      cimg_forXYZ(*this,x,y,z) if (*(ptrs++)) { // 3D version
        if (nb_seeds>=seeds._width) seeds.resize(2*seeds._width,3,1,1,0);
        seeds(nb_seeds,0) = x; seeds(nb_seeds,1) = y; seeds(nb_seeds++,2) = z;
        px = x - 1; nx = x + 1;
        py = y - 1; ny = y + 1;
        pz = z - 1; nz = z + 1;
        is_px = px>=0; is_nx = nx<width();
        is_py = py>=0; is_ny = ny<height();
        is_pz = pz>=0; is_nz = nz<depth();
        _cimg_watershed_init(is_px,px,y,z);
        _cimg_watershed_init(is_nx,nx,y,z);
        _cimg_watershed_init(is_py,x,py,z);
        _cimg_watershed_init(is_ny,x,ny,z);
        if (is_3d) {
          _cimg_watershed_init(is_pz,x,y,pz);
          _cimg_watershed_init(is_nz,x,y,nz);
        }
        if (is_high_connectivity) {
          _cimg_watershed_init(is_px && is_py,px,py,z);
          _cimg_watershed_init(is_nx && is_py,nx,py,z);
          _cimg_watershed_init(is_px && is_ny,px,ny,z);
          _cimg_watershed_init(is_nx && is_ny,nx,ny,z);
          if (is_3d) {
            _cimg_watershed_init(is_px && is_pz,px,y,pz);
            _cimg_watershed_init(is_nx && is_pz,nx,y,pz);
            _cimg_watershed_init(is_px && is_nz,px,y,nz);
            _cimg_watershed_init(is_nx && is_nz,nx,y,nz);
            _cimg_watershed_init(is_py && is_pz,x,py,pz);
            _cimg_watershed_init(is_ny && is_pz,x,ny,pz);
            _cimg_watershed_init(is_py && is_nz,x,py,nz);
            _cimg_watershed_init(is_ny && is_nz,x,ny,nz);
            _cimg_watershed_init(is_px && is_py && is_pz,px,py,pz);
            _cimg_watershed_init(is_nx && is_py && is_pz,nx,py,pz);
            _cimg_watershed_init(is_px && is_ny && is_pz,px,ny,pz);
            _cimg_watershed_init(is_nx && is_ny && is_pz,nx,ny,pz);
            _cimg_watershed_init(is_px && is_py && is_nz,px,py,nz);
            _cimg_watershed_init(is_nx && is_py && is_nz,nx,py,nz);
            _cimg_watershed_init(is_px && is_ny && is_nz,px,ny,nz);
            _cimg_watershed_init(is_nx && is_ny && is_nz,nx,ny,nz);
          }
        }
        labels(x,y,z) = nb_seeds;
      }

      // Start watershed computation.
      while (sizeQ) {

        // Get and remove point with maximal priority from the queue.
        const int x = (int)Q(0,1), y = (int)Q(0,2), z = (int)Q(0,3);
        const unsigned int n = labels(x,y,z);
        px = x - 1; nx = x + 1;
        py = y - 1; ny = y + 1;
        pz = z - 1; nz = z + 1;
        is_px = px>=0; is_nx = nx<width();
        is_py = py>=0; is_ny = ny<height();
        is_pz = pz>=0; is_nz = nz<depth();

        // Check labels of the neighbors.
        Q._priority_queue_remove(sizeQ);

        unsigned int xs, ys, zs, ns, nmin = 0;
        float d, dmin = cimg::type<float>::inf();
        T nlabel = (T)0;
        _cimg_watershed_propagate(is_px,px,y,z);
        _cimg_watershed_propagate(is_nx,nx,y,z);
        _cimg_watershed_propagate(is_py,x,py,z);
        _cimg_watershed_propagate(is_ny,x,ny,z);
        if (is_3d) {
          _cimg_watershed_propagate(is_pz,x,y,pz);
          _cimg_watershed_propagate(is_nz,x,y,nz);
        }
        if (is_high_connectivity) {
          _cimg_watershed_propagate(is_px && is_py,px,py,z);
          _cimg_watershed_propagate(is_nx && is_py,nx,py,z);
          _cimg_watershed_propagate(is_px && is_ny,px,ny,z);
          _cimg_watershed_propagate(is_nx && is_ny,nx,ny,z);
          if (is_3d) {
            _cimg_watershed_propagate(is_px && is_pz,px,y,pz);
            _cimg_watershed_propagate(is_nx && is_pz,nx,y,pz);
            _cimg_watershed_propagate(is_px && is_nz,px,y,nz);
            _cimg_watershed_propagate(is_nx && is_nz,nx,y,nz);
            _cimg_watershed_propagate(is_py && is_pz,x,py,pz);
            _cimg_watershed_propagate(is_ny && is_pz,x,ny,pz);
            _cimg_watershed_propagate(is_py && is_nz,x,py,nz);
            _cimg_watershed_propagate(is_ny && is_nz,x,ny,nz);
            _cimg_watershed_propagate(is_px && is_py && is_pz,px,py,pz);
            _cimg_watershed_propagate(is_nx && is_py && is_pz,nx,py,pz);
            _cimg_watershed_propagate(is_px && is_ny && is_pz,px,ny,pz);
            _cimg_watershed_propagate(is_nx && is_ny && is_pz,nx,ny,pz);
            _cimg_watershed_propagate(is_px && is_py && is_nz,px,py,nz);
            _cimg_watershed_propagate(is_nx && is_py && is_nz,nx,py,nz);
            _cimg_watershed_propagate(is_px && is_ny && is_nz,px,ny,nz);
            _cimg_watershed_propagate(is_nx && is_ny && is_nz,nx,ny,nz);
          }
        }
        (*this)(x,y,z) = nlabel;
        labels(x,y,z) = ++nmin;
      }
      return *this;
    }

    //! Compute watershed transform \newinstance.
    template<typename t>
    CImg<T> get_watershed(const CImg<t>& priority, const bool is_high_connectivity=false) const {
      return (+*this).watershed(priority,is_high_connectivity);
    }

    // [internal] Insert/Remove items in priority queue, for watershed/distance transforms.
    template<typename tq, typename tv>
    bool _priority_queue_insert(CImg<tq>& is_queued, unsigned int& siz, const tv value,
                                const unsigned int x, const unsigned int y, const unsigned int z,
                                const unsigned int n=1) {
      if (is_queued(x,y,z)) return false;
      is_queued(x,y,z) = (tq)n;
      if (++siz>=_width) { if (!is_empty()) resize(_width*2,4,1,1,0); else assign(64,4); }
      (*this)(siz - 1,0) = (T)value;
      (*this)(siz - 1,1) = (T)x;
      (*this)(siz - 1,2) = (T)y;
      (*this)(siz - 1,3) = (T)z;
      for (unsigned int pos = siz - 1, par = 0; pos && value>(tv)(*this)(par=(pos + 1)/2 - 1,0); pos = par) {
        cimg::swap((*this)(pos,0),(*this)(par,0));
        cimg::swap((*this)(pos,1),(*this)(par,1));
        cimg::swap((*this)(pos,2),(*this)(par,2));
        cimg::swap((*this)(pos,3),(*this)(par,3));
      }
      return true;
    }

    CImg<T>& _priority_queue_remove(unsigned int& siz) {
      (*this)(0,0) = (*this)(--siz,0);
      (*this)(0,1) = (*this)(siz,1);
      (*this)(0,2) = (*this)(siz,2);
      (*this)(0,3) = (*this)(siz,3);
      const float value = (*this)(0,0);
      unsigned int pos = 0, swap = 0;
      do {
        const unsigned int left = 2*pos + 1, right = left + 1;
        if (right<siz && value<(*this)(right,0)) swap = (*this)(left,0)>(*this)(right,0)?left:right;
        else if (left<siz && value<(*this)(left,0)) swap = left;
        else break;
        cimg::swap((*this)(pos,0),(*this)(swap,0));
        cimg::swap((*this)(pos,1),(*this)(swap,1));
        cimg::swap((*this)(pos,2),(*this)(swap,2));
        cimg::swap((*this)(pos,3),(*this)(swap,3));
        pos = swap;
      } while (true);
      return *this;
    }

    //! Apply recursive Deriche filter.
    /**
       \param sigma Standard deviation of the filter.
       \param order Order of the filter. Can be <tt>{ 0=smooth-filter | 1=1st-derivative | 2=2nd-derivative }</tt>.
       \param axis Axis along which the filter is computed. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
       \param boundary_conditions Boundary conditions.
         Can be <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
    **/
    CImg<T>& deriche(const float sigma, const unsigned int order=0, const char axis='x',
                     const unsigned int boundary_conditions=1) {
#define _cimg_deriche_apply \
  CImg<doubleT> Y(N); \
  double *ptrY = Y._data, yb = 0, yp = 0; \
  T xp = (T)0; \
  if (boundary_conditions) { xp = *ptrX; yb = yp = (double)(coefp*xp); } \
  for (int m = 0; m<N; ++m) { \
    const T xc = *ptrX; ptrX+=off; \
    const double yc = *(ptrY++) = (double)(a0*xc + a1*xp - b1*yp - b2*yb); \
    xp = xc; yb = yp; yp = yc; \
  } \
  T xn = (T)0, xa = (T)0; \
  double yn = 0, ya = 0; \
  if (boundary_conditions) { xn = xa = *(ptrX - off); yn = ya = (double)coefn*xn; } \
  for (int n = N - 1; n>=0; --n) { \
    const T xc = *(ptrX-=off); \
    const double yc = (double)(a2*xn + a3*xa - b1*yn - b2*ya); \
    xa = xn; xn = xc; ya = yn; yn = yc; \
    *ptrX = (T)(*(--ptrY)+yc); \
  }

      if (order>2)
        throw CImgArgumentException(_cimg_instance
                                    "deriche(): Invalid specified order '%d' "
                                    "('order' can be { 0=smoothing | 1=1st-derivative | 2=2nd-derivative }).",
                                    cimg_instance,
                                    order);

      const char naxis = cimg::lowercase(axis);
      if (naxis!='x' && naxis!='y' && naxis!='z' && naxis!='c')
        throw CImgArgumentException(_cimg_instance
                                    "deriche(): Invalid specified axis '%c'.",
                                    cimg_instance,
                                    axis);
      const double
        nsigma = sigma>=0?sigma:-sigma*(naxis=='x'?_width:
                                        naxis=='y'?_height:
                                        naxis=='z'?_depth:_spectrum)/100,
        nnsigma = nsigma<0.1f?0.1f:nsigma;

      if (is_empty() || (nsigma<0.1f && !order)) return *this;
      if (boundary_conditions>1) {
        const int w = width(), h = height(), d = depth(), s = spectrum(), border = (int)cimg::round(1 + 3*nnsigma);
        switch (naxis) {
        case 'x' :
          return draw_image(get_resize(w + 2*border,h,d,s,0,boundary_conditions,0.5).
                            deriche(nnsigma,order,naxis,1).columns(border,w - 1 + border));
        case 'y' :
          return draw_image(get_resize(w,h + 2*border,d,s,0,boundary_conditions,0,0.5).
                            deriche(nnsigma,order,naxis,1).rows(border,h - 1 + border));
        case 'z' :
          return draw_image(get_resize(w,h,d + 2*border,s,0,boundary_conditions,0,0,0.5).
                            deriche(nnsigma,order,naxis,1).slices(border,d - 1 + border));
        default :
          return draw_image(get_resize(w,h,d,s + 2*border,0,boundary_conditions,0,0,0,0.5).
                            deriche(nnsigma,order,naxis,1).channels(border,d - 1 + border));
        }
      }

      const double
        alpha = 1.695f/nnsigma,
        ema = std::exp(-alpha),
        ema2 = std::exp(-2*alpha),
        b1 = -2*ema,
        b2 = ema2;
      double a0 = 0, a1 = 0, a2 = 0, a3 = 0, coefp = 0, coefn = 0;
      switch (order) {
      case 0 : {
        const double k = (1-ema)*(1-ema)/(1 + 2*alpha*ema-ema2);
        a0 = k;
        a1 = k*(alpha - 1)*ema;
        a2 = k*(alpha + 1)*ema;
        a3 = -k*ema2;
      } break;
      case 1 : {
        const double k = -(1-ema)*(1-ema)*(1-ema)/(2*(ema + 1)*ema);
        a0 = a3 = 0;
        a1 = k*ema;
        a2 = -a1;
      } break;
      default : {
        const double
          ea = std::exp(-alpha),
          k = -(ema2 - 1)/(2*alpha*ema),
          kn = (-2*(-1 + 3*ea - 3*ea*ea + ea*ea*ea)/(3*ea + 1 + 3*ea*ea + ea*ea*ea));
        a0 = kn;
        a1 = -kn*(1 + k*alpha)*ema;
        a2 = kn*(1 - k*alpha)*ema;
        a3 = -kn*ema2;
      } break;
      }

      coefp = (a0 + a1)/(1 + b1 + b2);
      coefn = (a2 + a3)/(1 + b1 + b2);
      switch (naxis) {
      case 'x' : {
        const int N = width();
        const ulongT off = 1U;
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forYZC(*this,y,z,c) { T *ptrX = data(0,y,z,c); _cimg_deriche_apply; }
      } break;
      case 'y' : {
        const int N = height();
        const ulongT off = (ulongT)_width;
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forXZC(*this,x,z,c) { T *ptrX = data(x,0,z,c); _cimg_deriche_apply; }
      } break;
      case 'z' : {
        const int N = depth();
        const ulongT off = (ulongT)_width*_height;
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forXYC(*this,x,y,c) { T *ptrX = data(x,y,0,c); _cimg_deriche_apply; }
      } break;
      default : {
        const int N = spectrum();
        const ulongT off = (ulongT)_width*_height*_depth;
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forXYZ(*this,x,y,z) { T *ptrX = data(x,y,z,0); _cimg_deriche_apply; }
      }
      }
      return *this;
    }

    //! Apply recursive Deriche filter \newinstance.
    CImg<Tfloat> get_deriche(const float sigma, const unsigned int order=0, const char axis='x',
                             const unsigned int boundary_conditions=1) const {
      return CImg<Tfloat>(*this,false).deriche(sigma,order,axis,boundary_conditions);
    }

    // [internal] Apply a recursive filter (used by CImg<T>::vanvliet()).
    /*
       \param ptr the pointer of the data
       \param filter the coefficient of the filter in the following order [n,n - 1,n - 2,n - 3].
       \param N size of the data
       \param off the offset between two data point
       \param order the order of the filter 0 (smoothing), 1st derivative, 2nd derivative, 3rd derivative
       \param boundary_conditions Boundary conditions.
         Can be <tt>{ 0=dirichlet | 1=neumann }</tt>.
       \note Boundary condition using B. Triggs method (IEEE trans on Sig Proc 2005).
    */
    static void _cimg_recursive_apply(T *data, const double filter[], const int N, const ulongT off,
                                      const unsigned int order, const bool boundary_conditions) {
      double val[4] = {}; // res[n,n - 1,n - 2,n - 3,..] or res[n,n + 1,n + 2,n + 3,..]
      const double
        sumsq = filter[0], sum = sumsq * sumsq,
        a1 = filter[1], a2 = filter[2], a3 = filter[3],
        scaleM = 1. / ( (1. + a1 - a2 + a3) * (1. - a1 - a2 - a3) * (1. + a2 + (a1 - a3) * a3) );
      double M[9]; // Triggs matrix
      M[0] = scaleM * (-a3 * a1 + 1. - a3 * a3 - a2);
      M[1] = scaleM * (a3 + a1) * (a2 + a3 * a1);
      M[2] = scaleM * a3 * (a1 + a3 * a2);
      M[3] = scaleM * (a1 + a3 * a2);
      M[4] = -scaleM * (a2 - 1.) * (a2 + a3 * a1);
      M[5] = -scaleM * a3 * (a3 * a1 + a3 * a3 + a2 - 1.);
      M[6] = scaleM * (a3 * a1 + a2 + a1 * a1 - a2 * a2);
      M[7] = scaleM * (a1 * a2 + a3 * a2 * a2 - a1 * a3 * a3 - a3 * a3 * a3 - a3 * a2 + a3);
      M[8] = scaleM * a3 * (a1 + a3 * a2);
      switch (order) {
      case 0 : {
        const double iplus = (boundary_conditions?data[(N - 1)*off]:(T)0);
        for (int pass = 0; pass<2; ++pass) {
          if (!pass) {
            for (int k = 1; k<4; ++k) val[k] = (boundary_conditions?*data/sumsq:0);
          } else {
            // Apply Triggs boundary conditions.
            const double
              uplus = iplus/(1. - a1 - a2 - a3), vplus = uplus/(1. - a1 - a2 - a3),
              unp  = val[1] - uplus, unp1 = val[2] - uplus, unp2 = val[3] - uplus;
            val[0] = (M[0] * unp + M[1] * unp1 + M[2] * unp2 + vplus) * sum;
            val[1] = (M[3] * unp + M[4] * unp1 + M[5] * unp2 + vplus) * sum;
            val[2] = (M[6] * unp + M[7] * unp1 + M[8] * unp2 + vplus) * sum;
            *data = (T)val[0];
            data -= off;
            for (int k = 3; k>0; --k) val[k] = val[k - 1];
          }
          for (int n = pass; n<N; ++n) {
            val[0] = (*data);
            if (pass) val[0] *= sum;
            for (int k = 1; k<4; ++k) val[0] += val[k] * filter[k];
            *data = (T)val[0];
            if (!pass) data += off; else data -= off;
            for (int k = 3; k>0; --k) val[k] = val[k - 1];
          }
          if (!pass) data -= off;
        }
      } break;
      case 1 : {
        double x[3]; // [front,center,back]
        for (int pass = 0; pass<2; ++pass) {
          if (!pass) {
            for (int k = 0; k<3; ++k) x[k] = (boundary_conditions?*data:(T)0);
            for (int k = 0; k<4; ++k) val[k] = 0;
          } else {
            // Apply Triggs boundary conditions.
            const double
              unp  = val[1], unp1 = val[2], unp2 = val[3];
            val[0] = (M[0] * unp + M[1] * unp1 + M[2] * unp2) * sum;
            val[1] = (M[3] * unp + M[4] * unp1 + M[5] * unp2) * sum;
            val[2] = (M[6] * unp + M[7] * unp1 + M[8] * unp2) * sum;
            *data = (T)val[0];
            data -= off;
            for (int k = 3; k>0; --k) val[k] = val[k - 1];
          }
          for (int n = pass; n<N - 1; ++n) {
            if (!pass) {
              x[0] = *(data + off);
              val[0] = 0.5f * (x[0] - x[2]);
            } else val[0] = (*data) * sum;
            for (int k = 1; k<4; ++k) val[0] += val[k] * filter[k];
            *data = (T)val[0];
            if (!pass) {
              data += off;
              for (int k = 2; k>0; --k) x[k] = x[k - 1];
            } else { data-=off;}
            for (int k = 3; k>0; --k) val[k] = val[k - 1];
          }
          *data = (T)0;
        }
      } break;
      case 2: {
        double x[3]; // [front,center,back]
        for (int pass = 0; pass<2; ++pass) {
          if (!pass) {
            for (int k = 0; k<3; ++k) x[k] = (boundary_conditions?*data:(T)0);
            for (int k = 0; k<4; ++k) val[k] = 0;
          } else {
            // Apply Triggs boundary conditions.
            const double
              unp  = val[1], unp1 = val[2], unp2 = val[3];
            val[0] = (M[0] * unp + M[1] * unp1 + M[2] * unp2) * sum;
            val[1] = (M[3] * unp + M[4] * unp1 + M[5] * unp2) * sum;
            val[2] = (M[6] * unp + M[7] * unp1 + M[8] * unp2) * sum;
            *data = (T)val[0];
            data -= off;
            for (int k = 3; k>0; --k) val[k] = val[k - 1];
          }
          for (int n = pass; n<N - 1; ++n) {
            if (!pass) { x[0] = *(data + off); val[0] = (x[1] - x[2]); }
            else { x[0] = *(data - off); val[0] = (x[2] - x[1]) * sum; }
            for (int k = 1; k<4; ++k) val[0] += val[k]*filter[k];
            *data = (T)val[0];
            if (!pass) data += off; else data -= off;
            for (int k = 2; k>0; --k) x[k] = x[k - 1];
            for (int k = 3; k>0; --k) val[k] = val[k - 1];
          }
          *data = (T)0;
        }
      } break;
      case 3: {
        double x[3]; // [front,center,back]
        for (int pass = 0; pass<2; ++pass) {
          if (!pass) {
            for (int k = 0; k<3; ++k) x[k] = (boundary_conditions?*data:(T)0);
            for (int k = 0; k<4; ++k) val[k] = 0;
          } else {
            // Apply Triggs boundary conditions.
            const double
              unp = val[1], unp1 = val[2], unp2 = val[3];
            val[0] = (M[0] * unp + M[1] * unp1 + M[2] * unp2) * sum;
            val[1] = (M[3] * unp + M[4] * unp1 + M[5] * unp2) * sum;
            val[2] = (M[6] * unp + M[7] * unp1 + M[8] * unp2) * sum;
            *data = (T)val[0];
            data -= off;
            for (int k = 3; k>0; --k) val[k] = val[k - 1];
          }
          for (int n = pass; n<N - 1; ++n) {
            if (!pass) { x[0] = *(data + off); val[0] = (x[0] - 2*x[1] + x[2]); }
            else { x[0] = *(data - off); val[0] = 0.5f * (x[2] - x[0]) * sum; }
            for (int k = 1; k<4; ++k) val[0] += val[k] * filter[k];
            *data = (T)val[0];
            if (!pass) data += off; else data -= off;
            for (int k = 2; k>0; --k) x[k] = x[k - 1];
            for (int k = 3; k>0; --k) val[k] = val[k - 1];
          }
          *data = (T)0;
        }
      } break;
      }
    }

    //! Van Vliet recursive Gaussian filter.
    /**
       \param sigma standard deviation of the Gaussian filter
       \param order the order of the filter 0,1,2,3
       \param axis  Axis along which the filter is computed. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
       \param boundary_conditions Boundary conditions.
         Can be <tt>{ 0=dirichlet | 1=neumann | 2=periodic | 3=mirror }</tt>.
       \note dirichlet boundary condition has a strange behavior

       I.T. Young, L.J. van Vliet, M. van Ginkel, Recursive Gabor filtering.
       IEEE Trans. Sig. Proc., vol. 50, pp. 2799-2805, 2002.

       (this is an improvement over Young-Van Vliet, Sig. Proc. 44, 1995)

       Boundary conditions (only for order 0) using Triggs matrix, from
       B. Triggs and M. Sdika. Boundary conditions for Young-van Vliet
       recursive filtering. IEEE Trans. Signal Processing,
       vol. 54, pp. 2365-2367, 2006.
    **/
    CImg<T>& vanvliet(const float sigma, const unsigned int order, const char axis='x',
                      const unsigned int boundary_conditions=1) {

      if (order>2)
        throw CImgArgumentException(_cimg_instance
                                    "deriche(): Invalid specified order '%d' "
                                    "('order' can be { 0=smoothing | 1=1st-derivative | 2=2nd-derivative }).",
                                    cimg_instance,
                                    order);

      const char naxis = cimg::lowercase(axis);
      if (naxis!='x' && naxis!='y' && naxis!='z' && naxis!='c')
        throw CImgArgumentException(_cimg_instance
                                    "deriche(): Invalid specified axis '%c'.",
                                    cimg_instance,
                                    axis);
      const double
        nsigma = sigma>=0?sigma:-sigma*(naxis=='x'?_width:
                                        naxis=='y'?_height:
                                        naxis=='z'?_depth:_spectrum)/100,
        nnsigma = nsigma<0.5f?0.5f:nsigma;

      if (is_empty() || (nsigma<0.1f && !order)) return *this;
      if (nsigma<0.5f) return deriche(nsigma,order,axis,boundary_conditions);
      if (!cimg::type<T>::is_float()
#if cimg_is_float16==1
          || cimg::type<T>::string()==cimg::type<cimg_float16>::string()
#endif
          )
        return CImg<Tfloat>(*this,false).vanvliet(sigma,order,axis,boundary_conditions).move_to(*this);

      if (boundary_conditions>1) {
        const int w = width(), h = height(), d = depth(), s = spectrum(), border = (int)cimg::round(1 + 3*nnsigma);
        switch (naxis) {
        case 'x' :
          return draw_image(get_resize(w + 2*border,h,d,s,0,boundary_conditions,0.5).
                            vanvliet(nnsigma,order,naxis,1).columns(border,w - 1 + border));
        case 'y' :
          return draw_image(get_resize(w,h + 2*border,d,s,0,boundary_conditions,0,0.5).
                            vanvliet(nnsigma,order,naxis,1).rows(border,h - 1 + border));
        case 'z' :
          return draw_image(get_resize(w,h,d + 2*border,s,0,boundary_conditions,0,0,0.5).
                            vanvliet(nnsigma,order,naxis,1).slices(border,d - 1 + border));
        default :
          return draw_image(get_resize(w,h,d,s + 2*border,0,boundary_conditions,0,0,0,0.5).
                            vanvliet(nnsigma,order,naxis,1).channels(border,d - 1 + border));
        }
      }

      const double
        m0 = 1.16680, m1 = 1.10783, m2 = 1.40586,
        m1sq = m1 * m1, m2sq = m2 * m2,
        q = (nnsigma<3.556?-0.2568 + 0.5784*nnsigma + 0.0561*nnsigma*nnsigma:2.5091 + 0.9804*(nnsigma - 3.556)),
        qsq = q * q,
        scale = (m0 + q) * (m1sq + m2sq + 2 * m1 * q + qsq),
        b1 = -q * (2 * m0 * m1 + m1sq + m2sq + (2 * m0 + 4 * m1) * q + 3 * qsq) / scale,
        b2 = qsq * (m0 + 2 * m1 + 3 * q) / scale,
        b3 = -qsq * q / scale,
        B = ( m0 * (m1sq + m2sq) ) / scale;
      double filter[4];
      filter[0] = B; filter[1] = -b1; filter[2] = -b2; filter[3] = -b3;
      switch (naxis) {
      case 'x' : {
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forYZC(*this,y,z,c)
          _cimg_recursive_apply(data(0,y,z,c),filter,_width,1U,order,boundary_conditions);
      } break;
      case 'y' : {
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forXZC(*this,x,z,c)
          _cimg_recursive_apply(data(x,0,z,c),filter,_height,(ulongT)_width,order,boundary_conditions);
      } break;
      case 'z' : {
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forXYC(*this,x,y,c)
          _cimg_recursive_apply(data(x,y,0,c),filter,_depth,(ulongT)_width*_height,
                                order,boundary_conditions);
      } break;
      default : {
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*256 &&
                                                                   _height*_depth*_spectrum>=16))
        cimg_forXYZ(*this,x,y,z)
          _cimg_recursive_apply(data(x,y,z,0),filter,_spectrum,(ulongT)_width*_height*_depth,
                                order,boundary_conditions);
      }
      }
      return *this;
    }

    //! Blur image using Van Vliet recursive Gaussian filter. \newinstance.
    CImg<Tfloat> get_vanvliet(const float sigma, const unsigned int order, const char axis='x',
                              const unsigned int boundary_conditions=1) const {
      return CImg<Tfloat>(*this,false).vanvliet(sigma,order,axis,boundary_conditions);
    }

#endif
