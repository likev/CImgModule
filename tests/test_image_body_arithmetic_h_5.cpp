
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
CImg<unsigned char> img(100,100,1,1,255); // Construct a 100x100 image with pixel values '255'
       img+=1; // Add '1' to each pixels -> Overflow
       // here all pixels of image 'img' are equal to '0'.
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
