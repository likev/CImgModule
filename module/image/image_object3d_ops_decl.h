#ifndef CIMG_MODULE_IMAGE_OBJECT3D_OPS_DECL_H
#define CIMG_MODULE_IMAGE_OBJECT3D_OPS_DECL_H

    template<typename tp, typename tc, typename to>
    bool is_object3d(const CImgList<tp>& primitives,
                     const CImgList<tc>& colors,
                     const to& opacities,
                     const bool full_check=true,
                     char *const error_message=0) const;

    bool is_CImg3d(const bool full_check=true, char *const error_message=0) const;

    static bool _is_CImg3d(const T val, const char c);

#endif
