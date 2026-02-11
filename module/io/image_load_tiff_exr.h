#ifndef CIMG_MODULE_IMAGE_LOAD_TIFF_EXR_H
#define CIMG_MODULE_IMAGE_LOAD_TIFF_EXR_H

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
#endif
