#ifndef MYLIB_CLASS_H
#define MYLIB_CLASS_H

namespace mylib {

template <typename T>
class Calculator {
public:
    Calculator(T initial_value) : current_value(initial_value) {}

    void add(const T& value) {
        current_value += value;
    }

    void multiply(const T& value) {
        current_value *= value;
    }

    T get_value() const {
        return current_value;
    }

private:
    T current_value;
};

} // namespace mylib

#endif // MYLIB_CLASS_H
