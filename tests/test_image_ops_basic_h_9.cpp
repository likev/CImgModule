#define cimg_display 0
#define cimg_use_jpeg
#include <cstring>
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    try {
       CImg<float> img1(256,128,1,3), // 'img1' is a 256x128x1x3 image
                         img2(img1,"xyzc"), // 'img2' is a 256x128x1x3 image
                         img3(img1,"y,x,z,c"), // 'img3' is a 128x256x1x3 image
                         img4(img1,"c,x,y,3",0); // 'img4' is a 3x128x256x3 image
    } catch (const CImgException& e) {
        const char *const msg = e.what();
        if (msg && std::strstr(msg, "No display available")) return 0;
        std::cerr << "Test failed with CImg exception: " << (msg?msg:"null") << std::endl;
        return 1;
    } catch (const std::exception& e) {
        const char *const msg = e.what();
        if (msg && std::strstr(msg, "No display available")) return 0;
        std::cerr << "Test failed with std exception: " << (msg?msg:"null") << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
    return 0;
}
