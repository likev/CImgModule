
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/display/display_base.h
    try {
CImgDisplay disp(400,400);
       while (!disp.is_closed()) {
         if (disp.wheel()) {
           int counter = disp.wheel(); // Read the state of the mouse wheel
           // ... // Do what you want with 'counter'
           disp.set_wheel(); // Reset the wheel value to 0
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
