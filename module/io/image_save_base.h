#ifndef CIMG_MODULE_IMAGE_SAVE_BASE_H
#define CIMG_MODULE_IMAGE_SAVE_BASE_H


    //! Save image as a file.
    /**
       \param filename Filename, as a C-string.
       \param number When positive, represents an index added to the filename. Otherwise, no number is added.
       \param digits Number of digits used for adding the number to the filename.
       \note
       - The used file format is defined by the file extension in the filename \p filename.
       - Parameter \p number can be used to add a 6-digit number to the filename before saving.

    **/
    const CImg<T>& save(const char *const filename, const int number=-1, const unsigned int digits=6) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save(): Specified filename is (null).",
                                    cimg_instance);
      // Do not test for empty instances, since .cimg format is able to manage empty instances.
      const bool is_stdout = *filename=='-' && (!filename[1] || filename[1]=='.');
      const char *const ext = cimg::split_filename(filename);
      CImg<charT> nfilename(1024);
      const char *const fn = is_stdout?filename:(number>=0)?cimg::number_filename(filename,number,digits,nfilename):
        filename;

#ifdef cimg_save_plugin
      cimg_save_plugin(fn);
#endif
#ifdef cimg_save_plugin1
      cimg_save_plugin1(fn);
#endif
#ifdef cimg_save_plugin2
      cimg_save_plugin2(fn);
#endif
#ifdef cimg_save_plugin3
      cimg_save_plugin3(fn);
#endif
#ifdef cimg_save_plugin4
      cimg_save_plugin4(fn);
#endif
#ifdef cimg_save_plugin5
      cimg_save_plugin5(fn);
#endif
#ifdef cimg_save_plugin6
      cimg_save_plugin6(fn);
#endif
#ifdef cimg_save_plugin7
      cimg_save_plugin7(fn);
#endif
#ifdef cimg_save_plugin8
      cimg_save_plugin8(fn);
#endif
      // Text formats.
      if (!cimg::strcasecmp(ext,"asc")) return save_ascii(fn);
      else if (!cimg::strcasecmp(ext,"csv") ||
               !cimg::strcasecmp(ext,"dlm") ||
               !cimg::strcasecmp(ext,"txt")) return save_dlm(fn);
      else if (!cimg::strcasecmp(ext,"cpp") ||
               !cimg::strcasecmp(ext,"hpp") ||
               !cimg::strcasecmp(ext,"h") ||
               !cimg::strcasecmp(ext,"c")) return save_cpp(fn);

      // 2D binary formats.
      else if (!cimg::strcasecmp(ext,"bmp")) return save_bmp(fn);
      else if (!cimg::strcasecmp(ext,"jpg") ||
               !cimg::strcasecmp(ext,"jpeg") ||
               !cimg::strcasecmp(ext,"jpe") ||
               !cimg::strcasecmp(ext,"jfif") ||
               !cimg::strcasecmp(ext,"jif")) return save_jpeg(fn);
      else if (!cimg::strcasecmp(ext,"rgb")) return save_rgb(fn);
      else if (!cimg::strcasecmp(ext,"rgba")) return save_rgba(fn);
      else if (!cimg::strcasecmp(ext,"png")) return save_png(fn);
      else if (!cimg::strcasecmp(ext,"pgm") ||
               !cimg::strcasecmp(ext,"ppm") ||
               !cimg::strcasecmp(ext,"pnm")) return save_pnm(fn);
      else if (!cimg::strcasecmp(ext,"pnk")) return save_pnk(fn);
      else if (!cimg::strcasecmp(ext,"pfm")) return save_pfm(fn);
      else if (!cimg::strcasecmp(ext,"exr")) return save_exr(fn);
      else if (!cimg::strcasecmp(ext,"tif") ||
               !cimg::strcasecmp(ext,"tiff")) return save_tiff(fn);
      else if (!cimg::strcasecmp(ext,"webp")) return save_webp(fn);
      else if (!cimg::strcasecmp(ext,"jxl")) return save_jxl(fn);

      // 3D binary formats.
      else if (!*ext) {
#ifdef cimg_use_zlib
        return save_cimg(fn,true);
#else
        return save_cimg(fn,false);
#endif
      } else if (!cimg::strcasecmp(ext,"cimgz")) return save_cimg(fn,true);
      else if (!cimg::strcasecmp(ext,"cimg")) return save_cimg(fn,false);
      else if (!cimg::strcasecmp(ext,"dcm")) return save_medcon_external(fn);
      else if (!cimg::strcasecmp(ext,"hdr") ||
               !cimg::strcasecmp(ext,"nii")) return save_analyze(fn);
      else if (!cimg::strcasecmp(ext,"inr")) return save_inr(fn);
      else if (!cimg::strcasecmp(ext,"mnc")) return save_minc2(fn);
      else if (!cimg::strcasecmp(ext,"pan")) return save_pandore(fn);
      else if (!cimg::strcasecmp(ext,"raw")) return save_raw(fn);

      // Archive files.
      else if (!cimg::strcasecmp(ext,"gz")) return save_gzip_external(fn);

      // Image sequences.
      else if (!cimg::strcasecmp(ext,"yuv")) return save_yuv(fn,444,true);
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
               !cimg::strcasecmp(ext,"mpeg")) return save_video(fn);
      return save_other(fn);
    }

    //! Save image as an ascii file.
    /**
      \param filename Filename, as a C-string.
    **/
    const CImg<T>& save_ascii(const char *const filename) const {
      return _save_ascii(0,filename);
    }

    //! Save image as an Ascii file \overloading.
    const CImg<T>& save_ascii(std::FILE *const file) const {
      return _save_ascii(file,0);
    }

    const CImg<T>& _save_ascii(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_ascii(): Specified filename is (null).",
                                    cimg_instance);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"w");
      std::fprintf(nfile,"%u %u %u %u\n",_width,_height,_depth,_spectrum);
      const T* ptrs = _data;
      cimg_forYZC(*this,y,z,c) {
        cimg_forX(*this,x) std::fprintf(nfile,"%.17g ",(double)*(ptrs++));
        std::fputc('\n',nfile);
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save image as a .cpp source file.
    /**
      \param filename Filename, as a C-string.
    **/
    const CImg<T>& save_cpp(const char *const filename) const {
      return _save_cpp(0,filename);
    }

    //! Save image as a .cpp source file \overloading.
    const CImg<T>& save_cpp(std::FILE *const file) const {
      return _save_cpp(file,0);
    }

    const CImg<T>& _save_cpp(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_cpp(): Specified filename is (null).",
                                    cimg_instance);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"w");
      CImg<charT> varname(1024); *varname = 0;
      if (filename) cimg_sscanf(cimg::basename(filename),"%1023[a-zA-Z0-9_]",varname._data);
      if (!*varname) cimg_snprintf(varname,varname._width,"unnamed");
      std::fprintf(nfile,
                   "/* Define image '%s' of size %ux%ux%ux%u and type '%s' */\n"
                   "%s data_%s[] = { %s\n  ",
                   varname._data,_width,_height,_depth,_spectrum,pixel_type(),pixel_type(),varname._data,
                   is_empty()?"};":"");
      if (!is_empty()) for (ulongT off = 0, siz = size() - 1; off<=siz; ++off) {
        std::fprintf(nfile,cimg::type<T>::format(),cimg::type<T>::format((*this)[off]));
        if (off==siz) std::fprintf(nfile," };\n");
        else if (!((off + 1)%16)) std::fprintf(nfile,",\n  ");
        else std::fprintf(nfile,", ");
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save image as a DLM file.
    /**
       \param filename Filename, as a C-string.
    **/
    const CImg<T>& save_dlm(const char *const filename) const {
      return _save_dlm(0,filename);
    }

    //! Save image as a DLM file \overloading.
    const CImg<T>& save_dlm(std::FILE *const file) const {
      return _save_dlm(file,0);
    }

    const CImg<T>& _save_dlm(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_dlm(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_dlm(): Instance is volumetric, values along Z will be unrolled in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (_spectrum>1)
        cimg::warn(_cimg_instance
                   "save_dlm(): Instance is multispectral, values along C will be unrolled in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"w");
      const T* ptrs = _data;
      cimg_forYZC(*this,y,z,c) {
        cimg_forX(*this,x) std::fprintf(nfile,"%.17g%s",(double)*(ptrs++),(x==width() - 1)?"":",");
        std::fputc('\n',nfile);
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save image as a BMP file.
    /**
      \param filename Filename, as a C-string.
    **/
#endif
