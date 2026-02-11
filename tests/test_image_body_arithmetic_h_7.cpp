
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
CImg<float> img1("reference.jpg"); // Load a RGB color image (img1.spectrum()==3)
       // Construct a scalar shading (img2.spectrum()==1).
       const CImg<float> img2(img1.width(),img.height(),1,1,"255*(x/w)^2");
       img1+=img2; // Add shading to each channel of 'img1'
       img1.cut(0,255); // Prevent [0,255] overflow
       (img2,img1).display();
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
