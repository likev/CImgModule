#ifndef CIMG_MODULE_IMAGE_VALUE_OPS_DECL_H
#define CIMG_MODULE_IMAGE_VALUE_OPS_DECL_H

    CImg<T>& set_linear_atX(const T& value, const float fx, const int y=0, const int z=0, const int c=0,
                            const bool is_added=false);
    CImg<T>& set_linear_atXY(const T& value, const float fx, const float fy=0, const int z=0, const int c=0,
                             const bool is_added=false);
    CImg<T>& set_linear_atXYZ(const T& value, const float fx, const float fy=0, const float fz=0, const int c=0,
                              const bool is_added=false);
    CImg<charT> value_string(const char separator=',', const unsigned int max_size=0,
                             const char *const format=0) const;

#endif
