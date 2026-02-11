#ifndef CIMG_MODULE_MATH_TRAITS_H
#define CIMG_MODULE_MATH_TRAITS_H




namespace cimg_library {
  namespace cimg {

    inline std::FILE* output(std::FILE *file);
    inline void info();

    template<typename T>
    inline void unused(const T&, ...);

    inline int mutex(const unsigned int n, const int lock_mode);
    inline unsigned int& exception_mode(const unsigned int value, const bool is_set);
    inline unsigned int& exception_mode(const unsigned int mode);
    inline unsigned int& exception_mode();
    inline FILE* _stdin(const bool throw_exception);
    inline FILE* _stdout(const bool throw_exception);
    inline FILE* _stderr(const bool throw_exception);
    inline unsigned int openmp_mode(const unsigned int value, const bool is_set);
    inline unsigned int openmp_mode(const unsigned int mode);
    inline unsigned int openmp_mode();

    // Define traits that will be used to determine the best data type to work in CImg functions.
    //
    template<typename T> struct type {
      static const char* string() {
        static const char* s[] = { "unknown",   "unknown8",   "unknown16",  "unknown24",
                                   "unknown32", "unknown40",  "unknown48",  "unknown56",
                                   "unknown64", "unknown72",  "unknown80",  "unknown88",
                                   "unknown96", "unknown104", "unknown112", "unknown120",
                                   "unknown128" };
        return s[(sizeof(T)<17)?sizeof(T):0];
      }
      static bool is_float() { return false; }
      static bool is_inf(const T) { return false; }
      static bool is_nan(const T) { return false; }
      static bool is_finite(const T) { return true; }
      static T min() { return ~max(); }
      static T max() { return (T)1<<(8*sizeof(T) - 1); }
      static T inf() { return max(); }
      static T nan() { return inf(); }
      static T cut(const double val) { return val<(double)min()?min():val>(double)max()?max():(T)val; }
      static const char* format() { return "%s"; }
      static const char* format_s() { return "%s"; }
      static const char* format(const T& val) { static const char *const s = "unknown"; cimg::unused(val); return s; }
    };

    template<> struct type<bool> {
      static const char* string() {
        static const char *const s = "bool";
        return s;
      }
      static bool is_float() { return false; }
      static bool is_inf(const bool) { return false; }
      static bool is_nan(const bool) { return false; }
      static bool is_finite(const bool) { return true; }
      static bool min() { return false; }
      static bool max() { return true; }
      static bool inf() { return max(); }
      static bool nan() { return inf(); }
      static bool is_inf() { return false; }
      static bool cut(const double val) { return val<(double)min()?min():val>(double)max()?max():(bool)val; }
      static const char* format() { return "%s"; }
      static const char* format_s() { return "%s"; }
      static const char* format(const bool val) { static const char* s[] = { "false", "true" }; return s[val?1:0]; }
    };

    template<> struct type<unsigned char> {
      static const char* string() { static const char *const s = "uint8"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const unsigned char) { return false; }
      static bool is_nan(const unsigned char) { return false; }
      static bool is_finite(const unsigned char) { return true; }
      static unsigned char min() { return 0; }
      static unsigned char max() { return (unsigned char)-1; }
      static unsigned char inf() { return max(); }
      static unsigned char nan() { return inf(); }
      static unsigned char cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(unsigned char)val; }
      static const char* format() { return "%u"; }
      static const char* format_s() { return "%u"; }
      static unsigned int format(const unsigned char val) { return (unsigned int)val; }
    };

#if defined(CHAR_MAX) && CHAR_MAX==255
    template<> struct type<char> {
      static const char* string() { static const char *const s = "uint8"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const char) { return false; }
      static bool is_nan(const char) { return false; }
      static bool is_finite(const char) { return true; }
      static char min() { return 0; }
      static char max() { return (char)-1; }
      static char inf() { return max(); }
      static char nan() { return inf(); }
      static char cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(unsigned char)val; }
      static const char* format() { return "%u"; }
      static const char* format_s() { return "%u"; }
      static unsigned int format(const char val) { return (unsigned int)val; }
    };
#else
    template<> struct type<char> {
      static const char* string() { static const char *const s = "int8"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const char) { return false; }
      static bool is_nan(const char) { return false; }
      static bool is_finite(const char) { return true; }
      static char min() { return ~max(); }
      static char max() { return (char)((unsigned char)-1>>1); }
      static char inf() { return max(); }
      static char nan() { return inf(); }
      static char cut(const double val) { return val<(double)min()?min():val>(double)max()?max():(char)val; }
      static const char* format() { return "%d"; }
      static const char* format_s() { return "%d"; }
      static int format(const char val) { return (int)val; }
    };
#endif

    template<> struct type<signed char> {
      static const char* string() { static const char *const s = "int8"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const signed char) { return false; }
      static bool is_nan(const signed char) { return false; }
      static bool is_finite(const signed char) { return true; }
      static signed char min() { return ~max(); }
      static signed char max() { return (signed char)((unsigned char)-1>>1); }
      static signed char inf() { return max(); }
      static signed char nan() { return inf(); }
      static signed char cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(signed char)val; }
      static const char* format() { return "%d"; }
      static const char* format_s() { return "%d"; }
      static int format(const signed char val) { return (int)val; }
    };

    template<> struct type<unsigned short> {
      static const char* string() { static const char *const s = "uint16"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const unsigned short) { return false; }
      static bool is_nan(const unsigned short) { return false; }
      static bool is_finite(const unsigned short) { return true; }
      static unsigned short min() { return 0; }
      static unsigned short max() { return (unsigned short)-1; }
      static unsigned short inf() { return max(); }
      static unsigned short nan() { return inf(); }
      static unsigned short cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(unsigned short)val; }
      static const char* format() { return "%u"; }
      static const char* format_s() { return "%u"; }
      static unsigned int format(const unsigned short val) { return (unsigned int)val; }
    };

    template<> struct type<short> {
      static const char* string() { static const char *const s = "int16"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const short) { return false; }
      static bool is_nan(const short) { return false; }
      static bool is_finite(const short) { return true; }
      static short min() { return ~max(); }
      static short max() { return (short)((unsigned short)-1>>1); }
      static short inf() { return max(); }
      static short nan() { return inf(); }
      static short cut(const double val) { return val<(double)min()?min():val>(double)max()?max():(short)val; }
      static const char* format() { return "%d"; }
      static const char* format_s() { return "%d"; }
      static int format(const short val) { return (int)val; }
    };

    template<> struct type<unsigned int> {
      static const char* string() { static const char *const s = "uint32"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const unsigned int) { return false; }
      static bool is_nan(const unsigned int) { return false; }
      static bool is_finite(const unsigned int) { return true; }
      static unsigned int min() { return 0; }
      static unsigned int max() { return (unsigned int)-1; }
      static unsigned int inf() { return max(); }
      static unsigned int nan() { return inf(); }
      static unsigned int cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(unsigned int)val; }
      static const char* format() { return "%u"; }
      static const char* format_s() { return "%u"; }
      static unsigned int format(const unsigned int val) { return val; }
    };

    template<> struct type<int> {
      static const char* string() { static const char *const s = "int32"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const int) { return false; }
      static bool is_nan(const int) { return false; }
      static bool is_finite(const int) { return true; }
      static int min() { return ~max(); }
      static int max() { return (int)(~0U>>1); }
      static int inf() { return max(); }
      static int nan() { return inf(); }
      static int cut(const double val) { return val<(double)min()?min():val>(double)max()?max():(int)val; }
      static const char* format() { return "%d"; }
      static const char* format_s() { return "%d"; }
      static int format(const int val) { return val; }
    };

    template<> struct type<cimg_uint64> {
      static const char* string() { static const char *const s = "uint64"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const cimg_uint64) { return false; }
      static bool is_nan(const cimg_uint64) { return false; }
      static bool is_finite(const cimg_uint64) { return true; }
      static cimg_uint64 min() { return 0; }
      static cimg_uint64 max() { return (cimg_uint64)-1; }
      static cimg_uint64 inf() { return max(); }
      static cimg_uint64 nan() { return inf(); }
      static cimg_uint64 cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(cimg_uint64)val; }
      static const char* format() { return cimg_fuint64; }
      static const char* format_s() { return cimg_fuint64; }
      static cimg_uint64 format(const cimg_uint64 val) { return val; }
    };

    template<> struct type<cimg_int64> {
      static const char* string() { static const char *const s = "int64"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const cimg_int64) { return false; }
      static bool is_nan(const cimg_int64) { return false; }
      static bool is_finite(const cimg_int64) { return true; }
      static cimg_int64 min() { return ~max(); }
      static cimg_int64 max() { return (cimg_int64)((cimg_uint64)-1>>1); }
      static cimg_int64 inf() { return max(); }
      static cimg_int64 nan() { return inf(); }
      static cimg_int64 cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(cimg_int64)val;
      }
      static const char* format() { return cimg_fint64; }
      static const char* format_s() { return cimg_fint64; }
      static long format(const long val) { return (long)val; }
    };

#if cimg_use_cpp11==1 && \
    (!(UINTPTR_MAX==0xffffffff || defined(__arm__) || defined(_M_ARM) || ((ULONG_MAX)==(UINT_MAX))))
    template<> struct type<unsigned long long> {
      static const char* string() { static const char *const s = "uint64"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const cimg_uint64) { return false; }
      static bool is_nan(const cimg_uint64) { return false; }
      static bool is_finite(const cimg_uint64) { return true; }
      static cimg_uint64 min() { return 0; }
      static cimg_uint64 max() { return (cimg_uint64)-1; }
      static cimg_uint64 inf() { return max(); }
      static cimg_uint64 nan() { return inf(); }
      static cimg_uint64 cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(cimg_uint64)val; }
      static const char* format() { return cimg_fuint64; }
      static const char* format_s() { return cimg_fuint64; }
      static cimg_uint64 format(const cimg_uint64 val) { return val; }
    };

    template<> struct type<long long> {
      static const char* string() { static const char *const s = "int64"; return s; }
      static bool is_float() { return false; }
      static bool is_inf(const cimg_int64) { return false; }
      static bool is_nan(const cimg_int64) { return false; }
      static bool is_finite(const cimg_int64) { return true; }
      static long long min() { return ~max(); }
      static long long max() { return (cimg_int64)((cimg_uint64)-1>>1); }
      static long long inf() { return max(); }
      static long long nan() { return max(); }
      static long long cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(cimg_int64)val;
      }
      static const char* format() { return cimg_fint64; }
      static const char* format_s() { return cimg_fint64; }
      static long format(const long val) { return (long)val; }
    };
#endif

    template<> struct type<double> {
      static const char* string() { static const char *const s = "float64"; return s; }
      static bool is_float() { return true; }
      static bool is_inf(const double val) {
#ifdef isinf
        return (bool)isinf(val);
#else
        return !is_nan(val) && (val<cimg::type<double>::min() || val>cimg::type<double>::max());
#endif
      }
      static bool is_nan(const double val) { // Custom version that works with '-ffast-math'
        if (sizeof(double)==8) {
          cimg_uint64 u;
          std::memcpy(&u,&val,sizeof(double));
          return ((unsigned int)(u>>32)&0x7fffffff) + ((unsigned int)u!=0)>0x7ff00000;
        }
#ifdef isnan
        return (bool)isnan(val);
#else
        return !(val==val);
#endif
      }
      static bool is_finite(const double val) {
#ifdef isfinite
        return (bool)isfinite(val);
#else
        return !is_nan(val) && !is_inf(val);
#endif
      }
      static double min() { return -DBL_MAX; }
      static double max() { return DBL_MAX; }
      static double inf() {
#ifdef INFINITY
        return (double)INFINITY;
#else
        return max()*max();
#endif
      }
      static double nan() {
#ifdef NAN
        return (double)NAN;
#else
        const double val_nan = -std::sqrt(-1.); return val_nan;
#endif
      }
      static double cut(const double val) { return val; }
      static const char* format() { return "%.17g"; }
      static const char* format_s() { return "%g"; }
      static double format(const double val) { return val; }
    };

    template<> struct type<float> {
      static const char* string() { static const char *const s = "float32"; return s; }
      static bool is_float() { return true; }
      static bool is_inf(const float val) {
#ifdef isinf
        return (bool)isinf(val);
#else
        return !is_nan(val) && (val<cimg::type<float>::min() || val>cimg::type<float>::max());
#endif
      }
      static bool is_nan(const float val) { // Custom version that works with '-ffast-math'
        if (sizeof(float)==4) {
          unsigned int u;
          std::memcpy(&u,&val,sizeof(float));
          return (u&0x7fffffff)>0x7f800000;
        }
#ifdef isnan
        return (bool)isnan(val);
#else
        return !(val==val);
#endif
      }
      static bool is_finite(const float val) {
#ifdef isfinite
        return (bool)isfinite(val);
#else
        return !is_nan(val) && !is_inf(val);
#endif
      }
      static float min() { return -FLT_MAX; }
      static float max() { return FLT_MAX; }
      static float inf() { return (float)cimg::type<double>::inf(); }
      static float nan() { return (float)cimg::type<double>::nan(); }
      static float cut(const double val) { return (float)val; }
      static float cut(const float val) { return (float)val; }
      static const char* format() { return "%.9g"; }
      static const char* format_s() { return "%g"; }
      static double format(const float val) { return (double)val; }
    };

    template<> struct type<long double> {
      static const char* string() { static const char *const s = "float128"; return s; }
      static bool is_float() { return true; }
      static bool is_inf(const long double val) {
#ifdef isinf
        return (bool)isinf(val);
#else
        return !is_nan(val) && (val<cimg::type<long double>::min() || val>cimg::type<long double>::max());
#endif
      }
      static bool is_nan(const long double val) {
#ifdef isnan
        return (bool)isnan(val);
#else
        return !(val==val);
#endif
      }
      static bool is_finite(const long double val) {
#ifdef isfinite
        return (bool)isfinite(val);
#else
        return !is_nan(val) && !is_inf(val);
#endif
      }
      static long double min() { return -LDBL_MAX; }
      static long double max() { return LDBL_MAX; }
      static long double inf() { return max()*max(); }
      static long double nan() { const long double val_nan = -std::sqrt(-1.L); return val_nan; }
      static long double cut(const long double val) { return val; }
      static const char* format() { return "%.17g"; }
      static const char* format_s() { return "%g"; }
      static double format(const long double val) { return (double)val; }
    };

#if cimg_is_float16==1
    template<> struct type<cimg_float16> {
      static const char* string() { static const char *const s = "float16"; return s; }
      static bool is_float() { return true; }
      static bool is_inf(const long double val) {
#ifdef isinf
        return (bool)isinf(val);
#else
        return !is_nan(val) && (val<cimg::type<cimg_float16>::min() || val>cimg::type<cimg_float16>::max());
#endif
      }
      static bool is_nan(const cimg_float16 val) { // Custom version that works with '-ffast-math'
        if (sizeof(cimg_float16)==2) {
          short u;
          std::memcpy(&u,&val,sizeof(short));
          return (bool)((u&0x7fff)>0x7c00);
        }
        return cimg::type<float>::is_nan((float)val);
      }
      static bool is_finite(const cimg_float16 val) {
#ifdef isfinite
        return (bool)isfinite(val);
#else
        return !is_nan(val) && !is_inf(val);
#endif
      }
      static cimg_float16 min() { return (cimg_float16)-65504; }
      static cimg_float16 max() { return (cimg_float16)65504; }
      static cimg_float16 inf() { return max()*max(); }
      static cimg_float16 nan() { const cimg_float16 val_nan = (cimg_float16)-std::sqrt(-1.); return val_nan; }
      static cimg_float16 cut(const double val) { return (cimg_float16)val; }
      static const char* format() { return "%.9g"; }
      static const char* format_s() { return "%g"; }
      static double format(const cimg_float16 val) { return (double)val; }
    };
#endif

    template<typename T, typename t> struct superset { typedef T type; };
    template<> struct superset<bool,unsigned char> { typedef unsigned char type; };
    template<> struct superset<bool,char> { typedef char type; };
    template<> struct superset<bool,signed char> { typedef signed char type; };
    template<> struct superset<bool,unsigned short> { typedef unsigned short type; };
    template<> struct superset<bool,short> { typedef short type; };
    template<> struct superset<bool,unsigned int> { typedef unsigned int type; };
    template<> struct superset<bool,int> { typedef int type; };
    template<> struct superset<bool,cimg_uint64> { typedef cimg_uint64 type; };
    template<> struct superset<bool,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<bool,float> { typedef float type; };
    template<> struct superset<bool,double> { typedef double type; };
    template<> struct superset<unsigned char,char> { typedef short type; };
    template<> struct superset<unsigned char,signed char> { typedef short type; };
    template<> struct superset<unsigned char,unsigned short> { typedef unsigned short type; };
    template<> struct superset<unsigned char,short> { typedef short type; };
    template<> struct superset<unsigned char,unsigned int> { typedef unsigned int type; };
    template<> struct superset<unsigned char,int> { typedef int type; };
    template<> struct superset<unsigned char,cimg_uint64> { typedef cimg_uint64 type; };
    template<> struct superset<unsigned char,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<unsigned char,float> { typedef float type; };
    template<> struct superset<unsigned char,double> { typedef double type; };
    template<> struct superset<signed char,unsigned char> { typedef short type; };
    template<> struct superset<signed char,char> { typedef short type; };
    template<> struct superset<signed char,unsigned short> { typedef int type; };
    template<> struct superset<signed char,short> { typedef short type; };
    template<> struct superset<signed char,unsigned int> { typedef cimg_int64 type; };
    template<> struct superset<signed char,int> { typedef int type; };
    template<> struct superset<signed char,cimg_uint64> { typedef cimg_int64 type; };
    template<> struct superset<signed char,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<signed char,float> { typedef float type; };
    template<> struct superset<signed char,double> { typedef double type; };
    template<> struct superset<char,unsigned char> { typedef short type; };
    template<> struct superset<char,signed char> { typedef short type; };
    template<> struct superset<char,unsigned short> { typedef int type; };
    template<> struct superset<char,short> { typedef short type; };
    template<> struct superset<char,unsigned int> { typedef cimg_int64 type; };
    template<> struct superset<char,int> { typedef int type; };
    template<> struct superset<char,cimg_uint64> { typedef cimg_int64 type; };
    template<> struct superset<char,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<char,float> { typedef float type; };
    template<> struct superset<char,double> { typedef double type; };
    template<> struct superset<unsigned short,char> { typedef int type; };
    template<> struct superset<unsigned short,signed char> { typedef int type; };
    template<> struct superset<unsigned short,short> { typedef int type; };
    template<> struct superset<unsigned short,unsigned int> { typedef unsigned int type; };
    template<> struct superset<unsigned short,int> { typedef int type; };
    template<> struct superset<unsigned short,cimg_uint64> { typedef cimg_uint64 type; };
    template<> struct superset<unsigned short,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<unsigned short,float> { typedef float type; };
    template<> struct superset<unsigned short,double> { typedef double type; };
    template<> struct superset<short,unsigned short> { typedef int type; };
    template<> struct superset<short,unsigned int> { typedef cimg_int64 type; };
    template<> struct superset<short,int> { typedef int type; };
    template<> struct superset<short,cimg_uint64> { typedef cimg_int64 type; };
    template<> struct superset<short,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<short,float> { typedef float type; };
    template<> struct superset<short,double> { typedef double type; };
    template<> struct superset<unsigned int,char> { typedef cimg_int64 type; };
    template<> struct superset<unsigned int,signed char> { typedef cimg_int64 type; };
    template<> struct superset<unsigned int,short> { typedef cimg_int64 type; };
    template<> struct superset<unsigned int,int> { typedef cimg_int64 type; };
    template<> struct superset<unsigned int,cimg_uint64> { typedef cimg_uint64 type; };
    template<> struct superset<unsigned int,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<unsigned int,float> { typedef float type; };
    template<> struct superset<unsigned int,double> { typedef double type; };
    template<> struct superset<int,unsigned int> { typedef cimg_int64 type; };
    template<> struct superset<int,cimg_uint64> { typedef cimg_int64 type; };
    template<> struct superset<int,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<int,float> { typedef float type; };
    template<> struct superset<int,double> { typedef double type; };
    template<> struct superset<cimg_uint64,char> { typedef cimg_int64 type; };
    template<> struct superset<cimg_uint64,signed char> { typedef cimg_int64 type; };
    template<> struct superset<cimg_uint64,short> { typedef cimg_int64 type; };
    template<> struct superset<cimg_uint64,int> { typedef cimg_int64 type; };
    template<> struct superset<cimg_uint64,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<cimg_uint64,float> { typedef double type; };
    template<> struct superset<cimg_uint64,double> { typedef double type; };
    template<> struct superset<cimg_int64,float> { typedef double type; };
    template<> struct superset<cimg_int64,double> { typedef double type; };
#if cimg_use_cpp11==1 && \
    (!(UINTPTR_MAX==0xffffffff || defined(__arm__) || defined(_M_ARM) || ((ULONG_MAX)==(UINT_MAX))))
    template<> struct superset<unsigned long long,char> { typedef cimg_int64 type; };
    template<> struct superset<unsigned long long,signed char> { typedef cimg_int64 type; };
    template<> struct superset<unsigned long long,short> { typedef cimg_int64 type; };
    template<> struct superset<unsigned long long,int> { typedef cimg_int64 type; };
    template<> struct superset<unsigned long long,cimg_int64> { typedef cimg_int64 type; };
    template<> struct superset<unsigned long long,float> { typedef double type; };
    template<> struct superset<unsigned long long,double> { typedef double type; };
    template<> struct superset<long long,float> { typedef double type; };
    template<> struct superset<long long,double> { typedef double type; };
#endif
    template<> struct superset<float,cimg_uint64> { typedef double type; };
    template<> struct superset<float,cimg_int64> { typedef double type; };
    template<> struct superset<float,double> { typedef double type; };
#if cimg_is_float16==1
    template<> struct superset<cimg_float16,unsigned short> { typedef float type; };
    template<> struct superset<cimg_float16,short> { typedef float type; };
    template<> struct superset<cimg_float16,unsigned int> { typedef float type; };
    template<> struct superset<cimg_float16,int> { typedef float type; };
    template<> struct superset<cimg_float16,cimg_uint64> { typedef float type; };
    template<> struct superset<cimg_float16,cimg_int64> { typedef float type; };
    template<> struct superset<cimg_float16,float> { typedef float type; };
    template<> struct superset<cimg_float16,double> { typedef double type; };
#endif

    template<typename t1, typename t2, typename t3> struct superset2 {
      typedef typename superset<t1, typename superset<t2,t3>::type>::type type;
    };

    template<typename t1, typename t2, typename t3, typename t4> struct superset3 {
      typedef typename superset<t1, typename superset2<t2,t3,t4>::type>::type type;
    };

    template<typename t1, typename t2> struct last { typedef t2 type; };

#define _cimg_Tt typename cimg::superset<T,t>::type
#define _cimg_Tfloat typename cimg::superset<T,float>::type
#define _cimg_tfloat typename cimg::superset<t,float>::type
#define _cimg_Ttfloat typename cimg::superset2<T,t,float>::type
#define _cimg_Ttdouble typename cimg::superset2<T,t,double>::type

#endif
