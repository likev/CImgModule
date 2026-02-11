#ifndef CIMG_MODULE_IMAGE_SAVE_BMP_PNM_H
#define CIMG_MODULE_IMAGE_SAVE_BMP_PNM_H

    const CImg<T>& save_bmp(const char *const filename) const {
      return _save_bmp(0,filename);
    }

    //! Save image as a BMP file \overloading.
    const CImg<T>& save_bmp(std::FILE *const file) const {
      return _save_bmp(file,0);
    }

    const CImg<T>& _save_bmp(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_bmp(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_bmp(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (_spectrum>3)
        cimg::warn(_cimg_instance
                   "save_bmp(): Instance is multispectral, only the three first channels will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      CImg<ucharT> header(54,1,1,1,0);
      unsigned char align_buf[4] = {};
      const unsigned int
        align = (4 - (3*_width)%4)%4,
        buf_size = (3*_width + align)*height(),
        file_size = 54 + buf_size;
      header[0] = 'B'; header[1] = 'M';
      header[0x02] = file_size&0xFF;
      header[0x03] = (file_size>>8)&0xFF;
      header[0x04] = (file_size>>16)&0xFF;
      header[0x05] = (file_size>>24)&0xFF;
      header[0x0A] = 0x36;
      header[0x0E] = 0x28;
      header[0x12] = _width&0xFF;
      header[0x13] = (_width>>8)&0xFF;
      header[0x14] = (_width>>16)&0xFF;
      header[0x15] = (_width>>24)&0xFF;
      header[0x16] = _height&0xFF;
      header[0x17] = (_height>>8)&0xFF;
      header[0x18] = (_height>>16)&0xFF;
      header[0x19] = (_height>>24)&0xFF;
      header[0x1A] = 1;
      header[0x1B] = 0;
      header[0x1C] = 24;
      header[0x1D] = 0;
      header[0x22] = buf_size&0xFF;
      header[0x23] = (buf_size>>8)&0xFF;
      header[0x24] = (buf_size>>16)&0xFF;
      header[0x25] = (buf_size>>24)&0xFF;
      header[0x27] = 0x1;
      header[0x2B] = 0x1;
      cimg::fwrite(header._data,54,nfile);

      const T
        *ptr_r = data(0,_height - 1,0,0),
        *ptr_g = (_spectrum>=2)?data(0,_height - 1,0,1):0,
        *ptr_b = (_spectrum>=3)?data(0,_height - 1,0,2):0;

      switch (_spectrum) {
      case 1 : {
        cimg_forY(*this,y) {
          cimg_forX(*this,x) {
            const unsigned char val = (unsigned char)*(ptr_r++);
            std::fputc(val,nfile); std::fputc(val,nfile); std::fputc(val,nfile);
          }
          cimg::fwrite(align_buf,align,nfile);
          ptr_r-=2*_width;
        }
      } break;
      case 2 : {
        cimg_forY(*this,y) {
          cimg_forX(*this,x) {
            std::fputc(0,nfile);
            std::fputc((unsigned char)(*(ptr_g++)),nfile);
            std::fputc((unsigned char)(*(ptr_r++)),nfile);
          }
          cimg::fwrite(align_buf,align,nfile);
          ptr_r-=2*_width; ptr_g-=2*_width;
        }
      } break;
      default : {
        cimg_forY(*this,y) {
          cimg_forX(*this,x) {
            std::fputc((unsigned char)(*(ptr_b++)),nfile);
            std::fputc((unsigned char)(*(ptr_g++)),nfile);
            std::fputc((unsigned char)(*(ptr_r++)),nfile);
          }
          cimg::fwrite(align_buf,align,nfile);
          ptr_r-=2*_width; ptr_g-=2*_width; ptr_b-=2*_width;
        }
      }
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save image as a WebP file.
    /**
      \param filename Filename, as a C-string.
      \param quality Image quality (in %)
    **/
    const CImg<T>& save_webp(const char *const filename, const int quality=100) const {
      return _save_webp(filename,quality);
    }

    const CImg<T>& _save_webp(const char *const filename, const int quality) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_webp(): Specified filename is (null).",
                                    cimg_instance);
      if (_spectrum!=3 && _spectrum!=4)
        throw CImgArgumentException(_cimg_instance
                                    "save_webp(): WebP only supports (A)RGB colorspace.",
                                    cimg_instance);
#ifndef cimg_use_webp
      cimg::unused(quality);
      return save_other(filename);
#else
      std::FILE *file = cimg::fopen(filename, "wb");
      CImg<uint8_t> rgbaBuffer(size());
      T *ptr_r = _data, *ptr_g = _data + 1UL*_width*_height,
        *ptr_b = _data + 2UL*_width*_height, *ptr_a = _spectrum==3?NULL:_data + 3UL*_width*_height;
      uint8_t *ptr = rgbaBuffer._data;
      cimg_forY(*this,y) {
        cimg_forX(*this,x) {
          *(ptr++) = (T)*(ptr_r++);
          *(ptr++) = (T)*(ptr_g++);
          *(ptr++) = (T)*(ptr_b++);
          if (ptr_a) *(ptr++) = (T)*(ptr_a++);
        }
      }
      uint8_t *imgData = NULL;
      const int stride = _width*_spectrum*sizeof(uint8_t);
      size_t size = 0;
      if (_spectrum==3) {
        size = WebPEncodeRGB(rgbaBuffer._data, _width, _height, stride, (float)quality, &imgData);
      } else {
        size = WebPEncodeRGBA(rgbaBuffer._data, _width, _height, stride, (float)quality, &imgData);
      }
      if (!imgData) {
        throw CImgIOException(_cimg_instance
                              "save_webp(): Failed to encode image to file '%s'.",
                              cimg_instance,
                              filename);
      }
      cimg::fwrite(imgData, size, file);
      cimg::fclose(file);
      WebPFree(imgData);
      return *this;
#endif
    }

    //! Save image as a JPEG file.
    /**
      \param filename Filename, as a C-string.
      \param quality Image quality (in %)
    **/
    const CImg<T>& save_jpeg(const char *const filename, const unsigned int quality=100) const {
      return _save_jpeg(0,filename,quality);
    }

    //! Save image as a JPEG file \overloading.
    const CImg<T>& save_jpeg(std::FILE *const file, const unsigned int quality=100) const {
      return _save_jpeg(file,0,quality);
    }

    const CImg<T>& _save_jpeg(std::FILE *const file, const char *const filename, const unsigned int quality) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_jpeg(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_jpeg(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

#ifndef cimg_use_jpeg
      if (!file) return save_other(filename,quality);
      else throw CImgIOException(_cimg_instance
                                 "save_jpeg(): Unable to save data in '(*FILE)' unless libjpeg is enabled.",
                                 cimg_instance);
#else
      unsigned int dimbuf = 0;
      J_COLOR_SPACE colortype = JCS_RGB;

      switch (_spectrum) {
      case 1 : dimbuf = 1; colortype = JCS_GRAYSCALE; break;
      case 2 : dimbuf = 3; colortype = JCS_RGB; break;
      case 3 : dimbuf = 3; colortype = JCS_RGB; break;
      default : dimbuf = 4; colortype = JCS_CMYK; break;
      }

      // Call libjpeg functions.
      struct jpeg_compress_struct cinfo;
      struct jpeg_error_mgr jerr;
      cinfo.err = jpeg_std_error(&jerr);
      jpeg_create_compress(&cinfo);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      jpeg_stdio_dest(&cinfo,nfile);
      cinfo.image_width = _width;
      cinfo.image_height = _height;
      cinfo.input_components = dimbuf;
      cinfo.in_color_space = colortype;
      jpeg_set_defaults(&cinfo);
      jpeg_set_quality(&cinfo,quality<100?quality:100,TRUE);
      jpeg_start_compress(&cinfo,TRUE);

      JSAMPROW row_pointer[1];
      CImg<ucharT> buffer(_width*dimbuf);

      while (cinfo.next_scanline<cinfo.image_height) {
        unsigned char *ptrd = buffer._data;

        // Fill pixel buffer.
        switch (_spectrum) {
        case 1 : { // Greyscale images
          const T *ptr_g = data(0, cinfo.next_scanline);
          for (unsigned int b = 0; b<cinfo.image_width; b++)
            *(ptrd++) = (unsigned char)*(ptr_g++);
        } break;
        case 2 : { // RG images
          const T *ptr_r = data(0,cinfo.next_scanline,0,0),
            *ptr_g = data(0,cinfo.next_scanline,0,1);
          for (unsigned int b = 0; b<cinfo.image_width; ++b) {
            *(ptrd++) = (unsigned char)*(ptr_r++);
            *(ptrd++) = (unsigned char)*(ptr_g++);
            *(ptrd++) = 0;
          }
        } break;
        case 3 : { // RGB images
          const T *ptr_r = data(0,cinfo.next_scanline,0,0),
            *ptr_g = data(0,cinfo.next_scanline,0,1),
            *ptr_b = data(0,cinfo.next_scanline,0,2);
          for (unsigned int b = 0; b<cinfo.image_width; ++b) {
            *(ptrd++) = (unsigned char)*(ptr_r++);
            *(ptrd++) = (unsigned char)*(ptr_g++);
            *(ptrd++) = (unsigned char)*(ptr_b++);
          }
        } break;
        default : { // CMYK images
          const T *ptr_r = data(0,cinfo.next_scanline,0,0),
            *ptr_g = data(0,cinfo.next_scanline,0,1),
            *ptr_b = data(0,cinfo.next_scanline,0,2),
            *ptr_a = data(0,cinfo.next_scanline,0,3);
          for (unsigned int b = 0; b<cinfo.image_width; ++b) {
            *(ptrd++) = (unsigned char)*(ptr_r++);
            *(ptrd++) = (unsigned char)*(ptr_g++);
            *(ptrd++) = (unsigned char)*(ptr_b++);
            *(ptrd++) = (unsigned char)*(ptr_a++);
          }
        }
        }
        *row_pointer = buffer._data;
        jpeg_write_scanlines(&cinfo,row_pointer,1);
      }
      jpeg_finish_compress(&cinfo);
      if (!file) cimg::fclose(nfile);
      jpeg_destroy_compress(&cinfo);
      return *this;
#endif
    }

    //! Save image as a JPEG XL file.
    /**
      \param filename Filename, as a C-string.
      \param distance Sets the level for lossy compression: lower = higher quality.
        Range: 0 .. 25. 0.0 = mathematically lossless
      \param bytes_per_pixel Force the number of bytes per pixels for the saving, when possible.
    **/
    const CImg<T>& save_jxl(const char *const filename, const float distance=1.0f,
                            const unsigned int bytes_per_pixel=0) const {
      return _save_jxl(filename,distance,bytes_per_pixel);
    }

    const CImg<T>& _save_jxl(const char *const filename, const float distance=1.0f,
                             const unsigned int bytes_per_pixel=0) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_jxl(): Specified filename is (null).",
                                    cimg_instance);
      if (_spectrum > 4)
        throw CImgArgumentException(_cimg_instance
                                    "save_jxl(): JPEG XL only supports at most 4 channels.",
                                    cimg_instance);
      double stmin, stmax = (double)max_min(stmin);
      if (stmin<0 || (bytes_per_pixel==1 && stmax>=256) || stmax>=65536)
        cimg::warn(_cimg_instance
                   "save_jxl(): Instance has pixel values in [%g,%g], probable type overflow in file '%s'.",
                   cimg_instance,
                   stmin,stmax,filename);
      if (bytes_per_pixel>2 || sizeof(T)<bytes_per_pixel)
        throw CImgArgumentException(_cimg_instance
                                    "save_jxl(): bytes_per_pixel must be in [0, 2] and less than or equal to sizeof(T)",
                                    cimg_instance);
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_jxl(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename);
#ifndef cimg_use_jxl
      cimg::unused(distance);
      return save_other(filename);
#else
      std::FILE *file = cimg::fopen(filename, "wb");
      cimg_uint32 nChannels = _spectrum;
      bool hasAlpha = _spectrum==2 || _spectrum==4;
      if (hasAlpha) --nChannels;
      unsigned int bytesPerPixel = bytes_per_pixel;
      if (bytesPerPixel==0)
          bytesPerPixel = stmax<256?1:2;
      CImg<ucharT> rgbBuffer(_width*_height*nChannels*bytesPerPixel);
      const T *ptr_r = 0, *ptr_g = 0, *ptr_b = 0, *ptr_a = 0;
      switch (_spectrum) {
      case 1:
        ptr_r = data(0,0,0,0);
        break;
      case 2:
        ptr_r = data(0,0,0,0);
        ptr_a = data(0,0,0,1);
        break;
      case 3:
        ptr_r = data(0,0,0,0);
        ptr_g = data(0,0,0,1);
        ptr_b = data(0,0,0,2);
        break;
      case 4:
        ptr_r = data(0,0,0,0);
        ptr_g = data(0,0,0,1);
        ptr_b = data(0,0,0,2);
        ptr_a = data(0,0,0,3);
        break;
      }
      switch (bytesPerPixel) {
      case 1: {
        unsigned char *ptr = (unsigned char*)rgbBuffer._data;
        cimg_forXY(*this,x,y) {
          *(ptr++) = (unsigned char)*(ptr_r++);
          if (ptr_g) *(ptr++) = (unsigned char)*(ptr_g++);
          if (ptr_b) *(ptr++) = (unsigned char)*(ptr_b++);
        }
      } break;
      case 2: {
        cimg_uint16 *ptr = (cimg_uint16*)rgbBuffer._data;
        cimg_forXY(*this,x,y) {
          *(ptr++) = (cimg_uint16)*(ptr_r++);
          if (ptr_g) *(ptr++) = (cimg_uint16)*(ptr_g++);
          if (ptr_b) *(ptr++) = (cimg_uint16)*(ptr_b++);
        }
      } break;
      }

      JxlEncoder *encoder = JxlEncoderCreate(NULL);

      JxlDataType dataType = bytesPerPixel==2?JXL_TYPE_UINT16:JXL_TYPE_UINT8;
      JxlPixelFormat pixelFormat = {
        nChannels,
        dataType,
        cimg::endianness()?JXL_BIG_ENDIAN:JXL_LITTLE_ENDIAN,
        0,
      };
      JxlBasicInfo basicInfo;
      JxlEncoderInitBasicInfo(&basicInfo);
      basicInfo.bits_per_sample = bytesPerPixel*8;
      basicInfo.exponent_bits_per_sample = 0;
      basicInfo.xsize = _width;
      basicInfo.ysize = _height;
      basicInfo.uses_original_profile = JXL_FALSE;
      if (hasAlpha) {
        basicInfo.alpha_bits = bytesPerPixel*8;
        basicInfo.alpha_exponent_bits = 0;
        basicInfo.num_extra_channels = 1;
      }
      basicInfo.num_color_channels = nChannels;
      if (JXL_ENC_SUCCESS!=JxlEncoderSetBasicInfo(encoder,&basicInfo)) {
        cimg::fclose(file);
        JxlEncoderDestroy(encoder);
        throw CImgIOException(_cimg_instance
                              "save_jxl(): Failed to set basic info when saving file '%s'.",
                              cimg_instance,
                              filename);
      }

      JxlColorEncoding colorEncoding = {};
      JXL_BOOL isGray = pixelFormat.num_channels<3?JXL_TRUE:JXL_FALSE;
      JxlColorEncodingSetToSRGB(&colorEncoding, isGray);
      if (JXL_ENC_SUCCESS!=JxlEncoderSetColorEncoding(encoder,&colorEncoding)) {
        cimg::fclose(file);
        JxlEncoderDestroy(encoder);
        throw CImgIOException(_cimg_instance
                              "save_jxl(): Failed to set color encoding when saving file '%s'.",
                              cimg_instance,
                              filename);
      }

      JxlEncoderFrameSettings* frameSettings = JxlEncoderFrameSettingsCreate(encoder,NULL);
      if (JXL_ENC_SUCCESS!=JxlEncoderSetFrameDistance(frameSettings,distance)) {
        cimg::fclose(file);
        JxlEncoderDestroy(encoder);
        throw CImgIOException(_cimg_instance
                              "save_jxl(): Failed to set lossy compression level when saving file '%s'.",
                              cimg_instance,
                              filename);
      }
      if (JXL_ENC_SUCCESS!=JxlEncoderAddImageFrame(frameSettings,&pixelFormat,(const void*)rgbBuffer._data,
                                                   rgbBuffer.size())) {
        cimg::fclose(file);
        JxlEncoderDestroy(encoder);
        throw CImgIOException(_cimg_instance
                              "save_jxl(): Failed to set image data when saving file '%s'.",
                              cimg_instance,
                              filename);
      }

      CImg<ucharT> alphaBuffer;
      if (hasAlpha) {
        ucharT *alphaPtr = (ucharT*)ptr_a;
        if (sizeof(T)!=bytesPerPixel) {
          alphaBuffer.assign(_width*_height*bytesPerPixel);
          switch (bytesPerPixel) {
          case 1: {
            unsigned char *ptr = (unsigned char*)alphaBuffer._data;
            cimg_forY(*this,y) {
              cimg_forX(*this,x) {
                *(ptr++) = (unsigned char)*(ptr_a++);
              }
            }
          } break;
          case 2: {
            cimg_uint16 *ptr = (cimg_uint16*)alphaBuffer._data;
            cimg_forY(*this,y) {
              cimg_forX(*this,x) {
                *(ptr++) = (cimg_uint16)*(ptr_a++);
              }
            }
          } break;
          }
          alphaPtr = (ucharT*)alphaBuffer._data;
        }
        if (JXL_ENC_SUCCESS!=JxlEncoderSetExtraChannelBuffer(frameSettings,&pixelFormat,(const void*)alphaPtr,
                                                             _width*_height*bytesPerPixel,0)) {
          cimg::fclose(file);
          JxlEncoderDestroy(encoder);
          throw CImgIOException(_cimg_instance
                                "save_jxl(): Failed to set alpha channel when saving file '%s'.",
                                cimg_instance,
                                filename);
        }
      }
      JxlEncoderCloseInput(encoder);

      CImg<cimg_uint8> compressed(1024);
      cimg_uint8 *nextOut = compressed.data();
      size_t availOut = compressed.size() - (nextOut - compressed.data());
      JxlEncoderStatus processResult = JXL_ENC_NEED_MORE_OUTPUT;
      while (processResult==JXL_ENC_NEED_MORE_OUTPUT) {
        processResult = JxlEncoderProcessOutput(encoder,&nextOut,&availOut);
        if (processResult==JXL_ENC_NEED_MORE_OUTPUT) {
          size_t offset = nextOut - compressed.data();
          compressed.resize(compressed.size()*2,1,1,1,-1);
          nextOut = compressed.data() + offset;
          availOut = compressed.size() - offset;
        }
      }
      compressed.resize(nextOut - compressed.data(),1,1,1,-1);
      if (JXL_ENC_SUCCESS!=processResult) {
        cimg::fclose(file);
        JxlEncoderDestroy(encoder);
        throw CImgIOException(_cimg_instance
                              "save_jxl(): Failed to encode image data when saving file '%s'.",
                              cimg_instance,
                              filename);
      }

      cimg::fwrite(compressed.data(),compressed.size(),file);
      cimg::fclose(file);
      JxlEncoderDestroy(encoder);
      return *this;
#endif
    }

    //! Save image, using built-in ImageMagick++ library.
    /**
      \param filename Filename, as a C-string.
      \param bytes_per_pixel Force the number of bytes per pixel for the saving, when possible.
    **/
    const CImg<T>& save_magick(const char *const filename, const unsigned int bytes_per_pixel=0) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_magick(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

#ifdef cimg_use_magick
      double stmin, stmax = (double)max_min(stmin);
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_magick(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename);

      if (_spectrum>3)
        cimg::warn(_cimg_instance
                   "save_magick(): Instance is multispectral, only the three first channels will be "
                   "saved in file '%s'.",
                   cimg_instance,
                   filename);

      if (stmin<0 || (bytes_per_pixel==1 && stmax>=256) || stmax>=65536)
        cimg::warn(_cimg_instance
                   "save_magick(): Instance has pixel values in [%g,%g], probable type overflow in file '%s'.",
                   cimg_instance,
                   stmin,stmax,filename);

      Magick::Image image(Magick::Geometry(_width,_height),"black");
      image.type(Magick::TrueColorType);
      image.depth(bytes_per_pixel?(8*bytes_per_pixel):(stmax>=256?16:8));
      const T
        *ptr_r = data(0,0,0,0),
        *ptr_g = _spectrum>1?data(0,0,0,1):0,
        *ptr_b = _spectrum>2?data(0,0,0,2):0;
      Magick::PixelPacket *pixels = image.getPixels(0,0,_width,_height);
      switch (_spectrum) {
      case 1 : // Scalar images
        for (ulongT off = (ulongT)_width*_height; off; --off) {
          pixels->red = pixels->green = pixels->blue = (Magick::Quantum)*(ptr_r++);
          ++pixels;
        }
        break;
      case 2 : // RG images
        for (ulongT off = (ulongT)_width*_height; off; --off) {
          pixels->red = (Magick::Quantum)*(ptr_r++);
          pixels->green = (Magick::Quantum)*(ptr_g++);
          pixels->blue = 0; ++pixels;
        }
        break;
      default : // RGB images
        for (ulongT off = (ulongT)_width*_height; off; --off) {
          pixels->red = (Magick::Quantum)*(ptr_r++);
          pixels->green = (Magick::Quantum)*(ptr_g++);
          pixels->blue = (Magick::Quantum)*(ptr_b++);
          ++pixels;
        }
      }
      image.syncPixels();
      image.write(filename);
      return *this;
#else
      cimg::unused(bytes_per_pixel);
      throw CImgIOException(_cimg_instance
                            "save_magick(): Unable to save file '%s' unless libMagick++ is enabled.",
                            cimg_instance,
                            filename);
#endif
    }

    //! Save image as a PNG file.
    /**
       \param filename Filename, as a C-string.
       \param bytes_per_pixel Force the number of bytes per pixels for the saving, when possible.
    **/
    const CImg<T>& save_png(const char *const filename, const unsigned int bytes_per_pixel=0) const {
      return _save_png(0,filename,bytes_per_pixel);
    }

    //! Save image as a PNG file \overloading.
    const CImg<T>& save_png(std::FILE *const file, const unsigned int bytes_per_pixel=0) const {
      return _save_png(file,0,bytes_per_pixel);
    }

    const CImg<T>& _save_png(std::FILE *const file, const char *const filename,
                             const unsigned int bytes_per_pixel=0) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_png(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

#ifndef cimg_use_png
      cimg::unused(bytes_per_pixel);
      if (!file) return save_other(filename);
      else throw CImgIOException(_cimg_instance
                                 "save_png(): Unable to save data in '(*FILE)' unless libpng is enabled.",
                                 cimg_instance);
#else

#if defined __GNUC__
      const char *volatile nfilename = filename; // Use 'volatile' to avoid (wrong) g++ warning
      std::FILE *volatile nfile = file?file:cimg::fopen(nfilename,"wb");
      volatile double stmin, stmax = (double)max_min(stmin);
#else
      const char *nfilename = filename;
      std::FILE *nfile = file?file:cimg::fopen(nfilename,"wb");
      double stmin, stmax = (double)max_min(stmin);
#endif

      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_png(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename);

      if (_spectrum>4)
        cimg::warn(_cimg_instance
                   "save_png(): Instance is multispectral, only the three first channels will be saved in file '%s'.",
                   cimg_instance,
                   filename);

      if (stmin<0 || (bytes_per_pixel==1 && stmax>=256) || stmax>=65536)
        cimg::warn(_cimg_instance
                   "save_png(): Instance has pixel values in [%g,%g], probable type overflow in file '%s'.",
                   cimg_instance,
                   stmin,stmax,filename);

      // Setup PNG structures for write.
      png_voidp user_error_ptr = 0;
      png_error_ptr user_error_fn = 0, user_warning_fn = 0;
      png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,user_error_ptr, user_error_fn,
                                                    user_warning_fn);
      if (!png_ptr){
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "save_png(): Failed to initialize 'png_ptr' structure when saving file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_infop info_ptr = png_create_info_struct(png_ptr);
      if (!info_ptr) {
        png_destroy_write_struct(&png_ptr,(png_infopp)0);
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "save_png(): Failed to initialize 'info_ptr' structure when saving file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "save_png(): Encountered unknown fatal error in libpng when saving file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_init_io(png_ptr, nfile);

      const int bit_depth = bytes_per_pixel?(bytes_per_pixel*8):(stmax>=256?16:8);

      int color_type;
      switch (spectrum()) {
      case 1 : color_type = PNG_COLOR_TYPE_GRAY; break;
      case 2 : color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
      case 3 : color_type = PNG_COLOR_TYPE_RGB; break;
      default : color_type = PNG_COLOR_TYPE_RGB_ALPHA;
      }
      const int interlace_type = PNG_INTERLACE_NONE;
      const int compression_type = PNG_COMPRESSION_TYPE_DEFAULT;
      const int filter_method = PNG_FILTER_TYPE_DEFAULT;
      png_set_IHDR(png_ptr,info_ptr,_width,_height,bit_depth,color_type,interlace_type,compression_type,filter_method);
      png_write_info(png_ptr,info_ptr);
      const int byte_depth = bit_depth>>3;
      const int numChan = spectrum()>4?4:spectrum();
      const int pixel_bit_depth_flag = numChan * (bit_depth - 1);

      // Allocate Memory for Image Save and Fill pixel data.
      png_bytep *const imgData = new png_byte*[_height];
      for (unsigned int row = 0; row<_height; ++row) imgData[row] = new png_byte[byte_depth*numChan*_width];
      const T *pC0 = data(0,0,0,0);
      switch (pixel_bit_depth_flag) {
      case 7 :  { // Gray 8-bit
        cimg_forY(*this,y) {
          unsigned char *ptrd = imgData[y];
          cimg_forX(*this,x) *(ptrd++) = (unsigned char)*(pC0++);
        }
      } break;
      case 14 : { // Gray w/ Alpha 8-bit
        const T *pC1 = data(0,0,0,1);
        cimg_forY(*this,y) {
          unsigned char *ptrd = imgData[y];
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned char)*(pC0++);
            *(ptrd++) = (unsigned char)*(pC1++);
          }
        }
      } break;
      case 21 :  { // RGB 8-bit
        const T *pC1 = data(0,0,0,1), *pC2 = data(0,0,0,2);
        cimg_forY(*this,y) {
          unsigned char *ptrd = imgData[y];
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned char)*(pC0++);
            *(ptrd++) = (unsigned char)*(pC1++);
            *(ptrd++) = (unsigned char)*(pC2++);
          }
        }
      } break;
      case 28 : { // RGB x/ Alpha 8-bit
        const T *pC1 = data(0,0,0,1), *pC2 = data(0,0,0,2), *pC3 = data(0,0,0,3);
        cimg_forY(*this,y){
          unsigned char *ptrd = imgData[y];
          cimg_forX(*this,x){
            *(ptrd++) = (unsigned char)*(pC0++);
            *(ptrd++) = (unsigned char)*(pC1++);
            *(ptrd++) = (unsigned char)*(pC2++);
            *(ptrd++) = (unsigned char)*(pC3++);
          }
        }
      } break;
      case 15 : { // Gray 16-bit
        cimg_forY(*this,y){
          unsigned short *ptrd = (unsigned short*)(imgData[y]);
          cimg_forX(*this,x) *(ptrd++) = (unsigned short)*(pC0++);
          if (!cimg::endianness()) cimg::invert_endianness((unsigned short*)imgData[y],_width);
        }
      } break;
      case 30 : { // Gray w/ Alpha 16-bit
        const T *pC1 = data(0,0,0,1);
        cimg_forY(*this,y){
          unsigned short *ptrd = (unsigned short*)(imgData[y]);
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned short)*(pC0++);
            *(ptrd++) = (unsigned short)*(pC1++);
          }
          if (!cimg::endianness()) cimg::invert_endianness((unsigned short*)imgData[y],2*_width);
        }
      } break;
      case 45 : { // RGB 16-bit
        const T *pC1 = data(0,0,0,1), *pC2 = data(0,0,0,2);
        cimg_forY(*this,y) {
          unsigned short *ptrd = (unsigned short*)(imgData[y]);
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned short)*(pC0++);
            *(ptrd++) = (unsigned short)*(pC1++);
            *(ptrd++) = (unsigned short)*(pC2++);
          }
          if (!cimg::endianness()) cimg::invert_endianness((unsigned short*)imgData[y],3*_width);
        }
      } break;
      case 60 : { // RGB w/ Alpha 16-bit
        const T *pC1 = data(0,0,0,1), *pC2 = data(0,0,0,2), *pC3 = data(0,0,0,3);
        cimg_forY(*this,y) {
          unsigned short *ptrd = (unsigned short*)(imgData[y]);
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned short)*(pC0++);
            *(ptrd++) = (unsigned short)*(pC1++);
            *(ptrd++) = (unsigned short)*(pC2++);
            *(ptrd++) = (unsigned short)*(pC3++);
          }
          if (!cimg::endianness()) cimg::invert_endianness((unsigned short*)imgData[y],4*_width);
        }
      } break;
      default :
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "save_png(): Encountered unknown fatal error in libpng when saving file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_write_image(png_ptr,imgData);
      png_write_end(png_ptr,info_ptr);
      png_destroy_write_struct(&png_ptr, &info_ptr);

      // Deallocate Image Write Memory.
      cimg_forY(*this,n) delete[] imgData[n];
      delete[] imgData;

      if (!file) cimg::fclose(nfile);
      return *this;
#endif
    }

    //! Save image as a PNM file.
    /**
      \param filename Filename, as a C-string.
      \param bytes_per_pixel Force the number of bytes per pixels for the saving.
    **/
    const CImg<T>& save_pnm(const char *const filename, const unsigned int bytes_per_pixel=0) const {
      return _save_pnm(0,filename,bytes_per_pixel);
    }

    //! Save image as a PNM file \overloading.
    const CImg<T>& save_pnm(std::FILE *const file, const unsigned int bytes_per_pixel=0) const {
      return _save_pnm(file,0,bytes_per_pixel);
    }

    const CImg<T>& _save_pnm(std::FILE *const file, const char *const filename,
                             const unsigned int bytes_per_pixel=0) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_pnm(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

      double stmin, stmax = (double)max_min(stmin);
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_pnm(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (_spectrum>3)
        cimg::warn(_cimg_instance
                   "save_pnm(): Instance is multispectral, only the three first channels will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (stmin<0 || (bytes_per_pixel==1 && stmax>=256) || stmax>=65536)
        cimg::warn(_cimg_instance
                   "save_pnm(): Instance has pixel values in [%g,%g], probable type overflow in file '%s'.",
                   cimg_instance,
                   stmin,stmax,filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const T
        *ptr_r = data(0,0,0,0),
        *ptr_g = (_spectrum>=2)?data(0,0,0,1):0,
        *ptr_b = (_spectrum>=3)?data(0,0,0,2):0;
      const ulongT buf_size = std::min((ulongT)(1024*1024),(ulongT)(_width*_height*(_spectrum==1?1UL:3UL)));

      std::fprintf(nfile,"P%c\n%u %u\n%u\n",
                   (_spectrum==1?'5':'6'),_width,_height,stmax<256?255:(stmax<4096?4095:65535));

      switch (_spectrum) {
      case 1 : { // Scalar image
        if (bytes_per_pixel==1 || (!bytes_per_pixel && stmax<256)) { // Binary PGM 8 bits
          CImg<ucharT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size);
            unsigned char *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) *(ptrd++) = (unsigned char)*(ptr_r++);
            cimg::fwrite(buf._data,N,nfile);
            to_write-=N;
          }
        } else { // Binary PGM 16 bits
          CImg<ushortT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size);
            unsigned short *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) *(ptrd++) = (unsigned short)*(ptr_r++);
            if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
            cimg::fwrite(buf._data,N,nfile);
            to_write-=N;
          }
        }
      } break;
      case 2 : { // RG image
        if (bytes_per_pixel==1 || (!bytes_per_pixel && stmax<256)) { // Binary PPM 8 bits
          CImg<ucharT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size/3);
            unsigned char *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) {
              *(ptrd++) = (unsigned char)*(ptr_r++);
              *(ptrd++) = (unsigned char)*(ptr_g++);
              *(ptrd++) = 0;
            }
            cimg::fwrite(buf._data,3*N,nfile);
            to_write-=N;
          }
        } else { // Binary PPM 16 bits
          CImg<ushortT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size/3);
            unsigned short *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) {
              *(ptrd++) = (unsigned short)*(ptr_r++);
              *(ptrd++) = (unsigned short)*(ptr_g++);
              *(ptrd++) = 0;
            }
            if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
            cimg::fwrite(buf._data,3*N,nfile);
            to_write-=N;
          }
        }
      } break;
      default : { // RGB image
        if (bytes_per_pixel==1 || (!bytes_per_pixel && stmax<256)) { // Binary PPM 8 bits
          CImg<ucharT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size/3);
            unsigned char *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) {
              *(ptrd++) = (unsigned char)*(ptr_r++);
              *(ptrd++) = (unsigned char)*(ptr_g++);
              *(ptrd++) = (unsigned char)*(ptr_b++);
            }
            cimg::fwrite(buf._data,3*N,nfile);
            to_write-=N;
          }
        } else { // Binary PPM 16 bits
          CImg<ushortT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size/3);
            unsigned short *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) {
              *(ptrd++) = (unsigned short)*(ptr_r++);
              *(ptrd++) = (unsigned short)*(ptr_g++);
              *(ptrd++) = (unsigned short)*(ptr_b++);
            }
            if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
            cimg::fwrite(buf._data,3*N,nfile);
            to_write-=N;
          }
        }
      }
      }
      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save image as a PNK file.
    /**
      \param filename Filename, as a C-string.
    **/
    const CImg<T>& save_pnk(const char *const filename) const {
      return _save_pnk(0,filename);
    }

    //! Save image as a PNK file \overloading.
    const CImg<T>& save_pnk(std::FILE *const file) const {
      return _save_pnk(file,0);
    }

    const CImg<T>& _save_pnk(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_pnk(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_spectrum>1)
        cimg::warn(_cimg_instance
                   "save_pnk(): Instance is multispectral, only the first channel will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      const ulongT buf_size = std::min((ulongT)1024*1024,(ulongT)_width*_height*_depth);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const T *ptr = data(0,0,0,0);

      if (!cimg::type<T>::is_float() && sizeof(T)==1 && _depth<2) // Can be saved as regular PNM file
        _save_pnm(file,filename,0);
      else if (!cimg::type<T>::is_float() && sizeof(T)==1) { // Save as extended P5 file: Binary byte-valued 3D
        std::fprintf(nfile,"P5\n%u %u %u\n255\n",_width,_height,_depth);
        CImg<ucharT> buf((unsigned int)buf_size);
        for (longT to_write = (longT)width()*height()*depth(); to_write>0; ) {
          const ulongT N = std::min((ulongT)to_write,buf_size);
          unsigned char *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) *(ptrd++) = (unsigned char)*(ptr++);
          cimg::fwrite(buf._data,N,nfile);
          to_write-=N;
        }
      } else if (!cimg::type<T>::is_float()) { // Save as P8: Binary int32-valued 3D
        if (_depth>1) std::fprintf(nfile,"P8\n%u %u %u\n%d\n",_width,_height,_depth,(int)max());
        else std::fprintf(nfile,"P8\n%u %u\n%d\n",_width,_height,(int)max());
        CImg<intT> buf((unsigned int)buf_size);
        for (longT to_write = (longT)width()*height()*depth(); to_write>0; ) {
          const ulongT N = std::min((ulongT)to_write,buf_size);
          int *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) *(ptrd++) = (int)*(ptr++);
          cimg::fwrite(buf._data,N,nfile);
          to_write-=N;
        }
      } else { // Save as P9: Binary float-valued 3D
        if (_depth>1) std::fprintf(nfile,"P9\n%u %u %u\n%g\n",_width,_height,_depth,(double)max());
        else std::fprintf(nfile,"P9\n%u %u\n%g\n",_width,_height,(double)max());
        CImg<floatT> buf((unsigned int)buf_size);
        for (longT to_write = (longT)width()*height()*depth(); to_write>0; ) {
          const ulongT N = std::min((ulongT)to_write,buf_size);
          float *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) *(ptrd++) = (float)*(ptr++);
          cimg::fwrite(buf._data,N,nfile);
          to_write-=N;
        }
      }

      if (!file) cimg::fclose(nfile);
      return *this;
    }

    //! Save image as a PFM file.
    /**
      \param filename Filename, as a C-string.
    **/
#endif
