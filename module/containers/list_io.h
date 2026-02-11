#ifndef CIMG_MODULE_LIST_IO_H
#define CIMG_MODULE_LIST_IO_H

    //----------------------------------
    //
    //! \name Data Output
    //@{
    //----------------------------------

    //! Print information about the list on the standard output.
    /**
      \param title Label set to the information displayed.
      \param display_stats Tells if image statistics must be computed and displayed.
    **/
    const CImgList<T>& print(const char *const title=0, const bool display_stats=true) const {
      unsigned int msiz = 0;
      cimglist_for(*this,l) msiz+=_data[l].size();
      msiz*=sizeof(T);
      const unsigned int mdisp = msiz<8*1024?0U:msiz<8*1024*1024?1U:2U;
      CImg<charT> _title(64);
      if (!title) cimg_snprintf(_title,_title._width,"CImgList<%s>",pixel_type());
      std::fprintf(cimg::output(),"%s%s%s%s: %sthis%s = %p, %ssize%s = %u/%u [%u %s], %sdata%s = (CImg<%s>*)%p",
                   cimg::t_magenta,cimg::t_bold,title?title:_title._data,cimg::t_normal,
                   cimg::t_bold,cimg::t_normal,(void*)this,
                   cimg::t_bold,cimg::t_normal,_width,_allocated_width,
                   mdisp==0?msiz:(mdisp==1?(msiz>>10):(msiz>>20)),
                   mdisp==0?"b":(mdisp==1?"Kio":"Mio"),
                   cimg::t_bold,cimg::t_normal,pixel_type(),(void*)begin());
      if (_data) std::fprintf(cimg::output(),"..%p.\n",(void*)((char*)end() - 1));
      else std::fprintf(cimg::output(),".\n");

      char tmp[16] = {};
      cimglist_for(*this,ll) {
        cimg_snprintf(tmp,sizeof(tmp),"[%d]",ll);
        std::fprintf(cimg::output(),"  ");
        _data[ll].print(tmp,display_stats);
        if (ll==3 && width()>8) { ll = width() - 5; std::fprintf(cimg::output(),"  ...\n"); }
      }
      std::fflush(cimg::output());
      return *this;
    }

    //! Display the current CImgList instance in an existing CImgDisplay window (by reference).
    /**
       \param disp Reference to an existing CImgDisplay instance, where the current image list will be displayed.
       \param axis Appending axis. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
       \param align Appending alignment.
       \note This function displays the list images of the current CImgList instance into an existing
         CImgDisplay window.
       Images of the list are appended in a single temporary image for visualization purposes.
       The function returns immediately.
    **/
    const CImgList<T>& display(CImgDisplay &disp, const char axis='x', const float align=0) const {
      disp.display(*this,axis,align);
      return *this;
    }

    //! Display the current CImgList instance in a new display window.
    /**
        \param disp Display window.
        \param display_info Tells if image information are displayed on the standard output.
        \param axis Alignment axis for images viewing.
        \param align Appending alignment.
        \param[in,out] XYZ Contains the XYZ coordinates at start / exit of the function.
        \param exit_on_anykey Exit function when any key is pressed.
        \note This function opens a new window with a specific title and displays the list images of the
          current CImgList instance into it.
        Images of the list are appended in a single temporary image for visualization purposes.
        The function returns when a key is pressed or the display window is closed by the user.
    **/
    const CImgList<T>& display(CImgDisplay &disp, const bool display_info,
                               const char axis='x', const float align=0,
                               unsigned int *const XYZ=0, const bool exit_on_anykey=false) const {
      bool is_exit = false;
      return _display(disp,0,0,display_info,axis,align,XYZ,exit_on_anykey,0,true,is_exit);
    }

    //! Display the current CImgList instance in a new display window.
    /**
      \param title Title of the opening display window.
      \param display_info Tells if list information must be written on standard output.
      \param axis Appending axis. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
      \param align Appending alignment.
      \param[in,out] XYZ Contains the XYZ coordinates at start / exit of the function.
      \param exit_on_anykey Exit function when any key is pressed.
    **/
    const CImgList<T>& display(const char *const title=0, const bool display_info=true,
                               const char axis='x', const float align=0,
                               unsigned int *const XYZ=0, const bool exit_on_anykey=false) const {
      CImgDisplay disp;
      bool is_exit = false;
      return _display(disp,title,0,display_info,axis,align,XYZ,exit_on_anykey,0,true,is_exit);
    }

    const CImgList<T>& _display(CImgDisplay &disp, const char *const title, const CImgList<charT> *const titles,
                                const bool display_info, const char axis, const float align, unsigned int *const XYZ,
                                const bool exit_on_anykey, const unsigned int orig, const bool is_first_call,
                                bool &is_exit) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "display(): Empty instance.",
                                    cimglist_instance);
      if (!disp) {
        if (axis=='x') {
          unsigned int sum_width = 0, max_height = 0;
          cimglist_for(*this,l) {
            const CImg<T> &img = _data[l];
            const unsigned int
              w = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,false),
              h = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,true);
            sum_width+=w;
            if (h>max_height) max_height = h;
          }
          disp.assign(cimg_fitscreen(sum_width,max_height,1),title?title:titles?titles->__display()._data:0,1);
        } else {
          unsigned int max_width = 0, sum_height = 0;
          cimglist_for(*this,l) {
            const CImg<T> &img = _data[l];
            const unsigned int
              w = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,false),
              h = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,true);
            if (w>max_width) max_width = w;
            sum_height+=h;
          }
          disp.assign(cimg_fitscreen(max_width,sum_height,1),title?title:titles?titles->__display()._data:0,1);
        }
        if (!title && !titles) disp.set_title("CImgList<%s> (%u)",pixel_type(),_width);
      } else if (title) disp.set_title("%s",title);
      else if (titles) disp.set_title("%s",titles->__display()._data);
      const CImg<char> dtitle = CImg<char>::string(disp.title());
      if (display_info) print(disp.title());
      disp.show().flush();

      if (_width==1) {
        const unsigned int dw = disp._width, dh = disp._height;
        if (!is_first_call)
          disp.resize(cimg_fitscreen(_data[0]._width,_data[0]._height,_data[0]._depth),false);
        disp.set_title("%s (%ux%ux%ux%u)",
                       dtitle.data(),_data[0]._width,_data[0]._height,_data[0]._depth,_data[0]._spectrum);
        _data[0]._display(disp,0,false,XYZ,exit_on_anykey,!is_first_call);
        if (disp.key()) is_exit = true;
        disp.resize(cimg_fitscreen(dw,dh,1),false).set_title("%s",dtitle.data());
      } else {
        bool disp_resize = !is_first_call;
        while (!disp.is_closed() && !is_exit) {
          const CImg<intT> s = _select(disp,0,true,axis,align,exit_on_anykey,orig,disp_resize,!is_first_call,true);
          disp_resize = true;
          if (s[0]<0 && !disp.wheel()) { // No selections done
            if (disp.button()&2) { disp.flush(); break; }
            is_exit = true;
          } else if (disp.wheel()) { // Zoom in/out
            const int wheel = disp.wheel();
            disp.set_wheel();
            if (!is_first_call && wheel<0) break;
            if (wheel>0 && _width>=4) {
              const unsigned int
                delta = std::max(1U,(unsigned int)cimg::round(0.3*_width)),
                ind0 = (unsigned int)std::max(0,s[0] - (int)delta),
                ind1 = (unsigned int)std::min(width() - 1,s[0] + (int)delta);
              if ((ind0!=0 || ind1!=_width - 1) && ind1 - ind0>=3) {
                const CImgList<T> sublist = get_shared_images(ind0,ind1);
                CImgList<charT> t_sublist;
                if (titles) t_sublist = titles->get_shared_images(ind0,ind1);
                sublist._display(disp,0,titles?&t_sublist:0,false,axis,align,XYZ,exit_on_anykey,
                                 orig + ind0,false,is_exit);
              }
            }
          } else if (s[0]!=0 || s[1]!=width() - 1) {
            const CImgList<T> sublist = get_shared_images(s[0],s[1]);
            CImgList<charT> t_sublist;
            if (titles) t_sublist = titles->get_shared_images(s[0],s[1]);
            sublist._display(disp,0,titles?&t_sublist:0,false,axis,align,XYZ,exit_on_anykey,
                             orig + s[0],false,is_exit);
          }
          disp.set_title("%s",dtitle.data());
        }
      }
      return *this;
    }

    // [internal] Return string to describe display title.
    CImg<charT> __display() const {
      CImg<charT> res, str;
      cimglist_for(*this,l) {
        CImg<charT>::string((char*)_data[l]._data).move_to(str);
        if (l!=width() - 1) {
          str.resize(str._width + 1,1,1,1,0);
          str[str._width - 2] = ',';
          str[str._width - 1] = ' ';
        }
        res.append(str,'x');
      }
      if (!res) return CImg<charT>(1,1,1,1,0).move_to(res);
      cimg::strellipsize(res,128,false);
      if (_width>1) {
        const unsigned int l = (unsigned int)std::strlen(res);
        if (res._width<=l + 16) res.resize(l + 16,1,1,1,0);
        cimg_snprintf(res._data + l,16," (#%u)",_width);
      }
      return res;
    }

    //! Save list into a file.
    /**
      \param filename Filename to write data to.
      \param number When positive, represents an index added to the filename. Otherwise, no number is added.
      \param digits Number of digits used for adding the number to the filename.
    **/
    const CImgList<T>& save(const char *const filename, const int number=-1, const unsigned int digits=6) const {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save(): Specified filename is (null).",
                                    cimglist_instance);
      // Do not test for empty instances, since .cimg format is able to manage empty instances.
      const bool is_stdout = *filename=='-' && (!filename[1] || filename[1]=='.');
      const char *const ext = cimg::split_filename(filename);
      CImg<charT> nfilename(1024);
      const char *const fn = is_stdout?filename:number>=0?cimg::number_filename(filename,number,digits,nfilename):
        filename;

#ifdef cimglist_save_plugin
      cimglist_save_plugin(fn);
#endif
#ifdef cimglist_save_plugin1
      cimglist_save_plugin1(fn);
#endif
#ifdef cimglist_save_plugin2
      cimglist_save_plugin2(fn);
#endif
#ifdef cimglist_save_plugin3
      cimglist_save_plugin3(fn);
#endif
#ifdef cimglist_save_plugin4
      cimglist_save_plugin4(fn);
#endif
#ifdef cimglist_save_plugin5
      cimglist_save_plugin5(fn);
#endif
#ifdef cimglist_save_plugin6
      cimglist_save_plugin6(fn);
#endif
#ifdef cimglist_save_plugin7
      cimglist_save_plugin7(fn);
#endif
#ifdef cimglist_save_plugin8
      cimglist_save_plugin8(fn);
#endif
      if (!cimg::strcasecmp(ext,"cimgz")) return save_cimg(fn,true);
      else if (!cimg::strcasecmp(ext,"cimg") || !*ext) return save_cimg(fn,false);
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
#ifdef cimg_use_tiff
      else if (!cimg::strcasecmp(ext,"tif") ||
          !cimg::strcasecmp(ext,"tiff")) return save_tiff(fn);
#endif
      else if (!cimg::strcasecmp(ext,"gz")) return save_gzip_external(fn);
      else {
        if (_width==1) _data[0].save(fn,-1);
        else cimglist_for(*this,l) { _data[l].save(fn,is_stdout?-1:l); if (is_stdout) std::fputc(EOF,cimg::_stdout()); }
      }
      return *this;
    }

    //! Tell if an image list can be saved as one single file.
    /**
       \param filename Filename, as a C-string.
       \return \c true if the file format supports multiple images, \c false otherwise.
    **/
    static bool is_saveable(const char *const filename) {
      const char *const ext = cimg::split_filename(filename);
      if (!cimg::strcasecmp(ext,"cimgz") ||
#ifdef cimg_use_tiff
          !cimg::strcasecmp(ext,"tif") ||
          !cimg::strcasecmp(ext,"tiff") ||
#endif
          !cimg::strcasecmp(ext,"yuv") ||
          !cimg::strcasecmp(ext,"avi") ||
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
          !cimg::strcasecmp(ext,"mpeg")) return true;
      return false;
    }

    //! Save image sequence as a GIF animated file.
    /**
       \param filename Filename to write data to.
       \param fps Number of desired frames per second.
       \param nb_loops Number of loops (\c 0 for infinite looping).
    **/
    const CImgList<T>& save_gif_external(const char *const filename, const float fps=25,
                                         const unsigned int nb_loops=0) {
      CImg<charT> command(1024), filename_tmp(256), filename_tmp2(256);
      CImgList<charT> filenames;

#ifdef cimg_use_png
#define _cimg_save_gif_extension "png"
#else
#define _cimg_save_gif_extension "ppm"
#endif
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_000001." _cimg_save_gif_extension,filename_tmp._data);
      } while (cimg::path_exists(filename_tmp2));
      cimglist_for(*this,l) {
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_%.6u." _cimg_save_gif_extension,filename_tmp._data,l + 1);
        CImg<charT>::string(filename_tmp2).move_to(filenames);
        CImg<T> frame;
        if (_data[l]._depth>1) _data[l].get_slice(0).move_to(frame); else frame.assign(_data[l],true);
        if (frame._spectrum>4) frame.assign(frame.get_channels(0,3),false);
        else if (frame._spectrum==1) frame.assign(frame.get_resize(-100,-100,1,3),false);
        else if (frame._spectrum==2)
          frame.assign(frame.get_resize(-100,-100,1,4).draw_image(0,0,0,2,frame.get_shared_channel(0)),false);
        frame.save(filename_tmp2);
      }
      cimg_snprintf(command,command._width,"\"%s\" -delay %u -loop %u -dispose previous",
                    cimg::imagemagick_path(),
                    (unsigned int)std::max(0.f,cimg::round(100/fps)),
                    nb_loops);
      CImg<ucharT>::string(command).move_to(filenames,0);
      cimg_snprintf(command,command._width,"\"%s\"",
                    CImg<charT>::string(filename)._system_strescape().data());
      CImg<ucharT>::string(command).move_to(filenames);
      CImg<charT> _command = filenames>'x';
      cimg_for(_command,p,char) if (!*p) *p = ' ';
      _command.back() = 0;

      if (cimg::system(_command,cimg::imagemagick_path())!=0)
        throw CImgIOException(_cimglist_instance
                              "save_gif_external(): Failed to save file '%s' with external command 'magick/convert'.",
                              cimglist_instance,
                              filename);
      if (!cimg::path_exists(filename))
        throw CImgIOException(_cimglist_instance
                              "save_gif_external(): Failed to save file '%s' with external command 'magick/convert'.",
                              cimglist_instance,
                              filename);
      cimglist_for_in(*this,1,filenames._width - 1,l) std::remove(filenames[l]);
      return *this;
    }

    //! Save list as a YUV image sequence file.
    /**
      \param filename Filename to write data to.
      \param chroma_subsampling Type of chroma subsampling. Can be <tt>{ 420 | 422 | 444 }</tt>.
      \param is_rgb Tells if the RGB to YUV conversion must be done for saving.
    **/
    const CImgList<T>& save_yuv(const char *const filename=0,
                                const unsigned int chroma_subsampling=444,
                                const bool is_rgb=true) const {
      return _save_yuv(0,filename,chroma_subsampling,is_rgb);
    }

    //! Save image sequence into a YUV file.
    /**
      \param file File to write data to.
      \param chroma_subsampling Type of chroma subsampling. Can be <tt>{ 420 | 422 | 444 }</tt>.
      \param is_rgb Tells if the RGB to YUV conversion must be done for saving.
    **/
    const CImgList<T>& save_yuv(std::FILE *const file,
                                const unsigned int chroma_subsampling=444,
                                const bool is_rgb=true) const {
      return _save_yuv(file,0,chroma_subsampling,is_rgb);
    }

    const CImgList<T>& _save_yuv(std::FILE *const file, const char *const filename,
                                 const unsigned int chroma_subsampling,
                                 const bool is_rgb) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_yuv(): Specified filename is (null).",
                                    cimglist_instance);
      if (chroma_subsampling!=420 && chroma_subsampling!=422 && chroma_subsampling!=444)
        throw CImgArgumentException(_cimglist_instance
                                    "save_yuv(): Specified chroma subsampling %u is invalid, for file '%s'.",
                                    cimglist_instance,
                                    chroma_subsampling,filename?filename:"(FILE*)");
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      const unsigned int
        cfx = chroma_subsampling==420 || chroma_subsampling==422?2:1,
        cfy = chroma_subsampling==420?2:1,
        w0 = (*this)[0]._width, h0 = (*this)[0]._height,
        width0 = w0 + (w0%cfx), height0 = h0 + (h0%cfy);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      cimglist_for(*this,l) {
        const CImg<T> &frame = (*this)[l];
        cimg_forZ(frame,z) {
          CImg<ucharT> YUV;
          if (sizeof(T)==1 && !is_rgb &&
              frame._width==width0 && frame._height==height0 && frame._depth==1 && frame._spectrum==3)
            YUV.assign((unsigned char*)frame._data,width0,height0,1,3,true);
          else {
            YUV = frame.get_slice(z);
            if (YUV._width!=width0 || YUV._height!=height0) YUV.resize(width0,height0,1,-100,0);
            if (YUV._spectrum!=3) YUV.resize(-100,-100,1,3,YUV._spectrum==1?1:0);
            if (is_rgb) YUV.RGBtoYCbCr();
          }
          if (chroma_subsampling==444)
            cimg::fwrite(YUV._data,(size_t)YUV._width*YUV._height*3,nfile);
          else {
            cimg::fwrite(YUV._data,(size_t)YUV._width*YUV._height,nfile);
            CImg<ucharT> UV = YUV.get_channels(1,2);
            UV.resize(UV._width/cfx,UV._height/cfy,1,2,2);
            cimg::fwrite(UV._data,(size_t)UV._width*UV._height*2,nfile);
          }
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save list into a .cimg file.
    /**
       \param filename Filename to write data to.
       \param is_compressed Tells if data compression must be enabled.
    **/
    const CImgList<T>& save_cimg(const char *const filename, const bool is_compressed=false) const {
      return _save_cimg(0,filename,is_compressed);
    }

    const CImgList<T>& _save_cimg(std::FILE *const file, const char *const filename, const bool is_compressed) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_cimg(): Specified filename is (null).",
                                    cimglist_instance);
#ifndef cimg_use_zlib
      if (is_compressed)
        cimg::warn(_cimglist_instance
                   "save_cimg(): Unable to save compressed data in file '%s' unless zlib is enabled, "
                   "saving them uncompressed.",
                   cimglist_instance,
                   filename?filename:"(FILE*)");
#endif
      const char *const ptype = pixel_type(), *const etype = cimg::endianness()?"big":"little";
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const bool is_bool = ptype==cimg::type<bool>::string();
      std::fprintf(nfile,"%u %s %s_endian\n",_width,ptype,etype);

      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        std::fprintf(nfile,"%u %u %u %u",img._width,img._height,img._depth,img._spectrum);
        if (img._data) {
          CImg<T> tmp;
          if (cimg::endianness()) { tmp = img; cimg::invert_endianness(tmp._data,tmp.size()); }
          const CImg<T>& ref = cimg::endianness()?tmp:img;
          bool failed_to_compress = true;
          if (is_compressed) {
#ifdef cimg_use_zlib
            Bytef *cbuf = 0;
            uLongf csiz = 0;

            if (is_bool) { // Boolean data (bitwise)
              ulongT siz;
              const unsigned char *const buf = ref._bool2uchar(siz,false);
              csiz = siz + siz/100 + 16;
              cbuf = new Bytef[csiz];
              failed_to_compress = (bool)compress(cbuf,&csiz,(Bytef*)buf,siz);
              if (!failed_to_compress) {
                std::fprintf(nfile," #%lu\n",csiz);
                cimg::fwrite(cbuf,csiz,nfile);
              }
              delete[] buf;
            } else { // Non-boolean data
              const ulongT siz = sizeof(T)*ref.size();
              csiz = siz + siz/100 + 16;
              cbuf = new Bytef[csiz];
              failed_to_compress = (bool)compress(cbuf,&csiz,(Bytef*)ref._data,siz);
              if (!failed_to_compress) {
                std::fprintf(nfile," #%lu\n",csiz);
                cimg::fwrite(cbuf,csiz,nfile);
              }
            }
            if (failed_to_compress)
              cimg::warn(_cimglist_instance
                         "save_cimg(): Failed to save compressed data for file '%s', saving them uncompressed.",
                         cimglist_instance,
                         filename?filename:"(FILE*)");
            delete[] cbuf;
#endif
          }
          if (failed_to_compress) { // Write non-compressed
            std::fputc('\n',nfile);
            if (is_bool) { // Boolean data (bitwise)
              ulongT siz;
              const unsigned char *const buf = ref._bool2uchar(siz,false);
              cimg::fwrite(buf,siz,nfile);
              delete[] buf;
            } else cimg::fwrite(ref._data,ref.size(),nfile); // Non-boolean data
          }
        } else std::fputc('\n',nfile);
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save list into a .cimg file.
    /**
       \param file File to write data to.
       \param is_compressed Tells if data compression must be enabled.
    **/
    const CImgList<T>& save_cimg(std::FILE *file, const bool is_compressed=false) const {
      return _save_cimg(file,0,is_compressed);
    }

    const CImgList<T>& _save_cimg(std::FILE *const file, const char *const filename,
                                 const unsigned int n0,
                                 const unsigned int x0, const unsigned int y0,
                                 const unsigned int z0, const unsigned int c0) const {
#define _cimg_save_cimg_case(Ts1,Ts2,Ts3,Tss) \
      if (!saved && ((Ts1 && !cimg::strcasecmp(Ts1,str_pixeltype)) || \
                     (Ts2 && !cimg::strcasecmp(Ts2,str_pixeltype)) || \
                     (Ts3 && !cimg::strcasecmp(Ts3,str_pixeltype)))) { \
        for (unsigned int l = 0; l<lmax; ++l) { \
          j = 0; while ((i=std::fgetc(nfile))!='\n') tmp[j++]=(char)i; tmp[j] = 0; \
          W = H = D = C = 0; \
          if (cimg_sscanf(tmp,"%u %u %u %u",&W,&H,&D,&C)!=4) \
            throw CImgIOException(_cimglist_instance \
                                  "save_cimg(): Invalid size (%u,%u,%u,%u) of image[%u], for file '%s'.", \
                                  cimglist_instance, \
                                  W,H,D,C,l,filename?filename:"(FILE*)"); \
          if (W*H*D*C>0) { \
            if (l<n0 || x0>=W || y0>=H || z0>=D || c0>=D) cimg::fseek(nfile,W*H*D*C*sizeof(Tss),SEEK_CUR); \
            else { \
              const CImg<T>& img = (*this)[l - n0]; \
              const T *ptrs = img._data; \
              const unsigned int \
                x1 = x0 + img._width - 1, \
                y1 = y0 + img._height - 1, \
                z1 = z0 + img._depth - 1, \
                c1 = c0 + img._spectrum - 1, \
                nx1 = x1>=W?W - 1:x1, \
                ny1 = y1>=H?H - 1:y1, \
                nz1 = z1>=D?D - 1:z1, \
                nc1 = c1>=C?C - 1:c1; \
              CImg<Tss> raw(1 + nx1 - x0); \
              const unsigned int skipvb = c0*W*H*D*sizeof(Tss); \
              if (skipvb) cimg::fseek(nfile,skipvb,SEEK_CUR); \
              for (unsigned int v = 1 + nc1 - c0; v; --v) { \
                const unsigned int skipzb = z0*W*H*sizeof(Tss); \
                if (skipzb) cimg::fseek(nfile,skipzb,SEEK_CUR); \
                for (unsigned int z = 1 + nz1 - z0; z; --z) { \
                  const unsigned int skipyb = y0*W*sizeof(Tss); \
                  if (skipyb) cimg::fseek(nfile,skipyb,SEEK_CUR); \
                  for (unsigned int y = 1 + ny1 - y0; y; --y) { \
                    const unsigned int skipxb = x0*sizeof(Tss); \
                    if (skipxb) cimg::fseek(nfile,skipxb,SEEK_CUR); \
                    raw.assign(ptrs, raw._width); \
                    ptrs+=img._width; \
                    if (endian) cimg::invert_endianness(raw._data,raw._width); \
                    cimg::fwrite(raw._data,raw._width,nfile); \
                    const unsigned int skipxe = (W - 1 - nx1)*sizeof(Tss); \
                    if (skipxe) cimg::fseek(nfile,skipxe,SEEK_CUR); \
                  } \
                  const unsigned int skipye = (H - 1 - ny1)*W*sizeof(Tss); \
                  if (skipye) cimg::fseek(nfile,skipye,SEEK_CUR); \
                } \
                const unsigned int skipze = (D - 1 - nz1)*W*H*sizeof(Tss); \
                if (skipze) cimg::fseek(nfile,skipze,SEEK_CUR); \
              } \
              const unsigned int skipve = (C - 1 - nc1)*W*H*D*sizeof(Tss); \
              if (skipve) cimg::fseek(nfile,skipve,SEEK_CUR); \
            } \
          } \
        } \
        saved = true; \
      }

      if (!file && !filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_cimg(): Specified filename is (null).",
                                    cimglist_instance);
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "save_cimg(): Empty instance, for file '%s'.",
                                    cimglist_instance,
                                    filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb+");
      bool saved = false, endian = cimg::endianness();
      CImg<charT> tmp(256), str_pixeltype(256), str_endian(256);
      *tmp = *str_pixeltype = *str_endian = 0;
      unsigned int j, N, W, H, D, C;
      int i, err;
      j = 0; while ((i=std::fgetc(nfile))!='\n' && i!=EOF && j<256) tmp[j++] = (char)i; tmp[j] = 0;
      err = cimg_sscanf(tmp,"%u%*c%255[A-Za-z123468_]%*c%255[sA-Za-z_ ]",&N,str_pixeltype._data,str_endian._data);
      if (err<2) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "save_cimg(): CImg header not found in file '%s'.",
                              cimglist_instance,
                              filename?filename:"(FILE*)");
      }
      if (!cimg::strncasecmp("little",str_endian,6)) endian = false;
      else if (!cimg::strncasecmp("big",str_endian,3)) endian = true;
      const unsigned int lmax = std::min(N,n0 + _width);
      _cimg_save_cimg_case("bool",0,0,cimg_uint8);
      _cimg_save_cimg_case("uint8","unsigned_char","uchar",cimg_uint8);
      _cimg_save_cimg_case("int8",0,0,cimg_int8);
      _cimg_save_cimg_case("char",0,0,char);
      _cimg_save_cimg_case("uint16","unsigned_short","ushort",cimg_uint16);
      _cimg_save_cimg_case("int16","short",0,cimg_int16);
      _cimg_save_cimg_case("uint32","unsigned_int","uint",cimg_uint32);
      _cimg_save_cimg_case("int32","int",0,cimg_int32);
      _cimg_save_cimg_case("uint64","unsigned_int64",0,cimg_uint64);
      _cimg_save_cimg_case("int64",0,0,cimg_int64);
      _cimg_save_cimg_case("float","float32",0,cimg_float32);
      _cimg_save_cimg_case("float64","double",0,cimg_float64);

      if (!saved) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimglist_instance
                              "save_cimg(): Unsupported data type '%s' for file '%s'.",
                              cimglist_instance,
                              filename?filename:"(FILE*)",str_pixeltype._data);
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Insert the image instance into into an existing .cimg file, at specified coordinates.
    /**
      \param filename Filename to write data to.
      \param n0 Starting index of images to write.
      \param x0 Starting X-coordinates of image regions to write.
      \param y0 Starting Y-coordinates of image regions to write.
      \param z0 Starting Z-coordinates of image regions to write.
      \param c0 Starting C-coordinates of image regions to write.
    **/
    const CImgList<T>& save_cimg(const char *const filename,
                                 const unsigned int n0,
                                 const unsigned int x0, const unsigned int y0,
                                 const unsigned int z0, const unsigned int c0) const {
      return _save_cimg(0,filename,n0,x0,y0,z0,c0);
    }

    //! Insert the image instance into into an existing .cimg file, at specified coordinates.
    /**
      \param file File to write data to.
      \param n0 Starting index of images to write.
      \param x0 Starting X-coordinates of image regions to write.
      \param y0 Starting Y-coordinates of image regions to write.
      \param z0 Starting Z-coordinates of image regions to write.
      \param c0 Starting C-coordinates of image regions to write.
    **/
    const CImgList<T>& save_cimg(std::FILE *const file,
                                 const unsigned int n0,
                                 const unsigned int x0, const unsigned int y0,
                                 const unsigned int z0, const unsigned int c0) const {
      return _save_cimg(file,0,n0,x0,y0,z0,c0);
    }

    static void _save_empty_cimg(std::FILE *const file, const char *const filename,
                                const unsigned int nb,
                                const unsigned int dx, const unsigned int dy,
                                const unsigned int dz, const unsigned int dc) {
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const ulongT siz = (ulongT)dx*dy*dz*dc*sizeof(T);
      std::fprintf(nfile,"%u %s\n",nb,pixel_type());
      for (unsigned int i=nb; i; --i) {
        std::fprintf(nfile,"%u %u %u %u\n",dx,dy,dz,dc);
        for (ulongT off = siz; off; --off) std::fputc(0,nfile);
      }
      if (!file) cimg::fclose(nfile);
    }

    //! Save empty (non-compressed) .cimg file with specified dimensions.
    /**
        \param filename Filename to write data to.
        \param nb Number of images to write.
        \param dx Width of images in the written file.
        \param dy Height of images in the written file.
        \param dz Depth of images in the written file.
        \param dc Spectrum of images in the written file.
    **/
    static void save_empty_cimg(const char *const filename,
                                const unsigned int nb,
                                const unsigned int dx, const unsigned int dy=1,
                                const unsigned int dz=1, const unsigned int dc=1) {
      return _save_empty_cimg(0,filename,nb,dx,dy,dz,dc);
    }

    //! Save empty .cimg file with specified dimensions.
    /**
        \param file File to write data to.
        \param nb Number of images to write.
        \param dx Width of images in the written file.
        \param dy Height of images in the written file.
        \param dz Depth of images in the written file.
        \param dc Spectrum of images in the written file.
    **/
    static void save_empty_cimg(std::FILE *const file,
                                const unsigned int nb,
                                const unsigned int dx, const unsigned int dy=1,
                                const unsigned int dz=1, const unsigned int dc=1) {
      return _save_empty_cimg(file,0,nb,dx,dy,dz,dc);
    }

    //! Save list as a TIFF file.
    /**
      \param filename Filename to write data to.
      \param compression_type Compression mode used to write data.
      \param voxel_size Voxel size, to be stored in the filename.
      \param description Description, to be stored in the filename.
      \param use_bigtiff Allow to save big tiff files (>4Gb).
    **/
    const CImgList<T>& save_tiff(const char *const filename, const unsigned int compression_type=0,
                                 const float *const voxel_size=0, const char *const description=0,
                                 const bool use_bigtiff=true) const {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_tiff(): Specified filename is (null).",
                                    cimglist_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

#ifndef cimg_use_tiff
      if (_width==1) _data[0].save_tiff(filename,compression_type,voxel_size,description,use_bigtiff);
      else cimglist_for(*this,l) {
          CImg<charT> nfilename(1024);
          cimg::number_filename(filename,l,6,nfilename);
          _data[l].save_tiff(nfilename,compression_type,voxel_size,description,use_bigtiff);
        }
#else
      ulongT siz = 0;
      cimglist_for(*this,l) siz+=_data[l].size();
      const bool _use_bigtiff = use_bigtiff && sizeof(siz)>=8 && siz*sizeof(T)>=1UL<<31; // No bigtiff for small images
      TIFF *tif = TIFFOpen(filename,_use_bigtiff?"w8":"w4");
      if (tif) {
        double val_min = cimg::type<double>::inf(), val_max = -val_min;
        cimglist_for(*this,l) {
          double l_val_min, l_val_max = (double)_data[l].max_min(l_val_min);
          if (l_val_min<val_min) val_min = l_val_min;
          if (l_val_max>val_max) val_max = l_val_max;
        }
        for (unsigned int dir = 0, l = 0; l<_width; ++l) {
          const CImg<T>& img = (*this)[l];
          cimg_forZ(img,z) img._save_tiff(tif,dir++,z,compression_type,voxel_size,description,val_min,val_max);
        }
        TIFFClose(tif);
      } else
        throw CImgIOException(_cimglist_instance
                              "save_tiff(): Failed to open stream for file '%s'.",
                              cimglist_instance,
                              filename);
#endif
      return *this;
    }

    //! Save list as a gzipped file, using external tool 'gzip'.
    /**
      \param filename Filename to write data to.
    **/
    const CImgList<T>& save_gzip_external(const char *const filename) const {
      if (!filename)
        throw CImgIOException(_cimglist_instance
                              "save_gzip_external(): Specified filename is (null).",
                              cimglist_instance);
      CImg<charT> command(1024), filename_tmp(256), body(256);
      const char
        *ext = cimg::split_filename(filename,body),
        *ext2 = cimg::split_filename(body,0);
      do {
        if (!cimg::strcasecmp(ext,"gz")) {
          if (*ext2) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                   cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext2);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.cimg",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        } else {
          if (*ext) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                  cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.cimg",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        }
      } while (cimg::path_exists(filename_tmp));

      if (is_saveable(body)) {
        save(filename_tmp);
        cimg_snprintf(command,command._width,"\"%s\" -c \"%s\" > \"%s\"",
                      cimg::gzip_path(),
                      CImg<charT>::string(filename_tmp)._system_strescape().data(),
                      CImg<charT>::string(filename)._system_strescape().data());
        if (cimg::system(command,cimg::gzip_path())!=0)
          throw CImgIOException(_cimglist_instance
                                "save_gzip_external(): Failed to save file '%s' with external command 'gzip'.",
                                cimglist_instance,
                                filename);
        if (!cimg::path_exists(filename))
          throw CImgIOException(_cimglist_instance
                                "save_gzip_external(): Failed to save file '%s' with external command 'gzip'.",
                                cimglist_instance,
                                filename);
        std::remove(filename_tmp);
      } else {
        CImg<charT> nfilename(1024);
        cimglist_for(*this,l) {
          cimg::number_filename(body,l,6,nfilename);
          if (*ext) cimg_snprintf(nfilename._data + std::strlen(nfilename),64,".%s",ext);
          _data[l].save_gzip_external(nfilename);
        }
      }
      return *this;
    }

    //! Save image sequence (using the OpenCV library when available).
    /**
       \param filename Filename to write data to.
       \param fps Number of frames per second.
       \param codec Type of compression (See http://www.fourcc.org/codecs.php to see available codecs).
       \param keep_open Tells if the video writer associated to the specified filename
       must be kept open or not (to allow frames to be added in the same file afterwards).
    **/
    const CImgList<T>& save_video(const char *const filename, const unsigned int fps=25,
                                  const char *codec=0, const bool keep_open=false) const {
#ifndef cimg_use_opencv
      cimg::unused(codec,keep_open);
      if (keep_open) cimg::warn(_cimglist_instance
                                "save_video(): Cannot output streamed video, as this requires features from the "
                                "OpenCV library ('-Dcimg_use_opencv') must be defined).",
                                cimglist_instance);
      if (!is_empty()) return save_ffmpeg_external(filename,fps);
      return *this;
#else
      try {
        static cv::VideoWriter *writers[32] = {};
        static CImgList<charT> filenames(32);
        static CImg<intT> sizes(32,2,1,1,0);
        static int last_used_index = -1;

        // Detect if a video writer already exists for the specified filename.
        cimg::mutex(9);
        int index = -1;
        if (filename) {
          if (last_used_index>=0 && !std::strcmp(filename,filenames[last_used_index])) {
            index = last_used_index;
          } else cimglist_for(filenames,l) if (filenames[l] && !std::strcmp(filename,filenames[l])) {
              index = l; break;
            }
        } else index = last_used_index;
        cimg::mutex(9,0);

        // Find empty slot for capturing video stream.
        if (index<0) {
          if (!filename)
            throw CImgArgumentException(_cimglist_instance
                                        "save_video(): No already open video writer found. You must specify a "
                                        "non-(null) filename argument for the first call.",
                                        cimglist_instance);
          else { cimg::mutex(9); cimglist_for(filenames,l) if (!filenames[l]) { index = l; break; } cimg::mutex(9,0); }
          if (index<0)
            throw CImgIOException(_cimglist_instance
                                  "save_video(): File '%s', no video writer slots available. "
                                  "You have to release some of your previously opened videos.",
                                  cimglist_instance,filename);
          if (is_empty())
            throw CImgInstanceException(_cimglist_instance
                                        "save_video(): Instance list is empty.",
                                        cimglist_instance);
          const unsigned int W = _data?_data[0]._width:0, H = _data?_data[0]._height:0;
          if (!W || !H)
            throw CImgInstanceException(_cimglist_instance
                                        "save_video(): Frame [0] is an empty image.",
                                        cimglist_instance);
          const char
            *const _codec = codec && *codec?codec:"h264",
            codec0 = cimg::uppercase(_codec[0]),
            codec1 = _codec[0]?cimg::uppercase(_codec[1]):0,
            codec2 = _codec[1]?cimg::uppercase(_codec[2]):0,
            codec3 = _codec[2]?cimg::uppercase(_codec[3]):0;
          cimg::mutex(9);
          writers[index] = new cv::VideoWriter(filename,_cimg_fourcc(codec0,codec1,codec2,codec3),fps,cv::Size(W,H));
          if (!writers[index]->isOpened()) {
            delete writers[index];
            writers[index] = 0;
            cimg::mutex(9,0);
            throw CImgIOException(_cimglist_instance
                                  "save_video(): File '%s', unable to initialize video writer with codec '%c%c%c%c'.",
                                  cimglist_instance,filename,
                                  codec0,codec1,codec2,codec3);
          }
          CImg<charT>::string(filename).move_to(filenames[index]);
          sizes(index,0) = W;
          sizes(index,1) = H;
          cimg::mutex(9,0);
        }

        if (!is_empty()) {
          const unsigned int W = sizes(index,0), H = sizes(index,1);
          cimg::mutex(9);
          cimglist_for(*this,l) {
            CImg<T> &src = _data[l];
            if (src.is_empty())
              cimg::warn(_cimglist_instance
                         "save_video(): Skip empty frame %d for file '%s'.",
                         cimglist_instance,l,filename);
            if (src._spectrum>3)
              cimg::warn(_cimglist_instance
                         "save_video(): Frame %u has incompatible dimension (%u,%u,%u,%u). "
                         "Some image data may be ignored when writing frame into video file '%s'.",
                         cimglist_instance,l,src._width,src._height,src._depth,src._spectrum,filename);
            cimg_forZ(src,z) {
              CImg<T> _src = src._depth>1?src.get_slice(z):src.get_shared();
              if (_src._width==W && _src._height==H && _src._spectrum==3)
                writers[index]->write(CImg<ucharT>(_src)._cimg2cvmat());
              else {
                CImg<ucharT> __src(_src,false);
                __src.channels(0,std::min(__src._spectrum - 1,2U)).resize(W,H);
                __src.resize(W,H,1,3,__src._spectrum==1);
                writers[index]->write(__src._cimg2cvmat());
              }
            }
          }
          cimg::mutex(9,0);
        }

        cimg::mutex(9);
        if (!keep_open) {
          delete writers[index];
          writers[index] = 0;
          filenames[index].assign();
          sizes(index,0) = sizes(index,1) = 0;
          last_used_index = -1;
        } else last_used_index = index;
        cimg::mutex(9,0);
      } catch (CImgIOException &e) {
        if (!keep_open) return save_ffmpeg_external(filename,fps);
        throw e;
      }
      return *this;
#endif
    }

    //! Save image sequence, using the external tool 'ffmpeg'.
    /**
      \param filename Filename to write data to.
      \param fps Number of frames per second.
      \param codec Type of compression.
      \param bitrate Output bitrate
    **/
    const CImgList<T>& save_ffmpeg_external(const char *const filename, const unsigned int fps=25,
                                            const char *const codec=0, const unsigned int bitrate=2048) const {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_ffmpeg_external(): Specified filename is (null).",
                                    cimglist_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

      const char
        *const ext = cimg::split_filename(filename),
        *const _codec = codec?codec:
        !cimg::strcasecmp(ext,"flv")?"flv":
        !cimg::strcasecmp(ext,"mp4")?"h264":"mpeg2video";

      CImg<charT> command(1024), filename_tmp(256), filename_tmp2(256);
      CImgList<charT> filenames;
      cimglist_for(*this,l) if (!_data[l].is_sameXYZ(_data[0]))
        throw CImgInstanceException(_cimglist_instance
                                    "save_ffmpeg_external(): Invalid instance dimensions for file '%s'.",
                                    cimglist_instance,
                                    filename);
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_000001.ppm",filename_tmp._data);
      } while (cimg::path_exists(filename_tmp2));
      unsigned int frame = 1;
      cimglist_for(*this,l) {
        CImg<T>& src = _data[l];
        cimg_forZ(src,z) {
          cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_%.6u.ppm",filename_tmp._data,frame);
          CImg<charT>::string(filename_tmp2).move_to(filenames);
          CImg<T> _src = src._depth>1?src.get_slice(z):src.get_shared();
          if (_src._width%2 || _src._height%2) // Force output to have an even number of columns and rows
            _src.assign(_src.get_resize(_src._width + (_src._width%2),_src._height + (_src._height%2),1,-100,0),false);
          if (_src._spectrum!=3) // Force output to be one slice, in color
            _src.assign(_src.get_resize(-100,-100,1,3),false);
          _src.save_pnm(filename_tmp2);
          ++frame;
        }
      }
      cimg_snprintf(command,command._width,
                    "\"%s\" -framerate %u -v -8 -y -i \"%s_%%6d.ppm\" -pix_fmt yuv420p -vcodec %s -b %uk -r %u \"%s\"",
                    cimg::ffmpeg_path(),
                    fps,CImg<charT>::string(filename_tmp)._system_strescape().data(),
                    _codec,bitrate,fps,
                    CImg<charT>::string(filename)._system_strescape().data());
      if (cimg::system(command,cimg::ffmpeg_path())!=0)
        throw CImgIOException(_cimglist_instance
                              "save_ffmpeg_external(): Failed to save file '%s' with external command 'ffmpeg'.",
                              cimglist_instance,
                              filename);
      if (!cimg::path_exists(filename))
        throw CImgIOException(_cimglist_instance
                              "save_ffmpeg_external(): Failed to save file '%s' with external command 'ffmpeg'.",
                              cimglist_instance,
                              filename);
      cimglist_for(*this,l) std::remove(filenames[l]);
      return *this;
    }

    //! Serialize a CImgList<T> instance into a raw CImg<unsigned char> buffer.
    /**
       \param is_compressed tells if zlib compression must be used for serialization
       (this requires 'cimg_use_zlib' been enabled).
       \param header_size Reserve empty bytes as a starting header.
    **/
    CImg<ucharT> get_serialize(const bool is_compressed=false, const unsigned int header_size=0) const {
#ifndef cimg_use_zlib
      if (is_compressed)
        cimg::warn(_cimglist_instance
                   "get_serialize(): Unable to compress data unless zlib is enabled, "
                   "storing them uncompressed.",
                   cimglist_instance);
#endif
      CImgList<ucharT> stream;
      if (header_size) CImg<ucharT>(1,header_size,1,1,0).move_to(stream);
      CImg<charT> tmpstr(128);
      const char *const ptype = pixel_type(), *const etype = cimg::endianness()?"big":"little";
      cimg_snprintf(tmpstr,tmpstr._width,"%u %s %s_endian\n",_width,ptype,etype);
      CImg<ucharT>::string(tmpstr,false).move_to(stream);
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_snprintf(tmpstr,tmpstr._width,"%u %u %u %u",img._width,img._height,img._depth,img._spectrum);
        CImg<ucharT>::string(tmpstr,false).move_to(stream);
        if (img._data) {
          CImg<T> tmp;
          if (cimg::endianness()) { tmp = img; cimg::invert_endianness(tmp._data,tmp.size()); }
          const CImg<T>& ref = cimg::endianness()?tmp:img;
          bool failed_to_compress = true;
          if (is_compressed) {
#ifdef cimg_use_zlib
            const ulongT siz = sizeof(T)*ref.size();
            uLongf csiz = (ulongT)compressBound(siz);
            Bytef *const cbuf = new Bytef[csiz];
            if (compress(cbuf,&csiz,(Bytef*)ref._data,siz))
              cimg::warn(_cimglist_instance
                         "get_serialize(): Failed to save compressed data, saving them uncompressed.",
                         cimglist_instance);
            else {
              cimg_snprintf(tmpstr,tmpstr._width," #%lu\n",csiz);
              CImg<ucharT>::string(tmpstr,false).move_to(stream);
              CImg<ucharT>(cbuf,csiz).move_to(stream);
              delete[] cbuf;
              failed_to_compress = false;
            }
#endif
          }
          if (failed_to_compress) { // Write in a non-compressed way
            CImg<charT>::string("\n",false).move_to(stream);
            stream.insert(1);
            stream.back().
              assign((unsigned char*)ref._data,ref._width,ref._height,ref._depth,ref._spectrum*sizeof(T),true);
          }
        } else CImg<charT>::string("\n",false).move_to(stream);
      }

      // Determine best serialized image dimensions to store the whole buffer.
      ulongT siz = 0;
      cimglist_for(stream,l) siz+=stream[l].size();
      const ulongT max_siz = (ulongT)cimg::type<int>::max();
      const unsigned int
        nw = (unsigned int)(siz/max_siz + ((siz%max_siz)?1:0)),
        nh = (unsigned int)(siz/nw + (siz%nw?1:0));
      CImg<ucharT> res(nw,nh,1,1,0);
      unsigned char *ptr = res.data();
      cimglist_for(stream,l) { siz = stream[l].size(); std::memcpy(ptr,stream[l]._data,siz); ptr+=siz; }
      return res;
    }

    //! Unserialize a CImg<unsigned char> serialized buffer into a CImgList<T> list.
    template<typename t>
    static CImgList<T> get_unserialize(const CImg<t>& buffer, const unsigned int header_size=0) {
#ifdef cimg_use_zlib
#define _cimgz_unserialize_case(Tss) { \
        Bytef *cbuf = 0; \
        if (sizeof(t)!=1 || buffer.pixel_type()==cimg::type<bool>::string()) { \
          cbuf = new Bytef[csiz]; Bytef *_cbuf = cbuf; \
          for (ulongT k = 0; k<csiz && stream<estream; ++k) *(_cbuf++) = (Bytef)*(stream++); \
          is_bytef = false; \
        } else { cbuf = (Bytef*)stream; stream+=csiz; is_bytef = true; } \
        raw.assign(W,H,D,C); \
        uLongf destlen = raw.size()*sizeof(Tss); \
        uncompress((Bytef*)raw._data,&destlen,cbuf,csiz); \
        if (!is_bytef) delete[] cbuf; \
      }
#else
#define _cimgz_unserialize_case(Tss) \
      throw CImgArgumentException("CImgList<%s>::get_unserialize(): Unable to unserialize compressed data " \
                                  "unless zlib is enabled.", \
                                  pixel_type());
#endif

#define _cimg_unserialize_case(Ts1,Ts2,Ts3,Tss) \
      if (!loaded && ((Ts1 && !cimg::strcasecmp(Ts1,str_pixeltype)) || \
                      (Ts2 && !cimg::strcasecmp(Ts2,str_pixeltype)) || \
                      (Ts3 && !cimg::strcasecmp(Ts3,str_pixeltype)))) { \
        for (unsigned int l = 0; l<N; ++l) { \
          j = 0; while ((i=(int)*stream)!='\n' && stream<estream && j<255) { ++stream; tmp[j++] = (char)i; } \
          ++stream; tmp[j] = 0; \
          W = H = D = C = 0; csiz = 0; \
          if ((err = cimg_sscanf(tmp,"%u %u %u %u #" cimg_fuint64,&W,&H,&D,&C,&csiz))<4) \
            throw CImgArgumentException("CImgList<%s>::unserialize(): Invalid specified size (%u,%u,%u,%u) for " \
                                        "image #%u in serialized buffer.", \
                                        pixel_type(),W,H,D,C,l); \
          if (W*H*D*C>0) { \
            CImg<Tss> raw; \
            CImg<T> &img = res._data[l]; \
            if (err==5) _cimgz_unserialize_case(Tss) \
            else { \
              raw.assign(W,H,D,C); \
              CImg<ucharT> _raw((unsigned char*)raw._data,W*sizeof(Tss),H,D,C,true); \
              if (sizeof(t)==1) { std::memcpy(_raw,stream,_raw.size()); stream+=_raw.size(); } \
              else cimg_for(_raw,p,unsigned char) *p = (unsigned char)*(stream++); \
            } \
            if (endian!=cimg::endianness()) cimg::invert_endianness(raw._data,raw.size()); \
            raw.move_to(img); \
          } \
        } \
        loaded = true; \
      }

      if (buffer.is_empty())
        throw CImgArgumentException("CImgList<%s>::get_unserialize(): Specified serialized buffer is (null).",
                                    pixel_type());
      CImgList<T> res;
      const t *stream = buffer._data + header_size, *const estream = buffer._data + buffer.size();
      bool loaded = false, endian = cimg::endianness(), is_bytef = false;
      CImg<charT> tmp(256), str_pixeltype(256), str_endian(256);
      *tmp = *str_pixeltype = *str_endian = 0;
      unsigned int j, N = 0, W, H, D, C;
      uint64T csiz;
      int i, err;
      cimg::unused(is_bytef);
      do {
        j = 0; while ((i=(int)*stream)!='\n' && stream<estream && j<255) { ++stream; tmp[j++] = (char)i; }
        ++stream; tmp[j] = 0;
      } while (*tmp=='#' && stream<estream);
      err = cimg_sscanf(tmp,"%u%*c%255[A-Za-z123468_]%*c%255[sA-Za-z_ ]",
                        &N,str_pixeltype._data,str_endian._data);
      if (err<2)
        throw CImgArgumentException("CImgList<%s>::get_unserialize(): CImg header not found in serialized buffer.",
                                    pixel_type());
      if (!cimg::strncasecmp("little",str_endian,6)) endian = false;
      else if (!cimg::strncasecmp("big",str_endian,3)) endian = true;
      res.assign(N);
      _cimg_unserialize_case("bool",0,0,cimg_uint8);
      _cimg_unserialize_case("uint8","unsigned_char","uchar",cimg_uint8);
      _cimg_unserialize_case("int8",0,0,cimg_int8);
      _cimg_unserialize_case("char",0,0,char);
      _cimg_unserialize_case("uint16","unsigned_short","ushort",cimg_uint16);
      _cimg_unserialize_case("int16","short",0,cimg_int16);
      _cimg_unserialize_case("uint32","unsigned_int","uint",cimg_uint32);
      _cimg_unserialize_case("int32","int",0,cimg_int32);
      _cimg_unserialize_case("uint64","unsigned_int64",0,cimg_uint64);
      _cimg_unserialize_case("int64",0,0,cimg_int64);
#if cimg_is_float16==1
      _cimg_unserialize_case("float16","half",0,cimg_float16);
#endif
      _cimg_unserialize_case("float32","float",0,cimg_float32);
      _cimg_unserialize_case("float64","double",0,cimg_float64);
      if (!loaded)
        throw CImgArgumentException("CImgList<%s>::get_unserialize(): Unsupported pixel type '%s' defined "
                                    "in serialized buffer.",
                                    pixel_type(),str_pixeltype._data);
      return res;
    }

    //@}
    //----------------------------------
    //
    //! \name Others
    //@{
    //----------------------------------

    //! Return a CImg pre-defined font with requested height.
    /**
       \param font_height Height of the desired font (exact match for 13,23,53,103).
       \param is_variable_width Decide if the font has a variable (\c true) or fixed (\c false) width.
    **/
    static const CImgList<ucharT>& font(const unsigned int requested_height, const bool is_variable_width=true) {
      if (!requested_height) return CImgList<ucharT>::const_empty();
      cimg::mutex(11);
      static const unsigned char font_resizemap[] = {
        0, 4, 7, 9, 11, 13, 15, 17, 19, 21, 22, 24, 26, 27, 29, 30,
        32, 33, 35, 36, 38, 39, 41, 42, 43, 45, 46, 47, 49, 50, 51, 52,
        54, 55, 56, 58, 59, 60, 61, 62, 64, 65, 66, 67, 68, 69, 71, 72,
        73, 74, 75, 76, 77, 78, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
        90, 91, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
        107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
        123, 124, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
        138, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 148, 149, 150, 151,
        152, 153, 154, 155, 156, 157, 157, 158, 159, 160, 161, 162, 163, 164, 164, 165,
        166, 167, 168, 169, 170, 170, 171, 172, 173, 174, 175, 176, 176, 177, 178, 179,
        180, 181, 181, 182, 183, 184, 185, 186, 186, 187, 188, 189, 190, 191, 191, 192,
        193, 194, 195, 196, 196, 197, 198, 199, 200, 200, 201, 202, 203, 204, 205, 205,
        206, 207, 208, 209, 209, 210, 211, 212, 213, 213, 214, 215, 216, 216, 217, 218,
        219, 220, 220, 221, 222, 223, 224, 224, 225, 226, 227, 227, 228, 229, 230, 231,
        231, 232, 233, 234, 234, 235, 236, 237, 238, 238, 239, 240, 241, 241, 242, 243,
        244, 244, 245, 246, 247, 247, 248, 249, 250, 250, 251, 252, 253, 253, 254, 255 };
      static const char *const *font_data[] = {
        cimg::data_font_small,
        cimg::data_font_normal,
        cimg::data_font_large,
        cimg::data_font_huge };
      static const unsigned int
        font_width[] = { 10,26,52,104 },
        font_height[] = { 13,32,64,128 },
        font_M[] = { 86,91,91,47 },
        font_chunk[] = { sizeof(cimg::data_font_small)/sizeof(char*),
                         sizeof(cimg::data_font_normal)/sizeof(char*),
                         sizeof(cimg::data_font_large)/sizeof(char*),
                         sizeof(cimg::data_font_huge)/sizeof(char*) };
      static const unsigned char font_is_binary[] = { 1,0,0,1 };
      static CImg<ucharT> font_base[4];

      unsigned int ind =
        requested_height<=font_height[0]?0U:
        requested_height<=font_height[1]?1U:
        requested_height<=font_height[2]?2U:3U;

      // Decompress nearest base font data if needed.
      CImg<ucharT> &basef = font_base[ind];
      if (!basef) {
        basef.assign(256*font_width[ind],font_height[ind]);

        unsigned char *ptrd = basef;
        const unsigned char *const ptrde = basef.end();

        // Recompose font data from several chunks, to deal with MS compiler limit with big strings (64 Kb).
        CImg<char> dataf;
        for (unsigned int k = 0; k<font_chunk[ind]; ++k)
          dataf.append(CImg<char>::string(font_data[ind][k],k==font_chunk[ind] - 1,true),'x');

        // Uncompress font data (decode RLE).
        const unsigned int M = font_M[ind];
        if (font_is_binary[ind])
          for (const char *ptrs = dataf; *ptrs; ++ptrs) {
            const int _n = (int)(*ptrs - M - 32), v = _n>=0?255:0, n = _n>=0?_n:-_n;
            if (ptrd + n<=ptrde) { std::memset(ptrd,v,n); ptrd+=n; }
            else { std::memset(ptrd,v,ptrde - ptrd); break; }
          }
        else
          for (const char *ptrs = dataf; *ptrs; ++ptrs) {
            int n = (int)*ptrs - M - 32, v = 0;
            if (n>=0) { v = 85*n; n = 1; }
            else {
              n = -n;
              v = (int)*(++ptrs) - M - 32;
              if (v<0) { v = 0; --ptrs; } else v*=85;
            }
            if (ptrd + n<=ptrde) { std::memset(ptrd,v,n); ptrd+=n; }
            else { std::memset(ptrd,v,ptrde - ptrd); break; }
          }
      }

      // Find optimal font cache location to return.
      static CImgList<ucharT> fonts[16];
      static bool is_variable_widths[16] = {};
      ind = ~0U;
      for (int i = 0; i<16; ++i)
        if (!fonts[i] || (is_variable_widths[i]==is_variable_width && requested_height==fonts[i][0]._height)) {
          ind = (unsigned int)i; break; // Found empty slot or cached font
        }
      if (ind==~0U) { // No empty slots nor existing font in cache
        fonts->assign();
        std::memmove((void*)fonts,(void*)(fonts + 1),15*sizeof(CImgList<ucharT>));
        std::memmove(is_variable_widths,is_variable_widths + 1,15*sizeof(bool));
        std::memset((void*)(fonts + (ind=15)),0,sizeof(CImgList<ucharT>)); // Free a slot in cache for new font
      }
      CImgList<ucharT> &font = fonts[ind];

      // Render requested font.
      if (!font) {
        is_variable_widths[ind] = is_variable_width;
        basef.get_split('x',256).move_to(font);
        if (requested_height!=font[0]._height)
          cimglist_for(font,l) {
            font[l].resize(std::max(1U,font[l]._width*requested_height/font[l]._height),requested_height,-100,-100,5);
            cimg_for(font[l],ptr,ucharT) *ptr = font_resizemap[*ptr];
          }
        if (is_variable_width) { // Crop font
          cimglist_for(font,l) {
            CImg<ucharT>& letter = font[l];
            int xmin = letter.width(), xmax = 0;
            cimg_forX(letter,x) { // Find xmin
              cimg_forY(letter,y) if (letter(x,y)) { xmin = x; break; }
              if (xmin!=letter.width()) break;
            }
            cimg_rofX(letter,x) { // Find xmax
              cimg_forY(letter,y) if (letter(x,y)) { xmax = x; break; }
              if (xmax) break;
            }
            if (xmin<=xmax) letter.crop(xmin,0,xmax,letter._height - 1);
          }
          font[(int)' '].resize(font[(int)'f']._width,-100,-100,-100,0);
          if (' ' + 256<font.size()) font[' ' + 256].resize(font[(int)'f']._width,-100,-100,-100,0);
        }
        font.insert(256,0);
        cimglist_for_in(font,0,255,l) font[l].assign(font[l + 256]._width,font[l + 256]._height,1,1,255);
      }
      cimg::mutex(11,0);
      return font;
    }

    //! Compute a 1D Fast Fourier Transform, along specified axis.
    /**
       \param axis Axis along which the Fourier transform is computed.
       \param invert Tells if the direct (\c false) or inverse transform (\c true) is computed.
    **/
    CImgList<T>& FFT(const char axis, const bool invert=false) {
      if (is_empty()) return *this;
      if (_width==1) insert(1);
      if (_width>2)
        cimg::warn(_cimglist_instance
                   "FFT(): Instance has more than 2 images",
                   cimglist_instance);
      CImg<T>::FFT(_data[0],_data[1],axis,invert);
      return *this;
    }

    //! Compute a 1-D Fast Fourier Transform, along specified axis \newinstance.
    CImgList<Tfloat> get_FFT(const char axis, const bool invert=false) const {
      return CImgList<Tfloat>(*this,false).FFT(axis,invert);
    }

    //! Compute n-D Fast Fourier Transform.
    /**
      \param invert Tells if the direct (\c false) or inverse transform (\c true) is computed.
    **/
    CImgList<T>& FFT(const bool invert=false) {
      if (is_empty()) return *this;
      if (_width==1) insert(1);
      if (_width>2)
        cimg::warn(_cimglist_instance
                   "FFT(): Instance has more than 2 images",
                   cimglist_instance);

      CImg<T>::FFT(_data[0],_data[1],invert);
      return *this;
    }

    //! Compute n-D Fast Fourier Transform \newinstance.
    CImgList<Tfloat> get_FFT(const bool invert=false) const {
      return CImgList<Tfloat>(*this,false).FFT(invert);
    }

    //! Reverse primitives orientations of a 3D object.
    /**
    **/
    CImgList<T>& reverse_object3d() {
      cimglist_for(*this,l) {
        CImg<T>& p = _data[l];
        switch (p.size()) {
        case 2 : case 3: cimg::swap(p[0],p[1]); break;
        case 6 : cimg::swap(p[0],p[1],p[2],p[4],p[3],p[5]); break;
        case 9 : cimg::swap(p[0],p[1],p[3],p[5],p[4],p[6]); break;
        case 4 : cimg::swap(p[0],p[1],p[2],p[3]); break;
        case 12 : cimg::swap(p[0],p[1],p[2],p[3],p[4],p[6],p[5],p[7],p[8],p[10],p[9],p[11]); break;
        }
      }
      return *this;
    }

    //! Reverse primitives orientations of a 3D object \newinstance.
    CImgList<T> get_reverse_object3d() const {
      return (+*this).reverse_object3d();
    }

    //@}
  }; // struct CImgList { ...

#endif
