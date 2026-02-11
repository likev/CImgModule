#ifndef CIMG_MODULE_MATH_PARSER_COMPILE_3_H
#define CIMG_MODULE_MATH_PARSER_COMPILE_3_H

                } else if (opcode._height<11 || (opcode._height<13 && is_vector((unsigned int)opcode[10]))) {
                  // x,y,z,c,dx,dy,dz,dc,S[,opac,M,maxM].
                  _cimg_mp_check_type((unsigned int)opcode[1],2 + arg1,1,0); // x
                  _cimg_mp_check_type((unsigned int)opcode[2],3 + arg1,1,0); // y
                  _cimg_mp_check_type((unsigned int)opcode[3],4 + arg1,1,0); // z
                  _cimg_mp_check_type((unsigned int)opcode[4],5 + arg1,1,0); // c
                  _cimg_mp_check_type((unsigned int)opcode[5],6 + arg1,1,0); // dx
                  _cimg_mp_check_type((unsigned int)opcode[6],7 + arg1,1,0); // dy
                  _cimg_mp_check_type((unsigned int)opcode[7],8 + arg1,1,0); // dz
                  _cimg_mp_check_type((unsigned int)opcode[8],9 + arg1,1,0); // dc
                  if (opcode._height>9) _cimg_mp_check_type((unsigned int)opcode[9],10 + arg1,1,0); // opac
                  if (opcode._height>11) _cimg_mp_check_type((unsigned int)opcode[11],12 + arg1,1,0); // maxM
                  CImg<ulongT>::vector((ulongT)mp_image_draw,
                                       *opcode,size((unsigned int)*opcode),p1, // 1-3: S,sizS,#ind
                                       opcode[1],opcode[2],opcode[3],opcode[4], // 4-7: x,y,z,c
                                       opcode[5],opcode[6],opcode[7],opcode[8], // 8-11: dx,dy,dz,dc
                                       opcode._height<10?1:opcode[9], // 12: opac
                                       opcode._height<11?~0U:opcode[10], // 13: M
                                       opcode._height<11?0:size((unsigned int)opcode[10]), // 14: sizM
                                       opcode._height<12?1:opcode[11]).move_to(code); // 15: maxM
                } else {
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Invalid types in specified arguments, "
                                              "in expression '%s'.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,s0);
                }
              }
              _cimg_mp_return_nan();
            }
            break;

          case 'e' :
            if (!std::strncmp(ss,"echo(",5)) { // Echo
              _cimg_mp_op("Function 'echo()'");
              CImg<ulongT>::vector((ulongT)mp_echo,_cimg_mp_slot_nan,0).move_to(l_opcode);
              for (s = ss5; s<se1; ++s) {
                ns = s; while (ns<se1 && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg1 = compile(s,ns,depth1,0,block_flags);
                CImg<ulongT>::vector(arg1,size(arg1)).move_to(l_opcode);
                s = ns;
              }
              (l_opcode>'y').move_to(opcode);
              opcode[2] = opcode._height;
              opcode.move_to(code);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"eig(",4)) { // Matrix eigenvalues/eigenvector
              _cimg_mp_op("Function 'eig()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              _cimg_mp_check_matrix_square(arg1,1);
              p1 = (unsigned int)cimg::round(std::sqrt((float)size(arg1)));
              pos = vector((p1 + 1)*p1);
              CImg<ulongT>::vector((ulongT)mp_matrix_eigen,pos,arg1,p1).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"ellipse(",8)) { // Ellipse/circle drawing
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_op("Function 'ellipse()'");
              if (*ss8=='#') { // Index specified
                s0 = ss + 9; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss + 9,s0++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss + 9);
                _cimg_mp_check_list();
              } else { p1 = ~0U; s0 = ss8; }
              if (s0==se1) compile(s0,se1,depth1,0,block_flags); // 'missing' argument error
              CImg<ulongT>::vector((ulongT)mp_ellipse,_cimg_mp_slot_nan,0,p1).move_to(l_opcode);
              for (s = s0; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg2 = compile(s,ns,depth1,0,block_flags);
                if (is_vector(arg2)) // Vector argument allowed to specify cordinates and color
                  CImg<ulongT>::sequence(size(arg2),arg2 + 1,arg2 + (ulongT)size(arg2)).move_to(l_opcode);
                else
                  CImg<ulongT>::vector(arg2).move_to(l_opcode);
                s = ns;
              }
              (l_opcode>'y').move_to(opcode);
              opcode[2] = opcode._height;
              opcode.move_to(code);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"end(",4)) { // End
              _cimg_mp_op("Function 'end()'");
              s1 = ss4; while (s1<se1 && cimg::is_blank(*s1)) ++s1;
              if (s1!=se1) {
                if (!is_inside_end) code.swap(code_end);
                pos = compile(s1,se1,depth1,p_ref,8);
                if (!is_inside_end) code.swap(code_end);
                result_end_dim = size(pos);
                result_end = mem._data + pos;
                is_end_code = true;
              }
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"end_t(",6)) { // End thread
              _cimg_mp_op("Function 'end_t()'");
              s1 = ss6; while (s1<se1 && cimg::is_blank(*s1)) ++s1;
              if (s1!=se1) {
                if (!is_inside_end_t) code.swap(code_end_t);
                compile(s1,se1,depth1,p_ref,16);
                if (!is_inside_end_t) code.swap(code_end_t);
                is_end_code = true;
              }
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"epoch(",6)) { // Convert date to epoch
              _cimg_mp_op("Function 'epoch()'");
              is_sth = true; // Tell if all arguments are constant
              CImg<ulongT> _op(1,8,1,1,(ulongT)~0U);
              arg1 = 2;
              if (ss6<se1)
                for (s = std::strchr(ss,'(') + 1; s<se; ++s) {
                  if (arg1>=_op._height) { arg1 = ~0U; break; }
                  ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                                 (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                  arg2 = compile(s,ns,depth1,0,block_flags);
                  if (is_vector(arg2)) {
                    p2 = size(arg2);
                    for (unsigned int k = 1; k<=p2; ++k) {
                      if (arg1>=_op._height) { arg1 = ~0U; break; }
                      _op[arg1++] = arg2 + k;
                    }
                  } else _op[arg1++] = arg2;
                  is_sth&=is_const_scalar(arg2);
                  s = ns;
                }
              else is_sth = false;
              if (arg1==~0U) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Too much arguments specified, in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s0);
              }

              _op.move_to(opcode);
              opcode[0] = (ulongT)mp_epoch;
              if (is_sth) _cimg_mp_const_scalar(mp_epoch(*this));
              pos = opcode[1] = scalar();
              opcode.move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"equalize(",9)) { // Equalize
              _cimg_mp_op("Function 'equalize()'");
              s0 = ss + 9;
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(s0,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              arg3 = arg4 = ~0U;
              if (s2<se1) {
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg3 = compile(s2,s1,depth1,0,block_flags);
                arg4 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U;
              }
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_const_scalar(arg2,2,3);
              if (arg3!=~0U) _cimg_mp_check_type(arg3,3,1,0);
              if (arg4!=~0U) _cimg_mp_check_type(arg4,3,1,0);
              p1 = size(arg1);
              arg2 = (unsigned int)mem[arg2];
              pos = vector(p1);
              CImg<ulongT>::vector((ulongT)mp_equalize,pos,arg1,p1,arg2,arg3,arg4).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

#if cimg_use_cpp11==1
            if (!std::strncmp(ss,"erf(",4)) { // Error function
              _cimg_mp_op("Function 'erf()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(erf,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::erf(mem[arg1]));
              _cimg_mp_scalar1(erf,arg1);
            }
#endif

            if (!std::strncmp(ss,"erfinv(",7)) { // Inverse of error function
              _cimg_mp_op("Function 'erfinv()'");
              arg1 = compile(ss7,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(erfinv,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::erfinv(mem[arg1]));
              _cimg_mp_scalar1(erfinv,arg1);
            }

            if (!std::strncmp(ss,"exp(",4)) { // Exponential
              _cimg_mp_op("Function 'exp()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(exp,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::exp(mem[arg1]));
              _cimg_mp_scalar1(exp,arg1);
            }

            if (!std::strncmp(ss,"expr(",5)) { // Vector from expression
              _cimg_mp_op("Function 'expr()'");
              s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss5,s1,depth1,0,block_flags); // formula
              _cimg_mp_check_type(arg1,1,2,0);
              p1 = size(arg1);
              arg2 = arg3 = arg4 = arg5 = 0; arg6 = ~0U;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg2 = compile(s1,s2,depth1,0,block_flags); // w
                _cimg_mp_check_const_scalar(arg2,2,3);
                arg2 = (unsigned int)mem[arg2];
                if (arg2) arg3 = arg4 = arg5 = 1;
                if (s2<se1) {
                  s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                  arg3 = compile(s2,s1,depth1,0,block_flags); // h
                  _cimg_mp_check_const_scalar(arg3,3,3);
                  arg3 = (unsigned int)mem[arg3];
                  if (s1<se1) {
                    s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                    arg4 = compile(s1,s2,depth1,0,block_flags); // d
                    _cimg_mp_check_const_scalar(arg4,4,3);
                    arg4 = (unsigned int)mem[arg4];
                    if (s2<se1) {
                      s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                      arg5 = compile(s2,s1,depth1,0,block_flags); // s
                      _cimg_mp_check_const_scalar(arg5,5,3);
                      arg5 = (unsigned int)mem[arg5];
                      arg6 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U; // A
                    }
                  }
                }
              }
              p2 = arg2*arg3*arg4; // whd
              p3 = p2*arg5; // whds

              unsigned int sizA = 0;
              if (arg6!=~0U) {
                _cimg_mp_check_type(arg6,6,2,0);
                sizA = size(arg6);
                if (sizA%p2) {
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Argument 'A' has an invalid size "
                                              "(%u, not a multiplier of %u*%u*%u = %u), "
                                              "in expression '%s'.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,
                                              sizA,arg2,arg3,arg4,arg2*arg3*arg4,
                                              ss);
                }
              }
              if (p3) pos = vector(p3); else pos = scalar();
              CImg<ulongT>::vector((ulongT)mp_expr,pos,arg1,p1,arg2,arg3,arg4,arg5,arg6,sizA/p2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"eye(",4)) { // Identity matrix
              _cimg_mp_op("Function 'eye()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              _cimg_mp_check_const_scalar(arg1,1,3);
              p1 = (unsigned int)mem[arg1];
              pos = vector(p1*p1);
              CImg<ulongT>::vector((ulongT)mp_eye,pos,p1).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            break;

          case 'f' :
            if (!std::strncmp(ss,"f2ui(",5)) { // Special float->uint conversion
              _cimg_mp_op("Function 'f2ui()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(f2ui,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar((double)cimg::float2uint((T)mem[arg1]));
              _cimg_mp_scalar1(f2ui,arg1);
            }

            if (!std::strncmp(ss,"fact(",5)) { // Factorial
              _cimg_mp_op("Function 'fact()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(factorial,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::factorial((int)mem[arg1]));
              _cimg_mp_scalar1(factorial,arg1);
            }

            if (!std::strncmp(ss,"fibo(",5)) { // Fibonacci
              _cimg_mp_op("Function 'fibo()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(fibonacci,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::fibonacci((int)mem[arg1]));
              _cimg_mp_scalar1(fibonacci,arg1);
            }

            if (!std::strncmp(ss,"fill(",5)) { // Fill
              _cimg_mp_op("Function 'fill()'");
              s0 = ss5; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
              arg1 = compile(ss5,s0,depth1,0,block_flags); // target
              _cimg_mp_check_type(arg1,1,2,0);
              s1 = ++s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              p1 = code._width;
              if (s1<se1) { // Version with >=3 arguments
                s2 = s1 + 1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                if (s2<se1) { // Version with 6 arguments: 'fill(target,w,h,d,s,'expr')'
                  arg2 = compile(s0,s1,depth1,0,block_flags); // w
                  arg3 = compile(++s1,s2,depth1,0,block_flags); // h
                  s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                  arg4 = compile(s2,s1,depth1,0,block_flags); // d
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg5 = compile(s1,s2,depth1,0,block_flags); // s
                  arg6 = compile(++s2,se1,depth1,0,block_flags); // expr
                  _cimg_mp_check_const_scalar(arg2,2,3);
                  _cimg_mp_check_const_scalar(arg3,3,3);
                  _cimg_mp_check_const_scalar(arg4,4,3);
                  _cimg_mp_check_const_scalar(arg5,5,3);
                  _cimg_mp_check_type(arg6,6,2,0);
                  arg2 = (unsigned int)mem[arg2];
                  arg3 = (unsigned int)mem[arg3];
                  arg4 = (unsigned int)mem[arg4];
                  arg5 = (unsigned int)mem[arg5];
                  p1 = size(arg1);
                  if (p1!=arg2*arg3*arg4*arg5) {
                    _cimg_mp_strerr;
                    throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                "CImg<%s>::%s: %s: Input vector size (%u values) and its specified "
                                                "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                                pixel_type(),_cimg_mp_calling_function,s_op,
                                                p1,arg2,arg3,arg4,arg5,arg2*arg3*arg4*arg5);
                  }
                  CImg<ulongT>::vector((ulongT)mp_vector_fill_ext,arg1,arg2,arg3,arg4,arg5,arg6,size(arg6)).
                    move_to(code,p1);
                  _cimg_mp_return_nan();
                } else { // Version with 3 arguments: 'fill(target,index_name,expr)'
                  variable_name.assign(s0,(unsigned int)(s1 + 1 - s0)).back() = 0;
                  cimg::strpare(variable_name,false,true);
                  if (!cimg::is_varname(variable_name)) { // Invalid variable name
                    cimg::strellipsize(variable_name,64);
                    _cimg_mp_strerr;
                    throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                "CImg<%s>::%s: %s: Invalid loop variable name '%s', "
                                                "in expression '%s'.",
                                                pixel_type(),_cimg_mp_calling_function,s_op,
                                                variable_name._data,s0);
                  }
                  get_variable_pos(variable_name,arg2,arg3);
                  arg2 = arg3!=~0U?reserved_label[arg3]:arg2!=~0U?variable_pos[arg2]:~0U; // Variable slot
                  if (arg2!=~0U && (!is_scalar(arg2) ||
                                    is_const_scalar(arg2))) { // Variable is not a vector or is a constant->error
                    cimg::strellipsize(variable_name,64);
                    _cimg_mp_strerr;
                    throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                "CImg<%s>::%s: %s: Invalid type '%s' for variable '%s' "
                                                "(expected 'scalar'), in expression '%s'.",
                                                pixel_type(),_cimg_mp_calling_function,s_op,
                                                s_type(arg2)._data,variable_name._data,s0);
                  } else if (arg2==~0U) { // Variable does not exist -> create it
                    arg2 = scalar();
                    if (arg3!=~0U) reserved_label[arg3] = arg2;
                    else {
                      if (variable_def._width>=variable_pos._width) variable_pos.resize(-200,1,1,1,0);
                      variable_pos[variable_def._width] = arg2;
                      variable_name.move_to(variable_def);
                    }
                    memtype[arg2] = -1;
                  }
                  arg3 = compile(++s1,se1,depth1,0,block_flags);
                  _cimg_mp_check_type(arg3,3,1,0);
                }
              } else { // Version with 2 arguments : 'fill(target,expr)'
                arg2 = ~0U;
                arg3 = compile(s0,se1,depth1,0,block_flags);
              }
              // arg2 = variable slot, arg3 = fill expression.
              _cimg_mp_check_type(arg3,3,1,0);
              CImg<ulongT>::vector((ulongT)mp_vector_fill,arg1,size(arg1),arg2,arg3,code._width - p1).
                move_to(code,p1);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"find(",5)) { // Find
              _cimg_mp_op("Function 'find()'");

              // First argument: data to look at.
              s0 = ss5; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
              if (*ss5=='#') { // Index specified
                p1 = compile(ss6,s0,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss6);
                _cimg_mp_check_list();
                arg1 = ~0U;
              } else { // Vector specified
                arg1 = compile(ss5,s0,depth1,0,block_flags);
                if (!arg1) _cimg_mp_return(11); // Manage empty string -> -1
                _cimg_mp_check_type(arg1,1,2,0);
                p1 = ~0U;
              }

              // Second argument: data to find.
              s1 = ++s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg2 = compile(s0,s1,depth1,0,block_flags);

              // Third and fourth arguments: starting index and search direction.
              arg3 = _cimg_mp_slot_nan; arg4 = 1;
              if (s1<se1) {
                s0 = ++s1; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                arg3 = compile(s1,s0,depth1,0,block_flags);
                _cimg_mp_check_type(arg3,3,1,0);
                if (s0<se1) {
                  arg4 = compile(++s0,se1,depth1,0,block_flags);
                  _cimg_mp_check_type(arg4,4,1,0);
                }
              }
              if (p1!=~0U) {
                if (size(arg2)>1)
                  _cimg_mp_scalar5(image_find_seq,p1,arg2,size(arg2),arg3,arg4);
                _cimg_mp_scalar4(image_find,p1,arg2 + (size(arg2)?1:0),arg3,arg4);
              }
              if (size(arg2)>1)
                _cimg_mp_scalar6(find_seq,arg1,size(arg1),arg2,size(arg2),arg3,arg4);
              _cimg_mp_scalar5(find,arg1,size(arg1),arg2 + (size(arg2)?1:0),arg3,arg4);
            }

            if (*ss1=='o' && *ss2=='r' && *ss3=='(') { // For loop
              _cimg_mp_op("Function 'for()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              s2 = s1 + 1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              s3 = s2 + 1; while (s3<se1 && (*s3!=',' || level[s3 - expr._data]!=clevel1)) ++s3;
              arg1 = code._width;
              p1 = compile(ss4,s1,depth1,0,block_flags); // Init
              arg2 = code._width;
              p2 = compile(++s1,s2,depth1,0,block_flags); // Cond
              arg3 = code._width;
              arg6 = mempos;
              if (s3<se1) { // Body + post
                p3 = compile(s3 + 1,se1,depth1,0,block_flags); // Body
                arg4 = code._width;
                pos = compile(++s2,s3,depth1,0,block_flags); // Post
              } else {
                p3 = compile(++s2,se1,depth1,0,block_flags); // Body only
                arg4 = pos = code._width;
              }
              _cimg_mp_check_type(p2,2,1,0);
              arg5 = size(pos);
              CImg<ulongT>::vector((ulongT)mp_for,p3,(ulongT)size(p3),p2,arg2 - arg1,arg3 - arg2,
                                   arg4 - arg3,code._width - arg4,
                                   p3>=arg6 && !is_const_scalar(p3),
                                   p2>=arg6 && !is_const_scalar(p2)).move_to(code,arg1);
              _cimg_mp_return(p3);
            }

            if (!std::strncmp(ss,"flood(",6)) { // Flood fill image
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_op("Function 'flood()'");
              if (*ss6=='#') { // Index specified
                s0 = ss7; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss7,s0++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss7);
                _cimg_mp_check_list();
              } else { p1 = ~0U; s0 = ss6; }
              CImg<ulongT>::vector((ulongT)mp_flood,_cimg_mp_slot_nan,0,p1).move_to(l_opcode);
              for (s = s0; s<se1; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg2 = compile(s,ns,depth1,0,block_flags);
                if (is_vector(arg2)) // Vector argument allowed to specify cordinates and color
                  CImg<ulongT>::sequence(size(arg2),arg2 + 1,arg2 + (ulongT)size(arg2)).move_to(l_opcode);
                else
                  CImg<ulongT>::vector(arg2).move_to(l_opcode);
                s = ns;
              }
              (l_opcode>'y').move_to(opcode);
              opcode[2] = opcode._height;
              opcode.move_to(code);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"floor(",6)) { // Floor
              _cimg_mp_op("Function 'floor()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(floor,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::floor(mem[arg1]));
              _cimg_mp_scalar1(floor,arg1);
            }

            if (!std::strncmp(ss,"frac(",5)) { // Fractional part
              _cimg_mp_op("Function 'frac()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(frac,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::frac(mem[arg1]));
              _cimg_mp_scalar1(frac,arg1);
            }

            if (!std::strncmp(ss,"fsize(",6)) { // File size
              _cimg_mp_op("Function 'fsize()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,2,0);
              pos = scalar();
              CImg<ulongT>::vector((ulongT)mp_fsize,pos,arg1,(ulongT)size(arg1)).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            break;

          case 'g' :
#if cimg_use_cpp11==1
            if (!std::strncmp(ss,"gamma(",6)) { // Gamma
              _cimg_mp_op("Function 'gamma()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(gamma,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::tgamma(mem[arg1]));
              _cimg_mp_scalar1(gamma,arg1);
            }
#endif

            if (!std::strncmp(ss,"gauss(",6)) { // Gaussian function
              _cimg_mp_op("Function 'gauss()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags);
              arg2 = arg3 = 1;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg2 = compile(s1,s2,depth1,0,block_flags);
                arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):1;
              }
              _cimg_mp_check_type(arg2,2,1,0);
              _cimg_mp_check_type(arg3,3,1,0);
              if (is_vector(arg1)) _cimg_mp_vector3_vss(gauss,arg1,arg2,arg3);
              if (is_const_scalar(arg1) && is_const_scalar(arg2) && is_const_scalar(arg3)) {
                val1 = mem[arg1];
                val2 = mem[arg2];
                _cimg_mp_const_scalar(std::exp(-val1*val1/(2*val2*val2))/(mem[arg3]?std::sqrt(2*val2*val2*cimg::PI):1));
              }
              _cimg_mp_scalar3(gauss,arg1,arg2,arg3);
            }

#ifdef cimg_mp_func_get
            if (!std::strncmp(ss,"get(",4)) { // Get value from external variable
              _cimg_mp_op("Function 'get()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              arg2 = arg3 = 0;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg2 = compile(s1,s2,depth1,0,block_flags);
                arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0;
              }
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_const_scalar(arg2,2,2);
              _cimg_mp_check_type(arg3,3,1,0);
              p1 = size(arg1);

              const int siz_max = cimg::type<int>::max();
              if (mem[arg2]>=siz_max) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Specified variable size %g is larger than %d.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            mem[arg2],siz_max);
              }
              arg2 = (unsigned int)mem[arg2];
              if (arg2) pos = vector(arg2); else pos = scalar();
              CImg<ulongT>::vector((ulongT)mp_get,pos,arg1,p1,arg2,arg3).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
#endif
            break;

          case 'h' :
            if (*ss1=='(') { // Image height
              _cimg_mp_op("Function 'h()'");
              if (*ss2=='#') { // Index specified
                p1 = compile(ss3,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss3);
                _cimg_mp_check_list();
              } else { if (ss2!=se1) break; p1 = ~0U; }
              _cimg_mp_scalar1(image_height,p1);
            }

            if (!std::strncmp(ss,"histogram(",10)) { // Compute histogram
              _cimg_mp_op("Function 'histogram()'");
              s0 = ss + 10;
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(s0,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              arg3 = arg4 = ~0U;
              if (s2<se1) {
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg3 = compile(s2,s1,depth1,0,block_flags);
                arg4 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U;
              }
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_const_scalar(arg2,2,3);
              if (arg3!=~0U) _cimg_mp_check_type(arg3,3,1,0);
              if (arg4!=~0U) _cimg_mp_check_type(arg4,3,1,0);
              p1 = size(arg1);
              arg2 = (unsigned int)mem[arg2];
              pos = vector(arg2);
              CImg<ulongT>::vector((ulongT)mp_histogram,pos,arg1,p1,arg2,arg3,arg4).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            break;

          case 'i' :
            if (*ss1=='c' && *ss2=='(') { // Image median
              _cimg_mp_op("Function 'ic()'");
              if (*ss3=='#') { // Index specified
                p1 = compile(ss4,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss4);
                _cimg_mp_check_list();
              } else { if (ss3!=se1) break; p1 = ~0U; }
              pos = scalar();
              CImg<ulongT>::vector((ulongT)mp_image_median,pos,p1).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (*ss1=='f' && *ss2=='(') { // If..then[..else.]
              _cimg_mp_op("Function 'if()'");
              s1 = ss3; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              s2 = s1 + 1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg1 = compile(ss3,s1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,1,0);
              if (is_const_scalar(arg1)) {
                if ((bool)mem[arg1]) return compile(++s1,s2,depth1,0,block_flags);
                else return s2<se1?compile(++s2,se1,depth1,0,block_flags):0;
              }
              p2 = code._width;
              arg2 = compile(++s1,s2,depth1,0,block_flags);
              p3 = code._width;
              arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):
                is_vector(arg2)?vector(size(arg2),0):0;
              _cimg_mp_check_type(arg3,3,is_vector(arg2)?2:1,size(arg2));
              arg4 = size(arg2);
              if (arg4) pos = vector(arg4); else pos = scalar();
              CImg<ulongT>::vector((ulongT)mp_if,pos,arg1,arg2,arg3,
                                   p3 - p2,code._width - p3,arg4).move_to(code,p2);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (*ss1=='n' && *ss2=='(') { // Image norm
              _cimg_mp_op("Function 'in()'");
              if (*ss3=='#') { // Index specified
                p1 = compile(ss4,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss4);
                _cimg_mp_check_list();
              } else { if (ss3!=se1) break; p1 = ~0U; }
              pos = scalar();
              CImg<ulongT>::vector((ulongT)mp_image_norm,pos,p1).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"index(",6)) { // Index colors
              _cimg_mp_op("Function 'index()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg3 = compile(s2,s1,depth1,0,block_flags);
              arg4 = arg5 = 0;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg4 = compile(s1,s2,depth1,0,block_flags);
                arg5 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0;
              }
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_type(arg2,2,2,0);
              _cimg_mp_check_const_scalar(arg3,3,3);
              _cimg_mp_check_type(arg4,4,1,0);
              _cimg_mp_check_const_scalar(arg5,5,2);
              p1 = size(arg1);
              p2 = size(arg2);
              arg3 = (unsigned int)mem[arg3];
              arg5 = (unsigned int)mem[arg5];
              p3 = p2/arg3; // Number of color entries
              if (p2!=p3*arg3) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Colormap size (%u values) and specified "
                                            "dimension of colormap entries (%u) do not match.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            std::max(p2,1U),arg3);
              }
              if (p1%arg3) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Input vector size (%u values) and specified "
                                            "dimension of colormap entries (%u) do not match.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            std::max(p1,1U),arg3);
              }
              pos = vector(arg5?p1:p1/arg3);
              CImg<ulongT>::vector((ulongT)mp_index,pos,arg1,p1,arg2,p2,arg3,arg4,arg5).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"indexof(",8)) { // Index of element in list of elements
              _cimg_mp_op("Function 'indexof()'");
              s1 = ss8; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss8,s1,depth1,0,block_flags); // elt
              is_sth = is_const_scalar(arg1); // All elements are constant scalars ?
              if (is_sth) val = mem[arg1];
              p1 = 0;
              CImg<ulongT>::vector((ulongT)mp_indexof,0,0,arg1,size(arg1)).move_to(l_opcode);
              for (s = ++s1; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg2 = compile(s,ns,depth1,0,block_flags);
                is_sth&=is_const_scalar(arg2);
                if (is_sth && mem[arg2]==val) _cimg_mp_const_scalar((double)p1);
                CImg<ulongT>::vector(arg2,size(arg2)).move_to(l_opcode);
                s = ns;
                ++p1;
              }
              if (is_sth) _cimg_mp_return(11); // Const case: no match
              (l_opcode>'y').move_to(opcode);
              opcode[1] = pos = scalar();
              opcode[2] = opcode._height;
              opcode.move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"inrange(",8)) { // Check value range
              _cimg_mp_op("Function 'inrange()'");
              s1 = ss8; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss8,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg3 = compile(s2,s1,depth1,0,block_flags);
              arg4 = arg5 = 1;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg4 = compile(s1,s2,depth1,0,block_flags);
                arg5 = s2<se1?compile(++s2,se1,depth1,0,block_flags):arg4;
                _cimg_mp_check_type(arg4,4,1,0);
                _cimg_mp_check_type(arg5,5,1,0);
              }
              if (is_const_scalar(arg1) && is_const_scalar(arg2) &&
                  is_const_scalar(arg3) && is_const_scalar(arg4) &&
                  is_const_scalar(arg5)) { // Optimize constant case
                val = mem[arg1]; val1 = mem[arg2]; val2 = mem[arg3];
                if (val2>=val1)
                  is_sth = (mem[arg4]?(val>=val1):(val>val1)) && (mem[arg5]?(val<=val2):(val<val2));
                else
                  is_sth = (mem[arg5]?(val>=val2):(val>val2)) && (mem[arg4]?(val<=val1):(val<val1));
                _cimg_mp_return(is_sth?1:0);
              }
              p1 = size(arg1);
              p2 = size(arg2);
              p3 = size(arg3);
              arg6 = ~0U; // Size of return value
              if (!p1) {
                arg6 = p2?p2:p3;
                if (arg6) _cimg_mp_check_type(arg3,3,3,arg6);
              } else {
                arg6 = p1;
                _cimg_mp_check_type(arg2,2,3,arg6);
                _cimg_mp_check_type(arg3,3,3,arg6);
              }
              pos = arg6?vector(arg6):scalar();
              CImg<ulongT>::vector((ulongT)mp_inrange,pos,arg6,arg1,p1,arg2,p2,arg3,p3,arg4,arg5).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"int(",4)) { // Integer cast
              _cimg_mp_op("Function 'int()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(int,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar((longT)mem[arg1]);
              _cimg_mp_scalar1(int,arg1);
            }

            if (!std::strncmp(ss,"invert(",7)) { // Matrix/scalar inverse (or pseudoinverse)
              _cimg_mp_op("Function 'invert()'");
              s1 = ss7; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss7,s1,depth1,0,block_flags); // A
              arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U; // nb_colsA
              if (arg2!=~0U) {
                _cimg_mp_check_const_scalar(arg2,2,3);
                arg2 = (unsigned int)mem[arg2];
              }
              if (is_vector(arg1)) {
                p1 = size(arg1);
                if (arg2==~0U) { // nb_colsA not specified: assuming square matrix
                  _cimg_mp_check_matrix_square(arg1,1);
                  p2 = p3 = (unsigned int)cimg::round(std::sqrt((float)p1));
                } else {
                  p2 = arg2;
                  p3 = p1/p2;
                  if (p3*p2!=p1) {
                    _cimg_mp_strerr;
                    throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                "CImg<%s>::%s: %s: Type of first argument ('%s') "
                                                "does not match with second argument 'nb_colsA=%u', "
                                                "in expression '%s'.",
                                                pixel_type(),_cimg_mp_calling_function,s_op,
                                                s_type(arg1)._data,p2,s0);
                  }
                }
                pos = vector(p1);
                CImg<ulongT>::vector((ulongT)mp_matrix_invert,pos,arg1,p2,p3).move_to(code);
                return_comp = true;
                _cimg_mp_return(pos);
              }
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(1/mem[arg1]);
              _cimg_mp_scalar2(div,1,arg1);
            }

            if (*ss1=='s') { // Family of 'is_?()' functions

              if (!std::strncmp(ss,"isbool(",7)) { // Is boolean?
                _cimg_mp_op("Function 'isbool()'");
                if (ss7==se1) _cimg_mp_return(0);
                try { arg1 = compile(ss7,se1,depth1,0,block_flags); }
                catch (CImgException&) { _cimg_mp_return(0); }
                if (is_vector(arg1)) _cimg_mp_vector1_v(isbool,arg1);
                if (is_const_scalar(arg1)) _cimg_mp_return(mem[arg1]==0. || mem[arg1]==1.);
                _cimg_mp_scalar1(isbool,arg1);
              }

              if (!std::strncmp(ss,"isconst(",8)) { // Is constant?
                _cimg_mp_op("Function 'isconst()'");
                if (ss8==se1) _cimg_mp_return(0);
                try { arg1 = compile(ss8,se1,depth1,0,block_flags); }
                catch (CImgException&) { _cimg_mp_return(0); }
                if (is_const_scalar(arg1)) _cimg_mp_return(1);
                _cimg_mp_return(0);
              }

              if (!std::strncmp(ss,"isdir(",6)) { // Is directory?
                _cimg_mp_op("Function 'isdir()'");
                arg1 = compile(ss6,se1,depth1,0,block_flags);
                pos = scalar();
                CImg<ulongT>::vector((ulongT)mp_isdir,pos,arg1,(ulongT)size(arg1)).move_to(code);
                return_comp = true;
                _cimg_mp_return(pos);
              }

              if (!std::strncmp(ss,"isexpr(",7)) { // Is valid expression?
                _cimg_mp_op("Function 'isexpr()'");
                if (ss7==se1) _cimg_mp_return(0);
                try { arg1 = compile(ss7,se1,depth1,0,block_flags); }
                catch (CImgException&) { _cimg_mp_return(0); }
                _cimg_mp_return(1);
              }

              if (!std::strncmp(ss,"isfile(",7)) { // Is file?
                _cimg_mp_op("Function 'isfile()'");
                arg1 = compile(ss7,se1,depth1,0,block_flags);
                pos = scalar();
                CImg<ulongT>::vector((ulongT)mp_isfile,pos,arg1,(ulongT)size(arg1)).move_to(code);
                return_comp = true;
                _cimg_mp_return(pos);
              }

              if (!std::strncmp(ss,"isfinite(",9)) { // Is finite?
                _cimg_mp_op("Function 'isfinite()'");
                s0 = ss + 9;
                if (s0==se1) _cimg_mp_return(0);
                arg1 = compile(s0,se1,depth1,0,block_flags);
                if (is_vector(arg1)) _cimg_mp_vector1_v(isfinite,arg1);
                if (is_const_scalar(arg1))
                  _cimg_mp_return((unsigned int)cimg::type<double>::is_finite(mem[arg1]));
                _cimg_mp_scalar1(isfinite,arg1);
              }

              if (!std::strncmp(ss,"isin(",5)) { // Is in sequence/vector?
                if (ss5>=se1) _cimg_mp_return(0);
                _cimg_mp_op("Function 'isin()'");
                is_sth = true; // Are all arguments constants ?
                for (s = ss5; s<se; ++s) {
                  ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                                 (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                  arg1 = compile(s,ns,depth1,0,block_flags);
                  if (!is_const_scalar(arg1)) is_sth = false;
                  CImg<ulongT>::vector(arg1,size(arg1)).move_to(l_opcode);
                  s = ns;
                }
                if (is_sth) { // All arguments are constant -> return constant
                  if (l_opcode._width<2) _cimg_mp_return(0);
                  const double target = mem[l_opcode(0,0)];
                  for (unsigned int i = 1; i<l_opcode._width; ++i) {
                    if (mem[l_opcode(i,0)]==target) _cimg_mp_return(1);
                  }
                  _cimg_mp_return(0);
                }
                pos = scalar();
                CImg<ulongT>::vector((ulongT)mp_isin,pos,0).move_to(l_opcode,0);
                (l_opcode>'y').move_to(opcode);
                opcode[2] = opcode._height;
                opcode.move_to(code);
                return_comp = true;
                _cimg_mp_return(pos);
              }

              if (!std::strncmp(ss,"isinf(",6)) { // Is infinite?
                _cimg_mp_op("Function 'isinf()'");
                if (ss6==se1) _cimg_mp_return(0);
                arg1 = compile(ss6,se1,depth1,0,block_flags);
                if (is_vector(arg1)) _cimg_mp_vector1_v(isinf,arg1);
                if (is_const_scalar(arg1))
                  _cimg_mp_return((unsigned int)cimg::type<double>::is_inf(mem[arg1]));
                _cimg_mp_scalar1(isinf,arg1);
              }

              if (!std::strncmp(ss,"isint(",6)) { // Is integer?
                _cimg_mp_op("Function 'isint()'");
                if (ss6==se1) _cimg_mp_return(0);
                s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                try { arg1 = compile(ss6,s1,depth1,0,block_flags); }
                catch (CImgException&) { _cimg_mp_return(0); }
                arg2 = arg3 = ~0U;
                if (s1<se1) {
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg2 = compile(s1,s2,depth1,0,block_flags);
                  arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):~0U;
                }
                if (arg2!=~0U) _cimg_mp_check_type(arg2,2,1,0);
                if (arg3!=~0U) _cimg_mp_check_type(arg3,3,1,0);
                if (is_vector(arg1)) _cimg_mp_vector3_vss(isint,arg1,arg2,arg3);
                if (is_const_scalar(arg1) && (arg2==~0U || is_const_scalar(arg2)) &&
                    (arg3==~0U || is_const_scalar(arg3))) {
                  val = mem[arg1];
                  is_sth = std::modf(val,&val1)==0;
                  if (arg2==~0U) _cimg_mp_return(is_sth);
                  if (arg3==~0U) _cimg_mp_return(is_sth && val>=mem[arg2]);
                  _cimg_mp_return(is_sth && val>=mem[arg2] && val<=mem[arg3]);
                }
                _cimg_mp_scalar3(isint,arg1,arg2,arg3);
              }

              if (!std::strncmp(ss,"isnan(",6)) { // Is NaN?
                _cimg_mp_op("Function 'isnan()'");
                if (ss6==se1) _cimg_mp_return(0);
                arg1 = compile(ss6,se1,depth1,0,block_flags);
                if (is_vector(arg1)) _cimg_mp_vector1_v(isnan,arg1);
                if (is_const_scalar(arg1))
                  _cimg_mp_return((unsigned int)cimg::type<double>::is_nan(mem[arg1]));
                _cimg_mp_scalar1(isnan,arg1);
              }

              if (!std::strncmp(ss,"isnum(",6)) { // Is number?
                _cimg_mp_op("Function 'isnum()'");
                val = 0;
                if (cimg_sscanf(ss6,"%lf%c%c",&val,&sep,&end)==2 && sep==')') _cimg_mp_return(1);
                _cimg_mp_return(0);
              }

              if (!std::strncmp(ss,"ispercentage(",13)) { // Does argument ends with '%'?
                _cimg_mp_op("Function 'ispercentage()'");
                _cimg_mp_return((unsigned int)(*se2=='%'));
              }

              if (!std::strncmp(ss,"isvar(",6)) { // Is existing variable?
                _cimg_mp_op("Function 'isvar()'");
                if (ss6==se1) _cimg_mp_return(0);
                s1 = ss6; while (s1<se && (*s1!=')' || level[s1 - expr._data]!=clevel)) ++s1; // Closing parenthesis
                if (*s1!=')') _cimg_mp_return(0);
                variable_name.assign(s1 - ss6 + 1);
                if (variable_name._width) std::memcpy(variable_name,ss6,variable_name.width() - 1);
                variable_name.back() = 0;
                get_variable_pos(variable_name,arg1,arg2);
                if (arg1!=~0U || arg2!=~0U) _cimg_mp_return(1);
                _cimg_mp_return(0);
              }

              if (!std::strncmp(ss,"isvarname(",10)) { // Is variable name?
                _cimg_mp_op("Function 'isvarname()'");
                arg1 = compile(ss + 10,se1,depth1,0,block_flags);
                pos = scalar();
                CImg<ulongT>::vector((ulongT)mp_isvarname,pos,arg1,(ulongT)size(arg1)).move_to(code);
                return_comp = true;
                _cimg_mp_return(pos);
              }
            }
            break;

          case 'l' :
            if (*ss1=='(') { // Size of image list
              _cimg_mp_op("Function 'l()'");
              if (ss2!=se1) break;
              _cimg_mp_scalar0(size_list);
            }

            if (!std::strncmp(ss,"lerp(",5)) { // Linear interpolation
              _cimg_mp_op("Function 'lerp()'");
              s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss5,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              arg3 = compile(++s2,se1,depth1,0,block_flags);
              if (is_vector(arg1)) p1 = size(arg1);
              else if (is_vector(arg2)) p1 = size(arg2);
              else p1 = size(arg3);
              _cimg_mp_check_type(arg1,1,3,p1);
              _cimg_mp_check_type(arg2,2,3,p1);
              _cimg_mp_check_type(arg3,3,3,p1);
              if (is_const_scalar(arg3)) { // Optimize constant cases
                if (!arg3) _cimg_mp_same(arg1);
                if (arg3==1) _cimg_mp_same(arg2);
                if (is_const_scalar(arg1) && is_const_scalar(arg2)) {
                  const double t = mem[arg3];
                  _cimg_mp_const_scalar(mem[arg1]*(1 - t) + mem[arg2]*t);
                }
              }
              if (!p1) _cimg_mp_scalar3(lerp,arg1,arg2,arg3);
              pos = vector(p1);
              CImg<ulongT>::vector((ulongT)mp_vector_lerp,pos,p1,arg1,size(arg1),arg2,size(arg2),arg3,size(arg3)).
                move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"log(",4)) { // Natural logarithm
              _cimg_mp_op("Function 'log()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(log,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::log(mem[arg1]));
              _cimg_mp_scalar1(log,arg1);
            }

            if (!std::strncmp(ss,"log10(",6)) { // Base-10 logarithm
              _cimg_mp_op("Function 'log10()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(log10,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::log10(mem[arg1]));
              _cimg_mp_scalar1(log10,arg1);
            }

            if (!std::strncmp(ss,"log2(",5)) { // Base-2 logarithm
              _cimg_mp_op("Function 'log2()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(log2,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::log2(mem[arg1]));
              _cimg_mp_scalar1(log2,arg1);
            }

            if (!std::strncmp(ss,"logit(",6)) { // Logit
              _cimg_mp_op("Function 'logit()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(logit,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::logit(mem[arg1]));
              _cimg_mp_scalar1(logit,arg1);
            }

            if (!std::strncmp(ss,"lowercase(",10)) { // Lower case
              _cimg_mp_op("Function 'lowercase()'");
              arg1 = compile(ss + 10,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(lowercase,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::lowercase(mem[arg1]));
              _cimg_mp_scalar1(lowercase,arg1);
            }
            break;

          case 'm' :
            if (!std::strncmp(ss,"map(",4)) { // Map vector
              _cimg_mp_op("Function 'map()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags); // X
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags); // P
              arg3 = arg4 = 1; arg5 = 0;
              if (s2<se1) {
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg3 = compile(s2,s1,depth1,0,block_flags); // nb_channelsX
                if (s1<se1) {
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg4 = compile(s1,s2,depth1,0,block_flags); // nb_channelsP
                  arg5 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0; // boundary_conditions
                }
              }
              _cimg_mp_check_type(arg2,2,2,0);
              _cimg_mp_check_const_scalar(arg3,3,3);
              _cimg_mp_check_const_scalar(arg4,4,3);
              _cimg_mp_check_type(arg5,5,1,0);
              p1 = size(arg1);
              p2 = size(arg2);
              arg3 = (unsigned int)mem[arg3];
              arg4 = (unsigned int)mem[arg4];
              if ((p1%arg3) || (!p1 && arg3!=1)) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Type of first arguments ('%s') "
                                            "does not match with third argument 'nb_channelsX=%u', "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s_type(arg1)._data,arg3,s0);
              }
              if (p2%arg4) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Type of second arguments ('%s') "
                                            "does not match with fourth argument 'nb_channelsP=%u', "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s_type(arg2)._data,arg4,s0);
              }
              if (!p1 && arg4==1) pos = scalar();
              else pos = vector(std::max(p1,1U)*arg4);
              CImg<ulongT>::vector((ulongT)mp_map,pos,arg1,p1,arg2,p2,arg3,arg4,arg5).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"merge(",6)) { // Merge inter-thread variables
              _cimg_mp_op("Function 'merge()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              ref.assign(7);
              pos = compile(ss6,s1,depth1,ref,block_flags);
              if (*ref) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: First argument cannot be a linked reference, "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s0);
              }
              arg1 = ~0U; // Merge operator
                          // (0='=',1='+',2='-',3='*',4='/',5='&',6='|',7='xor',8='&&',9=='||',10='min',11='max')
              if (s1<se1) {
                ++s1;
                char st_op[4] = {};
                is_sth = false; // blank after operator?
                if (cimg_sscanf(s1," %3[=-+/*&|minaxor]%c",st_op,&sep)==2 && (sep==')' ||
                                                                              (is_sth=cimg::is_blank(sep)))) {
                  if (!is_sth || (is_sth && cimg_sscanf(s1," %*[=+-*/&|minaxor ]%c",&sep)==1 && sep==')')) {
                    cimg::strpare(st_op,' ',false,true);
                    if (!st_op[1])
                      arg1 = *st_op=='='?0:*st_op=='+'?1:*st_op=='-'?2:*st_op=='*'?3:*st_op=='/'?4:
                        *st_op=='&'?5:*st_op=='|'?6:~0U;
                    else if (*st_op=='x' && st_op[1]=='o' && st_op[2]=='r' && !st_op[3]) arg1 = 7;
                    else if (*st_op=='&' && st_op[1]=='&' && !st_op[2]) arg1 = 8;
                    else if (*st_op=='|' && st_op[1]=='|' && !st_op[2]) arg1 = 9;
                    else if (*st_op=='m' && st_op[1]=='i' && st_op[2]=='n' && !st_op[3]) arg1 = 10;
                    else if (*st_op=='m' && st_op[1]=='a' && st_op[2]=='x' && !st_op[3]) arg1 = 11;
                  }
                }
              }
              cimg_rofY(memmerge,k) if (memmerge(0,k)==(int)pos) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Merge has already been requested before "
                                            "for specified variable "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,s0);
              }
              if (arg1==~0U) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Invalid specified operator "
                                            "(should be one of '=,+,-,*,/,&,|,xor,&&,||,min,max'), "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,s0);
              }
              memmerge.resize(3,memmerge._height + 1,1,1,0,0);
              memmerge(0,memmerge._height - 1) = (int)pos;
              memmerge(1,memmerge._height - 1) = (int)size(pos);
              memmerge(2,memmerge._height - 1) = (int)arg1;
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"mirror(",7)) { // Mirror image
              _cimg_mp_op("Function 'mirror()'");
              s1 = ss7; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss7,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg3 = compile(s2,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg4 = compile(s1,s2,depth1,0,block_flags);
              s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg5 = compile(s2,s1,depth1,0,block_flags);
              arg6 = compile(++s1,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_const_scalar(arg2,2,3);
              _cimg_mp_check_const_scalar(arg3,3,3);
              _cimg_mp_check_const_scalar(arg4,4,3);
              _cimg_mp_check_const_scalar(arg5,5,3);
              p1 = size(arg1);
              p2 = size(arg6);
              arg2 = (unsigned int)mem[arg2];
              arg3 = (unsigned int)mem[arg3];
              arg4 = (unsigned int)mem[arg4];
              arg5 = (unsigned int)mem[arg5];
              if (arg2*arg3*arg4*arg5!=std::max(1U,p1)) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Input vector size (%u values) and its specified "
                                            "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            std::max(p1,1U),arg2,arg3,arg4,arg5,arg2*arg3*arg4*arg5);
              }
              pos = vector(arg2*arg3*arg4*arg5);
              CImg<ulongT>::vector((ulongT)mp_mirror,pos,arg1,arg2,arg3,arg4,arg5,arg6,p2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"mproj(",6)) { // Project matrix onto dictionary
              _cimg_mp_op("Function 'mproj()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags); // S
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags); // ncolS
              s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg3 = compile(s2,s1,depth1,0,block_flags); // D
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg4 = compile(s1,s2,depth1,0,block_flags); // ncolD
              arg5 = arg6 = p3 = 0;
              if (s2<se1) {
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg5 = compile(s2,s1,depth1,0,block_flags); // method
                if (s1<se1) {
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg6 = compile(s1,s2,depth1,0,block_flags); // max_iter
                  p3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0; // method
                }
              }
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_const_scalar(arg2,2,3);
              _cimg_mp_check_type(arg3,3,2,0);
              _cimg_mp_check_const_scalar(arg4,4,3);
              _cimg_mp_check_type(arg5,5,1,0);
              _cimg_mp_check_type(arg6,6,1,0);
              _cimg_mp_check_type(p3,7,1,0);
              p1 = size(arg1);
              p2 = size(arg3);
              const unsigned int
                wS = (unsigned int)mem[arg2],
                wD = (unsigned int)mem[arg4],
                hS = p1/wS,
                hD = p2/wD;

              if (wS*hS!=p1) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Type of first argument ('%s') "
                                            "do not match with second argument 'nb_colsS=%u', "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s_type(arg1)._data,wS,s0);
              }
              if (wD*hD!=p2) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Type of third argument ('%s') "
                                            "do not match with fourth argument 'nb_colsD=%u', "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s_type(arg3)._data,wD,s0);
              }
              if (hS!=hD) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Type of first argument ('%s') "
                                            "do not match with third argument ('%s'), "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s_type(arg1)._data,s_type(arg3)._data,s0);
              }
              pos = vector(wS*wD);
              CImg<ulongT>::vector((ulongT)mp_mproj,pos,arg1,wS,hS,arg3,wD,arg5,arg6,p3).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"mse(",4)) { // Mean-squared error
              _cimg_mp_op("Function 'mse()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              arg2 = compile(++s1,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg2,2,is_scalar(arg1)?1:2,size(arg1));
              _cimg_mp_scalar3(mse,arg1,arg2,size(arg1));
            }

            if (!std::strncmp(ss,"mul(",4)) { // Matrix multiplication
              _cimg_mp_op("Function 'mul()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):1;
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_type(arg2,2,2,0);
              _cimg_mp_check_const_scalar(arg3,3,3);
              p1 = size(arg1);
              p2 = size(arg2);
              p3 = (unsigned int)mem[arg3];
              arg5 = p2/p3;
              arg4 = p1/arg5;
              if (arg4*arg5!=p1 || arg5*p3!=p2) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Types of first and second arguments ('%s' and '%s') "
                                            "do not match with third argument 'nb_colsB=%u', "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s_type(arg1)._data,s_type(arg2)._data,p3,s0);
              }
              pos = vector(arg4*p3);
              CImg<ulongT>::vector((ulongT)mp_matrix_mul,pos,arg1,arg2,arg4,arg5,p3).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            break;

          case 'n' :
#ifdef cimg_mp_func_name
            if (!std::strncmp(ss,"name(",5)) { // Get image name as a string vector
              _cimg_mp_op("Function 'name()'");
              if (*ss5=='#') { // Index specified
                s0 = ss6; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss6,s0++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss6);
                _cimg_mp_check_list();
                is_sth = true; // is_index_specified?
              } else { s0 = ss5; p1 = get_mem_img_index(); is_sth = false; }
              arg1 = s0<se1?compile(s0,se1,depth1,0,block_flags):~0U;
              if (arg1!=~0U) {
                _cimg_mp_check_const_scalar(arg1,is_sth?2:1,3);
                arg1 = (unsigned int)mem[arg1];
              } else arg1 = 1024;
              pos = vector(arg1);
              CImg<ulongT>::vector((ulongT)mp_name,pos,p1,arg1).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
#endif

            if (!std::strncmp(ss,"narg(",5)) { // Number of arguments
              _cimg_mp_op("Function 'narg()'");
              if (ss5>=se1) _cimg_mp_return(0);
              arg1 = 0;
              for (s = ss5; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                ++arg1; s = ns;
              }
              _cimg_mp_const_scalar((double)arg1);
            }

            if (!std::strncmp(ss,"noise(",6)) { // Add noise
              _cimg_mp_op("Function 'noise()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0;
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_type(arg2,2,1,0);
              _cimg_mp_check_type(arg3,3,1,0);
              p1 = size(arg1);
              pos = vector(p1);
              CImg<ulongT>::vector((ulongT)mp_noise,pos,arg1,p1,arg2,arg3).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"normalize(",10)) { // Normalize
              _cimg_mp_op("Function 'normalize()'");
              s0 = ss + 10;
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(s0,s1,depth1,0,block_flags);

              arg2 = arg4 = 0; arg3 = 1;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg3 = compile(s1,s2,depth1,0,block_flags);
                if (s2<se1) {
                  s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                  arg2 = arg3;
                  arg3 = compile(s2,s1,depth1,0,block_flags);
                  arg4 = s1<se1?compile(++s1,se1,depth1,0,block_flags):0;
                }
              }
              _cimg_mp_check_type(arg2,2,1,0);
              _cimg_mp_check_type(arg3,3,1,0);
              _cimg_mp_check_type(arg4,4,1,0);
              if (is_scalar(arg1)) _cimg_mp_scalar3(lerp,arg2,arg3,arg4);
              p1 = size(arg1);
              pos = vector(p1);
              CImg<ulongT>::vector((ulongT)mp_normalize,pos,arg1,p1,arg2,arg3,arg4).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"normp(",6)) { // Lp norm, with variable argument p
              _cimg_mp_op("Function 'normp()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags);
              arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):2;
              _cimg_mp_check_type(arg2,0,1,0);
              p1 = size(arg1);
              _cimg_mp_scalar3(normp,arg1,p1,arg2);
            }
            break;

          case 'o' :
            if (!std::strncmp(ss,"o2c(",4)) { // Offset to coordinates
              _cimg_mp_op("Function 'o2c()'");
              if (*ss4=='#') { // Index specified
                s0 = ss5; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss5,s0++,depth1,0,block_flags);
                p2 = 1;
                _cimg_mp_check_notnan_index(p1,ss5);
                _cimg_mp_check_list();
              } else { p1 = ~0U; s0 = ss4; p2 = 0; }
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(s0,s1,depth1,0,block_flags);
              arg2 = s1!=se1?compile(++s1,se1,depth1,0,block_flags):0;
              _cimg_mp_check_type(arg1,p2 + 1,1,0);
              _cimg_mp_check_type(arg2,p2 + 2,1,0);
              pos = vector(4);
              CImg<ulongT>::vector((ulongT)mp_o2c,pos,p1,arg1,arg2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            break;

          case 'p' :
            if (!std::strncmp(ss,"perm(",5)) { // Number of permutations
              _cimg_mp_op("Function 'perm()'");
              s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss5,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0;
              _cimg_mp_check_type(arg1,1,1,0);
              _cimg_mp_check_type(arg2,2,1,0);
              _cimg_mp_check_type(arg3,3,1,0);
              if (is_const_scalar(arg1) && is_const_scalar(arg2) && is_const_scalar(arg3))
                _cimg_mp_const_scalar(cimg::permutations((int)mem[arg1],(int)mem[arg2],(bool)mem[arg3]));
              _cimg_mp_scalar3(permutations,arg1,arg2,arg3);
            }

            if (!std::strncmp(ss,"permute(",8)) { // Permute axes
              _cimg_mp_op("Function 'permute()'");
              s1 = ss8; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss8,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg3 = compile(s2,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg4 = compile(s1,s2,depth1,0,block_flags);
              s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg5 = compile(s2,s1,depth1,0,block_flags);
              arg6 = compile(++s1,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_const_scalar(arg2,2,3);
              _cimg_mp_check_const_scalar(arg3,3,3);
              _cimg_mp_check_const_scalar(arg4,4,3);
              _cimg_mp_check_const_scalar(arg5,5,3);
              _cimg_mp_check_type(arg6,6,2,0);
              p1 = size(arg1);
              p2 = size(arg6);
              arg2 = (unsigned int)mem[arg2];
              arg3 = (unsigned int)mem[arg3];
              arg4 = (unsigned int)mem[arg4];
              arg5 = (unsigned int)mem[arg5];
              if (arg2*arg3*arg4*arg5!=std::max(1U,p1)) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Input vector size (%u values) and its specified "
                                            "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            std::max(p1,1U),arg2,arg3,arg4,arg5,arg2*arg3*arg4*arg5);
              }
              pos = vector(arg2*arg3*arg4*arg5);
              CImg<ulongT>::vector((ulongT)mp_permute,pos,arg1,arg2,arg3,arg4,arg5,arg6,p2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"polygon(",8)) { // Polygon/line drawing
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_op("Function 'polygon()'");
              if (*ss8=='#') { // Index specified
                s0 = ss + 9; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss + 9,s0++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss + 9);
                _cimg_mp_check_list();
                pos = 2;
              } else { p1 = ~0U; s0 = ss8; pos = 1; }
              if (s0==se1) compile(s0,se1,depth1,0,block_flags); // 'missing' argument error
              CImg<ulongT>::vector((ulongT)mp_polygon,_cimg_mp_slot_nan,0,p1).move_to(l_opcode);
              for (s = s0; s<se; ++s, ++pos) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg2 = compile(s,ns,depth1,0,block_flags);
                if (pos>1 && is_vector(arg2)) // Vector argument allowed to specify coordinates and color
                  CImg<ulongT>::sequence(size(arg2),arg2 + 1,
                                         arg2 + (ulongT)size(arg2)).
                    move_to(l_opcode);
                else {
                  _cimg_mp_check_type(arg2,pos,1,0);
                  CImg<ulongT>::vector(arg2).move_to(l_opcode);
                }
                s = ns;
              }
              (l_opcode>'y').move_to(opcode);
              opcode[2] = opcode._height;
              opcode.move_to(code);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"print(",6) ||
                !std::strncmp(ss,"prints(",7)) { // Print expressions
              s0 = ss6 + (*ss5=='('?0:1);
              is_sth = *ss5=='s'; // corresponding string must be printed?
              _cimg_mp_op(is_sth?"Function 'prints()'":"Function 'print()'");
              if (!is_sth && *s0=='#') { // Image
                p1 = compile(ss7,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss7);
                _cimg_mp_check_list();
                CImg<ulongT>::vector((ulongT)mp_image_print,_cimg_mp_slot_nan,p1).move_to(code);
                _cimg_mp_return_nan();
              }

              // Regular expression.
              for (s = s0; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                ref.assign(7);
                pos = compile(s,ns,depth1,ref,block_flags);
                c1 = *ns; *ns = 0;
                variable_name.assign(CImg<charT>::string(s,true,true).unroll('y'),true);
                cimg::strpare(variable_name,false,true);
                if (is_const_scalar(pos)) // Const scalar
                  std::fprintf(cimg::output(),"\n[" cimg_appname "_math_parser] %s = %.17g "
                               "(mem[%u]: %s%s)",
                               variable_name._data,mem[pos],pos,s_type(pos)._data,s_ref(ref)._data);
                else // Vector or non-const scalar
                  std::fprintf(cimg::output(),"\n[" cimg_appname "_math_parser] %s = (uninitialized) "
                               "(mem[%u]: %s%s)",
                               variable_name._data,pos,s_type(pos)._data,s_ref(ref)._data);

                if (p_ref) std::memcpy(p_ref,ref,siz_ref);

                if (is_vector(pos)) // Vector
                  ((CImg<ulongT>::vector((ulongT)mp_vector_print,pos,0,(ulongT)size(pos),is_sth?1:0),
                    variable_name)>'y').move_to(opcode);
                else // Scalar
                  ((CImg<ulongT>::vector((ulongT)mp_print,pos,0,is_sth?1:0),
                    variable_name)>'y').move_to(opcode);

                opcode[2] = opcode._height;
                opcode.move_to(code);
                *ns = c1; s = ns;
              }
              _cimg_mp_return(pos);
            }
            break;

          case 'q' :
            if (!std::strncmp(ss,"qr(",3)) { // QR decomposition
              _cimg_mp_op("Function 'qr()'");
              s1 = ss3; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss3,s1,depth1,0,block_flags); // A
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags); // nb_colsA
              arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):1; // is_reduced_form
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_const_scalar(arg2,2,3);
              _cimg_mp_check_const_scalar(arg3,3,2);

              p1 = size(arg1);
              p2 = (unsigned int)mem[arg2];
              p3 = p1/p2;
              arg3 = (unsigned int)mem[arg3];
              if (p3*p2!=p1) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Type of first argument ('%s') "
                                            "does not match with second argument 'nb_colsA=%u', "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s_type(arg1)._data,p2,s0);
              }
              pos = vector((p2 + p3)*(arg3?std::min(p2,p3):p3)); // Size of output
              CImg<ulongT>::vector((ulongT)mp_matrix_qr,pos,arg1,p2,p3,arg3).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            break;

          case 'r' :
            if (!std::strncmp(ss,"rad2deg(",8)) { // Degrees to radians
              _cimg_mp_op("Function 'rad2deg()'");
              arg1 = compile(ss8,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(rad2deg,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(mem[arg1]*180/cimg::PI);
              _cimg_mp_scalar1(rad2deg,arg1);
            }

            if ((cimg_sscanf(ss,"rand%u%c",&(arg1=~0U),&sep)==2 && sep=='(' && arg1>0) ||
                !std::strncmp(ss,"rand(#",6) ||
                (!std::strncmp(ss,"rand",4) && ss4<se1 && *ss4!='(' && (s=std::strchr(ss4,'('))!=0)) { // Random vector
              _cimg_mp_op("Function 'rand()'");
              p3 = 0;
              if (arg1==~0U && *ss4!='(') {
                arg1 = compile(ss4,s++,depth1,0,block_flags);
                _cimg_mp_check_const_scalar(arg1,0,3);
                arg1 = (unsigned int)mem[arg1];
              } else s = std::strchr(ss4,'(') + 1;

              if (arg1==~0U && *s=='#') { // Number of elements specified as first argument with '#'
                s0 = ++s; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                arg1 = compile(s,s0++,depth1,0,block_flags);
                _cimg_mp_check_const_scalar(arg1,1,3);
                arg1 = (unsigned int)mem[arg1];
                p3 = 1;
                s = s0;
              }

              arg2 = p2 = 0; arg3 = 1; arg4 = arg5 = ~0U;
              if (s<se1) {
                s1 = s + 1; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg3 = compile(s,s1,depth1,0,block_flags);
                if (s1<se1) {
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg2 = arg3;
                  arg3 = compile(s1,s2,depth1,0,block_flags);
                  if (s2<se1) {
                    s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                    arg4 = compile(s2,s1,depth1,0,block_flags);
                    arg5 = s1<se1?compile(++s1,se1,depth1,0,block_flags):~0U;
                    p2 = size(arg4);
                  }
                }
              }
              _cimg_mp_check_type(arg2,p3 + 1,1,0);
              _cimg_mp_check_type(arg3,p3 + 2,1,0);
              if (arg4!=~0U) _cimg_mp_check_type(arg4,p3 + 3,2,0);
              if (arg5!=~0U) _cimg_mp_check_type(arg5,p3 + 4,1,0);
              pos = vector(arg1);
              CImg<ulongT>::vector((ulongT)mp_vector_rand,pos,arg1,arg2,arg3,arg4,p2,arg5).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"ref(",4)) { // Variable declaration
              _cimg_mp_op("Function 'ref()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              if (s1>=se1 || !*s1) compile(s1,s1,depth1,0,block_flags); // Will throw missing argument error
              arg3 = compile(ss4,s1++,depth1,p_ref,block_flags|32);
              variable_name.assign(s1,(unsigned int)(se - s1)).back() = 0;
              cimg::strpare(variable_name,false,true);
              *se1 = 0;

              if (!cimg::is_varname(variable_name)) { // Invalid variable name
                cimg::strellipsize(variable_name,64);
                *se1 = ')';
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Invalid specified variable name '%s', "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            variable_name._data,s0);
              }
              get_variable_pos(variable_name,arg1,arg2);
              if (arg2!=~0U) reserved_label[arg2] = arg3;
              else if (arg1!=~0U) variable_pos[arg1] = arg3;
              else { // New variable
                if (variable_def._width>=variable_pos._width) variable_pos.resize(-200,1,1,1,0);
                variable_pos[variable_def._width] = arg3;
                variable_name.move_to(variable_def);
              }
              if (is_vector(arg3))
                set_reserved_vector(arg3); // Prevent from being used in further optimization
              else if (is_comp_scalar(arg3)) memtype[arg3] = -1;
              *se1 = ')';
              _cimg_mp_return(arg3);
            }

            if (!std::strncmp(ss,"repeat(",7)) { // Repeat
              _cimg_mp_op("Function 'repeat()'");
              s0 = ss7; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
#endif
