#ifndef _TRACE_H_
#define _TRACE_H_

void trace_init(void);
void trace_record_error(uint8_t mod, uint8_t func, uint8_t pos, uint8_t error);

#endif
