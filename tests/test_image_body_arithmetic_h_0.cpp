
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
CImg<float> img(100,100,1,3,0); // Construct a 100x100x1x3 (color) image with pixels set to '0'
       const float
          valR = img(10,10,0,0), // Read red value at coordinates (10,10)
          valG = img(10,10,0,1), // Read green value at coordinates (10,10)
          valB = img(10,10,2), // Read blue value at coordinates (10,10) (Z-coordinate can be omitted)
          avg = (valR + valG + valB)/3; // Compute average pixel value
       img(10,10,0) = img(10,10,1) = img(10,10,2) = avg; // Replace the color pixel (10,10) by the average grey value
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
