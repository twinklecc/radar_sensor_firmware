#ifndef CALTERAH_ERROR_H
#define CALTERAH_ERROR_H
#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_assert.h"
#endif

#define E_REFPLL_UNLOCK            (-128)
#define E_PLL_UNLOCK               (-129)
#define E_CLOCK_SWITCH_FAIL        (-130)


#define CALTERAH_CHECK(st) do{if (st != E_OK) EMBARC_HALT("error(%d)\n\r", st);}while(0)

#endif
