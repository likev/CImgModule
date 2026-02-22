#ifndef CIMG_MODULE_IMAGE_CHECKS_DECL_H
#define CIMG_MODULE_IMAGE_CHECKS_DECL_H

    bool is_shared() const;
    bool is_empty() const;
    bool is_inf() const;
    bool is_nan() const;

    bool is_sameX(const unsigned int size_x) const;
    template<typename t>
    bool is_sameX(const CImg<t>& img) const;
    bool is_sameX(const CImgDisplay& disp) const;

    bool is_sameY(const unsigned int size_y) const;
    template<typename t>
    bool is_sameY(const CImg<t>& img) const;
    bool is_sameY(const CImgDisplay& disp) const;

    bool is_sameZ(const unsigned int size_z) const;
    template<typename t>
    bool is_sameZ(const CImg<t>& img) const;

    bool is_sameC(const unsigned int size_c) const;
    template<typename t>
    bool is_sameC(const CImg<t>& img) const;

    bool is_sameXY(const unsigned int size_x, const unsigned int size_y) const;
    template<typename t>
    bool is_sameXY(const CImg<t>& img) const;
    bool is_sameXY(const CImgDisplay& disp) const;

    bool is_sameXZ(const unsigned int size_x, const unsigned int size_z) const;
    template<typename t>
    bool is_sameXZ(const CImg<t>& img) const;

    bool is_sameXC(const unsigned int size_x, const unsigned int size_c) const;
    template<typename t>
    bool is_sameXC(const CImg<t>& img) const;

    bool is_sameYZ(const unsigned int size_y, const unsigned int size_z) const;
    template<typename t>
    bool is_sameYZ(const CImg<t>& img) const;

    bool is_sameYC(const unsigned int size_y, const unsigned int size_c) const;
    template<typename t>
    bool is_sameYC(const CImg<t>& img) const;

    bool is_sameZC(const unsigned int size_z, const unsigned int size_c) const;
    template<typename t>
    bool is_sameZC(const CImg<t>& img) const;

    bool is_sameXYZ(const unsigned int size_x, const unsigned int size_y, const unsigned int size_z) const;
    template<typename t>
    bool is_sameXYZ(const CImg<t>& img) const;

    bool is_sameXYC(const unsigned int size_x, const unsigned int size_y, const unsigned int size_c) const;
    template<typename t>
    bool is_sameXYC(const CImg<t>& img) const;

    bool is_sameXZC(const unsigned int size_x, const unsigned int size_z, const unsigned int size_c) const;
    template<typename t>
    bool is_sameXZC(const CImg<t>& img) const;

    bool is_sameYZC(const unsigned int size_y, const unsigned int size_z, const unsigned int size_c) const;
    template<typename t>
    bool is_sameYZC(const CImg<t>& img) const;

    bool is_sameXYZC(const unsigned int size_x, const unsigned int size_y,
                     const unsigned int size_z, const unsigned int size_c) const;
    template<typename t>
    bool is_sameXYZC(const CImg<t>& img) const;

    bool containsXYZC(const int x, const int y=0, const int z=0, const int c=0) const;

    template<typename t>
    bool contains(const T& pixel, t& x, t& y, t& z, t& c) const;
    template<typename t>
    bool contains(const T& pixel, t& x, t& y, t& z) const;
    template<typename t>
    bool contains(const T& pixel, t& x, t& y) const;
    template<typename t>
    bool contains(const T& pixel, t& x) const;
    bool contains(const T& pixel) const;

    template<typename t>
    bool is_overlapped(const CImg<t>& img) const;

#endif
