#ifndef CIMG_MODULE_IO_IMAGE_LOAD_H
#define CIMG_MODULE_IO_IMAGE_LOAD_H

    //! Load image from a file.
    /**
       \param filename Filename, as a C-string.
       \note The extension of \c filename defines the file format. If no filename
       extension is provided, CImg<T>::get_load() will try to load the file as a .cimg or .cimgz file.
    **/
    CImg<T>& load(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load(): Specified filename is (null).",
                                    cimg_instance);

      if (!cimg::strncasecmp(filename,"http://",7) || !cimg::strncasecmp(filename,"https://",8)) {
        CImg<charT> filename_local(256);
        load(cimg::load_network(filename,filename_local));
        std::remove(filename_local);
        return *this;
      }

      const char *const ext = cimg::split_filename(filename);
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      bool is_loaded = true;
      try {
#ifdef cimg_load_plugin
        cimg_load_plugin(filename);
#endif
#ifdef cimg_load_plugin1
        cimg_load_plugin1(filename);
#endif
#ifdef cimg_load_plugin2
        cimg_load_plugin2(filename);
#endif
#ifdef cimg_load_plugin3
        cimg_load_plugin3(filename);
#endif
#ifdef cimg_load_plugin4
        cimg_load_plugin4(filename);
#endif
#ifdef cimg_load_plugin5
        cimg_load_plugin5(filename);
#endif
#ifdef cimg_load_plugin6
        cimg_load_plugin6(filename);
#endif
#ifdef cimg_load_plugin7
        cimg_load_plugin7(filename);
#endif
#ifdef cimg_load_plugin8
        cimg_load_plugin8(filename);
#endif
        // Text formats.
        if (!cimg::strcasecmp(ext,"asc")) load_ascii(filename);
        else if (!cimg::strcasecmp(ext,"csv") ||
                 !cimg::strcasecmp(ext,"dlm") ||
                 !cimg::strcasecmp(ext,"txt")) load_dlm(filename);
        else if (!cimg::strcasecmp(ext,"pdf")) load_pdf_external(filename);

        // 2D binary formats.
        else if (!cimg::strcasecmp(ext,"bmp")) load_bmp(filename);
        else if (!cimg::strcasecmp(ext,"jpg") ||
                 !cimg::strcasecmp(ext,"jpeg") ||
                 !cimg::strcasecmp(ext,"jpe") ||
                 !cimg::strcasecmp(ext,"jfif") ||
                 !cimg::strcasecmp(ext,"jif")) load_jpeg(filename);
        else if (!cimg::strcasecmp(ext,"png")) load_png(filename);
        else if (!cimg::strcasecmp(ext,"ppm") ||
                 !cimg::strcasecmp(ext,"pgm") ||
                 !cimg::strcasecmp(ext,"pnm") ||
                 !cimg::strcasecmp(ext,"pbm") ||
                 !cimg::strcasecmp(ext,"pnk")) load_pnm(filename);
        else if (!cimg::strcasecmp(ext,"pfm")) load_pfm(filename);
        else if (!cimg::strcasecmp(ext,"tif") ||
                 !cimg::strcasecmp(ext,"tiff")) load_tiff(filename);
        else if (!cimg::strcasecmp(ext,"exr")) load_exr(filename);
        else if (!cimg::strcasecmp(ext,"arw") ||
                 !cimg::strcasecmp(ext,"cr2") ||
                 !cimg::strcasecmp(ext,"crw") ||
                 !cimg::strcasecmp(ext,"dcr") ||
                 !cimg::strcasecmp(ext,"dng") ||
                 !cimg::strcasecmp(ext,"mrw") ||
                 !cimg::strcasecmp(ext,"nef") ||
                 !cimg::strcasecmp(ext,"orf") ||
                 !cimg::strcasecmp(ext,"pix") ||
                 !cimg::strcasecmp(ext,"ptx") ||
                 !cimg::strcasecmp(ext,"raf") ||
                 !cimg::strcasecmp(ext,"srf")) load_dcraw_external(filename);
        else if (!cimg::strcasecmp(ext,"gif")) load_gif_external(filename);
        else if (!cimg::strcasecmp(ext,"heic") ||
                 !cimg::strcasecmp(ext,"heif") ||
                 !cimg::strcasecmp(ext,"avif")) load_heif(filename);
        else if (!cimg::strcasecmp(ext,"webp")) load_webp(filename);
        else if (!cimg::strcasecmp(ext,"jxl")) load_jxl(filename);

        // 3D binary formats.
        else if (!cimg::strcasecmp(ext,"dcm") ||
                 !cimg::strcasecmp(ext,"dicom")) load_medcon_external(filename);
        else if (!cimg::strcasecmp(ext,"hdr") ||
                 !cimg::strcasecmp(ext,"nii")) load_analyze(filename);
        else if (!cimg::strcasecmp(ext,"par") ||
                 !cimg::strcasecmp(ext,"rec")) load_parrec(filename);
        else if (!cimg::strcasecmp(ext,"mnc")) load_minc2(filename);
        else if (!cimg::strcasecmp(ext,"inr")) load_inr(filename);
        else if (!cimg::strcasecmp(ext,"pan")) load_pandore(filename);
        else if (!cimg::strcasecmp(ext,"cimg") ||
                 !cimg::strcasecmp(ext,"cimgz") ||
                 !*ext)  return load_cimg(filename);

        // Archive files.
        else if (!cimg::strcasecmp(ext,"gz")) load_gzip_external(filename);

        // Image sequences.
        else if (!cimg::strcasecmp(ext,"avi") ||
                 !cimg::strcasecmp(ext,"mov") ||
                 !cimg::strcasecmp(ext,"asf") ||
                 !cimg::strcasecmp(ext,"divx") ||
                 !cimg::strcasecmp(ext,"flv") ||
                 !cimg::strcasecmp(ext,"mpg") ||
                 !cimg::strcasecmp(ext,"m1v") ||
                 !cimg::strcasecmp(ext,"m2v") ||
                 !cimg::strcasecmp(ext,"m4v") ||
                 !cimg::strcasecmp(ext,"mjp") ||
                 !cimg::strcasecmp(ext,"mp4") ||
                 !cimg::strcasecmp(ext,"mkv") ||
                 !cimg::strcasecmp(ext,"mpe") ||
                 !cimg::strcasecmp(ext,"movie") ||
                 !cimg::strcasecmp(ext,"ogm") ||
                 !cimg::strcasecmp(ext,"ogg") ||
                 !cimg::strcasecmp(ext,"ogv") ||
                 !cimg::strcasecmp(ext,"qt") ||
                 !cimg::strcasecmp(ext,"rm") ||
                 !cimg::strcasecmp(ext,"vob") ||
                 !cimg::strcasecmp(ext,"webm") ||
                 !cimg::strcasecmp(ext,"wmv") ||
                 !cimg::strcasecmp(ext,"xvid") ||
                 !cimg::strcasecmp(ext,"mpeg")) load_video(filename);
        else is_loaded = false;
      } catch (CImgIOException&) { is_loaded = false; }

      // If nothing loaded, try to guess file format from magic number in file.
      if (!is_loaded) {
        std::FILE *file = cimg::std_fopen(filename,"rb");
        if (!file) {
          cimg::exception_mode(omode);
          throw CImgIOException(_cimg_instance
                                "load(): Failed to open file '%s'.",
                                cimg_instance,
                                filename);
        }

        const char *const f_type = cimg::ftype(file,filename);
        cimg::fclose(file);
        is_loaded = true;
        try {
          if (!cimg::strcasecmp(f_type,"pnm")) load_pnm(filename);
          else if (!cimg::strcasecmp(f_type,"pfm")) load_pfm(filename);
          else if (!cimg::strcasecmp(f_type,"bmp")) load_bmp(filename);
          else if (!cimg::strcasecmp(f_type,"inr")) load_inr(filename);
          else if (!cimg::strcasecmp(f_type,"jpg")) load_jpeg(filename);
          else if (!cimg::strcasecmp(f_type,"pan")) load_pandore(filename);
          else if (!cimg::strcasecmp(f_type,"png")) load_png(filename);
          else if (!cimg::strcasecmp(f_type,"tif")) load_tiff(filename);
          else if (!cimg::strcasecmp(f_type,"gif")) load_gif_external(filename);
          else if (!cimg::strcasecmp(f_type,"heif")) load_heif(filename);
          else if (!cimg::strcasecmp(f_type,"dcm")) load_medcon_external(filename);
          else if (!cimg::strcasecmp(f_type,"webp")) load_webp(filename);
          else if (!cimg::strcasecmp(f_type,"jxl")) load_jxl(filename);
          else is_loaded = false;
        } catch (CImgIOException&) { is_loaded = false; }
      }

      // If nothing loaded, try to load file with other means.
      if (!is_loaded) {
        try {
          load_other(filename);
        } catch (CImgIOException&) {
          cimg::exception_mode(omode);
          throw CImgIOException(_cimg_instance
                                "load(): Failed to recognize format of file '%s'.",
                                cimg_instance,
                                filename);
        }
      }
      cimg::exception_mode(omode);
      return *this;
    }

    //! Load image from a file \newinstance.
    static CImg<T> get_load(const char *const filename) {
      return CImg<T>().load(filename);
    }

    //! Load image from an ascii file.
    /**
       \param filename Filename, as a C -string.
    **/
    CImg<T>& load_ascii(const char *const filename) {
      return _load_ascii(0,filename);
    }

    //! Load image from an ascii file \inplace.
    static CImg<T> get_load_ascii(const char *const filename) {
      return CImg<T>().load_ascii(filename);
    }

    //! Load image from an ascii file \overloading.
    CImg<T>& load_ascii(std::FILE *const file) {
      return _load_ascii(file,0);
    }

    //! Loadimage from an ascii file \newinstance.
    static CImg<T> get_load_ascii(std::FILE *const file) {
      return CImg<T>().load_ascii(file);
    }

    CImg<T>& _load_ascii(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_ascii(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      CImg<charT> line(256); *line = 0;
      int err = std::fscanf(nfile,"%255[^\n]",line._data);
      unsigned int dx = 0, dy = 1, dz = 1, dc = 1;
      cimg_sscanf(line,"%u%*c%u%*c%u%*c%u",&dx,&dy,&dz,&dc);
      err = std::fscanf(nfile,"%*[^0-9.eEinfa+-]");
      if (!dx || !dy || !dz || !dc) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_ascii(): Invalid ascii header in file '%s', image dimensions are set "
                              "to (%u,%u,%u,%u).",
                              cimg_instance,
                              filename?filename:"(FILE*)",dx,dy,dz,dc);
      }
      assign(dx,dy,dz,dc);
      const ulongT siz = size();
      ulongT off = 0;
      double val;
      T *ptr = _data;
      for (err = 1, off = 0; off<siz && err==1; ++off) {
        err = std::fscanf(nfile,"%lf%*[^0-9.eEinfa+-]",&val);
        *(ptr++) = (T)val;
      }
      if (err!=1)
        cimg::warn(_cimg_instance
                   "load_ascii(): Only %lu/%lu values read from file '%s'.",
                   cimg_instance,
                   off - 1,siz,filename?filename:"(FILE*)");

      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load image from a DLM file.
    /**
      \param filename Filename, as a C-string.
    **/
    CImg<T>& load_dlm(const char *const filename) {
      return _load_dlm(0,filename);
    }

    //! Load image from a DLM file \newinstance.
    static CImg<T> get_load_dlm(const char *const filename) {
      return CImg<T>().load_dlm(filename);
    }

    //! Load image from a DLM file \overloading.
    CImg<T>& load_dlm(std::FILE *const file) {
      return _load_dlm(file,0);
    }

    //! Load image from a DLM file \newinstance.
    static CImg<T> get_load_dlm(std::FILE *const file) {
      return CImg<T>().load_dlm(file);
    }

    CImg<T>& _load_dlm(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_dlm(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"r");
      CImg<charT> delimiter(256), tmp(256); *delimiter = *tmp = 0;
      unsigned int cdx = 0, dx = 0, dy = 0;
      int err = 0;
      double val;
      assign(256,256,1,1,(T)0);
      while ((err = std::fscanf(nfile,"%lf%255[^0-9eEinfa.+-]",&val,delimiter._data))>0) {
        if (err>0) (*this)(cdx++,dy) = (T)val;
        if (cdx>=_width) resize(3*_width/2,_height,1,1,0);
        char c = 0;
        if (!cimg_sscanf(delimiter,"%255[^\n]%c",tmp._data,&c) || c=='\n') {
          dx = std::max(cdx,dx);
          if (++dy>=_height) resize(_width,3*_height/2,1,1,0);
          cdx = 0;
        }
      }
      if (cdx && err==1) { dx = cdx; ++dy; }
      if (!dx || !dy) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_dlm(): Invalid DLM file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      resize(dx,dy,1,1,0);
      if (!file) cimg::fclose(nfile);
      return *this;
    }

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

    //! Load image from a TIFF file.
    /**
       \param filename Filename, as a C-string.
       \param first_frame First frame to read (for multi-pages tiff).
       \param last_frame Last frame to read (for multi-pages tiff).
       \param step_frame Step value of frame reading.
       \param[out] bits_per_value Number of bits used to store a scalar value in the image file.
       \param[out] voxel_size Voxel size, as stored in the filename.
       \param[out] description Description, as stored in the filename.
       \note
       - libtiff support is enabled by defining the precompilation
        directive \c cimg_use_tiff.
       - When libtiff is enabled, 2D and 3D (multipage) several
        channel per pixel are supported for
        <tt>char,uchar,short,ushort,float</tt> and \c double pixel types.
       - If \c cimg_use_tiff is not defined at compile time the
        function uses CImg<T>& load_other(const char*).
     **/
    CImg<T>& load_tiff(const char *const filename,
                       const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                       const unsigned int step_frame=1, unsigned int *const bits_per_value=0,
                       float *const voxel_size=0, CImg<charT> *const description=0) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_tiff(): Specified filename is (null).",
                                    cimg_instance);

      const unsigned int
        nfirst_frame = first_frame<last_frame?first_frame:last_frame,
        nstep_frame = step_frame?step_frame:1;
      unsigned int nlast_frame = first_frame<last_frame?last_frame:first_frame;

#ifndef cimg_use_tiff
      cimg::unused(bits_per_value,voxel_size,description);
      if (nfirst_frame || nlast_frame!=~0U || nstep_frame>1)
        throw CImgArgumentException(_cimg_instance
                                    "load_tiff(): Unable to read sub-images from file '%s' unless libtiff is enabled.",
                                    cimg_instance,
                                    filename);
      return load_other(filename);
#else
#if cimg_verbosity<3
      TIFFSetWarningHandler(0);
      TIFFSetErrorHandler(0);
#endif
      TIFF *tif = TIFFOpen(filename,"r");
      if (tif) {
        unsigned int nb_images = 0;
        do ++nb_images; while (TIFFReadDirectory(tif));
        if (nfirst_frame>=nb_images || (nlast_frame!=~0U && nlast_frame>=nb_images))
          cimg::warn(_cimg_instance
                     "load_tiff(): File '%s' contains %u image(s) while specified frame range is [%u,%u] (step %u).",
                     cimg_instance,
                     filename,nb_images,nfirst_frame,nlast_frame,nstep_frame);

        if (nfirst_frame>=nb_images) return assign();
        if (nlast_frame>=nb_images) nlast_frame = nb_images - 1;
        TIFFSetDirectory(tif,0);
        CImg<T> frame;
        for (unsigned int l = nfirst_frame; l<=nlast_frame; l+=nstep_frame) {
          frame._load_tiff(tif,l,bits_per_value,voxel_size,description);
          if (l==nfirst_frame)
            assign(frame._width,frame._height,1 + (nlast_frame - nfirst_frame)/nstep_frame,frame._spectrum);
          if (frame._width>_width || frame._height>_height || frame._spectrum>_spectrum)
            resize(std::max(frame._width,_width),
                   std::max(frame._height,_height),-100,
                   std::max(frame._spectrum,_spectrum),0);
          draw_image(0,0,(l - nfirst_frame)/nstep_frame,frame);
        }
        TIFFClose(tif);
      } else throw CImgIOException(_cimg_instance
                                   "load_tiff(): Failed to open file '%s'.",
                                   cimg_instance,
                                   filename);
      return *this;
#endif
    }

    //! Load image from a TIFF file \newinstance.
    static CImg<T> get_load_tiff(const char *const filename,
                                 const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                                 const unsigned int step_frame=1, unsigned int *const bits_per_value=0,
                                 float *const voxel_size=0, CImg<charT> *const description=0) {
      return CImg<T>().load_tiff(filename,first_frame,last_frame,step_frame,bits_per_value,voxel_size,description);
    }

    // (Original contribution by Jerome Boulanger).
#ifdef cimg_use_tiff
    template<typename t>
    void _load_tiff_tiled_contig(TIFF *const tif, const cimg_uint16 samplesperpixel,
                                 const cimg_uint32 nx, const cimg_uint32 ny,
                                 const cimg_uint32 tw, const cimg_uint32 th) {
      t *const buf = (t*)_TIFFmalloc(TIFFTileSize(tif));
      if (buf) {
        for (unsigned int row = 0; row<ny; row+=th)
          for (unsigned int col = 0; col<nx; col+=tw) {
            if (TIFFReadTile(tif,buf,col,row,0,0)<0) {
              _TIFFfree(buf); TIFFClose(tif);
              throw CImgIOException(_cimg_instance
                                    "load_tiff(): Invalid tile in file '%s'.",
                                    cimg_instance,
                                    TIFFFileName(tif));
            }
            const t *ptr = buf;
            for (unsigned int rr = row; rr<std::min((unsigned int)(row + th),(unsigned int)ny); ++rr)
              for (unsigned int cc = col; cc<std::min((unsigned int)(col + tw),(unsigned int)nx); ++cc)
                for (unsigned int vv = 0; vv<samplesperpixel; ++vv)
                  (*this)(cc,rr,vv) = (T)(ptr[(rr - row)*th*samplesperpixel + (cc - col)*samplesperpixel + vv]);
          }
        _TIFFfree(buf);
      }
    }

    template<typename t>
    void _load_tiff_tiled_separate(TIFF *const tif, const cimg_uint16 samplesperpixel,
                                   const cimg_uint32 nx, const cimg_uint32 ny,
                                   const cimg_uint32 tw, const cimg_uint32 th) {
      t *const buf = (t*)_TIFFmalloc(TIFFTileSize(tif));
      if (buf) {
        for (unsigned int vv = 0; vv<samplesperpixel; ++vv)
          for (unsigned int row = 0; row<ny; row+=th)
            for (unsigned int col = 0; col<nx; col+=tw) {
              if (TIFFReadTile(tif,buf,col,row,0,vv)<0) {
                _TIFFfree(buf); TIFFClose(tif);
                throw CImgIOException(_cimg_instance
                                      "load_tiff(): Invalid tile in file '%s'.",
                                      cimg_instance,
                                      TIFFFileName(tif));
              }
              const t *ptr = buf;
              for (unsigned int rr = row; rr<std::min((unsigned int)(row + th),(unsigned int)ny); ++rr)
                for (unsigned int cc = col; cc<std::min((unsigned int)(col + tw),(unsigned int)nx); ++cc)
                  (*this)(cc,rr,vv) = (T)*(ptr++);
            }
        _TIFFfree(buf);
      }
    }

    template<typename t>
    void _load_tiff_contig(TIFF *const tif, const cimg_uint16 samplesperpixel,
                           const cimg_uint32 nx, const cimg_uint32 ny) {
      t *const buf = (t*)_TIFFmalloc(TIFFStripSize(tif));
      if (buf) {
        cimg_uint32 row, rowsperstrip = (cimg_uint32)-1;
        TIFFGetField(tif,TIFFTAG_ROWSPERSTRIP,&rowsperstrip);
        for (row = 0; row<ny; row+= rowsperstrip) {
          cimg_uint32 nrow = (row + rowsperstrip>ny?ny - row:rowsperstrip);
          tstrip_t strip = TIFFComputeStrip(tif, row, 0);
          if ((TIFFReadEncodedStrip(tif,strip,buf,-1))<0) {
            _TIFFfree(buf); TIFFClose(tif);
            throw CImgIOException(_cimg_instance
                                  "load_tiff(): Invalid strip in file '%s'.",
                                  cimg_instance,
                                  TIFFFileName(tif));
          }
          const t *ptr = buf;
          for (unsigned int rr = 0; rr<nrow; ++rr)
            for (unsigned int cc = 0; cc<nx; ++cc)
              for (unsigned int vv = 0; vv<samplesperpixel; ++vv) (*this)(cc,row + rr,vv) = (T)*(ptr++);
        }
        _TIFFfree(buf);
      }
    }

    template<typename t>
    void _load_tiff_separate(TIFF *const tif, const cimg_uint16 samplesperpixel,
                             const cimg_uint32 nx, const cimg_uint32 ny) {
      t *buf = (t*)_TIFFmalloc(TIFFStripSize(tif));
      if (buf) {
        cimg_uint32 row, rowsperstrip = (cimg_uint32)-1;
        TIFFGetField(tif,TIFFTAG_ROWSPERSTRIP,&rowsperstrip);
        for (unsigned int vv = 0; vv<samplesperpixel; ++vv)
          for (row = 0; row<ny; row+= rowsperstrip) {
            cimg_uint32 nrow = (row + rowsperstrip>ny?ny - row:rowsperstrip);
            tstrip_t strip = TIFFComputeStrip(tif, row, vv);
            if ((TIFFReadEncodedStrip(tif,strip,buf,-1))<0) {
              _TIFFfree(buf); TIFFClose(tif);
              throw CImgIOException(_cimg_instance
                                    "load_tiff(): Invalid strip in file '%s'.",
                                    cimg_instance,
                                    TIFFFileName(tif));
            }
            const t *ptr = buf;
            for (unsigned int rr = 0;rr<nrow; ++rr)
              for (unsigned int cc = 0; cc<nx; ++cc)
                (*this)(cc,row + rr,vv) = (T)*(ptr++);
          }
        _TIFFfree(buf);
      }
    }

    CImg<T>& _load_tiff(TIFF *const tif, const unsigned int directory, unsigned int *const bits_per_value,
                        float *const voxel_size, CImg<charT> *const description) {
      if (!TIFFSetDirectory(tif,directory)) return assign();
      cimg_uint16 samplesperpixel = 1, bitspersample = 8, photo = 0;
      cimg_uint16 sampleformat = 1;
      cimg_uint32 nx = 1, ny = 1;
      const char *const filename = TIFFFileName(tif);
      const bool is_spp = (bool)TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&samplesperpixel);
      TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
      TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
      TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleformat);
      TIFFGetFieldDefaulted(tif,TIFFTAG_BITSPERSAMPLE,&bitspersample);
      TIFFGetField(tif,TIFFTAG_PHOTOMETRIC,&photo);
      if (bits_per_value) *bits_per_value = (unsigned int)bitspersample;
      if (voxel_size) {
        const char *s_description = 0;
        float vx = 0, vy = 0, vz = 0;
        if (TIFFGetField(tif,TIFFTAG_IMAGEDESCRIPTION,&s_description) && s_description) {
          const char *s_desc = std::strstr(s_description,"VX=");
          if (s_desc && cimg_sscanf(s_desc,"VX=%f VY=%f VZ=%f",&vx,&vy,&vz)==3) { // CImg format
            voxel_size[0] = vx; voxel_size[1] = vy; voxel_size[2] = vz;
          }
          s_desc = std::strstr(s_description,"spacing=");
          if (s_desc && cimg_sscanf(s_desc,"spacing=%f",&vz)==1) { // Fiji format
            voxel_size[2] = vz;
          }
        }
        TIFFGetField(tif,TIFFTAG_XRESOLUTION,voxel_size);
        TIFFGetField(tif,TIFFTAG_YRESOLUTION,voxel_size + 1);
        voxel_size[0] = 1.f/voxel_size[0];
        voxel_size[1] = 1.f/voxel_size[1];
      }
      if (description) {
        const char *s_description = 0;
        if (TIFFGetField(tif,TIFFTAG_IMAGEDESCRIPTION,&s_description) && s_description)
          CImg<charT>::string(s_description).move_to(*description);
      }
      const unsigned int spectrum = !is_spp || photo>=3?(photo>1?3:1):samplesperpixel;
      assign(nx,ny,1,spectrum);

      if ((photo>=3 && sampleformat==1 &&
           (bitspersample==4 || bitspersample==8) &&
           (samplesperpixel==1 || samplesperpixel==3 || samplesperpixel==4)) ||
          (bitspersample==1 && samplesperpixel==1)) {
        // Special case for unsigned color images.
        cimg_uint32 *const raster = (cimg_uint32*)_TIFFmalloc(nx*ny*sizeof(cimg_uint32));
        if (!raster) {
          _TIFFfree(raster); TIFFClose(tif);
          throw CImgException(_cimg_instance
                              "load_tiff(): Failed to allocate memory (%s) for file '%s'.",
                              cimg_instance,
                              cimg::strbuffersize(nx*ny*sizeof(cimg_uint32)),filename);
        }
        TIFFReadRGBAImage(tif,nx,ny,raster,0);
        switch (spectrum) {
        case 1 :
          cimg_forXY(*this,x,y)
            (*this)(x,y,0) = (T)(float)TIFFGetR(raster[nx*(ny - 1 -y) + x]);
          break;
        case 3 :
          cimg_forXY(*this,x,y) {
            (*this)(x,y,0) = (T)(float)TIFFGetR(raster[nx*(ny - 1 - y) + x]);
            (*this)(x,y,1) = (T)(float)TIFFGetG(raster[nx*(ny - 1 - y) + x]);
            (*this)(x,y,2) = (T)(float)TIFFGetB(raster[nx*(ny - 1 - y) + x]);
          }
          break;
        case 4 :
          cimg_forXY(*this,x,y) {
            (*this)(x,y,0) = (T)(float)TIFFGetR(raster[nx*(ny - 1 - y) + x]);
            (*this)(x,y,1) = (T)(float)TIFFGetG(raster[nx*(ny - 1 - y) + x]);
            (*this)(x,y,2) = (T)(float)TIFFGetB(raster[nx*(ny - 1 - y) + x]);
            (*this)(x,y,3) = (T)(float)TIFFGetA(raster[nx*(ny - 1 - y) + x]);
          }
          break;
        }
        _TIFFfree(raster);
      } else { // Other cases
        cimg_uint16 config;
        TIFFGetField(tif,TIFFTAG_PLANARCONFIG,&config);
        if (TIFFIsTiled(tif)) {
          cimg_uint32 tw = 1, th = 1;
          TIFFGetField(tif,TIFFTAG_TILEWIDTH,&tw);
          TIFFGetField(tif,TIFFTAG_TILELENGTH,&th);
          if (config==PLANARCONFIG_CONTIG) switch (bitspersample) {
            case 8 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_contig<unsigned char>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_contig<signed char>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 16 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_contig<unsigned short>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_contig<short>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 32 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_contig<unsigned int>(tif,samplesperpixel,nx,ny,tw,th);
              else if (sampleformat==SAMPLEFORMAT_INT)
                _load_tiff_tiled_contig<int>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_contig<float>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 64 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_contig<uint64T>(tif,samplesperpixel,nx,ny,tw,th);
              else if (sampleformat==SAMPLEFORMAT_INT)
                _load_tiff_tiled_contig<int64T>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_contig<double>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            } else switch (bitspersample) {
            case 8 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_separate<unsigned char>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_separate<signed char>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 16 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_separate<unsigned short>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_separate<short>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 32 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_separate<unsigned int>(tif,samplesperpixel,nx,ny,tw,th);
              else if (sampleformat==SAMPLEFORMAT_INT)
                _load_tiff_tiled_separate<int>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_separate<float>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 64 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_separate<uint64T>(tif,samplesperpixel,nx,ny,tw,th);
              else if (sampleformat==SAMPLEFORMAT_INT)
                _load_tiff_tiled_separate<int64T>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_separate<double>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            }
        } else {
          if (config==PLANARCONFIG_CONTIG) switch (bitspersample) {
            case 8 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_contig<unsigned char>(tif,samplesperpixel,nx,ny);
              else _load_tiff_contig<signed char>(tif,samplesperpixel,nx,ny);
              break;
            case 16 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_contig<unsigned short>(tif,samplesperpixel,nx,ny);
              else _load_tiff_contig<short>(tif,samplesperpixel,nx,ny);
              break;
            case 32 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_contig<unsigned int>(tif,samplesperpixel,nx,ny);
              else if (sampleformat==SAMPLEFORMAT_INT) _load_tiff_contig<int>(tif,samplesperpixel,nx,ny);
              else _load_tiff_contig<float>(tif,samplesperpixel,nx,ny);
              break;
            case 64 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_contig<uint64T>(tif,samplesperpixel,nx,ny);
              else if (sampleformat==SAMPLEFORMAT_INT) _load_tiff_contig<int64T>(tif,samplesperpixel,nx,ny);
              else _load_tiff_contig<double>(tif,samplesperpixel,nx,ny);
              break;
            } else switch (bitspersample) {
            case 8 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_separate<unsigned char>(tif,samplesperpixel,nx,ny);
              else _load_tiff_separate<signed char>(tif,samplesperpixel,nx,ny);
              break;
            case 16 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_separate<unsigned short>(tif,samplesperpixel,nx,ny);
              else _load_tiff_separate<short>(tif,samplesperpixel,nx,ny);
              break;
            case 32 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_separate<unsigned int>(tif,samplesperpixel,nx,ny);
              else if (sampleformat==SAMPLEFORMAT_INT) _load_tiff_separate<int>(tif,samplesperpixel,nx,ny);
              else _load_tiff_separate<float>(tif,samplesperpixel,nx,ny);
              break;
            case 64 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_separate<uint64T>(tif,samplesperpixel,nx,ny);
              else if (sampleformat==SAMPLEFORMAT_INT) _load_tiff_separate<int64T>(tif,samplesperpixel,nx,ny);
              else _load_tiff_separate<double>(tif,samplesperpixel,nx,ny);
              break;
            }
        }
      }
      return *this;
    }
#endif

    //! Load image from a MINC2 file.
    /**
        \param filename Filename, as a C-string.
    **/
    // (Original code by Haz-Edine Assemlal).
    CImg<T>& load_minc2(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_minc2(): Specified filename is (null).",
                                    cimg_instance);
#ifndef cimg_use_minc2
      return load_other(filename);
#else
      minc::minc_1_reader rdr;
      rdr.open(filename);
      assign(rdr.ndim(1)?rdr.ndim(1):1,
             rdr.ndim(2)?rdr.ndim(2):1,
             rdr.ndim(3)?rdr.ndim(3):1,
             rdr.ndim(4)?rdr.ndim(4):1);
      if (pixel_type()==cimg::type<unsigned char>::string())
        rdr.setup_read_byte();
      else if (pixel_type()==cimg::type<int>::string())
        rdr.setup_read_int();
      else if (pixel_type()==cimg::type<double>::string())
        rdr.setup_read_double();
      else
        rdr.setup_read_float();
      minc::load_standard_volume(rdr,this->_data);
      return *this;
#endif
    }

    //! Load image from a MINC2 file \newinstance.
    static CImg<T> get_load_minc2(const char *const filename) {
      return CImg<T>().load_analyze(filename);
    }

    //! Load image from an ANALYZE7.5/NIFTI file.
    /**
       \param filename Filename, as a C-string.
       \param[out] voxel_size Pointer to the three voxel sizes read from the file.
    **/
    CImg<T>& load_analyze(const char *const filename, float *const voxel_size=0) {
      return _load_analyze(0,filename,voxel_size);
    }

    //! Load image from an ANALYZE7.5/NIFTI file \newinstance.
    static CImg<T> get_load_analyze(const char *const filename, float *const voxel_size=0) {
      return CImg<T>().load_analyze(filename,voxel_size);
    }

    //! Load image from an ANALYZE7.5/NIFTI file \overloading.
    CImg<T>& load_analyze(std::FILE *const file, float *const voxel_size=0) {
      return _load_analyze(file,0,voxel_size);
    }

    //! Load image from an ANALYZE7.5/NIFTI file \newinstance.
    static CImg<T> get_load_analyze(std::FILE *const file, float *const voxel_size=0) {
      return CImg<T>().load_analyze(file,voxel_size);
    }

    CImg<T>& _load_analyze(std::FILE *const file, const char *const filename, float *const voxel_size=0) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_analyze(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *nfile_header = 0, *nfile = 0;
      if (!file) {
        CImg<charT> body(1024);
        const char *const ext = cimg::split_filename(filename,body);
        const unsigned int len = (unsigned int)std::strlen(body);
        if (!cimg::strcasecmp(ext,"hdr")) { // File is an Analyze header file
          nfile_header = cimg::fopen(filename,"rb");
          cimg_snprintf(body._data + len,body._width - len,".img");
          nfile = cimg::fopen(body,"rb");
        } else if (!cimg::strcasecmp(ext,"img")) { // File is an Analyze data file
          nfile = cimg::fopen(filename,"rb");
          cimg_snprintf(body._data + len,body._width - len,".hdr");
          nfile_header = cimg::fopen(body,"rb");
        } else nfile_header = nfile = cimg::fopen(filename,"rb"); // File is a Niftii file
      } else nfile_header = nfile = file; // File is a Niftii file
      if (!nfile || !nfile_header)
        throw CImgIOException(_cimg_instance
                              "load_analyze(): Invalid Analyze7.5 or NIFTI header in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");

      // Read header.
      bool endian = false;
      unsigned int header_size;
      cimg::fread(&header_size,1,nfile_header);
      if (header_size>=4096) { endian = true; cimg::invert_endianness(header_size); }
      if (header_size<128)
        throw CImgIOException(_cimg_instance
                              "load_analyze(): Invalid header size (%u) specified in file '%s'.",
                              cimg_instance,
                              header_size,filename?filename:"(FILE*)");

      unsigned char *const header = new unsigned char[header_size];
      const size_t header_size_read = cimg::fread(header + 4,header_size - 4,nfile_header);
      if (header_size_read!=header_size - 4)
        throw CImgIOException(_cimg_instance
                              "load_analyze(): Cannot read header (of size %u) in file '%s'.",
                              cimg_instance,
                              header_size,filename?filename:"(FILE*)");

      if (!file && nfile_header!=nfile) cimg::fclose(nfile_header);
      if (endian) {
        cimg::invert_endianness((short*)(header + 40),5);
        cimg::invert_endianness((short*)(header + 70),1);
        cimg::invert_endianness((short*)(header + 72),1);
        cimg::invert_endianness((float*)(header + 76),4);
        cimg::invert_endianness((float*)(header + 108),1);
        cimg::invert_endianness((float*)(header + 112),1);
      }

      if (nfile_header==nfile) {
        const unsigned int vox_offset = (unsigned int)*(float*)(header + 108);
        std::fseek(nfile,vox_offset,SEEK_SET);
      }

      unsigned short *dim = (unsigned short*)(header + 40), dimx = 1, dimy = 1, dimz = 1, dimv = 1;
      if (!dim[0])
        cimg::warn(_cimg_instance
                   "load_analyze(): File '%s' defines an image with zero dimensions.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      if (dim[0]>4)
        cimg::warn(_cimg_instance
                   "load_analyze(): File '%s' defines an image with %u dimensions, reading only the 4 first.",
                   cimg_instance,
                   filename?filename:"(FILE*)",dim[0]);

      if (dim[0]>=1) dimx = dim[1];
      if (dim[0]>=2) dimy = dim[2];
      if (dim[0]>=3) dimz = dim[3];
      if (dim[0]>=4) dimv = dim[4];
      float scalefactor = *(float*)(header + 112); if (scalefactor==0) scalefactor = 1;
      const unsigned short datatype = *(unsigned short*)(header + 70);
      if (voxel_size) {
        const float *vsize = (float*)(header + 76);
        voxel_size[0] = vsize[1]; voxel_size[1] = vsize[2]; voxel_size[2] = vsize[3];
      }
      delete[] header;

      // Read pixel data.
      assign(dimx,dimy,dimz,dimv);
      const size_t pdim = (size_t)dimx*dimy*dimz*dimv;
      switch (datatype) {
      case 2 : {
        unsigned char *const buffer = new unsigned char[pdim];
        cimg::fread(buffer,pdim,nfile);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      case 4 : {
        short *const buffer = new short[pdim];
        cimg::fread(buffer,pdim,nfile);
        if (endian) cimg::invert_endianness(buffer,pdim);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      case 8 : {
        int *const buffer = new int[pdim];
        cimg::fread(buffer,pdim,nfile);
        if (endian) cimg::invert_endianness(buffer,pdim);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      case 16 : {
        float *const buffer = new float[pdim];
        cimg::fread(buffer,pdim,nfile);
        if (endian) cimg::invert_endianness(buffer,pdim);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      case 64 : {
        double *const buffer = new double[pdim];
        cimg::fread(buffer,pdim,nfile);
        if (endian) cimg::invert_endianness(buffer,pdim);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      default :
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_analyze(): Unable to load datatype %d in file '%s'",
                              cimg_instance,
                              datatype,filename?filename:"(FILE*)");
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load image from a .cimg[z] file.
    /**
      \param filename Filename, as a C-string.
      \param axis Appending axis, if file contains multiple images. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
      \param align Appending alignment.
    **/
    CImg<T>& load_cimg(const char *const filename, const char axis='z', const float align=0) {
      CImgList<T> list;
      list.load_cimg(filename);
      if (list._width==1) return list[0].move_to(*this);
      return assign(list.get_append(axis,align));
    }

    //! Load image from a .cimg[z] file \newinstance.
    static CImg<T> get_load_cimg(const char *const filename, const char axis='z', const float align=0) {
      return CImg<T>().load_cimg(filename,axis,align);
    }

    //! Load image from a .cimg[z] file \overloading.
    CImg<T>& load_cimg(std::FILE *const file, const char axis='z', const float align=0) {
      CImgList<T> list;
      list.load_cimg(file);
      if (list._width==1) return list[0].move_to(*this);
      return assign(list.get_append(axis,align));
    }

    //! Load image from a .cimg[z] file \newinstance.
    static CImg<T> get_load_cimg(std::FILE *const file, const char axis='z', const float align=0) {
      return CImg<T>().load_cimg(file,axis,align);
    }

    //! Load sub-images of a .cimg file.
    /**
      \param filename Filename, as a C-string.
      \param n0 Starting frame.
      \param n1 Ending frame (~0U for max).
      \param x0 X-coordinate of the starting sub-image vertex.
      \param y0 Y-coordinate of the starting sub-image vertex.
      \param z0 Z-coordinate of the starting sub-image vertex.
      \param c0 C-coordinate of the starting sub-image vertex.
      \param x1 X-coordinate of the ending sub-image vertex (~0U for max).
      \param y1 Y-coordinate of the ending sub-image vertex (~0U for max).
      \param z1 Z-coordinate of the ending sub-image vertex (~0U for max).
      \param c1 C-coordinate of the ending sub-image vertex (~0U for max).
      \param axis Appending axis, if file contains multiple images. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
      \param align Appending alignment.
    **/
    CImg<T>& load_cimg(const char *const filename,
                       const unsigned int n0, const unsigned int n1,
                       const unsigned int x0, const unsigned int y0,
                       const unsigned int z0, const unsigned int c0,
                       const unsigned int x1, const unsigned int y1,
                       const unsigned int z1, const unsigned int c1,
                       const char axis='z', const float align=0) {
      CImgList<T> list;
      list.load_cimg(filename,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1);
      if (list._width==1) return list[0].move_to(*this);
      return assign(list.get_append(axis,align));
    }

    //! Load sub-images of a .cimg file \newinstance.
    static CImg<T> get_load_cimg(const char *const filename,
                                 const unsigned int n0, const unsigned int n1,
                                 const unsigned int x0, const unsigned int y0,
                                 const unsigned int z0, const unsigned int c0,
                                 const unsigned int x1, const unsigned int y1,
                                 const unsigned int z1, const unsigned int c1,
                                 const char axis='z', const float align=0) {
      return CImg<T>().load_cimg(filename,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1,axis,align);
    }

    //! Load sub-images of a .cimg file \overloading.
    CImg<T>& load_cimg(std::FILE *const file,
                       const unsigned int n0, const unsigned int n1,
                       const unsigned int x0, const unsigned int y0,
                       const unsigned int z0, const unsigned int c0,
                       const unsigned int x1, const unsigned int y1,
                       const unsigned int z1, const unsigned int c1,
                       const char axis='z', const float align=0) {
      CImgList<T> list;
      list.load_cimg(file,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1);
      if (list._width==1) return list[0].move_to(*this);
      return assign(list.get_append(axis,align));
    }

    //! Load sub-images of a .cimg file \newinstance.
    static CImg<T> get_load_cimg(std::FILE *const file,
                                 const unsigned int n0, const unsigned int n1,
                                 const unsigned int x0, const unsigned int y0,
                                 const unsigned int z0, const unsigned int c0,
                                 const unsigned int x1, const unsigned int y1,
                                 const unsigned int z1, const unsigned int c1,
                                 const char axis='z', const float align=0) {
      return CImg<T>().load_cimg(file,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1,axis,align);
    }

    //! Load image from an INRIMAGE-4 file.
    /**
       \param filename Filename, as a C-string.
       \param[out] voxel_size Pointer to the three voxel sizes read from the file.
    **/
    CImg<T>& load_inr(const char *const filename, float *const voxel_size=0) {
      return _load_inr(0,filename,voxel_size);
    }

    //! Load image from an INRIMAGE-4 file \newinstance.
    static CImg<T> get_load_inr(const char *const filename, float *const voxel_size=0) {
      return CImg<T>().load_inr(filename,voxel_size);
    }

    //! Load image from an INRIMAGE-4 file \overloading.
    CImg<T>& load_inr(std::FILE *const file, float *const voxel_size=0) {
      return _load_inr(file,0,voxel_size);
    }

    //! Load image from an INRIMAGE-4 file \newinstance.
    static CImg<T> get_load_inr(std::FILE *const file, float *voxel_size=0) {
      return CImg<T>().load_inr(file,voxel_size);
    }

    static void _load_inr_header(std::FILE *file, int out[8], float *const voxel_size) {
      CImg<charT> item(1024), tmp1(64), tmp2(64);
      *item = *tmp1 = *tmp2 = 0;
      out[0] = std::fscanf(file,"%63s",item._data);
      out[0] = out[1] = out[2] = out[3] = out[5] = 1; out[4] = out[6] = out[7] = -1;
      if (cimg::strncasecmp(item,"#INRIMAGE-4#{",13)!=0)
        throw CImgIOException("CImg<%s>::load_inr(): INRIMAGE-4 header not found.",
                              pixel_type());

      while (std::fscanf(file," %63[^\n]%*c",item._data)!=EOF && std::strncmp(item,"##}",3)) {
        cimg_sscanf(item," XDIM%*[^0-9]%d",out);
        cimg_sscanf(item," YDIM%*[^0-9]%d",out + 1);
        cimg_sscanf(item," ZDIM%*[^0-9]%d",out + 2);
        cimg_sscanf(item," VDIM%*[^0-9]%d",out + 3);
        cimg_sscanf(item," PIXSIZE%*[^0-9]%d",out + 6);
        if (voxel_size) {
          cimg_sscanf(item," VX%*[^0-9.+-]%f",voxel_size);
          cimg_sscanf(item," VY%*[^0-9.+-]%f",voxel_size + 1);
          cimg_sscanf(item," VZ%*[^0-9.+-]%f",voxel_size + 2);
        }
        if (cimg_sscanf(item," CPU%*[ =]%s",tmp1._data)) out[7] = cimg::strncasecmp(tmp1,"sun",3)?0:1;
        switch (cimg_sscanf(item," TYPE%*[ =]%s %s",tmp1._data,tmp2._data)) {
        case 0 : break;
        case 2 :
          out[5] = cimg::strncasecmp(tmp1,"unsigned",8)?1:0;
          std::strncpy(tmp1,tmp2,tmp1._width - 1); // Fallthrough
        case 1 :
          if (!cimg::strncasecmp(tmp1,"int",3) || !cimg::strncasecmp(tmp1,"fixed",5))  out[4] = 0;
          if (!cimg::strncasecmp(tmp1,"float",5) || !cimg::strncasecmp(tmp1,"double",6)) out[4] = 1;
          if (!cimg::strncasecmp(tmp1,"packed",6)) out[4] = 2;
          if (out[4]>=0) break; // Fallthrough
        default :
          throw CImgIOException("CImg<%s>::load_inr(): Invalid pixel type '%s' defined in header.",
                                pixel_type(),
                                tmp2._data);
        }
      }
      if (out[0]<0 || out[1]<0 || out[2]<0 || out[3]<0)
        throw CImgIOException("CImg<%s>::load_inr(): Invalid dimensions (%d,%d,%d,%d) defined in header.",
                              pixel_type(),
                              out[0],out[1],out[2],out[3]);
      if (out[4]<0 || out[5]<0)
        throw CImgIOException("CImg<%s>::load_inr(): Incomplete pixel type defined in header.",
                              pixel_type());
      if (out[6]<0)
        throw CImgIOException("CImg<%s>::load_inr(): Incomplete PIXSIZE field defined in header.",
                              pixel_type());
      if (out[7]<0)
        throw CImgIOException("CImg<%s>::load_inr(): Big/Little Endian coding type undefined in header.",
                              pixel_type());
    }

    CImg<T>& _load_inr(std::FILE *const file, const char *const filename, float *const voxel_size) {
#define _cimg_load_inr_case(Tf,sign,pixsize,Ts) \
     if (!loaded && fopt[6]==pixsize && fopt[4]==Tf && fopt[5]==sign) { \
        Ts *xval, *const val = new Ts[(size_t)fopt[0]*fopt[3]]; \
        cimg_forYZ(*this,y,z) { \
            cimg::fread(val,fopt[0]*fopt[3],nfile); \
            if (fopt[7]!=endian) cimg::invert_endianness(val,fopt[0]*fopt[3]); \
            xval = val; cimg_forX(*this,x) cimg_forC(*this,c) (*this)(x,y,z,c) = (T)*(xval++); \
          } \
        delete[] val; \
        loaded = true; \
      }

      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_inr(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      int fopt[8], endian = cimg::endianness()?1:0;
      bool loaded = false;
      if (voxel_size) voxel_size[0] = voxel_size[1] = voxel_size[2] = 1;
      _load_inr_header(nfile,fopt,voxel_size);
      assign(fopt[0],fopt[1],fopt[2],fopt[3]);
      _cimg_load_inr_case(0,0,8,unsigned char);
      _cimg_load_inr_case(0,1,8,char);
      _cimg_load_inr_case(0,0,16,unsigned short);
      _cimg_load_inr_case(0,1,16,short);
      _cimg_load_inr_case(0,0,32,unsigned int);
      _cimg_load_inr_case(0,1,32,int);
      _cimg_load_inr_case(1,0,32,float);
      _cimg_load_inr_case(1,1,32,float);
      _cimg_load_inr_case(1,0,64,double);
      _cimg_load_inr_case(1,1,64,double);
      if (!loaded) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_inr(): Unknown pixel type defined in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load image from a EXR file.
    /**
      \param filename Filename, as a C-string.
    **/
    CImg<T>& load_exr(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_exr(): Specified filename is (null).",
                                    cimg_instance);
#if defined(cimg_use_openexr)
      Imf::RgbaInputFile file(filename);
      Imath::Box2i dw = file.dataWindow();
      const int
        inwidth = dw.max.x - dw.min.x + 1,
        inheight = dw.max.y - dw.min.y + 1;
      Imf::Array2D<Imf::Rgba> pixels;
      pixels.resizeErase(inheight,inwidth);
      file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y*inwidth, 1, inwidth);
      file.readPixels(dw.min.y, dw.max.y);
      assign(inwidth,inheight,1,4);
      T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2), *ptr_a = data(0,0,0,3);
      cimg_forXY(*this,x,y) {
        *(ptr_r++) = (T)pixels[y][x].r;
        *(ptr_g++) = (T)pixels[y][x].g;
        *(ptr_b++) = (T)pixels[y][x].b;
        *(ptr_a++) = (T)pixels[y][x].a;
      }
      return *this;
#elif defined(cimg_use_tinyexr)
      float *res;
      const char *err = 0;
      int width = 0, height = 0;
      const int ret = LoadEXR(&res,&width,&height,filename,&err);
      if (ret) throw CImgIOException(_cimg_instance
                                     "load_exr(): Unable to load EXR file '%s'.",
                                     cimg_instance,filename);
      CImg<floatT>(res,4,width,height,1,true).get_permute_axes("yzcx").move_to(*this);
      std::free(res);
      return *this;
#else
      return load_other(filename);
#endif
    }

    //! Load image from a EXR file \newinstance.
    static CImg<T> get_load_exr(const char *const filename) {
      return CImg<T>().load_exr(filename);
    }

    //! Load image from a PANDORE-5 file.
    /**
      \param filename Filename, as a C-string.
    **/
    CImg<T>& load_pandore(const char *const filename) {
      return _load_pandore(0,filename);
    }

    //! Load image from a PANDORE-5 file \newinstance.
    static CImg<T> get_load_pandore(const char *const filename) {
      return CImg<T>().load_pandore(filename);
    }

    //! Load image from a PANDORE-5 file \overloading.
    CImg<T>& load_pandore(std::FILE *const file) {
      return _load_pandore(file,0);
    }

    //! Load image from a PANDORE-5 file \newinstance.
    static CImg<T> get_load_pandore(std::FILE *const file) {
      return CImg<T>().load_pandore(file);
    }

    CImg<T>& _load_pandore(std::FILE *const file, const char *const filename) {
#define __cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,ndim,stype) \
        cimg::fread(dims,nbdim,nfile); \
        if (endian) cimg::invert_endianness(dims,nbdim); \
        if ((ulongT)nwidth*nheight*ndepth*ndim>fsiz) \
          throw CImgIOException(_cimg_instance \
                                "load_pandore(): File size %lu for filename '%s' does not match "\
                                "encoded image dimensions (%d,%d,%d,%d).",\
                                cimg_instance,\
                                (long)fsiz,filename?filename:"(FILE*)",\
                                (int)nwidth,(int)nheight,(int)ndepth,(int)ndim); \
        assign(nwidth,nheight,ndepth,ndim); \
        const size_t siz = size(); \
        stype *buffer = new stype[siz]; \
        cimg::fread(buffer,siz,nfile); \
        if (endian) cimg::invert_endianness(buffer,siz); \
        T *ptrd = _data; \
        cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++); \
        buffer-=siz; \
        delete[] buffer

#define _cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,dim,stype1,stype2,stype3,ltype) { \
        if (sizeof(stype1)==ltype) { __cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,dim,stype1); } \
        else if (sizeof(stype2)==ltype) { __cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,dim,stype2); } \
        else if (sizeof(stype3)==ltype) { __cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,dim,stype3); } \
        else throw CImgIOException(_cimg_instance \
                                   "load_pandore(): Unknown pixel datatype in file '%s'.", \
                                   cimg_instance, \
                                   filename?filename:"(FILE*)"); }
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_pandore(): Specified filename is (null).",
                                    cimg_instance);

      const ulongT fsiz = file?(ulongT)cimg_max_buf_size:(ulongT)cimg::fsize(filename);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      CImg<charT> header(32);
      cimg::fread(header._data,12,nfile);
      if (cimg::strncasecmp("PANDORE",header,7)) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pandore(): PANDORE header not found in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      unsigned int imageid, dims[8] = {};
      int ptbuf[4] = {};
      cimg::fread(&imageid,1,nfile);
      const bool endian = imageid>255;
      if (endian) cimg::invert_endianness(imageid);
      cimg::fread(header._data,20,nfile);

      switch (imageid) {
      case 2 : _cimg_load_pandore_case(2,dims[1],1,1,1,unsigned char,unsigned char,unsigned char,1); break;
      case 3 : _cimg_load_pandore_case(2,dims[1],1,1,1,long,int,short,4); break;
      case 4 : _cimg_load_pandore_case(2,dims[1],1,1,1,double,float,float,4); break;
      case 5 : _cimg_load_pandore_case(3,dims[2],dims[1],1,1,unsigned char,unsigned char,unsigned char,1); break;
      case 6 : _cimg_load_pandore_case(3,dims[2],dims[1],1,1,long,int,short,4); break;
      case 7 : _cimg_load_pandore_case(3,dims[2],dims[1],1,1,double,float,float,4); break;
      case 8 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],1,unsigned char,unsigned char,unsigned char,1); break;
      case 9 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],1,long,int,short,4); break;
      case 10 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],1,double,float,float,4); break;
      case 11 : { // Region 1D
        cimg::fread(dims,3,nfile);
        if (endian) cimg::invert_endianness(dims,3);
        assign(dims[1],1,1,1);
        const unsigned siz = size();
        if (dims[2]<256) {
          unsigned char *buffer = new unsigned char[siz];
          cimg::fread(buffer,siz,nfile);
          T *ptrd = _data;
          cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
          buffer-=siz;
          delete[] buffer;
        } else {
          if (dims[2]<65536) {
            unsigned short *buffer = new unsigned short[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          } else {
            unsigned int *buffer = new unsigned int[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          }
        }
      }
        break;
      case 12 : { // Region 2D
        cimg::fread(dims,4,nfile);
        if (endian) cimg::invert_endianness(dims,4);
        assign(dims[2],dims[1],1,1);
        const size_t siz = size();
        if (dims[3]<256) {
          unsigned char *buffer = new unsigned char[siz];
          cimg::fread(buffer,siz,nfile);
          T *ptrd = _data;
          cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
          buffer-=siz;
          delete[] buffer;
        } else {
          if (dims[3]<65536) {
            unsigned short *buffer = new unsigned short[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          } else {
            unsigned int *buffer = new unsigned int[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          }
        }
      }
        break;
      case 13 : { // Region 3D
        cimg::fread(dims,5,nfile);
        if (endian) cimg::invert_endianness(dims,5);
        assign(dims[3],dims[2],dims[1],1);
        const size_t siz = size();
        if (dims[4]<256) {
          unsigned char *buffer = new unsigned char[siz];
          cimg::fread(buffer,siz,nfile);
          T *ptrd = _data;
          cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
          buffer-=siz;
          delete[] buffer;
        } else {
          if (dims[4]<65536) {
            unsigned short *buffer = new unsigned short[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          } else {
            unsigned int *buffer = new unsigned int[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          }
        }
      }
        break;
      case 16 : _cimg_load_pandore_case(4,dims[2],dims[1],1,3,unsigned char,unsigned char,unsigned char,1); break;
      case 17 : _cimg_load_pandore_case(4,dims[2],dims[1],1,3,long,int,short,4); break;
      case 18 : _cimg_load_pandore_case(4,dims[2],dims[1],1,3,double,float,float,4); break;
      case 19 : _cimg_load_pandore_case(5,dims[3],dims[2],dims[1],3,unsigned char,unsigned char,unsigned char,1); break;
      case 20 : _cimg_load_pandore_case(5,dims[3],dims[2],dims[1],3,long,int,short,4); break;
      case 21 : _cimg_load_pandore_case(5,dims[3],dims[2],dims[1],3,double,float,float,4); break;
      case 22 : _cimg_load_pandore_case(2,dims[1],1,1,dims[0],unsigned char,unsigned char,unsigned char,1); break;
      case 23 : _cimg_load_pandore_case(2,dims[1],1,1,dims[0],long,int,short,4); break;
      case 24 : _cimg_load_pandore_case(2,dims[1],1,1,dims[0],unsigned long,unsigned int,unsigned short,4); break;
      case 25 : _cimg_load_pandore_case(2,dims[1],1,1,dims[0],double,float,float,4); break;
      case 26 : _cimg_load_pandore_case(3,dims[2],dims[1],1,dims[0],unsigned char,unsigned char,unsigned char,1); break;
      case 27 : _cimg_load_pandore_case(3,dims[2],dims[1],1,dims[0],long,int,short,4); break;
      case 28 : _cimg_load_pandore_case(3,dims[2],dims[1],1,dims[0],unsigned long,unsigned int,unsigned short,4); break;
      case 29 : _cimg_load_pandore_case(3,dims[2],dims[1],1,dims[0],double,float,float,4); break;
      case 30 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],dims[0],unsigned char,unsigned char,unsigned char,1);
        break;
      case 31 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],dims[0],long,int,short,4); break;
      case 32 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],dims[0],unsigned long,unsigned int,unsigned short,4);
        break;
      case 33 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],dims[0],double,float,float,4); break;
      case 34 : { // Points 1D
        cimg::fread(ptbuf,1,nfile);
        if (endian) cimg::invert_endianness(ptbuf,1);
        assign(1); (*this)(0) = (T)ptbuf[0];
      } break;
      case 35 : { // Points 2D
        cimg::fread(ptbuf,2,nfile);
        if (endian) cimg::invert_endianness(ptbuf,2);
        assign(2); (*this)(0) = (T)ptbuf[1]; (*this)(1) = (T)ptbuf[0];
      } break;
      case 36 : { // Points 3D
        cimg::fread(ptbuf,3,nfile);
        if (endian) cimg::invert_endianness(ptbuf,3);
        assign(3); (*this)(0) = (T)ptbuf[2]; (*this)(1) = (T)ptbuf[1]; (*this)(2) = (T)ptbuf[0];
      } break;
      default :
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pandore(): Unable to load data with ID_type %u in file '%s'.",
                              cimg_instance,
                              imageid,filename?filename:"(FILE*)");
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load image from a PAR-REC (Philips) file.
    /**
      \param filename Filename, as a C-string.
      \param axis Appending axis, if file contains multiple images. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
      \param align Appending alignment.
    **/
    CImg<T>& load_parrec(const char *const filename, const char axis='c', const float align=0) {
      CImgList<T> list;
      list.load_parrec(filename);
      if (list._width==1) return list[0].move_to(*this);
      return assign(list.get_append(axis,align));
    }

    //! Load image from a PAR-REC (Philips) file \newinstance.
    static CImg<T> get_load_parrec(const char *const filename, const char axis='c', const float align=0) {
      return CImg<T>().load_parrec(filename,axis,align);
    }

    //! Load image from a raw binary file.
    /**
      \param filename Filename, as a C-string.
      \param size_x Width of the image buffer.
      \param size_y Height of the image buffer.
      \param size_z Depth of the image buffer.
      \param size_c Spectrum of the image buffer.
      \param is_multiplexed Tells if the image values are multiplexed along the C-axis.
      \param invert_endianness Tells if the endianness of the image buffer must be inverted.
      \param offset Starting offset of the read in the specified file.
    **/
    CImg<T>& load_raw(const char *const filename,
                      const unsigned int size_x=0, const unsigned int size_y=1,
                      const unsigned int size_z=1, const unsigned int size_c=1,
                      const bool is_multiplexed=false, const bool invert_endianness=false,
                      const ulongT offset=0) {
      return _load_raw(0,filename,size_x,size_y,size_z,size_c,is_multiplexed,invert_endianness,offset);
    }

    //! Load image from a raw binary file \newinstance.
    static CImg<T> get_load_raw(const char *const filename,
                                const unsigned int size_x=0, const unsigned int size_y=1,
                                const unsigned int size_z=1, const unsigned int size_c=1,
                                const bool is_multiplexed=false, const bool invert_endianness=false,
                                const ulongT offset=0) {
      return CImg<T>().load_raw(filename,size_x,size_y,size_z,size_c,is_multiplexed,invert_endianness,offset);
    }

    //! Load image from a raw binary file \overloading.
    CImg<T>& load_raw(std::FILE *const file,
                      const unsigned int size_x=0, const unsigned int size_y=1,
                      const unsigned int size_z=1, const unsigned int size_c=1,
                      const bool is_multiplexed=false, const bool invert_endianness=false,
                      const ulongT offset=0) {
      return _load_raw(file,0,size_x,size_y,size_z,size_c,is_multiplexed,invert_endianness,offset);
    }

    //! Load image from a raw binary file \newinstance.
    static CImg<T> get_load_raw(std::FILE *const file,
                                const unsigned int size_x=0, const unsigned int size_y=1,
                                const unsigned int size_z=1, const unsigned int size_c=1,
                                const bool is_multiplexed=false, const bool invert_endianness=false,
                                const ulongT offset=0) {
      return CImg<T>().load_raw(file,size_x,size_y,size_z,size_c,is_multiplexed,invert_endianness,offset);
    }

    CImg<T>& _load_raw(std::FILE *const file, const char *const filename,
                       const unsigned int size_x, const unsigned int size_y,
                       const unsigned int size_z, const unsigned int size_c,
                       const bool is_multiplexed, const bool invert_endianness,
                       const ulongT offset) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_raw(): Specified filename is (null).",
                                    cimg_instance);
      if (cimg::is_directory(filename))
        throw CImgArgumentException(_cimg_instance
                                    "load_raw(): Specified filename '%s' is a directory.",
                                    cimg_instance,filename);
      const bool is_bool = pixel_type()==cimg::type<bool>::string();
      ulongT siz = (ulongT)size_x*size_y*size_z*size_c;
      unsigned int
        _size_x = size_x,
        _size_y = size_y,
        _size_z = size_z,
        _size_c = size_c;
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      if (!siz) { // Retrieve file size
        const longT fpos = cimg::ftell(nfile);
        if (fpos<0) throw CImgArgumentException(_cimg_instance
                                                "load_raw(): Cannot determine size of input file '%s'.",
                                                cimg_instance,filename?filename:"(FILE*)");
        cimg::fseek(nfile,0,SEEK_END);
        siz = (ulongT)cimg::ftell(nfile);
        if (!is_bool) { siz/=sizeof(T); _size_y = (unsigned int)siz; }
        else _size_y = (unsigned int)(siz*8);
        _size_x = _size_z = _size_c = 1;
        cimg::fseek(nfile,fpos,SEEK_SET);
      }
      cimg::fseek(nfile,(longT)offset,SEEK_SET);
      assign(_size_x,_size_y,_size_z,_size_c,0);

      if (is_bool) { // Boolean data (bitwise)
        unsigned char *const buf = new unsigned char[siz];
        cimg::fread(buf,siz,nfile);
        _uchar2bool(buf,siz,is_multiplexed);
        delete[] buf;
      } else { // Non-boolean data
        if (siz && (!is_multiplexed || size_c==1)) { // Non-multiplexed
          cimg::fread(_data,siz,nfile);
          if (invert_endianness) cimg::invert_endianness(_data,siz);
        } else if (siz) { // Multiplexed
          CImg<T> buf(1,1,1,_size_c);
          cimg_forXYZ(*this,x,y,z) {
            cimg::fread(buf._data,_size_c,nfile);
            if (invert_endianness) cimg::invert_endianness(buf._data,_size_c);
            set_vector_at(buf,x,y,z);
          }
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Load image sequence from a YUV file.
    /**
      \param filename Filename, as a C-string.
      \param size_x Width of the frames.
      \param size_y Height of the frames.
      \param chroma_subsampling Type of chroma subsampling. Can be <tt>{ 420 | 422 | 444 }</tt>.
      \param first_frame Index of the first frame to read.
      \param last_frame Index of the last frame to read.
      \param step_frame Step value for frame reading.
      \param yuv2rgb Tells if the YUV to RGB transform must be applied.
      \param axis Appending axis, if file contains multiple images. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
    **/
    CImg<T>& load_yuv(const char *const filename,
                      const unsigned int size_x, const unsigned int size_y=1,
                      const unsigned int chroma_subsampling=444,
                      const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                      const unsigned int step_frame=1, const bool yuv2rgb=true, const char axis='z') {
      return get_load_yuv(filename,size_x,size_y,chroma_subsampling,
                          first_frame,last_frame,step_frame,yuv2rgb,axis).move_to(*this);
    }

    //! Load image sequence from a YUV file \newinstance.
    static CImg<T> get_load_yuv(const char *const filename,
                                const unsigned int size_x, const unsigned int size_y=1,
                                const unsigned int chroma_subsampling=444,
                                const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                                const unsigned int step_frame=1, const bool yuv2rgb=true, const char axis='z') {
      return CImgList<T>().load_yuv(filename,size_x,size_y,chroma_subsampling,
                                    first_frame,last_frame,step_frame,yuv2rgb).get_append(axis);
    }

    //! Load image sequence from a YUV file \overloading.
    CImg<T>& load_yuv(std::FILE *const file,
                      const unsigned int size_x, const unsigned int size_y=1,
                      const unsigned int chroma_subsampling=444,
                      const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                      const unsigned int step_frame=1, const bool yuv2rgb=true, const char axis='z') {
      return get_load_yuv(file,size_x,size_y,chroma_subsampling,
                          first_frame,last_frame,step_frame,yuv2rgb,axis).move_to(*this);
    }

    //! Load image sequence from a YUV file \newinstance.
    static CImg<T> get_load_yuv(std::FILE *const file,
                                const unsigned int size_x, const unsigned int size_y=1,
                                const unsigned int chroma_subsampling=444,
                                const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                                const unsigned int step_frame=1, const bool yuv2rgb=true, const char axis='z') {
      return CImgList<T>().load_yuv(file,size_x,size_y,chroma_subsampling,
                                    first_frame,last_frame,step_frame,yuv2rgb).get_append(axis);
    }

    //! Load 3D object from a .OFF file.
    /**
        \param[out] primitives Primitives data of the 3D object.
        \param[out] colors Colors data of the 3D object.
        \param filename Filename, as a C-string.
    **/
    template<typename tf, typename tc>
    CImg<T>& load_off(CImgList<tf>& primitives, CImgList<tc>& colors, const char *const filename) {
      return _load_off(primitives,colors,0,filename);
    }

    //! Load 3D object from a .OFF file \newinstance.
    template<typename tf, typename tc>
    static CImg<T> get_load_off(CImgList<tf>& primitives, CImgList<tc>& colors, const char *const filename) {
      return CImg<T>().load_off(primitives,colors,filename);
    }

    //! Load 3D object from a .OFF file \overloading.
    template<typename tf, typename tc>
    CImg<T>& load_off(CImgList<tf>& primitives, CImgList<tc>& colors, std::FILE *const file) {
      return _load_off(primitives,colors,file,0);
    }

    //! Load 3D object from a .OFF file \newinstance.
    template<typename tf, typename tc>
    static CImg<T> get_load_off(CImgList<tf>& primitives, CImgList<tc>& colors, std::FILE *const file) {
      return CImg<T>().load_off(primitives,colors,file);
    }

    template<typename tf, typename tc>
    CImg<T>& _load_off(CImgList<tf>& primitives, CImgList<tc>& colors,
                       std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_off(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"r");
      unsigned int nb_points = 0, nb_primitives = 0, nb_read = 0;
      CImg<charT> line(256); *line = 0;
      int err;

      // Skip comments, and read magic string OFF.
      do { err = std::fscanf(nfile,"%255[^\n] ",line._data); } while (!err || (err==1 && *line=='#'));
      if (cimg::strncasecmp(line,"OFF",3) && cimg::strncasecmp(line,"COFF",4)) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_off(): OFF header not found in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      do { err = std::fscanf(nfile,"%255[^\n] ",line._data); } while (!err || (err==1 && *line=='#'));
      if ((err = cimg_sscanf(line,"%u%u%*[^\n] ",&nb_points,&nb_primitives))!=2) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_off(): Invalid number of vertices or primitives specified in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }

      // Read points data.
      assign(nb_points,3);
      float X = 0, Y = 0, Z = 0;
      cimg_forX(*this,l) {
        do { err = std::fscanf(nfile,"%255[^\n] ",line._data); } while (!err || (err==1 && *line=='#'));
        if ((err = cimg_sscanf(line,"%f%f%f%*[^\n] ",&X,&Y,&Z))!=3) {
          if (!file) cimg::fclose(nfile);
          throw CImgIOException(_cimg_instance
                                "load_off(): Failed to read vertex %u/%u in file '%s'.",
                                cimg_instance,
                                l + 1,nb_points,filename?filename:"(FILE*)");
        }
        (*this)(l,0) = (T)X; (*this)(l,1) = (T)Y; (*this)(l,2) = (T)Z;
      }

      // Read primitive data.
      primitives.assign();
      colors.assign();
      bool stop_flag = false;
      while (!stop_flag) {
        float c0 = 0.7f, c1 = 0.7f, c2 = 0.7f;
        unsigned int prim = 0, i0 = 0, i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0, i7 = 0;
        *line = 0;
        if ((err = std::fscanf(nfile,"%u",&prim))!=1) stop_flag = true;
        else {
          ++nb_read;
          switch (prim) {
          case 1 : {
            if ((err = std::fscanf(nfile,"%u%255[^\n] ",&i0,line._data))<2) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0).move_to(primitives);
              CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)).move_to(colors);
            }
          } break;
          case 2 : {
            if ((err = std::fscanf(nfile,"%u%u%255[^\n] ",&i0,&i1,line._data))<2) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i1).move_to(primitives);
              CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)).move_to(colors);
            }
          } break;
          case 3 : {
            if ((err = std::fscanf(nfile,"%u%u%u%255[^\n] ",&i0,&i1,&i2,line._data))<3) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i2,i1).move_to(primitives);
              CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)).move_to(colors);
            }
          } break;
          case 4 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,line._data))<4) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i3,i2,i1).move_to(primitives);
              CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)).move_to(colors);
            }
          } break;
          case 5 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,&i4,line._data))<5) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i3,i2,i1).move_to(primitives);
              CImg<tf>::vector(i0,i4,i3).move_to(primitives);
              colors.insert(2,CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)));
              ++nb_primitives;
            }
          } break;
          case 6 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,&i4,&i5,line._data))<6) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i3,i2,i1).move_to(primitives);
              CImg<tf>::vector(i0,i5,i4,i3).move_to(primitives);
              colors.insert(2,CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)));
              ++nb_primitives;
            }
          } break;
          case 7 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,&i4,&i5,&i6,line._data))<7) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i4,i3,i1).move_to(primitives);
              CImg<tf>::vector(i0,i6,i5,i4).move_to(primitives);
              CImg<tf>::vector(i3,i2,i1).move_to(primitives);
              colors.insert(3,CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)));
              ++(++nb_primitives);
            }
          } break;
          case 8 : {
            if ((err = std::fscanf(nfile,"%u%u%u%u%u%u%u%u%255[^\n] ",&i0,&i1,&i2,&i3,&i4,&i5,&i6,&i7,line._data))<7) {
              cimg::warn(_cimg_instance
                         "load_off(): Failed to read primitive %u/%u from file '%s'.",
                         cimg_instance,
                         nb_read,nb_primitives,filename?filename:"(FILE*)");

              err = std::fscanf(nfile,"%*[^\n] ");
            } else {
              err = cimg_sscanf(line,"%f%f%f",&c0,&c1,&c2);
              CImg<tf>::vector(i0,i3,i2,i1).move_to(primitives);
              CImg<tf>::vector(i0,i5,i4,i3).move_to(primitives);
              CImg<tf>::vector(i0,i7,i6,i5).move_to(primitives);
              colors.insert(3,CImg<tc>::vector((tc)(c0*255),(tc)(c1*255),(tc)(c2*255)));
              ++(++nb_primitives);
            }
          } break;
          default :
            cimg::warn(_cimg_instance
                       "load_off(): Failed to read primitive %u/%u (%u vertices) from file '%s'.",
                       cimg_instance,
                       nb_read,nb_primitives,prim,filename?filename:"(FILE*)");

            err = std::fscanf(nfile,"%*[^\n] ");
          }
        }
      }
      if (!file) cimg::fclose(nfile);
      if (primitives._width!=nb_primitives)
        cimg::warn(_cimg_instance
                   "load_off(): Only %u/%u primitives read from file '%s'.",
                   cimg_instance,
                   primitives._width,nb_primitives,filename?filename:"(FILE*)");
      return *this;
    }

    //! Load image sequence from a video file, using OpenCV library.
    /**
      \param filename Filename, as a C-string.
      \param first_frame Index of the first frame to read.
      \param last_frame Index of the last frame to read.
      \param step_frame Step value for frame reading.
      \param axis Alignment axis.
      \param align Appending alignment.
    **/
    CImg<T>& load_video(const char *const filename,
                        const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                        const unsigned int step_frame=1,
                        const char axis='z', const float align=0) {
      return get_load_video(filename,first_frame,last_frame,step_frame,axis,align).move_to(*this);
    }

    //! Load image sequence from a video file, using OpenCV library \newinstance.
    static CImg<T> get_load_video(const char *const filename,
                                  const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                                  const unsigned int step_frame=1,
                                  const char axis='z', const float align=0) {
      return CImgList<T>().load_video(filename,first_frame,last_frame,step_frame).get_append(axis,align);
    }

    //! Load image sequence using FFMPEG's external tool 'ffmpeg'.
    /**
      \param filename Filename, as a C-string.
      \param axis Appending axis, if file contains multiple images. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
      \param align Appending alignment.
    **/
    CImg<T>& load_ffmpeg_external(const char *const filename, const char axis='z', const float align=0) {
      return get_load_ffmpeg_external(filename,axis,align).move_to(*this);
    }

    //! Load image sequence using FFMPEG's external tool 'ffmpeg' \newinstance.
    static CImg<T> get_load_ffmpeg_external(const char *const filename, const char axis='z', const float align=0) {
      return CImgList<T>().load_ffmpeg_external(filename).get_append(axis,align);
    }

    //! Load gif file, using Imagemagick or GraphicsMagicks's external tools.
    /**
      \param filename Filename, as a C-string.
      \param axis Appending axis, if file contains multiple images. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
      \param align Appending alignment.
    **/
    CImg<T>& load_gif_external(const char *const filename,
                               const char axis='z', const float align=0) {
      return get_load_gif_external(filename,axis,align).move_to(*this);
    }

    //! Load gif file, using ImageMagick or GraphicsMagick's external tool 'convert' \newinstance.
    static CImg<T> get_load_gif_external(const char *const filename,
                                         const char axis='z', const float align=0) {
      return CImgList<T>().load_gif_external(filename).get_append(axis,align);
    }

    //! Load image from a HEIC file.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_heif(const char *const filename) {
      return _load_heif(filename);
    }

    //! Load image from a HEIC file \newinstance.
    static CImg<T> get_load_heif(const char *const filename) {
      return CImg<T>().load_heif(filename);
    }

    CImg<T>& _load_heif(const char *const filename) {
#ifndef cimg_use_heif
      return load_other(filename);
#else
      try {
        heif::Context ctx;
        ctx.read_from_file(filename);

        heif::ImageHandle handle = ctx.get_primary_image_handle();
        const heif::Image image =
          handle.decode_image(heif_colorspace_RGB,handle.has_alpha_channel()?heif_chroma_interleaved_RGBA:
                              heif_chroma_interleaved_RGB);
        const int
          W = image.get_width(heif_channel_interleaved),
          H = image.get_height(heif_channel_interleaved),
          S = handle.has_alpha_channel()?4:3;
        assign(W,H,1,S);

#if LIBHEIF_NUMERIC_VERSION < LIBHEIF_MAKE_VERSION(1, 20, 0) || LIBHEIF_NUMERIC_VERSION > LIBHEIF_MAKE_VERSION(1, 20, 1)
        int stride = 0;
#else
        size_t stride = 0;
#endif
        const unsigned char *const buffer = image.get_plane(heif_channel_interleaved,&stride);
        T *ptr_r = _data, *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2), *ptr_a = S>3?data(0,0,0,3):0;
        cimg_forY(*this,y) {
          const unsigned char *ptrs = buffer + y*stride;
          if (ptr_a) cimg_forX(*this,x) { // RGBA
              *(ptr_r++) = (T)*(ptrs++);
              *(ptr_g++) = (T)*(ptrs++);
              *(ptr_b++) = (T)*(ptrs++);
              *(ptr_a++) = (T)*(ptrs++);
            }
          else cimg_forX(*this,x) { // RGB
              *(ptr_r++) = (T)*(ptrs++);
              *(ptr_g++) = (T)*(ptrs++);
              *(ptr_b++) = (T)*(ptrs++);
            }
        }
      } catch (const heif::Error& e) {
        throw CImgInstanceException(_cimg_instance
                                    "load_heif(): Unable to decode image: %s",
                                    cimg_instance,
                                    e.get_message().c_str());
      } catch (...) {
        throw;
      }
      return *this;
#endif
    }

    //! Load image from a WebP file.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_webp(const char *const filename) {
      return _load_webp(filename);
    }

    //! Load image from a WebP file \newinstance.
    static CImg<T> get_load_webp(const char *const filename) {
      return CImg<T>().load_webp(filename);
    }

    CImg<T>& _load_webp(const char *const filename) {
#ifndef cimg_use_webp
      return load_other(filename);
#else
      std::FILE *file = cimg::fopen(filename, "rb");
      const long data_size = cimg::fsize(file);
      if (data_size <= 0) {
        cimg::fclose(file);
        throw CImgIOException(_cimg_instance
                              "load_webp(): Failed to get file size '%s'.",
                              cimg_instance,
                              filename);
      }
      CImg<ucharT> buffer(data_size);
      cimg::fread(buffer._data, buffer._width, file);
      cimg::fclose(file);

      WebPDecoderConfig config;
      if (!WebPInitDecoderConfig(&config))
        throw CImgIOException(_cimg_instance
                              "load_webp(): Failed to init WebP decoder config.",
                              cimg_instance);

      if (WebPGetFeatures(buffer._data, data_size, &config.input)!=VP8_STATUS_OK)
        throw CImgIOException(_cimg_instance
                              "load_webp(): Failed to get image meta info of '%s'.",
                              cimg_instance,
                              filename);

      if (config.input.has_animation)
        throw CImgIOException(_cimg_instance
                              "load_webp(): Does not support animated WebP '%s'.",
                              cimg_instance,
                              filename);

      int width = config.input.width, height = config.input.height;
      if (config.input.has_alpha) {
        config.output.colorspace = MODE_RGBA;
        assign(width,height,1,4);
      } else {
        config.output.colorspace = MODE_RGB;
        assign(width,height,1,3);
      }
      if (WebPDecode(buffer._data, data_size, &config)!=VP8_STATUS_OK)
        throw CImgIOException(_cimg_instance
                              "load_webp(): Failed to decode image '%s'.",
                              cimg_instance,
                              filename);

      uint8_t *imgData = config.output.u.RGBA.rgba;
      T *ptr_r = _data, *ptr_g = _data + 1UL*width*height,
        *ptr_b = _data + 2UL*width*height;
      T *ptr_a = _spectrum==3 ? NULL : _data + 3UL*width*height;
      cimg_forY(*this,y) {
        const unsigned char *ptrs = (unsigned char*)&imgData[y*width*_spectrum];
        cimg_forX(*this,x) {
          *(ptr_r++) = (T)*(ptrs++);
          *(ptr_g++) = (T)*(ptrs++);
          *(ptr_b++) = (T)*(ptrs++);
          if (ptr_a) *(ptr_a++) = (T)*(ptrs++);
        }
      }
      WebPFreeDecBuffer(&config.output);
      return *this;
#endif
    }

    //! Load image using GraphicsMagick's external tool 'gm'.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_graphicsmagick_external(const char *const filename) {
      if (!filename || !cimg::is_file(filename))
        throw CImgArgumentException(_cimg_instance
                                    "load_graphicsmagick_external(): Specified filename is (null) or does not exist.",
                                    cimg_instance);
      CImg<charT> command(1024), filename_tmp(256);
      const CImg<charT> s_filename = CImg<charT>::string(filename)._system_strescape();
#if cimg_OS==1
      if (cimg::posix_searchpath("gm")) {
        cimg_snprintf(command,command._width,"%s convert \"%s\" %s:-",
                      cimg::graphicsmagick_path(),
                      s_filename.data(),
#ifdef cimg_use_png
                      "png"
#else
                      "pnm"
#endif
                      );
        std::FILE *file = popen(command,"r");
        if (file) {
          const unsigned int omode = cimg::exception_mode();
          cimg::exception_mode(0);
          try {
#ifdef cimg_use_png
            load_png(file);
#else
            load_pnm(file);
#endif
          } catch (...) {
            pclose(file);
            cimg::exception_mode(omode);
            throw CImgIOException(_cimg_instance
                                  "load_graphicsmagick_external(): Failed to load file '%s' "
                                  "with external command 'gm'.",
                                  cimg_instance,
                                  filename);
          }
          pclose(file);
          return *this;
        }
      }
#endif
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                      cimg::temporary_path(),
                      cimg_file_separator,
                      cimg::filenamerand(),
#ifdef cimg_use_png
                      "png"
#else
                      "pnm"
#endif
                      );
      } while (cimg::path_exists(filename_tmp));
      cimg_snprintf(command,command._width,"\"%s\" convert \"%s\" \"%s\"",
                    cimg::graphicsmagick_path(),
                    s_filename.data(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data());
      if (cimg::system(command,cimg::graphicsmagick_path())!=0)
        throw CImgIOException(_cimg_instance
                              "load_graphicsmagick_external(): Failed to load file '%s' with external command 'gm'.",
                              cimg_instance,
                              filename);
      if (!cimg::path_exists(filename_tmp)) {
        cimg::fclose(cimg::fopen(filename,"r"));
        throw CImgIOException(_cimg_instance
                              "load_graphicsmagick_external(): Failed to load file '%s' with external command 'gm'.",
                              cimg_instance,
                              filename);

      }
#ifdef cimg_use_png
      load_png(filename_tmp);
#else
      load_pnm(filename_tmp);
#endif
      std::remove(filename_tmp);
      return *this;
    }

    //! Load image using GraphicsMagick's external tool 'gm' \newinstance.
    static CImg<T> get_load_graphicsmagick_external(const char *const filename) {
      return CImg<T>().load_graphicsmagick_external(filename);
    }

    //! Load gzipped image file, using external tool 'gunzip'.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_gzip_external(const char *const filename) {
      if (!filename || !cimg::is_file(filename))
        throw CImgIOException(_cimg_instance
                              "load_gzip_external(): Specified filename is (null) or does not exist.",
                              cimg_instance);
      CImg<charT> command(1024), filename_tmp(256), body(256);
      const char
        *const ext = cimg::split_filename(filename,body),
        *const ext2 = cimg::split_filename(body,0);

      do {
        if (!cimg::strcasecmp(ext,"gz")) {
          if (*ext2) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                   cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext2);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        } else {
          if (*ext) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                  cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        }
      } while (cimg::path_exists(filename_tmp));
      cimg_snprintf(command,command._width,"\"%s\" -c \"%s\" > \"%s\"",
                    cimg::gunzip_path(),
                    CImg<charT>::string(filename)._system_strescape().data(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data());
      if (cimg::system(command)!=0)
        throw CImgIOException(_cimg_instance
                              "load_gzip_external(): Failed to load file '%s' with external command 'gunzip'.",
                              cimg_instance,
                              filename);
      if (!cimg::path_exists(filename_tmp)) {
        cimg::fclose(cimg::fopen(filename,"r"));
        throw CImgIOException(_cimg_instance
                              "load_gzip_external(): Failed to load file '%s' with external command 'gunzip'.",
                              cimg_instance,
                              filename);

      }
      load(filename_tmp);
      std::remove(filename_tmp);
      return *this;
    }

    //! Load gzipped image file, using external tool 'gunzip' \newinstance.
    static CImg<T> get_load_gzip_external(const char *const filename) {
      return CImg<T>().load_gzip_external(filename);
    }

    //! Load image using ImageMagick's external tool 'convert'.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_imagemagick_external(const char *const filename) {
      if (!filename || !cimg::is_file(filename))
        throw CImgArgumentException(_cimg_instance
                                    "load_imagemagick_external(): Specified filename is (null) or does not exist.",
                                    cimg_instance);
      CImg<charT> command(1024), filename_tmp(256);
      const CImg<charT> s_filename = CImg<charT>::string(filename)._system_strescape();
      const char *magick_path = cimg::imagemagick_path();
#if cimg_OS==1
      if (cimg::posix_searchpath("magick") || cimg::posix_searchpath("convert")) {
        cimg_snprintf(command,command._width,"%s%s \"%s\" %s:-",
                      magick_path,
                      !cimg::strcasecmp(cimg::split_filename(filename),"pdf")?" -density 400x400":"",
                      s_filename.data(),
#ifdef cimg_use_png
                      "png"
#else
                      "pnm"
#endif
                      );
        std::FILE *file = popen(command,"r");
        if (file) {
          const unsigned int omode = cimg::exception_mode();
          cimg::exception_mode(0);
          try {
#ifdef cimg_use_png
            load_png(file);
#else
            load_pnm(file);
#endif
          } catch (...) {
            pclose(file);
            cimg::exception_mode(omode);
            throw CImgIOException(_cimg_instance
                                  "load_imagemagick_external(): Failed to load file '%s' with "
                                  "external command 'magick/convert'.",
                                  cimg_instance,
                                  filename);
          }
          pclose(file);
          return *this;
        }
      }
#endif
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                      cimg::temporary_path(),
                      cimg_file_separator,
                      cimg::filenamerand(),
#ifdef cimg_use_png
                      "png"
#else
                      "pnm"
#endif
                      );
      } while (cimg::path_exists(filename_tmp));
      cimg_snprintf(command,command._width,"\"%s\"%s \"%s\" \"%s\"",
                    magick_path,
                    !cimg::strcasecmp(cimg::split_filename(filename),"pdf")?" -density 400x400":"",
                    s_filename.data(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data());
      if (cimg::system(command,magick_path)!=0)
        throw CImgIOException(_cimg_instance
                              "load_imagemagick_external(): Failed to load file '%s' with "
                              "external command 'magick/convert'.",
                              cimg_instance,
                              filename);
      if (!cimg::path_exists(filename_tmp)) {
        cimg::fclose(cimg::fopen(filename,"r"));
        throw CImgIOException(_cimg_instance
                              "load_imagemagick_external(): Failed to load file '%s' with "
                              "external command 'magick/convert'.",
                              cimg_instance,
                              filename);

      }
#ifdef cimg_use_png
      load_png(filename_tmp);
#else
      load_pnm(filename_tmp);
#endif
      std::remove(filename_tmp);
      return *this;
    }

    //! Load image using ImageMagick's external tool 'convert' \newinstance.
    static CImg<T> get_load_imagemagick_external(const char *const filename) {
      return CImg<T>().load_imagemagick_external(filename);
    }

    //! Load image from a DICOM file, using Medcon's external tool 'medcon'.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_medcon_external(const char *const filename) {
      if (!filename || !cimg::is_file(filename))
        throw CImgArgumentException(_cimg_instance
                                    "load_medcon_external(): Specified filename is (null) or does not exist.",
                                    cimg_instance);
      CImg<charT> command(1024), filename_tmp(256), body(256);
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s.hdr",cimg::filenamerand());
      } while (cimg::path_exists(filename_tmp));
      cimg_snprintf(command,command._width,"\"%s\" -w -c anlz -o \"%s\" -f \"%s\"",
                    cimg::medcon_path(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data(),
                    CImg<charT>::string(filename)._system_strescape().data());
      cimg::system(command,cimg::medcon_path());
      cimg::split_filename(filename_tmp,body);

      cimg_snprintf(command,command._width,"%s.hdr",body._data);
      if (!cimg::path_exists(command)) {
        cimg_snprintf(command,command._width,"m000-%s.hdr",body._data);
        if (!cimg::path_exists(command)) {
          throw CImgIOException(_cimg_instance
                                "load_medcon_external(): Failed to load file '%s' with external command 'medcon'.",
                                cimg_instance,
                                filename);
        }
      }
      load_analyze(command);
      std::remove(command);
      cimg::split_filename(command,body);
      cimg_snprintf(command,command._width,"%s.img",body._data);
      std::remove(command);
      return *this;
    }

    //! Load image from a DICOM file, using Medcon's external tool 'medcon' \newinstance.
    static CImg<T> get_load_medcon_external(const char *const filename) {
      return CImg<T>().load_medcon_external(filename);
    }

    //! Load image from a .pdf file.
    /**
       \param filename Filename, as a C-string.
       \param resolution Image resolution.
    **/
    CImg<T>& load_pdf_external(const char *const filename, const unsigned int resolution=400) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_pdf_external(): Specified filename is (null).",
                                    cimg_instance);
      CImg<charT> command(1024), filename_tmp(256);
      const CImg<charT> s_filename = CImg<charT>::string(filename)._system_strescape();
#if cimg_OS==1
      std::FILE *file = 0;
      cimg_snprintf(command,command._width,"gs -q -dNOPAUSE -sDEVICE=ppmraw -o - -r%u \"%s\"",
                    resolution,s_filename.data());
      file = popen(command,"r");
      if (file) {
        const unsigned int omode = cimg::exception_mode();
        cimg::exception_mode(0);
        try { load_pnm(file); } catch (...) {
          pclose(file);
          cimg::exception_mode(omode);
          throw CImgIOException(_cimg_instance
                                "load_pdf_external(): Failed to load file '%s' with external command 'gs'.",
                                cimg_instance,
                                filename);
        }
        pclose(file);
        return *this;
      }
#endif
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.ppm",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
      } while (cimg::path_exists(filename_tmp));
      cimg_snprintf(command,command._width,"gs -q -dNOPAUSE -sDEVICE=ppmraw -o \"%s\" -r%u \"%s\"",
                    CImg<charT>::string(filename_tmp)._system_strescape().data(),resolution,s_filename.data());
      cimg::system(command,"gs");
      if (!cimg::path_exists(filename_tmp)) {
        cimg::fclose(cimg::fopen(filename,"r"));
        throw CImgIOException(_cimg_instance
                              "load_pdf_external(): Failed to load file '%s' with external command 'gs'.",
                              cimg_instance,
                              filename);
      }
      load_pnm(filename_tmp);
      std::remove(filename_tmp);
      return *this;
    }

    //! Load image from a .pdf file \newinstance.
    static CImg<T> get_load_pdf_external(const char *const filename, const unsigned int resolution=400) {
      return CImg<T>().load_pdf_external(filename,resolution);
    }

    //! Load image from a RAW Color Camera file, using external tool 'dcraw'.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_dcraw_external(const char *const filename) {
      if (!filename || !cimg::is_file(filename))
        throw CImgArgumentException(_cimg_instance
                                    "load_dcraw_external(): Specified filename is (null) or does not exist.",
                                    cimg_instance);
      CImg<charT> command(1024), filename_tmp(256);
      const CImg<charT> s_filename = CImg<charT>::string(filename)._system_strescape();
#if cimg_OS==1
      cimg_snprintf(command,command._width,"%s -w -4 -c \"%s\"",
                    cimg::dcraw_path(),s_filename.data());
      std::FILE *file = popen(command,"r");
      if (file) {
        const unsigned int omode = cimg::exception_mode();
        cimg::exception_mode(0);
        try { load_pnm(file); } catch (...) {
          pclose(file);
          cimg::exception_mode(omode);
          throw CImgIOException(_cimg_instance
                                "load_dcraw_external(): Failed to load file '%s' with external command 'dcraw'.",
                                cimg_instance,
                                filename);
        }
        pclose(file);
        return *this;
      }
#endif
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.ppm",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
      } while (cimg::path_exists(filename_tmp));
      cimg_snprintf(command,command._width,"\"%s\" -w -4 -c \"%s\" > \"%s\"",
                    cimg::dcraw_path(),s_filename.data(),CImg<charT>::string(filename_tmp)._system_strescape().data());
      cimg::system(command,cimg::dcraw_path());
      if (!cimg::path_exists(filename_tmp)) {
        cimg::fclose(cimg::fopen(filename,"r"));
        throw CImgIOException(_cimg_instance
                              "load_dcraw_external(): Failed to load file '%s' with external command 'dcraw'.",
                              cimg_instance,
                              filename);

      }
      load_pnm(filename_tmp);
      std::remove(filename_tmp);
      return *this;
    }

    //! Load image from a RAW Color Camera file, using external tool 'dcraw' \newinstance.
    static CImg<T> get_load_dcraw_external(const char *const filename) {
      return CImg<T>().load_dcraw_external(filename);
    }

#ifdef cimg_use_opencv

    // Convert a continuous cv::Mat<uchar> to a CImg<uchar>.
    static CImg<ucharT> _cvmat2cimg(const cv::Mat &src) {
      if (src.channels()==1) return CImg<ucharT>(src.ptr(),src.cols,src.rows,1,1);
      else if (src.channels()==3) { // BGR
        CImg<ucharT> res(src.cols,src.rows,1,src.channels());
        const unsigned char *ptrs = src.ptr();
        unsigned char *pR = res.data(), *pG = res.data(0,0,0,1), *pB = res.data(0,0,0,2);
        cimg_forXY(res,x,y) { *(pB++) = *(ptrs++); *(pG++) = *(ptrs++); *(pR++) = *(ptrs++); }
        return res;
      }
      return CImg<ucharT>(src.ptr(),src.channels(),src.cols,src.rows,1,true).get_permute_axes("yzcx");
    }

    // Convert a CImg<T> to a cv::Mat.
    cv::Mat _cimg2cvmat() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "_cimg2cvmat() : Instance image is empty.",
                                    cimg_instance);
      if (_spectrum==2)
        throw CImgInstanceException(_cimg_instance
                                    "_cimg2cvmat() : Invalid number of channels (should be '1' or '3+').",
                                    cimg_instance);
      if (_depth!=1)
        throw CImgInstanceException(_cimg_instance
                                    "_cimg2cvmat() : Invalid number of slices (should be '1').",
                                    cimg_instance);
      int mat_type = -1;
      if (pixel_type()==cimg::type<unsigned char>::string()) mat_type = CV_8UC1;
      if (pixel_type()==cimg::type<char>::string()) mat_type = CV_8SC1;
      if (pixel_type()==cimg::type<unsigned short>::string()) mat_type = CV_16UC1;
      if (pixel_type()==cimg::type<short>::string()) mat_type = CV_16SC1;
      if (pixel_type()==cimg::type<int>::string()) mat_type = CV_32SC1;
      if (pixel_type()==cimg::type<float>::string()) mat_type = CV_32FC1;
      if (pixel_type()==cimg::type<double>::string()) mat_type = CV_64FC1;
      if (mat_type<0)
        throw CImgInstanceException(_cimg_instance
                                    "_cvmat2cimg() : pixel type '%s' is not supported.",
                                    cimg_instance,pixel_type());
      cv::Mat res;
      if (_spectrum>1) {
        cv::Mat *const channels = new cv::Mat[_spectrum];
        cimg_forC(*this,c)
          channels[c] = cv::Mat(_height,_width,mat_type,_data + _width*_height*(_spectrum - 1 - c));
        cv::merge(channels,_spectrum,res);
        delete[] channels;
      } else res = cv::Mat(_height,_width,mat_type,_data).clone();

      return res;
    }

#endif

    //! Load image from a camera stream, using OpenCV.
    /**
       \param index Index of the camera to capture images from (from 0 to 63).
       \param capture_width Width of the desired image ('0' stands for default value).
       \param capture_height Height of the desired image ('0' stands for default value).
       \param skip_frames Number of frames to skip before the capture.
       \param release_camera Tells if the camera resource must be released at the end of the method.
    **/
    CImg<T>& load_camera(const unsigned int camera_index=0,
                         const unsigned int capture_width=0, const unsigned int capture_height=0,
                         const unsigned int skip_frames=0, const bool release_camera=true) {
#ifdef cimg_use_opencv
      if (camera_index>=64)
        throw CImgArgumentException(_cimg_instance
                                    "load_camera(): Invalid request for camera #%u "
                                    "(no more than 100 cameras can be managed simultaneously).",
                                    cimg_instance,
                                    camera_index);
      static cv::VideoCapture *captures[64] = {};
      static unsigned int captures_w[64], captures_h[64];
      if (release_camera) {
        cimg::mutex(9);
        if (captures[camera_index]) captures[camera_index]->release();
        delete captures[camera_index];
        captures[camera_index] = 0;
        captures_w[camera_index] = captures_h[camera_index] = 0;
        cimg::mutex(9,0);
        return *this;
      }
      if (!captures[camera_index]) {
        cimg::mutex(9);
        captures[camera_index] = new cv::VideoCapture(camera_index);
        captures_w[camera_index] = captures_h[camera_index] = 0;
        if (!captures[camera_index]->isOpened()) {
          delete captures[camera_index];
          captures[camera_index] = 0;
          cimg::mutex(9,0);
          throw CImgIOException(_cimg_instance
                                "load_camera(): Failed to initialize camera #%u.",
                                cimg_instance,
                                camera_index);
        }
        cimg::mutex(9,0);
      }
      cimg::mutex(9);
      if (capture_width!=captures_w[camera_index]) {
        captures[camera_index]->set(_cimg_cap_prop_frame_width,capture_width);
        captures_w[camera_index] = capture_width;
      }
      if (capture_height!=captures_h[camera_index]) {
        captures[camera_index]->set(_cimg_cap_prop_frame_height,capture_height);
        captures_h[camera_index] = capture_height;
      }
      for (unsigned int i = 0; i<skip_frames; ++i) captures[camera_index]->grab();
      cv::Mat cvimg;
      captures[camera_index]->read(cvimg);
      if (cvimg.empty()) {
        cimg::mutex(9,0);
        load_camera(camera_index,0,0,0,true); // Release camera
        throw CImgIOException(_cimg_instance
                              "load_camera(): Failed to retrieve a %ux%u frame from camera #%u.",
                              cimg_instance,
                              capture_width,capture_height,camera_index);
      } else _cvmat2cimg(cvimg).move_to(*this);
      cimg::mutex(9,0);
      return *this;
#else
      cimg::unused(camera_index,skip_frames,release_camera,capture_width,capture_height);
      throw CImgIOException(_cimg_instance
                            "load_camera(): This function requires features from the OpenCV library "
                            "('-Dcimg_use_opencv' must be defined).",
                            cimg_instance);
#endif
    }

    //! Load image from a camera stream, using OpenCV \newinstance.
    static CImg<T> get_load_camera(const unsigned int camera_index=0,
                                   const unsigned int capture_width=0, const unsigned int capture_height=0,
                                   const unsigned int skip_frames=0, const bool release_camera=true) {
      return CImg<T>().load_camera(camera_index,capture_width,capture_height,skip_frames,release_camera);
    }

    //! Load image using various non-native ways.
    /**
       \param filename Filename, as a C-string.
    **/
    CImg<T>& load_other(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_other(): Specified filename is (null).",
                                    cimg_instance);

      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      try { load_magick(filename); }
      catch (CImgException&) {
        try { load_imagemagick_external(filename); }
        catch (CImgException&) {
          try { load_graphicsmagick_external(filename); }
          catch (CImgException&) {
            try { load_cimg(filename); }
            catch (CImgException&) {
              try {
                cimg::fclose(cimg::fopen(filename,"rb"));
              } catch (CImgException&) {
                cimg::exception_mode(omode);
                throw CImgIOException(_cimg_instance
                                      "load_other(): Failed to open file '%s'.",
                                      cimg_instance,
                                      filename);
              }
              cimg::exception_mode(omode);
              throw CImgIOException(_cimg_instance
                                    "load_other(): Failed to recognize format of file '%s'.",
                                    cimg_instance,
                                    filename);
            }
          }
        }
      }
      cimg::exception_mode(omode);
      return *this;
    }

    //! Load image using various non-native ways \newinstance.
    static CImg<T> get_load_other(const char *const filename) {
      return CImg<T>().load_other(filename);
    }

    //@}
    //---------------------------
    //
    //! \name Data Output
    //@{
    //---------------------------

    //! Display information about the image data.
    /**
       \param title Name for the considered image.
       \param display_stats Tells to compute and display image statistics.
    **/
    const CImg<T>& print(const char *const title=0, const bool display_stats=true) const {

      int xm = 0, ym = 0, zm = 0, vm = 0, xM = 0, yM = 0, zM = 0, vM = 0;
      CImg<doubleT> st;
      if (!is_empty() && display_stats) {
        st = get_stats();
        xm = (int)st[4]; ym = (int)st[5], zm = (int)st[6], vm = (int)st[7];
        xM = (int)st[8]; yM = (int)st[9], zM = (int)st[10], vM = (int)st[11];
      }

      const ulongT siz = size(), msiz = siz*sizeof(T), siz1 = siz - 1,
        mdisp = msiz<8*1024?0U:msiz<8*1024*1024?1U:2U, width1 = _width - 1;

      CImg<charT> _title(64);
      if (!title) cimg_snprintf(_title,_title._width,"CImg<%s>",pixel_type());

      std::fprintf(cimg::output(),"%s%s%s%s: %sthis%s = %p, %ssize%s = (%u,%u,%u,%u) [%lu %s], %sdata%s = (%s*)%p",
                   cimg::t_magenta,cimg::t_bold,title?title:_title._data,cimg::t_normal,
                   cimg::t_bold,cimg::t_normal,(void*)this,
                   cimg::t_bold,cimg::t_normal,_width,_height,_depth,_spectrum,
                   (unsigned long)(mdisp==0?msiz:(mdisp==1?(msiz>>10):(msiz>>20))),
                   mdisp==0?"b":(mdisp==1?"Kio":"Mio"),
                   cimg::t_bold,cimg::t_normal,pixel_type(),(void*)begin());
      if (_data)
        std::fprintf(cimg::output(),"..%p (%s) = [ ",(void*)((char*)end() - 1),_is_shared?"shared":"non-shared");
      else std::fprintf(cimg::output()," (%s) = [ ",_is_shared?"shared":"non-shared");

      if (!is_empty()) cimg_foroff(*this,off) {
        std::fprintf(cimg::output(),cimg::type<T>::format_s(),cimg::type<T>::format(_data[off]));
        if (off!=siz1) std::fprintf(cimg::output(),"%s",off%_width==width1?" ; ":" ");
        if (off==7 && siz>16) { off = siz1 - 8; std::fprintf(cimg::output(),"... "); }
      }
      if (!is_empty() && display_stats)
        std::fprintf(cimg::output(),
                     " ], %smin%s = %g, %smax%s = %g, %smean%s = %g, %sstd%s = %g, %scoords_min%s = (%u,%u,%u,%u), "
                     "%scoords_max%s = (%u,%u,%u,%u).\n",
                     cimg::t_bold,cimg::t_normal,st[0],
                     cimg::t_bold,cimg::t_normal,st[1],
                     cimg::t_bold,cimg::t_normal,st[2],
                     cimg::t_bold,cimg::t_normal,std::sqrt(st[3]),
                     cimg::t_bold,cimg::t_normal,xm,ym,zm,vm,
                     cimg::t_bold,cimg::t_normal,xM,yM,zM,vM);
      else std::fprintf(cimg::output(),"%s].\n",is_empty()?"":" ");
      std::fflush(cimg::output());
      return *this;
    }

    //! Display image into a CImgDisplay window.
    /**
       \param disp Display window.
    **/
    const CImg<T>& display(CImgDisplay& disp) const {
      disp.display(*this);
      return *this;
    }

    //! Display image into a CImgDisplay window, in an interactive way.
    /**
        \param disp Display window.
        \param display_info Tells if image information are displayed on the standard output.
        \param[in,out] XYZ Contains the XYZ coordinates at start / exit of the function.
        \param exit_on_anykey Exit function when any key is pressed.
    **/
    const CImg<T>& display(CImgDisplay &disp, const bool display_info, unsigned int *const XYZ=0,
                           const bool exit_on_anykey=false) const {
      return _display(disp,0,display_info,XYZ,exit_on_anykey,false);
    }

    //! Display image into an interactive window.
    /**
        \param title Window title
        \param display_info Tells if image information are displayed on the standard output.
        \param[in,out] XYZ Contains the XYZ coordinates at start / exit of the function.
        \param exit_on_anykey Exit function when any key is pressed.
    **/
    const CImg<T>& display(const char *const title=0, const bool display_info=true, unsigned int *const XYZ=0,
                           const bool exit_on_anykey=false) const {
      CImgDisplay disp;
      return _display(disp,title,display_info,XYZ,exit_on_anykey,false);
    }

    const CImg<T>& _display(CImgDisplay &disp, const char *const title, const bool display_info,
                            unsigned int *const XYZ, const bool exit_on_anykey,
                            const bool exit_on_singleclick) const {
      unsigned int oldw = 0, oldh = 0, _XYZ[3] = {}, key = 0;
      int x0 = 0, y0 = 0, z0 = 0, x1 = width() - 1, y1 = height() - 1, z1 = depth() - 1,
        old_mouse_x = -1, old_mouse_y = -1;

      if (!disp) {
        disp.assign(cimg_fitscreen(_width,_height,_depth),title?title:0,1);
        if (!title) disp.set_title("CImg<%s> (%ux%ux%ux%u)",pixel_type(),_width,_height,_depth,_spectrum);
        else disp.set_title("%s",title);
      } else if (title) disp.set_title("%s",title);

      disp.show().flush();
      const CImg<char> dtitle = CImg<char>::string(disp.title());
      if (display_info) print(dtitle);

      CImg<T> zoom;
      for (bool reset_view = true, resize_disp = false, is_first_select = true; !key && !disp.is_closed(); ) {
        if (reset_view) {
          if (XYZ) { _XYZ[0] = XYZ[0]; _XYZ[1] = XYZ[1]; _XYZ[2] = XYZ[2]; }
          else {
            _XYZ[0] = (unsigned int)(x0 + x1 + 1)/2;
            _XYZ[1] = (unsigned int)(y0 + y1 + 1)/2;
            _XYZ[2] = (unsigned int)(z0 + z1 + 1)/2;
          }
          x0 = 0; y0 = 0; z0 = 0; x1 = width() - 1; y1 = height() - 1; z1 = depth() - 1;
          disp.resize(cimg_fitscreen(_width,_height,_depth),false);
          oldw = disp._width; oldh = disp._height;
          resize_disp = true;
          reset_view = false;
        }
        if (!x0 && !y0 && !z0 && x1==width() - 1 && y1==height() - 1 && z1==depth() - 1) {
          if (is_empty()) zoom.assign(1,1,1,1,(T)0); else zoom.assign();
        } else zoom = get_crop(x0,y0,z0,x1,y1,z1);

        const CImg<T>& visu = zoom?zoom:*this;
        const unsigned int
          dx = 1U + x1 - x0, dy = 1U + y1 - y0, dz = 1U + z1 - z0,
          tw = dx + (dz>1?dz:0U), th = dy + (dz>1?dz:0U);
        if (!is_empty() && !disp.is_fullscreen() && resize_disp) {
          const float
            ttw = (float)tw*disp.width()/oldw, tth = (float)th*disp.height()/oldh,
            dM = std::max(ttw,tth), diM = (float)std::max(disp.width(),disp.height());
          const unsigned int
            imgw = (unsigned int)(ttw*diM/dM), imgh = (unsigned int)(tth*diM/dM);
          disp.set_fullscreen(false).resize(cimg_fitscreen(imgw,imgh,1),false);
          resize_disp = false;
        }
        oldw = tw; oldh = th;

        bool
          go_up = false, go_down = false, go_left = false, go_right = false,
          go_inc = false, go_dec = false, go_in = false, go_out = false,
          go_in_center = false;

        disp.set_title("%s",dtitle._data);
        if (_width>1 && visu._width==1) disp.set_title("%s | x=%u",disp._title,x0);
        if (_height>1 && visu._height==1) disp.set_title("%s | y=%u",disp._title,y0);
        if (_depth>1 && visu._depth==1) disp.set_title("%s | z=%u",disp._title,z0);

        disp._mouse_x = old_mouse_x; disp._mouse_y = old_mouse_y;
        CImg<intT> selection = visu._select(disp,0,2,_XYZ,x0,y0,z0,true,is_first_select,_depth>1,true);
        old_mouse_x = disp._mouse_x; old_mouse_y = disp._mouse_y;
        is_first_select = false;

        if (disp.wheel()) {
          if ((disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) &&
              (disp.is_keySHIFTLEFT() || disp.is_keySHIFTRIGHT())) {
            go_left = !(go_right = disp.wheel()>0);
          } else if (disp.is_keySHIFTLEFT() || disp.is_keySHIFTRIGHT()) {
            go_down = !(go_up = disp.wheel()>0);
          } else if (depth()==1 || disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            go_out = !(go_in = disp.wheel()>0); go_in_center = false;
          }
          disp.set_wheel();
        }

        const int
          sx0 = selection(0), sy0 = selection(1), sz0 = selection(2),
          sx1 = selection(3), sy1 = selection(4), sz1 = selection(5);
        if (sx0>=0 && sy0>=0 && sz0>=0 && sx1>=0 && sy1>=0 && sz1>=0) {
          x1 = x0 + sx1; y1 = y0 + sy1; z1 = z0 + sz1;
          x0+=sx0; y0+=sy0; z0+=sz0;
          if ((sx0==sx1 && sy0==sy1) || (_depth>1 && sx0==sx1 && sz0==sz1) || (_depth>1 && sy0==sy1 && sz0==sz1)) {
            if (exit_on_singleclick && (!zoom || is_empty())) break; else reset_view = true;
          }
          resize_disp = true;
        } else switch (key = disp.key()) {
#if cimg_OS!=2
          case cimg::keyCTRLRIGHT : case cimg::keySHIFTRIGHT :
#endif
          case 0 : case cimg::keyCTRLLEFT : case cimg::keySHIFTLEFT : key = 0; break;
          case cimg::keyP : if (visu._depth>1 && (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT())) {
              // Special mode: play stack of frames.
              const unsigned int
                w1 = visu._width*disp.width()/(visu._width + (visu._depth>1?visu._depth:0)),
                h1 = visu._height*disp.height()/(visu._height + (visu._depth>1?visu._depth:0));
              float frame_timing = 5;
              bool is_stopped = false;
              disp.set_key(key,false).set_wheel().resize(cimg_fitscreen(w1,h1,1),false); key = 0;
              for (unsigned int timer = 0; !key && !disp.is_closed() && !disp.button(); ) {
                if (disp.is_resized()) disp.resize(false);
                if (!timer) {
                  visu.get_slice((int)_XYZ[2]).display(disp.set_title("%s | z=%d",dtitle.data(),_XYZ[2]));
                  (++_XYZ[2])%=visu._depth;
                }
                if (!is_stopped) { if (++timer>(unsigned int)frame_timing) timer = 0; } else timer = ~0U;
                if (disp.wheel()) { frame_timing-=disp.wheel()/3.f; disp.set_wheel(); }
                switch (key = disp.key()) {
#if cimg_OS!=2
                case cimg::keyCTRLRIGHT :
#endif
                case cimg::keyCTRLLEFT : key = 0; break;
                case cimg::keyPAGEUP : frame_timing-=0.3f; key = 0; break;
                case cimg::keyPAGEDOWN : frame_timing+=0.3f; key = 0; break;
                case cimg::keySPACE : is_stopped = !is_stopped; disp.set_key(key,false); key = 0; break;
                case cimg::keyARROWLEFT : case cimg::keyARROWUP : is_stopped = true; timer = 0; key = 0; break;
                case cimg::keyARROWRIGHT : case cimg::keyARROWDOWN : is_stopped = true;
                  (_XYZ[2]+=visu._depth - 2)%=visu._depth; timer = 0; key = 0; break;
                case cimg::keyD : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
                    disp.set_fullscreen(false).
                      resize(CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,false),
                             CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,true),false);
                    disp.set_key(key,false); key = 0;
                  } break;
                case cimg::keyC : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
                    disp.set_fullscreen(false).
                      resize(cimg_fitscreen(2*disp.width()/3,2*disp.height()/3,1),false).set_key(key,false); key = 0;
                  } break;
                case cimg::keyR : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
                    disp.set_fullscreen(false).
                      resize(cimg_fitscreen(_width,_height,_depth),false).set_key(key,false); key = 0;
                  } break;
                case cimg::keyF : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
                    disp.resize(disp.screen_width(),disp.screen_height(),false).
                      toggle_fullscreen().set_key(key,false); key = 0;
                  } break;
                }
                frame_timing = frame_timing<1?1:(frame_timing>39?39:frame_timing);
                disp.wait(20);
              }
              const unsigned int
                w2 = (visu._width + (visu._depth>1?visu._depth:0))*disp.width()/visu._width,
                h2 = (visu._height + (visu._depth>1?visu._depth:0))*disp.height()/visu._height;
              disp.resize(cimg_fitscreen(w2,h2,1),false).set_title(dtitle.data()).set_key().set_button().set_wheel();
              key = 0;
            } break;
          case cimg::keyHOME : reset_view = resize_disp = true; key = 0; break;
          case cimg::keyPADADD : go_in = true; go_in_center = true; key = 0; break;
          case cimg::keyPADSUB : go_out = true; key = 0; break;
          case cimg::keyARROWLEFT : case cimg::keyPAD4: go_left = true; key = 0; break;
          case cimg::keyARROWRIGHT : case cimg::keyPAD6: go_right = true; key = 0; break;
          case cimg::keyARROWUP : case cimg::keyPAD8: go_up = true; key = 0; break;
          case cimg::keyARROWDOWN : case cimg::keyPAD2: go_down = true; key = 0; break;
          case cimg::keyPAD7 : go_up = go_left = true; key = 0; break;
          case cimg::keyPAD9 : go_up = go_right = true; key = 0; break;
          case cimg::keyPAD1 : go_down = go_left = true; key = 0; break;
          case cimg::keyPAD3 : go_down = go_right = true; key = 0; break;
          case cimg::keyPAGEUP : go_inc = true; key = 0; break;
          case cimg::keyPAGEDOWN : go_dec = true; key = 0; break;
          }
        if (go_in) {
          const int
            mx = go_in_center?disp.width()/2:disp.mouse_x(),
            my = go_in_center?disp.height()/2:disp.mouse_y(),
            mX = mx*(width() + (depth()>1?depth():0))/disp.width(),
            mY = my*(height() + (depth()>1?depth():0))/disp.height();
          int X = (int)_XYZ[0], Y = (int)_XYZ[1], Z = (int)_XYZ[2];
          if (mX<width() && mY<height())  {
            X = x0 + mX*(1 + x1 - x0)/width(); Y = y0 + mY*(1 + y1 - y0)/height();
          }
          if (mX<width() && mY>=height()) {
            X = x0 + mX*(1 + x1 - x0)/width(); Z = z0 + (mY - height())*(1 + z1 - z0)/depth();
          }
          if (mX>=width() && mY<height()) {
            Y = y0 + mY*(1 + y1 - y0)/height(); Z = z0 + (mX - width())*(1 + z1 - z0)/depth();
          }
          if (x1 - x0>4) { x0 = X - 3*(X - x0)/4; x1 = X + 3*(x1 - X)/4; }
          if (y1 - y0>4) { y0 = Y - 3*(Y - y0)/4; y1 = Y + 3*(y1 - Y)/4; }
          if (z1 - z0>4) { z0 = Z - 3*(Z - z0)/4; z1 = Z + 3*(z1 - Z)/4; }
        }
        if (go_out) {
          const int
            delta_x = (x1 - x0)/8, delta_y = (y1 - y0)/8, delta_z = (z1 - z0)/8,
            ndelta_x = delta_x?delta_x:(_width>1),
            ndelta_y = delta_y?delta_y:(_height>1),
            ndelta_z = delta_z?delta_z:(_depth>1);
          x0-=ndelta_x; y0-=ndelta_y; z0-=ndelta_z;
          x1+=ndelta_x; y1+=ndelta_y; z1+=ndelta_z;
          if (x0<0) { x1-=x0; x0 = 0; if (x1>=width()) x1 = width() - 1; }
          if (y0<0) { y1-=y0; y0 = 0; if (y1>=height()) y1 = height() - 1; }
          if (z0<0) { z1-=z0; z0 = 0; if (z1>=depth()) z1 = depth() - 1; }
          if (x1>=width()) { x0-=(x1 - width() + 1); x1 = width() - 1; if (x0<0) x0 = 0; }
          if (y1>=height()) { y0-=(y1 - height() + 1); y1 = height() - 1; if (y0<0) y0 = 0; }
          if (z1>=depth()) { z0-=(z1 - depth() + 1); z1 = depth() - 1; if (z0<0) z0 = 0; }
          const float
            ratio = (float)(x1-x0)/(y1-y0),
            ratiow = (float)disp._width/disp._height,
            sub = std::min(cimg::abs(ratio - ratiow),cimg::abs(1/ratio-1/ratiow));
          if (sub>0.01) resize_disp = true;
        }
        if (go_left) {
          const int delta = (x1 - x0)/4, ndelta = delta?delta:(_width>1);
          if (x0 - ndelta>=0) { x0-=ndelta; x1-=ndelta; }
          else { x1-=x0; x0 = 0; }
        }
        if (go_right) {
          const int delta = (x1 - x0)/4, ndelta = delta?delta:(_width>1);
          if (x1+ndelta<width()) { x0+=ndelta; x1+=ndelta; }
          else { x0+=(width() - 1 - x1); x1 = width() - 1; }
        }
        if (go_up) {
          const int delta = (y1 - y0)/4, ndelta = delta?delta:(_height>1);
          if (y0 - ndelta>=0) { y0-=ndelta; y1-=ndelta; }
          else { y1-=y0; y0 = 0; }
        }
        if (go_down) {
          const int delta = (y1 - y0)/4, ndelta = delta?delta:(_height>1);
          if (y1+ndelta<height()) { y0+=ndelta; y1+=ndelta; }
          else { y0+=(height() - 1 - y1); y1 = height() - 1; }
        }
        if (go_inc) {
          const int delta = (z1 - z0)/4, ndelta = delta?delta:(_depth>1);
          if (z0 - ndelta>=0) { z0-=ndelta; z1-=ndelta; }
          else { z1-=z0; z0 = 0; }
        }
        if (go_dec) {
          const int delta = (z1 - z0)/4, ndelta = delta?delta:(_depth>1);
          if (z1+ndelta<depth()) { z0+=ndelta; z1+=ndelta; }
          else { z0+=(depth() - 1 - z1); z1 = depth() - 1; }
        }
        disp.wait(100);
        if (!exit_on_anykey && key && key!=cimg::keyESC &&
            (key!=cimg::keyW || (!disp.is_keyCTRLLEFT() && !disp.is_keyCTRLRIGHT()))) {
          key = 0;
        }
      }
      disp.set_key(key);
      if (XYZ) { XYZ[0] = _XYZ[0]; XYZ[1] = _XYZ[1]; XYZ[2] = _XYZ[2]; }
      return *this;
    }

    //! Display object 3D in an interactive window.
    /**
       \param disp Display window.
       \param vertices Vertices data of the 3D object.
       \param primitives Primitives data of the 3D object.
       \param colors Colors data of the 3D object.
       \param opacities Opacities data of the 3D object.
       \param centering Tells if the 3D object must be centered for the display.
       \param render_static Rendering mode.
       \param render_motion Rendering mode, when the 3D object is moved.
       \param is_double_sided Tells if the object primitives are double-sided.
       \param focale Focale
       \param light_x X-coordinate of the light source.
       \param light_y Y-coordinate of the light source.
       \param light_z Z-coordinate of the light source.
       \param specular_lightness Amount of specular light.
       \param specular_shininess Shininess of the object material.
       \param display_axes Tells if the 3D axes are displayed.
       \param pose_matrix Pointer to 12 values, defining a 3D pose (as a 4x3 matrix).
       \param exit_on_anykey Exit function when any key is pressed.
    **/
    template<typename tp, typename tf, typename tc, typename to>
    const CImg<T>& display_object3d(CImgDisplay& disp,
                                    const CImg<tp>& vertices,
                                    const CImgList<tf>& primitives,
                                    const CImgList<tc>& colors,
                                    const to& opacities,
                                    const bool centering=true,
                                    const int render_static=4, const int render_motion=1,
                                    const bool is_double_sided=false, const float focale=700,
                                    const float light_x=0, const float light_y=0, const float light_z=-5e8f,
                                    const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                                    const bool display_axes=true, float *const pose_matrix=0,
                                    const bool exit_on_anykey=false) const {
      return _display_object3d(disp,0,vertices,primitives,colors,opacities,centering,render_static,
                               render_motion,is_double_sided,focale,
                               light_x,light_y,light_z,specular_lightness,specular_shininess,
                               display_axes,pose_matrix,exit_on_anykey);
    }

    //! Display object 3D in an interactive window \simplification.
    template<typename tp, typename tf, typename tc, typename to>
    const CImg<T>& display_object3d(const char *const title,
                                    const CImg<tp>& vertices,
                                    const CImgList<tf>& primitives,
                                    const CImgList<tc>& colors,
                                    const to& opacities,
                                    const bool centering=true,
                                    const int render_static=4, const int render_motion=1,
                                    const bool is_double_sided=false, const float focale=700,
                                    const float light_x=0, const float light_y=0, const float light_z=-5e8f,
                                    const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                                    const bool display_axes=true, float *const pose_matrix=0,
                                    const bool exit_on_anykey=false) const {
      CImgDisplay disp;
      return _display_object3d(disp,title,vertices,primitives,colors,opacities,centering,render_static,
                               render_motion,is_double_sided,focale,
                               light_x,light_y,light_z,specular_lightness,specular_shininess,
                               display_axes,pose_matrix,exit_on_anykey);
    }

    //! Display object 3D in an interactive window \simplification.
    template<typename tp, typename tf, typename tc>
    const CImg<T>& display_object3d(CImgDisplay &disp,
                                    const CImg<tp>& vertices,
                                    const CImgList<tf>& primitives,
                                    const CImgList<tc>& colors,
                                    const bool centering=true,
                                    const int render_static=4, const int render_motion=1,
                                    const bool is_double_sided=false, const float focale=700,
                                    const float light_x=0, const float light_y=0, const float light_z=-5e8f,
                                    const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                                    const bool display_axes=true, float *const pose_matrix=0,
                                    const bool exit_on_anykey=false) const {
      return display_object3d(disp,vertices,primitives,colors,CImgList<floatT>(),centering,
                              render_static,render_motion,is_double_sided,focale,
                              light_x,light_y,light_z,specular_lightness,specular_shininess,
                              display_axes,pose_matrix,exit_on_anykey);
    }

    //! Display object 3D in an interactive window \simplification.
    template<typename tp, typename tf, typename tc>
    const CImg<T>& display_object3d(const char *const title,
                                    const CImg<tp>& vertices,
                                    const CImgList<tf>& primitives,
                                    const CImgList<tc>& colors,
                                    const bool centering=true,
                                    const int render_static=4, const int render_motion=1,
                                    const bool is_double_sided=false, const float focale=700,
                                    const float light_x=0, const float light_y=0, const float light_z=-5e8f,
                                    const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                                    const bool display_axes=true, float *const pose_matrix=0,
                                    const bool exit_on_anykey=false) const {
      return display_object3d(title,vertices,primitives,colors,CImgList<floatT>(),centering,
                              render_static,render_motion,is_double_sided,focale,
                              light_x,light_y,light_z,specular_lightness,specular_shininess,
                              display_axes,pose_matrix,exit_on_anykey);
    }

    //! Display object 3D in an interactive window \simplification.
    template<typename tp, typename tf>
    const CImg<T>& display_object3d(CImgDisplay &disp,
                                    const CImg<tp>& vertices,
                                    const CImgList<tf>& primitives,
                                    const bool centering=true,
                                    const int render_static=4, const int render_motion=1,
                                    const bool is_double_sided=false, const float focale=700,
                                    const float light_x=0, const float light_y=0, const float light_z=-5e8f,
                                    const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                                    const bool display_axes=true, float *const pose_matrix=0,
                                    const bool exit_on_anykey=false) const {
      return display_object3d(disp,vertices,primitives,CImgList<T>(),centering,
                              render_static,render_motion,is_double_sided,focale,
                              light_x,light_y,light_z,specular_lightness,specular_shininess,
                              display_axes,pose_matrix,exit_on_anykey);
    }


    //! Display object 3D in an interactive window \simplification.
    template<typename tp, typename tf>
    const CImg<T>& display_object3d(const char *const title,
                                    const CImg<tp>& vertices,
                                    const CImgList<tf>& primitives,
                                    const bool centering=true,
                                    const int render_static=4, const int render_motion=1,
                                    const bool is_double_sided=false, const float focale=700,
                                    const float light_x=0, const float light_y=0, const float light_z=-5e8f,
                                    const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                                    const bool display_axes=true, float *const pose_matrix=0,
                                    const bool exit_on_anykey=false) const {
      return display_object3d(title,vertices,primitives,CImgList<T>(),centering,
                              render_static,render_motion,is_double_sided,focale,
                              light_x,light_y,light_z,specular_lightness,specular_shininess,
                              display_axes,pose_matrix,exit_on_anykey);
    }

    //! Display object 3D in an interactive window \simplification.
    template<typename tp>
    const CImg<T>& display_object3d(CImgDisplay &disp,
                                    const CImg<tp>& vertices,
                                    const bool centering=true,
                                    const int render_static=4, const int render_motion=1,
                                    const bool is_double_sided=false, const float focale=700,
                                    const float light_x=0, const float light_y=0, const float light_z=-5e8f,
                                    const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                                    const bool display_axes=true, float *const pose_matrix=0,
                                    const bool exit_on_anykey=false) const {
      return display_object3d(disp,vertices,CImgList<uintT>(),centering,
                              render_static,render_motion,is_double_sided,focale,
                              light_x,light_y,light_z,specular_lightness,specular_shininess,
                              display_axes,pose_matrix,exit_on_anykey);
    }

    //! Display object 3D in an interactive window \simplification.
    template<typename tp>
    const CImg<T>& display_object3d(const char *const title,
                                    const CImg<tp>& vertices,
                                    const bool centering=true,
                                    const int render_static=4, const int render_motion=1,
                                    const bool is_double_sided=false, const float focale=700,
                                    const float light_x=0, const float light_y=0, const float light_z=-5e8f,
                                    const float specular_lightness=0.25f, const float specular_shininess=0.1f,
                                    const bool display_axes=true, float *const pose_matrix=0,
                                    const bool exit_on_anykey=false) const {
      return display_object3d(title,vertices,CImgList<uintT>(),centering,
                              render_static,render_motion,is_double_sided,focale,
                              light_x,light_y,light_z,specular_lightness,specular_shininess,
                              display_axes,pose_matrix,exit_on_anykey);
    }

    template<typename tp, typename tf, typename tc, typename to>
    const CImg<T>& _display_object3d(CImgDisplay& disp, const char *const title,
                                     const CImg<tp>& vertices,
                                     const CImgList<tf>& primitives,
                                     const CImgList<tc>& colors,
                                     const to& opacities,
                                     const bool centering,
                                     const int render_static, const int render_motion,
                                     const bool is_double_sided, const float focale,
                                     const float light_x, const float light_y, const float light_z,
                                     const float specular_lightness, const float specular_shininess,
                                     const bool display_axes, float *const pose_matrix,
                                     const bool exit_on_anykey) const {
      // Check input arguments.
      if (is_empty()) {
        CImg<T> background;
        if (colors && colors[0].size()==1) background.assign(1,2,1,1,64,128);
        else background.assign(1,2,1,3,32,64,32,116,64,96);
        if (disp) background.resize(disp.width(),disp.height(),1,-100,3);
        else background.resize(cimg_fitscreen(CImgDisplay::screen_width()/2,
                                              CImgDisplay::screen_height()/2,1),1,-100,3);
        return background._display_object3d(disp,title,vertices,primitives,colors,opacities,centering,
                                            render_static,render_motion,is_double_sided,focale,
                                            light_x,light_y,light_z,specular_lightness,specular_shininess,
                                            display_axes,pose_matrix,exit_on_anykey);
      } else { if (disp) disp.resize(*this,false); }
      CImg<charT> error_message(1024);
      if (!vertices.is_object3d(primitives,colors,opacities,true,error_message))
        throw CImgArgumentException(_cimg_instance
                                    "display_object3d(): Invalid specified 3D object (%u,%u) (%s).",
                                    cimg_instance,vertices._width,primitives._width,error_message.data());
      if (vertices._width && !primitives) {
        CImgList<tf> nprimitives(vertices._width,1,1,1,1);
        cimglist_for(nprimitives,l) nprimitives(l,0) = (tf)l;
        return _display_object3d(disp,title,vertices,nprimitives,colors,opacities,centering,
                                 render_static,render_motion,is_double_sided,focale,
                                 light_x,light_y,light_z,specular_lightness,specular_shininess,
                                 display_axes,pose_matrix,exit_on_anykey);
      }
      if (!disp) {
        disp.assign(cimg_fitscreen(_width,_height,_depth),title?title:0,3);
        if (!title) disp.set_title("CImg<%s> (%u vertices, %u primitives)",
                                   pixel_type(),vertices._width,primitives._width);
      } else if (title) disp.set_title("%s",title);

      // Init 3D objects and compute object statistics.
      CImg<floatT>
        pose,
        rotated_vertices(vertices._width,3),
        bbox_vertices, rotated_bbox_vertices,
        axes_vertices, rotated_axes_vertices,
        bbox_opacities, axes_opacities;
      CImgList<uintT> bbox_primitives, axes_primitives;
      CImgList<tf> reverse_primitives;
      CImgList<T> bbox_colors, bbox_colors2, axes_colors;
      unsigned int ns_width = 0, ns_height = 0;
      int _is_double_sided = (int)is_double_sided;
      bool ndisplay_axes = display_axes;
      const CImg<T>
        background_color(1,1,1,_spectrum,0),
        foreground_color(1,1,1,_spectrum,(T)std::min((int)cimg::type<T>::max(),255));
      float
        Xoff = 0, Yoff = 0, Zoff = 0, sprite_scale = 1,
        xm = 0, xM = vertices?vertices.get_shared_row(0).max_min(xm):0,
        ym = 0, yM = vertices?vertices.get_shared_row(1).max_min(ym):0,
        zm = 0, zM = vertices?vertices.get_shared_row(2).max_min(zm):0;
      const float delta = cimg::max(xM - xm,yM - ym,zM - zm);

      rotated_bbox_vertices = bbox_vertices.assign(8,3,1,1,
                                                   xm,xM,xM,xm,xm,xM,xM,xm,
                                                   ym,ym,yM,yM,ym,ym,yM,yM,
                                                   zm,zm,zm,zm,zM,zM,zM,zM);
      bbox_primitives.assign(6,1,4,1,1, 0,3,2,1, 4,5,6,7, 1,2,6,5, 0,4,7,3, 0,1,5,4, 2,3,7,6);
      bbox_colors.assign(6,_spectrum,1,1,1,background_color[0]);
      bbox_colors2.assign(6,_spectrum,1,1,1,foreground_color[0]);
      bbox_opacities.assign(bbox_colors._width,1,1,1,0.3f);

      rotated_axes_vertices = axes_vertices.assign(7,3,1,1,
                                                   0,20,0,0,22,-6,-6,
                                                   0,0,20,0,-6,22,-6,
                                                   0,0,0,20,0,0,22);
      axes_opacities.assign(3,1,1,1,1);
      axes_colors.assign(3,_spectrum,1,1,1,foreground_color[0]);
      axes_primitives.assign(3,1,2,1,1, 0,1, 0,2, 0,3);

      // Begin user interaction loop.
      CImg<T> visu0(*this,false), visu;
      CImg<floatT> zbuffer(visu0.width(),visu0.height(),1,1,0);
      bool init_pose = true, clicked = false, redraw = true;
      unsigned int key = 0, font_size = 32;
      int
        x0 = 0, y0 = 0, x1 = 0, y1 = 0,
        nrender_static = render_static,
        nrender_motion = render_motion;
      disp.show().flush();

      while (!disp.is_closed() && !key) {

        // Init object pose.
        if (init_pose) {
          const float
            ratio = delta>0?(2.f*std::min(disp.width(),disp.height())/(3.f*delta)):1,
            dx = (xM + xm)/2, dy = (yM + ym)/2, dz = (zM + zm)/2;
          if (centering)
            CImg<floatT>(4,3,1,1, ratio,0.,0.,-ratio*dx, 0.,ratio,0.,-ratio*dy, 0.,0.,ratio,-ratio*dz).move_to(pose);
          else CImg<floatT>(4,3,1,1, 1,0,0,0, 0,1,0,0, 0,0,1,0).move_to(pose);
          if (pose_matrix) {
            CImg<floatT> pose0(pose_matrix,4,3,1,1,false);
            pose0.resize(4,4,1,1,0); pose.resize(4,4,1,1,0);
            pose0(3,3) = pose(3,3) = 1;
            (pose0*pose).get_crop(0,0,3,2).move_to(pose);
            Xoff = pose_matrix[12]; Yoff = pose_matrix[13]; Zoff = pose_matrix[14]; sprite_scale = pose_matrix[15];
          } else { Xoff = Yoff = Zoff = 0; sprite_scale = 1; }
          init_pose = false;
          redraw = true;
        }

        // Rotate and draw 3D object.
        if (redraw) {
          const float
            r00 = pose(0,0), r10 = pose(1,0), r20 = pose(2,0), r30 = pose(3,0),
            r01 = pose(0,1), r11 = pose(1,1), r21 = pose(2,1), r31 = pose(3,1),
            r02 = pose(0,2), r12 = pose(1,2), r22 = pose(2,2), r32 = pose(3,2);
          if ((clicked && nrender_motion>=0) || (!clicked && nrender_static>=0)) {
            const tp *const pv0 = vertices.data(), *const pv1 = vertices.data(0,1), *const pv2 = vertices.data(0,2);
            float
              *const prv0 = rotated_vertices.data(),
              *const prv1 = rotated_vertices.data(0,1),
              *const prv2 = rotated_vertices.data(0,2);
            cimg_pragma_openmp(parallel for cimg_openmp_if(vertices.width()>(cimg_openmp_sizefactor)*1024))
            cimg_forX(vertices,l) {
              const float x = (float)pv0[l], y = (float)pv1[l], z = (float)pv2[l];
              prv0[l] = r00*x + r10*y + r20*z + r30;
              prv1[l] = r01*x + r11*y + r21*z + r31;
              prv2[l] = r02*x + r12*y + r22*z + r32;
            }
          }
          else cimg_forX(bbox_vertices,l) {
              const float x = bbox_vertices(l,0), y = bbox_vertices(l,1), z = bbox_vertices(l,2);
              rotated_bbox_vertices(l,0) = r00*x + r10*y + r20*z + r30;
              rotated_bbox_vertices(l,1) = r01*x + r11*y + r21*z + r31;
              rotated_bbox_vertices(l,2) = r02*x + r12*y + r22*z + r32;
            }

          // Draw objects.
          const bool render_with_zbuffer = !clicked && nrender_static>0;
          visu = visu0;
          if ((clicked && nrender_motion<0) || (!clicked && nrender_static<0))
            visu.draw_object3d(Xoff + visu._width/2.f,Yoff + visu._height/2.f,Zoff,
                               rotated_bbox_vertices,bbox_primitives,bbox_colors,bbox_opacities,2,false,focale).
              draw_object3d(Xoff + visu._width/2.f,Yoff + visu._height/2.f,Zoff,
                            rotated_bbox_vertices,bbox_primitives,bbox_colors2,1,false,focale);
          else visu._draw_object3d(render_with_zbuffer?zbuffer.fill(0):CImg<floatT>::empty(),
                                   Xoff + visu._width/2.f,Yoff + visu._height/2.f,Zoff,
                                   rotated_vertices,reverse_primitives?reverse_primitives:primitives,
                                   colors,opacities,
                                   clicked?nrender_motion:nrender_static,
                                   _is_double_sided==1,focale,
                                   width()/2.f + light_x,height()/2.f + light_y,light_z + Zoff,
                                   specular_lightness,specular_shininess,1,sprite_scale,
                                   clicked);
          // Draw axes.
          if (ndisplay_axes) {
            const float
              n = 1e-8f + cimg::hypot(r00,r01,r02),
              _r00 = r00/n, _r10 = r10/n, _r20 = r20/n,
              _r01 = r01/n, _r11 = r11/n, _r21 = r21/n,
              _r02 = r01/n, _r12 = r12/n, _r22 = r22/n,
              Xaxes = 25, Yaxes = visu._height - 38.f;
            cimg_forX(axes_vertices,l) {
              const float
                x = axes_vertices(l,0),
                y = axes_vertices(l,1),
                z = axes_vertices(l,2);
              rotated_axes_vertices(l,0) = _r00*x + _r10*y + _r20*z;
              rotated_axes_vertices(l,1) = _r01*x + _r11*y + _r21*z;
              rotated_axes_vertices(l,2) = _r02*x + _r12*y + _r22*z;
            }
            axes_opacities(0,0) = (rotated_axes_vertices(1,2)>0)?0.5f:1.f;
            axes_opacities(1,0) = (rotated_axes_vertices(2,2)>0)?0.5f:1.f;
            axes_opacities(2,0) = (rotated_axes_vertices(3,2)>0)?0.5f:1.f;
            visu.draw_object3d(Xaxes,Yaxes,0,rotated_axes_vertices,axes_primitives,
                               axes_colors,axes_opacities,1,false,focale).
              draw_text((int)(Xaxes + rotated_axes_vertices(4,0)),
                        (int)(Yaxes + rotated_axes_vertices(4,1)),
                        "X",axes_colors[0]._data,0,axes_opacities(0,0),13).
              draw_text((int)(Xaxes + rotated_axes_vertices(5,0)),
                        (int)(Yaxes + rotated_axes_vertices(5,1)),
                        "Y",axes_colors[1]._data,0,axes_opacities(1,0),13).
              draw_text((int)(Xaxes + rotated_axes_vertices(6,0)),
                        (int)(Yaxes + rotated_axes_vertices(6,1)),
                        "Z",axes_colors[2]._data,0,axes_opacities(2,0),13);
          }
          visu.display(disp);
          if (!clicked || nrender_motion==nrender_static) redraw = false;
        }

        // Handle user interaction.
        if (!redraw) disp.wait();
        if ((disp.button() || disp.wheel()) && disp.mouse_x()>=0 && disp.mouse_y()>=0) {
          redraw = true;
          if (!clicked) { x0 = x1 = disp.mouse_x(); y0 = y1 = disp.mouse_y(); if (!disp.wheel()) clicked = true; }
          else { x1 = disp.mouse_x(); y1 = disp.mouse_y(); }
          const bool is_keyCTRL = disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT();
          if (disp.button()&1 && !is_keyCTRL) {
            const float
              R = 0.45f*std::min(disp.width(),disp.height()),
              R2 = R*R,
              u0 = (float)(x0 - disp.width()/2),
              v0 = (float)(y0 - disp.height()/2),
              u1 = (float)(x1 - disp.width()/2),
              v1 = (float)(y1 - disp.height()/2),
              n0 = cimg::hypot(u0,v0),
              n1 = cimg::hypot(u1,v1),
              nu0 = n0>R?(u0*R/n0):u0,
              nv0 = n0>R?(v0*R/n0):v0,
              nw0 = (float)std::sqrt(std::max(0.f,R2 - nu0*nu0 - nv0*nv0)),
              nu1 = n1>R?(u1*R/n1):u1,
              nv1 = n1>R?(v1*R/n1):v1,
              nw1 = (float)std::sqrt(std::max(0.f,R2 - nu1*nu1 - nv1*nv1)),
              u = nv0*nw1 - nw0*nv1,
              v = nw0*nu1 - nu0*nw1,
              w = nv0*nu1 - nu0*nv1,
              n = cimg::hypot(u,v,w),
              alpha = (float)std::asin(n/R2)*180/cimg::PI;
            (CImg<floatT>::rotation_matrix(u,v,w,-alpha)*pose).move_to(pose);
            x0 = x1; y0 = y1;
          }
          if (disp.button()&2 && !is_keyCTRL) {
            if (focale>0) Zoff-=(y0 - y1)*focale/400;
            else { const float s = std::exp((y0 - y1)/400.f); pose*=s; sprite_scale*=s; }
            x0 = x1; y0 = y1;
          }
          if (disp.wheel()) {
            if (focale>0) Zoff-=disp.wheel()*focale/20;
            else { const float s = std::exp(disp.wheel()/20.f); pose*=s; sprite_scale*=s; }
            disp.set_wheel();
          }
          if (disp.button()&4 || (disp.button()&1 && is_keyCTRL)) {
            Xoff+=(x1 - x0); Yoff+=(y1 - y0); x0 = x1; y0 = y1;
          }
          if ((disp.button()&1) && (disp.button()&2) && !is_keyCTRL) {
            init_pose = true; disp.set_button(); x0 = x1; y0 = y1;
            pose = CImg<floatT>(4,3,1,1, 1,0,0,0, 0,1,0,0, 0,0,1,0);
          }
        } else if (clicked) { x0 = x1; y0 = y1; clicked = false; redraw = true; }

        CImg<charT> filename(32);
        switch (key = disp.key()) {
#if cimg_OS!=2
        case cimg::keyCTRLRIGHT :
#endif
        case 0 : case cimg::keyCTRLLEFT : key = 0; break;
        case cimg::keyD: if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).
              resize(CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,false),
                     CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,true),false).
              _is_resized = true;
            disp.set_key(key,false); key = 0;
          } break;
        case cimg::keyC : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).
              resize(cimg_fitscreen(2*disp.width()/3,2*disp.height()/3,1),false)._is_resized = true;
            disp.set_key(key,false); key = 0;
          } break;
        case cimg::keyR : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).resize(cimg_fitscreen(_width,_height,_depth),false)._is_resized = true;
            disp.set_key(key,false); key = 0;
          } break;
        case cimg::keyF : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            if (!ns_width || !ns_height ||
                ns_width>(unsigned int)disp.screen_width() || ns_height>(unsigned int)disp.screen_height()) {
              ns_width = disp.screen_width()*3U/4;
              ns_height = disp.screen_height()*3U/4;
            }
            if (disp.is_fullscreen()) disp.resize(ns_width,ns_height,false);
            else {
              ns_width = disp._width; ns_height = disp._height;
              disp.resize(disp.screen_width(),disp.screen_height(),false);
            }
            disp.toggle_fullscreen()._is_resized = true;
            disp.set_key(key,false); key = 0;
          } break;
        case cimg::keyT : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            // Switch single/double-sided primitives.
            if (--_is_double_sided==-2) _is_double_sided = 1;
            if (_is_double_sided>=0) reverse_primitives.assign();
            else primitives.get_reverse_object3d().move_to(reverse_primitives);
            disp.set_key(key,false); key = 0; redraw = true;
          } break;
        case cimg::keyZ : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Enable/disable Z-buffer
            if (zbuffer) zbuffer.assign();
            else zbuffer.assign(visu0.width(),visu0.height(),1,1,0);
            disp.set_key(key,false); key = 0; redraw = true;
          } break;
        case cimg::keyX : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Show/hide 3D axes
            ndisplay_axes = !ndisplay_axes;
            disp.set_key(key,false); key = 0; redraw = true;
          } break;
        case cimg::keyF1 : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Set rendering mode to points
            nrender_motion = (nrender_static==0 && nrender_motion!=0)?0:-1; nrender_static = 0;
            disp.set_key(key,false); key = 0; redraw = true;
          } break;
        case cimg::keyF2 : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Set rendering mode to lines
            nrender_motion = (nrender_static==1 && nrender_motion!=1)?1:-1; nrender_static = 1;
            disp.set_key(key,false); key = 0; redraw = true;
          } break;
        case cimg::keyF3 : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Set rendering mode to flat
            nrender_motion = (nrender_static==2 && nrender_motion!=2)?2:-1; nrender_static = 2;
            disp.set_key(key,false); key = 0; redraw = true;
          } break;
        case cimg::keyF4 : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Set rendering mode to flat-shaded
            nrender_motion = (nrender_static==3 && nrender_motion!=3)?3:-1; nrender_static = 3;
            disp.set_key(key,false); key = 0; redraw = true;
          } break;
        case cimg::keyF5 : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            // Set rendering mode to gouraud-shaded.
            nrender_motion = (nrender_static==4 && nrender_motion!=4)?4:-1; nrender_static = 4;
            disp.set_key(key,false); key = 0; redraw = true;
          } break;
        case cimg::keyF6 : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Set rendering mode to phong-shaded
            nrender_motion = (nrender_static==5 && nrender_motion!=5)?5:-1; nrender_static = 5;
            disp.set_key(key,false); key = 0; redraw = true;
          } break;
        case cimg::keyS : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Save snapshot
            static unsigned int snap_number = 0;
            do {
              cimg_snprintf(filename,filename._width,cimg_appname "_%.6u."
#ifdef cimg_use_png
                            "png",
#else
                            "bmp",
#endif
                            snap_number++);
            } while (cimg::path_exists(filename));
            (+visu).__draw_text(" Saving snapshot... ",font_size,0).display(disp);
            visu.save(filename);
            (+visu).__draw_text(" Snapshot '%s' saved. ",font_size,0,filename._data).display(disp);
            disp.set_key(key,false); key = 0;
          } break;
        case cimg::keyG : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Save object as a .off file
            static unsigned int snap_number = 0;
            do {
              cimg_snprintf(filename,filename._width,cimg_appname "_%.6u.off",snap_number++);
            } while (cimg::path_exists(filename));
            (+visu).__draw_text(" Saving object... ",font_size,0).display(disp);
            vertices.save_off(reverse_primitives?reverse_primitives:primitives,colors,filename);
            (+visu).__draw_text(" Object '%s' saved. ",font_size,0,filename._data).display(disp);
            disp.set_key(key,false); key = 0;
          } break;
        case cimg::keyO : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { // Save object as a .cimg file
            static unsigned int snap_number = 0;
            do {

#ifdef cimg_use_zlib
              cimg_snprintf(filename,filename._width,cimg_appname "_%.6u.cimgz",snap_number++);
#else
              cimg_snprintf(filename,filename._width,cimg_appname "_%.6u.cimg",snap_number++);
#endif
            } while (cimg::path_exists(filename));
            (+visu).__draw_text(" Saving object... ",font_size,0).display(disp);
            vertices.get_object3dtoCImg3d(reverse_primitives?reverse_primitives:primitives,colors,opacities).
              save(filename);
            (+visu).__draw_text(" Object '%s' saved. ",font_size,0,filename._data).display(disp);
            disp.set_key(key,false); key = 0;
          } break;
        }
        if (disp.is_resized()) {
          disp.resize(false); visu0 = get_resize(disp,1);
          if (zbuffer) zbuffer.assign(disp.width(),disp.height());
          redraw = true;
        }
        if (!exit_on_anykey && key && key!=cimg::keyESC &&
            (key!=cimg::keyW || (!disp.is_keyCTRLLEFT() && !disp.is_keyCTRLRIGHT()))) {
          key = 0;
        }
      }
      if (pose_matrix) {
        std::memcpy(pose_matrix,pose._data,12*sizeof(float));
        pose_matrix[12] = Xoff; pose_matrix[13] = Yoff; pose_matrix[14] = Zoff; pose_matrix[15] = sprite_scale;
      }
      disp.set_button().set_key(key);
      return *this;
    }

    //! Display 1D graph in an interactive window.
    /**
       \param disp Display window.
       \param plot_type Plot type. Can be <tt>{ 0=points | 1=segments | 2=splines | 3=bars }</tt>.
       \param vertex_type Vertex type.
       \param labelx Title for the horizontal axis, as a C-string.
       \param xmin Minimum value along the X-axis.
       \param xmax Maximum value along the X-axis.
       \param labely Title for the vertical axis, as a C-string.
       \param ymin Minimum value along the X-axis.
       \param ymax Maximum value along the X-axis.
       \param exit_on_anykey Exit function when any key is pressed.
    **/
    const CImg<T>& display_graph(CImgDisplay &disp,
                                 const unsigned int plot_type=1, const unsigned int vertex_type=1,
                                 const char *const labelx=0, const double xmin=0, const double xmax=0,
                                 const char *const labely=0, const double ymin=0, const double ymax=0,
                                 const bool exit_on_anykey=false) const {
      return _display_graph(disp,0,plot_type,vertex_type,labelx,xmin,xmax,labely,ymin,ymax,exit_on_anykey);
    }

    //! Display 1D graph in an interactive window \overloading.
    const CImg<T>& display_graph(const char *const title=0,
                                 const unsigned int plot_type=1, const unsigned int vertex_type=1,
                                 const char *const labelx=0, const double xmin=0, const double xmax=0,
                                 const char *const labely=0, const double ymin=0, const double ymax=0,
                                 const bool exit_on_anykey=false) const {
      CImgDisplay disp;
      return _display_graph(disp,title,plot_type,vertex_type,labelx,xmin,xmax,labely,ymin,ymax,exit_on_anykey);
    }

    const CImg<T>& _display_graph(CImgDisplay &disp, const char *const title=0,
                                  const unsigned int plot_type=1, const unsigned int vertex_type=1,
                                  const char *const labelx=0, const double xmin=0, const double xmax=0,
                                  const char *const labely=0, const double ymin=0, const double ymax=0,
                                  const bool exit_on_anykey=false) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "display_graph(): Empty instance.",
                                    cimg_instance);
      if (!disp) disp.assign(cimg_fitscreen(CImgDisplay::screen_width()/2,CImgDisplay::screen_height()/2,1),0,0).
                   set_title(title?"%s":"CImg<%s>",title?title:pixel_type());
      const ulongT siz = (ulongT)_width*_height*_depth, siz1 = std::max((ulongT)1,siz - 1);
      const unsigned int old_normalization = disp.normalization();
      disp.show().flush()._normalization = 0;

      double y0 = ymin, y1 = ymax, nxmin = xmin, nxmax = xmax;
      if (nxmin==nxmax) { nxmin = 0; nxmax = siz1; }
      int x0 = 0, x1 = width()*height()*depth() - 1, key = 0;

      for (bool reset_view = true; !key && !disp.is_closed(); ) {
        if (reset_view) { x0 = 0; x1 = width()*height()*depth() - 1; y0 = ymin; y1 = ymax; reset_view = false; }
        CImg<T> zoom(x1 - x0 + 1,1,1,spectrum());
        cimg_forC(*this,c) zoom.get_shared_channel(c) = CImg<T>(data(x0,0,0,c),x1 - x0 + 1,1,1,1,true);
        if (y0==y1) { y0 = zoom.min_max(y1); const double dy = y1 - y0; y0-=dy/20; y1+=dy/20; }
        if (y0==y1) { --y0; ++y1; }

        const CImg<intT> selection = zoom.get_select_graph(disp,plot_type,vertex_type,
                                                           labelx,
                                                           nxmin + x0*(nxmax - nxmin)/siz1,
                                                           nxmin + x1*(nxmax - nxmin)/siz1,
                                                           labely,y0,y1,true);
        const int mouse_x = disp.mouse_x(), mouse_y = disp.mouse_y();
        if (selection[0]>=0) {
          if (selection[2]<0) reset_view = true;
          else {
            x1 = x0 + selection[2]; x0+=selection[0];
            if (selection[1]>=0 && selection[3]>=0) {
              y0 = y1 - selection[3]*(y1 - y0)/(disp.height() - 32);
              y1-=selection[1]*(y1 - y0)/(disp.height() - 32);
            }
          }
        } else {
          bool go_in = false, go_out = false, go_left = false, go_right = false, go_up = false, go_down = false;
          switch (key = (int)disp.key()) {
          case cimg::keyHOME : reset_view = true; key = 0; disp.set_key(); break;
          case cimg::keyPADADD : go_in = true; go_out = false; key = 0; disp.set_key(); break;
          case cimg::keyPADSUB : go_out = true; go_in = false; key = 0; disp.set_key(); break;
          case cimg::keyARROWLEFT : case cimg::keyPAD4 : go_left = true; go_right = false; key = 0; disp.set_key();
            break;
          case cimg::keyARROWRIGHT : case cimg::keyPAD6 : go_right = true; go_left = false; key = 0; disp.set_key();
            break;
          case cimg::keyARROWUP : case cimg::keyPAD8 : go_up = true; go_down = false; key = 0; disp.set_key(); break;
          case cimg::keyARROWDOWN : case cimg::keyPAD2 : go_down = true; go_up = false; key = 0; disp.set_key(); break;
          case cimg::keyPAD7 : go_left = true; go_up = true; key = 0; disp.set_key(); break;
          case cimg::keyPAD9 : go_right = true; go_up = true; key = 0; disp.set_key(); break;
          case cimg::keyPAD1 : go_left = true; go_down = true; key = 0; disp.set_key(); break;
          case cimg::keyPAD3 : go_right = true; go_down = true; key = 0; disp.set_key(); break;
          }
          if (disp.wheel()) {
            if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) go_up = !(go_down = disp.wheel()<0);
            else if (disp.is_keySHIFTLEFT() || disp.is_keySHIFTRIGHT()) go_left = !(go_right = disp.wheel()>0);
            else go_out = !(go_in = disp.wheel()>0);
            key = 0;
          }

          if (go_in) {
            const int
              xsiz = x1 - x0,
              mx = (mouse_x - 16)*xsiz/(disp.width() - 32),
              cx = x0 + cimg::cut(mx,0,xsiz);
            if (x1 - x0>4) {
              x0 = cx - 7*(cx - x0)/8; x1 = cx + 7*(x1 - cx)/8;
              if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
                const double
                  ysiz = y1 - y0,
                  my = (mouse_y - 16)*ysiz/(disp.height() - 32),
                  cy = y1 - cimg::cut(my,0.,ysiz);
                y0 = cy - 7*(cy - y0)/8; y1 = cy + 7*(y1 - cy)/8;
              } else y0 = y1 = 0;
            }
          }
          if (go_out) {
            if (x0>0 || x1<(int)siz1) {
              const int delta_x = (x1 - x0)/8, ndelta_x = delta_x?delta_x:(siz>1);
              const double ndelta_y = (y1 - y0)/8;
              x0-=ndelta_x; x1+=ndelta_x;
              y0-=ndelta_y; y1+=ndelta_y;
              if (x0<0) { x1-=x0; x0 = 0; if (x1>=(int)siz) x1 = (int)siz1; }
              if (x1>=(int)siz) { x0-=(x1 - siz1); x1 = (int)siz1; if (x0<0) x0 = 0; }
            }
          }
          if (go_left) {
            const int delta = (x1 - x0)/5, ndelta = delta?delta:1;
            if (x0 - ndelta>=0) { x0-=ndelta; x1-=ndelta; }
            else { x1-=x0; x0 = 0; }
            go_left = false;
          }
          if (go_right) {
            const int delta = (x1 - x0)/5, ndelta = delta?delta:1;
            if (x1 + ndelta<(int)siz) { x0+=ndelta; x1+=ndelta; }
            else { x0+=(siz1 - x1); x1 = (int)siz1; }
            go_right = false;
          }
          if (go_up) {
            const double delta = (y1 - y0)/10, ndelta = delta?delta:1;
            y0+=ndelta; y1+=ndelta;
            go_up = false;
          }
          if (go_down) {
            const double delta = (y1 - y0)/10, ndelta = delta?delta:1;
            y0-=ndelta; y1-=ndelta;
            go_down = false;
          }
        }
        if (!exit_on_anykey && key && key!=(int)cimg::keyESC &&
            (key!=(int)cimg::keyW || (!disp.is_keyCTRLLEFT() && !disp.is_keyCTRLRIGHT()))) {
          disp.set_key(key,false);
          key = 0;
        }
      }
      disp._normalization = old_normalization;
      return *this;
    }



#endif
