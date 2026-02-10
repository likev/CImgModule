#ifndef CIMG_MODULE_IO_FORMATS_IMAGE_H
#define CIMG_MODULE_IO_FORMATS_IMAGE_H

// Configure Magick++ support.
// (http://www.imagemagick.org/Magick++)
//
// Define 'cimg_use_magick' to enable Magick++ support.
//
// Magick++ library may be used to get a native support of various image file formats.
// (see methods 'CImg<T>::{load,save}()').
#ifdef cimg_use_magick
#include "Magick++.h"
#endif

// Configure FFTW3 support.
// (http://www.fftw.org)
//
// Define 'cimg_use_fftw3' to enable libFFTW3 support.
//
// FFTW3 library may be used to efficiently compute the Fast Fourier Transform.
// of image data, without restriction on the image size.
// (see method 'CImg[List]<T>::FFT()').
#ifdef cimg_use_fftw3
extern "C" {
#include "fftw3.h"
}
#endif

// Configure OpenEXR support.
// (http://www.openexr.com/)
//
// Define 'cimg_use_openexr' to enable OpenEXR support.
//
// OpenEXR library may be used to get a native support of '.exr' files.
// (see methods 'CImg<T>::{load,save}_exr()').
#ifdef cimg_use_openexr
#if __GNUC__>=5
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include "ImfRgbaFile.h"
#include "ImfInputFile.h"
#include "ImfChannelList.h"
#include "ImfMatrixAttribute.h"
#include "ImfArray.h"
#if __GNUC__>=5
#pragma GCC diagnostic pop
#endif
#endif

// Configure TinyEXR support.
// (https://github.com/syoyo/tinyexr)
//
// Define 'cimg_use_tinyexr' to enable TinyEXR support.
//
// TinyEXR is a small, single header-only library to load and save OpenEXR(.exr) images.
#ifdef cimg_use_tinyexr
#ifndef TINYEXR_IMPLEMENTATION
#define TINYEXR_IMPLEMENTATION
#endif
#include "tinyexr.h"
#endif

#endif
