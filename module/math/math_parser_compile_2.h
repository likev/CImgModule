#ifndef CIMG_MODULE_MATH_PARSER_COMPILE_2_H
#define CIMG_MODULE_MATH_PARSER_COMPILE_2_H

        // Array-like access to vectors and image values 'i/j/I/J[_#ind,offset,_boundary]' and 'vector[offset]'.
        if (*se1==']') {
          _cimg_mp_op("Value accessor '[]'");

          // Find opening bracket for the offset.
          s0 = se1; while (s0>ss && (*s0!='[' || level[s0 - expr._data]!=clevel)) --s0;
          if (s0>ss) { s1 = s0; do { --s1; } while (cimg::is_blank(*s1)); cimg::swap(*s0,*++s1); }
          is_sth=s0>ss && *(s0-1)==']'; // Particular case s.a. '..[..][..]' ?
          is_relative = *ss=='j' || *ss=='J';

          if (!is_sth && (*ss=='I' || *ss=='J') && *ss1=='[' &&
              (reserved_label[(int)*ss]==~0U ||
               !is_vector(reserved_label[(int)*ss]))) { // Image value as a vector
            if (*ss2=='#') { // Index specified
              s0 = ss3; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
              p1 = compile(ss3,s0++,depth1,0,block_flags);
              _cimg_mp_check_const_index(p1);
              _cimg_mp_check_notnan_index(p1,ss3);
              _cimg_mp_check_list();
            } else { p1 = ~0U; s0 = ss2; }
            s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
            p2 = 1 + (p1!=~0U);
            arg1 = compile(s0,s1,depth1,0,block_flags); // Offset
            _cimg_mp_check_type(arg1,p2,1,0);
            arg2 = ~0U;
            if (s1<se1) {
              arg2 = compile(++s1,se1,depth1,0,block_flags); // Boundary
              _cimg_mp_check_type(arg2,p2 + 1,1,0);
            }
            if (p_ref && arg2==~0U) {
              *p_ref = 4;
              p_ref[1] = p1;
              p_ref[2] = (unsigned int)is_relative;
              p_ref[3] = arg1;
              if (is_comp_scalar(arg1)) memtype[arg1] = -1;
            }
            p2 = ~0U; // 'p2' must be the dimension of the vector-valued operand if any
            if (p1==~0U) p2 = imgin._spectrum;
            else {
              p3 = (unsigned int)cimg::mod((int)mem[p1],imglist.width());
              p2 = imglist[p3]._spectrum;
            }
            if (!p2) _cimg_mp_return(0);
            pos = vector(p2);
            if (p1==~0U) need_input_copy = true;
            CImg<ulongT>::vector((ulongT)mp_IJoff,pos,(ulongT)is_relative,p1,
                                 arg1,arg2==~0U?_cimg_mp_boundary:arg2,p2).move_to(code);
            return_comp = true;
            _cimg_mp_return(pos);
          }

          if (!is_sth && (*ss=='i' || *ss=='j') && *ss1=='[' &&
              (reserved_label[(int)*ss]==~0U ||
               !is_vector(reserved_label[(int)*ss]))) { // Image value as a scalar
            if (*ss2=='#') { // Index specified
              s0 = ss3; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
              p1 = compile(ss3,s0++,depth1,0,block_flags);
              _cimg_mp_check_notnan_index(p1,ss3);
            } else { p1 = ~0U; s0 = ss2; }
            s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
            arg1 = compile(s0,s1,depth1,0,block_flags); // Offset
            arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U; // Boundary
            if (p_ref && arg2==~0U) {
              *p_ref = 2;
              p_ref[1] = p1;
              p_ref[2] = (unsigned int)is_relative;
              p_ref[3] = arg1;
              if (p1!=~0U && is_comp_scalar(p1)) memtype[p1] = -1; // Prevent from being used in further optimization
              if (is_comp_scalar(arg1)) memtype[arg1] = -1;
            }
            if (p1==~0U) { if (!imgin) _cimg_mp_return(0); need_input_copy = true; }
            else if (!imglist) _cimg_mp_return(0);
            pos = scalar4(mp_ijoff,(unsigned int)is_relative,p1,arg1,arg2==~0U?_cimg_mp_boundary:arg2);
            memtype[pos] = -1; // Prevent from being used in further optimization
            _cimg_mp_return(pos);
          }

          s0 = se1; while (s0>ss && (*s0!='[' || level[s0 - expr._data]!=clevel)) --s0;
          if (s0>ss) { // Vector element
            arg1 = compile(ss,s0,depth1,0,block_flags);
            if (is_scalar(arg1)) {
              variable_name.assign(ss,(unsigned int)(s0 - ss + 1)).back() = 0;
              cimg::strellipsize(variable_name,64);
              _cimg_mp_strerr;
              throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                          "CImg<%s>::%s: %s: Array brackets used on non-vector variable '%s', "
                                          "in expression '%s'.",
                                          pixel_type(),_cimg_mp_calling_function,s_op,
                                          variable_name._data,s0);
            }
            s1 = s0 + 1; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;

            if (s1<se1) { // Two or three arguments -> sub-vector extraction
              p1 = size(arg1);
              arg2 = compile(++s0,s1,depth1,0,block_flags); // Starting index
              s0 = ++s1; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
              arg3 = compile(s1,s0,depth1,0,block_flags); // Length
              arg4 = s0<se1?compile(++s0,se1,depth1,0,block_flags):1; // Step
              _cimg_mp_check_const_scalar(arg3,2,3);
              arg3 = (unsigned int)mem[arg3];
              pos = vector(arg3);
              CImg<ulongT>::vector((ulongT)mp_vector_crop,pos,arg1,p1,arg2,arg3,arg4).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            // One argument -> vector value reference.
            arg2 = compile(++s0,se1,depth1,0,block_flags);
            if (is_const_scalar(arg2)) { // Constant index
              nb = (int)mem[arg2];
              if (nb>=0 && nb<(int)size(arg1)) _cimg_mp_return(arg1 + 1 + nb);
              variable_name.assign(ss,(unsigned int)(s0 - ss)).back() = 0;
              cimg::strellipsize(variable_name,64);
              _cimg_mp_strerr;
              throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                          "CImg<%s>::%s: Out-of-bounds reference '%s[%d]' "
                                          "(vector '%s' has dimension %u), "
                                          "in expression '%s'.",
                                          pixel_type(),_cimg_mp_calling_function,
                                          variable_name._data,nb,
                                          variable_name._data,size(arg1),s0);
            }
            if (p_ref) {
              *p_ref = 1;
              p_ref[1] = arg1;
              p_ref[2] = arg2;
              if (is_comp_scalar(arg2)) memtype[arg2] = -1; // Prevent from being used in further optimization
            }
            pos = scalar3(mp_vector_off,arg1,size(arg1),arg2);
            memtype[pos] = -1; // Prevent from being used in further optimization
            _cimg_mp_return(pos);
          }
        }

        // Look for a function call, an access to image value, or a parenthesis.
        if (*se1==')') {
          if (*ss=='(') _cimg_mp_return(compile(ss1,se1,depth1,p_ref,block_flags)); // Simple parentheses
          _cimg_mp_op("Value accessor '()'");
          is_relative = *ss=='j' || *ss=='J';
          s0 = s1 = std::strchr(ss,'('); if (s0) { do { --s1; } while (cimg::is_blank(*s1)); cimg::swap(*s0,*++s1); }

          // I/J(_#ind,_x,_y,_z,_interpolation,_boundary_conditions).
          if ((*ss=='I' || *ss=='J') && *ss1=='(') { // Image value as scalar
            if (*ss2=='#') { // Index specified
              s0 = ss3; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
              p1 = compile(ss3,s0++,depth1,0,block_flags);
              _cimg_mp_check_const_index(p1);
              _cimg_mp_check_notnan_index(p1,ss3);
              _cimg_mp_check_list();
            } else { p1 = ~0U; s0 = ss2; }
            arg1 = is_relative?0U:(unsigned int)_cimg_mp_slot_x;
            arg2 = is_relative?0U:(unsigned int)_cimg_mp_slot_y;
            arg3 = is_relative?0U:(unsigned int)_cimg_mp_slot_z;
            arg4 = arg5 = ~0U;
            if (s0<se1) {
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(s0,s1,depth1,0,block_flags);
              if (is_vector(arg1)) { // Coordinates specified as a vector
                p2 = size(arg1);
                ++arg1;
                if (p2>1) {
                  arg2 = arg1 + 1;
                  if (p2>2) arg3 = arg2 + 1;
                }
                if (s1<se1) {
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg4 = compile(s1,s2,depth1,0,block_flags);
                  arg5 = s2<se1?compile(++s2,se1,depth1,0,block_flags):~0U;
                }
              } else if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg2 = compile(s1,s2,depth1,0,block_flags);
                if (s2<se1) {
                  s3 = ++s2; while (s3<se1 && (*s3!=',' || level[s3 - expr._data]!=clevel1)) ++s3;
                  arg3 = compile(s2,s3,depth1,0,block_flags);
                  if (s3<se1) {
                    s2 = ++s3; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                    arg4 = compile(s3,s2,depth1,0,block_flags);
                    arg5 = s2<se1?compile(++s2,se1,depth1,0,block_flags):~0U;
                  }
                }
              }
            }
            if (p_ref && arg4==~0U && arg5==~0U) {
              *p_ref = 5;
              p_ref[1] = p1;
              p_ref[2] = (unsigned int)is_relative;
              p_ref[3] = arg1;
              p_ref[4] = arg2;
              p_ref[5] = arg3;
              if (p1!=~0U && is_comp_scalar(p1)) memtype[p1] = -1; // Prevent from being used in further optimization
              if (is_comp_scalar(arg1)) memtype[arg1] = -1;
              if (is_comp_scalar(arg2)) memtype[arg2] = -1;
              if (is_comp_scalar(arg3)) memtype[arg3] = -1;
            }
            p2 = ~0U; // 'p2' must be the dimension of the vector-valued operand if any
            if (p1==~0U) p2 = imgin._spectrum;
            else if (is_const_scalar(p1)) {
              p3 = (unsigned int)cimg::mod((int)mem[p1],imglist.width());
              p2 = imglist[p3]._spectrum;
            }
            if (!p2) _cimg_mp_return(0);
            pos = vector(p2);
            const bool
              is_intz = is_const_integer(arg3) || (arg3>=_cimg_mp_slot_x && arg3<=_cimg_mp_slot_c), // optimized for 2D?
              is_inty = is_intz && // optimized for 1D?
              (is_const_integer(arg2) || (arg2>=_cimg_mp_slot_x && arg2<=_cimg_mp_slot_c));
            if (p1==~0U) need_input_copy = true;
            CImg<ulongT>::vector((ulongT)(is_inty?mp_IJxyz1:is_intz?mp_IJxyz2:mp_IJxyz3),
                                 pos,(ulongT)is_relative,p1,arg1,arg2,arg3,
                                 arg4==~0U?_cimg_mp_interpolation:arg4,
                                 arg5==~0U?_cimg_mp_boundary:arg5,p2).move_to(code);
            return_comp = true;
            _cimg_mp_return(pos);
          }

          // i/j(_#ind,_x,_y,_z,_c,_interpolation,_boundary_conditions).
          if ((*ss=='i' || *ss=='j') && *ss1=='(') { // Image value as scalar
            if (*ss2=='#') { // Index specified
              s0 = ss3; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
              p1 = compile(ss3,s0++,depth1,0,block_flags);
              _cimg_mp_check_notnan_index(p1,ss3);
            } else { p1 = ~0U; s0 = ss2; }
            arg1 = is_relative?0U:(unsigned int)_cimg_mp_slot_x;
            arg2 = is_relative?0U:(unsigned int)_cimg_mp_slot_y;
            arg3 = is_relative?0U:(unsigned int)_cimg_mp_slot_z;
            arg4 = is_relative?0U:(unsigned int)_cimg_mp_slot_c;
            arg5 = arg6 = ~0U;
            if (s0<se1) {
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(s0,s1,depth1,0,block_flags);
              if (is_vector(arg1)) { // Coordinates specified as a vector
                p2 = size(arg1);
                ++arg1;
                if (p2>1) {
                  arg2 = arg1 + 1;
                  if (p2>2) {
                    arg3 = arg2 + 1;
                    if (p2>3) arg4 = arg3 + 1;
                  }
                }
                if (s1<se1) {
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg5 = compile(s1,s2,depth1,0,block_flags);
                  arg6 = s2<se1?compile(++s2,se1,depth1,0,block_flags):~0U;
                }
              } else if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg2 = compile(s1,s2,depth1,0,block_flags);
                if (s2<se1) {
                  s3 = ++s2; while (s3<se1 && (*s3!=',' || level[s3 - expr._data]!=clevel1)) ++s3;
                  arg3 = compile(s2,s3,depth1,0,block_flags);
                  if (s3<se1) {
                    s2 = ++s3; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                    arg4 = compile(s3,s2,depth1,0,block_flags);
                    if (s2<se1) {
                      s3 = ++s2; while (s3<se1 && (*s3!=',' || level[s3 - expr._data]!=clevel1)) ++s3;
                      arg5 = compile(s2,s3,depth1,0,block_flags);
                      arg6 = s3<se1?compile(++s3,se1,depth1,0,block_flags):~0U;
                    }
                  }
                }
              }
            }
            if (p_ref && arg5==~0U && arg6==~0U) {
              *p_ref = 3;
              p_ref[1] = p1;
              p_ref[2] = (unsigned int)is_relative;
              p_ref[3] = arg1;
              p_ref[4] = arg2;
              p_ref[5] = arg3;
              p_ref[6] = arg4;
              if (p1!=~0U && is_comp_scalar(p1)) memtype[p1] = -1; // Prevent from being used in further optimization
              if (is_comp_scalar(arg1)) memtype[arg1] = -1;
              if (is_comp_scalar(arg2)) memtype[arg2] = -1;
              if (is_comp_scalar(arg3)) memtype[arg3] = -1;
              if (is_comp_scalar(arg4)) memtype[arg4] = -1;
            }

            const bool
              is_intz = is_const_integer(arg3) || (arg3>=_cimg_mp_slot_x && arg3<=_cimg_mp_slot_c), // optimized for 2D?
              is_inty = is_intz && // optimized for 1D?
              (is_const_integer(arg2) || (arg2>=_cimg_mp_slot_x && arg2<=_cimg_mp_slot_c));
            if (p1==~0U) { if (!imgin) _cimg_mp_return(0); need_input_copy = true; }
            else if (!imglist) _cimg_mp_return(0);
            pos = scalar8(is_inty?mp_ijxyzc1:is_intz?mp_ijxyzc2:mp_ijxyzc3,
                          (unsigned int)is_relative,p1,arg1,arg2,arg3,arg4,
                          arg5==~0U?_cimg_mp_interpolation:arg5,
                          arg6==~0U?_cimg_mp_boundary:arg6);
            memtype[pos] = -1; // Prevent from being used in further optimization
            _cimg_mp_return(pos);
          }

          // Mathematical functions.
          switch (*ss) {

          case 'a' :

#ifdef cimg_mp_func_abort
            if (!std::strncmp(ss,"abort(",6)) { // Abort
              _cimg_mp_op("Function 'abort()'");
              if (pexpr[se2 - expr._data]=='(') { // no arguments?
                CImg<ulongT>::vector((ulongT)mp_abort,_cimg_mp_slot_nan).move_to(code);
                _cimg_mp_return_nan();
              }
            }
#endif

            if (!std::strncmp(ss,"abs(",4)) { // Absolute value
              _cimg_mp_op("Function 'abs()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              if (code) { // Spot 'abs(maxabs())' and 'abs(minabs())' cases
                CImg<ulongT> &pop = code.back();
                if (pop[0]==(ulongT)mp_maxabs) { pop[0] = (ulongT)mp_absmaxabs; _cimg_mp_return(pop[1]); }
                else if (pop[0]==(ulongT)mp_minabs) { pop[0] = (ulongT)mp_absminabs; _cimg_mp_return(pop[1]); }
              }
              if (is_vector(arg1)) _cimg_mp_vector1_v(abs,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::abs(mem[arg1]));
              _cimg_mp_scalar1(abs,arg1);
            }

            if (!std::strncmp(ss,"abscut(",7)) { // Cut of absolute value
              _cimg_mp_op("Function 'abscut()'");
              s1 = ss7; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss7,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              if (s2<se1) {
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg3 = compile(s2,s1,depth1,0,block_flags);
                arg4 = s1<se1?compile(++s1,se1,depth1,0,block_flags):0;
              } else { arg3 = const_scalar(cimg::type<double>::inf()); arg4 = 0; }
              _cimg_mp_check_type(arg2,2,1,0);
              _cimg_mp_check_type(arg3,3,1,0);
              _cimg_mp_check_type(arg4,3,1,0);
              if (is_vector(arg1)) _cimg_mp_vector4_vsss(abscut,arg1,arg2,arg3,arg4);
              if (is_const_scalar(arg1) && is_const_scalar(arg2) && is_const_scalar(arg3) && is_const_scalar(arg4)) {
                val = mem[arg1];
                val1 = mem[arg2];
                val2 = mem[arg3];
                val3 = mem[arg4];
                _cimg_mp_const_scalar(cimg::abscut(val,val1,val2,val3));
              }
              _cimg_mp_scalar4(abscut,arg1,arg2,arg3,arg4);
            }

            if (!std::strncmp(ss,"addr(",5)) { // Pointer address
              _cimg_mp_op("Function 'addr()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_const_scalar((double)arg1);
            }

            if (!std::strncmp(ss,"acos(",5)) { // Arccos
              _cimg_mp_op("Function 'acos()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(acos,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::acos(mem[arg1]));
              _cimg_mp_scalar1(acos,arg1);
            }

            if (!std::strncmp(ss,"acosh(",6)) { // Hyperbolic arccosine
              _cimg_mp_op("Function 'acosh()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(acosh,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::acosh(mem[arg1]));
              _cimg_mp_scalar1(acosh,arg1);
            }

            if (!std::strncmp(ss,"arg0(",5) ||
                !std::strncmp(ss,"arg1(",5)) { // Nth argument
              _cimg_mp_op(*ss3=='0'?"Function 'arg0()'":"Function 'arg1()'");
              s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss5,s1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,1,0);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              p2 = size(arg2);
              p3 = 3;
              CImg<ulongT>::vector((ulongT)(*ss3=='0'?mp_arg0:mp_arg1),0,0,p2,arg1,arg2).move_to(l_opcode);
              for (s = ++s2; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg3 = compile(s,ns,depth1,0,block_flags);
                _cimg_mp_check_type(arg3,p3,p2?2:1,p2);
                CImg<ulongT>::vector(arg3).move_to(l_opcode);
                ++p3;
                s = ns;
              }
              (l_opcode>'y').move_to(opcode);
              opcode[2] = opcode._height;
              if (is_const_scalar(arg1)) {
                p3-=1; // Number of args
                if (*ss3=='0') arg1 = (unsigned int)(mem[arg1]<0?mem[arg1] + p3:mem[arg1] + 1);
                else arg1 = (unsigned int)(mem[arg1]<0?mem[arg1] + p3:mem[arg1]);
                if (arg1<p3) _cimg_mp_return(opcode[4 + arg1]);
                if (p2) {
                  pos = vector(p2);
                  std::memset(&mem[pos] + 1,0,p2*sizeof(double));
                  return_comp = true;
                  _cimg_mp_return(pos);
                } else _cimg_mp_return(0);
              }
              pos = opcode[1] = p2?vector(p2):scalar();
              opcode.move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"asin(",5)) { // Arcsin
              _cimg_mp_op("Function 'asin()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(asin,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::asin(mem[arg1]));
              _cimg_mp_scalar1(asin,arg1);
            }

            if (!std::strncmp(ss,"asinh(",6)) { // Hyperbolic arcsine
              _cimg_mp_op("Function 'asinh()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(asinh,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::asinh(mem[arg1]));
              _cimg_mp_scalar1(asinh,arg1);
            }

            if (!std::strncmp(ss,"atan(",5)) { // Arctan
              _cimg_mp_op("Function 'atan()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(atan,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::atan(mem[arg1]));
              _cimg_mp_scalar1(atan,arg1);
            }

            if (!std::strncmp(ss,"atan2(",6)) { // Arctan2
              _cimg_mp_op("Function 'atan2()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags);
              arg2 = compile(++s1,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg2,2,3,size(arg1));
              if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(atan2,arg1,arg2);
              if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(atan2,arg1,arg2);
              if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(atan2,arg1,arg2);
              if (is_const_scalar(arg1) && is_const_scalar(arg2))
                _cimg_mp_const_scalar(std::atan2(mem[arg1],mem[arg2]));
              _cimg_mp_scalar2(atan2,arg1,arg2);
            }

            if (!std::strncmp(ss,"atanh(",6)) { // Hyperbolic arctangent
              _cimg_mp_op("Function 'atanh()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(atanh,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::atanh(mem[arg1]));
              _cimg_mp_scalar1(atanh,arg1);
            }
            break;

          case 'b' :
            if (!std::strncmp(ss,"begin(",6)) { // Begin
              _cimg_mp_op("Function 'begin()'");
              s1 = ss6; while (s1<se1 && cimg::is_blank(*s1)) ++s1;
              if (s1!=se1) {
                if (!is_inside_begin) code.swap(code_begin);
                arg1 = compile(s1,se1,depth1,p_ref,2);
                if (!is_inside_begin) code.swap(code_begin);
                _cimg_mp_return(arg1);
              } else _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"begin_t(",8)) { // Begin thread
              _cimg_mp_op("Function 'begin_t()'");
              s1 = ss8; while (s1<se1 && cimg::is_blank(*s1)) ++s1;
              if (s1!=se1) {
                if (!is_inside_begin_t) code.swap(code_begin_t);
                arg1 = compile(s1,se1,depth1,p_ref,4);
                if (!is_inside_begin_t) code.swap(code_begin_t);
                _cimg_mp_return(arg1);
              } else _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"bool(",5)) { // Boolean cast
              _cimg_mp_op("Function 'bool()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(bool,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar((bool)mem[arg1]);
              _cimg_mp_scalar1(bool,arg1);
            }

            if (!std::strncmp(ss,"break(",6)) { // Break current block
              if (pexpr[se2 - expr._data]=='(') { // no arguments?
                CImg<ulongT>::vector((ulongT)mp_break,_cimg_mp_slot_nan).move_to(code);
                _cimg_mp_return_nan();
              }
            }

            if (!std::strncmp(ss,"breakpoint(",11)) { // Break point (for abort test)
              _cimg_mp_op("Function 'breakpoint()'");
              if (pexpr[se2 - expr._data]=='(') { // no arguments?
                CImg<ulongT>::vector((ulongT)mp_breakpoint,_cimg_mp_slot_nan).move_to(code);
                _cimg_mp_return_nan();
              }
            }
            break;

          case 'c' :
            if (!std::strncmp(ss,"c2o(",4)) { // Coordinates to offset
              _cimg_mp_op("Function 'c2o()'");
              if (*ss4=='#') { // Index specified
                s0 = ss5; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss5,s0++,depth1,0,block_flags);
                p3 = 1;
                _cimg_mp_check_notnan_index(p1,ss5);
                _cimg_mp_check_list();
              } else { p1 = ~0U; s0 = ss4; p3 = 0; }
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              pos = compile(s0,s1,depth1,0,block_flags);
              p2 = size(pos);
              if (p2) { // Coordinates specified as a vector
                if (p2>4) {
                  *s1 = 0; s1 = s0;
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Argument '%s' is a vector of size %u (should be <=4), "
                                              "in expression '%s'.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,s1,p2,s0);
                }
                arg1 = pos + 1;
                arg2 = p2>1?pos + 2:0;
                arg3 = p2>2?pos + 3:0;
                arg4 = p2>3?pos + 4:0;
                arg5 = s1!=se1?compile(s1 + 1,se1,depth1,0,block_flags):0; // boundary conditions
                _cimg_mp_check_type(arg5,p3 + 2,1,0);
              } else { // Coordinates specified as scalars
                arg1 = pos; arg2 = arg3 = arg4 = arg5 = 0;
                if (s1<se1) {
                  s0 = ++s1; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                  arg2 = compile(s1,s0,depth1,0,block_flags);
                  _cimg_mp_check_type(arg2,p3 + 2,1,0);
                  if (s0<se1) {
                    s1 = ++s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                    arg3 = compile(s0,s1,depth1,0,block_flags);
                    _cimg_mp_check_type(arg3,p3 + 3,1,0);
                    if (s1<se1) {
                      s0 = ++s1; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                      arg4 = compile(s1,s0,depth1,0,block_flags);
                      _cimg_mp_check_type(arg4,p3 + 4,1,0);
                      arg5 = s0<se1?compile(++s0,se1,depth1,0,block_flags):0; // boundary conditions
                      _cimg_mp_check_type(arg5,p3 + 5,1,0);
                    }
                  } else arg3 = 0;
                } else arg2 = 0;
              }
              _cimg_mp_scalar6(c2o,p1,arg1,arg2,arg3,arg4,arg5);
            }

            if (!std::strncmp(ss,"cabs(",5)) { // Complex absolute value
              _cimg_mp_op("Function 'cabs()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              if (is_scalar(arg1)) _cimg_mp_scalar2(complex_abs,arg1,0);
              _cimg_mp_scalar2(complex_abs,arg1 + 1,arg1 + 2);
            }

            if (!std::strncmp(ss,"carg(",5)) { // Complex argument
              _cimg_mp_op("Function 'carg()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              if (is_scalar(arg1)) _cimg_mp_scalar2(atan2,0,arg1);
              _cimg_mp_scalar2(atan2,arg1 + 2,arg1 + 1);
            }

            if (!std::strncmp(ss,"cbrt(",5)) { // Cubic root
              _cimg_mp_op("Function 'cbrt()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(cbrt,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::cbrt(mem[arg1]));
              _cimg_mp_scalar1(cbrt,arg1);
            }

            if (!std::strncmp(ss,"cconj(",6)) { // Complex conjugate
              _cimg_mp_op("Function 'cconj()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_conj,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_conj,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"ccos(",5)) { // Complex cosine
              _cimg_mp_op("Function 'ccos()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_cos,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_cos,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"ccosh(",6)) { // Complex hyperbolic cosine
              _cimg_mp_op("Function 'ccosh()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_cosh,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_cosh,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"ceil(",5)) { // Ceil
              _cimg_mp_op("Function 'ceil()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(ceil,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::ceil(mem[arg1]));
              _cimg_mp_scalar1(ceil,arg1);
            }

            if (!std::strncmp(ss,"cexp(",5)) { // Complex exponential
              _cimg_mp_op("Function 'cexp()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_exp,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_exp,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"clog(",5)) { // Complex logarithm
              _cimg_mp_op("Function 'clog()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_log,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_log,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"continue(",9)) { // Continue loop
              if (pexpr[se2 - expr._data]=='(') { // no arguments?
                CImg<ulongT>::vector((ulongT)mp_continue,_cimg_mp_slot_nan).move_to(code);
                _cimg_mp_return_nan();
              }
            }

            if (!std::strncmp(ss,"convolve(",9) || !std::strncmp(ss,"correlate(",10)) { // Convolve & Correlate
              is_sth = *ss2=='n'; // is_convolve?
              _cimg_mp_op(is_sth?"Function 'convolve()'":"Function 'correlate()'");
              op = is_sth?mp_convolve:mp_correlate;
              const ulongT default_params[] = { (ulongT)op,0, // [0]=function, [1]=result vector
                0,0,0,0,0, // [2]=I, [3]=wI, [4]=hI, [5]=dI, [6]=sI
                0,0,0,0,0, // [7]=K, [8]=wK, [9]=hK, [10]=dK, [11]=sK
                1,0,1, // [12]=boundary_conditions, [13]=is_normalized, [14]=chan._mode
                ~0U,~0U,~0U, // [15]=xcenter, [16]=ycenter, [17]=zcenter
                1,1,1, // [18]=xstride, [19]=ystride, [20]=zstride
                1,1,1, // [21]=xdilation, [22]=ydilation, [23]=zdilation,
                0,0,0, // [24]=xoffset, [25]=yoffset, [26]=zoffset
                ~0U,~0U,~0U}; // [27]=xsize, [28]=ysize, [29]=zsize

              l_opcode.assign(); // Don't use 'opcode': it could be modified by further calls to 'compile()'!
              CImg<ulongT>(default_params,1,sizeof(default_params)/sizeof(ulongT)).move_to(l_opcode);

              arg1 = 2;
              for (s = std::strchr(ss,'(') + 1; s<se && arg1<l_opcode[0]._height; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                l_opcode(0,arg1++) = compile(s,ns,depth1,0,block_flags);
                s = ns;
              }
              l_opcode[0].move_to(opcode);

              if (arg1<12 || arg1>opcode._height) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: %s arguments provided, in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            arg1<12?"Not enough":"Too much",s0);
              }
              _cimg_mp_check_type(opcode[2],1,2,0); // I
              _cimg_mp_check_const_scalar(opcode[3],2,3); // wI
              _cimg_mp_check_const_scalar(opcode[4],3,3); // hI
              _cimg_mp_check_const_scalar(opcode[5],4,3); // dI
              _cimg_mp_check_const_scalar(opcode[6],5,3); // sI
              _cimg_mp_check_type(opcode[7],6,2,0); // K
              _cimg_mp_check_const_scalar(opcode[8],7,3); // wK
              _cimg_mp_check_const_scalar(opcode[9],8,3); // hK
              _cimg_mp_check_const_scalar(opcode[10],9,3); // dK
              _cimg_mp_check_const_scalar(opcode[11],10,3); // sK
              _cimg_mp_check_type(opcode[12],11,1,0); // boundary_conditions
              _cimg_mp_check_type(opcode[13],12,1,0); // is_normalized
              _cimg_mp_check_const_scalar(opcode[14],13,1); // channel_mode
              if (opcode[15]!=~0U) _cimg_mp_check_type(opcode[15],14,1,0); // xcenter
              if (opcode[16]!=~0U) _cimg_mp_check_type(opcode[16],15,1,0); // ycenter
              if (opcode[17]!=~0U) _cimg_mp_check_type(opcode[17],16,1,0); // zcenter
              _cimg_mp_check_const_scalar(opcode[18],17,3); // xstride
              _cimg_mp_check_const_scalar(opcode[19],18,3); // ystride
              _cimg_mp_check_const_scalar(opcode[20],19,3); // zstride
              _cimg_mp_check_type(opcode[21],20,1,0); // xdilation
              _cimg_mp_check_type(opcode[22],21,1,0); // ydilation
              _cimg_mp_check_type(opcode[23],22,1,0); // zdilation
              _cimg_mp_check_type(opcode[24],23,1,0); // xoffset
              _cimg_mp_check_type(opcode[25],24,1,0); // yoffset
              _cimg_mp_check_type(opcode[26],25,1,0); // zoffset
              if (opcode[27]!=~0U) _cimg_mp_check_const_scalar(opcode[27],26,3); // xsize
              if (opcode[28]!=~0U) _cimg_mp_check_const_scalar(opcode[28],27,3); // ysize
              if (opcode[29]!=~0U) _cimg_mp_check_const_scalar(opcode[29],28,3); // zsize

              const unsigned int
                wI = (unsigned int)mem[opcode[3]],
                hI = (unsigned int)mem[opcode[4]],
                dI = (unsigned int)mem[opcode[5]],
                sI = (unsigned int)mem[opcode[6]],
                wK = (unsigned int)mem[opcode[8]],
                hK = (unsigned int)mem[opcode[9]],
                dK = (unsigned int)mem[opcode[10]],
                sK = (unsigned int)mem[opcode[11]],
                channel_mode = (unsigned int)mem[opcode[14]],
                xstride = (int)mem[opcode[18]],
                ystride = (int)mem[opcode[19]],
                zstride = (int)mem[opcode[20]],
                xsize = opcode[27]==~0U?wI/xstride:(unsigned int)mem[opcode[27]],
                ysize = opcode[28]==~0U?hI/ystride:(unsigned int)mem[opcode[28]],
                zsize = opcode[29]==~0U?dI/zstride:(unsigned int)mem[opcode[29]];

              if (wI*hI*dI*sI!=size(opcode[2])) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Specified input size (%ux%ux%ux%u = %u) does "
                                            "not match size of input variable (%u), in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            wI,hI,dI,sI,wI*hI*dI*sI,size(opcode[2]),
                                            s0);
              }
              if (wK*hK*dK*sK!=size(opcode[7])) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Specified kernel size (%ux%ux%ux%u = %u) does "
                                            "not match size of kernel variable (%u), in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            wK,hK,dK,sK,wK*hK*dK*sK,size(opcode[7]),
                                            s0);
              }

              arg2 = !channel_mode?sI*sK:channel_mode==1?std::max(sI,sK):
                channel_mode==2?std::max(sI,sK)/std::min(sI,sK):1U;

              opcode[1] = pos = vector(xsize*ysize*zsize*arg2);
              opcode[3] = (ulongT)wI;
              opcode[4] = (ulongT)hI;
              opcode[5] = (ulongT)dI;
              opcode[6] = (ulongT)sI;
              opcode[8] = (ulongT)wK;
              opcode[9] = (ulongT)hK;
              opcode[10] = (ulongT)dK;
              opcode[11] = (ulongT)sK;
              opcode[14] = (ulongT)channel_mode;
              opcode[18] = (ulongT)xstride;
              opcode[19] = (ulongT)ystride;
              opcode[20] = (ulongT)zstride;
              opcode[27] = (ulongT)xsize;
              opcode[28] = (ulongT)ysize;
              opcode[29] = (ulongT)zsize;
              opcode.move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"copy(",5)) { // Memory copy
              _cimg_mp_op("Function 'copy()'");
              ref.assign(14);
              s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = p1 = compile(ss5,s1,depth1,ref,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,ref._data + 7,block_flags);
              arg3 = arg4 = arg5 = ~0U; arg6 = 1;
              if (s2<se1) {
                s3 = ++s2; while (s3<se1 && (*s3!=',' || level[s3 - expr._data]!=clevel1)) ++s3;
                arg3 = compile(s2,s3,depth1,0,block_flags);
                if (s3<se1) {
                  s1 = ++s3; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                  arg4 = compile(s3,s1,depth1,0,block_flags);
                  if (s1<se1) {
                    s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                    arg5 = compile(s1,s2,depth1,0,block_flags);
                    arg6 = s2<se1?compile(++s2,se1,depth1,0,block_flags):1;
                  }
                }
              }
              if (is_vector(arg1)) {
                if (!ref[0]) ++arg1;
                else if (ref[0]>=4 && arg4==~0U) arg4 = scalar1(mp_image_whd,ref[1]);
              }
              if (is_vector(arg2)) {
                if (arg3==~0U) arg3 = const_scalar(size(arg2));
                if (!ref[7]) ++arg2;
                if (ref[7]>=4 && arg5==~0U) arg5 = scalar1(mp_image_whd,ref[8]);
              }
              if (arg3==~0U) arg3 = 1;
              if (arg4==~0U) arg4 = 1;
              if (arg5==~0U) arg5 = 1;
              _cimg_mp_check_type(arg3,3,1,0);
              _cimg_mp_check_type(arg4,4,1,0);
              _cimg_mp_check_type(arg5,5,1,0);
              _cimg_mp_check_type(arg6,5,1,0);
              CImg<ulongT>(1,22).move_to(code);
              code.back().get_shared_rows(0,7).fill((ulongT)mp_mem_copy,p1,arg1,arg2,arg3,arg4,arg5,arg6);
              code.back().get_shared_rows(8,21).fill(ref);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"cos(",4)) { // Cosine
              _cimg_mp_op("Function 'cos()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(cos,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::cos(mem[arg1]));
              _cimg_mp_scalar1(cos,arg1);
            }

            if (!std::strncmp(ss,"cosh(",5)) { // Hyperbolic cosine
              _cimg_mp_op("Function 'cosh()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(cosh,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::cosh(mem[arg1]));
              _cimg_mp_scalar1(cosh,arg1);
            }

            if (!std::strncmp(ss,"cov(",4)) { // Covariance
              _cimg_mp_op("Function 'cov()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              arg3 = arg4 = ~0U;
              if (s2<se1) {
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg3 = compile(s2,s1,depth1,0,block_flags);
                arg4 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U;
              }
              _cimg_mp_check_type(arg2,2,is_scalar(arg1)?1:2,size(arg1));
              if (arg3!=~0U) _cimg_mp_check_type(arg3,3,1,0);
              if (arg4!=~0U) _cimg_mp_check_type(arg4,4,1,0);
              if (arg3==~0U && arg4==~0U && size(arg1)<2) _cimg_mp_const_scalar(0);
              _cimg_mp_scalar5(cov,arg1,arg2,size(arg1),arg3,arg4);
            }

            if (!std::strncmp(ss,"critical(",9)) { // Critical section (single thread at a time)
              _cimg_mp_op("Function 'critical()'");
              p1 = code._width;
              arg1 = compile(ss + 9,se1,depth1,p_ref,block_flags | 1);
              CImg<ulongT>::vector((ulongT)mp_critical,arg1,code._width - p1).move_to(code,p1);
              _cimg_mp_return(arg1);
            }

            if (!std::strncmp(ss,"crop(",5)) { // Image or vector crop
              _cimg_mp_op("Function 'crop()'");
              is_sth = false; // is image crop ?
              arg1 = 0;
              if (*ss5=='#') { // Index specified
                s0 = ss6; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss6,s0++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss6);
                _cimg_mp_check_list();
                pos = 2;
                is_sth = true;
              } else { p1 = ~0U; s0 = ss5; need_input_copy = true; pos = 1; }
              if (s0<se1) for (s = s0; s<se; ++s, ++pos) {
                  ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                                 (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                  arg1 = compile(s,ns,depth1,0,block_flags);
                  if (pos==1 && is_scalar(arg1)) is_sth = true;
                  else if (pos>1) _cimg_mp_check_type(arg1,pos,1,0);
                  CImg<ulongT>::vector(arg1).move_to(l_opcode);
                  s = ns;
                }
              (l_opcode>'y').move_to(opcode);

              if (!opcode || is_sth) { // Image crop
                arg1 = 0; arg2 = (p1!=~0U);
                switch (opcode._height) {
                case 0 : case 1 :
                  CImg<ulongT>::vector(0,0,0,0,~0U,~0U,~0U,~0U,0).move_to(opcode);
                  break;
                case 2 :
                  CImg<ulongT>::vector(*opcode,0,0,0,opcode[1],~0U,~0U,~0U,_cimg_mp_boundary).move_to(opcode);
                  arg1 = 2 + arg2; break;
                case 3 :
                  CImg<ulongT>::vector(*opcode,0,0,0,opcode[1],~0U,~0U,~0U,opcode[2]).move_to(opcode);
                  arg1 = 2 + arg2; break;
                case 4 :
                  CImg<ulongT>::vector(*opcode,opcode[1],0,0,opcode[2],opcode[3],~0U,~0U,_cimg_mp_boundary).
                    move_to(opcode);
                  arg1 = 3 + arg2; break;
                case 5 :
                  CImg<ulongT>::vector(*opcode,opcode[1],0,0,opcode[2],opcode[3],~0U,~0U,opcode[4]).
                    move_to(opcode);
                  arg1 = 3 + arg2; break;
                case 6 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],0,opcode[3],opcode[4],opcode[5],~0U,
                                       _cimg_mp_boundary).move_to(opcode);
                  arg1 = 4 + arg2; break;
                case 7 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],0,opcode[3],opcode[4],opcode[5],~0U,
                                       opcode[6]).move_to(opcode);
                  arg1 = 4 + arg2; break;
                case 8 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],opcode[3],opcode[4],opcode[5],opcode[6],
                                       opcode[7],_cimg_mp_boundary).move_to(opcode);
                  arg1 = 5 + arg2; break;
                case 9 :
                  arg1 = 5 + arg2; break;
                default : // Error -> too much arguments
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Too much arguments specified, "
                                              "in expression '%s'.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,s0);
                }

                if (opcode[4]!=(ulongT)~0U) {
                  _cimg_mp_check_const_scalar((unsigned int)opcode[4],arg1,3);
                  opcode[4] = (ulongT)mem[opcode[4]];
                }
                if (opcode[5]!=(ulongT)~0U) {
                  _cimg_mp_check_const_scalar((unsigned int)opcode[5],arg1 + 1,3);
                  opcode[5] = (ulongT)mem[opcode[5]];
                }
                if (opcode[6]!=(ulongT)~0U) {
                  _cimg_mp_check_const_scalar((unsigned int)opcode[6],arg1 + 2,3);
                  opcode[6] = (ulongT)mem[opcode[6]];
                }
                if (opcode[7]!=(ulongT)~0U) {
                  _cimg_mp_check_const_scalar((unsigned int)opcode[7],arg1 + 3,3);
                  opcode[7] = (ulongT)mem[opcode[7]];
                }
                _cimg_mp_check_type((unsigned int)opcode[8],arg1 + 4,1,0);

                if (opcode[4]==(ulongT)~0U || opcode[5]==(ulongT)~0U ||
                    opcode[6]==(ulongT)~0U || opcode[7]==(ulongT)~0U) {
                  p2 = 0;
                  if (p1!=~0U) {
                    _cimg_mp_check_const_scalar(p1,1,1);
                    p2 = (unsigned int)cimg::mod((int)mem[p1],imglist.width());
                  }
                  const CImg<T> &img = p1!=~0U?imglist[p2]:imgin;
                  if (!img) {
                    _cimg_mp_strerr;
                    throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                "CImg<%s>::%s: %s: Cannot crop empty image when "
                                                "some xyzc-coordinates are unspecified, in expression '%s'.",
                                                pixel_type(),_cimg_mp_calling_function,s_op,s0);
                  }
                  if (opcode[4]==(ulongT)~0U) opcode[4] = (ulongT)img._width;
                  if (opcode[5]==(ulongT)~0U) opcode[5] = (ulongT)img._height;
                  if (opcode[6]==(ulongT)~0U) opcode[6] = (ulongT)img._depth;
                  if (opcode[7]==(ulongT)~0U) opcode[7] = (ulongT)img._spectrum;
                }

                pos = vector((unsigned int)(opcode[4]*opcode[5]*opcode[6]*opcode[7]));
                CImg<ulongT>::vector((ulongT)mp_image_crop,
                                     pos,p1, // 1-2: res,#ind
                                     *opcode,opcode[1],opcode[2],opcode[3], // 3-6: x,y,z,c
                                     opcode[4],opcode[5],opcode[6],opcode[7], // 7-10: dx,dy,dz,dc
                                     opcode[8]).move_to(code); // 11: boundary conditions

              } else { // Vector crop
                switch (opcode._height) {
                case 5 : case 6 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],opcode[3],opcode[4],
                                       0,0,0,0,~0U,~0U,~0U,~0U,0).move_to(opcode);
                  break;
                case 7 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],opcode[3],opcode[4],
                                       opcode[5],0,0,0,opcode[6],~0U,~0U,~0U,_cimg_mp_boundary).move_to(opcode);
                  arg1 = 7; break;
                case 8 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],opcode[3],opcode[4],
                                       opcode[5],0,0,0,opcode[6],~0U,~0U,~0U,opcode[7]).move_to(opcode);
                  arg1 = 7; break;
                case 9 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],opcode[3],opcode[4],
                                       opcode[5],opcode[6],0,0,opcode[7],opcode[8],~0U,~0U,_cimg_mp_boundary).
                    move_to(opcode);
                  arg1 = 8; break;
                case 10 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],opcode[3],opcode[4],
                                       opcode[5],opcode[6],0,0,opcode[7],opcode[8],~0U,~0U,opcode[9]).
                    move_to(opcode);
                  arg1 = 8; break;
                case 11 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],opcode[3],opcode[4],
                                       opcode[5],opcode[6],opcode[7],0,opcode[8],opcode[9],opcode[10],~0U,
                                       _cimg_mp_boundary).move_to(opcode);
                  arg1 = 9; break;
                case 12 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],opcode[3],opcode[4],
                                       opcode[5],opcode[6],opcode[7],0,opcode[8],opcode[9],opcode[10],~0U,
                                       opcode[11]).move_to(opcode);
                  arg1 = 9; break;
                case 13 :
                  CImg<ulongT>::vector(*opcode,opcode[1],opcode[2],opcode[3],opcode[4],
                                       opcode[5],opcode[6],opcode[7],opcode[8],opcode[9],opcode[10],opcode[11],
                                       opcode[12],_cimg_mp_boundary).move_to(opcode);
                  arg1 = 10; break;
                case 14 :
                  arg1 = 10; break;
                default : // Error -> too few or too much arguments
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Too %s arguments specified, "
                                              "in expression '%s'.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,
                                              opcode._height<5?"few":"much",s0);
                }

                _cimg_mp_check_const_scalar((unsigned int)opcode[1],2,3); // w
                opcode[1] = (ulongT)mem[opcode[1]];
                _cimg_mp_check_const_scalar((unsigned int)opcode[2],3,3); // h
                opcode[2] = (ulongT)mem[opcode[2]];
                _cimg_mp_check_const_scalar((unsigned int)opcode[3],4,3); // d
                opcode[3] = (ulongT)mem[opcode[3]];
                _cimg_mp_check_const_scalar((unsigned int)opcode[4],5,3); // s
                opcode[4] = (ulongT)mem[opcode[4]];
                p1 = size((unsigned int)opcode[0]);
                arg2 = (unsigned int)opcode[1];
                arg3 = (unsigned int)opcode[2];
                arg4 = (unsigned int)opcode[3];
                arg5 = (unsigned int)opcode[4];
                if (arg2*arg3*arg4*arg5!=p1) {
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Input vector size (%u values) and its specified "
                                              "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,
                                              p1,arg2,arg3,arg4,arg5,arg2*arg3*arg4*arg5);
                }
                if (opcode[9]!=(ulongT)~0U) {
                  _cimg_mp_check_const_scalar((unsigned int)opcode[9],arg1,3);
                  opcode[9] = (ulongT)mem[opcode[9]];
                } else opcode[9] = opcode[1];
                if (opcode[10]!=(ulongT)~0U) {
                  _cimg_mp_check_const_scalar((unsigned int)opcode[10],arg1 + 1,3);
                  opcode[10] = (ulongT)mem[opcode[10]];
                } else opcode[10] = opcode[2];
                if (opcode[11]!=(ulongT)~0U) {
                  _cimg_mp_check_const_scalar((unsigned int)opcode[11],arg1 + 2,3);
                  opcode[11] = (ulongT)mem[opcode[11]];
                } else opcode[11] = opcode[3];
                if (opcode[12]!=(ulongT)~0U) {
                  _cimg_mp_check_const_scalar((unsigned int)opcode[12],arg1 + 3,3);
                  opcode[12] = (ulongT)mem[opcode[12]];
                } else opcode[12] = opcode[4];
                _cimg_mp_check_type((unsigned int)opcode[13],arg1 + 4,1,0);

                pos = vector((unsigned int)(opcode[9]*opcode[10]*opcode[11]*opcode[12]));
                CImg<ulongT>::vector((ulongT)mp_vector_crop_ext,
                                     pos,*opcode, // 1-2: res,S
                                     opcode[1],opcode[2],opcode[3],opcode[4], // 3-6: w,h,d,s
                                     opcode[5],opcode[6],opcode[7],opcode[8], // 7-10: x,y,z,c
                                     opcode[9],opcode[10],opcode[11],opcode[12], // 11-14: dx,dy,dz,dc
                                     opcode[13]).move_to(code); // 15: boundary conditions
              }
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"cross(",6)) { // Cross product
              _cimg_mp_op("Function 'cross()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags);
              arg2 = compile(++s1,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,2,3);
              _cimg_mp_check_type(arg2,2,2,3);
              pos = vector(3);
              CImg<ulongT>::vector((ulongT)mp_cross,pos,arg1,arg2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"csin(",5)) { // Complex sine
              _cimg_mp_op("Function 'csin()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_sin,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_sin,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"csinh(",6)) { // Complex hyperbolic sine
              _cimg_mp_op("Function 'csinh()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_sinh,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_sinh,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"csqr(",5)) { // Complex square
              _cimg_mp_op("Function 'csqr()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_sqr,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_sqr,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"csqrt(",6)) { // Complex square root
              _cimg_mp_op("Function 'csqrt()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_sqrt,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_sqrt,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"ctan(",5)) { // Complex tangent
              _cimg_mp_op("Function 'ctan()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_tan,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_tan,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"ctanh(",6)) { // Complex hyperbolic tangent
              _cimg_mp_op("Function 'ctanh()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,0,3,2);
              pos = vector(2);
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_tanh,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_tanh,pos,arg1 + 1,arg1 + 2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"cumulate(",9)) { // Cumulate
              _cimg_mp_op("Function 'cumulate()'");
              s0 = ss + 9;
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(s0,s1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,2,0);
              p1 = size(arg1);
              arg2 = p1;
              arg3 = arg4 = arg5 = 1;
              arg6 = p2 = ~0U;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg2 = compile(s1,s2,depth1,0,block_flags);
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg3 = compile(s2,s1,depth1,0,block_flags);
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg4 = compile(s1,s2,depth1,0,block_flags);
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg5 = compile(s2,s1,depth1,0,block_flags);
                arg6 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U;
                _cimg_mp_check_const_scalar(arg2,2,3);
                _cimg_mp_check_const_scalar(arg3,3,3);
                _cimg_mp_check_const_scalar(arg4,4,3);
                _cimg_mp_check_const_scalar(arg5,5,3);
                arg2 = (unsigned int)mem[arg2];
                arg3 = (unsigned int)mem[arg3];
                arg4 = (unsigned int)mem[arg4];
                arg5 = (unsigned int)mem[arg5];
                p2 = arg6!=~0U?size(arg6):~0U;
              }
              if (arg2*arg3*arg4*arg5!=std::max(1U,p1)) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Input vector size (%u values) and its specified "
                                            "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            std::max(p1,1U),arg2,arg3,arg4,arg5,arg2*arg3*arg4*arg5);
              }
              pos = vector(arg2*arg3*arg4*arg5);
              CImg<ulongT>::vector((ulongT)mp_cumulate,pos,arg1,arg2,arg3,arg4,arg5,arg6,p2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"cut(",4)) { // Cut
              _cimg_mp_op("Function 'cut()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):const_scalar(cimg::type<double>::inf());
              _cimg_mp_check_type(arg2,2,1,0);
              _cimg_mp_check_type(arg3,3,1,0);
              if (is_vector(arg1)) _cimg_mp_vector3_vss(cut,arg1,arg2,arg3);
              if (is_const_scalar(arg1) && is_const_scalar(arg2) && is_const_scalar(arg3)) {
                val = mem[arg1];
                val1 = mem[arg2];
                val2 = mem[arg3];
                _cimg_mp_const_scalar(cimg::cut(val,val1,val2));
              }
              _cimg_mp_scalar3(cut,arg1,arg2,arg3);
            }
            break;

          case 'd' :
            if (*ss1=='(') { // Image depth
              _cimg_mp_op("Function 'd()'");
              if (*ss2=='#') { // Index specified
                p1 = compile(ss3,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss3);
                _cimg_mp_check_list();
              } else { if (ss2!=se1) break; p1 = ~0U; }
              _cimg_mp_scalar1(image_depth,p1);
            }

            if (!std::strncmp(ss,"da_back(",8) ||
                !std::strncmp(ss,"da_pop(",7) ||
                !std::strncmp(ss,"da_pop_heap(",12)) { // Get latest element in a dynamic array
              if (!is_inside_critical) is_parallelizable = false;
              const bool is_pop = *ss3=='p', is_pop_heap = *ss6=='_';
              _cimg_mp_op(is_pop_heap?"Function 'da_pop_heap()":
                          is_pop?"Function 'da_pop()'":"Function 'da_back()'");
              s0 = ss + (is_pop_heap?12:is_pop?7:8);
              if (*s0=='#') { // Index specified
                s1 = ++s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                p1 = compile(s0,s1++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,s0);
              } else { p1 = 11; s1 = s0; }
              _cimg_mp_check_list();
              _cimg_mp_check_const_scalar(p1,1,1);
              p3 = (unsigned int)cimg::mod((int)mem[p1],imglist.width());
              p2 = imglist[p3]._spectrum;
              if (p2>1) pos = vector(p2); else pos = scalar(); // Return vector or scalar result
              CImg<ulongT>::vector((ulongT)mp_da_back_or_pop,pos,p2,p1,is_pop_heap?2:is_pop?1:0).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"da_freeze(",10)) { // Freeze dynamic array
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_op("Function 'da_freeze()'");
              s0 = ss + 10;
              if (*s0=='#') { // Index specified
                s1 = ++s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                p1 = compile(s0,s1++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,s0);
              } else { p1 = 11; s1 = s0; }
              _cimg_mp_check_list();
              CImg<ulongT>::vector((ulongT)mp_da_freeze,_cimg_mp_slot_nan,p1).move_to(code);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"da_insert(",10) ||
                !std::strncmp(ss,"da_push(",8) ||
                !std::strncmp(ss,"da_push_heap(",13)) { // Insert element(s) in a dynamic array
              if (!is_inside_critical) is_parallelizable = false;
              const bool is_push = *ss3=='p', is_push_heap = *ss7=='_';
              _cimg_mp_op(is_push_heap?"Function 'da_push_heap()'":
                          is_push?"Function 'da_push()'":"Function 'da_insert()'");
              s0 = ss + (is_push_heap?13:is_push?8:10);
              if (*s0=='#') { // Index specified
                s1 = ++s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                p1 = compile(s0,s1++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,s0);
              } else { p1 = 11; s1 = s0; }
              _cimg_mp_check_list();
              if (!is_push) {
                s0 = s1; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg1 = compile(s0,s1++,depth1,0,block_flags); // Position
              } else if (is_push_heap) arg1 = ~0U - 1;
              else arg1 = ~0U;
              CImg<ulongT>::vector((ulongT)mp_da_insert_or_push,_cimg_mp_slot_nan,p1,arg1,0,0).move_to(l_opcode);
              p3 = p1==~0U?2:3;
              p1 = ~0U;
              for (s = s1; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg2 = compile(s,ns,depth1,0,block_flags); // Element
                p2 = size(arg2);
                if (p1==~0U) p1 = p2;
                else {
                  if (!p1) _cimg_mp_check_type(arg2,p3,1,0);
                  else _cimg_mp_check_type(arg2,p3,2,p1);
                }
                CImg<ulongT>::vector(arg2).move_to(l_opcode);
                s = ns;
                ++p3;
              }
              if (p1==~0U) compile(++s1,se1,depth1,0,block_flags); // Missing element -> error
              (l_opcode>'y').move_to(opcode);
              opcode[4] = p1;
              opcode[5] = opcode._height;
              opcode.move_to(code);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"da_remove(",10)) { // Remove element(s) in a dynamic array
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_op("Function 'da_remove()'");
              if (ss[10]=='#') { // Index specified
                s0 = ss + 11; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss + 11,s0++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss + 11);
              } else { p1 = 11; s0 = ss + 10; }
              _cimg_mp_check_list();

              arg1 = arg2 = ~0U;
              if (s0<se1) {
                s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg1 = compile(s0,s1,depth1,0,block_flags); // Starting position
                arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U; // Ending position
              }
              CImg<ulongT>::vector((ulongT)mp_da_remove,_cimg_mp_slot_nan,p1,arg1,arg2).move_to(code);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"da_size(",8)) { // Size of a dynamic array
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_op("Function 'da_size()'");
              if (ss[8]=='#') { // Index specified
                s0 = ss + 9; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss + 9,s0++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss + 9);
              } else { p1 = 11; s0 = ss + 8; }
              _cimg_mp_check_list();
              _cimg_mp_scalar1(da_size,p1);
            }

            if (!std::strncmp(ss,"date(",5)) { // Current date or file date
              _cimg_mp_op("Function 'date()'");
              s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = ss5!=se1?compile(ss5,s1,depth1,0,block_flags):~0U;
              arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U;
              if (arg2!=~0U) _cimg_mp_check_type(arg2,1,2,0);
              pos = arg1==~0U || is_vector(arg1)?vector(arg1==~0U?7:size(arg1)):scalar();
              CImg<ulongT>::vector((ulongT)mp_date,pos,size(pos),
                                   arg1,arg1==~0U?~0U:size(arg1),
                                   arg2,arg2==~0U?~0U:size(arg2)).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"debug(",6)) { // Print debug info
              _cimg_mp_op("Function 'debug()'");
              p1 = code._width;
              arg1 = compile(ss6,se1,depth1,p_ref,block_flags);
              *se1 = 0;
              variable_name.assign(CImg<charT>::string(ss6,true,true).unroll('y'),true);
              cimg::strpare(variable_name,false,true);
              ((CImg<ulongT>::vector((ulongT)mp_debug,arg1,0,code._width - p1),
                variable_name)>'y').move_to(opcode);
              opcode[2] = opcode._height;
              opcode.move_to(code,p1);
              *se1 = ')';
              _cimg_mp_return(arg1);
            }

            if (!std::strncmp(ss,"deg2rad(",8)) { // Degrees to radians
              _cimg_mp_op("Function 'deg2rad()'");
              arg1 = compile(ss8,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(deg2rad,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(mem[arg1]*cimg::PI/180);
              _cimg_mp_scalar1(deg2rad,arg1);
            }

            if (!std::strncmp(ss,"det(",4)) { // Matrix determinant
              _cimg_mp_op("Function 'det()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              _cimg_mp_check_matrix_square(arg1,1);
              p1 = (unsigned int)cimg::round(std::sqrt((float)size(arg1)));
              _cimg_mp_scalar2(det,arg1,p1);
            }

            if (!std::strncmp(ss,"diag(",5)) { // Diagonal matrix
              _cimg_mp_op("Function 'diag()'");
              CImg<ulongT>::vector((ulongT)mp_diag,0,0).move_to(l_opcode);
              for (s = ss5; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg2 = compile(s,ns,depth1,0,block_flags);
                if (is_vector(arg2))
                  CImg<ulongT>::sequence(size(arg2),arg2 + 1,
                                         arg2 + (ulongT)size(arg2)).
                    move_to(l_opcode);
                else CImg<ulongT>::vector(arg2).move_to(l_opcode);
                s = ns;
              }
              (l_opcode>'y').move_to(opcode);
              arg1 = opcode._height - 3;
              pos = vector(arg1*arg1);
              opcode[1] = pos;
              opcode[2] = opcode._height;
              opcode.move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"display(",8)) { // Display memory, vector or image
              _cimg_mp_op("Function 'display()'");
              if (pexpr[se2 - expr._data]=='(') { // no arguments?
                CImg<ulongT>::vector((ulongT)mp_mem_display,_cimg_mp_slot_nan).move_to(code);
                _cimg_mp_return_nan();
              }
              if (*ss8!='#') { // Vector
                s1 = ss8; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg1 = compile(ss8,s1,depth1,0,block_flags);
                arg2 = 0; arg3 = arg4 = arg5 = 1;
                if (s1<se1) {
                  s2 = s1 + 1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg2 = compile(s1 + 1,s2,depth1,0,block_flags);
                  if (s2<se1) {
                    s3 = ++s2; while (s3<se1 && (*s3!=',' || level[s3 - expr._data]!=clevel1)) ++s3;
                    arg3 = compile(s2,s3,depth1,0,block_flags);
                    if (s3<se1) {
                      s2 = ++s3; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                      arg4 = compile(s3,s2,depth1,0,block_flags);
                      arg5 = s2<se1?compile(++s2,se1,depth1,0,block_flags):1;
                    }
                  }
                }
                _cimg_mp_check_type(arg2,2,1,0);
                _cimg_mp_check_type(arg3,3,1,0);
                _cimg_mp_check_type(arg4,4,1,0);
                _cimg_mp_check_type(arg5,5,1,0);

                c1 = *s1; *s1 = 0;
                variable_name.assign(CImg<charT>::string(ss8,true,true).unroll('y'),true);
                cimg::strpare(variable_name,false,true);
                if (is_vector(arg1))
                  ((CImg<ulongT>::vector((ulongT)mp_vector_print,arg1,0,(ulongT)size(arg1),0),
                    variable_name)>'y').move_to(opcode);
                else
                  ((CImg<ulongT>::vector((ulongT)mp_print,arg1,0,0),
                    variable_name)>'y').move_to(opcode);
                opcode[2] = opcode._height;
                opcode.move_to(code);

                ((CImg<ulongT>::vector((ulongT)mp_vector_display,arg1,0,(ulongT)size(arg1),
                                       arg2,arg3,arg4,arg5),
                  variable_name)>'y').move_to(opcode);
                opcode[2] = opcode._height;
                opcode.move_to(code);
                *s1 = c1;
                _cimg_mp_return(arg1);

              } else { // Image
                p1 = compile(ss8 + 1,se1,depth1,0,block_flags);
                _cimg_mp_check_list();
                CImg<ulongT>::vector((ulongT)mp_image_display,_cimg_mp_slot_nan,p1).move_to(code);
                _cimg_mp_return_nan();
              }
            }

            if (!std::strncmp(ss,"do(",3)) { // Do..while
              _cimg_mp_op("Function 'do()'");
              s0 = *ss2=='('?ss3:ss8;
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = code._width;
              arg6 = mempos;
              p1 = compile(s0,s1,depth1,0,block_flags); // Body
              arg2 = code._width;
              p2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):1; // Condition
              _cimg_mp_check_type(p2,2,1,0);
              CImg<ulongT>::vector((ulongT)mp_do,p1,p2,arg2 - arg1,code._width - arg2,size(p1),
                                   p1>=arg6 && !is_const_scalar(p1),
                                   p2>=arg6 && !is_const_scalar(p2)).move_to(code,arg1);
              _cimg_mp_return(p1);
            }

            if (!std::strncmp(ss,"dot(",4)) { // Dot product
              _cimg_mp_op("Function 'dot()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              arg2 = compile(++s1,se1,depth1,0,block_flags);
              if (is_vector(arg1)) {
                _cimg_mp_check_type(arg2,2,2,size(arg1));
                _cimg_mp_scalar3(dot,arg1,arg2,size(arg1));
              }
              _cimg_mp_check_type(arg2,2,1,0);
              _cimg_mp_scalar2(mul,arg1,arg2);
            }

            if (!std::strncmp(ss,"draw(",5)) { // Draw image
              _cimg_mp_op("Function 'draw()'");
              if (*ss5=='#') { // Index specified
                s0 = ss6; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss6,s0++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss6);
                _cimg_mp_check_list();
              } else { p1 = ~0U; s0 = ss5; }

              for (s = s0; s<se; ++s, ++pos) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg1 = compile(s,ns,depth1,0,block_flags);
                CImg<ulongT>::vector(arg1).move_to(l_opcode);
                s = ns;
              }
              (l_opcode>'y').move_to(opcode);

              is_sth = p1==~0U && opcode._height>5 && is_vector((unsigned int)opcode[5]); // Is vector drawing?
              if ((is_sth && (opcode._height<6 || opcode._height>17)) ||
                  (!is_sth && (opcode._height<1 || opcode._height>12))) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Too %s arguments specified, "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            opcode._height>12?"much":"few",s0);
              }

              if (is_sth) { // Drawing in a vector
                _cimg_mp_check_type((unsigned int)*opcode,1,2,0); // D
                _cimg_mp_check_type((unsigned int)opcode[1],2,1,0); // w
                _cimg_mp_check_type((unsigned int)opcode[2],3,1,0); // h
                _cimg_mp_check_type((unsigned int)opcode[3],4,1,0); // d
                _cimg_mp_check_type((unsigned int)opcode[4],5,1,0); // s

                if (opcode._height<8 || (opcode._height<10 && is_vector((unsigned int)opcode[7]))) {
                  // D,w,h,d,s,S[,opac,M,maxM].
                  if (opcode._height>6) _cimg_mp_check_type((unsigned int)opcode[6],7,1,0); // opac
                  if (opcode._height>8) _cimg_mp_check_type((unsigned int)opcode[8],9,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_vector_draw,
                                       *opcode,size((unsigned int)*opcode), // 1-2: D,sizD
                                       opcode[1],opcode[2],opcode[3],opcode[4], // 3-6: w,h,d,s
                                       opcode[5],size((unsigned int)opcode[5]), // 7-8: S,sizS
                                       0,0,0,0, // 9-12: x,y,z,c
                                       ~0U,~0U,~0U,~0U, // 13-16: dx,dy,dz,dc
                                       opcode._height<7?1:opcode[6], // 17: opac
                                       opcode._height<8?~0U:opcode[7], // 18: M
                                       opcode._height<8?0:size((unsigned int)opcode[7]), // 19: sizM
                                       opcode._height<9?1:opcode[8]).move_to(code); // 20: maxM
                } else if (opcode._height<10 || (opcode._height<12 && is_vector((unsigned int)opcode[9]))) {
                  // D,w,h,d,s,S,x,dx[,opac,M,maxM].
                  _cimg_mp_check_type((unsigned int)opcode[6],7,1,0); // x
                  _cimg_mp_check_type((unsigned int)opcode[7],8,1,0); // dx
                  if (opcode._height>8) _cimg_mp_check_type((unsigned int)opcode[8],9,1,0); // opac
                  if (opcode._height>10) _cimg_mp_check_type((unsigned int)opcode[10],11,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_vector_draw,
                                       *opcode,size((unsigned int)*opcode), // 1-2: D,sizD
                                       opcode[1],opcode[2],opcode[3],opcode[4], // 3-6: w,h,d,s
                                       opcode[5],size((unsigned int)opcode[5]), // 7-8: S,sizS
                                       opcode[6],0,0,0, // 9-12: x,y,z,c
                                       opcode[7],~0U,~0U,~0U, // 13-16: dx,dy,dz,dc
                                       opcode._height<9?1:opcode[8], // 17: opac
                                       opcode._height<10?~0U:opcode[9], // 18: M
                                       opcode._height<10?0:size((unsigned int)opcode[9]), // 19: sizM
                                       opcode._height<11?1:opcode[10]).move_to(code); // 20: maxM
                } else if (opcode._height<12 || (opcode._height<14 && is_vector((unsigned int)opcode[11]))) {
                  // D,w,h,d,s,S,x,y,dx,dy[,opac,M,maxM].
                  _cimg_mp_check_type((unsigned int)opcode[6],7,1,0); // x
                  _cimg_mp_check_type((unsigned int)opcode[7],8,1,0); // y
                  _cimg_mp_check_type((unsigned int)opcode[8],9,1,0); // dx
                  _cimg_mp_check_type((unsigned int)opcode[9],10,1,0); // dy
                  if (opcode._height>10) _cimg_mp_check_type((unsigned int)opcode[10],11,1,0); // opac
                  if (opcode._height>12) _cimg_mp_check_type((unsigned int)opcode[12],13,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_vector_draw,
                                       *opcode,size((unsigned int)*opcode), // 1-2: D,sizD
                                       opcode[1],opcode[2],opcode[3],opcode[4], // 3-6: w,h,d,s
                                       opcode[5],size((unsigned int)opcode[5]), // 7-8: S,sizS
                                       opcode[6],opcode[7],0,0, // 9-12: x,y,z,c
                                       opcode[8],opcode[9],~0U,~0U, // 13-16: dx,dy,dz,dc
                                       opcode._height<11?1:opcode[10], // 17: opac
                                       opcode._height<12?~0U:opcode[11], // 18: M
                                       opcode._height<12?0:size((unsigned int)opcode[11]), // 19: sizM
                                       opcode._height<13?1:opcode[12]).move_to(code); // 20: maxM
                } else if (opcode._height<14 || (opcode._height<16 && is_vector((unsigned int)opcode[13]))) {
                  // D,w,h,d,s,S,x,y,z,dx,dy,dz[,opac,M,maxM].
                  _cimg_mp_check_type((unsigned int)opcode[6],7,1,0); // x
                  _cimg_mp_check_type((unsigned int)opcode[7],8,1,0); // y
                  _cimg_mp_check_type((unsigned int)opcode[8],9,1,0); // z
                  _cimg_mp_check_type((unsigned int)opcode[9],10,1,0); // dx
                  _cimg_mp_check_type((unsigned int)opcode[10],11,1,0); // dy
                  _cimg_mp_check_type((unsigned int)opcode[11],12,1,0); // dz
                  if (opcode._height>12) _cimg_mp_check_type((unsigned int)opcode[12],13,1,0); // opac
                  if (opcode._height>14) _cimg_mp_check_type((unsigned int)opcode[14],15,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_vector_draw,
                                       *opcode,size((unsigned int)*opcode), // 1-2: D,sizD
                                       opcode[1],opcode[2],opcode[3],opcode[4], // 3-6: w,h,d,s
                                       opcode[5],size((unsigned int)opcode[5]), // 7-8: S,sizS
                                       opcode[6],opcode[7],opcode[8],0, // 9-12: x,y,z,c
                                       opcode[9],opcode[10],opcode[11],~0U, // 13-16: dx,dy,dz,dc
                                       opcode._height<13?1:opcode[12], // 17: opac
                                       opcode._height<14?~0U:opcode[13], // 18: M
                                       opcode._height<14?0:size((unsigned int)opcode[13]), // 19: sizM
                                       opcode._height<15?1:opcode[14]).move_to(code); // 20: maxM
                } else if (opcode._height<16 || (opcode._height<18 && is_vector((unsigned int)opcode[15]))) {
                  // D,w,h,d,s,S,x,y,z,c,dx,dy,dz,dc[,opac,M,maxM].
                  _cimg_mp_check_type((unsigned int)opcode[6],7,1,0); // x
                  _cimg_mp_check_type((unsigned int)opcode[7],8,1,0); // y
                  _cimg_mp_check_type((unsigned int)opcode[8],9,1,0); // z
                  _cimg_mp_check_type((unsigned int)opcode[9],10,1,0); // c
                  _cimg_mp_check_type((unsigned int)opcode[10],11,1,0); // dx
                  _cimg_mp_check_type((unsigned int)opcode[11],12,1,0); // dy
                  _cimg_mp_check_type((unsigned int)opcode[12],13,1,0); // dz
                  _cimg_mp_check_type((unsigned int)opcode[13],14,1,0); // dc
                  if (opcode._height>14) _cimg_mp_check_type((unsigned int)opcode[14],15,1,0); // opac
                  if (opcode._height>16) _cimg_mp_check_type((unsigned int)opcode[16],17,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_vector_draw,
                                       *opcode,size((unsigned int)*opcode), // 1-2: D,sizD
                                       opcode[1],opcode[2],opcode[3],opcode[4], // 3-6: w,h,d,s
                                       opcode[5],size((unsigned int)opcode[5]), // 7-8: S,sizS
                                       opcode[6],opcode[7],opcode[8],opcode[9], // 9-12: x,y,z,c
                                       opcode[10],opcode[11],opcode[12],opcode[13], // 13-16: dx,dy,dz,dc
                                       opcode._height<15?1:opcode[14], // 17: opac
                                       opcode._height<16?~0U:opcode[15], // 18: M
                                       opcode._height<16?0:size((unsigned int)opcode[15]), // 19: sizM
                                       opcode._height<17?1:opcode[16]).move_to(code); // 20: maxM
                } else {
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Invalid types in specified arguments, "
                                              "in expression '%s'.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,s0);
                }

              } else { // Drawing in an image
                if (!is_inside_critical) is_parallelizable = false;
                arg1 = p1!=~0U;
                _cimg_mp_check_type((unsigned int)*opcode,1 + arg1,2,0); // S
                if (opcode._height<3 || (opcode._height<5 && is_vector((unsigned int)opcode[2]))) {
                  // S[,opac,M,maxM].
                  if (opcode._height>1) _cimg_mp_check_type((unsigned int)opcode[1],2 + arg1,1,0); // opac
                  if (opcode._height>3) _cimg_mp_check_type((unsigned int)opcode[3],4 + arg1,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_image_draw,
                                       *opcode,size((unsigned int)*opcode),p1, // 1-3: S,sizS,#ind
                                       0,0,0,0, // 4-7: x,y,z,c
                                       ~0U,~0U,~0U,~0U, // 8-11: dx,dy,dz,dc
                                       opcode._height<2?1:opcode[1], // 12: opac
                                       opcode._height<3?~0U:opcode[2], // 13: M
                                       opcode._height<3?0:size((unsigned int)opcode[2]), // 14: sizM
                                       opcode._height<4?1:opcode[3]).move_to(code); // 15: maxM
                } else if (opcode._height<5 || (opcode._height<7 && is_vector((unsigned int)opcode[4]))) {
                  // x,dx,S[,opac,M,maxM].
                  _cimg_mp_check_type((unsigned int)opcode[1],2 + arg1,1,0); // x
                  _cimg_mp_check_type((unsigned int)opcode[2],3 + arg1,1,0); // dx
                  if (opcode._height>3) _cimg_mp_check_type((unsigned int)opcode[3],4 + arg1,1,0); // opac
                  if (opcode._height>5) _cimg_mp_check_type((unsigned int)opcode[5],6 + arg1,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_image_draw,
                                       *opcode,size((unsigned int)*opcode),p1, // 1-3: S,sizS,#ind
                                       opcode[1],0,0,0, // 4-7: x,y,z,c
                                       opcode[2],~0U,~0U,~0U, // 8-11: dx,dy,dz,dc
                                       opcode._height<4?1:opcode[3], // 12: opac
                                       opcode._height<5?~0U:opcode[4], // 13: M
                                       opcode._height<5?0:size((unsigned int)opcode[4]), // 14: sizM
                                       opcode._height<6?1:opcode[5]).move_to(code); // 15: maxM
                } else if (opcode._height<7 || (opcode._height<9 && is_vector((unsigned int)opcode[6]))) {
                  // x,y,dx,dy,S[,opac,M,maxM].
                  _cimg_mp_check_type((unsigned int)opcode[1],2 + arg1,1,0); // x
                  _cimg_mp_check_type((unsigned int)opcode[2],3 + arg1,1,0); // y
                  _cimg_mp_check_type((unsigned int)opcode[3],4 + arg1,1,0); // dx
                  _cimg_mp_check_type((unsigned int)opcode[4],5 + arg1,1,0); // dy
                  if (opcode._height>5) _cimg_mp_check_type((unsigned int)opcode[5],6 + arg1,1,0); // opac
                  if (opcode._height>7) _cimg_mp_check_type((unsigned int)opcode[7],8 + arg1,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_image_draw,
                                       *opcode,size((unsigned int)*opcode),p1, // 1-3: S,sizS,#ind
                                       opcode[1],opcode[2],0,0, // 4-7: x,y,z,c
                                       opcode[3],opcode[4],~0U,~0U, // 8-11: dx,dy,dz,dc
                                       opcode._height<6?1:opcode[5], // 12: opac
                                       opcode._height<7?~0U:opcode[6], // 13: M
                                       opcode._height<7?0:size((unsigned int)opcode[6]), // 14: sizM
                                       opcode._height<8?1:opcode[7]).move_to(code); // 15: maxM
                } else if (opcode._height<9 || (opcode._height<11 && is_vector((unsigned int)opcode[8]))) {
                  // x,y,z,dx,dy,dz,S[,opac,M,maxM].
                  _cimg_mp_check_type((unsigned int)opcode[1],2 + arg1,1,0); // x
                  _cimg_mp_check_type((unsigned int)opcode[2],3 + arg1,1,0); // y
                  _cimg_mp_check_type((unsigned int)opcode[3],4 + arg1,1,0); // z
                  _cimg_mp_check_type((unsigned int)opcode[4],5 + arg1,1,0); // dx
                  _cimg_mp_check_type((unsigned int)opcode[5],6 + arg1,1,0); // dy
                  _cimg_mp_check_type((unsigned int)opcode[6],7 + arg1,1,0); // dz
                  if (opcode._height>7) _cimg_mp_check_type((unsigned int)opcode[7],8 + arg1,1,0); // opac
                  if (opcode._height>9) _cimg_mp_check_type((unsigned int)opcode[9],10 + arg1,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_image_draw,
                                       *opcode,size((unsigned int)*opcode),p1, // 1-3: S,sizS,#ind
                                       opcode[1],opcode[2],opcode[3],0, // 4-7: x,y,z,c
                                       opcode[4],opcode[5],opcode[6],~0U, // 8-11: dx,dy,dz,dc
                                       opcode._height<8?1:opcode[7], // 12: opac
                                       opcode._height<9?~0U:opcode[8], // 13: M
                                       opcode._height<9?0:size((unsigned int)opcode[8]), // 14: sizM
                                       opcode._height<10?1:opcode[9]).move_to(code); // 15: maxM
#endif
