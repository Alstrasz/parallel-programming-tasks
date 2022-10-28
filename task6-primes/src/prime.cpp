#include "prime.h"

bool is_prime ( int32_t val ) {
    if (val == 2 || val == 3 || val == 5 || val == 7) {
        return true;
    }
    if ( val <= 7 ) {
        return false;
    }

    for (int32_t i = 3; i * i <= val; i += 2) {
        if (val % i == 0) {
            return false;
        }
    }

    return true;
}