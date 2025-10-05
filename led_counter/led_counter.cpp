// led_counter.cpp
// Vitis HLS 2022.2 - Basys3 LED Counter Example
// 간단한 8비트 카운터로 LED를 제어하는 예제

#include "ap_int.h"

// 함수 프로토타입
void led_counter(ap_uint<8> &led_out, ap_uint<1> enable);

// HLS Top Function
void led_counter(ap_uint<8> &led_out, ap_uint<1> enable) {
    #pragma HLS INTERFACE mode=ap_none port=led_out
    #pragma HLS INTERFACE mode=ap_none port=enable
    #pragma HLS INTERFACE mode=ap_ctrl_none port=return

    static ap_uint<8> counter = 0;

    if (enable == 1) {
        counter++;
    }

    led_out = counter;
}
