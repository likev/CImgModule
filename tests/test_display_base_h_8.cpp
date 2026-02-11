
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/display/display_base.h
    try {
CImgDisplay disp(100,100,"Test Display",0);
const char* window_title = "Test Window";
disp.set_title("%s",window_title);
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
