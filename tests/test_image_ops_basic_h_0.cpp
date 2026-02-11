
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_ops_basic.h
    try {
CImg<float> img2(100,100,1,3);
CImg<float> img1, img2; // Construct two empty images
       img1.assign(256,256,1,3); // Re-assign 'img1' to be a 256x256x1x3 (color) image
       img2 = img1.get_rand(0,255); // Re-assign 'img2' to be a random-valued version of 'img1'
       img2.assign(); // Re-assign 'img2' to be an empty image again
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
