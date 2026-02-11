
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_class_decl.h
    try {
CImg<float> img("reference.jpg"); // Load image from file
       // Set all pixels to '0', with a CImg iterator.
       for (CImg<float>::iterator it = img.begin(), it<img.end(); ++it) *it = 0;
       img.fill(0); // Do the same with a built-in method
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
