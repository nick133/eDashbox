
#ifndef _DEBUG_H_
#define _DEBUG_H_

# ifdef DEBUG
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#include <math.h>

#define debug_printf(...)    SEGGER_RTT_printf(0, __VA_ARGS__)

/* Fractional part of float to integer macro for SEGGER_RTT_printf()
 * Usage: fftoi(1.238, 3) == 238
 *        fftoi(1.238, 2) == 24
 *        debug_printf("%u.%u", (int)1.238, fftoi(1.238, 2));
 */
#define fftoi(f, n)    (int)roundf((f - (int)f) * pow(10U, n))

# else

#define debug_printf(...)

# endif

#endif /* _DEBUG_H_ */