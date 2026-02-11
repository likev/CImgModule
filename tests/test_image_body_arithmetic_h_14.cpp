
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
const CImg<float>
         img0("reference.jpg"), // Load reference color image
         img1 = (img0/255).pow(1.8)*=255, // Compute gamma correction, with gamma = 1.8
         img2 = (img0/255).pow(0.5)*=255; // Compute gamma correction, with gamma = 0.5
       (img0,img1,img2).display();
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
