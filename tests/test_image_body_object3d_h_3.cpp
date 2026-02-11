
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_object3d.h
    try {
CImgList<unsigned int> faces3d;
       const CImg<float> points3d = CImg<float>::box3d(faces3d,10,20,30);
       CImg<unsigned char>().display_object3d("Box3d",points3d,faces3d);
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
