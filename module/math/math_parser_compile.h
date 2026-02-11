#ifndef CIMG_MODULE_MATH_PARSER_COMPILE_1_H
#define CIMG_MODULE_MATH_PARSER_COMPILE_1_H

      unsigned int compile(char *ss, char *se, const unsigned int depth, unsigned int *const p_ref,
                           unsigned char block_flags) {
        if (depth>256) {
          cimg::strellipsize(expr,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: Call stack overflow (infinite recursion?), "
                                      "in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,
                                      (ss - 4)>expr._data?ss - 4:expr._data);
        }
        char c1, c2;

        // Simplify expression when possible.
        do {
          c2 = 0;
          if (ss<se) {
            while (*ss && (cimg::is_blank(*ss) || *ss==';')) ++ss; // Remove leading blanks and ';'
            while (se>ss && (cimg::is_blank(c1 = *(se - 1)) || c1==';')) --se; // Remove trailing blanks and ';'
          }
          while (*ss=='(' && *(se - 1)==')' && std::strchr(ss,')')==se - 1) { // Remove useless start/end parentheses
            ++ss; --se; c2 = 1;
          }
          if (*ss=='_' && ss + 1<se && ss[1]=='(') { // Remove leading '_(something)' comment
            const unsigned int clevel = level[ss - expr._data];
            ss+=2;
            while (ss<se && (*ss!=')' || level[ss - expr._data]!=clevel)) ++ss;
            if (ss<se) ++ss;
            if (ss>=se) return _cimg_mp_slot_nan;
            c2 = 1;
          }
        } while (c2 && ss<se);

        if (se<=ss || !*ss) {
          cimg::strellipsize(expr,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s Missing %s, in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      *s_op=='F'?"argument":"item",
                                      ss_op);
        }

        static const size_t siz_ref = 7*sizeof(unsigned int);
        const char *const previous_s_op = s_op, *const previous_ss_op = ss_op;
        const unsigned int depth1 = depth + 1;
        unsigned int pos, p1, p2, p3, arg1, arg2, arg3, arg4, arg5, arg6;
        char
          *const se1 = se - 1, *const se2 = se - 2, *const se3 = se - 3, *const se4 = se - 4,
          *const ss1 = ss + 1, *const ss2 = ss + 2, *const ss3 = ss + 3, *const ss4 = ss + 4,
          *const ss5 = ss + 5, *const ss6 = ss + 6, *const ss7 = ss + 7, *const ss8 = ss + 8,
          *s, *ps, *ns, *s0, *s1, *s2, *s3, sep = 0, end = 0;
        double val = 0, val1, val2, val3;
        mp_func op;

        return_comp = false;

        // Bits of 'block_flags' tell about in which code block we currently are:
        // 0: critical(), 1: begin(), 2: begin_t(), 3: end(), 4: end_t(), 5: is_new_variable_assignment
        const bool
          is_inside_critical = (bool)(block_flags&1),
          is_inside_begin = (bool)(block_flags&2),
          is_inside_begin_t = (bool)(block_flags&4),
          is_inside_end = (bool)(block_flags&8),
          is_inside_end_t = (bool)(block_flags&16),
          is_new_variable_assignment = (bool)(block_flags&32);
        block_flags&=31; // Deeper calls to 'compile()' won't automatically pass 'is_new_variable_assigment=true'

        // 'p_ref' is a 'unsigned int[7]' used to return a reference to an image or vector value
        // linked to the returned memory slot (reference that cannot be determined at compile time).
        // p_ref[0] can be { 0 = scalar (unlinked) | 1 = vector value | 2 = image value (offset) |
        //                   3 = image value (coordinates) | 4 = image value as a vector (offset) |
        //                   5 = image value as a vector (coordinates) }.
        // Depending on p_ref[0], the remaining p_ref[k] have the following meaning:
        // When p_ref[0]==0, p_ref is actually unlinked.
        // When p_ref[0]==1, p_ref = [ 1, vector_ind, offset ].
        // When p_ref[0]==2, p_ref = [ 2, image_ind (or ~0U), is_relative, offset ].
        // When p_ref[0]==3, p_ref = [ 3, image_ind (or ~0U), is_relative, x, y, z, c ].
        // When p_ref[0]==4, p_ref = [ 4, image_ind (or ~0U), is_relative, offset ].
        // When p_ref[0]==5, p_ref = [ 5, image_ind (or ~0U), is_relative, x, y, z ].
        if (p_ref) { *p_ref = 0; p_ref[1] = p_ref[2] = p_ref[3] = p_ref[4] = p_ref[5] = p_ref[6] = ~0U; }

        const char saved_char = *se; *se = 0;
        const unsigned int clevel = level[ss - expr._data], clevel1 = clevel + 1;
        bool is_sth, is_relative;
        CImg<uintT> ref;
        CImg<charT> variable_name;
        CImgList<ulongT> l_opcode;

        // Look for a single value or a pre-defined variable.
        int nb = 0;
        is_sth = *ss=='-';
        s = ss + (*ss=='+' || is_sth?1:0);
        if (*s=='i' || *s=='I' || *s=='n' || *s=='N') { // Particular cases : +/-NaN and +/-Inf
          if (!cimg::strcasecmp(s,"inf")) { val = cimg::type<double>::inf(); nb = 1; }
          else if (!cimg::strcasecmp(s,"nan")) { val = cimg::type<double>::nan(); nb = 1; }
        } else if (*s=='0' && (s[1]=='x' || s[1]=='X') && s[2]!='-') { // Hexadecimal literal
          cimg_uint64 num;
          if ((nb = cimg_sscanf(s + 2,cimg_fhex64 "%c",&num,&sep))==1 || (nb==2 && sep=='%'))
            val = (double)num;
        } else if (*s=='0' && (s[1]=='b' || s[1]=='B') && s[2]!='-') { // Binary literal
          variable_name.assign(65);
          if ((nb = cimg_sscanf(s + 2,"%64[01]%c",variable_name.data(),&sep))==1 || (nb==2 && sep=='%'))
            val = (double)std::strtoll(variable_name,0,2);
          variable_name.assign();
        }
        if (is_sth && nb) val = -val;
        else if (!nb) nb = cimg_sscanf(ss,"%lf%c%c",&val,&(sep=0),&(end=0));
        if (nb==1) _cimg_mp_const_scalar(val);
        if (nb==2 && sep=='%') _cimg_mp_const_scalar(val/100);

        if (ss1==se) switch (*ss) { // One-char reserved variable
          case 'c' : _cimg_mp_return(reserved_label[(int)'c']!=~0U?reserved_label[(int)'c']:_cimg_mp_slot_c);
          case 'd' : _cimg_mp_return(reserved_label[(int)'d']!=~0U?reserved_label[(int)'d']:20);
          case 'e' : _cimg_mp_return(reserved_label[(int)'e']!=~0U?reserved_label[(int)'e']:27);
          case 'h' : _cimg_mp_return(reserved_label[(int)'h']!=~0U?reserved_label[(int)'h']:19);
          case 'k' :
            if (reserved_label[(int)'k']!=~0U) _cimg_mp_return(reserved_label[(int)'k']);
            pos = get_mem_img_index();
            if (pos!=~0U) _cimg_mp_return(pos);
            _cimg_mp_return_nan();
          case 'l' : _cimg_mp_return(reserved_label[(int)'l']!=~0U?reserved_label[(int)'l']:26);
          case 'r' : _cimg_mp_return(reserved_label[(int)'r']!=~0U?reserved_label[(int)'r']:22);
          case 's' : _cimg_mp_return(reserved_label[(int)'s']!=~0U?reserved_label[(int)'s']:21);
          case 't' : _cimg_mp_return(reserved_label[(int)'t']!=~0U?reserved_label[(int)'t']:_cimg_mp_slot_t);
          case 'n' :
            if (reserved_label[(int)'n']!=~0U) _cimg_mp_return(reserved_label[(int)'n']);
#if cimg_use_openmp!=0
            _cimg_mp_const_scalar((double)omp_get_max_threads());
#else
            _cimg_mp_return(1);
#endif
          case 'w' : _cimg_mp_return(reserved_label[(int)'w']!=~0U?reserved_label[(int)'w']:18);
          case 'x' : _cimg_mp_return(reserved_label[(int)'x']!=~0U?reserved_label[(int)'x']:_cimg_mp_slot_x);
          case 'y' : _cimg_mp_return(reserved_label[(int)'y']!=~0U?reserved_label[(int)'y']:_cimg_mp_slot_y);
          case 'z' : _cimg_mp_return(reserved_label[(int)'z']!=~0U?reserved_label[(int)'z']:_cimg_mp_slot_z);
          case 'u' :
            if (reserved_label[(int)'u']!=~0U) _cimg_mp_return(reserved_label[(int)'u']);
            _cimg_mp_scalar0(rand_double_0_1);
          case 'v' :
            if (reserved_label[(int)'v']!=~0U) _cimg_mp_return(reserved_label[(int)'v']);
            _cimg_mp_scalar0(rand_int_0_1);
          case 'g' :
            if (reserved_label[(int)'g']!=~0U) _cimg_mp_return(reserved_label[(int)'g']);
            _cimg_mp_scalar0(rand_double_gaussian);
          case 'i' :
            if (reserved_label[(int)'i']!=~0U) _cimg_mp_return(reserved_label[(int)'i']);
            _cimg_mp_scalar0(i);
          case 'I' :
            _cimg_mp_op("Variable 'I'");
            if (reserved_label[(int)'I']!=~0U) _cimg_mp_return(reserved_label[(int)'I']);
            if (!imgin._spectrum) _cimg_mp_return(0);
            need_input_copy = true;
            pos = vector(imgin._spectrum);
            CImg<ulongT>::vector((ulongT)mp_I,pos,imgin._spectrum).move_to(code);
            return_comp = true;
            _cimg_mp_return(pos);
          case 'R' :
            if (reserved_label[(int)'R']!=~0U) _cimg_mp_return(reserved_label[(int)'R']);
            need_input_copy = true;
            _cimg_mp_scalar8(ijxyzc1,0,~0U,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,0,0,0);
          case 'G' :
            if (reserved_label[(int)'G']!=~0U) _cimg_mp_return(reserved_label[(int)'G']);
            need_input_copy = true;
            _cimg_mp_scalar8(ijxyzc1,0,~0U,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,1,0,0);
          case 'B' :
            if (reserved_label[(int)'B']!=~0U) _cimg_mp_return(reserved_label[(int)'B']);
            need_input_copy = true;
            _cimg_mp_scalar8(ijxyzc1,0,~0U,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,2,0,0);
          case 'A' :
            if (reserved_label[(int)'A']!=~0U) _cimg_mp_return(reserved_label[(int)'A']);
            need_input_copy = true;
            _cimg_mp_scalar8(ijxyzc1,0,~0U,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,3,0,0);
          }
        else if (ss2==se) { // Two-chars reserved variable
          arg1 = arg2 = ~0U;
          if (*ss=='w' && *ss1=='h') // wh
            _cimg_mp_return(reserved_label[0]!=~0U?reserved_label[0]:23);
          if (*ss=='p' && *ss1=='i') // pi
            _cimg_mp_return(reserved_label[3]!=~0U?reserved_label[3]:28);
          if (*ss=='i') {
            if (*ss1>='0' && *ss1<='9') { // i0...i9
              pos = 21 + *ss1 - '0';
              if (reserved_label[pos]!=~0U) _cimg_mp_return(reserved_label[pos]);
              need_input_copy = true;
              _cimg_mp_scalar8(ijxyzc1,0,~0U,_cimg_mp_slot_x,_cimg_mp_slot_y,_cimg_mp_slot_z,pos - 21,0,0);
            }
            switch (*ss1) {
            case 'a' : arg1 = 6; arg2 = 2; break; // ia
            case 'c' : // ic
              if (reserved_label[11]!=~0U) _cimg_mp_return(reserved_label[11]);
              if (mem_img_median==~0U) mem_img_median = imgin?const_scalar(imgin.median()):0;
              _cimg_mp_return(mem_img_median);
              break;
            case 'd' : arg1 = 8; arg2 = 3; break; // id
            case 'm' : arg1 = 4; arg2 = 0; break; // im
            case 'M' : arg1 = 5; arg2 = 1; break; // iM
            case 'n' : // in
              if (reserved_label[12]!=~0U) _cimg_mp_return(reserved_label[12]);
              if (mem_img_norm==~0U) mem_img_norm = imgin?const_scalar(imgin.magnitude(2)):0;
              _cimg_mp_return(mem_img_norm);
              break;
            case 'p' : arg1 = 10; arg2 = 13; break; // ip
            case 's' : arg1 = 9; arg2 = 12; break; // is
            case 'v' : arg1 = 7; arg2 = 3; break; // iv
            }
          }
          else if (*ss1=='m') switch (*ss) {
            case 'x' : arg1 = 13; arg2 = 4; break; // xm
            case 'y' : arg1 = 14; arg2 = 5; break; // ym
            case 'z' : arg1 = 15; arg2 = 6; break; // zm
            case 'c' : arg1 = 16; arg2 = 7; break; // cm
            }
          else if (*ss1=='M') switch (*ss) {
            case 'x' : arg1 = 17; arg2 = 8; break; // xM
            case 'y' : arg1 = 18; arg2 = 9; break; // yM
            case 'z' : arg1 = 19; arg2 = 10; break; // zM
            case 'c' : arg1 = 20; arg2 = 11; break; // cM
            }
          if (arg1!=~0U) {
            if (reserved_label[arg1]!=~0U) _cimg_mp_return(reserved_label[arg1]);
            if (!img_stats) {
              img_stats.assign(1,14,1,1,0).fill(imgin.get_stats(),false);
              mem_img_stats.assign(1,14,1,1,~0U);
            }
            if (mem_img_stats[arg2]==~0U) mem_img_stats[arg2] = const_scalar(img_stats[arg2]);
            if (arg1==8) _cimg_mp_const_scalar(std::sqrt(img_stats[arg2])); // id: std variation
            _cimg_mp_return(mem_img_stats[arg2]);
          }
        } else if (ss3==se) { // Three-chars reserved variable
          if (*ss=='w' && *ss1=='h' && *ss2=='d') // whd
            _cimg_mp_return(reserved_label[1]!=~0U?reserved_label[1]:24);
          if (*ss=='e' && *ss1=='p' && *ss2=='s') // eps
            _cimg_mp_return(reserved_label[33]!=~0U?reserved_label[33]:29);
        } else if (ss4==se) { // Four-chars reserved variable
          if (*ss=='w' && *ss1=='h' && *ss2=='d' && *ss3=='s') // whds
            _cimg_mp_return(reserved_label[2]!=~0U?reserved_label[2]:25);
        }

        pos = ~0U;
        is_sth = false;
        for (s0 = ss, s1 = ss1; s1<se1; ++s1) {
          if (!(s1 = (char*)std::memchr(s1,';',se1 - s1))) break;
          if (level[s1 - expr._data]==clevel) { // Separator ';'
            is_end_code = false;
            arg1 = compile(s0,s1++,depth,0,block_flags);
            if (!is_end_code) pos = arg1; // 'end()' and 'end_t()' return void
            is_sth = true;
            while (*s1 && (cimg::is_blank(*s1) || *s1==';')) ++s1;
            s0 = s1;
          }
        }
        if (is_sth) {
          is_end_code = false;
          arg1 = compile(s0,se,depth,p_ref,block_flags);
          if (!is_end_code) pos = arg1; // 'end()' and 'end_t()' return void
          _cimg_mp_return(pos!=~0U?pos:_cimg_mp_slot_nan);
        }

        // Declare / assign variable, vector value or image value.
        for (s = ss1; s<se1; ++s) {
          if (!(s = (char*)std::memchr(s,'=',se1 - s))) break;
          ps = s - 1; ns = s + 1;
          const char pc = *ps;
          if (level[s - expr._data]==clevel &&
              *ns!='=' && pc!='=' && pc!='>' && pc!='<' && pc!='!' &&
              pc!='+' && pc!='-' && pc!='*' && pc!='/' && pc!='%' &&
              pc!='>' && pc!='<' && pc!='&' && pc!='|' && pc!='^') {
            variable_name.assign(ss,(unsigned int)(s + 1 - ss)).back() = 0;
            cimg::strpare(variable_name,false,true);
            const unsigned int l_variable_name = (unsigned int)std::strlen(variable_name);
            char *const ve1 = ss + l_variable_name - 1;
            _cimg_mp_op("Operator '='");

            // Assign image value (direct).
            if (l_variable_name>2 && (*ss=='i' || *ss=='j' || *ss=='I' || *ss=='J') && (*ss1=='(' || *ss1=='[') &&
                (reserved_label[(int)*ss]==~0U || *ss1=='(' || !is_vector(reserved_label[(int)*ss]))) {
              is_relative = *ss=='j' || *ss=='J';

              if (*ss1=='[' && *ve1==']') { // i/j/I/J[_#ind,offset] = value
                if (!is_inside_critical) is_parallelizable = false;
                if (*ss2=='#') { // Index specified
                  s0 = ss3; while (s0<ve1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                  p1 = compile(ss3,s0++,depth1,0,block_flags);
                  _cimg_mp_check_notnan_index(p1,ss3);
                  _cimg_mp_check_list();
                } else { p1 = ~0U; s0 = ss2; }
                arg1 = compile(s0,ve1,depth1,0,block_flags); // Offset
                _cimg_mp_check_type(arg1,0,1,0);
                arg2 = compile(s + 1,se,depth1,0,block_flags); // Value to assign
                _cimg_mp_check_type(arg2,2,*ss>='i'?1:3,0);
                if (is_vector(arg2)) {
                  if (p1!=~0U) {
                    _cimg_mp_check_const_index(p1);
                    p3 = (unsigned int)cimg::mod((int)mem[p1],imglist.width());
                    p2 = imglist[p3]._spectrum;
                  } else p2 = imgin._spectrum;
                  if (!p2) _cimg_mp_return(0);
                  _cimg_mp_check_type(arg2,2,2,p2);
                } else p2 = 0;

                if (p_ref) {
                  *p_ref = *ss=='I' || *ss=='J'?4:2;
                  p_ref[1] = p1;
                  p_ref[2] = (unsigned int)is_relative;
                  p_ref[3] = arg1;
                  if (is_vector(arg2))
                    set_reserved_vector(arg2); // Prevent from being used in further optimization
                  else if (is_comp_scalar(arg2)) memtype[arg2] = -1;
                  if (is_comp_scalar(arg1)) memtype[arg1] = -1;
                }

                if ((p1==~0U && imgout) || (p1!=~0U && imglist)) {
                  if (*ss>='i')
                    CImg<ulongT>::vector((ulongT)mp_set_ijoff,arg2,(ulongT)is_relative,
                                         p1,arg1).move_to(code);
                  else if (is_scalar(arg2))
                    CImg<ulongT>::vector((ulongT)mp_set_IJoff_s,arg2,(ulongT)is_relative,
                                         p1,arg1).move_to(code);
                  else
                    CImg<ulongT>::vector((ulongT)mp_set_IJoff_v,arg2,(ulongT)is_relative,
                                         p1,arg1,size(arg2)).move_to(code);
                }
                _cimg_mp_return(arg2);
              }

              if (*ss1=='(' && *ve1==')') { // i/j/I/J(_#ind,_x,_y,_z,_c) = value
                if (!is_inside_critical) is_parallelizable = false;
                if (*ss2=='#') { // Index specified
                  s0 = ss3; while (s0<ve1 && (*s0!=',' || level[s0 - expr._data]!=clevel1)) ++s0;
                  p1 = compile(ss3,s0++,depth1,0,block_flags);
                  _cimg_mp_check_notnan_index(p1,ss3);
                  _cimg_mp_check_list();
                } else { p1 = ~0U; s0 = ss2; }
                arg1 = is_relative?0U:(unsigned int)_cimg_mp_slot_x;
                arg2 = is_relative?0U:(unsigned int)_cimg_mp_slot_y;
                arg3 = is_relative?0U:(unsigned int)_cimg_mp_slot_z;
                arg4 = is_relative?0U:(unsigned int)_cimg_mp_slot_c;
                arg5 = compile(s + 1,se,depth1,0,block_flags); // Value to assign
                _cimg_mp_check_type(arg5,2,*ss>='i'?1:3,0);
                if (s0<ve1) { // X or [ X,_Y,_Z,_C ]
                  s1 = s0; while (s1<ve1 && (*s1!=',' || level[s1 - expr._data]!=clevel1)) ++s1;
                  arg1 = compile(s0,s1,depth1,0,block_flags);
                  if (is_vector(arg1)) { // Coordinates specified as a vector
                    p2 = size(arg1); // Vector size
                    ++arg1;
                    if (p2>1) {
                      arg2 = arg1 + 1;
                      if (p2>2) {
                        arg3 = arg2 + 1;
                        if (p2>3) arg4 = arg3 + 1;
                      }
                    }
                  } else if (s1<ve1) { // Y
                    s2 = ++s1; while (s2<ve1 && (*s2!=',' || level[s2 - expr._data]!=clevel1)) ++s2;
                    arg2 = compile(s1,s2,depth1,0,block_flags);
                    if (s2<ve1) { // Z
                      s3 = ++s2; while (s3<ve1 && (*s3!=',' || level[s3 - expr._data]!=clevel1)) ++s3;
                      arg3 = compile(s2,s3,depth1,0,block_flags);
                      if (s3<ve1) arg4 = compile(++s3,ve1,depth1,0,block_flags); // C
                    }
                  }
                }

                if (is_vector(arg5)) {
                  if (p1!=~0U) {
                    _cimg_mp_check_const_index(p1);
                    p3 = (unsigned int)cimg::mod((int)mem[p1],imglist.width());
                    p2 = imglist[p3]._spectrum;
                  } else p2 = imgin._spectrum;
                  if (!p2) _cimg_mp_return(0);
                  _cimg_mp_check_type(arg5,2,2,p2);
                } else p2 = 0;


                if (p_ref) {
                  *p_ref = *ss=='I' || *ss=='J'?5:3;
                  p_ref[1] = p1;
                  p_ref[2] = (unsigned int)is_relative;
                  p_ref[3] = arg1;
                  p_ref[4] = arg2;
                  p_ref[5] = arg3;
                  p_ref[6] = arg4;
                  if (is_vector(arg5))
                    set_reserved_vector(arg5); // Prevent from being used in further optimization
                  else if (is_comp_scalar(arg5)) memtype[arg5] = -1;
                  if (p1!=~0U && is_comp_scalar(p1)) memtype[p1] = -1;
                  if (is_comp_scalar(arg1)) memtype[arg1] = -1;
                  if (is_comp_scalar(arg2)) memtype[arg2] = -1;
                  if (is_comp_scalar(arg3)) memtype[arg3] = -1;
                  if (is_comp_scalar(arg4)) memtype[arg4] = -1;
                }

                if ((p1==~0U && imgout) || (p1!=~0U && imglist)) {
                  if (*ss>='i')
                    CImg<ulongT>::vector((ulongT)mp_set_ijxyzc,arg5,(ulongT)is_relative,
                                         p1,arg1,arg2,arg3,arg4).move_to(code);
                  else if (is_scalar(arg5))
                    CImg<ulongT>::vector((ulongT)mp_set_IJxyz_s,arg5,(ulongT)is_relative,
                                         p1,arg1,arg2,arg3).move_to(code);
                  else
                    CImg<ulongT>::vector((ulongT)mp_set_IJxyz_v,arg5,(ulongT)is_relative,
                                         p1,arg1,arg2,arg3,size(arg5)).move_to(code);
                }
                _cimg_mp_return(arg5);
              }
            }

            // Assign vector value (direct).
            if (l_variable_name>3 && *ve1==']' && *ss!='[') {
              s0 = ve1; while (s0>ss && (*s0!='[' || level[s0 - expr._data]!=clevel)) --s0;
              if (s0>ss && cimg::is_varname(ss,s0 - ss)) {
                variable_name[s0 - ss] = 0; // Remove brackets in variable name
                get_variable_pos(variable_name,arg1,arg2);
                arg1 = arg2!=~0U?reserved_label[arg2]:arg1!=~0U?variable_pos[arg1]:~0U; // Vector slot
                if (arg1==~0U || is_scalar(arg1))
                  compile(ss,s0,depth1,0,block_flags); // Variable does not exist or is not a vector -> error

                arg2 = compile(++s0,ve1,depth1,0,block_flags); // Index
                arg3 = compile(s + 1,se,depth1,0,block_flags); // Value to assign
                _cimg_mp_check_type(arg3,2,1,0);

                if (is_const_scalar(arg2)) { // Constant index -> return corresponding variable slot directly
                  nb = (int)mem[arg2];
                  if (nb>=0 && nb<(int)size(arg1)) {
                    arg1+=nb + 1;
                    CImg<ulongT>::vector((ulongT)mp_copy,arg1,arg3).move_to(code);
                    _cimg_mp_return(arg1);
                  }
                  compile(ss,s,depth1,0,block_flags); // Out-of-bounds reference -> error
                }

                // Case of non-constant index -> return assigned value + linked reference.
                if (p_ref) {
                  *p_ref = 1;
                  p_ref[1] = arg1;
                  p_ref[2] = arg2;
                  if (is_comp_scalar(arg3)) memtype[arg3] = -1; // Prevent from being used in further optimization
                  if (is_comp_scalar(arg2)) memtype[arg2] = -1;
                }
                CImg<ulongT>::vector((ulongT)mp_vector_set_off,arg3,arg1,(ulongT)size(arg1),arg2).
                  move_to(code);
                _cimg_mp_return(arg3);
              }
            }

            // Assign user-defined macro.
            if (l_variable_name>2 && *ve1==')' && *ss!='(') {
              s0 = ve1; while (s0>ss && *s0!='(') --s0;
              if (cimg::is_varname(ss,s0 - ss) && std::strncmp(variable_name,"debug(",6) &&
                  std::strncmp(variable_name,"print(",6)) { // Valid macro name
                s0 = variable_name._data + (s0 - ss);
                *s0 = 0;
                s1 = variable_name._data + l_variable_name - 1; // Pointer to closing parenthesis
                CImg<charT>(variable_name._data,(unsigned int)(s0 - variable_name._data + 1)).move_to(macro_def,0);
                ++s; while (*s && cimg::is_blank(*s)) ++s;
                CImg<charT>(s,(unsigned int)(se - s + 1)).move_to(macro_body,0);

                bool is_variadic = false;
                p1 = 1; // Index of current parsed argument
                for (s = s0 + 1; s<=s1; ++p1, s = ns + 1) { // Parse function arguments
                  if (is_variadic && p1>1) {
                    _cimg_mp_strerr;
                    cimg::strellipsize(variable_name,64);
                    throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                "CImg<%s>::%s: %s: Multiple arguments not allowed when first one is "
                                                "variadic, in macro definition '%s()', in expression '%s'.",
                                                pixel_type(),_cimg_mp_calling_function,s_op,
                                                variable_name._data,s0);
                  }
                  if (p1>24) {
                    _cimg_mp_strerr;
                    cimg::strellipsize(variable_name,64);
                    throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                "CImg<%s>::%s: %s: Too much specified arguments (>24), in macro "
                                                "definition '%s()', in expression '%s'.",
                                                pixel_type(),_cimg_mp_calling_function,s_op,
                                                variable_name._data,s0);
                  }
                  while (*s && cimg::is_blank(*s)) ++s;
                  if (*s==')' && p1==1) break; // Function has no arguments
                  s2 = s; // Start of the argument name
                  is_sth = true; // is_valid_argument_name?
                  if (*s2>='0' && *s2<='9') is_sth = false;
                  else for (ns = s2; ns<s1 && *ns!=',' && *ns!='.' && !cimg::is_blank(*ns); ++ns)
                         if (!cimg::is_varchar(*ns)) { is_sth = false; break; }
                  s3 = ns; // End of the argument name
                  if (is_sth && ns>s2 && *ns=='.' && ns[1]=='.' && ns[2]=='.') { is_variadic = true; ns+=3; }
                  else if (*ns=='.') is_sth = false;
                  while (*ns && cimg::is_blank(*ns)) ++ns;

                  if (!is_sth || s2==s3 || (*ns!=',' && ns!=s1)) {
                    cimg::strellipsize(variable_name,64);
                    _cimg_mp_strerr;
                    if (is_sth)
                      throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                  "CImg<%s>::%s: %s: Empty argument name for argument %u of "
                                                  "macro '%s()', in expression '%s'.",
                                                  pixel_type(),_cimg_mp_calling_function,s_op,p1,
                                                  variable_name._data,s0);
                    else {
                      for (s3 = s2; *s3 && *s3!=',' && *s3!=')'; ++s3) {}
                      CImg<charT> argument_name(s2,s3 - s2 + 1); argument_name.back() = 0;
                      throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                                  "CImg<%s>::%s: %s: Invalid argument name '%s' for argument %u of "
                                                  "macro '%s()', in expression '%s'.",
                                                  pixel_type(),_cimg_mp_calling_function,s_op,argument_name._data,p1,
                                                  variable_name._data,s0);
                    }
                  }

                  if (ns==s1 || *ns==',' || (is_variadic && *ns=='.')) { // New argument found
                    *s3 = 0;
                    p2 = (unsigned int)(s3 - s2); // Argument length
                    for (ps = std::strstr(macro_body[0],s2); ps; ps = std::strstr(ps,s2)) { // Replace by arg number
                      if (!((ps>macro_body[0]._data && cimg::is_varchar(*(ps - 1))) ||
                            (ps + p2<macro_body[0].end() && cimg::is_varchar(*(ps + p2))))) {
                        if (ps>macro_body[0]._data && *(ps - 1)=='#') { // Remove pre-number sign
                          *(ps - 1) = (char)p1;
                          if (ps + p2<macro_body[0].end() && *(ps + p2)=='#') { // Has pre & post number signs
                            std::memmove(ps,ps + p2 + 1,macro_body[0].end() - ps - p2 - 1);
                            macro_body[0]._width-=p2 + 1;
                          } else { // Has pre number sign only
                            std::memmove(ps,ps + p2,macro_body[0].end() - ps - p2);
                            macro_body[0]._width-=p2;
                          }
                        } else if (ps + p2<macro_body[0].end() && *(ps + p2)=='#') { // Remove post-number sign
                          *(ps++) = (char)p1;
                          std::memmove(ps,ps + p2,macro_body[0].end() - ps - p2);
                          macro_body[0]._width-=p2;
                        } else if (is_variadic) { // Replace variadic argument
                          *(ps++) = (char)p1;
                          std::memmove(ps,ps + p2 - 1,macro_body[0].end() - ps - p2 + 1);
                          macro_body[0]._width-=p2 - 1;
                        } else { // Not near a number sign
                          if (p2<3) {
                            ps-=(ulongT)macro_body[0]._data;
                            macro_body[0].resize(macro_body[0]._width - p2 + 3,1,1,1,0);
                            ps+=(ulongT)macro_body[0]._data;
                          } else macro_body[0]._width-=p2 - 3;
                          std::memmove(ps + 3,ps + p2,macro_body[0].end() - ps - 3);
                          *(ps++) = '(';
                          *(ps++) = (char)p1;
                          *(ps++) = ')';
                        }
                      } else ++ps;
                    }
                  }
                }

                // Store number of arguments.
                macro_def[0].resize(macro_def[0]._width + 1,1,1,1,0).back() = is_variadic?(char)-1:(char)(p1 - 1);
                _cimg_mp_return_nan();
              }
            }

            // Check if the variable name could be valid. If not, this is probably an lvalue assignment.
            const bool is_const = l_variable_name>6 && !std::strncmp(variable_name,"const ",6);
            s0 = variable_name._data;
            if (is_const) {
              s0+=6; while (cimg::is_blank(*s0)) ++s0;
              variable_name.resize(variable_name.end() - s0,1,1,1,0,0,1);
            }
            if (cimg::is_varname(variable_name)) { // Valid variable name

              // Assign variable (direct).
              get_variable_pos(variable_name,arg1,arg2);
              arg1 = arg2!=~0U?reserved_label[arg2]:arg1!=~0U?variable_pos[arg1]:~0U;
              arg3 = compile(s + 1,se,depth1,0,block_flags|(arg1==~0U?32:0));
              is_sth = return_comp; // is arg3 a new blank object?
              if (is_const) _cimg_mp_check_const_scalar(arg3,2,0);

              if (arg1==~0U) { // Create new variable
                if (is_vector(arg3)) { // Vector variable
                  arg1 = is_sth || is_comp_vector(arg3)?arg3:copy(arg3);
                  set_reserved_vector(arg1); // Prevent from being used in further optimization
                } else { // Scalar variable
                  if (is_const) arg1 = arg3;
                  else {
                    arg1 = is_sth || is_comp_scalar(arg3)?arg3:scalar1(mp_copy,arg3);
                    memtype[arg1] = -1;
                  }
                }

                if (arg2!=~0U) reserved_label[arg2] = arg1;
                else {
                  if (variable_def._width>=variable_pos._width) variable_pos.resize(-200,1,1,1,0);
                  variable_pos[variable_def._width] = arg1;
                  variable_name.move_to(variable_def);
                }

              } else { // Variable already exists -> assign a new value
                if (is_const || is_const_scalar(arg1)) {
                  cimg::strellipsize(variable_name,64);
                  _cimg_mp_strerr;
                  throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                              "CImg<%s>::%s: %s: Invalid assignment of %sconst variable '%s'%s, "
                                              "in expression '%s'.",
                                              pixel_type(),_cimg_mp_calling_function,s_op,
                                              is_const_scalar(arg1)?"":"non-",
                                              variable_name._data,
                                              !is_const_scalar(arg1) && is_const?" as a const variable":"",
                                              s0);
                }
                _cimg_mp_check_type(arg3,2,is_vector(arg1)?3:1,size(arg1));
                if (is_vector(arg1)) { // Vector
                  if (is_vector(arg3)) // From vector
                    CImg<ulongT>::vector((ulongT)mp_vector_copy,arg1,arg3,(ulongT)size(arg1)).
                      move_to(code);
                  else // From scalar
                    CImg<ulongT>::vector((ulongT)mp_vector_init,arg1,1,(ulongT)size(arg1),arg3).
                      move_to(code);
                } else // Scalar
                  CImg<ulongT>::vector((ulongT)mp_copy,arg1,arg3).move_to(code);
              }
              return_comp = false;
              _cimg_mp_return(arg1);
            }

            // Assign lvalue (variable name was not valid for a direct assignment).
            arg1 = ~0U;
            is_sth = (bool)std::strchr(variable_name,'?'); // Contains_ternary_operator?
            if (is_sth) break; // Do nothing and make ternary operator priority over assignment

            if (l_variable_name>2 && (std::strchr(variable_name,'(') || std::strchr(variable_name,'['))) {
              ref.assign(7);
              arg1 = compile(ss,s,depth1,ref,block_flags); // Lvalue slot
              arg2 = compile(s + 1,se,depth1,0,block_flags); // Value to assign

              if (*ref==1) { // Vector value (scalar): V[k] = scalar
                _cimg_mp_check_type(arg2,2,1,0);
                arg3 = ref[1]; // Vector slot
                arg4 = ref[2]; // Index
                if (p_ref) std::memcpy(p_ref,ref,siz_ref);
                CImg<ulongT>::vector((ulongT)mp_vector_set_off,arg2,arg3,(ulongT)size(arg3),arg4).
                  move_to(code);
                _cimg_mp_return(arg2);
              }

              if (*ref==2) { // Image value (scalar): i/j[_#ind,off] = scalar
                if (!is_inside_critical) is_parallelizable = false;
                _cimg_mp_check_type(arg2,2,1,0);
                p1 = ref[1]; // Index
                is_relative = (bool)ref[2];
                arg3 = ref[3]; // Offset
                if (p_ref) std::memcpy(p_ref,ref,siz_ref);
                if ((p1==~0U && imgout) || (p1!=~0U && imglist))
                  CImg<ulongT>::vector((ulongT)mp_set_ijoff,arg2,(ulongT)is_relative,
                                       p1,arg3).move_to(code);
                _cimg_mp_return(arg2);
              }

              if (*ref==3) { // Image value (scalar): i/j(_#ind,_x,_y,_z,_c) = scalar
                if (!is_inside_critical) is_parallelizable = false;
                _cimg_mp_check_type(arg2,2,1,0);
                p1 = ref[1]; // Index
                is_relative = (bool)ref[2];
                arg3 = ref[3]; // X
                arg4 = ref[4]; // Y
                arg5 = ref[5]; // Z
                arg6 = ref[6]; // C
                if (p_ref) std::memcpy(p_ref,ref,siz_ref);
                if ((p1==~0U && imgout) || (p1!=~0U && imglist))
                  CImg<ulongT>::vector((ulongT)mp_set_ijxyzc,arg2,(ulongT)is_relative,
                                       p1,arg3,arg4,arg5,arg6).move_to(code);
                _cimg_mp_return(arg2);
              }

              if (*ref==4) { // Image value (vector): I/J[_#ind,off] = value
                if (!is_inside_critical) is_parallelizable = false;
                _cimg_mp_check_type(arg2,2,3,size(arg1));
                p1 = ref[1]; // Index
                is_relative = (bool)ref[2];
                arg3 = ref[3]; // Offset
                if (p_ref) std::memcpy(p_ref,ref,siz_ref);
                if ((p1==~0U && imgout) || (p1!=~0U && imglist)) {
                  if (is_scalar(arg2))
                    CImg<ulongT>::vector((ulongT)mp_set_IJoff_s,arg2,(ulongT)is_relative,
                                         p1,arg3).move_to(code);
                  else {
                    _cimg_mp_check_const_index(p1);
                    CImg<ulongT>::vector((ulongT)mp_set_IJoff_v,arg2,(ulongT)is_relative,
                                         p1,arg3,size(arg2)).move_to(code);
                  }
                }
                _cimg_mp_return(arg2);
              }

              if (*ref==5) { // Image value (vector): I/J(_#ind,_x,_y,_z,_c) = value
                if (!is_inside_critical) is_parallelizable = false;
                _cimg_mp_check_type(arg2,2,3,size(arg1));
                p1 = ref[1]; // Index
                is_relative = (bool)ref[2];
                arg3 = ref[3]; // X
                arg4 = ref[4]; // Y
                arg5 = ref[5]; // Z
                if (p_ref) std::memcpy(p_ref,ref,siz_ref);
                if ((p1==~0U && imgout) || (p1!=~0U && imglist)) {
                  if (is_scalar(arg2))
                    CImg<ulongT>::vector((ulongT)mp_set_IJxyz_s,arg2,(ulongT)is_relative,
                                         p1,arg3,arg4,arg5).move_to(code);
                  else {
                    _cimg_mp_check_const_index(p1);
                    CImg<ulongT>::vector((ulongT)mp_set_IJxyz_v,arg2,(ulongT)is_relative,
                                         p1,arg3,arg4,arg5,size(arg2)).move_to(code);
                  }
                }
                _cimg_mp_return(arg2);
              }

              if (is_vector(arg1)) { // Vector variable: V = value
                _cimg_mp_check_type(arg2,2,3,size(arg1));
                if (is_vector(arg2)) // From vector
                  CImg<ulongT>::vector((ulongT)mp_vector_copy,arg1,arg2,(ulongT)size(arg1)).
                    move_to(code);
                else // From scalar
                  CImg<ulongT>::vector((ulongT)mp_vector_init,arg1,1,(ulongT)size(arg1),arg2).
                    move_to(code);
                _cimg_mp_return(arg1);
              }

              if (is_reserved(arg1) && arg1>_cimg_mp_slot_c) { // Scalar variable: s = scalar
                _cimg_mp_check_type(arg2,2,1,0);
                CImg<ulongT>::vector((ulongT)mp_copy,arg1,arg2).move_to(code);
                _cimg_mp_return(arg1);
              }
            }

            // No assignment expressions match -> error.
            cimg::strellipsize(variable_name,64);
            _cimg_mp_strerr;
            throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                        "CImg<%s>::%s: %s: Invalid %slvalue '%s', "
                                        "in expression '%s'.",
                                        pixel_type(),_cimg_mp_calling_function,s_op,
                                        arg1!=~0U && is_const_scalar(arg1)?"const ":"",
                                        variable_name._data,s0);
          }
        }

        // Apply unary/binary/ternary operators. The operator precedences should be the same as in C++.
        is_sth = true;
        for (s = ss; s<se1; ++s) // Do a first pass to quickly skip costly operator checks that require for() loop
          if (cimg::_cimg_math_parser_check_operator[(unsigned char)*s] && level[s - expr._data]==clevel) {
            is_sth = false; break;
          }
        if (is_sth) goto cimg_skip_iterative_operator_parsing;

        for (s = se2, ps = se3, ns = ps - 1; s>ss1; --s, --ps, --ns) // Here, ns = ps - 1
          if (*s=='=' && (*ps=='*' || *ps=='/' || *ps=='^') && *ns==*ps &&
              level[s - expr._data]==clevel) { // Self-operators for complex numbers only (**=,//=,^^=)
            _cimg_mp_op(*ps=='*'?"Operator '**='":*ps=='/'?"Operator '//='":"Operator '^^='");

            ref.assign(7);
            arg1 = compile(ss,ns,depth1,ref,block_flags); // Vector slot
            arg2 = compile(s + 1,se,depth1,0,block_flags); // Right operand
            _cimg_mp_check_type(arg1,1,2,2);
            _cimg_mp_check_type(arg2,2,3,2);
            if (is_vector(arg2)) { // Complex **= complex
              if (*ps=='*')
                CImg<ulongT>::vector((ulongT)mp_complex_mul,arg1,arg1,arg2).move_to(code);
              else if (*ps=='/')
                CImg<ulongT>::vector((ulongT)mp_complex_div_vv,arg1,arg1,arg2).move_to(code);
              else
                CImg<ulongT>::vector((ulongT)mp_complex_pow_vv,arg1,arg1,arg2).move_to(code);
            } else { // Complex **= scalar
              if (arg2==1) _cimg_mp_return(arg1);
              if (*ps=='*') self_vector_s(arg1,mp_self_mul,arg2);
              else if (*ps=='/') self_vector_s(arg1,mp_self_div,arg2);
              else if (arg2==2) CImg<ulongT>::vector((ulongT)mp_complex_sqr,arg1,arg1 + 1,arg1 + 2).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_pow_vs,arg1,arg1,arg2).move_to(code);
            }

            if (*ref==4) { // Image value (vector): I/J[_#ind,off] **= value
              if (!is_inside_critical) is_parallelizable = false;
              p1 = ref[1]; // Index
              is_relative = (bool)ref[2];
              arg3 = ref[3]; // Offset
              if (p_ref) std::memcpy(p_ref,ref,siz_ref);
              if ((p1==~0U && imgout) || (p1!=~0U && imglist)) {
                if (p1!=~0U) _cimg_mp_check_const_index(p1);
                CImg<ulongT>::vector((ulongT)mp_set_IJoff_v,arg1,(ulongT)is_relative,
                                     p1,arg3,size(arg1)).move_to(code);
              }
            } else if (*ref==5) { // Image value (vector): I/J(_#ind,_x,_y,_z,_c) **= value
              if (!is_inside_critical) is_parallelizable = false;
              p1 = ref[1]; // Index
              is_relative = (bool)ref[2];
              arg3 = ref[3]; // X
              arg4 = ref[4]; // Y
              arg5 = ref[5]; // Z
              if (p_ref) std::memcpy(p_ref,ref,siz_ref);
              if ((p1==~0U && imgout) || (p1!=~0U && imglist)) {
                if (p1!=~0U) _cimg_mp_check_const_index(p1);
                CImg<ulongT>::vector((ulongT)mp_set_IJxyz_v,arg1,(ulongT)is_relative,
                                     p1,arg3,arg4,arg5,size(arg1)).move_to(code);
              }
            }
            _cimg_mp_return(arg1);
          }

        for (s = se2, ps = se3, ns = ps - 1; s>ss1; --s, --ps, --ns) // Here, ns = ps - 1
          if (*s=='=' && (*ps=='+' || *ps=='-' || *ps=='*' || *ps=='/' || (*ps=='%' && s[1]!='=') ||
                          *ps=='&' || *ps=='^' || *ps=='|' ||
                          (*ps=='>' && *ns=='>') || (*ps=='<' && *ns=='<')) &&
              level[s - expr._data]==clevel) { // Self-operators (+=,-=,*=,/=,%=,>>=,<<=,&=,^=,|=)
            switch (*ps) {
            case '+' : op = mp_self_add; _cimg_mp_op("Operator '+='"); break;
            case '-' : op = mp_self_sub; _cimg_mp_op("Operator '-='"); break;
            case '*' : op = mp_self_mul; _cimg_mp_op("Operator '*='"); break;
            case '/' : op = mp_self_div; _cimg_mp_op("Operator '/='"); break;
            case '%' : op = mp_self_modulo; _cimg_mp_op("Operator '%='"); break;
            case '<' : op = mp_self_bitwise_left_shift; _cimg_mp_op("Operator '<<='"); break;
            case '>' : op = mp_self_bitwise_right_shift; _cimg_mp_op("Operator '>>='"); break;
            case '&' : op = mp_self_bitwise_and; _cimg_mp_op("Operator '&='"); break;
            case '|' : op = mp_self_bitwise_or; _cimg_mp_op("Operator '|='"); break;
            default : op = mp_self_pow; _cimg_mp_op("Operator '^='"); break;
            }
            s1 = *ps=='>' || *ps=='<'?ns:ps;

            ref.assign(7);
            arg1 = compile(ss,s1,depth1,ref,block_flags); // Variable slot
            arg2 = compile(s + 1,se,depth1,0,block_flags); // Value to apply

            // Check for particular case to be simplified.
            if ((op==mp_self_add || op==mp_self_sub) && !arg2) _cimg_mp_return(arg1);
            if ((op==mp_self_mul || op==mp_self_div || op==mp_self_pow) && arg2==1) _cimg_mp_return(arg1);
            if (op==mp_self_pow && (arg2==0 || arg2==2)) {
              if (!arg2) CImg<ulongT>::vector((ulongT)mp_copy,arg1,1).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_sqr,arg1,arg1).move_to(code);
              _cimg_mp_return(arg1);
            }

            // Apply operator on a copy to prevent modifying a constant or a variable.
            if (*ref && (is_const_scalar(arg1) || is_vector(arg1) || is_reserved(arg1)))
              arg1 = copy(arg1);

            if (*ref==1) { // Vector value (scalar): V[k] += scalar
              _cimg_mp_check_type(arg2,2,1,0);
              arg3 = ref[1]; // Vector slot
              arg4 = ref[2]; // Index
              if (p_ref) std::memcpy(p_ref,ref,siz_ref);
              CImg<ulongT>::vector((ulongT)op,arg1,arg2).move_to(code);
              CImg<ulongT>::vector((ulongT)mp_vector_set_off,arg1,arg3,(ulongT)size(arg3),arg4).
                move_to(code);
              _cimg_mp_return(arg1);
            }

            if (*ref==2) { // Image value (scalar): i/j[_#ind,off] += scalar
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_check_type(arg2,2,1,0);
              p1 = ref[1]; // Index
              is_relative = (bool)ref[2];
              arg3 = ref[3]; // Offset
              if (p_ref) std::memcpy(p_ref,ref,siz_ref);
              CImg<ulongT>::vector((ulongT)op,arg1,arg2).move_to(code);
              if ((p1==~0U && imgout) || (p1!=~0U && imglist))
                CImg<ulongT>::vector((ulongT)mp_set_ijoff,arg1,(ulongT)is_relative,
                                     p1,arg3).move_to(code);
              _cimg_mp_return(arg1);
            }

            if (*ref==3) { // Image value (scalar): i/j(_#ind,_x,_y,_z,_c) += scalar
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_check_type(arg2,2,1,0);
              p1 = ref[1]; // Index
              is_relative = (bool)ref[2];
              arg3 = ref[3]; // X
              arg4 = ref[4]; // Y
              arg5 = ref[5]; // Z
              arg6 = ref[6]; // C
              if (p_ref) std::memcpy(p_ref,ref,siz_ref);
              CImg<ulongT>::vector((ulongT)op,arg1,arg2).move_to(code);
              if ((p1==~0U && imgout) || (p1!=~0U && imglist))
                CImg<ulongT>::vector((ulongT)mp_set_ijxyzc,arg1,(ulongT)is_relative,
                                     p1,arg3,arg4,arg5,arg6).move_to(code);
              _cimg_mp_return(arg1);
            }

            if (*ref==4) { // Image value (vector): I/J[_#ind,off] += value
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_check_type(arg2,2,3,size(arg1));
              p1 = ref[1]; // Index
              is_relative = (bool)ref[2];
              arg3 = ref[3]; // Offset
              if (p_ref) std::memcpy(p_ref,ref,siz_ref);
              if (is_scalar(arg2)) self_vector_s(arg1,op,arg2); else self_vector_v(arg1,op,arg2);
              if ((p1==~0U && imgout) || (p1!=~0U && imglist))
                CImg<ulongT>::vector((ulongT)mp_set_IJoff_v,arg1,(ulongT)is_relative,
                                     p1,arg3,size(arg1)).move_to(code);
              _cimg_mp_return(arg1);
            }

            if (*ref==5) { // Image value (vector): I/J(_#ind,_x,_y,_z,_c) += value
              if (!is_inside_critical) is_parallelizable = false;
              _cimg_mp_check_type(arg2,2,3,size(arg1));
              p1 = ref[1]; // Index
              is_relative = (bool)ref[2];
              arg3 = ref[3]; // X
              arg4 = ref[4]; // Y
              arg5 = ref[5]; // Z
              if (p_ref) std::memcpy(p_ref,ref,siz_ref);
              if (is_scalar(arg2)) self_vector_s(arg1,op,arg2); else self_vector_v(arg1,op,arg2);
              if ((p1==~0U && imgout) || (p1!=~0U && imglist))
                CImg<ulongT>::vector((ulongT)mp_set_IJxyz_v,arg1,(ulongT)is_relative,
                                     p1,arg3,arg4,arg5,size(arg1)).move_to(code);
              _cimg_mp_return(arg1);
            }

            if (is_vector(arg1)) { // Vector variable: V += value
              _cimg_mp_check_type(arg2,2,3,size(arg1));
              if (is_vector(arg2)) self_vector_v(arg1,op,arg2); // Vector += vector
              else self_vector_s(arg1,op,arg2); // Vector += scalar
              _cimg_mp_return(arg1);
            }

            if (is_reserved(arg1)) { // Scalar variable: s += scalar
              _cimg_mp_check_type(arg2,2,1,0);
              CImg<ulongT>::vector((ulongT)op,arg1,arg2).move_to(code);
              _cimg_mp_return(arg1);
            }

            variable_name.assign(ss,(unsigned int)(s - ss)).back() = 0;
            cimg::strpare(variable_name,false,true);
            cimg::strellipsize(variable_name,64);
            _cimg_mp_strerr;
            throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                        "CImg<%s>::%s: %s: Invalid %slvalue '%s', "
                                        "in expression '%s'.",
                                        pixel_type(),_cimg_mp_calling_function,s_op,
                                        is_const_scalar(arg1)?"const ":"",
                                        variable_name._data,s0);
          }

        for (s = ss1; s<se1; ++s)
          if (*s=='?' && level[s - expr._data]==clevel) { // Ternary operator 'cond?expr1:expr2'
            _cimg_mp_op("Operator '?:'");
            s1 = s + 1; while (s1<se1 && (*s1!=':' || level[s1 - expr._data]!=clevel)) ++s1;
            arg1 = compile(ss,s,depth1,0,block_flags);
            _cimg_mp_check_type(arg1,1,1,0);
            if (is_const_scalar(arg1)) {
              if ((bool)mem[arg1]) return compile(s + 1,*s1!=':'?se:s1,depth1,0,block_flags);
              else return *s1!=':'?0:compile(++s1,se,depth1,0,block_flags);
            }
            p2 = code._width;
            arg2 = compile(s + 1,*s1!=':'?se:s1,depth1,0,block_flags);
            p3 = code._width;
            arg3 = *s1==':'?compile(++s1,se,depth1,0,block_flags):
              is_vector(arg2)?vector(size(arg2),0):0;
            _cimg_mp_check_type(arg3,3,is_vector(arg2)?2:1,size(arg2));
            arg4 = size(arg2);
            if (arg4) pos = vector(arg4); else pos = scalar();
            CImg<ulongT>::vector((ulongT)mp_if,pos,arg1,arg2,arg3,
                                 p3 - p2,code._width - p3,arg4).move_to(code,p2);
            return_comp = true;
            _cimg_mp_return(pos);
          }

        for (s = se3, ns = se2; s>ss; --s, --ns)
          if (*s=='|' && *ns=='|' && level[s - expr._data]==clevel) { // Logical or ('||')
            _cimg_mp_op("Operator '||'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            _cimg_mp_check_type(arg1,1,1,0);
            if (is_const_scalar(arg1) && mem[arg1]) _cimg_mp_return(1);
            p2 = code._width;
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,1,0);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1] || mem[arg2]);
            if (!arg1) _cimg_mp_scalar1(bool,arg2);
            pos = scalar();
            CImg<ulongT>::vector((ulongT)mp_logical_or,pos,arg1,arg2,code._width - p2).move_to(code,p2);
            return_comp = true;
            _cimg_mp_return(pos);
          }

        for (s = se3, ns = se2; s>ss; --s, --ns)
          if (*s=='&' && *ns=='&' && level[s - expr._data]==clevel) { // Logical and ('&&')
            _cimg_mp_op("Operator '&&'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            _cimg_mp_check_type(arg1,1,1,0);
            if (!arg1) _cimg_mp_return(0);
            p2 = code._width;
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,1,0);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1] && mem[arg2]);
            if (is_const_scalar(arg1) && mem[arg1]) _cimg_mp_scalar1(bool,arg2);
            pos = scalar();
            CImg<ulongT>::vector((ulongT)mp_logical_and,pos,arg1,arg2,code._width - p2).
              move_to(code,p2);
            return_comp = true;
            _cimg_mp_return(pos);
          }

        for (s = se2; s>ss; --s)
          if (*s=='|' && level[s - expr._data]==clevel) { // Bitwise or ('|')
            _cimg_mp_op("Operator '|'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(bitwise_or,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) {
              if (!arg2) _cimg_mp_same(arg1);
              _cimg_mp_vector2_vs(bitwise_or,arg1,arg2);
            }
            if (is_scalar(arg1) && is_vector(arg2)) {
              if (!arg1) _cimg_mp_same(arg2);
              _cimg_mp_vector2_sv(bitwise_or,arg1,arg2);
            }
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar((longT)mem[arg1] | (longT)mem[arg2]);
            if (!arg2) _cimg_mp_same(arg1);
            if (!arg1) _cimg_mp_same(arg2);
            _cimg_mp_scalar2(bitwise_or,arg1,arg2);
          }

        for (s = se2; s>ss; --s)
          if (*s=='&' && level[s - expr._data]==clevel) { // Bitwise and ('&')
            _cimg_mp_op("Operator '&'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(bitwise_and,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(bitwise_and,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(bitwise_and,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar((longT)mem[arg1] & (longT)mem[arg2]);
            if (!arg1 || !arg2) _cimg_mp_return(0);
            _cimg_mp_scalar2(bitwise_and,arg1,arg2);
          }

        for (s = se3, ns = se2; s>ss; --s, --ns)
          if (*s=='!' && *ns=='=' && level[s - expr._data]==clevel) { // Not equal to ('!=')
            _cimg_mp_op("Operator '!='");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            if (arg1==arg2) _cimg_mp_return(0);
            p1 = size(arg1);
            p2 = size(arg2);
            if (p1 || p2) {
              if (p1 && p2 && p1!=p2) _cimg_mp_return(1);
              pos = scalar();
              CImg<ulongT>::vector((ulongT)mp_vector_neq,pos,arg1,p1,arg2,p2,11,1).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1]!=mem[arg2]);
            _cimg_mp_scalar2(neq,arg1,arg2);
          }

        for (s = se3, ns = se2; s>ss; --s, --ns)
          if (*s=='=' && *ns=='=' && level[s - expr._data]==clevel) { // Equal to ('==')
            _cimg_mp_op("Operator '=='");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            if (arg1==arg2) _cimg_mp_return(1);
            p1 = size(arg1);
            p2 = size(arg2);
            if (p1 || p2) {
              if (p1 && p2 && p1!=p2) _cimg_mp_return(0);
              pos = scalar();
              CImg<ulongT>::vector((ulongT)mp_vector_eq,pos,arg1,p1,arg2,p2,11,1).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1]==mem[arg2]);
            _cimg_mp_scalar2(eq,arg1,arg2);
          }

        for (s = se3, ns = se2, ps = se4; s>ss; --s, --ns)
          if (*s=='<' && *ns=='=' && *ps!='$' && level[s - expr._data]==clevel) { // Less or equal than ('<=')
            _cimg_mp_op("Operator '<='");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(lte,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(lte,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(lte,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1]<=mem[arg2]);
            if (arg1==arg2) _cimg_mp_return(1);
            _cimg_mp_scalar2(lte,arg1,arg2);
          }

        for (s = se3, ns = se2, ps = se4; s>ss; --s, --ns)
          if (*s=='>' && *ns=='=' && *ps!='$' && level[s - expr._data]==clevel) { // Greater or equal than ('>=')
            _cimg_mp_op("Operator '>='");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(gte,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(gte,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(gte,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1]>=mem[arg2]);
            if (arg1==arg2) _cimg_mp_return(1);
            _cimg_mp_scalar2(gte,arg1,arg2);
          }

        for (s = se2, ns = se1, ps = se3; s>ss; --s, --ns, --ps)
          if (*s=='<' && *ns!='<' && *ps!='<' && *ps!='$' && level[s - expr._data]==clevel) { // Less than ('<')
            _cimg_mp_op("Operator '<'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(lt,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(lt,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(lt,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1]<mem[arg2]);
            if (arg1==arg2) _cimg_mp_return(0);
            _cimg_mp_scalar2(lt,arg1,arg2);
          }

        for (s = se2, ns = se1, ps = se3; s>ss; --s, --ns, --ps)
          if (*s=='>' && *ns!='>' && *ps!='>' && *ps!='$' && level[s - expr._data]==clevel) { // Greater than ('>')
            _cimg_mp_op("Operator '>'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(gt,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(gt,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(gt,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1]>mem[arg2]);
            if (arg1==arg2) _cimg_mp_return(0);
            _cimg_mp_scalar2(gt,arg1,arg2);
          }

        for (s = se3, ns = se2, ps = se4; s>ss; --s, --ns)
          if (*s=='<' && *ns=='<' && *ps!='$' && level[s - expr._data]==clevel) { // Left bit shift ('<<')
            _cimg_mp_op("Operator '<<'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(bitwise_left_shift,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) {
              if (!arg2) _cimg_mp_same(arg1);
              _cimg_mp_vector2_vs(bitwise_left_shift,arg1,arg2);
            }
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(bitwise_left_shift,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar((longT)mem[arg1]<<(unsigned int)mem[arg2]);
            if (!arg1) _cimg_mp_return(0);
            if (!arg2) _cimg_mp_same(arg1);
            _cimg_mp_scalar2(bitwise_left_shift,arg1,arg2);
          }

        for (s = se3, ns = se2, ps = se4; s>ss; --s, --ns)
          if (*s=='>' && *ns=='>' && *ps!='$' && level[s - expr._data]==clevel) { // Right bit shift ('>>')
            _cimg_mp_op("Operator '>>'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(bitwise_right_shift,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) {
              if (!arg2) _cimg_mp_same(arg1);
              _cimg_mp_vector2_vs(bitwise_right_shift,arg1,arg2);
            }
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(bitwise_right_shift,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar((longT)mem[arg1]>>(unsigned int)mem[arg2]);
            if (!arg1) _cimg_mp_return(0);
            if (!arg2) _cimg_mp_same(arg1);
            _cimg_mp_scalar2(bitwise_right_shift,arg1,arg2);
          }

        for (s = se2, ns = se1, ps = pexpr._data + (se3 - expr._data); s>ss; --ns, --s, --ps)
          if (*s=='+' && (*ns!='+' || ns!=se1) && *ps!='-' && *ps!='+' && *ps!='*' && *ps!='/' && *ps!='%' &&
              *ps!='&' && *ps!='|' && *ps!='^' && *ps!='!' && *ps!='~' && *ps!='#' &&
              (*ps!='e' || !(ps - pexpr._data>ss - expr._data && (*(ps - 1)=='.' || (*(ps - 1)>='0' &&
                                                                                     *(ps - 1)<='9')))) &&
              level[s - expr._data]==clevel) { // Addition ('+')
            _cimg_mp_op("Operator '+'");
            p1 = p2 = ~0U;
            const ulongT *ptr1 = 0, *ptr2 = 0;
            p3 = code.size();
            arg1 = compile(ss,s,depth1,0,block_flags);
            if (is_scalar(arg1) && code.size()>p3) { // Spot potential linear case 'a*b + c'
              CImg<ulongT>& pop = code.back();
              if (pop[0]==(ulongT)mp_mul && pop[1]==arg1) { p1 = code.size() - 1; ptr1 = pop.data(); }
            }
            p3 = code.size();
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            if (!ptr1 && is_scalar(arg2) && code.size()>p3) { // Spot potential linear case 'c + a*b'
              CImg<ulongT>& pop = code.back();
              if (pop[0]==(ulongT)mp_mul && pop[1]==arg2) { p2 = code.size() - 1; ptr2 = pop.data(); }
            }
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (!arg2) _cimg_mp_same(arg1);
            if (!arg1) _cimg_mp_same(arg2);
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(add,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(add,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(add,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2)) _cimg_mp_const_scalar(mem[arg1] + mem[arg2]);
            if (p1<code.size() && code[p1].data()==ptr1 &&
                *ptr1==(ulongT)mp_mul && ptr1[1]==(ulongT)arg1) { // Particular case 'a*b + c'
              arg3 = (unsigned int)ptr1[2]; arg4 = (unsigned int)ptr1[3];
              code.remove(p1);
              _cimg_mp_scalar3(mul_add,arg3,arg4,arg2);
            }
            if (p2<code.size() && code[p2].data()==ptr2 &&
                *ptr2==(ulongT)mp_mul && ptr2[1]==(ulongT)arg2) { // Particular case 'c + a*b'
              arg3 = (unsigned int)ptr2[2]; arg4 = (unsigned int)ptr2[3];
              code.remove(p2);
              _cimg_mp_scalar3(mul_add,arg3,arg4,arg1);
            }
            if (arg2==1) _cimg_mp_scalar1(increment,arg1);
            if (arg1==1) _cimg_mp_scalar1(increment,arg2);
            _cimg_mp_scalar2(add,arg1,arg2);
          }

        for (s = se2, ns = se1, ps = pexpr._data + (se3 - expr._data); s>ss; --ns, --s, --ps)
          if (*s=='-' && (*ns!='-' || ns!=se1) && *ps!='-' && *ps!='+' && *ps!='*' && *ps!='/' && *ps!='%' &&
              *ps!='&' && *ps!='|' && *ps!='^' && *ps!='!' && *ps!='~' && *ps!='#' &&
              (*ps!='e' || !(ps - pexpr._data>ss - expr._data && (*(ps - 1)=='.' || (*(ps - 1)>='0' &&
                                                                                     *(ps - 1)<='9')))) &&
              level[s - expr._data]==clevel) { // Subtraction ('-')
            _cimg_mp_op("Operator '-'");
            p1 = p2 = ~0U;
            const ulong *ptr1 = 0, *ptr2 = 0;
            p3 = code.size();
            arg1 = compile(ss,s,depth1,0,block_flags);
            if (is_scalar(arg1) && code.size()>p3) { // Spot potential linear case 'a*b - c'
              CImg<ulongT>& pop = code.back();
              if (pop[0]==(ulongT)mp_mul && pop[1]==arg1) { p1 = code.size() - 1; ptr1 = pop.data(); }
            }
            p3 = code.size();
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            if (!ptr1 && is_scalar(arg2) && code.size()>p3) { // Spot potential linear case 'c - a*b'
              CImg<ulongT>& pop = code.back();
              if (pop[0]==(ulongT)mp_mul && pop[1]==arg2) { p2 = code.size() - 1; ptr2 = pop.data(); }
            }
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (!arg2) _cimg_mp_same(arg1);
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(sub,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(sub,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) {
              if (!arg1) _cimg_mp_vector1_v(minus,arg2);
              _cimg_mp_vector2_sv(sub,arg1,arg2);
            }
            if (is_const_scalar(arg1) && is_const_scalar(arg2)) _cimg_mp_const_scalar(mem[arg1] - mem[arg2]);
            if (!arg1) _cimg_mp_scalar1(minus,arg2);
            if (p1<code.size() && code[p1].data()==ptr1 &&
                *ptr1==(ulongT)mp_mul && ptr1[1]==(ulongT)arg1) { // Particular case 'a*b - c'
              arg3 = (unsigned int)ptr1[2]; arg4 = (unsigned int)ptr1[3];
              code.remove(p1);
              _cimg_mp_scalar3(mul_sub,arg3,arg4,arg2);
            }
            if (p2<code.size() && code[p2].data()==ptr2 &&
                *ptr2==(ulongT)mp_mul && ptr2[1]==(ulongT)arg2) { // Particular case 'c - a*b'
              arg3 = (unsigned int)ptr2[2]; arg4 = (unsigned int)ptr2[3];
              code.remove(p2);
              _cimg_mp_scalar3(sub_mul,arg1,arg3,arg4);
            }
            if (arg2==1) _cimg_mp_scalar1(decrement,arg1);
            _cimg_mp_scalar2(sub,arg1,arg2);
          }

        for (s = se3, ns = se2; s>ss; --s, --ns)
          if (*s=='*' && *ns=='*' && level[s - expr._data]==clevel) { // Complex multiplication ('**')
            _cimg_mp_op("Operator '**'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg1,1,3,2);
            _cimg_mp_check_type(arg2,2,3,2);
            if (arg2==1) _cimg_mp_same(arg1);
            if (arg1==1) _cimg_mp_same(arg2);
            if (is_vector(arg1) && is_vector(arg2)) {
              pos = vector(2);
              CImg<ulongT>::vector((ulongT)mp_complex_mul,pos,arg1,arg2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(mul,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(mul,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1]*mem[arg2]);
            if (!arg1 || !arg2) _cimg_mp_return(0);
            _cimg_mp_scalar2(mul,arg1,arg2);
          }

        for (s = se3, ns = se2; s>ss; --s, --ns)
          if (*s=='/' && *ns=='/' && level[s - expr._data]==clevel) { // Complex division ('//')
            _cimg_mp_op("Operator '//'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg1,1,3,2);
            _cimg_mp_check_type(arg2,2,3,2);
            if (arg2==1) _cimg_mp_same(arg1);
            if (is_vector(arg1) && is_vector(arg2)) {
              pos = vector(2);
              CImg<ulongT>::vector((ulongT)mp_complex_div_vv,pos,arg1,arg2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(div,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) {
              pos = vector(2);
              CImg<ulongT>::vector((ulongT)mp_complex_div_sv,pos,arg1,arg2).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1]/mem[arg2]);
            if (!arg1) _cimg_mp_return(0);
            _cimg_mp_scalar2(div,arg1,arg2);
          }

        for (s = se2; s>ss; --s) if (*s=='*' && level[s - expr._data]==clevel) { // Multiplication ('*')
            _cimg_mp_op("Operator '*'");
            p1 = ~0U;
            const ulongT *ptr1 = 0;
            p3 = code.size();
            arg1 = compile(ss,s,depth1,0,block_flags);
            if (is_scalar(arg1) && code.size()>p3) { // Spot potential double product case 'a*b*c'
              CImg<ulongT>& pop = code.back();
              if (pop[0]==(ulongT)mp_mul && pop[1]==arg1) { p1 = code.size() - 1; ptr1 = pop.data(); }
            }
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            p2 = size(arg2);
            if (p2>0 && (ulongT)size(arg1)==(ulongT)p2*p2) { // Particular case of matrix multiplication
              pos = vector(p2);
              CImg<ulongT>::vector((ulongT)mp_matrix_mul,pos,arg1,arg2,p2,p2,1).move_to(code);
              return_comp = true;
              _cimg_mp_return(pos);
            }
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (arg2==1) _cimg_mp_same(arg1);
            if (arg1==1) _cimg_mp_same(arg2);
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(mul,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(mul,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(mul,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2)) _cimg_mp_const_scalar(mem[arg1]*mem[arg2]);
            if (p1<code.size() && code[p1].data()==ptr1 &&
                *ptr1==(ulongT)mp_mul && ptr1[1]==(ulongT)arg1) { // Particular case 'a*b*c'
              arg3 = (unsigned int)ptr1[2]; arg4 = (unsigned int)ptr1[3];
              code.remove(p1);
              if (is_const_scalar(arg2)) { // Manage cases where '(a,c)' or '(b,c)' are constants
                if (is_const_scalar(arg4)) cimg::swap(arg3,arg4);
                if (is_const_scalar(arg3)) {
                  arg2 = const_scalar(mem[arg2]*mem[arg3]);
                  if (!arg2) _cimg_mp_return(0);
                  _cimg_mp_scalar2(mul,arg2,arg4);
                }
              }
              _cimg_mp_scalar3(mul_mul,arg3,arg4,arg2);
            }
            if (!arg1 || !arg2) _cimg_mp_return(0);
            _cimg_mp_scalar2(mul,arg1,arg2);
          }

        for (s = se2; s>ss; --s) if (*s=='/' && level[s - expr._data]==clevel) { // Division ('/')
            _cimg_mp_op("Operator '/'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (arg2==1) _cimg_mp_same(arg1);
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(div,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(div,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(div,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(mem[arg1]/mem[arg2]);
            if (!arg1) _cimg_mp_return(0);
            _cimg_mp_scalar2(div,arg1,arg2);
          }

        for (s = se2, ns = se1, ps = se3; s>ss; --s, --ns, --ps)
          if (*s=='%' && *ns!='^' && *ps!='$' && level[s - expr._data]==clevel) { // Modulo ('%')
            _cimg_mp_op("Operator '%'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(modulo,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(modulo,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(modulo,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(cimg::mod(mem[arg1],mem[arg2]));
            _cimg_mp_scalar2(modulo,arg1,arg2);
          }

        if (se1>ss) {
          if (*ss=='+' && (*ss1!='+' || (ss2<se && *ss2>='0' && *ss2<='9'))) { // Unary plus ('+')
            _cimg_mp_op("Operator '+'");
            _cimg_mp_return(compile(ss1,se,depth1,0,block_flags));
          }

          if (*ss=='-' && (*ss1!='-' || (ss2<se && *ss2>='0' && *ss2<='9'))) { // Unary minus ('-')
            _cimg_mp_op("Operator '-'");
            arg1 = compile(ss1,se,depth1,0,block_flags);
            if (is_vector(arg1)) _cimg_mp_vector1_v(minus,arg1);
            if (is_const_scalar(arg1)) _cimg_mp_const_scalar(-mem[arg1]);
            _cimg_mp_scalar1(minus,arg1);
          }

          if (*ss=='!') { // Logical not ('!')
            _cimg_mp_op("Operator '!'");
            if (*ss1=='!') { // '!!expr' optimized as 'bool(expr)'
              arg1 = compile(ss2,se,depth1,0,block_flags);
              if (is_vector(arg1)) _cimg_mp_vector1_v(bool,arg1);
              if (is_const_scalar(arg1)) _cimg_mp_const_scalar((bool)mem[arg1]);
              _cimg_mp_scalar1(bool,arg1);
            }
            arg1 = compile(ss1,se,depth1,0,block_flags);
            if (is_vector(arg1)) _cimg_mp_vector1_v(logical_not,arg1);
            if (is_const_scalar(arg1)) _cimg_mp_const_scalar(!mem[arg1]);
            _cimg_mp_scalar1(logical_not,arg1);
          }

          if (*ss=='~') { // Bitwise not ('~')
            _cimg_mp_op("Operator '~'");
            arg1 = compile(ss1,se,depth1,0,block_flags);
            if (is_vector(arg1)) _cimg_mp_vector1_v(bitwise_not,arg1);
            if (is_const_scalar(arg1)) _cimg_mp_const_scalar(~(unsigned int)mem[arg1]);
            _cimg_mp_scalar1(bitwise_not,arg1);
          }
        }

        for (s = se3, ns = se2; s>ss; --s, --ns)
          if (*s=='^' && *ns=='^' && level[s - expr._data]==clevel) { // Complex power ('^^')
            _cimg_mp_op("Operator '^^'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 2,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg1,1,3,2);
            _cimg_mp_check_type(arg2,2,3,2);
            if (arg2==1) _cimg_mp_same(arg1);
            pos = vector(2);
            if (!arg2) CImg<ulongT>::vector((ulongT)mp_complex_one,pos).move_to(code);
            else if (arg2==2) {
              if (is_scalar(arg1)) CImg<ulongT>::vector((ulongT)mp_complex_sqr,pos,arg1,0).move_to(code);
              else CImg<ulongT>::vector((ulongT)mp_complex_sqr,pos,arg1 + 1,arg1 + 2).move_to(code);
            } else {
              if (is_vector(arg1) && is_vector(arg2))
                CImg<ulongT>::vector((ulongT)mp_complex_pow_vv,pos,arg1,arg2).move_to(code);
              else if (is_vector(arg1) && is_scalar(arg2))
                CImg<ulongT>::vector((ulongT)mp_complex_pow_vs,pos,arg1,arg2).move_to(code);
              else if (is_scalar(arg1) && is_vector(arg2))
                CImg<ulongT>::vector((ulongT)mp_complex_pow_sv,pos,arg1,arg2).move_to(code);
              else
                CImg<ulongT>::vector((ulongT)mp_complex_pow_ss,pos,arg1,arg2).move_to(code);
            }
            return_comp = true;
            _cimg_mp_return(pos);
          }

        for (s = se2; s>ss; --s)
          if (*s=='^' && level[s - expr._data]==clevel) { // Power ('^')
            _cimg_mp_op("Operator '^'");
            arg1 = compile(ss,s,depth1,0,block_flags);
            arg2 = compile(s + 1,se,depth1,0,block_flags);
            _cimg_mp_check_type(arg2,2,3,size(arg1));
            if (arg2==1) _cimg_mp_same(arg1);
            if (is_vector(arg1) && is_vector(arg2)) _cimg_mp_vector2_vv(pow,arg1,arg2);
            if (is_vector(arg1) && is_scalar(arg2)) _cimg_mp_vector2_vs(pow,arg1,arg2);
            if (is_scalar(arg1) && is_vector(arg2)) _cimg_mp_vector2_sv(pow,arg1,arg2);
            if (is_const_scalar(arg1) && is_const_scalar(arg2))
              _cimg_mp_const_scalar(std::pow(mem[arg1],mem[arg2]));
            switch (arg2) {
            case 0 : _cimg_mp_return(1);
            case 2 : _cimg_mp_scalar1(sqr,arg1);
            case 3 : _cimg_mp_scalar1(pow3,arg1);
            case 4 : _cimg_mp_scalar1(pow4,arg1);
            default :
              if (is_const_scalar(arg2)) {
                if (mem[arg2]==0.5) { _cimg_mp_scalar1(sqrt,arg1); }
                else if (mem[arg2]==0.25) { _cimg_mp_scalar1(pow0_25,arg1); }
              }
              _cimg_mp_scalar2(pow,arg1,arg2);
            }
          }

      cimg_skip_iterative_operator_parsing :

        // Percentage computation.
        if (*se1=='%' && se1>ss && *se2!='$') {
          arg1 = compile(ss,se1,depth1,0,block_flags);
          arg2 = is_const_scalar(arg1)?0:const_scalar(100);
          if (is_vector(arg1)) _cimg_mp_vector2_vs(div,arg1,arg2);
          if (is_const_scalar(arg1)) _cimg_mp_const_scalar(mem[arg1]/100);
          _cimg_mp_scalar2(div,arg1,arg2);
        }

        // Degree to radian postfix operator ('°' in UTF-8).
        if (se2>ss && (unsigned char)*se2==0xC2 && (unsigned char)*se1==0xB0) {
          arg1 = compile(ss,se2,depth1,0,block_flags);
          if (is_vector(arg1)) _cimg_mp_vector1_v(deg2rad,arg1);
          if (is_const_scalar(arg1)) _cimg_mp_const_scalar(mem[arg1]*cimg::PI/180);
          _cimg_mp_scalar1(deg2rad,arg1);
        }

        // Pre/post-decrement and increment.
        is_sth = ss1<se1 && (*ss=='+' || *ss=='-') && *ss1==*ss; // is pre-?
        if (is_sth || (se2>ss && (*se1=='+' || *se1=='-') && *se2==*se1)) {
          if ((is_sth && *ss=='+') || (!is_sth && *se1=='+')) {
            _cimg_mp_op("Operator '++'");
            op = mp_self_increment;
          } else {
            _cimg_mp_op("Operator '--'");
            op = mp_self_decrement;
          }
          ref.assign(7);
          arg1 = is_sth?compile(ss2,se,depth1,ref,block_flags):
            compile(ss,se2,depth1,ref,block_flags); // Variable slot

          // Apply operator on a copy to prevent modifying a constant or a variable.
          if (*ref && (is_const_scalar(arg1) || is_vector(arg1) || is_reserved(arg1)))
            arg1 = copy(arg1);

          pos = is_sth?arg1:copy(arg1); // Determine return index, depending on pre/post action

          if (*ref==1) { // Vector value (scalar): V[k]++
            arg3 = ref[1]; // Vector slot
            arg4 = ref[2]; // Index
            if (is_sth && p_ref) std::memcpy(p_ref,ref,ref._width*sizeof(unsigned int));
            CImg<ulongT>::vector((ulongT)op,arg1,1).move_to(code);
            CImg<ulongT>::vector((ulongT)mp_vector_set_off,arg1,arg3,(ulongT)size(arg3),arg4).
              move_to(code);
            _cimg_mp_return(pos);
          }

          if (*ref==2) { // Image value (scalar): i/j[_#ind,off]++
            if (!is_inside_critical) is_parallelizable = false;
            p1 = ref[1]; // Index
            is_relative = (bool)ref[2];
            arg3 = ref[3]; // Offset
            if (is_sth && p_ref) std::memcpy(p_ref,ref,ref._width*sizeof(unsigned int));
            CImg<ulongT>::vector((ulongT)op,arg1).move_to(code);
            if ((p1==~0U && imgout) || (p1!=~0U && imglist))
              CImg<ulongT>::vector((ulongT)mp_set_ijoff,arg1,(ulongT)is_relative,
                                   p1,arg3).move_to(code);
            _cimg_mp_return(pos);
          }

          if (*ref==3) { // Image value (scalar): i/j(_#ind,_x,_y,_z,_c)++
            if (!is_inside_critical) is_parallelizable = false;
            p1 = ref[1]; // Index
            is_relative = (bool)ref[2];
            arg3 = ref[3]; // X
            arg4 = ref[4]; // Y
            arg5 = ref[5]; // Z
            arg6 = ref[6]; // C
            if (is_sth && p_ref) std::memcpy(p_ref,ref,ref._width*sizeof(unsigned int));
            CImg<ulongT>::vector((ulongT)op,arg1).move_to(code);
            if ((p1==~0U && imgout) || (p1!=~0U && imglist))
              CImg<ulongT>::vector((ulongT)mp_set_ijxyzc,arg1,(ulongT)is_relative,
                                   p1,arg3,arg4,arg5,arg6).move_to(code);
            _cimg_mp_return(pos);
          }

          if (*ref==4) { // Image value (vector): I/J[_#ind,off]++
            if (!is_inside_critical) is_parallelizable = false;
            p1 = ref[1]; // Index
            is_relative = (bool)ref[2];
            arg3 = ref[3]; // Offset
            if (is_sth && p_ref) std::memcpy(p_ref,ref,ref._width*sizeof(unsigned int));
            self_vector_s(arg1,op==mp_self_increment?mp_self_add:mp_self_sub,1);
            if ((p1==~0U && imgout) || (p1!=~0U && imglist))
              CImg<ulongT>::vector((ulongT)mp_set_IJoff_v,arg1,(ulongT)is_relative,
                                   p1,arg3,size(arg1)).move_to(code);
            _cimg_mp_return(pos);
          }

          if (*ref==5) { // Image value (vector): I/J(_#ind,_x,_y,_z,_c)++
            if (!is_inside_critical) is_parallelizable = false;
            p1 = ref[1]; // Index
            is_relative = (bool)ref[2];
            arg3 = ref[3]; // X
            arg4 = ref[4]; // Y
            arg5 = ref[5]; // Z
            if (is_sth && p_ref) std::memcpy(p_ref,ref,ref._width*sizeof(unsigned int));
            self_vector_s(arg1,op==mp_self_increment?mp_self_add:mp_self_sub,1);
            if ((p1==~0U && imgout) || (p1!=~0U && imglist))
              CImg<ulongT>::vector((ulongT)mp_set_IJxyz_v,arg1,(ulongT)is_relative,
                                   p1,arg3,arg4,arg5,size(arg1)).move_to(code);
            _cimg_mp_return(pos);
          }

          if (is_vector(arg1)) { // Vector variable: V++
            self_vector_s(arg1,op==mp_self_increment?mp_self_add:mp_self_sub,1);
            _cimg_mp_return(pos);
          }

          if (is_reserved(arg1)) { // Scalar variable: s++
            CImg<ulongT>::vector((ulongT)op,arg1).move_to(code);
            _cimg_mp_return(pos);
          }

          if (is_sth) variable_name.assign(ss2,(unsigned int)(se - ss1));
          else variable_name.assign(ss,(unsigned int)(se1 - ss));
          variable_name.back() = 0;
          cimg::strpare(variable_name,false,true);
          cimg::strellipsize(variable_name,64);
          _cimg_mp_strerr;
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s: Invalid %slvalue '%s', "
                                      "in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,
                                      is_const_scalar(arg1)?"const ":"",
                                      variable_name._data,s0);
        }


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
                                    s_op,variable_name._data,s0);
      }

#endif
