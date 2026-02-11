
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_ops_basic.h
    try {
CImg<float> src(256,256,1,3,0), // Construct a 256x256x1x3 (color) image filled with value '0'
                   dest(16,16); // Construct a 16x16x1x1 (scalar) image
       src.move_to(dest); // Now, 'src' is empty and 'dest' is the 256x256x1x3 image
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
