#ifndef __DEBUG_LOG_H__
#define __DEBUG_LOG_H__
#include "SEGGER_RTT.h"


#define __DEBUG_LOG__

#ifdef __DEBUG_LOG__
#define LOG(...)  SEGGER_RTT_printf(0,__VA_ARGS__)
#define LOG_HEX(buff, len)  SEGGER_RTT_printf_HEX(0, buff, len)
#else
#define LOG(...)  
#define LOG_HEX(...)  
#endif

#endif
