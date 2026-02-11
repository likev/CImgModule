
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
CImg<float> img1(100,100), img2; // 'img1' is a 100x100 image, 'img2' is an empty image
       if (img1) { // Test succeeds, 'img1' is not an empty image
         if (!img2) { // Test succeeds, 'img2' is an empty image
           std::printf("'img1' is not empty, 'img2' is empty.");
         }
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
