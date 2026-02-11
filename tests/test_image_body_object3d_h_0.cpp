
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_object3d.h
    try {
const CImg<float> img("reference.jpg");
       CImgList<unsigned int> faces3d;
       CImgList<unsigned char> colors3d;
       const CImg<float> points3d = img.get_elevation3d(faces3d,colors3d,img.get_norm()*0.2);
       CImg<unsigned char>().display_object3d("Elevation3d",points3d,faces3d,colors3d);
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
