
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_draw_1.h
    try {
CImg<unsigned char> img(100,100,1,3,0), texture("texture256x256.ppm");
       const unsigned char color[] = { 255,128,64 };
       img.draw_line(40,40,80,70,texture,0,0,255,255);
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
