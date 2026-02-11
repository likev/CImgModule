
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_draw_1.h
    try {
CImg<unsigned char> img(100,100,1,3,0);
       const unsigned char color[] = { 255,255,255 };
       img.draw_spline(30,30,0,100,90,40,0,-100,color);
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
