#ifndef CIMG_MODULE_LIST_INSERT_DECL_H
#define CIMG_MODULE_LIST_INSERT_DECL_H

    template<typename t>
    CImgList<T>& insert(const CImg<t>& img, const unsigned int pos=~0U, const bool is_shared=false);

    CImgList<T>& insert(const CImg<T>& img, const unsigned int pos=~0U, const bool is_shared=false);

    template<typename t>
    CImgList<T> get_insert(const CImg<t>& img, const unsigned int pos=~0U, const bool is_shared=false) const;

    CImgList<T>& insert(const unsigned int n, const unsigned int pos=~0U);

    CImgList<T> get_insert(const unsigned int n, const unsigned int pos=~0U) const;

    template<typename t>
    CImgList<T>& insert(const unsigned int n, const CImg<t>& img, const unsigned int pos=~0U,
                        const bool is_shared=false);

    template<typename t>
    CImgList<T> get_insert(const unsigned int n, const CImg<t>& img, const unsigned int pos=~0U,
                           const bool is_shared=false) const;

    template<typename t>
    CImgList<T>& insert(const CImgList<t>& list, const unsigned int pos=~0U, const bool is_shared=false);

    template<typename t>
    CImgList<T> get_insert(const CImgList<t>& list, const unsigned int pos=~0U, const bool is_shared=false) const;

    template<typename t>
    CImgList<T>& insert(const unsigned int n, const CImgList<t>& list, const unsigned int pos=~0U,
                        const bool is_shared=false);

    template<typename t>
    CImgList<T> get_insert(const unsigned int n, const CImgList<t>& list, const unsigned int pos=~0U,
                           const bool is_shared=false) const;

#endif
