// led_counter_tb.cpp
// Testbench for LED Counter

#include <iostream>
#include "ap_int.h"

// 테스트할 함수 선언
void led_counter(ap_uint<8> &led_out, ap_uint<1> enable);

int main() {
    ap_uint<8> led;
    ap_uint<1> en;

    std::cout << "=== LED Counter Testbench ===" << std::endl;

    // Test 1: Counter disabled (enable = 0)
    std::cout << "\nTest 1: Enable = 0 (Counter should not increment)" << std::endl;
    en = 0;
    for (int i = 0; i < 5; i++) {
        led_counter(led, en);
        std::cout << "Cycle " << i << ": LED = " << (int)led << " (0x" << std::hex << (int)led << std::dec << ")" << std::endl;
    }

    // Test 2: Counter enabled (enable = 1)
    std::cout << "\nTest 2: Enable = 1 (Counter should increment)" << std::endl;
    en = 1;
    for (int i = 0; i < 20; i++) {
        led_counter(led, en);
        std::cout << "Cycle " << i << ": LED = " << (int)led << " (0x" << std::hex << (int)led << std::dec << ")" << std::endl;
    }

    // Test 3: Toggle enable
    std::cout << "\nTest 3: Toggle Enable" << std::endl;
    for (int i = 0; i < 10; i++) {
        en = i % 2;  // 0, 1, 0, 1, ...
        led_counter(led, en);
        std::cout << "Cycle " << i << ": Enable = " << (int)en << ", LED = " << (int)led << std::endl;
    }

    std::cout << "\n=== Testbench completed successfully! ===" << std::endl;
    return 0;
}
