
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/core/cimg_namespace_runtime.h
    try {
const double
       res1 = cimg::eval("cos(x)^2 + sin(y)^2",2,2), // will return '1'
       res2 = cimg::eval(0,1,1); // will return '1' too
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
