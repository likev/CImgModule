#ifndef CIMG_MODULE_MATH_PARSER_FUNCTIONS_1_H
#define CIMG_MODULE_MATH_PARSER_FUNCTIONS_1_H

#ifdef _mp_arg
#undef _mp_arg
#endif
#define _mp_arg(x) mp.mem[mp.opcode[x]]

#ifdef cimg_mp_func_abort
      static double mp_abort(_cimg_math_parser& mp) {
        cimg::unused(mp);
        cimg_mp_func_abort();
        return cimg::type<double>::nan();
      }
#else
      static double mp_abort(_cimg_math_parser& mp) { cimg::unused(mp); return cimg::type<double>::nan(); }
#endif

      static double mp_abs(_cimg_math_parser& mp) {
        return cimg::abs(_mp_arg(2));
      }

      static double mp_abscut(_cimg_math_parser& mp) {
        return cimg::abscut(_mp_arg(2),_mp_arg(3),_mp_arg(4),_mp_arg(5));
      }

      static double mp_absmaxabs(_cimg_math_parser& mp) {
        return _mp_maxabs(mp,true);
      }

      static double mp_absminabs(_cimg_math_parser& mp) {
        return _mp_minabs(mp,true);
      }

      static double mp_acos(_cimg_math_parser& mp) {
        return std::acos(_mp_arg(2));
      }

      static double mp_acosh(_cimg_math_parser& mp) {
        return cimg::acosh(_mp_arg(2));
      }

      static double mp_add(_cimg_math_parser& mp) {
        return _mp_arg(2) + _mp_arg(3);
      }

      static double mp_arg0(_cimg_math_parser& mp) {
        const int _ind = (int)_mp_arg(4);
        const unsigned int
          nb_args = (unsigned int)mp.opcode[2] - 4,
          ind = _ind<0?_ind + nb_args:_ind + 1U,
          siz = (unsigned int)mp.opcode[3];
        if (siz>0) {
          if (ind>=nb_args) std::memset(&_mp_arg(1) + 1,0,siz*sizeof(double));
          else std::memcpy(&_mp_arg(1) + 1,&_mp_arg(ind + 4) + 1,siz*sizeof(double));
          return cimg::type<double>::nan();
        }
        if (ind>=nb_args) return 0;
        return _mp_arg(ind + 4);
      }

      static double mp_arg1(_cimg_math_parser& mp) {
        const int _ind = (int)_mp_arg(4);
        const unsigned int
          nb_args = (unsigned int)mp.opcode[2] - 4,
          ind = _ind<0?_ind + nb_args:(unsigned int)_ind,
          siz = (unsigned int)mp.opcode[3];
        if (siz>0) {
          if (ind>=nb_args) std::memset(&_mp_arg(1) + 1,0,siz*sizeof(double));
          else std::memcpy(&_mp_arg(1) + 1,&_mp_arg(ind + 4) + 1,siz*sizeof(double));
          return cimg::type<double>::nan();
        }
        if (ind>=nb_args) return 0;
        return _mp_arg(ind + 4);
      }

      static double mp_argkth(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        CImg<doubleT> values;
        if (i_end==5) values.assign(&_mp_arg(3),(unsigned int)mp.opcode[4],1,1,1,true); // Only a single argument
        else {
          unsigned int siz = 0;
          for (unsigned int i = 4; i<i_end; i+=2) siz+=(unsigned int)mp.opcode[i];
          values.assign(siz);
          double *ptr = values;
          for (unsigned int i = 3; i<i_end; i+=2) {
            const unsigned int len = (unsigned int)mp.opcode[i + 1];
            if (len>1) std::memcpy(ptr,&_mp_arg(i),len*sizeof(double));
            else *ptr = _mp_arg(i);
            ptr+=len;
          }
        }
        longT ind = (longT)cimg::round(_mp_arg(3));
        ++values._data; --values._width; // Skip first value
        if (ind<0) ind+=values.width() + 1;
        ind = cimg::cut(ind,(longT)1,(longT)values.width());
        const double kth = values.kth_smallest((ulongT)(ind - 1));
        --values._data; ++values._width;
        for (unsigned int argkth = 1; argkth<values._width; ++argkth)
          if (values[argkth]==kth) return argkth;
        return cimg::type<double>::nan();
      }

      static double mp_argmax(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val, valmax = -cimg::type<double>::inf();
        unsigned int siz = 0, argmax = 0;
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) { val = *(ptr++); if (val>valmax) { valmax = val; argmax = siz + k; } }
          } else { val = _mp_arg(i); if (val>valmax) { valmax = val; argmax = siz; } }
          siz+=len;
        }
        return (double)argmax;
      }

      static double mp_argmax2(_cimg_math_parser& mp) {
        return _mp_arg(2)>=_mp_arg(3)?0:1;
      }

      static double mp_argmaxabs(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val, abs_val, abs_valmaxabs = 0;
        unsigned int siz = 0, argmaxabs = 0;
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) {
              val = *(ptr++);
              abs_val = cimg::abs(val);
              if (abs_val>abs_valmaxabs) { abs_valmaxabs = abs_val; argmaxabs = siz + k; }
            }
          } else {
            val = _mp_arg(i);
            abs_val = cimg::abs(val);
            if (abs_val>abs_valmaxabs) { abs_valmaxabs = abs_val; argmaxabs = siz; }
          }
          siz+=len;
        }
        return (double)argmaxabs;
      }

      static double mp_argmaxabs2(_cimg_math_parser& mp) {
        return cimg::abs(_mp_arg(2))>=cimg::abs(_mp_arg(3))?0:1;
      }

      static double mp_argmin(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val, valmin = cimg::type<double>::inf();
        unsigned int siz = 0, argmin = 0;
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) { val = *(ptr++); if (val<valmin) { valmin = val; argmin = siz + k; } }
          } else { val = _mp_arg(i); if (val<valmin) { valmin = val; argmin = siz; } }
          siz+=len;
        }
        return (double)argmin;
      }

      static double mp_argmin2(_cimg_math_parser& mp) {
        return _mp_arg(2)<=_mp_arg(3)?0:1;
      }

      static double mp_argminabs(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val, abs_val, abs_valminabs = cimg::type<double>::inf();
        unsigned int siz = 0, argminabs = 0;
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) {
              val = *(ptr++);
              abs_val = cimg::abs(val);
              if (abs_val<abs_valminabs) { abs_valminabs = abs_val; argminabs = siz + k; }
            }
          } else {
            val = _mp_arg(i);
            abs_val = cimg::abs(val);
            if (abs_val<abs_valminabs) { abs_valminabs = abs_val; argminabs = siz; }
          }
          siz+=len;
        }
        return (double)argminabs;
      }

      static double mp_argminabs2(_cimg_math_parser& mp) {
        return cimg::abs(_mp_arg(2))<=cimg::abs(_mp_arg(3))?0:1;
      }

      static double mp_asin(_cimg_math_parser& mp) {
        return std::asin(_mp_arg(2));
      }

      static double mp_asinh(_cimg_math_parser& mp) {
        return cimg::asinh(_mp_arg(2));
      }

      static double mp_atan(_cimg_math_parser& mp) {
        return std::atan(_mp_arg(2));
      }

      static double mp_atan2(_cimg_math_parser& mp) {
        return std::atan2(_mp_arg(2),_mp_arg(3));
      }

      static double mp_atanh(_cimg_math_parser& mp) {
        return cimg::atanh(_mp_arg(2));
      }

      static double mp_avg(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        unsigned int siz = 0;
        double sum = 0;
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) sum+=*(ptr++);
          } else sum+=_mp_arg(i);
          siz+=len;
        }
        return sum/siz;
      }

      static double mp_avg2(_cimg_math_parser& mp) {
        return (_mp_arg(2) + _mp_arg(3))/2;
      }

      static double mp_bitwise_and(_cimg_math_parser& mp) {
        return (double)((longT)_mp_arg(2) & (longT)_mp_arg(3));
      }

      static double mp_bitwise_left_shift(_cimg_math_parser& mp) {
        return (double)((longT)_mp_arg(2)<<(unsigned int)_mp_arg(3));
      }

      static double mp_bitwise_not(_cimg_math_parser& mp) {
        // Limit result to 32bits such that it can be entirely represented as a 'double'.
        return (double)~(unsigned int)_mp_arg(2);
      }

      static double mp_bitwise_or(_cimg_math_parser& mp) {
        return (double)((longT)_mp_arg(2) | (longT)_mp_arg(3));
      }

      static double mp_bitwise_right_shift(_cimg_math_parser& mp) {
        return (double)((longT)_mp_arg(2)>>(unsigned int)_mp_arg(3));
      }

      static double mp_bitwise_xor(_cimg_math_parser& mp) {
        return (double)((longT)_mp_arg(2) ^ (longT)_mp_arg(3));
      }

      static double mp_bool(_cimg_math_parser& mp) {
        return (double)(bool)_mp_arg(2);
      }

      static double mp_break(_cimg_math_parser& mp) {
        mp.break_type = 1;
        mp.p_code = mp.p_break - 1;
        return cimg::type<double>::nan();
      }

      static double mp_breakpoint(_cimg_math_parser& mp) {
        cimg_abort_init;
        cimg_abort_test;
        cimg::unused(mp);
        return cimg::type<double>::nan();
      }

      static double mp_c2o(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          mp_check_list(mp,"c2o");
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        const int x = (int)_mp_arg(3), y = (int)_mp_arg(4), z = (int)_mp_arg(5), c = (int)_mp_arg(6);
        const bool
          boundary_conditions = (bool)_mp_arg(7),
          is_contained = boundary_conditions?img.containsXYZC(x,y,z,c):true;
        return is_contained?img.offset(x,y,z,c):-1;
      }

      static double mp_cbrt(_cimg_math_parser& mp) {
        return cimg::cbrt(_mp_arg(2));
      }

      static double mp_ceil(_cimg_math_parser& mp) {
        return std::ceil(_mp_arg(2));
      }

      static double mp_complex_abs(_cimg_math_parser& mp) {
        return cimg::hypot(_mp_arg(2),_mp_arg(3));
      }

      static double mp_complex_conj(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = real;
        ptrd[1] = -imag;
        return cimg::type<double>::nan();
      }

      static double mp_complex_cos(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = std::cos(real)*std::cosh(imag);
        ptrd[1] = -std::sin(real)*std::sinh(imag);
        return cimg::type<double>::nan();
      }

      static double mp_complex_cosh(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = std::cosh(real)*std::cos(imag);
        ptrd[1] = std::sinh(real)*std::sin(imag);
        return cimg::type<double>::nan();
      }

      static double mp_complex_div_sv(_cimg_math_parser& mp) {
        const double
          *ptr2 = &_mp_arg(3) + 1,
          r1 = _mp_arg(2),
          r2 = *(ptr2++), i2 = *ptr2;
        double *ptrd = &_mp_arg(1) + 1;
        const double denom = r2*r2 + i2*i2;
        *(ptrd++) = r1*r2/denom;
        *ptrd =  -r1*i2/denom;
        return cimg::type<double>::nan();
      }

      static double mp_complex_div_vv(_cimg_math_parser& mp) {
        const double
          *ptr1 = &_mp_arg(2) + 1, *ptr2 = &_mp_arg(3) + 1,
          r1 = *(ptr1++), i1 = *ptr1,
          r2 = *(ptr2++), i2 = *ptr2;
        double *ptrd = &_mp_arg(1) + 1;
        const double denom = r2*r2 + i2*i2;
        *(ptrd++) = (r1*r2 + i1*i2)/denom;
        *ptrd = (r2*i1 - r1*i2)/denom;
        return cimg::type<double>::nan();
      }

      static double mp_complex_exp(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3), exp_real = std::exp(real);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = exp_real*std::cos(imag);
        ptrd[1] = exp_real*std::sin(imag);
        return cimg::type<double>::nan();
      }

      static double mp_complex_log(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = 0.5*std::log(real*real + imag*imag);
        ptrd[1] = std::atan2(imag,real);
        return cimg::type<double>::nan();
      }

      static double mp_complex_mul(_cimg_math_parser& mp) {
        const double
          *ptr1 = &_mp_arg(2) + 1, *ptr2 = &_mp_arg(3) + 1,
          r1 = *(ptr1++), i1 = *ptr1,
          r2 = *(ptr2++), i2 = *ptr2;
        double *ptrd = &_mp_arg(1) + 1;
        *(ptrd++) = r1*r2 - i1*i2;
        *(ptrd++) = r1*i2 + r2*i1;
        return cimg::type<double>::nan();
      }

      static double mp_complex_one(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = 1;
        ptrd[1] = 0;
        return cimg::type<double>::nan();
      }

      static void _mp_complex_pow(const double r1, const double i1,
                                  const double r2, const double i2,
                                  double *ptrd) {
        double ro, io;
        if (cimg::abs(i2)<1e-15) { // Exponent is real
          if (cimg::abs(r1)<1e-15 && cimg::abs(i1)<1e-15) {
            if (cimg::abs(r2)<1e-15) { ro = 1; io = 0; }
            else ro = io = 0;
          } else {
            const double
              mod1_2 = r1*r1 + i1*i1,
              phi1 = std::atan2(i1,r1),
              modo = std::pow(mod1_2,0.5*r2),
              phio = r2*phi1;
            ro = modo*std::cos(phio);
            io = modo*std::sin(phio);
          }
        } else { // Exponent is complex
          if (cimg::abs(r1)<1e-15 && cimg::abs(i1)<1e-15) ro = io = 0;
          const double
            mod1_2 = r1*r1 + i1*i1,
            phi1 = std::atan2(i1,r1),
            modo = std::pow(mod1_2,0.5*r2)*std::exp(-i2*phi1),
            phio = r2*phi1 + 0.5*i2*std::log(mod1_2);
          ro = modo*std::cos(phio);
          io = modo*std::sin(phio);
        }
        *(ptrd++) = ro;
        *ptrd = io;
      }

      static double mp_complex_pow_ss(_cimg_math_parser& mp) {
        const double val1 = _mp_arg(2), val2 = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        _mp_complex_pow(val1,0,val2,0,ptrd);
        return cimg::type<double>::nan();
      }

      static double mp_complex_pow_sv(_cimg_math_parser& mp) {
        const double val1 = _mp_arg(2), *ptr2 = &_mp_arg(3) + 1;
        double *ptrd = &_mp_arg(1) + 1;
        _mp_complex_pow(val1,0,ptr2[0],ptr2[1],ptrd);
        return cimg::type<double>::nan();
      }

      static double mp_complex_pow_vs(_cimg_math_parser& mp) {
        const double *ptr1 = &_mp_arg(2) + 1, val2 = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        _mp_complex_pow(ptr1[0],ptr1[1],val2,0,ptrd);
        return cimg::type<double>::nan();
      }

      static double mp_complex_pow_vv(_cimg_math_parser& mp) {
        const double *ptr1 = &_mp_arg(2) + 1, *ptr2 = &_mp_arg(3) + 1;
        double *ptrd = &_mp_arg(1) + 1;
        _mp_complex_pow(ptr1[0],ptr1[1],ptr2[0],ptr2[1],ptrd);
        return cimg::type<double>::nan();
      }

      static double mp_complex_sin(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = std::sin(real)*std::cosh(imag);
        ptrd[1] = std::cos(real)*std::sinh(imag);
        return cimg::type<double>::nan();
      }

      static double mp_complex_sinh(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = std::sinh(real)*std::cos(imag);
        ptrd[1] = std::cosh(real)*std::sin(imag);
        return cimg::type<double>::nan();
      }

      static double mp_complex_sqr(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = cimg::sqr(real) - cimg::sqr(imag);
        ptrd[1] = 2*real*imag;
        return cimg::type<double>::nan();
      }

      static double mp_complex_sqrt(_cimg_math_parser& mp) {
        const double
          real = _mp_arg(2), imag = _mp_arg(3),
          r = std::sqrt(cimg::hypot(real,imag)),
          theta = std::atan2(imag,real)/2;
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = r*std::cos(theta);
        ptrd[1] = r*std::sin(theta);
        return cimg::type<double>::nan();
      }

      static double mp_complex_tan(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3), denom = std::cos(2*real) + std::cosh(2*imag);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = std::sin(2*real)/denom;
        ptrd[1] = std::sinh(2*imag)/denom;
        return cimg::type<double>::nan();
      }

      static double mp_complex_tanh(_cimg_math_parser& mp) {
        const double real = _mp_arg(2), imag = _mp_arg(3), denom = std::cosh(2*real) + std::cos(2*imag);
        double *ptrd = &_mp_arg(1) + 1;
        ptrd[0] = std::sinh(2*real)/denom;
        ptrd[1] = std::sin(2*imag)/denom;
        return cimg::type<double>::nan();
      }

      static double mp_continue(_cimg_math_parser& mp) {
        mp.break_type = 2;
        mp.p_code = mp.p_break - 1;
        return cimg::type<double>::nan();
      }

      static double mp_convolve(_cimg_math_parser &mp) {
        return _mp_correlate(mp,true);
      }

      static double mp_copy(_cimg_math_parser& mp) {
        return _mp_arg(2);
      }

      static double mp_correlate(_cimg_math_parser &mp) {
        return _mp_correlate(mp,false);
      }

      static double _mp_correlate(_cimg_math_parser &mp, bool is_convolve) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *const ptrI = &_mp_arg(2) + 1, *const ptrK = &_mp_arg(7) + 1;
        const unsigned int
          wI = (unsigned int)mp.opcode[3],
          hI = (unsigned int)mp.opcode[4],
          dI = (unsigned int)mp.opcode[5],
          sI = (unsigned int)mp.opcode[6],
          wK = (unsigned int)mp.opcode[8],
          hK = (unsigned int)mp.opcode[9],
          dK = (unsigned int)mp.opcode[10],
          sK = (unsigned int)mp.opcode[11],
          boundary_conditions = (unsigned int)_mp_arg(12),
          channel_mode = (unsigned int)mp.opcode[14],
          xstride = (unsigned int)mp.opcode[18],
          ystride = (unsigned int)mp.opcode[19],
          zstride = (unsigned int)mp.opcode[20],
          xsize = (unsigned int)mp.opcode[27],
          ysize = (unsigned int)mp.opcode[28],
          zsize = (unsigned int)mp.opcode[29];
        const bool is_normalized = (bool)_mp_arg(13);
        const int
          xcenter = mp.opcode[15]!=~0U?(int)_mp_arg(15):(int)(~0U>>1),
          ycenter = mp.opcode[16]!=~0U?(int)_mp_arg(16):(int)(~0U>>1),
          zcenter = mp.opcode[17]!=~0U?(int)_mp_arg(17):(int)(~0U>>1),
          xdilation = (int)_mp_arg(21),
          ydilation = (int)_mp_arg(22),
          zdilation = (int)_mp_arg(23),
          xoffset = (int)_mp_arg(24),
          yoffset = (int)_mp_arg(25),
          zoffset = (int)_mp_arg(26);
        CImg<doubleT> res;
        if (is_convolve)
          res = CImg<doubleT>(ptrI,wI,hI,dI,sI,true).
            get_convolve(CImg<doubleT>(ptrK,wK,hK,dK,sK,true),
                         boundary_conditions,is_normalized,channel_mode,
                         xcenter,ycenter,zcenter,xstride,ystride,zstride,xdilation,ydilation,zdilation,
                         xoffset,yoffset,zoffset,xsize,ysize,zsize);
        else
          res = CImg<doubleT>(ptrI,wI,hI,dI,sI,true).
            get_correlate(CImg<doubleT>(ptrK,wK,hK,dK,sK,true),
                          boundary_conditions,is_normalized,channel_mode,
                          xcenter,ycenter,zcenter,xstride,ystride,zstride,xdilation,ydilation,zdilation,
                          xoffset,yoffset,zoffset,xsize,ysize,zsize);
        CImg<doubleT>(ptrd,res._width,res._height,res._depth,res._spectrum,true) = res;
        return cimg::type<double>::nan();
      }

      static double mp_cos(_cimg_math_parser& mp) {
        return std::cos(_mp_arg(2));
      }

      static double mp_cosh(_cimg_math_parser& mp) {
        return std::cosh(_mp_arg(2));
      }

      static double mp_cov(_cimg_math_parser& mp) {
        const unsigned int
          _siz = (unsigned int)mp.opcode[4],
          siz = std::max(_siz,1U),
          off = _siz?1:0,
          sizm1 = siz>1?siz - 1:1;
        const CImg<doubleT>
          A(&_mp_arg(2) + off,1,siz,1,1,true),
          B(&_mp_arg(3) + off,1,siz,1,1,true);
        const double
          avgA = (unsigned int)mp.opcode[5]==~0U?A.mean():_mp_arg(5),
          avgB = (unsigned int)mp.opcode[6]==~0U?B.mean():_mp_arg(6);
        double res = 0;
        cimg_forY(A,k) res+=(A[k] - avgA)*(B[k] - avgB);
        return res/sizm1;
      }

      static double mp_critical(_cimg_math_parser& mp) {
        const ulongT g_target = mp.opcode[1];
        cimg_pragma_openmp(critical(mp_critical)) {
          const CImg<ulongT> *const p_end = ++mp.p_code + mp.opcode[2];
          mp.eval(mp.p_code,p_end);
        }
        --mp.p_code;
        return mp.mem[g_target];
      }

      static double mp_cross(_cimg_math_parser& mp) {
        CImg<doubleT>
          vout(&_mp_arg(1) + 1,1,3,1,1,true),
          v1(&_mp_arg(2) + 1,1,3,1,1,true),
          v2(&_mp_arg(3) + 1,1,3,1,1,true);
        (vout = v1).cross(v2);
        return cimg::type<double>::nan();
      }

      static double mp_cumulate(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int
          wA = (unsigned int)mp.opcode[3],
          hA = (unsigned int)mp.opcode[4],
          dA = (unsigned int)mp.opcode[5],
          sA = (unsigned int)mp.opcode[6],
          sizp = (unsigned int)mp.opcode[8];
        const double
          *const ptrs = &_mp_arg(2) + 1,
          *const ptrp = sizp!=~0U?&_mp_arg(7) + 1:0;
        CImg<charT> str;
        if (ptrp) {
          str.assign(std::max(1U,sizp) + 1);
          if (!sizp) str[0] = _mp_arg(7);
          else for (unsigned int p = 0; p<sizp; ++p) str[p] = (char)ptrp[p];
          str.back() = 0;
        }
        CImg<doubleT>(ptrd,wA,hA,dA,sA,true) = CImg<doubleT>(ptrs,wA,hA,dA,sA,true).
          get_cumulate(str);
        return cimg::type<double>::nan();
      }

      static double mp_cut(_cimg_math_parser& mp) {
        double val = _mp_arg(2), cmin = _mp_arg(3), cmax = _mp_arg(4);
        return cimg::cut(val,cmin,cmax);
      }

      static double mp_da_back_or_pop(_cimg_math_parser& mp) {
        const bool is_pop_heap = mp.opcode[4]==2, is_pop = (bool)mp.opcode[4];
        const char *const s_op = is_pop_heap?"da_pop_heap":is_pop?"da_pop":"da_back";
        mp_check_list(mp,s_op);
        const unsigned int
          dim = (unsigned int)mp.opcode[2],
          ind = (unsigned int)cimg::mod((int)_mp_arg(3),mp.imglist.width());
        double *const ptrd = &_mp_arg(1) + (dim>1?1:0);
        CImg<T> &img = mp.imglist[ind];
        int siz = img?(int)cimg::float2uint(img[img._height - 1]):0;
        if (img && (img._width!=1 || img._depth!=1 || siz<0 || siz>img.height() - 1))
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function '%s()': "
                                      "Specified image #%u of size (%d,%d,%d,%d) cannot be used as dynamic array%s.",
                                      mp.imgout.pixel_type(),s_op,ind,
                                      img.width(),img.height(),img.depth(),img.spectrum(),
                                      img._width==1 && img._depth==1?"":" (contains invalid element counter)");
        if (!siz)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function '%s()': "
                                      "Specified dynamic array #%u contains no elements.",
                                      mp.imgout.pixel_type(),s_op,ind);
        const int siz1 = siz - 1;
        if (is_pop_heap) { // Heapify-down
          if (dim==1) cimg::swap(img[0],img[siz1]);
          else {
            T *ptr0 = img.data(), *ptr1 = img.data(0,siz1);
            cimg_forC(img,c) { cimg::swap(*ptr0,*ptr1); ptr0+=img._height; ptr1+=img._height; }
          }
          int index = 0;
          while (true) {
            const int child_left = 2*index + 1, child_right = child_left + 1;
            int smallest = index;
            if (child_left<siz1 && img[child_left]<img[smallest]) smallest = child_left;
            if (child_right<siz1 && img[child_right]<img[smallest]) smallest = child_right;
            if (smallest!=index) {
              if (dim==1) cimg::swap(img[index],img[smallest]);
              else {
                T *ptr0 = img.data(0,index), *ptr1 = img.data(0,smallest);
                cimg_forC(img,c) { cimg::swap(*ptr0,*ptr1); ptr0+=img._height; ptr1+=img._height; }
              }
              index = smallest;
            } else break;
          }
        }

        double ret = cimg::type<double>::nan();
        if (dim==1) ret = img[siz1]; // Scalar element
        else {
          const T *ptrs = img.data(0,siz1);
          cimg_forC(img,c) { ptrd[c] = *ptrs; ptrs+=img._height; } // Vector element
        }
        if (is_pop) { // Remove element from array
          --siz;
          if (img.height()>32 && siz<img.height()/8) // Reduce size of dynamic array
            img.resize(1,std::max(2*siz + 1,32),1,-100,0);
          img[img._height - 1] = cimg::uint2float(siz,(T)0);
        }
        return ret;
      }

      static double mp_da_freeze(_cimg_math_parser& mp) {
        const char *const s_op = "da_freeze";
        mp_check_list(mp,s_op);
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        CImg<T> &img = mp.imglist[ind];
        int siz = img?(int)cimg::float2uint(img[img._height - 1]):0;
        if (img && (img._width!=1 || img._depth!=1 || siz<0 || siz>img.height() - 1))
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function '%s()': "
                                      "Specified image #%u of size (%d,%d,%d,%d) cannot be used as dynamic array%s.",
                                      mp.imgout.pixel_type(),s_op,ind,
                                      img.width(),img.height(),img.depth(),img.spectrum(),
                                      img._width==1 && img._depth==1?"":" (contains invalid element counter)");
        if (siz) img.resize(1,siz,1,-100,0,0); else img.assign();
        return cimg::type<double>::nan();
      }

      static double mp_da_insert_or_push(_cimg_math_parser& mp) {
        const bool is_push_heap = mp.opcode[3]==~0U - 1, is_push = mp.opcode[3]>=~0U - 1;
        const char *const s_op = is_push_heap?"da_push_heap":is_push?"da_push":"da_insert";
        mp_check_list(mp,s_op);
        const unsigned int
          dim = (unsigned int)mp.opcode[4],
          _dim = std::max(1U,dim),
          nb_elts = (unsigned int)mp.opcode[5] - 6,
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        CImg<T> &img = mp.imglist[ind];
        const int
          siz = img?(int)cimg::float2uint(img[img._height - 1]):0,
          pos0 = is_push?siz:(int)_mp_arg(3),
          pos = pos0<0?pos0 + siz:pos0;

        if (img && _dim!=img._spectrum)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function '%s()': "
                                      "Element to insert has invalid size %u (should be %u).",
                                      mp.imgout.pixel_type(),s_op,_dim,img._spectrum);
        if (img && (img._width!=1 || img._depth!=1 || siz<0 || siz>img.height() - 1))
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function '%s()': "
                                      "Specified image #%u of size (%d,%d,%d,%d) cannot be used as dynamic array%s.",
                                      mp.imgout.pixel_type(),s_op,ind,
                                      img.width(),img.height(),img.depth(),img.spectrum(),
                                      img._width==1 && img._depth==1?"":" (contains invalid element counter)");
        if (pos<0 || pos>siz)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function '%s()': "
                                      "Invalid position %d (not in range -%d...%d).",
                                      mp.imgout.pixel_type(),s_op,pos0,siz,siz);

        if (siz + nb_elts + 1>=img._height) // Increase size of dynamic array, if necessary
          img.resize(1,2*siz + nb_elts + 1,1,_dim,0);

        if (pos!=siz) // Move existing data in dynamic array
          cimg_forC(img,c) std::memmove(img.data(0,pos + nb_elts,0,c),img.data(0,pos,0,c),(siz - pos)*sizeof(T));

        if (!dim) // Scalar or vector1() elements
          for (unsigned int k = 0; k<nb_elts; ++k) {
            int index = pos + k;
            img[index] = (T)_mp_arg(6 + k);
            if (is_push_heap) while (index>0) { // Heapify-up
                const int index_parent = (index - 1)/2;
                if (img[index]<img[index_parent]) {
                  cimg::swap(img[index],img[index_parent]);
                  index = index_parent; }
                else break;
              }
          }
        else // vectorN() elements, with N>1
          for (unsigned int k = 0; k<nb_elts; ++k) {
            int index = pos + k;
            const double *const ptrs = &_mp_arg(6 + k) + 1;
            T *ptrd = img.data(0,index);
            cimg_forC(img,c) { *ptrd = ptrs[c]; ptrd+=img._height; }
            if (is_push_heap) while (index>0) { // Heapify-up
                const int index_parent = (index - 1)/2;
                if (img[index]<img[index_parent]) {
                  T *ptr0 = img.data(0,index), *ptr1 = img.data(0,index_parent);
                  cimg_forC(img,c) { cimg::swap(*ptr0,*ptr1); ptr0+=img._height; ptr1+=img._height; }
                  index = index_parent;
                }
                else break;
              }
          }
        img[img._height - 1] = cimg::uint2float(siz + nb_elts,(T)0);
        return cimg::type<double>::nan();
      }

      static double mp_da_remove(_cimg_math_parser& mp) {
        mp_check_list(mp,"da_remove");
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        CImg<T> &img = mp.imglist[ind];
        int siz = img?(int)cimg::float2uint(img[img._height - 1]):0;
        if (img && (img._width!=1 || img._depth!=1 || siz<0 || siz>img.height() - 1))
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'da_remove()': "
                                      "Specified image #%u of size (%d,%d,%d,%d) cannot be used as dynamic array%s.",
                                      mp.imgout.pixel_type(),ind,
                                      img.width(),img.height(),img.depth(),img.spectrum(),
                                      img._width==1 && img._depth==1?"":" (contains invalid element counter)");
        if (!siz)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'da_remove()': "
                                      "Dynamic array is empty.",
                                      mp.imgout.pixel_type());
        int
          start0 = mp.opcode[3]==~0U?siz - 1:_mp_arg(3),
          end0 = mp.opcode[4]==~0U?start0:_mp_arg(4),
          start = start0<0?start0 + siz:start0,
          end = end0<0?end0 + siz:end0;
        if (start<0 || start>=siz || end<0 || end>=siz || start>end)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'da_remove()': "
                                      "Invalid starting (%d) and ending (%d) positions "
                                      "(not ordered, in range -%d...%d).",
                                      mp.imgout.pixel_type(),start0,end0,siz,siz - 1);
        if (end<siz - 1) // Move remaining data in dynamic array
          cimg_forC(img,c) std::memmove(img.data(0,start,0,c),img.data(0,end + 1,0,c),(siz - 1 - end)*sizeof(T));
        siz-=end - start + 1;
        if (img.height()>32 && siz<img.height()/8) // Reduce size of dynamic array
          img.resize(1,std::max(2*siz + 1,32),1,-100,0);
        img[img._height - 1] = cimg::uint2float(siz,(T)0);
        return cimg::type<double>::nan();
      }

      static double mp_da_size(_cimg_math_parser& mp) {
        mp_check_list(mp,"da_size");
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        CImg<T> &img = mp.imglist[ind];
        const int siz = img?(int)cimg::float2uint(img[img._height - 1]):0;
        if (img && (img._width!=1 || img._depth!=1 || siz<0 || siz>img.height() - 1))
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'da_size()': "
                                      "Specified image #%u of size (%d,%d,%d,%d) cannot be used as dynamic array%s.",
                                      mp.imgout.pixel_type(),ind,
                                      img.width(),img.height(),img.depth(),img.spectrum(),
                                      img._width==1 && img._depth==1?"":" (contains invalid element counter)");
        return siz;
      }

      static double mp_date(_cimg_math_parser& mp) {
        const unsigned int
          siz_out = (unsigned int)mp.opcode[2],
          siz_arg1 = (unsigned int)mp.opcode[4],
          siz_arg2 = (unsigned int)mp.opcode[6];
        double *ptr_out = &_mp_arg(1) + (siz_out?1:0);
        const double
          *ptr_arg1 = siz_arg1==~0U?0:&_mp_arg(3) + (siz_arg1?1:0),
          *ptr_arg2 = siz_arg2==~0U?0:&_mp_arg(5) + 1;

        if (!ptr_arg2) { // No filename specified
          if (!siz_arg1) return cimg::date((unsigned int)*ptr_arg1);
          if (siz_arg1==~0U) for (unsigned int k = 0; k<siz_out; ++k) ptr_out[k] = k;
          else for (unsigned int k = 0; k<siz_out; ++k) ptr_out[k] = ptr_arg1[k];
          cimg::date(ptr_out,siz_out);
          return cimg::type<double>::nan();
        }

        // Filename specified.
        CImg<charT> ss(siz_arg2 + 1);
        cimg_forX(ss,i) ss[i] = (char)ptr_arg2[i];
        ss.back() = 0;
        if (!siz_arg1) return cimg::fdate(ss,(unsigned int)*ptr_arg1);
        for (unsigned int k = 0; k<siz_out; ++k) ptr_out[k] = ptr_arg1[k];
        cimg::fdate(ss,ptr_out,siz_out);
        return cimg::type<double>::nan();
      }

      static double mp_debug(_cimg_math_parser& mp) {
        CImg<charT> expr(mp.opcode[2] - 4);
        {
          const ulongT *ptrs = mp.opcode._data + 4;
          cimg_for(expr,ptrd,char) *ptrd = (char)*(ptrs++);
        }
        cimg::strellipsize(expr);
        const ulongT g_target = mp.opcode[1];

#if cimg_use_openmp==0
        const unsigned int n_thread = 0;
#else
        const unsigned int n_thread = omp_get_thread_num();
#endif
        cimg_pragma_openmp(critical(mp_debug)) {
          std::fprintf(cimg::output(),
                       "\n[" cimg_appname "_math_parser] %p[thread #%u]:%*c"
                       "Start debugging '%s', code length: %u -> mem[%u] (memsize: %u)",
                       (void*)&mp,n_thread,mp.debug_indent,' ',
                       expr._data,(unsigned int)mp.opcode[3],(unsigned int)g_target,mp.mem._width);
          std::fflush(cimg::output());
          mp.debug_indent+=3;
        }
        const CImg<ulongT> *const p_end = ++mp.p_code + mp.opcode[3];
        CImg<ulongT> _op;
        while (mp.p_code<p_end) {
          const CImg<ulongT> &op = *mp.p_code;
          mp.opcode._data = op._data;

          _op.assign(1,op._height - 1);
          const ulongT *ptrs = op._data + 1;
          for (ulongT *ptrd = _op._data, *const ptrde = _op._data + _op._height; ptrd<ptrde; ++ptrd)
            *ptrd = *(ptrs++);

          const ulongT target = mp.opcode[1];
          mp.eval(mp.p_code,mp.p_code + 1);

#define _mp_debug(name) fn==mp_##name?#name
          const mp_func fn = (mp_func)*mp.opcode;
          const char *const s_fn =
            _mp_debug(abort): _mp_debug(abort): _mp_debug(abs): _mp_debug(abscut): _mp_debug(absmaxabs):
            _mp_debug(absminabs): _mp_debug(acos): _mp_debug(acosh): _mp_debug(add): _mp_debug(arg0):
            _mp_debug(arg1): _mp_debug(argkth): _mp_debug(argmax): _mp_debug(argmax2): _mp_debug(argmaxabs):
            _mp_debug(argmaxabs2): _mp_debug(argmin): _mp_debug(argmin2): _mp_debug(argminabs):
            _mp_debug(argminabs2): _mp_debug(asin): _mp_debug(asinh): _mp_debug(atan): _mp_debug(atan2):
            _mp_debug(atanh): _mp_debug(avg): _mp_debug(avg2): _mp_debug(bitwise_and):
            _mp_debug(bitwise_left_shift): _mp_debug(bitwise_not): _mp_debug(bitwise_or):
            _mp_debug(bitwise_right_shift): _mp_debug(bitwise_xor): _mp_debug(bool): _mp_debug(break):
            _mp_debug(breakpoint): _mp_debug(c2o): _mp_debug(cbrt): _mp_debug(ceil): _mp_debug(complex_abs):
            _mp_debug(complex_conj): _mp_debug(complex_cos): _mp_debug(complex_cosh):
            _mp_debug(complex_div_sv): _mp_debug(complex_div_vv): _mp_debug(complex_exp):
            _mp_debug(complex_log): _mp_debug(complex_mul): _mp_debug(complex_one): _mp_debug(complex_pow_ss):
            _mp_debug(complex_pow_sv): _mp_debug(complex_pow_vs): _mp_debug(complex_pow_vv):
            _mp_debug(complex_sin): _mp_debug(complex_sinh): _mp_debug(complex_sqr): _mp_debug(complex_sqrt):
            _mp_debug(complex_tan): _mp_debug(complex_tanh): _mp_debug(continue): _mp_debug(convolve):
            _mp_debug(copy): _mp_debug(correlate): _mp_debug(cos): _mp_debug(cosh): _mp_debug(cov):
            _mp_debug(critical): _mp_debug(cross): _mp_debug(cumulate): _mp_debug(cut):
            _mp_debug(da_back_or_pop): _mp_debug(da_freeze): _mp_debug(da_insert_or_push):
            _mp_debug(da_remove): _mp_debug(da_size): _mp_debug(date): _mp_debug(debug): _mp_debug(decrement):
            _mp_debug(deg2rad): _mp_debug(det): _mp_debug(diag): _mp_debug(div): _mp_debug(do): _mp_debug(dot):
            _mp_debug(echo): _mp_debug(ellipse): _mp_debug(epoch): _mp_debug(eq): _mp_debug(equalize):
            _mp_debug(erf): _mp_debug(erfinv): _mp_debug(exp): _mp_debug(expr): _mp_debug(eye):
            _mp_debug(f2ui): _mp_debug(factorial): _mp_debug(fft): _mp_debug(fibonacci): _mp_debug(find):
            _mp_debug(find_seq): _mp_debug(flood): _mp_debug(floor): _mp_debug(for): _mp_debug(frac):
            _mp_debug(fsize): _mp_debug(gamma): _mp_debug(gauss): _mp_debug(gcd): _mp_debug(gcd2):
            _mp_debug(get): _mp_debug(get): _mp_debug(gt): _mp_debug(gte): _mp_debug(histogram): _mp_debug(I):
            _mp_debug(i): _mp_debug(if): _mp_debug(IJoff): _mp_debug(ijoff): _mp_debug(IJxyz1):
            _mp_debug(IJxyz2): _mp_debug(IJxyz3): _mp_debug(ijxyzc1): _mp_debug(ijxyzc2): _mp_debug(ijxyzc3):
            _mp_debug(image_crop): _mp_debug(image_depth): _mp_debug(image_display): _mp_debug(image_draw):
            _mp_debug(image_find): _mp_debug(image_find_seq): _mp_debug(image_height):
            _mp_debug(image_is_shared): _mp_debug(image_median): _mp_debug(image_median_static):
            _mp_debug(image_norm): _mp_debug(image_norm_static): _mp_debug(image_print):
            _mp_debug(image_resize): _mp_debug(image_shift): _mp_debug(image_sort): _mp_debug(image_spectrum):
            _mp_debug(image_stats): _mp_debug(image_stats_static): _mp_debug(image_std_static):
            _mp_debug(image_swap): _mp_debug(image_wh): _mp_debug(image_whd): _mp_debug(image_whds):
            _mp_debug(image_width): _mp_debug(increment): _mp_debug(index): _mp_debug(indexof):
            _mp_debug(inrange): _mp_debug(int): _mp_debug(isbool): _mp_debug(isdir): _mp_debug(isfile):
            _mp_debug(isfinite): _mp_debug(isin): _mp_debug(isinf): _mp_debug(isint): _mp_debug(isnan):
            _mp_debug(isvarname): _mp_debug(kth): _mp_debug(lcm): _mp_debug(lcm2): _mp_debug(lerp):
            _mp_debug(log): _mp_debug(log10): _mp_debug(log2): _mp_debug(logical_and): _mp_debug(logical_not):
            _mp_debug(logical_or): _mp_debug(logit): _mp_debug(lowercase): _mp_debug(lt): _mp_debug(lte):
            _mp_debug(map): _mp_debug(matrix_eigen): _mp_debug(matrix_invert): _mp_debug(matrix_mul):
            _mp_debug(matrix_qr): _mp_debug(matrix_svd): _mp_debug(max): _mp_debug(max2): _mp_debug(maxabs):
            _mp_debug(maxabs2): _mp_debug(med): _mp_debug(med2): _mp_debug(mem_copy): _mp_debug(mem_display):
            _mp_debug(min): _mp_debug(min2): _mp_debug(minabs): _mp_debug(minabs2): _mp_debug(minus):
            _mp_debug(mirror): _mp_debug(modulo): _mp_debug(mproj): _mp_debug(mse): _mp_debug(mul):
            _mp_debug(mul_add): _mp_debug(mul_mul): _mp_debug(mul_sub): _mp_debug(name): _mp_debug(name):
            _mp_debug(neq): _mp_debug(noise): _mp_debug(normalize): _mp_debug(normp): _mp_debug(o2c):
            _mp_debug(permutations): _mp_debug(permute): _mp_debug(polygon): _mp_debug(pow):
            _mp_debug(pow0_25): _mp_debug(pow3): _mp_debug(pow4): _mp_debug(print): _mp_debug(prod):
            _mp_debug(prod2): _mp_debug(rad2deg): _mp_debug(rand_double): _mp_debug(rand_double_0_1):
            _mp_debug(rand_double_0_N): _mp_debug(rand_double_ext): _mp_debug(rand_double_gaussian):
            _mp_debug(rand_double_m1_1): _mp_debug(rand_int): _mp_debug(rand_int_0_1): _mp_debug(rand_int_0_N):
            _mp_debug(rand_int_ext): _mp_debug(rand_int_m1_1): _mp_debug(repeat): _mp_debug(reverse):
            _mp_debug(rol): _mp_debug(ror): _mp_debug(rot2d): _mp_debug(rot3d): _mp_debug(round):
            _mp_debug(run): _mp_debug(run): _mp_debug(s2v): _mp_debug(self_add): _mp_debug(self_bitwise_and):
            _mp_debug(self_bitwise_left_shift): _mp_debug(self_bitwise_or):
            _mp_debug(self_bitwise_right_shift): _mp_debug(self_decrement): _mp_debug(self_div):
            _mp_debug(self_increment): _mp_debug(self_map_vector_s): _mp_debug(self_map_vector_v):
            _mp_debug(self_modulo): _mp_debug(self_mul): _mp_debug(self_pow): _mp_debug(self_sub):
            _mp_debug(set): _mp_debug(set): _mp_debug(set_ijoff): _mp_debug(set_IJoff_s):
            _mp_debug(set_IJoff_v): _mp_debug(set_IJxyz_s): _mp_debug(set_IJxyz_v): _mp_debug(set_ijxyzc):
            _mp_debug(sigmoid): _mp_debug(sign): _mp_debug(sin): _mp_debug(sinc): _mp_debug(sinh):
            _mp_debug(size_list): _mp_debug(softargmax): _mp_debug(softargmin): _mp_debug(softmax):
            _mp_debug(softmin): _mp_debug(solve): _mp_debug(sort): _mp_debug(sqr): _mp_debug(sqrt):
            _mp_debug(srand): _mp_debug(srand0): _mp_debug(std): _mp_debug(std2): _mp_debug(store):
            _mp_debug(store): _mp_debug(string): _mp_debug(string_init): _mp_debug(sub): _mp_debug(sub_mul):
            _mp_debug(sum): _mp_debug(sum2): _mp_debug(swap): _mp_debug(tan): _mp_debug(tanh):
            _mp_debug(trace): _mp_debug(transpose): _mp_debug(ui2f): _mp_debug(unitnorm): _mp_debug(uppercase):
            _mp_debug(v2s): _mp_debug(var): _mp_debug(var2): _mp_debug(vargkth): _mp_debug(vargmax):
            _mp_debug(vargmaxabs): _mp_debug(vargmin): _mp_debug(vargminabs): _mp_debug(vavg):
            _mp_debug(vector_copy): _mp_debug(vector_crop): _mp_debug(vector_crop_ext):
            _mp_debug(vector_display): _mp_debug(vector_draw): _mp_debug(vector_eq): _mp_debug(vector_fill):
            _mp_debug(vector_fill_ext): _mp_debug(vector_hypot): _mp_debug(vector_init):
            _mp_debug(vector_lerp): _mp_debug(vector_map_sv): _mp_debug(vector_map_v):
            _mp_debug(vector_map_vv): _mp_debug(vector_neq): _mp_debug(vector_norm0): _mp_debug(vector_norm1):
            _mp_debug(vector_norm2): _mp_debug(vector_norminf): _mp_debug(vector_off): _mp_debug(vector_print):
            _mp_debug(vector_rand): _mp_debug(vector_resize): _mp_debug(vector_resize_ext):
            _mp_debug(vector_set_off): _mp_debug(vector_shift): _mp_debug(vector_stats): _mp_debug(vkth):
            _mp_debug(vmax): _mp_debug(vmaxabs): _mp_debug(vmedian): _mp_debug(vmin): _mp_debug(vminabs):
            _mp_debug(vprod): _mp_debug(vstd): _mp_debug(vsum): _mp_debug(vvar): _mp_debug(warp):
            _mp_debug(wave): _mp_debug(while):
            "unknown";

          cimg_pragma_openmp(critical(mp_debug)) {
            std::fprintf(cimg::output(),
                         "\n[" cimg_appname "_math_parser] %p[thread #%u]:%*c"
                         "Opcode %p = [ %p (%s),%s ] -> mem[%u] = %.17g",
                         (void*)&mp,n_thread,mp.debug_indent,' ',
                         (void*)mp.opcode._data,(void*)*mp.opcode,s_fn,_op.value_string().data(),
                         (unsigned int)target,mp.mem[target]);
            std::fflush(cimg::output());
          }
        }
        cimg_pragma_openmp(critical(mp_debug)) {
          mp.debug_indent-=3;
          std::fprintf(cimg::output(),
                       "\n[" cimg_appname "_math_parser] %p[thread #%u]:%*c"
                       "End debugging '%s' -> mem[%u] = %.17g (memsize: %u)",
                       (void*)&mp,n_thread,mp.debug_indent,' ',
                       expr._data,(unsigned int)g_target,mp.mem[g_target],mp.mem._width);
          std::fflush(cimg::output());
        }
        --mp.p_code;
        return mp.mem[g_target];
      }

      static double mp_decrement(_cimg_math_parser& mp) {
        return _mp_arg(2) - 1;
      }

      static double mp_deg2rad(_cimg_math_parser& mp) {
        return _mp_arg(2)*cimg::PI/180;
      }

      static double mp_det(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2) + 1;
        const unsigned int k = (unsigned int)mp.opcode[3];
        return CImg<doubleT>(ptrs,k,k,1,1,true).det();
      }

      static double mp_diag(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2], siz = mp.opcode[2] - 3;
        double *ptrd = &_mp_arg(1) + 1;
        std::memset(ptrd,0,siz*siz*sizeof(double));
        for (unsigned int i = 3; i<i_end; ++i) { *(ptrd++) = _mp_arg(i); ptrd+=siz; }
        return cimg::type<double>::nan();
      }

      static double mp_div(_cimg_math_parser& mp) {
        return _mp_arg(2)/_mp_arg(3);
      }

      static double mp_do(_cimg_math_parser& mp) {
        const ulongT
          mem_body = mp.opcode[1],
          mem_cond = mp.opcode[2];
        const CImg<ulongT>
          *const p_body = ++mp.p_code,
          *const p_cond = p_body + mp.opcode[3],
          *const p_end = p_cond + mp.opcode[4];
        const unsigned int siz = (unsigned int)mp.opcode[5];
        if (mp.opcode[6]) { // Set default value for result and condition if necessary
          if (siz) CImg<doubleT>(&mp.mem[mem_body] + 1,siz,1,1,1,true).fill(cimg::type<double>::nan());
          else mp.mem[mem_body] = cimg::type<double>::nan();
        }
        if (mp.opcode[7]) mp.mem[mem_cond] = 0;
        const unsigned int _break_type = mp.break_type;
        mp.break_type = 0;
        do {
          mp.eval(p_body,p_cond); // Evaluate body
          if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
          mp.eval(p_cond,p_end); // Evaluate condition
          if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
        } while (mp.mem[mem_cond]);
        mp.break_type = _break_type;
        mp.p_code = p_end - 1;
        return mp.mem[mem_body];
      }

      static double mp_dot(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[4];
        return CImg<doubleT>(&_mp_arg(2) + 1,1,siz,1,1,true).
          dot(CImg<doubleT>(&_mp_arg(3) + 1,1,siz,1,1,true));
      }

      static double mp_echo(_cimg_math_parser& mp) {
        const unsigned int nb_args = (unsigned int)(mp.opcode[2] - 3)/2;
        if (!nb_args) { std::fputc('\n',cimg::output()); return cimg::type<double>::nan(); } // No arguments
        CImgList<charT> _str;
        CImg<charT> it;
        for (unsigned int n = 0; n<nb_args; ++n) {
          const unsigned int siz = (unsigned int)mp.opcode[4 + 2*n];
          if (siz) { // Vector argument -> string
            const double *ptr = &_mp_arg(3 + 2*n) + 1;
            unsigned int l = 0;
            while (l<siz && ptr[l]) ++l;
            CImg<doubleT>(ptr,l,1,1,1,true).move_to(_str);
          } else { // Scalar argument -> number
            it.assign(24);
            cimg_snprintf(it,it._width,"%.17g",_mp_arg(3 + 2*n));
            CImg<charT>::string(it,false,true).move_to(_str);
          }
        }
        CImg(1,1,1,1,0).move_to(_str);
        const CImg<charT> str = _str>'x';
        std::fprintf(cimg::output(),"\n%s",str._data);
        return cimg::type<double>::nan();
      }

      static double mp_ellipse(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        unsigned int ind = (unsigned int)mp.opcode[3];
        if (ind!=~0U) {
          mp_check_list(mp,"ellipse");
          ind = (unsigned int)cimg::mod((int)_mp_arg(3),mp.imglist.width());
        }
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        CImg<T> color(img._spectrum,1,1,1,0);
        bool is_invalid_arguments = false, is_outlined = false;
        float r1 = 0, r2 = 0, angle = 0, opacity = 1;
        unsigned int i = 4, pattern = ~0U;
        int x0 = 0, y0 = 0;
        if (i>=i_end) is_invalid_arguments = true;
        else {
          x0 = (int)cimg::round(_mp_arg(i++));
          if (i>=i_end) is_invalid_arguments = true;
          else {
            y0 = (int)cimg::round(_mp_arg(i++));
            if (i>=i_end) is_invalid_arguments = true;
            else {
              r1 = (float)_mp_arg(i++);
              if (i>=i_end) r2 = r1;
              else {
                r2 = (float)_mp_arg(i++);
                if (i<i_end) {
                  angle = (float)(_mp_arg(i++)*180/cimg::PI);
                  if (i<i_end) {
                    opacity = (float)_mp_arg(i++);
                    if (r1<0 && r2<0) {
                      pattern = (unsigned int)_mp_arg(i++);
                      is_outlined = true;
                      r1 = -r1; r2 = -r2;
                    }
                    if (i<i_end) {
                      cimg_forX(color,k) if (i<i_end) color[k] = (T)_mp_arg(i++);
                      else { color.resize(k,1,1,1,-1); break; }
                      color.resize(img._spectrum,1,1,1,0,2);
                    }
                  }
                }
              }
            }
          }
        }
        if (!is_invalid_arguments) {
          if (is_outlined) img.draw_ellipse(x0,y0,r1,r2,angle,color._data,opacity,pattern);
          else img.draw_ellipse(x0,y0,r1,r2,angle,color._data,opacity);
        } else {
          CImg<doubleT> args(i_end - 4);
          cimg_forX(args,k) args[k] = _mp_arg(4 + k);
          if (ind==~0U)
            throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'ellipse()': "
                                        "Invalid arguments '%s'. ",
                                        mp.imgin.pixel_type(),args.value_string()._data);
          else
            throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'ellipse()': "
                                        "Invalid arguments '#%u%s%s'. ",
                                        mp.imgin.pixel_type(),ind,args._width?",":"",args.value_string()._data);
        }
        return cimg::type<double>::nan();
      }

      static double mp_epoch(_cimg_math_parser& mp) {
        unsigned int
          year = (unsigned int)mp.opcode[2]==~0U?~0U:(unsigned int)std::max(_mp_arg(2),1900.),
          month = (unsigned int)mp.opcode[3]==~0U?~0U:(unsigned int)cimg::cut(_mp_arg(3),1.,12.),
          day = (unsigned int)mp.opcode[4]==~0U?~0U:(unsigned int)cimg::cut(_mp_arg(4),1.,31.),
          hour = (unsigned int)mp.opcode[5]==~0U?~0U:(unsigned int)cimg::cut(_mp_arg(5),0.,23.),
          minute = (unsigned int)mp.opcode[6]==~0U?~0U:(unsigned int)cimg::cut(_mp_arg(6),0.,59.),
          second = (unsigned int)mp.opcode[7]==~0U?~0U:(unsigned int)cimg::cut(_mp_arg(7),0.,60.);
        if (year==~0U && month==~0U && day==~0U &&
            hour==~0U && minute==~0U && second==~0U) // No argument -> current date
          return (double)cimg::epoch(cimg::date(0),cimg::date(1),cimg::date(2),
                                     cimg::date(4),cimg::date(5),cimg::date(6));
        if (year==~0U) year = (unsigned int)cimg::date(0);
        if (month==~0U) month = 1U;
        if (day==~0U) day = 1U;
        if (hour==~0U) hour = 0U;
        if (minute==~0U) minute = 0U;
        if (second==~0U) second = 0U;
        return (double)cimg::epoch((int)year,(int)month,(int)day,(int)hour,(int)minute,(int)second);
      }

      static double mp_eq(_cimg_math_parser& mp) {
        return (double)(_mp_arg(2)==_mp_arg(3));
      }

      static double mp_equalize(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int
          siz = (unsigned int)mp.opcode[3],
          nb_levels = (unsigned int)mp.opcode[4];
        const double *const ptrs = &_mp_arg(2) + 1;
        CImg<doubleT> img(ptrs,siz,1,1,1,true);
        double min_value = 0, max_value = 0;
        if ((unsigned int)mp.opcode[5]==~0U || (unsigned int)mp.opcode[6]==~0U)
          min_value = img.min_max(max_value);
        if ((unsigned int)mp.opcode[5]!=~0U) min_value = _mp_arg(5);
        if ((unsigned int)mp.opcode[6]!=~0U) max_value = _mp_arg(6);
        CImg<doubleT>(ptrd,siz,1,1,1,true) = img.get_equalize(nb_levels,min_value,max_value);
        return cimg::type<double>::nan();
      }

#if cimg_use_cpp11==1
      static double mp_erf(_cimg_math_parser& mp) {
        return std::erf(_mp_arg(2));
      }
#endif

      static double mp_erfinv(_cimg_math_parser& mp) {
        return cimg::erfinv(_mp_arg(2));
      }

      static double mp_exp(_cimg_math_parser& mp) {
        return std::exp(_mp_arg(2));
      }

      static double mp_expr(_cimg_math_parser& mp) {
        const unsigned int
          sizs = (unsigned int)mp.opcode[3],
          w = (unsigned int)mp.opcode[4],
          h = (unsigned int)mp.opcode[5],
          d = (unsigned int)mp.opcode[6],
          s = (unsigned int)mp.opcode[7],
          A = (unsigned int)mp.opcode[8],
          sA = (unsigned int)mp.opcode[9];
        const double
          *ptrs = &_mp_arg(2) + 1,
          *ptrA = A==~0U?0:&_mp_arg(8) + 1;
        double *ptrd = &_mp_arg(1) + 1;
        CImg<charT> ss(sizs + 1);
        cimg_for_inX(ss,0,ss.width() - 2,i) ss[i] = (char)ptrs[i];
        ss.back() = 0;
        if (ptrA) {
          CImg<doubleT>
            out(ptrd,w,h,d,s,true),
            in = CImg<doubleT>(ptrA,w,h,d,sA);
          if (sA<s) in.channels(0,s - 1);
          in._fill(ss,true,3,&mp.imglist,"mp_expr",0,0);
          if (sA==s) out = in; else out.draw_image(in);
        } else
          CImg<doubleT>(ptrd,w,h,d,s,true) = CImg<doubleT>(w,h,d,s,0).
            _fill(ss,true,3,&mp.imglist,"mp_expr",0,0);
        return cimg::type<double>::nan();
      }

      static double mp_eye(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const unsigned int k = (unsigned int)mp.opcode[2];
        CImg<doubleT>(ptrd,k,k,1,1,true).identity_matrix();
        return cimg::type<double>::nan();
      }

      static double mp_f2ui(_cimg_math_parser& mp) {
        return (double)cimg::float2uint((T)_mp_arg(2));
      }

      static double mp_factorial(_cimg_math_parser& mp) {
        return cimg::factorial((int)_mp_arg(2));
      }

      static double mp_fibonacci(_cimg_math_parser& mp) {
        return cimg::fibonacci((int)_mp_arg(2));
      }

      static double mp_find(_cimg_math_parser& mp) {
        const int _step = (int)_mp_arg(6), step = _step?_step:-1;
        const ulongT siz = (ulongT)mp.opcode[3];
        longT ind = (longT)(mp.opcode[5]!=_cimg_mp_slot_nan?_mp_arg(5):step>0?0:siz - 1);
        if (ind<0 || ind>=(longT)siz) return -1.;
        const double
          *const ptrb = &_mp_arg(2) + 1,
          *const ptre = ptrb + siz,
          val = _mp_arg(4),
          *ptr = ptrb + ind;

        // Forward search.
        if (step>0) {
          if (cimg::type<double>::is_nan(val))
            while (ptr<ptre && !cimg::type<double>::is_nan(*ptr)) ptr+=step;
          else
            while (ptr<ptre && *ptr!=val) ptr+=step;
          return ptr>=ptre?-1.:(double)(ptr - ptrb);
        }

        // Backward search.
        if (cimg::type<double>::is_nan(val))
          while (ptr>=ptrb && !cimg::type<double>::is_nan(*ptr)) ptr+=step;
        else
          while (ptr>=ptrb && *ptr!=val) ptr+=step;
        return ptr<ptrb?-1.:(double)(ptr - ptrb);
      }

      static double mp_find_seq(_cimg_math_parser& mp) {
        const int _step = (int)_mp_arg(7), step = _step?_step:-1;
        const ulongT
          siz1 = (ulongT)mp.opcode[3],
          siz2 = (ulongT)mp.opcode[5];
        longT ind = (longT)(mp.opcode[6]!=_cimg_mp_slot_nan?_mp_arg(6):step>0?0:siz1 - 1);
        if (ind<0 || ind>=(longT)siz1) return -1.;
        const double
          *const ptr1b = &_mp_arg(2) + 1,
          *const ptr1e = ptr1b + siz1,
          *const ptr2b = &_mp_arg(4) + 1,
          *const ptr2e = ptr2b + siz2,
          *ptr1 = ptr1b + ind,
          *p1 = 0,
          *p2 = 0;

        // Forward search.
        if (step>0) {
          do {
            if (cimg::type<double>::is_nan(*ptr2b))
              while (ptr1<ptr1e && !cimg::type<double>::is_nan(*ptr1)) ptr1+=step;
            else
              while (ptr1<ptr1e && *ptr1!=*ptr2b) ptr1+=step;
            if (ptr1>=ptr1e) return -1.;
            p1 = ptr1 + 1;
            p2 = ptr2b + 1;
            while (p1<ptr1e && p2<ptr2e &&
                   ((cimg::type<double>::is_nan(*p1) && cimg::type<double>::is_nan(*p2)) || *p1==*p2)) { ++p1; ++p2; }
          } while (p2<ptr2e && (ptr1+=step)<ptr1e);
          return p2<ptr2e?-1.:(double)(ptr1 - ptr1b);
        }

        // Backward search.
        do {
          if (cimg::type<double>::is_nan(*ptr2b))
            while (ptr1>=ptr1b && !cimg::type<double>::is_nan(*ptr1)) ptr1+=step;
          else
            while (ptr1>=ptr1b && *ptr1!=*ptr2b) ptr1+=step;
          if (ptr1<ptr1b) return -1.;
          p1 = ptr1 + 1;
          p2 = ptr2b + 1;
          while (p1<ptr1e && p2<ptr2e &&
                 ((cimg::type<double>::is_nan(*p1) && cimg::type<double>::is_nan(*p2)) || *p1==*p2)) { ++p1; ++p2; }
        } while (p2<ptr2e && (ptr1+=step)>=ptr1b);
        return p2<ptr2e?-1.:(double)(ptr1 - ptr1b);
      }

      static double mp_fft(_cimg_math_parser& mp) {
        const bool is_ifft = (bool)mp.opcode[2];
        double *const ptr_r = &_mp_arg(3) + 1, *const ptr_i = &_mp_arg(4) + 1;
        const unsigned int
          w = (unsigned int)mp.opcode[5],
          h = (unsigned int)mp.opcode[6],
          d = (unsigned int)mp.opcode[7],
          s = (unsigned int)mp.opcode[8];
        unsigned int
          sa = (unsigned int)mp.opcode[10];
        const double *const axes = &_mp_arg(9) + (sa?1:0);
        if (!sa) ++sa;
        CImgList<doubleT> pair(2);
        pair[0].assign(ptr_r,w,h,d,s,true); // Real part
        pair[1].assign(ptr_i,w,h,d,s,true); // Imaginary part
        if (*axes) for (unsigned int k = 0; k<sa; ++k) {
            const double axis = axes[k];
            if (axis=='x' || axis=='y' || axis=='z') pair.FFT(axis,is_ifft);
            else {
              CImg<char> str(sa + 1); str.get_shared_points(0,sa - 1) = CImg<double>(axes,sa); str.back() = 0;
              throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function mp_%s()': "
                                          "Invalid specified axes '%s' (should contain only 'x','y' or 'z').",
                                          mp.imgout.pixel_type(),is_ifft?"ifft":"fft",
                                          str._data);
            }
          } else pair.FFT(is_ifft);
        return cimg::type<double>::nan();
      }

      static double mp_flood(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        unsigned int ind = (unsigned int)mp.opcode[3];
        if (ind!=~0U) {
          mp_check_list(mp,"flood");
          ind = (unsigned int)cimg::mod((int)_mp_arg(3),mp.imglist.width());
        }
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        CImg<T> color(img._spectrum,1,1,1,0);
        bool is_high_connectivity = false;
        float tolerance = 0, opacity = 1;
        int x0 = 0, y0 = 0, z0 = 0;
        unsigned int i = 4;
        if (i<i_end) {
          x0 = (int)cimg::round(_mp_arg(i++));
          if (i<i_end) {
            y0 = (int)cimg::round(_mp_arg(i++));
            if (i<i_end) {
              z0 = (int)cimg::round(_mp_arg(i++));
              if (i<i_end) {
                tolerance = (float)_mp_arg(i++);
                if (i<i_end) {
                  is_high_connectivity = (bool)_mp_arg(i++);
                  if (i<i_end) {
                    opacity = (float)_mp_arg(i++);
                    if (i<i_end) {
                      cimg_forX(color,k) if (i<i_end) color[k] = (T)_mp_arg(i++);
                      else { color.resize(k,1,1,1,-1); break; }
                      color.resize(img._spectrum,1,1,1,0,2);
                    }
                  }
                }
              }
            }
          }
        }
        img.draw_fill(x0,y0,z0,color._data,opacity,tolerance,is_high_connectivity);
        return cimg::type<double>::nan();
      }

      static double mp_floor(_cimg_math_parser& mp) {
        return std::floor(_mp_arg(2));
      }

      static double mp_for(_cimg_math_parser& mp) {
        const ulongT
          mem_body = mp.opcode[1],
          mem_cond = mp.opcode[3];
        const CImg<ulongT>
          *const p_init = ++mp.p_code,
          *const p_cond = p_init + mp.opcode[4],
          *const p_body = p_cond + mp.opcode[5],
          *const p_post = p_body + mp.opcode[6],
          *const p_end = p_post + mp.opcode[7];
        const unsigned int siz = (unsigned int)mp.opcode[2];
        bool is_cond = false;
        if (mp.opcode[8]) { // Set default value for result and condition if necessary
          if (siz) CImg<doubleT>(&mp.mem[mem_body] + 1,siz,1,1,1,true).fill(cimg::type<double>::nan());
          else mp.mem[mem_body] = cimg::type<double>::nan();
        }
        if (mp.opcode[9]) mp.mem[mem_cond] = 0;
        const unsigned int _break_type = mp.break_type;
        mp.break_type = 0;
        mp.eval(p_init,p_cond); // Evaluate init
        if (!mp.break_type) do {
            mp.eval(p_cond,p_body); // Evaluate condition
            if (mp.break_type==1) break;
            is_cond = (bool)mp.mem[mem_cond];
            if (is_cond && !mp.break_type) {
              mp.eval(p_body,p_post); // Evaluate body
              if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
              mp.eval(p_post,p_end); // Evaluate post-code
              if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
            }
          } while (is_cond);

        mp.break_type = _break_type;
        mp.p_code = p_end - 1;
        return mp.mem[mem_body];
      }

      static double mp_frac(_cimg_math_parser& mp) {
        return cimg::frac(_mp_arg(2));
      }

      static double mp_fsize(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2) + 1;
        const ulongT siz = (ulongT)mp.opcode[3];
        CImg<charT> ss(siz + 1);
        cimg_forX(ss,i) ss[i] = (char)ptrs[i];
        ss.back() = 0;
        return (double)cimg::fsize(ss);
      }

#if cimg_use_cpp11==1
      static double mp_gamma(_cimg_math_parser& mp) {
        return std::tgamma(_mp_arg(2));
      }
#endif

      static double mp_gauss(_cimg_math_parser& mp) {
        const double x = _mp_arg(2), s = _mp_arg(3);
        return std::exp(-x*x/(2*s*s))/(_mp_arg(4)?std::sqrt(2*s*s*cimg::PI):1);
      }

      static double mp_gcd(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        CImg<cimg_int64> values;
        if (i_end==5) { // Only a single argument
          if ((unsigned)mp.opcode[4]==1) return _mp_arg(3); // Real value
          else values.assign(&_mp_arg(3),(unsigned int)mp.opcode[4]); // Vector value
        } else if (i_end==7 && (unsigned int)mp.opcode[4]==1 && (unsigned int)mp.opcode[6]==1) // Two real arguments
          return (double)cimg::gcd((cimg_int64)_mp_arg(3),(cimg_int64)_mp_arg(5));
        else {
          unsigned int siz = 0;
          for (unsigned int i = 4; i<i_end; i+=2) siz+=(unsigned int)mp.opcode[i];
          values.assign(siz);
          cimg_int64 *ptr = values;
          for (unsigned int i = 3; i<i_end; i+=2) {
            const unsigned int len = (unsigned int)mp.opcode[i + 1];
            double *ptrs = &_mp_arg(i);
            if (len>1) for (unsigned int k = 0; k<len; ++k) *(ptr++) = (cimg_int64)ptrs[k];
            else *(ptr++) = (cimg_int64)_mp_arg(i);
          }
        }
        return values.gcd();
      }

      static double mp_gcd2(_cimg_math_parser& mp) {
        return (double)cimg::gcd((cimg_int64)_mp_arg(2),(cimg_int64)_mp_arg(3));
      }

#ifdef cimg_mp_func_get
      static double mp_get(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2) + 1;
        double *ptrd = &_mp_arg(1);
        const unsigned int
          sizs = (unsigned int)mp.opcode[3],
          sizd = (unsigned int)mp.opcode[4];
        const bool to_string = (bool)mp.opcode[5];
        CImg<charT> ss(sizs + 1);
        cimg_for_inX(ss,0,ss.width() - 2,i) ss[i] = (char)ptrs[i];
        ss.back() = 0;
        if (sizd) cimg_mp_func_get(ptrd + 1,sizd,to_string,ss._data);
        else cimg_mp_func_get(ptrd,0,to_string,ss._data);
        return cimg::type<double>::nan();
      }
#else
      static double mp_get(_cimg_math_parser& mp) { cimg::unused(mp); return cimg::type<double>::nan(); }
#endif

      static double mp_gt(_cimg_math_parser& mp) {
        return (double)(_mp_arg(2)>_mp_arg(3));
      }

      static double mp_gte(_cimg_math_parser& mp) {
        return (double)(_mp_arg(2)>=_mp_arg(3));
      }

      static double mp_histogram(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int
          siz = (unsigned int)mp.opcode[3],
          nb_levels = (unsigned int)mp.opcode[4];
        const double *const ptrs = &_mp_arg(2) + 1;
        CImg<doubleT> img(ptrs,siz,1,1,1,true);
        double min_value = 0, max_value = 0;
        if ((unsigned int)mp.opcode[5]==~0U || (unsigned int)mp.opcode[6]==~0U)
          min_value = img.min_max(max_value);
        if ((unsigned int)mp.opcode[5]!=~0U) min_value = _mp_arg(5);
        if ((unsigned int)mp.opcode[6]!=~0U) max_value = _mp_arg(6);
        CImg<doubleT>(ptrd,nb_levels,1,1,1,true) = img.get_histogram(nb_levels,min_value,max_value);
        return cimg::type<double>::nan();
      }

      static double mp_i(_cimg_math_parser& mp) {
        if (mp.imgin)
          return (double)mp.imgin((int)mp.mem[_cimg_mp_slot_x],(int)mp.mem[_cimg_mp_slot_y],
                                  (int)mp.mem[_cimg_mp_slot_z],(int)mp.mem[_cimg_mp_slot_c]);
        return 0;
      }

      static double mp_if(_cimg_math_parser& mp) {
        const bool is_cond = (bool)_mp_arg(2);
        const ulongT
          mem_left = mp.opcode[3],
          mem_right = mp.opcode[4];
        const CImg<ulongT>
          *const p_right = ++mp.p_code + mp.opcode[5],
          *const p_end = p_right + mp.opcode[6];
        const unsigned int vtarget = (unsigned int)mp.opcode[1], siz = (unsigned int)mp.opcode[7];
        if (is_cond) mp.eval(mp.p_code,p_right); else mp.eval(p_right,p_end);
        if (mp.p_code==mp.p_break) --mp.p_code; else mp.p_code = p_end - 1;
        if (siz) std::memcpy(&mp.mem[vtarget] + 1,&mp.mem[is_cond?mem_left:mem_right] + 1,sizeof(double)*siz);
        return mp.mem[is_cond?mem_left:mem_right];
      }

      static double mp_ijoff(_cimg_math_parser& mp) {
        const bool is_relative = (bool)mp.opcode[2];
        const unsigned int
          _ind = (unsigned int)mp.opcode[3],
          ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width()),
          boundary_conditions = (unsigned int)_mp_arg(5);
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        const longT
          off = (longT)(is_relative?img.offset((int)mp.mem[_cimg_mp_slot_x],
                                               (int)mp.mem[_cimg_mp_slot_y],
                                               (int)mp.mem[_cimg_mp_slot_z],
                                               (int)mp.mem[_cimg_mp_slot_c]) + _mp_arg(4):_mp_arg(4)),
          whds = (longT)img.size();
        if (off>=0 && off<whds) return (double)img[off];
        if (img._data) switch (boundary_conditions) {
          case 3 : { // Mirror
            const longT whds2 = 2*whds, moff = cimg::mod(off,whds2);
            return (double)img[moff<whds?moff:whds2 - moff - 1];
          }
          case 2 : // Periodic
            return (double)img[cimg::mod(off,whds)];
          case 1 : // Neumann
            return (double)img[off<0?0:whds - 1];
          default : // Dirichlet
            return 0;
          }
        return 0;
      }

      static double mp_ijxyzc3(_cimg_math_parser& mp) {
        const bool is_relative = (bool)mp.opcode[2];
        const unsigned int
          _ind = (unsigned int)mp.opcode[3],
          ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width()),
          interpolation = (unsigned int)_mp_arg(8),
          boundary_conditions = (unsigned int)_mp_arg(9);
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        const double
          x = is_relative?(mp.mem[_cimg_mp_slot_x] + _mp_arg(4)):_mp_arg(4),
          y = is_relative?(mp.mem[_cimg_mp_slot_y] + _mp_arg(5)):_mp_arg(5),
          z = is_relative?(mp.mem[_cimg_mp_slot_z] + _mp_arg(6)):_mp_arg(6);
        const int c = (int)(is_relative?(mp.mem[_cimg_mp_slot_c] + _mp_arg(7)):_mp_arg(7));
        switch (interpolation) {
        case 2 : // Cubic interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              mc = cimg::mod(c,s2);
            const float
              mx = cimg::mod((float)x,(float)w2),
              my = cimg::mod((float)y,(float)h2),
              mz = cimg::mod((float)z,(float)d2);
            return (double)img._cubic_atXYZ(mx<img.width()?mx:w2 - mx - 1,my<img.height()?my:h2 - my - 1,
                                            mz<img.depth()?mz:d2 - mz - 1,mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img._cubic_atXYZ_p((float)x,(float)y,(float)z,cimg::mod(c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._cubic_atXYZ((float)x,(float)y,(float)z,c<0?0:c>=img.spectrum()?img.spectrum() - 1:c);
          default : // Dirichlet
            if (c<0 || c>=img.spectrum()) return (T)0;
            return (double)img.cubic_atXYZ((float)x,(float)y,(float)z,c,(T)0);
          }
        case 1 : // Linear interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              mc = cimg::mod(c,s2);
            const float
              mx = cimg::mod((float)x,(float)w2),
              my = cimg::mod((float)y,(float)h2),
              mz = cimg::mod((float)z,(float)d2);
            return (double)img._linear_atXYZ(mx<img.width()?mx:w2 - mx - 1,my<img.height()?my:h2 - my - 1,
                                             mz<img.depth()?mz:d2 - mz - 1,mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img._linear_atXYZ_p((float)x,(float)y,(float)z,cimg::mod(c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._linear_atXYZ((float)x,(float)y,(float)z,c<0?0:c>=img.spectrum()?img.spectrum() - 1:c);
          default : // Dirichlet
            if (c<0 || c>=img.spectrum()) return (T)0;
            return (double)img.linear_atXYZ((float)x,(float)y,(float)z,c,(T)0);
          }
        default : // Nearest neighbor interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              mc = cimg::mod(c,s2),
              mx = cimg::mod((int)x,w2),
              my = cimg::mod((int)y,h2),
              mz = cimg::mod((int)z,d2);
            return (double)img(mx<img.width()?mx:w2 - mx - 1,my<img.height()?my:h2 - my - 1,
                               mz<img.depth()?mz:d2 - mz - 1,mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img((int)cimg::mod(x,(double)img._width),(int)cimg::mod(y,(double)img._height),
                               (int)cimg::mod(z,(double)img._depth),cimg::mod(c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._atXYZC((int)x,(int)y,(int)z,c);
          default : // Dirichlet
            return (double)img.atXYZC((int)x,(int)y,(int)z,c,(T)0);
          }
        }
      }

      static double mp_ijxyzc2(_cimg_math_parser& mp) {
        const bool is_relative = (bool)mp.opcode[2];
        const unsigned int
          _ind = (unsigned int)mp.opcode[3],
          ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width()),
          interpolation = (unsigned int)_mp_arg(8),
          boundary_conditions = (unsigned int)_mp_arg(9);
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        const double
          x = is_relative?(mp.mem[_cimg_mp_slot_x] + _mp_arg(4)):_mp_arg(4),
          y = is_relative?(mp.mem[_cimg_mp_slot_y] + _mp_arg(5)):_mp_arg(5);
        const int
          z = (int)(is_relative?(mp.mem[_cimg_mp_slot_z] + _mp_arg(6)):_mp_arg(6)),
          c = (int)(is_relative?(mp.mem[_cimg_mp_slot_c] + _mp_arg(7)):_mp_arg(7));
        switch (interpolation) {
        case 2 : // Cubic interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              mz = cimg::mod(z,d2),
              mc = cimg::mod(c,s2);
            const float
              mx = cimg::mod((float)x,(float)w2),
              my = cimg::mod((float)y,(float)h2);
            return (double)img._cubic_atXY(mx<img.width()?mx:w2 - mx - 1,my<img.height()?my:h2 - my - 1,
                                           mz<img.depth()?mz:d2 - mz - 1,mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img._cubic_atXY_p((float)x,(float)y,cimg::mod(z,img.depth()),cimg::mod(c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._cubic_atXY((float)x,(float)y,
                                           z<0?0:z>=img.depth()?img.depth() - 1:z,
                                           c<0?0:c>=img.spectrum()?img.spectrum() - 1:c);
          default : // Dirichlet
            if (z<0 || z>=img.depth() || c<0 || c>=img.spectrum()) return (T)0;
            return (double)img.cubic_atXY((float)x,(float)y,z,c,(T)0);
          }
        case 1 : // Linear interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              mz = cimg::mod(z,d2),
              mc = cimg::mod(c,s2);
            const float
              mx = cimg::mod((float)x,(float)w2),
              my = cimg::mod((float)y,(float)h2);
            return (double)img._linear_atXY(mx<img.width()?mx:w2 - mx - 1,my<img.height()?my:h2 - my - 1,
                                            mz<img.depth()?mz:d2 - mz - 1,mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img._linear_atXY_p((float)x,(float)y,cimg::mod(z,img.depth()),cimg::mod(c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._linear_atXY((float)x,(float)y,
                                            z<0?0:z>=img.depth()?img.depth() - 1:z,
                                            c<0?0:c>=img.spectrum()?img.spectrum() - 1:c);
          default : // Dirichlet
            if (z<0 || z>=img.depth() || c<0 || c>=img.spectrum()) return (T)0;
            return (double)img.linear_atXY((float)x,(float)y,z,c,(T)0);
          }
        default : // Nearest neighbor interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              mz = cimg::mod(z,d2),
              mc = cimg::mod(c,s2),
              mx = cimg::mod((int)x,w2),
              my = cimg::mod((int)y,h2);
            return (double)img(mx<img.width()?mx:w2 - mx - 1,my<img.height()?my:h2 - my - 1,
                               mz<img.depth()?mz:d2 - mz - 1,mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img((int)cimg::mod(x,(double)img._width),(int)cimg::mod(y,(double)img._height),
                               cimg::mod(z,img.depth()),cimg::mod(c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._atXYZC((int)x,(int)y,z,c);
          default : // Dirichlet
            return (double)img.atXYZC((int)x,(int)y,z,c,(T)0);
          }
        }
      }

      static double mp_ijxyzc1(_cimg_math_parser& mp) {
        const bool is_relative = (bool)mp.opcode[2];
        const unsigned int
          _ind = (unsigned int)mp.opcode[3],
          ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width()),
          interpolation = (unsigned int)_mp_arg(8),
          boundary_conditions = (unsigned int)_mp_arg(9);
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        const double
          x = is_relative?(mp.mem[_cimg_mp_slot_x] + _mp_arg(4)):_mp_arg(4);
        const int
          y = (int)(is_relative?(mp.mem[_cimg_mp_slot_y] + _mp_arg(5)):_mp_arg(5)),
          z = (int)(is_relative?(mp.mem[_cimg_mp_slot_z] + _mp_arg(6)):_mp_arg(6)),
          c = (int)(is_relative?(mp.mem[_cimg_mp_slot_c] + _mp_arg(7)):_mp_arg(7));
        switch (interpolation) {
        case 2 : // Cubic interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              my = cimg::mod(y,h2),
              mz = cimg::mod(z,d2),
              mc = cimg::mod(c,s2);
            const float mx = cimg::mod((float)x,(float)w2);
            return (double)img._cubic_atX(mx<img.width()?mx:w2 - mx - 1,my<img.height()?my:h2 - my - 1,
                                          mz<img.depth()?mz:d2 - mz - 1,mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img._cubic_atX_p((float)x,cimg::mod(y,img.height()),
                                            cimg::mod(z,img.depth()),cimg::mod(c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._cubic_atX((float)x,
                                          y<0?0:y>=img.height()?img.height() - 1:y,
                                          z<0?0:z>=img.depth()?img.depth() - 1:z,
                                          c<0?0:c>=img.spectrum()?img.spectrum() - 1:c);
          default : // Dirichlet
            if (y<0 || y>=img.height() || z<0 || z>=img.depth() || c<0 || c>=img.spectrum()) return (T)0;
            return (double)img.cubic_atX((float)x,y,z,c,(T)0);
          }
        case 1 : // Linear interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              my = cimg::mod(y,h2),
              mz = cimg::mod(z,d2),
              mc = cimg::mod(c,s2);
            const float mx = cimg::mod((float)x,(float)w2);
            return (double)img._linear_atX(mx<img.width()?mx:w2 - mx - 1,my<img.height()?my:h2 - my - 1,
                                           mz<img.depth()?mz:d2 - mz - 1,mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img._linear_atX_p((float)x,cimg::mod(y,img.height()),
                                             cimg::mod(z,img.depth()),cimg::mod(c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._linear_atX((float)x,
                                           y<0?0:y>=img.height()?img.height() - 1:y,
                                           z<0?0:z>=img.depth()?img.depth() - 1:z,
                                           c<0?0:c>=img.spectrum()?img.spectrum() - 1:c);
          default : // Dirichlet
            if (y<0 || y>=img.height() || z<0 || z>=img.depth() || c<0 || c>=img.spectrum()) return (T)0;
            return (double)img.linear_atX((float)x,y,z,c,(T)0);
          }
        default : // Nearest neighbor interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              my = cimg::mod(y,h2),
              mz = cimg::mod(z,d2),
              mc = cimg::mod(c,s2),
              mx = cimg::mod((int)x,w2);
            return (double)img(mx<img.width()?mx:w2 - mx - 1,my<img.height()?my:h2 - my - 1,
                               mz<img.depth()?mz:d2 - mz - 1,mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img((int)cimg::mod(x,(double)img._width),cimg::mod(y,img.height()),
                               cimg::mod(z,img.depth()),cimg::mod(c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._atXYZC((int)x,y,z,c);
          default : // Dirichlet
            return (double)img.atXYZC((int)x,y,z,c,(T)0);
          }
        }
      }

      static double mp_image_crop(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const int x = (int)_mp_arg(3), y = (int)_mp_arg(4), z = (int)_mp_arg(5), c = (int)_mp_arg(6);
        const unsigned int
          dx = (unsigned int)mp.opcode[7],
          dy = (unsigned int)mp.opcode[8],
          dz = (unsigned int)mp.opcode[9],
          dc = (unsigned int)mp.opcode[10];
        const unsigned int boundary_conditions = (unsigned int)_mp_arg(11);
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        if (!img) std::memset(ptrd,0,dx*dy*dz*dc*sizeof(double));
        else CImg<doubleT>(ptrd,dx,dy,dz,dc,true) = img.get_crop(x,y,z,c,
                                                                 x + dx - 1,y + dy - 1,
                                                                 z + dz - 1,c + dc - 1,
                                                                 boundary_conditions);
        return cimg::type<double>::nan();
      }

      static double mp_image_depth(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        return (double)img.depth();
      }

      static double mp_image_display(_cimg_math_parser& mp) {
        mp_check_list(mp,"display");
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        cimg::mutex(6);
        CImg<T> &img = mp.imglist[ind];
        CImg<charT> title(256);
        std::fputc('\n',cimg::output());
        cimg_snprintf(title,title._width,"[ Image #%u ]",ind);
        img.display(title);
        cimg::mutex(6,0);
        return cimg::type<double>::nan();
      }

      static double mp_image_draw(_cimg_math_parser& mp) {
        const int x = (int)_mp_arg(4), y = (int)_mp_arg(5), z = (int)_mp_arg(6), c = (int)_mp_arg(7);
        unsigned int ind = (unsigned int)mp.opcode[3];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(3),mp.imglist.width());
        }
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        unsigned int
          dx = (unsigned int)mp.opcode[8],
          dy = (unsigned int)mp.opcode[9],
          dz = (unsigned int)mp.opcode[10],
          dc = (unsigned int)mp.opcode[11];
        dx = dx==~0U?img._width:(unsigned int)_mp_arg(8);
        dy = dy==~0U?img._height:(unsigned int)_mp_arg(9);
        dz = dz==~0U?img._depth:(unsigned int)_mp_arg(10);
        dc = dc==~0U?img._spectrum:(unsigned int)_mp_arg(11);

        const ulongT sizS = mp.opcode[2];
        if (sizS<(ulongT)dx*dy*dz*dc)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'draw()': "
                                      "Sprite vector (%lu values) and its specified geometry (%u,%u,%u,%u) "
                                      "(%lu values) do not match.",
                                      mp.imgin.pixel_type(),sizS,dx,dy,dz,dc,(ulongT)dx*dy*dz*dc);
        const CImg<doubleT> S(&_mp_arg(1) + 1,dx,dy,dz,dc,true);
        const float opacity = (float)_mp_arg(12);

        if (img._data) {
          if (mp.opcode[13]!=~0U) { // Opacity mask specified
            const ulongT sizM = mp.opcode[14];
            if (sizM<(ulongT)dx*dy*dz)
              throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'draw()': "
                                          "Mask vector (%lu values) and specified sprite geometry (%u,%u,%u,%u) "
                                          "(%lu values) do not match.",
                                          mp.imgin.pixel_type(),sizS,dx,dy,dz,dc,(ulongT)dx*dy*dz*dc);
            const CImg<doubleT> M(&_mp_arg(13) + 1,dx,dy,dz,(unsigned int)(sizM/(dx*dy*dz)),true);
            img.draw_image(x,y,z,c,S,M,opacity,(float)_mp_arg(15));
          } else img.draw_image(x,y,z,c,S,opacity);
        }
        return cimg::type<double>::nan();
      }

      static double mp_image_find(_cimg_math_parser& mp) {
        const unsigned int
          indi = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        const CImg<T> &img = mp.imglist[indi];
        const int _step = (int)_mp_arg(5), step = _step?_step:-1;
        const ulongT siz = (ulongT)img.size();
        longT ind = (longT)(mp.opcode[4]!=_cimg_mp_slot_nan?_mp_arg(4):step>0?0:siz - 1);
        if (ind<0 || ind>=(longT)siz) return -1.;
        const T
          *const ptrb = img.data(),
          *const ptre = img.end(),
          *ptr = ptrb + ind;
        const double val = _mp_arg(3);

        // Forward search.
        if (step>0) {
          if (cimg::type<double>::is_nan(val))
            while (ptr<ptre && !cimg::type<double>::is_nan(*ptr)) ptr+=step;
          else
            while (ptr<ptre && (double)*ptr!=val) ptr+=step;
          return ptr>=ptre?-1.:(double)(ptr - ptrb);
        }

        // Backward search.
        if (cimg::type<double>::is_nan(val))
          while (ptr>=ptrb && !cimg::type<double>::is_nan(*ptr)) ptr+=step;
        else
          while (ptr>=ptrb && (double)*ptr!=val) ptr+=step;
        return ptr<ptrb?-1.:(double)(ptr - ptrb);
      }

      static double mp_image_find_seq(_cimg_math_parser& mp) {
        const unsigned int
          indi = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        const CImg<T> &img = mp.imglist[indi];
        const int _step = (int)_mp_arg(6), step = _step?_step:-1;
        const ulongT
          siz1 = (ulongT)img.size(),
          siz2 = (ulongT)mp.opcode[4];
        longT ind = (longT)(mp.opcode[5]!=_cimg_mp_slot_nan?_mp_arg(5):step>0?0:siz1 - 1);
        if (ind<0 || ind>=(longT)siz1) return -1.;
        const T
          *const ptr1b = img.data(),
          *const ptr1e = ptr1b + siz1,
          *ptr1 = ptr1b + ind,
          *p1 = 0;
        const double
          *const ptr2b = &_mp_arg(3) + 1,
          *const ptr2e = ptr2b + siz2,
          *p2 = 0;

        // Forward search.
        if (step>0) {
          do {
            if (cimg::type<double>::is_nan(*ptr2b))
              while (ptr1<ptr1e && !cimg::type<double>::is_nan(*ptr1)) ptr1+=step;
            else
              while (ptr1<ptr1e && *ptr1!=*ptr2b) ptr1+=step;
            if (ptr1>=ptr1e) return -1.;
            p1 = ptr1 + 1;
            p2 = ptr2b + 1;
            while (p1<ptr1e && p2<ptr2e &&
                   ((cimg::type<double>::is_nan(*p1) && cimg::type<double>::is_nan(*p2)) || *p1==*p2)) { ++p1; ++p2; }
          } while (p2<ptr2e && (ptr1+=step)<ptr1e);
          return p2<ptr2e?-1.:(double)(ptr1 - ptr1b);
        }

        // Backward search.
        do {
          if (cimg::type<double>::is_nan(*ptr2b))
            while (ptr1>=ptr1b && !cimg::type<double>::is_nan(*ptr1)) ptr1+=step;
          else
            while (ptr1>=ptr1b && *ptr1!=*ptr2b) ptr1+=step;
          if (ptr1<ptr1b) return -1.;
          p1 = ptr1 + 1;
          p2 = ptr2b + 1;
          while (p1<ptr1e && p2<ptr2e &&
                 ((cimg::type<double>::is_nan(*p1) && cimg::type<double>::is_nan(*p2)) || *p1==*p2)) { ++p1; ++p2; }
        } while (p2<ptr2e && (ptr1+=step)>=ptr1b);
        return p2<ptr2e?-1.:(double)(ptr1 - ptr1b);
      }

      static double mp_image_height(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        return (double)img.height();
      }

      static double mp_image_is_shared(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        return (double)mp.imglist[ind]._is_shared;
      }

      static double mp_image_median(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        return (double)img.median();
      }

      static double mp_image_median_static(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        if (!mp.list_median) mp.list_median.assign(mp.imglist._width);
        if (!mp.list_median[ind]) CImg<doubleT>::vector(mp.imglist[ind].median()).move_to(mp.list_median[ind]);
        return *mp.list_median[ind];
      }

      static double mp_image_norm(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        return (double)img.magnitude(2);
      }

      static double mp_image_norm_static(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        if (!mp.list_norm) mp.list_norm.assign(mp.imglist._width);
        if (!mp.list_norm[ind]) CImg<doubleT>::vector(mp.imglist[ind].magnitude(2)).move_to(mp.list_norm[ind]);
        return *mp.list_norm[ind];
      }

      static double mp_image_print(_cimg_math_parser& mp) {
        mp_check_list(mp,"print");
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        cimg::mutex(6);
        CImg<T> &img = mp.imglist[ind];
        CImg<charT> title(256);
        std::fputc('\n',cimg::output());
        cimg_snprintf(title,title._width,"[ Image #%u ]",ind);
        img.print(title);
        cimg::mutex(6,0);
        return cimg::type<double>::nan();
      }

      static double mp_image_resize(_cimg_math_parser& mp) {
        mp_check_list(mp,"resize");
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        cimg::mutex(6);
        CImg<T> &img = mp.imglist[ind];
        const double
          _w = mp.opcode[3]==~0U?-100:_mp_arg(3),
          _h = mp.opcode[4]==~0U?-100:_mp_arg(4),
          _d = mp.opcode[5]==~0U?-100:_mp_arg(5),
          _s = mp.opcode[6]==~0U?-100:_mp_arg(6);
        const unsigned int
          w = (unsigned int)(_w>=0?_w:-_w*img.width()/100),
          h = (unsigned int)(_h>=0?_h:-_h*img.height()/100),
          d = (unsigned int)(_d>=0?_d:-_d*img.depth()/100),
          s = (unsigned int)(_s>=0?_s:-_s*img.spectrum()/100),
          interp = (int)_mp_arg(7);
        if (mp.is_fill && img._data==mp.imgout._data) {
          cimg::mutex(6,0);
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'resize()': "
                                      "Cannot both fill and resize image (%u,%u,%u,%u) "
                                      "to new dimensions (%u,%u,%u,%u).",
                                      img.pixel_type(),img._width,img._height,img._depth,img._spectrum,w,h,d,s);
        }
        const unsigned int
          boundary = (int)_mp_arg(8);
        const float
          cx = (float)_mp_arg(9),
          cy = (float)_mp_arg(10),
          cz = (float)_mp_arg(11),
          cc = (float)_mp_arg(12);
        img.resize(w,h,d,s,interp,boundary,cx,cy,cz,cc);
        cimg::mutex(6,0);
        return cimg::type<double>::nan();
      }

      static double mp_image_shift(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(2) + 1;
        const unsigned int
          w = (unsigned int)mp.opcode[3],
          h = (unsigned int)mp.opcode[4],
          d = (unsigned int)mp.opcode[5],
          s = (unsigned int)mp.opcode[6];
        const double dx = _mp_arg(7), dy = _mp_arg(8), dz = _mp_arg(9), dc = _mp_arg(10);
        const unsigned int boundary_conditions = (unsigned int)cimg::cut(_mp_arg(11),0.,3.);
        CImg<doubleT>(ptrd,w,h,d,s,true) =
          CImg<doubleT>(ptrs,w,h,d,s,true).get_shift(dx,dy,dz,dc,boundary_conditions);
        return cimg::type<double>::nan();
#endif
