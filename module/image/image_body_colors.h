#ifndef CIMG_MODULE_IMAGE_BODY_COLORS_H
#define CIMG_MODULE_IMAGE_BODY_COLORS_H

    //---------------------------------
    //
    //! \name Color Base Management
    //@{
    //---------------------------------

    //! Return palette \e "default", containing 256 colors entries in RGB.
    /**
       \return The following \c 256x1x1x3 palette is returned:
       \image html ref_colormap_default.jpg
    **/
    static const CImg<Tuchar>& default_LUT256() {
      static CImg<Tuchar> palette;
      cimg::mutex(8);
      if (!palette) {
        palette.assign(1,256,1,3);
        for (unsigned int index = 0, r = 16; r<256; r+=32)
          for (unsigned int g = 16; g<256; g+=32)
            for (unsigned int b = 32; b<256; b+=64) {
              palette(0,index,0) = (Tuchar)r;
              palette(0,index,1) = (Tuchar)g;
              palette(0,index++,2) = (Tuchar)b;
            }
      }
      cimg::mutex(8,0);
      return palette;
    }

    //! Return palette \e "HSV", containing 256 colors entries in RGB.
    /**
       \return The following \c 256x1x1x3 palette is returned:
       \image html ref_colormap_hsv.jpg
    **/
    static const CImg<Tuchar>& HSV_LUT256() {
      static CImg<Tuchar> palette;
      cimg::mutex(8);
      if (!palette) {
        CImg<Tint> tmp(1,256,1,3,1);
        tmp.get_shared_channel(0).sequence(0,359);
        palette = tmp.HSVtoRGB();
      }
      cimg::mutex(8,0);
      return palette;
    }

    //! Return palette \e "lines", containing 256 colors entries in RGB.
    /**
       \return The following \c 256x1x1x3 palette is returned:
       \image html ref_colormap_lines.jpg
    **/
    static const CImg<Tuchar>& lines_LUT256() {
      static const unsigned char pal[] = {
        0,255,255,0,0,28,125,125,235,210,186,182,36,0,125,255,
        53,32,255,210,89,186,65,45,125,210,210,97,130,194,0,125,
        206,53,190,89,255,146,20,190,154,73,255,36,130,215,0,138,
        101,210,61,194,206,0,77,45,255,154,174,0,190,239,89,125,
        16,36,158,223,117,0,97,69,223,255,40,239,0,0,255,0,
        97,170,93,255,138,40,117,210,0,170,53,158,186,255,0,121,
        227,121,186,40,20,190,89,255,77,57,130,142,255,73,186,85,
        210,8,32,166,243,130,210,40,255,45,61,142,223,49,121,255,
        20,162,158,73,89,255,53,138,210,190,57,235,36,73,255,49,
        210,0,210,85,57,97,255,121,85,174,40,255,162,178,0,121,
        166,125,53,146,166,255,97,121,65,89,235,231,12,170,36,190,
        85,255,166,97,198,77,20,146,109,166,255,28,40,202,121,81,
        247,0,210,255,49,0,65,255,36,166,93,77,255,85,251,0,
        170,178,0,182,255,0,162,16,154,142,162,223,223,0,0,81,
        215,4,215,162,215,125,77,206,121,36,125,231,101,16,255,121,
        0,57,190,215,65,125,89,142,255,101,73,53,146,223,125,125,
        0,255,0,255,0,206,93,138,49,255,0,202,154,85,45,219,
        251,53,0,255,40,130,219,158,16,117,186,130,202,49,65,239,
        89,202,49,28,247,134,150,0,255,117,202,4,215,81,186,57,
        202,89,73,210,40,93,45,251,206,28,223,142,40,134,162,125,
        32,247,97,170,0,255,57,134,73,247,162,0,251,40,142,142,
        8,166,206,81,154,194,93,89,125,243,28,109,227,0,190,65,
        194,186,0,255,53,45,109,186,186,0,255,130,49,170,69,210,
        154,0,109,227,45,255,125,105,81,81,255,0,219,134,170,85,
        146,28,170,89,223,97,8,210,255,158,49,40,125,174,174,125,
        0,227,166,28,219,130,0,93,239,0,85,255,81,178,125,49,
        89,255,53,206,73,113,146,255,0,150,36,219,162,0,210,125,
        69,134,255,85,40,89,235,49,215,121,0,206,36,223,174,69,
        40,182,178,130,69,45,255,210,85,77,215,0,231,146,0,194,
        125,174,0,255,40,89,121,206,57,0,206,170,231,150,81,0,
        125,255,4,174,4,190,121,255,4,166,109,130,49,239,170,93,
        16,174,210,0,255,16,105,158,93,255,0,125,0,255,158,85,
        0,255,0,0,255,170,166,61,121,28,198,215,45,243,61,97,
        255,53,81,130,109,255,8,117,235,121,40,178,174,0,182,49,
        162,121,255,69,206,0,219,125,0,101,255,239,121,32,210,130,
        36,231,32,125,81,142,215,158,4,178,255,0,40,251,125,125,
        219,89,130,0,166,255,24,65,194,125,255,125,77,125,93,125,
        202,24,138,174,178,32,255,85,194,40,85,36,174,174,125,210,
        85,255,53,16,93,206,40,130,170,202,93,255,0,24,117,255,
        97,113,105,81,255,186,194,57,69,206,57,53,223,190,4,255,
        85,97,130,255,85,0,125,223,85,219,0,215,146,77,40,239,
        89,36,142,154,227,0,255,85,162,0,162,0,235,178,45,166,
        0,247,255,20,69,210,89,142,53,255,40,146,166,255,69,0,
        174,154,142,130,162,0,215,255,0,89,40,255,166,61,146,69,
        162,40,255,32,121,255,117,178,0,186,206,0,57,215,215,81,
        158,77,166,210,77,89,210,0,24,202,150,186,0,255,20,97,
        57,170,235,251,16,73,142,251,93,0,202,0,255,121,219,4,
        73,219,8,162,206,16,219,93,117,0,255,8,130,174,223,45 };
      static const CImg<Tuchar> palette(pal,1,256,1,3,false);
      return palette;
    }

    //! Return palette \e "hot", containing 256 colors entries in RGB.
    /**
       \return The following \c 256x1x1x3 palette is returned:
       \image html ref_colormap_hot.jpg
    **/
    static const CImg<Tuchar>& hot_LUT256() {
      static CImg<Tuchar> palette;
      cimg::mutex(8);
      if (!palette) {
        palette.assign(1,4,1,3,(T)0);
        palette[1] = palette[2] = palette[3] = palette[6] = palette[7] = palette[11] = 255;
        palette.resize(1,256,1,3,3);
      }
      cimg::mutex(8,0);
      return palette;
    }

    //! Return palette \e "cool", containing 256 colors entries in RGB.
    /**
       \return The following \c 256x1x1x3 palette is returned:
       \image html ref_colormap_cool.jpg
    **/
    static const CImg<Tuchar>& cool_LUT256() {
      static CImg<Tuchar> palette;
      cimg::mutex(8);
      if (!palette) palette.assign(1,2,1,3).fill((T)0,(T)255,(T)255,(T)0,(T)255,(T)255).resize(1,256,1,3,3);
      cimg::mutex(8,0);
      return palette;
    }

    //! Return palette \e "jet", containing 256 colors entries in RGB.
    /**
       \return The following \c 256x1x1x3 palette is returned:
       \image html ref_colormap_jet.jpg
    **/
    static const CImg<Tuchar>& jet_LUT256() {
      static CImg<Tuchar> palette;
      cimg::mutex(8);
      if (!palette) {
        palette.assign(1,4,1,3,(T)0);
        palette[2] = palette[3] = palette[5] = palette[6] = palette[8] = palette[9] = 255;
        palette.resize(1,256,1,3,3);
      }
      cimg::mutex(8,0);
      return palette;
    }

    //! Return palette \e "flag", containing 256 colors entries in RGB.
    /**
       \return The following \c 256x1x1x3 colormap is returned:
       \image html ref_palette_flag.jpg
    **/
    static const CImg<Tuchar>& flag_LUT256() {
      static CImg<Tuchar> palette;
      cimg::mutex(8);
      if (!palette) {
        palette.assign(1,4,1,3,(T)0);
        palette[0] = palette[1] = palette[5] = palette[9] = palette[10] = 255;
        palette.resize(1,256,1,3,0,2);
      }
      cimg::mutex(8,0);
      return palette;
    }

    //! Return palette \e "cube", containing 256 colors entries in RGB.
    /**
       \return The following \c 256x1x1x3 palette is returned:
       \image html ref_colormap_cube.jpg
    **/
    static const CImg<Tuchar>& cube_LUT256() {
      static CImg<Tuchar> palette;
      cimg::mutex(8);
      if (!palette) {
        palette.assign(1,8,1,3,(T)0);
        palette[1] = palette[3] = palette[5] = palette[7] =
          palette[10] = palette[11] = palette[12] = palette[13] =
          palette[20] = palette[21] = palette[22] = palette[23] = 255;
        palette.resize(1,256,1,3,3);
      }
      cimg::mutex(8,0);
      return palette;
    }

    //! Convert pixel values from sRGB to RGB color spaces.
    CImg<T>& sRGBtoRGB() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32))
      cimg_rofoff(*this,off) {
        const Tfloat
          sval = (Tfloat)_data[off]/255,
          val = (Tfloat)(sval<=0.04045f?sval/12.92f:std::pow((sval + 0.055f)/(1.055f),2.4f));
        _data[off] = (T)cimg::cut(val*255,(Tfloat)0,(Tfloat)255);
      }
      return *this;
    }

    //! Convert pixel values from sRGB to RGB color spaces \newinstance.
    CImg<Tfloat> get_sRGBtoRGB() const {
      return CImg<Tfloat>(*this,false).sRGBtoRGB();
    }

    //! Convert pixel values from RGB to sRGB color spaces.
    CImg<T>& RGBtosRGB() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32))
      cimg_rofoff(*this,off) {
        const Tfloat
          val = (Tfloat)_data[off]/255,
          sval = (Tfloat)(val<=0.0031308f?val*12.92f:1.055f*std::pow(val,0.416667f) - 0.055f);
        _data[off] = (T)cimg::cut(sval*255,(Tfloat)0,(Tfloat)255);
      }
      return *this;
    }

    //! Convert pixel values from RGB to sRGB color spaces \newinstance.
    CImg<Tfloat> get_RGBtosRGB() const {
      return CImg<Tfloat>(*this,false).RGBtosRGB();
    }

    //! Convert pixel values from RGB to HSI color spaces.
    CImg<T>& RGBtoHSI() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoHSI(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,256))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          m = cimg::min(R,G,B),
          M = cimg::max(R,G,B),
          C = M - m,
          sum = R + G + B,
          H = 60*(C==0?0:M==R?cimg::mod((G - B)/C,(Tfloat)6):M==G?(B - R)/C + 2:(R - G)/C + 4),
          S = sum<=0?0:1 - 3*m/sum,
          I = sum/(3*255);
        p1[N] = (T)H;
        p2[N] = (T)S;
        p3[N] = (T)I;
      }
      return *this;
    }

    //! Convert pixel values from RGB to HSI color spaces \newinstance.
    CImg<Tfloat> get_RGBtoHSI() const {
      return CImg<Tfloat>(*this,false).RGBtoHSI();
    }

    //! Convert pixel values from HSI to RGB color spaces.
    CImg<T>& HSItoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "HSItoRGB(): Instance is not a HSI image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,256))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          H = cimg::mod((Tfloat)p1[N]/60,(Tfloat)6),
          S = (Tfloat)p2[N],
          I = (Tfloat)p3[N],
          Z = 1 - cimg::abs(cimg::mod(H,(Tfloat)2) - 1),
          C = I*S/(1 + Z),
          X = C*Z,
          m = I*(1 - S)/3;
        Tfloat R, G, B;
        switch ((int)H) {
        case 0 : R = C; G = X; B = 0; break;
        case 1 : R = X; G = C; B = 0; break;
        case 2 : R = 0; G = C; B = X; break;
        case 3 : R = 0; G = X; B = C; break;
        case 4 : R = X; G = 0; B = C; break;
        default : R = C; G = 0; B = X;
        }
        p1[N] = (T)((R + m)*3*255);
        p2[N] = (T)((G + m)*3*255);
        p3[N] = (T)((B + m)*3*255);
      }
      return *this;
    }

    //! Convert pixel values from HSI to RGB color spaces \newinstance.
    CImg<Tfloat> get_HSItoRGB() const {
      return CImg<Tuchar>(*this,false).HSItoRGB();
    }

    //! Convert pixel values from RGB to HSL color spaces.
    CImg<T>& RGBtoHSL() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoHSL(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,256))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          m = cimg::min(R,G,B),
          M = cimg::max(R,G,B),
          C = M - m,
          H = 60*(C==0?0:M==R?cimg::mod((G - B)/C,(Tfloat)6):M==G?(B - R)/C + 2:(R - G)/C + 4),
          L = 0.5f*(m + M)/255,
          S = L==1 || L==0?0:C/(1 - cimg::abs(2*L - 1))/255;
        p1[N] = (T)H;
        p2[N] = (T)S;
        p3[N] = (T)L;
      }
      return *this;
    }

    //! Convert pixel values from RGB to HSL color spaces \newinstance.
    CImg<Tfloat> get_RGBtoHSL() const {
      return CImg<Tfloat>(*this,false).RGBtoHSL();
    }

    //! Convert pixel values from HSL to RGB color spaces.
    CImg<T>& HSLtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "HSLtoRGB(): Instance is not a HSL image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,256))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          H = cimg::mod((Tfloat)p1[N]/60,(Tfloat)6),
          S = (Tfloat)p2[N],
          L = (Tfloat)p3[N],
          C = (1 - cimg::abs(2*L - 1))*S,
          X = C*(1 - cimg::abs(cimg::mod(H,(Tfloat)2) - 1)),
          m = L - C/2;
        Tfloat R, G, B;
        switch ((int)H) {
        case 0 : R = C; G = X; B = 0; break;
        case 1 : R = X; G = C; B = 0; break;
        case 2 : R = 0; G = C; B = X; break;
        case 3 : R = 0; G = X; B = C; break;
        case 4 : R = X; G = 0; B = C; break;
        default : R = C; G = 0; B = X;
        }
        p1[N] = (T)((R + m)*255);
        p2[N] = (T)((G + m)*255);
        p3[N] = (T)((B + m)*255);
      }
      return *this;
    }

    //! Convert pixel values from HSL to RGB color spaces \newinstance.
    CImg<Tuchar> get_HSLtoRGB() const {
      return CImg<Tuchar>(*this,false).HSLtoRGB();
    }

    //! Convert pixel values from RGB to HSV color spaces.
    CImg<T>& RGBtoHSV() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoHSV(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,256))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          M = cimg::max(R,G,B),
          C = M - cimg::min(R,G,B),
          H = 60*(C==0?0:M==R?cimg::mod((G-B)/C,(Tfloat)6):M==G?(B - R)/C + 2:(R - G)/C + 4),
          S = M<=0?0:C/M;
        p1[N] = (T)H;
        p2[N] = (T)S;
        p3[N] = (T)(M/255);
      }
      return *this;
    }

    //! Convert pixel values from RGB to HSV color spaces \newinstance.
    CImg<Tfloat> get_RGBtoHSV() const {
      return CImg<Tfloat>(*this,false).RGBtoHSV();
    }

    //! Convert pixel values from HSV to RGB color spaces.
    CImg<T>& HSVtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "HSVtoRGB(): Instance is not a HSV image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,256))
      for (longT N = 0; N<whd; ++N) {
        Tfloat
          H = cimg::mod((Tfloat)p1[N]/60,(Tfloat)6),
          S = (Tfloat)p2[N],
          V = (Tfloat)p3[N],
          C = V*S,
          X = C*(1 - cimg::abs(cimg::mod(H,(Tfloat)2) - 1)),
          m = V - C;
        Tfloat R, G, B;
        switch ((int)H) {
        case 0 : R = C; G = X; B = 0; break;
        case 1 : R = X; G = C; B = 0; break;
        case 2 : R = 0; G = C; B = X; break;
        case 3 : R = 0; G = X; B = C; break;
        case 4 : R = X; G = 0; B = C; break;
        default : R = C; G = 0; B = X;
        }
        p1[N] = (T)((R + m)*255);
        p2[N] = (T)((G + m)*255);
        p3[N] = (T)((B + m)*255);
      }
      return *this;
    }

    //! Convert pixel values from HSV to RGB color spaces \newinstance.
    CImg<Tuchar> get_HSVtoRGB() const {
      return CImg<Tuchar>(*this,false).HSVtoRGB();
    }

    //! Convert pixel values from RGB to YCbCr color spaces.
    CImg<T>& RGBtoYCbCr() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoYCbCr(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,512))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          Y = (66*R + 129*G + 25*B + 128)/256 + 16,
          Cb = (-38*R - 74*G + 112*B + 128)/256 + 128,
          Cr = (112*R - 94*G - 18*B + 128)/256 + 128;
        p1[N] = (T)cimg::cut(Y,(Tfloat)0,(Tfloat)255),
        p2[N] = (T)cimg::cut(Cb,(Tfloat)0,(Tfloat)255),
        p3[N] = (T)cimg::cut(Cr,(Tfloat)0,(Tfloat)255);
      }
      return *this;
    }

    //! Convert pixel values from RGB to YCbCr color spaces \newinstance.
    CImg<Tuchar> get_RGBtoYCbCr() const {
      return CImg<Tuchar>(*this,false).RGBtoYCbCr();
    }

    //! Convert pixel values from RGB to YCbCr color spaces.
    CImg<T>& YCbCrtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "YCbCrtoRGB(): Instance is not a YCbCr image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,512))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          Y = (Tfloat)p1[N] - 16,
          Cb = (Tfloat)p2[N] - 128,
          Cr = (Tfloat)p3[N] - 128,
          R = (298*Y + 409*Cr + 128)/256,
          G = (298*Y - 100*Cb - 208*Cr + 128)/256,
          B = (298*Y + 516*Cb + 128)/256;
        p1[N] = (T)cimg::cut(R,(Tfloat)0,(Tfloat)255),
        p2[N] = (T)cimg::cut(G,(Tfloat)0,(Tfloat)255),
        p3[N] = (T)cimg::cut(B,(Tfloat)0,(Tfloat)255);
      }
      return *this;
    }

    //! Convert pixel values from RGB to YCbCr color spaces \newinstance.
    CImg<Tuchar> get_YCbCrtoRGB() const {
      return CImg<Tuchar>(*this,false).YCbCrtoRGB();
    }

    //! Convert pixel values from RGB to YUV color spaces.
    CImg<T>& RGBtoYUV() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoYUV(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,16384))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N]/255,
          G = (Tfloat)p2[N]/255,
          B = (Tfloat)p3[N]/255,
          Y = 0.299f*R + 0.587f*G + 0.114f*B;
        p1[N] = (T)Y;
        p2[N] = (T)(0.492f*(B - Y));
        p3[N] = (T)(0.877*(R - Y));
      }
      return *this;
    }

    //! Convert pixel values from RGB to YUV color spaces \newinstance.
    CImg<Tfloat> get_RGBtoYUV() const {
      return CImg<Tfloat>(*this,false).RGBtoYUV();
    }

    //! Convert pixel values from YUV to RGB color spaces.
    CImg<T>& YUVtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "YUVtoRGB(): Instance is not a YUV image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,16384))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          Y = (Tfloat)p1[N],
          U = (Tfloat)p2[N],
          V = (Tfloat)p3[N],
          R = (Y + 1.140f*V)*255,
          G = (Y - 0.395f*U - 0.581f*V)*255,
          B = (Y + 2.032f*U)*255;
        p1[N] = (T)cimg::cut(R,(Tfloat)0,(Tfloat)255),
        p2[N] = (T)cimg::cut(G,(Tfloat)0,(Tfloat)255),
        p3[N] = (T)cimg::cut(B,(Tfloat)0,(Tfloat)255);
      }
      return *this;
    }

    //! Convert pixel values from YUV to RGB color spaces \newinstance.
    CImg<Tuchar> get_YUVtoRGB() const {
      return CImg<Tuchar>(*this,false).YUVtoRGB();
    }

    //! Convert pixel values from RGB to CMY color spaces.
    CImg<T>& RGBtoCMY() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoCMY(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,2048))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          C = 255 - R,
          M = 255 - G,
          Y = 255 - B;
        p1[N] = (T)cimg::cut(C,(Tfloat)0,(Tfloat)255),
        p2[N] = (T)cimg::cut(M,(Tfloat)0,(Tfloat)255),
        p3[N] = (T)cimg::cut(Y,(Tfloat)0,(Tfloat)255);
      }
      return *this;
    }

    //! Convert pixel values from RGB to CMY color spaces \newinstance.
    CImg<Tuchar> get_RGBtoCMY() const {
      return CImg<Tfloat>(*this,false).RGBtoCMY();
    }

    //! Convert pixel values from CMY to RGB color spaces.
    CImg<T>& CMYtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "CMYtoRGB(): Instance is not a CMY image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,2048))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          C = (Tfloat)p1[N],
          M = (Tfloat)p2[N],
          Y = (Tfloat)p3[N],
          R = 255 - C,
          G = 255 - M,
          B = 255 - Y;
        p1[N] = (T)cimg::cut(R,(Tfloat)0,(Tfloat)255),
        p2[N] = (T)cimg::cut(G,(Tfloat)0,(Tfloat)255),
        p3[N] = (T)cimg::cut(B,(Tfloat)0,(Tfloat)255);
      }
      return *this;
    }

    //! Convert pixel values from CMY to RGB color spaces \newinstance.
    CImg<Tuchar> get_CMYtoRGB() const {
      return CImg<Tuchar>(*this,false).CMYtoRGB();
    }

    //! Convert pixel values from CMY to CMYK color spaces.
    CImg<T>& CMYtoCMYK() {
      return get_CMYtoCMYK().move_to(*this);
    }

    //! Convert pixel values from CMY to CMYK color spaces \newinstance.
    CImg<Tuchar> get_CMYtoCMYK() const {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "CMYtoCMYK(): Instance is not a CMY image.",
                                    cimg_instance);

      CImg<Tfloat> res(_width,_height,_depth,4);
      const T *ps1 = data(0,0,0,0), *ps2 = data(0,0,0,1), *ps3 = data(0,0,0,2);
      Tfloat *pd1 = res.data(0,0,0,0), *pd2 = res.data(0,0,0,1), *pd3 = res.data(0,0,0,2), *pd4 = res.data(0,0,0,3);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,1024))
      for (longT N = 0; N<whd; ++N) {
        Tfloat
          C = (Tfloat)ps1[N],
          M = (Tfloat)ps2[N],
          Y = (Tfloat)ps3[N],
          K = cimg::min(C,M,Y);
        if (K>=255) C = M = Y = 0;
        else { const Tfloat K1 = 255 - K; C = 255*(C - K)/K1; M = 255*(M - K)/K1; Y = 255*(Y - K)/K1; }
        pd1[N] = (Tfloat)cimg::cut(C,(Tfloat)0,(Tfloat)255),
        pd2[N] = (Tfloat)cimg::cut(M,(Tfloat)0,(Tfloat)255),
        pd3[N] = (Tfloat)cimg::cut(Y,(Tfloat)0,(Tfloat)255),
        pd4[N] = (Tfloat)cimg::cut(K,(Tfloat)0,(Tfloat)255);
      }
      return res;
    }

    //! Convert pixel values from CMYK to CMY color spaces.
    CImg<T>& CMYKtoCMY() {
      return get_CMYKtoCMY().move_to(*this);
    }

    //! Convert pixel values from CMYK to CMY color spaces \newinstance.
    CImg<Tfloat> get_CMYKtoCMY() const {
      if (_spectrum!=4)
        throw CImgInstanceException(_cimg_instance
                                    "CMYKtoCMY(): Instance is not a CMYK image.",
                                    cimg_instance);

      CImg<Tfloat> res(_width,_height,_depth,3);
      const T *ps1 = data(0,0,0,0), *ps2 = data(0,0,0,1), *ps3 = data(0,0,0,2), *ps4 = data(0,0,0,3);
      Tfloat *pd1 = res.data(0,0,0,0), *pd2 = res.data(0,0,0,1), *pd3 = res.data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,1024))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          C = (Tfloat)ps1[N],
          M = (Tfloat)ps2[N],
          Y = (Tfloat)ps3[N],
          K = (Tfloat)ps4[N],
          K1 = 1 - K/255,
          nC = C*K1 + K,
          nM = M*K1 + K,
          nY = Y*K1 + K;
        pd1[N] = (Tfloat)cimg::cut(nC,(Tfloat)0,(Tfloat)255),
        pd2[N] = (Tfloat)cimg::cut(nM,(Tfloat)0,(Tfloat)255),
        pd3[N] = (Tfloat)cimg::cut(nY,(Tfloat)0,(Tfloat)255);
      }
      return res;
    }

    //! Convert pixel values from RGB to XYZ color spaces.
    /**
       \param use_D65 Tell to use the D65 illuminant (D50 otherwise).
    **/
    CImg<T>& RGBtoXYZ(const bool use_D65=true) {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoXYZ(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,2048))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N]/255,
          G = (Tfloat)p2[N]/255,
          B = (Tfloat)p3[N]/255;
        if (use_D65) { // D65
          p1[N] = (T)(0.4124564*R + 0.3575761*G + 0.1804375*B);
          p2[N] = (T)(0.2126729*R + 0.7151522*G + 0.0721750*B);
          p3[N] = (T)(0.0193339*R + 0.1191920*G + 0.9503041*B);
        } else { // D50
          p1[N] = (T)(0.43603516*R + 0.38511658*G + 0.14305115*B);
          p2[N] = (T)(0.22248840*R + 0.71690369*G + 0.06060791*B);
          p3[N] = (T)(0.01391602*R + 0.09706116*G + 0.71392822*B);
        }
      }
      return *this;
    }

    //! Convert pixel values from RGB to XYZ color spaces \newinstance.
    CImg<Tfloat> get_RGBtoXYZ(const bool use_D65=true) const {
      return CImg<Tfloat>(*this,false).RGBtoXYZ(use_D65);
    }

    //! Convert pixel values from XYZ to RGB color spaces.
    /**
       \param use_D65 Tell to use the D65 illuminant (D50 otherwise).
    **/
    CImg<T>& XYZtoRGB(const bool use_D65=true) {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "XYZtoRGB(): Instance is not a XYZ image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,2048))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          X = (Tfloat)p1[N]*255,
          Y = (Tfloat)p2[N]*255,
          Z = (Tfloat)p3[N]*255;
        if (use_D65) {
          p1[N] = (T)cimg::cut(3.2404542*X - 1.5371385*Y - 0.4985314*Z,0.,255.);
          p2[N] = (T)cimg::cut(-0.9692660*X + 1.8760108*Y + 0.0415560*Z,0.,255.);
          p3[N] = (T)cimg::cut(0.0556434*X - 0.2040259*Y + 1.0572252*Z,0.,255.);
        } else {
          p1[N] = (T)cimg::cut(3.134274799724*X  - 1.617275708956*Y - 0.490724283042*Z,0.,255.);
          p2[N] = (T)cimg::cut(-0.978795575994*X + 1.916161689117*Y + 0.033453331711*Z,0.,255.);
          p3[N] = (T)cimg::cut(0.071976988401*X - 0.228984974402*Y + 1.405718224383*Z,0.,255.);
        }
      }
      return *this;
    }

    //! Convert pixel values from XYZ to RGB color spaces \newinstance.
    CImg<Tuchar> get_XYZtoRGB(const bool use_D65=true) const {
      return CImg<Tuchar>(*this,false).XYZtoRGB(use_D65);
    }

    //! Convert pixel values from XYZ to Lab color spaces.
    CImg<T>& XYZtoLab(const bool use_D65=true) {
#define _cimg_Labf(x) (24389*(x)>216?cimg::cbrt(x):(24389*(x)/27 + 16)/116)

      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "XYZtoLab(): Instance is not a XYZ image.",
                                    cimg_instance);
      const CImg<Tfloat> white = CImg<Tfloat>(1,1,1,3,255).RGBtoXYZ(use_D65);
      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,128))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          X = (Tfloat)(p1[N]/white[0]),
          Y = (Tfloat)(p2[N]/white[1]),
          Z = (Tfloat)(p3[N]/white[2]),
          fX = (Tfloat)_cimg_Labf(X),
          fY = (Tfloat)_cimg_Labf(Y),
          fZ = (Tfloat)_cimg_Labf(Z);
        p1[N] = (T)cimg::cut(116*fY - 16,(Tfloat)0,(Tfloat)100);
        p2[N] = (T)(500*(fX - fY));
        p3[N] = (T)(200*(fY - fZ));
      }
      return *this;
    }

    //! Convert pixel values from XYZ to Lab color spaces \newinstance.
    CImg<Tfloat> get_XYZtoLab(const bool use_D65=true) const {
      return CImg<Tfloat>(*this,false).XYZtoLab(use_D65);
    }

    //! Convert pixel values from Lab to XYZ color spaces.
    CImg<T>& LabtoXYZ(const bool use_D65=true) {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "LabtoXYZ(): Instance is not a Lab image.",
                                    cimg_instance);
      const CImg<Tfloat> white = CImg<Tfloat>(1,1,1,3,255).RGBtoXYZ(use_D65);
      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,128))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          L = (Tfloat)p1[N],
          a = (Tfloat)p2[N],
          b = (Tfloat)p3[N],
          cY = (L + 16)/116,
          cZ = cY - b/200,
          cX = a/500 + cY,
          X = (Tfloat)(24389*cX>216?cX*cX*cX:(116*cX - 16)*27/24389),
          Y = (Tfloat)(27*L>216?cY*cY*cY:27*L/24389),
          Z = (Tfloat)(24389*cZ>216?cZ*cZ*cZ:(116*cZ - 16)*27/24389);
        p1[N] = (T)(X*white[0]);
        p2[N] = (T)(Y*white[1]);
        p3[N] = (T)(Z*white[2]);
      }
      return *this;
    }

    //! Convert pixel values from Lab to XYZ color spaces \newinstance.
    CImg<Tfloat> get_LabtoXYZ(const bool use_D65=true) const {
      return CImg<Tfloat>(*this,false).LabtoXYZ(use_D65);
    }

    //! Convert pixel values from XYZ to xyY color spaces.
    CImg<T>& XYZtoxyY() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "XYZtoxyY(): Instance is not a XYZ image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,4096))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          X = (Tfloat)p1[N],
          Y = (Tfloat)p2[N],
          Z = (Tfloat)p3[N],
          sum = X + Y + Z,
          nsum = sum>0?sum:1;
        p1[N] = (T)(X/nsum);
        p2[N] = (T)(Y/nsum);
        p3[N] = (T)Y;
      }
      return *this;
    }

    //! Convert pixel values from XYZ to xyY color spaces \newinstance.
    CImg<Tfloat> get_XYZtoxyY() const {
      return CImg<Tfloat>(*this,false).XYZtoxyY();
    }

    //! Convert pixel values from xyY pixels to XYZ color spaces.
    CImg<T>& xyYtoXYZ() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "xyYtoXYZ(): Instance is not a xyY image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,4096))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
         px = (Tfloat)p1[N],
         py = (Tfloat)p2[N],
         Y = (Tfloat)p3[N],
         ny = py>0?py:1;
        p1[N] = (T)(px*Y/ny);
        p2[N] = (T)Y;
        p3[N] = (T)((1 - px - py)*Y/ny);
      }
      return *this;
    }

    //! Convert pixel values from xyY pixels to XYZ color spaces \newinstance.
    CImg<Tfloat> get_xyYtoXYZ() const {
      return CImg<Tfloat>(*this,false).xyYtoXYZ();
    }

    //! Convert pixel values from RGB to Lab color spaces.
    CImg<T>& RGBtoLab(const bool use_D65=true) {
      return RGBtoXYZ(use_D65).XYZtoLab(use_D65);
    }

    //! Convert pixel values from RGB to Lab color spaces \newinstance.
    CImg<Tfloat> get_RGBtoLab(const bool use_D65=true) const {
      return CImg<Tfloat>(*this,false).RGBtoLab(use_D65);
    }

    //! Convert pixel values from Lab to RGB color spaces.
    CImg<T>& LabtoRGB(const bool use_D65=true) {
      return LabtoXYZ().XYZtoRGB(use_D65);
    }

    //! Convert pixel values from Lab to RGB color spaces \newinstance.
    CImg<Tuchar> get_LabtoRGB(const bool use_D65=true) const {
      return CImg<Tuchar>(*this,false).LabtoRGB(use_D65);
    }

    //! Convert pixel values from RGB to xyY color spaces.
    CImg<T>& RGBtoxyY(const bool use_D65=true) {
      return RGBtoXYZ(use_D65).XYZtoxyY();
    }

    //! Convert pixel values from RGB to xyY color spaces \newinstance.
    CImg<Tfloat> get_RGBtoxyY(const bool use_D65=true) const {
      return CImg<Tfloat>(*this,false).RGBtoxyY(use_D65);
    }

    //! Convert pixel values from xyY to RGB color spaces.
    CImg<T>& xyYtoRGB(const bool use_D65=true) {
      return xyYtoXYZ().XYZtoRGB(use_D65);
    }

    //! Convert pixel values from xyY to RGB color spaces \newinstance.
    CImg<Tuchar> get_xyYtoRGB(const bool use_D65=true) const {
      return CImg<Tuchar>(*this,false).xyYtoRGB(use_D65);
    }

    //! Convert pixel values from RGB to CMYK color spaces.
    CImg<T>& RGBtoCMYK() {
      return RGBtoCMY().CMYtoCMYK();
    }

    //! Convert pixel values from RGB to CMYK color spaces \newinstance.
    CImg<Tfloat> get_RGBtoCMYK() const {
      return CImg<Tfloat>(*this,false).RGBtoCMYK();
    }

    //! Convert pixel values from CMYK to RGB color spaces.
    CImg<T>& CMYKtoRGB() {
      return CMYKtoCMY().CMYtoRGB();
    }

    //! Convert pixel values from CMYK to RGB color spaces \newinstance.
    CImg<Tuchar> get_CMYKtoRGB() const {
      return CImg<Tuchar>(*this,false).CMYKtoRGB();
    }

    //@}
#endif
