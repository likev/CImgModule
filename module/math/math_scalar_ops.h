#ifndef CIMG_MODULE_MATH_SCALAR_OPS_H
#define CIMG_MODULE_MATH_SCALAR_OPS_H

    static const char _cimg_math_parser_check_operator[] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    //! Get/set default output stream for the \CImg library messages.
    /**
       \param file Desired output stream. Set to \c 0 to get the currently used output stream only.
       \return Currently used output stream.
    **/
    inline std::FILE* output(std::FILE *file) {
      cimg::mutex(1);
      static std::FILE *res = cimg::_stderr();
      if (file) res = file;
      cimg::mutex(1,0);
      return res;
    }

    // Return number of available CPU cores.
    inline unsigned int nb_cpus() {
      unsigned int res = 1;
#if cimg_OS==2
      SYSTEM_INFO sysinfo;
      GetSystemInfo(&sysinfo);
      res = (unsigned int)sysinfo.dwNumberOfProcessors;
#elif cimg_OS==1
      res = (unsigned int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
      return res?res:1U;
    }

    // Lock/unlock mutex for CImg multi-thread programming.
    inline int mutex(const unsigned int n, const int lock_mode) {
      switch (lock_mode) {
      case 0 : cimg::Mutex_attr::ref().unlock(n); return 0;
      case 1 : cimg::Mutex_attr::ref().lock(n); return 0;
      default : return cimg::Mutex_attr::ref().trylock(n);
      }
    }

    //! Display a warning message on the default output stream.
    /**
       \param format C-string containing the format of the message, as with <tt>std::printf()</tt>.
       \note If configuration macro \c cimg_strict_warnings is set, this function throws a
       \c CImgWarningException instead.
       \warning As the first argument is a format string, it is highly recommended to write
       \code
       cimg::warn("%s",warning_message);
       \endcode
       instead of
       \code
       cimg::warn(warning_message);
       \endcode
       if \c warning_message can be arbitrary, to prevent nasty memory access.
    **/
    inline void warn(const char *const format, ...) {
      if (cimg::exception_mode()>=1) {
        char *const message = new char[16384];
        std::va_list ap;
        va_start(ap,format);
        cimg_vsnprintf(message,16384,format,ap);
        va_end(ap);
#ifdef cimg_strict_warnings
        throw CImgWarningException(message);
#else
        std::fprintf(cimg::output(),"\n%s[CImg] *** Warning ***%s%s\n",cimg::t_red,cimg::t_normal,message);
#endif
        delete[] message;
      }
    }

    // Execute an external system command.
    /**
       \param command C-string containing the command line to execute.
       \param module_name Module name.
       \return Status value of the executed command, whose meaning is OS-dependent.
       \note This function is similar to <tt>std::system()</tt>
       but it does not open an extra console windows
       on Windows-based systems.
    **/
    inline int system(const char *const command, const char *const module_name=0, const bool is_verbose=false) {
      cimg::unused(module_name);
#ifdef cimg_no_system_calls
      cimg::unused(command,is_verbose);
      return -1;
#else

      if (is_verbose) return std::system(command);
#if cimg_OS==1
      const unsigned int l = (unsigned int)std::strlen(command);
      if (l) {
        char *const ncommand = new char[l + 24];
        std::memcpy(ncommand,command,l);
        std::strcpy(ncommand + l," >/dev/null 2>&1"); // Make command silent
        const int out_val = std::system(ncommand);
        delete[] ncommand;
        return out_val;
      } else return -1;
#elif cimg_OS==2
      PROCESS_INFORMATION pi;
      STARTUPINFOA si;
      std::memset(&pi,0,sizeof(PROCESS_INFORMATION));
      std::memset(&si,0,sizeof(STARTUPINFO));
      GetStartupInfoA(&si);
      si.cb = sizeof(si);
      si.wShowWindow = SW_HIDE;
      si.dwFlags |= SW_HIDE | STARTF_USESHOWWINDOW;
      const BOOL res = CreateProcessA((LPCSTR)module_name,(LPSTR)command,0,0,FALSE,0,0,0,&si,&pi);
      if (res) {
        WaitForSingleObject(pi.hProcess,INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return 0;
      } else {
        char* lpMsgBuf;

        // Get the error message.
        DWORD errorCode = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       0,errorCode,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPSTR)&lpMsgBuf,0,0);
        cimg::warn("cimg::system() : Command '%s' (module name '%s) failed with error %lu: %s",
                   module_name==0?"(null)":module_name,
                   command==0?"(null)":command,
                   errorCode,lpMsgBuf);
        return -1;
      }
#else
      return std::system(command);
#endif
#endif
    }

    //! Return a reference to a temporary variable of type T.
    template<typename T>
    inline T& temporary(const T&) {
      static T temp;
      return temp;
    }

    //! Exchange values of variables \c a and \c b.
    template<typename T>
    inline void swap(T& a, T& b) { T t = a; a = b; b = t; }

    //! Exchange values of variables (\c a1,\c a2) and (\c b1,\c b2).
    template<typename T1, typename T2>
    inline void swap(T1& a1, T1& b1, T2& a2, T2& b2) {
      cimg::swap(a1,b1); cimg::swap(a2,b2);
    }

    //! Exchange values of variables (\c a1,\c a2,\c a3) and (\c b1,\c b2,\c b3).
    template<typename T1, typename T2, typename T3>
    inline void swap(T1& a1, T1& b1, T2& a2, T2& b2, T3& a3, T3& b3) {
      cimg::swap(a1,b1,a2,b2); cimg::swap(a3,b3);
    }

    //! Exchange values of variables (\c a1,\c a2,...,\c a4) and (\c b1,\c b2,...,\c b4).
    template<typename T1, typename T2, typename T3, typename T4>
    inline void swap(T1& a1, T1& b1, T2& a2, T2& b2, T3& a3, T3& b3, T4& a4, T4& b4) {
      cimg::swap(a1,b1,a2,b2,a3,b3); cimg::swap(a4,b4);
    }

    //! Exchange values of variables (\c a1,\c a2,...,\c a5) and (\c b1,\c b2,...,\c b5).
    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    inline void swap(T1& a1, T1& b1, T2& a2, T2& b2, T3& a3, T3& b3, T4& a4, T4& b4, T5& a5, T5& b5) {
      cimg::swap(a1,b1,a2,b2,a3,b3,a4,b4); cimg::swap(a5,b5);
    }

    //! Exchange values of variables (\c a1,\c a2,...,\c a6) and (\c b1,\c b2,...,\c b6).
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline void swap(T1& a1, T1& b1, T2& a2, T2& b2, T3& a3, T3& b3, T4& a4, T4& b4, T5& a5, T5& b5, T6& a6, T6& b6) {
      cimg::swap(a1,b1,a2,b2,a3,b3,a4,b4,a5,b5); cimg::swap(a6,b6);
    }

    //! Exchange values of variables (\c a1,\c a2,...,\c a7) and (\c b1,\c b2,...,\c b7).
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    inline void swap(T1& a1, T1& b1, T2& a2, T2& b2, T3& a3, T3& b3, T4& a4, T4& b4, T5& a5, T5& b5, T6& a6, T6& b6,
                     T7& a7, T7& b7) {
      cimg::swap(a1,b1,a2,b2,a3,b3,a4,b4,a5,b5,a6,b6); cimg::swap(a7,b7);
    }

    //! Exchange values of variables (\c a1,\c a2,...,\c a8) and (\c b1,\c b2,...,\c b8).
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    inline void swap(T1& a1, T1& b1, T2& a2, T2& b2, T3& a3, T3& b3, T4& a4, T4& b4, T5& a5, T5& b5, T6& a6, T6& b6,
                     T7& a7, T7& b7, T8& a8, T8& b8) {
      cimg::swap(a1,b1,a2,b2,a3,b3,a4,b4,a5,b5,a6,b6,a7,b7); cimg::swap(a8,b8);
    }

    //! Return the endianness of the current architecture.
    /**
       \return \c false for <i>Little Endian</i> or \c true for <i>Big Endian</i>.
    **/
    inline bool endianness() {
      const int x = 1;
      return ((unsigned char*)&x)[0]?false:true;
    }

    //! Reverse endianness of all elements in a memory buffer.
    /**
       \param[in,out] buffer Memory buffer whose endianness must be reversed.
       \param size Number of buffer elements to reverse.
    **/
    template<typename T>
    inline void invert_endianness(T* const buffer, const cimg_ulong size) {
      if (size) switch (sizeof(T)) {
        case 1 : break;
        case 2 : {
          for (unsigned short *ptr = (unsigned short*)buffer + size; ptr>(unsigned short*)buffer; ) {
            const unsigned short val = *(--ptr);
            *ptr = (unsigned short)((val>>8) | ((val<<8)));
          }
        } break;
        case 4 : {
          for (unsigned int *ptr = (unsigned int*)buffer + size; ptr>(unsigned int*)buffer; ) {
            const unsigned int val = *(--ptr);
            *ptr = (val>>24) | ((val>>8)&0xff00) | ((val<<8)&0xff0000) | (val<<24);
          }
        } break;
        case 8 : {
          const cimg_uint64
            m0 = (cimg_uint64)0xff, m1 = m0<<8, m2 = m0<<16, m3 = m0<<24,
            m4 = m0<<32, m5 = m0<<40, m6 = m0<<48, m7 = m0<<56;
          for (cimg_uint64 *ptr = (cimg_uint64*)buffer + size; ptr>(cimg_uint64*)buffer; ) {
            const cimg_uint64 val = *(--ptr);
            *ptr =  (((val&m7)>>56) | ((val&m6)>>40) | ((val&m5)>>24) | ((val&m4)>>8) |
                     ((val&m3)<<8) |((val&m2)<<24) | ((val&m1)<<40) | ((val&m0)<<56));
          }
        } break;
        default : {
          for (T* ptr = buffer + size; ptr>buffer; ) {
            unsigned char *pb = (unsigned char*)(--ptr), *pe = pb + sizeof(T);
            for (int i = 0; i<(int)sizeof(T)/2; ++i) swap(*(pb++),*(--pe));
          }
        }
        }
    }
    inline void invert_endianness(bool* const, const cimg_ulong) {}
    inline void invert_endianness(unsigned char* const, const cimg_ulong) {}
    inline void invert_endianness(char* const, const cimg_ulong) {}

    //! Reverse endianness of a single variable.
    /**
       \param[in,out] a Variable to reverse.
       \return Reference to reversed variable.
    **/
    template<typename T>
    inline T& invert_endianness(T& a) {
      invert_endianness(&a,1);
      return a;
    }

    // Conversion functions to get more precision when trying to store 'unsigned int' values as 'float'.
    template<typename T>
    inline T uint2float(const unsigned int value, T) { // T is typically 'float' or 'float16'
      if (cimg::type<T>::string()==cimg::type<float>::string()) { // float32 version
        const unsigned int th = 1U<<24; // Consider 'float32' can safely store 'uint32' until 16777216
        if (value<th) return (float)value;
        float f;
        const unsigned int v = (value - th) | 0xC0000000; // Offset + set sign & first exponent bit to 1
        // use memcpy instead of simple assignment to avoid undesired optimizations by C++-compiler.
        std::memcpy(&f,&v,sizeof(float));
        return (T)f;
      }
#if cimg_is_float16
      else if (cimg::type<T>::string()==cimg::type<cimg_float16>::string()) { // float16 version
        const unsigned short th = 1U<<11; // Consider 'float16' can safely store 'uint16' until 2048
        if (value<th) return (float)value;
        cimg_float16 f;
        const unsigned short v = (value - th) | 0xC000; // Offset + set sign & first exponent bit to 1
        // use memcpy instead of simple assignment to avoid undesired optimizations by C++-compiler.
        std::memcpy((unsigned short*)&f,&v,sizeof(unsigned short));
        return (T)f;
      }
#endif
      return (unsigned int)value;
    }

    inline unsigned int float2uint(const float value) {
      const unsigned int th = 1U<<24; // Consider 'float32' can safely store 'uint32' until 16777216
      int tmp = 0;
      std::memcpy(&tmp,&value,sizeof(int));
      if (tmp>=0) return (unsigned int)value;
      unsigned int u;
      // use memcpy instead of assignment to avoid undesired optimizations by C++-compiler.
      std::memcpy(&u,&value,sizeof(unsigned int));
      return (u&0x3FFFFFFF) + th;
    }

#if cimg_is_float16
    inline unsigned int float2uint(const cimg_float16 value) {
      const unsigned short th = 1U<<11; // Consider 'float16' can safely store 'uint16' until 2048
      short tmp = 0;
      std::memcpy(&tmp,&value,sizeof(short));
      if (tmp>=0) return (unsigned short)value;
      unsigned short u;
      // use memcpy instead of assignment to avoid undesired optimizations by C++-compiler.
      std::memcpy(&u,&value,sizeof(unsigned short));
      return (u&0x3FFF) + th;
    }
#endif

    //! Return the value of a system timer, with a millisecond precision.
    /**
       \note The timer does not necessarily starts from \c 0.
    **/
    inline cimg_uint64 time() {
#if cimg_OS==1
      struct timeval st_time;
      gettimeofday(&st_time,0);
      return (cimg_uint64)st_time.tv_sec*1000 + (cimg_uint64)st_time.tv_usec/1000;
#elif cimg_OS==2
      ULARGE_INTEGER ul;
      FILETIME ft;
      GetSystemTimeAsFileTime(&ft);
      ul.LowPart = ft.dwLowDateTime;
      ul.HighPart = ft.dwHighDateTime;
      return (cimg_uint64)ul.QuadPart/10000;
#else
      return 0;
#endif
    }

    // Implement a tic/toc mechanism to display elapsed time of algorithms.
    inline cimg_uint64 tictoc(const bool is_tic);

    //! Start tic/toc timer for time measurement between code instructions.
    /**
       \return Current value of the timer (same value as time()).
    **/
    inline cimg_uint64 tic() {
      return cimg::tictoc(true);
    }

    //! End tic/toc timer and displays elapsed time from last call to tic().
    /**
       \return Time elapsed (in ms) since last call to tic().
    **/
    inline cimg_uint64 toc() {
      return cimg::tictoc(false);
    }

    //! Sleep for a given numbers of milliseconds.
    /**
       \param milliseconds Number of milliseconds to wait for.
       \note This function frees the CPU resources during the sleeping time.
       It can be used to temporize your program properly, without wasting CPU time.
    **/
    inline void sleep(const unsigned int milliseconds) {
#if cimg_OS==1
      struct timespec tv;
      tv.tv_sec = milliseconds/1000;
      tv.tv_nsec = (milliseconds%1000)*1000000;
      nanosleep(&tv,0);
#elif cimg_OS==2
      Sleep(milliseconds);
#else
      cimg::unused(milliseconds);
#endif
    }

    inline unsigned int wait(const unsigned int milliseconds, cimg_uint64 *const p_timer) {
      if (!*p_timer) *p_timer = cimg::time();
      const cimg_uint64 current_time = cimg::time();
      if (current_time<*p_timer || current_time>=*p_timer + milliseconds) { *p_timer = current_time; return 0; }
      const unsigned int time_diff = (unsigned int)(*p_timer + milliseconds - current_time);
      *p_timer = current_time + time_diff;
      cimg::sleep(time_diff);
      return time_diff;
    }

    //! Wait for a given number of milliseconds since the last call to wait().
    /**
       \param milliseconds Number of milliseconds to wait for.
       \return Number of milliseconds elapsed since the last call to wait().
       \note Same as sleep() with a waiting time computed with regard to the last call
       of wait(). It may be used to temporize your program properly, without wasting CPU time.
    **/
    inline unsigned int wait(const unsigned int milliseconds) {
      cimg::mutex(3);
      static cimg_uint64 timer = cimg::time();
      cimg::mutex(3,0);
      return cimg::wait(milliseconds,&timer);
    }

    // Custom random number generator (allow re-entrance).
    inline cimg_uint64& rng() { // Used as a shared global number for rng
      static cimg_uint64 rng = 0xB16B00B5U;
      return rng;
    }

    inline unsigned int _rand(cimg_uint64 *const p_rng) {
      *p_rng = *p_rng*1103515245 + 12345U;
      return (unsigned int)*p_rng;
    }

    inline unsigned int _rand() {
      cimg::mutex(4);
      const unsigned int res = cimg::_rand(&cimg::rng());
      cimg::mutex(4,0);
      return res;
    }

    inline void srand(cimg_uint64 *const p_rng) {
#if cimg_OS==1 || defined(__BORLANDC__)
      *p_rng = cimg::time() + (cimg_uint64)getpid();
#elif cimg_OS==2
      *p_rng = cimg::time() + (cimg_uint64)_getpid();
#else
      cimg::unused(p_rng);
#endif
    }

    inline void srand() {
      cimg::mutex(4);
      cimg::srand(&cimg::rng());
      cimg::mutex(4,0);
    }

    inline void srand(const cimg_uint64 seed) {
      cimg::mutex(4);
      cimg::rng() = seed;
      cimg::mutex(4,0);
    }

    inline double rand(const double val_min, const double val_max, cimg_uint64 *const p_rng) {
      return val_min + (val_max - val_min)*cimg::_rand(p_rng)/~0U;
    }

    inline double rand(const double val_min, const double val_max) {
      cimg::mutex(4);
      const double res = cimg::rand(val_min,val_max,&cimg::rng());
      cimg::mutex(4,0);
      return res;
    }

    inline double rand(const double val_max, cimg_uint64 *const p_rng) {
      return val_max*cimg::_rand(p_rng)/(double)~0U;
    }

    inline double rand(const double val_max=1) {
      cimg::mutex(4);
      const double res = cimg::rand(val_max,&cimg::rng());
      cimg::mutex(4,0);
      return res;
    }

    inline double grand(cimg_uint64 *const p_rng) {
      double x1, w;
      do {
        const double x2 = cimg::rand(-1,1,p_rng);
        x1 = cimg::rand(-1,1,p_rng);
        w = x1*x1 + x2*x2;
      } while (w<=0 || w>=1.);
      return x1*std::sqrt((-2*std::log(w))/w);
    }

    inline double grand() {
      cimg::mutex(4);
      const double res = cimg::grand(&cimg::rng());
      cimg::mutex(4,0);
      return res;
    }

    inline unsigned int prand(const double z, cimg_uint64 *const p_rng) {
      if (z<=1.e-10) return 0;
      if (z>100) return (unsigned int)((std::sqrt(z) * cimg::grand(p_rng)) + z);
      unsigned int k = 0;
      const double y = std::exp(-z);
      for (double s = 1.; s>=y; ++k) s*=cimg::rand(1,p_rng);
      return k - 1;
    }

    inline unsigned int prand(const double z) {
      cimg::mutex(4);
      const unsigned int res = cimg::prand(z,&cimg::rng());
      cimg::mutex(4,0);
      return res;
    }

    //! Bitwise-rotate value on the left.
    template<typename T>
    inline T rol(const T& a, const unsigned int n=1) {
      return n?(T)((a<<n)|(a>>((sizeof(T)<<3) - n))):a;
    }

    inline float rol(const float a, const unsigned int n=1) {
      return (float)rol((int)a,n);
    }

    inline double rol(const double a, const unsigned int n=1) {
      return (double)rol((cimg_long)a,n);
    }

    inline double rol(const long double a, const unsigned int n=1) {
      return (double)rol((cimg_long)a,n);
    }

#if cimg_is_float16==1
    inline cimg_float16 rol(const cimg_float16 a, const unsigned int n=1) {
      return (cimg_float16)rol((int)a,n);
    }
#endif

    //! Bitwise-rotate value on the right.
    template<typename T>
    inline T ror(const T& a, const unsigned int n=1) {
      return n?(T)((a>>n)|(a<<((sizeof(T)<<3) - n))):a;
    }

    inline float ror(const float a, const unsigned int n=1) {
      return (float)ror((int)a,n);
    }

    inline double ror(const double a, const unsigned int n=1) {
      return (double)ror((cimg_long)a,n);
    }

    inline double ror(const long double a, const unsigned int n=1) {
      return (double)ror((cimg_long)a,n);
    }

#if cimg_is_float16==1
    inline cimg_float16 ror(const cimg_float16 a, const unsigned int n=1) {
      return (cimg_float16)ror((int)a,n);
    }
#endif

    //! Return absolute value of a value.
    template<typename T>
    inline T abs(const T& a) {
      return a>=0?a:-a;
    }
    inline bool abs(const bool a) {
      return a;
    }
    inline int abs(const unsigned char a) {
      return (int)a;
    }
    inline int abs(const unsigned short a) {
      return (int)a;
    }
    inline int abs(const unsigned int a) {
      return (int)a;
    }
    inline int abs(const int a) {
      return std::abs(a);
    }
    inline cimg_int64 abs(const cimg_uint64 a) {
      return (cimg_int64)a;
    }
    inline double abs(const double a) {
      return std::fabs(a);
    }
    inline float abs(const float a) {
      return (float)std::fabs((double)a);
    }

    //! Return fractional part of a value.
    template<typename T>
    inline T frac(const T x) {
      return x - std::floor(x);
    }

    //! Return hyperbolic arcosine of a value.
    inline double acosh(const double x) {
#if cimg_use_cpp11==1 && !defined(_MSC_VER)
      return std::acosh(x);
#else
      return std::log(x + std::sqrt(x*x - 1));
#endif
    }

    //! Return hyperbolic arcsine of a value.
    inline double asinh(const double x) {
#if cimg_use_cpp11==1 && !defined(_MSC_VER)
      return std::asinh(x);
#else
      return std::log(x + std::sqrt(x*x + 1));
#endif
    }

    //! Return hyperbolic arctangent of a value.
    inline double atanh(const double x) {
#if cimg_use_cpp11==1 && !defined(_MSC_VER)
      return std::atanh(x);
#else
      return 0.5*std::log((1. + x)/(1. - x));
#endif
    }

    //! Return the sinc of a given value.
    inline double sinc(const double x) {
      return x?std::sin(x)/x:1;
    }

    //! Return base-2 logarithm of a value.
    inline double log2(const double x) {
#if cimg_use_cpp11==1 && !defined(_MSC_VER)
      return std::log2(x);
#else
      const double base2 = std::log(2.);
      return std::log(x)/base2;
#endif
    }

    //! Return square of a value.
    template<typename T>
    inline T sqr(const T& val) {
      return val*val;
    }

    // Return inverse of error function.
    template<typename T>
    inline T erfinv(const T& val) {
      const T
        sgn = val<0?-1:1,
        x = (1 - val)*(1 + val),
        lnx = std::log(x),
        tt1 = (T)(2/(cimg::PI*0.147) + 0.5*lnx),
        tt2 = lnx/(T)0.147;
      return sgn*std::sqrt(-tt1 + std::sqrt(tt1*tt1 - tt2));
    }

    //! Return cubic root of a value.
    template<typename T>
    inline double cbrt(const T& x) {
#if cimg_use_cpp11==1
      return std::cbrt(x);
#else
      return x>=0?std::pow((double)x,1./3):-std::pow(-(double)x,1./3);
#endif
    }

    template<typename T>
    inline T pow3(const T& val) {
      return val*val*val;
    }
    template<typename T>
    inline T pow4(const T& val) {
      return val*val*val*val;
    }

    //! Return the minimum between three values.
    template<typename t>
    inline t min(const t& a, const t& b, const t& c) {
      return std::min(std::min(a,b),c);
    }

    //! Return the minimum between four values.
    template<typename t>
    inline t min(const t& a, const t& b, const t& c, const t& d) {
      return std::min(std::min(a,b),std::min(c,d));
    }

    //! Return the minabs between two values.
    template<typename t>
    inline t minabs(const t& a, const t& b) {
      return cimg::abs(b)<cimg::abs(a)?b:a;
    }

    template<typename t>
    inline t minabs(const t& a, const t& b, const t& abs_b) {
      typedef typename cimg::superset<t,int>::type tint;
      return (tint)abs_b<(tint)cimg::abs(a)?b:a;
    }

    //! Return the maximum between three values.
    template<typename t>
    inline t max(const t& a, const t& b, const t& c) {
      return std::max(std::max(a,b),c);
    }

    //! Return the maximum between four values.
    template<typename t>
    inline t max(const t& a, const t& b, const t& c, const t& d) {
      return std::max(std::max(a,b),std::max(c,d));
    }

    //! Return the maxabs between two values.
    template<typename t>
    inline t maxabs(const t& a, const t& b) {
      return cimg::abs(b)>cimg::abs(a)?b:a;
    }

    template<typename t>
    inline t maxabs(const t& a, const t& b, const t& abs_b) {
      typedef typename cimg::superset<t,int>::type tint;
      return (tint)abs_b>(tint)cimg::abs(a)?b:a;
    }

    //! Return the sigmoid of a value.
    template<typename T>
    inline T sigmoid(const T& x) {
      return (T)((1 + std::tanh(x/2))/2);
    }

    //! Return the logit of a value.
    template<typename T>
    inline T logit(const T& x) {
      return (T)std::log(x/(1. - x));
    }

    //! Return the sign of a value.
    template<typename T>
    inline T sign(const T& x) {
      return (T)(cimg::type<T>::is_nan(x)?0:x<0?-1:x>0);
    }

    //! Cut (i.e. clamp) value in specified interval.
    template<typename T>
    inline T cut(const T& val, const T& val_min, const T& val_max) {
      return val<=val_min?val_min:val>=val_max?val_max:val;
    }

    //! Cut (i.e. clamp) absolute value in specified interval.
    template<typename T>
    inline T abscut(const T& val, const T& val_min, const T& val_max, const T& offset) {
      typedef typename cimg::superset<T,int>::type Tint;
      const T sgn = cimg::sign(val);
      return (T)cimg::cut((Tint)(val*sgn + offset),(Tint)val_min,(Tint)val_max)*sgn;
    }

    //! Return the nearest power of 2 higher than given value.
    template<typename T>
    inline cimg_uint64 nearest_pow2(const T& x) {
      cimg_uint64 i = 1;
      while (x>i) i<<=1;
      return i;
    }

    //! Return the modulo of a value.
    /**
       \param x Input value.
       \param m Modulo value.
       \note This modulo function accepts negative and floating-points modulo numbers, as well as variables of any type.
    **/
    template<typename T>
    inline T mod(const T& x, const T& m) {
      if (!m) {
        if (cimg::type<T>::is_float()) return cimg::type<T>::nan();
        else throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
      }
      const double dx = (double)x, dm = (double)m;
      if (!cimg::type<double>::is_finite(dm)) return x;
      if (cimg::type<double>::is_finite(dx)) return (T)(dx - dm * std::floor(dx / dm));
      return (T)0;
    }
    inline int mod(const bool x, const bool m) {
      if (!m) throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
      return x?1:0;
    }
    inline int mod(const unsigned char x, const unsigned char m) {
      if (!m) throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
      return x%m;
    }
    inline int mod(const char x, const char m) {
      if (!m) throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
#if defined(CHAR_MAX) && CHAR_MAX==255
      return x%m;
#else
      return x>=0?x%m:(x%m?m + x%m:0);
#endif
    }
    inline int mod(const unsigned short x, const unsigned short m) {
      if (!m) throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
      return (int)(x%m);
    }
    inline int mod(const short x, const short m) {
      if (!m) throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
      return (int)(x>=0?x%m:(x%m?m + x%m:0));
    }
    inline int mod(const unsigned int x, const unsigned int m) {
      if (!m) throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
      return (int)(x%m);
    }
    inline int mod(const int x, const int m) {
      if (!m) throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
      return (int)(x>=0?x%m:(x%m?m + x%m:0));
    }
    inline cimg_int64 mod(const cimg_uint64 x, const cimg_uint64 m) {
      if (!m) throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
      return (cimg_int64)(x%m);
    }
    inline cimg_int64 mod(const cimg_int64 x, const cimg_int64 m) {
      if (!m) throw CImgArgumentException("cimg::mod(): Specified modulo value is 0.");
      return (cimg_int64)(x>=0?x%m:(x%m?m + x%m:0));
    }

    //! Return the min-mod of two values.
    /**
       \note <i>minmod(\p a,\p b)</i> is defined to be:
       - <i>minmod(\p a,\p b) = min(\p a,\p b)</i>, if \p a and \p b have the same sign.
       - <i>minmod(\p a,\p b) = 0</i>, if \p a and \p b have different signs.
    **/
    template<typename T>
    inline T minmod(const T& a, const T& b) {
      return a*b<=0?0:(a>0?(a<b?a:b):(a<b?b:a));
    }

    template<typename T>
    inline T round(const T& x) {
      return (T)(cimg_int64)(x>=0?x + 0.5f:x - 0.5f);
    }

    template<typename T>
    inline int uiround(const T x) {
      return cimg::type<T>::is_float()?(int)(x + 0.5f):(int)x;
    }

    // Return 'round(a/b)' where 'a' and 'b>0' are integers. 'hb' must be equal to 'b/2' ("half b").
    // Particularity: When the fractional part of 'a/b' is 0.5, the rounding depends on the sign of 'a':
    // - If a>0, 'round_div(a/b) = ceil(a/b)'.
    // - If a<0, 'round_div(a.b) = floor(a/b)'.
    // (so, this is **not** a classical rounding behavior!).
    // This function is used by drawing methods, to get coherent rounded primitive coordinates.
    // Beware, 'b' must be strictly positive!
    template<typename T>
    T inline round_div(const T a, const T b, const T hb) {
      return b==1?a:a>=0?(a + hb)/b:-(hb - 1 - a)/b;
    }
    #define cimg_rd(a,b) cimg::round_div(a,b,h##b)

    //! Return rounded value.
    /**
       \param x Value to be rounded.
       \param y Rounding precision.
       \param rounding_type Type of rounding operation (\c 0 = nearest, \c -1 = backward, \c 1 = forward).
       \return Rounded value, having the same type as input value \c x.
    **/
    template<typename T>
    inline T round(const T& x, const double y, const int rounding_type=0) {
      if (y<=0) return x;
      if (y==1) switch (rounding_type) {
        case 0 : return cimg::round(x);
        case 1 : return (T)std::ceil((_cimg_Tfloat)x);
        default : return (T)std::floor((_cimg_Tfloat)x);
        }
      const double sx = (double)x/y, floor = std::floor(sx), delta =  sx - floor;
      return (T)(y*(rounding_type<0?floor:rounding_type>0?std::ceil(sx):delta<0.5?floor:std::ceil(sx)));
    }

    // Code to compute fast median from 2,3,5,7,9,13,25 and 49 values.
    // (contribution by RawTherapee: http://rawtherapee.com/).
    template<typename T>
    inline T median(T val0, T val1) {
      return (val0 + val1)/2;
    }

    template<typename T>
    inline T median(T val0, T val1, T val2) {
      return std::max(std::min(val0,val1),std::min(val2,std::max(val0,val1)));
    }

    template<typename T>
    inline T median(T val0, T val1, T val2, T val3) {
      const T
        tmp0 = cimg::min(val0,val1,val2,val3),
        tmp1 = cimg::max(val0,val1,val2,val3);
      return (val0 + val1 + val2 + val3 - tmp0 - tmp1)/2;
    }

    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4) {
      T tmp = std::min(val0,val1);
      val1 = std::max(val0,val1); val0 = tmp; tmp = std::min(val3,val4); val4 = std::max(val3,val4);
      val3 = std::max(val0,tmp);  val1 = std::min(val1,val4); tmp = std::min(val1,val2); val2 = std::max(val1,val2);
      val1 = tmp; tmp = std::min(val2,val3);
      return std::max(val1,tmp);
    }

    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4, T val5, T val6) {
      T tmp = std::min(val0,val5);
      val5 = std::max(val0,val5); val0 = tmp; tmp = std::min(val0,val3); val3 = std::max(val0,val3); val0 = tmp;
      tmp = std::min(val1,val6); val6 = std::max(val1,val6); val1 = tmp; tmp = std::min(val2,val4);
      val4 = std::max(val2,val4); val2 = tmp; val1 = std::max(val0,val1); tmp = std::min(val3,val5);
      val5 = std::max(val3,val5); val3 = tmp; tmp = std::min(val2,val6); val6 = std::max(val2,val6);
      val3 = std::max(tmp,val3); val3 = std::min(val3,val6); tmp = std::min(val4,val5); val4 = std::max(val1,tmp);
      tmp = std::min(val1,tmp); val3 = std::max(tmp,val3);
      return std::min(val3,val4);
    }

    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4, T val5, T val6, T val7, T val8) {
      T tmp = std::min(val1,val2);
      val2 = std::max(val1,val2); val1 = tmp; tmp = std::min(val4,val5);
      val5 = std::max(val4,val5); val4 = tmp; tmp = std::min(val7,val8);
      val8 = std::max(val7,val8); val7 = tmp; tmp = std::min(val0,val1);
      val1 = std::max(val0,val1); val0 = tmp; tmp = std::min(val3,val4);
      val4 = std::max(val3,val4); val3 = tmp; tmp = std::min(val6,val7);
      val7 = std::max(val6,val7); val6 = tmp; tmp = std::min(val1,val2);
      val2 = std::max(val1,val2); val1 = tmp; tmp = std::min(val4,val5);
      val5 = std::max(val4,val5); val4 = tmp; tmp = std::min(val7,val8);
      val8 = std::max(val7,val8); val3 = std::max(val0,val3); val5 = std::min(val5,val8);
      val7 = std::max(val4,tmp); tmp = std::min(val4,tmp); val6 = std::max(val3,val6);
      val4 = std::max(val1,tmp); val2 = std::min(val2,val5); val4 = std::min(val4,val7);
      tmp = std::min(val4,val2); val2 = std::max(val4,val2); val4 = std::max(val6,tmp);
      return std::min(val4,val2);
    }

    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4, T val5, T val6, T val7, T val8, T val9, T val10, T val11,
                    T val12) {
      T tmp = std::min(val1,val7);
      val7 = std::max(val1,val7); val1 = tmp; tmp = std::min(val9,val11); val11 = std::max(val9,val11); val9 = tmp;
      tmp = std::min(val3,val4);  val4 = std::max(val3,val4); val3 = tmp; tmp = std::min(val5,val8);
      val8 = std::max(val5,val8); val5 = tmp; tmp = std::min(val0,val12); val12 = std::max(val0,val12);
      val0 = tmp; tmp = std::min(val2,val6); val6 = std::max(val2,val6); val2 = tmp; tmp = std::min(val0,val1);
      val1 = std::max(val0,val1); val0 = tmp; tmp = std::min(val2,val3); val3 = std::max(val2,val3); val2 = tmp;
      tmp = std::min(val4,val6);  val6 = std::max(val4,val6); val4 = tmp; tmp = std::min(val8,val11);
      val11 = std::max(val8,val11); val8 = tmp; tmp = std::min(val7,val12); val12 = std::max(val7,val12); val7 = tmp;
      tmp = std::min(val5,val9); val9 = std::max(val5,val9); val5 = tmp; tmp = std::min(val0,val2);
      val2 = std::max(val0,val2); val0 = tmp; tmp = std::min(val3,val7); val7 = std::max(val3,val7); val3 = tmp;
      tmp = std::min(val10,val11); val11 = std::max(val10,val11); val10 = tmp; tmp = std::min(val1,val4);
      val4 = std::max(val1,val4); val1 = tmp; tmp = std::min(val6,val12); val12 = std::max(val6,val12); val6 = tmp;
      tmp = std::min(val7,val8); val8 = std::max(val7,val8); val7 = tmp; val11 = std::min(val11,val12);
      tmp = std::min(val4,val9); val9 = std::max(val4,val9); val4 = tmp; tmp = std::min(val6,val10);
      val10 = std::max(val6,val10); val6 = tmp; tmp = std::min(val3,val4); val4 = std::max(val3,val4); val3 = tmp;
      tmp = std::min(val5,val6); val6 = std::max(val5,val6); val5 = tmp; val8 = std::min(val8,val9);
      val10 = std::min(val10,val11); tmp = std::min(val1,val7); val7 = std::max(val1,val7); val1 = tmp;
      tmp = std::min(val2,val6); val6 = std::max(val2,val6); val2 = tmp; val3 = std::max(val1,val3);
      tmp = std::min(val4,val7); val7 = std::max(val4,val7); val4 = tmp; val8 = std::min(val8,val10);
      val5 = std::max(val0,val5); val5 = std::max(val2,val5); tmp = std::min(val6,val8); val8 = std::max(val6,val8);
      val5 = std::max(val3,val5); val7 = std::min(val7,val8); val6 = std::max(val4,tmp); tmp = std::min(val4,tmp);
      val5 = std::max(tmp,val5); val6 = std::min(val6,val7);
      return std::max(val5,val6);
    }

    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4,
                    T val5, T val6, T val7, T val8, T val9,
                    T val10, T val11, T val12, T val13, T val14,
                    T val15, T val16, T val17, T val18, T val19,
                    T val20, T val21, T val22, T val23, T val24) {
      T tmp = std::min(val0,val1);
      val1 = std::max(val0,val1); val0 = tmp; tmp = std::min(val3,val4); val4 = std::max(val3,val4);
      val3 = tmp; tmp = std::min(val2,val4); val4 = std::max(val2,val4); val2 = std::min(tmp,val3);
      val3 = std::max(tmp,val3); tmp = std::min(val6,val7); val7 = std::max(val6,val7); val6 = tmp;
      tmp = std::min(val5,val7); val7 = std::max(val5,val7); val5 = std::min(tmp,val6); val6 = std::max(tmp,val6);
      tmp = std::min(val9,val10); val10 = std::max(val9,val10); val9 = tmp; tmp = std::min(val8,val10);
      val10 = std::max(val8,val10); val8 = std::min(tmp,val9); val9 = std::max(tmp,val9);
      tmp = std::min(val12,val13); val13 = std::max(val12,val13); val12 = tmp; tmp = std::min(val11,val13);
      val13 = std::max(val11,val13); val11 = std::min(tmp,val12); val12 = std::max(tmp,val12);
      tmp = std::min(val15,val16); val16 = std::max(val15,val16); val15 = tmp; tmp = std::min(val14,val16);
      val16 = std::max(val14,val16); val14 = std::min(tmp,val15); val15 = std::max(tmp,val15);
      tmp = std::min(val18,val19); val19 = std::max(val18,val19); val18 = tmp; tmp = std::min(val17,val19);
      val19 = std::max(val17,val19); val17 = std::min(tmp,val18); val18 = std::max(tmp,val18);
      tmp = std::min(val21,val22); val22 = std::max(val21,val22); val21 = tmp; tmp = std::min(val20,val22);
      val22 = std::max(val20,val22); val20 = std::min(tmp,val21); val21 = std::max(tmp,val21);
      tmp = std::min(val23,val24); val24 = std::max(val23,val24); val23 = tmp; tmp = std::min(val2,val5);
      val5 = std::max(val2,val5); val2 = tmp; tmp = std::min(val3,val6); val6 = std::max(val3,val6); val3 = tmp;
      tmp = std::min(val0,val6); val6 = std::max(val0,val6); val0 = std::min(tmp,val3); val3 = std::max(tmp,val3);
      tmp = std::min(val4,val7); val7 = std::max(val4,val7); val4 = tmp; tmp = std::min(val1,val7);
      val7 = std::max(val1,val7); val1 = std::min(tmp,val4); val4 = std::max(tmp,val4); tmp = std::min(val11,val14);
      val14 = std::max(val11,val14); val11 = tmp; tmp = std::min(val8,val14); val14 = std::max(val8,val14);
      val8 = std::min(tmp,val11); val11 = std::max(tmp,val11); tmp = std::min(val12,val15);
      val15 = std::max(val12,val15); val12 = tmp; tmp = std::min(val9,val15); val15 = std::max(val9,val15);
      val9 = std::min(tmp,val12); val12 = std::max(tmp,val12); tmp = std::min(val13,val16);
      val16 = std::max(val13,val16); val13 = tmp; tmp = std::min(val10,val16); val16 = std::max(val10,val16);
      val10 = std::min(tmp,val13); val13 = std::max(tmp,val13); tmp = std::min(val20,val23);
      val23 = std::max(val20,val23); val20 = tmp; tmp = std::min(val17,val23); val23 = std::max(val17,val23);
      val17 = std::min(tmp,val20); val20 = std::max(tmp,val20); tmp = std::min(val21,val24);
      val24 = std::max(val21,val24); val21 = tmp; tmp = std::min(val18,val24); val24 = std::max(val18,val24);
      val18 = std::min(tmp,val21); val21 = std::max(tmp,val21); tmp = std::min(val19,val22);
      val22 = std::max(val19,val22); val19 = tmp; val17 = std::max(val8,val17); tmp = std::min(val9,val18);
      val18 = std::max(val9,val18); val9 = tmp; tmp = std::min(val0,val18); val18 = std::max(val0,val18);
      val9 = std::max(tmp,val9); tmp = std::min(val10,val19); val19 = std::max(val10,val19); val10 = tmp;
      tmp = std::min(val1,val19); val19 = std::max(val1,val19); val1 = std::min(tmp,val10);
      val10 = std::max(tmp,val10); tmp = std::min(val11,val20); val20 = std::max(val11,val20); val11 = tmp;
      tmp = std::min(val2,val20); val20 = std::max(val2,val20); val11 = std::max(tmp,val11);
      tmp = std::min(val12,val21); val21 = std::max(val12,val21); val12 = tmp; tmp = std::min(val3,val21);
      val21 = std::max(val3,val21); val3 = std::min(tmp,val12); val12 = std::max(tmp,val12);
      tmp = std::min(val13,val22); val22 = std::max(val13,val22); val4 = std::min(val4,val22);
      val13 = std::max(val4,tmp); tmp = std::min(val4,tmp); val4 = tmp; tmp = std::min(val14,val23);
      val23 = std::max(val14,val23); val14 = tmp; tmp = std::min(val5,val23); val23 = std::max(val5,val23);
      val5 = std::min(tmp,val14); val14 = std::max(tmp,val14); tmp = std::min(val15,val24);
      val24 = std::max(val15,val24); val15 = tmp; val6 = std::min(val6,val24); tmp = std::min(val6,val15);
      val15 = std::max(val6,val15); val6 = tmp; tmp = std::min(val7,val16); val7 = std::min(tmp,val19);
      tmp = std::min(val13,val21); val15 = std::min(val15,val23); tmp = std::min(val7,tmp);
      val7 = std::min(tmp,val15); val9 = std::max(val1,val9); val11 = std::max(val3,val11);
      val17 = std::max(val5,val17); val17 = std::max(val11,val17); val17 = std::max(val9,val17);
      tmp = std::min(val4,val10); val10 = std::max(val4,val10); val4 = tmp; tmp = std::min(val6,val12);
      val12 = std::max(val6,val12); val6 = tmp; tmp = std::min(val7,val14); val14 = std::max(val7,val14);
      val7 = tmp; tmp = std::min(val4,val6); val6 = std::max(val4,val6); val7 = std::max(tmp,val7);
      tmp = std::min(val12,val14); val14 = std::max(val12,val14); val12 = tmp; val10 = std::min(val10,val14);
      tmp = std::min(val6,val7); val7 = std::max(val6,val7); val6 = tmp; tmp = std::min(val10,val12);
      val12 = std::max(val10,val12); val10 = std::max(val6,tmp); tmp = std::min(val6,tmp);
      val17 = std::max(tmp,val17); tmp = std::min(val12,val17); val17 = std::max(val12,val17); val12 = tmp;
      val7 = std::min(val7,val17); tmp = std::min(val7,val10); val10 = std::max(val7,val10); val7 = tmp;
      tmp = std::min(val12,val18); val18 = std::max(val12,val18); val12 = std::max(val7,tmp);
      val10 = std::min(val10,val18); tmp = std::min(val12,val20); val20 = std::max(val12,val20); val12 = tmp;
      tmp = std::min(val10,val20);
      return std::max(tmp,val12);
    }

    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4, T val5, T val6,
                    T val7, T val8, T val9, T val10, T val11, T val12, T val13,
                    T val14, T val15, T val16, T val17, T val18, T val19, T val20,
                    T val21, T val22, T val23, T val24, T val25, T val26, T val27,
                    T val28, T val29, T val30, T val31, T val32, T val33, T val34,
                    T val35, T val36, T val37, T val38, T val39, T val40, T val41,
                    T val42, T val43, T val44, T val45, T val46, T val47, T val48) {
      T tmp = std::min(val0,val32);
      val32 = std::max(val0,val32); val0 = tmp; tmp = std::min(val1,val33); val33 = std::max(val1,val33); val1 = tmp;
      tmp = std::min(val2,val34); val34 = std::max(val2,val34); val2 = tmp; tmp = std::min(val3,val35);
      val35 = std::max(val3,val35); val3 = tmp; tmp = std::min(val4,val36); val36 = std::max(val4,val36); val4 = tmp;
      tmp = std::min(val5,val37); val37 = std::max(val5,val37); val5 = tmp; tmp = std::min(val6,val38);
      val38 = std::max(val6,val38); val6 = tmp; tmp = std::min(val7,val39); val39 = std::max(val7,val39); val7 = tmp;
      tmp = std::min(val8,val40); val40 = std::max(val8,val40); val8 = tmp; tmp = std::min(val9,val41);
      val41 = std::max(val9,val41); val9 = tmp; tmp = std::min(val10,val42); val42 = std::max(val10,val42);
      val10 = tmp; tmp = std::min(val11,val43); val43 = std::max(val11,val43); val11 = tmp;
      tmp = std::min(val12,val44); val44 = std::max(val12,val44); val12 = tmp; tmp = std::min(val13,val45);
      val45 = std::max(val13,val45); val13 = tmp; tmp = std::min(val14,val46); val46 = std::max(val14,val46);
      val14 = tmp; tmp = std::min(val15,val47); val47 = std::max(val15,val47); val15 = tmp;
      tmp = std::min(val16,val48); val48 = std::max(val16,val48); val16 = tmp; tmp = std::min(val0,val16);
      val16 = std::max(val0,val16); val0 = tmp; tmp = std::min(val1,val17); val17 = std::max(val1,val17);
      val1 = tmp; tmp = std::min(val2,val18); val18 = std::max(val2,val18); val2 = tmp; tmp = std::min(val3,val19);
      val19 = std::max(val3,val19); val3 = tmp; tmp = std::min(val4,val20); val20 = std::max(val4,val20); val4 = tmp;
      tmp = std::min(val5,val21); val21 = std::max(val5,val21); val5 = tmp; tmp = std::min(val6,val22);
      val22 = std::max(val6,val22); val6 = tmp; tmp = std::min(val7,val23); val23 = std::max(val7,val23); val7 = tmp;
      tmp = std::min(val8,val24); val24 = std::max(val8,val24); val8 = tmp; tmp = std::min(val9,val25);
      val25 = std::max(val9,val25); val9 = tmp; tmp = std::min(val10,val26); val26 = std::max(val10,val26);
      val10 = tmp; tmp = std::min(val11,val27); val27 = std::max(val11,val27); val11 = tmp;
      tmp = std::min(val12,val28); val28 = std::max(val12,val28); val12 = tmp; tmp = std::min(val13,val29);
      val29 = std::max(val13,val29); val13 = tmp; tmp = std::min(val14,val30); val30 = std::max(val14,val30);
      val14 = tmp; tmp = std::min(val15,val31); val31 = std::max(val15,val31); val15 = tmp;
      tmp = std::min(val32,val48); val48 = std::max(val32,val48); val32 = tmp; tmp = std::min(val16,val32);
      val32 = std::max(val16,val32); val16 = tmp; tmp = std::min(val17,val33); val33 = std::max(val17,val33);
      val17 = tmp; tmp = std::min(val18,val34); val34 = std::max(val18,val34); val18 = tmp;
      tmp = std::min(val19,val35); val35 = std::max(val19,val35); val19 = tmp; tmp = std::min(val20,val36);
      val36 = std::max(val20,val36); val20 = tmp; tmp = std::min(val21,val37); val37 = std::max(val21,val37);
      val21 = tmp; tmp = std::min(val22,val38); val38 = std::max(val22,val38); val22 = tmp;
      tmp = std::min(val23,val39); val39 = std::max(val23,val39); val23 = tmp; tmp = std::min(val24,val40);
      val40 = std::max(val24,val40); val24 = tmp; tmp = std::min(val25,val41); val41 = std::max(val25,val41);
      val25 = tmp; tmp = std::min(val26,val42); val42 = std::max(val26,val42); val26 = tmp;
      tmp = std::min(val27,val43); val43 = std::max(val27,val43); val27 = tmp; tmp = std::min(val28,val44);
      val44 = std::max(val28,val44); val28 = tmp; tmp = std::min(val29,val45); val45 = std::max(val29,val45);
      val29 = tmp; tmp = std::min(val30,val46); val46 = std::max(val30,val46); val30 = tmp;
      tmp = std::min(val31,val47); val47 = std::max(val31,val47); val31 = tmp; tmp = std::min(val0,val8);
      val8 = std::max(val0,val8); val0 = tmp; tmp = std::min(val1,val9); val9 = std::max(val1,val9); val1 = tmp;
      tmp = std::min(val2,val10); val10 = std::max(val2,val10); val2 = tmp; tmp = std::min(val3,val11);
      val11 = std::max(val3,val11); val3 = tmp; tmp = std::min(val4,val12); val12 = std::max(val4,val12); val4 = tmp;
      tmp = std::min(val5,val13); val13 = std::max(val5,val13); val5 = tmp; tmp = std::min(val6,val14);
      val14 = std::max(val6,val14); val6 = tmp; tmp = std::min(val7,val15); val15 = std::max(val7,val15); val7 = tmp;
      tmp = std::min(val16,val24); val24 = std::max(val16,val24); val16 = tmp; tmp = std::min(val17,val25);
      val25 = std::max(val17,val25); val17 = tmp; tmp = std::min(val18,val26); val26 = std::max(val18,val26);
      val18 = tmp; tmp = std::min(val19,val27); val27 = std::max(val19,val27); val19 = tmp;
      tmp = std::min(val20,val28); val28 = std::max(val20,val28); val20 = tmp; tmp = std::min(val21,val29);
      val29 = std::max(val21,val29); val21 = tmp; tmp = std::min(val22,val30); val30 = std::max(val22,val30);
      val22 = tmp; tmp = std::min(val23,val31); val31 = std::max(val23,val31); val23 = tmp;
      tmp = std::min(val32,val40); val40 = std::max(val32,val40); val32 = tmp; tmp = std::min(val33,val41);
      val41 = std::max(val33,val41); val33 = tmp; tmp = std::min(val34,val42); val42 = std::max(val34,val42);
      val34 = tmp; tmp = std::min(val35,val43); val43 = std::max(val35,val43); val35 = tmp;
      tmp = std::min(val36,val44); val44 = std::max(val36,val44); val36 = tmp; tmp = std::min(val37,val45);
      val45 = std::max(val37,val45); val37 = tmp; tmp = std::min(val38,val46); val46 = std::max(val38,val46);
      val38 = tmp; tmp = std::min(val39,val47); val47 = std::max(val39,val47); val39 = tmp;
      tmp = std::min(val8,val32); val32 = std::max(val8,val32); val8 = tmp; tmp = std::min(val9,val33);
      val33 = std::max(val9,val33); val9 = tmp; tmp = std::min(val10,val34); val34 = std::max(val10,val34);
      val10 = tmp; tmp = std::min(val11,val35); val35 = std::max(val11,val35); val11 = tmp;
      tmp = std::min(val12,val36); val36 = std::max(val12,val36); val12 = tmp; tmp = std::min(val13,val37);
      val37 = std::max(val13,val37); val13 = tmp; tmp = std::min(val14,val38); val38 = std::max(val14,val38);
      val14 = tmp; tmp = std::min(val15,val39); val39 = std::max(val15,val39); val15 = tmp;
      tmp = std::min(val24,val48); val48 = std::max(val24,val48); val24 = tmp; tmp = std::min(val8,val16);
      val16 = std::max(val8,val16); val8 = tmp; tmp = std::min(val9,val17); val17 = std::max(val9,val17);
      val9 = tmp; tmp = std::min(val10,val18); val18 = std::max(val10,val18); val10 = tmp;
      tmp = std::min(val11,val19); val19 = std::max(val11,val19); val11 = tmp; tmp = std::min(val12,val20);
      val20 = std::max(val12,val20); val12 = tmp; tmp = std::min(val13,val21); val21 = std::max(val13,val21);
      val13 = tmp; tmp = std::min(val14,val22); val22 = std::max(val14,val22); val14 = tmp;
      tmp = std::min(val15,val23); val23 = std::max(val15,val23); val15 = tmp; tmp = std::min(val24,val32);
      val32 = std::max(val24,val32); val24 = tmp; tmp = std::min(val25,val33); val33 = std::max(val25,val33);
      val25 = tmp; tmp = std::min(val26,val34); val34 = std::max(val26,val34); val26 = tmp;
      tmp = std::min(val27,val35); val35 = std::max(val27,val35); val27 = tmp; tmp = std::min(val28,val36);
      val36 = std::max(val28,val36); val28 = tmp; tmp = std::min(val29,val37); val37 = std::max(val29,val37);
      val29 = tmp; tmp = std::min(val30,val38); val38 = std::max(val30,val38); val30 = tmp;
      tmp = std::min(val31,val39); val39 = std::max(val31,val39); val31 = tmp; tmp = std::min(val40,val48);
      val48 = std::max(val40,val48); val40 = tmp; tmp = std::min(val0,val4); val4 = std::max(val0,val4);
      val0 = tmp; tmp = std::min(val1,val5); val5 = std::max(val1,val5); val1 = tmp; tmp = std::min(val2,val6);
      val6 = std::max(val2,val6); val2 = tmp; tmp = std::min(val3,val7); val7 = std::max(val3,val7); val3 = tmp;
      tmp = std::min(val8,val12); val12 = std::max(val8,val12); val8 = tmp; tmp = std::min(val9,val13);
      val13 = std::max(val9,val13); val9 = tmp; tmp = std::min(val10,val14); val14 = std::max(val10,val14);
      val10 = tmp; tmp = std::min(val11,val15); val15 = std::max(val11,val15); val11 = tmp;
      tmp = std::min(val16,val20); val20 = std::max(val16,val20); val16 = tmp; tmp = std::min(val17,val21);
      val21 = std::max(val17,val21); val17 = tmp; tmp = std::min(val18,val22); val22 = std::max(val18,val22);
      val18 = tmp; tmp = std::min(val19,val23); val23 = std::max(val19,val23); val19 = tmp;
      tmp = std::min(val24,val28); val28 = std::max(val24,val28); val24 = tmp; tmp = std::min(val25,val29);
      val29 = std::max(val25,val29); val25 = tmp; tmp = std::min(val26,val30); val30 = std::max(val26,val30);
      val26 = tmp; tmp = std::min(val27,val31); val31 = std::max(val27,val31); val27 = tmp;
      tmp = std::min(val32,val36); val36 = std::max(val32,val36); val32 = tmp; tmp = std::min(val33,val37);
      val37 = std::max(val33,val37); val33 = tmp; tmp = std::min(val34,val38); val38 = std::max(val34,val38);
      val34 = tmp; tmp = std::min(val35,val39); val39 = std::max(val35,val39); val35 = tmp;
      tmp = std::min(val40,val44); val44 = std::max(val40,val44); val40 = tmp; tmp = std::min(val41,val45);
      val45 = std::max(val41,val45); val41 = tmp; tmp = std::min(val42,val46); val46 = std::max(val42,val46);
      val42 = tmp; tmp = std::min(val43,val47); val47 = std::max(val43,val47); val43 = tmp;
      tmp = std::min(val4,val32); val32 = std::max(val4,val32); val4 = tmp; tmp = std::min(val5,val33);
      val33 = std::max(val5,val33); val5 = tmp; tmp = std::min(val6,val34); val34 = std::max(val6,val34);
      val6 = tmp; tmp = std::min(val7,val35); val35 = std::max(val7,val35); val7 = tmp;
      tmp = std::min(val12,val40); val40 = std::max(val12,val40); val12 = tmp; tmp = std::min(val13,val41);
      val41 = std::max(val13,val41); val13 = tmp; tmp = std::min(val14,val42); val42 = std::max(val14,val42);
      val14 = tmp; tmp = std::min(val15,val43); val43 = std::max(val15,val43); val15 = tmp;
      tmp = std::min(val20,val48); val48 = std::max(val20,val48); val20 = tmp; tmp = std::min(val4,val16);
      val16 = std::max(val4,val16); val4 = tmp; tmp = std::min(val5,val17); val17 = std::max(val5,val17);
      val5 = tmp; tmp = std::min(val6,val18); val18 = std::max(val6,val18); val6 = tmp;
      tmp = std::min(val7,val19); val19 = std::max(val7,val19); val7 = tmp; tmp = std::min(val12,val24);
      val24 = std::max(val12,val24); val12 = tmp; tmp = std::min(val13,val25); val25 = std::max(val13,val25);
      val13 = tmp; tmp = std::min(val14,val26); val26 = std::max(val14,val26); val14 = tmp;
      tmp = std::min(val15,val27); val27 = std::max(val15,val27); val15 = tmp; tmp = std::min(val20,val32);
      val32 = std::max(val20,val32); val20 = tmp; tmp = std::min(val21,val33); val33 = std::max(val21,val33);
      val21 = tmp; tmp = std::min(val22,val34); val34 = std::max(val22,val34); val22 = tmp;
      tmp = std::min(val23,val35); val35 = std::max(val23,val35); val23 = tmp; tmp = std::min(val28,val40);
      val40 = std::max(val28,val40); val28 = tmp; tmp = std::min(val29,val41); val41 = std::max(val29,val41);
      val29 = tmp; tmp = std::min(val30,val42); val42 = std::max(val30,val42); val30 = tmp;
      tmp = std::min(val31,val43); val43 = std::max(val31,val43); val31 = tmp; tmp = std::min(val36,val48);
      val48 = std::max(val36,val48); val36 = tmp; tmp = std::min(val4,val8); val8 = std::max(val4,val8);
      val4 = tmp; tmp = std::min(val5,val9); val9 = std::max(val5,val9); val5 = tmp; tmp = std::min(val6,val10);
      val10 = std::max(val6,val10); val6 = tmp; tmp = std::min(val7,val11); val11 = std::max(val7,val11); val7 = tmp;
      tmp = std::min(val12,val16); val16 = std::max(val12,val16); val12 = tmp; tmp = std::min(val13,val17);
      val17 = std::max(val13,val17); val13 = tmp; tmp = std::min(val14,val18); val18 = std::max(val14,val18);
      val14 = tmp; tmp = std::min(val15,val19); val19 = std::max(val15,val19); val15 = tmp;
      tmp = std::min(val20,val24); val24 = std::max(val20,val24); val20 = tmp; tmp = std::min(val21,val25);
      val25 = std::max(val21,val25); val21 = tmp; tmp = std::min(val22,val26); val26 = std::max(val22,val26);
      val22 = tmp; tmp = std::min(val23,val27); val27 = std::max(val23,val27); val23 = tmp;
      tmp = std::min(val28,val32); val32 = std::max(val28,val32); val28 = tmp; tmp = std::min(val29,val33);
      val33 = std::max(val29,val33); val29 = tmp; tmp = std::min(val30,val34); val34 = std::max(val30,val34);
      val30 = tmp; tmp = std::min(val31,val35); val35 = std::max(val31,val35); val31 = tmp;
      tmp = std::min(val36,val40); val40 = std::max(val36,val40); val36 = tmp; tmp = std::min(val37,val41);
      val41 = std::max(val37,val41); val37 = tmp; tmp = std::min(val38,val42); val42 = std::max(val38,val42);
      val38 = tmp; tmp = std::min(val39,val43); val43 = std::max(val39,val43); val39 = tmp;
      tmp = std::min(val44,val48); val48 = std::max(val44,val48); val44 = tmp; tmp = std::min(val0,val2);
      val2 = std::max(val0,val2); val0 = tmp; tmp = std::min(val1,val3); val3 = std::max(val1,val3); val1 = tmp;
      tmp = std::min(val4,val6); val6 = std::max(val4,val6); val4 = tmp; tmp = std::min(val5,val7);
      val7 = std::max(val5,val7); val5 = tmp; tmp = std::min(val8,val10); val10 = std::max(val8,val10); val8 = tmp;
      tmp = std::min(val9,val11); val11 = std::max(val9,val11); val9 = tmp; tmp = std::min(val12,val14);
      val14 = std::max(val12,val14); val12 = tmp; tmp = std::min(val13,val15); val15 = std::max(val13,val15);
      val13 = tmp; tmp = std::min(val16,val18); val18 = std::max(val16,val18); val16 = tmp;
      tmp = std::min(val17,val19); val19 = std::max(val17,val19); val17 = tmp; tmp = std::min(val20,val22);
      val22 = std::max(val20,val22); val20 = tmp; tmp = std::min(val21,val23); val23 = std::max(val21,val23);
      val21 = tmp; tmp = std::min(val24,val26); val26 = std::max(val24,val26); val24 = tmp;
      tmp = std::min(val25,val27); val27 = std::max(val25,val27); val25 = tmp; tmp = std::min(val28,val30);
      val30 = std::max(val28,val30); val28 = tmp; tmp = std::min(val29,val31); val31 = std::max(val29,val31);
      val29 = tmp; tmp = std::min(val32,val34); val34 = std::max(val32,val34); val32 = tmp;
      tmp = std::min(val33,val35); val35 = std::max(val33,val35); val33 = tmp; tmp = std::min(val36,val38);
      val38 = std::max(val36,val38); val36 = tmp; tmp = std::min(val37,val39); val39 = std::max(val37,val39);
      val37 = tmp; tmp = std::min(val40,val42); val42 = std::max(val40,val42); val40 = tmp;
      tmp = std::min(val41,val43); val43 = std::max(val41,val43); val41 = tmp; tmp = std::min(val44,val46);
      val46 = std::max(val44,val46); val44 = tmp; tmp = std::min(val45,val47); val47 = std::max(val45,val47);
      val45 = tmp; tmp = std::min(val2,val32); val32 = std::max(val2,val32); val2 = tmp; tmp = std::min(val3,val33);
      val33 = std::max(val3,val33); val3 = tmp; tmp = std::min(val6,val36); val36 = std::max(val6,val36); val6 = tmp;
      tmp = std::min(val7,val37); val37 = std::max(val7,val37); val7 = tmp; tmp = std::min(val10,val40);
      val40 = std::max(val10,val40); val10 = tmp; tmp = std::min(val11,val41); val41 = std::max(val11,val41);
      val11 = tmp; tmp = std::min(val14,val44); val44 = std::max(val14,val44); val14 = tmp;
      tmp = std::min(val15,val45); val45 = std::max(val15,val45); val15 = tmp; tmp = std::min(val18,val48);
      val48 = std::max(val18,val48); val18 = tmp; tmp = std::min(val2,val16); val16 = std::max(val2,val16);
      val2 = tmp; tmp = std::min(val3,val17); val17 = std::max(val3,val17); val3 = tmp;
      tmp = std::min(val6,val20); val20 = std::max(val6,val20); val6 = tmp; tmp = std::min(val7,val21);
      val21 = std::max(val7,val21); val7 = tmp; tmp = std::min(val10,val24); val24 = std::max(val10,val24);
      val10 = tmp; tmp = std::min(val11,val25); val25 = std::max(val11,val25); val11 = tmp;
      tmp = std::min(val14,val28); val28 = std::max(val14,val28); val14 = tmp; tmp = std::min(val15,val29);
      val29 = std::max(val15,val29); val15 = tmp; tmp = std::min(val18,val32); val32 = std::max(val18,val32);
      val18 = tmp; tmp = std::min(val19,val33); val33 = std::max(val19,val33); val19 = tmp;
      tmp = std::min(val22,val36); val36 = std::max(val22,val36); val22 = tmp; tmp = std::min(val23,val37);
      val37 = std::max(val23,val37); val23 = tmp; tmp = std::min(val26,val40); val40 = std::max(val26,val40);
      val26 = tmp; tmp = std::min(val27,val41); val41 = std::max(val27,val41); val27 = tmp;
      tmp = std::min(val30,val44); val44 = std::max(val30,val44); val30 = tmp; tmp = std::min(val31,val45);
      val45 = std::max(val31,val45); val31 = tmp; tmp = std::min(val34,val48); val48 = std::max(val34,val48);
      val34 = tmp; tmp = std::min(val2,val8); val8 = std::max(val2,val8); val2 = tmp; tmp = std::min(val3,val9);
      val9 = std::max(val3,val9); val3 = tmp; tmp = std::min(val6,val12); val12 = std::max(val6,val12); val6 = tmp;
      tmp = std::min(val7,val13); val13 = std::max(val7,val13); val7 = tmp; tmp = std::min(val10,val16);
      val16 = std::max(val10,val16); val10 = tmp; tmp = std::min(val11,val17); val17 = std::max(val11,val17);
      val11 = tmp; tmp = std::min(val14,val20); val20 = std::max(val14,val20); val14 = tmp;
      tmp = std::min(val15,val21); val21 = std::max(val15,val21); val15 = tmp; tmp = std::min(val18,val24);
      val24 = std::max(val18,val24); val18 = tmp; tmp = std::min(val19,val25); val25 = std::max(val19,val25);
      val19 = tmp; tmp = std::min(val22,val28); val28 = std::max(val22,val28); val22 = tmp;
      tmp = std::min(val23,val29); val29 = std::max(val23,val29); val23 = tmp; tmp = std::min(val26,val32);
      val32 = std::max(val26,val32); val26 = tmp; tmp = std::min(val27,val33); val33 = std::max(val27,val33);
      val27 = tmp; tmp = std::min(val30,val36); val36 = std::max(val30,val36); val30 = tmp;
      tmp = std::min(val31,val37); val37 = std::max(val31,val37); val31 = tmp; tmp = std::min(val34,val40);
      val40 = std::max(val34,val40); val34 = tmp; tmp = std::min(val35,val41); val41 = std::max(val35,val41);
      val35 = tmp; tmp = std::min(val38,val44); val44 = std::max(val38,val44); val38 = tmp;
      tmp = std::min(val39,val45); val45 = std::max(val39,val45); val39 = tmp; tmp = std::min(val42,val48);
      val48 = std::max(val42,val48); val42 = tmp; tmp = std::min(val2,val4); val4 = std::max(val2,val4);
      val2 = tmp; tmp = std::min(val3,val5); val5 = std::max(val3,val5); val3 = tmp; tmp = std::min(val6,val8);
      val8 = std::max(val6,val8); val6 = tmp; tmp = std::min(val7,val9); val9 = std::max(val7,val9); val7 = tmp;
      tmp = std::min(val10,val12); val12 = std::max(val10,val12); val10 = tmp; tmp = std::min(val11,val13);
      val13 = std::max(val11,val13); val11 = tmp; tmp = std::min(val14,val16); val16 = std::max(val14,val16);
      val14 = tmp; tmp = std::min(val15,val17); val17 = std::max(val15,val17); val15 = tmp;
      tmp = std::min(val18,val20); val20 = std::max(val18,val20); val18 = tmp; tmp = std::min(val19,val21);
      val21 = std::max(val19,val21); val19 = tmp; tmp = std::min(val22,val24); val24 = std::max(val22,val24);
      val22 = tmp; tmp = std::min(val23,val25); val25 = std::max(val23,val25); val23 = tmp;
      tmp = std::min(val26,val28); val28 = std::max(val26,val28); val26 = tmp; tmp = std::min(val27,val29);
      val29 = std::max(val27,val29); val27 = tmp; tmp = std::min(val30,val32); val32 = std::max(val30,val32);
      val30 = tmp; tmp = std::min(val31,val33); val33 = std::max(val31,val33); val31 = tmp;
      tmp = std::min(val34,val36); val36 = std::max(val34,val36); val34 = tmp; tmp = std::min(val35,val37);
      val37 = std::max(val35,val37); val35 = tmp; tmp = std::min(val38,val40); val40 = std::max(val38,val40);
      val38 = tmp; tmp = std::min(val39,val41); val41 = std::max(val39,val41); val39 = tmp;
      tmp = std::min(val42,val44); val44 = std::max(val42,val44); val42 = tmp; tmp = std::min(val43,val45);
      val45 = std::max(val43,val45); val43 = tmp; tmp = std::min(val46,val48); val48 = std::max(val46,val48);
      val46 = tmp; val1 = std::max(val0,val1); val3 = std::max(val2,val3); val5 = std::max(val4,val5);
      val7 = std::max(val6,val7); val9 = std::max(val8,val9); val11 = std::max(val10,val11);
      val13 = std::max(val12,val13); val15 = std::max(val14,val15); val17 = std::max(val16,val17);
      val19 = std::max(val18,val19); val21 = std::max(val20,val21); val23 = std::max(val22,val23);
      val24 = std::min(val24,val25); val26 = std::min(val26,val27); val28 = std::min(val28,val29);
      val30 = std::min(val30,val31); val32 = std::min(val32,val33); val34 = std::min(val34,val35);
      val36 = std::min(val36,val37); val38 = std::min(val38,val39); val40 = std::min(val40,val41);
      val42 = std::min(val42,val43); val44 = std::min(val44,val45); val46 = std::min(val46,val47);
      val32 = std::max(val1,val32); val34 = std::max(val3,val34); val36 = std::max(val5,val36);
      val38 = std::max(val7,val38); val9 = std::min(val9,val40); val11 = std::min(val11,val42);
      val13 = std::min(val13,val44); val15 = std::min(val15,val46); val17 = std::min(val17,val48);
      val24 = std::max(val9,val24); val26 = std::max(val11,val26); val28 = std::max(val13,val28);
      val30 = std::max(val15,val30); val17 = std::min(val17,val32); val19 = std::min(val19,val34);
      val21 = std::min(val21,val36); val23 = std::min(val23,val38); val24 = std::max(val17,val24);
      val26 = std::max(val19,val26); val21 = std::min(val21,val28); val23 = std::min(val23,val30);
      val24 = std::max(val21,val24); val23 = std::min(val23,val26);
      return std::max(val23,val24);
    }

    //! Return sqrt(x^2 + y^2).
    template<typename T>
    inline T hypot(const T x, const T y) {
#if cimg_use_cpp11==1 && !defined(_MSC_VER)
      return std::hypot(x,y);
#else
      return std::sqrt(x*x + y*y);
#endif
    }

    //! Return sqrt(x^2 + y^2 + z^2).
    template<typename T>
    inline T hypot(const T x, const T y, const T z) {
      return std::sqrt(x*x + y*y + z*z);
    }

    //! Return the factorial of n.
    inline double factorial(const int n) {
      if (n<0) return cimg::type<double>::nan();
      if (n<12) { const double tab[] = { 1,1,2,6,24,120,720,5040,40320,362880,3628800,39916800 }; return tab[n]; }
      double res = 2;
      for (int i = 3; i<=n; ++i) res*=i;
      return res;
    }

    //! Return the number of permutations of k objects in a set of n objects.
    inline double permutations(const int k, const int n, const bool with_order) {
      if (n<0 || k<0) return cimg::type<double>::nan();
      if (k>n) return 0;
      double res = 1;

      if (with_order) for (int i = n; i>=n - k + 1; --i) res*=i;
      else for (int i = 1; i<=k; ++i) { res*=(n - i + 1); res/=i; }
      return res;
    }

    inline double _fibonacci(int exp) {
      double
        base = (1 + std::sqrt(5.))/2,
        result = 1/std::sqrt(5.);
      while (exp) {
        if (exp&1) result*=base;
        exp>>=1;
        base*=base;
      }
      return result;
    }

    //! Calculate fibonacci number.
    // (Precise up to n = 78, less precise for n>78).
    inline double fibonacci(const int n) {
      if (n<0) return cimg::type<double>::nan();
      if (n<16) { const double tab[] = { 0,1,1,2,3,5,8,13,21,34,55,89,144,233,377,610 }; return tab[n]; }
      if (n<75) // precise up to n = 74, faster than the integer calculation above for n>10
        return (double)((cimg_uint64)(_fibonacci(n) + 0.5));
      if (n<94) { // precise up to n = 78, less precise for n>78 up to n = 93, overflows for n>93
        cimg_uint64
          fn1 = ((cimg_uint64)303836)<<32 | 3861581201UL, // 1304969544928657ULL (avoid C++98 warning with ULL)
          fn2 = ((cimg_uint64)187781)<<32 | 2279239217UL, // 806515533049393ULL
          fn = 0;
        for (int i = 75; i<=n; ++i) { fn = fn1 + fn2; fn2 = fn1; fn1 = fn; }
        return (double)fn;
      }
      return _fibonacci(n); // Not precise, but better than the wrong overflowing calculation
    }

    //! Wave function.
    /**
       \param x Value to evaluate.
       \param type Wave type.
       Can be { 0:Square | 1:Triangular | 2:Ascending sawtooth | 3: Descending sawtooth | 4:Sinusoidal }.
       \note A wave function has a period of 1, and has value in [-1,1].
       \return Function value.
    **/
    inline double wave(const double x, const unsigned int type=4) {
      const double p = cimg::frac(x);
      double res = 0;
      switch (type) {
      case 0 : // Square
        res = p<0.5?1:-1;
        break;
      case 1 : // Triangle
        res = p<0.25?4*p:p>0.75?4*(p - 1):1 - 4*(p - 0.25);
        break;
      case 2 : // Ascending sawtooth
        res = 2*(p - 0.5);
        break;
      case 3 : // Descending sawtooth
        res = -2*(p - 0.5);
        break;
      default : // Sine
        res = std::sin(2*cimg::PI*p);
      }
      return res;
    }

    //! Calculate greatest common divisor of two integers.
    template<typename T>
    inline T gcd(T a, T b) {
      if (a<0) a = -a;
      if (b<0) b = -b;
      while (a) { const T c = a; a = b%a; b = c; }
      return b;
    }

    //! Calculate least common multiple of two integers.
    template<typename T>
    inline T lcm(T a, T b) {
      if (a<0) a = -a;
      if (!a && !b) return 0;
      return a*(b/gcd(a,b));
    }

    //! Convert character to lower case.
    inline char lowercase(const char x) {
      return (char)((x<'A'||x>'Z')?x:x - 'A' + 'a');
    }
    inline double lowercase(const double x) {
      return (double)((x<'A'||x>'Z')?x:x - 'A' + 'a');
    }

    //! Convert C-string to lower case.
    inline void lowercase(char *const str) {
      if (str) for (char *ptr = str; *ptr; ++ptr) *ptr = lowercase(*ptr);
    }

    //! Convert character to upper case.
    inline char uppercase(const char x) {
      return (char)((x<'a'||x>'z')?x:x - 'a' + 'A');
    }

    inline double uppercase(const double x) {
      return (double)((x<'a'||x>'z')?x:x - 'a' + 'A');
    }

    //! Convert C-string to upper case.
    inline void uppercase(char *const str) {
      if (str) for (char *ptr = str; *ptr; ++ptr) *ptr = uppercase(*ptr);
    }

    //! Return \c true if input character is blank (space, tab, or non-printable character).
    inline bool is_blank(const char c) {
      return (unsigned char)c<=' ';
    }

    // Return \c true if specified argument is in set \c [a-zA-Z0-9_].
    inline bool is_varchar(const char c) {
      return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_';
    }

    //! Return \c true if argument \p str can be considered as a regular variable name.
    inline bool is_varname(const char *const str, const unsigned int length=~0U) {
      if (*str>='0' && *str<='9') return false;
      for (unsigned int l = 0; l<length && str[l]; ++l)
        if (!is_varchar(str[l])) return false;
      return true;
    }

    //! Read value in a C-string.
    /**
       \param str C-string containing the float value to read.
       \return Read value.
       \note Same as <tt>std::atof()</tt> extended to manage the retrieval of fractions from C-strings,
       as in <em>"1/2"</em>.
    **/
    inline double atof(const char *const str) {
      double x = 0, y = 1;
      return str && cimg_sscanf(str,"%lf/%lf",&x,&y)>0?x/y:0;
    }

    //! Compare the first \p length characters of two C-strings, ignoring the case.
    /**
       \param str1 C-string.
       \param str2 C-string.
       \param length Number of characters to compare.
       \return \c 0 if the two strings are equal, something else otherwise.
       \note This function has to be defined since it is not provided by all C++-compilers (not ANSI).
    **/
    inline int strncasecmp(const char *const str1, const char *const str2, const int length) {
      if (!length) return 0;
      if (!str1) return str2?-1:0;
      const char *nstr1 = str1, *nstr2 = str2;
      int k, diff = 0;
      for (k = 0; k<length && !(diff = lowercase(*nstr1) - lowercase(*nstr2)); ++k) { ++nstr1; ++nstr2; }
      return k!=length?diff:0;
    }

    //! Compare two C-strings, ignoring the case.
    /**
       \param str1 C-string.
       \param str2 C-string.
       \return \c 0 if the two strings are equal, something else otherwise.
       \note This function has to be defined since it is not provided by all C++-compilers (not ANSI).
    **/
    inline int strcasecmp(const char *const str1, const char *const str2) {
      if (!str1) return str2?-1:0;
      const int
        l1 = (int)std::strlen(str1),
        l2 = (int)std::strlen(str2);
      return cimg::strncasecmp(str1,str2,1 + (l1<l2?l1:l2));
    }

    //! Ellipsize a string.
    /**
       \param str C-string.
       \param l Max number of printed characters.
       \param is_ending Tell if the dots are placed at the end or at the center of the ellipsized string.
    **/
    inline char *strellipsize(char *const str, const unsigned int l=64,
                              const bool is_ending=true) {
      if (!str) return str;
      const unsigned int nl = l<5?5:l, ls = (unsigned int)std::strlen(str);
      if (ls<=nl) return str;
      if (is_ending) std::strcpy(str + nl - 5,"(...)");
      else {
        const unsigned int ll = (nl - 5)/2 + 1 - (nl%2), lr = nl - ll - 5;
        std::strcpy(str + ll,"(...)");
        std::memmove(str + ll + 5,str + ls - lr,lr);
      }
      str[nl] = 0;
      return str;
    }

    //! Ellipsize a string.
    /**
       \param str C-string.
       \param res output C-string.
       \param l Max number of printed characters. String 'res' must be a size of at least 'l+1'.
       \param is_ending Tell if the dots are placed at the end or at the center of the ellipsized string.
    **/
    inline char *strellipsize(const char *const str, char *const res, const unsigned int l=64,
                              const bool is_ending=true) {
      const unsigned int nl = l<5?5:l, ls = (unsigned int)std::strlen(str);
      if (ls<=nl) { std::strcpy(res,str); return res; }
      if (is_ending) {
        std::strncpy(res,str,nl - 5);
        std::strcpy(res + nl -5,"(...)");
      } else {
        const unsigned int ll = (nl - 5)/2 + 1 - (nl%2), lr = nl - ll - 5;
        std::strncpy(res,str,ll);
        std::strcpy(res + ll,"(...)");
        std::strncpy(res + ll + 5,str + ls - lr,lr);
      }
      res[nl] = 0;
      return res;
    }

    //! Remove delimiters on the start and/or end of a C-string.
    /**
       \param[in,out] str C-string to work with (modified at output).
       \param delimiter Delimiter character code to remove.
       \param is_symmetric Tells if the removal is done only if delimiters are symmetric
       (both at the beginning and the end of \c s).
       \param is_iterative Tells if the removal is done if several iterations are possible.
       \return \c true if delimiters have been removed, \c false otherwise.
   **/
    inline bool strpare(char *const str, const char delimiter,
                        const bool is_symmetric, const bool is_iterative) {
      if (!str) return false;
      const int l = (int)std::strlen(str);
      int p, q;
      if (is_symmetric) for (p = 0, q = l - 1; p<q && str[p]==delimiter && str[q]==delimiter; ) {
          --q; ++p; if (!is_iterative) break;
        } else {
        for (p = 0; p<l && str[p]==delimiter; ) { ++p; if (!is_iterative) break; }
        for (q = l - 1; q>p && str[q]==delimiter; ) { --q; if (!is_iterative) break; }
      }
      const int n = q - p + 1;
      if (n!=l) { std::memmove(str,str + p,(unsigned int)n); str[n] = 0; return true; }
      return false;
    }

    //! Remove white spaces on the start and/or end of a C-string.
    inline bool strpare(char *const str, const bool is_symmetric, const bool is_iterative) {
      if (!str) return false;
      const int l = (int)std::strlen(str);
      int p, q;
      if (is_symmetric) for (p = 0, q = l - 1; p<q && is_blank(str[p]) && is_blank(str[q]); ) {
          --q; ++p; if (!is_iterative) break;
        } else {
        for (p = 0; p<l && is_blank(str[p]); ) { ++p; if (!is_iterative) break; }
        for (q = l - 1; q>p && is_blank(str[q]); ) { --q; if (!is_iterative) break; }
      }
      const int n = q - p + 1;
      if (n!=l) { std::memmove(str,str + p,(unsigned int)n); str[n] = 0; return true; }
      return false;
    }

    //! Replace reserved characters (for Windows filename) by another character.
    /**
       \param[in,out] str C-string to work with (modified at output).
       \param[in] c Replacement character.
    **/
    inline void strwindows_reserved(char *const str, const char c='_') {
      for (char *s = str; *s; ++s) {
        const char i = *s;
        if (i=='<' || i=='>' || i==':' || i=='\"' || i=='/' || i=='\\' || i=='|' || i=='?' || i=='*') *s = c;
      }
    }

    //! Replace escape sequences in C-strings by character values.
    /**
       \param[in,out] str C-string to work with (modified at output).
    **/
    inline void strunescape(char *const str) {
#define cimg_strunescape(ci,co) case ci : *nd = co; ++ns; break;

      unsigned char val = 0;
      for (char *ns = str, *nd = str; *ns || (bool)(*nd = 0); ++nd) if (*ns=='\\') switch (*(++ns)) {
            cimg_strunescape('a','\a');
            cimg_strunescape('b','\b');
            cimg_strunescape('e',0x1B);
            cimg_strunescape('f','\f');
            cimg_strunescape('n','\n');
            cimg_strunescape('r','\r');
            cimg_strunescape('t','\t');
            cimg_strunescape('v','\v');
            cimg_strunescape('\\','\\');
            cimg_strunescape('\'','\'');
            cimg_strunescape('\"','\"');
            cimg_strunescape('\?','\?');
          case '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' :
            val = (unsigned char)(*(ns++) - '0');
            if (*ns>='0' && *ns<='7') (val<<=3)|=*(ns++) - '0';
            if (*ns>='0' && *ns<='7') (val<<=3)|=*(ns++) - '0';
            *nd = (char)val;
            break;
          case 'x' : {
            char c = lowercase(*(++ns));
            if ((c>='0' && c<='9') || (c>='a' && c<='f')) {
              val = (unsigned char)(c<='9'?c - '0':c - 'a' + 10);
              c = lowercase(*(++ns));
              if ((c>='0' && c<='9') || (c>='a' && c<='f')) {
                (val<<=4)|=(c<='9'?c - '0':c - 'a' + 10);
                ++ns;
              }
              *nd = (char)val;
            } else *nd = c;
          } break;
          case 'u' : { // UTF-8 BMP
            char c1, c2, c3, c4;
            if ((((c1 = lowercase(ns[1]))>='0' && c1<='9') || (c1>='a' && c1<='f')) &&
                (((c2 = lowercase(ns[2]))>='0' && c2<='9') || (c2>='a' && c2<='f')) &&
                (((c3 = lowercase(ns[3]))>='0' && c3<='9') || (c3>='a' && c3<='f')) &&
                (((c4 = lowercase(ns[4]))>='0' && c4<='9') || (c4>='a' && c4<='f'))) {
              c1 = (c1<='9'?c1 - '0':c1 - 'a' + 10);
              c2 = (c2<='9'?c2 - '0':c2 - 'a' + 10);
              c3 = (c3<='9'?c3 - '0':c3 - 'a' + 10);
              c4 = (c4<='9'?c4 - '0':c4 - 'a' + 10);
              const unsigned int ival =
                ((unsigned int)c1<<12) | ((unsigned int)c2<<8) | ((unsigned int)c3<<4) | c4;
              if (ival<=0x007f) *nd = (char)ival;
              else if (ival<=0x07ff) {
                *(nd++) = (char)((ival>>6)|0xc0);
                *nd = (char)((ival&0x3f)|0x80);
              } else {
                *(nd++) = (char)((ival>>12)|0xe0);
                *(nd++) = (char)(((ival>>6)&0x3f)|0x80);
                *nd = (char)((ival&0x3f)|0x80);
              }
              ns+=5;
            } else *nd = *(ns++);
          } break;
          case 'U' : { // UTF-8 astral planes
            char c1, c2, c3, c4, c5, c6, c7, c8;
            if ((((c1 = lowercase(ns[1]))>='0' && c1<='9') || (c1>='a' && c1<='f')) &&
                (((c2 = lowercase(ns[2]))>='0' && c2<='9') || (c2>='a' && c2<='f')) &&
                (((c3 = lowercase(ns[3]))>='0' && c3<='9') || (c3>='a' && c3<='f')) &&
                (((c4 = lowercase(ns[4]))>='0' && c4<='9') || (c4>='a' && c4<='f')) &&
                (((c5 = lowercase(ns[5]))>='0' && c5<='9') || (c5>='a' && c5<='f')) &&
                (((c6 = lowercase(ns[6]))>='0' && c6<='9') || (c6>='a' && c6<='f')) &&
                (((c7 = lowercase(ns[7]))>='0' && c7<='9') || (c7>='a' && c7<='f')) &&
                (((c8 = lowercase(ns[8]))>='0' && c8<='9') || (c8>='a' && c8<='f'))) {
              c1 = (c1<='9'?c1 - '0':c1 - 'a' + 10);
              c2 = (c2<='9'?c2 - '0':c2 - 'a' + 10);
              c3 = (c3<='9'?c3 - '0':c3 - 'a' + 10);
              c4 = (c4<='9'?c4 - '0':c4 - 'a' + 10);
              c5 = (c5<='9'?c5 - '0':c5 - 'a' + 10);
              c6 = (c6<='9'?c6 - '0':c6 - 'a' + 10);
              c7 = (c7<='9'?c7 - '0':c7 - 'a' + 10);
              c8 = (c8<='9'?c8 - '0':c8 - 'a' + 10);
              const unsigned int ival =
                ((unsigned int)c1<<28) | ((unsigned int)c2<<24) | ((unsigned int)c3<<20) | ((unsigned int)c4<<16) |
                ((unsigned int)c5<<12) | ((unsigned int)c6<<8) | ((unsigned int)c7<<4) | (unsigned int)c8;
              if (ival<=0x007f) *nd = (char)ival;
              else if (ival<=0x07ff) {
                *(nd++) = (char)((ival>>6)|0xc0);
                *nd = (char)((ival&0x3f)|0x80);
              } else if (ival<=0xffff) {
                *(nd++) = (char)((ival>>12)|0xe0);
                *(nd++) = (char)(((ival>>6)&0x3f)|0x80);
                *nd = (char)((ival&0x3f)|0x80);
              } else {
                *(nd++) = (char)((ival>>18)|0xf0);
                *(nd++) = (char)(((ival>>12)&0x3f)|0x80);
                *(nd++) = (char)(((ival>>6)&0x3f)|0x80);
                *nd = (char)((ival&0x3f)|0x80);
              }
              ns+=9;
            } else *nd = *(ns++);
          } break;
          default : if (*ns) *nd = *(ns++);
          }
        else *nd = *(ns++);
    }

    // Return a temporary string describing the size of a memory buffer.
    inline const char *strbuffersize(const cimg_ulong size);

    // Return string that identifies the running OS.
    inline const char *stros() {
#if defined(linux) || defined(__linux) || defined(__linux__)
      static const char *const str = "Linux";
#elif defined(sun) || defined(__sun)
      static const char *const str = "Sun OS";
#elif defined(BSD) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__) || defined (__DragonFly__)
      static const char *const str = "BSD";
#elif defined(sgi) || defined(__sgi)
      static const char *const str = "Irix";
#elif defined(__MACOSX__) || defined(__APPLE__)
      static const char *const str = "Mac OS";
#elif defined(unix) || defined(__unix) || defined(__unix__)
      static const char *const str = "Generic Unix";
#elif defined(_MSC_VER) || defined(WIN32)  || defined(_WIN32) || defined(__WIN32__) || \
  defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
      static const char *const str = "Windows";
#else
      const char
        *const _str1 = std::getenv("OSTYPE"),
        *const _str2 = _str1?_str1:std::getenv("OS"),
        *const str = _str2?_str2:"Unknown OS";
#endif
      return str;
    }

    //! Return the basename of a filename.
    inline const char* basename(const char *const s, const char separator=cimg_file_separator)  {
      const char *p = 0, *np = s;
      while (np>=s && (p=np)) np = std::strchr(np,separator) + 1;
      return p;
    }

    // Return a random filename.
    inline const char* filenamerand() {
      cimg::mutex(6);
      static char randomid[9];
      for (unsigned int k = 0; k<8; ++k) {
        const int v = (int)cimg::rand(65535)%3;
        randomid[k] = (char)(v==0?('0' + ((int)cimg::rand(65535)%10)):
                             (v==1?('a' + ((int)cimg::rand(65535)%26)):
                              ('A' + ((int)cimg::rand(65535)%26))));
      }
      cimg::mutex(6,0);
      return randomid;
    }

    // Convert filename as a Windows-style filename (short path name).
    inline void winformat_string(char *const str) {
      if (str && *str) {
#if cimg_OS==2
        char *const nstr = new char[MAX_PATH];
        if (GetShortPathNameA(str,nstr,MAX_PATH)) std::strcpy(str,nstr);
        delete[] nstr;
#endif
      }
    }

    // Open a file (similar to std:: fopen(), but with wide character support on Windows).
    inline std::FILE *std_fopen(const char *const path, const char *const mode);


    //! Open a file.
    /**
       \param path Path of the filename to open.
       \param mode C-string describing the opening mode.
       \return Opened file.
       \note Same as <tt>std::fopen()</tt> but throw a \c CImgIOException when
       the specified file cannot be opened, instead of returning \c 0.
    **/
    inline std::FILE *fopen(const char *const path, const char *const mode) {
      if (!path)
        throw CImgArgumentException("cimg::fopen(): Specified file path is (null).");
      if (!mode)
        throw CImgArgumentException("cimg::fopen(): File '%s', specified mode is (null).",
                                    path);
      std::FILE *res = 0;
      if (*path=='-' && (!path[1] || path[1]=='.')) {
        res = (*mode=='r')?cimg::_stdin():cimg::_stdout();
#if cimg_OS==2
        if (*mode && mode[1]=='b') { // Force stdin/stdout to be in binary mode
#ifdef __BORLANDC__
          if (setmode(_fileno(res),0x8000)==-1) res = 0;
#else
          if (_setmode(_fileno(res),0x8000)==-1) res = 0;
#endif
        }
#endif
      } else res = cimg::std_fopen(path,mode);
      if (!res) throw CImgIOException("cimg::fopen(): Failed to open file '%s' with mode '%s'.",
                                      path,mode);
      return res;
    }

    //! Close a file.
    /**
       \param file File to close.
       \return \c 0 if file has been closed properly, something else otherwise.
       \note Same as <tt>std::fclose()</tt> but display a warning message if
       the file has not been closed properly.
    **/
    inline int fclose(std::FILE *file) {
      if (!file) { warn("cimg::fclose(): Specified file is (null)."); return 0; }
      if (file==cimg::_stdin(false) || file==cimg::_stdout(false)) return 0;
      const int errn = std::fclose(file);
      if (errn!=0) warn("cimg::fclose(): Error code %d returned during file closing.",
                        errn);
      return errn;
    }

    //! Version of 'fseek()' that supports >=64bits offsets everywhere (for Windows).
    inline int fseek(FILE *stream, cimg_long offset, int origin) {
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
      return _fseeki64(stream,(__int64)offset,origin);
#else
      return std::fseek(stream,offset,origin);
#endif
    }

    //! Version of 'ftell()' that supports >=64bits offsets everywhere (for Windows).
    inline cimg_long ftell(FILE *stream) {
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
      return (cimg_long)_ftelli64(stream);
#else
      return (cimg_long)std::ftell(stream);
#endif
    }

    // Get the file or directory attributes with support for UTF-8 paths (Windows only).
#if cimg_OS==2
    inline DWORD win_getfileattributes(const char *const path);
#endif

    //! Check if a path is a directory.
    /**
       \param path Specified path to test.
    **/
    inline bool is_directory(const char *const path) {
      if (!path || !*path) return false;
#if cimg_OS==1
      struct stat st_buf;
      return (!stat(path,&st_buf) && S_ISDIR(st_buf.st_mode));
#elif cimg_OS==2
      const DWORD res = win_getfileattributes(path);
      return res!=INVALID_FILE_ATTRIBUTES && (res&FILE_ATTRIBUTE_DIRECTORY);
#else
      return false;
#endif
    }

    //! Check if a path is a file.
    /**
       \param path Specified path to test.
    **/
    inline bool is_file(const char *const path) {
      if (!path || !*path) return false;
#if cimg_OS==2
      const DWORD res = cimg::win_getfileattributes(path);
      return res!=INVALID_FILE_ATTRIBUTES && !(res&FILE_ATTRIBUTE_DIRECTORY);
#elif cimg_OS==1
      struct stat st_buf;
      return (!stat(path,&st_buf) && (S_ISREG(st_buf.st_mode) || S_ISFIFO(st_buf.st_mode) ||
                                      S_ISCHR(st_buf.st_mode) || S_ISBLK(st_buf.st_mode)));
#else
      std::FILE *const file = cimg::std_fopen(path,"rb");
      if (!file) return false;
      cimg::fclose(file);
      return !is_directory(path);
#endif
    }

    //! Check if a path exists.
    /**
       \param path Specified path to test.
    **/
    inline bool path_exists(const char *const path) {
      if (!path || !*path) return false;
#if cimg_OS==2
      const DWORD res = cimg::win_getfileattributes(path);
      return res!=INVALID_FILE_ATTRIBUTES;
#elif cimg_OS==1
      return faccessat(AT_FDCWD,path,F_OK,AT_EACCESS)==0;
#else
      std::FILE *const file = cimg::std_fopen(path,"rb");
      if (!file) return false;
      cimg::fclose(file);
      return true;
#endif
    }

    //! Get file size.
    /**
       \param filename Specified filename to get size from.
       \return File size or '-1' if file does not exist.
    **/
    inline cimg_int64 fsize(std::FILE *const file) {
      if (!file) return (cimg_int64)-1;
      const long pos = std::ftell(file);
      std::fseek(file,0,SEEK_END);
      const cimg_int64 siz = (cimg_int64)std::ftell(file);
      std::fseek(file,pos,SEEK_SET);
      return siz;
    }

    inline cimg_int64 fsize(const char *const filename) {
      std::FILE *const file = cimg::std_fopen(filename,"rb");
      const cimg_int64 siz = fsize(file);
      cimg::fclose(file);
      return siz;
    }

    //! Get last write time of a given file or directory (multiple-attributes version).
    /**
       \param path Specified path to get attributes from.
       \param[in,out] attr Type of requested time attributes.
                      Can be { 0=year | 1=month | 2=day | 3=day of week | 4=hour | 5=minute | 6=second }
                      Replaced by read attributes after return (or -1 if an error occurred).
       \param nb_attr Number of attributes to read/write.
       \return Latest read attribute.
    **/
    template<typename T>
    inline int fdate(const char *const path, T *attr, const unsigned int nb_attr) {
#define _cimg_fdate_err() for (unsigned int i = 0; i<nb_attr; ++i) attr[i] = (T)-1
      int res = -1;
      if (!path || !*path) { _cimg_fdate_err(); return -1; }
      cimg::mutex(6);
#if cimg_OS==2
      HANDLE file = CreateFileA(path,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
      if (file!=INVALID_HANDLE_VALUE) {
        FILETIME _ft;
        SYSTEMTIME ft;
        if (GetFileTime(file,0,0,&_ft) && FileTimeToSystemTime(&_ft,&ft)) {
          for (unsigned int i = 0; i<nb_attr; ++i) {
            res = (int)(attr[i]==0?ft.wYear:attr[i]==1?ft.wMonth:attr[i]==2?ft.wDay:
                        attr[i]==3?ft.wDayOfWeek:attr[i]==4?ft.wHour:attr[i]==5?ft.wMinute:
                        attr[i]==6?ft.wSecond:-1);
            attr[i] = (T)res;
          }
        } else _cimg_fdate_err();
        CloseHandle(file);
      } else _cimg_fdate_err();
#elif cimg_OS==1
      struct stat st_buf;
      if (!stat(path,&st_buf)) {
        const time_t _ft = st_buf.st_mtime;
        const struct tm& ft = *std::localtime(&_ft);
        for (unsigned int i = 0; i<nb_attr; ++i) {
          res = (int)(attr[i]==0?ft.tm_year + 1900:attr[i]==1?ft.tm_mon + 1:attr[i]==2?ft.tm_mday:
                      attr[i]==3?ft.tm_wday:attr[i]==4?ft.tm_hour:attr[i]==5?ft.tm_min:
                      attr[i]==6?ft.tm_sec:-1);
          attr[i] = (T)res;
        }
      } else _cimg_fdate_err();
#endif
      cimg::mutex(6,0);
      return res;
    }

    //! Get last write time of a given file or directory (single-attribute version).
    /**
       \param path Specified path to get attributes from.
       \param attr Type of requested time attributes.
                   Can be { 0=year | 1=month | 2=day | 3=day of week | 4=hour | 5=minute | 6=second }
       \return Specified attribute or -1 if an error occurred.
    **/
    inline int fdate(const char *const path, unsigned int attr) {
      int out = (int)attr;
      return fdate(path,&out,1);
    }

    //! Get current local time (multiple-attributes version).
    /**
       \param[in,out] attr Type of requested time attributes.
                           Can be { 0=year | 1=month | 2=day | 3=day of week | 4=hour | 5=minute | 6=second |
                                    7=millisecond }
                           Replaced by read attributes after return (or -1 if an error occurred).
       \param nb_attr Number of attributes to read/write.
       \return Latest read attribute.
    **/
    template<typename T>
    inline int date(T *attr, const unsigned int nb_attr) {
      int res = -1;
      cimg::mutex(6);
#if cimg_OS==2
      SYSTEMTIME st;
      GetLocalTime(&st);
      for (unsigned int i = 0; i<nb_attr; ++i) {
        res = (int)(attr[i]==0?st.wYear:
                    attr[i]==1?st.wMonth:
                    attr[i]==2?st.wDay:
                    attr[i]==3?st.wDayOfWeek:
                    attr[i]==4?st.wHour:
                    attr[i]==5?st.wMinute:
                    attr[i]==6?st.wSecond:
                    attr[i]==7?st.wMilliseconds:-1);
        attr[i] = (T)res;
      }
#else
      struct timeval _st;
      gettimeofday(&_st,0);
      struct tm *st = std::localtime(&_st.tv_sec);
      for (unsigned int i = 0; i<nb_attr; ++i) {
        res = (int)(attr[i]==0?st->tm_year + 1900:
                    attr[i]==1?st->tm_mon + 1:
                    attr[i]==2?st->tm_mday:
                    attr[i]==3?st->tm_wday:
                    attr[i]==4?st->tm_hour:
                    attr[i]==5?st->tm_min:
                    attr[i]==6?st->tm_sec:
                    attr[i]==7?_st.tv_usec/1000:-1);
        attr[i] = (T)res;
      }
#endif
      cimg::mutex(6,0);
      return res;
    }

    //! Get current local time (single-attribute version).
    /**
       \param attr Type of requested time attribute.
                   Can be { 0=year | 1=month | 2=day | 3=day of week | 4=hour | 5=minute | 6=second |
                            7=millisecond }
       \return Specified attribute or -1 if an error occurred.
    **/
    inline int date(unsigned int attr) {
      int out = (int)attr;
      return date(&out,1);
    }

    //! Convert date to epoch (UTC time).
    // 'year' must be >=1900, 'month' in [ 1,12 ], 'day' in [ 1,31 ], 'hour' in [ 0,23 ],
    // 'minute' in [ 0,59 ] and 'second' in [ 0,60 ].
    inline cimg_int64 epoch(const int year, const int month=1,
                            const int day=1, const int hour=0,
                            const int minute=0, const int second=0) {
      static const int tab_days[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
#define cimg_is_leap(y) ((!((y)%4) && ((y)%100)) || !((y)%400))
#define cimg_days_in_month(y,m) ((m)==2 && cimg_is_leap(y)?29:tab_days[cimg::cut(m,1,12) - 1])
      cimg_int64 days = 0;
      if (year>=1970) for (int y = 1970; y<year; ++y) days+=cimg_is_leap(y)?366:365;
      else for (int y = year; y<1970; ++y) days-=cimg_is_leap(y)?366:365;
      for (int m = 1; m<month; ++m) days+=cimg_days_in_month(year,m);
      days+=day - 1;
      return days*86400 + hour*3600 + minute*60 + second;
    }

    // Get/set path to the \c curl binary.
    inline const char *curl_path(const char *const user_path=0, const bool reinit_path=false);

    // Get/set path to the \c dcraw binary.
    inline const char *dcraw_path(const char *const user_path=0, const bool reinit_path=false);

    // Get/set path to the FFMPEG's \c ffmpeg binary.
    inline const char *ffmpeg_path(const char *const user_path=0, const bool reinit_path=false);

    // Get/set path to the GraphicsMagick's \c gm binary.
    inline const char* graphicsmagick_path(const char *const user_path=0, const bool reinit_path=false);

    // Get/set path to the \c gunzip binary.
    inline const char *gunzip_path(const char *const user_path=0, const bool reinit_path=false);

    // Get/set path to the \c gzip binary.
    inline const char *gzip_path(const char *const user_path=0, const bool reinit_path=false);

    // Get/set path to the ImageMagick's \c convert binary.
    inline const char* imagemagick_path(const char *const user_path=0, const bool reinit_path=false);

    // Get/set path to the Medcon's \c medcon binary.
    inline const char* medcon_path(const char *const user_path=0, const bool reinit_path=false);

    // Get/set path to store temporary files.
    inline const char* temporary_path(const char *const user_path=0, const bool reinit_path=false);

    // Get/set path to the \c wget binary.
    inline const char *wget_path(const char *const user_path=0, const bool reinit_path=false);

#if cimg_OS==2
    // Get/set path to the \c powershell binary.
    inline const char *powershell_path(const char *const user_path=0, const bool reinit_path=false);
#endif

#if cimg_OS==1
    inline bool posix_searchpath(const char *file);
#endif

    //! Split filename into two C-strings \c body and \c extension.
    /**
       filename and body must not overlap!
    **/
    inline const char *split_filename(const char *const filename, char *const body=0) {
      if (!filename) { if (body) *body = 0; return ""; }
      const char * p = std::strrchr(filename,'.');
      if (!p || std::strchr(p,'/') || std::strchr(p,'\\')) { // No extension
        if (body) std::strcpy(body,filename);
        return filename + std::strlen(filename);
      }
      const unsigned int l = (unsigned int)(p - filename);
      if (body) { if (l) std::memcpy(body,filename,l); body[l] = 0; }
      return p + 1;
    }

    // Generate a numbered version of a filename.
    inline char* number_filename(const char *const filename, const int number,
                                 const unsigned int digits, char *const str);

    //! Read data from file.
    /**
       \param[out] ptr Pointer to memory buffer that will contain the binary data read from file.
       \param nmemb Number of elements to read.
       \param stream File to read data from.
       \return Number of read elements.
       \note Same as <tt>std::fread()</tt> but may display warning message if all elements could not be read.
    **/
    template<typename T>
    inline size_t fread(T *const ptr, const size_t nmemb, std::FILE *stream) {
      if (!ptr || !stream)
        throw CImgArgumentException("cimg::fread(): Invalid reading request of %u %s%s from file %p to buffer %p.",
                                    nmemb,cimg::type<T>::string(),nmemb>1?"s":"",stream,ptr);
      if (!nmemb) return 0;
      const size_t wlimitT = 63*1024*1024, wlimit = wlimitT/sizeof(T);
      size_t to_read = nmemb, al_read = 0, l_to_read = 0, l_al_read = 0;
      do {
        l_to_read = (to_read*sizeof(T))<wlimitT?to_read:wlimit;
        l_al_read = std::fread((void*)(ptr + al_read),sizeof(T),l_to_read,stream);
        al_read+=l_al_read;
        to_read-=l_al_read;
      } while (l_to_read==l_al_read && to_read>0);
      if (to_read>0)
        warn("cimg::fread(): Only %lu/%lu elements could be read from file.",
             (unsigned long)al_read,(unsigned long)nmemb);
      return al_read;
    }

    //! Write data to file.
    /**
       \param ptr Pointer to memory buffer containing the binary data to write on file.
       \param nmemb Number of elements to write.
       \param[out] stream File to write data on.
       \return Number of written elements.
       \note Similar to <tt>std::fwrite</tt> but may display warning messages if all elements could not be written.
    **/
    template<typename T>
    inline size_t fwrite(const T *ptr, const size_t nmemb, std::FILE *stream) {
      if (!ptr || !stream)
        throw CImgArgumentException("cimg::fwrite(): Invalid writing request of %u %s%s from buffer %p to file %p.",
                                    nmemb,cimg::type<T>::string(),nmemb>1?"s":"",ptr,stream);
      if (!nmemb) return 0;
      const size_t wlimitT = 63*1024*1024, wlimit = wlimitT/sizeof(T);
      size_t to_write = nmemb, al_write = 0, l_to_write = 0, l_al_write = 0;
      do {
        l_to_write = (to_write*sizeof(T))<wlimitT?to_write:wlimit;
        l_al_write = std::fwrite((void*)(ptr + al_write),sizeof(T),l_to_write,stream);
        al_write+=l_al_write;
        to_write-=l_al_write;
      } while (l_to_write==l_al_write && to_write>0);
      if (to_write>0)
        warn("cimg::fwrite(): Only %lu/%lu elements could be written in file.",
             (unsigned long)al_write,(unsigned long)nmemb);
      return al_write;
    }

    //! Create an empty file.
    /**
       \param file Input file (can be \c 0 if \c filename is set).
       \param filename Filename, as a C-string (can be \c 0 if \c file is set).
    **/
    inline void fempty(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException("cimg::fempty(): Specified filename is (null).");
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      if (!file) cimg::fclose(nfile);
    }

    // Try to guess format from an image file.
    inline const char *ftype(std::FILE *const file, const char *const filename);

    // Get or set load from network mode (can be { 0=disabled | 1=enabled }).
    inline bool& network_mode(const bool value, const bool is_set) {
      static bool mode = true;
      if (is_set) { cimg::mutex(0); mode = value; cimg::mutex(0,0); }
      return mode;
    }

    inline bool& network_mode() {
      return network_mode(false,false);
    }

    // Load file from network as a local temporary file.
    inline char *load_network(const char *const url, char *const filename_local,
                              const unsigned int timeout=0, const bool try_fallback=false,
                              const char *const referer=0, const char *const user_agent=0);

    //! Return options specified on the command line.
    inline const char* option(const char *const name, const int argc, const char *const *const argv,
                              const char *const _default, const char *const usage, const bool reset_static) {
      static bool first = true, visu = false;
      if (reset_static) { first = true; return 0; }
      const char *res = 0;
      if (first) {
        first = false;
        visu = cimg::option("-h",argc,argv,(char*)0,(char*)0,false)!=0;
        visu |= cimg::option("-help",argc,argv,(char*)0,(char*)0,false)!=0;
        visu |= cimg::option("--help",argc,argv,(char*)0,(char*)0,false)!=0;
      }
      if (!name && visu) {
        if (usage) {
          std::fprintf(cimg::output(),"\n %s%s%s",cimg::t_red,cimg::basename(argv[0]),cimg::t_normal);
          std::fprintf(cimg::output(),": %s",usage);
          std::fprintf(cimg::output()," (%s, %s)\n\n",cimg_date,cimg_time);
        }
        if (_default) std::fprintf(cimg::output(),"%s\n",_default);
      }
      if (name) {
        if (argc>0) {
          int k = 0;
          while (k<argc && std::strcmp(argv[k],name)) ++k;
          res = (k++==argc?_default:(k==argc?argv[--k]:argv[k]));
        } else res = _default;
        if (visu && usage) std::fprintf(cimg::output(),"    %s%-16s%s %-24s %s%s%s\n",
                                        cimg::t_bold,name,cimg::t_normal,res?res:"0",
                                        cimg::t_green,usage,cimg::t_normal);
      }
      return res;
    }

    inline const char* option(const char *const name, const int argc, const char *const *const argv,
                              const char *const _default, const char *const usage=0) {
      return option(name,argc,argv,_default,usage,false);
    }

    inline bool option(const char *const name, const int argc, const char *const *const argv,
                       const bool _default, const char *const usage=0) {
      const char *const s = cimg::option(name,argc,argv,(char*)0);
      const bool res = s?(cimg::strcasecmp(s,"false") && cimg::strcasecmp(s,"off") && cimg::strcasecmp(s,"0")):_default;
      cimg::option(name,0,0,res?"true":"false",usage);
      return res;
    }

    inline int option(const char *const name, const int argc, const char *const *const argv,
                      const int _default, const char *const usage=0) {
      const char *const s = cimg::option(name,argc,argv,(char*)0);
      const int res = s?std::atoi(s):_default;
      char *const tmp = new char[256];
      cimg_snprintf(tmp,256,"%d",res);
      cimg::option(name,0,0,tmp,usage);
      delete[] tmp;
      return res;
    }

    inline char option(const char *const name, const int argc, const char *const *const argv,
                       const char _default, const char *const usage=0) {
      const char *const s = cimg::option(name,argc,argv,(char*)0);
      const char res = s?*s:_default;
      char tmp[8];
      *tmp = res; tmp[1] = 0;
      cimg::option(name,0,0,tmp,usage);
      return res;
    }

    inline float option(const char *const name, const int argc, const char *const *const argv,
                        const float _default, const char *const usage=0) {
      const char *const s = cimg::option(name,argc,argv,(char*)0);
      const float res = s?(float)cimg::atof(s):_default;
      char *const tmp = new char[256];
      cimg_snprintf(tmp,256,"%g",res);
      cimg::option(name,0,0,tmp,usage);
      delete[] tmp;
      return res;
    }

    inline double option(const char *const name, const int argc, const char *const *const argv,
                         const double _default, const char *const usage=0) {
      const char *const s = cimg::option(name,argc,argv,(char*)0);
      const double res = s?cimg::atof(s):_default;
      char *const tmp = new char[256];
      cimg_snprintf(tmp,256,"%g",res);
      cimg::option(name,0,0,tmp,usage);
      delete[] tmp;
      return res;
    }

    //! Print information about \CImg environment variables.
    /**
       \note Output is done on the default output stream.
    **/
    inline void info() {
      std::fprintf(cimg::output(),"\n %s%sCImg Library %u.%u.%u%s, compiled %s ( %s ) with the following flags:\n\n",
                   cimg::t_red,cimg::t_bold,cimg_version/100,(cimg_version/10)%10,cimg_version%10,
                   cimg::t_normal,cimg_date,cimg_time);

      std::fprintf(cimg::output(),"  > Operating System:         %s%-13s%s %s('cimg_OS'=%d)%s\n",
                   cimg::t_bold,
                   cimg_OS==1?"Unix":(cimg_OS==2?"Windows":"Unknown"),
                   cimg::t_normal,cimg::t_green,
                   cimg_OS,
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > CPU endianness:           %s%s Endian%s\n",
                   cimg::t_bold,
                   cimg::endianness()?"Big":"Little",
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Verbosity mode:           %s%-13s%s %s('cimg_verbosity'=%d)%s\n",
                   cimg::t_bold,
                   cimg_verbosity==0?"Quiet":
                   cimg_verbosity==1?"Console":
                   cimg_verbosity==2?"Dialog":
                   cimg_verbosity==3?"Console+Warnings":"Dialog+Warnings",
                   cimg::t_normal,cimg::t_green,
                   cimg_verbosity,
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Stricts warnings:         %s%-13s%s %s('cimg_strict_warnings' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_strict_warnings
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Support for C++11:        %s%-13s%s %s('cimg_use_cpp11'=%d)%s\n",
                   cimg::t_bold,
                   cimg_use_cpp11?"Yes":"No",
                   cimg::t_normal,cimg::t_green,
                   (int)cimg_use_cpp11,
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using VT100 messages:     %s%-13s%s %s('cimg_use_vt100' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_vt100
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Display type:             %s%-13s%s %s('cimg_display'=%d)%s\n",
                   cimg::t_bold,
                   cimg_display==0?"No display":cimg_display==1?"X11":cimg_display==2?"Windows GDI":"Unknown",
                   cimg::t_normal,cimg::t_green,
                   (int)cimg_display,
                   cimg::t_normal);

#if cimg_display==1
      std::fprintf(cimg::output(),"  > Using XShm for X11:       %s%-13s%s %s('cimg_use_xshm' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_xshm
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using XRand for X11:      %s%-13s%s %s('cimg_use_xrandr' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_xrandr
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);
#endif
      std::fprintf(cimg::output(),"  > Using OpenMP:             %s%-13s%s %s('cimg_use_openmp' %s)%s\n",
                   cimg::t_bold,
#if cimg_use_openmp!=0
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);
      std::fprintf(cimg::output(),"  > Using PNG library:        %s%-13s%s %s('cimg_use_png' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_png
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);
      std::fprintf(cimg::output(),"  > Using JPEG library:       %s%-13s%s %s('cimg_use_jpeg' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_jpeg
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using TIFF library:       %s%-13s%s %s('cimg_use_tiff' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_tiff
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using Magick++ library:   %s%-13s%s %s('cimg_use_magick' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_magick
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      std::fprintf(cimg::output(),"  > Using FFTW3 library:      %s%-13s%s %s('cimg_use_fftw3' %s)%s\n",
                   cimg::t_bold,
#ifdef cimg_use_fftw3
                   "Yes",cimg::t_normal,cimg::t_green,"defined",
#else
                   "No",cimg::t_normal,cimg::t_green,"undefined",
#endif
                   cimg::t_normal);

      char *const tmp = new char[1024];

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::curl_path());
      std::fprintf(cimg::output(),"  > Path of 'curl':           %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::dcraw_path());
      std::fprintf(cimg::output(),"  > Path of 'dcraw':          %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::ffmpeg_path());
      std::fprintf(cimg::output(),"  > Path of 'ffmpeg':         %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::graphicsmagick_path());
      std::fprintf(cimg::output(),"  > Path of 'graphicsmagick': %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::gunzip_path());
      std::fprintf(cimg::output(),"  > Path of 'gunzip':         %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::gzip_path());
      std::fprintf(cimg::output(),"  > Path of 'gzip':           %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::imagemagick_path());
      std::fprintf(cimg::output(),"  > Path of 'imagemagick':    %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::medcon_path());
      std::fprintf(cimg::output(),"  > Path of 'medcon':         %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::temporary_path());
      std::fprintf(cimg::output(),"  > Temporary path:           %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);

      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::wget_path());
      std::fprintf(cimg::output(),"  > Path of 'wget':           %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);
#if cimg_OS==2
      cimg_snprintf(tmp,1024,"\"%.1020s\"",cimg::powershell_path());
      std::fprintf(cimg::output(),"  > Path of 'powershell':     %s%-13s%s\n",
                   cimg::t_bold,
                   tmp,
                   cimg::t_normal);
#endif

      std::fprintf(cimg::output(),"\n");
      delete[] tmp;
    }

  } // namespace cimg { ...
}

#endif
