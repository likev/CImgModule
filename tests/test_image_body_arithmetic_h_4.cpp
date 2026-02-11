
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
CImg<float> img1(100,100), img2(img1), img3(img1); // Declare 3 scalar images 100x100 with uninitialized values
       img1 = "0,50,100,150,200,250,200,150,100,50"; // Set pixel values of 'img1' from a value sequence
       img2 = "10*((x*y)%25)"; // Set pixel values of 'img2' from a formula
       img3 = "reference.jpg"; // Set pixel values of 'img3' from a file (image size is modified)
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
