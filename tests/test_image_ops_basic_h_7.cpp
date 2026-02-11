
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_ops_basic.h
    try {
unsigned char tab[256*256] = {};
       CImg<unsigned char> img1(tab,256,256,1,1,false), // Construct new non-shared image from buffer 'tab'
                           img2(tab,256,256,1,1,true); // Construct new shared-image from buffer 'tab'
       tab[1024] = 255; // Here, 'img2' is indirectly modified, but not 'img1'
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
