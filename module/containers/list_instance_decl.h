#ifndef CIMG_MODULE_LIST_INSTANCE_DECL_H
#define CIMG_MODULE_LIST_INSTANCE_DECL_H

    int width() const;
    unsigned int size() const;

    CImg<T> *data();
    const CImg<T> *data() const;

#if cimg_verbosity>=3
    CImg<T> *data(const unsigned int pos);
    const CImg<T> *data(const unsigned int l) const;
#else
    CImg<T> *data(const unsigned int l);
    const CImg<T> *data(const unsigned int l) const;
#endif

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    CImg<T>& front();
    const CImg<T>& front() const;
    const CImg<T>& back() const;
    CImg<T>& back();

    CImg<T>& at(const int pos);

#endif
