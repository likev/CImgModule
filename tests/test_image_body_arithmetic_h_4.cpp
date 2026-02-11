
#define cimg_display 0
#define cimg_use_jpeg
#include <cstring>
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main(int argc, char** argv) {
    try {
CImg<float> img1(100,100), img2(img1), img3(img1); // Declare 3 scalar images 100x100 with uninitialized values
       img1 = "0,50,100,150,200,250,200,150,100,50"; // Set pixel values of 'img1' from a value sequence
       img2 = "10*((x*y)%25)"; // Set pixel values of 'img2' from a formula
       img3 = "reference.jpg"; // Set pixel values of 'img3' from a file (image size is modified)
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
