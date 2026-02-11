#ifndef CIMG_MODULE_IMAGE_ITERATORS_H
#define CIMG_MODULE_IMAGE_ITERATORS_H

    iterator begin() {
      return _data;
    }

    //! Return a CImg<T>::iterator pointing to the first value of the pixel buffer \const.
    const_iterator begin() const {
      return _data;
    }

    //! Return a CImg<T>::iterator pointing next to the last pixel value.
    /**
       \note
       - Writing \c img.end() is equivalent to <tt>img.data() + img.size()</tt>.
       - It has been mainly defined for compatibility with STL naming conventions.
       \warning
       - The returned iterator actually points to a value located \e outside the acceptable bounds of the pixel buffer.
         Trying to read or write the content of the returned iterator will probably result in a crash.
         Use it mainly as a strict upper bound for a CImg<T>::iterator.
       \par Example
       \code
       CImg<float> img(100,100,1,3); // Define a 100x100 RGB color image
       // 'img.end()' used below as an upper bound for the iterator.
       for (CImg<float>::iterator it = img.begin(); it<img.end(); ++it)
         *it = 0;
       \endcode
    **/
    iterator end() {
      return _data + size();
    }

    //! Return a CImg<T>::iterator pointing next to the last pixel value \const.
    const_iterator end() const {
      return _data + size();
    }

    //! Return a reference to the first pixel value.
    /**
       \note
       - Writing \c img.front() is equivalent to <tt>img[0]</tt>, or <tt>img(0,0,0,0)</tt>.
       - It has been mainly defined for compatibility with STL naming conventions.
    **/
    T& front() {
      return *_data;
    }

    //! Return a reference to the first pixel value \const.
    const T& front() const {
      return *_data;
    }

    //! Return a reference to the last pixel value.
    /**
       \note
       - Writing \c img.back() is equivalent to <tt>img[img.size() - 1]</tt>, or
         <tt>img(img.width() - 1,img.height() - 1,img.depth() - 1,img.spectrum() - 1)</tt>.
       - It has been mainly defined for compatibility with STL naming conventions.
    **/
    T& back() {
      return *(_data + size() - 1);
    }

    //! Return a reference to the last pixel value \const.
    const T& back() const {
      return *(_data + size() - 1);
    }

#endif
