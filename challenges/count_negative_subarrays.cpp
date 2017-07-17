/* NOTE: https://www.hackerrank.com/challenges/java-negative-subarray
 * Given an array of  integers, find and print its number of negative subarrays on a new line.
 * Naive approach of looping over all possible arrays, fast approach uses a tree structure to calculate the sums recursively.
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <intrin.h>

// NOTE: Actual data needed will be quadratic in list length for the fast method => 2^16 = 2^34 bytes!!
#define LIST_SIZE (1LLU << 16)
#define ArrayCount(x) (sizeof(x) / sizeof(*(x)))

#define SLOW 1
#define FAST 1

int main(int argc, char *argv[]) {
    srand((unsigned int)time(0));
    unsigned long long data_size = ((LIST_SIZE * (LIST_SIZE + 1LLU)) / 2LLU) * sizeof(int);
    int *src = (int*)malloc(data_size);
    printf("Count the number of all negative-sum subarrays contained in an int-list of length n.\n");
    printf("Slow count uses a naive approach, fast count uses a clever tree structure.\n");
    printf("------------------------------------------------------------------------------------\n\n");
#if SLOW
    char *out_slow = "slow.dat";
    FILE *fslow = fopen(out_slow, "w");
#endif
#if FAST
    char *out_fast = "fast.dat";
    FILE *ffast = fopen(out_fast, "w");
#endif
    for (int len = 1; len < LIST_SIZE; len <<= 1) {
        for (int i = 0; i < len; i++) {
            src[i] = (rand() % 2 ? -1 : 1) * (int)(((float)rand() / (float)(RAND_MAX)) * 10000.f);
        }
#if SLOW
        {
            int count = 0;
            unsigned long long start = __rdtsc();
            for (int sub_sz = 1; sub_sz <= len; sub_sz++) {
                for (int offset = 0; offset < len - sub_sz + 1; offset++) {
                    int sum = 0;
                    for (int i = 0; i < sub_sz; i++) {
                        sum += src[offset + i];
                    }
                    if (sum < 0) {
                        count++;
                    }
                }
            }
            unsigned long long end = __rdtsc();
            printf("Slow count in %llu kcycles for n = %d items (kcycles / n = : %llu) = %d\n", (end - start) / 1000UL, len, (end - start) / len / 1000UL, count);
            fprintf(fslow, "%d %llu\n", len, (end - start) / len / 1000UL);
        }
#endif
#if FAST
        {
            int count = 0;
            unsigned long long start = __rdtsc();

            // NOTE: First and second levels are special, so treat seperately
            for (size_t i = 0; i < len; i++) {
                if (src[i] < 0)
                    count++;
            }
            for (size_t i = 0; i < len - 1; i++) {
                int sum = src[i] + src[i + 1];
                src[len + i] = sum;
                if (sum < 0)
                    count++;
            }

            size_t read_offset2 = 1;
            size_t read_offset1 = read_offset2 + len - 1;
            size_t write_offset = read_offset1 + len - 1;
            for (int depth = 2; depth < len; depth++) {
                for (int i = 0; i < len - depth; i++) {
                    // TODO(OPTIMIZE): cache locality okay? we look at consecutive items so we should be fine (3 parallel cache lines)
                    // NOTE: To find the sum for the next level, add the two items one level up and correct for the sum of the item two levels up to remove double-countings
                    int sum = src[read_offset1 + i] + src[read_offset1 + i + 1] - src[read_offset2 + i];
                    src[write_offset + i] = sum;
                    if (sum < 0)
                        count++;
                }
                read_offset2 += (len - depth + 2);
                read_offset1 = read_offset2 + (len - depth);
                write_offset = read_offset1 + (len - depth);
            }
            unsigned long long end = __rdtsc();
            printf("Fast count in %llu kcycles for n = %d items (kcycles / n = : %llu) = %d\n", (end - start) / 1000UL, len, (end - start) / len / 1000UL, count);
            fprintf(ffast, "%d %llu\n", len, (end - start) / len / 1000UL);
        }
        printf("----------------------------------------------------------------\n");
#endif
    }

#ifdef _WIN32
    system("pause");
#endif

    return 0;
}