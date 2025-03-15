#include <chrono>
#include <iostream>
#include <cstdint>
#include <chrono>
#include <immintrin.h>
#include <avxintrin.h>

using namespace std::chrono;

#define DIM 128
#define NUM 10000

struct Descriptor {
    uint8_t features [DIM];
};

Descriptor X[NUM];
Descriptor Y[NUM];

// result vector
int R[NUM]; // 10000 elements of 0....99999
// R[x] = y <=> x and y descriptors are close
uint16_t distL1(Descriptor& x, Descriptor& y) {
    //uint16_t sum = 0;

    //for (int i = 0; i < DIM; i++) {
    //    sum = sum + abs(x.features[i] - y.features[i]);
    //}

    // AVX OPTIMIZATION
    __m256i r0 = _mm256_loadu_si256((__m256i*)&x.features[0]);
    __m256i r1 = _mm256_loadu_si256((__m256i*)&x.features[32]);
    __m256i r2 = _mm256_loadu_si256((__m256i*)&x.features[64]);
    __m256i r3 = _mm256_loadu_si256((__m256i*)&x.features[96]);

    __m256i r4 = _mm256_loadu_si256((__m256i*)&y.features[0]);
    __m256i r5 = _mm256_loadu_si256((__m256i*)&y.features[32]);
    __m256i r6 = _mm256_loadu_si256((__m256i*)&y.features[64]);
    __m256i r7 = _mm256_loadu_si256((__m256i*)&y.features[96]);

    // do the difference
    __m256i r8  = _mm256_sad_epu8(r0, r4); // dest is in bits 63:0
    __m256i r9  = _mm256_sad_epu8(r1, r5); //
    __m256i r10 = _mm256_sad_epu8(r2, r6);
    __m256i r11 = _mm256_sad_epu8(r3, r7);

    // sum the diff up
    __m256i r12 = _mm256_add_epi16(r8, r9);
    __m256i r13 = _mm256_add_epi16(r10, r11);
    __m256i r14 = _mm256_add_epi16(r12, r13);

    __m256i r15 = _mm256_hadd_epi16(r14, r14);

    r14 = _mm256_hadd_epi16(r15, r15); // result is in bits 15:0

    return (int)_mm256_extract_epi16(r14, 0);
}

int main() {

    // initialize
    for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < NUM; i++) {
            X[i].features[j] = rand();
            Y[i].features[j] = rand();
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    // match x and y
    for (int i = 0; i < NUM; i++) {
        uint16_t currentDistance = (uint16_t)-1;

        for (int j = 0; j < NUM; j++) {

            // 256*128  = 2**8 * 2**7  => 2 ** 15
            uint16_t dist = distL1(X[i], Y[j]);

            // if the distance is lower, put it in current
            // save the closest descriptor to i to be j
            if (dist < currentDistance) {
                currentDistance = dist;
                R[i] = j;
            }
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();

    std::cout << "Done in " << duration_cast<std::chrono::milliseconds>(stop-start).count() << " ms" << std::endl;

    return 0;
}