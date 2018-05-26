#ifndef _RANDOM_NUMBER_H__
#define _RANDOM_NUMBER_H__

#include "standard/Platform/Common.h"

#include <string>
#include <vector>
#include <sstream>

/* Return a random number in the range min..max; (max-min) must be smaller than 32768. */
TC_CFRAMEWORK_API int32 irand(int32 min, int32 max);

/* Return a random number in the range min..max (inclusive). For reliable results, the difference
* between max and min should be less than RAND32_MAX. */
TC_CFRAMEWORK_API uint32 urand(uint32 min, uint32 max);

/* Return a random number in the range min..max (inclusive). */
TC_CFRAMEWORK_API float frand(float min, float max);

/* Return a random number in the range 0 .. RAND32_MAX. */
TC_CFRAMEWORK_API int32 rand32();

/* Return a random double from 0.0 to 1.0 (exclusive). Floats support only 7 valid decimal digits.
 * A double supports up to 15 valid decimal digits and is used internally (RAND32_MAX has 10 digits).
 * With an FPU, there is usually no difference in performance between float and double. */
TC_CFRAMEWORK_API double rand_norm(void);

TC_CFRAMEWORK_API float rand_norm_f(void);

/* Return a random double from 0.0 to 99.9999999999999. Floats support only 7 valid decimal digits.
 * A double supports up to 15 valid decimal digits and is used internaly (RAND32_MAX has 10 digits).
 * With an FPU, there is usually no difference in performance between float and double. */
TC_CFRAMEWORK_API double rand_chance(void);

TC_CFRAMEWORK_API float rand_chance_f(void);

/* Return true if a random roll fits in the specified chance (range 0-100). */
inline bool roll_chance_f(float chance)
{
    return chance >= 100.f || chance > rand_chance();
}

/* Return true if a random roll fits in the specified chance (range 0-100). */
inline bool roll_chance_i(int chance)
{
    return chance >= 100 || chance > irand(0, 99);
}

#endif // _RANDOM_NUMBER_H__
