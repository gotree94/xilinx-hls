// aes128_simple_tb.cpp
// Simplified Testbench for debugging

#include <stdio.h>
#include "ap_int.h"

typedef ap_uint<8> uint8;

void aes128_encrypt(uint8 plaintext[16], uint8 key[16], uint8 ciphertext[16]);

int main() {
    // Single test - NIST FIPS-197
    uint8 plaintext[16] = {
        0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
        0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
    };
    
    uint8 key[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };
    
    uint8 ciphertext[16];
    
    uint8 expected[16] = {
        0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb,
        0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32
    };
    
    printf("Testing AES-128 Encryption\n");
    printf("Plaintext: ");
    for (int i = 0; i < 16; i++) printf("%02x", (int)plaintext[i]);
    printf("\n");
    
    printf("Key:       ");
    for (int i = 0; i < 16; i++) printf("%02x", (int)key[i]);
    printf("\n");
    
    // Call DUT
    aes128_encrypt(plaintext, key, ciphertext);
    
    printf("Ciphertext: ");
    for (int i = 0; i < 16; i++) printf("%02x", (int)ciphertext[i]);
    printf("\n");
    
    printf("Expected:   ");
    for (int i = 0; i < 16; i++) printf("%02x", (int)expected[i]);
    printf("\n");
    
    // Check result
    int pass = 1;
    for (int i = 0; i < 16; i++) {
        if (ciphertext[i] != expected[i]) {
            printf("FAIL at byte %d: got %02x, expected %02x\n", 
                   i, (int)ciphertext[i], (int)expected[i]);
            pass = 0;
        }
    }
    
    if (pass) {
        printf("\nTEST PASSED\n");
        return 0;
    } else {
        printf("\nTEST FAILED\n");
        return 1;
    }
}