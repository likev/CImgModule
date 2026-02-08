#include "mylib.h"

int main() {
    int sum = mylib::add(10, 20);
    mylib::print_result(sum);

    mylib::Calculator<double> calc(10.0);
    calc.add(5.5);
    calc.multiply(2.0);
    mylib::print_result(calc.get_value());

    return 0;
}
