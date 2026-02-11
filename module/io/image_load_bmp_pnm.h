#ifndef CIMG_MODULE_IMAGE_LOAD_BMP_PNM_H
#define CIMG_MODULE_IMAGE_LOAD_BMP_PNM_H

    //! Load image from a BMP file.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_bmp(const char *const filename) {
      return _load_bmp(0,filename);
    }

    //! Load image from a BMP file \newinstance.
    static CImg<T> get_load_bmp(const char *const filename) {
      return CImg<T>().load_bmp(filename);
    }

    //! Load image from a BMP file \overloading.
    CImg<T>& load_bmp(std::FILE *const file) {
      return _load_bmp(file,0);
    }

    //! Load image from a BMP file \newinstance.
    static CImg<T> get_load_bmp(std::FILE *const file) {
      return CImg<T>().load_bmp(file);
    }

    CImg<T>& _load_bmp(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_bmp(): Specified filename is (null).",
                                    cimg_instance);

      const ulongT fsiz = (ulongT)(file?cimg::fsize(file):cimg::fsize(filename));
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      CImg<ucharT> header(54);
      cimg::fread(header._data,54,nfile);
      if (*header!='B' || header[1]!='M') {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_bmp(): Invalid BMP file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }

      // Read header and pixel buffer.
      int
        file_size = header[0x02] + (header[0x03]<<8) + (header[0x04]<<16) + (header[0x05]<<24),
        offset = header[0x0A] + (header[0x0B]<<8) + (header[0x0C]<<16) + (header[0x0D]<<24),
        header_size = header[0x0E] + (header[0x0F]<<8) + (header[0x10]<<16) + (header[0x11]<<24),
        dx = header[0x12] + (header[0x13]<<8) + (header[0x14]<<16) + (header[0x15]<<24),
        dy = header[0x16] + (header[0x17]<<8) + (header[0x18]<<16) + (header[0x19]<<24),
        compression = header[0x1E] + (header[0x1F]<<8) + (header[0x20]<<16) + (header[0x21]<<24),
        nb_colors = header[0x2E] + (header[0x2F]<<8) + (header[0x30]<<16) + (header[0x31]<<24),
        bpp = header[0x1C] + (header[0x1D]<<8);

      if ((ulongT)file_size!=fsiz)
        throw CImgIOException(_cimg_instance
                              "load_bmp(): Invalid file_size %d specified in filename '%s' (expected %lu).",
                              cimg_instance,
                              file_size,filename?filename:"(FILE*)",fsiz);

      if (header_size<0 || header_size>=file_size)
        throw CImgIOException(_cimg_instance
                              "load_bmp(): Invalid header size %d specified in filename '%s'.",
                              cimg_instance,
                              header_size,filename?filename:"(FILE*)");

      if (offset<0 || offset>=file_size)
        throw CImgIOException(_cimg_instance
                              "load_bmp(): Invalid offset %d specified in filename '%s'.",
                              cimg_instance,
                              offset,filename?filename:"(FILE*)");

      if (header_size>40) cimg::fseek(nfile,header_size - 40,SEEK_CUR);
      const int
        dx_bytes = (bpp==1)?(dx/8 + (dx%8?1:0)):((bpp==4)?(dx/2 + (dx%2)):(int)((longT)dx*bpp/8)),
        align_bytes = (4 - dx_bytes%4)%4;
      const ulongT
        cimg_iobuffer = (ulongT)24*1024*1024,
        buf_size = (ulongT)cimg::abs(dy)*(dx_bytes + align_bytes);

      if (buf_size>=fsiz)
        throw CImgIOException(_cimg_instance
                              "load_bmp(): File size %lu for filename '%s' does not match "
                              "encoded image dimensions (%d,%d).",
                              cimg_instance,
                              (long)fsiz,filename?filename:"(FILE*)",dx,dy);

      CImg<intT> colormap;
      if (bpp<16) { if (!nb_colors) nb_colors = 1<<bpp; } else nb_colors = 0;
      if (nb_colors) { colormap.assign(nb_colors); cimg::fread(colormap._data,nb_colors,nfile); }

      const int xoffset = offset - 14 - header_size - 4*nb_colors;
      if (xoffset<0 || xoffset>=file_size)
        throw CImgIOException(_cimg_instance
                              "load_bmp(): Malformed header in filename '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      cimg::fseek(nfile,xoffset,SEEK_CUR);

      CImg<ucharT> buffer;
      if (buf_size<cimg_iobuffer) {
        buffer.assign(buf_size,1,1,1,0);
        cimg::fread(buffer._data,buf_size,nfile);
      } else buffer.assign(dx_bytes + align_bytes);
      unsigned char *ptrs = buffer;

      // Decompress buffer (if necessary).
      if (compression==1 || compression==2) {
        if (file)
          throw CImgIOException(_cimg_instance
                                "load_bmp(): Unable to load compressed data from '(*FILE)' inputs.",
                                cimg_instance);
        else {
          if (!file) cimg::fclose(nfile);
          return load_other(filename);
        }
      }

      // Read pixel data.
      assign(dx,cimg::abs(dy),1,3,0);
      switch (bpp) {
      case 1 : { // Monochrome
        if (colormap._width>=2) for (int y = height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            if (!cimg::fread(ptrs=buffer._data,dx_bytes,nfile)) break;
            cimg::fseek(nfile,align_bytes,SEEK_CUR);
          }
          unsigned char mask = 0x80, val = 0;
          cimg_forX(*this,x) {
            if (mask==0x80) val = *(ptrs++);
            const unsigned char *col = (unsigned char*)(colormap._data + (val&mask?1:0));
            (*this)(x,y,2) = (T)*(col++);
            (*this)(x,y,1) = (T)*(col++);
            (*this)(x,y,0) = (T)*(col++);
            mask = cimg::ror(mask);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 4 : { // 16 colors
        if (colormap._width>=16) for (int y = height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            if (!cimg::fread(ptrs=buffer._data,dx_bytes,nfile)) break;
            cimg::fseek(nfile,align_bytes,SEEK_CUR);
          }
          unsigned char mask = 0xF0, val = 0;
          cimg_forX(*this,x) {
            if (mask==0xF0) val = *(ptrs++);
            const unsigned char color = (unsigned char)((mask<16)?(val&mask):((val&mask)>>4));
            const unsigned char *col = (unsigned char*)(colormap._data + color);
            (*this)(x,y,2) = (T)*(col++);
            (*this)(x,y,1) = (T)*(col++);
            (*this)(x,y,0) = (T)*(col++);
            mask = cimg::ror(mask,4);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 8 : { // 256 colors
        if (colormap._width>=256) for (int y = height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            if (!cimg::fread(ptrs=buffer._data,dx_bytes,nfile)) break;
            cimg::fseek(nfile,align_bytes,SEEK_CUR);
          }
          cimg_forX(*this,x) {
            const unsigned char *col = (unsigned char*)(colormap._data + *(ptrs++));
            (*this)(x,y,2) = (T)*(col++);
            (*this)(x,y,1) = (T)*(col++);
            (*this)(x,y,0) = (T)*(col++);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 16 : { // 16 bits colors (RGB565)
        for (int y = height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            if (!cimg::fread(ptrs=buffer._data,dx_bytes,nfile)) break;
            cimg::fseek(nfile,align_bytes,SEEK_CUR);
          }
          cimg_forX(*this,x) {
            const unsigned char c1 = *(ptrs++), c2 = *(ptrs++);
            const unsigned short col = (unsigned short)c2<<8 | c1;
            (*this)(x,y,2) = (T)((col&0x1F)<<3);
            (*this)(x,y,1) = (T)(((col>>5)&0x3F)<<3);
            (*this)(x,y,0) = (T)(((col>>11)&0x1F)<<3);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 24 : { // 24 bits colors
        for (int y = height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            if (!cimg::fread(ptrs=buffer._data,dx_bytes,nfile)) break;
            cimg::fseek(nfile,align_bytes,SEEK_CUR);
          }
          cimg_forX(*this,x) {
            (*this)(x,y,2) = (T)*(ptrs++);
            (*this)(x,y,1) = (T)*(ptrs++);
            (*this)(x,y,0) = (T)*(ptrs++);
          }
          ptrs+=align_bytes;
        }
      } break;
      case 32 : { // 32 bits colors
        for (int y = height() - 1; y>=0; --y) {
          if (buf_size>=cimg_iobuffer) {
            if (!cimg::fread(ptrs=buffer._data,dx_bytes,nfile)) break;
            cimg::fseek(nfile,align_bytes,SEEK_CUR);
          }
          cimg_forX(*this,x) {
            (*this)(x,y,2) = (T)*(ptrs++);
            (*this)(x,y,1) = (T)*(ptrs++);
            (*this)(x,y,0) = (T)*(ptrs++);
            ++ptrs;
          }
          ptrs+=align_bytes;
        }
      } break;
      }
      if (dy<0) mirror('y');
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load image from a JPEG file.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_jpeg(const char *const filename) {
      return _load_jpeg(0,filename);
    }

    //! Load image from a JPEG file \newinstance.
    static CImg<T> get_load_jpeg(const char *const filename) {
      return CImg<T>().load_jpeg(filename);
    }

    //! Load image from a JPEG file \overloading.
    CImg<T>& load_jpeg(std::FILE *const file) {
      return _load_jpeg(file,0);
    }

    //! Load image from a JPEG file \newinstance.
    static CImg<T> get_load_jpeg(std::FILE *const file) {
      return CImg<T>().load_jpeg(file);
    }

    // Custom error handler for libjpeg.
#ifdef cimg_use_jpeg
    struct _cimg_error_mgr {
      struct jpeg_error_mgr original;
      jmp_buf setjmp_buffer;
      char message[JMSG_LENGTH_MAX];
    };

    typedef struct _cimg_error_mgr *_cimg_error_ptr;

    METHODDEF(void) _cimg_jpeg_error_exit(j_common_ptr cinfo) {
      _cimg_error_ptr c_err = (_cimg_error_ptr) cinfo->err; // Return control to the setjmp point
      (*cinfo->err->format_message)(cinfo,c_err->message);
      jpeg_destroy(cinfo); // Clean memory and temp files
      longjmp(c_err->setjmp_buffer,1);
    }
#endif

    CImg<T>& _load_jpeg(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_jpeg(): Specified filename is (null).",
                                    cimg_instance);

#ifndef cimg_use_jpeg
      if (file)
        throw CImgIOException(_cimg_instance
                              "load_jpeg(): Unable to load data from '(FILE*)' unless libjpeg is enabled.",
                              cimg_instance);
      else return load_other(filename);
#else

#if defined __GNUC__
      const char *volatile nfilename = filename; // Use 'volatile' to avoid (wrong) g++ warning
      std::FILE *volatile nfile = file?file:cimg::fopen(nfilename,"rb");
#else
      const char *nfilename = filename;
      std::FILE *nfile = file?file:cimg::fopen(nfilename,"rb");
#endif

      struct jpeg_decompress_struct cinfo;
      struct _cimg_error_mgr jerr;
      cinfo.err = jpeg_std_error(&jerr.original);
      jerr.original.error_exit = _cimg_jpeg_error_exit;
      if (setjmp(jerr.setjmp_buffer)) { // JPEG error
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                             "load_jpeg(): Error message returned by libjpeg: %s.",
                             cimg_instance,jerr.message);
      }

      jpeg_create_decompress(&cinfo);
      jpeg_stdio_src(&cinfo,nfile);
      jpeg_read_header(&cinfo,TRUE);
      jpeg_start_decompress(&cinfo);

      if (cinfo.output_components!=1 && cinfo.output_components!=3 && cinfo.output_components!=4) {
        if (!file) {
          cimg::fclose(nfile);
          return load_other(nfilename);
        } else
          throw CImgIOException(_cimg_instance
                                "load_jpeg(): Failed to load JPEG data from file '%s'.",
                                cimg_instance,nfilename?nfilename:"(FILE*)");
      }
      CImg<ucharT> buffer(cinfo.output_width*cinfo.output_components);
      JSAMPROW row_pointer[1];
      try { assign(cinfo.output_width,cinfo.output_height,1,cinfo.output_components); }
      catch (...) { if (!file) cimg::fclose(nfile); throw; }
      T *ptr_r = _data, *ptr_g = _data + 1UL*_width*_height, *ptr_b = _data + 2UL*_width*_height,
        *ptr_a = _data + 3UL*_width*_height;
      while (cinfo.output_scanline<cinfo.output_height) {
        *row_pointer = buffer._data;
        if (jpeg_read_scanlines(&cinfo,row_pointer,1)!=1) {
          cimg::warn(_cimg_instance
                     "load_jpeg(): Incomplete data in file '%s'.",
                     cimg_instance,nfilename?nfilename:"(FILE*)");
          break;
        }
        const unsigned char *ptrs = buffer._data;
        switch (_spectrum) {
        case 1 : {
          cimg_forX(*this,x) *(ptr_r++) = (T)*(ptrs++);
        } break;
        case 3 : {
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            *(ptr_g++) = (T)*(ptrs++);
            *(ptr_b++) = (T)*(ptrs++);
          }
        } break;
        case 4 : {
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            *(ptr_g++) = (T)*(ptrs++);
            *(ptr_b++) = (T)*(ptrs++);
            *(ptr_a++) = (T)*(ptrs++);
          }
        } break;
        }
      }
      jpeg_finish_decompress(&cinfo);
      jpeg_destroy_decompress(&cinfo);
      if (!file) cimg::fclose(nfile);
      return *this;
#endif
    }

    //! Load image from a JPEG XL file.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_jxl(const char *const filename) {
      return _load_jxl(0,filename);
    }

    //! Load image from a JPEG XL file \newinstance.
    static CImg<T> get_load_jxl(const char *const filename) {
      return CImg<T>().load_jxl(filename);
    }

    //! Load image from a JPEG XL file \overloading.
    CImg<T>& load_jxl(std::FILE *const file) {
      return _load_jxl(file,0);
    }

    //! Load image from a JPEG XL file \newinstance.
    static CImg<T> get_load_jxl(std::FILE *const file) {
      return CImg<T>().load_jxl(file);
    }

    CImg<T>& _load_jxl(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_jxl(): Specified filename is (null).",
                                    cimg_instance);

#ifndef cimg_use_jxl
      if (file)
        throw CImgIOException(_cimg_instance
                              "load_jxl(): Unable to load data from '(FILE*)' unless libjxl is enabled.",
                              cimg_instance);
      else return load_other(filename);
#else
      const char *nfilename = filename;
      std::FILE *nfile = file?file:cimg::fopen(nfilename,"rb");
      const long dataSize = cimg::fsize(nfile);
      if (dataSize<=0) {
        cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_jxl(): Failed to get file size '%s'.",
                              cimg_instance,
                              nfilename);
      }
      CImg<ucharT> buffer(dataSize);
      cimg::fread(buffer._data,buffer._width,nfile);
      cimg::fclose(nfile);

      bool hasAlpha = false, isGray = false;
      uint32_t nChannels = 0;
      JxlBasicInfo jxlInfo;
      JxlPixelFormat format = { 1,JXL_TYPE_UINT8,cimg::endianness()?JXL_BIG_ENDIAN:JXL_LITTLE_ENDIAN,0 };
      CImg<ucharT> imgData;
      JxlDecoder *decoder = JxlDecoderCreate(NULL);
      if (JXL_DEC_SUCCESS!=JxlDecoderSubscribeEvents(decoder,JXL_DEC_BASIC_INFO | JXL_DEC_FULL_IMAGE)) {
        JxlDecoderDestroy(decoder);
        throw CImgIOException(_cimg_instance
                              "load_jxl(): Failed to configure decoder '%s'.",
                              cimg_instance,
                              nfilename);
      }
      if (JXL_DEC_SUCCESS!=JxlDecoderSetInput(decoder,buffer._data,buffer._width)) {
        JxlDecoderDestroy(decoder);
        throw CImgIOException(_cimg_instance
                              "load_jxl(): Failed to load image data '%s'.",
                              cimg_instance,
                              nfilename);
      }
      JxlDecoderCloseInput(decoder);

      while (true) {
        JxlDecoderStatus status = JxlDecoderProcessInput(decoder);
        if (status==JXL_DEC_SUCCESS || status==JXL_DEC_FULL_IMAGE) break;
        else if (status==JXL_DEC_ERROR || status==JXL_DEC_NEED_MORE_INPUT) {
          JxlDecoderDestroy(decoder);
          throw CImgIOException(_cimg_instance
                                "load_jxl(): Failed to decode image '%s'.",
                                cimg_instance,
                                nfilename);
        } else if (status==JXL_DEC_BASIC_INFO) {
          if (JXL_DEC_SUCCESS!=JxlDecoderGetBasicInfo(decoder,&jxlInfo)) {
            JxlDecoderDestroy(decoder);
            throw CImgIOException(_cimg_instance
                                  "load_jxl(): Failed to load image data '%s'.",
                                  cimg_instance,
                                  nfilename);
          }
          if (jxlInfo.have_animation!=0) {
            JxlDecoderDestroy(decoder);
            throw CImgIOException(_cimg_instance
                                  "load_jxl(): Does not support animated JPEG XL '%s'.",
                                  cimg_instance,
                                  nfilename);
          }
          hasAlpha = jxlInfo.alpha_bits!=0;
          nChannels = hasAlpha?jxlInfo.num_color_channels + 1:jxlInfo.num_color_channels;
          isGray = jxlInfo.num_color_channels==1;
        } else if (status==JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
          std::size_t imgDataSize = 0;
          format.num_channels = nChannels;
          format.data_type = jxlInfo.bits_per_sample==16?JXL_TYPE_UINT16:JXL_TYPE_UINT8;
          if (JXL_DEC_SUCCESS!=JxlDecoderImageOutBufferSize(decoder,&format,&imgDataSize)) {
            JxlDecoderDestroy(decoder);
            throw CImgIOException(_cimg_instance
                                  "load_jxl(): Failed to decode image data '%s'.",
                                  cimg_instance,
                                  nfilename);
          }
          imgData.assign(imgDataSize);
          if (JXL_DEC_SUCCESS!=JxlDecoderSetImageOutBuffer(decoder,&format,(void*)imgData.data(),imgDataSize)) {
            JxlDecoderDestroy(decoder);
            throw CImgIOException(_cimg_instance
                                  "load_jxl(): Failed to set decode buffer '%s'.",
                                  cimg_instance,
                                  nfilename);
          }
        }
      }

      assign(jxlInfo.xsize,jxlInfo.ysize,1,nChannels);
      T
        *ptr_r = data(0,0,0,0),
        *ptr_g = isGray?0:data(0,0,0,1),
        *ptr_b = isGray?0:data(0,0,0,2),
        *ptr_a = !hasAlpha?0:data(0,0,0,isGray?1:3);
      switch (jxlInfo.bits_per_sample) {
      case 8: {
        cimg_forY(*this,y) {
          const uint8_t *ptrs = (uint8_t*)&imgData[y*_width*_spectrum*sizeof(uint8_t)];
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            if (ptr_g) *(ptr_g++) = (T)*(ptrs++);
            if (ptr_b) *(ptr_b++) = (T)*(ptrs++);
            if (ptr_a) *(ptr_a++) = (T)*(ptrs++);
          }
        }
      } break;
      case 16: {
        cimg_forY(*this,y) {
          const uint16_t *ptrs = (uint16_t*)&imgData[y*_width*_spectrum*sizeof(uint16_t)];
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            if (ptr_g) *(ptr_g++) = (T)*(ptrs++);
            if (ptr_b) *(ptr_b++) = (T)*(ptrs++);
            if (ptr_a) *(ptr_a++) = (T)*(ptrs++);
          }
        }
      } break;
      }
      JxlDecoderDestroy(decoder);
      return *this;
#endif
    }

    //! Load image from a file, using Magick++ library.
    /**
       \param filename Filename, as a C-string.
    **/
    // Added April/may 2006 by Christoph Hormann <chris_hormann@gmx.de>.
    // This is experimental code, not much tested, use with care.
    CImg<T>& load_magick(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_magick(): Specified filename is (null).",
                                    cimg_instance);

#ifdef cimg_use_magick
      Magick::Image image(filename);
      const unsigned int W = image.size().width(), H = image.size().height();
      switch (image.type()) {
      case Magick::PaletteMatteType :
      case Magick::TrueColorMatteType :
      case Magick::ColorSeparationType : {
        assign(W,H,1,4);
        T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2), *ptr_a = data(0,0,0,3);
        Magick::PixelPacket *pixels = image.getPixels(0,0,W,H);
        for (ulongT off = (ulongT)W*H; off; --off) {
          *(ptr_r++) = (T)(pixels->red);
          *(ptr_g++) = (T)(pixels->green);
          *(ptr_b++) = (T)(pixels->blue);
          *(ptr_a++) = (T)(pixels->opacity);
          ++pixels;
        }
      } break;
      case Magick::PaletteType :
      case Magick::TrueColorType : {
        assign(W,H,1,3);
        T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2);
        Magick::PixelPacket *pixels = image.getPixels(0,0,W,H);
        for (ulongT off = (ulongT)W*H; off; --off) {
          *(ptr_r++) = (T)(pixels->red);
          *(ptr_g++) = (T)(pixels->green);
          *(ptr_b++) = (T)(pixels->blue);
          ++pixels;
        }
      } break;
      case Magick::GrayscaleMatteType : {
        assign(W,H,1,2);
        T *ptr_r = data(0,0,0,0), *ptr_a = data(0,0,0,1);
        Magick::PixelPacket *pixels = image.getPixels(0,0,W,H);
        for (ulongT off = (ulongT)W*H; off; --off) {
          *(ptr_r++) = (T)(pixels->red);
          *(ptr_a++) = (T)(pixels->opacity);
          ++pixels;
        }
      } break;
      default : {
        assign(W,H,1,1);
        T *ptr_r = data(0,0,0,0);
        Magick::PixelPacket *pixels = image.getPixels(0,0,W,H);
        for (ulongT off = (ulongT)W*H; off; --off) {
          *(ptr_r++) = (T)(pixels->red);
          ++pixels;
        }
      }
      }
      return *this;
#else
      throw CImgIOException(_cimg_instance
                            "load_magick(): Unable to load file '%s' unless libMagick++ is enabled.",
                            cimg_instance,
                            filename);
#endif
    }

    //! Load image from a file, using Magick++ library \newinstance.
    static CImg<T> get_load_magick(const char *const filename) {
      return CImg<T>().load_magick(filename);
    }

#endif
