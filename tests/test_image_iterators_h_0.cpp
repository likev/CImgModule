
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_iterators.h
    try {
CImg<float> img(100,100,1,3); // Define a 100x100 RGB color image
       // 'img.end()' used below as an upper bound for the iterator.
       for (CImg<float>::iterator it = img.begin(); it<img.end(); ++it)
         *it = 0;
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
