#include <iostream>
#include <chrono> // used for time measurement
#include <cstdlib>
#include <bitset>
#include <omp.h>

using namespace std::chrono;

#define ONE_MILLION (100000000)

// Done in 530 ms naive with OpenMP 12 Threads
// Done in 34 ms LUT 8 bit with OpenMP 12 Threads
// Done in 227 ms LUT 8 bit
// Done in 149 ms LUT 16 bit
// i7 12800H @ 2.4 Ghz (20 Threads)

void naiveInvertBits(uint32_t* pData, uint32_t dataSize) {

    #pragma omp parallel for
    for (uint32_t i = 0; i < dataSize; i++) {
        uint32_t invertedBits = 0ULL;
        uint32_t number   = *(pData + i);
        for (uint32_t j = 0; j < 32; j += 2) {
            // b31 b30 b29 ... b2 b1 b0
            // b0 b2 b4 ... b30 b1 b3 ... b31
            uint32_t evenBit = (uint32_t)((number & (1 << j)) >> j << (31 - j/2));
            invertedBits |= evenBit;

            uint32_t oddBit = (uint32_t)((number & (1 << (j + 1))) >> (j+1) << (15 - j/2));
            invertedBits |= oddBit;
        }
        *(pData + i) = invertedBits;
    }
}

void optimizedInvertBits(uint32_t* pData, uint32_t dataSize) {
    for (uint32_t i = 0; i < dataSize; i++) {
        uint32_t invertedBits = 0ULL;
        uint32_t number = pData[i];
        {
            // b31 b30 b29 ... b2 b1 b0
            // b0 b2 b4 ... b30 b1 b3 ... b31
            uint32_t evenBit = (uint32_t)(number & (1 << 0) << 31); // 31
            invertedBits |= evenBit;

            uint32_t oddBit = (uint32_t)((number & (1 << 1)) << 14);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 2)) << 28); // 30
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 3)) << 11);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 4)) << 25); // 29
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 5)) << 8);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 6)) << 23); // 28
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 7)) << 5);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 8)) << 19); // 27
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 9)) << 3);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 10)) << 16); // 26
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 11)));
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 12)) << 13); // 25
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 13)) >> 3);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 14)) << 10); // 24
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 15)) >> 6);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 16)) << 7); // 23
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 17)) >> 9);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 18)) << 4); // 22
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 19)) >> 12);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 20)) << 1); // 21
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 21)) >> 15);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 22)) >> 2); // 20
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 23)) >> 18);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 24)) >> 5); // 19
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 25)) >> 21);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 26)) >> 8); // 18
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 27)) >> 24);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 28)) >> 11); // 17
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 29)) >> 27);
            invertedBits |= oddBit;

            evenBit = (uint32_t)((number & (1 << 30)) >> 14); // 16
            invertedBits |= evenBit;

            oddBit = (uint32_t)((number & (1 << 31)) >> 30);
            invertedBits |= oddBit;


        }
        pData[i] = invertedBits;
    }
}

/*
 * No calculation, just LUT accesses
 */
uint32_t lut8Bit[4][256];
uint32_t lut16Bit[2][65536];

void computeLut8Bit() {
    // Initialize lookup tables
    for ( int pos = 0; pos < 4; pos++) {
        const int global_pos = pos * 8;

        for (int val = 0; val < 256; val++) { // Iterate all possible values of a byte 2^8 = 256
            uint32_t result = 0;

            // Even bits
            for (int bit = 0; bit < 8; bit = bit + 2) { // for each bit in byte
                // result is initialized with 0s, so no need to account for 0 bits
                if (val & (1 << bit)) { // if bit is 1, determine if even or odd and place accordingly
                    const int global_bit = global_pos + bit;
                    result |= 1U << (31 - (global_bit >> 1)); // global_bit / 2
                    // resut += 1 << (31 - (i / 2))
                }
            }
            // Odd bits
            for (int bit = 1; bit < 8; bit = bit + 2) { // for each bit in byte
                // result is initialized with 0s, so no need to account for 0 bits
                if (val & (1 << bit)) { // if bit is 1, determine if even or odd and place accordingly
                    const int global_bit = global_pos + bit;
                    result |= 1U << (15 - (global_bit >> 1)); // global_bit / 2
                }
            }
            lut8Bit[pos][val] = result;
        }
    }

}

void computeLut16Bit() {
    // Initialize lookup tables
    for ( int pos = 0; pos < 2; pos++) {
        const int global_pos = pos * 16;

        for (int val = 0; val < 65536; val++) { // Iterate all possible values of a byte 2^8 = 256
            uint32_t result = 0;

            // Even bits
            for (int bit = 0; bit < 16; bit = bit + 2) { // for each bit in byte
                // result is initialized with 0s, so no need to account for 0 bits
                if (val & (1 << bit)) { // if bit is 1, determine if even or odd and place accordingly
                    const int global_bit = global_pos + bit;
                    result |= 1U << (31 - (global_bit >> 1)); // global_bit / 2
                    // resut += 1 << (31 - (i / 2))
                }
            }
            // Odd bits
            for (int bit = 1; bit < 16; bit = bit + 2) { // for each bit in byte
                // result is initialized with 0s, so no need to account for 0 bits
                if (val & (1 << bit)) { // if bit is 1, determine if even or odd and place accordingly
                    const int global_bit = global_pos + bit;
                    result |= 1U << (15 - (global_bit >> 1)); // global_bit / 2
                }
            }
            lut16Bit[pos][val] = result;
        }
    }

}

void lut8BitInvertBits(uint32_t* pData, uint32_t dataSize) {

    #pragma omp parallel for
    for (uint32_t i = 0; i < dataSize; i++) {
        pData[i] = (lut8Bit[0][pData[i] & 0xFF] | lut8Bit[1][(pData[i]>>8) & 0xFF] | lut8Bit[2][(pData[i]>>16) & 0xFF] | lut8Bit[3][pData[i]>>24]);
    }
}

void lut16BitInvertBits(uint32_t* pData, uint32_t dataSize) {
    for (uint32_t i = 0; i < dataSize; i++) {
        pData[i] = (lut16Bit[0][pData[i] & 0xFFFF] | lut16Bit[1][(pData[i]>>16)]);
    }
}

int main() {

    // data setup
    // allocate memory for all ONE_MILION numbers
    uint32_t* pData = (uint32_t*) malloc(sizeof(uint32_t) * ONE_MILLION);

    // inititalize the numbers with random values
    for (uint32_t i = 0; i < ONE_MILLION; i++) {
        *(pData + i) = rand();
    }

    *pData = 0xAAAAAAAA;
    uint32_t refNumber = *pData;

    computeLut8Bit();
    computeLut16Bit();

    auto start = std::chrono::high_resolution_clock::now();

    //naiveInvertBits(pData, ONE_MILLION);
    //optimizedInvertBits(pData, ONE_MILLION);

    lut8BitInvertBits(pData, ONE_MILLION);
    // lut16BitInvertBits(pData, ONE_MILLION);

    auto stop = std::chrono::high_resolution_clock::now();
    
    // verification
    std::bitset<32> refNumberBits(refNumber);
    std::bitset<32> invertNumberBits(*pData);
    std::cout << refNumberBits << "\n";
    std::cout << invertNumberBits << "\n";

    // result output
    std::cout << "Done in " << duration_cast<std::chrono::milliseconds>(stop-start).count() << " ms" << std::endl;
    return 0;
}
