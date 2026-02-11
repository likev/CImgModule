#ifndef CIMG_MODULE_IMAGE_CLASS_BODY_H
#define CIMG_MODULE_IMAGE_CLASS_BODY_H

    // Return 'dx*dy*dz*dc' as a 'size_t' and check no overflow occurs.
    static size_t safe_size(const unsigned int dx, const unsigned int dy,
                            const unsigned int dz, const unsigned int dc) {
      if (!(dx && dy && dz && dc)) return 0;
      size_t siz = (size_t)dx, osiz = siz;
      if ((dy==1 || (siz*=dy)>osiz) &&
          ((osiz = siz), dz==1 || (siz*=dz)>osiz) &&
          ((osiz = siz), dc==1 || (siz*=dc)>osiz) &&
          ((osiz = siz), sizeof(T)==1 || (siz*sizeof(T))>osiz)) {
        if (siz>cimg_max_buf_size){
          throw CImgArgumentException("CImg<%s>::safe_size(): Specified size (%u,%u,%u,%u) exceeds maximum "
                                      "allowed buffer size of %lu ",
                                      pixel_type(),dx,dy,dz,dc,cimg_max_buf_size);
        }
        return siz;
      }
      throw CImgArgumentException("CImg<%s>::safe_size(): Specified size (%u,%u,%u,%u) overflows 'size_t'.",
                                  pixel_type(),dx,dy,dz,dc);
    }

    //@}
    //---------------------------
    //
    //! \name Plugins
    //@{
    //---------------------------
#ifdef cimg_plugin
#include cimg_plugin
#endif
#ifdef cimg_plugin1
#include cimg_plugin1
#endif
#ifdef cimg_plugin2
#include cimg_plugin2
#endif
#ifdef cimg_plugin3
#include cimg_plugin3
#endif
#ifdef cimg_plugin4
#include cimg_plugin4
#endif
#ifdef cimg_plugin5
#include cimg_plugin5
#endif
#ifdef cimg_plugin6
#include cimg_plugin6
#endif
#ifdef cimg_plugin7
#include cimg_plugin7
#endif
#ifdef cimg_plugin8
#include cimg_plugin8
#endif

    //@}
    #include "module/image/image_ops_basic.h"
    #include "module/image/image_ops_advanced.h"
    #include "module/image/image_pixels.h"
    #include "module/math/math_parser.h"
    #include "module/image/image_body_arithmetic.h"

    #include "module/image/image_body_analysis.h"

    #include "module/image/image_body_pointwise.h"

    #include "module/image/image_body_colors.h"

    #include "module/image/image_body_geometry.h"

    #include "module/image/image_body_filters.h"

    #include "module/image/image_body_object3d.h"

    #include "module/image/image_body_draw.h"

    #include "module/image/image_body_io.h"


#endif
