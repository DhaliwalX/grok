#include <iostream>
#include "../src/jsobject.h"

int main() {

    std::shared_ptr<JSNumber> num1 = std::make_shared<JSNumber>(10);
    Object l { num1 };
    std::shared_ptr<JSString> num2 = std::make_shared<JSString>("Prince Dhaliwal");
    Object r { num2 };

    try {
        std::cout << (num1 - num2).as<JSNumber>()->ToString() << std::endl;
    } catch (Exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}