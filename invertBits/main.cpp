#include <iostream>
#include <chrono> // used for time measurement
#include <cstdlib>
#include <bitset>

using namespace std::chrono;

#define ONE_MILLION (100000000)

// Done in 3801 ms
// i7 12800H @ 2.4 Ghz

void naiveInvertBits(uint32_t* pData, uint32_t dataSize) {
    for (uint32_t i = 0; i < dataSize; i++) {
        uint32_t invertedBits = 0ULL;
        uint32_t number   = *(pData + i);
        for (uint32_t j = 0; j < 32; j += 2) {
            // b31 b30 b29 ... b2 b1 b0
            // b0 b2 b4 ... b30 b1 b3 ... b31
            uint32_t evenBit = (uint32_t)((number & (1 << j)) >> j << (31 - j/2));
            invertedBits |= evenBit;

            uint32_t oddBit = (uint32_t)((number & (1 << (j))) >> (j) << (15 - j/2));
            invertedBits |= oddBit;
        }
        *(pData + i) = invertedBits;
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

    uint32_t refNumber = *pData;

    auto start = std::chrono::high_resolution_clock::now();

    naiveInvertBits(pData, ONE_MILLION);

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
