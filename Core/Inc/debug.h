
#ifndef _DEBUG_H_
#define _DEBUG_H_

# ifdef DEBUG

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"

#define debug_printf(...)   SEGGER_RTT_printf(0, __VA_ARGS__)

//extern uint8_t dbgflag;

# else

#define debug_printf(...)

# endif

#endif /* _DEBUG_H_ */