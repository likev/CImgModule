
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_ops_basic.h
    try {
const CImg<float> img1(256,128,1,3), // 'img1' is a 256x128x1x3 image
                         img2(img1,"xyzc"), // 'img2' is a 256x128x1x3 image
                         img3(img1,"y,x,z,c"), // 'img3' is a 128x256x1x3 image
                         img4(img1,"c,x,y,3",0), // 'img4' is a 3x128x256x3 image (with pixels initialized to '0')
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
