#ifndef MYLIB_CLASS_DECL_H
#define MYLIB_CLASS_DECL_H

namespace mylib {

template <typename T>
class Calculator {
public:
    Calculator(T initial_value) : current_value(initial_value) {}

    void add(const T& value);
    void multiply(const T& value);

    T get_value() const {
        return current_value;
    }

private:
    T current_value;
};

} // namespace mylib

#include "mylib_class_add.h"
#include "mylib_class_multiply.h"

#endif // MYLIB_CLASS_DECL_H
