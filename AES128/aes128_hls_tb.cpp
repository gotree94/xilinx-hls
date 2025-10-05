// aes128_hls_tb.cpp
// Complete Testbench for AES-128 HLS Implementation
// Tests 4 different test vectors to verify correctness

#include <stdio.h>
#include <string.h>
#include "ap_int.h"

typedef ap_uint<8> uint8;

// Function under test
void aes128_encrypt(uint8 plaintext[16], uint8 key[16], uint8 ciphertext[16]);

// ========================================================================
// Helper Functions
// ========================================================================

void print_hex(const char* label, uint8 data[16]) {
    printf("%s: ", label);
    for (int i = 0; i < 16; i++) {
        printf("%02x", (unsigned int)data[i]);
    }
    printf("\n");
}

void print_separator() {
    printf("--------------------------------------------------------------------------------\n");
}

int compare_arrays(uint8 a[16], uint8 b[16]) {
    for (int i = 0; i < 16; i++) {
        if (a[i] != b[i]) {
            return 0;  // Not equal
        }
    }
    return 1;  // Equal
}

void copy_array(uint8 dest[16], const unsigned char src[16]) {
    for (int i = 0; i < 16; i++) {
        dest[i] = src[i];
    }
}

// ========================================================================
// Test Vectors
// ========================================================================

struct TestVector {
    const char* name;
    unsigned char plaintext[16];
    unsigned char key[16];
    unsigned char expected[16];
};

TestVector test_vectors[4] = {
    // Test 1: NIST FIPS-197 Standard Test Vector
    {
        "NIST FIPS-197 Standard Test Vector",
        {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
         0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34},
        {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
         0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c},
        {0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb,
         0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32}
    },
    
    // Test 2: All Zeros
    {
        "All Zeros Plaintext and Key",
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b,
         0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e}
    },
    
    // Test 3: All Ones
    {
        "All Ones Plaintext and Key",
        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        {0xa1, 0xf6, 0x25, 0x8c, 0x87, 0x7d, 0x5f, 0xcd,
         0x89, 0x64, 0x48, 0x45, 0x38, 0xbf, 0xc9, 0x2c}
    },
    
    // Test 4: Pattern
    {
        "Pattern Plaintext",
        {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
         0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
        {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
         0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
        {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
         0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a}
    }
};

// ========================================================================
// Main Test Function
// ========================================================================

int main() {
    int total_tests = 4;
    int passed_tests = 0;
    int failed_tests = 0;
    
    printf("\n");
    printf("================================================================================\n");
    printf("                   AES-128 HLS Implementation Testbench\n");
    printf("================================================================================\n");
    printf("\n");
    
    // Run all test vectors
    for (int test_num = 0; test_num < total_tests; test_num++) {
        printf("Test %d: %s\n", test_num + 1, test_vectors[test_num].name);
        print_separator();
        
        // Prepare test data
        uint8 plaintext[16];
        uint8 key[16];
        uint8 ciphertext[16];
        uint8 expected[16];
        
        copy_array(plaintext, test_vectors[test_num].plaintext);
        copy_array(key, test_vectors[test_num].key);
        copy_array(expected, test_vectors[test_num].expected);
        
        // Print inputs
        printf("Input:\n");
        print_hex("  Plaintext ", plaintext);
        print_hex("  Key       ", key);
        
        // Call DUT (Device Under Test)
        aes128_encrypt(plaintext, key, ciphertext);
        
        // Print outputs
        printf("Output:\n");
        print_hex("  Ciphertext", ciphertext);
        print_hex("  Expected  ", expected);
        
        // Verify result
        if (compare_arrays(ciphertext, expected)) {
            printf("  Result: PASS ✓\n");
            passed_tests++;
        } else {
            printf("  Result: FAIL ✗\n");
            printf("  ERROR: Ciphertext does not match expected value!\n");
            
            // Show detailed mismatch
            printf("  Mismatch at bytes: ");
            for (int i = 0; i < 16; i++) {
                if (ciphertext[i] != expected[i]) {
                    printf("%d ", i);
                }
            }
            printf("\n");
            
            failed_tests++;
        }
        
        printf("\n");
    }
    
    // Print summary
    printf("================================================================================\n");
    printf("                              Test Summary\n");
    printf("================================================================================\n");
    printf("  Total Tests:  %d\n", total_tests);
    printf("  Passed:       %d\n", passed_tests);
    printf("  Failed:       %d\n", failed_tests);
    printf("  Success Rate: %.1f%%\n", (passed_tests * 100.0) / total_tests);
    printf("================================================================================\n");
    printf("\n");
    
    // Final verdict
    if (failed_tests == 0) {
        printf("  ███████████████████████████████████████\n");
        printf("  █                                     █\n");
        printf("  █     ALL TESTS PASSED! ✓ ✓ ✓        █\n");
        printf("  █                                     █\n");
        printf("  ███████████████████████████████████████\n");
        printf("\n");
        printf("  The AES-128 implementation is CORRECT!\n");
        printf("\n");
        return 0;
    } else {
        printf("  ███████████████████████████████████████\n");
        printf("  █                                     █\n");
        printf("  █     SOME TESTS FAILED! ✗            █\n");
        printf("  █                                     █\n");
        printf("  ███████████████████████████████████████\n");
        printf("\n");
        printf("  Please check the implementation!\n");
        printf("\n");
        return 1;
    }
}