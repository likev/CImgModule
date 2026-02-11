
#define cimg_display 0
#define cimg_use_jpeg
#include <cstring>
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main(int argc, char** argv) {
    try {
const CImg<float>
          img_x(100,100,1,1,"x-w/2",false), // Define an horizontal centered gradient, from '-width/2' to 'width/2'
          img_y(100,100,1,1,"y-h/2",false), // Define a vertical centered gradient, from '-height/2' to 'height/2'
          img_atan2 = img_y.get_atan2(img_x); // Compute atan2(y,x) for each pixel value
       // (img_x,img_y,img_atan2).display();
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
