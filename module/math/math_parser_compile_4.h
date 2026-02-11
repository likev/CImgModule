#ifndef CIMG_MODULE_MATH_PARSER_COMPILE_4_H
#define CIMG_MODULE_MATH_PARSER_COMPILE_4_H

              arg1 = compile(ss7,s0,depth1,0,block_flags); // Number of iterations
              _cimg_mp_check_type(arg1,1,1,0);
              s1 = ++s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              p1 = code._width;

              if (s1<se1) { // Version with 3 arguments
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
              } else { // Version with 2 arguments
                arg2 = ~0U;
                arg3 = compile(s0,se1,depth1,0,block_flags);
              }
              // arg2 = variable slot, arg3 = fill expression.
              CImg<ulongT>::vector((ulongT)mp_repeat,arg3,arg1,arg2,code._width - p1).move_to(code,p1);
              _cimg_mp_return_nan();
            }

            if (!std::strncmp(ss,"resize(",7)) { // Vector or image resize
              _cimg_mp_op("Function 'resize()'");
              if (*ss7!='#') { // Vector
                pos = 1;
                for (s = ss7; s<se; ++s, ++pos) {
                  ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                                 (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                  arg2 = compile(s,ns,depth1,0,block_flags);
                  if (s!=ss7) _cimg_mp_check_type(arg2,pos,1,0);
                  CImg<ulongT>::vector(arg2).move_to(l_opcode);
                  s = ns;
                }
                (l_opcode>'y').move_to(opcode);
                if (opcode.height()<2) compile(s,se1,depth1,0,block_flags); // Not enough arguments -> throw exception
                arg1 = (unsigned int)opcode[0]; // Vector to resize
                p1 = size(arg1);

                if (opcode.height()<=4) { // Simple vector resize
                  arg2 = (unsigned int)opcode[1];
                  _cimg_mp_check_const_scalar(arg2,2,3);
                  arg2 = (unsigned int)mem[arg2];
                  arg3 = opcode.height()<3?1U:(unsigned int)opcode[2];
                  _cimg_mp_check_type(arg3,3,1,0);
                  arg4 = opcode.height()<4?0U:(unsigned int)opcode[3];
                  _cimg_mp_check_type(arg4,4,1,0);
                  if (p1 && p1==arg2) // Particular case: copy input (same size)
                    pos = copy(arg1);
                  else {
                    pos = vector(arg2);
                    CImg<ulongT>::vector((ulongT)mp_vector_resize,pos,arg2,arg1,p1,arg3,arg4).move_to(code);
                  }
                } else { // Advanced vector resize (vector viewed as an image)
                  // opcode = [ A, ow,oh,od,os, nw,nh,nd,ns, interp, boundary_cond, ax,ay,az,ac ]
                  //          [ 0   1  2  3  4   5  6  7  8       9             10  11 12 13 14 ]

                  if (opcode.height()<6) compile(s,se1,depth1,0,block_flags); // Not enough arguments -> throw exception
                  p2 = opcode.height();
                  opcode.resize(1,15,1,1,0);
                  if (p2<7) opcode[6] = opcode[2];
                  if (p2<8) opcode[7] = opcode[3];
                  if (p2<9) opcode[8] = opcode[4];
                  if (p2<10) opcode[9] = 1;
                  _cimg_mp_check_const_scalar(opcode[1],2,3);
                  _cimg_mp_check_const_scalar(opcode[2],3,3);
                  _cimg_mp_check_const_scalar(opcode[3],4,3);
                  _cimg_mp_check_const_scalar(opcode[4],5,3);
                  _cimg_mp_check_const_scalar(opcode[5],6,3);
                  _cimg_mp_check_const_scalar(opcode[6],7,3);
                  _cimg_mp_check_const_scalar(opcode[7],8,3);
                  _cimg_mp_check_const_scalar(opcode[8],9,3);
                  arg2 = (unsigned int)mem[opcode[1]]; opcode[1] = arg2;
                  arg3 = (unsigned int)mem[opcode[2]]; opcode[2] = arg3;
                  arg4 = (unsigned int)mem[opcode[3]]; opcode[3] = arg4;
                  arg5 = (unsigned int)mem[opcode[4]]; opcode[4] = arg5;
                  if (arg2*arg3*arg4*arg5!=std::max(1U,p1)) {
                    _cimg_mp_strerr;
                    throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                "CImg<%s>::%s: %s: Input vector size (%u values) and its specified "
                                                "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                                pixel_type(),_cimg_mp_calling_function,s_op,
                                                std::max(p1,1U),arg2,arg3,arg4,arg5,arg2*arg3*arg4*arg5);
                  }
                  arg2 = (unsigned int)mem[opcode[5]]; opcode[5] = arg2;
                  arg3 = (unsigned int)mem[opcode[6]]; opcode[6] = arg3;
                  arg4 = (unsigned int)mem[opcode[7]]; opcode[7] = arg4;
                  arg5 = (unsigned int)mem[opcode[8]]; opcode[8] = arg5;
                  if (arg2==opcode[1] && arg3==opcode[2] && arg4==opcode[3] && arg5==opcode[4]) // Particular case: copy
                    pos = copy(arg1);
                  else {
                    pos = vector(arg2*arg3*arg4*arg5);
                    opcode.resize(1,18,1,1,0,0,0,1);
                    opcode[0] = (ulongT)mp_vector_resize_ext;
                    opcode[1] = (ulongT)pos;
                    opcode[2] = (ulongT)p1;
                    opcode.move_to(code);
                  }
                }
                return_comp = true;
                _cimg_mp_return(pos);

              } else { // Image
                if (!is_inside_critical) is_parallelizable = false;
                s0 = ss8; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                p1 = compile(ss8,s0++,depth1,0,block_flags);
                _cimg_mp_check_list();
                l_opcode.assign(); // Don't use 'opcode': it can be modified by further calls to 'compile()'!
                CImg<ulongT>::vector((ulongT)mp_image_resize,_cimg_mp_slot_nan,p1,~0U,~0U,~0U,~0U,1,0,0,0,0,0).
                  move_to(l_opcode);
                pos = 0;
                for (s = s0; s<se && pos<10; ++s) {
                  ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                                 (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                  arg1 = compile(s,ns,depth1,0,block_flags);
                  _cimg_mp_check_type(arg1,pos + 2,1,0);
                  l_opcode(0,pos + 3) = arg1;
                  s = ns;
                  ++pos;
                }
                if (pos<1 || pos>10) {
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: %s arguments, in expression '%s'.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,
                                              pos<1?"Missing":"Too much",s0);
                }
                l_opcode[0].move_to(code);
                _cimg_mp_return_nan();
              }
            }

            if (!std::strncmp(ss,"reverse(",8)) { // Vector reverse
              _cimg_mp_op("Function 'reverse()'");
              arg1 = compile(ss8,se1,depth1,0,block_flags);
              if (!is_vector(arg1)) _cimg_mp_same(arg1);
              p1 = size(arg1);
              pos = vector(p1);
              CImg<ulongT>::vector((ulongT)mp_reverse,pos,arg1,p1).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"rol(",4) || !std::strncmp(ss,"ror(",4)) { // Bitwise rotation
              _cimg_mp_op(ss[2]=='l'?"Function 'rol()'":"Function 'ror()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1-expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):1;
              _cimg_mp_check_type(arg2,2,1,0);
              if (is_vector(arg1)) _cimg_mp_return(vector2_vs(*ss2=='l'?mp_rol:mp_ror,arg1,arg2));
              if (is_const_scalar(arg1) && is_const_scalar(arg2))
                _cimg_mp_const_scalar(*ss2=='l'?cimg::rol(mem[arg1],(unsigned int)mem[arg2]):
                                      cimg::ror(mem[arg1],(unsigned int)mem[arg2]));
              _cimg_mp_return(scalar2(*ss2=='l'?mp_rol:mp_ror,arg1,arg2));
            }

            if (!std::strncmp(ss,"rot(",4)) { // 2D/3D rotation matrix
              _cimg_mp_op("Function 'rot()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              if (s1<se1) { // 3D rotation
                _cimg_mp_check_type(arg1,1,3,3);
                is_sth = false; // Is coordinates as vector?
                if (is_vector(arg1)) { // Coordinates specified as a vector
                  is_sth = true;
                  p2 = size(arg1);
                  ++arg1;
                  arg2 = arg3 = 0;
                  if (p2>1) {
                    arg2 = arg1 + 1;
                    if (p2>2) arg3 = arg2 + 1;
                  }
                  arg4 = compile(++s1,se1,depth1,0,block_flags);
                } else {
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg2 = compile(s1,s2,depth1,0,block_flags);
                  s3 = ++s2; while (s3<se1 && (*s3!=',' || level[s3 - expr._data]!=clevel1)) ++s3;
                  arg3 = compile(s2,s3,depth1,0,block_flags);
                  arg4 = compile(++s3,se1,depth1,0,block_flags);
                  _cimg_mp_check_type(arg2,2,1,0);
                  _cimg_mp_check_type(arg3,3,1,0);
                }
                _cimg_mp_check_type(arg4,is_sth?2:4,1,0);
                pos = vector(9);
                CImg<ulongT>::vector((ulongT)mp_rot3d,pos,arg1,arg2,arg3,arg4).move_to(code);
              } else { // 2D rotation
                _cimg_mp_check_type(arg1,1,1,0);
                pos = vector(4);
                CImg<ulongT>::vector((ulongT)mp_rot2d,pos,arg1).move_to(code);
              }
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"round(",6)) { // Value rounding
              _cimg_mp_op("Function 'round()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags);
              arg2 = 1;
              arg3 = 0;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg2 = compile(s1,s2,depth1,0,block_flags);
                arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0;
              }
              _cimg_mp_check_type(arg2,2,1,0);
              _cimg_mp_check_type(arg3,3,1,0);
              if (is_vector(arg1)) _cimg_mp_vector3_vss(round,arg1,arg2,arg3);
              if (is_const_scalar(arg1) && is_const_scalar(arg2) && is_const_scalar(arg3))
                _cimg_mp_const_scalar(cimg::round(mem[arg1],mem[arg2],(int)mem[arg3]));
              _cimg_mp_scalar3(round,arg1,arg2,arg3);
            }

#ifdef cimg_mp_func_run
            if (!std::strncmp(ss,"run(",4)) { // Run external command
              _cimg_mp_op("Function 'run()'");
              if (!is_inside_critical && !is_inside_begin && !is_inside_end) is_noncritical_run = true;
              CImg<ulongT>::vector((ulongT)mp_run,0,0).move_to(l_opcode);
              pos = 1;
              for (s = ss4; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg1 = compile(s,ns,depth1,0,block_flags);
                CImg<ulongT>::vector(arg1,size(arg1)).move_to(l_opcode);
                s = ns;
              }
              (l_opcode>'y').move_to(opcode);
              pos = scalar();
              opcode[1] = pos;
              opcode[2] = opcode._height;
              opcode.move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
#endif
            break;

          case 's' :
            if (*ss1=='(') { // Image spectrum
              _cimg_mp_op("Function 's()'");
              if (*ss2=='#') { // Index specified
                p1 = compile(ss3,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss3);
                _cimg_mp_check_list();
              } else { if (ss2!=se1) break; p1 = ~0U; }
              _cimg_mp_scalar1(image_spectrum,p1);
            }

            if (!std::strncmp(ss,"s2v(",4)) { // String to double
              _cimg_mp_op("Function 's2v()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              arg2 = arg3 = 0;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg2 = compile(s1,s2,depth1,0,block_flags);
                arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0;
              }
              _cimg_mp_check_type(arg2,2,1,0);
              _cimg_mp_check_type(arg3,3,1,0);
              p1 = size(arg1);
              pos = scalar();
              CImg<ulongT>::vector((ulongT)mp_s2v,pos,arg1,p1,arg2,arg3).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"same(",5)) { // Test if operands have the same values
              _cimg_mp_op("Function 'same()'");
              s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss5,s1,depth1,0,block_flags);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              arg3 = 11;
              arg4 = 1;
              if (s2<se1) {
                s3 = ++s2; while (s3<se1 && (*s3!=',' || level[s3 - expr._data]!=clevel1)) ++s3;
                arg3 = compile(s2,s3,depth1,0,block_flags);
                _cimg_mp_check_type(arg3,3,1,0);
                arg4 = s3<se1?compile(++s3,se1,depth1,0,block_flags):1;
              }
              p1 = size(arg1);
              p2 = size(arg2);
              _cimg_mp_scalar6(vector_eq,arg1,p1,arg2,p2,arg3,arg4);
            }

#ifdef cimg_mp_func_set
            if (!std::strncmp(ss,"set(",4)) { // Set value to external variable
              _cimg_mp_op("Function 'set()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              arg2 = compile(++s1,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,2,0);
              p1 = size(arg1);
              p2 = size(arg2);
              CImg<ulongT>::vector((ulongT)mp_set,arg2,p2,arg1,p1).move_to(code);
              _cimg_mp_return_nan();
            }
#endif

            if (!std::strncmp(ss,"shift(",6)) { // Shift vector
              _cimg_mp_op("Function 'shift()'");

              // Parse list of arguments.
              CImg<unsigned int> args(12,1,1,1,0);
              p1 = 0;
              for (s = ss6; s<se && p1<9; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg1 = compile(s,ns,depth1,0,block_flags);
                args[p1++] = arg1;
                s = ns;
              }
              if (s<se1) args[p1++] = compile(s,se1,depth1,0,block_flags); // Last argument

              if (p1<4) { // Shift vector
                if (p1<1) compile(s,se1,depth1,0,block_flags); // -> Error, missing arguments
                arg1 = args[0];
                arg2 = p1>1?args[1]:1;
                arg3 = p1>2?args[2]:0;
                _cimg_mp_check_type(arg1,1,2,0);
                _cimg_mp_check_type(arg2,2,1,0);
                _cimg_mp_check_type(arg3,3,1,0);
                p1 = size(arg1);
                pos = vector(p1);
                CImg<ulongT>::vector((ulongT)mp_vector_shift,pos,arg1,p1,arg2,arg3).move_to(code);
                return_comp = true;
                _cimg_mp_return(pos);
              } else { // Shift image-valued vector
                if (p1<5) compile(s,se1,depth1,0,block_flags); // -> Error, missing arguments
                _cimg_mp_check_type(args[0],1,2,0); // I
                _cimg_mp_check_const_scalar(args[1],2,3); // w
                _cimg_mp_check_const_scalar(args[2],3,3); // h
                _cimg_mp_check_const_scalar(args[3],4,3); // d
                _cimg_mp_check_const_scalar(args[4],5,3); // s
                args[1] = mem[args[1]];
                args[2] = mem[args[2]];
                args[3] = mem[args[3]];
                args[4] = mem[args[4]];
                arg1 = p1>5?args[5]:1; // dx
                arg2 = p1>6?args[6]:0; // dy
                arg3 = p1>7?args[7]:0; // dz
                arg4 = p1>8?args[8]:0; // dc
                arg5 = p1>9?args[9]:0; // boundary_conditions
                _cimg_mp_check_type(arg1,6,1,0);
                _cimg_mp_check_type(arg2,7,1,0);
                _cimg_mp_check_type(arg3,8,1,0);
                _cimg_mp_check_type(arg4,9,1,0);
                _cimg_mp_check_type(arg5,10,1,0);
                p1 = size(args[0]);
                p2 = args[1]*args[2]*args[3]*args[4];
                if (p1!=p2) {
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Vector size (%u values) and its specified "
                                              "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,
                                              std::max(p1,1U),args[1],args[2],args[3],args[4],
                                              args[1]*args[2]*args[3]*args[4]);
                }
                pos = vector(p1);
                CImg<ulongT>::vector((ulongT)mp_image_shift,pos,args[0],
                                     args[1],args[2],args[3],args[4],
                                     arg1,arg2,arg3,arg4,arg5).move_to(code);
                return_comp = true;
                _cimg_mp_return(pos);
              }
            }

            if (!std::strncmp(ss,"sigmoid(",8)) { // Sigmoid
              _cimg_mp_op("Function 'sigmoid()'");
              arg1 = compile(ss8,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(sigmoid,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::sigmoid(mem[arg1]));
              _cimg_mp_scalar1(sigmoid,arg1);
            }

            if (!std::strncmp(ss,"sign(",5)) { // Sign
              _cimg_mp_op("Function 'sign()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(sign,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::sign(mem[arg1]));
              _cimg_mp_scalar1(sign,arg1);
            }

            if (!std::strncmp(ss,"sin(",4)) { // Sine
              _cimg_mp_op("Function 'sin()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(sin,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::sin(mem[arg1]));
              _cimg_mp_scalar1(sin,arg1);
            }

            if (!std::strncmp(ss,"sinc(",5)) { // Sine cardinal
              _cimg_mp_op("Function 'sinc()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(sinc,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::sinc(mem[arg1]));
              _cimg_mp_scalar1(sinc,arg1);
            }

            if (!std::strncmp(ss,"sinh(",5)) { // Hyperbolic sine
              _cimg_mp_op("Function 'sinh()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(sinh,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::sinh(mem[arg1]));
              _cimg_mp_scalar1(sinh,arg1);
            }

            if (!std::strncmp(ss,"size(",5)) { // Vector size
              _cimg_mp_op("Function 'size()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              _cimg_mp_const_scalar(is_scalar(arg1)?0:size(arg1));
            }

            if (!std::strncmp(ss,"softargmax(",11) || // Softargmax & Softargmin
                !std::strncmp(ss,"softargmin(",11)) {
              _cimg_mp_op(*ss8=='a'?"Function 'softargmax()'":"Function 'softargmin()'");
              s0 = ss + 11;
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(s0,s1,depth1,0,block_flags);
              arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):1;
              _cimg_mp_check_type(arg2,2,1,0);
              p1 = size(arg1);
              if (!p1) _cimg_mp_return(0);
              _cimg_mp_return(scalar3(*ss8=='a'?mp_softargmax:mp_softargmin,arg1,p1,arg2));
            }

            if (!std::strncmp(ss,"softmax(",8) || !std::strncmp(ss,"softmin(",8)) { // Softmax & softmin
              _cimg_mp_op(*ss5=='a'?"Function 'softmax()'":"Function 'softmin()'");
              s1 = ss8; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss8,s1,depth1,0,block_flags);
              arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):1;
              _cimg_mp_check_type(arg2,2,1,0);
              p1 = size(arg1);
              if (!p1) _cimg_mp_return(1);
              pos = is_comp_vector(arg1)?arg1:((return_comp = true), vector(p1));
              CImg<ulongT>::vector((ulongT)(*ss5=='a'?mp_softmax:mp_softmin),pos,arg1,p1,arg2).move_to(code);
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"solve(",6)) { // Solve square linear system
              _cimg_mp_op("Function 'solve()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags);
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
              arg6 = p1/arg5;
              if (arg6*arg5!=p1 || arg5*p3!=p2) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Types of first and second arguments ('%s' and '%s') "
                                            "do not match with third argument 'nb_colsB=%u', "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s_type(arg1)._data,s_type(arg2)._data,p3,s0);
              }
              pos = vector(arg6*p3);
              CImg<ulongT>::vector((ulongT)mp_solve,pos,arg1,arg2,arg6,arg5,p3).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"sort(",5)) { // Sort vector
              _cimg_mp_op("Function 'sort()'");
              s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss5,s1,depth1,0,block_flags);
              arg2 = arg4 = 1; arg3 = ~0U; arg5 = 0;
              if (s1<se1) {
                s0 = ++s1; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                arg2 = compile(s1,s0,depth1,0,block_flags);
                if (s0<se1) {
                  s1 = ++s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                  arg3 = compile(s0,s1,depth1,0,block_flags);
                  if (s1<se1) {
                    s0 = ++s1; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                    arg4 = compile(s1,s0,depth1,0,block_flags);
                    arg5 = s0<se1?compile(++s0,se1,depth1,0,block_flags):0;
                  }
                }
              }
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_type(arg2,2,1,0);
              if (arg3!=~0U) _cimg_mp_check_type(arg3,3,1,0);
              _cimg_mp_check_type(arg4,4,1,0);
              _cimg_mp_check_type(arg5,5,1,0);
              p1 = size(arg1);
              pos = vector(p1);
              CImg<ulongT>::vector((ulongT)mp_sort,pos,arg1,p1,arg2,arg3,arg4,arg5).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"sqr(",4)) { // Square
              _cimg_mp_op("Function 'sqr()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(sqr,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::sqr(mem[arg1]));
              _cimg_mp_scalar1(sqr,arg1);
            }

            if (!std::strncmp(ss,"sqrt(",5)) { // Square root
              _cimg_mp_op("Function 'sqrt()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(sqrt,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::sqrt(mem[arg1]));
              _cimg_mp_scalar1(sqrt,arg1);
            }

            if (!std::strncmp(ss,"srand(",6)) { // Set RNG seed
              _cimg_mp_op("Function 'srand()'");
              arg1 = ss6<se1?compile(ss6,se1,depth1,0,block_flags):~0U;
              if (arg1!=~0U) { _cimg_mp_check_type(arg1,1,1,0); _cimg_mp_scalar1(srand,arg1); }
              _cimg_mp_scalar0(srand0);
            }

            if (!std::strncmp(ss,"stats(",6)) { // Image/vector statistics
              _cimg_mp_op("Function 'stats()'");
              if (*ss6!='#' && ss6!=se1) { // Vector
                s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg1 = compile(ss6,s1,depth1,0,block_flags); // Vector
                _cimg_mp_check_type(arg1,1,2,0);
                p1 = size(arg1);
                arg2 = arg3 = arg4 = arg5 = 1;
                if (s1<se1) {
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg2 = compile(s1,s2,depth1,0,block_flags); // Width
                  _cimg_mp_check_const_scalar(2,arg2,3);
                  arg2 = (unsigned int)mem[arg2];
                  if (s2<se1) {
                    s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                    arg3 = compile(s2,s1,depth1,0,block_flags); // Height
                    _cimg_mp_check_const_scalar(3,arg3,3);
                    arg3 = (unsigned int)mem[arg3];
                    if (s1<se1) {
                      s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                      arg4 = compile(s1,s2,depth1,0,block_flags); // Depth
                      _cimg_mp_check_const_scalar(4,arg4,3);
                      arg4 = (unsigned int)mem[arg4];
                      arg5 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0; // Spectrum
                      _cimg_mp_check_const_scalar(5,arg5,3);
                      arg5 = (unsigned int)mem[arg5];
                    }
                  }
                } else arg2 = p1;
                p2 = arg2*arg3*arg4*arg5;
                if (p1!=p2) {
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Vector size (%u values) and its specified "
                                              "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,
                                              std::max(p1,1U),arg2,arg3,arg4,arg5,p2);
                }
                pos = vector(14);
                CImg<ulongT>::vector((ulongT)mp_vector_stats,pos,arg1,arg2,arg3,arg4,arg5).move_to(code);
              } else { // Image
                if (*ss6=='#') { // Index specified
                  p1 = compile(ss7,se1,depth1,0,block_flags);
                  _cimg_mp_check_notnan_index(p1,ss7);
                  _cimg_mp_check_list();
                } else { if (ss6!=se1) break; p1 = ~0U; }
                pos = vector(14);
                CImg<ulongT>::vector((ulongT)mp_image_stats,pos,p1).move_to(code);
              }
              return_comp = true;
              _cimg_mp_return(pos);
            }

#ifdef cimg_mp_func_store
            if (!std::strncmp(ss,"store(",6)) { // Store vector to variable
              _cimg_mp_op("Function 'store()'");
              s1 = ss6; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss6,s1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,2,2,0);
              p1 = size(arg1);
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              p2 = size(arg2);
              p3 = std::max(1U,p2);

              arg3 = ~0U; arg4 = arg5 = arg6 = 1U; pos = 0;
              if (s2<se1) {
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg3 = compile(s2,s1,depth1,0,block_flags);
                _cimg_mp_check_type(arg3,3,1,0);
                arg4 = arg5 = arg6 = 1U;
                if (s1<se1) {
                  s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                  arg4 = compile(s1,s2,depth1,0,block_flags);
                  _cimg_mp_check_type(arg4,4,1,0);
                  if (s2<se1) {
                    s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                    arg5 = compile(s2,s1,depth1,0,block_flags);
                    _cimg_mp_check_type(arg5,5,1,0);
                    if (s1<se1) {
                      s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                      arg6 = compile(s1,s2,depth1,0,block_flags);
                      pos = s2<se1?compile(++s2,se1,depth1,0,block_flags):0;
                      _cimg_mp_check_type(arg6,6,1,0);
                      _cimg_mp_check_type(pos,7,1,0);
                    }
                  }
                }
              }
              if (arg3==~0U) arg3 = const_scalar(p3);
              CImg<ulongT>::vector((ulongT)mp_store,_cimg_mp_slot_nan,arg2,p2,arg1,p1,
                                   arg3,arg4,arg5,arg6,pos).move_to(code);
              _cimg_mp_return_nan();
            }
#endif


            if (!std::strncmp(ss,"string(",7)) { // Construct string from list of arguments
              _cimg_mp_op("Function 'string()'");
              CImg<ulongT>::vector((ulongT)mp_string,0,0,0).move_to(l_opcode);

              if (*ss7=='#') { // Output vector size specified, with '#'
                s0 = ss8; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                arg1 = compile(ss8,s0++,depth1,0,block_flags);
                _cimg_mp_check_const_scalar(arg1,1,3);
                arg1 = (unsigned int)mem[arg1];
                s = s0;
              } else { arg1=~0U; s = ss7; }

              p1 = 0;
              for (; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg2 = compile(s,ns,depth1,0,block_flags);
                p2 = size(arg2);
                if (p2) p1+=p2;
                else {
                  if (is_const_scalar(arg2)) p1+=cimg_snprintf(variable_name.assign(24),24,"%.17g",mem[arg2]);
                  else p1+=23;
                }
                CImg<ulongT>::vector(arg2,p2).move_to(l_opcode);
                s = ns;
              }
              if (arg1==~0U) arg1 = p1;
              pos = vector(arg1,0);
              (l_opcode>'y').move_to(opcode);
              opcode[1] = pos;
              opcode[2] = arg1;
              opcode[3] = opcode._height;
              opcode.move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"svd(",4)) { // Matrix SVD
              _cimg_mp_op("Function 'svd()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):1;
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_const_scalar(arg2,2,3);
              p1 = size(arg1);
              p2 = (unsigned int)mem[arg2];
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
              pos = vector(p1 + p2 + p2*p2);
              CImg<ulongT>::vector((ulongT)mp_matrix_svd,pos,arg1,p2,p3).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"swap(",5)) { // Swap values
              _cimg_mp_op("Function 'swap()'");

              if (*ss5=='#') { // Swap image values
                s0 = ss6; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0; // #ind
                p1 = compile(ss6,s0++,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss6);
                _cimg_mp_check_list();
                s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1; // pos0
                arg1 = compile(s0,s1,depth1,0,block_flags);
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2; // pos1
                arg2 = compile(s1,s2,depth1,0,block_flags);
                arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0; // is_vector?
                CImg<ulongT>::vector((ulongT)mp_image_swap,_cimg_mp_slot_nan,p1,arg1,arg2,arg3).move_to(code);

              } else {
                s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                ref.assign(14);
                arg1 = compile(ss5,s1,depth1,ref,block_flags);
                arg2 = compile(++s1,se1,depth1,ref._data + 7,block_flags);
                p1 = size(arg1);
                _cimg_mp_check_type(arg2,2,p1?2:1,p1);
                if (is_const_scalar(arg1) || is_const_scalar(arg2)) {
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: %s argument cannot be a constant, "
                                              "in expression '%s'.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,
                                              is_const_scalar(arg1)?"First":"Second",s0);
                }
                CImg<ulongT>::vector((ulongT)mp_swap,arg1,arg2,p1).move_to(code);

                // Write back values of linked arg1 and arg2.
                const unsigned int *_ref = ref;
                is_sth = true; // Is first argument?
                do {
                  switch (*_ref) {
                  case 1 : // arg1: V[k]
                    arg3 = _ref[1]; // Vector slot
                    arg4 = _ref[2]; // Index
                    CImg<ulongT>::vector((ulongT)mp_vector_set_off,arg1,arg3,(ulongT)size(arg3),arg4).
                      move_to(code);
                    break;
                  case 2 : // arg1: i/j[_#ind,off]
                    if (!is_inside_critical) is_parallelizable = false;
                    p1 = _ref[1]; // Index
                    is_relative = (bool)_ref[2];
                    arg3 = _ref[3]; // Offset
                    if ((p1==~0U && imgout) || (p1!=~0U && imglist))
                      CImg<ulongT>::vector((ulongT)mp_set_ijoff,arg1,(ulongT)is_relative,
                                           p1,arg3).move_to(code);
                    break;
                  case 3 : // arg1: i/j(_#ind,_x,_y,_z,_c)
                    if (!is_inside_critical) is_parallelizable = false;
                    p1 = _ref[1]; // Index
                    is_relative = (bool)_ref[2];
                    arg3 = _ref[3]; // X
                    arg4 = _ref[4]; // Y
                    arg5 = _ref[5]; // Z
                    arg6 = _ref[6]; // C
                    if ((p1==~0U && imgout) || (p1!=~0U && imglist))
                      CImg<ulongT>::vector((ulongT)mp_set_ijxyzc,arg1,(ulongT)is_relative,
                                           p1,arg3,arg4,arg5,arg6).move_to(code);
                    break;
                  case 4: // arg1: I/J[_#ind,off]
                    if (!is_inside_critical) is_parallelizable = false;
                    p1 = _ref[1]; // Index
                    is_relative = (bool)_ref[2];
                    arg3 = _ref[3]; // Offset
                    if ((p1==~0U && imgout) || (p1!=~0U && imglist)) {
                      if (is_scalar(arg1))
                        CImg<ulongT>::vector((ulongT)mp_set_IJoff_s,arg1,(ulongT)is_relative,
                                             p1,arg3).move_to(code);
                      else {
                        _cimg_mp_check_const_index(p1);
                        CImg<ulongT>::vector((ulongT)mp_set_IJoff_v,arg1,(ulongT)is_relative,
                                             p1,arg3,size(arg1)).move_to(code);
                      }
                    }
                    break;
                  case 5 : // arg1: I/J(_#ind,_x,_y,_z,_c)
                    if (!is_inside_critical) is_parallelizable = false;
                    p1 = _ref[1]; // Index
                    is_relative = (bool)_ref[2];
                    arg3 = _ref[3]; // X
                    arg4 = _ref[4]; // Y
                    arg5 = _ref[5]; // Z
                    if ((p1==~0U && imgout) || (p1!=~0U && imglist)) {
                      if (is_scalar(arg1))
                        CImg<ulongT>::vector((ulongT)mp_set_IJxyz_s,arg1,(ulongT)is_relative,
                                             p1,arg3,arg4,arg5).move_to(code);
                      else {
                        _cimg_mp_check_const_index(p1);
                        CImg<ulongT>::vector((ulongT)mp_set_IJxyz_v,arg1,(ulongT)is_relative,
                                             p1,arg3,arg4,arg5,size(arg1)).move_to(code);
                      }
                    }
                    break;
                  }

                  _ref+=7;
                  arg1 = arg2;
                  is_sth = !is_sth;
                } while (!is_sth);

                if (p_ref) std::memcpy(p_ref,ref,siz_ref);
              }

              _cimg_mp_return_nan();
            }
            break;

          case 't' :
            if (!std::strncmp(ss,"tan(",4)) { // Tangent
              _cimg_mp_op("Function 'tan()'");
              arg1 = compile(ss4,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(tan,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::tan(mem[arg1]));
              _cimg_mp_scalar1(tan,arg1);
            }

            if (!std::strncmp(ss,"tanh(",5)) { // Hyperbolic tangent
              _cimg_mp_op("Function 'tanh()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(tanh,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(std::tanh(mem[arg1]));
              _cimg_mp_scalar1(tanh,arg1);
            }

            if (!std::strncmp(ss,"trace(",6)) { // Matrix trace
              _cimg_mp_op("Function 'trace()'");
              arg1 = compile(ss6,se1,depth1,0,block_flags);
              _cimg_mp_check_matrix_square(arg1,1);
              p1 = (unsigned int)cimg::round(std::sqrt((float)size(arg1)));
              _cimg_mp_scalar2(trace,arg1,p1);
            }

            if (!std::strncmp(ss,"transpose(",10)) { // Matrix transpose
              _cimg_mp_op("Function 'transpose()'");
              s1 = ss + 10; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss + 10,s1,depth1,0,block_flags);
              arg2 = compile(++s1,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,2,0);
              _cimg_mp_check_const_scalar(arg2,2,3);
              p1 = size(arg1);
              p2 = (unsigned int)mem[arg2];
              p3 = p1/p2;
              if (p2*p3!=p1) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Size of first argument ('%s') does not match "
                                            "second argument 'nb_cols=%u', in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            s_type(arg1)._data,p2,s0);
              }
              pos = vector(p3*p2);
              CImg<ulongT>::vector((ulongT)mp_transpose,pos,arg1,p2,p3).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            break;

          case 'u' :
            if (!std::strncmp(ss,"ui2f(",5)) { // Special uint->float[16,32] conversion
              _cimg_mp_op("Function 'ui2f()'");
              arg1 = compile(ss5,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(ui2f,arg1);
              if (is_const_scalar(arg1))
                _cimg_mp_const_scalar((double)cimg::uint2float(mem[arg1],(T)0));
              _cimg_mp_scalar1(ui2f,arg1);
            }

            if (!std::strncmp(ss,"unitnorm(",9)) { // Normalize vector to unit norm
              _cimg_mp_op("Function 'unitnorm()'");
              s0 = ss + 9;
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(s0,s1,depth1,0,block_flags);
              arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):2;
              _cimg_mp_check_type(arg2,2,1,0);
              p1 = size(arg1);
              if (p1>0) pos = is_comp_vector(arg1)?arg1:((return_comp = true), vector(p1));
              else {
                pos = scalar();
                if (is_const_scalar(arg1) && is_const_scalar(arg2)) {
                  val = mem[arg1];
                  _cimg_mp_const_scalar(val?(mem[arg2]?1:val):0);
                }
              }
              CImg<ulongT>::vector((ulongT)mp_unitnorm,pos,arg1,p1,arg2).move_to(code);
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"unref(",6)) { // Un-reference variable
              _cimg_mp_op("Function 'unref()'");
              arg1=~0U;
              for (s0 = ss6; s0<se1; s0 = s1) {
                if (s0>ss6 && *s0==',') ++s0;
                s1 = s0; while (s1<se1 && *s1!=',') ++s1;
                c1 = *s1;
                if (s1>s0) {
                  *s1 = 0;
                  get_variable_pos(s0,arg1,arg2);
                  if (arg2!=~0U) reserved_label[arg2] = ~0U;
                  else if (arg1!=~0U) {
                    variable_def.remove(arg1);
                    if (arg1<variable_pos._width - 1)
                      std::memmove(variable_pos._data + arg1,variable_pos._data + arg1 + 1,
                                   sizeof(uintT)*(variable_pos._width - arg1 - 1));
                    --variable_pos._width;
                  }
                  *s1 = c1;
                } else compile(s0,s1,depth1,0,block_flags); // Will throw a 'missing argument' exception
              }
              _cimg_mp_return(arg1!=~0U?arg1:_cimg_mp_slot_nan); // Return value of last specified variable
            }

            if (!std::strncmp(ss,"uppercase(",10)) { // Upper case
              _cimg_mp_op("Function 'uppercase()'");
              arg1 = compile(ss + 10,se1,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(uppercase,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar(cimg::uppercase(mem[arg1]));
              _cimg_mp_scalar1(uppercase,arg1);
            }
            break;

          case 'v' :
            if (!std::strncmp(ss,"v2s(",4)) { // Double(s) to string
              _cimg_mp_op("Function 'v2s()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              arg2 = 0; arg3 = ~0U;
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg2 = compile(s1,s2,depth1,0,block_flags);
                arg3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):~0U;
              }
              _cimg_mp_check_type(arg2,2,1,0);
              if (arg3==~0U) { // Auto-guess best output vector size
                p1 = size(arg1);
                p1 = p1?25*p1 - 1:24;
              } else {
                _cimg_mp_check_const_scalar(arg3,3,3);
                p1 = (unsigned int)mem[arg3];
              }
              pos = vector(p1);
              CImg<ulongT>::vector((ulongT)mp_v2s,pos,p1,arg1,size(arg1),arg2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if ((cimg_sscanf(ss,"vector%u%c",&(arg1=~0U),&sep)==2 && sep=='(' && arg1>0) ||
                !std::strncmp(ss,"vector(",7) ||
                (!std::strncmp(ss,"vector",6) && ss7<se1 && (s=std::strchr(ss7,'('))!=0)) { // Vector
              _cimg_mp_op("Function 'vector()'");
              arg2 = 0; // Number of specified values
              if (arg1==~0U && *ss6!='(') {
                arg1 = compile(ss6,s++,depth1,0,block_flags);
                _cimg_mp_check_const_scalar(arg1,0,3);
                arg1 = (unsigned int)mem[arg1];
              } else s = std::strchr(ss6,'(') + 1;

              if (arg1==~0U && *s=='#') { // Number of elements specified as first argument with '#'
                s0 = ++s; while (s0<se1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                arg1 = compile(s,s0++,depth1,0,block_flags);
                _cimg_mp_check_const_scalar(arg1,1,3);
                arg1 = (unsigned int)mem[arg1];
                s = s0;
              }

              is_sth = !is_new_variable_assignment; // Can vector be defined once in 'begin()'?
              if (s<se1 || arg1==~0U) for ( ; s<se; ++s) {
                  ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                                 (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                  const CImgList<ulongT> &rcode = is_inside_begin?code:code_begin;
                  p1 = rcode.size();
                  p2 = variable_def.size();
                  arg3 = compile(s,ns,depth1,0,block_flags);
                  p3 = rcode.size();
                  if (is_vector(arg3)) {
                    arg4 = size(arg3);
                    CImg<ulongT>::sequence(arg4,arg3 + 1,arg3 + arg4).move_to(l_opcode);
                    arg2+=arg4;
                    const CImg<ulongT> &rcode_back = rcode.back();
                    is_sth&=p3>p1 && rcode_back[1]==arg3 &&
                      (rcode_back[0]==(ulongT)mp_string_init ||
                       rcode_back[0]==(ulongT)mp_vector_init) && variable_def.size()==p2 && !is_comp_vector(arg3);
                    // ^^ Tricky part: detect if 'arg2' is a newly constructed vector not assigned to a variable
                    // (i.e. a vector-valued literal).
                  } else {
                    CImg<ulongT>::vector(arg3).move_to(l_opcode);
                    ++arg2;
                    is_sth&=is_const_scalar(arg3);
                  }
                  s = ns;
                }
              if (arg1==~0U) arg1 = arg2;
              if (!arg1) _cimg_mp_return(0);
              pos = vector(arg1);
              l_opcode.insert(CImg<ulongT>::vector((ulongT)mp_vector_init,pos,0,arg1),0);
              (l_opcode>'y').move_to(opcode);
              opcode[2] = opcode._height;
              opcode.move_to(!is_sth || is_inside_begin || is_new_variable_assignment?code:code_begin);
              return_comp = !is_sth && is_new_variable_assignment;
              if (!return_comp) set_reserved_vector(pos); // Prevent from being used in further optimization
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"vmax(",5) || !std::strncmp(ss,"vmin(",5) ||
                !std::strncmp(ss,"vmaxabs(",8) || !std::strncmp(ss,"vminabs(",8) ||
                !std::strncmp(ss,"vmed(",5) || !std::strncmp(ss,"vkth(",5) ||
                !std::strncmp(ss,"vsum(",5) || !std::strncmp(ss,"vavg(",5) ||
                !std::strncmp(ss,"vstd(",5) || !std::strncmp(ss,"vvar(",5) ||
                !std::strncmp(ss,"vprod(",6) ||
                !std::strncmp(ss,"vargmin(",8) || !std::strncmp(ss,"vargmax(",8) ||
                !std::strncmp(ss,"vargminabs(",11) || !std::strncmp(ss,"vargmaxabs(",11) ||
                !std::strncmp(ss,"vargkth(",8)) { // Multi-argument vector functions
              _cimg_mp_op(ss[1]=='a'?(ss[2]=='v'?"Function 'vavg()'":
                                      ss[4]=='k'?"Function 'vargkth()'":
                                      ss[5]=='i' && ss[7]=='('?"Function 'vargmin()'":
                                      ss[5]=='i'?"Function vargminabs()'":
                                      ss[7]=='('?"Function 'vargmax()'":
                                      "Function 'vargmaxabs()'"):
                          ss[1]=='s'?(ss[2]=='u'?"Function 'vsum()'":"Function 'vstd()'"):
                          ss[1]=='k'?"Function 'vkth()'":
                          ss[1]=='p'?"Function 'vprod()'":
                          ss[1]=='v'?"Function 'vvar()'":
                          ss[2]=='i'?(ss[4]=='('?"Function 'vmin()'":
                                      "Function 'vminabs()'"):
                          ss[2]=='a'?(ss[4]=='('?"Function 'vmax()'":
                                      "Function 'vmaxabs()'"):
                          "Function 'vmed()'");
              op = ss[1]=='a'?(ss[2]=='v'?mp_vavg:
                               ss[4]=='k'?mp_vargkth:
                               ss[5]=='i' && ss[7]=='('?mp_vargmin:
                               ss[5]=='i'?mp_vargminabs:
                               ss[7]=='('?mp_vargmax:mp_vargmaxabs):
                ss[1]=='s'?(ss[2]=='u'?mp_vsum:mp_vstd):
                ss[1]=='k'?mp_vkth:
                ss[1]=='p'?mp_vprod:
                ss[1]=='v'?mp_vvar:
                ss[2]=='i'?(ss[4]=='('?mp_vmin:mp_vminabs):
                ss[2]=='a'?(ss[4]=='('?mp_vmax:mp_vmaxabs):
                mp_vmedian;
              CImg<ulongT>::vector((ulongT)op,0,0,0).move_to(l_opcode);
              p1 = ~0U;
              p3 = 1;
              for (s = std::strchr(ss,'(') + 1; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg2 = compile(s,ns,depth1,0,block_flags);
                p2 = size(arg2);
                if (p1==~0U) { if (is_vector(arg2)) p1 = p2; }
                else _cimg_mp_check_type(arg2,p3,3,p1);
                CImg<ulongT>::vector(arg2,p2).move_to(l_opcode);
                s = ns;
                ++p3;
              }
              (l_opcode>'y').move_to(opcode);
              if (p1==~0U) { pos = scalar(); p1 = 0; } else pos = vector(p1);
              opcode[1] = pos;
              opcode[2] = p1;
              opcode[3] = opcode._height;
              opcode.move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            break;

          case 'w' :
            if (*ss1=='(') { // Image width
              _cimg_mp_op("Function 'w()'");
              if (*ss2=='#') { // Index specified
                p1 = compile(ss3,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss3);
                _cimg_mp_check_list();
              } else { if (ss2!=se1) break; p1 = ~0U; }
              _cimg_mp_scalar1(image_width,p1);
            }

            if (!std::strncmp(ss,"warp(",5)) { // Image warping
              _cimg_mp_op("Function 'warp()'");
              pos = 1;
              for (s = ss5; s<se; ++s, ++pos) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                arg2 = compile(s,ns,depth1,0,block_flags);
                if (pos==1 || pos==6) _cimg_mp_check_type(arg2,pos,2,0);
                else if (pos<11) _cimg_mp_check_const_scalar(arg2,pos,3);
                else _cimg_mp_check_type(arg2,pos,1,0);
                CImg<ulongT>::vector(arg2).move_to(l_opcode);
                s = ns;
              }
              (l_opcode>'y').move_to(opcode);
              // opcode = [ A, wA,hA,dA,sA, B, wB,hB,dB,sB, mode, interp, boundary_cond ]
              //          [ 0   1  2  3  4  5   6  7  8  9    10      11             12 ]

              if (opcode.height()<10) compile(s,se1,depth1,0,block_flags); // Not enough arguments -> throw exception
              arg1 = (unsigned int)opcode[0]; // Image to warp
              arg2 = (unsigned int)opcode[5]; // Warp map
              p1 = size(arg1);
              p2 = size(arg2);
              p3 = opcode.height();
              opcode.resize(1,13,1,1,0);
              if (p3<11) opcode[10] = 0;
              if (p3<12) opcode[11] = 1;
              if (p3<13) opcode[12] = 0;
              arg3 = (unsigned int)mem[opcode[1]]; opcode[1] = arg3;
              arg4 = (unsigned int)mem[opcode[2]]; opcode[2] = arg4;
              arg5 = (unsigned int)mem[opcode[3]]; opcode[3] = arg5;
              arg6 = (unsigned int)mem[opcode[4]]; opcode[4] = arg6;
              if (arg3*arg4*arg5*arg6!=std::max(1U,p1)) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Input vector size (%u values) and its specified "
                                            "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            std::max(p1,1U),arg3,arg4,arg5,arg6,arg3*arg4*arg5*arg6);
              }
              arg3 = (unsigned int)mem[opcode[6]]; opcode[6] = arg3;
              arg4 = (unsigned int)mem[opcode[7]]; opcode[7] = arg4;
              arg5 = (unsigned int)mem[opcode[8]]; opcode[8] = arg5;
              arg6 = (unsigned int)mem[opcode[9]]; opcode[9] = arg6;
              if (arg3*arg4*arg5*arg6!=std::max(1U,p2)) {
                _cimg_mp_strerr;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: %s: Warp vector size (%u values) and its specified "
                                            "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                            pixel_type(),_cimg_mp_calling_function,s_op,
                                            std::max(p2,1U),arg3,arg4,arg5,arg6,arg3*arg4*arg5*arg6);
              }
              pos = vector(arg3*arg4*arg5*(unsigned int)opcode[4]);
              opcode.resize(1,15,1,1,0,0,0,1);
              opcode[0] = (ulongT)mp_warp;
              opcode[1] = (ulongT)pos;
              opcode.move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }

            if (!std::strncmp(ss,"wave(",5)) { // Wave function
              _cimg_mp_op("Function 'wave()'");
              s1 = ss5; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss5,s1,depth1,0,block_flags); // x
              arg2 = s1<se1?compile(++s1,se1,depth1,0,block_flags):4; // type
              _cimg_mp_check_type(arg2,2,1,0);
              if (is_vector(arg1)) _cimg_mp_vector2_vs(wave,arg1,arg2);
              if (is_const_scalar(arg1) && is_const_scalar(arg2)) // Optimize constant case
                _cimg_mp_const_scalar(cimg::wave(mem[arg1],(unsigned int)mem[arg2]));
              _cimg_mp_scalar2(wave,arg1,arg2);
            }

            if (*ss1=='h' && *ss2=='(') { // Image width*height
              _cimg_mp_op("Function 'wh()'");
              if (*ss3=='#') { // Index specified
                p1 = compile(ss4,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss4);
                _cimg_mp_check_list();
              } else { if (ss3!=se1) break; p1 = ~0U; }
              _cimg_mp_scalar1(image_wh,p1);
            }

            if (*ss1=='h' && *ss2=='d' && *ss3=='(') { // Image width*height*depth
              _cimg_mp_op("Function 'whd()'");
              if (*ss4=='#') { // Index specified
                p1 = compile(ss5,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss5);
                _cimg_mp_check_list();
              } else { if (ss4!=se1) break; p1 = ~0U; }
              _cimg_mp_scalar1(image_whd,p1);
            }

            if (!std::strncmp(ss,"while(",6)) { // While...do
              _cimg_mp_op("Function 'while()'");
              s0 = *ss5=='('?ss6:ss8;
              s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              p1 = code._width;
              arg1 = compile(s0,s1,depth1,0,block_flags);
              p2 = code._width;
              arg6 = mempos;
              pos = compile(++s1,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg1,1,1,0);
              arg2 = size(pos);
              CImg<ulongT>::vector((ulongT)mp_while,pos,arg1,p2 - p1,code._width - p2,arg2,
                                   pos>=arg6 && !is_const_scalar(pos),
                                   arg1>=arg6 && !is_const_scalar(arg1)).move_to(code,p1);
              _cimg_mp_return(pos);
            }

            if (*ss1=='h' && *ss2=='d' && *ss3=='s' && *ss4=='(') { // Image width*height*depth*spectrum
              _cimg_mp_op("Function 'whds()'");
              if (*ss5=='#') { // Index specified
                p1 = compile(ss6,se1,depth1,0,block_flags);
                _cimg_mp_check_notnan_index(p1,ss6);
                _cimg_mp_check_list();
              } else { if (ss5!=se1) break; p1 = ~0U; }
              _cimg_mp_scalar1(image_whds,p1);
            }
            break;

          case 'x' :
            if (!std::strncmp(ss,"xor(",4)) { // Xor
              _cimg_mp_op("Function 'xor()'");
              s1 = ss4; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg1 = compile(ss4,s1,depth1,0,block_flags);
              arg2 = compile(++s1,se1,depth1,0,block_flags);
              _cimg_mp_check_type(arg2,2,3,size(arg1));
              if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(bitwise_xor,arg1,arg2);
              if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(bitwise_xor,arg1,arg2);
              if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(bitwise_xor,arg1,arg2);
              if (is_const_scalar(arg1) && is_const_scalar(arg2))
                _cimg_mp_const_scalar((longT)mem[arg1] ^ (longT)mem[arg2]);
              _cimg_mp_scalar2(bitwise_xor,arg1,arg2);
            }
            break;
          }

          if (!std::strncmp(ss,"fft(",4) || !std::strncmp(ss,"ifft(",5)) { // FFT and iFFT
            is_sth = *ss=='i'; // is_ifft ?
            _cimg_mp_op(is_sth?"Function 'ifft()'":"Function 'fft()'");
            s0 = is_sth?ss5:ss4;
            s1 = s0; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
            arg1 = compile(s0,s1,depth1,0,block_flags); // Real part
            p1 = size(arg1);
            s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
            arg2 = compile(s1,s2,depth1,0,block_flags); // Imaginary part
            arg3 = ~0U; arg4 = arg5 = arg6 = 1; p3 = 0;
            if (s2<se1) {
              s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
              arg3 = compile(s2,s1,depth1,0,block_flags); // Width
              if (s1<se1) {
                s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                arg4 = compile(s1,s2,depth1,0,block_flags); // Height
                if (s2<se1) {
                  s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                  arg5 = compile(s2,s1,depth1,0,block_flags); // Depth
                  if (s1<se1) {
                    s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                    arg6 = compile(s1,s2,depth1,0,block_flags); // Spectrum
                    p3 = s2<se1?compile(++s2,se1,depth1,0,block_flags):0; // Axes
                  }
                }
              }
            }
            if (arg3!=~0U) {
              _cimg_mp_check_const_scalar(3,arg3,3);
              _cimg_mp_check_const_scalar(4,arg4,3);
              _cimg_mp_check_const_scalar(5,arg5,3);
              _cimg_mp_check_const_scalar(6,arg6,3);
              arg3 = (unsigned int)mem[arg3];
              arg4 = (unsigned int)mem[arg4];
              arg5 = (unsigned int)mem[arg5];
              arg6 = (unsigned int)mem[arg6];
            } else arg3 = p1;
            p2 = arg3*arg4*arg5*arg6;
            if (p1!=p2) {
              _cimg_mp_strerr;
              throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                          "CImg<%s>::%s: %s: Vector size (%u values) and its specified "
                                          "geometry (%u,%u,%u,%u) (%u values) do not match.",
                                          pixel_type(),_cimg_mp_calling_function,s_op,
                                          std::max(p1,1U),arg3,arg4,arg5,arg6,p2);
            }
            _cimg_mp_check_type(arg1,1,2,p2);
            _cimg_mp_check_type(arg2,2,2,p2);
            CImg<ulongT>::vector((ulongT)mp_fft,_cimg_mp_slot_nan,is_sth,arg1,arg2,arg3,arg4,arg5,arg6,p3,size(p3)).
              move_to(code);
            _cimg_mp_return_nan();
          }

          if ((!std::strncmp(ss,"norm",4) && ss4<se1 && (s = std::strchr(ss4,'('))!=0) || // Lp norm (constant p)
              (!std::strncmp(ss,"hypot(",6) && (s = ss5))) {
            const bool is_hypot = *ss=='h';
            _cimg_mp_op(is_hypot?"Function 'hypot()'":"Function 'norm()'");
            arg1 = !is_hypot && s!=ss4?compile(ss4,s,depth1,0,block_flags):2;
            _cimg_mp_check_const_scalar(arg1,0,0);
            val = mem[arg1];
            is_sth = true; // Tell if all arguments are constant
            CImg<ulongT>::vector(0,0,0,arg1).move_to(l_opcode);
            for (++s; s<se; ++s) {
              ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                             (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
              arg2 = compile(s,ns,depth1,0,block_flags);
              if (is_vector(arg2))
                CImg<ulongT>::sequence(size(arg2),arg2 + 1,arg2 + (ulongT)size(arg2)).
                  move_to(l_opcode);
              else CImg<ulongT>::vector(arg2).move_to(l_opcode);
              is_sth&=is_const_scalar(arg2);
              s = ns;
            }
            (l_opcode>'y').move_to(opcode);
            op = val==2?(is_hypot && opcode._height<8?mp_vector_hypot:mp_vector_norm2):
              val==1?mp_vector_norm1:!val?mp_vector_norm0:
              cimg::type<double>::is_inf(val)?mp_vector_norminf:mp_normp;
            opcode[0] = (ulongT)op;
            opcode[2] = opcode._height;
            if (is_sth) _cimg_mp_const_scalar(op(*this));
            if (opcode._height==5) { // Single argument
              if (arg1) { _cimg_mp_scalar1(abs,opcode[4]); } else { _cimg_mp_scalar2(neq,opcode[4],0); }
            }
            opcode[1] = pos = scalar();
            opcode.move_to(code);
            return_comp = true;
            _cimg_mp_return(pos);
          }

          if ((*ss=='u' || *ss=='v') && *ss1=='(') { // Random value with uniform distribution in specified range
            is_sth = *ss=='v'; // is integer generator?
            _cimg_mp_op(is_sth?"Function 'v()'":"Function 'u()'");
            if (*ss2==')') _cimg_mp_return(scalar0(is_sth?mp_rand_int_0_1:mp_rand_double_0_1));
            s1 = ss2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
            arg1 = compile(ss2,s1,depth1,0,block_flags);
            arg3 = arg4 = 1;
            if (s1<se1) {
              s2 = ++s1; while (s2<se1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
              arg2 = compile(s1,s2,depth1,0,block_flags);
              if (s2<se1) {
                s1 = ++s2; while (s1<se1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                arg3 = compile(s2,s1,depth1,0,block_flags);
                arg4 = s1<se1?compile(++s1,se1,depth1,0,block_flags):1;
              }
            } else { arg2 = arg1; arg1 = 0; }
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            _cimg_mp_check_type(arg3,3,1,0);
            _cimg_mp_check_type(arg4,4,1,0);
            if (arg3==1 && arg4==1) { // Fastest version (closed set)
              op = is_sth?mp_rand_int:mp_rand_double;
              if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_return(vector2_vv(op,arg1,arg2));
              if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_return(vector2_vs(op,arg1,arg2));
              if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_return(vector2_sv(op,arg1,arg2));
              if (arg1==arg2) _cimg_mp_same(arg1);
              if (arg2==1) {
                if (!arg1) _cimg_mp_return(scalar0(is_sth?mp_rand_int_0_1:mp_rand_double_0_1));
                if (arg1==11) _cimg_mp_return(scalar0(is_sth?mp_rand_int_m1_1:mp_rand_double_m1_1));
              } else if (!arg1) _cimg_mp_return(scalar1(is_sth?mp_rand_int_0_N:mp_rand_double_0_N,arg2));
              _cimg_mp_return(scalar2(op,arg1,arg2));
            } else { // Slower version (potentially an open set)
              op = is_sth?mp_rand_int_ext:mp_rand_double_ext;
              if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_return(vector4_vvss(op,arg1,arg2,arg3,arg4));
              if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_return(vector4_vsss(op,arg1,arg2,arg3,arg4));
              if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_return(vector4_svss(op,arg1,arg2,arg3,arg4));
              _cimg_mp_return(scalar4(op,arg1,arg2,arg3,arg4));
            }
          }

          if (!std::strncmp(ss,"argkth(",7) || !std::strncmp(ss,"avg(",4) ||
              !std::strncmp(ss,"argmax(",7) || !std::strncmp(ss,"argmaxabs(",10) ||
              !std::strncmp(ss,"argmin(",7) || !std::strncmp(ss,"argminabs(",10) ||
              !std::strncmp(ss,"gcd(",4) || !std::strncmp(ss,"kth(",4) || !std::strncmp(ss,"lcm(",4) ||
              !std::strncmp(ss,"max(",4) || !std::strncmp(ss,"maxabs(",7) || !std::strncmp(ss,"med(",4) ||
              !std::strncmp(ss,"min(",4) || !std::strncmp(ss,"minabs(",7) || !std::strncmp(ss,"prod(",5) ||
              !std::strncmp(ss,"std(",4) || !std::strncmp(ss,"sum(",4) || !std::strncmp(ss,"var(",4)
              ) { // Multi-argument functions
            _cimg_mp_op(*ss=='a'?(ss[3]=='k'?"Function 'argkth()'":
                                  ss[1]=='v'?"Function 'avg()'":
                                  ss[4]=='a'?(ss[6]=='('?"Function 'argmax()'":"Function 'argmaxabs()'"):
                                  ss[6]=='('?"Function 'argmin()'":"Function 'argminabs()'"):
                        *ss=='g'?"Function 'gcd()'":
                        *ss=='k'?"Function 'kth()'":
                        *ss=='l'?"Function 'lcm()'":
                        *ss=='m'?(ss[1]=='a'?(ss[3]=='('?"Function 'max()'":"Function 'maxabs()'"):
                                  ss[1]=='e'?"Function 'med()'":
                                  ss[3]=='('?"Function 'min()'":"Function 'minabs()'"):
                        *ss=='p'?"Function 'prod()'":
                        *ss=='s'?(ss[1]=='t'?"Function 'std()'":"Function 'sum()'"):
                        "Function 'var()'");

            op = *ss=='a'?(ss[3]=='k'?mp_argkth:
                           ss[1]=='v'?mp_avg:
                           ss[4]=='a'?(ss[6]=='('?mp_argmax:mp_argmaxabs):
                           ss[6]=='('?mp_argmin:mp_argminabs):
              *ss=='g'?mp_gcd:
              *ss=='k'?mp_kth:
              *ss=='l'?mp_lcm:
              *ss=='m'?(ss[1]=='a'?(ss[3]=='('?mp_max:mp_maxabs):
                        ss[1]=='e'?mp_med:
                        ss[3]=='('?mp_min:mp_minabs):
              *ss=='p'?mp_prod:
              *ss=='s'?(ss[1]=='t'?mp_std:mp_sum):
              mp_var;

            is_sth = true; // Tell if all arguments are constant
            bool is_scalar = true; // Tell if all arguments are scalars
            CImg<ulongT>::vector((ulongT)op,0,0).move_to(l_opcode);

            for (s = std::strchr(ss,'(') + 1; s<se; ++s) {
              ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                             (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
              arg2 = compile(s,ns,depth1,0,block_flags);
              if (is_vector(arg2)) {
                CImg<ulongT>::vector(arg2 + 1,size(arg2)).move_to(l_opcode);
                is_scalar = false;
              } else CImg<ulongT>::vector(arg2,1).move_to(l_opcode);
              is_sth&=is_const_scalar(arg2);
              s = ns;
            }
            (l_opcode>'y').move_to(opcode);
            opcode[2] = opcode._height;

            if (is_scalar && opcode[2]==5) { // Special optimizable case 'fn(a)'
              if (op==mp_avg || op==mp_gcd || op==mp_lcm || op==mp_max || op==mp_maxabs ||
                  op==mp_med || op==mp_min || op==mp_minabs || op==mp_prod || op==mp_sum)
                _cimg_mp_same(arg2);
              if (op==mp_argmax || op==mp_argmaxabs || op==mp_argmin || op==mp_argminabs)
                _cimg_mp_return(0);
            }
            if (is_scalar && opcode[2]==7) { // Special optimizable case 'fn(a,b)'
#define _cimg_mp_func2(fn) if (op==mp_##fn) { \
  if (is_sth) { opcode[2] = opcode[3]; opcode[3] = opcode[5]; _cimg_mp_const_scalar(mp_##fn##2(*this)); } \
                _cimg_mp_scalar2(fn##2,opcode[3],opcode[5]); }
              if (op==mp_kth) _cimg_mp_same(arg2);
              if (op==mp_argkth) _cimg_mp_return(1);
              _cimg_mp_func2(avg); _cimg_mp_func2(argmax); _cimg_mp_func2(argmaxabs); _cimg_mp_func2(argmin);
              _cimg_mp_func2(argminabs); _cimg_mp_func2(gcd); _cimg_mp_func2(lcm); _cimg_mp_func2(max);
              _cimg_mp_func2(maxabs); _cimg_mp_func2(med); _cimg_mp_func2(min); _cimg_mp_func2(minabs);
              _cimg_mp_func2(prod); _cimg_mp_func2(std); _cimg_mp_func2(sum); _cimg_mp_func2(var);
            }
            if (is_sth) _cimg_mp_const_scalar(op(*this));
            pos = opcode[1] = scalar();
            opcode.move_to(code);
            return_comp = true;
            _cimg_mp_return(pos);
          }

          // No corresponding built-in function -> Look for a user-defined macro call.
          s0 = strchr(ss,'(');
          if (s0) {
            variable_name.assign(ss,(unsigned int)(s0 - ss + 1)).back() = 0;

            // Count number of specified arguments.
            p1 = 0;
            for (s = s0 + 1; s<=se1; ++p1, s = ns + 1) {
              while (*s && cimg::is_blank(*s)) ++s;
              if (*s==')' && !p1) break;
              ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                             (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
            }

            char mb = 0;
            arg3 = 0; // Number of possible name matches
            cimglist_for(macro_def,l) if (!std::strcmp(macro_def[l],variable_name) && ++arg3 &&
                                          ((mb = macro_def[l].back())==(char)p1 || mb==(char)-1)) {
              const bool is_variadic = mb==(char)-1;
              p2 = is_variadic?1U:(unsigned int)mb; // Number of required arguments
              CImg<charT> _expr = macro_body[l]; // Expression to be substituted

              p1 = 1; // Index of current parsed argument
              for (s = s0 + 1; s<=se1; ++p1, s = ns + 1) { // Parse function arguments
                while (*s && cimg::is_blank(*s)) ++s;
                if (!is_variadic && *s==')' && p1==1) break; // Function has no arguments
                if (p1>p2) { ++p1; break; }

                if (is_variadic) ns = se1;
                else {
                  ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                                 (*ns!=')' || level[ns - expr._data]!=clevel)) ++ns;
                }
                variable_name.assign(s,(unsigned int)(ns - s + 1)).back() = 0; // Argument to write

                arg2 = 0;
                cimg_forX(_expr,k) {
                  if (_expr[k]==(char)p1) { // Perform argument substitution
                    arg1 = _expr._width;
                    if (variable_name._width>1) {
                      _expr.resize(arg1 + variable_name._width - 2,1,1,1,0);
                      std::memmove(_expr._data + k + variable_name._width - 1,_expr._data + k + 1,arg1 - k - 1);
                      std::memcpy(_expr._data + k,variable_name,variable_name._width - 1);
                      k+=variable_name._width - 2;
                    } else {
                      std::memmove(_expr._data + k,_expr._data + k + 1,arg1 - k - 1);
                      --k;
                    }
                  }
                  ++arg2;
                }
              }

              // Recompute 'pexpr' and 'level' for evaluating substituted expression.
              CImg<charT> _pexpr(_expr._width);
              ns = _pexpr._data;
              for (ps = _expr._data, c1 = ' '; *ps; ++ps) {
                if (!cimg::is_blank(*ps)) c1 = *ps;
                *(ns++) = c1;
              }
              *ns = 0;

              CImg<uintT> _level = get_level(_expr);
              expr.swap(_expr);
              pexpr.swap(_pexpr);
              level.swap(_level);
              s0 = user_macro;
              user_macro = macro_def[l];
              pos = compile(expr._data,expr._data + expr._width - 1,depth1,p_ref,block_flags);
              user_macro = s0;
              level.swap(_level);
              pexpr.swap(_pexpr);
              expr.swap(_expr);
              _cimg_mp_return(pos);
            }

            if (arg3) { // Macro name matched but number of arguments does not
              CImg<uintT> sig_nargs(arg3);
              arg1 = 0;
              cimglist_for(macro_def,l) if (!std::strcmp(macro_def[l],variable_name))
                sig_nargs[arg1++] = (unsigned int)macro_def[l].back();
              _cimg_mp_strerr;
              cimg::strellipsize(variable_name,64);
              if (sig_nargs._width>1) {
                sig_nargs.sort();
                arg1 = sig_nargs.back();
                --sig_nargs._width;
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: Function '%s()': Number of specified arguments (%u) "
                                            "does not match macro declaration (defined for %s or %u arguments), "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,variable_name._data,
                                            p1,sig_nargs.value_string()._data,arg1,s0);
              } else {
                throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                            "CImg<%s>::%s: Function '%s()': Number of specified arguments (%u) "
                                            "does not match macro declaration (defined for %u argument%s), "
                                            "in expression '%s'.",
                                            pixel_type(),_cimg_mp_calling_function,variable_name._data,
                                            p1,*sig_nargs,*sig_nargs!=1?"s":"",s0);
              }
            }
          }
        } // if (se1==')')

        // Char / string initializer.
        if (*se1=='\'' &&
            ((se1>ss && *ss=='\'') ||
             (se1>ss1 && *ss=='_' && *ss1=='\''))) {
          if (*ss=='_') { _cimg_mp_op("Char initializer"); s1 = ss2; }
          else { _cimg_mp_op("String initializer"); s1 = ss1; }
          arg1 = (unsigned int)(se1 - s1); // Original string length
          if (arg1) {
            CImg<charT>(s1,arg1 + 1).move_to(variable_name).back() = 0;
            cimg::strunescape(variable_name);
            arg1 = (unsigned int)std::strlen(variable_name);
          }
          if (!arg1) _cimg_mp_return(0); // Empty string -> 0
          if (*ss=='_') {
            if (arg1==1) _cimg_mp_const_scalar((unsigned char)*variable_name);
            cimg::strellipsize(variable_name,64);
            _cimg_mp_strerr;
            throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                        "CImg<%s>::%s: %s: Literal %s contains more than one byte, "
                                        "in expression '%s'.",
                                        pixel_type(),_cimg_mp_calling_function,s_op,
                                        ss1,s0);
          }
          pos = vector(arg1);
          CImg<ulongT>::vector((ulongT)mp_string_init,pos,arg1).move_to(l_opcode);
          CImg<ulongT>(1,arg1/sizeof(ulongT) + (arg1%sizeof(ulongT)?1:0)).move_to(l_opcode);
          std::memcpy((char*)l_opcode[1]._data,variable_name,arg1);
          (l_opcode>'y').move_to(is_inside_begin || is_new_variable_assignment?code:code_begin);
          return_comp = is_new_variable_assignment;
          if (!return_comp) set_reserved_vector(pos); // Prevent from being used in further optimization
          _cimg_mp_return(pos);
        }

        // Vector initializer [ ... ].
        if (*ss=='[' && *se1==']') {
          _cimg_mp_op("Vector initializer");
          s1 = ss1; while (s1<se2 && cimg::is_blank(*s1)) ++s1;
          s2 = se2; while (s2>s1 && cimg::is_blank(*s2)) --s2;
          if (s2>s1 && *s1=='\'' && *s2=='\'') { // Vector values provided as a string
            arg1 = (unsigned int)(s2 - s1 - 1); // Original string length
            if (arg1) {
              CImg<charT>(s1 + 1,arg1 + 1).move_to(variable_name).back() = 0;
              cimg::strunescape(variable_name);
              arg1 = (unsigned int)std::strlen(variable_name);
            }
            if (!arg1) _cimg_mp_return(0); // Empty string -> 0
            pos = vector(arg1);
            CImg<ulongT>::vector((ulongT)mp_string_init,pos,arg1).move_to(l_opcode);
            CImg<ulongT>(1,arg1/sizeof(ulongT) + (arg1%sizeof(ulongT)?1:0)).move_to(l_opcode);
            std::memcpy((char*)l_opcode[1]._data,variable_name,arg1);
            (l_opcode>'y').move_to(is_inside_begin || is_new_variable_assignment?code:code_begin);
            return_comp = is_new_variable_assignment;
            if (!return_comp) set_reserved_vector(pos); // Prevent from being used in further optimization
          } else { // Vector values provided as a list of items
            is_sth = !is_new_variable_assignment; // Can vector be defined once in 'begin()'?
            arg1 = 0; // Number of specified values
            if (*ss1!=']') for (s = ss1; s<se; ++s) {
                ns = s; while (ns<se && (*ns!=',' || level[ns - expr._data]!=clevel1) &&
                               (*ns!=']' || level[ns - expr._data]!=clevel)) ++ns;
                const CImgList<ulongT> &rcode = is_inside_begin?code:code_begin;
                p1 = rcode.size();
                p2 = variable_def.size();
                arg2 = compile(s,ns,depth1,0,block_flags);
                p3 = rcode.size();
                if (is_vector(arg2)) {
                  arg3 = size(arg2);
                  CImg<ulongT>::sequence(arg3,arg2 + 1,arg2 + arg3).move_to(l_opcode);
                  arg1+=arg3;
                  const CImg<ulongT> &rcode_back = rcode.back();
                  is_sth&=p3>p1 && rcode_back[1]==arg2 &&
                    (rcode_back[0]==(ulongT)mp_string_init ||
                     rcode_back[0]==(ulongT)mp_vector_init) && variable_def.size()==p2 && !is_comp_vector(arg2);
                  // ^^ Tricky part: detect if 'arg2' is a newly constructed vector not assigned to a variable
                  // (i.e. a vector-valued literal).
                } else {
                  CImg<ulongT>::vector(arg2).move_to(l_opcode);
                  ++arg1;
                  is_sth&=is_const_scalar(arg2);
                }
                s = ns;
              }
            if (!arg1) _cimg_mp_return(0);
            if (l_opcode.size()==1 && is_vector(arg2)) // Special case: '[ [ item(s) ] ]'
              _cimg_mp_return(arg2);

            pos = vector(arg1);
            l_opcode.insert(CImg<ulongT>::vector((ulongT)mp_vector_init,pos,0,arg1),0);
            (l_opcode>'y').move_to(opcode);
            opcode[2] = opcode._height;
            opcode.move_to(!is_sth || is_inside_begin || is_new_variable_assignment?code:code_begin);
            return_comp = !is_sth && is_new_variable_assignment;
            if (!return_comp) set_reserved_vector(pos); // Prevent from being used in further optimization
          }
          _cimg_mp_return(pos);
        }

        // Variables related to the input list of images.
        if (*ss1=='#' && ss2<se) {
          arg1 = compile(ss2,se,depth1,0,block_flags);
          _cimg_mp_check_notnan_index(arg1,ss2);
          p1 = (unsigned int)(imglist._width && is_const_scalar(arg1)?
                              cimg::mod((int)mem[arg1],imglist.width()):~0U);
          switch (*ss) {
          case 'w' : // w#ind
            if (!imglist) _cimg_mp_return(0);
            if (p1!=~0U) _cimg_mp_const_scalar(imglist[p1]._width);
            _cimg_mp_scalar1(image_width,arg1);
          case 'h' : // h#ind
            if (!imglist) _cimg_mp_return(0);
            if (p1!=~0U) _cimg_mp_const_scalar(imglist[p1]._height);
            _cimg_mp_scalar1(image_height,arg1);
          case 'd' : // d#ind
            if (!imglist) _cimg_mp_return(0);
            if (p1!=~0U) _cimg_mp_const_scalar(imglist[p1]._depth);
            _cimg_mp_scalar1(image_depth,arg1);
          case 'r' : // r#ind
            if (!imglist) _cimg_mp_return(0);
            if (p1!=~0U) _cimg_mp_const_scalar(imglist[p1]._is_shared);
            _cimg_mp_scalar1(image_is_shared,arg1);
          case 's' : // s#ind
            if (!imglist) _cimg_mp_return(0);
            if (p1!=~0U) _cimg_mp_const_scalar(imglist[p1]._spectrum);
            _cimg_mp_scalar1(image_spectrum,arg1);
          case 'i' : // i#ind
            if (!imglist) _cimg_mp_return(0);
            _cimg_mp_scalar8(ijxyzc1,0,arg1,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,_cimg_mp_slot_c,
                             0,_cimg_mp_boundary);
          case 'I' : // I#ind
            p2 = p1!=~0U?imglist[p1]._spectrum:imglist._width?~0U:0;
            if (!p2) _cimg_mp_return(0);
            pos = vector(p2);
            CImg<ulongT>::vector((ulongT)mp_IJoff,pos,1,p1,0,0,p2).move_to(code);
            return_comp = true;
            _cimg_mp_return(pos);
          case 'R' : // R#ind
            if (!imglist) _cimg_mp_return(0);
            _cimg_mp_scalar8(ijxyzc1,0,arg1,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,0,
                             0,_cimg_mp_boundary);
          case 'G' : // G#ind
            if (!imglist) _cimg_mp_return(0);
            _cimg_mp_scalar8(ijxyzc1,0,arg1,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,1,
                             0,_cimg_mp_boundary);
          case 'B' : // B#ind
            if (!imglist) _cimg_mp_return(0);
            _cimg_mp_scalar8(ijxyzc1,0,arg1,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,2,
                             0,_cimg_mp_boundary);
          case 'A' : // A#ind
            if (!imglist) _cimg_mp_return(0);
            _cimg_mp_scalar8(ijxyzc1,0,arg1,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,3,
                             0,_cimg_mp_boundary);
          }
        }

        if (*ss1 && *ss2=='#' && ss3<se) {
          arg1 = compile(ss3,se,depth1,0,block_flags);
          _cimg_mp_check_notnan_index(arg1,ss3);
          p1 = (unsigned int)(imglist._width && is_const_scalar(arg1)?
                              cimg::mod((int)mem[arg1],imglist.width()):~0U);
          if (*ss=='w' && *ss1=='h') { // wh#ind
            if (!imglist) _cimg_mp_return(0);
            if (p1!=~0U) _cimg_mp_const_scalar(imglist[p1]._width*imglist[p1]._height);
            _cimg_mp_scalar1(image_wh,arg1);
          }
          arg2 = ~0U;

          if (*ss=='i') {
            if (*ss1>='0' && *ss1<='9') { // i0#ind...i9#ind
              if (!imglist) _cimg_mp_return(0);
              _cimg_mp_scalar8(ijxyzc1,0,arg1,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,*ss1 - '0',
                               0,_cimg_mp_boundary);
            }

            if (*ss1=='c') { // ic#ind
              if (!imglist) _cimg_mp_return(0);
              if (is_const_scalar(arg1)) {
                if (!list_median) list_median.assign(imglist._width);
                if (!list_median[p1]) CImg<doubleT>::vector(imglist[p1].median()).move_to(list_median[p1]);
                _cimg_mp_const_scalar(*list_median[p1]);
              }
              _cimg_mp_scalar1(image_median_static,arg1);
            }

            if (*ss1=='d') { // id#ind
              if (!imglist) _cimg_mp_return(0);
              if (is_const_scalar(arg1)) {
                if (!list_stats) list_stats.assign(imglist._width);
                if (!list_stats[p1]) list_stats[p1].assign(1,14,1,1,0).fill(imglist[p1].get_stats(),false);
                _cimg_mp_const_scalar(std::sqrt(list_stats(p1,3)));
              }
              _cimg_mp_scalar1(image_std_static,arg1);
            }

            if (*ss1=='n') { // in#ind
              if (!imglist) _cimg_mp_return(0);
              if (is_const_scalar(arg1)) {
                if (!list_norm) list_norm.assign(imglist._width);
                if (!list_norm[p1]) CImg<doubleT>::vector(imglist[p1].magnitude(2)).move_to(list_norm[p1]);
                _cimg_mp_const_scalar(*list_norm[p1]);
              }
              _cimg_mp_scalar1(image_norm_static,arg1);
            }

            switch (*ss1) {
            case 'a' : arg2 = 2; break; // ia#ind
            case 'm' : arg2 = 0; break; // im#ind
            case 'M' : arg2 = 1; break; // iM#ind
            case 'p' : arg2 = 13; break; // ip#ind
            case 's' : arg2 = 12; break; // is#ind
            case 'v' : arg2 = 3; break; // iv#ind
            }
          } else if (*ss1=='m') switch (*ss) {
            case 'x' : arg2 = 4; break; // xm#ind
            case 'y' : arg2 = 5; break; // ym#ind
            case 'z' : arg2 = 6; break; // zm#ind
            case 'c' : arg2 = 7; break; // cm#ind
            } else if (*ss1=='M') switch (*ss) {
            case 'x' : arg2 = 8; break; // xM#ind
            case 'y' : arg2 = 9; break; // yM#ind
            case 'z' : arg2 = 10; break; // zM#ind
            case 'c' : arg2 = 11; break; // cM#ind
            }
          if (arg2!=~0U) {
            if (!imglist) _cimg_mp_return(0);
            if (is_const_scalar(arg1)) {
              if (!list_stats) list_stats.assign(imglist._width);
              if (!list_stats[p1]) list_stats[p1].assign(1,14,1,1,0).fill(imglist[p1].get_stats(),false);
              _cimg_mp_const_scalar(list_stats(p1,arg2));
            }
            _cimg_mp_scalar2(image_stats_static,arg1,arg2);
          }
        }

        if (*ss=='w' && *ss1=='h' && *ss2=='d' && *ss3=='#' && ss4<se) { // whd#ind
          arg1 = compile(ss4,se,depth1,0,block_flags);
          _cimg_mp_check_notnan_index(arg1,ss4);
          if (!imglist) _cimg_mp_return(0);
          p1 = (unsigned int)(is_const_scalar(arg1)?cimg::mod((int)mem[arg1],imglist.width()):~0U);
          if (p1!=~0U) _cimg_mp_const_scalar(imglist[p1]._width*imglist[p1]._height*imglist[p1]._depth);
          _cimg_mp_scalar1(image_whd,arg1);
        }
        if (*ss=='w' && *ss1=='h' && *ss2=='d' && *ss3=='s' && *ss4=='#' && ss5<se) { // whds#ind
          arg1 = compile(ss5,se,depth1,0,block_flags);
          _cimg_mp_check_notnan_index(arg1,ss5);
          if (!imglist) _cimg_mp_return(0);
          p1 = (unsigned int)(is_const_scalar(arg1)?cimg::mod((int)mem[arg1],imglist.width()):~0U);
          if (p1!=~0U)
            _cimg_mp_const_scalar(imglist[p1]._width*imglist[p1]._height*imglist[p1]._depth*imglist[p1]._spectrum);
          _cimg_mp_scalar1(image_whds,arg1);
        }

        if (!std::strcmp(ss,"interpolation")) _cimg_mp_return(_cimg_mp_interpolation); // interpolation
        if (!std::strcmp(ss,"boundary")) _cimg_mp_return(_cimg_mp_boundary); // boundary

        variable_name.assign(ss,(unsigned int)(se + 1 - ss)).back() = 0;

#ifdef cimg_mp_operator_dollar
        if (*ss=='$' && ss1<se) // External variable '$varname'
          _cimg_mp_const_scalar(cimg_mp_operator_dollar(variable_name._data + 1));
#endif

        // No known item found, assuming this is an already initialized variable.
        if (cimg::is_varname(variable_name)) { // Valid variable name
          get_variable_pos(variable_name,arg1,arg2);
          arg1 = arg2!=~0U?reserved_label[arg2]:arg1!=~0U?variable_pos[arg1]:~0U;
          if (arg1!=~0U) _cimg_mp_return(arg1);
        }

        // Reached an unknown item -> error.
        c1 = *se1;
        cimg::strellipsize(variable_name,64);
        _cimg_mp_strerr;
        if (is_sth)
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: Undefined variable '%s' in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,
                                      variable_name._data,s0);
        s1 = std::strchr(ss,'(');
        s_op = s1 && c1==')'?"function call":"item";
        throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                    "CImg<%s>::%s: Unrecognized %s '%s' in expression '%s'.",
                                    pixel_type(),_cimg_mp_calling_function,
#endif
