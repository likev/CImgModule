
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
CImg<unsigned char> img1("reference.jpg"); // Load a 8-bits RGB image (values in [0,255])
       CImg<float> img2(img1); // Construct a float-valued copy of 'img1'
       img2+=100; // Add '100' to pixel values -> goes out of [0,255] but no problems with floats
       img2.cut(0,255); // Cut values in [0,255] to fit the 'unsigned char' constraint
       img1 = img2; // Rewrite safe result in 'unsigned char' version 'img1'
       const CImg<unsigned char> img3 = (img1 + 100).cut(0,255); // Do the same in a more simple and elegant way
       (img1,img2,img3).display();
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
