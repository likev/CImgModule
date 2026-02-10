#ifndef CIMG_MODULE_CORE_TYPES_H
#define CIMG_MODULE_CORE_TYPES_H

// Define own datatypes to ensure portability.
// ( 'sizeof(cimg_ulong/cimg_long) = sizeof(void*)' ).
#define cimg_uint8 unsigned char
#if defined(CHAR_MAX) && CHAR_MAX==255
#define cimg_int8 signed char
#else
#define cimg_int8 char
#endif
#define cimg_uint16 unsigned short
#define cimg_int16 short
#define cimg_uint32 unsigned int
#define cimg_int32 int
#define cimg_float32 float
#define cimg_float64 double

#if cimg_OS==2

#define cimg_uint64 unsigned __int64
#define cimg_int64 __int64
#define cimg_ulong UINT_PTR
#define cimg_long INT_PTR
#ifdef _MSC_VER
#define cimg_fuint64 "%I64u"
#define cimg_fint64 "%I64d"
#else
#define cimg_fuint64 "%llu"
#define cimg_fint64 "%lld"
#endif
#define cimg_fhex64 "%llx"

#else

#if UINTPTR_MAX==0xffffffff || defined(__arm__) || defined(_M_ARM) || ((ULONG_MAX)==(UINT_MAX))
#define cimg_uint64 unsigned long long
#define cimg_int64 long long
#define cimg_fuint64 "%llu"
#define cimg_fint64 "%lld"
#define cimg_fhex64 "%llx"
#else
#define cimg_uint64 unsigned long
#define cimg_int64 long
#define cimg_fuint64 "%lu"
#define cimg_fint64 "%ld"
#define cimg_fhex64 "%lx"
#endif

#if defined(__arm__) || defined(_M_ARM)
#define cimg_ulong unsigned long long
#define cimg_long long long
#else
#define cimg_ulong unsigned long
#define cimg_long long
#endif

#endif

#ifndef cimg_max_buf_size
#if UINTPTR_MAX==0xffffffff
#define cimg_max_buf_size ((cimg_ulong)3*1024*1024*1024)
#else
#define cimg_max_buf_size ((cimg_ulong)16*1024*1024*1024)
#endif
#endif

#endif
