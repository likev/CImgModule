
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/core/exceptions.h
    try {
CImg<float> img(100,100,1,3); // Define a 100x100 color image with float-valued pixels
      img.mirror('e'); // Try to mirror image along the (non-existing) 'e'-axis
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
