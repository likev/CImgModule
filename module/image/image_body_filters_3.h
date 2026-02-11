#ifndef CIMG_MODULE_IMAGE_BODY_FILTERS_3_H
#define CIMG_MODULE_IMAGE_BODY_FILTERS_3_H
          'guide' may have a last channel with boolean values (0=false | other=true) that
          tells for each pixel if its correspondence vector is constrained to its initial value (constraint mask).
        \param[out] matching_score Returned as the image of matching scores.
    **/
    template<typename t1, typename t2>
    CImg<T>& matchpatch(const CImg<T>& patch_image,
                        const unsigned int patch_width,
                        const unsigned int patch_height,
                        const unsigned int patch_depth,
                        const unsigned int nb_iterations,
                        const unsigned int nb_randoms,
                        const float patch_penalization,
                        const CImg<t1> &guide,
                        CImg<t2> &matching_score) {
      return get_matchpatch(patch_image,patch_width,patch_height,patch_depth,
                            nb_iterations,nb_randoms,patch_penalization,guide,matching_score).move_to(*this);
    }

    //! Compute correspondence map between two images, using the patch-match algorithm \newinstance.
    template<typename t1, typename t2>
    CImg<intT> get_matchpatch(const CImg<T>& patch_image,
                              const unsigned int patch_width,
                              const unsigned int patch_height,
                              const unsigned int patch_depth,
                              const unsigned int nb_iterations,
                              const unsigned int nb_randoms,
                              const float patch_penalization,
                              const CImg<t1> &guide,
                              CImg<t2> &matching_score) const {
      return _matchpatch(patch_image,patch_width,patch_height,patch_depth,
                         nb_iterations,nb_randoms,patch_penalization,
                         guide,true,matching_score);
    }

    //! Compute correspondence map between two images, using the patch-match algorithm \overloading.
    template<typename t>
    CImg<T>& matchpatch(const CImg<T>& patch_image,
                        const unsigned int patch_width,
                        const unsigned int patch_height,
                        const unsigned int patch_depth,
                        const unsigned int nb_iterations=5,
                        const unsigned int nb_randoms=5,
                        const float patch_penalization=0,
                        const CImg<t> &guide=CImg<t>::const_empty()) {
      return get_matchpatch(patch_image,patch_width,patch_height,patch_depth,
                            nb_iterations,nb_randoms,patch_penalization,guide).move_to(*this);
    }

    //! Compute correspondence map between two images, using the patch-match algorithm \overloading.
    template<typename t>
    CImg<intT> get_matchpatch(const CImg<T>& patch_image,
                              const unsigned int patch_width,
                              const unsigned int patch_height,
                              const unsigned int patch_depth,
                              const unsigned int nb_iterations=5,
                              const unsigned int nb_randoms=5,
                              const float patch_penalization=0,
                              const CImg<t> &guide=CImg<t>::const_empty()) const {
      CImg<T> matching_score;
      return _matchpatch(patch_image,patch_width,patch_height,patch_depth,
                         nb_iterations,nb_randoms,patch_penalization,guide,false,matching_score);
    }

    template<typename t1, typename t2>
    CImg<intT> _matchpatch(const CImg<T>& patch_image,
                           const unsigned int patch_width,
                           const unsigned int patch_height,
                           const unsigned int patch_depth,
                           const unsigned int nb_iterations,
                           const unsigned int nb_randoms,
                           const float patch_penalization,
                           const CImg<t1> &guide,
                           const bool is_matching_score,
                           CImg<t2> &matching_score) const {
      if (is_empty()) return CImg<intT>::const_empty();
      if (patch_image._spectrum!=_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "matchpatch(): Instance image and specified patch image (%u,%u,%u,%u,%p) "
                                    "have different spectrums.",
                                    cimg_instance,
                                    patch_image._width,patch_image._height,patch_image._depth,patch_image._spectrum,
                                    patch_image._data);
      if (patch_width>_width || patch_height>_height || patch_depth>_depth)
        throw CImgArgumentException(_cimg_instance
                                    "matchpatch(): Specified patch size %ux%ux%u is bigger than the dimensions "
                                    "of the instance image.",
                                    cimg_instance,patch_width,patch_height,patch_depth);
      if (patch_width>patch_image._width || patch_height>patch_image._height || patch_depth>patch_image._depth)
        throw CImgArgumentException(_cimg_instance
                                    "matchpatch(): Specified patch size %ux%ux%u is bigger than the dimensions "
                                    "of the patch image image (%u,%u,%u,%u,%p).",
                                    cimg_instance,patch_width,patch_height,patch_depth,
                                    patch_image._width,patch_image._height,patch_image._depth,patch_image._spectrum,
                                    patch_image._data);
      const unsigned int
        _constraint = patch_image._depth>1?3:2,
        constraint = guide._spectrum>_constraint?_constraint:0;

      if (guide &&
          (guide._width!=_width || guide._height!=_height || guide._depth!=_depth || guide._spectrum<_constraint))
        throw CImgArgumentException(_cimg_instance
                                    "matchpatch(): Specified guide (%u,%u,%u,%u,%p) has invalid dimensions "
                                    "considering instance and patch image (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data,
                                    patch_image._width,patch_image._height,patch_image._depth,patch_image._spectrum,
                                    patch_image._data);

      CImg<intT> a_map(_width,_height,_depth,patch_image._depth>1?3:2);
      CImg<ucharT> is_updated(_width,_height,_depth,1,3);
      CImg<floatT> score(_width,_height,_depth), penalty;
      const float _patch_penalization = cimg::abs(patch_penalization);
      const bool allow_identity = patch_penalization>=0;
      if (_patch_penalization!=0)
        penalty.assign(patch_image._width,patch_image._height,patch_image._depth,1,0);

      const int
        psizew = (int)patch_width,  psizew1 = psizew/2, psizew2 = psizew - psizew1 - 1,
        psizeh = (int)patch_height, psizeh1 = psizeh/2, psizeh2 = psizeh - psizeh1 - 1,
        psized = (int)patch_depth,  psized1 = psized/2, psized2 = psized - psized1 - 1;

      // Interleave image buffers to speed up patch comparison (more cache-friendly).
      CImg<T> in_this = get_permute_axes("cxyz");
      in_this._width = _width*_spectrum;
      in_this._height = _height;
      in_this._depth = _depth;
      in_this._spectrum = 1;
      CImg<T> in_patch = patch_image.get_permute_axes("cxyz");
      in_patch._width = patch_image._width*patch_image._spectrum;
      in_patch._height = patch_image._height;
      in_patch._depth = patch_image._depth;
      in_patch._spectrum = 1;

      if (_depth>1 || patch_image._depth>1) { // 3D version

        // Initialize correspondence map.
        if (guide)
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if_size(_width,64))
          cimg_forXYZ(*this,x,y,z) { // User-defined initialization
            const int
              cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
              cz1 = z<=psized1?z:(z<depth()  - psized2?psized1:psized + z - depth()),  cz2 = psized - cz1 - 1,
              u = cimg::cut((int)guide(x,y,z,0),cx1,patch_image.width() - 1 - cx2),
              v = cimg::cut((int)guide(x,y,z,1),cy1,patch_image.height() - 1 - cy2),
              w = cimg::cut((int)guide(x,y,z,2),cz1,patch_image.depth() - 1 - cz2);
            a_map(x,y,z,0) = u;
            a_map(x,y,z,1) = v;
            a_map(x,y,z,2) = w;
            score(x,y,z) = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                       x - cx1,y - cy1,z - cz1,
                                       u - cx1,v - cy1,w - cz1,
                                       u,v,w,0,allow_identity,cimg::type<float>::inf());
          } else cimg_pragma_openmp(parallel cimg_openmp_if_size(_width,64)) {
            cimg_uint64 rng = (cimg::_rand(),cimg::rng());
#if cimg_use_openmp!=0
            rng+=omp_get_thread_num();
#endif
            cimg_pragma_openmp(for cimg_openmp_collapse(2))
            cimg_forXYZ(*this,x,y,z) { // Random initialization
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
                cz1 = z<=psized1?z:(z<depth()  - psized2?psized1:psized + z - depth()),  cz2 = psized - cz1 - 1,
                u = (int)cimg::round(cimg::rand(cx1,patch_image.width() - 1 - cx2,&rng)),
                v = (int)cimg::round(cimg::rand(cy1,patch_image.height() - 1 - cy2,&rng)),
                w = (int)cimg::round(cimg::rand(cz1,patch_image.depth() - 1 - cz2,&rng));
              a_map(x,y,z,0) = u;
              a_map(x,y,z,1) = v;
              a_map(x,y,z,2) = w;
              score(x,y,z) = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                         x - cx1,y - cy1,z - cz1,
                                         u - cx1,v - cy1,w - cz1,
                                         u,v,w,0,allow_identity,cimg::type<float>::inf());
            }
            cimg::srand(rng);
          }

        // Start iteration loop.
        cimg_abort_init;
        for (unsigned int iter = 0; iter<nb_iterations; ++iter) {
          cimg_abort_test;
          const bool is_backward = iter&1, is_forward = !is_backward;
          const unsigned int cmask = is_backward?1:2, nmask = 3 - cmask;

          cimg_pragma_openmp(parallel cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64)) {
            cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
            rng+=omp_get_thread_num();
#endif
            cimg_pragma_openmp(for cimg_openmp_collapse(2))
            cimg_forXYZ(*this,X,Y,Z) {
              const int
                x = is_backward?width() - 1 - X:X,
                y = is_backward?height() - 1 - Y:Y,
                z = is_backward?depth() - 1 - Z:Z;
              if (score(x,y,z)<=1e-5 || (constraint && guide(x,y,z,constraint)!=0)) continue;
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
                cz1 = z<=psized1?z:(z<depth()  - psized2?psized1:psized + z - depth()),  cz2 = psized - cz1 - 1,
                xp = x - cx1,
                yp = y - cy1,
                zp = z - cz1;

              int best_u = a_map(x,y,z,0), best_v = a_map(x,y,z,1), best_w = a_map(x,y,z,2), u, v, w;
              const float best_score0 = score(x,y,z);
              float best_score = best_score0, s;

              if (is_forward && x>0 && (is_updated(x - 1,y,z)&cmask)) { // Compare with left neighbor
                u = a_map(x - 1,y,z,0);
                v = a_map(x - 1,y,z,1);
                w = a_map(x - 1,y,z,2);
                if (u>=cx1 - 1 && u<patch_image.width() - 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u + 1 - cx1,v - cy1,w - cz1,
                                  u,v,w,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u + 1; best_v = v; best_w = w; best_score = s; }
                }
              }
              if (is_forward && y>0 && (is_updated(x,y - 1,z)&cmask)) { // Compare with up neighbor
                u = a_map(x,y - 1,z,0);
                v = a_map(x,y - 1,z,1);
                w = a_map(x,y - 1,z,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 - 1 && v<patch_image.height() - 1 - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v + 1 - cy1,w - cz1,
                                  u,v,w,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u; best_v = v + 1; best_w = w; best_score = s; }
                }
              }
              if (is_forward && z>0 && (is_updated(x,y,z - 1)&cmask)) { // Compare with backward neighbor
                u = a_map(x,y,z - 1,0);
                v = a_map(x,y,z - 1,1);
                w = a_map(x,y,z - 1,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 - 1 && w<patch_image.depth() - 1 - cz2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v - cy1,w + 1 - cz1,
                                  u,v,w,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u; best_v = v; best_w = w + 1; best_score = s; }
                }
              }
              if (is_backward && x<width() - 1 && (is_updated(x + 1,y,z)&cmask)) { // Compare with right neighbor
                u = a_map(x + 1,y,z,0);
                v = a_map(x + 1,y,z,1);
                w = a_map(x + 1,y,z,2);
                if (u>=cx1 + 1 && u<patch_image.width() + 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - 1 - cx1,v - cy1,w - cz1,
                                  u,v,w,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u - 1; best_v = v; best_w = w; best_score = s; }
                }
              }
              if (is_backward && y<height() - 1 && (is_updated(x,y + 1,z)&cmask)) { // Compare with bottom neighbor
                u = a_map(x,y + 1,z,0);
                v = a_map(x,y + 1,z,1);
                w = a_map(x,y + 1,z,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 + 1 && v<patch_image.height() + 1 - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v - 1 - cy1,w - cz1,
                                  u,v,w,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u; best_v = v - 1; best_w = w; best_score = s; }
                }
              }
              if (is_backward && z<depth() - 1 && (is_updated(x,y,z + 1)&cmask)) { // Compare with forward neighbor
                u = a_map(x,y,z + 1,0);
                v = a_map(x,y,z + 1,1);
                w = a_map(x,y,z + 1,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 + 1 && w<patch_image.depth() + 1 - cz2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v - cy1,w - 1 - cz1,
                                  u,v,w,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u; best_v = v; best_w = w - 1; best_score = s; }
                }
              }

              float
                dw = (float)patch_image.width(),
                dh = (float)patch_image.height(),
                dd = (float)patch_image.depth();
              for (unsigned int i = 0; i<nb_randoms; ++i) {
                u = (int)cimg::round(cimg::rand(std::max((float)cx1,best_u - dw),
                                                std::min(patch_image.width() - 1.f - cx2,best_u + dw),&rng));
                v = (int)cimg::round(cimg::rand(std::max((float)cy1,best_v - dh),
                                                std::min(patch_image.height() - 1.f - cy2,best_v + dh),&rng));
                w = (int)cimg::round(cimg::rand(std::max((float)cz1,best_w - dd),
                                                std::min(patch_image.depth() - 1.f - cz2,best_w + dd),&rng));
                if (u!=best_u || v!=best_v || w!=best_w) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                  xp,yp,zp,u - cx1,v - cy1,w - cz1,
                                  u,v,w,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u; best_v = v; best_w = w; best_score = s; }
                  dw = std::max(5.f,dw*0.5f); dh = std::max(5.f,dh*0.5f); dd = std::max(5.f,dd*0.5f);
                }
              }

              if (best_score<best_score0) {
                if (_patch_penalization!=0) {
                  float &p_penalty = penalty(a_map(x,y,z,0),a_map(x,y,z,1),a_map(x,y,z,2));
                  if (p_penalty) cimg_pragma_openmp(atomic) --p_penalty;
                }
                a_map(x,y,z,0) = best_u;
                a_map(x,y,z,1) = best_v;
                a_map(x,y,z,2) = best_w;
                score(x,y,z) = best_score;
                is_updated(x,y,z) = 3;
              } else is_updated(x,y,z)&=~nmask;
              if (_patch_penalization!=0) cimg_pragma_openmp(atomic) ++penalty(best_u,best_v,best_w);
            }
            cimg::srand(rng);
          }

          // Update score according to new penalties.
          if (penalty)
            cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64))
            cimg_forXYZ(score,x,y,z) {
              const float p_score = score(x,y,z);
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()),
                cz1 = z<=psized1?z:(z<depth()  - psized2?psized1:psized + z - depth()),
                xp = x - cx1,
                yp = y - cy1,
                zp = z - cz1,
                u = a_map(x,y,z,0),
                v = a_map(x,y,z,1),
                w = a_map(x,y,z,2);
              const float n_score = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,patch_depth,_spectrum,
                                                xp,yp,zp,u - cx1,v - cy1,w - cz1,
                                                u,v,w,_patch_penalization,allow_identity,cimg::type<float>::inf());
              if (n_score!=p_score) { score(x,y,z) = n_score; is_updated(x,y) = 3; }
            }
        }

      } else { // 2D version

        // Initialize correspondence map.
        if (guide)
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(_width,64))
          cimg_forXY(*this,x,y) { // User-defined initialization
            const int
              cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
              u = cimg::cut((int)guide(x,y,0),cx1,patch_image.width() - 1 - cx2),
              v = cimg::cut((int)guide(x,y,1),cy1,patch_image.height() - 1 - cy2);
            a_map(x,y,0) = u;
            a_map(x,y,1) = v;
            score(x,y) = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,_spectrum,
                                     x - cx1,y - cy1,u - cx1,v - cy1,
                                     u,v,0,allow_identity,cimg::type<float>::inf());
          } else cimg_pragma_openmp(parallel cimg_openmp_if_size(_width,64)) {
            cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
            rng+=omp_get_thread_num();
#endif
            cimg_pragma_openmp(for)
            cimg_forXY(*this,x,y) { // Random initialization
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
                u = (int)cimg::round(cimg::rand(cx1,patch_image.width() - 1 - cx2,&rng)),
                v = (int)cimg::round(cimg::rand(cy1,patch_image.height() - 1 - cy2,&rng));
              a_map(x,y,0) = u;
              a_map(x,y,1) = v;
              score(x,y) = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,_spectrum,
                                       x - cx1,y - cy1,u - cx1,v - cy1,
                                       u,v,0,allow_identity,cimg::type<float>::inf());
            }
            cimg::srand(rng);
          }

        // Start iteration loop.
        cimg_abort_init;
        for (unsigned int iter = 0; iter<nb_iterations; ++iter) {
          cimg_abort_test;
          const bool is_backward = iter&1, is_forward = !is_backward;
          const unsigned int cmask = is_backward?1:2, nmask = 3 - cmask;

          cimg_pragma_openmp(parallel cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64)) {
            cimg_uint64 rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
            rng+=omp_get_thread_num();
#endif
            cimg_pragma_openmp(for)
            cimg_forXY(*this,X,Y) {
              const int
                x = is_backward?width() - 1 - X:X,
                y = is_backward?height() - 1 - Y:Y;
              if (score(x,y)<=1e-5 || (constraint && guide(x,y,constraint)!=0)) continue;
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),  cx2 = psizew - cx1 - 1,
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
                xp = x - cx1,
                yp = y - cy1;

              int best_u = a_map(x,y,0), best_v = a_map(x,y,1), u, v;
              const float best_score0 = score(x,y);
              float best_score = best_score0, s;

              if (is_forward && x>0 && (is_updated(x - 1,y)&cmask)) { // Compare with left neighbor
                u = a_map(x - 1,y,0);
                v = a_map(x - 1,y,1);
                if (u>=cx1 - 1 && u<patch_image.width() - 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,_spectrum,
                                  xp,yp,u + 1 - cx1,v - cy1,
                                  u,v,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u + 1; best_v = v; best_score = s; }
                }
              }
              if (is_forward && y>0 && (is_updated(x,y - 1)&cmask)) { // Compare with up neighbor
                u = a_map(x,y - 1,0);
                v = a_map(x,y - 1,1);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 - 1 && v<patch_image.height() - 1 - cy2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,_spectrum,
                                  xp,yp,u - cx1,v + 1 - cy1,
                                  u,v,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u; best_v = v + 1; best_score = s; }
                }
              }
              if (is_backward && x<width() - 1 && (is_updated(x + 1,y)&cmask)) { // Compare with right neighbor
                u = a_map(x + 1,y,0);
                v = a_map(x + 1,y,1);
                if (u>=cx1 + 1 && u<patch_image.width() + 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,_spectrum,
                                  xp,yp,u - 1 - cx1,v - cy1,
                                  u,v,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u - 1; best_v = v; best_score = s; }
                }
              }
              if (is_backward && y<height() - 1 && (is_updated(x,y + 1)&cmask)) { // Compare with bottom neighbor
                u = a_map(x,y + 1,0);
                v = a_map(x,y + 1,1);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 + 1 && v<patch_image.height() + 1 - cy2) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,_spectrum,
                                  xp,yp,u - cx1,v - 1 - cy1,
                                  u,v,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u; best_v = v - 1; best_score = s; }
                }
              }

              float
                dw = (float)patch_image.width(),
                dh = (float)patch_image.height();
              for (unsigned int i = 0; i<nb_randoms; ++i) {
                u = (int)cimg::round(cimg::rand(std::max((float)cx1,best_u - dw),
                                                std::min(patch_image.width() - 1.f - cx2,best_u + dw),&rng));
                v = (int)cimg::round(cimg::rand(std::max((float)cy1,best_v - dh),
                                                std::min(patch_image.height() - 1.f - cy2,best_v + dh),&rng));
                if (u!=best_u || v!=best_v) {
                  s = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,_spectrum,
                                  xp,yp,u - cx1,v - cy1,
                                  u,v,_patch_penalization,allow_identity,best_score);
                  if (s<best_score) { best_u = u; best_v = v; best_score = s; }
                  dw = std::max(5.f,dw*0.5f); dh = std::max(5.f,dh*0.5f);
                }
              }

              if (best_score<best_score0) {
                if (_patch_penalization!=0) {
                  float &p_penalty = penalty(a_map(x,y,0),a_map(x,y,1));
                  if (p_penalty) cimg_pragma_openmp(atomic) --p_penalty;
                }
                a_map(x,y,0) = best_u;
                a_map(x,y,1) = best_v;
                score(x,y) = best_score;
                is_updated(x,y) = 3;
              } else is_updated(x,y)&=~nmask;
              if (_patch_penalization!=0) cimg_pragma_openmp(atomic) ++penalty(best_u,best_v);
            }
            cimg::srand(rng);
          }

          // Update score according to new penalties.
          if (penalty)
            cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*64))
            cimg_forXY(score,x,y) {
              const float p_score = score(x,y);
              const int
                cx1 = x<=psizew1?x:(x<width()  - psizew2?psizew1:psizew + x - width()),
                cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()),
                xp = x - cx1,
                yp = y - cy1,
                u = a_map(x,y,0),
                v = a_map(x,y,1);
              const float n_score = _matchpatch(in_this,in_patch,penalty,patch_width,patch_height,_spectrum,
                                                xp,yp,u - cx1,v - cy1,
                                                u,v,_patch_penalization,allow_identity,cimg::type<float>::inf());
              if (n_score!=p_score) { score(x,y) = n_score; is_updated(x,y) = 3; }
            }
        }
      }

      if (is_matching_score) score.move_to(matching_score);
      return a_map;
    }

    // Compute SSD between two patches in different images.
    static float _matchpatch(const CImg<T>& img1, const CImg<T>& img2, const CImg<floatT>& penalty,
                             const unsigned int psizew, const unsigned int psizeh,
                             const unsigned int psized, const unsigned int psizec,
                             const int x1, const int y1, const int z1,
                             const int x2, const int y2, const int z2,
                             const int xc, const int yc, const int zc,
                             const float patch_penalization,
                             const bool allow_identity,
                             const float max_score) { // 3D version
      if (!allow_identity && cimg::hypot((float)x1 - x2,(float)y1 - y2,(float)z1 - z2)<patch_penalization)
        return cimg::type<float>::inf();
      const T *p1 = img1.data(x1*psizec,y1,z1), *p2 = img2.data(x2*psizec,y2,z2);
      const unsigned int psizewc = psizew*psizec;
      const ulongT
        offx1 = (ulongT)img1._width - psizewc,
        offx2 = (ulongT)img2._width - psizewc,
        offy1 = (ulongT)img1._width*img1._height - (ulongT)psizeh*img1._width,
        offy2 = (ulongT)img2._width*img2._height - (ulongT)psizeh*img2._width;
      float ssd = 0;
      for (unsigned int k = 0; k<psized; ++k) {
        for (unsigned int j = 0; j<psizeh; ++j) {
          for (unsigned int i = 0; i<psizewc; ++i)
            ssd += cimg::sqr((Tfloat)*(p1++) - *(p2++));
          if (ssd>max_score) return max_score;
          p1+=offx1; p2+=offx2;
        }
        p1+=offy1; p2+=offy2;
      }
      return patch_penalization==0?ssd:cimg::sqr(std::sqrt(ssd) +
                                                 patch_penalization*psizewc*psizeh*psized*penalty(xc,yc,zc)/100);
    }

    static float _matchpatch(const CImg<T>& img1, const CImg<T>& img2, const CImg<floatT>& penalty,
                             const unsigned int psizew, const unsigned int psizeh, const unsigned int psizec,
                             const int x1, const int y1,
                             const int x2, const int y2,
                             const int xc, const int yc,
                             const float patch_penalization,
                             const bool allow_identity,
                             const float max_score) { // 2D version
      if (!allow_identity && cimg::hypot((float)x1 - x2,(float)y1 - y2)<patch_penalization)
        return cimg::type<float>::inf();
      const T *p1 = img1.data(x1*psizec,y1), *p2 = img2.data(x2*psizec,y2);
      const unsigned int psizewc = psizew*psizec;
      const ulongT
        offx1 = (ulongT)img1._width - psizewc,
        offx2 = (ulongT)img2._width - psizewc;
      float ssd = 0;
      for (unsigned int j = 0; j<psizeh; ++j) {
        for (unsigned int i = 0; i<psizewc; ++i)
          ssd += cimg::sqr((Tfloat)*(p1++) - *(p2++));
        if (ssd>max_score) return max_score;
        p1+=offx1; p2+=offx2;
      }
      return patch_penalization==0?ssd:cimg::sqr(std::sqrt(ssd) +
                                                 patch_penalization*psizewc*psizeh*penalty(xc,yc)/100);
    }

    //! Compute Euclidean distance function to a specified value.
    /**
        \param value Reference value.
        \param metric Type of metric. Can be <tt>{ 0=Chebyshev | 1=Manhattan | 2=Euclidean | 3=Squared-euclidean }</tt>.
        \note
        The distance transform implementation has been submitted by A. Meijster, and implements
        the article 'W.H. Hesselink, A. Meijster, J.B.T.M. Roerdink,
                     "A general algorithm for computing distance transforms in linear time.",
                     In: Mathematical Morphology and its Applications to Image and Signal Processing,
                     J. Goutsias, L. Vincent, and D.S. Bloomberg (eds.), Kluwer, 2000, pp. 331-340.'
         The submitted code has then been modified to fit CImg coding style and constraints.
    **/
    CImg<T>& distance(const T& value, const unsigned int metric=2) {
      if (is_empty()) return *this;
      if (cimg::type<Tint>::string()!=pixel_type()) // For datatype < int
        return CImg<Tint>(*this,false).distance((Tint)value,metric).
          cut((Tint)cimg::type<T>::min(),(Tint)cimg::type<T>::max()).move_to(*this);
      bool is_value = false;
      cimg_for(*this,ptr,T) *ptr = *ptr==value?is_value=true,(T)0:(T)std::max(0,99999999); // (avoid VC++ warning)
      if (!is_value) return fill(cimg::type<T>::max());
      switch (metric) {
      case 0 : return _distance_core(_distance_sep_cdt,_distance_dist_cdt); // Chebyshev
      case 1 : return _distance_core(_distance_sep_mdt,_distance_dist_mdt); // Manhattan
      case 3 : return _distance_core(_distance_sep_edt,_distance_dist_edt); // Squared Euclidean
      default : return _distance_core(_distance_sep_edt,_distance_dist_edt).sqrt(); // Euclidean
      }
      return *this;
    }

    //! Compute distance to a specified value \newinstance.
    CImg<Tfloat> get_distance(const T& value, const unsigned int metric=2) const {
      return CImg<Tfloat>(*this,false).distance((Tfloat)value,metric);
    }

    static longT _distance_sep_edt(const longT i, const longT u, const longT *const g) {
      return (u*u - i*i + g[u] - g[i])/(2*(u - i));
    }

    static longT _distance_dist_edt(const longT x, const longT i, const longT *const g) {
      return (x - i)*(x - i) + g[i];
    }

    static longT _distance_sep_mdt(const longT i, const longT u, const longT *const g) {
      return (u - i<=g[u] - g[i]?999999999:(g[u] - g[i] + u + i)/2);
    }

    static longT _distance_dist_mdt(const longT x, const longT i, const longT *const g) {
      return (x<i?i - x:x - i) + g[i];
    }

    static longT _distance_sep_cdt(const longT i, const longT u, const longT *const g) {
      const longT h = (i + u)/2;
      if (g[i]<=g[u]) { return h<i + g[u]?i + g[u]:h; }
      return h<u - g[i]?h:u - g[i];
    }

    static longT _distance_dist_cdt(const longT x, const longT i, const longT *const g) {
      const longT d = x<i?i - x:x - i;
      return d<g[i]?g[i]:d;
    }

    static void _distance_scan(const unsigned int len,
                               const longT *const g,
                               longT (*const sep)(const longT, const longT, const longT *const),
                               longT (*const f)(const longT, const longT, const longT *const),
                               longT *const s,
                               longT *const t,
                               longT *const dt) {
      longT q = s[0] = t[0] = 0;
      for (int u = 1; u<(int)len; ++u) { // Forward scan
        while ((q>=0) && f(t[q],s[q],g)>f(t[q],u,g)) { --q; }
        if (q<0) { q = 0; s[0] = u; }
        else { const longT w = 1 + sep(s[q], u, g); if (w<(longT)len) { ++q; s[q] = u; t[q] = w; }}
      }
      for (int u = (int)len - 1; u>=0; --u) { dt[u] = f(u,s[q],g); if (u==t[q]) --q; } // Backward scan
    }

    CImg<T>& _distance_core(longT (*const sep)(const longT, const longT, const longT *const),
                            longT (*const f)(const longT, const longT, const longT *const)) {
 // Check for g++ 4.9.X, as OpenMP seems to crash for this particular function. I have no clues why.
#define cimg_is_gcc49x (__GNUC__==4 && __GNUC_MINOR__==9)

      const ulongT wh = (ulongT)_width*_height;
#if cimg_use_openmp!=0 && !cimg_is_gcc49x
      cimg_pragma_openmp(parallel for cimg_openmp_if(_spectrum>=2))
#endif
      cimg_forC(*this,c) {
        CImg<longT> g(_width), dt(_width), s(_width), t(_width);
        CImg<T> img = get_shared_channel(c);
#if cimg_use_openmp!=0 && !cimg_is_gcc49x
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(2) cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*512 &&
                                                                   _height*_depth>=16)
                           firstprivate(g,dt,s,t))
#endif
        cimg_forYZ(*this,y,z) { // Over X-direction
          cimg_forX(*this,x) g[x] = (longT)img(x,y,z,0,wh);
          _distance_scan(_width,g,sep,f,s,t,dt);
          cimg_forX(*this,x) img(x,y,z,0,wh) = (T)dt[x];
        }
        if (_height>1) {
          g.assign(_height); dt.assign(_height); s.assign(_height); t.assign(_height);
#if cimg_use_openmp!=0 && !cimg_is_gcc49x
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                             cimg_openmp_if(_height>=(cimg_openmp_sizefactor)*512 && _width*_depth>=16)
                             firstprivate(g,dt,s,t))
#endif
          cimg_forXZ(*this,x,z) { // Over Y-direction
            cimg_forY(*this,y) g[y] = (longT)img(x,y,z,0,wh);
            _distance_scan(_height,g,sep,f,s,t,dt);
            cimg_forY(*this,y) img(x,y,z,0,wh) = (T)dt[y];
          }
        }
        if (_depth>1) {
          g.assign(_depth); dt.assign(_depth); s.assign(_depth); t.assign(_depth);
#if cimg_use_openmp!=0 && !cimg_is_gcc49x
          cimg_pragma_openmp(parallel for cimg_openmp_collapse(2)
                             cimg_openmp_if(_depth>=(cimg_openmp_sizefactor)*512 && _width*_height>=16)
                             firstprivate(g,dt,s,t))
#endif
          cimg_forXY(*this,x,y) { // Over Z-direction
            cimg_forZ(*this,z) g[z] = (longT)img(x,y,z,0,wh);
            _distance_scan(_depth,g,sep,f,s,t,dt);
            cimg_forZ(*this,z) img(x,y,z,0,wh) = (T)dt[z];
          }
        }
      }
      return *this;
    }

    //! Compute chamfer distance to a specified value, with a custom metric.
    /**
       \param value Reference value.
       \param metric_mask Metric mask.
       \note The algorithm code has been initially proposed by A. Meijster, and modified by D. Tschumperlé.
    **/
    template<typename t>
    CImg<T>& distance(const T& value, const CImg<t>& metric_mask) {
      if (is_empty()) return *this;
      bool is_value = false;
      cimg_for(*this,ptr,T) *ptr = *ptr==value?is_value=true,0:(T)999999999;
      if (!is_value) return fill(cimg::type<T>::max());
      const ulongT wh = (ulongT)_width*_height;
      cimg_pragma_openmp(parallel for cimg_openmp_if(_spectrum>=2))
      cimg_forC(*this,c) {
        CImg<T> img = get_shared_channel(c);
        cimg_pragma_openmp(parallel for cimg_openmp_collapse(3)
                           cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*1024))
        cimg_forXYZ(metric_mask,dx,dy,dz) {
          const t weight = metric_mask(dx,dy,dz);
          if (weight) {
            for (int z = dz, nz = 0; z<depth(); ++z,++nz) { // Forward scan
              for (int y = dy , ny = 0; y<height(); ++y,++ny) {
                for (int x = dx, nx = 0; x<width(); ++x,++nx) {
                  const T dd = img(nx,ny,nz,0,wh) + weight;
                  if (dd<img(x,y,z,0,wh)) img(x,y,z,0,wh) = dd;
                }
              }
            }
            for (int z = depth() - 1 - dz, nz = depth() - 1; z>=0; --z,--nz) { // Backward scan
              for (int y = height() - 1 - dy, ny = height() - 1; y>=0; --y,--ny) {
                for (int x = width() - 1 - dx, nx = width() - 1; x>=0; --x,--nx) {
                  const T dd = img(nx,ny,nz,0,wh) + weight;
                  if (dd<img(x,y,z,0,wh)) img(x,y,z,0,wh) = dd;
                }
              }
            }
          }
        }
      }
      return *this;
    }

    //! Compute chamfer distance to a specified value, with a custom metric \newinstance.
    template<typename t>
    CImg<Tfloat> get_distance(const T& value, const CImg<t>& metric_mask) const {
      return CImg<Tfloat>(*this,false).distance(value,metric_mask);
    }

    //! Compute distance to a specified value, according to a custom metric (use dijkstra algorithm).
    /**
       \param value Reference value.
       \param metric Field of distance potentials.
       \param is_high_connectivity Tells if the algorithm uses low or high connectivity.
       \param[out] return_path An image containing the nodes of the minimal path.
     **/
    template<typename t, typename to>
    CImg<T>& distance_dijkstra(const T& value, const CImg<t>& metric, const bool is_high_connectivity,
                               CImg<to>& return_path) {
      return get_distance_dijkstra(value,metric,is_high_connectivity,return_path).move_to(*this);
    }

    //! Compute distance map to a specified value, according to a custom metric (use dijkstra algorithm) \newinstance.
    template<typename t, typename to>
    CImg<typename cimg::superset<t,long>::type>
    get_distance_dijkstra(const T& value, const CImg<t>& metric, const bool is_high_connectivity,
                          CImg<to>& return_path) const {
      if (is_empty()) return return_path.assign();
      if (!is_sameXYZ(metric))
        throw CImgArgumentException(_cimg_instance
                                    "distance_dijkstra(): image instance and metric map (%u,%u,%u,%u) "
                                    "have incompatible dimensions.",
                                    cimg_instance,
                                    metric._width,metric._height,metric._depth,metric._spectrum);
      typedef typename cimg::superset<t,long>::type td; // Type used for computing cumulative distances
      CImg<td> result(_width,_height,_depth,_spectrum), Q;
      CImg<boolT> is_queued(_width,_height,_depth,1);
      if (return_path) return_path.assign(_width,_height,_depth,_spectrum);

      cimg_forC(*this,c) {
        const CImg<T> img = get_shared_channel(c);
        const CImg<t> met = metric.get_shared_channel(c%metric._spectrum);
        CImg<td> res = result.get_shared_channel(c);
        CImg<to> path = return_path?return_path.get_shared_channel(c):CImg<to>();
        unsigned int sizeQ = 0;

        // Detect initial seeds.
        is_queued.fill(0);
        cimg_forXYZ(img,x,y,z) if (img(x,y,z)==value) {
          Q._priority_queue_insert(is_queued,sizeQ,0,x,y,z);
          res(x,y,z) = 0;
          if (path) path(x,y,z) = (to)0;
        }

        // Start distance propagation.
        while (sizeQ) {

          // Get and remove point with minimal potential from the queue.
          const int x = (int)Q(0,1), y = (int)Q(0,2), z = (int)Q(0,3);
          const td P = (td)-Q(0,0);
          Q._priority_queue_remove(sizeQ);

          // Update neighbors.
          td npot = 0;
          if (x - 1>=0 && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x - 1,y,z) + P),x - 1,y,z)) {
            res(x - 1,y,z) = npot; if (path) path(x - 1,y,z) = (to)2;
          }
          if (x + 1<width() && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x + 1,y,z) + P),x + 1,y,z)) {
            res(x + 1,y,z) = npot; if (path) path(x + 1,y,z) = (to)1;
          }
          if (y - 1>=0 && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x,y - 1,z) + P),x,y - 1,z)) {
            res(x,y - 1,z) = npot; if (path) path(x,y - 1,z) = (to)8;
          }
          if (y + 1<height() && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x,y + 1,z) + P),x,y + 1,z)) {
            res(x,y + 1,z) = npot; if (path) path(x,y + 1,z) = (to)4;
          }
          if (z - 1>=0 && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x,y,z - 1) + P),x,y,z - 1)) {
            res(x,y,z - 1) = npot; if (path) path(x,y,z - 1) = (to)32;
          }
          if (z + 1<depth() && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x,y,z + 1) + P),x,y,z + 1)) {
            res(x,y,z + 1) = npot; if (path) path(x,y,z + 1) = (to)16;
          }

          if (is_high_connectivity) {
            const float sqrt2 = std::sqrt(2.f), sqrt3 = std::sqrt(3.f);

            // Diagonal neighbors on slice z.
            if (x - 1>=0 && y - 1>=0 &&
                Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x - 1,y - 1,z) + P)),x - 1,y - 1,z)) {
              res(x - 1,y - 1,z) = npot; if (path) path(x - 1,y - 1,z) = (to)10;
            }
            if (x + 1<width() && y - 1>=0 &&
                Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x + 1,y - 1,z) + P)),x + 1,y - 1,z)) {
              res(x + 1,y - 1,z) = npot; if (path) path(x + 1,y - 1,z) = (to)9;
            }
            if (x - 1>=0 && y + 1<height() &&
                Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x - 1,y + 1,z) + P)),x - 1,y + 1,z)) {
              res(x - 1,y + 1,z) = npot; if (path) path(x - 1,y + 1,z) = (to)6;
            }
            if (x + 1<width() && y + 1<height() &&
                Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x + 1,y + 1,z) + P)),x + 1,y + 1,z)) {
              res(x + 1,y + 1,z) = npot; if (path) path(x + 1,y + 1,z) = (to)5;
            }

            if (z - 1>=0) { // Diagonal neighbors on slice z - 1
              if (x - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x - 1,y,z - 1) + P)),x - 1,y,z - 1)) {
                res(x - 1,y,z - 1) = npot; if (path) path(x - 1,y,z - 1) = (to)34;
              }
              if (x + 1<width() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x + 1,y,z - 1) + P)),x + 1,y,z - 1)) {
                res(x + 1,y,z - 1) = npot; if (path) path(x + 1,y,z - 1) = (to)33;
              }
              if (y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x,y - 1,z - 1) + P)),x,y - 1,z - 1)) {
                res(x,y - 1,z - 1) = npot; if (path) path(x,y - 1,z - 1) = (to)40;
              }
              if (y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x,y + 1,z - 1) + P)),x,y + 1,z - 1)) {
                res(x,y + 1,z - 1) = npot; if (path) path(x,y + 1,z - 1) = (to)36;
              }
              if (x - 1>=0 && y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x - 1,y - 1,z - 1) + P)),
                                           x - 1,y - 1,z - 1)) {
                res(x - 1,y - 1,z - 1) = npot; if (path) path(x - 1,y - 1,z - 1) = (to)42;
              }
              if (x + 1<width() && y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x + 1,y - 1,z - 1) + P)),
                                           x + 1,y - 1,z - 1)) {
                res(x + 1,y - 1,z - 1) = npot; if (path) path(x + 1,y - 1,z - 1) = (to)41;
              }
              if (x - 1>=0 && y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x - 1,y + 1,z - 1) + P)),
                                           x - 1,y + 1,z - 1)) {
                res(x - 1,y + 1,z - 1) = npot; if (path) path(x - 1,y + 1,z - 1) = (to)38;
              }
              if (x + 1<width() && y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x + 1,y + 1,z - 1) + P)),
                                           x + 1,y + 1,z - 1)) {
                res(x + 1,y + 1,z - 1) = npot; if (path) path(x + 1,y + 1,z - 1) = (to)37;
              }
            }

            if (z + 1<depth()) { // Diagonal neighbors on slice z + 1
              if (x - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x - 1,y,z + 1) + P)),x - 1,y,z + 1)) {
                res(x - 1,y,z + 1) = npot; if (path) path(x - 1,y,z + 1) = (to)18;
              }
              if (x + 1<width() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x + 1,y,z + 1) + P)),x + 1,y,z + 1)) {
                res(x + 1,y,z + 1) = npot; if (path) path(x + 1,y,z + 1) = (to)17;
              }
              if (y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x,y - 1,z + 1) + P)),x,y - 1,z + 1)) {
                res(x,y - 1,z + 1) = npot; if (path) path(x,y - 1,z + 1) = (to)24;
              }
              if (y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x,y + 1,z + 1) + P)),x,y + 1,z + 1)) {
                res(x,y + 1,z + 1) = npot; if (path) path(x,y + 1,z + 1) = (to)20;
              }
              if (x - 1>=0 && y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x - 1,y - 1,z + 1) + P)),
                                           x - 1,y - 1,z + 1)) {
                res(x - 1,y - 1,z + 1) = npot; if (path) path(x - 1,y - 1,z + 1) = (to)26;
              }
              if (x + 1<width() && y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x + 1,y - 1,z + 1) + P)),
                                           x + 1,y - 1,z + 1)) {
                res(x + 1,y - 1,z + 1) = npot; if (path) path(x + 1,y - 1,z + 1) = (to)25;
              }
              if (x - 1>=0 && y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x - 1,y + 1,z + 1) + P)),
                                           x - 1,y + 1,z + 1)) {
                res(x - 1,y + 1,z + 1) = npot; if (path) path(x - 1,y + 1,z + 1) = (to)22;
              }
              if (x + 1<width() && y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x + 1,y + 1,z + 1) + P)),
                                           x + 1,y + 1,z + 1)) {
                res(x + 1,y + 1,z + 1) = npot; if (path) path(x + 1,y + 1,z + 1) = (to)21;
              }
            }
          }
        }
      }
      return result;
    }

    //! Compute distance map to a specified value, according to a custom metric (use dijkstra algorithm). \overloading.
    template<typename t>
    CImg<T>& distance_dijkstra(const T& value, const CImg<t>& metric,
                               const bool is_high_connectivity=false) {
      return get_distance_dijkstra(value,metric,is_high_connectivity).move_to(*this);
    }

    //! Compute distance map to a specified value, according to a custom metric (use dijkstra algorithm). \newinstance.
    template<typename t>
    CImg<Tfloat> get_distance_dijkstra(const T& value, const CImg<t>& metric,
                                       const bool is_high_connectivity=false) const {
      CImg<T> return_path;
      return get_distance_dijkstra(value,metric,is_high_connectivity,return_path);
    }

    //! Compute distance map to one source point, according to a custom metric (use fast marching algorithm).
    /**
       \param value Reference value.
       \param metric Field of distance potentials.
     **/
    template<typename t>
    CImg<T>& distance_eikonal(const T& value, const CImg<t>& metric) {
      return get_distance_eikonal(value,metric).move_to(*this);
    }

    //! Compute distance map to one source point, according to a custom metric (use fast marching algorithm).
    template<typename t>
    CImg<Tfloat> get_distance_eikonal(const T& value, const CImg<t>& metric) const {
      if (is_empty()) return *this;
      if (!is_sameXYZ(metric))
        throw CImgArgumentException(_cimg_instance
                                    "distance_eikonal(): image instance and metric map (%u,%u,%u,%u) have "
                                    "incompatible dimensions.",
                                    cimg_instance,
                                    metric._width,metric._height,metric._depth,metric._spectrum);
      CImg<Tfloat> result(_width,_height,_depth,_spectrum,cimg::type<Tfloat>::max()), Q;
      CImg<charT> state(_width,_height,_depth); // -1=far away, 0=narrow, 1=frozen

      cimg_pragma_openmp(parallel for cimg_openmp_if(_spectrum>=2) firstprivate(Q,state))
      cimg_forC(*this,c) {
        const CImg<T> img = get_shared_channel(c);
        const CImg<t> met = metric.get_shared_channel(c%metric._spectrum);
        CImg<Tfloat> res = result.get_shared_channel(c);
        unsigned int sizeQ = 0;
        state.fill(-1);

        // Detect initial seeds.
        Tfloat *ptr1 = res._data; char *ptr2 = state._data;
        cimg_for(img,ptr0,T) { if (*ptr0==value) { *ptr1 = 0; *ptr2 = 1; } ++ptr1; ++ptr2; }

        // Initialize seeds neighbors.
        ptr2 = state._data;
        cimg_forXYZ(img,x,y,z) if (*(ptr2++)==1) {
          if (x - 1>=0 && state(x - 1,y,z)==-1) {
            const Tfloat dist = res(x - 1,y,z) = __distance_eikonal(res,met(x - 1,y,z),x - 1,y,z);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x - 1,y,z);
          }
          if (x + 1<width() && state(x + 1,y,z)==-1) {
            const Tfloat dist = res(x + 1,y,z) = __distance_eikonal(res,met(x + 1,y,z),x + 1,y,z);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x + 1,y,z);
          }
          if (y - 1>=0 && state(x,y - 1,z)==-1) {
            const Tfloat dist = res(x,y - 1,z) = __distance_eikonal(res,met(x,y - 1,z),x,y - 1,z);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y - 1,z);
          }
          if (y + 1<height() && state(x,y + 1,z)==-1) {
            const Tfloat dist = res(x,y + 1,z) = __distance_eikonal(res,met(x,y + 1,z),x,y + 1,z);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y + 1,z);
          }
          if (z - 1>=0 && state(x,y,z - 1)==-1) {
            const Tfloat dist = res(x,y,z - 1) = __distance_eikonal(res,met(x,y,z - 1),x,y,z - 1);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y,z - 1);
          }
          if (z + 1<depth() && state(x,y,z + 1)==-1) {
            const Tfloat dist = res(x,y,z + 1) = __distance_eikonal(res,met(x,y,z + 1),x,y,z + 1);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y,z + 1);
          }
        }

        // Propagate front.
        while (sizeQ) {
          int x = -1, y = -1, z = -1;
          while (sizeQ && x<0) {
            x = (int)Q(0,1); y = (int)Q(0,2); z = (int)Q(0,3);
            Q._priority_queue_remove(sizeQ);
            if (state(x,y,z)==1) x = -1; else state(x,y,z) = 1;
          }
          if (x>=0) {
            if (x - 1>=0 && state(x - 1,y,z)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x - 1,y,z),x - 1,y,z);
              if (dist<res(x - 1,y,z)) {
                res(x - 1,y,z) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x - 1,y,z);
              }
            }
            if (x + 1<width() && state(x + 1,y,z)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x + 1,y,z),x + 1,y,z);
              if (dist<res(x + 1,y,z)) {
                res(x + 1,y,z) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x + 1,y,z);
              }
            }
            if (y - 1>=0 && state(x,y - 1,z)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x,y - 1,z),x,y - 1,z);
              if (dist<res(x,y - 1,z)) {
                res(x,y - 1,z) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y - 1,z);
              }
            }
            if (y + 1<height() && state(x,y + 1,z)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x,y + 1,z),x,y + 1,z);
              if (dist<res(x,y + 1,z)) {
                res(x,y + 1,z) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y + 1,z);
              }
            }
            if (z - 1>=0 && state(x,y,z - 1)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x,y,z - 1),x,y,z - 1);
              if (dist<res(x,y,z - 1)) {
                res(x,y,z - 1) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y,z - 1);
              }
            }
            if (z + 1<depth() && state(x,y,z + 1)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x,y,z + 1),x,y,z + 1);
              if (dist<res(x,y,z + 1)) {
                res(x,y,z + 1) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y,z + 1);
              }
            }
          }
        }
      }
      return result;
    }

    // Locally solve eikonal equation.
    Tfloat __distance_eikonal(const CImg<Tfloat>& res, const Tfloat P,
                              const int x=0, const int y=0, const int z=0) const {
      const Tfloat M = (Tfloat)cimg::type<T>::max();
      T T1 = (T)std::min(x - 1>=0?res(x - 1,y,z):M,x + 1<width()?res(x + 1,y,z):M);
      Tfloat root = 0;
      if (_depth>1) { // 3D
        T
          T2 = (T)std::min(y - 1>=0?res(x,y - 1,z):M,y + 1<height()?res(x,y + 1,z):M),
          T3 = (T)std::min(z - 1>=0?res(x,y,z - 1):M,z + 1<depth()?res(x,y,z + 1):M);
        if (T1>T2) cimg::swap(T1,T2);
        if (T2>T3) cimg::swap(T2,T3);
        if (T1>T2) cimg::swap(T1,T2);
        if (P<=0) return (Tfloat)T1;
        if (T3<M && ___distance_eikonal(3,-2*(T1 + T2 + T3),T1*T1 + T2*T2 + T3*T3 - P*P,root))
          return std::max((Tfloat)T3,root);
        if (T2<M && ___distance_eikonal(2,-2*(T1 + T2),T1*T1 + T2*T2 - P*P,root))
          return std::max((Tfloat)T2,root);
        return P + T1;
      } else if (_height>1) { // 2D
        T T2 = (T)std::min(y - 1>=0?res(x,y - 1,z):M,y + 1<height()?res(x,y + 1,z):M);
        if (T1>T2) cimg::swap(T1,T2);
        if (P<=0) return (Tfloat)T1;
        if (T2<M && ___distance_eikonal(2,-2*(T1 + T2),T1*T1 + T2*T2 - P*P,root))
          return std::max((Tfloat)T2,root);
        return P + T1;
      } else { // 1D
        if (P<=0) return (Tfloat)T1;
        return P + T1;
      }
      return 0;
    }

    // Find max root of a 2nd-order polynomial.
    static bool ___distance_eikonal(const Tfloat a, const Tfloat b, const Tfloat c, Tfloat &root) {
      const Tfloat delta = b*b - 4*a*c;
      if (delta<0) return false;
      root = 0.5f*(-b + std::sqrt(delta))/a;
      return true;
    }

    // Insert new point in heap.
    template<typename t>
    void _eik_priority_queue_insert(CImg<charT>& state, unsigned int& siz, const t value,
                                    const unsigned int x, const unsigned int y, const unsigned int z) {
      if (state(x,y,z)>0) return;
      state(x,y,z) = 0;
      if (++siz>=_width) { if (!is_empty()) resize(_width*2,4,1,1,0); else assign(64,4); }
      (*this)(siz - 1,0) = (T)value; (*this)(siz - 1,1) = (T)x; (*this)(siz - 1,2) = (T)y; (*this)(siz - 1,3) = (T)z;
      for (unsigned int pos = siz - 1, par = 0; pos && value>(t)(*this)(par=(pos + 1)/2 - 1,0); pos = par) {
        cimg::swap((*this)(pos,0),(*this)(par,0)); cimg::swap((*this)(pos,1),(*this)(par,1));
        cimg::swap((*this)(pos,2),(*this)(par,2)); cimg::swap((*this)(pos,3),(*this)(par,3));
      }
    }

    //! Compute distance function to 0-valued isophotes, using the Eikonal PDE.
    /**
       \param nb_iterations Number of PDE iterations.
       \param band_size Size of the narrow band.
       \param time_step Time step of the PDE iterations.
    **/
    CImg<T>& distance_eikonal(const unsigned int nb_iterations, const float band_size=0, const float time_step=0.5f) {
      if (is_empty()) return *this;
      CImg<Tfloat> velocity(*this,false);
      for (unsigned int iteration = 0; iteration<nb_iterations; ++iteration) {
        Tfloat *ptrd = velocity._data, veloc_max = 0;
        if (_depth>1) { // 3D
          CImg_3x3x3(I,Tfloat);
          cimg_forC(*this,c) cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) if (band_size<=0 || cimg::abs(Iccc)<band_size) {
            const Tfloat
              gx = (Incc - Ipcc)/2,
              gy = (Icnc - Icpc)/2,
              gz = (Iccn - Iccp)/2,
              sgn = -cimg::sign(Iccc),
              ix = gx*sgn>0?(Incc - Iccc):(Iccc - Ipcc),
              iy = gy*sgn>0?(Icnc - Iccc):(Iccc - Icpc),
              iz = gz*sgn>0?(Iccn - Iccc):(Iccc - Iccp),
              ng = 1e-5f + cimg::hypot(gx,gy,gz),
              ngx = gx/ng,
              ngy = gy/ng,
              ngz = gz/ng,
              veloc = sgn*(ngx*ix + ngy*iy + ngz*iz - 1);
            *(ptrd++) = veloc;
            if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
          } else *(ptrd++) = 0;
        } else { // 2D version
          CImg_3x3(I,Tfloat);
          cimg_forC(*this,c) cimg_for3x3(*this,x,y,0,c,I,Tfloat) if (band_size<=0 || cimg::abs(Icc)<band_size) {
            const Tfloat
              gx = (Inc - Ipc)/2,
              gy = (Icn - Icp)/2,
              sgn = -cimg::sign(Icc),
              ix = gx*sgn>0?(Inc - Icc):(Icc - Ipc),
              iy = gy*sgn>0?(Icn - Icc):(Icc - Icp),
              ng = std::max((Tfloat)1e-5,cimg::hypot(gx,gy)),
              ngx = gx/ng,
              ngy = gy/ng,
              veloc = sgn*(ngx*ix + ngy*iy - 1);
            *(ptrd++) = veloc;
            if (veloc>veloc_max) veloc_max = veloc; else if (-veloc>veloc_max) veloc_max = -veloc;
          } else *(ptrd++) = 0;
        }
        if (veloc_max>0) *this+=(velocity*=time_step/veloc_max);
      }
      return *this;
    }

    //! Compute distance function to 0-valued isophotes, using the Eikonal PDE \newinstance.
    CImg<Tfloat> get_distance_eikonal(const unsigned int nb_iterations, const float band_size=0,
                                      const float time_step=0.5f) const {
      return CImg<Tfloat>(*this,false).distance_eikonal(nb_iterations,band_size,time_step);
    }

    //! Compute Haar multiscale wavelet transform.
    /**
       \param axis Axis considered for the transform.
       \param invert Set inverse of direct transform.
       \param nb_scales Number of scales used for the transform.
    **/
    CImg<T>& haar(const char axis, const bool invert=false, const unsigned int nb_scales=1) {
      return get_haar(axis,invert,nb_scales).move_to(*this);
    }

    //! Compute Haar multiscale wavelet transform \newinstance.
    CImg<Tfloat> get_haar(const char axis, const bool invert=false, const unsigned int nb_scales=1) const {
      if (is_empty() || !nb_scales) return +*this;
      CImg<Tfloat> res;
      const Tfloat sqrt2 = std::sqrt(2.f);
      if (nb_scales==1) {
        switch (cimg::lowercase(axis)) { // Single scale transform
        case 'x' : {
          const unsigned int w = _width/2;
          if (w) {
            if ((w%2) && w!=1)
              throw CImgInstanceException(_cimg_instance
                                          "haar(): Sub-image width %u is not even.",
                                          cimg_instance,
                                          w);

            res.assign(_width,_height,_depth,_spectrum);
            if (invert) cimg_forYZC(*this,y,z,c) { // Inverse transform along X
              for (unsigned int x = 0, xw = w, x2 = 0; x<w; ++x, ++xw) {
                const Tfloat val0 = (Tfloat)(*this)(x,y,z,c), val1 = (Tfloat)(*this)(xw,y,z,c);
                res(x2++,y,z,c) = (val0 - val1)/sqrt2;
                res(x2++,y,z,c) = (val0 + val1)/sqrt2;
              }
            } else cimg_forYZC(*this,y,z,c) { // Direct transform along X
              for (unsigned int x = 0, xw = w, x2 = 0; x<w; ++x, ++xw) {
                const Tfloat val0 = (Tfloat)(*this)(x2++,y,z,c), val1 = (Tfloat)(*this)(x2++,y,z,c);
                res(x,y,z,c) = (val0 + val1)/sqrt2;
                res(xw,y,z,c) = (val1 - val0)/sqrt2;
              }
            }
          } else return *this;
        } break;
        case 'y' : {
          const unsigned int h = _height/2;
          if (h) {
            if ((h%2) && h!=1)
              throw CImgInstanceException(_cimg_instance
                                          "haar(): Sub-image height %u is not even.",
                                          cimg_instance,
                                          h);

            res.assign(_width,_height,_depth,_spectrum);
            if (invert) cimg_forXZC(*this,x,z,c) { // Inverse transform along Y
              for (unsigned int y = 0, yh = h, y2 = 0; y<h; ++y, ++yh) {
                const Tfloat val0 = (Tfloat)(*this)(x,y,z,c), val1 = (Tfloat)(*this)(x,yh,z,c);
                res(x,y2++,z,c) = (val0 - val1)/sqrt2;
                res(x,y2++,z,c) = (val0 + val1)/sqrt2;
              }
            } else cimg_forXZC(*this,x,z,c) {
              for (unsigned int y = 0, yh = h, y2 = 0; y<h; ++y, ++yh) { // Direct transform along Y
                const Tfloat val0 = (Tfloat)(*this)(x,y2++,z,c), val1 = (Tfloat)(*this)(x,y2++,z,c);
                res(x,y,z,c)  = (val0 + val1)/sqrt2;
                res(x,yh,z,c) = (val1 - val0)/sqrt2;
              }
            }
          } else return *this;
        } break;
        case 'z' : {
          const unsigned int d = _depth/2;
          if (d) {
            if ((d%2) && d!=1)
              throw CImgInstanceException(_cimg_instance
                                          "haar(): Sub-image depth %u is not even.",
                                          cimg_instance,
                                          d);

            res.assign(_width,_height,_depth,_spectrum);
            if (invert) cimg_forXYC(*this,x,y,c) { // Inverse transform along Z
              for (unsigned int z = 0, zd = d, z2 = 0; z<d; ++z, ++zd) {
                const Tfloat val0 = (Tfloat)(*this)(x,y,z,c), val1 = (Tfloat)(*this)(x,y,zd,c);
                res(x,y,z2++,c) = (val0 - val1)/sqrt2;
                res(x,y,z2++,c) = (val0 + val1)/sqrt2;
              }
            } else cimg_forXYC(*this,x,y,c) {
              for (unsigned int z = 0, zd = d, z2 = 0; z<d; ++z, ++zd) { // Direct transform along Z
                const Tfloat val0 = (Tfloat)(*this)(x,y,z2++,c), val1 = (Tfloat)(*this)(x,y,z2++,c);
                res(x,y,z,c)  = (val0 + val1)/sqrt2;
                res(x,y,zd,c) = (val1 - val0)/sqrt2;
              }
            }
          } else return *this;
        } break;
        default :
          throw CImgArgumentException(_cimg_instance
                                      "haar(): Invalid specified axis '%c' "
                                      "(should be { x | y | z }).",
                                      cimg_instance,
                                      axis);
        }
      } else { // Multi-scale version
        if (invert) {
          res.assign(*this,false);
          switch (cimg::lowercase(axis)) {
          case 'x' : {
            unsigned int w = _width;
            for (unsigned int s = 1; w && s<nb_scales; ++s) w/=2;
            for (w = w?w:1; w<=_width; w*=2) res.draw_image(res.get_crop(0,w - 1).get_haar('x',true,1));
          } break;
          case 'y' : {
            unsigned int h = _width;
            for (unsigned int s = 1; h && s<nb_scales; ++s) h/=2;
            for (h = h?h:1; h<=_height; h*=2) res.draw_image(res.get_crop(0,0,_width - 1,h - 1).get_haar('y',true,1));
          } break;
          case 'z' : {
            unsigned int d = _depth;
            for (unsigned int s = 1; d && s<nb_scales; ++s) d/=2;
            for (d = d?d:1; d<=_depth; d*=2)
              res.draw_image(res.get_crop(0,0,0,_width - 1,_height - 1,d - 1).get_haar('z',true,1));
          } break;
          default :
            throw CImgArgumentException(_cimg_instance
                                        "haar(): Invalid specified axis '%c' "
                                        "(should be { x | y | z }).",
                                        cimg_instance,
                                        axis);
          }
        } else { // Direct transform
          res = get_haar(axis,false,1);
          switch (cimg::lowercase(axis)) {
          case 'x' : {
            for (unsigned int s = 1, w = _width/2; w && s<nb_scales; ++s, w/=2)
              res.draw_image(res.get_crop(0,w - 1).get_haar('x',false,1));
          } break;
          case 'y' : {
            for (unsigned int s = 1, h = _height/2; h && s<nb_scales; ++s, h/=2)
              res.draw_image(res.get_crop(0,0,_width - 1,h - 1).get_haar('y',false,1));
          } break;
          case 'z' : {
            for (unsigned int s = 1, d = _depth/2; d && s<nb_scales; ++s, d/=2)
              res.draw_image(res.get_crop(0,0,0,_width - 1,_height - 1,d - 1).get_haar('z',false,1));
          } break;
          default :
            throw CImgArgumentException(_cimg_instance
                                        "haar(): Invalid specified axis '%c' "
                                        "(should be { x | y | z }).",
                                        cimg_instance,
                                        axis);
          }
        }
      }
      return res;
    }

    //! Compute Haar multiscale wavelet transform \overloading.
    /**
       \param invert Set inverse of direct transform.
       \param nb_scales Number of scales used for the transform.
    **/
    CImg<T>& haar(const bool invert=false, const unsigned int nb_scales=1) {
      return get_haar(invert,nb_scales).move_to(*this);
    }

    //! Compute Haar multiscale wavelet transform \newinstance.
    CImg<Tfloat> get_haar(const bool invert=false, const unsigned int nb_scales=1) const {
      CImg<Tfloat> res;
      if (nb_scales==1) { // Single scale transform
        if (_width>1) get_haar('x',invert,1).move_to(res);
        if (_height>1) { if (res) res.haar('y',invert,1); else get_haar('y',invert,1).move_to(res); }
        if (_depth>1) { if (res) res.haar('z',invert,1); else get_haar('z',invert,1).move_to(res); }
        if (res) return res;
      } else { // Multi-scale transform
        if (invert) { // Inverse transform
          res.assign(*this,false);
          if (_width>1) {
            if (_height>1) {
              if (_depth>1) {
                unsigned int w = _width, h = _height, d = _depth;
                for (unsigned int s = 1; w && h && d && s<nb_scales; ++s) { w/=2; h/=2; d/=2; }
                for (w = w?w:1, h = h?h:1, d = d?d:1; w<=_width && h<=_height && d<=_depth; w*=2, h*=2, d*=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,h - 1,d - 1).get_haar(true,1));
              } else {
                unsigned int w = _width, h = _height;
                for (unsigned int s = 1; w && h && s<nb_scales; ++s) { w/=2; h/=2; }
                for (w = w?w:1, h = h?h:1; w<=_width && h<=_height; w*=2, h*=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,h - 1,0).get_haar(true,1));
              }
            } else {
              if (_depth>1) {
                unsigned int w = _width, d = _depth;
                for (unsigned int s = 1; w && d && s<nb_scales; ++s) { w/=2; d/=2; }
                for (w = w?w:1, d = d?d:1; w<=_width && d<=_depth; w*=2, d*=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,0,d - 1).get_haar(true,1));
              } else {
                unsigned int w = _width;
                for (unsigned int s = 1; w && s<nb_scales; ++s) w/=2;
                for (w = w?w:1; w<=_width; w*=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,0,0).get_haar(true,1));
              }
            }
          } else {
            if (_height>1) {
              if (_depth>1) {
                unsigned int h = _height, d = _depth;
                for (unsigned int s = 1; h && d && s<nb_scales; ++s) { h/=2; d/=2; }
                for (h = h?h:1, d = d?d:1; h<=_height && d<=_depth; h*=2, d*=2)
                  res.draw_image(res.get_crop(0,0,0,0,h - 1,d - 1).get_haar(true,1));
              } else {
                unsigned int h = _height;
                for (unsigned int s = 1; h && s<nb_scales; ++s) h/=2;
                for (h = h?h:1; h<=_height; h*=2)
                  res.draw_image(res.get_crop(0,0,0,0,h - 1,0).get_haar(true,1));
              }
            } else {
              if (_depth>1) {
                unsigned int d = _depth;
                for (unsigned int s = 1; d && s<nb_scales; ++s) d/=2;
                for (d = d?d:1; d<=_depth; d*=2)
                  res.draw_image(res.get_crop(0,0,0,0,0,d - 1).get_haar(true,1));
              } else return *this;
            }
          }
        } else { // Direct transform
          res = get_haar(false,1);
          if (_width>1) {
            if (_height>1) {
              if (_depth>1)
                for (unsigned int s = 1, w = _width/2, h = _height/2, d = _depth/2; w && h && d && s<nb_scales;
                     ++s, w/=2, h/=2, d/=2)
                  res.draw_image(res.get_crop(0,0,0,w - 1,h - 1,d - 1).haar(false,1));
              else for (unsigned int s = 1, w = _width/2, h = _height/2; w && h && s<nb_scales; ++s, w/=2, h/=2)
                     res.draw_image(res.get_crop(0,0,0,w - 1,h - 1,0).haar(false,1));
            } else {
              if (_depth>1) for (unsigned int s = 1, w = _width/2, d = _depth/2; w && d && s<nb_scales; ++s, w/=2, d/=2)
                              res.draw_image(res.get_crop(0,0,0,w - 1,0,d - 1).haar(false,1));
              else for (unsigned int s = 1, w = _width/2; w && s<nb_scales; ++s, w/=2)
                     res.draw_image(res.get_crop(0,0,0,w - 1,0,0).haar(false,1));
            }
          } else {
            if (_height>1) {
              if (_depth>1)
                for (unsigned int s = 1, h = _height/2, d = _depth/2; h && d && s<nb_scales; ++s, h/=2, d/=2)
                  res.draw_image(res.get_crop(0,0,0,0,h - 1,d - 1).haar(false,1));
              else for (unsigned int s = 1, h = _height/2; h && s<nb_scales; ++s, h/=2)
                     res.draw_image(res.get_crop(0,0,0,0,h - 1,0).haar(false,1));
            } else {
              if (_depth>1) for (unsigned int s = 1, d = _depth/2; d && s<nb_scales; ++s, d/=2)
                              res.draw_image(res.get_crop(0,0,0,0,0,d - 1).haar(false,1));
              else return *this;
            }
          }
        }
        return res;
      }
      return *this;
    }

    //! Compute 1D Fast Fourier Transform, along a specified axis.
    /**
       \param axis Axis along which the FFT is computed.
       \param is_inverse Tells if the forward (\c false) or inverse (\c true) FFT is computed.
    **/
    CImgList<Tfloat> get_FFT(const char axis, const bool is_inverse=false) const {
      CImgList<Tfloat> res(*this,CImg<Tfloat>());
      CImg<Tfloat>::FFT(res[0],res[1],axis,is_inverse);
      return res;
    }

    //! Compute n-D Fast Fourier Transform.
    /*
      \param is_inverse Tells if the forward (\c false) or inverse (\c true) FFT is computed.
    **/
    CImgList<Tfloat> get_FFT(const bool is_inverse=false) const {
      CImgList<Tfloat> res(*this,CImg<Tfloat>());
      CImg<Tfloat>::FFT(res[0],res[1],is_inverse);
      return res;
    }

    //! Compute 1D Fast Fourier Transform, along a specified axis.
    /**
       \param[in,out] real Real part of the pixel values.
       \param[in,out] imag Imaginary part of the pixel values.
       \param axis Axis along which the FFT is computed.
       \param is_inverse Tells if the forward (\c false) or inverse (\c true) FFT is computed.
    **/
    static void FFT(CImg<T>& real, CImg<T>& imag, const char axis, const bool is_inverse=false,
                    const unsigned int nb_threads=0) {
      if (!real)
        throw CImgInstanceException("CImg<%s>::FFT(): Specified real part is empty.",
                                    pixel_type());
      if (!imag) imag.assign(real._width,real._height,real._depth,real._spectrum,(T)0);
      if (!real.is_sameXYZC(imag))
        throw CImgInstanceException("CImg<%s>::FFT(): Specified real part (%u,%u,%u,%u,%p) and "
                                    "imaginary part (%u,%u,%u,%u,%p) have different dimensions.",
                                    pixel_type(),
                                    real._width,real._height,real._depth,real._spectrum,real._data,
                                    imag._width,imag._height,imag._depth,imag._spectrum,imag._data);
      const char _axis = cimg::lowercase(axis);
      if (_axis!='x' && _axis!='y' && _axis!='z')
        throw CImgArgumentException("CImgList<%s>::FFT(): Invalid specified axis '%c' for real and imaginary parts "
                                    "(%u,%u,%u,%u) "
                                    "(should be { x | y | z }).",
                                    pixel_type(),axis,
                                    real._width,real._height,real._depth,real._spectrum);
      cimg::unused(nb_threads);
#ifdef cimg_use_fftw3
      cimg::mutex(12);
#ifndef cimg_use_fftw3_singlethread
      fftw_plan_with_nthreads(nb_threads?nb_threads:cimg::nb_cpus());
#endif
      fftw_complex *data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*real._width*real._height*real._depth);
      if (!data_in)
        throw CImgInstanceException("CImgList<%s>::FFT(): Failed to allocate memory (%s) "
                                    "for computing FFT of image (%u,%u,%u,%u) along the X-axis.",
                                    pixel_type(),
                                    cimg::strbuffersize(sizeof(fftw_complex)*real._width*real._height*real._depth),
                                    real._width,real._height,real._depth,real._spectrum);
      double *const ptrf = (double*)data_in;
      fftw_plan data_plan =
        _axis=='x'?fftw_plan_many_dft(1,(int*)&real._width,real.height()*real.depth(),
                                      data_in,0,1,real.width(),
                                      data_in,0,1,real.width(),
                                      is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE):
        _axis=='y'?fftw_plan_many_dft(1,(int*)&real._height,real.width()*real.depth(),
                                      data_in,0,1,real.height(),
                                      data_in,0,1,real.height(),
                                      is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE):
        fftw_plan_many_dft(1,(int*)&real._depth,real.width()*real.height(),
                           data_in,0,1,real.depth(),
                           data_in,0,1,real.depth(),
                           is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE);
      cimg_forC(real,c) {
        CImg<T> realc = real.get_shared_channel(c), imagc = imag.get_shared_channel(c);
        switch (_axis) {
        case 'x' :
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_forXYZ(realc,x,y,z) {
            const ulongT
              i = realc.offset(x,y,z),
              j = 2*(x + (ulongT)y*realc._width + (ulongT)z*realc._width*realc._height);
            ptrf[j] = (double)realc[i];
            ptrf[j + 1] = (double)imagc[i];
          }
          break;
        case 'y' :
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_forXYZ(realc,x,y,z) {
            const ulongT
              i = realc.offset(x,y,z),
              j = 2*(y + (ulongT)x*realc._height + (ulongT)z*realc._width*realc._height);
            ptrf[j] = (double)realc[i];
            ptrf[j + 1] = (double)imagc[i];
          }
          break;
        default :
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_forXYZ(realc,x,y,z) {
            const ulongT
              i = realc.offset(x,y,z),
              j = 2*(z + (ulongT)x*realc._depth + (ulongT)y*realc._width*realc._depth);
            ptrf[j] = (double)realc[i];
            ptrf[j + 1] = (double)imagc[i];
          }
        }

        fftw_execute(data_plan);

        const double a = is_inverse?1.0/(_axis=='x'?real.width():_axis=='y'?real.height():real.depth()):1.0;
        switch (_axis) {
        case 'x' :
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_forXYZ(realc,x,y,z) {
            const ulongT
              i = 2*(x + (ulongT)y*realc._width + (ulongT)z*realc._width*realc._height),
              j = realc.offset(x,y,z);
            realc[j] = (T)(a*ptrf[i]);
            imagc[j] = (T)(a*ptrf[i + 1]);
          }
          break;
        case 'y' :
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_forXYZ(realc,x,y,z) {
            const ulongT
              i = 2*(y + (ulongT)x*realc._height + (ulongT)z*realc._width*realc._height),
              j = realc.offset(x,y,z);
            realc[j] = (T)(a*ptrf[i]);
            imagc[j] = (T)(a*ptrf[i + 1]);
          }
          break;
        default :
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_forXYZ(realc,x,y,z) {
            const ulongT
              i = 2*(z + (ulongT)x*realc._depth + (ulongT)y*realc._width*realc._depth),
              j = realc.offset(x,y,z);
            realc[j] = (T)(a*ptrf[i]);
            imagc[j] = (T)(a*ptrf[i + 1]);
          }
        }
      }

      fftw_destroy_plan(data_plan);
      fftw_free(data_in);
#ifndef cimg_use_fftw3_singlethread
      fftw_cleanup_threads();
#endif
      cimg::mutex(12,0);
#else
      switch (_axis) {
      case 'x' : { // Fourier along X, using built-in functions
        const unsigned int N = real._width, N2 = N>>1;
        if (((N - 1)&N) && N!=1)
          throw CImgInstanceException("CImgList<%s>::FFT(): Specified real and imaginary parts (%u,%u,%u,%u) "
                                      "have non 2^N dimension along the X-axis.",
                                      pixel_type(),
                                      real._width,real._height,real._depth,real._spectrum);

        for (unsigned int i = 0, j = 0; i<N2; ++i) {
          if (j>i) cimg_forYZC(real,y,z,c) {
              cimg::swap(real(i,y,z,c),real(j,y,z,c));
              cimg::swap(imag(i,y,z,c),imag(j,y,z,c));
              if (j<N2) {
                const unsigned int ri = N - 1 - i, rj = N - 1 - j;
                cimg::swap(real(ri,y,z,c),real(rj,y,z,c));
                cimg::swap(imag(ri,y,z,c),imag(rj,y,z,c));
              }
            }
          for (unsigned int m = N, n = N2; (j+=n)>=m; j-=m, m = n, n>>=1) {}
        }
        for (unsigned int delta = 2; delta<=N; delta<<=1) {
          const unsigned int delta2 = delta>>1;
          for (unsigned int i = 0; i<N; i+=delta) {
            float wr = 1, wi = 0;
            const float
              angle = (float)((is_inverse?+1:-1)*2*cimg::PI/delta),
              ca = (float)std::cos(angle),
              sa = (float)std::sin(angle);
            for (unsigned int k = 0; k<delta2; ++k) {
              const unsigned int j = i + k, nj = j + delta2;
              cimg_forYZC(real,y,z,c) {
                T &ir = real(j,y,z,c), &ii = imag(j,y,z,c), &nir = real(nj,y,z,c), &nii = imag(nj,y,z,c);
                const float tmpr = (float)(wr*nir - wi*nii), tmpi = (float)(wr*nii + wi*nir);
                nir = (T)(ir - tmpr);
                nii = (T)(ii - tmpi);
                ir+=(T)tmpr;
                ii+=(T)tmpi;
              }
              const float nwr = wr*ca-wi*sa;
              wi = wi*ca + wr*sa;
              wr = nwr;
            }
          }
        }
        if (is_inverse) { real/=N; imag/=N; }
      } break;
      case 'y' : { // Fourier along Y, using built-in functions
        const unsigned int N = real._height, N2 = N>>1;
        if (((N - 1)&N) && N!=1)
          throw CImgInstanceException("CImgList<%s>::FFT(): Specified real and imaginary parts (%u,%u,%u,%u) "
                                      "have non 2^N dimension along the Y-axis.",
                                      pixel_type(),
                                      real._width,real._height,real._depth,real._spectrum);

        for (unsigned int i = 0, j = 0; i<N2; ++i) {
          if (j>i) cimg_forXZC(real,x,z,c) {
              cimg::swap(real(x,i,z,c),real(x,j,z,c));
              cimg::swap(imag(x,i,z,c),imag(x,j,z,c));
              if (j<N2) {
                const unsigned int ri = N - 1 - i, rj = N - 1 - j;
                cimg::swap(real(x,ri,z,c),real(x,rj,z,c));
                cimg::swap(imag(x,ri,z,c),imag(x,rj,z,c));
              }
            }
          for (unsigned int m = N, n = N2; (j+=n)>=m; j-=m, m = n, n>>=1) {}
        }
        for (unsigned int delta = 2; delta<=N; delta<<=1) {
          const unsigned int delta2 = (delta>>1);
          for (unsigned int i = 0; i<N; i+=delta) {
            float wr = 1, wi = 0;
            const float
              angle = (float)((is_inverse?+1:-1)*2*cimg::PI/delta),
              ca = (float)std::cos(angle),
              sa = (float)std::sin(angle);
            for (unsigned int k = 0; k<delta2; ++k) {
              const unsigned int j = i + k, nj = j + delta2;
              cimg_forXZC(real,x,z,c) {
                T &ir = real(x,j,z,c), &ii = imag(x,j,z,c), &nir = real(x,nj,z,c), &nii = imag(x,nj,z,c);
                const float tmpr = (float)(wr*nir - wi*nii), tmpi = (float)(wr*nii + wi*nir);
                nir = (T)(ir - tmpr);
                nii = (T)(ii - tmpi);
                ir+=(T)tmpr;
                ii+=(T)tmpi;
              }
              const float nwr = wr*ca-wi*sa;
              wi = wi*ca + wr*sa;
              wr = nwr;
            }
          }
        }
        if (is_inverse) { real/=N; imag/=N; }
      } break;
      default : { // Fourier along Z, using built-in functions
        const unsigned int N = real._depth, N2 = N>>1;
        if (((N - 1)&N) && N!=1)
          throw CImgInstanceException("CImgList<%s>::FFT(): Specified real and imaginary parts (%u,%u,%u,%u) "
                                      "have non 2^N dimension along the Z-axis.",
                                      pixel_type(),
                                      real._width,real._height,real._depth,real._spectrum);

        for (unsigned int i = 0, j = 0; i<N2; ++i) {
          if (j>i) cimg_forXYC(real,x,y,c) {
              cimg::swap(real(x,y,i,c),real(x,y,j,c));
              cimg::swap(imag(x,y,i,c),imag(x,y,j,c));
              if (j<N2) {
                const unsigned int ri = N - 1 - i, rj = N - 1 - j;
                cimg::swap(real(x,y,ri,c),real(x,y,rj,c));
                cimg::swap(imag(x,y,ri,c),imag(x,y,rj,c));
              }
            }
          for (unsigned int m = N, n = N2; (j+=n)>=m; j-=m, m = n, n>>=1) {}
        }
        for (unsigned int delta = 2; delta<=N; delta<<=1) {
          const unsigned int delta2 = (delta>>1);
          for (unsigned int i = 0; i<N; i+=delta) {
            float wr = 1, wi = 0;
            const float
              angle = (float)((is_inverse?+1:-1)*2*cimg::PI/delta),
              ca = (float)std::cos(angle),
              sa = (float)std::sin(angle);
            for (unsigned int k = 0; k<delta2; ++k) {
              const unsigned int j = i + k, nj = j + delta2;
              cimg_forXYC(real,x,y,c) {
                T &ir = real(x,y,j,c), &ii = imag(x,y,j,c), &nir = real(x,y,nj,c), &nii = imag(x,y,nj,c);
                const float tmpr = (float)(wr*nir - wi*nii), tmpi = (float)(wr*nii + wi*nir);
                nir = (T)(ir - tmpr);
                nii = (T)(ii - tmpi);
                ir+=(T)tmpr;
                ii+=(T)tmpi;
              }
              const float nwr = wr*ca-wi*sa;
              wi = wi*ca + wr*sa;
              wr = nwr;
            }
          }
        }
        if (is_inverse) { real/=N; imag/=N; }
      } break;
      }
#endif
    }

    //! Compute n-D Fast Fourier Transform.
    /**
       \param[in,out] real Real part of the pixel values.
       \param[in,out] imag Imaginary part of the pixel values.
       \param is_inverse Tells if the forward (\c false) or inverse (\c true) FFT is computed.
       \param nb_threads Number of parallel threads used for the computation.
         Use \c 0 to set this to the number of available cpus.
    **/
    static void FFT(CImg<T>& real, CImg<T>& imag, const bool is_inverse=false,
                    const unsigned int nb_threads=0) {
      if (!real)
        throw CImgInstanceException("CImgList<%s>::FFT(): Empty specified real part.",
                                    pixel_type());
      if (!imag) imag.assign(real._width,real._height,real._depth,real._spectrum,(T)0);
      if (!real.is_sameXYZC(imag))
        throw CImgInstanceException("CImgList<%s>::FFT(): Specified real part (%u,%u,%u,%u,%p) and "
                                    "imaginary part (%u,%u,%u,%u,%p) have different dimensions.",
                                    pixel_type(),
                                    real._width,real._height,real._depth,real._spectrum,real._data,
                                    imag._width,imag._height,imag._depth,imag._spectrum,imag._data);
      cimg::unused(nb_threads);
#ifdef cimg_use_fftw3
      cimg::mutex(12);
#ifndef cimg_use_fftw3_singlethread
      fftw_plan_with_nthreads(nb_threads?nb_threads:cimg::nb_cpus());
#endif
      fftw_complex *data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*real._width*real._height*real._depth);
      if (!data_in)
        throw CImgInstanceException("CImgList<%s>::FFT(): Failed to allocate memory (%s) "
                                    "for computing FFT of image (%u,%u,%u,%u).",
                                    pixel_type(),
                                    cimg::strbuffersize(sizeof(fftw_complex)*real._width*
                                                        real._height*real._depth*real._spectrum),
                                    real._width,real._height,real._depth,real._spectrum);
      double *const ptrf = (double*)data_in;
      fftw_plan data_plan =
        real._depth>1?fftw_plan_dft_3d(real._depth,real._height,real._width,data_in,data_in,
                                       is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE):
        real._height>1?fftw_plan_dft_2d(real._height,real._width,data_in,data_in,
                                        is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE):
        fftw_plan_dft_1d(real._width,data_in,data_in,
                         is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE);
      cimg_forC(real,c) {
        CImg<T> realc = real.get_shared_channel(c), imagc = imag.get_shared_channel(c);
        cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
        cimg_rofoff(realc,i) { const ulongT i2 = 2*i; ptrf[i2] = (double)realc[i]; ptrf[i2 + 1] = (double)imagc[i]; }
        fftw_execute(data_plan);
        if (is_inverse) {
          const double a = 1.0/(real.width()*real.height()*real.depth());
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_rofoff(realc,i) { const ulongT i2 = 2*i; realc[i] = (T)(a*ptrf[i2]); imagc[i] = (T)(a*ptrf[i2 + 1]); }
        } else
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_rofoff(realc,i) { const ulongT i2 = 2*i; realc[i] = (T)ptrf[i2]; imagc[i] = (T)ptrf[i2 + 1]; }
      }
      fftw_destroy_plan(data_plan);
      fftw_free(data_in);
#ifndef cimg_use_fftw3_singlethread
      fftw_cleanup_threads();
#endif
      cimg::mutex(12,0);
#else
      if (real._depth>1) FFT(real,imag,'z',is_inverse);
      if (real._height>1) FFT(real,imag,'y',is_inverse);
      if (real._width>1) FFT(real,imag,'x',is_inverse);
#endif
    }

    //@}
    //-------------------------------------
#endif
