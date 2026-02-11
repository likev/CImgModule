
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/core/exceptions.h
    try {
#define "CImg.h"
      using namespace cimg_library;
      int main() {
        cimg::exception_mode(0); // Enable quiet exception mode
        try {
          ... // Here, do what you want to stress CImg
        } catch (CImgException& e) { // You succeeded: something went wrong!
          std::fprintf(stderr,"CImg Library Error: %s",e.what()); // Display your custom error message
          ... // Do what you want now to save the ship!
          }
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
