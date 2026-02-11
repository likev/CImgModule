
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_pixels.h
    try {
const CImg<float> img(100,100,1,3); // Define a 100x100 RGB-color image
       const long off = img.offset(10,10,0,2); // Get the offset of the blue value of the pixel located at (10,10)
       const float val = img[off]; // Get the blue value of this pixel
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
