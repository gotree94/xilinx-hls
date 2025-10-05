// aes128_hls.cpp
// Complete AES-128 Encryption Implementation for Vitis HLS
// Target: Xilinx FPGA (Basys3 - xc7a35tcpg236-1)

#include "ap_int.h"

typedef ap_uint<8> uint8;
typedef ap_uint<32> uint32;

// ========================================================================
// AES S-box - Complete 256 bytes lookup table
// ========================================================================
const uint8 sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// ========================================================================
// Rcon values for key expansion
// ========================================================================
const uint8 Rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

// ========================================================================
// Galois Field multiplication by 2 in GF(2^8)
// ========================================================================
uint8 gmul2(uint8 a) {
    #pragma HLS INLINE
    uint8 result;
    if (a & 0x80) {
        result = (a << 1) ^ 0x1b;
    } else {
        result = a << 1;
    }
    return result;
}

// ========================================================================
// Galois Field multiplication by 3 in GF(2^8)
// ========================================================================
uint8 gmul3(uint8 a) {
    #pragma HLS INLINE
    return gmul2(a) ^ a;
}

// ========================================================================
// SubBytes transformation - Apply S-box to each byte
// ========================================================================
void SubBytes(uint8 state[16]) {
    #pragma HLS INLINE off
    
    SUB_LOOP: for (int i = 0; i < 16; i++) {
        #pragma HLS UNROLL
        state[i] = sbox[state[i]];
    }
}

// ========================================================================
// ShiftRows transformation - Cyclically shift rows
// ========================================================================
void ShiftRows(uint8 state[16]) {
    #pragma HLS INLINE off
    
    uint8 temp;
    
    // Row 1: shift left by 1
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;
    
    // Row 2: shift left by 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;
    
    // Row 3: shift left by 3 (or right by 1)
    temp = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = state[3];
    state[3] = temp;
}

// ========================================================================
// MixColumns transformation - Mix columns of the state
// ========================================================================
void MixColumns(uint8 state[16]) {
    #pragma HLS INLINE off
    
    uint8 temp[16];
    #pragma HLS ARRAY_PARTITION variable=temp complete
    
    MIX_LOOP: for (int i = 0; i < 4; i++) {
        #pragma HLS UNROLL
        
        int col = i * 4;
        uint8 s0 = state[col];
        uint8 s1 = state[col + 1];
        uint8 s2 = state[col + 2];
        uint8 s3 = state[col + 3];
        
        temp[col]     = gmul2(s0) ^ gmul3(s1) ^ s2 ^ s3;
        temp[col + 1] = s0 ^ gmul2(s1) ^ gmul3(s2) ^ s3;
        temp[col + 2] = s0 ^ s1 ^ gmul2(s2) ^ gmul3(s3);
        temp[col + 3] = gmul3(s0) ^ s1 ^ s2 ^ gmul2(s3);
    }
    
    COPY_LOOP: for (int i = 0; i < 16; i++) {
        #pragma HLS UNROLL
        state[i] = temp[i];
    }
}

// ========================================================================
// AddRoundKey transformation - XOR state with round key
// ========================================================================
void AddRoundKey(uint8 state[16], uint8 roundKey[16]) {
    #pragma HLS INLINE off
    
    ADD_LOOP: for (int i = 0; i < 16; i++) {
        #pragma HLS UNROLL
        state[i] = state[i] ^ roundKey[i];
    }
}

// ========================================================================
// Key Expansion - Generate 11 round keys from original key
// ========================================================================
void KeyExpansion(uint8 key[16], uint8 roundKeys[176]) {
    #pragma HLS INLINE off
    
    // Copy original key to first round key
    INIT_KEY: for (int i = 0; i < 16; i++) {
        #pragma HLS UNROLL
        roundKeys[i] = key[i];
    }
    
    // Generate remaining round keys
    EXPAND: for (int i = 4; i < 44; i++) {
        uint8 temp[4];
        #pragma HLS ARRAY_PARTITION variable=temp complete
        
        // Copy previous word
        temp[0] = roundKeys[(i - 1) * 4 + 0];
        temp[1] = roundKeys[(i - 1) * 4 + 1];
        temp[2] = roundKeys[(i - 1) * 4 + 2];
        temp[3] = roundKeys[(i - 1) * 4 + 3];
        
        if (i % 4 == 0) {
            // RotWord - rotate bytes
            uint8 k = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = k;
            
            // SubWord - apply S-box
            temp[0] = sbox[temp[0]];
            temp[1] = sbox[temp[1]];
            temp[2] = sbox[temp[2]];
            temp[3] = sbox[temp[3]];
            
            // Add round constant
            temp[0] = temp[0] ^ Rcon[i / 4];
        }
        
        // XOR with word 4 positions back
        roundKeys[i * 4 + 0] = roundKeys[(i - 4) * 4 + 0] ^ temp[0];
        roundKeys[i * 4 + 1] = roundKeys[(i - 4) * 4 + 1] ^ temp[1];
        roundKeys[i * 4 + 2] = roundKeys[(i - 4) * 4 + 2] ^ temp[2];
        roundKeys[i * 4 + 3] = roundKeys[(i - 4) * 4 + 3] ^ temp[3];
    }
}

// ========================================================================
// Top-level AES-128 Encryption Function
// ========================================================================
void aes128_encrypt(uint8 plaintext[16], uint8 key[16], uint8 ciphertext[16]) {
    // HLS Interface directives
    #pragma HLS INTERFACE mode=s_axilite port=plaintext
    #pragma HLS INTERFACE mode=s_axilite port=key
    #pragma HLS INTERFACE mode=s_axilite port=ciphertext
    #pragma HLS INTERFACE mode=s_axilite port=return
    
    // Local arrays
    uint8 state[16];
    uint8 roundKeys[176];  // 11 round keys * 16 bytes each
    
    #pragma HLS ARRAY_PARTITION variable=state complete
    #pragma HLS ARRAY_PARTITION variable=roundKeys cyclic factor=16
    
    // Initialize state with plaintext
    INIT_STATE: for (int i = 0; i < 16; i++) {
        #pragma HLS UNROLL
        state[i] = plaintext[i];
    }
    
    // Expand key into round keys
    KeyExpansion(key, roundKeys);
    
    // Initial round - just AddRoundKey
    AddRoundKey(state, &roundKeys[0]);
    
    // Main rounds (1 to 9)
    MAIN_ROUNDS: for (int round = 1; round < 10; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, &roundKeys[round * 16]);
    }
    
    // Final round (no MixColumns)
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, &roundKeys[10 * 16]);
    
    // Copy state to output
    OUTPUT: for (int i = 0; i < 16; i++) {
        #pragma HLS UNROLL
        ciphertext[i] = state[i];
    }
}