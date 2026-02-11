
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
const CImg<unsigned char> img("reference.jpg"); // Load a RGB color image
       const CImgList<unsigned char> list = (img<'c'); // Get a list of its three R,G,B channels
       (img,list).display();
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
