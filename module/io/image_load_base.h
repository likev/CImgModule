#ifndef CIMG_MODULE_IMAGE_LOAD_BASE_H
#define CIMG_MODULE_IMAGE_LOAD_BASE_H


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

#endif
