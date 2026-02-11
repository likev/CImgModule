#ifndef CIMG_MODULE_IMAGE_LOAD_PNG_JPEG_H
#define CIMG_MODULE_IMAGE_LOAD_PNG_JPEG_H

    //! Load image from a PNG file.
    /**
       \param filename Filename, as a C-string.
       \param[out] bits_per_value Number of bits used to store a scalar value in the image file.
    **/
    CImg<T>& load_png(const char *const filename, unsigned int *const bits_per_value=0) {
      return _load_png(0,filename,bits_per_value);
    }

    //! Load image from a PNG file \newinstance.
    static CImg<T> get_load_png(const char *const filename, unsigned int *const bits_per_value=0) {
      return CImg<T>().load_png(filename,bits_per_value);
    }

    //! Load image from a PNG file \overloading.
    CImg<T>& load_png(std::FILE *const file, unsigned int *const bits_per_value=0) {
      return _load_png(file,0,bits_per_value);
    }

    //! Load image from a PNG file \newinstance.
    static CImg<T> get_load_png(std::FILE *const file, unsigned int *const bits_per_value=0) {
      return CImg<T>().load_png(file,bits_per_value);
    }

    // (Note: Most of this function has been written by Eric Fausett).
    CImg<T>& _load_png(std::FILE *const file, const char *const filename, unsigned int *const bits_per_value) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_png(): Specified filename is (null).",
                                    cimg_instance);

#ifndef cimg_use_png
      cimg::unused(bits_per_value);
      if (file)
        throw CImgIOException(_cimg_instance
                              "load_png(): Unable to load data from '(FILE*)' unless libpng is enabled.",
                              cimg_instance);

      else return load_other(filename);
#else
      // Open file and check for PNG validity.
#if defined __GNUC__
      const char *volatile nfilename = filename; // Use 'volatile' to avoid (wrong) g++ warning
      std::FILE *volatile nfile = file?file:cimg::fopen(nfilename,"rb");
#else
      const char *nfilename = filename;
      std::FILE *nfile = file?file:cimg::fopen(nfilename,"rb");
#endif
      unsigned char pngCheck[8] = {};
      cimg::fread(pngCheck,8,(std::FILE*)nfile);
      if (png_sig_cmp(pngCheck,0,8)) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_png(): Invalid PNG file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }

      // Setup PNG structures for read.
      png_voidp user_error_ptr = 0;
      png_error_ptr user_error_fn = 0, user_warning_fn = 0;
      png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,user_error_ptr,user_error_fn,user_warning_fn);
      if (!png_ptr) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_png(): Failed to initialize 'png_ptr' structure for file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_infop info_ptr = png_create_info_struct(png_ptr);
      if (!info_ptr) {
        if (!file) cimg::fclose(nfile);
        png_destroy_read_struct(&png_ptr,(png_infopp)0,(png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Failed to initialize 'info_ptr' structure for file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_infop end_info = png_create_info_struct(png_ptr);
      if (!end_info) {
        if (!file) cimg::fclose(nfile);
        png_destroy_read_struct(&png_ptr,&info_ptr,(png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Failed to initialize 'end_info' structure for file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }

      // Error handling callback for png file reading.
      if (setjmp(png_jmpbuf(png_ptr))) {
        if (!file) cimg::fclose((std::FILE*)nfile);
        png_destroy_read_struct(&png_ptr, &end_info, (png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Encountered unknown fatal error in libpng for file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_init_io(png_ptr, nfile);
      png_set_sig_bytes(png_ptr, 8);

      // Get PNG Header Info up to data block.
      png_read_info(png_ptr,info_ptr);
      png_uint_32 W, H;
      int bit_depth, color_type, interlace_type;
      bool is_gray = false;
      png_get_IHDR(png_ptr,info_ptr,&W,&H,&bit_depth,&color_type,&interlace_type,(int*)0,(int*)0);
      png_set_interlace_handling(png_ptr);
      if (bits_per_value) *bits_per_value = (unsigned int)bit_depth;

      // Transforms to unify image data.
      if (color_type==PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
        color_type = PNG_COLOR_TYPE_RGB;
        bit_depth = 8;
      }
      if (color_type==PNG_COLOR_TYPE_GRAY && bit_depth<8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
        is_gray = true;
        bit_depth = 8;
      }
      if (png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
        color_type |= PNG_COLOR_MASK_ALPHA;
      }
      if (color_type==PNG_COLOR_TYPE_GRAY || color_type==PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);
        color_type |= PNG_COLOR_MASK_COLOR;
        is_gray = true;
      }
      if (color_type==PNG_COLOR_TYPE_RGB)
        png_set_filler(png_ptr,0xffffU,PNG_FILLER_AFTER);

      png_read_update_info(png_ptr,info_ptr);
      if (bit_depth!=8 && bit_depth!=16) {
        if (!file) cimg::fclose(nfile);
        png_destroy_read_struct(&png_ptr,&end_info,(png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Invalid bit depth %u in file '%s'.",
                              cimg_instance,
                              bit_depth,nfilename?nfilename:"(FILE*)");
      }
      const int byte_depth = bit_depth>>3;

      // Allocate memory for image reading.
      png_bytep *const imgData = new png_bytep[H];
      for (unsigned int row = 0; row<H; ++row) imgData[row] = new png_byte[(size_t)byte_depth*4*W];
      png_read_image(png_ptr,imgData);
      png_read_end(png_ptr,end_info);

      // Read pixel data.
      if (color_type!=PNG_COLOR_TYPE_RGB && color_type!=PNG_COLOR_TYPE_RGB_ALPHA) {
        if (!file) cimg::fclose(nfile);
        png_destroy_read_struct(&png_ptr,&end_info,(png_infopp)0);
        throw CImgIOException(_cimg_instance
                              "load_png(): Invalid color coding type %u in file '%s'.",
                              cimg_instance,
                              color_type,nfilename?nfilename:"(FILE*)");
      }
      const bool is_alpha = (color_type==PNG_COLOR_TYPE_RGBA);
      try { assign(W,H,1,(is_gray?1:3) + (is_alpha?1:0)); }
      catch (...) { if (!file) cimg::fclose(nfile); throw; }
      T
        *ptr_r = data(0,0,0,0),
        *ptr_g = is_gray?0:data(0,0,0,1),
        *ptr_b = is_gray?0:data(0,0,0,2),
        *ptr_a = !is_alpha?0:data(0,0,0,is_gray?1:3);
      switch (bit_depth) {
      case 8 : {
        cimg_forY(*this,y) {
          const unsigned char *ptrs = (unsigned char*)imgData[y];
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            if (ptr_g) *(ptr_g++) = (T)*(ptrs++); else ++ptrs;
            if (ptr_b) *(ptr_b++) = (T)*(ptrs++); else ++ptrs;
            if (ptr_a) *(ptr_a++) = (T)*(ptrs++); else ++ptrs;
          }
        }
      } break;
      case 16 : {
        cimg_forY(*this,y) {
          const unsigned short *ptrs = (unsigned short*)(imgData[y]);
          if (!cimg::endianness()) cimg::invert_endianness(ptrs,4*_width);
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            if (ptr_g) *(ptr_g++) = (T)*(ptrs++); else ++ptrs;
            if (ptr_b) *(ptr_b++) = (T)*(ptrs++); else ++ptrs;
            if (ptr_a) *(ptr_a++) = (T)*(ptrs++); else ++ptrs;
          }
        }
      } break;
      }
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

      // Deallocate image read memory.
      cimg_forY(*this,n) delete[] imgData[n];
      delete[] imgData;
      if (!file) cimg::fclose(nfile);
      return *this;
#endif
    }

    //! Load image from a PNM file.
    /**
      \param filename Filename, as a C-string.
    **/
    CImg<T>& load_pnm(const char *const filename) {
      return _load_pnm(0,filename);
    }

    //! Load image from a PNM file \newinstance.
    static CImg<T> get_load_pnm(const char *const filename) {
      return CImg<T>().load_pnm(filename);
    }

    //! Load image from a PNM file \overloading.
    CImg<T>& load_pnm(std::FILE *const file) {
      return _load_pnm(file,0);
    }

    //! Load image from a PNM file \newinstance.
    static CImg<T> get_load_pnm(std::FILE *const file) {
      return CImg<T>().load_pnm(file);
    }

    CImg<T>& _load_pnm(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_pnm(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      unsigned int ppm_type, W, H, D = 1, colormax = 255;
      CImg<charT> item(16384,1,1,1,0);
      int err, rval, gval, bval;
      const longT cimg_iobuffer = (longT)24*1024*1024;
      while ((err=std::fscanf(nfile,"%16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
      if (cimg_sscanf(item," P%u",&ppm_type)!=1) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pnm(): PNM header not found in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      while ((err=std::fscanf(nfile," %16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
      if ((err=cimg_sscanf(item," %u %u %u %u",&W,&H,&D,&colormax))<2) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pnm(): WIDTH and HEIGHT fields undefined in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      if (ppm_type!=1 && ppm_type!=4) {
        if (err==2 || (err==3 && (ppm_type==5 || ppm_type==7 || ppm_type==8 || ppm_type==9))) {
          while ((err=std::fscanf(nfile," %16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
          if (cimg_sscanf(item,"%u",&colormax)!=1)
            cimg::warn(_cimg_instance
                       "load_pnm(): COLORMAX field is undefined in file '%s'.",
                       cimg_instance,
                       filename?filename:"(FILE*)");
        } else { colormax = D; D = 1; }
      }
      std::fgetc(nfile);

      if (filename) { // Check that dimensions specified in file does not exceed the buffer dimension
        const cimg_int64 siz = cimg::fsize(filename);
        if (W*H*D>siz)
          throw CImgIOException(_cimg_instance
                                "load_pnm(): Specified image dimensions in file '%s' exceed file size.",
                                cimg_instance,
                                filename);
      }

      switch (ppm_type) {
      case 1 : { // 2D B&W ascii
        assign(W,H,1,1);
        T* ptrd = _data;
        cimg_foroff(*this,off) { if (std::fscanf(nfile,"%d",&rval)>0) *(ptrd++) = (T)(rval?0:255); else break; }
      } break;
      case 2 : { // 2D grey ascii
        assign(W,H,1,1);
        T* ptrd = _data;
        cimg_foroff(*this,off) { if (std::fscanf(nfile,"%d",&rval)>0) *(ptrd++) = (T)rval; else break; }
      } break;
      case 3 : { // 2D color ascii
        assign(W,H,1,3);
        T *ptrd = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2);
        cimg_forXY(*this,x,y) {
          if (std::fscanf(nfile,"%d %d %d",&rval,&gval,&bval)==3) {
            *(ptrd++) = (T)rval; *(ptr_g++) = (T)gval; *(ptr_b++) = (T)bval;
          } else break;
        }
      } break;
      case 4 : { // 2D b&w binary (support 3D PINK extension)
        CImg<ucharT> raw;
        assign(W,H,D,1);
        T *ptrd = data(0,0,0,0);
        unsigned int w = 0, h = 0, d = 0;
        for (longT to_read = (longT)((W/8 + (W%8?1:0))*H*D); to_read>0; ) {
          raw.assign(std::min(to_read,cimg_iobuffer));
          cimg::fread(raw._data,raw._width,nfile);
          to_read-=raw._width;
          const unsigned char *ptrs = raw._data;
          unsigned char mask = 0, val = 0;
          for (ulongT off = (ulongT)raw._width; off || mask; mask>>=1) {
            if (!mask) { if (off--) val = *(ptrs++); mask = 128; }
            *(ptrd++) = (T)((val&mask)?0:255);
            if (++w==W) { w = 0; mask = 0; if (++h==H) { h = 0; if (++d==D) break; }}
          }
        }
      } break;
      case 5 : case 7 : { // 2D/3D grey binary (support 3D PINK extension)
        if (colormax<256) { // 8 bits
          CImg<ucharT> raw;
          assign(W,H,D,1);
          T *ptrd = data(0,0,0,0);
          for (longT to_read = (longT)size(); to_read>0; ) {
            raw.assign(std::min(to_read,cimg_iobuffer));
            cimg::fread(raw._data,raw._width,nfile);
            to_read-=raw._width;
            const unsigned char *ptrs = raw._data;
            for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++);
          }
        } else { // 16 bits
          CImg<ushortT> raw;
          assign(W,H,D,1);
          T *ptrd = data(0,0,0,0);
          for (longT to_read = (longT)size(); to_read>0; ) {
            raw.assign(std::min(to_read,cimg_iobuffer/2));
            cimg::fread(raw._data,raw._width,nfile);
            if (!cimg::endianness()) cimg::invert_endianness(raw._data,raw._width);
            to_read-=raw._width;
            const unsigned short *ptrs = raw._data;
            for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++);
          }
        }
      } break;
      case 6 : { // 2D color binary
        if (colormax<256) { // 8 bits
          CImg<ucharT> raw;
          assign(W,H,1,3);
          T
            *ptr_r = data(0,0,0,0),
            *ptr_g = data(0,0,0,1),
            *ptr_b = data(0,0,0,2);
          for (longT to_read = (longT)size(); to_read>0; ) {
            raw.assign(std::min(to_read,cimg_iobuffer));
            cimg::fread(raw._data,raw._width,nfile);
            to_read-=raw._width;
            const unsigned char *ptrs = raw._data;
            for (ulongT off = (ulongT)raw._width/3; off; --off) {
              *(ptr_r++) = (T)*(ptrs++);
              *(ptr_g++) = (T)*(ptrs++);
              *(ptr_b++) = (T)*(ptrs++);
            }
          }
        } else { // 16 bits
          CImg<ushortT> raw;
          assign(W,H,1,3);
          T
            *ptr_r = data(0,0,0,0),
            *ptr_g = data(0,0,0,1),
            *ptr_b = data(0,0,0,2);
          for (longT to_read = (longT)size(); to_read>0; ) {
            raw.assign(std::min(to_read,cimg_iobuffer/2));
            cimg::fread(raw._data,raw._width,nfile);
            if (!cimg::endianness()) cimg::invert_endianness(raw._data,raw._width);
            to_read-=raw._width;
            const unsigned short *ptrs = raw._data;
            for (ulongT off = (ulongT)raw._width/3; off; --off) {
              *(ptr_r++) = (T)*(ptrs++);
              *(ptr_g++) = (T)*(ptrs++);
              *(ptr_b++) = (T)*(ptrs++);
            }
          }
        }
      } break;
      case 8 : { // 2D/3D grey binary with int32 integers (PINK extension)
        CImg<intT> raw;
        assign(W,H,D,1);
        T *ptrd = data(0,0,0,0);
        for (longT to_read = (longT)size(); to_read>0; ) {
          raw.assign(std::min(to_read,cimg_iobuffer));
          cimg::fread(raw._data,raw._width,nfile);
          to_read-=raw._width;
          const int *ptrs = raw._data;
          for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++);
        }
      } break;
      case 9 : { // 2D/3D grey binary with float values (PINK extension)
        CImg<floatT> raw;
        assign(W,H,D,1);
        T *ptrd = data(0,0,0,0);
        for (longT to_read = (longT)size(); to_read>0; ) {
          raw.assign(std::min(to_read,cimg_iobuffer));
          cimg::fread(raw._data,raw._width,nfile);
          to_read-=raw._width;
          const float *ptrs = raw._data;
          for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++);
        }
      } break;
      default :
        assign();
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pnm(): PNM type 'P%d' found, but type is not supported.",
                              cimg_instance,
                              filename?filename:"(FILE*)",ppm_type);
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load image from a PFM file.
    /**
      \param filename Filename, as a C-string.
    **/
    CImg<T>& load_pfm(const char *const filename) {
      return _load_pfm(0,filename);
    }

    //! Load image from a PFM file \newinstance.
    static CImg<T> get_load_pfm(const char *const filename) {
      return CImg<T>().load_pfm(filename);
    }

    //! Load image from a PFM file \overloading.
    CImg<T>& load_pfm(std::FILE *const file) {
      return _load_pfm(file,0);
    }

    //! Load image from a PFM file \newinstance.
    static CImg<T> get_load_pfm(std::FILE *const file) {
      return CImg<T>().load_pfm(file);
    }

    CImg<T>& _load_pfm(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_pfm(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      char pfm_type;
      CImg<charT> item(16384,1,1,1,0);
      int W = 0, H = 0, err = 0;
      double scale = 0;
      while ((err=std::fscanf(nfile,"%16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
      if (cimg_sscanf(item," P%c",&pfm_type)!=1) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pfm(): PFM header not found in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      while ((err=std::fscanf(nfile," %16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
      if ((err=cimg_sscanf(item," %d %d",&W,&H))<2) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pfm(): WIDTH and HEIGHT fields are undefined in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      } else if (W<=0 || H<=0) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pfm(): WIDTH (%d) and HEIGHT (%d) fields are invalid in file '%s'.",
                              cimg_instance,W,H,
                              filename?filename:"(FILE*)");
      }
      if (err==2) {
        while ((err=std::fscanf(nfile," %16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
        if (cimg_sscanf(item,"%lf",&scale)!=1)
          cimg::warn(_cimg_instance
                     "load_pfm(): SCALE field is undefined in file '%s'.",
                     cimg_instance,
                     filename?filename:"(FILE*)");
      }
      std::fgetc(nfile);
      const bool is_color = (pfm_type=='F'), is_inverted = (scale>0)!=cimg::endianness();
      if (is_color) {
        assign(W,H,1,3,(T)0);
        CImg<floatT> buf(3*W);
        T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2);
        cimg_forY(*this,y) {
          cimg::fread(buf._data,3*W,nfile);
          if (is_inverted) cimg::invert_endianness(buf._data,3*W);
          const float *ptrs = buf._data;
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            *(ptr_g++) = (T)*(ptrs++);
            *(ptr_b++) = (T)*(ptrs++);
          }
        }
      } else {
        assign(W,H,1,1,(T)0);
        CImg<floatT> buf(W);
        T *ptrd = data(0,0,0,0);
        cimg_forY(*this,y) {
          cimg::fread(buf._data,W,nfile);
          if (is_inverted) cimg::invert_endianness(buf._data,W);
          const float *ptrs = buf._data;
          cimg_forX(*this,x) *(ptrd++) = (T)*(ptrs++);
        }
      }
      if (!file) cimg::fclose(nfile);
      return mirror('y'); // Most of the .pfm files are flipped along the y-axis
    }

    //! Load image from a RGB file.
    /**
      \param filename Filename, as a C-string.
      \param dimw Width of the image buffer.
      \param dimh Height of the image buffer.
    **/
    CImg<T>& load_rgb(const char *const filename, const unsigned int dimw, const unsigned int dimh=1) {
      return _load_rgb(0,filename,dimw,dimh);
    }

    //! Load image from a RGB file \newinstance.
    static CImg<T> get_load_rgb(const char *const filename, const unsigned int dimw, const unsigned int dimh=1) {
      return CImg<T>().load_rgb(filename,dimw,dimh);
    }

    //! Load image from a RGB file \overloading.
    CImg<T>& load_rgb(std::FILE *const file, const unsigned int dimw, const unsigned int dimh=1) {
      return _load_rgb(file,0,dimw,dimh);
    }

    //! Load image from a RGB file \newinstance.
    static CImg<T> get_load_rgb(std::FILE *const file, const unsigned int dimw, const unsigned int dimh=1) {
      return CImg<T>().load_rgb(file,dimw,dimh);
    }

    CImg<T>& _load_rgb(std::FILE *const file, const char *const filename,
                       const unsigned int dimw, const unsigned int dimh) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_rgb(): Specified filename is (null).",
                                    cimg_instance);

      if (!dimw || !dimh) return assign();
      const longT cimg_iobuffer = (longT)24*1024*1024;
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      CImg<ucharT> raw;
      assign(dimw,dimh,1,3);
      T
        *ptr_r = data(0,0,0,0),
        *ptr_g = data(0,0,0,1),
        *ptr_b = data(0,0,0,2);
      for (longT to_read = (longT)size(); to_read>0; ) {
        raw.assign(std::min(to_read,cimg_iobuffer));
        cimg::fread(raw._data,raw._width,nfile);
        to_read-=raw._width;
        const unsigned char *ptrs = raw._data;
        for (ulongT off = raw._width/3UL; off; --off) {
          *(ptr_r++) = (T)*(ptrs++);
          *(ptr_g++) = (T)*(ptrs++);
          *(ptr_b++) = (T)*(ptrs++);
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load image from a RGBA file.
    /**
       \param filename Filename, as a C-string.
       \param dimw Width of the image buffer.
       \param dimh Height of the image buffer.
    **/
    CImg<T>& load_rgba(const char *const filename, const unsigned int dimw, const unsigned int dimh=1) {
      return _load_rgba(0,filename,dimw,dimh);
    }

    //! Load image from a RGBA file \newinstance.
    static CImg<T> get_load_rgba(const char *const filename, const unsigned int dimw, const unsigned int dimh=1) {
      return CImg<T>().load_rgba(filename,dimw,dimh);
    }

    //! Load image from a RGBA file \overloading.
    CImg<T>& load_rgba(std::FILE *const file, const unsigned int dimw, const unsigned int dimh=1) {
      return _load_rgba(file,0,dimw,dimh);
    }

    //! Load image from a RGBA file \newinstance.
    static CImg<T> get_load_rgba(std::FILE *const file, const unsigned int dimw, const unsigned int dimh=1) {
      return CImg<T>().load_rgba(file,dimw,dimh);
    }

    CImg<T>& _load_rgba(std::FILE *const file, const char *const filename,
                        const unsigned int dimw, const unsigned int dimh) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_rgba(): Specified filename is (null).",
                                    cimg_instance);

      if (!dimw || !dimh) return assign();
      const longT cimg_iobuffer = (longT)24*1024*1024;
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      CImg<ucharT> raw;
      assign(dimw,dimh,1,4);
      T
        *ptr_r = data(0,0,0,0),
        *ptr_g = data(0,0,0,1),
        *ptr_b = data(0,0,0,2),
        *ptr_a = data(0,0,0,3);
      for (longT to_read = (longT)size(); to_read>0; ) {
        raw.assign(std::min(to_read,cimg_iobuffer));
        cimg::fread(raw._data,raw._width,nfile);
        to_read-=raw._width;
        const unsigned char *ptrs = raw._data;
        for (ulongT off = raw._width/4UL; off; --off) {
          *(ptr_r++) = (T)*(ptrs++);
          *(ptr_g++) = (T)*(ptrs++);
          *(ptr_b++) = (T)*(ptrs++);
          *(ptr_a++) = (T)*(ptrs++);
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

#endif
