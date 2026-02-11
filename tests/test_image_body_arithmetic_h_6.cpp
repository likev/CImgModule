
#define cimg_display 0
#define cimg_use_jpeg
#include <cstring>
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main(int argc, char** argv) {
    try {
CImg<unsigned char> img1("reference.jpg"); // Load a 8-bits RGB image (values in [0,255])
       CImg<float> img2(img1); // Construct a float-valued copy of 'img1'
       img2+=100; // Add '100' to pixel values -> goes out of [0,255] but no problems with floats
       img2.cut(0,255); // Cut values in [0,255] to fit the 'unsigned char' constraint
       img1 = img2; // Rewrite safe result in 'unsigned char' version 'img1'
       const CImg<unsigned char> img3 = (img1 + 100).cut(0,255); // Do the same in a more simple and elegant way
       // (img1,img2,img3).display();
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
