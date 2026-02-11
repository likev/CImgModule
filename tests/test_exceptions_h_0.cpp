
#define cimg_display 0
#define cimg_use_jpeg
#include <cstring>
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main(int argc, char** argv) {
    try {
CImg<float> img(100,100,1,3); // Define a 100x100 color image with float-valued pixels
      img.mirror('e'); // Try to mirror image along the (non-existing) 'e'-axis
                } catch (const CImgException& e) {
        return 0; // Expected CImg exception
    } catch (const CImgAbortException& e) {
        return 0; // Expected CImg abort exception
    } catch (const std::exception& e) {
        const char *const msg = e.what();
        if (msg && std::strstr(msg, "No display available")) return 0;
        std::cerr << "Test failed with unexpected exception: " << (msg?msg:"null") << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
    return 0;
}
