
#include "CImg.h"
#include <iostream>
#include <cstdio>
using namespace cimg_library;

int main() {
    // Extracted from module/containers/list_base.h
    try {
CImgList<float> list(4, 100, 100, 1, 3);
cimglist_for(list,l) list[l].mirror('x');
    } catch (const CImgException& e) {
        std::cerr << "CImg Exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
