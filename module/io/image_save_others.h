#ifndef CIMG_MODULE_IMAGE_SAVE_OTHERS_H
#define CIMG_MODULE_IMAGE_SAVE_OTHERS_H
    //! Save image as a PFM file.
    /**
      \param filename Filename, as a C-string.
    **/

    const CImg<T>& save_pfm(const char *const filename) const {
      get_mirror('y')._save_pfm(0,filename);
      return *this;
    }

    //! Save image as a PFM file \overloading.
    const CImg<T>& save_pfm(std::FILE *const file) const {
      get_mirror('y')._save_pfm(file,0);
      return *this;
    }

    const CImg<T>& _save_pfm(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_pfm(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_pfm(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (_spectrum>3)
        cimg::warn(_cimg_instance
                   "save_pfm(): image instance is multispectral, only the three first channels will be saved "
                   "in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const T
        *ptr_r = data(0,0,0,0),
        *ptr_g = (_spectrum>=2)?data(0,0,0,1):0,
        *ptr_b = (_spectrum>=3)?data(0,0,0,2):0;
      const unsigned int buf_size = std::min(1024*1024U,_width*_height*(_spectrum==1?1:3));

      std::fprintf(nfile,"P%c\n%u %u\n1.0\n",
                   (_spectrum==1?'f':'F'),_width,_height);

      switch (_spectrum) {
      case 1 : { // Scalar image
        CImg<floatT> buf(buf_size);
        for (longT to_write = (longT)width()*height(); to_write>0; ) {
          const ulongT N = std::min((ulongT)to_write,(ulongT)buf_size);
          float *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) *(ptrd++) = (float)*(ptr_r++);
          if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
          cimg::fwrite(buf._data,N,nfile);
          to_write-=N;
        }
      } break;
      case 2 : { // RG image
        CImg<floatT> buf(buf_size);
        for (longT to_write = (longT)width()*height(); to_write>0; ) {
          const unsigned int N = std::min((unsigned int)to_write,buf_size/3);
          float *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) {
            *(ptrd++) = (float)*(ptr_r++);
            *(ptrd++) = (float)*(ptr_g++);
            *(ptrd++) = 0;
          }
          if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
          cimg::fwrite(buf._data,3*N,nfile);
          to_write-=N;
        }
      } break;
      default : { // RGB image
        CImg<floatT> buf(buf_size);
        for (longT to_write = (longT)width()*height(); to_write>0; ) {
          const unsigned int N = std::min((unsigned int)to_write,buf_size/3);
          float *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) {
            *(ptrd++) = (float)*(ptr_r++);
            *(ptrd++) = (float)*(ptr_g++);
            *(ptrd++) = (float)*(ptr_b++);
          }
          if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
          cimg::fwrite(buf._data,3*N,nfile);
          to_write-=N;
        }
      }
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save image as a RGB file.
    /**
      \param filename Filename, as a C-string.
    **/
    const CImg<T>& save_rgb(const char *const filename) const {
      return _save_rgb(0,filename);
    }

    //! Save image as a RGB file \overloading.
    const CImg<T>& save_rgb(std::FILE *const file) const {
      return _save_rgb(file,0);
    }

    const CImg<T>& _save_rgb(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_rgb(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_spectrum!=3)
        cimg::warn(_cimg_instance
                   "save_rgb(): image instance has not exactly 3 channels, for file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const ulongT wh = (ulongT)_width*_height;
      unsigned char *const buffer = new unsigned char[3*wh], *nbuffer = buffer;
      const T
        *ptr1 = data(0,0,0,0),
        *ptr2 = _spectrum>1?data(0,0,0,1):0,
        *ptr3 = _spectrum>2?data(0,0,0,2):0;
      switch (_spectrum) {
      case 1 : { // Scalar image
        for (ulongT k = 0; k<wh; ++k) {
          const unsigned char val = (unsigned char)*(ptr1++);
          *(nbuffer++) = val;
          *(nbuffer++) = val;
          *(nbuffer++) = val;
        }
      } break;
      case 2 : { // RG image
        for (ulongT k = 0; k<wh; ++k) {
          *(nbuffer++) = (unsigned char)(*(ptr1++));
          *(nbuffer++) = (unsigned char)(*(ptr2++));
          *(nbuffer++) = 0;
        }
      } break;
      default : { // RGB image
        for (ulongT k = 0; k<wh; ++k) {
          *(nbuffer++) = (unsigned char)(*(ptr1++));
          *(nbuffer++) = (unsigned char)(*(ptr2++));
          *(nbuffer++) = (unsigned char)(*(ptr3++));
        }
      }
      }
      cimg::fwrite(buffer,3*wh,nfile);
      if (!file) cimg::fclose(nfile);
      delete[] buffer;
      return *this;
    }

    //! Save image as a RGBA file.
    /**
       \param filename Filename, as a C-string.
    **/
    const CImg<T>& save_rgba(const char *const filename) const {
      return _save_rgba(0,filename);
    }

    //! Save image as a RGBA file \overloading.
    const CImg<T>& save_rgba(std::FILE *const file) const {
      return _save_rgba(file,0);
    }

    const CImg<T>& _save_rgba(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_rgba(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_spectrum!=4)
        cimg::warn(_cimg_instance
                   "save_rgba(): image instance has not exactly 4 channels, for file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const ulongT wh = (ulongT)_width*_height;
      unsigned char *const buffer = new unsigned char[4*wh], *nbuffer = buffer;
      const T
        *ptr1 = data(0,0,0,0),
        *ptr2 = _spectrum>1?data(0,0,0,1):0,
        *ptr3 = _spectrum>2?data(0,0,0,2):0,
        *ptr4 = _spectrum>3?data(0,0,0,3):0;
      switch (_spectrum) {
      case 1 : { // Scalar images
        for (ulongT k = 0; k<wh; ++k) {
          const unsigned char val = (unsigned char)*(ptr1++);
          *(nbuffer++) = val;
          *(nbuffer++) = val;
          *(nbuffer++) = val;
          *(nbuffer++) = 255;
        }
      } break;
      case 2 : { // RG images
        for (ulongT k = 0; k<wh; ++k) {
          *(nbuffer++) = (unsigned char)(*(ptr1++));
          *(nbuffer++) = (unsigned char)(*(ptr2++));
          *(nbuffer++) = 0;
          *(nbuffer++) = 255;
        }
      } break;
      case 3 : { // RGB images
        for (ulongT k = 0; k<wh; ++k) {
          *(nbuffer++) = (unsigned char)(*(ptr1++));
          *(nbuffer++) = (unsigned char)(*(ptr2++));
          *(nbuffer++) = (unsigned char)(*(ptr3++));
          *(nbuffer++) = 255;
        }
      } break;
      default : { // RGBA images
        for (ulongT k = 0; k<wh; ++k) {
          *(nbuffer++) = (unsigned char)(*(ptr1++));
          *(nbuffer++) = (unsigned char)(*(ptr2++));
          *(nbuffer++) = (unsigned char)(*(ptr3++));
          *(nbuffer++) = (unsigned char)(*(ptr4++));
        }
      }
      }
      cimg::fwrite(buffer,4*wh,nfile);
      if (!file) cimg::fclose(nfile);
      delete[] buffer;
      return *this;
    }

    //! Save image as a TIFF file.
    /**
       \param filename Filename, as a C-string.
       \param compression_type Type of data compression. Can be
        <tt>{ 0=None | 1=ADOBE_DEFLATE | 2=CCITT_T4 | 3=CCITT_T6 | 4=CCITTFAX3 | 5=CCITTFAX4 | 6=CCITTRLE |
          7=CCITTRLEW | 8=DCS | 9=DEFLATE | 10=IT8BL | 11=IT8CTPAD | 12=IT8LW | 13=IT8MP | 14=JBIG |
          15=JP2000 | 16=JPEG | 17=JXL | 18=LERC | 19=LZMA | 20=LZW | 21=NEXT | 22=OJPEG | 23=PACKBITS |
          24=PIXARFILM | 25=PIXARLOG | 26=SGILOG | 27=SGILOG24 | 28=T43 | 29=T85 | 30=THUNDERSCAN |
          31=WEBP | 32=ZSTD }</tt>
       \param[out] voxel_size Voxel size, to be stored in the filename.
       \param[out] description Description, to be stored in the filename.
       \param use_bigtiff Allow to save big tiff files (>4Gb).
       \note
       - libtiff support is enabled by defining the precompilation
        directive \c cimg_use_tiff.
       - When libtiff is enabled, 2D and 3D (multipage) several
        channel per pixel are supported for
        <tt>char,uchar,short,ushort,float</tt> and \c double pixel types.
       - If \c cimg_use_tiff is not defined at compile time the
        function uses CImg<T>&save_other(const char*).
     **/
    const CImg<T>& save_tiff(const char *const filename, const unsigned int compression_type=0,
                             const float *const voxel_size=0, const char *const description=0,
                             const bool use_bigtiff=true) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_tiff(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

#ifdef cimg_use_tiff
      const bool
        _use_bigtiff = use_bigtiff && sizeof(ulongT)>=8 && size()*sizeof(T)>=1UL<<31; // No bigtiff for small images
      TIFF *tif = TIFFOpen(filename,_use_bigtiff?"w8":"w4");
      if (tif) {
        double val_min, val_max = (double)max_min(val_min);
        cimg_forZ(*this,z) _save_tiff(tif,z,z,compression_type,voxel_size,description,val_min,val_max);
        TIFFClose(tif);
      } else throw CImgIOException(_cimg_instance
                                   "save_tiff(): Failed to open file '%s' for writing.",
                                   cimg_instance,
                                   filename);
      return *this;
#else
      cimg::unused(compression_type,voxel_size,description,use_bigtiff);
      return save_other(filename);
#endif
    }

#ifdef cimg_use_tiff

#define _cimg_save_tiff(types,typed) if (!std::strcmp(types,pixel_type())) { \
    const typed foo = (typed)0; return _save_tiff(tif,directory,z,foo,compression_type,voxel_size,description,\
                                                  val_min,val_max); }

    // [internal] Save a plane into a tiff file.
    template<typename t>
    const CImg<T>& _save_tiff(TIFF *tif, const unsigned int directory, const unsigned int z, const t& pixel_t,
                              const unsigned int compression_type, const float *const voxel_size,
                              const char *const description, double val_min, double val_max) const {
      if (is_empty() || !tif || pixel_t) return *this;
      const unsigned int compression_codes[] = {
        COMPRESSION_NONE, COMPRESSION_ADOBE_DEFLATE, COMPRESSION_CCITT_T4, COMPRESSION_CCITT_T6,
        COMPRESSION_CCITTFAX3, COMPRESSION_CCITTFAX4, COMPRESSION_CCITTRLE, COMPRESSION_CCITTRLEW,
        COMPRESSION_DCS, COMPRESSION_DEFLATE, COMPRESSION_IT8BL, COMPRESSION_IT8CTPAD, COMPRESSION_IT8LW,
        COMPRESSION_IT8MP, COMPRESSION_JBIG, COMPRESSION_JP2000, COMPRESSION_JPEG,
#ifdef COMPRESSION_JXL
        COMPRESSION_JXL,
#else
        COMPRESSION_NONE,
#endif
        COMPRESSION_LERC, COMPRESSION_LZMA, COMPRESSION_LZW, COMPRESSION_NEXT, COMPRESSION_OJPEG,
        COMPRESSION_PACKBITS, COMPRESSION_PIXARFILM, COMPRESSION_PIXARLOG, COMPRESSION_SGILOG,
        COMPRESSION_SGILOG24, COMPRESSION_T43, COMPRESSION_T85, COMPRESSION_THUNDERSCAN, COMPRESSION_WEBP,
        COMPRESSION_ZSTD },
        nb_compression_codes = sizeof(compression_codes)/sizeof(unsigned int);
      if (compression_type>=nb_compression_codes)
        throw CImgArgumentException(_cimg_instance
                                    "save_tiff(): Specified compression type (%u) is invalid "
                                    "(should be in range [0,%u]).",
                                    cimg_instance,nb_compression_codes - 1);
      const unsigned int compression_code = compression_codes[compression_type];
      const char *const filename = TIFFFileName(tif);
      cimg_uint32 rowsperstrip = (cimg_uint32)-1;
      cimg_uint16 spp = _spectrum, bpp = sizeof(t)*8, photometric;
      if (spp==3 || spp==4) photometric = PHOTOMETRIC_RGB;
      else photometric = PHOTOMETRIC_MINISBLACK;
      TIFFSetDirectory(tif,directory);
      TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,_width);
      TIFFSetField(tif,TIFFTAG_IMAGELENGTH,_height);
      if (voxel_size) {
        const float vx = voxel_size[0], vy = voxel_size[1], vz = voxel_size[2];
        TIFFSetField(tif,TIFFTAG_RESOLUTIONUNIT,RESUNIT_NONE);
        TIFFSetField(tif,TIFFTAG_XRESOLUTION,1.f/vx);
        TIFFSetField(tif,TIFFTAG_YRESOLUTION,1.f/vy);
        CImg<charT> s_description(256);
        cimg_snprintf(s_description,s_description._width,"VX=%g VY=%g VZ=%g spacing=%g",vx,vy,vz,vz);
        TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,s_description.data());
      }
      if (description) TIFFSetField(tif,TIFFTAG_IMAGEDESCRIPTION,description);
      TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
      TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,spp);
      if (cimg::type<t>::is_float()) TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,3);
      else if (cimg::type<t>::min()==0) TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,1);
      else TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,2);
      TIFFSetField(tif,TIFFTAG_SMINSAMPLEVALUE,val_min);
      TIFFSetField(tif,TIFFTAG_SMAXSAMPLEVALUE,val_max);
      TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,bpp);
      TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
      TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,photometric);
      TIFFSetField(tif,TIFFTAG_COMPRESSION,compression_code);
      rowsperstrip = TIFFDefaultStripSize(tif,rowsperstrip);
      TIFFSetField(tif,TIFFTAG_ROWSPERSTRIP,rowsperstrip);
      TIFFSetField(tif,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
      TIFFSetField(tif,TIFFTAG_SOFTWARE,cimg_appname);

      t *const buf = (t*)_TIFFmalloc(TIFFStripSize(tif));
      if (buf) {
        for (unsigned int row = 0; row<_height; row+=rowsperstrip) {
          cimg_uint32 nrow = (row + rowsperstrip>_height?_height - row:rowsperstrip);
          tstrip_t strip = TIFFComputeStrip(tif,row,0);
          tsize_t i = 0;
          for (unsigned int rr = 0; rr<nrow; ++rr)
            for (unsigned int cc = 0; cc<_width; ++cc)
              for (unsigned int vv = 0; vv<spp; ++vv)
                buf[i++] = (t)(*this)(cc,row + rr,z,vv);
          if (TIFFWriteEncodedStrip(tif,strip,buf,i*sizeof(t))<0)
            throw CImgIOException(_cimg_instance
                                  "save_tiff(): Invalid strip writing when saving file '%s'.",
                                  cimg_instance,
                                  filename?filename:"(FILE*)");
        }
        _TIFFfree(buf);
      }
      TIFFWriteDirectory(tif);
      return *this;
    }

    const CImg<T>& _save_tiff(TIFF *tif, const unsigned int directory, const unsigned int z,
                              const unsigned int compression_type, const float *const voxel_size,
                              const char *const description, double val_min, double val_max) const {
      _cimg_save_tiff("uint8",cimg_uint8);
      _cimg_save_tiff("int8",cimg_int8);
      _cimg_save_tiff("uint16",cimg_uint16);
      _cimg_save_tiff("int16",cimg_int16);
      _cimg_save_tiff("uint32",cimg_uint32);
      _cimg_save_tiff("int32",cimg_int32);
      _cimg_save_tiff("uint64",cimg_uint32); // 'int64' as 'int32'
      _cimg_save_tiff("int64",cimg_int32);
      _cimg_save_tiff("float16",cimg_float32); // 'float16' as 'float32'
      _cimg_save_tiff("float32",cimg_float32);
      _cimg_save_tiff("float64",cimg_float32); // 'float64' as 'float32'
      const char *const filename = TIFFFileName(tif);
      throw CImgInstanceException(_cimg_instance
                                  "save_tiff(): Unsupported pixel type '%s' for file '%s'.",
                                  cimg_instance,
                                  pixel_type(),filename?filename:"(FILE*)");
      return *this;
    }
#endif

    //! Save image as a MINC2 file.
    /**
       \param filename Filename, as a C-string.
       \param imitate_file If non-zero, reference filename, as a C-string, to borrow header from.
    **/
    const CImg<T>& save_minc2(const char *const filename,
                              const char *const imitate_file=0) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                   "save_minc2(): Specified filename is (null).",
                                   cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

#ifndef cimg_use_minc2
     cimg::unused(imitate_file);
     return save_other(filename);
#else
     minc::minc_1_writer wtr;
     if (imitate_file)
       wtr.open(filename, imitate_file);
     else {
       minc::minc_info di;
       if (width()) di.push_back(minc::dim_info(width(),width()*0.5,-1,minc::dim_info::DIM_X));
       if (height()) di.push_back(minc::dim_info(height(),height()*0.5,-1,minc::dim_info::DIM_Y));
       if (depth()) di.push_back(minc::dim_info(depth(),depth()*0.5,-1,minc::dim_info::DIM_Z));
       if (spectrum()) di.push_back(minc::dim_info(spectrum(),spectrum()*0.5,-1,minc::dim_info::DIM_TIME));
       wtr.open(filename,di,1,NC_FLOAT,0);
     }
     if (pixel_type()==cimg::type<unsigned char>::string())
       wtr.setup_write_byte();
     else if (pixel_type()==cimg::type<int>::string())
       wtr.setup_write_int();
     else if (pixel_type()==cimg::type<double>::string())
       wtr.setup_write_double();
     else
       wtr.setup_write_float();
     minc::save_standard_volume(wtr, this->_data);
     return *this;
#endif
    }

    //! Save image as an ANALYZE7.5 or NIFTI file.
    /**
      \param filename Filename, as a C-string.
      \param voxel_size Pointer to 3 consecutive values that tell about the voxel sizes along the X,Y and Z dimensions.
    **/
    const CImg<T>& save_analyze(const char *const filename, const float *const voxel_size=0) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_analyze(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

      std::FILE *file;
      CImg<charT> hname(1024), iname(1024);
      const char *const ext = cimg::split_filename(filename);
      short datatype = -1;
      if (!*ext) {
        cimg_snprintf(hname,hname._width,"%s.hdr",filename);
        cimg_snprintf(iname,iname._width,"%s.img",filename);
      }
      if (!cimg::strncasecmp(ext,"hdr",3)) {
        std::strcpy(hname,filename);
        std::strncpy(iname,filename,iname._width - 1);
        cimg_snprintf(iname._data + std::strlen(iname) - 3,4,"img");
      }
      if (!cimg::strncasecmp(ext,"img",3)) {
        std::strcpy(hname,filename);
        std::strncpy(iname,filename,iname._width - 1);
        cimg_snprintf(hname._data + std::strlen(iname) - 3,4,"hdr");
      }
      if (!cimg::strncasecmp(ext,"nii",3)) {
        std::strncpy(hname,filename,hname._width - 1); *iname = 0;
      }

      CImg<charT> header(*iname?348:352,1,1,1,0);
      int *const iheader = (int*)header._data;
      *iheader = 348;
      std::strcpy(header._data + 4,"CImg");
      std::strcpy(header._data + 14," ");
      ((short*)&(header[36]))[0] = 4096;
      ((char*)&(header[38]))[0] = 114;
      ((short*)&(header[40]))[0] = 4;
      ((short*)&(header[40]))[1] = (short)_width;
      ((short*)&(header[40]))[2] = (short)_height;
      ((short*)&(header[40]))[3] = (short)_depth;
      ((short*)&(header[40]))[4] = (short)_spectrum;
      if (!cimg::strcasecmp(pixel_type(),"bool") ||
          !cimg::strcasecmp(pixel_type(),"uint8") ||
          !cimg::strcasecmp(pixel_type(),"int8")) datatype = 2;
      if (!cimg::strcasecmp(pixel_type(),"uint16") ||
          !cimg::strcasecmp(pixel_type(),"int16")) datatype = 4;
      if (!cimg::strcasecmp(pixel_type(),"uint32") ||
          !cimg::strcasecmp(pixel_type(),"int32")) datatype = 8;
      if (!cimg::strcasecmp(pixel_type(),"uint64") ||
          !cimg::strcasecmp(pixel_type(),"int64")) datatype = 8;
      if (!cimg::strcasecmp(pixel_type(),"float32")) datatype = 16;
      if (!cimg::strcasecmp(pixel_type(),"float64")) datatype = 64;
      if (datatype<0)
        throw CImgIOException(_cimg_instance
                              "save_analyze(): Unsupported pixel type '%s' for file '%s'.",
                              cimg_instance,
                              pixel_type(),filename);

      ((short*)&(header[70]))[0] = datatype;
      ((short*)&(header[72]))[0] = sizeof(T);
      ((float*)&(header[108]))[0] = (float)(*iname?0:header.width());
      ((float*)&(header[112]))[0] = 1;
      ((float*)&(header[76]))[0] = 0;
      if (voxel_size) {
        ((float*)&(header[76]))[1] = voxel_size[0];
        ((float*)&(header[76]))[2] = voxel_size[1];
        ((float*)&(header[76]))[3] = voxel_size[2];
      } else ((float*)&(header[76]))[1] = ((float*)&(header[76]))[2] = ((float*)&(header[76]))[3] = 1;
      file = cimg::fopen(hname,"wb");
      cimg::fwrite(header._data,header.width(),file);
      if (*iname) { cimg::fclose(file); file = cimg::fopen(iname,"wb"); }
      cimg::fwrite(_data,size(),file);
      cimg::fclose(file);
      return *this;
    }

    //! Save image as a .cimg file.
    /**
      \param filename Filename, as a C-string.
      \param is_compressed Tells if the file contains compressed image data.
    **/
    const CImg<T>& save_cimg(const char *const filename, const bool is_compressed=false) const {
      CImgList<T>(*this,true).save_cimg(filename,is_compressed);
      return *this;
    }

    //! Save image as a .cimg file \overloading.
    const CImg<T>& save_cimg(std::FILE *const file, const bool is_compressed=false) const {
      CImgList<T>(*this,true).save_cimg(file,is_compressed);
      return *this;
    }

    //! Save image as a sub-image into an existing .cimg file.
    /**
      \param filename Filename, as a C-string.
      \param n0 Index of the image inside the file.
      \param x0 X-coordinate of the sub-image location.
      \param y0 Y-coordinate of the sub-image location.
      \param z0 Z-coordinate of the sub-image location.
      \param c0 C-coordinate of the sub-image location.
    **/
    const CImg<T>& save_cimg(const char *const filename,
                             const unsigned int n0,
                             const unsigned int x0, const unsigned int y0,
                             const unsigned int z0, const unsigned int c0) const {
      CImgList<T>(*this,true).save_cimg(filename,n0,x0,y0,z0,c0);
      return *this;
    }

    //! Save image as a sub-image into an existing .cimg file \overloading.
    const CImg<T>& save_cimg(std::FILE *const file,
                             const unsigned int n0,
                             const unsigned int x0, const unsigned int y0,
                             const unsigned int z0, const unsigned int c0) const {
      CImgList<T>(*this,true).save_cimg(file,n0,x0,y0,z0,c0);
      return *this;
    }

    //! Save blank image as a .cimg file.
    /**
        \param filename Filename, as a C-string.
        \param dx Width of the image.
        \param dy Height of the image.
        \param dz Depth of the image.
        \param dc Number of channels of the image.
        \note
        - All pixel values of the saved image are set to \c 0.
        - Use this method to save large images without having to instantiate and allocate them.
    **/
    static void save_empty_cimg(const char *const filename,
                                const unsigned int dx, const unsigned int dy=1,
                                const unsigned int dz=1, const unsigned int dc=1) {
      return CImgList<T>::save_empty_cimg(filename,1,dx,dy,dz,dc);
    }

    //! Save blank image as a .cimg file \overloading.
    /**
       Same as save_empty_cimg(const char *,unsigned int,unsigned int,unsigned int,unsigned int)
       with a file stream argument instead of a filename string.
    **/
    static void save_empty_cimg(std::FILE *const file,
                                const unsigned int dx, const unsigned int dy=1,
                                const unsigned int dz=1, const unsigned int dc=1) {
      return CImgList<T>::save_empty_cimg(file,1,dx,dy,dz,dc);
    }

    //! Save image as an INRIMAGE-4 file.
    /**
      \param filename Filename, as a C-string.
      \param voxel_size Pointer to 3 values specifying the voxel sizes along the X,Y and Z dimensions.
    **/
    const CImg<T>& save_inr(const char *const filename, const float *const voxel_size=0) const {
      return _save_inr(0,filename,voxel_size);
    }

    //! Save image as an INRIMAGE-4 file \overloading.
    const CImg<T>& save_inr(std::FILE *const file, const float *const voxel_size=0) const {
      return _save_inr(file,0,voxel_size);
    }

    const CImg<T>& _save_inr(std::FILE *const file, const char *const filename, const float *const voxel_size) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_inr(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

      int inrpixsize = -1;
      const char *inrtype = "unsigned fixed\nPIXSIZE=8 bits\nSCALE=2**0";
      if (!cimg::strcasecmp(pixel_type(),"uint8")) {
        inrtype = "unsigned fixed\nPIXSIZE=8 bits\nSCALE=2**0"; inrpixsize = 1;
      }
      if (!cimg::strcasecmp(pixel_type(),"int8")) {
        inrtype = "fixed\nPIXSIZE=8 bits\nSCALE=2**0"; inrpixsize = 1;
      }
      if (!cimg::strcasecmp(pixel_type(),"uint16")) {
        inrtype = "unsigned fixed\nPIXSIZE=16 bits\nSCALE=2**0";inrpixsize = 2;
      }
      if (!cimg::strcasecmp(pixel_type(),"int16")) {
        inrtype = "fixed\nPIXSIZE=16 bits\nSCALE=2**0"; inrpixsize = 2;
      }
      if (!cimg::strcasecmp(pixel_type(),"uint32")) {
        inrtype = "unsigned fixed\nPIXSIZE=32 bits\nSCALE=2**0";inrpixsize = 4;
      }
      if (!cimg::strcasecmp(pixel_type(),"int32")) {
        inrtype = "fixed\nPIXSIZE=32 bits\nSCALE=2**0"; inrpixsize = 4;
      }
      if (!cimg::strcasecmp(pixel_type(),"float32")) {
        inrtype = "float\nPIXSIZE=32 bits"; inrpixsize = 4;
      }
      if (!cimg::strcasecmp(pixel_type(),"float64")) {
        inrtype = "float\nPIXSIZE=64 bits"; inrpixsize = 8;
      }
      if (inrpixsize<=0)
        throw CImgIOException(_cimg_instance
                              "save_inr(): Unsupported pixel type '%s' for file '%s'",
                              cimg_instance,
                              pixel_type(),filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      CImg<charT> header(257);
      int err = cimg_snprintf(header,header._width,"#INRIMAGE-4#{\nXDIM=%u\nYDIM=%u\nZDIM=%u\nVDIM=%u\n",
                              _width,_height,_depth,_spectrum);
      if (voxel_size)
        err+=cimg_snprintf(header._data + err,128,"VX=%g\nVY=%g\nVZ=%g\n",
                          voxel_size[0],voxel_size[1],voxel_size[2]);
      err+=cimg_snprintf(header._data + err,128,"TYPE=%s\nCPU=%s\n",
                         inrtype,cimg::endianness()?"sun":"decm");
      std::memset(header._data + err,'\n',252 - err);
      std::memcpy(header._data + 252,"##}\n",4);
      cimg::fwrite(header._data,256,nfile);
      cimg_forXYZ(*this,x,y,z) cimg_forC(*this,c) cimg::fwrite(&((*this)(x,y,z,c)),1,nfile);
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save image as an OpenEXR file.
    /**
       \param filename Filename, as a C-string.
       \note The OpenEXR file format is <a href="http://en.wikipedia.org/wiki/OpenEXR">described here</a>.
    **/
    const CImg<T>& save_exr(const char *const filename) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_exr(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_exr(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename);

#ifndef cimg_use_openexr
      return save_other(filename);
#else
      Imf::Rgba *const ptrd0 = new Imf::Rgba[(size_t)_width*_height], *ptrd = ptrd0, rgba;
      switch (_spectrum) {
      case 1 : { // Grayscale image
        for (const T *ptr_r = data(), *const ptr_e = ptr_r + (ulongT)_width*_height; ptr_r<ptr_e;) {
          rgba.r = (cimg_float16)(*(ptr_r));
          rgba.g = (cimg_float16)(*(ptr_r));
          rgba.b = (cimg_float16)(*(ptr_r++));
          rgba.a = (cimg_float16)1;
          *(ptrd++) = rgba;
        }
      } break;
      case 2 : { // RG image
        for (const T *ptr_r = data(), *ptr_g = data(0,0,0,1),
               *const ptr_e = ptr_r + (ulongT)_width*_height; ptr_r<ptr_e; ) {
          rgba.r = (cimg_float16)(*(ptr_r++));
          rgba.g = (cimg_float16)(*(ptr_g++));
          rgba.b = (cimg_float16)0;
          rgba.a = (cimg_float16)1;
          *(ptrd++) = rgba;
        }
      } break;
      case 3 : { // RGB image
        for (const T *ptr_r = data(), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2),
               *const ptr_e = ptr_r + (ulongT)_width*_height; ptr_r<ptr_e;) {
          rgba.r = (cimg_float16)(*(ptr_r++));
          rgba.g = (cimg_float16)(*(ptr_g++));
          rgba.b = (cimg_float16)(*(ptr_b++));
          rgba.a = (cimg_float16)1;
          *(ptrd++) = rgba;
        }
      } break;
      default : { // RGBA image
        for (const T *ptr_r = data(), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2), *ptr_a = data(0,0,0,3),
               *const ptr_e = ptr_r + (ulongT)_width*_height; ptr_r<ptr_e;) {
          rgba.r = (cimg_float16)(*(ptr_r++));
          rgba.g = (cimg_float16)(*(ptr_g++));
          rgba.b = (cimg_float16)(*(ptr_b++));
          rgba.a = (cimg_float16)(*(ptr_a++));
          *(ptrd++) = rgba;
        }
      } break;
      }
      Imf::RgbaOutputFile outFile(filename,_width,_height,
                                  _spectrum==1?Imf::WRITE_Y:_spectrum==2?Imf::WRITE_YA:_spectrum==3?
                                  Imf::WRITE_RGB:Imf::WRITE_RGBA);
      outFile.setFrameBuffer(ptrd0,1,_width);
      outFile.writePixels(_height);
      delete[] ptrd0;
      return *this;
#endif
    }

    //! Save image as a Pandore-5 file.
    /**
       \param filename Filename, as a C-string.
       \param colorspace Colorspace data field in output file
       (see <a href="http://www.greyc.ensicaen.fr/~regis/Pandore">Pandore file specifications</a>
       for more information).
    **/
    const CImg<T>& save_pandore(const char *const filename, const unsigned int colorspace=0) const {
      return _save_pandore(0,filename,colorspace);
    }

    //! Save image as a Pandore-5 file \overloading.
    /**
        Same as save_pandore(const char *,unsigned int) const
        with a file stream argument instead of a filename string.
    **/
    const CImg<T>& save_pandore(std::FILE *const file, const unsigned int colorspace=0) const {
      return _save_pandore(file,0,colorspace);
    }

    unsigned int _save_pandore_header_length(unsigned int id, unsigned int *dims, const unsigned int colorspace) const {
      unsigned int nbdims = 0;
      if (id==2 || id==3 || id==4) {
        dims[0] = 1; dims[1] = _width; nbdims = 2;
      }
      if (id==5 || id==6 || id==7) {
        dims[0] = 1; dims[1] = _height; dims[2] = _width; nbdims=3;
      }
      if (id==8 || id==9 || id==10) {
        dims[0] = _spectrum; dims[1] = _depth; dims[2] = _height; dims[3] = _width; nbdims = 4;
      }
      if (id==16 || id==17 || id==18) {
        dims[0] = 3; dims[1] = _height; dims[2] = _width; dims[3] = colorspace; nbdims = 4;
      }
      if (id==19 || id==20 || id==21) {
        dims[0] = 3; dims[1] = _depth; dims[2] = _height; dims[3] = _width; dims[4] = colorspace; nbdims = 5;
      }
      if (id==22 || id==23 || id==25) {
        dims[0] = _spectrum; dims[1] = _width; nbdims = 2;
      }
      if (id==26 || id==27 || id==29) {
        dims[0] = _spectrum; dims[1] = _height; dims[2] = _width; nbdims=3;
      }
      if (id==30 || id==31 || id==33) {
        dims[0] = _spectrum; dims[1] = _depth; dims[2] = _height; dims[3] = _width; nbdims = 4;
      }
      return nbdims;
    }

    const CImg<T>& _save_pandore(std::FILE *const file, const char *const filename,
                                 const unsigned int colorspace) const {

#define __cimg_save_pandore_case(dtype) \
       dtype *buffer = new dtype[size()]; \
       const T *ptrs = _data; \
       cimg_foroff(*this,off) *(buffer++) = (dtype)(*(ptrs++)); \
       buffer-=size(); \
       cimg::fwrite(buffer,size(),nfile); \
       delete[] buffer

#define _cimg_save_pandore_case(sy,sz,sv,stype,id) \
      if (!saved && (sy?(sy==_height):true) && (sz?(sz==_depth):true) && \
          (sv?(sv==_spectrum):true) && !std::strcmp(stype,pixel_type())) { \
        unsigned int *iheader = (unsigned int*)(header + 12); \
        nbdims = _save_pandore_header_length((*iheader=id),dims,colorspace); \
        cimg::fwrite(header,36,nfile); \
        if (sizeof(unsigned long)==4) { CImg<ulongT> ndims(5); \
          for (int d = 0; d<5; ++d) ndims[d] = (unsigned long)dims[d]; \
          cimg::fwrite(ndims._data,nbdims,nfile); } \
        else if (sizeof(unsigned int)==4) { CImg<uintT> ndims(5); \
          for (int d = 0; d<5; ++d) ndims[d] = (unsigned int)dims[d]; \
          cimg::fwrite(ndims._data,nbdims,nfile); } \
        else if (sizeof(unsigned short)==4) { CImg<ushortT> ndims(5); \
          for (int d = 0; d<5; ++d) ndims[d] = (unsigned short)dims[d]; \
          cimg::fwrite(ndims._data,nbdims,nfile); } \
        else throw CImgIOException(_cimg_instance \
                                   "save_pandore(): Unsupported datatype for file '%s'.",\
                                   cimg_instance, \
                                   filename?filename:"(FILE*)"); \
        if (id==2 || id==5 || id==8 || id==16 || id==19 || id==22 || id==26 || id==30) { \
          __cimg_save_pandore_case(unsigned char); \
        } else if (id==3 || id==6 || id==9 || id==17 || id==20 || id==23 || id==27 || id==31) { \
          if (sizeof(unsigned long)==4) { __cimg_save_pandore_case(unsigned long); } \
          else if (sizeof(unsigned int)==4) { __cimg_save_pandore_case(unsigned int); } \
          else if (sizeof(unsigned short)==4) { __cimg_save_pandore_case(unsigned short); } \
          else throw CImgIOException(_cimg_instance \
                                     "save_pandore(): Unsupported datatype for file '%s'.",\
                                     cimg_instance, \
                                     filename?filename:"(FILE*)"); \
        } else if (id==4 || id==7 || id==10 || id==18 || id==21 || id==25 || id==29 || id==33) { \
          if (sizeof(double)==4) { __cimg_save_pandore_case(double); } \
          else if (sizeof(float)==4) { __cimg_save_pandore_case(float); } \
          else throw CImgIOException(_cimg_instance \
                                     "save_pandore(): Unsupported datatype for file '%s'.",\
                                     cimg_instance, \
                                     filename?filename:"(FILE*)"); \
        } \
        saved = true; \
      }

      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_pandore(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      unsigned char header[36] = { 'P','A','N','D','O','R','E','0','4',0,0,0,
                                   0,0,0,0,'C','I','m','g',0,0,0,0,0,
                                   'N','o',' ','d','a','t','e',0,0,0,0 };
      unsigned int nbdims, dims[5] = {};
      bool saved = false;
      _cimg_save_pandore_case(1,1,1,"uint8",2);
      _cimg_save_pandore_case(1,1,1,"int8",3);
      _cimg_save_pandore_case(1,1,1,"uint16",3);
      _cimg_save_pandore_case(1,1,1,"int16",3);
      _cimg_save_pandore_case(1,1,1,"uint32",3);
      _cimg_save_pandore_case(1,1,1,"int32",3);
      _cimg_save_pandore_case(1,1,1,"uint64",3);
      _cimg_save_pandore_case(1,1,1,"int64",3);
      _cimg_save_pandore_case(1,1,1,"float32",4);
      _cimg_save_pandore_case(1,1,1,"float64",4);

      _cimg_save_pandore_case(0,1,1,"uint8",5);
      _cimg_save_pandore_case(0,1,1,"int8",6);
      _cimg_save_pandore_case(0,1,1,"uint16",6);
      _cimg_save_pandore_case(0,1,1,"int16",6);
      _cimg_save_pandore_case(0,1,1,"uint32",6);
      _cimg_save_pandore_case(0,1,1,"int32",6);
      _cimg_save_pandore_case(0,1,1,"uint64",6);
      _cimg_save_pandore_case(0,1,1,"int64",6);
      _cimg_save_pandore_case(0,1,1,"float32",7);
      _cimg_save_pandore_case(0,1,1,"float64",7);

      _cimg_save_pandore_case(0,0,1,"uint8",8);
      _cimg_save_pandore_case(0,0,1,"int8",9);
      _cimg_save_pandore_case(0,0,1,"uint16",9);
      _cimg_save_pandore_case(0,0,1,"int16",9);
      _cimg_save_pandore_case(0,0,1,"uint32",9);
      _cimg_save_pandore_case(0,0,1,"int32",9);
      _cimg_save_pandore_case(0,0,1,"uint64",9);
      _cimg_save_pandore_case(0,0,1,"int64",9);
      _cimg_save_pandore_case(0,0,1,"float32",10);
      _cimg_save_pandore_case(0,0,1,"float64",10);

      _cimg_save_pandore_case(0,1,3,"uint8",16);
      _cimg_save_pandore_case(0,1,3,"int8",17);
      _cimg_save_pandore_case(0,1,3,"uint16",17);
      _cimg_save_pandore_case(0,1,3,"int16",17);
      _cimg_save_pandore_case(0,1,3,"uint32",17);
      _cimg_save_pandore_case(0,1,3,"int32",17);
      _cimg_save_pandore_case(0,1,3,"uint64",17);
      _cimg_save_pandore_case(0,1,3,"int64",17);
      _cimg_save_pandore_case(0,1,3,"float32",18);
      _cimg_save_pandore_case(0,1,3,"float64",18);

      _cimg_save_pandore_case(0,0,3,"uint8",19);
      _cimg_save_pandore_case(0,0,3,"int8",20);
      _cimg_save_pandore_case(0,0,3,"uint16",20);
      _cimg_save_pandore_case(0,0,3,"int16",20);
      _cimg_save_pandore_case(0,0,3,"uint32",20);
      _cimg_save_pandore_case(0,0,3,"int32",20);
      _cimg_save_pandore_case(0,0,3,"uint64",20);
      _cimg_save_pandore_case(0,0,3,"int64",20);
      _cimg_save_pandore_case(0,0,3,"float32",21);
      _cimg_save_pandore_case(0,0,3,"float64",21);

      _cimg_save_pandore_case(1,1,0,"uint8",22);
      _cimg_save_pandore_case(1,1,0,"int8",23);
      _cimg_save_pandore_case(1,1,0,"uint16",23);
      _cimg_save_pandore_case(1,1,0,"int16",23);
      _cimg_save_pandore_case(1,1,0,"uint32",23);
      _cimg_save_pandore_case(1,1,0,"int32",23);
      _cimg_save_pandore_case(1,1,0,"uint64",23);
      _cimg_save_pandore_case(1,1,0,"int64",23);
      _cimg_save_pandore_case(1,1,0,"float32",25);
      _cimg_save_pandore_case(1,1,0,"float64",25);

      _cimg_save_pandore_case(0,1,0,"uint8",26);
      _cimg_save_pandore_case(0,1,0,"int8",27);
      _cimg_save_pandore_case(0,1,0,"uint16",27);
      _cimg_save_pandore_case(0,1,0,"int16",27);
      _cimg_save_pandore_case(0,1,0,"uint32",27);
      _cimg_save_pandore_case(0,1,0,"int32",27);
      _cimg_save_pandore_case(0,1,0,"uint64",27);
      _cimg_save_pandore_case(0,1,0,"int64",27);
      _cimg_save_pandore_case(0,1,0,"float32",29);
      _cimg_save_pandore_case(0,1,0,"float64",29);

      _cimg_save_pandore_case(0,0,0,"uint8",30);
      _cimg_save_pandore_case(0,0,0,"int8",31);
      _cimg_save_pandore_case(0,0,0,"uint16",31);
      _cimg_save_pandore_case(0,0,0,"int16",31);
      _cimg_save_pandore_case(0,0,0,"uint32",31);
      _cimg_save_pandore_case(0,0,0,"int32",31);
      _cimg_save_pandore_case(0,0,0,"uint64",31);
      _cimg_save_pandore_case(0,0,0,"int64",31);
      _cimg_save_pandore_case(0,0,0,"float32",33);
      _cimg_save_pandore_case(0,0,0,"float64",33);

      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save image as a raw data file.
    /**
       \param filename Filename, as a C-string.
       \param is_multiplexed Tells if the image channels are stored in a multiplexed way (\c true) or not (\c false).
       \note The .raw format does not store the image dimensions in the output file,
       so you have to keep track of them somewhere to be able to read the file correctly afterwards.
    **/
    const CImg<T>& save_raw(const char *const filename, const bool is_multiplexed=false) const {
      return _save_raw(0,filename,is_multiplexed);
    }

    //! Save image as a raw data file \overloading.
    /**
       Same as save_raw(const char *,bool) const
       with a file stream argument instead of a filename string.
    **/
    const CImg<T>& save_raw(std::FILE *const file, const bool is_multiplexed=false) const {
      return _save_raw(file,0,is_multiplexed);
    }

    const CImg<T>& _save_raw(std::FILE *const file, const char *const filename, const bool is_multiplexed) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_raw(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      if (pixel_type()==cimg::type<bool>::string()) { // Boolean data (bitwise)
        ulongT siz;
        const unsigned char *const buf = _bool2uchar(siz,is_multiplexed);
        cimg::fwrite(buf,siz,nfile);
        delete[] buf;
      } else { // Non boolean data
        if (!is_multiplexed || _spectrum==1) cimg::fwrite(_data,size(),nfile); // Non-multiplexed
        else { // Multiplexed
          CImg<T> buf(_spectrum);
          cimg_forXYZ(*this,x,y,z) {
            cimg_forC(*this,c) buf[c] = (*this)(x,y,z,c);
            cimg::fwrite(buf._data,_spectrum,nfile);
          }
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    // Return unsigned char buffer that encodes data of a CImg<bool> instance bitwise.
    // (buffer needs to be deallocated afterwards, with delete[]).
    const unsigned char *_bool2uchar(ulongT &siz, const bool is_multiplexed) const {
      const ulongT _siz = size();
      siz = _siz/8 + (_siz%8?1:0);
      unsigned char *const buf = new unsigned char[siz], *ptrd = buf, val = 0, bit = 0;

      if (!is_multiplexed || _spectrum==1) // Non-multiplexed
        cimg_for(*this,ptrs,T) { (val<<=1)|=(*ptrs?1:0); if (++bit==8) { *(ptrd++) = val; val = bit = 0; }}
      else // Multiplexed
        cimg_forXYZ(*this,x,y,z) cimg_forC(*this,c) {
          (val<<=1)|=((*this)(x,y,z,c)?1:0); if (++bit==8) { *(ptrd++) = val; val = bit = 0; }
        }
      if (bit) *ptrd = val;
      return buf;
    }

    // Fill CImg<T> instance from bitwise data encoded in an unsigned char buffer.
    void _uchar2bool(const unsigned char *buf, const ulongT siz, const bool is_multiplexed) {
      const ulongT S = std::min(siz*8,size());
      const unsigned char *ptrs = buf;
      unsigned char val = 0, mask = 0;
      T *ptrd = _data;
      if (S && (!is_multiplexed || _spectrum==1)) // Non-multiplexed
        for (ulongT off = 0; off<S; ++off) {
          if (!(mask>>=1)) { val = *(ptrs++); mask = 128; }
          *(ptrd++) = (T)((val&mask)?1:0);
        }
      else if (S) { // Multiplexed
        ulongT off = 0;
        for (int z = 0; z<depth() && off<=S; ++z)
          for (int y = 0; y<height() && off<=S; ++y)
            for (int x = 0; x<width() && off<=S; ++x)
              for (int c = 0; c<spectrum() && off<=S; ++c) {
                if (!(mask>>=1)) { val = *(ptrs++); ++off; mask = 128; }
                (*this)(x,y,z,c) = (T)((val&mask)?1:0);
              }
      }
    }

    //! Save image as a .yuv video file.
    /**
       \param filename Filename, as a C-string.
       \param chroma_subsampling Type of chroma subsampling. Can be <tt>{ 420 | 422 | 444 }</tt>.
       \param is_rgb Tells if pixel values of the instance image are RGB-coded (\c true) or YUV-coded (\c false).
       \note Each slice of the instance image is considered to be a single frame of the output video file.
    **/
    const CImg<T>& save_yuv(const char *const filename,
                            const unsigned int chroma_subsampling=444,
                            const bool is_rgb=true) const {
      CImgList<T>(*this,true).save_yuv(filename,chroma_subsampling,is_rgb);
      return *this;
    }

    //! Save image as a .yuv video file \overloading.
    /**
       Same as save_yuv(const char*,const unsigned int,const bool) const
       with a file stream argument instead of a filename string.
    **/
    const CImg<T>& save_yuv(std::FILE *const file,
                            const unsigned int chroma_subsampling=444,
                            const bool is_rgb=true) const {
      CImgList<T>(*this,true).save_yuv(file,chroma_subsampling,is_rgb);
      return *this;
    }

    //! Save 3D object as an Object File Format (.off) file.
    /**
       \param filename Filename, as a C-string.
       \param primitives List of 3D object primitives.
       \param colors List of 3D object colors.
       \note
       - Instance image contains the vertices data of the 3D object.
       - Textured, transparent or sphere-shaped primitives cannot be managed by the .off file format.
       Such primitives will be lost or simplified during file saving.
       - The .off file format is <a href="http://people.sc.fsu.edu/~jburkardt/html/off_format.html">described here</a>.
    **/
    template<typename tf, typename tc>
    const CImg<T>& save_off(const CImgList<tf>& primitives, const CImgList<tc>& colors,
                            const char *const filename) const {
      return _save_off(primitives,colors,0,filename);
    }

    //! Save 3D object as an Object File Format (.off) file \overloading.
    /**
       Same as save_off(const CImgList<tf>&,const CImgList<tc>&,const char*) const
       with a file stream argument instead of a filename string.
    **/
    template<typename tf, typename tc>
    const CImg<T>& save_off(const CImgList<tf>& primitives, const CImgList<tc>& colors,
                            std::FILE *const file) const {
      return _save_off(primitives,colors,file,0);
    }

    template<typename tf, typename tc>
    const CImg<T>& _save_off(const CImgList<tf>& primitives, const CImgList<tc>& colors,
                             std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_off(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "save_off(): Empty instance, for file '%s'.",
                                    cimg_instance,
                                    filename?filename:"(FILE*)");

      CImgList<T> opacities;
      CImg<charT> error_message(1024);
      if (!is_object3d(primitives,colors,opacities,true,error_message))
        throw CImgInstanceException(_cimg_instance
                                    "save_off(): Invalid specified 3D object, for file '%s' (%s).",
                                    cimg_instance,
                                    filename?filename:"(FILE*)",error_message.data());

      const CImg<tc> default_color(1,3,1,1,(tc)std::min((int)cimg::type<tc>::max(),200));
      std::FILE *const nfile = file?file:cimg::fopen(filename,"w");
      unsigned int supported_primitives = 0;
      cimglist_for(primitives,l) if (primitives[l].size()!=5) ++supported_primitives;
      std::fprintf(nfile,"OFF\n%u %u %u\n",_width,supported_primitives,3*primitives._width);
      cimg_forX(*this,i) std::fprintf(nfile,"%f %f %f\n",
                                      (float)((*this)(i,0)),(float)((*this)(i,1)),(float)((*this)(i,2)));
      cimglist_for(primitives,l) {
        const CImg<tc>& color = l<colors.width()?colors[l]:default_color;
        const unsigned int psiz = primitives[l].size(), csiz = color.size();
        const float r = color[0]/255.f, g = (csiz>1?color[1]:r)/255.f, b = (csiz>2?color[2]:g)/255.f;
        switch (psiz) {
        case 1 : std::fprintf(nfile,"1 %u %f %f %f\n",
                              (unsigned int)primitives(l,0),r,g,b); break;
        case 2 : std::fprintf(nfile,"2 %u %u %f %f %f\n",
                              (unsigned int)primitives(l,0),(unsigned int)primitives(l,1),r,g,b); break;
        case 3 : std::fprintf(nfile,"3 %u %u %u %f %f %f\n",
                              (unsigned int)primitives(l,0),(unsigned int)primitives(l,2),
                              (unsigned int)primitives(l,1),r,g,b); break;
        case 4 : std::fprintf(nfile,"4 %u %u %u %u %f %f %f\n",
                              (unsigned int)primitives(l,0),(unsigned int)primitives(l,3),
                              (unsigned int)primitives(l,2),(unsigned int)primitives(l,1),r,g,b); break;
        case 5 : std::fprintf(nfile,"2 %u %u %f %f %f\n",
                              (unsigned int)primitives(l,0),(unsigned int)primitives(l,1),r,g,b); break;
        case 6 : {
          const unsigned int xt = (unsigned int)primitives(l,2), yt = (unsigned int)primitives(l,3);
          const float
            rt = color.atXY(xt,yt,0)/255.f,
            gt = (csiz>1?color.atXY(xt,yt,1):r)/255.f,
            bt = (csiz>2?color.atXY(xt,yt,2):g)/255.f;
          std::fprintf(nfile,"2 %u %u %f %f %f\n",
                       (unsigned int)primitives(l,0),(unsigned int)primitives(l,1),rt,gt,bt);
        } break;
        case 9 : {
          const unsigned int xt = (unsigned int)primitives(l,3), yt = (unsigned int)primitives(l,4);
          const float
            rt = color.atXY(xt,yt,0)/255.f,
            gt = (csiz>1?color.atXY(xt,yt,1):r)/255.f,
            bt = (csiz>2?color.atXY(xt,yt,2):g)/255.f;
          std::fprintf(nfile,"3 %u %u %u %f %f %f\n",
                       (unsigned int)primitives(l,0),(unsigned int)primitives(l,2),
                       (unsigned int)primitives(l,1),rt,gt,bt);
        } break;
        case 12 : {
          const unsigned int xt = (unsigned int)primitives(l,4), yt = (unsigned int)primitives(l,5);
          const float
            rt = color.atXY(xt,yt,0)/255.f,
            gt = (csiz>1?color.atXY(xt,yt,1):r)/255.f,
            bt = (csiz>2?color.atXY(xt,yt,2):g)/255.f;
          std::fprintf(nfile,"4 %u %u %u %u %f %f %f\n",
                       (unsigned int)primitives(l,0),(unsigned int)primitives(l,3),
                       (unsigned int)primitives(l,2),(unsigned int)primitives(l,1),rt,gt,bt);
        } break;
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save volumetric image as a video (using the OpenCV library when available).
    /**
      \param filename Filename to write data to.
      \param fps Number of frames per second.
      \param codec Type of compression (See http://www.fourcc.org/codecs.php to see available codecs).
      \param keep_open Tells if the video writer associated to the specified filename
        must be kept open or not (to allow frames to be added in the same file afterwards).
    **/
    const CImg<T>& save_video(const char *const filename, const unsigned int fps=25,
                              const char *codec=0, const bool keep_open=false) const {
      if (is_empty()) { CImgList<T>().save_video(filename,fps,codec,keep_open); return *this; }
      CImgList<T> list;
      get_split('z').move_to(list);
      list.save_video(filename,fps,codec,keep_open);
      return *this;
    }

    //! Save volumetric image as a video, using ffmpeg external binary.
    /**
       \param filename Filename, as a C-string.
       \param fps Video framerate.
       \param codec Video codec, as a C-string.
       \param bitrate Video bitrate.
       \note
       - Each slice of the instance image is considered to be a single frame of the output video file.
       - This method uses \c ffmpeg, an external executable binary provided by
         <a href="http://www.ffmpeg.org">FFmpeg</a>.
       It must be installed for the method to succeed.
    **/
    const CImg<T>& save_ffmpeg_external(const char *const filename, const unsigned int fps=25,
                                        const char *const codec=0, const unsigned int bitrate=2048) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_ffmpeg_external(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

      CImgList<T> list;
      get_split('z').move_to(list);
      list.save_ffmpeg_external(filename,fps,codec,bitrate);
      return *this;
    }

    //! Save image using gzip external binary.
    /**
       \param filename Filename, as a C-string.
       \note This method uses \c gzip, an external executable binary provided by
         <a href="//http://www.gzip.org">gzip</a>.
       It must be installed for the method to succeed.
    **/
    const CImg<T>& save_gzip_external(const char *const filename) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_gzip_external(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

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
      save(filename_tmp);
      cimg_snprintf(command,command._width,"\"%s\" -c \"%s\" > \"%s\"",
                    cimg::gzip_path(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data(),
                    CImg<charT>::string(filename)._system_strescape().data());
      if (cimg::system(command,cimg::gzip_path())!=0)
        throw CImgIOException(_cimg_instance
                              "save_gzip_external(): Failed to save file '%s' with external command 'gzip'.",
                              cimg_instance,
                              filename);
      if (!cimg::path_exists(filename))
        throw CImgIOException(_cimg_instance
                              "save_gzip_external(): Failed to save file '%s' with external command 'gzip'.",
                              cimg_instance,
                              filename);

      std::remove(filename_tmp);
      return *this;
    }

    //! Save image using GraphicsMagick's external binary.
    /**
       \param filename Filename, as a C-string.
       \param quality Image quality (expressed in percent), when the file format supports it.
       \note This method uses \c gm, an external executable binary provided by
         <a href="http://www.graphicsmagick.org">GraphicsMagick</a>.
       It must be installed for the method to succeed.
    **/
    const CImg<T>& save_graphicsmagick_external(const char *const filename, const unsigned int quality=100) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_graphicsmagick_external(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_other(): File '%s', saving a volumetric image with an external call to "
                   "GraphicsMagick only writes the first image slice.",
                   cimg_instance,filename);

#ifdef cimg_use_png
#define _cimg_sge_extension1 "png"
#define _cimg_sge_extension2 "png"
#else
#define _cimg_sge_extension1 "pgm"
#define _cimg_sge_extension2 "ppm"
#endif
      CImg<charT> command(1024), filename_tmp(256);
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),
                      _spectrum==1?_cimg_sge_extension1:_cimg_sge_extension2);
      } while (cimg::path_exists(filename_tmp));

#ifdef cimg_use_png
      save_png(filename_tmp);
#else
      save_pnm(filename_tmp);
#endif
      cimg_snprintf(command,command._width,"\"%s\" convert -quality %u \"%s\" \"%s\"",
                    cimg::graphicsmagick_path(),quality,
                    CImg<charT>::string(filename_tmp)._system_strescape().data(),
                    CImg<charT>::string(filename)._system_strescape().data());
      if (cimg::system(command,cimg::graphicsmagick_path())!=0)
        throw CImgIOException(_cimg_instance
                              "save_graphicsmagick_external(): Failed to save file '%s' with external command 'gm'.",
                              cimg_instance,
                              filename);
      if (!cimg::path_exists(filename))
        throw CImgIOException(_cimg_instance
                              "save_graphicsmagick_external(): Failed to save file '%s' with external command 'gm'.",
                              cimg_instance,
                              filename);

      std::remove(filename_tmp);
      return *this;
    }

    //! Save image using ImageMagick's external binary.
    /**
       \param filename Filename, as a C-string.
       \param quality Image quality (expressed in percent), when the file format supports it.
       \note This method uses \c convert, an external executable binary provided by
       <a href="http://www.imagemagick.org">ImageMagick</a>.
       It must be installed for the method to succeed.
    **/
    const CImg<T>& save_imagemagick_external(const char *const filename, const unsigned int quality=100) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_imagemagick_external(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_other(): File '%s', saving a volumetric image with an external call to "
                   "ImageMagick only writes the first image slice.",
                   cimg_instance,filename);
#ifdef cimg_use_png
#define _cimg_sie_extension1 "png"
#define _cimg_sie_extension2 "png"
#else
#define _cimg_sie_extension1 "pgm"
#define _cimg_sie_extension2 "ppm"
#endif
      CImg<charT> command(1024), filename_tmp(256);
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",cimg::temporary_path(),
                      cimg_file_separator,cimg::filenamerand(),_spectrum==1?_cimg_sie_extension1:_cimg_sie_extension2);
      } while (cimg::path_exists(filename_tmp));
#ifdef cimg_use_png
      save_png(filename_tmp);
#else
      save_pnm(filename_tmp);
#endif
      const char *magick_path = cimg::imagemagick_path();
      cimg_snprintf(command,command._width,"\"%s\" -quality %u \"%s\" \"%s\"",
                    magick_path,quality,
                    CImg<charT>::string(filename_tmp)._system_strescape().data(),
                    CImg<charT>::string(filename)._system_strescape().data());
      if (cimg::system(command,magick_path)!=0)
        throw CImgIOException(_cimg_instance
                              "save_imagemagick_external(): Failed to save file '%s' with "
                              "external command 'magick/convert'.",
                              cimg_instance,
                              filename);
      if (!cimg::path_exists(filename))
        throw CImgIOException(_cimg_instance
                              "save_imagemagick_external(): Failed to save file '%s' with "
                              "external command 'magick/convert'.",
                              cimg_instance,
                              filename);

      std::remove(filename_tmp);
      return *this;
    }

    //! Save image as a Dicom file.
    /**
       \param filename Filename, as a C-string.
       \note This method uses \c medcon, an external executable binary provided by
         <a href="http://xmedcon.sourceforge.net">(X)Medcon</a>.
       It must be installed for the method to succeed.
    **/
    const CImg<T>& save_medcon_external(const char *const filename) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_medcon_external(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

      CImg<charT> command(1024), filename_tmp(256), body(256);
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s.hdr",cimg::filenamerand());
      } while (cimg::path_exists(filename_tmp));
      save_analyze(filename_tmp);
      cimg_snprintf(command,command._width,"\"%s\" -w -c dicom -o \"%s\" -f \"%s\"",
                    cimg::medcon_path(),
                    CImg<charT>::string(filename)._system_strescape().data(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data());
      if (cimg::system(command,cimg::medcon_path())!=0)
        throw CImgIOException(_cimg_instance
                                "save_medcon_external(): Failed to save file '%s' with external command 'medcon'.",
                                cimg_instance,
                                filename);
      std::remove(filename_tmp);
      cimg::split_filename(filename_tmp,body);
      cimg_snprintf(filename_tmp,filename_tmp._width,"%s.img",body._data);
      std::remove(filename_tmp);

      if (!cimg::path_exists(filename)) {
        cimg_snprintf(command,command._width,"m000-%s",filename);
        if (!cimg::path_exists(command)) {
          cimg::fclose(cimg::fopen(filename,"r"));
          throw CImgIOException(_cimg_instance
                                "save_medcon_external(): Failed to save file '%s' with external command 'medcon'.",
                                cimg_instance,
                                filename);
        }
      }
      std::rename(command,filename);
      return *this;
    }

    // Save image for non natively supported formats.
    /**
       \param filename Filename, as a C-string.
       \param quality Image quality (expressed in percent), when the file format supports it.
       \note
       - The filename extension tells about the desired file format.
       - This method tries to save the instance image as a file, using external tools from
       <a href="http://www.imagemagick.org">ImageMagick</a> or
       <a href="http://www.graphicsmagick.org">GraphicsMagick</a>.
         At least one of these tool must be installed for the method to succeed.
       - It is recommended to use the generic method save(const char*, int) const instead,
         as it can handle some file formats natively.
    **/
    const CImg<T>& save_other(const char *const filename, const unsigned int quality=100) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_other(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_other(): File '%s', saving a volumetric image with an external call to "
                   "ImageMagick or GraphicsMagick only writes the first image slice.",
                   cimg_instance,filename);

      const unsigned int omode = cimg::exception_mode();
      bool is_saved = true;
      cimg::exception_mode(0);
      try { save_magick(filename); }
      catch (CImgException&) {
        try { save_imagemagick_external(filename,quality); }
        catch (CImgException&) {
          try { save_graphicsmagick_external(filename,quality); }
          catch (CImgException&) {
            is_saved = false;
          }
        }
      }
      cimg::exception_mode(omode);
      if (!is_saved)
        throw CImgIOException(_cimg_instance
                              "save_other(): Failed to save file '%s'. Format is not natively supported, "
                              "and no external commands succeeded.",
                              cimg_instance,
                              filename);
      return *this;
    }



#endif
