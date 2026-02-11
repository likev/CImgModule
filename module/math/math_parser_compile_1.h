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

#endif
