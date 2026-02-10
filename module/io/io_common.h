#ifndef CIMG_MODULE_IO_COMMON_H
#define CIMG_MODULE_IO_COMMON_H

// Configure OpenCV support.
// (http://opencv.willowgarage.com/wiki/)
//
// Define 'cimg_use_opencv' to enable OpenCV support.
//
// OpenCV library may be used to access images from cameras.
// (see method 'CImg<T>::load_camera()').
#ifdef cimg_use_opencv
#ifdef True
#undef True
#define _cimg_redefine_True
#endif
#ifdef False
#undef False
#define _cimg_redefine_False
#endif
#ifdef Status
#undef Status
#define _cimg_redefine_Status
#endif
#include <cstddef>
#include <opencv2/opencv.hpp>
#if CV_MAJOR_VERSION>=3
#define _cimg_fourcc cv::VideoWriter::fourcc
#define _cimg_cap_prop_frame_width cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH
#define _cimg_cap_prop_frame_height cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT
#define _cimg_cap_prop_frame_count cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT
#else
#define _cimg_fourcc CV_FOURCC
#define _cimg_cap_prop_frame_width CV_CAP_PROP_FRAME_WIDTH
#define _cimg_cap_prop_frame_height CV_CAP_PROP_FRAME_HEIGHT
#define _cimg_cap_prop_frame_count CV_CAP_PROP_FRAME_COUNT
#endif
#endif

// Configure LibPNG support.
// (http://www.libpng.org)
//
// Define 'cimg_use_png' to enable LibPNG support.
//
// PNG library may be used to get a native support of '.png' files.
// (see methods 'CImg<T>::{load,save}_png()'.
#ifdef cimg_use_png
extern "C" {
#include "png.h"
}
#endif

// Configure LibJPEG support.
// (http://en.wikipedia.org/wiki/Libjpeg)
//
// Define 'cimg_use_jpeg' to enable LibJPEG support.
//
// JPEG library may be used to get a native support of '.jpg' files.
// (see methods 'CImg<T>::{load,save}_jpeg()').
#ifdef cimg_use_jpeg
extern "C" {
#include "jpeglib.h"
#include "setjmp.h"
}
#endif

// Configure JPEG XL support.
// (https://en.wikipedia.org/wiki/JPEG_XL)
//
// Define 'cimg_use_jxl' to enable JPEG XL support.
//
// Libjxl may be used to get a native support of '.jxl' files.
// (see methods 'CImg<T>::{load,save}_jxl()').
#ifdef cimg_use_jxl
#include <jxl/decode.h>
#include <jxl/encode.h>
#endif

// Configure LibTIFF support.
// (http://www.libtiff.org)
//
// Define 'cimg_use_tiff' to enable LibTIFF support.
//
// TIFF library may be used to get a native support of '.tif' files.
// (see methods 'CImg[List]<T>::{load,save}_tiff()').
#ifdef cimg_use_tiff
extern "C" {
#define uint64 uint64_hack_
#define int64 int64_hack_
#include "tiffio.h"
#undef uint64
#undef int64
}
#endif

// Configure HEIF support.
// (https://github.com/strukturag/libheif)
//
// Define 'cimg_use_heif' to enable HEIF support.
//
// HEIF library may be used to get a native support of '.heic' and '.avif' files.
// (see method 'CImg<T>::load_heif()').
#ifdef cimg_use_heif
#include <libheif/heif_cxx.h>
#endif

// Configure WebP support.
// (https://chromium.googlesource.com/webm/libwebp/)
//
// Define 'cimg_use_webp' to enable WebP support.
//
// WebP library may be used to get a native support of '.webp' files.
// (see method 'CImg<T>::{load,save}_webp()').
#ifdef cimg_use_webp
#include <webp/decode.h>
#include <webp/encode.h>
#endif

// Configure LibMINC2 support.
// (http://en.wikibooks.org/wiki/MINC/Reference/MINC2.0_File_Format_Reference)
//
// Define 'cimg_use_minc2' to enable LibMINC2 support.
//
// MINC2 library may be used to get a native support of '.mnc' files.
// (see methods 'CImg<T>::{load,save}_minc2()').
#ifdef cimg_use_minc2
#include "minc_io_simple_volume.h"
#include "minc_1_simple.h"
#include "minc_1_simple_rw.h"
#endif

// Configure Zlib support.
// (http://www.zlib.net)
//
// Define 'cimg_use_zlib' to enable Zlib support.
//
// Zlib library may be used to allow compressed data in '.cimgz' files.
// (see methods 'CImg[List]<T>::{load,save}_cimg()').
#ifdef cimg_use_zlib
extern "C" {
#include "zlib.h"
}
#endif

// Configure libcurl support.
// (http://curl.haxx.se/libcurl/)
//
// Define 'cimg_use_curl' to enable libcurl support.
//
// Libcurl may be used to get a native support of file downloading from the network.
// (see method 'cimg::load_network()'.)
#ifdef cimg_use_curl
#include "curl/curl.h"
#endif

#endif
