#ifndef CIMG_MODULE_IMAGE_CLASS_H
#define CIMG_MODULE_IMAGE_CLASS_H

  template<typename T>
  struct CImg {

    unsigned int _width, _height, _depth, _spectrum;
    bool _is_shared;
    T *_data;

    #include "module/image/image_class_decl.h"

    #include "module/image/image_class_body.h"
  }; // struct CImg { ...

#endif
