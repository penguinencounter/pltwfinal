#include <iostream>
#include <wiringPi.h>

int main() {
    wiringPiSetup();
    std::cout << "Hello, world?!\n";
    return 0;
}