#ifndef FUNC_SAFETY_H
#define FUNC_SAFETY_H

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_toolchain.h"
#endif

typedef struct func_safety {
        uint32_t dummy;
} func_safety_t;

void func_safety_init(func_safety_t *fsm);
void func_safety_start(func_safety_t *fsm);
void func_safety_stop(func_safety_t *fsm);

#endif
