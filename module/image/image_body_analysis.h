#ifndef CIMG_MODULE_IMAGE_BODY_ANALYSIS_H
#define CIMG_MODULE_IMAGE_BODY_ANALYSIS_H

    //-------------------------------------
    //
    //! \name Vector / Matrix Operations
    //@{
    //-------------------------------------

    //! Compute norm of the image, viewed as a matrix.
    /**
       \param magnitude_type Can be:
       - \c 0: L0-norm
       - \c 1: L1-norm
       - \c 2: L2-norm
       - \c p>2 : Lp-norm
       - \c ~0U: Linf-norm
    **/
    double magnitude(const float magnitude_type=2) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "magnitude(): Empty instance.",
                                    cimg_instance);
      const ulongT siz = size();
      double res = 0;
      if (magnitude_type==2) { // L2
        cimg_pragma_openmp(parallel for reduction(+:res) cimg_openmp_if_size(size(),8192))
        for (longT off = 0; off<(longT)siz; ++off) res+=(double)cimg::sqr(_data[off]);
        res = (double)std::sqrt(res);
      } else if (magnitude_type==1) { // L1
        cimg_pragma_openmp(parallel for reduction(+:res) cimg_openmp_if_size(size(),8192))
        for (longT off = 0; off<(longT)siz; ++off) res+=(double)cimg::abs(_data[off]);
      } else if (!magnitude_type) { // L0
        cimg_pragma_openmp(parallel for reduction(+:res) cimg_openmp_if_size(size(),8192))
        for (longT off = 0; off<(longT)siz; ++off) res+=(double)(_data[off]?1:0);
      } else if (cimg::type<float>::is_inf(magnitude_type)) { // L-inf
        cimg_for(*this,ptrs,T) { const double val = (double)cimg::abs(*ptrs); if (val>res) res = val; }
      } else { // L-p
        cimg_pragma_openmp(parallel for reduction(+:res) cimg_openmp_if_size(size(),8192))
        for (longT off = 0; off<(longT)siz; ++off)
          res+=(double)std::pow((double)cimg::abs(_data[off]),(double)magnitude_type);
        res = (double)std::pow(res,1.0/magnitude_type);
      }
      return res;
    }

    //! Compute the trace of the image, viewed as a matrix.
    /**
     **/
    double trace() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "trace(): Empty instance.",
                                    cimg_instance);
      double res = 0;
      cimg_forX(*this,k) res+=(double)(*this)(k,k);
      return res;
    }

    //! Compute the determinant of the image, viewed as a matrix.
    /**
     **/
    double det() const {
      if (is_empty() || _width!=_height || _depth!=1 || _spectrum!=1)
        throw CImgInstanceException(_cimg_instance
                                    "det(): Instance is not a square matrix.",
                                    cimg_instance);
      switch (_width) {
      case 1 : return (double)((*this)(0,0));
      case 2 : return (double)((*this)(0,0))*(double)((*this)(1,1)) - (double)((*this)(0,1))*(double)((*this)(1,0));
      case 3 : {
        const double
          a = (double)_data[0], d = (double)_data[1], g = (double)_data[2],
          b = (double)_data[3], e = (double)_data[4], h = (double)_data[5],
          c = (double)_data[6], f = (double)_data[7], i = (double)_data[8];
        return i*a*e - a*h*f - i*b*d + b*g*f + c*d*h - c*g*e;
      }
      default : {
        CImg<doubleT> Q, R;
        QR(Q,R,true);
        double res = 1;
        cimg_forX(R,i) res*=R(i,i);
        return res;
      }
      }
    }

    //! Compute the dot product between instance and argument, viewed as matrices.
    /**
       \param img Image used as a second argument of the dot product.
    **/
    template<typename t>
    double dot(const CImg<t>& img) const {
      const ulongT nb = std::min(size(),img.size());
      double res = 0;
      cimg_pragma_openmp(parallel for reduction(+:res) cimg_openmp_if_size(nb,8192))
      for (longT off = 0; off<(longT)nb; ++off) res+=(double)_data[off]*(double)img[off];
      return res;
    }

    //! Get vector-valued pixel located at specified position.
    /**
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
    **/
    CImg<T> get_vector_at(const unsigned int x, const unsigned int y=0, const unsigned int z=0) const {
      CImg<T> res;
      if (res._height!=_spectrum) res.assign(1,_spectrum);
      const ulongT whd = (ulongT)_width*_height*_depth;
      const T *ptrs = data(x,y,z);
      T *ptrd = res._data;
      cimg_forC(*this,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
      return res;
    }

    //! Get (square) matrix-valued pixel located at specified position.
    /**
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
       \note - The spectrum() of the image must be a square.
     **/
    CImg<T> get_matrix_at(const unsigned int x=0, const unsigned int y=0, const unsigned int z=0) const {
      const int n = (int)cimg::round(std::sqrt((double)_spectrum));
      const T *ptrs = data(x,y,z,0);
      const ulongT whd = (ulongT)_width*_height*_depth;
      CImg<T> res(n,n);
      T *ptrd = res._data;
      cimg_forC(*this,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
      return res;
    }

    //! Get tensor-valued pixel located at specified position.
    /**
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
    **/
    CImg<T> get_tensor_at(const unsigned int x, const unsigned int y=0, const unsigned int z=0) const {
      const T *ptrs = data(x,y,z,0);
      const ulongT whd = (ulongT)_width*_height*_depth;
      if (_spectrum==6)
        return tensor(*ptrs,*(ptrs + whd),*(ptrs + 2*whd),*(ptrs + 3*whd),*(ptrs + 4*whd),*(ptrs + 5*whd));
      if (_spectrum==3)
        return tensor(*ptrs,*(ptrs + whd),*(ptrs + 2*whd));
      return tensor(*ptrs);
    }

    //! Set vector-valued pixel at specified position.
    /**
       \param vec Vector to put on the instance image.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
    **/
    template<typename t>
    CImg<T>& set_vector_at(const CImg<t>& vec, const unsigned int x, const unsigned int y=0, const unsigned int z=0) {
      if (x<_width && y<_height && z<_depth) {
        const t *ptrs = vec._data;
        const ulongT whd = (ulongT)_width*_height*_depth;
        T *ptrd = data(x,y,z);
        for (unsigned int k = std::min((unsigned int)vec.size(),_spectrum); k; --k) {
          *ptrd = (T)*(ptrs++); ptrd+=whd;
        }
      }
      return *this;
    }

    //! Set (square) matrix-valued pixel at specified position.
    /**
       \param mat Matrix to put on the instance image.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
    **/
    template<typename t>
    CImg<T>& set_matrix_at(const CImg<t>& mat, const unsigned int x=0, const unsigned int y=0, const unsigned int z=0) {
      return set_vector_at(mat,x,y,z);
    }

    //! Set tensor-valued pixel at specified position.
    /**
       \param ten Tensor to put on the instance image.
       \param x X-coordinate of the pixel value.
       \param y Y-coordinate of the pixel value.
       \param z Z-coordinate of the pixel value.
    **/
    template<typename t>
    CImg<T>& set_tensor_at(const CImg<t>& ten, const unsigned int x=0, const unsigned int y=0, const unsigned int z=0) {
      T *ptrd = data(x,y,z,0);
      const ulongT siz = (ulongT)_width*_height*_depth;
      if (ten._height==2) {
        *ptrd = (T)ten[0]; ptrd+=siz;
        *ptrd = (T)ten[1]; ptrd+=siz;
        *ptrd = (T)ten[3];
      }
      else {
        *ptrd = (T)ten[0]; ptrd+=siz;
        *ptrd = (T)ten[1]; ptrd+=siz;
        *ptrd = (T)ten[2]; ptrd+=siz;
        *ptrd = (T)ten[4]; ptrd+=siz;
        *ptrd = (T)ten[5]; ptrd+=siz;
        *ptrd = (T)ten[8];
      }
      return *this;
    }

    //! Resize image to become a diagonal matrix.
    /**
       \note Transform the image as a diagonal matrix so that each of its initial value becomes a diagonal coefficient.
    **/
    CImg<T>& diagonal() {
      return get_diagonal().move_to(*this);
    }

    //! Resize image to become a diagonal matrix \newinstance.
    CImg<T> get_diagonal() const {
      if (is_empty()) return *this;
      const unsigned int siz = (unsigned int)size();
      CImg<T> res(siz,siz,1,1,0);
      cimg_foroff(*this,off) res((unsigned int)off,(unsigned int)off) = (*this)[off];
      return res;
    }

    //! Replace the image by an identity matrix.
    /**
       \note If the instance image is not square, it is resized to a square matrix using its maximum
       dimension as a reference.
    **/
    CImg<T>& identity_matrix() {
      return identity_matrix(std::max(_width,_height)).move_to(*this);
    }

    //! Replace the image by an identity matrix \newinstance.
    CImg<T> get_identity_matrix() const {
      return identity_matrix(std::max(_width,_height));
    }

    //! Fill image with a linear sequence of values.
    /**
       \param a0 Starting value of the sequence.
       \param a1 Ending value of the sequence.
    **/
    CImg<T>& sequence(const T& a0, const T& a1) {
      if (is_empty()) return *this;
      const ulongT siz = size() - 1;
      T* ptr = _data;
      if (siz) {
        const double delta = (double)a1 - (double)a0;
        cimg_foroff(*this,l) *(ptr++) = (T)(a0 + delta*l/siz);
      } else *ptr = a0;
      return *this;
    }

    //! Fill image with a linear sequence of values \newinstance.
    CImg<T> get_sequence(const T& a0, const T& a1) const {
      return (+*this).sequence(a0,a1);
    }

    //! Transpose the image, viewed as a matrix.
    /**
       \note Equivalent to \code permute_axes("yxzc"); \endcode.
    **/
    CImg<T>& transpose() {
      if (_width==1) { _width = _height; _height = 1; return *this; }
      if (_height==1) { _height = _width; _width = 1; return *this; }
      if (_width==_height) {
        cimg_forYZC(*this,y,z,c) for (int x = y; x<width(); ++x) cimg::swap((*this)(x,y,z,c),(*this)(y,x,z,c));
        return *this;
      }
      return get_transpose().move_to(*this);
    }

    //! Transpose the image, viewed as a matrix \newinstance.
    CImg<T> get_transpose() const {
      return get_permute_axes("yxzc");
    }

    //! Compute the cross product between two \c 1x3 images, viewed as 3D vectors.
    /**
       \param img Image used as the second argument of the cross product.
       \note The first argument of the cross product is \c *this.
     **/
    template<typename t>
    CImg<T>& cross(const CImg<t>& img) {
      if (_width!=1 || _height<3 || img._width!=1 || img._height<3)
        throw CImgInstanceException(_cimg_instance
                                    "cross(): Instance and/or specified image (%u,%u,%u,%u,%p) are not 3D vectors.",
                                    cimg_instance,
                                    img._width,img._height,img._depth,img._spectrum,img._data);

      const T x = (*this)[0], y = (*this)[1], z = (*this)[2];
      (*this)[0] = (T)(y*img[2] - z*img[1]);
      (*this)[1] = (T)(z*img[0] - x*img[2]);
      (*this)[2] = (T)(x*img[1] - y*img[0]);
      return *this;
    }

    //! Compute the cross product between two \c 1x3 images, viewed as 3D vectors \newinstance.
    template<typename t>
    CImg<_cimg_Tt> get_cross(const CImg<t>& img) const {
      return CImg<_cimg_Tt>(*this).cross(img);
    }

    //! Invert the instance matrix.
    /**
       If the instance matrix is not square, the Moore-Penrose pseudo-inverse is computed instead.
    **/
    CImg<T>& invert() {
      return get_invert().move_to(*this);
    }

    //! Invert the instance matrix.
    CImg<Tfloat> get_invert() const {
      if (_depth!=1 || _spectrum!=1)
        throw CImgArgumentException(_cimg_instance
              "invert(): Instance is not a matrix.",
                                    cimg_instance);
      if (_height==_width) // Square matrix
        return CImg<T>::identity_matrix(_height).get_solve(*this);
      if (_height>_width) { // Tall matrix: (A^t.A)^-1.A^t
        const CImg<T> At = get_transpose();
        return At.get_solve(At*(*this));
      }
      // Wide matrix: ((A.A^t)^-1.A)^t
      return get_solve((*this)*get_transpose()).transpose();
    }

    //! Solve a (possibly over- or under-determined) linear system using QR decomposition.
    /**
       \brief Solve the matrix equation \f$ A\,X = B \f$, where the current instance \fs *this represents \f$ B \f$,
       and the argument \c A is the system matrix. This method supports both over-determined and
       under-determined systems by internally performing a QR decomposition.

       - If \f$ A \f$ has more rows than columns (\f$ m \ge n \f$), the system is square or over-determined,
       and the least-squares solution minimizing \f$ \|A\,X - B\|_2 \f$ is computed.
       - If \f$ A \f$ has more columns than rows (\f$ m < n \f$), the system is under-determined.
       The solution of minimal norm is computed using the QR decomposition of the transposed system.

       The computation is performed in double precision for numerical stability.
    **/
    template<typename t>
    CImg<T>& solve(const CImg<t>& A) {
      return get_solve(A).move_to(*this);
    }

    //! Solve a (possibly over- or under-determined) linear system using QR decomposition \newinstance.
    template<typename t>
    CImg<_cimg_Ttfloat> get_solve(const CImg<t>& A) const {
      if (_depth!=1 || _spectrum!=1 || _height!=A._height || A._depth!=1 || A._spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "solve(): Instance and specified matrix (%u,%u,%u,%u,%p) have "
                                    "incompatible dimensions.",
                                    cimg_instance,
                                    A._width,A._height,A._depth,A._spectrum,A._data);
      typedef _cimg_Ttfloat Ttfloat;
      if (is_empty()) return CImg<Ttfloat>();
      if (A._height!=A._width) {
        // Non-square systems: use Moore-Penrose pseudo-inverse to lower memory usage in QR decomposition.
        const CImg<T> At = A.get_transpose();
        const CImg<Ttfloat> invA = A._height>A._width?At.get_solve(At*A):A.get_solve(A*At).transpose();
        return invA*(*this);
      }

      const int m = A.height(), n = A.width(), p = width();
      CImg<doubleT> Q, R;

      // m>=n: Over-determined or square system.
      if (m>=n) {
        A.QR(Q,R,true); // Reduced QR decomposition
        const CImg<doubleT> y = Q.get_transpose()*(*this);

        // Solve R*x = y (R is upper triangular).
        CImg<doubleT> x(p,n);
        cimg_forX(x,k) {
          cimg_rofY(x,i) {
            double sum = y(k,i);
            for (int j = i + 1; j<n; ++j) sum-=R(j,i)*x(k,j);
            x(k,i) = sum/R(i,i);
          }
        }
        return x;
      }

      // m<n -> under-determined system.
      A.get_transpose().QR(Q,R,true);

      // Solve R^T*z = b, where z = Q^T*x.
      CImg<doubleT> z(p,m);
      cimg_forX(*this,k) {
        cimg_forY(z,i) {
          double sum = (*this)(k,i);
          for (int j = 0; j<i; ++j) sum-=R(i,j)*z(k,j);
          z(k,i) = sum/R(i,i);
        }
      }
      return Q*z;
    }

    //! Solve a tridiagonal system of linear equations.
    /**
       \param A Coefficients of the tridiagonal system.
       A is a tridiagonal matrix A = [ b0,c0,0,...; a1,b1,c1,0,... ; ... ; ...,0,aN,bN ],
       stored as a 3 columns matrix
       \note Solve AX=B where \c B=*this, using the Thomas algorithm.
    **/
    template<typename t>
    CImg<T>& solve_tridiagonal(const CImg<t>& A) {
      const unsigned int siz = (unsigned int)size();
      if (A._width!=3 || A._height!=siz)
        throw CImgArgumentException(_cimg_instance
                                    "solve_tridiagonal(): Instance and tridiagonal matrix "
                                    "(%u,%u,%u,%u,%p) have incompatible dimensions.",
                                    cimg_instance,
                                    A._width,A._height,A._depth,A._spectrum,A._data);
      typedef _cimg_Ttfloat Ttfloat;
      const Ttfloat eps = 1e-4f;
      CImg<Ttfloat> B = A.get_column(1), V(*this,false);
      for (int i = 1; i<(int)siz; ++i) {
        const Ttfloat m = A(0,i)/(B[i - 1]?B[i - 1]:eps);
        B[i] -= m*A(2,i - 1);
        V[i] -= m*V[i - 1];
      }
      (*this)[siz - 1] = (T)(V[siz - 1]/(B[siz - 1]?B[siz - 1]:eps));
      for (int i = (int)siz - 2; i>=0; --i) (*this)[i] = (T)((V[i] - A(2,i)*(*this)[i + 1])/(B[i]?B[i]:eps));
      return *this;
    }

    //! Solve a tridiagonal system of linear equations \newinstance.
    template<typename t>
    CImg<_cimg_Ttfloat> get_solve_tridiagonal(const CImg<t>& A) const {
      return CImg<_cimg_Ttfloat>(*this,false).solve_tridiagonal(A);
    }

    //! Compute eigenvalues and eigenvectors of the instance image, viewed as a matrix.
    /**
       \param[out] val Vector of the estimated eigenvalues, in decreasing order.
       \param[out] vec Matrix of the estimated eigenvectors, sorted by columns.
    **/
    template<typename t>
    const CImg<T>& eigen(CImg<t>& val, CImg<t> &vec) const {
      if (is_empty()) { val.assign(); vec.assign(); }
      else {
        if (_width!=_height || _depth>1 || _spectrum>1)
          throw CImgInstanceException(_cimg_instance
                                      "eigen(): Instance is not a square matrix.",
                                      cimg_instance);

        if (val.size()<(ulongT)_width) val.assign(1,_width);
        if (vec.size()<(ulongT)_width*_width) vec.assign(_width,_width);
        switch (_width) {
        case 1 : { val[0] = (t)(*this)[0]; vec[0] = (t)1; } break;
        case 2 : {
          const double a = (*this)[0], b = (*this)[1], c = (*this)[2], d = (*this)[3], e = a + d;
          double f = e*e - 4*(a*d - b*c);
          if (f<0) cimg::warn(_cimg_instance
                              "eigen(): Complex eigenvalues found.",
                              cimg_instance);
          f = std::sqrt(f);
          const double
            l1 = 0.5*(e - f),
            l2 = 0.5*(e + f),
            b2 = b*b,
            norm1 = std::sqrt(cimg::sqr(l2 - a) + b2),
            norm2 = std::sqrt(cimg::sqr(l1 - a) + b2);
          val[0] = (t)l2;
          val[1] = (t)l1;
          if (norm1>0) { vec(0,0) = (t)(b/norm1); vec(0,1) = (t)((l2 - a)/norm1); } else { vec(0,0) = 1; vec(0,1) = 0; }
          if (norm2>0) { vec(1,0) = (t)(b/norm2); vec(1,1) = (t)((l1 - a)/norm2); } else { vec(1,0) = 1; vec(1,1) = 0; }
        } break;
        default :
          throw CImgInstanceException(_cimg_instance
                                      "eigen(): Eigenvalues computation of general matrices is limited "
                                      "to 2x2 matrices.",
                                      cimg_instance);
        }
      }
      return *this;
    }

    //! Compute eigenvalues and eigenvectors of the instance image, viewed as a matrix.
    /**
       \return A list of two images <tt>[val; vec]</tt>, whose meaning is similar as in eigen(CImg<t>&,CImg<t>&) const.
    **/
    CImgList<Tfloat> get_eigen() const {
      CImgList<Tfloat> res(2);
      eigen(res[0],res[1]);
      return res;
    }

    //! Compute eigenvalues and eigenvectors of the instance image, viewed as a symmetric matrix.
    /**
       \param[out] val Vector of the estimated eigenvalues, in decreasing order.
       \param[out] vec Matrix of the estimated eigenvectors, sorted by columns.
    **/
    template<typename t>
    const CImg<T>& symmetric_eigen(CImg<t>& val, CImg<t>& vec) const {
      if (is_empty()) { val.assign(); vec.assign(); return *this; }
      if (_width!=_height || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "eigen(): Instance is not a square matrix.",
                                    cimg_instance);
      val.assign(1,_width);
      vec.assign(_width,_width);

      if (_width==1) { val[0] = cimg::abs((*this)[0]); vec[0] = 1; return *this; }
      if (_width==2) {
        const double
          a = (*this)[0], b = (*this)[1], c = (*this)[2], d = (*this)[3],
          e = a + d, f = std::sqrt(std::max(e*e - 4*(a*d - b*c),0.0)),
          l1 = 0.5*(e - f), l2 = 0.5*(e + f),
          n = std::sqrt(cimg::sqr(l2 - a) + b*b);
        val[0] = (t)l2;
        val[1] = (t)l1;
        if (n>0) { vec[0] = (t)(b/n); vec[2] = (t)((l2 - a)/n); } else { vec[0] = 1; vec[2] = 0; }
        vec[1] = -vec[2];
        vec[3] = vec[0];
        return *this;
      }
      CImg<t> V(_width,_width);
      Tfloat M = 0, m = (Tfloat)min_max(M), maxabs = cimg::max((Tfloat)1,cimg::abs(m),cimg::abs(M));
      (CImg<Tfloat>(*this,false)/=maxabs).SVD(vec,val,V,false);
      if (maxabs!=1) val*=maxabs;

      bool is_ambiguous = false;
      float eig = 0;
      cimg_forY(val,p) { // Check for ambiguous cases
        if (val[p]>eig) eig = (float)val[p];
        t scal = 0;
        cimg_forY(vec,y) scal+=vec(p,y)*V(p,y);
        if (cimg::abs(scal)<0.9f) is_ambiguous = true;
        if (scal<0) val[p] = -val[p];
      }
      if (is_ambiguous) {
        ++(eig*=2);
        SVD(vec,val,V,false,40,eig);
        val-=eig;
      }

      CImg<intT> permutations; // Sort eigenvalues in decreasing order
      CImg<t> tmp(_width);
      val.sort(permutations,false);
      cimg_forY(vec,k) {
        cimg_forY(permutations,y) tmp(y) = vec(permutations(y),k);
        std::memcpy(vec.data(0,k),tmp._data,sizeof(t)*_width);
      }
      return *this;
    }

    //! Compute eigenvalues and eigenvectors of the instance image, viewed as a symmetric matrix.
    /**
       \return A list of two images <tt>[val; vec]</tt>, whose meaning are similar as in
         symmetric_eigen(CImg<t>&,CImg<t>&) const.
    **/
    CImgList<Tfloat> get_symmetric_eigen() const {
      CImgList<Tfloat> res(2);
      symmetric_eigen(res[0],res[1]);
      return res;
    }

    //! Sort pixel values and get sorting permutations.
    /**
       \param[out] permutations Permutation map used for the sorting.
       \param is_increasing Tells if pixel values are sorted in an increasing (\c true) or decreasing (\c false) way.
    **/
    template<typename t>
    CImg<T>& sort(CImg<t>& permutations, const bool is_increasing=true) {
      permutations.assign(_width,_height,_depth,_spectrum);
      if (is_empty()) return *this;
      cimg_foroff(permutations,off) permutations[off] = (t)off;
      return _quicksort(0,size() - 1,permutations,is_increasing,true);
    }

    //! Sort pixel values and get sorting permutations \newinstance.
    template<typename t>
    CImg<T> get_sort(CImg<t>& permutations, const bool is_increasing=true) const {
      return (+*this).sort(permutations,is_increasing);
    }

    //! Sort pixel values.
    /**
       \param is_increasing Tells if pixel values are sorted in an increasing (\c true) or decreasing (\c false) way.
       \param axis Tells if the value sorting must be done along a specific axis. Can be:
       - \c 0: All pixel values are sorted, independently on their initial position.
       - \c 'x': Image columns are sorted, according to the first value in each column.
       - \c 'y': Image rows are sorted, according to the first value in each row.
       - \c 'z': Image slices are sorted, according to the first value in each slice.
       - \c 'c': Image channels are sorted, according to the first value in each channel.
    **/
    CImg<T>& sort(const bool is_increasing=true, const char axis=0) {
      if (is_empty()) return *this;
      CImg<uintT> perm;
      switch (cimg::lowercase(axis)) {
      case 0 :
        _quicksort(0,size() - 1,perm,is_increasing,false);
        break;
      case 'x' : {
        perm.assign(_width);
        get_crop(0,0,0,0,_width - 1,0,0,0).sort(perm,is_increasing);
        CImg<T> img(*this,false);
        cimg_forXYZC(*this,x,y,z,c) (*this)(x,y,z,c) = img(perm[x],y,z,c);
      } break;
      case 'y' : {
        perm.assign(_height);
        get_crop(0,0,0,0,0,_height - 1,0,0).sort(perm,is_increasing);
        CImg<T> img(*this,false);
        cimg_forXYZC(*this,x,y,z,c) (*this)(x,y,z,c) = img(x,perm[y],z,c);
      } break;
      case 'z' : {
        perm.assign(_depth);
        get_crop(0,0,0,0,0,0,_depth - 1,0).sort(perm,is_increasing);
        CImg<T> img(*this,false);
        cimg_forXYZC(*this,x,y,z,c) (*this)(x,y,z,c) = img(x,y,perm[z],c);
      } break;
      case 'c' : {
        perm.assign(_spectrum);
        get_crop(0,0,0,0,0,0,0,_spectrum - 1).sort(perm,is_increasing);
        CImg<T> img(*this,false);
        cimg_forXYZC(*this,x,y,z,c) (*this)(x,y,z,c) = img(x,y,z,perm[c]);
      } break;
      default :
        throw CImgArgumentException(_cimg_instance
                                    "sort(): Invalid specified axis '%c' "
                                    "(should be { x | y | z | c }).",
                                    cimg_instance,axis);
      }
      return *this;
    }

    //! Sort pixel values \newinstance.
    CImg<T> get_sort(const bool is_increasing=true, const char axis=0) const {
      return (+*this).sort(is_increasing,axis);
    }

    template<typename t>
    CImg<T>& _quicksort(const long indm, const long indM, CImg<t>& permutations,
                        const bool is_increasing, const bool is_permutations) {
      if (indm<indM) {
        const long mid = (indm + indM)/2;
        if (is_increasing) {
          if ((*this)[indm]>(*this)[mid]) {
            cimg::swap((*this)[indm],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indm],permutations[mid]);
          }
          if ((*this)[mid]>(*this)[indM]) {
            cimg::swap((*this)[indM],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indM],permutations[mid]);
          }
          if ((*this)[indm]>(*this)[mid]) {
            cimg::swap((*this)[indm],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indm],permutations[mid]);
          }
        } else {
          if ((*this)[indm]<(*this)[mid]) {
            cimg::swap((*this)[indm],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indm],permutations[mid]);
          }
          if ((*this)[mid]<(*this)[indM]) {
            cimg::swap((*this)[indM],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indM],permutations[mid]);
          }
          if ((*this)[indm]<(*this)[mid]) {
            cimg::swap((*this)[indm],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indm],permutations[mid]);
          }
        }
        if (indM - indm>=3) {
          const T pivot = (*this)[mid];
          long i = indm, j = indM;
          if (is_increasing) {
            do {
              while ((*this)[i]<pivot) ++i;
              while ((*this)[j]>pivot) --j;
              if (i<=j) {
                if (is_permutations) cimg::swap(permutations[i],permutations[j]);
                cimg::swap((*this)[i++],(*this)[j--]);
              }
            } while (i<=j);
          } else {
            do {
              while ((*this)[i]>pivot) ++i;
              while ((*this)[j]<pivot) --j;
              if (i<=j) {
                if (is_permutations) cimg::swap(permutations[i],permutations[j]);
                cimg::swap((*this)[i++],(*this)[j--]);
              }
            } while (i<=j);
          }
          if (indm<j) _quicksort(indm,j,permutations,is_increasing,is_permutations);
          if (i<indM) _quicksort(i,indM,permutations,is_increasing,is_permutations);
        }
      }
      return *this;
    }

    //! Compute the SVD of the instance image, viewed as a general matrix.
    /**
       Compute the SVD decomposition \c *this=U*S*V' where \c U and \c V are orthogonal matrices
       and \c S is a diagonal matrix. \c V' denotes the matrix transpose of \c V.
       \param[out] U First matrix of the SVD product.
       \param[out] S Coefficients of the second (diagonal) matrix of the SVD product.
         These coefficients are stored as a vector.
       \param[out] V Third matrix of the SVD product.
       \param sorting Tells if the diagonal coefficients are sorted (in decreasing order).
       \param max_iteration Maximum number of iterations considered for the algorithm convergence.
       \param lambda Epsilon used for the algorithm convergence.
       \note The instance matrix can be computed from \c U,\c S and \c V by
       \code
       const CImg<> A; // Input matrix (assumed to contain some values)
       CImg<> U,S,V;
       A.SVD(U,S,V)
       \endcode
    **/
    template<typename t>
    const CImg<T>& SVD(CImg<t>& U, CImg<t>& S, CImg<t>& V, const bool sorting=true,
                       const unsigned int max_iteration=40, const float lambda=0) const {
      typedef _cimg_Ttfloat Ttfloat;
      const Ttfloat eps = (Ttfloat)1e-8f;
      if (is_empty()) { U.assign(); S.assign(); V.assign(); }
      else if (_depth!=1 || _spectrum!=1)
        throw CImgInstanceException(_cimg_instance
                                    "SVD(): Instance has invalid dimensions (depth or channels different from 1).",
                                    cimg_instance);
      else {
        U = *this;
        if (lambda!=0) {
          const unsigned int delta = std::min(U._width,U._height);
          for (unsigned int i = 0; i<delta; ++i) U(i,i) = (t)(U(i,i) + lambda);
        }
        if (S.size()<_width) S.assign(1,_width);
        if (V._width<_width || V._height<_height) V.assign(_width,_width);
        CImg<t> rv1(_width);
        Ttfloat anorm = 0, c, f, g = 0, h, s, scale = 0;
        int l = 0;

        cimg_forX(U,i) {
          l = i + 1;
          rv1[i] = scale*g;
          g = s = scale = 0;
          if (i<height()) {
            for (int k = i; k<height(); ++k) scale+=cimg::abs(U(i,k));
            if (scale) {
              for (int k = i; k<height(); ++k) {
                U(i,k)/=scale;
                s+=U(i,k)*U(i,k);
              }
              f = U(i,i);
              g = (Ttfloat)((f>=0?-1:1)*std::sqrt(s));
              h = f*g - s;
              U(i,i) = f - g;
              for (int j = l; j<width(); ++j) {
                s = 0;
                for (int k = i; k<height(); ++k) s+=U(i,k)*U(j,k);
                f = s/h;
                for (int k = i; k<height(); ++k) U(j,k)+=f*U(i,k);
              }
              for (int k = i; k<height(); ++k) U(i,k)*=scale;
            }
          }
          S[i] = scale*g;

          g = s = scale = 0;
          if (i<height() && i!=width() - 1) {
            for (int k = l; k<width(); ++k) scale+=cimg::abs(U(k,i));
            if (scale) {
              for (int k = l; k<width(); ++k) {
                U(k,i)/=scale;
                s+=U(k,i)*U(k,i);
              }
              f = U(l,i);
              g = (Ttfloat)((f>=0?-1:1)*std::sqrt(s));
              h = f*g - s;
              U(l,i) = f - g;
              for (int k = l; k<width(); ++k) rv1[k] = U(k,i)/h;
              for (int j = l; j<height(); ++j) {
                s = 0;
                for (int k = l; k<width(); ++k) s+=U(k,j)*U(k,i);
                for (int k = l; k<width(); ++k) U(k,j)+=s*rv1[k];
              }
              for (int k = l; k<width(); ++k) U(k,i)*=scale;
            }
          }
          anorm = (Ttfloat)std::max((float)anorm,(float)(cimg::abs(S[i]) + cimg::abs(rv1[i])));
        }

        for (int i = width() - 1; i>=0; --i) {
          if (i<width() - 1) {
            if (g) {
              for (int j = l; j<width(); ++j) V(i,j) =(U(j,i)/U(l,i))/g;
              for (int j = l; j<width(); ++j) {
                s = 0;
                for (int k = l; k<width(); ++k) s+=U(k,i)*V(j,k);
                for (int k = l; k<width(); ++k) V(j,k)+=s*V(i,k);
              }
            }
            for (int j = l; j<width(); ++j) V(j,i) = V(i,j) = (t)0.;
          }
          V(i,i) = (t)1;
          g = rv1[i];
          l = i;
        }

        for (int i = std::min(width(),height()) - 1; i>=0; --i) {
          l = i + 1;
          g = S[i];
          for (int j = l; j<width(); ++j) U(j,i) = 0;
          if (g) {
            g = 1/g;
            for (int j = l; j<width(); ++j) {
              s = 0;
              for (int k = l; k<height(); ++k) s+=U(i,k)*U(j,k);
              f = (s/U(i,i))*g;
              for (int k = i; k<height(); ++k) U(j,k)+=f*U(i,k);
            }
            for (int j = i; j<height(); ++j) U(i,j)*= g;
          } else for (int j = i; j<height(); ++j) U(i,j) = 0;
          ++U(i,i);
        }

        for (int k = width() - 1; k>=0; --k) {
          int nm = 0;
          for (unsigned int its = 0; its<max_iteration; ++its) {
            bool flag = true;
            for (l = k; l>=1; --l) {
              nm = l - 1;
              if (l==1 || cimg::abs(rv1[l])<=eps*anorm) { flag = false; break; }
              if (cimg::abs(S[nm])<=eps*anorm) break;
            }
            if (flag) {
              c = 0;
              s = 1;
              for (int i = l; i<=k; ++i) {
                f = s*rv1[i];
                rv1[i] = c*rv1[i];
                if ((cimg::abs(f) + anorm)==anorm) break;
                g = S[i];
                h = cimg::hypot(f,g);
                S[i] = h;
                h = 1/h;
                c = g*h;
                s = -f*h;
                cimg_forY(U,j) {
                  const t y = U(nm,j), z = U(i,j);
                  U(nm,j) = y*c + z*s;
                  U(i,j) = z*c - y*s;
                }
              }
            }

            const t z = S[k];
            if (l==k) { if (z<0) { S[k] = -z; cimg_forX(U,j) V(k,j) = -V(k,j); } break; }
            nm = k - 1;
            t x = S[l], y = S[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z)*(y + z) + (g - h)*(g + h))/std::max(eps,(Ttfloat)2*h*y);
            g = cimg::hypot(f,(Ttfloat)1);
            f = ((x - z)*(x + z) + h*((y/(f + (f>=0?g:-g))) - h))/std::max(eps,(Ttfloat)x);
            c = s = 1;
            for (int j = l; j<=nm; ++j) {
              const int i = j + 1;
              g = rv1[i];
              h = s*g;
              g = c*g;
              t y1 = S[i], z1 = cimg::hypot(f,h);
              rv1[j] = z1;
              c = f/std::max(eps,(Ttfloat)z1);
              s = h/std::max(eps,(Ttfloat)z1);
              f = x*c + g*s;
              g = g*c - x*s;
              h = y1*s;
              y1*=c;
              cimg_forX(U,jj) {
                const t x2 = V(j,jj), z2 = V(i,jj);
                V(j,jj) = x2*c + z2*s;
                V(i,jj) = z2*c - x2*s;
              }
              z1 = cimg::hypot(f,h);
              S[j] = z1;
              if (z1) {
                z1 = 1/std::max(eps,(Ttfloat)z1);
                c = f*z1;
                s = h*z1;
              }
              f = c*g + s*y1;
              x = c*y1 - s*g;
              cimg_forY(U,jj) {
                const t y2 = U(j,jj), z2 = U(i,jj);
                U(j,jj) = y2*c + z2*s;
                U(i,jj) = z2*c - y2*s;
              }
            }
            rv1[l] = 0;
            rv1[k] = f;
            S[k] = x;
          }
        }

        if (sorting) {
          CImg<intT> permutations;
          CImg<t> tmp(_width);
          S.sort(permutations,false);
          cimg_forY(U,k) {
            cimg_forY(permutations,y) tmp(y) = U(permutations(y),k);
            std::memcpy(U.data(0,k),tmp._data,sizeof(t)*_width);
          }
          cimg_forY(V,k) {
            cimg_forY(permutations,y) tmp(y) = V(permutations(y),k);
            std::memcpy(V.data(0,k),tmp._data,sizeof(t)*_width);
          }
        }
      }
      return *this;
    }

    //! Compute the SVD of the instance image, viewed as a general matrix.
    /**
       \return A list of three images <tt>[U; S; V]</tt>, whose meaning is similar as in
         SVD(CImg<t>&,CImg<t>&,CImg<t>&,bool,unsigned int,float) const.
    **/
    CImgList<Tfloat> get_SVD(const bool sorting=true,
                             const unsigned int max_iteration=40, const float lambda=0) const {
      CImgList<Tfloat> res(3);
      SVD(res[0],res[1],res[2],sorting,max_iteration,lambda);
      return res;
    }

    //! Compute the QR decomposition of the instance matrix.
    /**
       Given an instance matrix (*this) of size m×n (m rows, n columns),
       fill the matrices Q and R, so that *this = Q*R.
       - Q is an orthogonal matrix, of size 'm×m' if 'is_reduced_form==false', or 'm×min(m,n)' otherwise.
       - R is an upper-trianguler matrix of size 'm×n' if 'is_reduced_form==false' or 'min(m,n)×n' otherwise.
       - Q^T*Q = Id.
       - If n>m, only the first m×m part of R is upper triangular.
    **/
    template<typename t>
    const CImg<T>& QR(CImg<t>& Q, CImg<t>& R, const bool is_reduced_form=true) const {
      if (is_empty()) { Q.assign(); R.assign(); return *this; }
      if (_depth!=1 || _spectrum!=1)
        throw CImgInstanceException(_cimg_instance
                                    "QR(): Instance image is not a matrix.",
                                    cimg_instance);

      const int m = height(), n = width(), k = std::min(m,n);
      CImg<doubleT> _R(*this,false), _Q = CImg<doubleT>::identity_matrix(m);

      for (int j = 0; j<k; ++j) {

        // Build the Householder vector v.
        CImg<doubleT> x = _R.get_crop(j,j,j,m);
        const double normx = x.magnitude();
        if (normx<1e-15) continue;
        x[0]+=(x[0]>=0?1:-1)*normx;
        x/=x.magnitude();

        // Apply reflection to R
        cimg_pragma_openmp(parallel for cimg_openmp_if(m*(n - j)>=512*512))
        for (int col = j; col<n; ++col) {
          double dot = 0;
          for (int i = j; i<m; ++i) dot+=x[i - j]*_R(col,i);
          for (int i = j; i<m; ++i) _R(col,i)-=2*x[i - j]*dot;
        }

        // Apply reflection to Q.
        cimg_pragma_openmp(parallel for cimg_openmp_if(m>=512))
        for (int row = 0; row<m; ++row){
          double dot = 0;
          for (int i = j; i<m; ++i) dot+=_Q(i,row)*x[i - j];
          for (int i = j; i<m; ++i) _Q(i,row)-=2*x(i - j,0)*dot;
        }
      }

      // Force R to be upper-triangular.
      for (int y = 1; y<m; ++y) for (int x = 0; x<std::min(y,n); ++x) _R(x,y) = 0.;
      if (is_reduced_form && m>n) {
        _Q.crop(0,0,n - 1,m - 1);  // Keep only first n columns of Q
        _R.crop(0,0,n - 1,n - 1);  // Keep only top n×n part of R
      }
      _Q.move_to(Q);
      _R.move_to(R);
      return *this;
    }

    //! Compute the projection of the instance matrix onto the specified dictionary.
    /**
       Find the best matching projection of selected matrix onto the span of an over-complete dictionary D,
       using the orthogonal projection or (opt. Orthogonal) Matching Pursuit algorithm.
       Instance image must a 2D-matrix in which each column represent a signal to project.
       \param dictionary A matrix in which each column is an element of the dictionary D.
       \param method Tell what projection method is applied. It can be:
         - 0 = orthogonal projection (default).
         - 1 = matching pursuit.
         - 2 = matching pursuit, with a single orthogonal projection step at the end.
         - >=3 = orthogonal matching pursuit where an orthogonal projection step is performed
                 every 'method-2' iterations.
       \param max_iter Sets the max number of iterations processed for each signal.
                       If set to '0' (default), 'max_iter' is set to the number of dictionary columns.
                       (only meaningful for matching pursuit and its variants).
       \param max_residual Gives a stopping criterion on signal reconstruction accuracy.
                           (only meaningful for matching pursuit and its variants).
       \return A matrix W whose columns correspond to the sparse weights of associated to each input matrix column.
               Thus, the matrix product D*W is an approximation of the input matrix.
    **/
    template<typename t>
    CImg<T>& project_matrix(const CImg<t>& dictionary, const unsigned int method=0,
                            const unsigned int max_iter=0, const double max_residual=1e-6) {
      return get_project_matrix(dictionary,method,max_iter,max_residual).move_to(*this);
    }

    template<typename t>
    CImg<Tfloat> get_project_matrix(const CImg<t>& dictionary, const unsigned int method=0,
                                    const unsigned int max_iter=0, const double max_residual=1e-6) const {
      if (_depth!=1 || _spectrum!=1)
        throw CImgInstanceException(_cimg_instance
                                    "project_matrix(): Instance image is not a matrix.",
                                    cimg_instance);
      if (dictionary._height!=_height || dictionary._depth!=1 || dictionary._spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "project_matrix(): Specified dictionary (%u,%u,%u,%u) has an invalid size.",
                                    cimg_instance,
                                    dictionary._width,dictionary._height,dictionary._depth,dictionary._spectrum);
      if (!method) return get_solve(dictionary);

      // Compute norm of dictionary atoms.
      CImg<Tfloat> dictionary_norm(dictionary._width);
      cimg_pragma_openmp(parallel for
                         cimg_openmp_if(dictionary._width>=2 && dictionary._width*dictionary._height>=32))
      cimg_forX(dictionary_norm,atom) {
        Tfloat norm = 0;
        cimg_forY(dictionary,s) norm+=cimg::sqr(dictionary(atom,s));
        dictionary_norm[atom] = std::max((Tfloat)1e-8,std::sqrt(norm));
      }

      // Matching pursuit.
      CImg<Tfloat> weights(_width,dictionary._width,1,1,0);
      const unsigned int proj_step = method<3?1:method - 2;
      bool is_orthoproj = false;

      cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=2 && _width*_height>=32))
      cimg_forX(*this,signal) {
        CImg<Tfloat> R = get_column(signal); // Residual signal
        const CImg<Tfloat> R0 = method<2?CImg<Tfloat>():R;
        Tfloat residual = R.magnitude(2)/R._height;
        const unsigned int _max_iter = max_iter?max_iter:dictionary._width;

        for (unsigned int iter = 0; iter<_max_iter && residual>max_residual; ++iter) {

          // Find best matching column from dictionary D.
          int best_atom = 0;
          Tfloat max_absdot = 0, max_dot = 0;
          cimg_pragma_openmp(parallel for
                             cimg_openmp_if(dictionary._width>=2 && dictionary._width*dictionary._height>=32))
          cimg_forX(dictionary,atom) {
            Tfloat dot = 0;
            cimg_forY(R,s) dot+=R[s]*dictionary(atom,s);
            dot/=dictionary_norm[atom];
            const Tfloat absdot = cimg::abs(dot);
            cimg_pragma_openmp(critical(get_project_matrix)) {
              if (absdot>max_absdot) { best_atom = atom; max_dot = dot; max_absdot = absdot; }
            }
          }

          if (!iter || method<3 || iter%proj_step) {
            // Matching Pursuit: Subtract component to signal.
            max_dot/=dictionary_norm[best_atom];
            weights(signal,best_atom)+=max_dot;
            residual = 0;
            cimg_forY(R,s) { R[s]-=max_dot*dictionary(best_atom,s); residual+=cimg::sqr(R[s]); }
            residual = std::sqrt(residual)/R._height;
            is_orthoproj = false;

          } else {
            // Orthogonal Matching Pursuit: Orthogonal projection step.
            weights(signal,best_atom) = 1; // Use only as a marker
            unsigned int nb_weights = 0;
            cimg_forY(weights,atom) if (weights(signal,atom)) ++nb_weights;
            CImg<Tfloat> sub_dictionary(nb_weights,dictionary._height);
            CImg<uintT> sub_atoms(nb_weights);
            int ind = 0;
            cimg_forY(weights,atom) if (weights(signal,atom)) {
              cimg_forY(sub_dictionary,s) sub_dictionary(ind,s) = dictionary(atom,s);
              sub_atoms[ind++] = atom;
            }
            const CImg<Tfloat> sub_weights = R0.get_solve(sub_dictionary);

            // Recompute residual signal according to the projected weights.
            R = R0;
            cimg_forY(sub_weights,sub_atom) {
              const Tfloat weight = sub_weights[sub_atom];
              const unsigned int atom = sub_atoms[sub_atom];
              weights(signal,atom) = weight;
              cimg_forY(R,s) R[s]-=weight*dictionary(atom,s);
            }
            residual = R.magnitude(2)/R._height;
            is_orthoproj = true;
          }
        }

        // Perform last orthoprojection step if needed.
        if (method>=2 && !is_orthoproj) {
          unsigned int nb_weights = 0;
          cimg_forY(weights,atom) if (weights(signal,atom)) ++nb_weights;
          if (nb_weights) { // Avoid degenerated case where 0 coefs are used
            CImg<Tfloat> sub_dictionary(nb_weights,dictionary._height);
            CImg<uintT> sub_atoms(nb_weights);
            int ind = 0;
            cimg_forY(weights,atom) if (weights(signal,atom)) {
              cimg_forY(sub_dictionary,s) sub_dictionary(ind,s) = dictionary(atom,s);
              sub_atoms[ind++] = atom;
            }
            const CImg<Tfloat> sub_weights = R0.get_solve(sub_dictionary);
            cimg_forY(sub_weights,sub_atom) weights(signal,sub_atoms[sub_atom]) = sub_weights[sub_atom];
          }
        }
      }
      return weights;
    }

    //! Compute minimal path in a graph, using the Dijkstra algorithm.
    /**
       \param distance An object having operator()(unsigned int i, unsigned int j) which returns distance
         between two nodes (i,j).
       \param nb_nodes Number of graph nodes.
       \param starting_node Index of the starting node.
       \param ending_node Index of the ending node (set to ~0U to ignore ending node).
       \param previous_node Array that gives the previous node index in the path to the starting node
         (optional parameter).
       \return Array of distances of each node to the starting node.
    **/
    template<typename tf, typename t>
    static CImg<T> dijkstra(const tf& distance, const unsigned int nb_nodes,
                            const unsigned int starting_node, const unsigned int ending_node,
                            CImg<t>& previous_node) {
      if (starting_node>=nb_nodes)
        throw CImgArgumentException("CImg<%s>::dijkstra(): Specified index of starting node %u is higher "
                                    "than number of nodes %u.",
                                    pixel_type(),starting_node,nb_nodes);
      CImg<T> dist(1,nb_nodes,1,1,cimg::type<T>::max());
      dist(starting_node) = 0;
      previous_node.assign(1,nb_nodes,1,1,(t)-1);
      previous_node(starting_node) = (t)starting_node;
      CImg<uintT> Q(nb_nodes);
      cimg_forX(Q,u) Q(u) = (unsigned int)u;
      cimg::swap(Q(starting_node),Q(0));
      unsigned int sizeQ = nb_nodes;
      while (sizeQ) {
        // Update neighbors from minimal vertex.
        const unsigned int umin = Q(0);
        if (umin==ending_node) sizeQ = 0;
        else {
          const T dmin = dist(umin);
          const T infty = cimg::type<T>::max();
          for (unsigned int q = 1; q<sizeQ; ++q) {
            const unsigned int v = Q(q);
            const T d = (T)distance(v,umin);
            if (d<infty) {
              const T alt = dmin + d;
              if (alt<dist(v)) {
                dist(v) = alt;
                previous_node(v) = (t)umin;
                const T distpos = dist(Q(q));
                for (unsigned int pos = q, par = 0; pos && distpos<dist(Q(par=(pos + 1)/2 - 1)); pos=par)
                  cimg::swap(Q(pos),Q(par));
              }
            }
          }
          // Remove minimal vertex from queue.
          Q(0) = Q(--sizeQ);
          const T distpos = dist(Q(0));
          for (unsigned int pos = 0, left = 0, right = 0;
               ((right=2*(pos + 1),(left=right - 1))<sizeQ && distpos>dist(Q(left))) ||
                 (right<sizeQ && distpos>dist(Q(right)));) {
            if (right<sizeQ) {
              if (dist(Q(left))<dist(Q(right))) { cimg::swap(Q(pos),Q(left)); pos = left; }
              else { cimg::swap(Q(pos),Q(right)); pos = right; }
            } else { cimg::swap(Q(pos),Q(left)); pos = left; }
          }
        }
      }
      return dist;
    }

    //! Return minimal path in a graph, using the Dijkstra algorithm.
    template<typename tf, typename t>
    static CImg<T> dijkstra(const tf& distance, const unsigned int nb_nodes,
                            const unsigned int starting_node, const unsigned int ending_node=~0U) {
      CImg<uintT> foo;
      return dijkstra(distance,nb_nodes,starting_node,ending_node,foo);
    }

    //! Return minimal path in a graph, using the Dijkstra algorithm.
    /**
       \param starting_node Index of the starting node.
       \param ending_node Index of the ending node.
       \param previous_node Array that gives the previous node index in the path to the starting node
         (optional parameter).
       \return Array of distances of each node to the starting node.
       \note image instance corresponds to the adjacency matrix of the graph.
    **/
    template<typename t>
    CImg<T>& dijkstra(const unsigned int starting_node, const unsigned int ending_node,
                      CImg<t>& previous_node) {
      return get_dijkstra(starting_node,ending_node,previous_node).move_to(*this);
    }

    //! Return minimal path in a graph, using the Dijkstra algorithm \newinstance.
    template<typename t>
    CImg<T> get_dijkstra(const unsigned int starting_node, const unsigned int ending_node,
                         CImg<t>& previous_node) const {
      if (_width!=_height || _depth!=1 || _spectrum!=1)
        throw CImgInstanceException(_cimg_instance
                                    "dijkstra(): Instance is not a graph adjacency matrix.",
                                    cimg_instance);

      return dijkstra(*this,_width,starting_node,ending_node,previous_node);
    }

    //! Return minimal path in a graph, using the Dijkstra algorithm.
    CImg<T>& dijkstra(const unsigned int starting_node, const unsigned int ending_node=~0U) {
      return get_dijkstra(starting_node,ending_node).move_to(*this);
    }

    //! Return minimal path in a graph, using the Dijkstra algorithm \newinstance.
    CImg<Tfloat> get_dijkstra(const unsigned int starting_node, const unsigned int ending_node=~0U) const {
      CImg<uintT> foo;
      return get_dijkstra(starting_node,ending_node,foo);
    }

    //! Return an image containing the character codes of specified string.
    /**
       \param str input C-string to encode as an image.
       \param is_last_zero Tells if the ending \c '0' character appear in the resulting image.
       \param is_shared Return result that shares its buffer with \p str.
    **/
    static CImg<T> string(const char *const str, const bool is_last_zero=true, const bool is_shared=false) {
      if (!str) return CImg<T>();
      return CImg<T>(str,(unsigned int)std::strlen(str) + (is_last_zero?1:0),1,1,1,is_shared);
    }

    //! Return a \c 1x1 image containing specified value.
    /**
       \param a0 First vector value.
    **/
    static CImg<T> row_vector(const T& a0) {
      return vector(a0);
    }

    //! Return a \c 2x1 image containing specified values.
    /**
       \param a0 First vector value.
       \param a1 Second vector value.
    **/
    static CImg<T> row_vector(const T& a0, const T& a1) {
      CImg<T> r(2,1);
      r[0] = a0; r[1] = a1;
      return r;
    }

    //! Return a \c 3x1 image containing specified values.
    /**
       \param a0 First vector value.
       \param a1 Second vector value.
       \param a2 Third vector value.
    **/
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2) {
      CImg<T> r(3,1);
      r[0] = a0; r[1] = a1; r[2] = a2;
      return r;
    }

    //! Return a \c 4x1 image containing specified values.
    /**
       \param a0 First vector value.
       \param a1 Second vector value.
       \param a2 Third vector value.
       \param a3 Fourth vector value.
    **/
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3) {
      CImg<T> r(4,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3;
      return r;
    }

    //! Return a \c 5x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4) {
      CImg<T> r(5,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4;
      return r;
    }

    //! Return a \c 6x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5) {
      CImg<T> r(6,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5;
      return r;
    }

    //! Return a \c 7x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6) {
      CImg<T> r(7,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6;
      return r;
    }

    //! Return a \c 8x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6, const T& a7) {
      CImg<T> r(8,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7;
      return r;
    }

    //! Return a \c 9x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6, const T& a7,
                              const T& a8) {
      CImg<T> r(9,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8;
      return r;
    }

    //! Return a \c 10x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6, const T& a7,
                              const T& a8, const T& a9) {
      CImg<T> r(10,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      return r;
    }

    //! Return a \c 11x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6, const T& a7,
                              const T& a8, const T& a9, const T& a10) {
      CImg<T> r(11,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10;
      return r;
    }

    //! Return a \c 12x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6, const T& a7,
                              const T& a8, const T& a9, const T& a10, const T& a11) {
      CImg<T> r(12,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11;
      return r;
    }

    //! Return a \c 13x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6, const T& a7,
                              const T& a8, const T& a9, const T& a10, const T& a11,
                              const T& a12) {
      CImg<T> r(13,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12;
      return r;
    }

    //! Return a \c 14x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6, const T& a7,
                              const T& a8, const T& a9, const T& a10, const T& a11,
                              const T& a12, const T& a13) {
      CImg<T> r(14,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13;
      return r;
    }

    //! Return a \c 15x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6, const T& a7,
                              const T& a8, const T& a9, const T& a10, const T& a11,
                              const T& a12, const T& a13, const T& a14) {
      CImg<T> r(15,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13; r[14] = a14;
      return r;
    }

    //! Return a \c 16x1 image containing specified values.
    static CImg<T> row_vector(const T& a0, const T& a1, const T& a2, const T& a3,
                              const T& a4, const T& a5, const T& a6, const T& a7,
                              const T& a8, const T& a9, const T& a10, const T& a11,
                              const T& a12, const T& a13, const T& a14, const T& a15) {
      CImg<T> r(16,1);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13; r[14] = a14; r[15] = a15;
      return r;
    }

    //! Return a \c 1x1 image containing specified value.
    /**
       \param a0 First vector value.
    **/
    static CImg<T> vector(const T& a0) {
      CImg<T> r(1,1);
      r[0] = a0;
      return r;
    }

    //! Return a \c 1x2 image containing specified values.
    /**
       \param a0 First vector value.
       \param a1 Second vector value.
    **/
    static CImg<T> vector(const T& a0, const T& a1) {
      CImg<T> r(1,2);
      r[0] = a0; r[1] = a1;
      return r;
    }

    //! Return a \c 1x3 image containing specified values.
    /**
       \param a0 First vector value.
       \param a1 Second vector value.
       \param a2 Third vector value.
    **/
    static CImg<T> vector(const T& a0, const T& a1, const T& a2) {
      CImg<T> r(1,3);
      r[0] = a0; r[1] = a1; r[2] = a2;
      return r;
    }

    //! Return a \c 1x4 image containing specified values.
    /**
       \param a0 First vector value.
       \param a1 Second vector value.
       \param a2 Third vector value.
       \param a3 Fourth vector value.
    **/
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3) {
      CImg<T> r(1,4);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3;
      return r;
    }

    //! Return a \c 1x5 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4) {
      CImg<T> r(1,5);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4;
      return r;
    }

    //! Return a \c 1x6 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5) {
      CImg<T> r(1,6);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5;
      return r;
    }

    //! Return a \c 1x7 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6) {
      CImg<T> r(1,7);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6;
      return r;
    }

    //! Return a \c 1x8 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7) {
      CImg<T> r(1,8);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7;
      return r;
    }

    //! Return a \c 1x9 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8) {
      CImg<T> r(1,9);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8;
      return r;
    }

    //! Return a \c 1x10 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9) {
      CImg<T> r(1,10);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      return r;
    }

    //! Return a \c 1x11 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10) {
      CImg<T> r(1,11);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10;
      return r;
    }

    //! Return a \c 1x12 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11) {
      CImg<T> r(1,12);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11;
      return r;
    }

    //! Return a \c 1x13 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12) {
      CImg<T> r(1,13);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12;
      return r;
    }

    //! Return a \c 1x14 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12, const T& a13) {
      CImg<T> r(1,14);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13;
      return r;
    }

    //! Return a \c 1x15 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12, const T& a13, const T& a14) {
      CImg<T> r(1,15);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13; r[14] = a14;
      return r;
    }

    //! Return a \c 1x16 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12, const T& a13, const T& a14, const T& a15) {
      CImg<T> r(1,16);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13; r[14] = a14; r[15] = a15;
      return r;
    }

    //! Return a \c 1x17 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12, const T& a13, const T& a14, const T& a15,
                          const T& a16) {
      CImg<T> r(1,17);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13; r[14] = a14; r[15] = a15; r[16] = a16;
      return r;
    }

    //! Return a \c 1x18 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12, const T& a13, const T& a14, const T& a15,
                          const T& a16, const T& a17) {
      CImg<T> r(1,18);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13; r[14] = a14; r[15] = a15; r[16] = a16; r[17] = a17;
      return r;
    }

    //! Return a \c 1x19 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12, const T& a13, const T& a14, const T& a15,
                          const T& a16, const T& a17, const T& a18) {
      CImg<T> r(1,19);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13; r[14] = a14; r[15] = a15; r[16] = a16; r[17] = a17;
      r[18] = a18;
      return r;
    }

    //! Return a \c 1x20 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12, const T& a13, const T& a14, const T& a15,
                          const T& a16, const T& a17, const T& a18, const T& a19) {
      CImg<T> r(1,20);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13; r[14] = a14; r[15] = a15; r[16] = a16; r[17] = a17;
      r[18] = a18; r[19] = a19;
      return r;
    }

    //! Return a \c 1x21 image containing specified values.
    static CImg<T> vector(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12, const T& a13, const T& a14, const T& a15,
                          const T& a16, const T& a17, const T& a18, const T& a19,
                          const T& a20) {
      CImg<T> r(1,21);
      r[0] = a0; r[1] = a1; r[2] = a2; r[3] = a3; r[4] = a4; r[5] = a5; r[6] = a6; r[7] = a7; r[8] = a8; r[9] = a9;
      r[10] = a10; r[11] = a11; r[12] = a12; r[13] = a13; r[14] = a14; r[15] = a15; r[16] = a16; r[17] = a17;
      r[18] = a18; r[19] = a19; r[20] = a20;
      return r;
    }

    //! Return a 1x1 matrix containing specified coefficients.
    /**
       \param a0 First matrix value.
       \note Equivalent to vector(const T&).
    **/
    static CImg<T> matrix(const T& a0) {
      return vector(a0);
    }

    //! Return a 2x2 matrix containing specified coefficients.
    /**
       \param a0 First matrix value.
       \param a1 Second matrix value.
       \param a2 Third matrix value.
       \param a3 Fourth matrix value.
    **/
    static CImg<T> matrix(const T& a0, const T& a1,
                          const T& a2, const T& a3) {
      CImg<T> r(2,2); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1;
      *(ptr++) = a2; *(ptr++) = a3;
      return r;
    }

    //! Return a 3x3 matrix containing specified coefficients.
    /**
       \param a0 First matrix value.
       \param a1 Second matrix value.
       \param a2 Third matrix value.
       \param a3 Fourth matrix value.
       \param a4 Fifth matrix value.
       \param a5 Sixth matrix value.
       \param a6 Seventh matrix value.
       \param a7 Eighth matrix value.
       \param a8 Ninth matrix value.
    **/
    static CImg<T> matrix(const T& a0, const T& a1, const T& a2,
                          const T& a3, const T& a4, const T& a5,
                          const T& a6, const T& a7, const T& a8) {
      CImg<T> r(3,3); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1; *(ptr++) = a2;
      *(ptr++) = a3; *(ptr++) = a4; *(ptr++) = a5;
      *(ptr++) = a6; *(ptr++) = a7; *(ptr++) = a8;
      return r;
    }

    //! Return a 4x4 matrix containing specified coefficients.
    static CImg<T> matrix(const T& a0, const T& a1, const T& a2, const T& a3,
                          const T& a4, const T& a5, const T& a6, const T& a7,
                          const T& a8, const T& a9, const T& a10, const T& a11,
                          const T& a12, const T& a13, const T& a14, const T& a15) {
      CImg<T> r(4,4); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1; *(ptr++) = a2; *(ptr++) = a3;
      *(ptr++) = a4; *(ptr++) = a5; *(ptr++) = a6; *(ptr++) = a7;
      *(ptr++) = a8; *(ptr++) = a9; *(ptr++) = a10; *(ptr++) = a11;
      *(ptr++) = a12; *(ptr++) = a13; *(ptr++) = a14; *(ptr++) = a15;
      return r;
    }

    //! Return a 5x5 matrix containing specified coefficients.
    static CImg<T> matrix(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4,
                          const T& a5, const T& a6, const T& a7, const T& a8, const T& a9,
                          const T& a10, const T& a11, const T& a12, const T& a13, const T& a14,
                          const T& a15, const T& a16, const T& a17, const T& a18, const T& a19,
                          const T& a20, const T& a21, const T& a22, const T& a23, const T& a24) {
      CImg<T> r(5,5); T *ptr = r._data;
      *(ptr++) = a0; *(ptr++) = a1; *(ptr++) = a2; *(ptr++) = a3; *(ptr++) = a4;
      *(ptr++) = a5; *(ptr++) = a6; *(ptr++) = a7; *(ptr++) = a8; *(ptr++) = a9;
      *(ptr++) = a10; *(ptr++) = a11; *(ptr++) = a12; *(ptr++) = a13; *(ptr++) = a14;
      *(ptr++) = a15; *(ptr++) = a16; *(ptr++) = a17; *(ptr++) = a18; *(ptr++) = a19;
      *(ptr++) = a20; *(ptr++) = a21; *(ptr++) = a22; *(ptr++) = a23; *(ptr++) = a24;
      return r;
    }

    //! Return a 1x1 symmetric matrix containing specified coefficients.
    /**
       \param a0 First matrix value.
       \note Equivalent to vector(const T&).
    **/
    static CImg<T> tensor(const T& a0) {
      return matrix(a0);
    }

    //! Return a 2x2 symmetric matrix tensor containing specified coefficients.
    static CImg<T> tensor(const T& a0, const T& a1, const T& a2) {
      return matrix(a0,a1,a1,a2);
    }

    //! Return a 3x3 symmetric matrix containing specified coefficients.
    static CImg<T> tensor(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5) {
      return matrix(a0,a1,a2,a1,a3,a4,a2,a4,a5);
    }

    //! Return a 1x1 diagonal matrix containing specified coefficients.
    static CImg<T> diagonal(const T& a0) {
      return matrix(a0);
    }

    //! Return a 2x2 diagonal matrix containing specified coefficients.
    static CImg<T> diagonal(const T& a0, const T& a1) {
      return matrix(a0,0,0,a1);
    }

    //! Return a 3x3 diagonal matrix containing specified coefficients.
    static CImg<T> diagonal(const T& a0, const T& a1, const T& a2) {
      return matrix(a0,0,0,0,a1,0,0,0,a2);
    }

    //! Return a 4x4 diagonal matrix containing specified coefficients.
    static CImg<T> diagonal(const T& a0, const T& a1, const T& a2, const T& a3) {
      return matrix(a0,0,0,0,0,a1,0,0,0,0,a2,0,0,0,0,a3);
    }

    //! Return a 5x5 diagonal matrix containing specified coefficients.
    static CImg<T> diagonal(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4) {
      return matrix(a0,0,0,0,0,0,a1,0,0,0,0,0,a2,0,0,0,0,0,a3,0,0,0,0,0,a4);
    }

    //! Return a NxN identity matrix.
    /**
       \param N Dimension of the matrix.
    **/
    static CImg<T> identity_matrix(const unsigned int N) {
      CImg<T> res(N,N,1,1,0);
      cimg_forX(res,x) res(x,x) = 1;
      return res;
    }

    //! Return a N-numbered sequence vector from \p a0 to \p a1.
    /**
       \param N Size of the resulting vector.
       \param a0 Starting value of the sequence.
       \param a1 Ending value of the sequence.
     **/
    static CImg<T> sequence(const unsigned int N, const T& a0, const T& a1) {
      if (N) return CImg<T>(1,N).sequence(a0,a1);
      return CImg<T>();
    }

    //! Return a 3x3 rotation matrix from an { axis + angle } or a quaternion.
    /**
       \param x X-coordinate of the rotation axis, or first quaternion coordinate.
       \param y Y-coordinate of the rotation axis, or second quaternion coordinate.
       \param z Z-coordinate of the rotation axis, or third quaternion coordinate.
       \param w Angle of the rotation axis (in degree), or fourth quaternion coordinate.
       \param is_quaternion Tell is the four arguments denotes a set { axis + angle } or a quaternion (x,y,z,w).
     **/
    static CImg<T> rotation_matrix(const float x, const float y, const float z, const float w,
                                   const bool is_quaternion=false) {
      double X, Y, Z, W, N;
      if (is_quaternion) {
        N = std::sqrt((double)x*x + (double)y*y + (double)z*z + (double)w*w);
        if (N>0) { X = x/N; Y = y/N; Z = z/N; W = w/N; }
        else { X = Y = Z = 0; W = 1; }
        return CImg<T>::matrix((T)(X*X + Y*Y - Z*Z - W*W),(T)(2*Y*Z - 2*X*W),(T)(2*X*Z + 2*Y*W),
                               (T)(2*X*W + 2*Y*Z),(T)(X*X - Y*Y + Z*Z - W*W),(T)(2*Z*W - 2*X*Y),
                               (T)(2*Y*W - 2*X*Z),(T)(2*X*Y + 2*Z*W),(T)(X*X - Y*Y - Z*Z + W*W));
      }
      N = cimg::hypot((double)x,(double)y,(double)z);
      if (N>0) { X = x/N; Y = y/N; Z = z/N; }
      else { X = Y = 0; Z = 1; }
      const double ang = w*cimg::PI/180, c = std::cos(ang), omc = 1 - c, s = std::sin(ang);
      return CImg<T>::matrix((T)(X*X*omc + c),(T)(X*Y*omc - Z*s),(T)(X*Z*omc + Y*s),
                             (T)(X*Y*omc + Z*s),(T)(Y*Y*omc + c),(T)(Y*Z*omc - X*s),
                             (T)(X*Z*omc - Y*s),(T)(Y*Z*omc + X*s),(T)(Z*Z*omc + c));
    }

    //@}
#endif
