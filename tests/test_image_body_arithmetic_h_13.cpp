
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
CImg<float>
         img("reference.jpg"),
         shade(img.width,img.height(),1,1,"-(x-w/2)^2-(y-h/2)^2",false);
       shade.normalize(0,1);
       (img,shade,img.get_mul(shade)).display();
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
