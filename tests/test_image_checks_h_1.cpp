
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_checks.h
    try {
const CImg<float>
         img1("reference.jpg"), // Load RGB-color image
         img2 = img1.get_shared_channel(1); // Get shared version of the green channel
       if (img1.is_overlapped(img2)) { // Test succeeds, 'img1' and 'img2' overlaps
         std::printf("Buffers overlap!\n");
       }
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
