#ifndef CIMG_MODULE_MATH_PARSER_FUNCTIONS_2_H
#define CIMG_MODULE_MATH_PARSER_FUNCTIONS_2_H

      }

      static double mp_image_sort(_cimg_math_parser& mp) {
        mp_check_list(mp,"sort");
        const bool is_increasing = (bool)_mp_arg(3);
        const unsigned int
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width()),
          axis = (unsigned int)_mp_arg(4);
        cimg::mutex(6);
        CImg<T> &img = mp.imglist[ind];
        img.sort(is_increasing,
                 axis==0 || axis=='x'?'x':
                 axis==1 || axis=='y'?'y':
                 axis==2 || axis=='z'?'z':
                 axis==3 || axis=='c'?'c':0);
        cimg::mutex(6,0);
        return cimg::type<double>::nan();
      }

      static double mp_image_spectrum(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        return (double)img.spectrum();
      }

      static double mp_image_stats(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind==~0U)
          CImg<doubleT>(ptrd,14,1,1,1,true) = mp.imgout.get_stats();
        else {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
          CImg<doubleT>(ptrd,14,1,1,1,true) = mp.imglist[ind].get_stats();
        }
        return cimg::type<double>::nan();
      }

      static double mp_image_stats_static(_cimg_math_parser& mp) {
        const unsigned int
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width()),
          k = (unsigned int)mp.opcode[3];
        bool get_stats = false;
        cimg::mutex(13);
        if (!mp.list_stats || mp.list_stats.size()!=mp.imglist._width) mp.list_stats.assign(mp.imglist._width);
        if (!mp.list_stats[ind]) get_stats = true;
        cimg::mutex(13,0);
        if (get_stats) {
          CImg<Tdouble> st = mp.imglist[ind].get_stats();
          cimg::mutex(13);
          st.move_to(mp.list_stats[ind]);
          cimg::mutex(13,0);
        }
        return mp.list_stats(ind,k);
      }

      static double mp_image_std_static(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        bool get_stats = false;
        cimg::mutex(13);
        if (!mp.list_stats || mp.list_stats.size()!=mp.imglist._width) mp.list_stats.assign(mp.imglist._width);
        if (!mp.list_stats[ind]) get_stats = true;
        cimg::mutex(13,0);
        if (get_stats) {
          CImg<Tdouble> st = mp.imglist[ind].get_stats();
          cimg::mutex(13);
          st.move_to(mp.list_stats[ind]);
          cimg::mutex(13,0);
        }
        return std::sqrt(mp.list_stats(ind,3));
      }

      static double mp_image_swap(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (!mp.imglist.width()) return cimg::type<double>::nan();
        ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        CImg<T> &img = mp.imglist[ind];
        const longT
          pos0 = (longT)_mp_arg(3),
          pos1 = (longT)_mp_arg(4);
        const bool is_vector = (bool)_mp_arg(5);
        if (is_vector) {
          const longT whd = (longT)img.size()/img.spectrum();
          T *ptr0 = &img[pos0], *ptr1 = &img[pos1];
          if (pos0>=0 && pos0<=whd && pos1>=0 && pos1<=whd)
            for (unsigned int c = 0; c<img._spectrum; ++c) {
              cimg::swap(*ptr0,*ptr1);
              ptr0+=whd;
              ptr1+=whd;
            } else throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'swap()': "
                                               "Out-of-bounds offsets %ld and %ld (min offset: 0, max offset: %ld).",
                                               mp.imgin.pixel_type(),pos0,pos1,whd);
        } else {
          const longT whds = (longT)img.size();
          if (pos0>=0 && pos0<=whds && pos1>=0 && pos1<=whds)
            cimg::swap(img[pos0],img[pos1]);
          else throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'swap()': "
                                           "Out-of-bounds offsets %ld and %ld (min offset: 0, max offset: %ld).",
                                           mp.imgin.pixel_type(),pos0,pos1,whds);
        }
        return cimg::type<double>::nan();
      }

      static double mp_image_width(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        return (double)img.width();
      }

      static double mp_image_wh(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        return (double)img.width()*img.height();
      }

      static double mp_image_whd(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        return (double)img.width()*img.height()*img.depth();
      }

      static double mp_image_whds(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        return (double)img.width()*img.height()*img.depth()*img.spectrum();
      }

      static double mp_increment(_cimg_math_parser& mp) {
        return _mp_arg(2) + 1;
      }

      static double mp_index(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int
          sizA = (unsigned int)mp.opcode[3],
          sizP = (unsigned int)mp.opcode[5],
          dim_colors = (unsigned int)mp.opcode[6],
          nb_colors = sizP/dim_colors,
          wA = sizA/dim_colors;
        const double dithering = _mp_arg(7);
        const bool map_colors = (bool)mp.opcode[8];
        const double
          *const ptrs = &_mp_arg(2) + 1,
          *const ptrp = &_mp_arg(4) + 1;
        CImg<doubleT> colormap(ptrp,nb_colors,1,1,dim_colors,true);
        CImg<doubleT>(ptrd,wA,1,1,map_colors?dim_colors:1,true) = CImg<doubleT>(ptrs,wA,1,1,dim_colors,true).
          get_index(colormap,dithering,map_colors);
        return cimg::type<double>::nan();
      }

      static double mp_indexof(_cimg_math_parser& mp) {
        const unsigned int
          siz = (unsigned int)mp.opcode[2],
          siz_ref = (unsigned int)mp.opcode[4];
        if (!siz_ref) { // Reference value is a scalar
          const double ref = _mp_arg(3);
          for (unsigned int k = 5; k<siz; k+=2) {
            const unsigned int siz_elt = (unsigned int)mp.opcode[k + 1];
            if (!siz_elt) {
              const double elt = _mp_arg(k);
              if (elt==ref) return (double)((k - 5)/2);
            }
          }
        } else { // Reference value is a vector
          const double *const ptr_r = &_mp_arg(3) + 1;
          for (unsigned int k = 5; k<siz; k+=2) {
            const unsigned int siz_elt = (unsigned int)mp.opcode[k + 1];
            if (siz_elt==siz_ref) {
              const double *const ptr_e = &_mp_arg(k) + 1;
              bool is_same = true;
              for (unsigned int l = 0; l<siz_ref; ++l)
                if (ptr_e[l]!=ptr_r[l]) { is_same = false; break; }
              if (is_same) return (double)((k - 5)/2);
            }
          }
        }
        return -1;
      }

      static double mp_inrange(_cimg_math_parser& mp) {
        const unsigned int sizd = (unsigned int)mp.opcode[2];
        const bool
          include_m = (bool)_mp_arg(9),
          include_M = (bool)_mp_arg(10);
        if (!sizd) { // Scalar result
          const double val = _mp_arg(3);
          const double m = _mp_arg(5), M = _mp_arg(7);
          if (M>=m) return (double)((include_m?(val>=m):(val>m)) && (include_M?(val<=M):(val<M)));
          else return (double)((include_M?(val>=M):(val>M)) && (include_m?(val<=m):(val<m)));
        }

        // Vector result.
        const unsigned int
          siz1 = (unsigned int)mp.opcode[4],
          siz2 = (unsigned int)mp.opcode[6],
          siz3 = (unsigned int)mp.opcode[8],
          off1 = siz1?1:0,
          off2 = siz2?1:0,
          off3 = siz3?1:0;
        double *ptrd = &_mp_arg(1) + 1;
        const double
          *ptr1 = &_mp_arg(3) + off1,
          *ptr2 = &_mp_arg(5) + off2,
          *ptr3 = &_mp_arg(7) + off3;
        for (unsigned int k = 0; k<sizd; ++k) {
          const double val = *ptr1;
          const double m = *ptr2, M = *ptr3;
          if (M>=m)
            ptrd[k] = (double)((include_m?(val>=m):(val>m)) && (include_M?(val<=M):(val<M)));
          else
            ptrd[k] = (double)((include_M?(val>=M):(val>M)) && (include_m?(val<=m):(val<m)));
          ptr1+=off1;
          ptr2+=off2;
          ptr3+=off3;
        }
        return cimg::type<double>::nan();
      }

      static double mp_int(_cimg_math_parser& mp) {
        return (double)(longT)_mp_arg(2);
      }

      static double mp_isbool(_cimg_math_parser& mp) {
        const double val = _mp_arg(2);
        return (double)(val==0. || val==1.);
      }

      static double mp_isdir(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        const double *const ptrs = &_mp_arg(2) + (siz?1:0);
        if (!siz) { char str[2] = {}; *str = *ptrs; return (double)cimg::is_directory(str); }
        CImg<charT> ss(siz + 1);
        cimg_forX(ss,i) ss[i] = (char)ptrs[i];
        ss.back() = 0;
        return (double)cimg::is_directory(ss);
      }

      static double mp_isfile(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        const double *const ptrs = &_mp_arg(2) + (siz?1:0);
        if (!siz) { char str[2] = {}; *str = *ptrs; return (double)cimg::is_file(str); }
        CImg<charT> ss(siz + 1);
        cimg_forX(ss,i) ss[i] = (char)ptrs[i];
        ss.back() = 0;
        return (double)cimg::is_file(ss);
      }

      static double mp_isfinite(_cimg_math_parser& mp) {
        return (double)cimg::type<double>::is_finite(_mp_arg(2));
      }

      static double mp_isin(_cimg_math_parser& mp) {
        const unsigned int
          i_end = (unsigned int)mp.opcode[2],
          siz_ref = (unsigned int)mp.opcode[4];
        bool res = false;
        if (siz_ref) { // Reference value is a vector
          const CImg<doubleT> ref(&_mp_arg(3) + 1,siz_ref,1,1,1,true);
          for (unsigned int i = 5; i<i_end; i+=2) {
            const unsigned int siz = (unsigned int)mp.opcode[i + 1];
            if (siz==siz_ref && CImg<doubleT>(&_mp_arg(i) + 1,siz,1,1,1,true)==ref) { res = true; break; }
          }
        } else { // Reference value is a scalar
          const double ref = _mp_arg(3);
          for (unsigned i = 5; i<i_end; i+=2) {
            const unsigned int siz = (unsigned int)mp.opcode[i + 1];
            if (!siz && _mp_arg(i)==ref) { res = true; break; }
          }
        }
        return res?1.:0.;
      }

      static double mp_isinf(_cimg_math_parser& mp) {
        return (double)cimg::type<double>::is_inf(_mp_arg(2));
      }

      static double mp_isint(_cimg_math_parser& mp) {
        double val = _mp_arg(2), intpart;
        const bool is_int = std::modf(val,&intpart)==0;
        if (mp.opcode[3]==~0U) return is_int;
        if (mp.opcode[4]==~0U) return is_int && val>=_mp_arg(3);
        return is_int && val>=_mp_arg(3) && val<=_mp_arg(4);
      }

      static double mp_isnan(_cimg_math_parser& mp) {
        return (double)cimg::type<double>::is_nan(_mp_arg(2));
      }

      static double mp_isvarname(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        const double *ptrs = &_mp_arg(2) + (siz?1:0);
        if (!siz) {
          const char c = (char)*ptrs;
          return (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_';
        }
        if (*ptrs>='0' && *ptrs<='9') return 0;
        for (unsigned int k = 0; k<siz; ++k) if (!cimg::is_varchar((char)ptrs[k])) return 0;
        return 1;
      }

      static double mp_kth(_cimg_math_parser& mp) {
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
        longT ind = (longT)values[0];
        ++values._data; --values._width; // Skip first value
        if (ind<0) ind+=values.width() + 1;
        ind = cimg::cut(ind,(longT)1,(longT)values.width());
        const double &kth = values.kth_smallest((ulongT)(ind - 1));
        --values._data; ++values._width;
        return kth;
      }

      static double mp_lcm(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        CImg<cimg_int64> values;
        if (i_end==5) { // Only a single argument
          if ((unsigned)mp.opcode[4]==1) return _mp_arg(3); // Real value
          else values.assign(&_mp_arg(3),(unsigned int)mp.opcode[4]); // Vector value
        } else if (i_end==7 && (unsigned int)mp.opcode[4]==1 && (unsigned int)mp.opcode[6]==1) // Two real arguments
          return (double)cimg::lcm((cimg_int64)_mp_arg(3),(cimg_int64)_mp_arg(5));
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
        return values.lcm();
      }

      static double mp_lcm2(_cimg_math_parser& mp) {
        return cimg::lcm((cimg_int64)_mp_arg(2),(cimg_int64)_mp_arg(3));
      }

      static double mp_lerp(_cimg_math_parser& mp) {
        const double
          val0 = _mp_arg(2),
          val1 = _mp_arg(3),
          t = _mp_arg(4);
        return val0*(1 - t) + val1*t;
      }

      static double mp_log(_cimg_math_parser& mp) {
        return std::log(_mp_arg(2));
      }

      static double mp_log10(_cimg_math_parser& mp) {
        return std::log10(_mp_arg(2));
      }

      static double mp_log2(_cimg_math_parser& mp) {
        return cimg::log2(_mp_arg(2));
      }

      static double mp_logical_and(_cimg_math_parser& mp) {
        const bool val_left = (bool)_mp_arg(2);
        const CImg<ulongT> *const p_end = ++mp.p_code + mp.opcode[4];
        if (!val_left) { mp.p_code = p_end - 1; return 0; }
        const ulongT mem_right = mp.opcode[3];
        mp.eval(mp.p_code,p_end);
        --mp.p_code;
        return (double)(bool)mp.mem[mem_right];
      }

      static double mp_logical_not(_cimg_math_parser& mp) {
        return (double)!_mp_arg(2);
      }

      static double mp_logical_or(_cimg_math_parser& mp) {
        const bool val_left = (bool)_mp_arg(2);
        const CImg<ulongT> *const p_end = ++mp.p_code + mp.opcode[4];
        if (val_left) { mp.p_code = p_end - 1; return 1; }
        const ulongT mem_right = mp.opcode[3];
        mp.eval(mp.p_code,p_end);
        --mp.p_code;
        return (double)(bool)mp.mem[mem_right];
      }

      static double mp_logit(_cimg_math_parser& mp) {
        return cimg::logit(_mp_arg(2));
      }

      static double mp_lowercase(_cimg_math_parser& mp) {
        return cimg::lowercase(_mp_arg(2));
      }

      static double mp_lt(_cimg_math_parser& mp) {
        return (double)(_mp_arg(2)<_mp_arg(3));
      }

      static double mp_lte(_cimg_math_parser& mp) {
        return (double)(_mp_arg(2)<=_mp_arg(3));
      }

      static double mp_map(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1);
        const double *ptrX = &_mp_arg(2);
        unsigned int sizX = (unsigned int)mp.opcode[3];
        const double *const ptrP = &_mp_arg(4) + 1;
        const unsigned int
          sizP = (unsigned int)mp.opcode[5],
          nb_channelsX = (unsigned int)mp.opcode[6],
          nb_channelsP = (unsigned int)mp.opcode[7],
          boundary_conditions = (unsigned int)_mp_arg(8);
        const bool
          is_scalar_input = !sizX,
          is_scalar_output = is_scalar_input && nb_channelsP==1;
        if (!is_scalar_output) ++ptrd;
        if (is_scalar_input) ++sizX; else ++ptrX;
        CImg<doubleT>(ptrd,sizX/nb_channelsX,1,1,nb_channelsX*nb_channelsP,true) =
          CImg<doubleT>(ptrX,sizX/nb_channelsX,1,1,nb_channelsX,true).
          get_map(CImg<doubleT>(ptrP,sizP/nb_channelsP,1,1,nb_channelsP,true),boundary_conditions);
        return is_scalar_output?ptrd[0]:cimg::type<double>::nan();
      }

      static double mp_matrix_eigen(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptr1 = &_mp_arg(2) + 1;
        const unsigned int k = (unsigned int)mp.opcode[3];
        CImg<doubleT> val, vec;
        CImg<doubleT>(ptr1,k,k,1,1,true).symmetric_eigen(val,vec);
        CImg<doubleT>(ptrd,1,k,1,1,true) = val;
        CImg<doubleT>(ptrd + k,k,k,1,1,true) = vec.get_transpose();
        return cimg::type<double>::nan();
      }

      static double mp_matrix_invert(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptr1 = &_mp_arg(2) + 1;
        const unsigned int
          w = (unsigned int)mp.opcode[3],
          h = (unsigned int)mp.opcode[4];
        CImg<doubleT>(ptrd,h,w,1,1,true) = CImg<doubleT>(ptr1,w,h,1,1,true).get_invert();
        return cimg::type<double>::nan();
      }

      static double mp_matrix_mul(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double
          *ptr1 = &_mp_arg(2) + 1,
          *ptr2 = &_mp_arg(3) + 1;
        const unsigned int
          k = (unsigned int)mp.opcode[4],
          l = (unsigned int)mp.opcode[5],
          m = (unsigned int)mp.opcode[6];
        CImg<doubleT>(ptrd,m,k,1,1,true) = CImg<doubleT>(ptr1,l,k,1,1,true)*CImg<doubleT>(ptr2,m,l,1,1,true);
        return cimg::type<double>::nan();
      }

      static double mp_matrix_qr(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptrs = &_mp_arg(2) + 1;
        const unsigned int
          n = (unsigned int)mp.opcode[3],
          m = (unsigned int)mp.opcode[4],
          mn = std::min(m,n);
        const bool is_reduced_form = (bool)mp.opcode[5];
        CImg<doubleT> Q, R;
        CImg<doubleT>(ptrs,n,m,1,1,true).QR(Q,R,is_reduced_form);
        if (is_reduced_form) {
          CImg<doubleT>(ptrd,mn,m,1,1,true) = Q;
          CImg<doubleT>(ptrd + mn*m,n,mn,1,1,true) = R;
        } else {
          CImg<doubleT>(ptrd,m,m,1,1,true) = Q;
          CImg<doubleT>(ptrd + m*m,n,m,1,1,true) = R;
        }
        return cimg::type<double>::nan();
      }

      static double mp_matrix_svd(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptrs = &_mp_arg(2) + 1;
        const unsigned int
          k = (unsigned int)mp.opcode[3],
          l = (unsigned int)mp.opcode[4];
        CImg<doubleT> U, S, V;
        CImg<doubleT>(ptrs,k,l,1,1,true).SVD(U,S,V);
        CImg<doubleT>(ptrd,k,l,1,1,true) = U;
        CImg<doubleT>(ptrd + k*l,1,k,1,1,true) = S;
        CImg<doubleT>(ptrd + k*l + k,k,k,1,1,true) = V;
        return cimg::type<double>::nan();
      }

      static double mp_max(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val, valmax = -cimg::type<double>::inf();
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) { val = *(ptr++); if (val>valmax) valmax = val; }
          } else { val = _mp_arg(i); if (val>valmax) valmax = val; }
        }
        return valmax;
      }

      static double mp_max2(_cimg_math_parser& mp) {
        return std::max(_mp_arg(2),_mp_arg(3));
      }

      static double _mp_maxabs(_cimg_math_parser& mp, const bool compute_abs) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val, abs_val, valmaxabs = 0, abs_valmaxabs = 0;
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) {
              val = *(ptr++);
              abs_val = cimg::abs(val);
              if (abs_val>abs_valmaxabs) { valmaxabs = val; abs_valmaxabs = abs_val; }
            }
          } else {
            val = _mp_arg(i);
            abs_val = cimg::abs(val);
            if (abs_val>abs_valmaxabs) { valmaxabs = val; abs_valmaxabs = abs_val; }
          }
        }
        return compute_abs?std::abs(valmaxabs):valmaxabs;
      }

      static double mp_maxabs(_cimg_math_parser& mp) {
        return _mp_maxabs(mp,false);
      }

      static double mp_maxabs2(_cimg_math_parser& mp) {
        return cimg::maxabs(_mp_arg(2),_mp_arg(3));
      }

      static double mp_med(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        CImg<doubleT> values;
        if (i_end==5) { // Only a single argument
          if ((unsigned)mp.opcode[4]==1) return _mp_arg(3); // Real value
          else values.assign(&_mp_arg(3),(unsigned int)mp.opcode[4],1,1,1,true); // Vector value
        } else {
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
        return values.median();
      }

      static double mp_med2(_cimg_math_parser& mp) {
        return (_mp_arg(2) + _mp_arg(3))/2;
      }

      static double* _mp_mem_copy_double(_cimg_math_parser& mp, const unsigned int ind, const ulongT *const p_ref,
                                         const longT siz, const long inc) {
        const longT
          off = *p_ref?p_ref[1] + (longT)mp.mem[(longT)p_ref[2]] + 1:ind,
          eoff = off + (siz - 1)*inc;
        if (off<0 || eoff>=mp.mem.width())
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'copy()': "
                                      "Out-of-bounds variable pointer "
                                      "(length: %ld, increment: %ld, offset start: %ld, "
                                      "offset end: %ld, offset max: %u).",
                                      mp.imgin.pixel_type(),siz,inc,off,eoff,mp.mem._width - 1);
        return &mp.mem[off];
      }

      static T* _mp_mem_copy_T(_cimg_math_parser& mp, const ulongT *const p_ref,
                               const longT siz, const long inc, const bool is_out) {
        const unsigned ind = (unsigned int)p_ref[1];
        const CImg<T> &img = is_out?
          (ind==~0U?mp.imgout:mp.imglist[cimg::mod((int)mp.mem[ind],mp.imglist.width())]):
          (ind==~0U?mp.imgin:mp.imglist[cimg::mod((int)mp.mem[ind],mp.imglist.width())]);
        const bool is_relative = (bool)p_ref[2];
        int ox, oy, oz, oc;
        longT off = 0;
        if (is_relative) {
          ox = (int)mp.mem[_cimg_mp_slot_x];
          oy = (int)mp.mem[_cimg_mp_slot_y];
          oz = (int)mp.mem[_cimg_mp_slot_z];
          oc = (int)mp.mem[_cimg_mp_slot_c];
          off = img.offset(ox,oy,oz,oc);
        }
        if ((*p_ref)%2) {
          const int
            x = (int)mp.mem[p_ref[3]],
            y = (int)mp.mem[p_ref[4]],
            z = (int)mp.mem[p_ref[5]],
            c = *p_ref==5?0:(int)mp.mem[p_ref[6]];
          off+=img.offset(x,y,z,c);
        } else off+=(longT)mp.mem[p_ref[3]];
        const longT eoff = off + (siz - 1)*inc;
        if (off<0 || eoff>=(longT)img.size())
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'copy()': "
                                      "Out-of-bounds image pointer "
                                      "(length: %ld, increment: %ld, offset start: %ld, "
                                      "offset end: %ld, offset max: %lu).",
                                      mp.imgin.pixel_type(),siz,inc,off,eoff,img.size() - 1);
        return (T*)&img[off];
      }

      static double mp_mem_copy(_cimg_math_parser& mp) {
        longT siz = (longT)_mp_arg(4);
        const longT inc_d = (longT)_mp_arg(5), inc_s = (longT)_mp_arg(6);
        const float
          _opacity = (float)_mp_arg(7),
          opacity = (float)cimg::abs(_opacity),
          omopacity = 1 - std::max(_opacity,0.f);
        if (siz>0) {
          const bool
            is_doubled = mp.opcode[8]<=1,
            is_doubles = mp.opcode[15]<=1;
          if (is_doubled && is_doubles) { // (double*) <- (double*)
            double *ptrd = _mp_mem_copy_double(mp,(unsigned int)mp.opcode[2],&mp.opcode[8],siz,inc_d);
            const double *ptrs = _mp_mem_copy_double(mp,(unsigned int)mp.opcode[3],&mp.opcode[15],siz,inc_s);
            if (inc_d==1 && inc_s==1 && _opacity>=1) {
              if (ptrs + siz - 1<ptrd || ptrs>ptrd + siz - 1) std::memcpy(ptrd,ptrs,siz*sizeof(double));
              else std::memmove(ptrd,ptrs,siz*sizeof(double));
            } else {
              if (ptrs + (siz - 1)*inc_s<ptrd || ptrs>ptrd + (siz - 1)*inc_d) {
                if (_opacity>=1) while (siz-->0) { *ptrd = *ptrs; ptrd+=inc_d; ptrs+=inc_s; }
                else while (siz-->0) { *ptrd = omopacity**ptrd + opacity**ptrs; ptrd+=inc_d; ptrs+=inc_s; }
              } else { // Overlapping buffers
                CImg<doubleT> buf((unsigned int)siz);
                cimg_for(buf,ptr,double) { *ptr = *ptrs; ptrs+=inc_s; }
                ptrs = buf;
                if (_opacity>=1) while (siz-->0) { *ptrd = *(ptrs++); ptrd+=inc_d; }
                else while (siz-->0) { *ptrd = omopacity**ptrd + opacity**(ptrs++); ptrd+=inc_d; }
              }
            }
          } else if (is_doubled && !is_doubles) { // (double*) <- (T*)
            double *ptrd = _mp_mem_copy_double(mp,(unsigned int)mp.opcode[2],&mp.opcode[8],siz,inc_d);
            const T *ptrs = _mp_mem_copy_T(mp,&mp.opcode[15],siz,inc_s,false);
            if (_opacity>=1) while (siz-->0) { *ptrd = *ptrs; ptrd+=inc_d; ptrs+=inc_s; }
            else while (siz-->0) { *ptrd = omopacity**ptrd + _opacity**ptrs; ptrd+=inc_d; ptrs+=inc_s; }
          } else if (!is_doubled && is_doubles) { // (T*) <- (double*)
            T *ptrd = _mp_mem_copy_T(mp,&mp.opcode[8],siz,inc_d,true);
            const double *ptrs = _mp_mem_copy_double(mp,(unsigned int)mp.opcode[3],&mp.opcode[15],siz,inc_s);
            if (_opacity>=1) while (siz-->0) { *ptrd = (T)*ptrs; ptrd+=inc_d; ptrs+=inc_s; }
            else while (siz-->0) { *ptrd = (T)(omopacity**ptrd + opacity**ptrs); ptrd+=inc_d; ptrs+=inc_s; }
          } else { // (T*) <- (T*)
            T *ptrd = _mp_mem_copy_T(mp,&mp.opcode[8],siz,inc_d,true);
            const T *ptrs = _mp_mem_copy_T(mp,&mp.opcode[15],siz,inc_s,false);
            if (inc_d==1 && inc_s==1 && _opacity>=1) {
              if (ptrs + siz - 1<ptrd || ptrs>ptrd + siz - 1) std::memcpy(ptrd,ptrs,siz*sizeof(T));
              else std::memmove(ptrd,ptrs,siz*sizeof(T));
            } else {
              if (ptrs + (siz - 1)*inc_s<ptrd || ptrs>ptrd + (siz - 1)*inc_d) {
                if (_opacity>=1) while (siz-->0) { *ptrd = *ptrs; ptrd+=inc_d; ptrs+=inc_s; }
                else while (siz-->0) { *ptrd = omopacity**ptrd + opacity**ptrs; ptrd+=inc_d; ptrs+=inc_s; }
              } else { // Overlapping buffers
                CImg<T> buf((unsigned int)siz);
                cimg_for(buf,ptr,T) { *ptr = *ptrs; ptrs+=inc_s; }
                ptrs = buf;
                if (_opacity>=1) while (siz-->0) { *ptrd = *(ptrs++); ptrd+=inc_d; }
                else while (siz-->0) { *ptrd = omopacity**ptrd + opacity**(ptrs++); ptrd+=inc_d; }
              }
            }
          }
        }
        return _mp_arg(1);
      }

      static double mp_mem_display(_cimg_math_parser& mp) {
        cimg::unused(mp);
        std::fputc('\n',cimg::output());
        CImg<charT> title(128);
        cimg_snprintf(title,title._width,"%s (%u)","[" cimg_appname "_math_parser] Memory snapshot",mp.mem._width);
        mp.mem.display(title);
        return cimg::type<double>::nan();
      }

      static double mp_min(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val, valmin = cimg::type<double>::inf();
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) { val = *(ptr++); if (val<valmin) valmin = val; }
          } else { val = _mp_arg(i); if (val<valmin) valmin = val; }
        }
        return valmin;
      }

      static double mp_min2(_cimg_math_parser& mp) {
        return std::min(_mp_arg(2),_mp_arg(3));
      }

      static double _mp_minabs(_cimg_math_parser& mp, const bool compute_abs) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val, abs_val, valminabs = cimg::type<double>::inf(), abs_valminabs = cimg::type<double>::inf();
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) {
              val = *(ptr++);
              abs_val = cimg::abs(val);
              if (abs_val<abs_valminabs) { valminabs = val; abs_valminabs = abs_val; }
            }
          } else {
            val = _mp_arg(i);
            abs_val = cimg::abs(val);
            if (abs_val<abs_valminabs) { valminabs = val; abs_valminabs = abs_val; }
          }
        }
        return compute_abs?std::abs(valminabs):valminabs;
      }

      static double mp_minabs(_cimg_math_parser& mp) {
        return _mp_minabs(mp,false);
      }

      static double mp_minabs2(_cimg_math_parser& mp) {
        return cimg::minabs(_mp_arg(2),_mp_arg(3));
      }

      static double mp_minus(_cimg_math_parser& mp) {
        return -_mp_arg(2);
      }

      static double mp_mirror(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int
          wA = (unsigned int)mp.opcode[3],
          hA = (unsigned int)mp.opcode[4],
          dA = (unsigned int)mp.opcode[5],
          sA = (unsigned int)mp.opcode[6],
          sizp = (unsigned int)mp.opcode[8];
        const double
          *const ptrs = &_mp_arg(2) + 1,
          *const ptrp = &_mp_arg(7) + 1;
        CImg<charT> str(std::max(1U,sizp) + 1);
        if (!sizp) str[0] = _mp_arg(7);
        else for (unsigned int p = 0; p<sizp; ++p) str[p] = (char)ptrp[p];
        str.back() = 0;
        CImg<doubleT>(ptrd,wA,hA,dA,sA,true) = CImg<doubleT>(ptrs,wA,hA,dA,sA,true).
          get_mirror(str);
        return cimg::type<double>::nan();
      }

      static double mp_modulo(_cimg_math_parser& mp) {
        return cimg::mod(_mp_arg(2),_mp_arg(3));
      }

      static double mp_mproj(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double
          *ptrS = &_mp_arg(2) + 1,
          *ptrD = &_mp_arg(5) + 1;
        const unsigned int
          wS = (unsigned int)mp.opcode[3],
          hS = (unsigned int)mp.opcode[4],
          wD = (unsigned int)mp.opcode[6];
        const int
          method = std::max(0,(int)_mp_arg(7)),
          max_iter = std::max(0,(int)_mp_arg(8));
        const double
          max_residual = std::max(0.,_mp_arg(9));

        CImg<doubleT>(ptrd,wS,wD,1,1,true) = CImg<doubleT>(ptrS,wS,hS,1,1,false).
          project_matrix(CImg<doubleT>(ptrD,wD,hS,1,1,true),method,max_iter,max_residual);
        return cimg::type<double>::nan();
      }

      static double mp_mse(_cimg_math_parser& mp) {
        const unsigned int
          _siz = (unsigned int)mp.opcode[4],
          siz = std::max(_siz,1U),
          off = _siz?1:0;
        return CImg<doubleT>(&_mp_arg(2) + off,1,siz,1,1,true).
          MSE(CImg<doubleT>(&_mp_arg(3) + off,1,siz,1,1,true));
      }

      static double mp_mul(_cimg_math_parser& mp) {
        return _mp_arg(2)*_mp_arg(3);
      }

      static double mp_mul_mul(_cimg_math_parser& mp) {
        return _mp_arg(2)*_mp_arg(3)*_mp_arg(4);
      }

      static double mp_mul_add(_cimg_math_parser& mp) {
        return _mp_arg(2)*_mp_arg(3) + _mp_arg(4);
      }

      static double mp_mul_sub(_cimg_math_parser& mp) {
        return _mp_arg(2)*_mp_arg(3) - _mp_arg(4);
      }

#ifdef cimg_mp_func_name
      static double mp_name(_cimg_math_parser& mp) {
        double *const ptr = &_mp_arg(1) + 1;
        const unsigned int siz = (unsigned int)mp.opcode[3];
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind==~0U) std::memset(ptr,0,siz*sizeof(double));
        else {
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
          cimg_mp_func_name(ind,ptr,siz);
        }
        return cimg::type<double>::nan();
      }
#else
      static double mp_name(_cimg_math_parser& mp) { cimg::unused(mp); return cimg::type<double>::nan(); }
#endif

      static double mp_neq(_cimg_math_parser& mp) {
        return (double)(_mp_arg(2)!=_mp_arg(3));
      }

      static double mp_noise(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int
          siz = (unsigned int)mp.opcode[3],
          noise_type = (unsigned int)_mp_arg(5);
        const double
          *const ptrs = &_mp_arg(2) + 1,
          amplitude = _mp_arg(4);
        CImg<doubleT>(ptrd,siz,1,1,1,true) = CImg<doubleT>(ptrs,siz,1,1,1,true).get_noise(amplitude,noise_type);
        return cimg::type<double>::nan();
      }

      static double mp_normalize(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int siz = (unsigned int)mp.opcode[3];
        const double
          *const ptrs = &_mp_arg(2) + 1,
          min_value = _mp_arg(4),
          max_value = _mp_arg(5),
          constant_case_ratio = _mp_arg(6);
        CImg<doubleT>(ptrd,siz,1,1,1,true) = CImg<doubleT>(ptrs,siz,1,1,1,true).
          get_normalize(min_value,max_value,constant_case_ratio);
        return cimg::type<double>::nan();
      }

      static double mp_normp(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        const double p = _mp_arg(4);
        if (siz>0) { // Vector-valued argument
          const double *ptrs = &_mp_arg(2) + 1;
          double res = 0;
          if (p==2) { // L2
            for (unsigned int i = 0; i<siz; ++i) res+=(double)cimg::sqr(*(ptrs++));
            res = (double)std::sqrt(res);
          } else if (p==1) // L1
            for (unsigned int i = 0; i<siz; ++i) res+=(double)cimg::abs(*(ptrs++));
          else if (!p) // L0
            for (unsigned int i = 0; i<siz; ++i) res+=(double)(*(ptrs++)?1:0);
          else if (cimg::type<double>::is_inf(p)) { // L-inf
            for (unsigned int i = 0; i<siz; ++i) {
              const double val = (double)cimg::abs(*(ptrs++));
              if (val>res) res = val;
            }
          } else { // L-p
            for (unsigned int i = 0; i<siz; ++i) res+=(double)std::pow(cimg::abs(*(ptrs++)),p);
            res = (double)std::pow(res,1.0/p);
          }
          return res>0?res:0;
        }
        // Scalar-valued argument.
        const double val = _mp_arg(2);
        return p?cimg::abs(val):(val!=0);
      }

      static double mp_o2c(_cimg_math_parser& mp) {
        unsigned int ind = (unsigned int)mp.opcode[2];
        if (ind!=~0U) {
          mp_check_list(mp,"o2c");
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.imglist.width());
        }
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        double *ptrd = &_mp_arg(1) + 1;
        longT offset = (longT)_mp_arg(3);
        const bool boundary_conditions = (bool)_mp_arg(4);
        if (boundary_conditions && (!img || offset<0 || (ulongT)offset>=img.size()))
          ptrd[0] = ptrd[1] = ptrd[2] = ptrd[3] = -1;
        else {
          if (!img) ptrd[0] = ptrd[1] = ptrd[2] = ptrd[3] = cimg::type<double>::nan();
          else {
            *(ptrd++) = (double)(offset%img.width());
            offset/=img.width();
            *(ptrd++) = (double)(offset%img.height());
            offset/=img.height();
            *(ptrd++) = (double)(offset%img.depth());
            offset/=img.depth();
            *ptrd = (double)(offset%img.spectrum());
          }
        }
        return cimg::type<double>::nan();
      }

      static double mp_permutations(_cimg_math_parser& mp) {
        return cimg::permutations((int)_mp_arg(2),(int)_mp_arg(3),(bool)_mp_arg(4));
      }

      static double mp_permute(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int
          wA = (unsigned int)mp.opcode[3],
          hA = (unsigned int)mp.opcode[4],
          dA = (unsigned int)mp.opcode[5],
          sA = (unsigned int)mp.opcode[6],
          sizp = (unsigned int)mp.opcode[8];
        const double
          *const ptrs = &_mp_arg(2) + 1,
          *const ptrp = &_mp_arg(7) + 1;
        CImg<charT> str(sizp + 1);
        for (unsigned int p = 0; p<sizp; ++p) str[p] = (char)ptrp[p];
        str.back() = 0;
        CImg<doubleT>(ptrd,wA,hA,dA,sA,true) = CImg<doubleT>(ptrs,wA,hA,dA,sA,true).
          get_permute_axes(str);
        return cimg::type<double>::nan();
      }

      static double mp_polygon(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        unsigned int ind = (unsigned int)mp.opcode[3];
        if (ind!=~0U) {
          if (!mp.imglist.width()) return cimg::type<double>::nan();
          ind = (unsigned int)cimg::mod((int)_mp_arg(3),mp.imglist.width());
        }
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        bool is_invalid_arguments = i_end<=4, is_outlined = false, is_closed = true;
        if (!is_invalid_arguments) {
          int nbv = (int)_mp_arg(4);
          if (!nbv) is_invalid_arguments = true;
          else {
            if (nbv<0) { nbv = -nbv; is_outlined = true; }
            CImg<intT> points(nbv,2,1,1,0);
            CImg<T> color(img._spectrum,1,1,1,0);
            float opacity = 1;
            unsigned int i = 5, pattern=~0U;
            cimg_foroff(points,k) if (i<i_end) points(k/2,k%2) = (int)cimg::round(_mp_arg(i++));
            else { is_invalid_arguments = true; break; }
            if (!is_invalid_arguments) {
              if (i<i_end) opacity = (float)_mp_arg(i++);
              if (is_outlined && i<i_end) {
                double d_pattern = _mp_arg(i++);
                if (d_pattern<0) { d_pattern = -d_pattern; is_closed = false; }
                pattern = (unsigned int)d_pattern;
              }
              cimg_forX(color,k) if (i<i_end) color[k] = (T)_mp_arg(i++);
              else { color.resize(k,1,1,1,-1); break; }
              color.resize(img._spectrum,1,1,1,0,2);
              if (is_outlined) img.draw_polygon(points,color._data,opacity,pattern,is_closed);
              else img.draw_polygon(points,color._data,opacity);
            }
          }
        }
        if (is_invalid_arguments) {
          CImg<doubleT> args(i_end - 4);
          cimg_forX(args,k) args[k] = _mp_arg(4 + k);
          if (ind==~0U)
            throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'polygon()': "
                                        "Invalid arguments '%s'. ",
                                        mp.imgin.pixel_type(),args.value_string()._data);
          else
            throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'polygon()': "
                                        "Invalid arguments '#%u%s%s'. ",
                                        mp.imgin.pixel_type(),ind,args._width?",":"",args.value_string()._data);
        }
        return cimg::type<double>::nan();
      }

      static double mp_pow(_cimg_math_parser& mp) {
        const double v = _mp_arg(2), p = _mp_arg(3);
        return std::pow(v,p);
      }

      static double mp_pow0_25(_cimg_math_parser& mp) {
        const double val = _mp_arg(2);
        return std::sqrt(std::sqrt(val));
      }

      static double mp_pow3(_cimg_math_parser& mp) {
        const double val = _mp_arg(2);
        return val*val*val;
      }

      static double mp_pow4(_cimg_math_parser& mp) {
        const double val = _mp_arg(2);
        return val*val*val*val;
      }

      static double mp_print(_cimg_math_parser& mp) {
        const double val = _mp_arg(1);
        const bool print_char = (bool)mp.opcode[3];
        cimg_pragma_openmp(critical(mp_print)) {
          CImg<charT> _expr(mp.opcode[2] - 4);
          const ulongT *ptrs = mp.opcode._data + 4;
          cimg_for(_expr,ptrd,char) *ptrd = (char)*(ptrs++);
          cimg::strellipsize(_expr);
          cimg::mutex(6);
          if (print_char)
            std::fprintf(cimg::output(),"\n[" cimg_appname "_math_parser] %s = %.17g = '%c'",
                         _expr._data,val,(int)val);
          else
            std::fprintf(cimg::output(),"\n[" cimg_appname "_math_parser] %s = %.17g",
                         _expr._data,val);
          std::fflush(cimg::output());
          cimg::mutex(6,0);
        }
        return val;
      }

      static double mp_prod(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double prod = 1;
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) prod*=*(ptr++);
          } else prod*=_mp_arg(i);
        }
        return prod;
      }

      static double mp_prod2(_cimg_math_parser& mp) {
        return _mp_arg(2)*_mp_arg(3);
      }

      static double mp_rad2deg(_cimg_math_parser& mp) {
        return _mp_arg(2)*180/cimg::PI;
      }

      static double mp_repeat(_cimg_math_parser& mp) {
        const double nb_it = _mp_arg(2), nb_itm1 = nb_it - 1;
        double
          *const ptrc = mp.opcode[3]!=~0U?&_mp_arg(3):0,
          *const ptrs = &_mp_arg(1);
        const CImg<ulongT>
          *const p_body = ++mp.p_code,
          *const p_end = p_body + mp.opcode[4];

        if (nb_it>=1) {
          const unsigned int _break_type = mp.break_type;
          mp.break_type = 0;

          double it = 0;
          if (ptrc) { // Version with loop variable (3 arguments)
            while (it<=nb_itm1) {
              *ptrc = it;
              mp.eval(p_body,p_end);
              if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
              ++it;
            }
            *ptrc = it;
          } else // Version without loop variable (2 arguments)
            while (it<=nb_itm1) {
              mp.eval(p_body,p_end);
              if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
              ++it;
            }
          mp.break_type = _break_type;
        }

        mp.p_code = p_end - 1;
        return *ptrs;
      }

      static double mp_reverse(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(2) + 1;
        const unsigned int siz = (unsigned int)mp.opcode[3];
        CImg<doubleT>(ptrd,siz,1,1,1,true) = CImg<doubleT>(ptrs,siz,1,1,1,true).get_mirror('x');
        return cimg::type<double>::nan();
      }

      static double mp_rol(_cimg_math_parser& mp) {
        return cimg::rol(_mp_arg(2),(unsigned int)_mp_arg(3));
      }

      static double mp_ror(_cimg_math_parser& mp) {
        return cimg::ror(_mp_arg(2),(unsigned int)_mp_arg(3));
      }

      static double mp_rot2d(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double theta = _mp_arg(2), ca = std::cos(theta), sa = std::sin(theta);
        ptrd[0] = ptrd[3] = ca; ptrd[1] = -sa; ptrd[2] = sa;
        return cimg::type<double>::nan();
      }

      static double mp_rot3d(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const float
          x = (float)_mp_arg(2),
          y = (float)_mp_arg(3),
          z = (float)_mp_arg(4),
          theta = (float)_mp_arg(5);
        CImg<doubleT>(ptrd,3,3,1,1,true) = CImg<doubleT>::rotation_matrix(x,y,z,theta*180/cimg::PI);
        return cimg::type<double>::nan();
      }

      static double mp_round(_cimg_math_parser& mp) {
        return cimg::round(_mp_arg(2),_mp_arg(3),(int)_mp_arg(4));
      }

#ifdef cimg_mp_func_run
      static double mp_run(_cimg_math_parser& mp) {
        const unsigned int nb_args = (unsigned int)(mp.opcode[2] - 3)/2;
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
        CImg<charT> str = _str>'x';
#if cimg_use_openmp==0
        const unsigned int n_thread = 0;
#else
        const unsigned int n_thread = omp_get_thread_num();
#endif
        cimg_mp_func_run(str._data,n_thread && mp.is_noncritical_run);
        return cimg::type<double>::nan();
      }
#else
      static double mp_run(_cimg_math_parser& mp) { cimg::unused(mp); return cimg::type<double>::nan(); }
#endif

      static double mp_s2v(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2);
        const ulongT siz = (ulongT)mp.opcode[3];
        longT ind = (longT)_mp_arg(4);
        const bool is_strict = (bool)_mp_arg(5);
        double val = cimg::type<double>::nan();
        if (ind<0 || ind>=(longT)siz) return val;
        if (!siz) return *ptrs>='0' && *ptrs<='9'?*ptrs - '0':val;

        CImg<charT> ss(siz + 1 - ind);
        ptrs+=1 + ind;
        cimg_forX(ss,i) ss[i] = (char)ptrs[i];
        ss.back() = 0;

        const char *s = ss._data;
        while (*s && *s<=32) ++s;
        const bool is_negative = *s=='-';
        if (is_negative || *s=='+') ++s;
        int err = 0;
        char sep;
        if (*s=='0' && (s[1]=='x' || s[1]=='X') &&
            ((s[2]>='0' && s[2]<='9') || (s[2]>='a' && s[2]<='f') || (s[2]>='a' && s[2]<='f'))) { // Hexadecimal number
          val = (double)std::strtoll(s + 2,0,16);
          err = 1;
        } else if (*s=='0' && (s[1]=='b' || s[1]=='B') && (s[2]=='0' || s[2]=='1')) { // Binary number
          val = (double)std::strtoll(s + 2,0,2);
          err = 1;
        } else if (*s>32) { // Decimal number
          err = cimg_sscanf(s,"%lf%c",&val,&sep);
#if cimg_OS==2
          // Check for +/-NaN and +/-inf as Microsoft's sscanf() version is not able
          // to read those particular values.
          if (!err && (*s=='i' || *s=='I' || *s=='n' || *s=='N')) {
            if (!cimg::strncasecmp(s,"inf",3)) { val = cimg::type<double>::inf(); err = 1 + (s[3]!=0); }
            else if (!cimg::strncasecmp(s,"nan",3)) { val = cimg::type<double>::nan(); err = 1 + (s[3]!=0); }
          }
#endif
        }
        if (err<=0 || (is_strict && err!=1)) return cimg::type<double>::nan();
        if (is_negative) val = -val;
        return val;
      }

      static double mp_self_add(_cimg_math_parser& mp) {
        return _mp_arg(1)+=_mp_arg(2);
      }

      static double mp_self_bitwise_and(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = (double)((longT)val & (longT)_mp_arg(2));
      }

      static double mp_self_bitwise_left_shift(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = (double)((longT)val<<(unsigned int)_mp_arg(2));
      }

      static double mp_self_bitwise_or(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = (double)((longT)val | (longT)_mp_arg(2));
      }

      static double mp_self_bitwise_right_shift(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = (double)((longT)val>>(unsigned int)_mp_arg(2));
      }

      static double mp_self_decrement(_cimg_math_parser& mp) {
        return --_mp_arg(1);
      }

      static double mp_self_div(_cimg_math_parser& mp) {
        return _mp_arg(1)/=_mp_arg(2);
      }

      static double mp_self_increment(_cimg_math_parser& mp) {
        return ++_mp_arg(1);
      }

      static double mp_self_map_vector_s(_cimg_math_parser& mp) { // Vector += scalar
        unsigned int
          ptrd = (unsigned int)mp.opcode[1] + 1,
          siz = (unsigned int)mp.opcode[2];
        mp_func op = (mp_func)mp.opcode[3];
        CImg<ulongT> l_opcode(1,3);
        l_opcode[2] = mp.opcode[4]; // Scalar argument
        l_opcode.swap(mp.opcode);
        ulongT &target = mp.opcode[1];
        while (siz-->0) { target = ptrd++; (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();
      }

      static double mp_self_map_vector_v(_cimg_math_parser& mp) { // Vector += vector
        unsigned int
          ptrd = (unsigned int)mp.opcode[1] + 1,
          siz = (unsigned int)mp.opcode[2],
          ptrs = (unsigned int)mp.opcode[4] + 1;
        mp_func op = (mp_func)mp.opcode[3];
        CImg<ulongT> l_opcode(1,4);
        l_opcode.swap(mp.opcode);
        ulongT &target = mp.opcode[1], &argument = mp.opcode[2];
        while (siz-->0) { target = ptrd++; argument = ptrs++; (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();
      }

      static double mp_self_mul(_cimg_math_parser& mp) {
        return _mp_arg(1)*=_mp_arg(2);
      }

      static double mp_self_modulo(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = cimg::mod(val,_mp_arg(2));
      }

      static double mp_self_pow(_cimg_math_parser& mp) {
        double &val = _mp_arg(1);
        return val = std::pow(val,_mp_arg(2));
      }

      static double mp_self_sub(_cimg_math_parser& mp) {
        return _mp_arg(1)-=_mp_arg(2);
      }

#ifdef cimg_mp_func_set
      static double mp_set(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(1);
        double *ptrd = &_mp_arg(3) + 1;
        const unsigned int
          sizs = (unsigned int)mp.opcode[2],
          sizd = (unsigned int)mp.opcode[4];
        CImg<charT> sd(sizd + 1);
        cimg_for_inX(sd,0,sd.width() - 2,i) sd[i] = (char)ptrd[i];
        sd.back() = 0;
        if (sizs) cimg_mp_func_set(ptrs + 1,sizs,sd._data);
        else cimg_mp_func_set(ptrs,0,sd._data);
        return *ptrs;
      }
#else
      static double mp_set(_cimg_math_parser& mp) { cimg::unused(mp); return cimg::type<double>::nan(); }
#endif

      static double mp_set_ijoff(_cimg_math_parser& mp) {
        const unsigned int _ind = (unsigned int)mp.opcode[3];
        if (_ind!=~0U && !mp.imglist.width()) return cimg::type<double>::nan();
        const unsigned int ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width());
        const bool is_relative = (bool)mp.opcode[2];
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        const longT
          off = (longT)(is_relative?img.offset((int)mp.mem[_cimg_mp_slot_x],
                                               (int)mp.mem[_cimg_mp_slot_y],
                                               (int)mp.mem[_cimg_mp_slot_z],
                                               (int)mp.mem[_cimg_mp_slot_c]) + _mp_arg(4):_mp_arg(4)),
          whds = (longT)img.size();
        const double val = _mp_arg(1);
        if (off>=0 && off<whds) img[off] = (T)val;
        return val;
      }

      static double mp_set_ijxyzc(_cimg_math_parser& mp) {
        const unsigned int _ind = (unsigned int)mp.opcode[3];
        if (_ind!=~0U && !mp.imglist.width()) return cimg::type<double>::nan();
        const unsigned int ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width());
        const bool is_relative = (bool)mp.opcode[2];
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        const int
          x = (int)(is_relative?(mp.mem[_cimg_mp_slot_x] + _mp_arg(4)):_mp_arg(4)),
          y = (int)(is_relative?(mp.mem[_cimg_mp_slot_y] + _mp_arg(5)):_mp_arg(5)),
          z = (int)(is_relative?(mp.mem[_cimg_mp_slot_z] + _mp_arg(6)):_mp_arg(6)),
          c = (int)(is_relative?(mp.mem[_cimg_mp_slot_c] + _mp_arg(7)):_mp_arg(7));
        const double val = _mp_arg(1);
        if (img.containsXYZC(x,y,z,c)) img(x,y,z,c) = (T)val;
        return val;
      }

      static double mp_set_IJoff_s(_cimg_math_parser& mp) {
        const unsigned int _ind = (unsigned int)mp.opcode[3];
        if (_ind!=~0U && !mp.imglist.width()) return cimg::type<double>::nan();
        const unsigned int ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width());
        const bool is_relative = (bool)mp.opcode[2];
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        const longT
          off = (longT)(is_relative?img.offset((int)mp.mem[_cimg_mp_slot_x],
                                               (int)mp.mem[_cimg_mp_slot_y],
                                               (int)mp.mem[_cimg_mp_slot_z],
                                               (int)mp.mem[_cimg_mp_slot_c]) + _mp_arg(4):_mp_arg(4)),
          whd = (longT)img.width()*img.height()*img.depth();
        const T val = (T)_mp_arg(1);
        if (off>=0 && off<whd) {
          T *ptrd = &img[off];
          cimg_forC(img,c) { *ptrd = val; ptrd+=whd; }
        }
        return _mp_arg(1);
      }

      static double mp_set_IJoff_v(_cimg_math_parser& mp) {
        const unsigned int _ind = (unsigned int)mp.opcode[3];
        if (_ind!=~0U && !mp.imglist.width()) return cimg::type<double>::nan();
        const unsigned int ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width());
        const bool is_relative = (bool)mp.opcode[2];
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        const longT
          off = (longT)(is_relative?img.offset((int)mp.mem[_cimg_mp_slot_x],
                                               (int)mp.mem[_cimg_mp_slot_y],
                                               (int)mp.mem[_cimg_mp_slot_z],
                                               (int)mp.mem[_cimg_mp_slot_c]) + _mp_arg(4):_mp_arg(4)),
          whd = (longT)img.width()*img.height()*img.depth();
        const double *ptrs = &_mp_arg(1) + 1;
        if (off>=0 && off<whd) {
          const unsigned int siz = (unsigned int)mp.opcode[5];
          T *ptrd = &img[off];
          cimg_for_inC(img,0,siz - 1,c) { *ptrd = (T)*(ptrs++); ptrd+=whd; }
        }
        return cimg::type<double>::nan();
      }

      static double mp_set_IJxyz_s(_cimg_math_parser& mp) {
        const unsigned int _ind = (unsigned int)mp.opcode[3];
        if (_ind!=~0U && !mp.imglist.width()) return cimg::type<double>::nan();
        const unsigned int ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width());
        const bool is_relative = (bool)mp.opcode[2];
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        const int
          x = (int)(is_relative?(mp.mem[_cimg_mp_slot_x] + _mp_arg(4)):_mp_arg(4)),
          y = (int)(is_relative?(mp.mem[_cimg_mp_slot_y] + _mp_arg(5)):_mp_arg(5)),
          z = (int)(is_relative?(mp.mem[_cimg_mp_slot_z] + _mp_arg(6)):_mp_arg(6));
        const T val = (T)_mp_arg(1);
        if (x>=0 && x<img.width() && y>=0 && y<img.height() && z>=0 && z<img.depth()) {
          T *ptrd = &img(x,y,z);
          const ulongT whd = (ulongT)img._width*img._height*img._depth;
          cimg_forC(img,c) { *ptrd = val; ptrd+=whd; }
        }
        return _mp_arg(1);
      }

      static double mp_set_IJxyz_v(_cimg_math_parser& mp) {
        const unsigned int _ind = (unsigned int)mp.opcode[3];
        if (_ind!=~0U && !mp.imglist.width()) return cimg::type<double>::nan();
        const unsigned int ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width());
        const bool is_relative = (bool)mp.opcode[2];
        CImg<T> &img = ind==~0U?mp.imgout:mp.imglist[ind];
        const int
          x = (int)(is_relative?(mp.mem[_cimg_mp_slot_x] + _mp_arg(4)):_mp_arg(4)),
          y = (int)(is_relative?(mp.mem[_cimg_mp_slot_y] + _mp_arg(5)):_mp_arg(5)),
          z = (int)(is_relative?(mp.mem[_cimg_mp_slot_z] + _mp_arg(6)):_mp_arg(6));
        const double *ptrs = &_mp_arg(1) + 1;
        if (x>=0 && x<img.width() && y>=0 && y<img.height() && z>=0 && z<img.depth()) {
          const unsigned int siz = (unsigned int)mp.opcode[7];
          T *ptrd = &img(x,y,z);
          const ulongT whd = (ulongT)img._width*img._height*img._depth;
          cimg_for_inC(img,0,siz - 1,c) { *ptrd = (T)*(ptrs++); ptrd+=whd; }
        }
        return cimg::type<double>::nan();
      }

      static double mp_sigmoid(_cimg_math_parser& mp) {
        return cimg::sigmoid(_mp_arg(2));
      }

      static double mp_sign(_cimg_math_parser& mp) {
        return cimg::sign(_mp_arg(2));
      }

      static double mp_sin(_cimg_math_parser& mp) {
        return std::sin(_mp_arg(2));
      }

      static double mp_sinc(_cimg_math_parser& mp) {
        return cimg::sinc(_mp_arg(2));
      }

      static double mp_sinh(_cimg_math_parser& mp) {
        return std::sinh(_mp_arg(2));
      }

      static double mp_size_list(_cimg_math_parser& mp) {
        return (double)mp.imglist.width();
      }

      static double mp_softargmax(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        if (siz<2) return 0;
        const double *const ptrs = &_mp_arg(2) + 1, temperature = _mp_arg(4);
        CImg<doubleT> sm = CImg<doubleT>(ptrs,siz,1,1,1,true).get_softmax(temperature);
        double res = 0;
        cimg_forX(sm,x) res+=sm[x]*x;
        return res;
      }

      static double mp_softargmin(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        if (siz<2) return 0;
        const double *const ptrs = &_mp_arg(2) + 1, temperature = _mp_arg(4);
        CImg<doubleT> sm = CImg<doubleT>(ptrs,siz,1,1,1,true).get_softmin(temperature);
        double res = 0;
        cimg_forX(sm,x) res+=sm[x]*x;
        return res;
      }

      static double mp_softmax(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        if (!siz) return 1;
        const double *const ptrs = &_mp_arg(2) + 1, temperature = _mp_arg(4);
        double *const ptrd = &_mp_arg(1) + 1;
        CImg<doubleT>(ptrd,siz,1,1,1,true) = CImg<doubleT>(ptrs,siz,1,1,1,true).get_softmax(temperature);
        return cimg::type<double>::nan();
      }

      static double mp_softmin(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        if (!siz) return 1;
        const double *const ptrs = &_mp_arg(2) + 1, temperature = _mp_arg(4);
        double *const ptrd = &_mp_arg(1) + 1;
        CImg<doubleT>(ptrd,siz,1,1,1,true) = CImg<doubleT>(ptrs,siz,1,1,1,true).get_softmin(temperature);
        return cimg::type<double>::nan();
      }

      static double mp_solve(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double
          *ptr1 = &_mp_arg(2) + 1,
          *ptr2 = &_mp_arg(3) + 1;
        const unsigned int
          k = (unsigned int)mp.opcode[4],
          l = (unsigned int)mp.opcode[5],
          m = (unsigned int)mp.opcode[6];
        CImg<doubleT>(ptrd,m,k,1,1,true) = CImg<doubleT>(ptr2,m,l,1,1,false).
          solve(CImg<doubleT>(ptr1,k,l,1,1,true));
        return cimg::type<double>::nan();
      }

      static double mp_sort(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(2) + 1;
        const bool is_increasing = (bool)_mp_arg(4);
        const unsigned int
          siz = (unsigned int)mp.opcode[3],
          nb_elts = mp.opcode[5]==~0U?siz:(unsigned int)_mp_arg(5),
          siz_elt = (unsigned int)_mp_arg(6),
          sort_index = std::min((unsigned int)_mp_arg(7),siz_elt - 1);
        const ulongT sn = siz_elt*nb_elts;
        if (sn>siz || siz_elt<1)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'sort()': "
                                      "Arguments 'nb_elts=%g' and 'siz_elt=%g' are invalid "
                                      "for sorting a vector of size %u.",
                                      mp.imgin.pixel_type(),_mp_arg(5),_mp_arg(6),siz);
        CImg<doubleT>(ptrd,siz_elt,nb_elts,1,1,true) = CImg<doubleT>(ptrs,siz_elt,nb_elts,1,1,true).
          shift(-(int)sort_index,0,0,0,2).get_sort(is_increasing,siz_elt>1?'y':0).shift((int)sort_index,0,0,0,2);
        if (sn<siz) CImg<doubleT>(ptrd + sn,siz - sn,1,1,1,true) = CImg<doubleT>(ptrs + sn,siz - sn,1,1,1,true);
        return cimg::type<double>::nan();
      }

      static double mp_sqr(_cimg_math_parser& mp) {
        return cimg::sqr(_mp_arg(2));
      }

      static double mp_sqrt(_cimg_math_parser& mp) {
        return std::sqrt(_mp_arg(2));
      }

      static double mp_srand(_cimg_math_parser& mp) {
        mp.rng = (cimg_uint64)_mp_arg(2);
        return cimg::type<double>::nan();
      }

      static double mp_srand0(_cimg_math_parser& mp) {
        cimg::srand(&mp.rng);

#if cimg_use_openmp!=0
        mp.rng+=omp_get_thread_num();
#endif
        return cimg::type<double>::nan();
      }

      static double mp_std(_cimg_math_parser& mp) {
        return std::sqrt(mp_var(mp));
      }

      static double mp_std2(_cimg_math_parser& mp) {
        return std::sqrt(mp_var2(mp));
      }

#ifdef cimg_mp_func_store
      static double mp_store(_cimg_math_parser& mp) {
        const double
          *ptr1 = &_mp_arg(2),
          *ptr2 = &_mp_arg(4) + 1;
        const unsigned int
          siz1 = (unsigned int)mp.opcode[3],
          siz2 = (unsigned int)mp.opcode[5];
        const int
          w = (int)_mp_arg(6),
          h = (int)_mp_arg(7),
          d = (int)_mp_arg(8),
          s = (int)_mp_arg(9);

        const bool is_compressed = (bool)_mp_arg(10);
        if (w<0 || h<0 || d<0 || s<0)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'store()': "
                                      "Specified image dimensions (%d,%d,%d,%d) are invalid.",
                                      pixel_type(),w,h,d,s);
        CImg<charT> ss(siz2 + 1);
        cimg_for_inX(ss,0,ss.width() - 2,i) ss[i] = (char)ptr2[i];
        ss.back() = 0;
        if (siz1) cimg_mp_func_store(ptr1 + 1,siz1,
                                     (unsigned int)w,(unsigned int)h,(unsigned int)d,(unsigned int)s,
                                     is_compressed,ss._data);
        else cimg_mp_func_store(ptr1,1,(unsigned int)w,(unsigned int)h,(unsigned int)d,(unsigned int)s,
                                is_compressed,ss._data);
        return cimg::type<double>::nan();
      }
#else
      static double mp_store(_cimg_math_parser& mp) { cimg::unused(mp); return cimg::type<double>::nan(); }
#endif

      static double mp_string(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int nb_args = (unsigned int)(mp.opcode[3] - 3)/2;
        CImgList<charT> _str;
        CImg<charT> it;
        for (unsigned int n = 0; n<nb_args; ++n) {
          const unsigned int siz = (unsigned int)mp.opcode[5 + 2*n];
          if (siz) { // Vector argument -> string
            const double *ptr = &_mp_arg(4 + 2*n) + 1;
            unsigned int l = 0;
            while (l<siz && ptr[l]) ++l;
            CImg<doubleT>(ptr,l,1,1,1,true).move_to(_str);
          } else { // Scalar argument -> number
            it.assign(24);
            cimg_snprintf(it,it._width,"%.17g",_mp_arg(4 + 2*n));
            CImg<charT>::string(it,false,true).move_to(_str);
          }
        }
        const CImg<charT> str = _str>'x';
        const unsigned int sizd = std::min(str._width,(unsigned int)mp.opcode[2]);
        std::memset(ptrd,0,mp.opcode[2]*sizeof(double));
        for (unsigned int k = 0; k<sizd; ++k) ptrd[k] = (double)str[k];
        return cimg::type<double>::nan();
      }

      static double mp_string_init(_cimg_math_parser& mp) {
        const unsigned char *ptrs = (unsigned char*)&mp.opcode[3];
        unsigned int
          ptrd = (unsigned int)mp.opcode[1] + 1,
          siz = (unsigned int)mp.opcode[2];
        while (siz-->0) mp.mem[ptrd++] = (double)*(ptrs++);
        return cimg::type<double>::nan();
      }

      static double mp_sub(_cimg_math_parser& mp) {
        return _mp_arg(2) - _mp_arg(3);
      }

      static double mp_sub_mul(_cimg_math_parser& mp) {
        return _mp_arg(2) - _mp_arg(3)*_mp_arg(4);
      }

      static double mp_sum(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double sum = 0;
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) sum+=*(ptr++);
          } else sum+=_mp_arg(i);
        }
        return sum;
      }

      static double mp_sum2(_cimg_math_parser& mp) {
        return _mp_arg(2) + _mp_arg(3);
      }

      static double mp_swap(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        if (!siz) { // Scalar
          double &arg1 = _mp_arg(1), &arg2 = _mp_arg(2);
          cimg::swap(arg1,arg2);
        } else { // Vector
          double *const ptr1 = &_mp_arg(1) + 1, *const ptr2 = &_mp_arg(2) + 1;
          for (unsigned int k = 0; k<siz; ++k) cimg::swap(ptr1[k],ptr2[k]);
        }
        return _mp_arg(1);
      }

      static double mp_tan(_cimg_math_parser& mp) {
        return std::tan(_mp_arg(2));
      }

      static double mp_tanh(_cimg_math_parser& mp) {
        return std::tanh(_mp_arg(2));
      }

      static double mp_trace(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2) + 1;
        const unsigned int k = (unsigned int)mp.opcode[3];
        return CImg<doubleT>(ptrs,k,k,1,1,true).trace();
      }

      static double mp_transpose(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptrs = &_mp_arg(2) + 1;
        const unsigned int
          k = (unsigned int)mp.opcode[3],
          l = (unsigned int)mp.opcode[4];
        CImg<doubleT>(ptrd,l,k,1,1,true) = CImg<doubleT>(ptrs,k,l,1,1,true).get_transpose();
        return cimg::type<double>::nan();
      }

      static double mp_rand_double(_cimg_math_parser& mp) {
        return cimg::rand(_mp_arg(2),_mp_arg(3),&mp.rng);
      }

      static double mp_rand_double_0_1(_cimg_math_parser& mp) {
        return (double)cimg::_rand(&mp.rng)/~0U;
      }

      static double mp_rand_double_0_N(_cimg_math_parser& mp) {
        return _mp_arg(2)*mp_rand_double_0_1(mp);
      }

      static double mp_rand_double_ext(_cimg_math_parser& mp) {
        const double eps = 1e-5;
        const bool
          include_min = (bool)_mp_arg(4),
          include_max = (bool)_mp_arg(5);
        double
          m = _mp_arg(2),
          M = _mp_arg(3);
        if (m>M) cimg::swap(m,M);
        if (!include_min) m = m>0?m*(1 + eps):m<0?m*(1 - eps):eps;
        if (!include_max) M = M>0?M*(1 - eps):M<0?M*(1 + eps):-eps;
        return cimg::rand(m,M,&mp.rng);
      }

      static double mp_rand_double_m1_1(_cimg_math_parser& mp) {
        return 2*mp_rand_double_0_1(mp) - 1;
      }

      static double mp_rand_double_gaussian(_cimg_math_parser& mp) {
        return cimg::grand(&mp.rng);
      }

      static double _mp_rand_int(_cimg_math_parser& mp, const double delta) {
        if (delta>=cimg::type<cimg_uint64>::max()) return cimg::round(cimg::rand(0,delta,&mp.rng));
        const cimg_uint64 _delta = (cimg_uint64)delta;
        if (!_delta) return 0;
        cimg_uint64 val = 0;
        do { val = (cimg_uint64)std::floor(cimg::rand(_delta + 1,&mp.rng)); } while (val>_delta);
        return val;
      }

      static double mp_rand_int(_cimg_math_parser& mp) {
        double
          _m = _mp_arg(2),
          _M = _mp_arg(3);
        if (_m>_M) cimg::swap(_m,_M);
        const double
          m = cimg::type<cimg_int64>::cut(std::ceil(_m)),
          M = cimg::type<cimg_int64>::cut(std::floor(_M));
        return (double)m + _mp_rand_int(mp,M - m);
      }

      static double mp_rand_int_0_1(_cimg_math_parser& mp) {
        return cimg::_rand(&mp.rng)<(~0U>>1);
      }

      static double mp_rand_int_0_N(_cimg_math_parser& mp) {
        const double _M = _mp_arg(2);
        const int sgn = _M>=0?1:-1;
        return sgn*_mp_rand_int(mp,_M*sgn);
      }

      static double mp_rand_int_ext(_cimg_math_parser& mp) {
        const bool
          include_min = (bool)_mp_arg(4),
          include_max = (bool)_mp_arg(5);
        double
          _m = _mp_arg(2),
          _M = _mp_arg(3);
        if (_m>_M) cimg::swap(_m,_M);
        const int
          m = cimg::type<cimg_uint64>::cut(std::ceil(_m)) + (include_min?0:1),
          M = cimg::type<cimg_uint64>::cut(std::floor(_M)) - (include_max?0:1);
        return (double)m + _mp_rand_int(mp,M - m);
      }

      static double mp_rand_int_m1_1(_cimg_math_parser& mp) {
        const unsigned int
          th = ~0U/3,
          val = cimg::_rand(&mp.rng);
        return val<th?-1:val<2*th?0:1;
      }

      static double mp_ui2f(_cimg_math_parser& mp) {
        return (double)cimg::uint2float(_mp_arg(2),(T)0);
      }

      static double mp_unitnorm(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[3];
        const double p = _mp_arg(4);
        if (siz>0) { // Vector-valued argument
          double *const ptrd = &_mp_arg(1) + 1;
          const double *const ptrs = &_mp_arg(2) + 1;
          if (ptrd!=ptrs) std::memcpy(ptrd,ptrs,siz*sizeof(double));
          CImg<doubleT> vec(ptrd,siz,1,1,1,true);
          const double mag = vec.magnitude(p);
          if (mag>0) vec/=mag;
          return cimg::type<double>::nan();
        }
        // Scalar-valued argument.
        const double val = _mp_arg(2);
        return val?(_mp_arg(2)?1:val):0;
      }

      static double mp_uppercase(_cimg_math_parser& mp) {
        return cimg::uppercase(_mp_arg(2));
      }

      static double mp_v2s(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const unsigned int
          sizd = (unsigned int)mp.opcode[2],
          sizs = (unsigned int)mp.opcode[4];
        std::memset(ptrd,0,sizd*sizeof(double));
        const int nb_digits = (int)_mp_arg(5);
        CImg<charT> format(8);
        switch (nb_digits) {
        case -1 : std::strcpy(format,"%g"); break;
        case 0 : std::strcpy(format,"%.17g"); break;
        default :
          if (nb_digits>=-1) cimg_snprintf(format,format._width,"%%.%dg",nb_digits);
          else cimg_snprintf(format,format._width,"%%.%dld",-nb_digits);
        }
        CImg<charT> str;
        if (sizs) { // Vector expression
          const double *ptrs = &_mp_arg(3) + 1;
          if (nb_digits>=-1) CImg<doubleT>(ptrs,sizs,1,1,1,true).value_string(',',sizd + 1,format).move_to(str);
          else CImg<longT>(ptrs,sizs,1,1,1).value_string(',',sizd + 1,format).move_to(str);
        } else { // Scalar expression
          str.assign(sizd + 1);
          if (nb_digits>=-1) cimg_snprintf(str,sizd + 1,format,_mp_arg(3));
          else cimg_snprintf(str,sizd + 1,format,(long)_mp_arg(3));
        }
        const unsigned int l = std::min(sizd,(unsigned int)std::strlen(str) + 1);
        CImg<doubleT>(ptrd,l,1,1,1,true) = str.get_shared_points(0,l - 1);
        return cimg::type<double>::nan();
      }

      static double mp_var(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double m = 0, S2 = 0;
        ulongT n = 0;
        for (unsigned int i = 3; i<i_end; i+=2) {
          const unsigned int len = (unsigned int)mp.opcode[i + 1];
          if (len>1) {
            const double *ptr = &_mp_arg(i);
            for (unsigned int k = 0; k<len; ++k) {
              const double val = *(ptr++), delta = val - m;
              m+=delta/++n;
              const double delta2 = val - m;
              S2+=delta*delta2;
            }
          } else {
            const double val = _mp_arg(i), delta = val - m;
            m+=delta/++n;
            const double delta2 = val - m;
            S2+=delta*delta2;
          }
        }
        return n<2?0.:std::max(S2/n,0.);
      }

      static double mp_var2(_cimg_math_parser& mp) {
        return cimg::sqr(_mp_arg(2) - _mp_arg(3))/2;
      }

#define _cimg_mp_vfunc(func) \
      const longT sizd = (longT)mp.opcode[2]; \
      const unsigned int nbargs = (unsigned int)(mp.opcode[3] - 4)/2; \
      double *const ptrd = &_mp_arg(1) + (sizd?1:0); \
      cimg_pragma_openmp(parallel cimg_openmp_if_size(sizd,256)) { \
        CImg<doubleT> vec(nbargs); double res; \
        cimg_pragma_openmp(for) for (longT k = sizd?sizd - 1:0; k>=0; --k) { \
          cimg_forX(vec,n) vec[n] = *(&_mp_arg(4 + 2*n) + (k+1)*(mp.opcode[4 + 2*n + 1]?1:0)); \
          func; ptrd[k] = res; \
        }} \
      return sizd?cimg::type<double>::nan():*ptrd;

      static double _mp_vargkth(CImg<doubleT>& vec) {
        const double val = (+vec).get_shared_points(1,vec.width() - 1).
          kth_smallest((ulongT)cimg::cut((longT)*vec - 1,(longT)0,(longT)vec.width() - 2));
        cimg_for_inX(vec,1,vec.width() - 1,ind) if (vec[ind]==val) return ind - 1.;
        return 1.;
      }

      static double mp_vargkth(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = _mp_vargkth(vec));
      }

      static double mp_vargmax(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = (double)(&vec.max() - vec.data()));
      }

      static double mp_vargmaxabs(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = (double)(&vec.maxabs() - vec.data()));
      }

      static double mp_vargmin(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = (double)(&vec.min() - vec.data()));
      }

      static double mp_vargminabs(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = (double)(&vec.minabs() - vec.data()));
      }

      static double mp_vavg(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.mean());
      }

      static double mp_vector_copy(_cimg_math_parser& mp) {
        std::memcpy(&_mp_arg(1) + 1,&_mp_arg(2) + 1,sizeof(double)*mp.opcode[3]);
        return cimg::type<double>::nan();
      }

      static double mp_vector_crop(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptrs = &_mp_arg(2) + 1;
        const longT
          length = (longT)mp.opcode[3],
          start = (longT)_mp_arg(4),
          sublength = (longT)mp.opcode[5],
          step = (longT)_mp_arg(6);
        if (start<0 || start + step*(sublength-1)>=length)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Value accessor '[]': "
                                      "Out-of-bounds sub-vector request "
                                      "(length: %ld, start: %ld, sub-length: %ld, step: %ld).",
                                      mp.imgin.pixel_type(),length,start,sublength,step);
        ptrs+=start;
        if (step==1) std::memcpy(ptrd,ptrs,sublength*sizeof(double));
        else for (longT k = 0; k<sublength; ++k) { *(ptrd++) = *ptrs; ptrs+=step; }
        return cimg::type<double>::nan();
      }

      static double mp_vector_crop_ext(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(2) + 1;
        const unsigned int
          w = (unsigned int)mp.opcode[3],
          h = (unsigned int)mp.opcode[4],
          d = (unsigned int)mp.opcode[5],
          s = (unsigned int)mp.opcode[6],
          dx = (unsigned int)mp.opcode[11],
          dy = (unsigned int)mp.opcode[12],
          dz = (unsigned int)mp.opcode[13],
          dc = (unsigned int)mp.opcode[14],
          boundary_conditions = (int)_mp_arg(15);
        const int x = (int)_mp_arg(7), y = (int)_mp_arg(8), z = (int)_mp_arg(9), c = (int)_mp_arg(10);
        CImg<doubleT>(ptrd,dx,dy,dz,dc,true) = CImg<doubleT>(ptrs,w,h,d,s,true).
          get_crop(x,y,z,c,x + dx - 1,y + dy - 1,z + dz - 1,c + dc - 1,boundary_conditions);
        return cimg::type<double>::nan();
      }

      static double mp_vector_display(_cimg_math_parser& mp) {
        const unsigned int
          _siz = (unsigned int)mp.opcode[3],
          siz = _siz?_siz:1;
        const double *const ptr = &_mp_arg(1) + (_siz?1:0);
        const int
          w = (int)_mp_arg(4),
          h = (int)_mp_arg(5),
          d = (int)_mp_arg(6),
          s = (int)_mp_arg(7);
        CImg<doubleT> img;
        if (w>0 && h>0 && d>0 && s>0) {
          if ((unsigned int)w*h*d*s<=siz) img.assign(ptr,w,h,d,s,true);
          else img.assign(ptr,siz).resize(w,h,d,s,-1);
        } else img.assign(ptr,1,siz,1,1,true);

        CImg<charT> expr(mp.opcode[2] - 8);
        const ulongT *ptrs = mp.opcode._data + 8;
        cimg_for(expr,ptrd,char) *ptrd = (char)*(ptrs++);
        ((CImg<charT>::string("[" cimg_appname "_math_parser] ",false,true),expr)>'x').move_to(expr);
        cimg::strellipsize(expr);
        std::fputc('\n',cimg::output());
        img.display(expr._data);
        return cimg::type<double>::nan();
      }

      static double mp_vector_draw(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(7) + 1;
        const unsigned int
          sizD = (unsigned int)mp.opcode[2],
          sizS = (unsigned int)mp.opcode[8];
        const int
          w = (int)_mp_arg(3), h = (int)_mp_arg(4), d = (int)_mp_arg(5), s = (int)_mp_arg(6),
          x = (int)_mp_arg(9), y = (int)_mp_arg(10), z = (int)_mp_arg(11), c = (int)_mp_arg(12);
        int dx = (int)mp.opcode[13], dy = (int)mp.opcode[14], dz = (int)mp.opcode[15], dc = (int)mp.opcode[16];
        dx = (unsigned int)dx==~0U?w:(int)_mp_arg(13);
        dy = (unsigned int)dy==~0U?h:(int)_mp_arg(14);
        dz = (unsigned int)dz==~0U?d:(int)_mp_arg(15);
        dc = (unsigned int)dc==~0U?s:(int)_mp_arg(16);

        if (w<=0 || h<=0 || d<=0 || s<=0)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'draw()': "
                                      "Invalid specified target vector geometry (%d,%d,%d,%d).",
                                      mp.imgin.pixel_type(),w,h,d,s);
        if (sizD<(ulongT)w*h*d*s)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'draw()': "
                                      "Target vector (%lu values) and its specified target geometry (%d,%d,%d,%d) "
                                      "(%lu values) do not match.",
                                      mp.imgin.pixel_type(),sizD,w,h,d,s,(ulongT)w*h*d*s);
        if (dx<=0 || dy<=0 || dz<=0 || dc<=0)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'draw()': "
                                      "Invalid specified sprite geometry (%d,%d,%d,%d).",
                                      mp.imgin.pixel_type(),dx,dy,dz,dc);
        if (sizS<(ulongT)dx*dy*dz*dc)
          throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'draw()': "
                                      "Sprite vector (%lu values) and its specified sprite geometry (%d,%d,%d,%d) "
                                      "(%lu values) do not match.",
                                      mp.imgin.pixel_type(),sizS,dx,dy,dz,dc,(ulongT)dx*dy*dz*dc);

        CImg<doubleT> D(ptrd,w,h,d,s,true);
        const CImg<doubleT> S(ptrs,dx,dy,dz,dc,true);
        const float opacity = (float)_mp_arg(17);

        if (mp.opcode[18]!=~0U) { // Opacity mask specified
          const ulongT sizM = mp.opcode[19];
          if (sizM<(ulongT)dx*dy*dz)
            throw CImgArgumentException("[" cimg_appname "_math_parser] CImg<%s>: Function 'draw()': "
                                        "Mask vector (%lu values) and specified sprite geometry (%u,%u,%u,%u) "
                                        "(%lu values) do not match.",
                                        mp.imgin.pixel_type(),sizS,dx,dy,dz,dc,(ulongT)dx*dy*dz*dc);
          const CImg<doubleT> M(&_mp_arg(18) + 1,dx,dy,dz,(unsigned int)(sizM/(dx*dy*dz)),true);
          D.draw_image(x,y,z,c,S,M,opacity,(float)_mp_arg(20));
        } else D.draw_image(x,y,z,c,S,opacity);

        return cimg::type<double>::nan();
      }

      static double mp_vector_eq(_cimg_math_parser& mp) {
        const double
          *ptr1 = &_mp_arg(2) + 1,
          *ptr2 = &_mp_arg(4) + 1;
        unsigned int
          siz1 = (unsigned int)mp.opcode[3],
          siz2 = (unsigned int)mp.opcode[5],
          n;
        int N = (int)_mp_arg(6);
        const bool case_sensitive = (bool)_mp_arg(7);
        if (!N) return 1;
        if (N<0) { // Compare all values
          if (siz1>0 && siz2>0 && siz1!=siz2) return 0;
          N = std::max(siz1,siz2);
        }

        // Compare first N values.
        bool still_equal = true;
        if (siz1>0 && siz2>0) { // Vector==vector
          n = cimg::min((unsigned int)N,siz1,siz2);
          if (case_sensitive)
            while (still_equal && n--) {
              still_equal = (cimg::type<double>::is_nan(*ptr1) && cimg::type<double>::is_nan(*ptr2)) ||
                *ptr1==*ptr2;
              ++ptr1; ++ptr2;
            }
          else
            while (still_equal && n--) {
              still_equal = (cimg::type<double>::is_nan(*ptr1) && cimg::type<double>::is_nan(*ptr2)) ||
                cimg::lowercase(*ptr1)==cimg::lowercase(*ptr2);
              ++ptr1; ++ptr2;
            }
          return still_equal;
        } else if (siz1>0 && !siz2) { // Vector==scalar
          n = std::min((unsigned int)N,siz1);
          const double value = case_sensitive?_mp_arg(4):cimg::lowercase(_mp_arg(4));
          if (cimg::type<double>::is_nan(value))
            while (still_equal && n--) still_equal = cimg::type<double>::is_nan(*(ptr1++));
          else
            while (still_equal && n--) still_equal = *(ptr1++)==value;
          return still_equal;
        } else if (!siz1 && siz2>0) { // Scalar==vector
          n = std::min((unsigned int)N,siz2);
          const double value = case_sensitive?_mp_arg(2):cimg::lowercase(_mp_arg(2));
          if (cimg::type<double>::is_nan(value))
            while (still_equal && siz2--) still_equal = cimg::type<double>::is_nan(*(ptr2++));
          else
            while (still_equal && n--) still_equal = *(ptr2++)==value;
          return still_equal;
        } // Scalar==scalar
        if (case_sensitive) return _mp_arg(2)==_mp_arg(4);
        else if (cimg::type<double>::is_nan(_mp_arg(2))) return cimg::type<double>::is_nan(_mp_arg(4));
        return cimg::lowercase(_mp_arg(2))==cimg::lowercase(_mp_arg(4));
      }

      static double mp_vector_fill(_cimg_math_parser& mp) {
        double
          *ptrd = &_mp_arg(1) + 1,
          *const ptrc = mp.opcode[3]!=~0U?&_mp_arg(3):0,
          *const ptrs = &_mp_arg(4);
        const unsigned int siz = (unsigned int)mp.opcode[2];
        const CImg<ulongT>
          *const p_body = ++mp.p_code,
          *const p_end = p_body + mp.opcode[5];
        const unsigned int _break_type = mp.break_type;
        mp.break_type = 0;

        unsigned int it = 0;
        if (ptrc) { // Version with loop variable (3 arguments)
          while (it<siz) {
            *ptrc = (double)it;
            mp.eval(p_body,p_end);
            if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
            else ptrd[it] = *ptrs;
            ++it;
          }
          *ptrc = (double)it;
        } else // Version without loop variable (2 arguments)
          while (it<siz) {
            mp.eval(p_body,p_end);
            if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
            else ptrd[it] = *ptrs;
            ++it;
          }
        mp.break_type = _break_type;
        mp.p_code = p_end - 1;
        return *ptrd;
      }

      static double mp_vector_fill_ext(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const unsigned int
          w = (unsigned int)mp.opcode[2],
          h = (unsigned int)mp.opcode[3],
          d = (unsigned int)mp.opcode[4],
          s = (unsigned int)mp.opcode[5],
          sizs = (unsigned int)mp.opcode[7];
        const double *const ptrs = &_mp_arg(6) + 1;
        CImg<charT> ss(sizs + 1);
        cimg_for_inX(ss,0,ss.width() - 2,i) ss[i] = (char)ptrs[i];
        ss.back() = 0;
        CImg<doubleT>(ptrd,w,h,d,s,true)._fill(ss,true,3,&mp.imglist,"mp_image_fill",0,0);
        return cimg::type<double>::nan();
      }

      static double mp_vector_hypot(_cimg_math_parser& mp) {
        switch ((unsigned int)mp.opcode[2]) {
        case 5 : return cimg::abs(_mp_arg(4));
        case 6 : return cimg::hypot(_mp_arg(4),_mp_arg(5));
        case 7 : return cimg::hypot(_mp_arg(4),_mp_arg(5),_mp_arg(6));
        };
        return mp_vector_norm2(mp);
      }

      static double mp_vector_init(_cimg_math_parser& mp) {
        unsigned int
          ptrs = 4U,
          ptrd = (unsigned int)mp.opcode[1] + 1,
          siz = (unsigned int)mp.opcode[3];
        switch (mp.opcode[2] - 4) {
        case 0 : std::memset(mp.mem._data + ptrd,0,siz*sizeof(double)); break; // 0 values given
        case 1 : { const double val = _mp_arg(ptrs); while (siz-->0) mp.mem[ptrd++] = val; } break;
        default : while (siz-->0) { mp.mem[ptrd++] = _mp_arg(ptrs++); if (ptrs>=mp.opcode[2]) ptrs = 4U; }
        }
        return cimg::type<double>::nan();
      }

      static double mp_vector_lerp(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        unsigned int siz = (unsigned int)mp.opcode[2];
        const unsigned int
          siz0 = (unsigned int)mp.opcode[4],
          siz1 = (unsigned int)mp.opcode[6],
          step0 = siz0?1:0,
          step1 = siz1?1:0;
        const double
          *const ptrs0 = &_mp_arg(3) + step0,
          *const ptrs1 = &_mp_arg(5) + step1;

        if (mp.opcode[8]) { // t is vector-valued
          const double *const ptrt = &_mp_arg(7) + 1;
          for (unsigned int k0 = 0, k1 = 0, k = 0; k<siz; ++k) {
            double t = ptrt[k];
            ptrd[k] = ptrs0[k0]*(1 - t) + ptrs1[k1]*t;
            k0+=step0;
            k1+=step1;
          }
        } else { // t is scalar-valued
          const double t = _mp_arg(7);
          for (unsigned int k0 = 0, k1 = 0, k = 0; k<siz; ++k) {
            ptrd[k] = ptrs0[k0]*(1 - t) + ptrs1[k1]*t;
            k0+=step0;
            k1+=step1;
          }
        }
        return cimg::type<double>::nan();
      }

      static double mp_vector_map_sv(_cimg_math_parser& mp) { // Operator(scalar,vector,[...])
        unsigned int
          nb_args = (unsigned int)mp.opcode[2],
          siz_vector = (unsigned int)mp.opcode[3],
          ptrs = (unsigned int)mp.opcode[6] + 1;
        double *ptrd = &_mp_arg(1) + 1;
        mp_func op = (mp_func)mp.opcode[4];
        CImg<ulongT> l_opcode(mp.opcode._data + 3,nb_args + 2);
        l_opcode[0] = mp.opcode[1];
        l_opcode.swap(mp.opcode);
        ulongT &argument2 = mp.opcode[3];
        while (siz_vector-->0) { argument2 = ptrs++; *(ptrd++) = (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();
      }

      static double mp_vector_map_v(_cimg_math_parser& mp) { // Operator(vector,[...])
        unsigned int
          nb_args = (unsigned int)mp.opcode[2],
          siz_vector = (unsigned int)mp.opcode[3],
          ptrs = (unsigned int)mp.opcode[5] + 1;
        double *ptrd = &_mp_arg(1) + 1;
        mp_func op = (mp_func)mp.opcode[4];
        CImg<ulongT> l_opcode(mp.opcode._data + 3,nb_args + 2);
        l_opcode[0] = l_opcode[1];
        l_opcode.swap(mp.opcode);
        ulongT &argument = mp.opcode[2];
        while (siz_vector-->0) { argument = ptrs++; *(ptrd++) = (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();
      }

      static double mp_vector_map_vv(_cimg_math_parser& mp) { // Operator(vector,vector,[...])
        unsigned int
          nb_args = (unsigned int)mp.opcode[2],
          siz_vector = (unsigned int)mp.opcode[3],
          ptrs1 = (unsigned int)mp.opcode[5] + 1,
          ptrs2 = (unsigned int)mp.opcode[6] + 1;
        double *ptrd = &_mp_arg(1) + 1;
        mp_func op = (mp_func)mp.opcode[4];
        CImg<ulongT> l_opcode(mp.opcode._data + 3,nb_args + 2);
        l_opcode[0] = l_opcode[1];
        l_opcode.swap(mp.opcode);
        ulongT &argument1 = mp.opcode[2], &argument2 = mp.opcode[3];
        while (siz_vector-->0) { argument1 = ptrs1++; argument2 = ptrs2++; *(ptrd++) = (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();
      }

      static double mp_vector_neq(_cimg_math_parser& mp) {
        return !mp_vector_eq(mp);
      }

      static double mp_vector_norm0(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[2];
        double res = 0;
        for (unsigned int i = siz - 1; i>3; --i) res+=(double)(_mp_arg(i)?1:0);
        return res;
      }

      static double mp_vector_norm1(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[2];
        double res = 0;
        for (unsigned int i = siz - 1; i>3; --i) res+=(double)cimg::abs(_mp_arg(i));
        return res;
      }

      static double mp_vector_norm2(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[2];
        double res = 0;
        for (unsigned int i = siz - 1; i>3; --i) res+=(double)cimg::sqr(_mp_arg(i));
        return (double)std::sqrt(res);
      }

      static double mp_vector_norminf(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[2];
        double res = 0;
        for (unsigned int i = siz - 1; i>3; --i) {
          const double val = (double)cimg::abs(_mp_arg(i));
          if (val>res) res = val;
        }
        return res;
      }

      static double mp_vector_off(_cimg_math_parser& mp) {
        const unsigned int
          ptr = (unsigned int)mp.opcode[2] + 1,
          siz = (unsigned int)mp.opcode[3];
        const int off = (int)_mp_arg(4);
        return off>=0 && off<(int)siz?mp.mem[ptr + off]:cimg::type<double>::nan();
      }

      static double mp_vector_print(_cimg_math_parser& mp) {
        const bool print_string = (bool)mp.opcode[4];
        cimg_pragma_openmp(critical(mp_vector_print)) {
          CImg<charT> _expr(mp.opcode[2] - 5);
          const ulongT *ptrs = mp.opcode._data + 5;
          cimg_for(_expr,ptrd,char) *ptrd = (char)*(ptrs++);
          cimg::strellipsize(_expr);
          unsigned int
            ptr = (unsigned int)mp.opcode[1] + 1,
            siz0 = (unsigned int)mp.opcode[3],
            siz = siz0;
          cimg::mutex(6);
          std::fprintf(cimg::output(),"\n[" cimg_appname "_math_parser] %s = [ ",_expr._data);
          unsigned int count = 0;
          while (siz-->0) {
            if (count>=64 && siz>=64) {
              std::fprintf(cimg::output(),"...,");
              ptr = (unsigned int)mp.opcode[1] + 1 + siz0 - 64;
              siz = 64;
            } else std::fprintf(cimg::output(),"%.17g%s",mp.mem[ptr++],siz?",":"");
            ++count;
          }
          if (print_string) {
            CImg<charT> str(siz0 + 1);
            ptr = (unsigned int)mp.opcode[1] + 1;
            for (unsigned int k = 0; k<siz0; ++k) str[k] = (char)mp.mem[ptr++];
            str[siz0] = 0;
            cimg::strellipsize(str,1024,false);
            std::fprintf(cimg::output()," ] = '%s' (size: %u)",str._data,siz0);
          } else std::fprintf(cimg::output()," ] (size: %u)",siz0);
          std::fflush(cimg::output());
          cimg::mutex(6,0);
        }
        return cimg::type<double>::nan();
      }

      static double mp_vector_rand(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptr_pdf = (unsigned int)mp.opcode[5]!=~0U?&_mp_arg(5) + 1:0;
        const unsigned int
          siz = (unsigned int)mp.opcode[2],
          siz_pdf = (unsigned int)mp.opcode[6],
          prec = (unsigned int)mp.opcode[7]!=~0U?(unsigned int)std::abs(_mp_arg(7)):65536;
        const double
          val_min = _mp_arg(3),
          val_max = _mp_arg(4);
        if (!ptr_pdf)
          CImg<doubleT>(ptrd,siz,1,1,1,true).rand(val_min,val_max);
        else {
          CImg<doubleT> pdf(ptr_pdf,siz_pdf,1,1,1,true);
          CImg<doubleT>(ptrd,siz,1,1,1,true).rand(val_min,val_max,pdf,prec);
        }
        return cimg::type<double>::nan();
      }

      static double mp_vector_resize(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int target_siz = (unsigned int)mp.opcode[2], initial_siz = (unsigned int)mp.opcode[4];
        const int
          interpolation = (int)_mp_arg(5),
          boundary_conditions = (int)_mp_arg(6);
        if (initial_siz) { // Resize vector
          const double *const ptrs = &_mp_arg(3) + 1;
          CImg<doubleT>(ptrd,target_siz,1,1,1,true) = CImg<doubleT>(ptrs,initial_siz,1,1,1,true).
            get_resize(target_siz,1,1,1,interpolation,boundary_conditions);
        } else { // Resize scalar
          const double value = _mp_arg(3);
          CImg<doubleT>(ptrd,target_siz,1,1,1,true) = CImg<doubleT>(1,1,1,1,value).
            resize(target_siz,1,1,1,interpolation,boundary_conditions);
        }
        return cimg::type<double>::nan();
      }

      static double mp_vector_resize_ext(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int
          siz = (unsigned int)mp.opcode[2],
          ow = (unsigned int)mp.opcode[4],
          oh = (unsigned int)mp.opcode[5],
          od = (unsigned int)mp.opcode[6],
          os = (unsigned int)mp.opcode[7],
          nw = (unsigned int)mp.opcode[8],
          nh = (unsigned int)mp.opcode[9],
          nd = (unsigned int)mp.opcode[10],
          ns = (unsigned int)mp.opcode[11];
        const int
          interpolation = (int)_mp_arg(12),
          boundary_conditions = (int)_mp_arg(13);
        const float
          ax = (float)_mp_arg(14),
          ay = (float)_mp_arg(15),
          az = (float)_mp_arg(16),
          ac = (float)_mp_arg(17);
        if (siz) { // Resize vector
          const double *const ptrs = &_mp_arg(3) + 1;
          CImg<doubleT>(ptrd,nw,nh,nd,ns,true) = CImg<doubleT>(ptrs,ow,oh,od,os,true).
            get_resize(nw,nh,nd,ns,interpolation,boundary_conditions,ax,ay,az,ac);
        } else { // Resize scalar
          const double value = _mp_arg(3);
          CImg<doubleT>(ptrd,nw,nh,nd,ns,true) = CImg<doubleT>(1,1,1,1,value).
            resize(nw,nh,nd,ns,interpolation,boundary_conditions,ax,ay,az,ac);
        }
        return cimg::type<double>::nan();
      }

      static double mp_vector_set_off(_cimg_math_parser& mp) {
        const unsigned int
          ptr = (unsigned int)mp.opcode[2] + 1,
          siz = (unsigned int)mp.opcode[3];
        const int off = (int)_mp_arg(4);
        if (off>=0 && off<(int)siz) mp.mem[ptr + off] = _mp_arg(1);
        return _mp_arg(1);
      }

      static double mp_vector_shift(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(2) + 1;
        const unsigned int siz = (unsigned int)mp.opcode[3];
        const int
          shift = (int)_mp_arg(4),
          boundary_conditions = (int)_mp_arg(5);
        CImg<doubleT>(ptrd,siz,1,1,1,true) =
          CImg<doubleT>(ptrs,siz,1,1,1,true).get_shift(shift,0,0,0,boundary_conditions);
        return cimg::type<double>::nan();
      }

      static double mp_vector_stats(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptrs = &_mp_arg(2) + 1;
        const unsigned int
          w = (unsigned int)mp.opcode[3],
          h = (unsigned int)mp.opcode[4],
          d = (unsigned int)mp.opcode[5],
          s = (unsigned int)mp.opcode[6];
        CImg<doubleT>(ptrd,14,1,1,1,true) = CImg<doubleT>(ptrs,w,h,d,s,true).get_stats();
        return cimg::type<double>::nan();
      }

      static double mp_vkth(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.get_shared_points(1,vec.width() - 1).
                       kth_smallest((ulongT)cimg::cut((longT)*vec - 1,(longT)0,(longT)vec.width() - 2)));
      }

      static double mp_vmax(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.max());
      }

      static double mp_vmaxabs(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.maxabs());
      }

      static double mp_vmedian(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.median());
      }

      static double mp_vmin(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.min());
      }

      static double mp_vminabs(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.minabs());
      }

      static double mp_vprod(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.product());
      }

      static double mp_vstd(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = std::sqrt(vec.get_stats()[3]));
      }

      static double mp_vsum(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.sum());
      }

      static double mp_vvar(_cimg_math_parser& mp) {
        _cimg_mp_vfunc(res = vec.get_stats()[3]);
      }

      static double mp_warp(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const unsigned int
          wA = (unsigned int)mp.opcode[3],
          hA = (unsigned int)mp.opcode[4],
          dA = (unsigned int)mp.opcode[5],
          sA = (unsigned int)mp.opcode[6],
          wB = (unsigned int)mp.opcode[8],
          hB = (unsigned int)mp.opcode[9],
          dB = (unsigned int)mp.opcode[10],
          sB = (unsigned int)mp.opcode[11];
        const int
          mode = (int)_mp_arg(12),
          interpolation = (int)_mp_arg(13),
          boundary_conditions = (int)_mp_arg(14);
        const double
          *const ptrs = &_mp_arg(2) + 1,
          *const ptrw = &_mp_arg(7) + 1;
        CImg<doubleT>(ptrd,wB,hB,dB,sA,true) = CImg<doubleT>(ptrs,wA,hA,dA,sA,true).
          get_warp(CImg<doubleT>(ptrw,wB,hB,dB,sB,true),mode,interpolation,boundary_conditions);
        return cimg::type<double>::nan();
      }

      static double mp_wave(_cimg_math_parser& mp) {
        return cimg::wave(_mp_arg(2),(unsigned int)_mp_arg(3));
      }

      static double mp_while(_cimg_math_parser& mp) {
        const ulongT
          mem_body = mp.opcode[1],
          mem_cond = mp.opcode[2];
        const CImg<ulongT>
          *const p_cond = ++mp.p_code,
          *const p_body = p_cond + mp.opcode[3],
          *const p_end = p_body + mp.opcode[4];
        const unsigned int siz = (unsigned int)mp.opcode[5];
        bool is_cond = false;
        if (mp.opcode[6]) { // Set default value for result and condition if necessary
          if (siz) CImg<doubleT>(&mp.mem[mem_body] + 1,siz,1,1,1,true).fill(cimg::type<double>::nan());
          else mp.mem[mem_body] = cimg::type<double>::nan();
        }
        if (mp.opcode[7]) mp.mem[mem_cond] = 0;
        const unsigned int _break_type = mp.break_type;
        mp.break_type = 0;
        do {
          mp.eval(p_cond,p_body); // Evaluate condition
          if (mp.break_type==1) break;
          is_cond = (bool)mp.mem[mem_cond];
          if (is_cond && !mp.break_type) mp.eval(p_body,p_end); // Evaluate body
          if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
        } while (is_cond);

        mp.break_type = _break_type;
        mp.p_code = p_end - 1;
        return mp.mem[mem_body];
      }

      static double mp_I(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const unsigned int siz = (unsigned int)mp.opcode[2];
        const CImg<T> &img = mp.imgin;
        if (img) {
          const longT whd = (longT)img.width()*img.height()*img.depth();
          const int
            x = (int)mp.mem[_cimg_mp_slot_x],
            y = (int)mp.mem[_cimg_mp_slot_y],
            z = (int)mp.mem[_cimg_mp_slot_z];
          const T *ptrs = img.data(x,y,z);
          cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
        } else std::memset(ptrd,0,siz*sizeof(double));
        return cimg::type<double>::nan();
      }

      static double mp_IJoff(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const bool is_relative = (bool)mp.opcode[2];
        const unsigned int
          _ind = (unsigned int)mp.opcode[3],
          ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width()),
          boundary_conditions = (unsigned int)_mp_arg(5),
          siz = (unsigned int)mp.opcode[6];
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        const longT
          off = (longT)(is_relative?img.offset((int)mp.mem[_cimg_mp_slot_x],
                                               (int)mp.mem[_cimg_mp_slot_y],
                                               (int)mp.mem[_cimg_mp_slot_z]) + _mp_arg(4):_mp_arg(4)),
          whd = (longT)img.width()*img.height()*img.depth();
        const T *ptrs;
        if (off>=0 && off<whd) {
          ptrs = &img[off];
          cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
          return cimg::type<double>::nan();
        }
        if (img._data) switch (boundary_conditions) {
          case 3 : { // Mirror
            const longT whd2 = 2*whd, moff = cimg::mod(off,whd2);
            ptrs = &img[moff<whd?moff:whd2 - moff - 1];
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
            return cimg::type<double>::nan();
          }
          case 2 : // Periodic
            ptrs = &img[cimg::mod(off,whd)];
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
            return cimg::type<double>::nan();
          case 1 : // Neumann
            ptrs = off<0?&img[0]:&img[whd - 1];
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
            return cimg::type<double>::nan();
          default : // Dirichlet
            std::memset(ptrd,0,siz*sizeof(double));
            return cimg::type<double>::nan();
          }
        std::memset(ptrd,0,siz*sizeof(double));
        return cimg::type<double>::nan();
      }

      static double mp_IJxyz3(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const bool is_relative = (bool)mp.opcode[2];
        const unsigned int
          _ind = (unsigned int)mp.opcode[3],
          ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width()),
          interpolation = (unsigned int)_mp_arg(7),
          boundary_conditions = (unsigned int)_mp_arg(8),
          siz = (unsigned int)mp.opcode[9];
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        const double
          x = is_relative?(mp.mem[_cimg_mp_slot_x] + _mp_arg(4)):_mp_arg(4),
          y = is_relative?(mp.mem[_cimg_mp_slot_y] + _mp_arg(5)):_mp_arg(5),
          z = is_relative?(mp.mem[_cimg_mp_slot_z] + _mp_arg(6)):_mp_arg(6);
        const ulongT whd = (ulongT)img._width*img._height*img._depth;
        const T *ptrs;
        switch (interpolation) {
        case 2 : // Cubic interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth();
            const float
              mx = cimg::mod((float)x,(float)w2),
              my = cimg::mod((float)y,(float)h2),
              mz = cimg::mod((float)z,(float)d2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._cubic_atXYZ(cx,cy,cz,c);
          } break;
          case 2 : // Periodic
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._cubic_atXYZ_p((float)x,(float)y,(float)z,c);
            break;
          case 1 : // Neumann
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._cubic_atXYZ((float)x,(float)y,(float)z,c);
            break;
          default : // Dirichlet
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img.cubic_atXYZ((float)x,(float)y,(float)z,c,(T)0);
          } break;
        case 1 : // Linear interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth();
            const float
              mx = cimg::mod((float)x,(float)w2),
              my = cimg::mod((float)y,(float)h2),
              mz = cimg::mod((float)z,(float)d2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._linear_atXYZ(cx,cy,cz,c);
          } break;
          case 2 : // Periodic
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._linear_atXYZ_p((float)x,(float)y,(float)z,c);
            break;
          case 1 : // Neumann
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._linear_atXYZ((float)x,(float)y,(float)z,c);
            break;
          default : // Dirichlet
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img.linear_atXYZ((float)x,(float)y,(float)z,c,(T)0);
          } break;
        default : // Nearest neighbor interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(),
              mx = cimg::mod((int)x,w2),
              my = cimg::mod((int)y,h2),
              mz = cimg::mod((int)z,d2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            ptrs = &img(cx,cy,cz);
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          case 2 : { // Periodic
            const int
              cx = (int)cimg::mod(x,(double)img._width),
              cy = (int)cimg::mod(y,(double)img._height),
              cz = (int)cimg::mod(z,(double)img._depth);
            ptrs = &img(cx,cy,cz);
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          case 1 : { // Neumann
            ptrs = &img._atXYZ((int)x,(int)y,(int)z);
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          default : // Dirichlet
            if (img.containsXYZC((int)x,(int)y,(int)z)) {
              ptrs = &img((int)x,(int)y,(int)z);
              cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
            } else std::memset(ptrd,0,siz*sizeof(double));
          }
        }
        return cimg::type<double>::nan();
      }

      static double mp_IJxyz2(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const bool is_relative = (bool)mp.opcode[2];
        const unsigned int
          _ind = (unsigned int)mp.opcode[3],
          ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width()),
          interpolation = (unsigned int)_mp_arg(7),
          boundary_conditions = (unsigned int)_mp_arg(8),
          siz = (unsigned int)mp.opcode[9];
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        const double
          x = is_relative?(mp.mem[_cimg_mp_slot_x] + _mp_arg(4)):_mp_arg(4),
          y = is_relative?(mp.mem[_cimg_mp_slot_y] + _mp_arg(5)):_mp_arg(5);
        const int
          z = (int)(is_relative?(mp.mem[_cimg_mp_slot_z] + _mp_arg(6)):_mp_arg(6));
        const ulongT whd = (ulongT)img._width*img._height*img._depth;
        const T *ptrs;
        switch (interpolation) {
        case 2 : // Cubic interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(),
              mz = cimg::mod(z,d2),
              cz = mz<img.depth()?mz:d2 - mz - 1;
            const float
              mx = cimg::mod((float)x,(float)w2),
              my = cimg::mod((float)y,(float)h2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1;
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._cubic_atXY(cx,cy,cz,c);
          } break;
          case 2 : // Periodic
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._cubic_atXY_p((float)x,(float)y,z,c);
            break;
          case 1 : // Neumann
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._cubic_atXY((float)x,(float)y,z,c);
            break;
          default : // Dirichlet
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img.cubic_atXY((float)x,(float)y,z,c,(T)0);
          } break;
        case 1 : // Linear interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(),
              mz = cimg::mod(z,d2),
              cz = mz<img.depth()?mz:d2 - mz - 1;
            const float
              mx = cimg::mod((float)x,(float)w2),
              my = cimg::mod((float)y,(float)h2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1;
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._linear_atXY(cx,cy,cz,c);
          } break;
          case 2 : // Periodic
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._linear_atXY_p((float)x,(float)y,z,c);
            break;
          case 1 : // Neumann
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._linear_atXY((float)x,(float)y,z,c);
            break;
          default : // Dirichlet
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img.linear_atXY((float)x,(float)y,z,c,(T)0);
          } break;
        default : // Nearest neighbor interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(),
              mz = cimg::mod(z,d2),
              mx = cimg::mod((int)x,w2),
              my = cimg::mod((int)y,h2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            ptrs = &img(cx,cy,cz);
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          case 2 : { // Periodic
            const int
              cx = (int)cimg::mod(x,(double)img._width),
              cy = (int)cimg::mod(y,(double)img._height),
              cz = cimg::mod(z,img.depth());
            ptrs = &img(cx,cy,cz);
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          case 1 : { // Neumann
            ptrs = &img._atXYZ((int)x,(int)y,z);
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          default : // Dirichlet
            if (img.containsXYZC((int)x,(int)y,z)) {
              ptrs = &img((int)x,(int)y,z);
              cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
            } else std::memset(ptrd,0,siz*sizeof(double));
          }
        }
        return cimg::type<double>::nan();
      }

      static double mp_IJxyz1(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const bool is_relative = (bool)mp.opcode[2];
        const unsigned int
          _ind = (unsigned int)mp.opcode[3],
          ind = _ind==~0U?~0U:(unsigned int)cimg::mod((int)mp.mem[_ind],mp.imglist.width()),
          interpolation = (unsigned int)_mp_arg(7),
          boundary_conditions = (unsigned int)_mp_arg(8),
          siz = (unsigned int)mp.opcode[9];
        const CImg<T> &img = ind==~0U?mp.imgin:mp.imglist[ind];
        const double
          x = is_relative?(mp.mem[_cimg_mp_slot_x] + _mp_arg(4)):_mp_arg(4);
        const int
          y = (int)(is_relative?(mp.mem[_cimg_mp_slot_y] + _mp_arg(5)):_mp_arg(5)),
          z = (int)(is_relative?(mp.mem[_cimg_mp_slot_z] + _mp_arg(6)):_mp_arg(6));
        const ulongT whd = (ulongT)img._width*img._height*img._depth;
        const T *ptrs;
        switch (interpolation) {
        case 2 : // Cubic interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(),
              my = cimg::mod(y,h2),
              mz = cimg::mod(z,d2),
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            const float
              mx = cimg::mod((float)x,(float)w2),
              cx = mx<img.width()?mx:w2 - mx - 1;
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._cubic_atX(cx,cy,cz,c);
          } break;
          case 2 : // Periodic
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._cubic_atX_p((float)x,y,z,c);
            break;
          case 1 : // Neumann
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._cubic_atX((float)x,y,z,c);
            break;
          default : // Dirichlet
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img.cubic_atX((float)x,y,z,c,(T)0);
          } break;
        case 1 : // Linear interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(),
              my = cimg::mod(y,h2),
              mz = cimg::mod(z,d2),
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            const float
              mx = cimg::mod((float)x,(float)w2),
              cx = mx<img.width()?mx:w2 - mx - 1;
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._linear_atX(cx,cy,cz,c);
          } break;
          case 2 : // Periodic
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._linear_atX_p((float)x,y,z,c);
            break;
          case 1 : // Neumann
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img._linear_atX((float)x,y,z,c);
            break;
          default : // Dirichlet
            cimg_for_inC(img,0,siz - 1,c) *(ptrd++) = (double)img.linear_atX((float)x,y,z,c,(T)0);
          } break;
        default : // Nearest neighbor interpolation
          switch (boundary_conditions) {
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(),
              my = cimg::mod(y,h2),
              mz = cimg::mod(z,d2),
              mx = cimg::mod((int)x,w2),
              cx = mx<img.width()?mx:w2 - mx - 1,
              cy = my<img.height()?my:h2 - my - 1,
              cz = mz<img.depth()?mz:d2 - mz - 1;
            ptrs = &img(cx,cy,cz);
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          case 2 : { // Periodic
            const int
              cx = (int)cimg::mod(x,(double)img._width),
              cy = cimg::mod(y,img.height()),
              cz = cimg::mod(z,img.depth());
            ptrs = &img(cx,cy,cz);
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          case 1 : { // Neumann
            ptrs = &img._atXYZ((int)x,y,z);
            cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
          } break;
          default : // Dirichlet
            if (img.containsXYZC((int)x,y,z)) {
              ptrs = &img((int)x,y,z);
              cimg_for_inC(img,0,siz - 1,c) { *(ptrd++) = (double)*ptrs; ptrs+=whd; }
            } else std::memset(ptrd,0,siz*sizeof(double));
          }
        }
        return cimg::type<double>::nan();
      }

#undef _mp_arg

    }; // struct _cimg_math_parser {}

#endif
