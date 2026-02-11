
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
CImg<float> img2(100,100,1,3);
const CImg<float>
          img1("reference.jpg"),
          img2 = img1.get_mirror('x'),
          img3 = img2.get_blur(5);
       const CImgList<float> list = (img1,img2); // Create list of two elements from 'img1' and 'img2'
       (list,img3).display(); // Display image list containing copies of 'img1','img2' and 'img3'
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
