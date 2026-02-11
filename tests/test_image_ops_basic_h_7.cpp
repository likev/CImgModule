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
       CImg<float> img2(100,100,1,3);
       unsigned char tab[256*256] = {};
       CImg<unsigned char> img1(tab,256,256,1,1,false); // Construct new non-shared image from buffer 'tab'
       CImg<unsigned char> img3(tab,256,256,1,1,true); // Construct new shared-image from buffer 'tab'
       tab[1024] = 255; // Here, 'img3' is indirectly modified, but not 'img1'
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
