#ifndef MYLIB_CLASS_ADD_H
#define MYLIB_CLASS_ADD_H

namespace mylib {

template <typename T>
void Calculator<T>::add(const T& value) {
    current_value += value;
}

} // namespace mylib

#endif // MYLIB_CLASS_ADD_H
