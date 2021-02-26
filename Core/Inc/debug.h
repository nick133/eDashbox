
#ifndef _DEBUG_H_
#define _DEBUG_H_

# ifdef DEBUG
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#include <math.h>
#include "printf.h"

#define debug_printf(...)    SEGGER_RTT_printf(0, __VA_ARGS__)

/* Float to list of two integers - macro for SEGGER_RTT_printf()
 * Usage: ftoii(1.238, 3) --> 1, 238
 *        ftoii(1.238, 2) --> 1, 24
 *        debug_printf("%u.%u", fftoi(1.238, 2)); // prints '1.24'
 */
#define ftoii(f, n)    (int)f, (int)roundf((f - (int)f) * pow(10U, n))

/* For big floats
 * Usage: debug_fprintf("big float: %s\n", "%.4f", 800000000.1234567);
 */
#define debug_fprintf(fmt, ffmt, f, ...) \
    { char s[32]; snprintf_(s, 20, ffmt, f); SEGGER_RTT_printf(0, fmt, s, ##__VA_ARGS__); }

# else

#define debug_printf(...)
#define debug_fprintf(...)

# endif /* DEBUG */

#endif /* _DEBUG_H_ */