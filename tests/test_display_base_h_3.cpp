
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/display/display_base.h
    try {
CImgDisplay disp(400,400);
       const char *const keycode = "TAB";
       while (!disp.is_closed()) {
         if (disp.is_key(keycode)) { // ... } // Equivalent to 'if (disp.is_keyTAB())'
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
