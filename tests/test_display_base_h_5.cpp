
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/display/display_base.h
    try {
CImgDisplay disp(400,400);
       while (!disp.is_closed()) {
         if (disp.button()&1) { // Left button clicked
           ...
         }
         if (disp.button()&2) { // Right button clicked
           ...
         }
         if (disp.button()&4) { // Middle button clicked
           ...
         }
         disp.wait();
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
