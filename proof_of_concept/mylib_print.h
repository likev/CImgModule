#ifndef MYLIB_PRINT_H
#define MYLIB_PRINT_H

#include <iostream>

namespace mylib {

template <typename T>
void print_result(const T& result) {
    std::cout << "Result: " << result << std::endl;
}

} // namespace mylib

#endif // MYLIB_PRINT_H
