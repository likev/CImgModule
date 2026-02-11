#ifndef CIMG_MODULE_IMAGE_BODY_DRAW_EXTRA_H
#define CIMG_MODULE_IMAGE_BODY_DRAW_EXTRA_H

      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1,bs0,bs1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2,bs0,bs2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2,bs1,bs2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1,
        dbs01 = bs1 - bs0, dbs02 = bs2 - bs0, dbs12 = bs2 - bs1;
      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02,
          bsm = y<y1?(bs0 + dbs01*yy0/dy01):(bs1 + dbs12*yy1/dy12),
          bsM = bs0 + dbs02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,txzm,txzM,tyzm,tyzM,izm,izM,bsm,bsM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float dizmM = izM - izm, dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm, dbsmM = bsM - bsm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              const float
                txz = txzm + dtxzmM*xxm/dxmM,
                tyz = tyzm + dtyzmM*xxm/dxmM,
                cbs = cimg::cut(bsm + dbsmM*xxm/dxmM,0.f,2.f);
              const int
                tx = (int)(txz/iz),
                ty = (int)(tyz/iz);
              const tc *const color = &texture._atXY(tx,ty);
              cimg_forC(*this,c) {
                const tc col = color[c*twhd];
                const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;
    }

    //! Draw a textured Phong-shaded 2D triangle.
    /**
       \param x0 X-coordinate of the first vertex in the image instance.
       \param y0 Y-coordinate of the first vertex in the image instance.
       \param x1 X-coordinate of the second vertex in the image instance.
       \param y1 Y-coordinate of the second vertex in the image instance.
       \param x2 X-coordinate of the third vertex in the image instance.
       \param y2 Y-coordinate of the third vertex in the image instance.
       \param texture Texture image used to fill the triangle.
       \param tx0 X-coordinate of the first vertex in the texture image.
       \param ty0 Y-coordinate of the first vertex in the texture image.
       \param tx1 X-coordinate of the second vertex in the texture image.
       \param ty1 Y-coordinate of the second vertex in the texture image.
       \param tx2 X-coordinate of the third vertex in the texture image.
       \param ty2 Y-coordinate of the third vertex in the texture image.
       \param light Light image.
       \param lx0 X-coordinate of the first vertex in the light image.
       \param ly0 Y-coordinate of the first vertex in the light image.
       \param lx1 X-coordinate of the second vertex in the light image.
       \param ly1 Y-coordinate of the second vertex in the light image.
       \param lx2 X-coordinate of the third vertex in the light image.
       \param ly2 Y-coordinate of the third vertex in the light image.
       \param opacity Drawing opacity.
    **/
    template<typename tc, typename tl>
    CImg<T>& draw_triangle(int x0, int y0,
                           int x1, int y1,
                           int x2, int y2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const CImg<tl>& light,
                           int lx0, int ly0,
                           int lx1, int ly1,
                           int lx2, int ly2,
                           const float opacity=1) {
      if (is_empty()) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,x1,y1,x2,y2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      if (is_overlapped(light))
        return draw_triangle(x0,y0,x1,y1,x2,y2,texture,tx0,ty0,tx1,ty1,tx2,ty2,+light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);

      if (y0>y1) cimg::swap(x0,x1,y0,y1,tx0,tx1,ty0,ty1,lx0,lx1,ly0,ly1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,tx0,tx2,ty0,ty2,lx0,lx2,ly0,ly2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,tx1,tx2,ty1,ty2,lx1,lx2,ly1,ly2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2,
        dtx01 = (longT)tx1 - tx0, dtx02 = (longT)tx2 - tx0, dtx12 = (longT)tx2 - tx1,
        dty01 = (longT)ty1 - ty0, dty02 = (longT)ty2 - ty0, dty12 = (longT)ty2 - ty1,
        dlx01 = (longT)lx1 - lx0, dlx02 = (longT)lx2 - lx0, dlx12 = (longT)lx2 - lx1,
        dly01 = (longT)ly1 - ly0, dly02 = (longT)ly2 - ly0, dly12 = (longT)ly2 - ly1;
      const ulongT
        twhd = (ulongT)texture._width*texture._height*texture._depth,
        lwhd = (ulongT)light._width*light._height*light._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02),
          txm = y<y1?tx0 + cimg_rd(dtx01*yy0,dy01):tx1 + cimg_rd(dtx12*yy1,dy12),
          txM = tx0 + cimg_rd(dtx02*yy0,dy02),
          tym = y<y1?ty0 + cimg_rd(dty01*yy0,dy01):ty1 + cimg_rd(dty12*yy1,dy12),
          tyM = ty0 + cimg_rd(dty02*yy0,dy02),
          lxm = y<y1?lx0 + cimg_rd(dlx01*yy0,dy01):lx1 + cimg_rd(dlx12*yy1,dy12),
          lxM = lx0 + cimg_rd(dlx02*yy0,dy02),
          lym = y<y1?ly0 + cimg_rd(dly01*yy0,dy01):ly1 + cimg_rd(dly12*yy1,dy12),
          lyM = ly0 + cimg_rd(dly02*yy0,dy02);
        if (xm>xM) cimg::swap(xm,xM,txm,txM,tym,tyM,lxm,lxM,lym,lyM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT
            dxmM = std::max((longT)1,xM - xm), hdxmM = dxmM/2,
            dtxmM = txM - txm, dtymM = tyM - tym,
            dlxmM = lxM - lxm, dlymM = lyM - lym;

          for (int x = cxm; x<=cxM; ++x) {
            const longT
              xxm = (longT)x - xm,
              tx = (txm*dxmM + dtxmM*xxm + hdxmM)/dxmM,
              ty = (tym*dxmM + dtymM*xxm + hdxmM)/dxmM,
              lx = (lxm*dxmM + dlxmM*xxm + hdxmM)/dxmM,
              ly = (lym*dxmM + dlymM*xxm + hdxmM)/dxmM;
            const tc *const color = &texture._atXY(tx,ty);
            const tl *const lig = &light._atXY(lx,ly);
            cimg_forC(*this,c) {
              const tc col = color[c*twhd];
              const float cbs = cimg::cut((float)lig[c*lwhd],0.f,2.f);
              const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a textured Phong-shaded 2D triangle, with perspective correction.
    template<typename tc, typename tl>
    CImg<T>& draw_triangle(int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const CImg<tl>& light,
                           int lx0, int ly0,
                           int lx1, int ly1,
                           int lx2, int ly2,
                           const float opacity=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,
                             light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      if (is_overlapped(light))
        return draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,texture,tx0,ty0,tx1,ty1,tx2,ty2,
                             +light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1,lx0,lx1,ly0,ly1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2,lx0,lx2,ly0,ly2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2,lx1,lx2,ly1,ly2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1,
        lxz0 = lx0*iz0, lxz1 = lx1*iz1, lxz2 = lx2*iz2,
        lyz0 = ly0*iz0, lyz1 = ly1*iz1, lyz2 = ly2*iz2,
        dlxz01 = lxz1 - lxz0, dlxz02 = lxz2 - lxz0, dlxz12 = lxz2 - lxz1,
        dlyz01 = lyz1 - lyz0, dlyz02 = lyz2 - lyz0, dlyz12 = lyz2 - lyz1;
      const ulongT
        twhd = (ulongT)texture._width*texture._height*texture._depth,
        lwhd = (ulongT)light._width*light._height*light._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02,
          lxzm = y<y1?(lxz0 + dlxz01*yy0/dy01):(lxz1 + dlxz12*yy1/dy12),
          lxzM = lxz0 + dlxz02*yy0/dy02,
          lyzm = y<y1?(lyz0 + dlyz01*yy0/dy01):(lyz1 + dlyz12*yy1/dy12),
          lyzM = lyz0 + dlyz02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,izm,izM,txzm,txzM,tyzm,tyzM,lxzm,lxzM,lyzm,lyzM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float
            dizmM = izM - izm,
            dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm,
            dlxzmM = lxzM - lxzm, dlyzmM = lyzM - lyzm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float
              iz = izm + dizmM*xxm/dxmM,
              txz = txzm + dtxzmM*xxm/dxmM,
              tyz = tyzm + dtyzmM*xxm/dxmM,
              lxz = lxzm + dlxzmM*xxm/dxmM,
              lyz = lyzm + dlyzmM*xxm/dxmM;
            const int
              tx = (int)(txz/iz),
              ty = (int)(tyz/iz),
              lx = (int)(lxz/iz),
              ly = (int)(lyz/iz);
            const tc *const color = &texture._atXY(tx,ty);
            const tl *const lig = &light._atXY(lx,ly);
            cimg_forC(*this,c) {
              const tc col = color[c*twhd];
              const float cbs = cimg::cut((float)lig[c*lwhd],0.f,2.f);
              const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;
    }

    //! Draw a textured Phong-shaded 2D triangle, with perspective correction and z-buffering.
    template<typename tz, typename tc, typename tl>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const CImg<tl>& light,
                           int lx0, int ly0,
                           int lx1, int ly1,
                           int lx2, int ly2,
                           const float opacity=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (is_overlapped(texture))
        return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,
                             +texture,tx0,ty0,tx1,ty1,tx2,ty2,light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      if (is_overlapped(light))
        return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,
                             texture,tx0,ty0,tx1,ty1,tx2,ty2,+light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1,lx0,lx1,ly0,ly1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2,lx0,lx2,ly0,ly2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2,lx1,lx2,ly1,ly2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int w1 = width() - 1, h1 = height() - 1, cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1);
      const longT
        dx01 = (longT)x1 - x0, dx02 = (longT)x2 - x0, dx12 = (longT)x2 - x1,
        dy01 = std::max((longT)1,(longT)y1 - y0),
        dy02 = std::max((longT)1,(longT)y2 - y0),
        dy12 = std::max((longT)1,(longT)y2 - y1),
        hdy01 = dy01/2, hdy02 = dy02/2, hdy12 = dy12/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1,
        lxz0 = lx0*iz0, lxz1 = lx1*iz1, lxz2 = lx2*iz2,
        lyz0 = ly0*iz0, lyz1 = ly1*iz1, lyz2 = ly2*iz2,
        dlxz01 = lxz1 - lxz0, dlxz02 = lxz2 - lxz0, dlxz12 = lxz2 - lxz1,
        dlyz01 = lyz1 - lyz0, dlyz02 = lyz2 - lyz0, dlyz12 = lyz2 - lyz1;
      const ulongT
        twhd = (ulongT)texture._width*texture._height*texture._depth,
        lwhd = (ulongT)light._width*light._height*light._depth;

      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const longT yy0 = (longT)y - y0, yy1 = (longT)y - y1;
        longT
          xm = y<y1?x0 + cimg_rd(dx01*yy0,dy01):x1 + cimg_rd(dx12*yy1,dy12),
          xM = x0 + cimg_rd(dx02*yy0,dy02);
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02,
          lxzm = y<y1?(lxz0 + dlxz01*yy0/dy01):(lxz1 + dlxz12*yy1/dy12),
          lxzM = lxz0 + dlxz02*yy0/dy02,
          lyzm = y<y1?(lyz0 + dlyz01*yy0/dy01):(lyz1 + dlyz12*yy1/dy12),
          lyzM = lyz0 + dlyz02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,izm,izM,txzm,txzM,tyzm,tyzM,lxzm,lxzM,lyzm,lyzM);
        if (xM>=0 && xm<=w1) {
          const int
            cxm = (int)cimg::cut(xm,(longT)0,(longT)w1),
            cxM = (int)cimg::cut(xM,(longT)0,(longT)w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const longT dxmM = std::max((longT)1,xM - xm);
          const float
            dizmM = izM - izm,
            dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm,
            dlxzmM = lxzM - lxzm, dlyzmM = lyzM - lyzm;

          for (int x = cxm; x<=cxM; ++x) {
            const longT xxm = (longT)x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              const float
                txz = txzm + dtxzmM*xxm/dxmM,
                tyz = tyzm + dtyzmM*xxm/dxmM,
                lxz = lxzm + dlxzmM*xxm/dxmM,
                lyz = lyzm + dlyzmM*xxm/dxmM;
              const int
                tx = (int)(txz/iz),
                ty = (int)(tyz/iz),
                lx = (int)(lxz/iz),
                ly = (int)(lyz/iz);
              const tc *const color = &texture._atXY(tx,ty);
              const tl *const lig = &light._atXY(lx,ly);
              cimg_forC(*this,c) {
                const tc col = color[c*twhd];
                const float cbs = cimg::cut((float)lig[c*lwhd],0.f,2.f);
                const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;
    }

    //! Draw a filled 4D rectangle.
    /**
       \param x0 X-coordinate of the upper-left rectangle corner.
       \param y0 Y-coordinate of the upper-left rectangle corner.
       \param z0 Z-coordinate of the upper-left rectangle corner.
       \param c0 C-coordinate of the upper-left rectangle corner.
       \param x1 X-coordinate of the lower-right rectangle corner.
       \param y1 Y-coordinate of the lower-right rectangle corner.
       \param z1 Z-coordinate of the lower-right rectangle corner.
       \param c1 C-coordinate of the lower-right rectangle corner.
       \param val Scalar value used to fill the rectangle area.
       \param opacity Drawing opacity.
    **/
    CImg<T>& draw_rectangle(const int x0, const int y0, const int z0, const int c0,
                            const int x1, const int y1, const int z1, const int c1,
                            const T val, const float opacity=1) {
      if (is_empty()) return *this;
      const int
        nx0 = x0<x1?x0:x1, nx1 = x0^x1^nx0,
        ny0 = y0<y1?y0:y1, ny1 = y0^y1^ny0,
        nz0 = z0<z1?z0:z1, nz1 = z0^z1^nz0,
        nc0 = c0<c1?c0:c1, nc1 = c0^c1^nc0;
      const int
        lx = (1 + nx1 - nx0) + (nx1>=width()?width() - 1 - nx1:0) + (nx0<0?nx0:0),
        ly = (1 + ny1 - ny0) + (ny1>=height()?height() - 1 - ny1:0) + (ny0<0?ny0:0),
        lz = (1 + nz1 - nz0) + (nz1>=depth()?depth() - 1 - nz1:0) + (nz0<0?nz0:0),
        lc = (1 + nc1 - nc0) + (nc1>=spectrum()?spectrum() - 1 - nc1:0) + (nc0<0?nc0:0);
      const ulongT
        offX = (ulongT)_width - lx,
        offY = (ulongT)_width*(_height - ly),
        offZ = (ulongT)_width*_height*(_depth - lz);
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.f);
      T *ptrd = data(nx0<0?0:nx0,ny0<0?0:ny0,nz0<0?0:nz0,nc0<0?0:nc0);
      if (lx>0 && ly>0 && lz>0 && lc>0)
        for (int v = 0; v<lc; ++v) {
          for (int z = 0; z<lz; ++z) {
            for (int y = 0; y<ly; ++y) {
              if (opacity>=1) {
                if (sizeof(T)!=1) { for (int x = 0; x<lx; ++x) *(ptrd++) = val; ptrd+=offX; }
                else { std::memset(ptrd,(int)val,lx); ptrd+=_width; }
              } else { for (int x = 0; x<lx; ++x) { *ptrd = (T)(nopacity*val + *ptrd*copacity); ++ptrd; } ptrd+=offX; }
            }
            ptrd+=offY;
          }
          ptrd+=offZ;
        }
      return *this;
    }

    //! Draw a filled 3D rectangle.
    /**
       \param x0 X-coordinate of the upper-left rectangle corner.
       \param y0 Y-coordinate of the upper-left rectangle corner.
       \param z0 Z-coordinate of the upper-left rectangle corner.
       \param x1 X-coordinate of the lower-right rectangle corner.
       \param y1 Y-coordinate of the lower-right rectangle corner.
       \param z1 Z-coordinate of the lower-right rectangle corner.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0, const int z0,
                            const int x1, const int y1, const int z1,
                            const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_rectangle(): Specified color is (null).",
                                    cimg_instance);
      cimg_forC(*this,c) draw_rectangle(x0,y0,z0,c,x1,y1,z1,c,(T)color[c],opacity);
      return *this;
    }

    //! Draw a filled 2D rectangle.
    /**
       \param x0 X-coordinate of the upper-left rectangle corner.
       \param y0 Y-coordinate of the upper-left rectangle corner.
       \param x1 X-coordinate of the lower-right rectangle corner.
       \param y1 Y-coordinate of the lower-right rectangle corner.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0,
                            const int x1, const int y1,
                            const tc *const color, const float opacity=1) {
      return draw_rectangle(x0,y0,0,x1,y1,_depth - 1,color,opacity);
    }

    //! Draw a outlined 2D rectangle \overloading.
    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0,
                            const int x1, const int y1,
                            const tc *const color, const float opacity,
                            const unsigned int pattern) {
      if (is_empty()) return *this;
      if (y0==y1) return draw_line(x0,y0,x1,y0,color,opacity,pattern,true);
      if (x0==x1) return draw_line(x0,y0,x0,y1,color,opacity,pattern,true);
      const int
        nx0 = x0<x1?x0:x1, nx1 = x0^x1^nx0,
        ny0 = y0<y1?y0:y1, ny1 = y0^y1^ny0;
      if (ny1==ny0 + 1) return draw_line(nx0,ny0,nx1,ny0,color,opacity,pattern,true).
                      draw_line(nx1,ny1,nx0,ny1,color,opacity,pattern,false);
      return draw_line(nx0,ny0,nx1,ny0,color,opacity,pattern,true).
        draw_line(nx1,ny0 + 1,nx1,ny1 - 1,color,opacity,pattern,false).
        draw_line(nx1,ny1,nx0,ny1,color,opacity,pattern,false).
        draw_line(nx0,ny1 - 1,nx0,ny0 + 1,color,opacity,pattern,false);
    }

    //! Draw a filled 2D polygon.
    /**
       \param points Set of polygon vertices.
       \param color Pointer to \c spectrum() consecutive values of type \c T, defining the drawing color.
       \param opacity Drawing opacity.
     **/
    template<typename tp, typename tc>
    CImg<T>& draw_polygon(const CImg<tp>& points,
                          const tc *const color, const float opacity=1) {
      if (is_empty() || !points) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Specified color is (null).",
                                    cimg_instance);
      if (points.height()!=2)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Invalid specified point set (%u,%u,%u,%u).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum);
      CImg<intT> ipoints;
      if (cimg::type<tp>::is_float()) ipoints = points.get_round();
      else ipoints.assign(points,cimg::type<tp>::string()==cimg::type<int>::string());

      if (ipoints._width==1) return draw_point(ipoints(0,0),ipoints(0,1),color,opacity);
      if (ipoints._width==2) return draw_line(ipoints(0,0),ipoints(0,1),ipoints(1,0),ipoints(1,1),color,opacity);
      if (ipoints._width==3) return draw_triangle(ipoints(0,0),ipoints(0,1),ipoints(1,0),ipoints(1,1),
                                                  ipoints(2,0),ipoints(2,1),color,opacity);
      cimg_init_scanline(opacity);
      int
        xmin = 0, ymin = 0,
        xmax = ipoints.get_shared_row(0).max_min(xmin),
        ymax = ipoints.get_shared_row(1).max_min(ymin);
      if (xmax<0 || xmin>=width() || ymax<0 || ymin>=height()) return *this;
      if (ymin==ymax) return draw_line(xmin,ymin,xmax,ymax,color,opacity);
      ymin = std::max(0,ymin);
      ymax = std::min(height() - 1,ymax);

      CImg<intT> Xs(ipoints._width,ymax - ymin + 1);
      CImg<uintT> count(Xs._height,1,1,1,0);
      unsigned int n = 0, nn = 1;
      bool go_on = true;

      while (go_on) {
        unsigned int an = (nn + 1)%ipoints._width;
        const int x0 = ipoints(n,0), y0 = ipoints(n,1);
        if (ipoints(nn,1)==y0) while (ipoints(an,1)==y0) { nn = an; (an+=1)%=ipoints._width; }
        const int x1 = ipoints(nn,0), y1 = ipoints(nn,1);
        unsigned int tn = an;
        while (ipoints(tn,1)==y1) (tn+=1)%=ipoints._width;
        if (y0!=y1) {
          const int
            y2 = ipoints(tn,1),
            x01 = x1 - x0, y01 = y1 - y0, y12 = y2 - y1,
            step = cimg::sign(y01),
            tmax = std::max(1,cimg::abs(y01)),
            htmax = tmax*cimg::sign(x01)/2 + 1,
            tend = tmax - (step==cimg::sign(y12));
          unsigned int y = (unsigned int)y0 - ymin;
          for (int t = 0; t<=tend; ++t, y+=step)
            if (y<Xs._height) Xs(count[y]++,y) = x0 + cimg_rd(t*x01,tmax);
        }
        go_on = nn>n;
        n = nn;
        nn = an;
      }

      cimg_pragma_openmp(parallel for cimg_openmp_if(Xs._height>=(cimg_openmp_sizefactor)*512))
      cimg_forY(Xs,y) if (count[y]) {
        const CImg<intT> Xsy = Xs.get_shared_points(0,count[y] - 1,y).sort();
        int px = width();
        for (unsigned int k = 0; k<Xsy._width; k+=2) {
          int x0 = Xsy[k];
          const int x1 = Xsy[k + 1];
          x0+=x0==px;
          cimg_draw_scanline(x0,x1,y + ymin,color,opacity,1);
          px = x1;
        }
      }
      return *this;
    }

    //! Draw a outlined 2D or 3D polygon \overloading.
    template<typename tp, typename tc>
    CImg<T>& draw_polygon(const CImg<tp>& points,
                          const tc *const color, const float opacity, const unsigned int pattern,
                          const bool is_closed=true) {
      if (is_empty() || !points) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Specified color is (null).",
                                    cimg_instance);
      if (points.height()!=2)
        throw CImgArgumentException(_cimg_instance
                                    "draw_polygon(): Invalid specified point set (%u,%u,%u,%u).",
                                    cimg_instance,
                                    points._width,points._height,points._depth,points._spectrum);
      CImg<intT> ipoints;
      if (cimg::type<tp>::is_float()) ipoints = points.get_round();
      else ipoints.assign(points,cimg::type<tp>::string()==cimg::type<int>::string());

      if (ipoints._width==1) return draw_point(ipoints(0,0),ipoints(0,1),color,opacity);
      if (ipoints._width==2) return draw_line(ipoints(0,0),ipoints(0,1),ipoints(1,0),ipoints(1,1),
                                              color,opacity,pattern);
      bool ninit_hatch = true, is_drawn = false;
      int x0 = ipoints(0,0), y0 = ipoints(0,1);
      const unsigned int N = ipoints._width - (is_closed?0:1);
      for (unsigned int i = 0; i<N; ++i) {
        const int
          ni = (i + 1)%ipoints.width(),
          x1 = ipoints(ni,0), y1 = ipoints(ni,1),
          x01 = x1 - x0, y01 = y1 - y0,
          l = std::max(cimg::abs(x01),cimg::abs(y01));
        if (l) {
          const bool draw_last_pixel = is_closed || i<N - 1?false:true;
          draw_line(x0,y0,x1,y1,color,opacity,pattern,ninit_hatch,draw_last_pixel);
          is_drawn = true;
        }
        ninit_hatch = false;
        x0 = x1; y0 = y1;
      }
      if (!is_drawn) draw_point(ipoints(0,0),ipoints(0,1),color,opacity); // All vertices were the same
      return *this;
    }

    //! Draw a filled 2D ellipse.
    /**
       \param x0 X-coordinate of the ellipse center.
       \param y0 Y-coordinate of the ellipse center.
       \param r1 First radius of the ellipse.
       \param r2 Second radius of the ellipse.
       \param angle Angle of the first radius.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const float r1, const float r2, const float angle,
                          const tc *const color, const float opacity=1) {
      return _draw_ellipse(x0,y0,r1,r2,angle,color,opacity,0U,true);
    }

    //! Draw a filled 2D ellipse \overloading.
    /**
       \param x0 X-coordinate of the ellipse center.
       \param y0 Y-coordinate of the ellipse center.
       \param tensor Diffusion tensor describing the ellipse.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
    **/
    template<typename t, typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const CImg<t> &tensor,
                          const tc *const color, const float opacity=1) {
      CImgList<t> eig = tensor.get_symmetric_eigen();
      const CImg<t> &val = eig[0], &vec = eig[1];
      return draw_ellipse(x0,y0,std::sqrt(val(0)),std::sqrt(val(1)),
                          std::atan2(vec(0,1),vec(0,0))*180/cimg::PI,
                          color,opacity);
    }

    //! Draw an outlined 2D ellipse.
    /**
       \param x0 X-coordinate of the ellipse center.
       \param y0 Y-coordinate of the ellipse center.
       \param r1 First radius of the ellipse.
       \param r2 Second radius of the ellipse.
       \param angle Angle of the first radius.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the outline pattern.
    **/
    template<typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const float r1, const float r2, const float angle,
                          const tc *const color, const float opacity, const unsigned int pattern) {
      if (pattern) _draw_ellipse(x0,y0,r1,r2,angle,color,opacity,pattern,false);
      return *this;
    }

    //! Draw an outlined 2D ellipse \overloading.
    /**
       \param x0 X-coordinate of the ellipse center.
       \param y0 Y-coordinate of the ellipse center.
       \param tensor Diffusion tensor describing the ellipse.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the outline pattern.
    **/
    template<typename t, typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const CImg<t> &tensor,
                          const tc *const color, const float opacity,
                          const unsigned int pattern) {
      CImgList<t> eig = tensor.get_symmetric_eigen();
      const CImg<t> &val = eig[0], &vec = eig[1];
      return draw_ellipse(x0,y0,std::sqrt(val(0)),std::sqrt(val(1)),
                          std::atan2(vec(0,1),vec(0,0))*180/cimg::PI,
                          color,opacity,pattern);
    }

    template<typename tc>
    CImg<T>& _draw_ellipse(const int x0, const int y0, const float radius1, const float radius2, const float angle,
                           const tc *const color, const float opacity,
                           const unsigned int pattern, const bool is_filled) {
      if (is_empty() || (!is_filled && !pattern)) return *this;
      const float radiusM = std::max(radius1,radius2);
      if (radius1<0 || radius2<0 || x0 - radiusM>=width() || y0 + radiusM<0 || y0 - radiusM>=height()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_ellipse(): Specified color is (null).",
                                    cimg_instance);
      const int iradius1 = (int)cimg::round(radius1), iradius2 = (int)cimg::round(radius2);
      if (!iradius1 && !iradius2) return draw_point(x0,y0,color,opacity);
      if (iradius1==iradius2) {
        if (is_filled) return draw_circle(x0,y0,iradius1,color,opacity);
        else if (pattern==~0U) return draw_circle(x0,y0,iradius1,color,opacity,pattern);
      }
      const float ang = (float)(angle*cimg::PI/180);

      if (!is_filled) { // Outlined
        const float ca = std::cos(ang), sa = std::sin(ang);
        CImg<int> points((unsigned int)cimg::round(6*radiusM),2);
        cimg_forX(points,k) {
          const float
            _ang = (float)(2*cimg::PI*k/points._width),
            X = (float)(radius1*std::cos(_ang)),
            Y = (float)(radius2*std::sin(_ang));
          points(k,0) = (int)cimg::round(x0 + (X*ca - Y*sa));
          points(k,1) = (int)cimg::round(y0 + (X*sa + Y*ca));
        }
        draw_polygon(points,color,opacity,pattern);
      } else { // Filled
        cimg_init_scanline(opacity);
        const float
          ca = std::cos(ang),
          sa = -std::sin(ang),
          ca2 = ca*ca,
          sa2 = sa*sa,
          casa = ca*sa,
          i1 = 1/cimg::sqr(radius1),
          i2 = 1/cimg::sqr(radius2),
          t1 = i1*ca2 + i2*sa2,
          t2 = (i2 - i1)*casa,
          t3 = i2*ca2 + i1*sa2,
          t12 = t1*2;
        const int
          _ymin = (int)std::floor(y0 - radiusM),
          _ymax = (int)std::ceil(y0 + radiusM),
          ymin = _ymin<0?0:_ymin,
          ymax = _ymax>=height()?height() - 1:_ymax;
        for (int y = ymin; y<=ymax; ++y) {
          const float
            Y = y - y0 + 0.5f,
            B = 2*t2*Y,
            C = t3*Y*Y - 1,
            D = B*B - 4*t1*C;
          if (D>=0) {
            const float sD = std::sqrt(D);
            const int
              xmin = (int)(x0 + cimg::round((-B - sD)/t12)),
              xmax = (int)(x0 + cimg::round((-B + sD)/t12));
            cimg_draw_scanline(xmin,xmax,y,color,opacity,1);
          }
        }
      }
      return *this;
    }

    //! Draw a filled 2D circle.
    /**
       \param x0 X-coordinate of the circle center.
       \param y0 Y-coordinate of the circle center.
       \param radius  Circle radius.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \note
       - Circle version of the Bresenham's algorithm is used.
    **/
    template<typename tc>
    CImg<T>& draw_circle(const int x0, const int y0, int radius,
                         const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (radius<0 || x0 - radius>=width() || y0 + radius<0 || y0 - radius>=height()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_circle(): Specified color is (null).",
                                    cimg_instance);
      if (!radius) return draw_point(x0,y0,color,opacity);
      cimg_init_scanline(opacity);
      if (y0>=0 && y0<height()) cimg_draw_scanline(x0 - radius,x0 + radius,y0,color,opacity,1);
      for (int f = 1 - radius, ddFx = 0, ddFy = -(radius<<1), x = 0, y = radius; x<y; ) {
        if (f>=0) {
          const int x1 = x0 - x, x2 = x0 + x, y1 = y0 - y, y2 = y0 + y;
          if (y1>=0 && y1<height()) cimg_draw_scanline(x1,x2,y1,color,opacity,1);
          if (y2>=0 && y2<height()) cimg_draw_scanline(x1,x2,y2,color,opacity,1);
          f+=(ddFy+=2); --y;
        }
        const bool no_diag = y!=(x++);
        ++(f+=(ddFx+=2));
        const int x1 = x0 - y, x2 = x0 + y, y1 = y0 - x, y2 = y0 + x;
        if (no_diag) {
          if (y1>=0 && y1<height()) cimg_draw_scanline(x1,x2,y1,color,opacity,1);
          if (y2>=0 && y2<height()) cimg_draw_scanline(x1,x2,y2,color,opacity,1);
        }
      }
      return *this;
    }

    //! Draw an outlined 2D circle.
    /**
       \param x0 X-coordinate of the circle center.
       \param y0 Y-coordinate of the circle center.
       \param radius Circle radius.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern An integer whose bits describe the outline pattern.
    **/
    template<typename tc>
    CImg<T>& draw_circle(const int x0, const int y0, int radius,
                         const tc *const color, const float opacity,
                         const unsigned int pattern) {
      if (pattern!=~0U) return draw_ellipse(x0,y0,radius,radius,0,color,opacity,pattern);
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_circle(): Specified color is (null).",
                                    cimg_instance);
      if (radius<0 || x0 - radius>=width() || y0 + radius<0 || y0 - radius>=height()) return *this;
      if (!radius) return draw_point(x0,y0,color,opacity);

      draw_point(x0 - radius,y0,color,opacity).draw_point(x0 + radius,y0,color,opacity).
        draw_point(x0,y0 - radius,color,opacity).draw_point(x0,y0 + radius,color,opacity);
      if (radius==1) return *this;
      for (int f = 1 - radius, ddFx = 0, ddFy = -(radius<<1), x = 0, y = radius; x<y; ) {
        if (f>=0) { f+=(ddFy+=2); --y; }
        ++x; ++(f+=(ddFx+=2));
        if (x!=y + 1) {
          const int x1 = x0 - y, x2 = x0 + y, y1 = y0 - x, y2 = y0 + x,
            x3 = x0 - x, x4 = x0 + x, y3 = y0 - y, y4 = y0 + y;
          draw_point(x1,y1,color,opacity).draw_point(x1,y2,color,opacity).
            draw_point(x2,y1,color,opacity).draw_point(x2,y2,color,opacity);
          if (x!=y)
            draw_point(x3,y3,color,opacity).draw_point(x4,y4,color,opacity).
              draw_point(x4,y3,color,opacity).draw_point(x3,y4,color,opacity);
        }
      }
      return *this;
    }

    //! Draw an image.
    /**
       \param sprite Sprite image.
       \param x0 X-coordinate of the sprite position.
       \param y0 Y-coordinate of the sprite position.
       \param z0 Z-coordinate of the sprite position.
       \param c0 C-coordinate of the sprite position.
       \param opacity Drawing opacity.
    **/
    template<typename t>
    CImg<T>& draw_image(const int x0, const int y0, const int z0, const int c0,
                        const CImg<t>& sprite, const float opacity=1) {
      if (is_empty() || !sprite) return *this;
      if (is_overlapped(sprite)) return draw_image(x0,y0,z0,c0,+sprite,opacity);
      if (x0==0 && y0==0 && z0==0 && c0==0 && is_sameXYZC(sprite) && opacity>=1 && !is_shared())
        return assign(sprite,false);
      const bool bx = x0<0, by = y0<0, bz = z0<0, bc = c0<0;
      const int
        dx0 = bx?0:x0, dy0 = by?0:y0, dz0 = bz?0:z0, dc0 = bc?0:c0,
        sx0 = dx0 - x0,  sy0 = dy0 - y0, sz0 = dz0 - z0, sc0 = dc0 - c0,
        lx = sprite.width() - sx0 - (x0 + sprite.width()>width()?x0 + sprite.width() - width():0),
        ly = sprite.height() - sy0 - (y0 + sprite.height()>height()?y0 + sprite.height() - height():0),
        lz = sprite.depth() - sz0 - (z0 + sprite.depth()>depth()?z0 + sprite.depth() - depth():0),
        lc = sprite.spectrum() - sc0 - (c0 + sprite.spectrum()>spectrum()?c0 + sprite.spectrum() - spectrum():0);

      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.f);
      if (lx>0 && ly>0 && lz>0 && lc>0) {
        for (int c = 0; c<lc; ++c)
          for (int z = 0; z<lz; ++z)
            for (int y = 0; y<ly; ++y) {
              T *ptrd = data(dx0,dy0 + y,dz0 + z,dc0 + c);
              const t *ptrs = sprite.data(sx0,sy0 + y,sz0 + z,sc0 + c);
              if (opacity>=1) for (int x = 0; x<lx; ++x) *(ptrd++) = (T)*(ptrs++);
              else for (int x = 0; x<lx; ++x) { *ptrd = (T)(nopacity*(*(ptrs++)) + *ptrd*copacity); ++ptrd; }
            }
      }
      return *this;
    }

    //! Draw an image \specialization.
    CImg<T>& draw_image(const int x0, const int y0, const int z0, const int c0,
                        const CImg<T>& sprite, const float opacity=1) {
      if (is_empty() || !sprite) return *this;
      if (is_overlapped(sprite)) return draw_image(x0,y0,z0,c0,+sprite,opacity);
      if (x0==0 && y0==0 && z0==0 && c0==0 && is_sameXYZC(sprite) && opacity>=1 && !is_shared())
        return assign(sprite,false);
      const bool bx = x0<0, by = y0<0, bz = z0<0, bc = c0<0;
      const int
        dx0 = bx?0:x0, dy0 = by?0:y0, dz0 = bz?0:z0, dc0 = bc?0:c0,
        sx0 = dx0 - x0,  sy0 = dy0 - y0, sz0 = dz0 - z0, sc0 = dc0 - c0,
        lx = sprite.width() - sx0 - (x0 + sprite.width()>width()?x0 + sprite.width() - width():0),
        ly = sprite.height() - sy0 - (y0 + sprite.height()>height()?y0 + sprite.height() - height():0),
        lz = sprite.depth() - sz0 - (z0 + sprite.depth()>depth()?z0 + sprite.depth() - depth():0),
        lc = sprite.spectrum() - sc0 - (c0 + sprite.spectrum()>spectrum()?c0 + sprite.spectrum() - spectrum():0);
      const ulongT slx = lx*sizeof(T);

      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.f);
      if (lx>0 && ly>0 && lz>0 && lc>0) {
        for (int c = 0; c<lc; ++c)
          for (int z = 0; z<lz; ++z)
            for (int y = 0; y<ly; ++y) {
              T *ptrd = data(dx0,dy0 + y,dz0 + z,dc0 + c);
              const T *ptrs = sprite.data(sx0,sy0 + y,sz0 + z,sc0 + c);
              if (opacity>=1) std::memcpy(ptrd,ptrs,slx);
              else for (int x = 0; x<lx; ++x) { *ptrd = (T)(nopacity*(*(ptrs++)) + *ptrd*copacity); ++ptrd; }
            }
      }
      return *this;
    }

    //! Draw an image \overloading.
    template<typename t>
    CImg<T>& draw_image(const int x0, const int y0, const int z0,
                        const CImg<t>& sprite, const float opacity=1) {
      return draw_image(x0,y0,z0,0,sprite,opacity);
    }

    //! Draw an image \overloading.
    template<typename t>
    CImg<T>& draw_image(const int x0, const int y0,
                        const CImg<t>& sprite, const float opacity=1) {
      return draw_image(x0,y0,0,sprite,opacity);
    }

    //! Draw an image \overloading.
    template<typename t>
    CImg<T>& draw_image(const int x0,
                        const CImg<t>& sprite, const float opacity=1) {
      return draw_image(x0,0,sprite,opacity);
    }

    //! Draw an image \overloading.
    template<typename t>
    CImg<T>& draw_image(const CImg<t>& sprite, const float opacity=1) {
      return draw_image(0,sprite,opacity);
    }

    //! Draw a masked image.
    /**
       \param sprite Sprite image.
       \param mask Mask image.
       \param x0 X-coordinate of the sprite position in the image instance.
       \param y0 Y-coordinate of the sprite position in the image instance.
       \param z0 Z-coordinate of the sprite position in the image instance.
       \param c0 C-coordinate of the sprite position in the image instance.
       \param mask_max_value Maximum pixel value of the mask image \c mask.
       \param opacity Drawing opacity.
       \note
       - Pixel values of \c mask set the opacity of the corresponding pixels in \c sprite.
       - Dimensions along x,y and z of \p sprite and \p mask must be the same.
    **/
    template<typename ti, typename tm>
    CImg<T>& draw_image(const int x0, const int y0, const int z0, const int c0,
                        const CImg<ti>& sprite, const CImg<tm>& mask, const float opacity=1,
                        const float mask_max_value=1) {
      if (is_empty() || !sprite || !mask) return *this;
      if (is_overlapped(sprite)) return draw_image(x0,y0,z0,c0,+sprite,mask,opacity,mask_max_value);
      if (is_overlapped(mask)) return draw_image(x0,y0,z0,c0,sprite,+mask,opacity,mask_max_value);
      if (mask._width!=sprite._width || mask._height!=sprite._height || mask._depth!=sprite._depth)
        throw CImgArgumentException(_cimg_instance
                                    "draw_image(): Sprite (%u,%u,%u,%u,%p) and mask (%u,%u,%u,%u,%p) have "
                                    "incompatible dimensions.",
                                    cimg_instance,
                                    sprite._width,sprite._height,sprite._depth,sprite._spectrum,sprite._data,
                                    mask._width,mask._height,mask._depth,mask._spectrum,mask._data);

      const bool bx = x0<0, by = y0<0, bz = z0<0, bc = c0<0;
      const int
        dx0 = bx?0:x0, dy0 = by?0:y0, dz0 = bz?0:z0, dc0 = bc?0:c0,
        sx0 = dx0 - x0,  sy0 = dy0 - y0, sz0 = dz0 - z0, sc0 = dc0 - c0,
        lx = sprite.width() - sx0 - (x0 + sprite.width()>width()?x0 + sprite.width() - width():0),
        ly = sprite.height() - sy0 - (y0 + sprite.height()>height()?y0 + sprite.height() - height():0),
        lz = sprite.depth() - sz0 - (z0 + sprite.depth()>depth()?z0 + sprite.depth() - depth():0),
        lc = sprite.spectrum() - sc0 - (c0 + sprite.spectrum()>spectrum()?c0 + sprite.spectrum() - spectrum():0);
      const ulongT msize = mask.size();

      if (lx>0 && ly>0 && lz>0 && lc>0) {
        for (int c = 0; c<lc; ++c)
          for (int z = 0; z<lz; ++z)
            for (int y = 0; y<ly; ++y) {
              T *ptrd = data(dx0,dy0 + y,dz0 + z,dc0 + c);
              const ti *ptrs = sprite.data(sx0,sy0 + y,sz0 + z,sc0 + c);
              const tm *ptrm = mask._data + (mask.offset(sx0,sy0 + y,sz0 + z,sc0 + c)%msize);
              for (int x = 0; x<lx; ++x) {
                const float mopacity = (float)(*(ptrm++)*opacity),
                  nopacity = cimg::abs(mopacity), copacity = mask_max_value - std::max(mopacity,0.f);
                *ptrd = (T)((nopacity*(*(ptrs++)) + *ptrd*copacity)/mask_max_value);
                ++ptrd;
              }
            }
      }
      return *this;
    }

    //! Draw a masked image \overloading.
    template<typename ti, typename tm>
    CImg<T>& draw_image(const int x0, const int y0, const int z0,
                        const CImg<ti>& sprite, const CImg<tm>& mask, const float opacity=1,
                        const float mask_max_value=1) {
      return draw_image(x0,y0,z0,0,sprite,mask,opacity,mask_max_value);
    }

    //! Draw a image \overloading.
    template<typename ti, typename tm>
    CImg<T>& draw_image(const int x0, const int y0,
                        const CImg<ti>& sprite, const CImg<tm>& mask, const float opacity=1,
                        const float mask_max_value=1) {
      return draw_image(x0,y0,0,sprite,mask,opacity,mask_max_value);
    }

    //! Draw a image \overloading.
    template<typename ti, typename tm>
    CImg<T>& draw_image(const int x0,
                        const CImg<ti>& sprite, const CImg<tm>& mask, const float opacity=1,
                        const float mask_max_value=1) {
      return draw_image(x0,0,sprite,mask,opacity,mask_max_value);
    }

    //! Draw an image.
    template<typename ti, typename tm>
    CImg<T>& draw_image(const CImg<ti>& sprite, const CImg<tm>& mask, const float opacity=1,
                        const float mask_max_value=1) {
      return draw_image(0,sprite,mask,opacity,mask_max_value);
    }

    //! Draw a text string.
    /**
       \param x0 X-coordinate of the text in the image instance.
       \param y0 Y-coordinate of the text in the image instance.
       \param text Format of the text ('printf'-style format string).
       \param foreground_color Pointer to \c spectrum() consecutive values, defining the foreground drawing color.
       \param background_color Pointer to \c spectrum() consecutive values, defining the background drawing color.
       \param opacity Drawing opacity.
       \param font Font used for drawing text.
    **/
    template<typename tc1, typename tc2, typename t>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const tc1 *const foreground_color, const tc2 *const background_color,
                       const float opacity, const CImgList<t>* const font, ...) {
      if (!font || !*font) return *this;
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font); cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      return _draw_text(x0,y0,tmp,foreground_color,background_color,opacity,*font,false);
    }

    //! Draw a text string \overloading.
    /**
       \note A transparent background is used for the text.
    **/
    template<typename tc, typename t>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const tc *const foreground_color, const int,
                       const float opacity, const CImgList<t>* const font, ...) {
      if (!font || !*font) return *this;
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font); cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      return _draw_text(x0,y0,tmp,foreground_color,(tc*)0,opacity,*font,false);
    }

    //! Draw a text string \overloading.
    /**
       \note A transparent foreground is used for the text.
    **/
    template<typename tc, typename t>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const int, const tc *const background_color,
                       const float opacity, const CImgList<t>* const font, ...) {
      if (!font || !*font) return *this;
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font); cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      return _draw_text(x0,y0,tmp,(tc*)0,background_color,opacity,*font,false);
    }

    //! Draw a text string \overloading.
    /**
       \param x0 X-coordinate of the text in the image instance.
       \param y0 Y-coordinate of the text in the image instance.
       \param text Format of the text ('printf'-style format string).
       \param foreground_color Array of spectrum() values of type \c T,
         defining the foreground color (0 means 'transparent').
       \param background_color Array of spectrum() values of type \c T,
         defining the background color (0 means 'transparent').
       \param opacity Drawing opacity.
       \param font_height Height of the text font (exact match for 13,32,64,128, interpolated otherwise).
    **/
    template<typename tc1, typename tc2>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const tc1 *const foreground_color, const tc2 *const background_color,
                       const float opacity=1, const unsigned int font_height=13, ...) {
      if (!font_height) return *this;
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font_height); cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      const CImgList<ucharT>& font = CImgList<ucharT>::font(font_height,true);
      _draw_text(x0,y0,tmp,foreground_color,background_color,opacity,font,true);
      return *this;
    }

    //! Draw a text string \overloading.
    template<typename tc>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const tc *const foreground_color, const int background_color=0,
                       const float opacity=1, const unsigned int font_height=13, ...) {
      if (!font_height) return *this;
      cimg::unused(background_color);
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font_height); cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      return draw_text(x0,y0,"%s",foreground_color,(const tc*)0,opacity,font_height,tmp._data);
    }

    //! Draw a text string \overloading.
    template<typename tc>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const int, const tc *const background_color,
                       const float opacity=1, const unsigned int font_height=13, ...) {
      if (!font_height) return *this;
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font_height); cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      return draw_text(x0,y0,"%s",(tc*)0,background_color,opacity,font_height,tmp._data);
    }

    template<typename tc1, typename tc2, typename t>
    CImg<T>& _draw_text(const int x0, const int y0,
                        const char *const text,
                        const tc1 *const foreground_color, const tc2 *const background_color,
                        const float opacity, const CImgList<t>& font,
                        const bool is_native_font) {
      if (!text || !font) return *this;
      const unsigned int text_length = (unsigned int)std::strlen(text);
      const int padding_x = font[0]._height<48?1:font[0]._height<128?(int)std::ceil(font[0]._height/51.f + 0.745f):4;
      unsigned char o_ch, ch = 0;
      int x, y, w;
      CImg<intT> left_paddings(text_length,1,1,1,0);
      const CImg<t> empty = CImg<t>::empty();

      if (is_empty() || is_native_font) {
        // Pre-compute necessary size of the image as well as left paddings of each character.
        x = y = w = 0;
        o_ch = 0;
        for (unsigned int i = 0; i<text_length; ++i) {
          ch = (unsigned char)text[i];
          switch (ch) {
          case '\n' :
            if (font._width>10) y+=font[10]._height; else y+=font[0]._height;
            if (x>w) w = x;
            x = 0;
            break;
          case '\t' :
            if (font._width>32) x+=4*font[32]._width; else x+=4*font[0]._width;
            break;
          case ' ' :
            if (font._width>32) x+=font[32]._width; else x+=font[0]._width;
            break;
          default : if (ch<font._width) {
              int left_padding = 0;
              if (is_native_font && font[0]._height<128) { /// Determine left padding for native fonts
                if (ch==':' || ch=='!' || ch=='.' || ch==';')
                  left_padding = 2*padding_x;
                else if (o_ch==',' || (o_ch=='.' && (ch<'0' || ch>'9')) || o_ch==';' || o_ch==':' || o_ch=='!')
                  left_padding = 4*padding_x;
                else if (((o_ch=='i' || o_ch=='l' || o_ch=='I' || o_ch=='J' || o_ch=='M' || o_ch=='N') &&
                          ((ch>='0' && ch<='9') ||
                           (ch>='a' && ch<='z' && ch!='v' && ch!='x' && ch!='y') ||
                           (ch>='B' && ch<='Z' && ch!='J' && ch!='T' && ch!='V' && ch!='X' && ch!='Y'))) ||
                         o_ch=='.' || o_ch=='\'' || ch=='\'')
                  left_padding = padding_x;
                else if ((o_ch<'0' || o_ch>'9') && ch!='-') {
                  const CImg<t> &mask = ch + 256U<font._width?font[ch + 256]:empty;
                  if (o_ch && ch>' ' && o_ch>' ' && mask._height>13) {
                    const CImg<t> &o_mask = o_ch + 256U<font._width?font[o_ch + 256]:empty;
                    if (o_mask._height>13) {
                      const int w1 = mask.width()>0?o_mask.width() - 1:0, w2 = w1>1?w1 - 1:0, w3 = w2>1?w2 - 1:0;
                      left_padding = -10;
                      cimg_forY(mask,k) {
                        const int
                          lpad = o_mask(w1,k)>=8?0:
                                 o_mask._width<=2 || o_mask(w2,k)>=8?-1:
                                 o_mask._width<=3 || o_mask(w3,k)>=8?-2:-3,
                          rpad = mask(0,k)>=8?0:
                                 mask._width<=2 || mask(1,k)>=8?-1:
                                 mask._width<=3 || mask(2,k)>=8?-2:-3;
                        left_padding = std::max(left_padding,lpad + rpad);
                      }
                    }
                  }
                }
                left_paddings[i] = left_padding;
              }
              x+=left_padding + font[ch]._width + padding_x;
              o_ch = ch;
            }
          }
        }
        if (x!=0 || ch=='\n') { if (x>w) w = x; y+=font[0]._height; }
        if (is_empty()) assign(x0 + w,y0 + y,1,is_native_font?1:font[0]._spectrum,(T)0);
      }

      // Draw font characters on image.
      x = x0; y = y0;
      for (unsigned int i = 0; i<text_length; ++i) {
        ch = (unsigned char)text[i];
        switch (ch) {
        case '\n' :
          if (font._width>10) y+=font[10]._height; else y+=font[0]._height;
          x = x0;
          break;
        case '\t' :
        case ' ' : {
          const unsigned int
            lw = (ch=='\t'?4:1)*font[font._width>32?32:0]._width,
            lh = font[font._width>32?32:0]._height;
          if (background_color) draw_rectangle(x,y,x + lw - 1,y + lh - 1,background_color,opacity);
          x+=lw;
        } break;
        default : if (ch<font._width) {
            if (font[ch]) {
              CImg<T> letter = font[ch];
              const CImg<t> &mask = ch + 256U<font._width?font[ch + 256]:empty;
              const int posx = x + left_paddings[i] + padding_x;

              for (unsigned int c = 0; c<_spectrum; c+=letter._spectrum) {
                if (c) letter = font[ch];
                const unsigned int cmin = std::min(_spectrum - c,letter._spectrum);
                for (unsigned int d = 0; d<cmin; ++d)
                  if (foreground_color[c + d]!=255)
                    letter.get_shared_channel(d)*=foreground_color[c + d]/255.f;

                if (mask) { // Letter with alpha
                  if (background_color)
                    for (unsigned int d = 0; d<cmin; ++d)
                      draw_rectangle(x,y,0,c + d,posx + letter._width - 1,y + letter._height - 1,0,c + d,
                                     background_color[c + d],opacity);
                  draw_image(posx,y,0,c,letter,mask,opacity,255.f);
                } else // Letter without alpha
                  draw_image(posx,y,0,c,letter,opacity);
              }
              x = posx + letter._width;
            }
          }
        }
      }
      return *this;
    }

    // [internal] Version used to display text in interactive viewers.
    CImg<T>& __draw_text(const char *const text, unsigned int &font_size, const int is_down, ...) {
      CImg<charT> tmp(2048);
      std::va_list ap;
      va_start(ap,is_down);
      cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      CImg<ucharT> a_label, a_labelmask;
      const unsigned char a_labelcolor = 255;
      unsigned int ofs = font_size, fs = ofs;
      do { // Determine best font size
        a_label.assign().draw_text(0,0,"%s",&a_labelcolor,0,1,fs,tmp._data);
        if (a_label._width<7*_width/10 && a_label._height>_height/20 && a_label._height<_height/5) {
          font_size = fs; break;
        } else if ((a_label._width>7*_width/10 || a_label._height>_height/5) && fs>13 && ofs>=fs) {
          ofs = fs; fs = std::max(13U,(unsigned int)cimg::round(fs/1.25f));
        } else if (a_label._width<3*_width/10 && a_label._height<_height/20 && fs<64 && ofs<=fs) {
          ofs = fs; fs = std::min(64U,(unsigned int)cimg::round(fs*1.25f));
        } else { font_size = fs; break; }
      } while (true);
      a_label.normalize(0,255);
      a_label+=(255 - a_label.get_dilate(3)).normalize(0,80);
      a_label.resize(-100,-100,1,3,1);
      return draw_image(0,is_down?height() - a_label.height():0,a_label,0.85f);
    }

    //! Draw a 2D vector field.
    /**
       \param flow Image of 2D vectors used as input data.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param sampling Length (in pixels) between each arrow.
       \param factor Length factor of each arrow (if <0, computed as a percentage of the maximum length).
       \param is_arrow Tells if arrows must be drawn, instead of oriented segments.
       \param pattern Used pattern to draw lines.
       \note Clipping is supported.
    **/
    template<typename t1, typename t2>
    CImg<T>& draw_quiver(const CImg<t1>& flow,
                         const t2 *const color, const float opacity=1,
                         const unsigned int sampling=25, const float factor=-20,
                         const bool is_arrow=true, const unsigned int pattern=~0U) {
      return draw_quiver(flow,CImg<t2>(color,_spectrum,1,1,1,true),opacity,sampling,factor,is_arrow,pattern);
    }

    //! Draw a 2D vector field, using a field of colors.
    /**
       \param flow Image of 2D vectors used as input data.
       \param color Image of spectrum()-D vectors corresponding to the color of each arrow.
       \param opacity Opacity of the drawing.
       \param sampling Length (in pixels) between each arrow.
       \param factor Length factor of each arrow (if <0, computed as a percentage of the maximum length).
       \param is_arrow Tells if arrows must be drawn, instead of oriented segments.
       \param pattern Used pattern to draw lines.
       \note Clipping is supported.
    **/
    template<typename t1, typename t2>
    CImg<T>& draw_quiver(const CImg<t1>& flow,
                         const CImg<t2>& color, const float opacity=1,
                         const unsigned int sampling=25, const float factor=-20,
                         const bool is_arrow=true, const unsigned int pattern=~0U) {
      if (is_empty()) return *this;
      if (!flow || flow._spectrum!=2)
        throw CImgArgumentException(_cimg_instance
                                    "draw_quiver(): Invalid dimensions of specified flow (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    flow._width,flow._height,flow._depth,flow._spectrum,flow._data);
      if (sampling<=0)
        throw CImgArgumentException(_cimg_instance
                                    "draw_quiver(): Invalid sampling value %g "
                                    "(should be >0)",
                                    cimg_instance,
                                    sampling);
      const bool colorfield = (color._width==flow._width && color._height==flow._height &&
                               color._depth==1 && color._spectrum==_spectrum);
      if (is_overlapped(flow)) return draw_quiver(+flow,color,opacity,sampling,factor,is_arrow,pattern);
      float vmax,fact;
      if (factor<=0) {
        float m, M = (float)flow.get_norm(2).max_min(m);
        vmax = (float)std::max(cimg::abs(m),cimg::abs(M));
        if (!vmax) vmax = 1;
        fact = -factor;
      } else { fact = factor; vmax = 1; }

      for (unsigned int y = sampling/2; y<_height; y+=sampling)
        for (unsigned int x = sampling/2; x<_width; x+=sampling) {
          const unsigned int X = x*flow._width/_width, Y = y*flow._height/_height;
          float u = (float)flow(X,Y,0,0)*fact/vmax, v = (float)flow(X,Y,0,1)*fact/vmax;
          if (is_arrow) {
            const int xx = (int)(x + u), yy = (int)(y + v);
            if (colorfield) draw_arrow(x,y,xx,yy,color.get_vector_at(X,Y)._data,opacity,45,sampling/5.f,pattern);
            else draw_arrow(x,y,xx,yy,color._data,opacity,45,sampling/5.f,pattern);
          } else {
            if (colorfield)
              draw_line((int)(x - 0.5*u),(int)(y - 0.5*v),(int)(x + 0.5*u),(int)(y + 0.5*v),
                        color.get_vector_at(X,Y)._data,opacity,pattern);
            else draw_line((int)(x - 0.5*u),(int)(y - 0.5*v),(int)(x + 0.5*u),(int)(y + 0.5*v),
                           color._data,opacity,pattern);
          }
        }
      return *this;
    }

    //! Draw a labeled horizontal axis.
    /**
       \param values_x Values along the horizontal axis.
       \param y Y-coordinate of the horizontal axis in the image instance.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern Drawing pattern.
       \param font_height Height of the labels (exact match for 13,23,53,103, interpolated otherwise).
       \param allow_zero Enable/disable the drawing of label '0' if found.
    **/
    template<typename t, typename tc>
    CImg<T>& draw_axis(const CImg<t>& values_x, const int y,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const unsigned int font_height=13,
                       const bool allow_zero=true, const float round_x=0) {
      if (is_empty()) return *this;
      const int yt = (y + 3 + font_height)<_height?y + 3:y - 2 - (int)font_height;
      const int siz = (int)values_x.size() - 1;
      CImg<charT> txt(32);
      CImg<T> a_label;
      if (siz<=0) { // Degenerated case
        draw_line(0,y,_width - 1,y,color,opacity,pattern);
        if (!siz) {
          cimg_snprintf(txt,txt._width,"%g",round_x?cimg::round((double)*values_x,round_x):(double)*values_x);
          a_label.assign().draw_text(0,0,txt,color,(tc*)0,opacity,font_height);
          const int
            _xt = (width() - a_label.width())/2,
            xt = _xt<3?3:_xt + a_label.width()>=width() - 2?width() - 3 - a_label.width():_xt;
          draw_point(width()/2,y - 1,color,opacity).draw_point(width()/2,y + 1,color,opacity);
          if (allow_zero || *txt!='0' || txt[1]!=0)
            draw_text(xt,yt,txt,color,(tc*)0,opacity,font_height);
        }
      } else { // Regular case
        if (values_x[0]<values_x[siz]) draw_arrow(0,y,_width - 1,y,color,opacity,30,5,pattern);
        else draw_arrow(_width - 1,y,0,y,color,opacity,30,5,pattern);
        cimg_foroff(values_x,x) {
          cimg_snprintf(txt,txt._width,"%g",round_x?cimg::round((double)values_x(x),round_x):(double)values_x(x));
          a_label.assign().draw_text(0,0,txt,color,(tc*)0,opacity,font_height);
          const int
            xi = (int)(x*(_width - 1)/siz),
            _xt = xi - a_label.width()/2,
            xt = _xt<3?3:_xt + a_label.width()>=width() - 2?width() - 3 - a_label.width():_xt;
          draw_point(xi,y - 1,color,opacity).draw_point(xi,y + 1,color,opacity);
          if (allow_zero || *txt!='0' || txt[1]!=0)
            draw_text(xt,yt,txt,color,(tc*)0,opacity,font_height);
        }
      }
      return *this;
    }

    //! Draw a labeled vertical axis.
    /**
       \param x X-coordinate of the vertical axis in the image instance.
       \param values_y Values along the Y-axis.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern Drawing pattern.
       \param font_height Height of the labels (exact match for 13,23,53,103, interpolated otherwise).
       \param allow_zero Enable/disable the drawing of label '0' if found.
    **/
    template<typename t, typename tc>
    CImg<T>& draw_axis(const int x, const CImg<t>& values_y,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const unsigned int font_height=13,
                       const bool allow_zero=true, const float round_y=0) {
      if (is_empty()) return *this;
      int siz = (int)values_y.size() - 1;
      CImg<charT> txt(32);
      CImg<T> a_label;
      if (siz<=0) { // Degenerated case
        draw_line(x,0,x,_height - 1,color,opacity,pattern);
        if (!siz) {
          cimg_snprintf(txt,txt._width,"%g",round_y?cimg::round((double)*values_y,round_y):(double)*values_y);
          a_label.assign().draw_text(0,0,txt,color,(tc*)0,opacity,font_height);
          const int
            _yt = (height() - a_label.height())/2,
            yt = _yt<0?0:_yt + a_label.height()>=height()?height() - 1 - a_label.height():_yt,
            _xt = x - 2 - a_label.width(),
            xt = _xt>=0?_xt:x + 3;
          draw_point(x - 1,height()/2,color,opacity).draw_point(x + 1,height()/2,color,opacity);
          if (allow_zero || *txt!='0' || txt[1]!=0)
            draw_text(xt,yt,txt,color,(tc*)0,opacity,font_height);
        }
      } else { // Regular case
        if (values_y[0]<values_y[siz]) draw_arrow(x,0,x,_height - 1,color,opacity,30,5,pattern);
        else draw_arrow(x,_height - 1,x,0,color,opacity,30,5,pattern);
        cimg_foroff(values_y,y) {
          cimg_snprintf(txt,txt._width,"%g",round_y?cimg::round((double)values_y(y),round_y):(double)values_y(y));
          a_label.assign().draw_text(0,0,txt,color,(tc*)0,opacity,font_height);
          const int
            yi = (int)(y*(_height - 1)/siz),
            _yt = yi - a_label.height()/2,
            yt = _yt<0?0:_yt + a_label.height()>=height()?height() - 1 - a_label.height():_yt,
            _xt = x - 2 - a_label.width(),
            xt = _xt>=0?_xt:x + 3;
          draw_point(x - 1,yi,color,opacity).draw_point(x + 1,yi,color,opacity);
          if (allow_zero || *txt!='0' || txt[1]!=0)
            draw_text(xt,yt,txt,color,(tc*)0,opacity,font_height);
        }
      }
      return *this;
    }

    //! Draw labeled horizontal and vertical axes.
    /**
       \param values_x Values along the X-axis.
       \param values_y Values along the Y-axis.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern_x Drawing pattern for the X-axis.
       \param pattern_y Drawing pattern for the Y-axis.
       \param font_height Height of the labels (exact match for 13,23,53,103, interpolated otherwise).
       \param allow_zero Enable/disable the drawing of label '0' if found.
    **/
    template<typename tx, typename ty, typename tc>
    CImg<T>& draw_axes(const CImg<tx>& values_x, const CImg<ty>& values_y,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern_x=~0U, const unsigned int pattern_y=~0U,
                       const unsigned int font_height=13, const bool allow_zero=true,
                       const float round_x=0, const float round_y=0) {
      if (is_empty()) return *this;
      const CImg<tx> nvalues_x(values_x._data,values_x.size(),1,1,1,true);
      const int sizx = (int)values_x.size() - 1, wm1 = width() - 1;
      if (sizx>=0) {
        float ox = (float)*nvalues_x;
        for (unsigned int x = sizx?1U:0U; x<_width; ++x) {
          const float nx = (float)nvalues_x._linear_atX((float)x*sizx/wm1);
          if (nx*ox<=0) {
            draw_axis(nx==0?x:x - 1,values_y,color,opacity,pattern_y,font_height,allow_zero,round_y);
            break;
          }
          ox = nx;
        }
      }
      const CImg<ty> nvalues_y(values_y._data,values_y.size(),1,1,1,true);
      const int sizy = (int)values_y.size() - 1, hm1 = height() - 1;
      if (sizy>0) {
        float oy = (float)nvalues_y[0];
        for (unsigned int y = sizy?1U:0U; y<_height; ++y) {
          const float ny = (float)nvalues_y._linear_atX((float)y*sizy/hm1);
          if (ny*oy<=0) {
            draw_axis(values_x,ny==0?y:y - 1,color,opacity,pattern_x,font_height,allow_zero,round_x);
            break;
          }
          oy = ny;
        }
      }
      return *this;
    }

    //! Draw labeled horizontal and vertical axes \overloading.
    template<typename tc>
    CImg<T>& draw_axes(const float x0, const float x1, const float y0, const float y1,
                       const tc *const color, const float opacity=1,
                       const int subdivisionx=-60, const int subdivisiony=-60,
                       const float precisionx=0, const float precisiony=0,
                       const unsigned int pattern_x=~0U, const unsigned int pattern_y=~0U,
                       const unsigned int font_height=13) {
      if (is_empty()) return *this;
      const bool allow_zero = (x0*x1>0) || (y0*y1>0);
      const float
        dx = cimg::abs(x1 - x0), dy = cimg::abs(y1 - y0),
        px = dx<=0?1:precisionx==0?(float)std::pow(10.,(int)std::log10(dx) - 2.):precisionx,
        py = dy<=0?1:precisiony==0?(float)std::pow(10.,(int)std::log10(dy) - 2.):precisiony;
      if (x0!=x1 && y0!=y1)
        draw_axes(CImg<floatT>::sequence(subdivisionx>0?subdivisionx:1-width()/subdivisionx,x0,x1),
                  CImg<floatT>::sequence(subdivisiony>0?subdivisiony:1-height()/subdivisiony,y0,y1),
                  color,opacity,pattern_x,pattern_y,font_height,allow_zero,px,py);
      else if (x0==x1 && y0!=y1)
        draw_axis((int)x0,CImg<floatT>::sequence(subdivisiony>0?subdivisiony:1-height()/subdivisiony,y0,y1),
                  color,opacity,pattern_y,font_height,py);
      else if (x0!=x1 && y0==y1)
        draw_axis(CImg<floatT>::sequence(subdivisionx>0?subdivisionx:1-width()/subdivisionx,x0,x1),(int)y0,
                  color,opacity,pattern_x,font_height,px);
      return *this;
    }

    //! Draw 2D grid.
    /**
       \param values_x X-coordinates of the vertical lines.
       \param values_y Y-coordinates of the horizontal lines.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.
       \param pattern_x Drawing pattern for vertical lines.
       \param pattern_y Drawing pattern for horizontal lines.
    **/
    template<typename tx, typename ty, typename tc>
    CImg<T>& draw_grid(const CImg<tx>& values_x, const CImg<ty>& values_y,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern_x=~0U, const unsigned int pattern_y=~0U) {
      if (is_empty()) return *this;
      if (values_x) cimg_foroff(values_x,x) {
          const int xi = (int)values_x[x];
          if (xi>=0 && xi<width()) draw_line(xi,0,xi,_height - 1,color,opacity,pattern_x);
        }
      if (values_y) cimg_foroff(values_y,y) {
          const int yi = (int)values_y[y];
          if (yi>=0 && yi<height()) draw_line(0,yi,_width - 1,yi,color,opacity,pattern_y);
        }
      return *this;
    }

    //! Draw 2D grid \simplification.
    template<typename tc>
    CImg<T>& draw_grid(const float delta_x,  const float delta_y,
                       const float offsetx, const float offsety,
                       const bool invertx, const bool inverty,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern_x=~0U, const unsigned int pattern_y=~0U) {
      if (is_empty()) return *this;
      CImg<uintT> seqx, seqy;
      if (delta_x!=0) {
        const float dx = delta_x>0?delta_x:_width*-delta_x/100;
        const unsigned int nx = (unsigned int)(_width/dx);
        seqx = CImg<uintT>::sequence(1 + nx,0,(unsigned int)(dx*nx));
        if (offsetx) cimg_foroff(seqx,x) seqx(x) = (unsigned int)cimg::mod(seqx(x) + offsetx,(float)_width);
        if (invertx) cimg_foroff(seqx,x) seqx(x) = _width - 1 - seqx(x);
      }
      if (delta_y!=0) {
        const float dy = delta_y>0?delta_y:_height*-delta_y/100;
        const unsigned int ny = (unsigned int)(_height/dy);
        seqy = CImg<uintT>::sequence(1 + ny,0,(unsigned int)(dy*ny));
        if (offsety) cimg_foroff(seqy,y) seqy(y) = (unsigned int)cimg::mod(seqy(y) + offsety,(float)_height);
        if (inverty) cimg_foroff(seqy,y) seqy(y) = _height - 1 - seqy(y);
     }
      return draw_grid(seqx,seqy,color,opacity,pattern_x,pattern_y);
    }

    //! Draw 1D graph.
    /**
       \param data Image containing the graph values I = f(x).
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param opacity Drawing opacity.

       \param plot_type Define the type of the plot:
                      - 0 = No plot.
                      - 1 = Plot using segments.
                      - 2 = Plot using cubic splines.
                      - 3 = Plot with bars.
       \param vertex_type Define the type of points:
                      - 0 = No points.
                      - 1 = Point.
                      - 2 = Straight cross.
                      - 3 = Diagonal cross.
                      - 4 = Filled circle.
                      - 5 = Outlined circle.
                      - 6 = Square.
                      - 7 = Diamond.
       \param ymin Lower bound of the y-range.
       \param ymax Upper bound of the y-range.
       \param pattern Drawing pattern.
       \note
         - if \c ymin==ymax==0, the y-range is computed automatically from the input samples.
    **/
    template<typename t, typename tc>
    CImg<T>& draw_graph(const CImg<t>& data,
                        const tc *const color, const float opacity=1,
                        const unsigned int plot_type=1, const int vertex_type=1,
                        const double ymin=0, const double ymax=0, const unsigned int pattern=~0U) {
      if (is_empty() || _height<=1) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_graph(): Specified color is (null).",
                                    cimg_instance);

      // Create shaded colors for displaying bar plots.
      CImg<tc> color1, color2;
      if (plot_type==3) {
        color1.assign(_spectrum); color2.assign(_spectrum);
        cimg_forC(*this,c) {
          color1[c] = (tc)std::min((float)cimg::type<tc>::max(),(float)color[c]*1.2f);
          color2[c] = (tc)(color[c]*0.4f);
        }
      }

      // Compute min/max and normalization factors.
      const ulongT
        siz = data.size(),
        _siz1 = siz - (plot_type!=3),
        siz1 = _siz1?_siz1:1;
      const unsigned int
        _width1 = _width - (plot_type!=3),
        width1 = _width1?_width1:1;
      double m = ymin, M = ymax;
      if (ymin==ymax) m = (double)data.max_min(M);
      if (m==M) { --m; ++M; }
      const float ca = (float)(M-m)/(_height - 1);
      bool init_hatch = true;

      // Draw graph edges.
      switch (plot_type%4) {
      case 1 : { // Segments
        int oX = 0, oY = (int)cimg::round((data[0] - m)/ca);
        if (siz==1) {
          const int Y = (int)cimg::round((*data - m)/ca);
          draw_line(0,Y,width() - 1,Y,color,opacity,pattern);
        } else {
          const float fx = (float)_width/siz1;
          for (ulongT off = 1; off<siz; ++off) {
            const int
              X = (int)cimg::round(off*fx) - 1,
              Y = (int)cimg::round((data[off]-m)/ca);
            draw_line(oX,oY,X,Y,color,opacity,pattern,init_hatch);
            oX = X; oY = Y;
            init_hatch = false;
          }
        }
      } break;
      case 2 : { // Spline
        const CImg<t> ndata(data._data,siz,1,1,1,true);
        int oY = (int)cimg::round((data[0] - m)/ca);
        cimg_forX(*this,x) {
          const int Y = (int)cimg::round((ndata._cubic_atX((float)x*siz1/width1)-m)/ca);
          if (x>0) draw_line(x,oY,x + 1,Y,color,opacity,pattern,init_hatch);
          init_hatch = false;
          oY = Y;
        }
      } break;
      case 3 : { // Bars
        const int Y0 = (int)cimg::round(-m/ca);
        const float fx = (float)_width/siz1;
        int oX = 0;
        cimg_foroff(data,off) {
          const int
            X = (int)cimg::round((off + 1)*fx) - 1,
            Y = (int)cimg::round((data[off] - m)/ca);
          draw_rectangle(oX,Y0,X,Y,color,opacity).
            draw_line(oX,Y,oX,Y0,color2.data(),opacity).
            draw_line(oX,Y0,X,Y0,Y<=Y0?color2.data():color1.data(),opacity).
            draw_line(X,Y,X,Y0,color1.data(),opacity).
            draw_line(oX,Y,X,Y,Y<=Y0?color1.data():color2.data(),opacity);
          oX = X + 1;
        }
      } break;
      default : break; // No edges
      }

      // Draw graph points.
      const unsigned int wb2 = plot_type==3?_width1/(2*siz):0;
      const float fx = (float)_width1/siz1;
      switch (vertex_type%8) {
      case 1 : { // Point
        cimg_foroff(data,off) {
          const int
            X = (int)cimg::round(off*fx + wb2),
            Y = (int)cimg::round((data[off]-m)/ca);
          draw_point(X,Y,color,opacity);
        }
      } break;
      case 2 : { // Straight Cross
        cimg_foroff(data,off) {
          const int
            X = (int)cimg::round(off*fx + wb2),
            Y = (int)cimg::round((data[off]-m)/ca);
          draw_line(X - 3,Y,X + 3,Y,color,opacity).draw_line(X,Y - 3,X,Y + 3,color,opacity);
        }
      } break;
      case 3 : { // Diagonal Cross
        cimg_foroff(data,off) {
          const int
            X = (int)cimg::round(off*fx + wb2),
            Y = (int)cimg::round((data[off]-m)/ca);
          draw_line(X - 3,Y - 3,X + 3,Y + 3,color,opacity).draw_line(X - 3,Y + 3,X + 3,Y - 3,color,opacity);
        }
      } break;
      case 4 : { // Filled Circle
        cimg_foroff(data,off) {
          const int
            X = (int)cimg::round(off*fx + wb2),
            Y = (int)cimg::round((data[off]-m)/ca);
          draw_circle(X,Y,3,color,opacity);
        }
      } break;
      case 5 : { // Outlined circle
        cimg_foroff(data,off) {
          const int
            X = (int)cimg::round(off*fx + wb2),
            Y = (int)cimg::round((data[off]-m)/ca);
          draw_circle(X,Y,3,color,opacity,~0U);
        }
      } break;
      case 6 : { // Square
        cimg_foroff(data,off) {
          const int
            X = (int)cimg::round(off*fx + wb2),
            Y = (int)cimg::round((data[off]-m)/ca);
          draw_rectangle(X - 3,Y - 3,X + 3,Y + 3,color,opacity,~0U);
        }
      } break;
      case 7 : { // Diamond
        cimg_foroff(data,off) {
          const int
            X = (int)cimg::round(off*fx + wb2),
            Y = (int)cimg::round((data[off]-m)/ca);
          draw_line(X,Y - 4,X + 4,Y,color,opacity).
            draw_line(X + 4,Y,X,Y + 4,color,opacity).
            draw_line(X,Y + 4,X - 4,Y,color,opacity).
            draw_line(X - 4,Y,X,Y - 4,color,opacity);
        }
      } break;
      default : break; // No points
      }
      return *this;
    }

    bool _draw_fill(const int x, const int y, const int z,
                    const CImg<T>& ref, const float tolerance2) const {
      const T *ptr1 = data(x,y,z), *ptr2 = ref._data;
      const ulongT off = _width*_height*_depth;
      float diff = 0;
      cimg_forC(*this,c) { diff += cimg::sqr(*ptr1 - *(ptr2++)); ptr1+=off; }
      return diff<=tolerance2;
    }

    //! Draw filled 3D region with the flood fill algorithm.
    /**
       \param x0 X-coordinate of the starting point of the region to fill.
       \param y0 Y-coordinate of the starting point of the region to fill.
       \param z0 Z-coordinate of the starting point of the region to fill.
       \param color Pointer to \c spectrum() consecutive values, defining the drawing color.
       \param[out] region Image that will contain the mask of the filled region mask, as an output.
       \param tolerance Tolerance concerning neighborhood values.
       \param opacity Opacity of the drawing.
       \param is_high_connectivity Tells if 8-connexity must be used.
       \return \c region is initialized with the binary mask of the filled region.
    **/
    template<typename tc, typename t>
    CImg<T>& draw_fill(const int x0, const int y0, const int z0,
                        const tc *const color, const float opacity,
                        CImg<t> &region,
                        const float tolerance = 0,
                        const bool is_high_connectivity = false) {
#define _draw_fill_push(x,y,z) if (N>=stack._width) stack.resize(2*N + 1,1,1,3,0); \
                               stack[N] = x; stack(N,1) = y; stack(N++,2) = z
#define _draw_fill_pop(x,y,z) x = stack[--N]; y = stack(N,1); z = stack(N,2)
#define _draw_fill_is_inside(x,y,z) !_region(x,y,z) && _draw_fill(x,y,z,ref,tolerance2)

      if (!containsXYZC(x0,y0,z0,0)) return *this;
      const float nopacity = cimg::abs((float)opacity), copacity = 1 - std::max((float)opacity,0.f);
      const float tolerance2 = cimg::sqr(tolerance);
      const CImg<T> ref = get_vector_at(x0,y0,z0);
      CImg<uintT> stack(256,1,1,3);
      CImg<ucharT> _region(_width,_height,_depth,1,0);
      unsigned int N = 0;
      int x, y, z;

      _draw_fill_push(x0,y0,z0);
      while (N>0) {
        _draw_fill_pop(x,y,z);
        if (!_region(x,y,z)) {
          const int yp = y - 1, yn = y + 1, zp = z - 1, zn = z + 1;
          int xl = x, xr = x;

          // Using these booleans reduces the number of pushes drastically.
          bool is_yp = false, is_yn = false, is_zp = false, is_zn = false;
          for (int step = -1; step<2; step+=2) {
            while (x>=0 && x<width() && _draw_fill_is_inside(x,y,z)) {
              if (yp>=0 && _draw_fill_is_inside(x,yp,z)) {
                if (!is_yp) { _draw_fill_push(x,yp,z); is_yp = true; }
              } else is_yp = false;
              if (yn<height() && _draw_fill_is_inside(x,yn,z)) {
                if (!is_yn) { _draw_fill_push(x,yn,z); is_yn = true; }
              } else is_yn = false;
              if (depth()>1) {
                if (zp>=0 && _draw_fill_is_inside(x,y,zp)) {
                  if (!is_zp) { _draw_fill_push(x,y,zp); is_zp = true; }
                } else is_zp = false;
                if (zn<depth() && _draw_fill_is_inside(x,y,zn)) {
                  if (!is_zn) { _draw_fill_push(x,y,zn); is_zn = true; }
                } else is_zn = false;
              }
              if (is_high_connectivity) {
                const int xp = x - 1, xn = x + 1;
                if (yp>=0 && !is_yp) {
                  if (xp>=0 && _draw_fill_is_inside(xp,yp,z)) {
                    _draw_fill_push(xp,yp,z); if (step<0) is_yp = true;
                  }
                  if (xn<width() && _draw_fill_is_inside(xn,yp,z)) {
                    _draw_fill_push(xn,yp,z); if (step>0) is_yp = true;
                  }
                }
                if (yn<height() && !is_yn) {
                  if (xp>=0 && _draw_fill_is_inside(xp,yn,z)) {
                    _draw_fill_push(xp,yn,z); if (step<0) is_yn = true;
                  }
                  if (xn<width() && _draw_fill_is_inside(xn,yn,z)) {
                    _draw_fill_push(xn,yn,z); if (step>0) is_yn = true;
                  }
                }
                if (depth()>1) {
                  if (zp>=0 && !is_zp) {
                    if (xp>=0 && _draw_fill_is_inside(xp,y,zp)) {
                      _draw_fill_push(xp,y,zp); if (step<0) is_zp = true;
                    }
                    if (xn<width() && _draw_fill_is_inside(xn,y,zp)) {
                      _draw_fill_push(xn,y,zp); if (step>0) is_zp = true;
                    }

                    if (yp>=0 && !is_yp) {
                      if (_draw_fill_is_inside(x,yp,zp)) { _draw_fill_push(x,yp,zp); }
                      if (xp>=0 && _draw_fill_is_inside(xp,yp,zp)) { _draw_fill_push(xp,yp,zp); }
                      if (xn<width() && _draw_fill_is_inside(xn,yp,zp)) { _draw_fill_push(xn,yp,zp); }
                    }
                    if (yn<height() && !is_yn) {
                      if (_draw_fill_is_inside(x,yn,zp)) { _draw_fill_push(x,yn,zp); }
                      if (xp>=0 && _draw_fill_is_inside(xp,yn,zp)) { _draw_fill_push(xp,yn,zp); }
                      if (xn<width() && _draw_fill_is_inside(xn,yn,zp)) { _draw_fill_push(xn,yn,zp); }
                    }
                  }

                  if (zn<depth() && !is_zn) {
                    if (xp>=0 && _draw_fill_is_inside(xp,y,zn)) {
                      _draw_fill_push(xp,y,zn); if (step<0) is_zn = true;
                    }
                    if (xn<width() && _draw_fill_is_inside(xn,y,zn)) {
                      _draw_fill_push(xn,y,zn); if (step>0) is_zn = true;
                    }

                    if (yp>=0 && !is_yp) {
                      if (_draw_fill_is_inside(x,yp,zn)) { _draw_fill_push(x,yp,zn); }
                      if (xp>=0 && _draw_fill_is_inside(xp,yp,zn)) { _draw_fill_push(xp,yp,zn); }
                      if (xn<width() && _draw_fill_is_inside(xn,yp,zn)) { _draw_fill_push(xn,yp,zn); }
                    }
                    if (yn<height() && !is_yn) {
                      if (_draw_fill_is_inside(x,yn,zn)) { _draw_fill_push(x,yn,zn); }
                      if (xp>=0 && _draw_fill_is_inside(xp,yn,zn)) { _draw_fill_push(xp,yn,zn); }
                      if (xn<width() && _draw_fill_is_inside(xn,yn,zn)) { _draw_fill_push(xn,yn,zn); }
                    }
                  }
                }
              }
              x+=step;
            }
            if (step<0) { xl = ++x; x = xr + 1; is_yp = is_yn = is_zp = is_zn = false; }
            else xr = --x;
          }
          std::memset(_region.data(xl,y,z),1,xr - xl + 1);
          if (opacity==1) {
            if (sizeof(T)!=1) cimg_forC(*this,c) {
                const T val = (T)color[c];
                T *ptri = data(xl,y,z,c); for (int k = xl; k<=xr; ++k) *(ptri++) = val;
              }
            else {
              const int dx = xr - xl + 1;
              cimg_forC(*this,c) std::memset(data(xl,y,z,c),(int)color[c],dx);
            }
          } else cimg_forC(*this,c) {
              const T val = (T)(color[c]*nopacity);
              T *ptri = data(xl,y,z,c); for (int k = xl; k<=xr; ++k) { *ptri = (T)(val + *ptri*copacity); ++ptri; }
            }
        }
      }
      _region.move_to(region);
      return *this;
    }

    //! Draw filled 3D region with the flood fill algorithm \simplification.
    template<typename tc>
    CImg<T>& draw_fill(const int x0, const int y0, const int z0,
                       const tc *const color, const float opacity=1,
                       const float tolerance=0, const bool is_high_connexity=false) {
      CImg<ucharT> tmp;
      return draw_fill(x0,y0,z0,color,opacity,tmp,tolerance,is_high_connexity);
    }

    //! Draw filled 2D region with the flood fill algorithm \simplification.
    template<typename tc>
    CImg<T>& draw_fill(const int x0, const int y0,
                       const tc *const color, const float opacity=1,
                       const float tolerance=0, const bool is_high_connexity=false) {
      CImg<ucharT> tmp;
      return draw_fill(x0,y0,0,color,opacity,tmp,tolerance,is_high_connexity);
    }

    //! Draw a random plasma texture.
    /**
       \param alpha Alpha-parameter.
       \param beta Beta-parameter.
       \param scale Scale-parameter.
       \note Use the mid-point algorithm to render.
    **/
    CImg<T>& draw_plasma(const float alpha=1, const float beta=0, const unsigned int scale=8) {
      if (is_empty()) return *this;
      const int
        w0 = width(), h0 = height(),
        delta = 1<<std::min(scale,12U),
        w = cimg::round(w0,delta,1),
        h = cimg::round(h0,delta,1);
      cimg_uint64 rng = (cimg::_rand(),cimg::rng());
      CImg<T> canvas(w,h,depth(),spectrum());

      cimg_forZC(*this,z,c) {
        CImg<T> ref = canvas.get_shared_slice(z,c);

        // Init step.
        float r = alpha*delta + beta;
        for (int yt = 0; yt<h; yt+=delta)
          for (int xt = 0; xt<w; xt+=delta) {
            const Tfloat val = r*cimg::rand(-1,1,&rng);
#endif
