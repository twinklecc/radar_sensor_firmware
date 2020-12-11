#ifndef BASEBAND_REG_H
#define BASEBAND_REG_H

#ifdef CHIP_ALPS_A
#include "baseband_alps_a_reg.h"
#elif defined(CHIP_ALPS_B)
#include "baseband_alps_b_reg.h"
#elif defined(CHIP_ALPS_MP)
#include "baseband_alps_FM_reg.h"
#endif

#endif
