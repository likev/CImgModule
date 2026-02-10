#ifndef CIMG_MODULE_IO_HELPERS_H
#define CIMG_MODULE_IO_HELPERS_H

    // Functions to return standard streams 'stdin', 'stdout' and 'stderr'.
    // (throw a CImgIOException when macro 'cimg_use_r' is defined).
    inline FILE* _stdin(const bool throw_exception) {
#ifndef cimg_use_r
      cimg::unused(throw_exception);
      return stdin;
#else
      if (throw_exception) {
        cimg::exception_mode(0);
        throw CImgIOException("cimg::stdin(): Reference to 'stdin' stream not allowed in R mode "
                              "('cimg_use_r' is defined).");
      }
      return 0;
#endif
    }

    inline FILE* _stdout(const bool throw_exception) {
#ifndef cimg_use_r
      cimg::unused(throw_exception);
      return stdout;
#else
      if (throw_exception) {
        cimg::exception_mode(0);
        throw CImgIOException("cimg::stdout(): Reference to 'stdout' stream not allowed in R mode "
                              "('cimg_use_r' is defined).");
      }
      return 0;
#endif
    }

    inline FILE* _stderr(const bool throw_exception) {
#ifndef cimg_use_r
      cimg::unused(throw_exception);
      return stderr;
#else
      if (throw_exception) {
        cimg::exception_mode(0);
        throw CImgIOException("cimg::stderr(): Reference to 'stderr' stream not allowed in R mode "
                              "('cimg_use_r' is defined).");
      }
      return 0;
#endif
    }

    // Open a file (similar to std:: fopen(), but with wide character support on Windows).
    inline std::FILE *std_fopen(const char *const path, const char *const mode) {
      std::FILE *const res = std::fopen(path,mode);
      if (res) return res;
#if cimg_OS==2
      // Try alternative method, with wide-character string.
      int err = MultiByteToWideChar(CP_UTF8,0,path,-1,0,0);
      if (err) {
        CImg<wchar_t> wpath((unsigned int)err);
        err = MultiByteToWideChar(CP_UTF8,0,path,-1,wpath,err);
        if (err) { // Convert 'mode' to a wide-character string
          err = MultiByteToWideChar(CP_UTF8,0,mode,-1,0,0);
          if (err) {
            CImg<wchar_t> wmode((unsigned int)err);
            if (MultiByteToWideChar(CP_UTF8,0,mode,-1,wmode,err))
              return _wfopen(wpath,wmode);
          }
        }
      }
#endif
      return 0;
    }

    //! Search path of an executable.
#if cimg_OS==2
    inline bool win_searchpath(const char *const exec_name, char *const res, const unsigned int size_res) {
      char *ptr = 0;
      DWORD err = SearchPathA(0,exec_name,0,size_res,res,&ptr);
      return err!=0;
    }
#endif

#if cimg_OS==1
    inline bool posix_searchpath(const char *file) {
      if (!file || !*file) return false;
      const char *path = std::getenv("PATH");

      if (!path) path = "/usr/local/bin:/bin:/usr/bin";
      size_t file_len = strnlen(file,NAME_MAX + 1);
      if (file_len>NAME_MAX) return false;
      size_t path_total_len = strnlen(path,PATH_MAX - 1) + 1;

      char *buf = new char[path_total_len + file_len + 1];
      const char *p = path, *z = 0;
      while (true) {
        z = std::strchr(p,':');
        if (!z) z = p + std::strlen(p);
        if ((size_t)(z - p)>=path_total_len) {
          if (!*z++) break;
          continue;
        }
        std::memcpy(buf,p,z - p);
        buf[z - p] = '/';
        std::memcpy(buf + (z - p) + (z>p),file,file_len + 1);
        if (cimg::is_file(buf) && faccessat(AT_FDCWD, buf, X_OK, AT_EACCESS)==0) { delete[] buf; return true; }
        if (!*z++) break;
        p = z;
      }
      delete[] buf;
      return false;
    }
#endif

    //! Get the file or directory attributes with support for UTF-8 paths (Windows only).
#if cimg_OS==2
    inline DWORD win_getfileattributes(const char *const path) {
      DWORD res = GetFileAttributesA(path);
      if (res==INVALID_FILE_ATTRIBUTES) {
        // Try alternative method, with wide-character string.
        int err = MultiByteToWideChar(CP_UTF8,0,path,-1,0,0);
        if (err) {
          CImg<wchar_t> wpath((unsigned int)err);
          if (MultiByteToWideChar(CP_UTF8,0,path,-1,wpath,err)) res = GetFileAttributesW(wpath);
        }
      }
      return res;
    }
#endif

    //! Get/set path to the <i>Program Files/</i> directory (Windows only).
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the program files.
    **/
#if cimg_OS==2
    inline const char* win_programfiles_path(const char *const user_path=0, const bool reinit_path=false) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(MAX_PATH);
        *s_path = 0;
        // Note: in the following line, 0x26 = CSIDL_PROGRAM_FILES (not defined on every compiler).
#if !defined(__INTEL_COMPILER)
        if (!SHGetSpecialFolderPathA(0,s_path,0x0026,false)) {
          const char *const pfPath = std::getenv("PROGRAMFILES");
          if (pfPath) std::strncpy(s_path,pfPath,MAX_PATH - 1);
          else std::strcpy(s_path,"C:\\PROGRA~1");
        }
#else
        std::strcpy(s_path,"C:\\PROGRA~1");
#endif
      }
      cimg::mutex(7,0);
      return s_path;
    }
#endif

    //! Get/set path to the \c curl binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c curl binary.
    **/
    inline const char *curl_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
#if cimg_OS==2
        if (win_searchpath("curl.exe",s_path,s_path._width)) path_found = true;
        if (!path_found) {
          std::strcpy(s_path,".\\curl.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"curl.exe");
#else
        if (!path_found) {
          std::strcpy(s_path,"./curl");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"curl");
#endif
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }

    //! Get/set path to the \c dcraw binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c dcraw binary.
    **/
    inline const char *dcraw_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
#if cimg_OS==2
        if (win_searchpath("dcraw.exe",s_path,s_path._width)) path_found = true;
        if (!path_found) {
          std::strcpy(s_path,".\\dcraw.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"dcraw.exe");
#else
        if (!path_found) {
          std::strcpy(s_path,"./dcraw");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"dcraw");
#endif
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }

    //! Get/set path to the FFMPEG's \c ffmpeg binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c ffmpeg binary.
    **/
    inline const char *ffmpeg_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
#if cimg_OS==2
        if (win_searchpath("ffmpeg.exe",s_path,s_path._width)) path_found = true;
        if (!path_found) {
          std::strcpy(s_path,".\\ffmpeg.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"ffmpeg.exe");
#else
        if (!path_found) {
          std::strcpy(s_path,"./ffmpeg");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"ffmpeg");
#endif
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }

    //! Get/set path to the GraphicsMagick's \c gm binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c gm binary.
    **/
    inline const char* graphicsmagick_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
#if cimg_OS==2
        if (win_searchpath("gm.exe",s_path,s_path._width)) path_found = true;
        const char *const pf_path = win_programfiles_path();
        if (!path_found) {
          std::strcpy(s_path,".\\gm.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%.2d-\\gm.exe",pf_path,k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%d-Q\\gm.exe",pf_path,k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%d\\gm.exe",pf_path,k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%.2d-\\VISUA~1\\BIN\\gm.exe",pf_path,k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%d-Q\\VISUA~1\\BIN\\gm.exe",pf_path,k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%d\\VISUA~1\\BIN\\gm.exe",pf_path,k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%.2d-\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%d-Q\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%d\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%.2d-\\VISUA~1\\BIN\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%d-Q\\VISUA~1\\BIN\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%d\\VISUA~1\\BIN\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%.2d-\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%d-Q\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%d\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%.2d-\\VISUA~1\\BIN\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%d-Q\\VISUA~1\\BIN\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%d\\VISUA~1\\BIN\\gm.exe",k);
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"gm.exe");
#else
        if (!path_found) {
          std::strcpy(s_path,"./gm");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"gm");
#endif
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }

    //! Get/set path to the \c gunzip binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c gunzip binary.
    **/
    inline const char *gunzip_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
#if cimg_OS==2
        if (win_searchpath("gunzip.exe",s_path,s_path._width)) path_found = true;
        if (!path_found) {
          std::strcpy(s_path,".\\gunzip.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"gunzip.exe");
#else
        if (!path_found) {
          std::strcpy(s_path,"./gunzip");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"gunzip");
#endif
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }

    //! Get/set path to the \c gzip binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c gzip binary.
    **/
    inline const char *gzip_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
#if cimg_OS==2
        if (win_searchpath("gzip.exe",s_path,s_path._width)) path_found = true;
        if (!path_found) {
          std::strcpy(s_path,".\\gzip.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"gzip.exe");
#else
        if (!path_found) {
          std::strcpy(s_path,"./gzip");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"gzip");
#endif
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }

    //! Get/set path to the ImageMagick's \c convert binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c convert binary.
    **/
    inline const char* imagemagick_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
#if cimg_OS==2
        if (win_searchpath("magick.exe",s_path,s_path._width)) path_found = true;
        const char *const pf_path = win_programfiles_path();
        for (int l = 0; l<2 && !path_found; ++l) {
          const char *const s_exe = l?"convert":"magick";
          cimg_snprintf(s_path,s_path._width,".\\%s.exe",s_exe);
          if (cimg::is_file(s_path)) path_found = true;
          for (int k = 32; k>=10 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%.2d-\\%s.exe",pf_path,k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 9; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%d-Q\\%s.exe",pf_path,k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%d\\%s.exe",pf_path,k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=10 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%.2d-\\VISUA~1\\BIN\\%s.exe",pf_path,k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 9; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%d-Q\\VISUA~1\\BIN\\%s.exe",pf_path,k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%d\\VISUA~1\\BIN\\%s.exe",pf_path,k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=10 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%.2d-\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 9; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%d-Q\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%d\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=10 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%.2d-\\VISUA~1\\BIN\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 9; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%d-Q\\VISUA~1\\BIN\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%d\\VISUA~1\\BIN\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=10 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%.2d-\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 9; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%d-Q\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%d\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=10 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%.2d-\\VISUA~1\\BIN\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 9; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%d-Q\\VISUA~1\\BIN\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          for (int k = 32; k>=0 && !path_found; --k) {
            cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%d\\VISUA~1\\BIN\\%s.exe",k,s_exe);
            if (cimg::is_file(s_path)) path_found = true;
          }
          if (!path_found) cimg_snprintf(s_path,s_path._width,"%s.exe",s_exe);
        }
#else
        std::strcpy(s_path,"./magick");
        if (cimg::is_file(s_path)) path_found = true;
        if (!path_found) {
          std::strcpy(s_path,"./convert");
          if (cimg::is_file(s_path)) path_found = true;
        }
#if cimg_OS==1
        if (!path_found) {
          std::strcpy(s_path,"magick");
          if (cimg::posix_searchpath("magick")) path_found = true;
        }
#endif
        if (!path_found) std::strcpy(s_path,"convert");
#endif
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }

    //! Get/set path to the Medcon's \c medcon binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c medcon binary.
    **/
    inline const char* medcon_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
#if cimg_OS==2
        if (win_searchpath("medcon.exe",s_path,s_path._width)) path_found = true;
        const char *const pf_path = win_programfiles_path();
        if (!path_found) {
          std::strcpy(s_path,".\\medcon.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) {
          cimg_snprintf(s_path,s_path._width,"%s\\XMedCon\\bin\\medcon.bat",pf_path);
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) {
          cimg_snprintf(s_path,s_path._width,"%s\\XMedCon\\bin\\medcon.exe",pf_path);
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) {
          std::strcpy(s_path,"C:\\XMedCon\\bin\\medcon.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"medcon.exe");
#else
        if (!path_found) {
          std::strcpy(s_path,"./medcon");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"medcon");
#endif
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }

    //! Get/set path to store temporary files.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path where temporary files can be saved.
    **/
    inline const char* temporary_path(const char *const user_path, const bool reinit_path) {
#define _cimg_test_temporary_path(p) \
      if (!path_found) { \
        cimg_snprintf(s_path,s_path._width,"%s",p); \
        cimg_snprintf(tmp,tmp._width,"%s%c%s",s_path.data(),cimg_file_separator,filename_tmp._data); \
        if ((file=cimg::std_fopen(tmp,"wb"))!=0) { cimg::fclose(file); std::remove(tmp); path_found = true; } \
      }
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
        CImg<char> tmp(1024), filename_tmp(256);
        std::FILE *file = 0;
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s.tmp",cimg::filenamerand());
        char *tmpPath = std::getenv("TMP");
        if (!tmpPath) { tmpPath = std::getenv("TEMP"); winformat_string(tmpPath); }
        if (tmpPath) _cimg_test_temporary_path(tmpPath);
#if cimg_OS==2
        _cimg_test_temporary_path("C:\\WINNT\\Temp");
        _cimg_test_temporary_path("C:\\WINDOWS\\Temp");
        _cimg_test_temporary_path("C:\\Temp");
        _cimg_test_temporary_path("C:");
        _cimg_test_temporary_path("D:\\WINNT\\Temp");
        _cimg_test_temporary_path("D:\\WINDOWS\\Temp");
        _cimg_test_temporary_path("D:\\Temp");
        _cimg_test_temporary_path("D:");
#else
        _cimg_test_temporary_path("/tmp");
        _cimg_test_temporary_path("/var/tmp");
#endif
        if (!path_found) {
          *s_path = 0;
          std::strncpy(tmp,filename_tmp,tmp._width - 1);
          if ((file=cimg::std_fopen(tmp,"wb"))!=0) { cimg::fclose(file); std::remove(tmp); path_found = true; }
        }
        if (!path_found) {
          cimg::mutex(7,0);
          throw CImgIOException("cimg::temporary_path(): Failed to locate path for writing temporary files.\n");
        }
      }
      cimg::mutex(7,0);
      return s_path;
    }

    //! Get/set path to the \c wget binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c wget binary.
    **/
    inline const char *wget_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
#if cimg_OS==2
        if (win_searchpath("wget.exe",s_path,s_path._width)) path_found = true;
        if (!path_found) {
          std::strcpy(s_path,".\\wget.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"wget.exe");
#else
        if (!path_found) {
          std::strcpy(s_path,"./wget");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"wget");
#endif
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }

#if cimg_OS==2
    //! Get/set path to the \c powershell binary.
    /**
       \param user_path Specified path, or \c 0 to get the path currently used.
       \param reinit_path Force path to be recalculated (may take some time).
       \return Path containing the \c wget binary.
    **/
    inline const char *powershell_path(const char *const user_path, const bool reinit_path) {
      static CImg<char> s_path;
      cimg::mutex(7);
      if (reinit_path) s_path.assign();
      if (user_path) {
        if (!s_path) s_path.assign(1024);
        std::strncpy(s_path,user_path,1023);
      } else if (!s_path) {
        s_path.assign(1024);
        bool path_found = false;
        if (win_searchpath("powershell.exe",s_path,s_path._width)) path_found = true;
        if (!path_found) {
          std::strcpy(s_path,".\\powershell.exe");
          if (cimg::is_file(s_path)) path_found = true;
        }
        if (!path_found) std::strcpy(s_path,"powershell.exe");
        winformat_string(s_path);
      }
      cimg::mutex(7,0);
      return s_path;
    }
#endif

    // [internal] Sorting function, used by cimg::files().
    inline int _sort_files(const void* a, const void* b) {
      const CImg<char> &sa = *(CImg<char>*)a, &sb = *(CImg<char>*)b;
      return std::strcmp(sa._data,sb._data);
    }

    //! Generate a numbered version of a filename.
    inline char* number_filename(const char *const filename, const int number,
                                 const unsigned int digits, char *const str) {
      if (!filename) { if (str) *str = 0; return 0; }
      const unsigned int siz = (unsigned int)std::strlen(filename);
      CImg<char> format(16), body(siz + 32);
      const char *const ext = cimg::split_filename(filename,body);
      if (*ext) cimg_snprintf(format,format._width,"%%s_%%.%ud.%%s",digits);
      else cimg_snprintf(format,format._width,"%%s_%%.%ud",digits);
      cimg_snprintf(str,1024,format._data,body._data,number,ext);
      return str;
    }

    //! Return list of files/directories in specified directory.
    /**
       \param path Path to the directory. Set to 0 for current directory.
       \param is_pattern Tell if specified path has a matching pattern in it.
       \param mode Output type, can be primary { 0=files only | 1=folders only | 2=files + folders }.
       \param include_path Tell if \c path must be included in resulting filenames.
       \return A list of filenames.
    **/
    inline CImgList<char> files(const char *const path, const bool is_pattern=false,
                                const unsigned int mode=2, const bool include_path=false) {
      if (!path || !*path) return files("*",true,mode,include_path);
      CImgList<char> res;

      // If path is a valid folder name, ignore argument 'is_pattern'.
      const bool _is_pattern = is_pattern && !cimg::is_directory(path);
      bool is_root = false, is_current = false;
      cimg::unused(is_root,is_current);

      // Clean format of input path.
      CImg<char> pattern, _path = CImg<char>::string(path);
#if cimg_OS==2
      for (char *ps = _path; *ps; ++ps) if (*ps=='\\') *ps='/';
#endif
      char *pd = _path;
      for (char *ps = pd; *ps; ++ps) { if (*ps!='/' || *ps!=*(ps+1)) *(pd++) = *ps; }
      *pd = 0;
      unsigned int lp = (unsigned int)std::strlen(_path);
      if (!_is_pattern && lp && _path[lp - 1]=='/') {
        _path[lp - 1] = 0; --lp;
#if cimg_OS!=2
        is_root = !*_path;
#endif
      }

      // Separate folder path and matching pattern.
      if (_is_pattern) {
        const unsigned int bpos = (unsigned int)(cimg::basename(_path,'/') - _path.data());
        CImg<char>::string(_path).move_to(pattern);
        if (bpos) {
          _path[bpos - 1] = 0; // End 'path' at last slash
#if cimg_OS!=2
          is_root = !*_path;
#endif
        } else { // No path to folder specified, assuming current folder
          is_current = true; *_path = 0;
        }
        lp = (unsigned int)std::strlen(_path);
      }

      // Windows version.
#if cimg_OS==2
      if (!_is_pattern) {
        pattern.assign(lp + 3);
        std::memcpy(pattern,_path,lp);
        pattern[lp] = '/'; pattern[lp + 1] = '*'; pattern[lp + 2] = 0;
      }
      WIN32_FIND_DATAA file_data;
      const HANDLE dir = FindFirstFileA(pattern.data(),&file_data);
      if (dir==INVALID_HANDLE_VALUE) return CImgList<char>::const_empty();
      do {
        const char *const filename = file_data.cFileName;
        if (*filename!='.' || (filename[1] && (filename[1]!='.' || filename[2]))) {
          const unsigned int lf = (unsigned int)std::strlen(filename);
          const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0;
          if ((!mode && !is_directory) || (mode==1 && is_directory) || mode>=2) {
            if (include_path) {
              CImg<char> full_filename((lp?lp+1:0) + lf + 1);
              if (lp) { std::memcpy(full_filename,_path,lp); full_filename[lp] = '/'; }
              std::memcpy(full_filename._data + (lp?lp + 1:0),filename,lf + 1);
              full_filename.move_to(res);
            } else CImg<char>(filename,lf + 1).move_to(res);
          }
        }
      } while (FindNextFileA(dir,&file_data));
      FindClose(dir);

      // Unix version (posix).
#elif cimg_OS==1
      DIR *const dir = opendir(is_root?"/":is_current?".":_path.data());
      if (!dir) return CImgList<char>::const_empty();
      struct dirent *ent;
      while ((ent=readdir(dir))!=0) {
        const char *const filename = ent->d_name;
        if (*filename!='.' || (filename[1] && (filename[1]!='.' || filename[2]))) {
          const unsigned int lf = (unsigned int)std::strlen(filename);
          CImg<char> full_filename(lp + lf + 2);

          if (!is_current) {
            full_filename.assign(lp + lf + 2);
            if (lp) std::memcpy(full_filename,_path,lp);
            full_filename[lp] = '/';
            std::memcpy(full_filename._data + lp + 1,filename,lf + 1);
          } else full_filename.assign(filename,lf + 1);

          struct stat st;
          if (stat(full_filename,&st)==-1) continue;
          const bool is_directory = (st.st_mode & S_IFDIR)!=0;
          if ((!mode && !is_directory) || (mode==1 && is_directory) || mode==2) {
            if (include_path) {
              if (!_is_pattern || (_is_pattern && !fnmatch(pattern,full_filename,0)))
                full_filename.move_to(res);
            } else {
              if (!_is_pattern || (_is_pattern && !fnmatch(pattern,full_filename,0)))
                CImg<char>(filename,lf + 1).move_to(res);
            }
          }
        }
      }
      closedir(dir);
#endif

      // Sort resulting list by lexicographic order.
      if (res._width>=2) std::qsort(res._data,res._width,sizeof(CImg<char>),_sort_files);
      return res;
    }

    inline void _create_directory(const char *const dirname, const bool force_overwrite) {
      bool is_error = false;
      if (cimg::is_directory(dirname)) return;
      if (cimg::is_file(dirname)) { // In case 'dirname' is already an existing filename
        if (!force_overwrite) is_error = true;
        else {
#if cimg_OS==2
          is_error = !DeleteFileA(dirname);
#elif cimg_OS==1
          is_error = (bool)std::remove(dirname);
#endif
        }
      }
      if (!is_error) {
#if cimg_OS==2
        if (!CreateDirectoryA(dirname,0)) {
          // The path may be UTF-8, convert it to a wide-character string and try again.
          const int wideLength = MultiByteToWideChar(CP_UTF8,0,dirname,-1,0,0);
          if (!wideLength) is_error = true;
          else {
            CImg<wchar_t> wpath(wideLength);
            if (!MultiByteToWideChar(CP_UTF8,0,dirname,-1,wpath,wideLength)) is_error = true;
            else {
              DeleteFileW(wpath);
              is_error = !CreateDirectoryW(wpath,0);
            }
          }
        }
#elif cimg_OS==1
        is_error = (bool)mkdir(dirname,0777);
#endif
      }
      if (is_error)
        throw CImgIOException("cimg::create_dir(): Failed to create directory '%s'.",dirname);
    }

    //! Create a directory.
    /**
       \param dirname The path of the directory to create.
       \param force_overwrite Force overwrite of the directory when necessary.
    **/
    inline void create_directory(const char *const dirname, const bool force_overwrite) {
#if cimg_OS==2
      const char *const cs = "/\\";
#else
      const char *const cs = "/";
#endif
      const char *ptr0 = dirname, *ptr1 = std::strpbrk(ptr0,cs);
      if (ptr1) {
        while (ptr1) { // Manage case where 'dirname' is a path with separators (multiple directories)
          if (ptr1!=ptr0) {
            CImg<char> subdir(dirname,ptr1 - dirname + 1);
            subdir.back() = 0;
            _create_directory(subdir,force_overwrite);
          }
          ptr0 = ++ptr1;
          ptr1 = std::strpbrk(ptr0,cs);
        }
#if cimg_OS==2
        const bool cond = *ptr0!='/' && *ptr0!='\\';
#else
        const bool cond = *ptr0!='/';
#endif
        if (*ptr0 && cond) _create_directory(dirname,force_overwrite);
      } else _create_directory(dirname,force_overwrite);
    }

    //! Try to guess format from an image file.
    /**
       \param file Input file (can be \c 0 if \c filename is set).
       \param filename Filename, as a C-string (can be \c 0 if \c file is set).
       \return C-string containing the guessed file format, or \c 0 if nothing has been guessed.
    **/
    inline const char *ftype(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException("cimg::ftype(): Specified filename is (null).");
      static const char
        *const _bmp = "bmp",
        *const _cr2 = "cr2",
        *const _dcm = "dcm",
        *const _gif = "gif",
        *const _heif = "heif",
        *const _inr = "inr",
        *const _jpg = "jpg",
        *const _off = "off",
        *const _pan = "pan",
        *const _pfm = "pfm",
        *const _png = "png",
        *const _pnm = "pnm",
        *const _tif = "tif",
        *const _webp = "webp",
        *const _jxl = "jxl";

      const char *f_type = 0;
      CImg<char> header;
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      try {
        header._load_raw(file,filename,512,1,1,1,false,false,0);
        const unsigned char *const uheader = (unsigned char*)header._data;
        if (!std::strncmp(header,"OFF\n",4)) f_type = _off; // OFF
        else if (!std::strncmp(header,"#INRIMAGE",9)) // INRIMAGE
          f_type = _inr;
        else if (!std::strncmp(header,"PANDORE",7)) // PANDORE
          f_type = _pan;
        else if (!std::strncmp(header.data() + 128,"DICM",4)) // DICOM
          f_type = _dcm;
        else if (uheader[0]==0xFF && uheader[1]==0xD8 && uheader[2]==0xFF) // JPEG
          f_type = _jpg;
        else if (header[0]=='B' && header[1]=='M') // BMP
          f_type = _bmp;
        else if (header[0]=='G' && header[1]=='I' && header[2]=='F' && header[3]=='8' && header[5]=='a' &&
                 (header[4]=='7' || header[4]=='9')) // GIF
          f_type = _gif;
        else if (uheader[0]==0x89 && uheader[1]==0x50 && uheader[2]==0x4E && uheader[3]==0x47 &&
                 uheader[4]==0x0D && uheader[5]==0x0A && uheader[6]==0x1A && uheader[7]==0x0A) // PNG
          f_type = _png;
        else if (uheader[0]==0x49 && uheader[1]==0x49 && uheader[2]==0x2A && uheader[3]==0x00 && // CR2
                 uheader[4]==0x10 && uheader[5]==0x00 && uheader[6]==0x00 && uheader[7]==0x00 &&
                 uheader[8]==0x43 && uheader[9]==0x52)
          f_type = _cr2;
        else if ((uheader[0]==0x49 && uheader[1]==0x49 && uheader[2]==0x2A && uheader[3]==0x00) ||
                 (uheader[0]==0x4D && uheader[1]==0x4D && uheader[2]==0x00 && uheader[3]==0x2A)) // TIFF
          f_type = _tif;
        else if (uheader[0]==0x52 && uheader[1]==0x49 && uheader[2]==0x46 && uheader[3]==0x46 &&
                 uheader[8]==0x57 && uheader[9]==0x45 && uheader[10]==0x42 && uheader[11]==0x50) // WebP
          f_type = _webp;
        else if ((uheader[0]==0xFF && uheader[1]==0x0A) ||
                 (uheader[0]==0x00 && uheader[1]==0x00 && uheader[2]==0x00 && uheader[3]==0x0C &&
                  uheader[4]==0x4A && uheader[5]==0x58 && uheader[6]==0x4C && uheader[7]==0x20 &&
                  uheader[8]==0x0D && uheader[9]==0x0A && uheader[10]==0x87 && uheader[11]==0x0A)) // JPEG XL
          f_type = _jxl;
        else if ((uheader[0]==0x00 && uheader[1]==0x00 && uheader[2]==0x00 && uheader[3]==0x1C &&
                  uheader[4]==0x66 && uheader[5]==0x74 && uheader[6]==0x79 && uheader[7]==0x70) &&
                 ((uheader[8]==0x68 && uheader[9]==0x65 && uheader[10]==0x69 && uheader[11]==0x63) ||
                  (uheader[8]==0x61 && uheader[9]==0x76 && uheader[10]==0x69 && uheader[11]==0x66))) // HEIF
          f_type = _heif;
        else { // PNM or PFM
          CImgList<char> _header = header.get_split(CImg<char>::vector('\n'),0,false);
          cimglist_for(_header,l) {
            if (_header(l,0)=='#') continue;
            if (_header[l]._width==2 && _header(l,0)=='P') {
              const char c = _header(l,1);
              if (c=='f' || c=='F') { f_type = _pfm; break; }
              if (c>='1' && c<='9') { f_type = _pnm; break; }
            }
            f_type = 0; break;
          }
        }
      } catch (CImgIOException&) { }
      cimg::exception_mode(omode);
      return f_type;
    }

    //! Load file from network as a local temporary file.
    /**
       \param url URL of the filename, as a C-string.
       \param[out] filename_local C-string containing the path to a local copy of \c filename.
       \param timeout Maximum time (in seconds) authorized for downloading the file from the URL.
       \param try_fallback When using libcurl, tells using system calls as fallbacks in case of libcurl failure.
       \param referer Referer used, as a C-string.
       \param user_agent User agent used, as a C-string.
       \return Value of \c filename_local.
       \note Use the \c libcurl library, or the external binaries \c wget or \c curl to perform the download.
    **/
    inline char *load_network(const char *const url, char *const filename_local,
                              const unsigned int timeout, const bool try_fallback,
                              const char *const referer, const char *const user_agent) {
      if (!url)
        throw CImgArgumentException("cimg::load_network(): Specified URL is (null).");
      if (!filename_local)
        throw CImgArgumentException("cimg::load_network(): Specified destination string is (null).");
      if (!network_mode())
        throw CImgIOException("cimg::load_network(): Loading files from network is disabled.");

      const char *const __ext = cimg::split_filename(url), *const _ext = (*__ext && __ext>url)?__ext - 1:__ext;
      CImg<char> ext = CImg<char>::string(_ext);
      *filename_local = 0;
      if (ext._width>16 || !cimg::strncasecmp(ext,"cgi",3)) *ext = 0;
      else cimg::strwindows_reserved(ext);
      do {
        cimg_snprintf(filename_local,256,"%s%c%s%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext._data);
      } while (cimg::path_exists(filename_local));

#ifdef cimg_use_curl
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      try {
        CURL *curl = 0;
        CURLcode res;
        curl = curl_easy_init();
        if (curl) {
          std::FILE *file = cimg::fopen(filename_local,"wb");
          curl_easy_setopt(curl,CURLOPT_URL,url);
          curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,0);
          curl_easy_setopt(curl,CURLOPT_WRITEDATA,file);
          curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0L);
          curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0L);
          curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
          curl_easy_setopt(curl,CURLOPT_MAXREDIRS,20L);
          if (timeout) curl_easy_setopt(curl,CURLOPT_TIMEOUT,(long)timeout);
          if (std::strchr(url,'?')) curl_easy_setopt(curl,CURLOPT_HTTPGET,1L);
          if (referer) curl_easy_setopt(curl,CURLOPT_REFERER,referer);
          if (user_agent) curl_easy_setopt(curl,CURLOPT_USERAGENT,user_agent);
          res = curl_easy_perform(curl);
          curl_easy_cleanup(curl);
          cimg::fseek(file,0,SEEK_END); // Check if file size is 0
          const cimg_ulong siz = cimg::ftell(file);
          cimg::fclose(file);
          file = 0;
          if (siz>0 && res==CURLE_OK) { cimg::exception_mode(omode); return filename_local; }
          else std::remove(filename_local);
        }
      } catch (...) { }
      cimg::exception_mode(omode);
      if (!try_fallback) throw CImgIOException("cimg::load_network(): Failed to load file '%s' with libcurl.",url);
#endif

      CImg<char> command((unsigned int)std::strlen(url) + 1024), s_referer, s_user_agent, s_timeout;
      cimg::unused(try_fallback);

      // Try with 'curl' first.
      if (timeout) cimg_snprintf(s_timeout.assign(64),64,"-m %u ",timeout);
      else s_timeout.assign(1,1,1,1,0);
      if (referer) cimg_snprintf(s_referer.assign(1024),1024,"-e %s ",referer);
      else s_referer.assign(1,1,1,1,0);
      if (user_agent) cimg_snprintf(s_user_agent.assign(1024),1024,"-A \"%s\" ",user_agent);
      else s_user_agent.assign(1,1,1,1,0);
      cimg_snprintf(command,command._width,
                    "\"%s\" -L --max-redirs 20 %s%s%s-f --silent --compressed -o \"%s\" \"%s\"",
                    cimg::curl_path(),s_timeout._data,s_referer._data,s_user_agent._data,filename_local,
                    CImg<char>::string(url)._system_strescape().data());
      cimg::system(command,cimg::curl_path());

#if cimg_OS==2
      if (cimg::fsize(filename_local)<=0) { // Try with 'powershell' otherwise
        if (timeout) cimg_snprintf(s_timeout.assign(64),64,"-TimeoutSec %u ",timeout);
        else s_timeout.assign(1,1,1,1,0);
        if (referer) cimg_snprintf(s_referer.assign(1024),1024,"-Headers @{'Referer'='%s'} ",referer);
        else s_referer.assign(1,1,1,1,0);
        if (user_agent) cimg_snprintf(s_user_agent.assign(1024),1024,"-UserAgent \"%s\" ",user_agent);
        else s_user_agent.assign(1,1,1,1,0);
        cimg_snprintf(command,command._width,
                      "\"%s\" -NonInteractive -Command Invoke-WebRequest %s%s%s-OutFile \"%s\" -Uri \"%s\"",
                      cimg::powershell_path(),s_timeout._data,s_referer._data,s_user_agent._data,filename_local,
                      CImg<char>::string(url)._system_strescape().data());
        cimg::system(command,cimg::powershell_path());
      }
#endif

      if (cimg::fsize(filename_local)<=0) { // Try with 'wget' otherwise
        if (timeout) cimg_snprintf(s_timeout.assign(64),64,"-T %u ",timeout);
        else s_timeout.assign(1,1,1,1,0);
        if (referer) cimg_snprintf(s_referer.assign(1024),1024,"--referer=%s ",referer);
        else s_referer.assign(1,1,1,1,0);
        if (user_agent) cimg_snprintf(s_user_agent.assign(1024),1024,"--user-agent=\"%s\" ",user_agent);
        else s_user_agent.assign(1,1,1,1,0);
        cimg_snprintf(command,command._width,
                      "\"%s\" --max-redirect=20 %s%s%s-q -r -l 0 --no-cache -O \"%s\" \"%s\"",
                      cimg::wget_path(),s_timeout._data,s_referer._data,s_user_agent._data,filename_local,
                      CImg<char>::string(url)._system_strescape().data());
        cimg::system(command,cimg::wget_path());

        if (cimg::fsize(filename_local)<=0)
          throw CImgIOException("cimg::load_network(): Failed to load file '%s' with external commands "
#if cimg_OS==2
                                "'wget', 'curl', or 'powershell'.",url);
#else
                                "'wget' or 'curl'.",url);
#endif

        // Try gunzip it.
        cimg_snprintf(command,command._width,"%s.gz",filename_local);
        std::rename(filename_local,command);
        cimg_snprintf(command,command._width,"\"%s\" --quiet \"%s.gz\"",
                      gunzip_path(),filename_local);
        cimg::system(command,gunzip_path());
        if (!cimg::is_file(filename_local)) {
          cimg_snprintf(command,command._width,"%s.gz",filename_local);
          std::rename(command,filename_local);
        }
      }

      return filename_local;
    }



#endif
