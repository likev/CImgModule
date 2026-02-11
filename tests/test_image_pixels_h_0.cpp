
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_pixels.h
    try {
const CImg<float> img(100,100,1,3); // Construct new 100x100 color image
       if (img.size()==30000) // Test succeeds
         std::printf("Pixel buffer uses %lu bytes",
                     img.size()*sizeof(float));
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
