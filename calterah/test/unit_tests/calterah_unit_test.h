#ifndef CALTERAH_UNIT_TEST_H
#define CALTERAH_UNIT_TEST_H
#include <stdint.h>	/* C99 standard lib */
#include <limits.h>	/* C99 standard lib */
#include <stddef.h>	/* C99 standard lib */
#include <stdbool.h> 	/* C99 standard lib */

/* From embarc_error.h */
#define E_OK		(0)		/*!< ok */
#define E_SYS		(-5)		/*!< system error */
#define E_NOSPT		(-9)		/*!< unsupported features */
#define E_RSFN		(-10)		/*!< reserved function code */
#define E_RSATR		(-11)		/*!< reserved attribute */
#define E_PAR		(-17)		/*!< parameter error */
#define E_ID		(-18)		/*!< invalid ID number */
#define E_CTX		(-25)		/*!< context error */
#define E_MACV		(-26)		/*!< memory access violation */
#define E_OACV		(-27)		/*!< object access violation */
#define E_ILUSE		(-28)		/*!< illegal service call use */
#define E_NOMEM		(-33)		/*!< insufficient memory */
#define E_NOID		(-34)		/*!< no ID number available */
#define E_NORES		(-35)		/*!< no resource available */
#define E_OBJ		(-41)		/*!< object state error */
#define E_NOEXS		(-42)		/*!< non-existent object */
#define E_QOVR		(-43)		/*!< queue overflow */
#define E_RLWAI		(-49)		/*!< forced release from waiting */
#define E_TMOUT		(-50)		/*!< polling failure or timeout */
#define E_DLT		(-51)		/*!< waiting object deleted */
#define E_CLS		(-52)		/*!< waiting object state changed */
#define E_WBLK		(-57)		/*!< non-blocking accepted */
#define E_BOVR		(-58)		/*!< buffer overflow */
#define E_OPNED		(-6)		/*!< device is opened */
#define E_CLSED		(-7)		/*!< device is closed */

#define EMBARC_PRINTF printf

#endif
