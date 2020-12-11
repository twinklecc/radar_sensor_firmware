#ifndef _SW_TRACE_H_
#define _SW_TRACE_H_

#ifdef USE_SW_TRACE
void trace_info(uint32_t value);
#else
static inline void trace_info(uint32_t value)
{
}
#endif

#endif
