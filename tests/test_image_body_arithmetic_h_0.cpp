
#define cimg_display 0
#define cimg_use_jpeg
#include <cstring>
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main(int argc, char** argv) {
    try {
CImg<float> img(100,100,1,3,0); // Construct a 100x100x1x3 (color) image with pixels set to '0'
       const float
          valR = img(10,10,0,0), // Read red value at coordinates (10,10)
          valG = img(10,10,0,1), // Read green value at coordinates (10,10)
          valB = img(10,10,2), // Read blue value at coordinates (10,10) (Z-coordinate can be omitted)
          avg = (valR + valG + valB)/3; // Compute average pixel value
       img(10,10,0) = img(10,10,1) = img(10,10,2) = avg; // Replace the color pixel (10,10) by the average grey value
                } catch (const CImgException& e) {
        const char *const msg = e.what();
        if (msg && std::strstr(msg, "No display available")) return 0;
        std::cerr << "Test failed with CImg exception: " << (msg?msg:"null") << std::endl;
        return 1;
    } catch (const std::exception& e) {
        const char *const msg = e.what();
        if (msg && std::strstr(msg, "No display available")) return 0;
        std::cerr << "Test failed with std exception: " << (msg?msg:"null") << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
    return 0;
}
