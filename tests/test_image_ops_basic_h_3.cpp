
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_ops_basic.h
    try {
const CImg<float> img(2,2,1,3, // Construct a 2x2 color (RGB) image
                             { 0,255,0,255, // Set the 4 values for the red component
                               0,0,255,255, // Set the 4 values for the green component
                               64,64,64,64 }); // Set the 4 values for the blue component
       img.resize(150,150).display();
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
