
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/image/image_body_arithmetic.h
    try {
CImg<float> A(2,2,1,1, 1,2,3,4); // Construct 2x2 matrix A = [1,2;3,4]
       const CImg<float> X(1,2,1,1, 1,2); // Construct 1x2 vector X = [1;2]
       A*=X; // Assign matrix multiplication A*X to 'A'
       // 'A' is now a 1x2 vector whose values are [5;11].
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
