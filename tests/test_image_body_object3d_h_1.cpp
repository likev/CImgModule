
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
       CImg<float> img("reference.jpg");
       CImgList<unsigned int> faces3d;
       const CImg<float> points3d = img.get_channel(0).get_isoline3d(faces3d,100);
       CImg<unsigned char> colors3d;
       CImg<unsigned char>().display_object3d("Isoline3d",points3d,faces3d,colors3d);
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
