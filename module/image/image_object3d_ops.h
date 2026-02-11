#ifndef CIMG_MODULE_IMAGE_OBJECT3D_OPS_H
#define CIMG_MODULE_IMAGE_OBJECT3D_OPS_H

    //! Test if the set {\c *this,\c primitives,\c colors,\c opacities} defines a valid 3D object.
    /**
       Return \c true is the 3D object represented by the set {\c *this,\c primitives,\c colors,\c opacities} defines a
       valid 3D object, and \c false otherwise. The vertex coordinates are defined by the instance image.
       \param primitives List of primitives of the 3D object.
       \param colors List of colors of the 3D object.
       \param opacities List (or image) of opacities of the 3D object.
       \param full_check Tells if full checking of the 3D object must be performed.
       \param[out] error_message C-string to contain the error message, if the test does not succeed
                   (at least 256 bytes).
       \note
       - Set \c full_checking to \c false to speed-up the 3D object checking. In this case, only the size of
         each 3D object component is checked.
       - Size of the string \c error_message should be at least 128-bytes long, to be able to contain the error message.
    **/
    template<typename tp, typename tc, typename to>
    bool is_object3d(const CImgList<tp>& primitives,
                     const CImgList<tc>& colors,
                     const to& opacities,
                     const bool full_check=true,
                     char *const error_message=0) const {
      if (error_message) *error_message = 0;

      // Check consistency for the particular case of an empty 3D object.
      if (is_empty()) {
        if (primitives || colors || opacities) {
          if (error_message) cimg_snprintf(error_message,256,
                                          "3D object (%u,%u) defines no vertices but %u primitives, "
                                          "%u colors and %lu opacities",
                                          _width,primitives._width,primitives._width,
                                          colors._width,(unsigned long)opacities.size());
          return false;
        }
        return true;
      }

      // Check consistency of vertices.
      if (_height!=3 || _depth>1 || _spectrum>1) { // Check vertices dimensions
        if (error_message) cimg_snprintf(error_message,256,
                                        "3D object (%u,%u) has invalid vertex dimensions (%u,%u,%u,%u)",
                                        _width,primitives._width,_width,_height,_depth,_spectrum);
        return false;
      }
      if (colors._width>primitives._width + 1) {
        if (error_message) cimg_snprintf(error_message,256,
                                        "3D object (%u,%u) defines %u colors",
                                        _width,primitives._width,colors._width);
        return false;
      }
      if (opacities.size()>primitives._width) {
        if (error_message) cimg_snprintf(error_message,256,
                                        "3D object (%u,%u) defines %lu opacities",
                                        _width,primitives._width,(unsigned long)opacities.size());
        return false;
      }
      if (!full_check) return true;

      // Check consistency of primitives.
      cimglist_for(primitives,l) {
        const CImg<tp>& primitive = primitives[l];
        const unsigned int psiz = (unsigned int)primitive.size();
        switch (psiz) {
        case 1 : { // Point
          const unsigned int i0 = (unsigned int)primitive(0);
          if (i0>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex index %u in "
                                            "point primitive [%u]",
                                            _width,primitives._width,i0,l);
            return false;
          }
        } break;
        case 5 : { // Sphere
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          if (i0>=_width || i1>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                            "sphere primitive [%u]",
                                            _width,primitives._width,i0,i1,l);
            return false;
          }
        } break;
        case 2 : case 6 : { // Segment
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          if (i0>=_width || i1>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                            "segment primitive [%u]",
                                            _width,primitives._width,i0,i1,l);
            return false;
          }
        } break;
        case 3 : case 9 : { // Triangle
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2);
          if (i0>=_width || i1>=_width || i2>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex indices (%u,%u,%u) in "
                                            "triangle primitive [%u]",
                                            _width,primitives._width,i0,i1,i2,l);
            return false;
          }
        } break;
        case 4 : case 12 : { // Quadrangle
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2),
            i3 = (unsigned int)primitive(3);
          if (i0>=_width || i1>=_width || i2>=_width || i3>=_width) {
            if (error_message) cimg_snprintf(error_message,256,
                                            "3D object (%u,%u) refers to invalid vertex indices (%u,%u,%u,%u) in "
                                            "quadrangle primitive [%u]",
                                            _width,primitives._width,i0,i1,i2,i3,l);
            return false;
          }
        } break;
        default :
          if (error_message) cimg_snprintf(error_message,256,
                                          "3D object (%u,%u) defines an invalid primitive [%u] of size %u",
                                          _width,primitives._width,l,(unsigned int)psiz);
          return false;
        }
      }

      // Check consistency of colors.
      cimglist_for(colors,c) {
        const CImg<tc>& color = colors[c];
        if (!color) {
          if (error_message) cimg_snprintf(error_message,256,
                                          "3D object (%u,%u) defines no color for primitive [%u]",
                                          _width,primitives._width,c);
          return false;
        }
      }

      // Check consistency of light texture.
      if (colors._width>primitives._width) {
        const CImg<tc> &light = colors.back();
        if (!light || light._depth>1) {
          if (error_message) cimg_snprintf(error_message,256,
                                          "3D object (%u,%u) defines an invalid light texture (%u,%u,%u,%u)",
                                          _width,primitives._width,light._width,
                                          light._height,light._depth,light._spectrum);
          return false;
        }
      }

      return true;
    }

    //! Test if image instance represents a valid serialization of a 3D object.
    /**
       Return \c true if the image instance represents a valid serialization of a 3D object, and \c false otherwise.
       \param full_check Tells if full checking of the instance must be performed.
       \param[out] error_message C-string to contain the error message, if the test does not succeed.
       \note
       - Set \c full_check to \c false to speed-up the 3D object checking. In this case, only the size of
         each 3D object component is checked.
       - Size of the string \c error_message should be at least 256-bytes long, to be able to contain the error message.
    **/
    bool is_CImg3d(const bool full_check=true, char *const error_message=0) const {
      if (error_message) *error_message = 0;

      // Check instance dimension and header.
      if (_width!=1 || _height<8 || _depth!=1 || _spectrum!=1) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d has invalid dimensions (%u,%u,%u,%u)",
                                         _width,_height,_depth,_spectrum);
        return false;
      }
      const T *ptrs = _data, *const ptre = end();
      if (!_is_CImg3d(*(ptrs++),'C') || !_is_CImg3d(*(ptrs++),'I') || !_is_CImg3d(*(ptrs++),'m') ||
          !_is_CImg3d(*(ptrs++),'g') || !_is_CImg3d(*(ptrs++),'3') || !_is_CImg3d(*(ptrs++),'d')) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d header not found");
        return false;
      }
      if (!cimg::type<T>::is_finite(*ptrs) || !cimg::type<T>::is_finite(ptrs[1])) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "Specified numbers of vertices/primitives (%g/%g) are invalid",
                                         (double)*ptrs,(double)ptrs[1]);
        return false;
      }

      const unsigned int
        nb_points = cimg::float2uint(*(ptrs++)),
        nb_primitives = cimg::float2uint(*(ptrs++));

      // Check consistency of number of vertices / primitives.
      if (!full_check) {
        const ulongT minimal_size = 8UL + 3*nb_points + 6*nb_primitives;
        if (_data + minimal_size>ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) has only %lu values, "
                                           "while at least %lu values were expected",
                                           nb_points,nb_primitives,(unsigned long)size(),(unsigned long)minimal_size);
          return false;
        }
      }

      // Check consistency of vertex data.
      if (!nb_points) {
        if (nb_primitives) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) defines no vertices but %u primitives",
                                           nb_points,nb_primitives,nb_primitives);
          return false;
        }
        if (ptrs!=ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) is an empty object but contains %u value%s "
                                           "more than expected",
                                           nb_points,nb_primitives,(unsigned int)(ptre - ptrs),(ptre - ptrs)>1?"s":"");
          return false;
        }
        return true;
      }
      if (ptrs + 3*nb_points>ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) defines only %u vertices data",
                                         nb_points,nb_primitives,(unsigned int)(ptre - ptrs)/3);
        return false;
      }
      ptrs+=3*nb_points;

      // Check consistency of primitive data.
      if (ptrs==ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) defines %u vertices but no primitive",
                                         nb_points,nb_primitives,nb_points);
        return false;
      }

      if (!full_check) return true;

      for (unsigned int p = 0; p<nb_primitives; ++p) {
        const unsigned int nb_inds = (unsigned int)*(ptrs++);
        switch (nb_inds) {
        case 1 : { // Point
          const unsigned int i0 = cimg::float2uint(*(ptrs++));
          if (i0>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex index %u in point primitive [%u]",
                                             nb_points,nb_primitives,i0,p);
            return false;
          }
        } break;
        case 5 : { // Sphere
          const unsigned int
            i0 = cimg::float2uint(*(ptrs++)),
            i1 = cimg::float2uint(*(ptrs++));
          ptrs+=3;
          if (i0>=nb_points || i1>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                             "sphere primitive [%u]",
                                             nb_points,nb_primitives,i0,i1,p);
            return false;
          }
        } break;
        case 2 : case 6 : { // Segment
          const unsigned int
            i0 = cimg::float2uint(*(ptrs++)),
            i1 = cimg::float2uint(*(ptrs++));
          if (nb_inds==6) ptrs+=4;
          if (i0>=nb_points || i1>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u) in "
                                             "segment primitive [%u]",
                                             nb_points,nb_primitives,i0,i1,p);
            return false;
          }
        } break;
        case 3 : case 9 : { // Triangle
          const unsigned int
            i0 = cimg::float2uint(*(ptrs++)),
            i1 = cimg::float2uint(*(ptrs++)),
            i2 = cimg::float2uint(*(ptrs++));
          if (nb_inds==9) ptrs+=6;
          if (i0>=nb_points || i1>=nb_points || i2>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u,%u) in "
                                             "triangle primitive [%u]",
                                             nb_points,nb_primitives,i0,i1,i2,p);
            return false;
          }
        } break;
        case 4 : case 12 : { // Quadrangle
          const unsigned int
            i0 = cimg::float2uint(*(ptrs++)),
            i1 = cimg::float2uint(*(ptrs++)),
            i2 = cimg::float2uint(*(ptrs++)),
            i3 = cimg::float2uint(*(ptrs++));
          if (nb_inds==12) ptrs+=8;
          if (i0>=nb_points || i1>=nb_points || i2>=nb_points || i3>=nb_points) {
            if (error_message) cimg_snprintf(error_message,256,
                                             "CImg3d (%u,%u) refers to invalid vertex indices (%u,%u,%u,%u) in "
                                             "quadrangle primitive [%u]",
                                             nb_points,nb_primitives,i0,i1,i2,i3,p);
            return false;
          }
        } break;
        default :
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) defines an invalid primitive [%u] of size %u",
                                           nb_points,nb_primitives,p,nb_inds);
          return false;
        }
        if (ptrs>ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) has incomplete primitive data for primitive [%u], "
                                           "%u values missing",
                                           nb_points,nb_primitives,p,(unsigned int)(ptrs - ptre));
          return false;
        }
      }

      // Check consistency of color data.
      if (ptrs==ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) defines no color/texture data",
                                         nb_points,nb_primitives);
        return false;
      }
      for (unsigned int c = 0; c<nb_primitives; ++c) {
        if (*(ptrs++)!=(T)-128) ptrs+=2;
        else if ((ptrs+=3)<ptre) {
          const unsigned int
            w = (unsigned int)cimg::float2uint(*(ptrs - 3)),
            h = (unsigned int)*(ptrs - 2),
            s = (unsigned int)*(ptrs - 1);
          if (!h && !s) {
            if (w>=c) {
              if (error_message) cimg_snprintf(error_message,256,
                                               "CImg3d (%u,%u) refers to invalid shared sprite/texture index %u "
                                               "for primitive [%u]",
                                               nb_points,nb_primitives,w,c);
              return false;
            }
          } else ptrs+=w*h*s;
        }
        if (ptrs>ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) has incomplete color/texture data for primitive [%u], "
                                           "%u values missing",
                                           nb_points,nb_primitives,c,(unsigned int)(ptrs - ptre));
          return false;
        }
      }

      // Check consistency of opacity data.
      if (ptrs==ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) defines no opacity data",
                                         nb_points,nb_primitives);
        return false;
      }
      for (unsigned int o = 0; o<nb_primitives; ++o) {
        if (*(ptrs++)==(T)-128 && (ptrs+=3)<ptre) {
          const unsigned int
            w = (unsigned int)cimg::float2uint(*(ptrs - 3)),
            h = (unsigned int)*(ptrs - 2),
            s = (unsigned int)*(ptrs - 1);
          if (!h && !s) {
            if (w>=o) {
              if (error_message) cimg_snprintf(error_message,256,
                                               "CImg3d (%u,%u) refers to invalid shared opacity index %u "
                                               "for primitive [%u]",
                                               nb_points,nb_primitives,w,o);
              return false;
            }
          } else ptrs+=w*h*s;
        }
        if (ptrs>ptre) {
          if (error_message) cimg_snprintf(error_message,256,
                                           "CImg3d (%u,%u) has incomplete opacity data for primitive [%u]",
                                           nb_points,nb_primitives,o);
          return false;
        }
      }

      // Check end of data.
      if (ptrs<ptre) {
        if (error_message) cimg_snprintf(error_message,256,
                                         "CImg3d (%u,%u) contains %u value%s more than expected",
                                         nb_points,nb_primitives,(unsigned int)(ptre - ptrs),(ptre - ptrs)>1?"s":"");
        return false;
      }
      return true;
    }

    static bool _is_CImg3d(const T val, const char c) {
      return val>=(T)c && val<(T)(c + 1);
    }

    //@}
    //-------------------------------------


#endif
