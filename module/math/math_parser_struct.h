#ifndef CIMG_MODULE_MATH_PARSER_STRUCT_H
#define CIMG_MODULE_MATH_PARSER_STRUCT_H



    //! \name Mathematical Functions
    //@{
    //-------------------------------------

    // Define the math formula parser/compiler and expression evaluator.
    struct _cimg_math_parser {
      CImg<doubleT> mem;
      CImg<intT> memtype, memmerge;
      CImgList<ulongT> _code, &code, code_begin, code_end,
        _code_begin_t, &code_begin_t, _code_end_t, &code_end_t;
      CImg<ulongT> opcode;
      const CImg<ulongT> *p_code_end, *p_code;
      const CImg<ulongT> *const p_break;

      CImg<charT> expr, pexpr;
      const CImg<T>& imgin;
      CImg<T> &imgout;
      CImgList<T>& imglist;

      CImg<doubleT> _img_stats, &img_stats, constcache_vals;
      CImgList<doubleT> _list_stats, &list_stats, _list_median, &list_median, _list_norm, &list_norm;
      CImg<uintT> mem_img_stats, constcache_inds;

      CImg<uintT> level, variable_pos, reserved_label;
      CImgList<charT> variable_def, macro_def, macro_body;
      char *user_macro;

      unsigned int mempos, mem_img_median, mem_img_norm, mem_img_index, debug_indent,
        result_dim, result_end_dim, break_type, constcache_size;
      bool is_parallelizable, is_noncritical_run, is_end_code, is_fill, need_input_copy, return_comp;
      double *result, *result_end;
      cimg_uint64 rng;
      const char *const calling_function, *s_op, *ss_op;
      typedef double (*mp_func)(_cimg_math_parser&);

#define _cimg_mp_calling_function s_calling_function()._data
#define _cimg_mp_check_const_scalar(arg,n_arg,mode) check_const_scalar(arg,n_arg,mode,ss,se,saved_char)
#define _cimg_mp_check_const_index(arg) check_const_index(arg,ss,se,saved_char)
#define _cimg_mp_check_notnan_index(arg,s_arg) check_notnan_index(arg,s_arg,ss,se,saved_char)
#define _cimg_mp_check_list() check_list(ss,se,saved_char)
#define _cimg_mp_check_matrix_square(arg,n_arg) check_matrix_square(arg,n_arg,ss,se,saved_char)
#define _cimg_mp_check_type(arg,n_arg,mode,N) check_type(arg,n_arg,mode,N,ss,se,saved_char)
#define _cimg_mp_const_scalar(val) _cimg_mp_return(const_scalar((double)(val)))
#define _cimg_mp_op(s) s_op = s; ss_op = ss
#define _cimg_mp_return(x) { *se = saved_char; s_op = previous_s_op; ss_op = previous_ss_op; return x; }
#define _cimg_mp_return_nan() _cimg_mp_return(_cimg_mp_slot_nan)
#define _cimg_mp_same(x) _cimg_mp_return(same(x));
#define _cimg_mp_scalar0(op) _cimg_mp_return(scalar0(mp_##op))
#define _cimg_mp_scalar1(op,i1) _cimg_mp_return(scalar1(mp_##op,i1))
#define _cimg_mp_scalar2(op,i1,i2) _cimg_mp_return(scalar2(mp_##op,i1,i2))
#define _cimg_mp_scalar3(op,i1,i2,i3) _cimg_mp_return(scalar3(mp_##op,i1,i2,i3))
#define _cimg_mp_scalar4(op,i1,i2,i3,i4) _cimg_mp_return(scalar4(mp_##op,i1,i2,i3,i4))
#define _cimg_mp_scalar5(op,i1,i2,i3,i4,i5) _cimg_mp_return(scalar5(mp_##op,i1,i2,i3,i4,i5))
#define _cimg_mp_scalar6(op,i1,i2,i3,i4,i5,i6) _cimg_mp_return(scalar6(mp_##op,i1,i2,i3,i4,i5,i6))
#define _cimg_mp_scalar8(op,i1,i2,i3,i4,i5,i6,i7,i8) _cimg_mp_return(scalar8(mp_##op,i1,i2,i3,i4,i5,i6,i7,i8))
#define _cimg_mp_strerr \
  *se = saved_char; \
  for (s0 = ss; s0>expr._data && *s0!=';'; --s0) {} \
  if (*s0==';') ++s0; \
  while (cimg::is_blank(*s0)) ++s0; \
  cimg::strellipsize(s0,64)
#define _cimg_mp_vector1_v(op,i1) _cimg_mp_return(vector1_v(mp_##op,i1))
#define _cimg_mp_vector2_sv(op,i1,i2) _cimg_mp_return(vector2_sv(mp_##op,i1,i2))
#define _cimg_mp_vector2_vs(op,i1,i2) _cimg_mp_return(vector2_vs(mp_##op,i1,i2))
#define _cimg_mp_vector2_vv(op,i1,i2) _cimg_mp_return(vector2_vv(mp_##op,i1,i2))
#define _cimg_mp_vector3_vss(op,i1,i2,i3) _cimg_mp_return(vector3_vss(mp_##op,i1,i2,i3))
#define _cimg_mp_vector4_vsss(op,i1,i2,i3,i4) _cimg_mp_return(vector4_vsss(mp_##op,i1,i2,i3,i4))

      // Constructors / Destructors.
      ~_cimg_math_parser() {
        cimg::srand(rng);
      }

      _cimg_math_parser(const char *const expression, const char *const funcname=0,
                        const CImg<T>& img_input=CImg<T>::const_empty(), CImg<T> *const img_output=0,
                        CImgList<T> *const list_images=0, const bool _is_fill=false):
        code(_code),code_begin_t(_code_begin_t),code_end_t(_code_end_t),
        p_break((CImg<ulongT>*)(cimg_ulong)-2),imgin(img_input),
        imgout(img_output?*img_output:CImg<T>::empty()),imglist(list_images?*list_images:CImgList<T>::empty()),
        img_stats(_img_stats),list_stats(_list_stats),list_median(_list_median),list_norm(_list_norm),user_macro(0),
        mem_img_median(~0U),mem_img_norm(~0U),mem_img_index(~0U),debug_indent(0),result_dim(0),result_end_dim(0),
        break_type(0),constcache_size(0),is_parallelizable(true),is_noncritical_run(false),is_fill(_is_fill),
        need_input_copy(false),result_end(0),rng((cimg::_rand(),cimg::rng())),
        calling_function(funcname?funcname:"cimg_math_parser") {

#if cimg_use_openmp!=0
        rng+=omp_get_thread_num();
#endif
        if (!expression || !*expression)
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: Empty expression.",
                                      pixel_type(),_cimg_mp_calling_function);
        const char *_expression = expression;
        while (*_expression && (cimg::is_blank(*_expression) || *_expression==';')) ++_expression;
        CImg<charT>::string(_expression).move_to(expr);
        char *ps = &expr.back() - 1;
        while (ps>expr._data && (cimg::is_blank(*ps) || *ps==';')) --ps;
        *(++ps) = 0; expr._width = (unsigned int)(ps - expr._data + 1);

        // Ease the retrieval of previous non-space characters afterwards.
        pexpr.assign(expr._width);
        char c, *pe = pexpr._data;
        for (ps = expr._data, c = ' '; *ps; ++ps) {
          if (!cimg::is_blank(*ps)) c = *ps; else *ps = ' ';
          *(pe++) = c;
        }
        *pe = 0;
        level = get_level(expr);

        // Init constant values.
#define _cimg_mp_interpolation (reserved_label[31]!=~0U?reserved_label[31]:0)
#define _cimg_mp_boundary (reserved_label[32]!=~0U?reserved_label[32]:0)
#define _cimg_mp_slot_t 17
#define _cimg_mp_slot_nan 30
#define _cimg_mp_slot_x 31
#define _cimg_mp_slot_y 32
#define _cimg_mp_slot_z 33
#define _cimg_mp_slot_c 34

        mem.assign(96);
        for (unsigned int i = 0; i<=10; ++i) mem[i] = (double)i; // mem[0-10] = 0...10
        for (unsigned int i = 1; i<=5; ++i) mem[i + 10] = -(double)i; // mem[11-15] = -1...-5
        mem[16] = 0.5;
        mem[_cimg_mp_slot_t] = 0; // thread_id
        mem[18] = (double)imgin._width; // w
        mem[19] = (double)imgin._height; // h
        mem[20] = (double)imgin._depth; // d
        mem[21] = (double)imgin._spectrum; // s
        mem[22] = (double)imgin._is_shared; // r
        mem[23] = (double)imgin._width*imgin._height; // wh
        mem[24] = (double)imgin._width*imgin._height*imgin._depth; // whd
        mem[25] = (double)imgin._width*imgin._height*imgin._depth*imgin._spectrum; // whds
        mem[26] = (double)imglist._width; // l
        mem[27] = std::exp(1.); // e
        mem[28] = cimg::PI; // pi
        mem[29] = DBL_EPSILON; // eps
        mem[_cimg_mp_slot_nan] = cimg::type<double>::nan(); // nan

        // Type property for each value in memory :
        // { -1 = reserved (e.g. variable) | 0 = computation scalar |
        //    1 = compile-time constant | N>1 = start of a vector(#N-1) }.
        memtype.assign(mem._width,1,1,1,0);
        for (unsigned int i = 0; i<_cimg_mp_slot_x; ++i) memtype[i] = 1;
        memtype[_cimg_mp_slot_t] = memtype[_cimg_mp_slot_x] = memtype[_cimg_mp_slot_y] =
          memtype[_cimg_mp_slot_z] = memtype[_cimg_mp_slot_c] = -1;
        mempos = _cimg_mp_slot_c + 1;
        variable_pos.assign(8);

        reserved_label.assign(128,1,1,1,~0U);
        // reserved_label[0-32] are used to store the memory index of these variables:
        // [0] = wh, [1] = whd, [2] = whds, [3] = pi, [4] = im, [5] = iM, [6] = ia, [7] = iv, [8] = id,
        // [9] = is, [10] = ip, [11] = ic, [12] = in, [13] = xm, [14] = ym, [15] = zm, [16] = cm, [17] = xM,
        // [18] = yM, [19] = zM, [20] = cM, [21] = i0...[30] = i9, [31] = interpolation, [32] = boundary, [33] = eps

        // Compile expression into a sequence of opcodes.
        s_op = ""; ss_op = expr._data;
        const unsigned int ind_result = compile(expr._data,expr._data + expr._width - 1,0,0,0);
        if (!is_const_scalar(ind_result)) {
          if (is_vector(ind_result))
            CImg<doubleT>(&mem[ind_result] + 1,size(ind_result),1,1,1,true).
              fill(cimg::type<double>::nan());
          else if (ind_result!=_cimg_mp_slot_t) mem[ind_result] = cimg::type<double>::nan();
        }
        if (mem._width>=256 && mem._width - mempos>=mem._width/2) mem.resize(mempos,1,1,1,-1);
        result_dim = size(ind_result);
        result = mem._data + ind_result;

        // Free resources used for compiling expression and prepare evaluation.
        memtype.assign();
        constcache_vals.assign();
        constcache_inds.assign();
        level.assign();
        variable_pos.assign();
        reserved_label.assign();
        expr.assign();
        pexpr.assign();
        opcode.assign();
        opcode._is_shared = true;
        p_code_end = code.end();
        begin();
      }

      _cimg_math_parser():
        code(_code),code_begin_t(_code_begin_t),code_end_t(_code_end_t),
        p_code_end(0),p_break((CImg<ulongT>*)(cimg_ulong)-2),
        imgin(CImg<T>::const_empty()),imgout(CImg<T>::empty()),imglist(CImgList<T>::empty()),
        img_stats(_img_stats),list_stats(_list_stats),list_median(_list_median),list_norm(_list_norm),debug_indent(0),
        result_dim(0),result_end_dim(0),break_type(0),constcache_size(0),is_parallelizable(true),
        is_noncritical_run(false),is_fill(false),need_input_copy(false),
        result_end(0),rng(0),calling_function(0) {
        mem.assign(1 + _cimg_mp_slot_c,1,1,1,0); // Allow to skip 'is_empty?' test in operator()()
        result = mem._data;
      }

      _cimg_math_parser(const _cimg_math_parser& mp):
        mem(mp.mem),code(mp.code),code_begin_t(mp.code_begin_t),code_end_t(mp.code_end_t),
        p_code_end(mp.p_code_end),p_break(mp.p_break),
        imgin(mp.imgin),imgout(mp.imgout),imglist(mp.imglist),
        img_stats(mp.img_stats),list_stats(mp.list_stats),list_median(mp.list_median),list_norm(mp.list_norm),
        debug_indent(0),result_dim(mp.result_dim),result_end_dim(mp.result_end_dim),break_type(0),constcache_size(0),
        is_parallelizable(mp.is_parallelizable),is_noncritical_run(mp.is_noncritical_run),is_fill(mp.is_fill),
        need_input_copy(mp.need_input_copy),
        result(mem._data + (mp.result - mp.mem._data)),
        result_end(mp.result_end?mem._data + (mp.result_end - mp.mem._data):0),
        rng((cimg::_rand(),cimg::rng())),calling_function(0) {

#if cimg_use_openmp!=0
        mem[_cimg_mp_slot_t] = (double)omp_get_thread_num();
        rng+=omp_get_thread_num();
#endif
        opcode.assign();
        opcode._is_shared = true;
      }

      // Compilation procedure.
#endif
