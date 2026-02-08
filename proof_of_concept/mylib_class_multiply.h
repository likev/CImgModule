#ifndef MYLIB_CLASS_MULTIPLY_H
#define MYLIB_CLASS_MULTIPLY_H

namespace mylib {

template <typename T>
void Calculator<T>::multiply(const T& value) {
    current_value *= value;
}

} // namespace mylib

#endif // MYLIB_CLASS_MULTIPLY_H
