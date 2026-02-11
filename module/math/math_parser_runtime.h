#ifndef CIMG_MODULE_MATH_PARSER_RUNTIME_H
#define CIMG_MODULE_MATH_PARSER_RUNTIME_H

                                    s_op,variable_name._data,s0);
      }

      // Evaluate a portion of the compiled expression.
      void eval(const CImg<ulongT> *const p_begin, const CImg<ulongT> *const p_end) {
        for (p_code = p_begin; p_code<p_end; ++p_code) {
          opcode._data = p_code->_data;
          const ulongT target = opcode[1];
          mem[target] = (*(mp_func)*opcode)(*this);
        }
      }

      // Evaluate expression for a given set of (x,y,z,c) coordinates (return scalar value).
      double operator()(const double x, const double y, const double z, const double c) {
        mem[_cimg_mp_slot_x] = x; mem[_cimg_mp_slot_y] = y; mem[_cimg_mp_slot_z] = z; mem[_cimg_mp_slot_c] = c;
        eval(code,p_code_end);
        return *result;
      }

      // Evaluate expression for a given set of (x,y,z,c) coordinates (return output values in vector 'output').
      template<typename t>
      void operator()(const double x, const double y, const double z, const double c, t *const output) {
        mem[_cimg_mp_slot_x] = x; mem[_cimg_mp_slot_y] = y; mem[_cimg_mp_slot_z] = z; mem[_cimg_mp_slot_c] = c;
        eval(code,p_code_end);
        if (result_dim) {
          const double *ptrs = result + 1;
          t *ptrd = output;
          for (unsigned int k = 0; k<result_dim; ++k) *(ptrd++) = (t)*(ptrs++);
        } else *output = (t)*result;
      }

      // Evaluate begin() block.
      void begin() {
        if (!code_begin) return;
        mem[_cimg_mp_slot_x] = mem[_cimg_mp_slot_y] = mem[_cimg_mp_slot_z] = mem[_cimg_mp_slot_c] = 0;
        eval(code_begin,code_begin.end());
      }

      // Evaluate begin_t() block.
      void begin_t() {
        if (!code_begin_t) return;
        mem[_cimg_mp_slot_x] = mem[_cimg_mp_slot_y] = mem[_cimg_mp_slot_z] = mem[_cimg_mp_slot_c] = 0;
        eval(code_begin_t,code_begin_t.end());
      }

      // Evaluate end_t() block.
      void end_t() {
        if (!code_end_t) return;
        if (imgin) {
          mem[_cimg_mp_slot_x] = imgin._width - 1.;
          mem[_cimg_mp_slot_y] = imgin._height - 1.;
          mem[_cimg_mp_slot_z] = imgin._depth - 1.;
          mem[_cimg_mp_slot_c] = imgin._spectrum - 1.;
        } else mem[_cimg_mp_slot_x] = mem[_cimg_mp_slot_y] = mem[_cimg_mp_slot_z] = mem[_cimg_mp_slot_c] = 0;
        eval(code_end_t,code_end_t.end());
      }

      // Evaluate end() block.
      void end() {
        if (!code_end) return;
        if (imgin) {
          mem[_cimg_mp_slot_x] = imgin._width - 1.;
          mem[_cimg_mp_slot_y] = imgin._height - 1.;
          mem[_cimg_mp_slot_z] = imgin._depth - 1.;
          mem[_cimg_mp_slot_c] = imgin._spectrum - 1.;
        } else mem[_cimg_mp_slot_x] = mem[_cimg_mp_slot_y] = mem[_cimg_mp_slot_z] = mem[_cimg_mp_slot_c] = 0;
        eval(code_end,code_end.end());
      }

      // Merge inter-thread variables.
      // (argument 'mp' is the master instance).
      void merge(_cimg_math_parser& mp) {
        if (&mp==this) return;
        cimg_rofY(mp.memmerge,k) {
          const unsigned int
            pos = (unsigned int)mp.memmerge(0,k),
            siz = (unsigned int)mp.memmerge(1,k),
            iop = (unsigned int)mp.memmerge(2,k);
          if (!siz) switch (iop) { // Scalar value
            case 0 : mp.mem[pos] = mem[pos]; break; // Assignment
            case 1 : mp.mem[pos]+=mem[pos]; break; // Operator+
            case 2 : mp.mem[pos]-=mem[pos]; break; // Operator-
            case 3 : mp.mem[pos]*=mem[pos]; break; // Operator*
            case 4 : mp.mem[pos]/=mem[pos]; break; // Operator/
            case 5 : mp.mem[pos] = (double)((longT)mp.mem[pos] & (longT)mem[pos]); break; // Operator&
            case 6 : mp.mem[pos] = (double)((longT)mp.mem[pos] | (longT)mem[pos]); break; // Operator|
            case 7 : mp.mem[pos] = (double)((longT)mp.mem[pos] ^ (longT)mem[pos]); break; // Operator 'xor'
            case 8 : mp.mem[pos] = mp.mem[pos] && mem[pos]; break; // Operator&&
            case 9 : mp.mem[pos] = mp.mem[pos] || mem[pos]; break; // Operator||
            case 10 : mp.mem[pos] = std::min(mp.mem[pos],mem[pos]); break; // Operator 'min'
            case 11 : mp.mem[pos] = std::max(mp.mem[pos],mem[pos]); break; // Operator 'max'
            } else switch (iop) { // Vector value
            case 0 : // Assignment
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true) = CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true);
              break;
            case 1 : // Operator+
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true)+=CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true);
              break;
            case 2 : // Operator-
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true)-=CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true);
              break;
            case 3 : // Operator*
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true).mul(CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true));
              break;
            case 4 : // Operator/
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true).div(CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true));
              break;
            case 5 : // Operator&
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true)&=CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true);
              break;
            case 6 : // Operator|
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true)|=CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true);
              break;
            case 7 : // Operator 'xor'
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true)^=CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true);
              break;
            case 8 : { // Operator&&
              CImg<doubleT>
                arg1(&mp.mem[pos + 1],siz,1,1,1,true),
                arg2(&mem[pos + 1],siz,1,1,1,true);
              cimg_foroff(arg1,off) arg1[off] = arg1[off] && arg2[off];
            } break;
            case 9 : { // Operator||
              CImg<doubleT>
                arg1(&mp.mem[pos + 1],siz,1,1,1,true),
                arg2(&mem[pos + 1],siz,1,1,1,true);
              cimg_foroff(arg1,off) arg1[off] = arg1[off] || arg2[off];
            } break;
            case 10 : // Operator 'min'
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true).min(CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true));
              break;
            case 11 : // Operator 'max'
              CImg<doubleT>(&mp.mem[pos + 1],siz,1,1,1,true).max(CImg<doubleT>(&mem[pos + 1],siz,1,1,1,true));
              break;
            }
        }
      }

      // Return specified argument number as a string.
      static const char *s_argth(const unsigned int n_arg) {
        const char
          *_s_arg[] = { "", "First", "Second", "Third", "Fourth", "Fifth", "Sixth", "Seventh", "Eighth","Ninth",
          "10th", "11th", "12th", "13th", "14th", "15th", "16th", "17th", "18th", "19th",
          "20th", "21st", "22nd", "23rd", "24th", "25th", "26th", "27th", "28th", "One of the" };
        return _s_arg[n_arg<sizeof(_s_arg)/sizeof(char*)?n_arg:sizeof(_s_arg)/sizeof(char*)-1];
      }

      // Return a string that defines the calling function + the user-defined function scope.
      CImg<charT> s_calling_function() const {
        CImg<charT> res;
        const unsigned int
          l1 = calling_function?(unsigned int)std::strlen(calling_function):0U,
          l2 = user_macro?(unsigned int)std::strlen(user_macro):0U;
        if (l2) {
          res.assign(l1 + l2 + 48);
          cimg_snprintf(res,res._width,"%s(): When substituting function '%s()'",calling_function,user_macro);
        } else {
          res.assign(l1 + l2 + 4);
          cimg_snprintf(res,res._width,"%s()",calling_function);
        }
        return res;
      }

      // Return type of a memory slot as a string.
      CImg<charT> s_type(const unsigned int arg) const {
        CImg<charT> res;
        if (is_vector(arg)) { // Vector
          CImg<charT>::string("vectorXXXXXXXXXXXXXXXX").move_to(res);
          cimg_snprintf(res._data + 6,res._width - 6,"%u",size(arg));
        } else if (is_const_scalar(arg)) CImg<charT>::string("const scalar").move_to(res); // Const scalar
        else CImg<charT>::string("scalar").move_to(res); // Scalar
        return res;
      }

      // Return reference state of a memory slot as a string.
      CImg<charT> s_ref(const unsigned int *const p_ref) const {
        CImg<charT> res;
        if (!p_ref || !*p_ref) return res.assign(1,1,1,1,0);
        res.assign(32);
        switch (p_ref[0]) {
        case 1 : // Reference to vector value as a scalar
          cimg_snprintf(res,res._width,", ref: ([%u])[%u]",
                        p_ref[1],p_ref[2]);
          break;
        case 2 : // Reference to image value as a scalar (offset)
          if (p_ref[1]==~0U)
            cimg_snprintf(res,res._width,", ref: %c[%u]",
                          p_ref[2]?'j':'i',p_ref[3]);
          else
            cimg_snprintf(res,res._width,", ref: %c[#%u,%u]",
                          p_ref[2]?'j':'i',p_ref[1],p_ref[3]);
          break;
        case 3 : // Reference to image value as a scalar (coordinates)
          if (p_ref[1]==~0U)
            cimg_snprintf(res,res._width,", ref: %c(%u,%u,%u,%u)",
                          p_ref[2]?'j':'i',p_ref[3],p_ref[4],p_ref[5],p_ref[6]);
          else
            cimg_snprintf(res,res._width,", ref: %c(#%u,%u,%u,%u,%u)",
                          p_ref[2]?'j':'i',p_ref[1],p_ref[3],p_ref[4],p_ref[5],p_ref[6]);
          break;
        case 4 : // Reference to image value as a vector (offset)
          if (p_ref[1]==~0U)
            cimg_snprintf(res,res._width,", ref: %c[%u]",
                          p_ref[2]?'J':'I',p_ref[3]);
          else
            cimg_snprintf(res,res._width,", ref: %c[#%u,%u]",
                          p_ref[2]?'J':'I',p_ref[1],p_ref[3]);
          break;
        case 5 : // Reference to image value as a vector (coordinates)
          if (p_ref[1]==~0U)
            cimg_snprintf(res,res._width,", ref: %c(%u,%u,%u)",
                          p_ref[2]?'J':'I',p_ref[3],p_ref[4],p_ref[5]);
          else
            cimg_snprintf(res,res._width,", ref: %c(#%u,%u,%u,%u)",
                          p_ref[2]?'J':'I',p_ref[1],p_ref[3],p_ref[4],p_ref[5]);
          break;
        }
        return res;
      }

      // Count parentheses/brackets level of each character of the expression.
      CImg<uintT> get_level(CImg<charT>& _expr) const {
        bool is_escaped = false, next_is_escaped = false;
        unsigned int mode = 0, next_mode = 0; // { 0=normal | 1=char-string | 2=vector-string
        CImg<uintT> res(_expr._width - 1);
        unsigned int *pd = res._data;
        int _level = 0;
        for (const char *ps = _expr._data; *ps && _level>=0; ++ps) {
          if (!is_escaped && !next_is_escaped && *ps=='\\') next_is_escaped = true;
          if (!is_escaped && *ps=='\'') { // Non-escaped character
            if (!mode && ps>_expr._data && *(ps - 1)=='[') next_mode = mode = 2; // Start vector-string
            else if (mode==2 && *(ps + 1)==']') next_mode = !mode; // End vector-string
            else if (mode<2) next_mode = mode?(mode = 0):1; // Start/end char-string
          }
          *(pd++) = (unsigned int)(mode>=1 || is_escaped?_level + (mode==1):
                                   *ps=='(' || *ps=='['?_level++:
                                   *ps==')' || *ps==']'?--_level:
                                   _level);
          mode = next_mode;
          is_escaped = next_is_escaped;
          next_is_escaped = false;
        }
        if (mode) {
          cimg::strellipsize(_expr,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: Unterminated string literal, in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,
                                      _expr._data);
        }
        if (_level) {
          cimg::strellipsize(_expr,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: Unbalanced parentheses/brackets, in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,
                                      _expr._data);
        }
        return res;
      }

      // Find and return index of current image 'imgin' within image list 'imglist'.
      unsigned int get_mem_img_index() {
        if (mem_img_index==~0U) {
          if (&imgout>=imglist.data() && &imgout<imglist.end())
            mem_img_index = const_scalar((double)(&imgout - imglist.data()));
          else {
            unsigned int pos = ~0U;
            cimglist_for(imglist,l)
              if (imgout._data==imglist[l]._data && imgout.is_sameXYZC(imglist[l])) { pos = l; break; }
            if (pos!=~0U) mem_img_index = const_scalar((double)pos);
          }
        }
        return mem_img_index;
      }

      // Return indices for accessing math parser variables.
      void get_variable_pos(const char *variable_name, unsigned int &pos, unsigned int &rpos) {
        char c1, c2, c3, c4;
        pos = rpos = ~0U;
        if (!variable_name || !*variable_name) return;

        unsigned int rp = variable_name[1]?~0U:*variable_name; // One-char variable
        if (variable_name[1] && !variable_name[2]) { // Two-chars variable
          c1 = variable_name[0];
          c2 = variable_name[1];
          if (c1=='w' && c2=='h') rp = 0; // wh
          else if (c1=='p' && c2=='i') rp = 3; // pi
          else if (c1=='i') {
            if (c2>='0' && c2<='9') rp = 21 + c2 - '0'; // i0...i9
            else if (c2=='m') rp = 4; // im
            else if (c2=='M') rp = 5; // iM
            else if (c2=='a') rp = 6; // ia
            else if (c2=='v') rp = 7; // iv
            else if (c2=='d') rp = 8; // id
            else if (c2=='s') rp = 9; // is
            else if (c2=='p') rp = 10; // ip
            else if (c2=='c') rp = 11; // ic
            else if (c2=='n') rp = 12; // in
          } else if (c2=='m') {
            if (c1=='x') rp = 13; // xm
            else if (c1=='y') rp = 14; // ym
            else if (c1=='z') rp = 15; // zm
            else if (c1=='c') rp = 16; // cm
          } else if (c2=='M') {
            if (c1=='x') rp = 17; // xM
            else if (c1=='y') rp = 18; // yM
            else if (c1=='z') rp = 19; // zM
            else if (c1=='c') rp = 20; // cM
          }
        } else if (variable_name[1] && variable_name[2] && !variable_name[3]) { // Three-chars variable
          c1 = variable_name[0];
          c2 = variable_name[1];
          c3 = variable_name[2];
          if (c1=='w' && c2=='h' && c3=='d') rp = 1; // whd
          else if (c1=='e' && c2=='p' && c3=='s') rp = 33; // eps
        } else if (variable_name[1] && variable_name[2] && variable_name[3] &&
                   !variable_name[4]) { // Four-chars variable
          c1 = variable_name[0];
          c2 = variable_name[1];
          c3 = variable_name[2];
          c4 = variable_name[3];
          if (c1=='w' && c2=='h' && c3=='d' && c4=='s') rp = 2; // whds
        } else if (!std::strcmp(variable_name,"interpolation")) rp = 31; // interpolation
        else if (!std::strcmp(variable_name,"boundary")) rp = 32; // boundary

        if (rp!=~0U) { rpos = rp; return; } // One of the reserved labels

        // Multi-char variable name : check for existing variable with same name.
        cimglist_for(variable_def,i)
          if (!std::strcmp(variable_name,variable_def[i])) { pos = i; break; }
      }

      // Return true if all values of a vector are (temporary) computational values.
      bool is_comp_vector(const unsigned int arg) const {
        unsigned int siz = size(arg);
        if (siz>128) return false;
        const int *ptr = memtype.data(arg + 1);
        bool is_tmp = true;
        while (siz-->0) if (*(ptr++)) { is_tmp = false; break; }
        return is_tmp;
      }

      // Return true if specified scalar value is a (temporary) computational value.
      bool is_comp_scalar(const unsigned int arg) const {
        return !memtype[arg];
      }

      // Return true if specified value is a constant scalar.
      bool is_const_scalar(const unsigned int arg) const {
        return memtype[arg]==1;
      }

      bool is_const_integer(const unsigned int arg) const {
        return memtype[arg]==1 && (double)(int)mem[arg]==mem[arg];
      }

      // Return true if specified value is a scalar reserved value (e.g. variable).
      bool is_reserved(const unsigned int arg) const {
        return memtype[arg]==-1;
      }

      // Return true if specified value is a scalar.
      bool is_scalar(const unsigned int arg) const {
        return memtype[arg]<2;
      }

      // Return true if specified value is a vector.
      bool is_vector(const unsigned int arg) const {
        return memtype[arg]>1;
      }

      // Return size of specified value (0: scalar, N>0: vectorN).
      unsigned int size(const unsigned int arg) const {
        return is_scalar(arg)?0U:memtype[arg] - 1U;
      }

      // Check if a memory slot is a positive integer constant scalar value.
      // 'mode' can be:
      // { 0=constant | 1=integer constant | 2=positive integer constant | 3=strictly-positive integer constant }.
      void check_const_scalar(const unsigned int arg, const unsigned int n_arg,
                              const unsigned int mode,
                              char *const ss, char *const se, const char saved_char) {
        _cimg_mp_check_type(arg,n_arg,1,0);
        if (!is_const_scalar(arg)) {
          const char *const s_arg = s_argth(n_arg);
          char *s0;
          _cimg_mp_strerr;
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s %s%s (of type '%s') is not a constant, "
                                      "in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s_arg,*s_arg?" argument":" Argument",s_type(arg)._data,s0);
        }
        const double val = mem[arg];

        if (!((!mode || (double)(int)mem[arg]==mem[arg]) &&
              (mode<2 || mem[arg]>=(mode==3)))) {
          const char *const s_arg = s_argth(n_arg);
          char *s0;
          _cimg_mp_strerr;
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s %s%s (of type '%s' and value %g) is not a%s constant, "
                                      "in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s_arg,*s_arg?" argument":" Argument",s_type(arg)._data,val,
                                      !mode?"":mode==1?"n integer":
                                      mode==2?" positive integer":" strictly positive integer",s0);
        }
      }

      // Check if an image index is a constant value.
      void check_const_index(const unsigned int arg,
                             char *const ss, char *const se, const char saved_char) {
        if (arg!=~0U && !is_const_scalar(arg)) {
          char *s0;
          _cimg_mp_strerr;
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s Specified image index is not a constant, "
                                      "in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",s0);
        }
      }

      // Check that specified constant is not nan.
      void check_notnan_index(const unsigned int arg, const char *const s_arg,
                              char *const ss, char *const se, const char saved_char) {
        if (arg!=~0U &&
            (arg==_cimg_mp_slot_nan || (is_const_scalar(arg) && cimg::type<double>::is_nan(mem[arg])))) {
          char *s0;
          _cimg_mp_strerr;
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s Specified index '%s' is NaN.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",s_arg);
        }
      }

      // Check a matrix is square.
      void check_matrix_square(const unsigned int arg, const unsigned int n_arg,
                               char *const ss, char *const se, const char saved_char) {
        _cimg_mp_check_type(arg,n_arg,2,0);
        const unsigned int
          siz = size(arg),
          n = (unsigned int)cimg::round(std::sqrt((float)siz));
        if (n*n!=siz) {
          const char *s_arg;
          if (*s_op!='F') s_arg = !n_arg?"":n_arg==1?"Left-hand":"Right-hand";
          else s_arg = !n_arg?"":n_arg==1?"First":n_arg==2?"Second":n_arg==3?"Third":"One";
          char *s0;
          _cimg_mp_strerr;
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s %s%s (of type '%s') "
                                      "cannot be considered as a square matrix, in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s_arg,*s_op=='F'?(*s_arg?" argument":" Argument"):(*s_arg?" operand":" Operand"),
                                      s_type(arg)._data,s0);
        }
      }

      // Check type compatibility for one argument.
      // Bits of 'mode' tells what types are allowed:
      // { 1 = scalar | 2 = vectorN }.
      // If 'N' is not zero, it also restricts the vectors to be of size N only.
      void check_type(const unsigned int arg, const unsigned int n_arg,
                      const unsigned int mode, const unsigned int N,
                      char *const ss, char *const se, const char saved_char) {
        const bool
          _is_scalar = is_scalar(arg),
          _is_vector = is_vector(arg) && (!N || size(arg)==N);
        bool cond = false;
        if (mode&1) cond|=_is_scalar;
        if (mode&2) cond|=_is_vector;
        if (!cond) {
          const char *s_arg;
          if (*s_op!='F') s_arg = !n_arg?"":n_arg==1?"Left-hand":"Right-hand";
          else s_arg = s_argth(n_arg);
          CImg<charT> sb_type(32);
          if (mode==1) cimg_snprintf(sb_type,sb_type._width,"'scalar'");
          else if (mode==2) {
            if (N) cimg_snprintf(sb_type,sb_type._width,"'vector%u'",N);
            else cimg_snprintf(sb_type,sb_type._width,"'vector'");
          } else {
            if (N) cimg_snprintf(sb_type,sb_type._width,"'scalar' or 'vector%u'",N);
            else cimg_snprintf(sb_type,sb_type._width,"'scalar' or 'vector'");
          }
          char *s0;
          _cimg_mp_strerr;
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s %s%s has invalid type '%s' (should be %s), "
                                      "in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s_arg,*s_op=='F'?(*s_arg?" argument":" Argument"):(*s_arg?" operand":" Operand"),
                                      s_type(arg)._data,sb_type._data,s0);
        }
      }

      // Check that imglist are not empty.
      void check_list(char *const ss, char *const se, const char saved_char) {
        if (!imglist) {
          char *s0;
          _cimg_mp_strerr;
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s Image list cannot be empty, for expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",s0);
        }
      }

      static void mp_check_list(_cimg_math_parser& mp, const char *const funcname) {
        if (!mp.imglist)
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>: Function '%s()': Images list cannot be empty.",
                                      pixel_type(),funcname);
      }

      // Insert constant value in memory.
      unsigned int const_scalar(const double val) {

        // Search for built-in constant.
        if (cimg::type<double>::is_nan(val)) return _cimg_mp_slot_nan;
        if (val==(double)(int)val) {
          if (val>=0 && val<=10) return (unsigned int)val;
          if (val<0 && val>=-5) return (unsigned int)(10 - val);
        }
        if (val==0.5) return 16;

        // Search for constant already requested before (in const cache).
        unsigned int ind = ~0U;
        if (constcache_size<1024) {
          if (!constcache_size) {
            constcache_vals.assign(16,1,1,1,0);
            constcache_inds.assign(16,1,1,1,0);
            *constcache_vals = val;
            constcache_size = 1;
            ind = 0;
          } else { // Dichotomic search
            const double val_beg = *constcache_vals, val_end = constcache_vals[constcache_size - 1];
            if (val_beg>=val) ind = 0;
            else if (val_end==val) ind = constcache_size - 1;
            else if (val_end<val) ind = constcache_size;
            else {
              unsigned int i0 = 1, i1 = constcache_size - 2;
              while (i0<=i1) {
                const unsigned int mid = (i0 + i1)/2;
                if (constcache_vals[mid]==val) { i0 = mid; break; }
                else if (constcache_vals[mid]<val) i0 = mid + 1;
                else i1 = mid - 1;
              }
              ind = i0;
            }

            if (ind>=constcache_size || constcache_vals[ind]!=val) {
              ++constcache_size;
              if (constcache_size>constcache_vals._width) {
                constcache_vals.resize(-200,1,1,1,0);
                constcache_inds.resize(-200,1,1,1,0);
              }
              const int l = constcache_size - (int)ind - 1;
              if (l>0) {
                std::memmove(&constcache_vals[ind + 1],&constcache_vals[ind],l*sizeof(double));
                std::memmove(&constcache_inds[ind + 1],&constcache_inds[ind],l*sizeof(unsigned int));
              }
              constcache_vals[ind] = val;
              constcache_inds[ind] = 0;
            }
          }
          if (constcache_inds[ind]) return constcache_inds[ind];
        }

        // Insert new constant in memory if necessary.
        if (mempos>=mem._width) { mem.resize(-200,1,1,1,0); memtype.resize(-200,1,1,1,0); }
        const unsigned int pos = mempos++;
        mem[pos] = val;
        memtype[pos] = 1; // Set constant property
        if (ind!=~0U) constcache_inds[ind] = pos;
        return pos;
      }

      // Insert new scalar in memory.
      unsigned int scalar() {
        if (mempos>=mem._width) { mem.resize(-200,1,1,1,0); memtype.resize(mem._width,1,1,1,0); }
        return mempos++;
      }

      // Insert new vector of specified size in memory.
      unsigned int vector(const unsigned int siz) {
        if (mempos + siz>=mem._width) {
          mem.resize(2*mem._width + siz,1,1,1,0);
          memtype.resize(mem._width,1,1,1,0);
        }
        const unsigned int pos = mempos++;
        mem[pos] = cimg::type<double>::nan();
        memtype[pos] = siz + 1;
        mempos+=siz;
        return pos;
      }

      // Insert new initialized vector.
      unsigned int vector(const unsigned int siz, const double value) {
        const unsigned int pos = vector(siz);
        double *ptr = &mem[pos] + 1;
        for (unsigned int i = 0; i<siz; ++i) *(ptr++) = value;
        return pos;
      }

      // Return a copy of the specified value.
      // (this forces a copy to be made. Use 'same()' for cases where you may want to return 'arg').
      unsigned int copy(const unsigned int arg) {
        const unsigned int siz = size(arg);
        return_comp = true;
        if (siz) { // Vector
          const unsigned int pos = vector(siz);
          CImg<ulongT>::vector((ulongT)mp_vector_copy,pos,arg,siz).move_to(code);
          return pos;
        }
        return scalar1(mp_copy,arg); // Scalar
      }

      // Return same value as specified.
      // (this avoids a copy to be made when possible. Use 'copy()' to force the copy of a value).
      unsigned int same(const unsigned int arg) {
        if (is_const_scalar(arg)) return arg;
        if (is_comp_scalar(arg)) { return_comp = true; return arg; }
        const unsigned int siz = size(arg);
        if (siz) { // Vector
          if (is_comp_vector(arg)) { return_comp = true; return arg; }
          const unsigned int pos = vector(siz);
          CImg<ulongT>::vector((ulongT)mp_vector_copy,pos,arg,siz).move_to(code);
          return pos;
        }
        return_comp = true;
        return scalar1(mp_copy,arg); // Scalar
      }

      // Set reserved status to all values of a vector.
      void set_reserved_vector(const unsigned int arg) {
        unsigned int siz = size(arg);
        int *ptr = memtype.data(arg + 1);
        while (siz-->0) *(ptr++) = -1;
      }

      unsigned int scalar0(const mp_func op) {
        const unsigned int pos = scalar();
        CImg<ulongT>::vector((ulongT)op,pos).move_to(code);
        return_comp = true;
        return pos;
      }

      unsigned int scalar1(const mp_func op,
                           const unsigned int arg1) {
        const unsigned int pos =
          arg1!=~0U && arg1>_cimg_mp_slot_c && is_comp_scalar(arg1) && op!=mp_copy?arg1:
          ((return_comp = true), scalar());
        CImg<ulongT>::vector((ulongT)op,pos,arg1).move_to(code);
        return pos;
      }

      unsigned int scalar2(const mp_func op,
                           const unsigned int arg1, const unsigned int arg2) {
        const unsigned int pos =
          arg1!=~0U && arg1>_cimg_mp_slot_c && is_comp_scalar(arg1)?arg1:
          arg2!=~0U && arg2>_cimg_mp_slot_c && is_comp_scalar(arg2)?arg2:
          ((return_comp = true), scalar());
        CImg<ulongT>::vector((ulongT)op,pos,arg1,arg2).move_to(code);
        return pos;
      }

      unsigned int scalar3(const mp_func op,
                           const unsigned int arg1, const unsigned int arg2, const unsigned int arg3) {
        const unsigned int pos =
          arg1!=~0U && arg1>_cimg_mp_slot_c && is_comp_scalar(arg1)?arg1:
          arg2!=~0U && arg2>_cimg_mp_slot_c && is_comp_scalar(arg2)?arg2:
          arg3!=~0U && arg3>_cimg_mp_slot_c && is_comp_scalar(arg3)?arg3:
          ((return_comp = true), scalar());
        CImg<ulongT>::vector((ulongT)op,pos,arg1,arg2,arg3).move_to(code);
        return pos;
      }

      unsigned int scalar4(const mp_func op,
                           const unsigned int arg1, const unsigned int arg2, const unsigned int arg3,
                           const unsigned int arg4) {
        const unsigned int pos =
          arg1!=~0U && arg1>_cimg_mp_slot_c && is_comp_scalar(arg1)?arg1:
          arg2!=~0U && arg2>_cimg_mp_slot_c && is_comp_scalar(arg2)?arg2:
          arg3!=~0U && arg3>_cimg_mp_slot_c && is_comp_scalar(arg3)?arg3:
          arg4!=~0U && arg4>_cimg_mp_slot_c && is_comp_scalar(arg4)?arg4:
          ((return_comp = true), scalar());
        CImg<ulongT>::vector((ulongT)op,pos,arg1,arg2,arg3,arg4).move_to(code);
        return pos;
      }

      unsigned int scalar5(const mp_func op,
                           const unsigned int arg1, const unsigned int arg2, const unsigned int arg3,
                           const unsigned int arg4, const unsigned int arg5) {
        const unsigned int pos =
          arg1!=~0U && arg1>_cimg_mp_slot_c && is_comp_scalar(arg1)?arg1:
          arg2!=~0U && arg2>_cimg_mp_slot_c && is_comp_scalar(arg2)?arg2:
          arg3!=~0U && arg3>_cimg_mp_slot_c && is_comp_scalar(arg3)?arg3:
          arg4!=~0U && arg4>_cimg_mp_slot_c && is_comp_scalar(arg4)?arg4:
          arg5!=~0U && arg5>_cimg_mp_slot_c && is_comp_scalar(arg5)?arg5:
          ((return_comp = true), scalar());
        CImg<ulongT>::vector((ulongT)op,pos,arg1,arg2,arg3,arg4,arg5).move_to(code);
        return pos;
      }

      unsigned int scalar6(const mp_func op,
                           const unsigned int arg1, const unsigned int arg2, const unsigned int arg3,
                           const unsigned int arg4, const unsigned int arg5, const unsigned int arg6) {
        const unsigned int pos =
          arg1!=~0U && arg1>_cimg_mp_slot_c && is_comp_scalar(arg1)?arg1:
          arg2!=~0U && arg2>_cimg_mp_slot_c && is_comp_scalar(arg2)?arg2:
          arg3!=~0U && arg3>_cimg_mp_slot_c && is_comp_scalar(arg3)?arg3:
          arg4!=~0U && arg4>_cimg_mp_slot_c && is_comp_scalar(arg4)?arg4:
          arg5!=~0U && arg5>_cimg_mp_slot_c && is_comp_scalar(arg5)?arg5:
          arg6!=~0U && arg6>_cimg_mp_slot_c && is_comp_scalar(arg6)?arg6:
          ((return_comp = true), scalar());
        CImg<ulongT>::vector((ulongT)op,pos,arg1,arg2,arg3,arg4,arg5,arg6).move_to(code);
        return pos;
      }

      unsigned int scalar8(const mp_func op,
                           const unsigned int arg1, const unsigned int arg2, const unsigned int arg3,
                           const unsigned int arg4, const unsigned int arg5, const unsigned int arg6,
                           const unsigned int arg7, const unsigned int arg8) {
        const unsigned int pos =
          arg1!=~0U && arg1>_cimg_mp_slot_c && is_comp_scalar(arg1)?arg1:
          arg2!=~0U && arg2>_cimg_mp_slot_c && is_comp_scalar(arg2)?arg2:
          arg3!=~0U && arg3>_cimg_mp_slot_c && is_comp_scalar(arg3)?arg3:
          arg4!=~0U && arg4>_cimg_mp_slot_c && is_comp_scalar(arg4)?arg4:
          arg5!=~0U && arg5>_cimg_mp_slot_c && is_comp_scalar(arg5)?arg5:
          arg6!=~0U && arg6>_cimg_mp_slot_c && is_comp_scalar(arg6)?arg6:
          arg7!=~0U && arg7>_cimg_mp_slot_c && is_comp_scalar(arg7)?arg7:
          arg8!=~0U && arg8>_cimg_mp_slot_c && is_comp_scalar(arg8)?arg8:
          ((return_comp = true), scalar());
        CImg<ulongT>::vector((ulongT)op,pos,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8).move_to(code);
        return pos;
      }

      void self_vector_s(const unsigned int pos, const mp_func op, const unsigned int arg1) {
        const unsigned int siz = size(pos);
        if (siz>24) CImg<ulongT>::vector((ulongT)mp_self_map_vector_s,pos,siz,(ulongT)op,arg1).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1).move_to(code[code._width - 1 - siz + k]);
        }
      }

      void self_vector_v(const unsigned int pos, const mp_func op, const unsigned int arg1) {
        const unsigned int siz = size(pos);
        if (siz>24) CImg<ulongT>::vector((ulongT)mp_self_map_vector_v,pos,siz,(ulongT)op,arg1).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1 + k).move_to(code[code._width - 1 - siz + k]);
        }
      }

      unsigned int vector1_v(const mp_func op,
                             const unsigned int arg1) {
        const unsigned int
          siz = size(arg1),
          pos = is_comp_vector(arg1)?arg1:
          ((return_comp = true), vector(siz));
        if (siz>24) CImg<ulongT>::vector((ulongT)mp_vector_map_v,pos,1,siz,(ulongT)op,arg1).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1 + k).move_to(code[code._width - 1 - siz + k]);
        }
        return pos;
      }

      unsigned int vector2_vv(const mp_func op,
                              const unsigned int arg1, const unsigned int arg2) {
        const unsigned int
          siz = size(arg1),
          pos = is_comp_vector(arg1)?arg1:is_comp_vector(arg2)?arg2:
          ((return_comp = true), vector(siz));
        if (siz>24) CImg<ulongT>::vector((ulongT)mp_vector_map_vv,pos,2,siz,(ulongT)op,arg1,arg2).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1 + k,arg2 + k).move_to(code[code._width - 1 - siz + k]);
        }
        return pos;
      }

      unsigned int vector2_vs(const mp_func op,
                              const unsigned int arg1, const unsigned int arg2) {
        const unsigned int
          siz = size(arg1),
          pos = is_comp_vector(arg1)?arg1:
          ((return_comp = true), vector(siz));
        if (siz>24) CImg<ulongT>::vector((ulongT)mp_vector_map_v,pos,2,siz,(ulongT)op,arg1,arg2).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1 + k,arg2).move_to(code[code._width - 1 - siz + k]);
        }
        return pos;
      }

      unsigned int vector2_sv(const mp_func op,
                              const unsigned int arg1, const unsigned int arg2) {
        const unsigned int
          siz = size(arg2),
          pos = is_comp_vector(arg2)?arg2:
          ((return_comp = true), vector(siz));
        if (siz>24) CImg<ulongT>::vector((ulongT)mp_vector_map_sv,pos,2,siz,(ulongT)op,arg1,arg2).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1,arg2 + k).move_to(code[code._width - 1 - siz + k]);
        }
        return pos;
      }

      unsigned int vector3_vss(const mp_func op,
                               const unsigned int arg1, const unsigned int arg2, const unsigned int arg3) {
        const unsigned int
          siz = size(arg1),
          pos = is_comp_vector(arg1)?arg1:
          ((return_comp = true), vector(siz));
        if (siz>24) CImg<ulongT>::vector((ulongT)mp_vector_map_v,pos,3,siz,(ulongT)op,arg1,arg2,arg3).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1 + k,arg2,arg3).move_to(code[code._width - 1 - siz + k]);
        }
        return pos;
      }

      unsigned int vector4_vvss(const mp_func op,
                                const unsigned int arg1, const unsigned int arg2, const unsigned int arg3,
                                const unsigned int arg4) {
        const unsigned int
          siz = size(arg1),
          pos = is_comp_vector(arg1)?arg1:is_comp_vector(arg2)?arg2:
          ((return_comp = true), vector(siz));
        if (siz>24)
          CImg<ulongT>::vector((ulongT)mp_vector_map_vv,pos,4,siz,(ulongT)op,arg1,arg2,arg3,arg4).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1 + k,arg2 + k,arg3,arg4).
              move_to(code[code._width - 1 - siz + k]);
        }
        return pos;
      }

      unsigned int vector4_vsss(const mp_func op,
                                const unsigned int arg1, const unsigned int arg2, const unsigned int arg3,
                                const unsigned int arg4) {
        const unsigned int
          siz = size(arg1),
          pos = is_comp_vector(arg1)?arg1:
          ((return_comp = true), vector(siz));
        if (siz>24)
          CImg<ulongT>::vector((ulongT)mp_vector_map_v,pos,4,siz,(ulongT)op,arg1,arg2,arg3,arg4).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1 + k,arg2,arg3,arg4).
              move_to(code[code._width - 1 - siz + k]);
        }
        return pos;
      }

      unsigned int vector4_svss(const mp_func op,
                                const unsigned int arg1, const unsigned int arg2, const unsigned int arg3,
                                const unsigned int arg4) {
        const unsigned int
          siz = size(arg2),
          pos = is_comp_vector(arg2)?arg2:
          ((return_comp = true), vector(siz));
        if (siz>24)
          CImg<ulongT>::vector((ulongT)mp_vector_map_sv,pos,4,siz,(ulongT)op,arg1,arg2,arg3,arg4).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1,arg2 + k,arg3,arg4).
              move_to(code[code._width - 1 - siz + k]);
        }
        return pos;
      }

      // Evaluation mathematical operators and functions, known by the parser.
      // Defining these functions 'static' ensures that sizeof(mp_func)==sizeof(ulongT),
      // so we can store pointers to them directly in the opcode vectors.
#endif
