
#define cimg_display 0
#define cimg_use_jpeg
#include <cstring>
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main(int argc, char** argv) {
    try {
CImg<float> img1, img2; // Construct two empty images
       img1.assign(256,256,1,3); // Re-assign 'img1' to be a 256x256x1x3 (color) image
       img2 = img1.get_rand(0,255); // Re-assign 'img2' to be a random-valued version of 'img1'
       img2.assign(); // Re-assign 'img2' to be an empty image again
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
