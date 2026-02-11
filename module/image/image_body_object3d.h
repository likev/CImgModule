#ifndef CIMG_MODULE_IMAGE_BODY_OBJECT3D_H
#define CIMG_MODULE_IMAGE_BODY_OBJECT3D_H

    //-------------------------------------
    //
    //! \name 3D Objects Management
    //@{
    //-------------------------------------

    //! Rotate 3D object's vertices.
    /**
       \param x X-coordinate of the rotation axis, or first quaternion coordinate.
       \param y Y-coordinate of the rotation axis, or second quaternion coordinate.
       \param z Z-coordinate of the rotation axis, or second quaternion coordinate.
       \param w Angle of the rotation axis (in degree), or fourth quaternion coordinate.
       \param is_quaternion Tell is the four arguments denotes a set { axis + angle } or a quaternion (x,y,z,w).
    **/
    CImg<T>& rotate_object3d(const float x, const float y, const float z, const float w,
                             const bool is_quaternion=false) {
      return get_rotate_object3d(x,y,z,w,is_quaternion).move_to(*this);
    }

    CImg<Tfloat> get_rotate_object3d(const float x, const float y, const float z, const float w,
                                     const bool is_quaternion=false) const {
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "rotate_object3d(): Instance is not a set of 3D vertices.",
                                    cimg_instance);
      return CImg<Tfloat>::rotation_matrix(x,y,z,w,is_quaternion)**this;
    }

    //! Shift 3D object's vertices.
    /**
       \param tx X-coordinate of the 3D displacement vector.
       \param ty Y-coordinate of the 3D displacement vector.
       \param tz Z-coordinate of the 3D displacement vector.
    **/
    CImg<T>& shift_object3d(const float tx, const float ty=0, const float tz=0) {
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "shift_object3d(): Instance is not a set of 3D vertices.",
                                    cimg_instance);

      get_shared_row(0)+=tx; get_shared_row(1)+=ty; get_shared_row(2)+=tz;
      return *this;
    }

    //! Shift 3D object's vertices \newinstance.
    CImg<Tfloat> get_shift_object3d(const float tx, const float ty=0, const float tz=0) const {
      return CImg<Tfloat>(*this,false).shift_object3d(tx,ty,tz);
    }

    //! Shift 3D object's vertices, so that it becomes centered.
    /**
       \note The object center is computed as its barycenter.
    **/
    CImg<T>& shift_object3d() {
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "shift_object3d(): Instance is not a set of 3D vertices.",
                                    cimg_instance);

      CImg<T> xcoords = get_shared_row(0), ycoords = get_shared_row(1), zcoords = get_shared_row(2);
      float
        xm, xM = (float)xcoords.max_min(xm),
        ym, yM = (float)ycoords.max_min(ym),
        zm, zM = (float)zcoords.max_min(zm);
      xcoords-=(xm + xM)/2; ycoords-=(ym + yM)/2; zcoords-=(zm + zM)/2;
      return *this;
    }

    //! Shift 3D object's vertices, so that it becomes centered \newinstance.
    CImg<Tfloat> get_shift_object3d() const {
      return CImg<Tfloat>(*this,false).shift_object3d();
    }

    //! Resize 3D object.
    /**
       \param sx Width of the 3D object's bounding box.
       \param sy Height of the 3D object's bounding box.
       \param sz Depth of the 3D object's bounding box.
    **/
    CImg<T>& resize_object3d(const float sx, const float sy=-100, const float sz=-100) {
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "resize_object3d(): Instance is not a set of 3D vertices.",
                                    cimg_instance);

      CImg<T> xcoords = get_shared_row(0), ycoords = get_shared_row(1), zcoords = get_shared_row(2);
      float
        xm, xM = (float)xcoords.max_min(xm),
        ym, yM = (float)ycoords.max_min(ym),
        zm, zM = (float)zcoords.max_min(zm);
      if (xm<xM) { if (sx>0) xcoords*=sx/(xM-xm); else xcoords*=-sx/100; }
      if (ym<yM) { if (sy>0) ycoords*=sy/(yM-ym); else ycoords*=-sy/100; }
      if (zm<zM) { if (sz>0) zcoords*=sz/(zM-zm); else zcoords*=-sz/100; }
      return *this;
    }

    //! Resize 3D object \newinstance.
    CImg<Tfloat> get_resize_object3d(const float sx, const float sy=-100, const float sz=-100) const {
      return CImg<Tfloat>(*this,false).resize_object3d(sx,sy,sz);
    }

    //! Resize 3D object to unit size.
    CImg<T> resize_object3d() {
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "resize_object3d(): Instance is not a set of 3D vertices.",
                                    cimg_instance);

      CImg<T> xcoords = get_shared_row(0), ycoords = get_shared_row(1), zcoords = get_shared_row(2);
      float
        xm, xM = (float)xcoords.max_min(xm),
        ym, yM = (float)ycoords.max_min(ym),
        zm, zM = (float)zcoords.max_min(zm);
      const float dx = xM - xm, dy = yM - ym, dz = zM - zm, dmax = cimg::max(dx,dy,dz);
      if (dmax>0) { xcoords/=dmax; ycoords/=dmax; zcoords/=dmax; }
      return *this;
    }

    //! Resize 3D object to unit size \newinstance.
    CImg<Tfloat> get_resize_object3d() const {
      return CImg<Tfloat>(*this,false).resize_object3d();
    }

    //! Merge two 3D objects together.
    /**
       \param[in,out] primitives Primitives data of the current 3D object.
       \param obj_vertices Vertices data of the additional 3D object.
       \param obj_primitives Primitives data of the additional 3D object.
    **/
    template<typename tf, typename tp, typename tff>
    CImg<T>& append_object3d(CImgList<tf>& primitives, const CImg<tp>& obj_vertices,
                             const CImgList<tff>& obj_primitives) {
      if (!obj_vertices || !obj_primitives) return *this;
      if (obj_vertices._height!=3 || obj_vertices._depth>1 || obj_vertices._spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "append_object3d(): Specified vertice image (%u,%u,%u,%u,%p) is not a "
                                    "set of 3D vertices.",
                                    cimg_instance,
                                    obj_vertices._width,obj_vertices._height,
                                    obj_vertices._depth,obj_vertices._spectrum,obj_vertices._data);

      if (is_empty()) { primitives.assign(obj_primitives); return assign(obj_vertices); }
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "append_object3d(): Instance is not a set of 3D vertices.",
                                    cimg_instance);

      const unsigned int P = _width;
      append(obj_vertices,'x');
      const unsigned int N = primitives._width;
      primitives.insert(obj_primitives);
      for (unsigned int i = N; i<primitives._width; ++i) {
        CImg<tf> &p = primitives[i];
        switch (p.size()) {
        case 1 : p[0]+=P; break; // Point
        case 5 : p[0]+=P; p[1]+=P; break; // Sphere
        case 2 : case 6 : p[0]+=P; p[1]+=P; break; // Segment
        case 3 : case 9 : p[0]+=P; p[1]+=P; p[2]+=P; break; // Triangle
        case 4 : case 12 : p[0]+=P; p[1]+=P; p[2]+=P; p[3]+=P; break; // Rectangle
        }
      }
      return *this;
    }

    //! Texturize primitives of a 3D object.
    /**
       \param[in,out] primitives Primitives data of the 3D object.
       \param[in,out] colors Colors data of the 3D object.
       \param texture Texture image to map to 3D object.
       \param coords Texture-mapping coordinates.
    **/
    template<typename tp, typename tc, typename tt, typename tx>
    const CImg<T>& texturize_object3d(CImgList<tp>& primitives, CImgList<tc>& colors,
                                      const CImg<tt>& texture, const CImg<tx>& coords=CImg<tx>::const_empty()) const {
      if (is_empty()) return *this;
      if (_height!=3)
        throw CImgInstanceException(_cimg_instance
                                    "texturize_object3d(): image instance is not a set of 3D points.",
                                    cimg_instance);
      if (coords && (coords._width!=_width || coords._height!=2))
        throw CImgArgumentException(_cimg_instance
                                    "texturize_object3d(): Invalid specified texture coordinates (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    coords._width,coords._height,coords._depth,coords._spectrum,coords._data);
      CImg<intT> _coords;
      if (!coords) { // If no texture coordinates specified, do a default XY-projection
        _coords.assign(_width,2);
        float
          xmin, xmax = (float)get_shared_row(0).max_min(xmin),
          ymin, ymax = (float)get_shared_row(1).max_min(ymin),
          dx = xmax>xmin?xmax-xmin:1,
          dy = ymax>ymin?ymax-ymin:1;
        cimg_forX(*this,p) {
          _coords(p,0) = (int)(((*this)(p,0) - xmin)*texture._width/dx);
          _coords(p,1) = (int)(((*this)(p,1) - ymin)*texture._height/dy);
        }
      } else _coords = coords;

      int texture_ind = -1;
      cimglist_for(primitives,l) {
        CImg<tp> &p = primitives[l];
        const unsigned int siz = p.size();
        switch (siz) {
        case 1 : { // Point
          const unsigned int i0 = (unsigned int)p[0];
          const int x0 = _coords(i0,0), y0 = _coords(i0,1);
          texture.get_vector_at(x0<=0?0:x0>=texture.width()?texture.width() - 1:x0,
                                y0<=0?0:y0>=texture.height()?texture.height() - 1:y0).move_to(colors[l]);
        } break;
        case 2 : case 6 : { // Segment
          const unsigned int i0 = (unsigned int)p[0], i1 = (unsigned int)p[1];
          const int
            x0 = _coords(i0,0), y0 = _coords(i0,1),
            x1 = _coords(i1,0), y1 = _coords(i1,1);
          if (texture_ind<0) colors[texture_ind=l].assign(texture,false);
          else colors[l].assign(colors[texture_ind],true);
          CImg<tp>::vector(i0,i1,x0,y0,x1,y1).move_to(p);
        } break;
        case 3 : case 9 : { // Triangle
          const unsigned int i0 = (unsigned int)p[0], i1 = (unsigned int)p[1], i2 = (unsigned int)p[2];
          const int
            x0 = _coords(i0,0), y0 = _coords(i0,1),
            x1 = _coords(i1,0), y1 = _coords(i1,1),
            x2 = _coords(i2,0), y2 = _coords(i2,1);
          if (texture_ind<0) colors[texture_ind=l].assign(texture,false);
          else colors[l].assign(colors[texture_ind],true);
          CImg<tp>::vector(i0,i1,i2,x0,y0,x1,y1,x2,y2).move_to(p);
        } break;
        case 4 : case 12 : { // Quadrangle
          const unsigned int
            i0 = (unsigned int)p[0], i1 = (unsigned int)p[1], i2 = (unsigned int)p[2], i3 = (unsigned int)p[3];
          const int
            x0 = _coords(i0,0), y0 = _coords(i0,1),
            x1 = _coords(i1,0), y1 = _coords(i1,1),
            x2 = _coords(i2,0), y2 = _coords(i2,1),
            x3 = _coords(i3,0), y3 = _coords(i3,1);
          if (texture_ind<0) colors[texture_ind=l].assign(texture,false);
          else colors[l].assign(colors[texture_ind],true);
          CImg<tp>::vector(i0,i1,i2,i3,x0,y0,x1,y1,x2,y2,x3,y3).move_to(p);
        } break;
        }
      }
      return *this;
    }

    //! Generate a 3D elevation of the image instance.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param[out] colors The returned list of the 3D object colors.
       \param elevation The input elevation map.
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       const CImg<float> img("reference.jpg");
       CImgList<unsigned int> faces3d;
       CImgList<unsigned char> colors3d;
       const CImg<float> points3d = img.get_elevation3d(faces3d,colors3d,img.get_norm()*0.2);
       CImg<unsigned char>().display_object3d("Elevation3d",points3d,faces3d,colors3d);
       \endcode
       \image html ref_elevation3d.jpg
    **/
    template<typename tf, typename tc, typename te>
    CImg<floatT> get_elevation3d(CImgList<tf>& primitives, CImgList<tc>& colors, const CImg<te>& elevation) const {
      if (!is_sameXY(elevation) || elevation._depth>1 || elevation._spectrum>1)
        throw CImgArgumentException(_cimg_instance
                                    "get_elevation3d(): Instance and specified elevation (%u,%u,%u,%u,%p) "
                                    "have incompatible dimensions.",
                                    cimg_instance,
                                    elevation._width,elevation._height,elevation._depth,
                                    elevation._spectrum,elevation._data);
      if (is_empty()) return *this;
      float m, M = (float)max_min(m);
      if (M==m) ++M;
      colors.assign();
      const unsigned int size_x1 = _width - 1, size_y1 = _height - 1;
      for (unsigned int y = 0; y<size_y1; ++y)
        for (unsigned int x = 0; x<size_x1; ++x) {
          const unsigned char
            r = (unsigned char)(((*this)(x,y,0) - m)*255/(M-m)),
            g = (unsigned char)(_spectrum>1?((*this)(x,y,1) - m)*255/(M-m):r),
            b = (unsigned char)(_spectrum>2?((*this)(x,y,2) - m)*255/(M-m):_spectrum>1?0:r);
          CImg<tc>::vector((tc)r,(tc)g,(tc)b).move_to(colors);
        }
      const typename CImg<te>::_functor2d_int func(elevation);
      return elevation3d(primitives,func,0,0,_width - 1.f,_height - 1.f,_width,_height);
    }

    //! Generate the 3D projection planes of the image instance.
    /**
       \param[out] primitives Primitives data of the returned 3D object.
       \param[out] colors Colors data of the returned 3D object.
       \param x0 X-coordinate of the projection point.
       \param y0 Y-coordinate of the projection point.
       \param z0 Z-coordinate of the projection point.
       \param normalize_colors Tells if the created textures have normalized colors.
    **/
    template<typename tf, typename tc>
    CImg<floatT> get_projections3d(CImgList<tf>& primitives, CImgList<tc>& colors,
                                   const unsigned int x0, const unsigned int y0, const unsigned int z0,
                                   const bool normalize_colors=false) const {
      float m = 0, M = 0, delta = 1;
      if (normalize_colors) { m = (float)min_max(M); delta = 255/(m==M?1:M-m); }
      const unsigned int
        _x0 = (x0>=_width)?_width - 1:x0,
        _y0 = (y0>=_height)?_height - 1:y0,
        _z0 = (z0>=_depth)?_depth - 1:z0;
      CImg<tc> img_xy, img_xz, img_yz;
      if (normalize_colors) {
        ((get_crop(0,0,_z0,0,_width - 1,_height - 1,_z0,_spectrum - 1)-=m)*=delta).move_to(img_xy);
        ((get_crop(0,_y0,0,0,_width - 1,_y0,_depth - 1,_spectrum - 1)-=m)*=delta).resize(_width,_depth,1,-100,-1).
          move_to(img_xz);
        ((get_crop(_x0,0,0,0,_x0,_height - 1,_depth - 1,_spectrum - 1)-=m)*=delta).resize(_height,_depth,1,-100,-1).
          move_to(img_yz);
      } else {
        get_crop(0,0,_z0,0,_width - 1,_height - 1,_z0,_spectrum - 1).move_to(img_xy);
        get_crop(0,_y0,0,0,_width - 1,_y0,_depth - 1,_spectrum - 1).resize(_width,_depth,1,-100,-1).move_to(img_xz);
        get_crop(_x0,0,0,0,_x0,_height - 1,_depth - 1,_spectrum - 1).resize(_height,_depth,1,-100,-1).move_to(img_yz);
      }
      CImg<floatT> points(12,3,1,1,
                          0,_width - 1,_width - 1,0,   0,_width - 1,_width - 1,0, _x0,_x0,_x0,_x0,
                          0,0,_height - 1,_height - 1, _y0,_y0,_y0,_y0,       0,_height - 1,_height - 1,0,
                          _z0,_z0,_z0,_z0,         0,0,_depth - 1,_depth - 1, 0,0,_depth - 1,_depth - 1);
      primitives.assign();
      CImg<tf>::vector(0,1,2,3,0,0,img_xy._width - 1,0,img_xy._width - 1,img_xy._height - 1,0,img_xy._height - 1).
        move_to(primitives);
      CImg<tf>::vector(4,5,6,7,0,0,img_xz._width - 1,0,img_xz._width - 1,img_xz._height - 1,0,img_xz._height - 1).
        move_to(primitives);
      CImg<tf>::vector(8,9,10,11,0,0,img_yz._width - 1,0,img_yz._width - 1,img_yz._height - 1,0,img_yz._height - 1).
        move_to(primitives);
      colors.assign();
      img_xy.move_to(colors);
      img_xz.move_to(colors);
      img_yz.move_to(colors);
      return points;
    }

    //! Generate a isoline of the image instance as a 3D object.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param isovalue The returned list of the 3D object colors.
       \param size_x The number of subdivisions along the X-axis.
       \param size_y The number of subdisivions along the Y-axis.
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       const CImg<float> img("reference.jpg");
       CImgList<unsigned int> faces3d;
       const CImg<float> points3d = img.get_isoline3d(faces3d,100);
       CImg<unsigned char>().display_object3d("Isoline3d",points3d,faces3d,colors3d);
       \endcode
       \image html ref_isoline3d.jpg
    **/
    template<typename tf>
    CImg<floatT> get_isoline3d(CImgList<tf>& primitives, const float isovalue,
                               const int size_x=-100, const int size_y=-100) const {
      if (_spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "get_isoline3d(): Instance is not a scalar image.",
                                    cimg_instance);
      if (_depth>1)
        throw CImgInstanceException(_cimg_instance
                                    "get_isoline3d(): Instance is not a 2D image.",
                                    cimg_instance);
      primitives.assign();
      if (is_empty()) return *this;
      CImg<floatT> vertices;
      if ((size_x==-100 && size_y==-100) || (size_x==width() && size_y==height())) {
        const _functor2d_int func(*this);
        vertices = isoline3d(primitives,func,isovalue,0,0,width() - 1.f,height() - 1.f,width(),height());
      } else {
        const _functor2d_float func(*this);
        vertices = isoline3d(primitives,func,isovalue,0,0,width() - 1.f,height() - 1.f,size_x,size_y);
      }
      return vertices;
    }

    //! Compute isolines of a function, as a 3D object.
    /**
       \param[out] primitives Primitives data of the resulting 3D object.
       \param func Elevation functor. Must have <tt>operator()(x,y)</tt> defined.
       \param isovalue Isovalue to extract from function.
       \param x0 X-coordinate of the starting point.
       \param y0 Y-coordinate of the starting point.
       \param x1 X-coordinate of the ending point.
       \param y1 Y-coordinate of the ending point.
       \param size_x Resolution of the function along the X-axis.
       \param size_y Resolution of the function along the Y-axis.
       \note Use the marching squares algorithm for extracting the isolines.
     **/
    template<typename tf, typename tfunc>
    static CImg<floatT> isoline3d(CImgList<tf>& primitives, const tfunc& func, const float isovalue,
                                  const float x0, const float y0, const float x1, const float y1,
                                  const int size_x=256, const int size_y=256) {
      CImgList<floatT> vertices;
      primitives.assign();
      typename CImg<floatT>::_functor_isoline3d add_vertex(vertices);
      typename CImg<tf>::_functor_isoline3d add_segment(primitives);
      isoline3d(add_vertex,add_segment,func,isovalue,x0,y0,x1,y1,size_x,size_y);
      return vertices>'x';
    }

    //! Compute isolines of a function, as a 3D object.
    /**
       \param[out] add_vertex : Functor with operator()(x,y,z) defined for adding new vertex.
       \param[out] add_segment : Functor with operator()(i,j) defined for adding new segment.
       \param func Elevation function. Is of type <tt>float (*func)(const float x,const float y)</tt>.
       \param isovalue Isovalue to extract from function.
       \param x0 X-coordinate of the starting point.
       \param y0 Y-coordinate of the starting point.
       \param x1 X-coordinate of the ending point.
       \param y1 Y-coordinate of the ending point.
       \param size_x Resolution of the function along the X-axis.
       \param size_y Resolution of the function along the Y-axis.
       \note Use the marching squares algorithm for extracting the isolines.
     **/
    template<typename tv, typename tf, typename tfunc>
    static void isoline3d(tv& add_vertex, tf& add_segment, const tfunc& func, const float isovalue,
                          const float x0, const float y0, const float x1, const float y1,
                          const int size_x, const int size_y) {
      static const unsigned int edges[16] = { 0x0, 0x9, 0x3, 0xa, 0x6, 0xf, 0x5, 0xc, 0xc,
                                              0x5, 0xf, 0x6, 0xa, 0x3, 0x9, 0x0 };
      static const int segments[16][4] = { { -1,-1,-1,-1 }, { 0,3,-1,-1 }, { 0,1,-1,-1 }, { 1,3,-1,-1 },
                                           { 1,2,-1,-1 },   { 0,1,2,3 },   { 0,2,-1,-1 }, { 2,3,-1,-1 },
                                           { 2,3,-1,-1 },   { 0,2,-1,-1},  { 0,3,1,2 },   { 1,2,-1,-1 },
                                           { 1,3,-1,-1 },   { 0,1,-1,-1},  { 0,3,-1,-1},  { -1,-1,-1,-1 } };
      const unsigned int
        _nx = (unsigned int)(size_x>=0?size_x:cimg::round((x1-x0)*-size_x/100 + 1)),
        _ny = (unsigned int)(size_y>=0?size_y:cimg::round((y1-y0)*-size_y/100 + 1)),
        nx = _nx?_nx:1,
        ny = _ny?_ny:1,
        nxm1 = nx - 1,
        nym1 = ny - 1;

      if (!nxm1 || !nym1) return;
      const float dx = (x1 - x0)/nxm1, dy = (y1 - y0)/nym1;
      CImg<intT> indices1(nx,1,1,2,-1), indices2(nx,1,1,2);
      CImg<floatT> values1(nx), values2(nx);
      float X = x0, Y = y0, nX = X + dx, nY = Y + dy;
      int nb_vertices = 0;

      // Fill first line with values.
      cimg_forX(values1,x) { values1(x) = (float)func(X,Y); X+=dx; }

      // Run the marching squares algorithm.
      for (unsigned int yi = 0, nyi = 1; yi<nym1; ++yi, ++nyi, Y=nY, nY+=dy) {
        X = x0; nX = X + dx;
        indices2.fill(-1);
        values2(0) = (float)func(X,nY);
        for (unsigned int xi = 0, nxi = 1; xi<nxm1; ++xi, ++nxi, X=nX, nX+=dx) {

          // Determine square configuration.
          const float
            val0 = values1(xi),
            val1 = values1(nxi),
            val2 = values2(nxi) = (float)func(nX,nY),
            val3 = values2(xi);
          const unsigned int
            configuration = (val0<isovalue?1U:0U) | (val1<isovalue?2U:0U) |
            (val2<isovalue?4U:0U) | (val3<isovalue?8U:0U),
            edge = edges[configuration];

          // Compute intersection vertices.
          if (edge) {
            if ((edge&1) && indices1(xi,0)<0) {
              const float Xi = X + (isovalue-val0)*dx/(val1-val0);
              indices1(xi,0) = nb_vertices++;
              add_vertex(Xi,Y,0.f);
            }
            if ((edge&2) && indices1(nxi,1)<0) {
              const float Yi = Y + (isovalue-val1)*dy/(val2-val1);
              indices1(nxi,1) = nb_vertices++;
              add_vertex(nX,Yi,0.f);
            }
            if ((edge&4) && indices2(xi,0)<0) {
              const float Xi = X + (isovalue-val3)*dx/(val2-val3);
              indices2(xi,0) = nb_vertices++;
              add_vertex(Xi,nY,0.f);
            }
            if ((edge&8) && indices1(xi,1)<0) {
              const float Yi = Y + (isovalue-val0)*dy/(val3-val0);
              indices1(xi,1) = nb_vertices++;
              add_vertex(X,Yi,0.f);
            }

            // Create segments.
            for (const int *segment = segments[configuration]; *segment!=-1; ) {
              const unsigned int p0 = (unsigned int)*(segment++), p1 = (unsigned int)*(segment++);
              const int
                i0 = _isoline3d_index(p0,indices1,indices2,xi,nxi),
                i1 = _isoline3d_index(p1,indices1,indices2,xi,nxi);
              add_segment(i0,i1);
            }
          }
        }
        values1.swap(values2);
        indices1.swap(indices2);
      }
    }

    //! Compute isolines of a function, as a 3D object \overloading.
    template<typename tf>
    static CImg<floatT> isoline3d(CImgList<tf>& primitives, const char *const expression, const float isovalue,
                                  const float x0, const float y0, const float x1, const float y1,
                                  const int size_x=256, const int size_y=256) {
      const _functor2d_expr func(expression);
      return isoline3d(primitives,func,isovalue,x0,y0,x1,y1,size_x,size_y);
    }

    template<typename t>
    static int _isoline3d_index(const unsigned int edge, const CImg<t>& indices1, const CImg<t>& indices2,
                                 const unsigned int x, const unsigned int nx) {
      switch (edge) {
      case 0 : return (int)indices1(x,0);
      case 1 : return (int)indices1(nx,1);
      case 2 : return (int)indices2(x,0);
      case 3 : return (int)indices1(x,1);
      }
      return 0;
    }

    //! Generate an isosurface of the image instance as a 3D object.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param isovalue The returned list of the 3D object colors.
       \param size_x Number of subdivisions along the X-axis.
       \param size_y Number of subdisivions along the Y-axis.
       \param size_z Number of subdisivions along the Z-axis.
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       const CImg<float> img = CImg<unsigned char>("reference.jpg").resize(-100,-100,20);
       CImgList<unsigned int> faces3d;
       const CImg<float> points3d = img.get_isosurface3d(faces3d,100);
       CImg<unsigned char>().display_object3d("Isosurface3d",points3d,faces3d,colors3d);
       \endcode
       \image html ref_isosurface3d.jpg
    **/
    template<typename tf>
    CImg<floatT> get_isosurface3d(CImgList<tf>& primitives, const float isovalue,
                                  const int size_x=-100, const int size_y=-100, const int size_z=-100) const {
      if (_spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "get_isosurface3d(): Instance is not a scalar image.",
                                    cimg_instance);
      primitives.assign();
      if (is_empty()) return *this;
      CImg<floatT> vertices;
      if ((size_x==-100 && size_y==-100 && size_z==-100) || (size_x==width() && size_y==height() && size_z==depth())) {
        const _functor3d_int func(*this);
        vertices = isosurface3d(primitives,func,isovalue,0,0,0,width() - 1.f,height() - 1.f,depth() - 1.f,
                                width(),height(),depth());
      } else {
        const _functor3d_float func(*this);
        vertices = isosurface3d(primitives,func,isovalue,0,0,0,width() - 1.f,height() - 1.f,depth() - 1.f,
                                size_x,size_y,size_z);
      }
      return vertices;
    }

    //! Compute isosurface of a function, as a 3D object.
    /**
       \param[out] primitives Primitives data of the resulting 3D object.
       \param func Implicit function. Is of type <tt>float (*func)(const float x, const float y, const float z)</tt>.
       \param isovalue Isovalue to extract.
       \param x0 X-coordinate of the starting point.
       \param y0 Y-coordinate of the starting point.
       \param z0 Z-coordinate of the starting point.
       \param x1 X-coordinate of the ending point.
       \param y1 Y-coordinate of the ending point.
       \param z1 Z-coordinate of the ending point.
       \param size_x Resolution of the elevation function along the X-axis.
       \param size_y Resolution of the elevation function along the Y-axis.
       \param size_z Resolution of the elevation function along the Z-axis.
       \note Use the marching cubes algorithm for extracting the isosurface.
     **/
    template<typename tf, typename tfunc>
    static CImg<floatT> isosurface3d(CImgList<tf>& primitives, const tfunc& func, const float isovalue,
                                     const float x0, const float y0, const float z0,
                                     const float x1, const float y1, const float z1,
                                     const int size_x=32, const int size_y=32, const int size_z=32) {
      CImgList<floatT> vertices;
      primitives.assign();
      typename CImg<floatT>::_functor_isosurface3d add_vertex(vertices);
      typename CImg<tf>::_functor_isosurface3d add_triangle(primitives);
      isosurface3d(add_vertex,add_triangle,func,isovalue,x0,y0,z0,x1,y1,z1,size_x,size_y,size_z);
      return vertices>'x';
    }

    //! Compute isosurface of a function, as a 3D object.
    /**
       \param[out] add_vertex : Functor with operator()(x,y,z) defined for adding new vertex.
       \param[out] add_triangle : Functor with operator()(i,j) defined for adding new segment.
       \param func Implicit function. Is of type <tt>float (*func)(const float x, const float y, const float z)</tt>.
       \param isovalue Isovalue to extract.
       \param x0 X-coordinate of the starting point.
       \param y0 Y-coordinate of the starting point.
       \param z0 Z-coordinate of the starting point.
       \param x1 X-coordinate of the ending point.
       \param y1 Y-coordinate of the ending point.
       \param z1 Z-coordinate of the ending point.
       \param size_x Resolution of the elevation function along the X-axis.
       \param size_y Resolution of the elevation function along the Y-axis.
       \param size_z Resolution of the elevation function along the Z-axis.
       \note Use the marching cubes algorithm for extracting the isosurface.
     **/
    template<typename tv, typename tf, typename tfunc>
    static void isosurface3d(tv& add_vertex, tf& add_triangle, const tfunc& func, const float isovalue,
                             const float x0, const float y0, const float z0,
                             const float x1, const float y1, const float z1,
                             const int size_x, const int size_y, const int size_z) {
      static const unsigned int edges[256] = {
        0x000, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c, 0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
        0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c, 0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
        0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c, 0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
        0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac, 0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
        0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c, 0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
        0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc, 0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
        0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c, 0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
        0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc , 0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
        0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc, 0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
        0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c, 0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
        0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc, 0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
        0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c, 0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
        0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac, 0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
        0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c, 0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
        0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c, 0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
        0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c, 0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x000
      };

      static const int triangles[256][16] = {
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
        { 8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1 },
        { 3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1 },
        { 4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
        { 4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
        { 5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1 },
        { 9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1 },
        { 10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1 },
        { 5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
        { 5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1 },
        { 10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1 },
        { 8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1 },
        { 2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
        { 7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
        { 2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1 },
        { 11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1 },
        { 5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1 },
        { 11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1 },
        { 11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
        { 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1 },
        { 2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
        { 5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1 },
        { 6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
        { 3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1 },
        { 6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
        { 5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
        { 10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
        { 6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1 },
        { 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1 },
        { 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1 },
        { 3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
        { 5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
        { 0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1 },
        { 9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1 },
        { 8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
        { 5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1 },
        { 0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1 },
        { 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1 },
        { 10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1 },
        { 10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
        { 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
        { 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1 },
        { 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
        { 10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1 },
        { 3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
        { 6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1 },
        { 9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1 },
        { 8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1 },
        { 3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
        { 6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1 },
        { 10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1 },
        { 10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
        { 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1 },
        { 7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
        { 7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
        { 2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1 },
        { 1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1 },
        { 11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1 },
        { 8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1 },
        { 0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1 },
        { 7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
        { 10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
        { 6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1 },
        { 7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1 },
        { 10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
        { 10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1 },
        { 0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1 },
        { 7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
        { 6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
        { 8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1 },
        { 6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1 },
        { 4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1 },
        { 10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1 },
        { 8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1 },
        { 1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
        { 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1 },
        { 10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1 },
        { 10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
        { 5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
        { 11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1 },
        { 9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
        { 6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1 },
        { 7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1 },
        { 3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1 },
        { 7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1 },
        { 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1 },
        { 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1 },
        { 9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1 },
        { 1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1 },
        { 4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1 },
        { 7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1 },
        { 6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1 },
        { 0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1 },
        { 6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1 },
        { 0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1 },
        { 11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1 },
        { 6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1 },
        { 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1 },
        { 9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1 },
        { 1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1 },
        { 10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1 },
        { 0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1 },
        { 5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1 },
        { 10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1 },
        { 11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1 },
        { 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1 },
        { 7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1 },
        { 2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
        { 8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1 },
        { 9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1 },
        { 9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1 },
        { 1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
        { 5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1 },
        { 0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1 },
        { 10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1 },
        { 2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1 },
        { 0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1 },
        { 0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1 },
        { 9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1 },
        { 5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1 },
        { 5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1 },
        { 8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1 },
        { 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1 },
        { 1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1 },
        { 3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1 },
        { 4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1 },
        { 9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1 },
        { 11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
        { 11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1 },
        { 2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1 },
        { 9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1 },
        { 3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1 },
        { 1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1 },
        { 4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1 },
        { 0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
        { 9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1 },
        { 1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
      };

      const unsigned int
        _nx = (unsigned int)(size_x>=0?size_x:cimg::round((x1-x0)*-size_x/100 + 1)),
        _ny = (unsigned int)(size_y>=0?size_y:cimg::round((y1-y0)*-size_y/100 + 1)),
        _nz = (unsigned int)(size_z>=0?size_z:cimg::round((z1-z0)*-size_z/100 + 1)),
        nx = _nx?_nx:1,
        ny = _ny?_ny:1,
        nz = _nz?_nz:1,
        nxm1 = nx - 1,
        nym1 = ny - 1,
        nzm1 = nz - 1;
      if (!nxm1 || !nym1 || !nzm1) return;
      const float dx = (x1 - x0)/nxm1, dy = (y1 - y0)/nym1, dz = (z1 - z0)/nzm1;
      CImg<intT> indices1(nx,ny,1,3,-1), indices2(indices1);
      CImg<floatT> values1(nx,ny), values2(nx,ny);
      float X = 0, Y = 0, Z = 0, nX = 0, nY = 0, nZ = 0;
      int nb_vertices = 0;

      // Fill the first plane with function values.
      Y = y0;
      cimg_forY(values1,y) {
        X = x0;
        cimg_forX(values1,x) { values1(x,y) = (float)func(X,Y,z0); X+=dx; }
        Y+=dy;
      }

      // Run Marching Cubes algorithm.
      Z = z0; nZ = Z + dz;
      for (unsigned int zi = 0; zi<nzm1; ++zi, Z = nZ, nZ+=dz) {
        Y = y0; nY = Y + dy;
        indices2.fill(-1);
        X = x0; for (unsigned int xi = 0; xi<nx; ++xi) { values2(xi,0) = (float)func(X,Y,nZ); X += dx; }

        for (unsigned int yi = 0, nyi = 1; yi<nym1; ++yi, ++nyi, Y = nY, nY+=dy) {
          X = x0; nX = X + dx;
          values2(0,nyi) = (float)func(X,nY,nZ);

          for (unsigned int xi = 0, nxi = 1; xi<nxm1; ++xi, ++nxi, X = nX, nX+=dx) {

            // Determine cube configuration.
            const float
              val0 = values1(xi,yi),
              val1 = values1(nxi,yi),
              val2 = values1(nxi,nyi),
              val3 = values1(xi,nyi),
              val4 = values2(xi,yi),
              val5 = values2(nxi,yi),
              val6 = values2(nxi,nyi) = (float)func(nX,nY,nZ),
              val7 = values2(xi,nyi);

            const unsigned int configuration =
              (val0<isovalue?1U:0U)  | (val1<isovalue?2U:0U)  | (val2<isovalue?4U:0U)  | (val3<isovalue?8U:0U) |
              (val4<isovalue?16U:0U) | (val5<isovalue?32U:0U) | (val6<isovalue?64U:0U) | (val7<isovalue?128U:0U),
              edge = edges[configuration];

            // Compute intersection vertices.
            if (edge) {
              if ((edge&1) && indices1(xi,yi,0)<0) {
                const float Xi = X + (isovalue-val0)*dx/(val1-val0);
                indices1(xi,yi,0) = nb_vertices++;
                add_vertex(Xi,Y,Z);
              }
              if ((edge&2) && indices1(nxi,yi,1)<0) {
                const float Yi = Y + (isovalue-val1)*dy/(val2-val1);
                indices1(nxi,yi,1) = nb_vertices++;
                add_vertex(nX,Yi,Z);
              }
              if ((edge&4) && indices1(xi,nyi,0)<0) {
                const float Xi = X + (isovalue-val3)*dx/(val2-val3);
                indices1(xi,nyi,0) = nb_vertices++;
                add_vertex(Xi,nY,Z);
              }
              if ((edge&8) && indices1(xi,yi,1)<0) {
                const float Yi = Y + (isovalue-val0)*dy/(val3-val0);
                indices1(xi,yi,1) = nb_vertices++;
                add_vertex(X,Yi,Z);
              }
              if ((edge&16) && indices2(xi,yi,0)<0) {
                const float Xi = X + (isovalue-val4)*dx/(val5-val4);
                indices2(xi,yi,0) = nb_vertices++;
                add_vertex(Xi,Y,nZ);
              }
              if ((edge&32) && indices2(nxi,yi,1)<0) {
                const float Yi = Y + (isovalue-val5)*dy/(val6-val5);
                indices2(nxi,yi,1) = nb_vertices++;
                add_vertex(nX,Yi,nZ);
              }
              if ((edge&64) && indices2(xi,nyi,0)<0) {
                const float Xi = X + (isovalue-val7)*dx/(val6-val7);
                indices2(xi,nyi,0) = nb_vertices++;
                add_vertex(Xi,nY,nZ);
              }
              if ((edge&128) && indices2(xi,yi,1)<0)  {
                const float Yi = Y + (isovalue-val4)*dy/(val7-val4);
                indices2(xi,yi,1) = nb_vertices++;
                add_vertex(X,Yi,nZ);
              }
              if ((edge&256) && indices1(xi,yi,2)<0) {
                const float Zi = Z+ (isovalue-val0)*dz/(val4-val0);
                indices1(xi,yi,2) = nb_vertices++;
                add_vertex(X,Y,Zi);
              }
              if ((edge&512) && indices1(nxi,yi,2)<0)  {
                const float Zi = Z + (isovalue-val1)*dz/(val5-val1);
                indices1(nxi,yi,2) = nb_vertices++;
                add_vertex(nX,Y,Zi);
              }
              if ((edge&1024) && indices1(nxi,nyi,2)<0) {
                const float Zi = Z + (isovalue-val2)*dz/(val6-val2);
                indices1(nxi,nyi,2) = nb_vertices++;
                add_vertex(nX,nY,Zi);
              }
              if ((edge&2048) && indices1(xi,nyi,2)<0) {
                const float Zi = Z + (isovalue-val3)*dz/(val7-val3);
                indices1(xi,nyi,2) = nb_vertices++;
                add_vertex(X,nY,Zi);
              }

              // Create triangles.
              for (const int *triangle = triangles[configuration]; *triangle!=-1; ) {
                const unsigned int
                  p0 = (unsigned int)*(triangle++),
                  p1 = (unsigned int)*(triangle++),
                  p2 = (unsigned int)*(triangle++);
                const int
                  i0 = _isosurface3d_index(p0,indices1,indices2,xi,yi,nxi,nyi),
                  i1 = _isosurface3d_index(p1,indices1,indices2,xi,yi,nxi,nyi),
                  i2 = _isosurface3d_index(p2,indices1,indices2,xi,yi,nxi,nyi);
                add_triangle(i0,i2,i1);
              }
            }
          }
        }
        cimg::swap(values1,values2);
        cimg::swap(indices1,indices2);
      }
    }

    //! Compute isosurface of a function, as a 3D object \overloading.
    template<typename tf>
    static CImg<floatT> isosurface3d(CImgList<tf>& primitives, const char *const expression, const float isovalue,
                                     const float x0, const float y0, const float z0,
                                     const float x1, const float y1, const float z1,
                                     const int dx=32, const int dy=32, const int dz=32) {
      const _functor3d_expr func(expression);
      return isosurface3d(primitives,func,isovalue,x0,y0,z0,x1,y1,z1,dx,dy,dz);
    }

    template<typename t>
    static int _isosurface3d_index(const unsigned int edge, const CImg<t>& indices1, const CImg<t>& indices2,
                                    const unsigned int x, const unsigned int y,
                                    const unsigned int nx, const unsigned int ny) {
      switch (edge) {
      case 0 : return indices1(x,y,0);
      case 1 : return indices1(nx,y,1);
      case 2 : return indices1(x,ny,0);
      case 3 : return indices1(x,y,1);
      case 4 : return indices2(x,y,0);
      case 5 : return indices2(nx,y,1);
      case 6 : return indices2(x,ny,0);
      case 7 : return indices2(x,y,1);
      case 8 : return indices1(x,y,2);
      case 9 : return indices1(nx,y,2);
      case 10 : return indices1(nx,ny,2);
      case 11 : return indices1(x,ny,2);
      }
      return 0;
    }

    // Define functors for accessing image values (used in previous functions).
    struct _functor2d_int {
      const CImg<T>& ref;
      _functor2d_int(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y) const {
        return (float)ref((int)x,(int)y);
      }
    };

    struct _functor2d_float {
      const CImg<T>& ref;
      _functor2d_float(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y) const {
        return (float)ref._linear_atXY(x,y);
      }
    };

    struct _functor2d_expr {
      _cimg_math_parser *mp;
      ~_functor2d_expr() { mp->end(); delete mp; }
      _functor2d_expr(const char *const expr):mp(0) {
        mp = new _cimg_math_parser(expr,0,CImg<T>::const_empty(),0);
      }
      float operator()(const float x, const float y) const {
        return (float)(*mp)(x,y,0,0);
      }
    };

    struct _functor3d_int {
      const CImg<T>& ref;
      _functor3d_int(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y, const float z) const {
        return (float)ref((int)x,(int)y,(int)z);
      }
    };

    struct _functor3d_float {
      const CImg<T>& ref;
      _functor3d_float(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y, const float z) const {
        return (float)ref._linear_atXYZ(x,y,z);
      }
    };

    struct _functor3d_expr {
      _cimg_math_parser *mp;
      ~_functor3d_expr() { mp->end(); delete mp; }
      _functor3d_expr(const char *const expr):mp(0) {
        mp = new _cimg_math_parser(expr,0,CImg<T>::const_empty(),0);
      }
      float operator()(const float x, const float y, const float z) const {
        return (float)(*mp)(x,y,z,0);
      }
    };

    struct _functor4d_int {
      const CImg<T>& ref;
      _functor4d_int(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
        return (float)ref((int)x,(int)y,(int)z,c);
      }
    };

    struct _functor_isoline3d {
      CImgList<T>& list;
      _functor_isoline3d(CImgList<T>& _list):list(_list) {}
      template<typename t>
      void operator()(const t x, const t y, const t z) { CImg<T>::vector((T)x,(T)y,(T)z).move_to(list); }
      template<typename t>
      void operator()(const t i, const t j) { CImg<T>::vector((T)i,(T)j).move_to(list); }
    };

    struct _functor_isosurface3d {
      CImgList<T>& list;
      _functor_isosurface3d(CImgList<T>& _list):list(_list) {}
      template<typename t>
      void operator()(const t x, const t y, const t z) { CImg<T>::vector((T)x,(T)y,(T)z).move_to(list); }
    };

    //! Compute 3D elevation of a function as a 3D object.
    /**
       \param[out] primitives Primitives data of the resulting 3D object.
       \param func Elevation function. Is of type <tt>float (*func)(const float x,const float y)</tt>.
       \param x0 X-coordinate of the starting point.
       \param y0 Y-coordinate of the starting point.
       \param x1 X-coordinate of the ending point.
       \param y1 Y-coordinate of the ending point.
       \param size_x Resolution of the function along the X-axis.
       \param size_y Resolution of the function along the Y-axis.
    **/
    template<typename tf, typename tfunc>
    static CImg<floatT> elevation3d(CImgList<tf>& primitives, const tfunc& func,
                                    const float x0, const float y0, const float x1, const float y1,
                                    const int size_x=256, const int size_y=256) {
      const float
        nx0 = x0<x1?x0:x1, ny0 = y0<y1?y0:y1,
        nx1 = x0<x1?x1:x0, ny1 = y0<y1?y1:y0;
      const unsigned int
        _nsize_x = (unsigned int)(size_x>=0?size_x:(nx1-nx0)*-size_x/100),
        nsize_x = _nsize_x?_nsize_x:1, nsize_x1 = nsize_x - 1,
        _nsize_y = (unsigned int)(size_y>=0?size_y:(ny1-ny0)*-size_y/100),
        nsize_y = _nsize_y?_nsize_y:1, nsize_y1 = nsize_y - 1;
      if (nsize_x<2 || nsize_y<2)
        throw CImgArgumentException("CImg<%s>::elevation3d(): Invalid specified size (%d,%d).",
                                    pixel_type(),
                                    nsize_x,nsize_y);

      CImg<floatT> vertices(nsize_x*nsize_y,3);
      floatT *ptr_x = vertices.data(0,0), *ptr_y = vertices.data(0,1), *ptr_z = vertices.data(0,2);
      for (unsigned int y = 0; y<nsize_y; ++y) {
        const float Y = ny0 + y*(ny1-ny0)/nsize_y1;
        for (unsigned int x = 0; x<nsize_x; ++x) {
          const float X = nx0 + x*(nx1-nx0)/nsize_x1;
          *(ptr_x++) = (float)x;
          *(ptr_y++) = (float)y;
          *(ptr_z++) = (float)func(X,Y);
        }
      }
      primitives.assign(nsize_x1*nsize_y1,1,4);
      for (unsigned int p = 0, y = 0; y<nsize_y1; ++y) {
        const unsigned int yw = y*nsize_x;
        for (unsigned int x = 0; x<nsize_x1; ++x) {
          const unsigned int xpyw = x + yw, xpyww = xpyw + nsize_x;
          primitives[p++].fill(xpyw,xpyww,xpyww + 1,xpyw + 1);
        }
      }
      return vertices;
    }

    //! Compute 3D elevation of a function, as a 3D object \overloading.
    template<typename tf>
    static CImg<floatT> elevation3d(CImgList<tf>& primitives, const char *const expression,
                                    const float x0, const float y0, const float x1, const float y1,
                                    const int size_x=256, const int size_y=256) {
      const _functor2d_expr func(expression);
      return elevation3d(primitives,func,x0,y0,x1,y1,size_x,size_y);
    }

    //! Generate a 3D box object.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param size_x The width of the box (dimension along the X-axis).
       \param size_y The height of the box (dimension along the Y-axis).
       \param size_z The depth of the box (dimension along the Z-axis).
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       CImgList<unsigned int> faces3d;
       const CImg<float> points3d = CImg<float>::box3d(faces3d,10,20,30);
       CImg<unsigned char>().display_object3d("Box3d",points3d,faces3d);
       \endcode
       \image html ref_box3d.jpg
    **/
    template<typename tf>
    static CImg<floatT> box3d(CImgList<tf>& primitives,
                              const float size_x=200, const float size_y=100, const float size_z=100) {
      primitives.assign(6,1,4,1,1, 0,3,2,1, 4,5,6,7, 0,1,5,4, 3,7,6,2, 0,4,7,3, 1,2,6,5);
      return CImg<floatT>(8,3,1,1,
                          0.,size_x,size_x,    0.,    0.,size_x,size_x,    0.,
                          0.,    0.,size_y,size_y,    0.,    0.,size_y,size_y,
                          0.,    0.,    0.,    0.,size_z,size_z,size_z,size_z);
    }

    //! Generate a 3D cone.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param radius The radius of the cone basis.
       \param size_z The cone's height.
       \param subdivisions The number of basis angular subdivisions.
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       CImgList<unsigned int> faces3d;
       const CImg<float> points3d = CImg<float>::cone3d(faces3d,50);
       CImg<unsigned char>().display_object3d("Cone3d",points3d,faces3d);
       \endcode
       \image html ref_cone3d.jpg
    **/
    template<typename tf>
    static CImg<floatT> cone3d(CImgList<tf>& primitives,
                               const float radius=50, const float size_z=100, const unsigned int subdivisions=24) {
      primitives.assign();
      if (!subdivisions) return CImg<floatT>();
      CImgList<floatT> vertices(2,1,3,1,1,
                                0.,0.,size_z,
                                0.,0.,0.);
      for (float delta = 360.f/subdivisions, angle = 0; angle<360; angle+=delta) {
        const float a = (float)(angle*cimg::PI/180);
        CImg<floatT>::vector((float)(radius*std::cos(a)),(float)(radius*std::sin(a)),0).move_to(vertices);
      }
      const unsigned int nbr = vertices._width - 2;
      for (unsigned int p = 0; p<nbr; ++p) {
        const unsigned int curr = 2 + p, next = 2 + ((p + 1)%nbr);
        CImg<tf>::vector(1,next,curr).move_to(primitives);
        CImg<tf>::vector(0,curr,next).move_to(primitives);
      }
      return vertices>'x';
    }

    //! Generate a 3D cylinder.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param radius The radius of the cylinder basis.
       \param size_z The cylinder's height.
       \param subdivisions The number of basis angular subdivisions.
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       CImgList<unsigned int> faces3d;
       const CImg<float> points3d = CImg<float>::cylinder3d(faces3d,50);
       CImg<unsigned char>().display_object3d("Cylinder3d",points3d,faces3d);
       \endcode
       \image html ref_cylinder3d.jpg
    **/
    template<typename tf>
    static CImg<floatT> cylinder3d(CImgList<tf>& primitives,
                                   const float radius=50, const float size_z=100, const unsigned int subdivisions=24) {
      primitives.assign();
      if (!subdivisions) return CImg<floatT>();
      CImgList<floatT> vertices(2,1,3,1,1,
                                0.,0.,0.,
                                0.,0.,size_z);
      for (float delta = 360.f/subdivisions, angle = 0; angle<360; angle+=delta) {
        const float a = (float)(angle*cimg::PI/180);
        CImg<floatT>::vector((float)(radius*std::cos(a)),(float)(radius*std::sin(a)),0.f).move_to(vertices);
        CImg<floatT>::vector((float)(radius*std::cos(a)),(float)(radius*std::sin(a)),size_z).move_to(vertices);
      }
      const unsigned int nbr = (vertices._width - 2)/2;
      for (unsigned int p = 0; p<nbr; ++p) {
        const unsigned int curr = 2 + 2*p, next = 2 + (2*((p + 1)%nbr));
        CImg<tf>::vector(0,next,curr).move_to(primitives);
        CImg<tf>::vector(1,curr + 1,next + 1).move_to(primitives);
        CImg<tf>::vector(curr,next,next + 1,curr + 1).move_to(primitives);
      }
      return vertices>'x';
    }

    //! Generate a 3D torus.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param radius1 The large radius.
       \param radius2 The small radius.
       \param subdivisions1 The number of angular subdivisions for the large radius.
       \param subdivisions2 The number of angular subdivisions for the small radius.
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       CImgList<unsigned int> faces3d;
       const CImg<float> points3d = CImg<float>::torus3d(faces3d,20,4);
       CImg<unsigned char>().display_object3d("Torus3d",points3d,faces3d);
       \endcode
       \image html ref_torus3d.jpg
    **/
    template<typename tf>
    static CImg<floatT> torus3d(CImgList<tf>& primitives,
                                const float radius1=100, const float radius2=30,
                                const unsigned int subdivisions1=24, const unsigned int subdivisions2=12) {
      primitives.assign();
      if (!subdivisions1 || !subdivisions2) return CImg<floatT>();
      CImgList<floatT> vertices;
      for (unsigned int v = 0; v<subdivisions1; ++v) {
        const float
          beta = (float)(v*2*cimg::PI/subdivisions1),
          xc = radius1*(float)std::cos(beta),
          yc = radius1*(float)std::sin(beta);
        for (unsigned int u = 0; u<subdivisions2; ++u) {
          const float
            alpha = (float)(u*2*cimg::PI/subdivisions2),
            x = xc + radius2*(float)(std::cos(alpha)*std::cos(beta)),
            y = yc + radius2*(float)(std::cos(alpha)*std::sin(beta)),
            z = radius2*(float)std::sin(alpha);
          CImg<floatT>::vector(x,y,z).move_to(vertices);
        }
      }
      for (unsigned int vv = 0; vv<subdivisions1; ++vv) {
        const unsigned int nv = (vv + 1)%subdivisions1;
        for (unsigned int uu = 0; uu<subdivisions2; ++uu) {
          const unsigned int nu = (uu + 1)%subdivisions2, svv = subdivisions2*vv, snv = subdivisions2*nv;
          CImg<tf>::vector(svv + nu,svv + uu,snv + uu,snv + nu).move_to(primitives);
        }
      }
      return vertices>'x';
    }

    //! Generate a 3D XY-plane.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param size_x The width of the plane (dimension along the X-axis).
       \param size_y The height of the plane (dimensions along the Y-axis).
       \param subdivisions_x The number of planar subdivisions along the X-axis.
       \param subdivisions_y The number of planar subdivisions along the Y-axis.
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       CImgList<unsigned int> faces3d;
       const CImg<float> points3d = CImg<float>::plane3d(faces3d,100,50);
       CImg<unsigned char>().display_object3d("Plane3d",points3d,faces3d);
       \endcode
       \image html ref_plane3d.jpg
    **/
    template<typename tf>
    static CImg<floatT> plane3d(CImgList<tf>& primitives,
                                const float size_x=100, const float size_y=100,
                                const unsigned int subdivisions_x=10, const unsigned int subdivisions_y=10) {
      primitives.assign();
      if (!subdivisions_x || !subdivisions_y) return CImg<floatT>();
      CImgList<floatT> vertices;
      const unsigned int w = subdivisions_x + 1, h = subdivisions_y + 1;
      const float fx = (float)size_x/w, fy = (float)size_y/h;
      for (unsigned int y = 0; y<h; ++y) for (unsigned int x = 0; x<w; ++x)
        CImg<floatT>::vector(fx*x,fy*y,0).move_to(vertices);
      for (unsigned int y = 0; y<subdivisions_y; ++y) for (unsigned int x = 0; x<subdivisions_x; ++x) {
        const int off1 = x + y*w, off2 = x + 1 + y*w, off3 = x + 1 + (y + 1)*w, off4 = x + (y + 1)*w;
        CImg<tf>::vector(off1,off4,off3,off2).move_to(primitives);
      }
      return vertices>'x';
    }

    //! Generate a 3D sphere.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param radius The radius of the sphere (dimension along the X-axis).
       \param subdivisions The number of recursive subdivisions from an initial icosahedron.
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       CImgList<unsigned int> faces3d;
       const CImg<float> points3d = CImg<float>::sphere3d(faces3d,100,4);
       CImg<unsigned char>().display_object3d("Sphere3d",points3d,faces3d);
       \endcode
       \image html ref_sphere3d.jpg
    **/
    template<typename tf>
    static CImg<floatT> sphere3d(CImgList<tf>& primitives,
                                 const float radius=50, const unsigned int subdivisions=3) {

      // Create initial icosahedron.
      primitives.assign();
      const double tmp = (1 + std::sqrt(5.f))/2, a = 1./std::sqrt(1 + tmp*tmp), b = tmp*a;
      CImgList<floatT> vertices(12,1,3,1,1, b,a,0., -b,a,0., -b,-a,0., b,-a,0., a,0.,b, a,0.,-b,
                                -a,0.,-b, -a,0.,b, 0.,b,a, 0.,-b,a, 0.,-b,-a, 0.,b,-a);
      primitives.assign(20,1,3,1,1, 4,8,7, 4,7,9, 5,6,11, 5,10,6, 0,4,3, 0,3,5, 2,7,1, 2,1,6,
                        8,0,11, 8,11,1, 9,10,3, 9,2,10, 8,4,0, 11,0,5, 4,9,3,
                        5,3,10, 7,8,1, 6,1,11, 7,2,9, 6,10,2);
      // edge - length/2.
      float he = (float)a;

      // Recurse subdivisions.
      for (unsigned int i = 0; i<subdivisions; ++i) {
        const unsigned int L = primitives._width;
        he/=2;
        const float he2 = he*he;
        for (unsigned int l = 0; l<L; ++l) {
          const unsigned int
            p0 = (unsigned int)primitives(0,0), p1 = (unsigned int)primitives(0,1), p2 = (unsigned int)primitives(0,2);
          const float
            x0 = vertices(p0,0), y0 = vertices(p0,1), z0 = vertices(p0,2),
            x1 = vertices(p1,0), y1 = vertices(p1,1), z1 = vertices(p1,2),
            x2 = vertices(p2,0), y2 = vertices(p2,1), z2 = vertices(p2,2),
            tnx0 = (x0 + x1)/2, tny0 = (y0 + y1)/2, tnz0 = (z0 + z1)/2,
            nn0 = cimg::hypot(tnx0,tny0,tnz0),
            tnx1 = (x0 + x2)/2, tny1 = (y0 + y2)/2, tnz1 = (z0 + z2)/2,
            nn1 = cimg::hypot(tnx1,tny1,tnz1),
            tnx2 = (x1 + x2)/2, tny2 = (y1 + y2)/2, tnz2 = (z1 + z2)/2,
            nn2 = cimg::hypot(tnx2,tny2,tnz2),
            nx0 = tnx0/nn0, ny0 = tny0/nn0, nz0 = tnz0/nn0,
            nx1 = tnx1/nn1, ny1 = tny1/nn1, nz1 = tnz1/nn1,
            nx2 = tnx2/nn2, ny2 = tny2/nn2, nz2 = tnz2/nn2;
          int i0 = -1, i1 = -1, i2 = -1;
          cimglist_for(vertices,p) {
            const float x = (float)vertices(p,0), y = (float)vertices(p,1), z = (float)vertices(p,2);
            if (cimg::sqr(x-nx0) + cimg::sqr(y-ny0) + cimg::sqr(z-nz0)<he2) i0 = p;
            if (cimg::sqr(x-nx1) + cimg::sqr(y-ny1) + cimg::sqr(z-nz1)<he2) i1 = p;
            if (cimg::sqr(x-nx2) + cimg::sqr(y-ny2) + cimg::sqr(z-nz2)<he2) i2 = p;
          }
          if (i0<0) { CImg<floatT>::vector(nx0,ny0,nz0).move_to(vertices); i0 = vertices.width() - 1; }
          if (i1<0) { CImg<floatT>::vector(nx1,ny1,nz1).move_to(vertices); i1 = vertices.width() - 1; }
          if (i2<0) { CImg<floatT>::vector(nx2,ny2,nz2).move_to(vertices); i2 = vertices.width() - 1; }
          primitives.remove(0);
          CImg<tf>::vector(p0,i0,i1).move_to(primitives);
          CImg<tf>::vector((tf)i0,(tf)p1,(tf)i2).move_to(primitives);
          CImg<tf>::vector((tf)i1,(tf)i2,(tf)p2).move_to(primitives);
          CImg<tf>::vector((tf)i1,(tf)i0,(tf)i2).move_to(primitives);
        }
      }
      return (vertices>'x')*=radius;
    }

    //! Generate a 3D ellipsoid.
    /**
       \param[out] primitives The returned list of the 3D object primitives
                              (template type \e tf should be at least \e unsigned \e int).
       \param tensor The tensor which gives the shape and size of the ellipsoid.
       \param subdivisions The number of recursive subdivisions from an initial stretched icosahedron.
       \return The N vertices (xi,yi,zi) of the 3D object as a Nx3 CImg<float> image (0<=i<=N - 1).
       \par Example
       \code
       CImgList<unsigned int> faces3d;
       const CImg<float> tensor = CImg<float>::diagonal(10,7,3),
                         points3d = CImg<float>::ellipsoid3d(faces3d,tensor,4);
       CImg<unsigned char>().display_object3d("Ellipsoid3d",points3d,faces3d);
       \endcode
       \image html ref_ellipsoid3d.jpg
    **/
    template<typename tf, typename t>
    static CImg<floatT> ellipsoid3d(CImgList<tf>& primitives,
                                    const CImg<t>& tensor, const unsigned int subdivisions=3) {
      primitives.assign();
      if (!subdivisions) return CImg<floatT>();
      CImg<floatT> S, V;
      tensor.symmetric_eigen(S,V);
      const float orient =
        (V(0,1)*V(1,2) - V(0,2)*V(1,1))*V(2,0) +
        (V(0,2)*V(1,0) - V(0,0)*V(1,2))*V(2,1) +
        (V(0,0)*V(1,1) - V(0,1)*V(1,0))*V(2,2);
      if (orient<0) { V(2,0) = -V(2,0); V(2,1) = -V(2,1); V(2,2) = -V(2,2); }
      const float l0 = S[0], l1 = S[1], l2 = S[2];
      CImg<floatT> vertices = sphere3d(primitives,1.,subdivisions);
      vertices.get_shared_row(0)*=l0;
      vertices.get_shared_row(1)*=l1;
      vertices.get_shared_row(2)*=l2;
      return V*vertices;
    }

    //! Convert 3D object into a CImg3d representation.
    /**
       \param primitives Primitives data of the 3D object.
       \param colors Colors data of the 3D object.
       \param opacities Opacities data of the 3D object.
       \param full_check Tells if full checking of the 3D object must be performed.
    **/
    template<typename tp, typename tc, typename to>
    CImg<T>& object3dtoCImg3d(const CImgList<tp>& primitives,
                              const CImgList<tc>& colors,
                              const to& opacities,
                              const bool full_check=true) {
      return get_object3dtoCImg3d(primitives,colors,opacities,full_check).move_to(*this);
    }

    //! Convert 3D object into a CImg3d representation \overloading.
    template<typename tp, typename tc>
    CImg<T>& object3dtoCImg3d(const CImgList<tp>& primitives,
                              const CImgList<tc>& colors,
                              const bool full_check=true) {
      return get_object3dtoCImg3d(primitives,colors,full_check).move_to(*this);
    }

    //! Convert 3D object into a CImg3d representation \overloading.
    template<typename tp>
    CImg<T>& object3dtoCImg3d(const CImgList<tp>& primitives,
                              const bool full_check=true) {
      return get_object3dtoCImg3d(primitives,full_check).move_to(*this);
    }

    //! Convert 3D object into a CImg3d representation \overloading.
    CImg<T>& object3dtoCImg3d(const bool full_check=true) {
      return get_object3dtoCImg3d(full_check).move_to(*this);
    }

    //! Convert 3D object into a CImg3d representation \newinstance.
    template<typename tp, typename tc, typename to>
    CImg<floatT> get_object3dtoCImg3d(const CImgList<tp>& primitives,
                                      const CImgList<tc>& colors,
                                      const to& opacities,
                                      const bool full_check=true) const {
      CImg<charT> error_message(1024);
      if (!is_object3d(primitives,colors,opacities,full_check,error_message))
        throw CImgInstanceException(_cimg_instance
                                    "object3dtoCImg3d(): Invalid specified 3D object (%u,%u) (%s).",
                                    cimg_instance,_width,primitives._width,error_message.data());
      CImg<floatT> res(1,_size_object3dtoCImg3d(primitives,colors,opacities));
      float *ptrd = res._data;

      // Put magick number.
      *(ptrd++) = 'C' + 0.5f; *(ptrd++) = 'I' + 0.5f; *(ptrd++) = 'm' + 0.5f;
      *(ptrd++) = 'g' + 0.5f; *(ptrd++) = '3' + 0.5f; *(ptrd++) = 'd' + 0.5f;

      // Put number of vertices and primitives.
      *(ptrd++) = cimg::uint2float(_width,0.0f);
      *(ptrd++) = cimg::uint2float(primitives._width,0.0f);

      // Put vertex data.
      if (is_empty() || !primitives) return res;
      const T *ptrx = data(0,0), *ptry = data(0,1), *ptrz = data(0,2);
      cimg_forX(*this,p) {
        *(ptrd++) = (float)*(ptrx++);
        *(ptrd++) = (float)*(ptry++);
        *(ptrd++) = (float)*(ptrz++);
      }

      // Put primitive data.
      cimglist_for(primitives,p) {
        *(ptrd++) = (float)primitives[p].size();
        const tp *ptrp = primitives[p]._data;
        cimg_foroff(primitives[p],i) *(ptrd++) = cimg::uint2float((unsigned int)*(ptrp++),0.0f);
      }

      // Put color/texture data.
      const unsigned int csiz = std::min(colors._width,primitives._width);
      for (int c = 0; c<(int)csiz; ++c) {
        const CImg<tc>& color = colors[c];
        const tc *ptrc = color._data;
        if (color.size()==3) { *(ptrd++) = (float)*(ptrc++); *(ptrd++) = (float)*(ptrc++); *(ptrd++) = (float)*ptrc; }
        else {
          *(ptrd++) = -128.f;
          int shared_ind = -1;
          if (color.is_shared()) for (int i = 0; i<c; ++i) if (ptrc==colors[i]._data) { shared_ind = i; break; }
          if (shared_ind<0) {
            *(ptrd++) = (float)color._width;
            *(ptrd++) = (float)color._height;
            *(ptrd++) = (float)color._spectrum;
            cimg_foroff(color,l) *(ptrd++) = (float)*(ptrc++);
          } else {
            *(ptrd++) = cimg::uint2float((unsigned int)shared_ind,0.0f);
            *(ptrd++) = 0;
            *(ptrd++) = 0;
          }
        }
      }
      const int csiz2 = primitives.width() - colors.width();
      for (int c = 0; c<csiz2; ++c) { *(ptrd++) = 200.f; *(ptrd++) = 200.f; *(ptrd++) = 200.f; }

      // Put opacity data.
      ptrd = _object3dtoCImg3d(opacities,ptrd);
      const float *ptre = res.end();
      while (ptrd<ptre) *(ptrd++) = 1.f;
      return res;
    }

    template<typename to>
    float* _object3dtoCImg3d(const CImgList<to>& opacities, float *ptrd) const {
      cimglist_for(opacities,o) {
        const CImg<to>& opacity = opacities[o];
        const to *ptro = opacity._data;
        if (opacity.size()==1) *(ptrd++) = (float)*ptro;
        else {
          *(ptrd++) = -128.f;
          int shared_ind = -1;
          if (opacity.is_shared()) for (int i = 0; i<o; ++i) if (ptro==opacities[i]._data) { shared_ind = i; break; }
          if (shared_ind<0) {
            *(ptrd++) = (float)opacity._width;
            *(ptrd++) = (float)opacity._height;
            *(ptrd++) = (float)opacity._spectrum;
            cimg_foroff(opacity,l) *(ptrd++) = (float)*(ptro++);
          } else {
            *(ptrd++) = cimg::uint2float((unsigned int)shared_ind,0.0f);
            *(ptrd++) = 0;
            *(ptrd++) = 0;
          }
        }
      }
      return ptrd;
    }

    template<typename to>
    float* _object3dtoCImg3d(const CImg<to>& opacities, float *ptrd) const {
      const to *ptro = opacities._data;
      cimg_foroff(opacities,o) *(ptrd++) = (float)*(ptro++);
      return ptrd;
    }

    template<typename tp, typename tc, typename to>
    unsigned int _size_object3dtoCImg3d(const CImgList<tp>& primitives,
                                        const CImgList<tc>& colors,
                                        const CImgList<to>& opacities) const {
      unsigned int siz = 8U + 3*_width;
      cimglist_for(primitives,p) siz+=primitives[p].size() + 1;
      for (int c = std::min(primitives.width(),colors.width()) - 1; c>=0; --c) {
        if (colors[c].is_shared()) siz+=4;
        else { const unsigned int csiz = colors[c].size(); siz+=(csiz!=3)?4 + csiz:3; }
      }
      if (colors._width<primitives._width) siz+=3*(primitives._width - colors._width);
      cimglist_for(opacities,o) {
        if (opacities[o].is_shared()) siz+=4;
        else { const unsigned int osiz = opacities[o].size(); siz+=(osiz!=1)?4 + osiz:1; }
      }
      siz+=primitives._width - opacities._width;
      return siz;
    }

    template<typename tp, typename tc, typename to>
    unsigned int _size_object3dtoCImg3d(const CImgList<tp>& primitives,
                                        const CImgList<tc>& colors,
                                        const CImg<to>& opacities) const {
      unsigned int siz = 8U + 3*_width;
      cimglist_for(primitives,p) siz+=primitives[p].size() + 1;
      for (int c = std::min(primitives.width(),colors.width()) - 1; c>=0; --c) {
        const unsigned int csiz = colors[c].size(); siz+=(csiz!=3)?4 + csiz:3;
      }
      if (colors._width<primitives._width) siz+=3*(primitives._width - colors._width);
      siz+=primitives.size();
      cimg::unused(opacities);
      return siz;
    }

    //! Convert 3D object into a CImg3d representation \overloading.
    template<typename tp, typename tc>
    CImg<floatT> get_object3dtoCImg3d(const CImgList<tp>& primitives,
                                      const CImgList<tc>& colors,
                                      const bool full_check=true) const {
      CImgList<T> opacities;
      return get_object3dtoCImg3d(primitives,colors,opacities,full_check);
    }

    //! Convert 3D object into a CImg3d representation \overloading.
    template<typename tp>
    CImg<floatT> get_object3dtoCImg3d(const CImgList<tp>& primitives,
                                      const bool full_check=true) const {
      CImgList<T> colors, opacities;
      return get_object3dtoCImg3d(primitives,colors,opacities,full_check);
    }

    //! Convert 3D object into a CImg3d representation \overloading.
    CImg<floatT> get_object3dtoCImg3d(const bool full_check=true) const {
      CImgList<T> opacities, colors;
      CImgList<uintT> primitives(width(),1,1,1,1);
      cimglist_for(primitives,p) primitives(p,0) = p;
      return get_object3dtoCImg3d(primitives,colors,opacities,full_check);
    }

    //! Convert CImg3d representation into a 3D object.
    /**
       \param[out] primitives Primitives data of the 3D object.
       \param[out] colors Colors data of the 3D object.
       \param[out] opacities Opacities data of the 3D object.
       \param full_check Tells if full checking of the 3D object must be performed.
    **/
    template<typename tp, typename tc, typename to>
    CImg<T>& CImg3dtoobject3d(CImgList<tp>& primitives,
                              CImgList<tc>& colors,
                              CImgList<to>& opacities,
                              const bool full_check=true) {
      return get_CImg3dtoobject3d(primitives,colors,opacities,full_check).move_to(*this);
    }

    //! Convert CImg3d representation into a 3D object \newinstance.
    template<typename tp, typename tc, typename to>
    CImg<T> get_CImg3dtoobject3d(CImgList<tp>& primitives,
                                 CImgList<tc>& colors,
                                 CImgList<to>& opacities,
                                 const bool full_check=true) const {
      CImg<charT> error_message(1024);
      if (!is_CImg3d(full_check,error_message))
        throw CImgInstanceException(_cimg_instance
                                    "CImg3dtoobject3d(): image instance is not a CImg3d (%s).",
                                    cimg_instance,error_message.data());
      const T *ptrs = _data + 6;
      const unsigned int
        nb_points = cimg::float2uint(*(ptrs++)),
        nb_primitives = cimg::float2uint(*(ptrs++));
      const CImg<T> points = CImg<T>(ptrs,3,nb_points,1,1,true).get_transpose();
      ptrs+=3*nb_points;
      primitives.assign(nb_primitives);
      cimglist_for(primitives,p) {
        const unsigned int nb_inds = (unsigned int)*(ptrs++);
        primitives[p].assign(1,nb_inds);
        tp *ptrp = primitives[p]._data;
        for (unsigned int i = 0; i<nb_inds; ++i)
          *(ptrp++) = (tp)cimg::float2uint(*(ptrs++));
      }
      colors.assign(nb_primitives);

      cimglist_for(colors,c) {
        if (*ptrs==(T)-128) {
          ++ptrs;
          const unsigned int
            w = (unsigned int)cimg::float2uint(*(ptrs++)),
            h = (unsigned int)*(ptrs++),
            s = (unsigned int)*(ptrs++);
          if (!h && !s) colors[c].assign(colors[w],true);
          else { colors[c].assign(ptrs,w,h,1,s,false); ptrs+=(ulongT)w*h*s; }
        } else { colors[c].assign(ptrs,1,1,1,3,false); ptrs+=3; }
      }
      opacities.assign(nb_primitives);
      cimglist_for(opacities,o) {
        if (*ptrs==(T)-128) {
          ++ptrs;
          const unsigned int
            w = (unsigned int)cimg::float2uint(*(ptrs++)),
            h = (unsigned int)*(ptrs++),
            s = (unsigned int)*(ptrs++);
          if (!h && !s) opacities[o].assign(opacities[w],true);
          else { opacities[o].assign(ptrs,w,h,1,s,false); ptrs+=(ulongT)w*h*s; }
        } else opacities[o].assign(1,1,1,1,*(ptrs++));
      }
      return points;
    }

    //@}
#endif
