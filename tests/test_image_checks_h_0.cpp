
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_checks.h
    try {
const CImg<float> img(100,100,1,3); // Construct a 100x100 RGB color image
       const unsigned long offset = 1249; // Offset to the pixel (49,12,0,0)
       unsigned int x,y,z,c;
       if (img.contains(img[offset],x,y,z,c)) { // Convert offset to (x,y,z,c) coordinates
         std::printf("Offset %u refers to pixel located at (%u,%u,%u,%u).\n",
                     offset,x,y,z,c);
       }
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
