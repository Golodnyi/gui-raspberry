#if defined(WIN32) || defined(WINDOWS)
#include "stdafx.h";
#endif

unsigned char xor_sum(unsigned char *buffer, unsigned int length) {
    unsigned char temp_sum = 0;

    while (length-- > 0) {
        temp_sum ^= *buffer++;
    }
    return (temp_sum);
}
