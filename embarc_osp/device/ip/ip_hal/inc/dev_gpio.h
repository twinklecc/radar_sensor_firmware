/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

/**
 * \defgroup	DEVICE_HAL_GPIO		GPIO Device HAL Interface
 * \ingroup	DEVICE_HAL_DEF
 * \brief	definitions for gpio device hardware layer (\ref dev_gpio.h)
 * \details	provide interfaces for gpio driver to implement
 *  Here is a diagram for the gpio interface.
 *
 *  \htmlonly
 *  <div class="imagebox">
 *      <div style="width: 600px">
 *          <img src="pic/dev_gpio_hal.jpg" alt="GPIO Device HAL Interface Diagram"/>
 *          <p>GPIO Device HAL Interface Diagram</p>
 *      </div>
 *  </div>
 *  \endhtmlonly
 *
 * @{
 *
 * \file
 * \brief	gpio device hardware layer definitions
 * \details	Provide common definitions for gpio device,
 * 	then the software developer can develop gpio driver
 * 	following these definitions, and the applications
 * 	can directly call this definition to realize functions
 *
 */

#ifndef _DEVICE_HAL_GPIO_H_
#define _DEVICE_HAL_GPIO_H_

#include "dev_common.h"

/**
 * \defgroup	DEVICE_HAL_GPIO_DEFDIR	GPIO Port Direction Definition
 * \ingroup	DEVICE_HAL_GPIO
 * \brief	Define macros to indicate gpio directions
 * @{
 */
/*
 * defines for gpio directions
 */
#define GPIO_DIR_INPUT				(0)		/*!< gpio works as input */
#define GPIO_DIR_OUTPUT				(1)		/*!< gpio works as output */
/** @} */


/**
 * \defgroup	DEVICE_HAL_GPIO_CTRLCMD		GPIO Device Control Commands
 * \ingroup	DEVICE_HAL_GPIO
 * \brief	Definitions for gpio control command, used in \ref dev_gpio::gpio_control "GPIO IO Control"
 * \details	These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref E_OK,	Control device successfully
 *   - \ref E_CLSED,	Device is not opened
 *   - \ref E_OBJ,	Device object is not valid or not exists
 *   - \ref E_PAR,	Parameter is not valid for current control command
 *   - \ref E_SYS,	Control device failed, due to hardware issues such as device is disabled
 *   - \ref E_NOSPT,	Control command is not supported or not valid
 * @{
 */
/**
 * Set the \ref dev_gpio_info::direction "direction" of masked bits of gpio port into \ref GPIO_DIR_INPUT "input"
 * - Param type : uint32_t
 * - Param usage : 1 in each bit will be masked.
 * - Return value explanation :
 */
#define GPIO_CMD_SET_BIT_DIR_INPUT			DEV_SET_SYSCMD(0)
/**
 * Set the \ref dev_gpio_info::direction "direction" of masked bits of gpio port into \ref GPIO_DIR_OUTPUT "output"
 * - Param type : uint32_t
 * - Param usage : 1 in each bit will be masked.
 * - Return value explanation :
 */
#define GPIO_CMD_SET_BIT_DIR_OUTPUT			DEV_SET_SYSCMD(1)
/**
 * Get \ref dev_gpio_info::direction "gpio port direction".
 * - Param type : uint32_t
 * - Param usage : 1 bit for 1 bit of gpio port, 0 for input, 1 for output
 * - Return value explanation :
 */
#define GPIO_CMD_GET_BIT_DIR				DEV_SET_SYSCMD(2)
/**
 * Set gpio interrupt configuration for each bit.
 * - Param type : \ref DEV_GPIO_INT_CFG *
 * - Param usage : store gpio interrupt configuration for each bit.
 * - Return value explanation :
 */
#define GPIO_CMD_SET_BIT_INT_CFG			DEV_SET_SYSCMD(3)
/**
 * Get gpio interrupt configuration for each bit.
 * - Param type : \ref DEV_GPIO_INT_CFG *
 * - Param usage : First set int_bit_mask in DEV_GPIO_INT_CFG structure to
 * the mask of which bit of GPIO you want to get. And the interrupt configuration
 * will be stored in the structure DEV_GPIO_INT_CFG, each bit stand for each bit of port.
 * - Return value explanation :
 */
#define GPIO_CMD_GET_BIT_INT_CFG			DEV_SET_SYSCMD(4)
/**
 * Set gpio service routine for each bit.
 * - Param type : \ref DEV_GPIO_BIT_ISR *
 * - Param usage : store gpio handler information for each bit, int handler's param will be DEV_GPIO *.
 * - Return value explanation :
 */
#define GPIO_CMD_SET_BIT_ISR				DEV_SET_SYSCMD(5)
/**
 * Get gpio service routine for each bit.
 * - Param type : \ref DEV_GPIO_BIT_ISR *
 * - Param usage : By passing int_bit_ofs in DEV_GPIO_BIT_ISR,
 * it will return interrupt handler for this bit and store it in int_bit_handler.
 * - Return value explanation :
 */
#define GPIO_CMD_GET_BIT_ISR				DEV_SET_SYSCMD(6)
/**
 * Enable gpio interrupt of the masked bits.
 * - Param type : uint32_t
 * - Param usage : 1 in each bit will be masked.
 * - Return value explanation :
 */
#define GPIO_CMD_ENA_BIT_INT				DEV_SET_SYSCMD(7)
/**
 * Disable gpio interrupt of the masked bits.
 * - Param type : uint32_t
 * - Param usage : 1 in each bit will be masked.
 * - Return value explanation :
 */
#define GPIO_CMD_DIS_BIT_INT				DEV_SET_SYSCMD(8)
/**
 * Get \ref dev_gpio_info::method "gpio interrupt enable status".
 * - Param type : uint32_t *
 * - Param usage : 1 bit for 1 bit of gpio port, 0 for poll, 1 for interrupt
 * - Return value explanation :
 */
#define GPIO_CMD_GET_BIT_MTHD				DEV_SET_SYSCMD(9)
/**
 * Toggle GPIO output of the masked bits(pins).
 * - Param type : uint32_t
 * - Param usage : 1 in each bit will be masked.
 * - Return value explanation :
 */
#define GPIO_CMD_TOGGLE_BITS				DEV_SET_SYSCMD(10)
/* @} */


#define GPIO_CMD_SET_SOURCE_SELECTOR				DEV_SET_SYSCMD(32)
#define GPIO_CMD_GET_SOURCE_SELECTOR				DEV_SET_SYSCMD(33)

typedef struct dev_gpio_info {
	void *gpio_ctrl;
	uint32_t opn_flag;	/*!< gpio open flag, open it will set 1, close it will set 0. */
	uint32_t direction;
	void * extra;
} DEV_GPIO_INFO, * DEV_GPIO_INFO_PTR;


typedef struct dev_general_gpio {
	DEV_GPIO_INFO gpio_info;
	int32_t (*gpio_open) (uint32_t gpio_no, uint32_t dir);	/*!< open gpio with pre-defined gpio direction */
	int32_t (*gpio_close) (uint32_t gpio_no);		/*!< close gpio device */
	int32_t (*gpio_control) (uint32_t ctrl_cmd, uint32_t gpio_no, void *param);	/*!< control gpio device */
	int32_t (*gpio_write) (uint32_t gpio_no, uint32_t val);	/*!< write gpio device with val. */
	int32_t (*gpio_read) (uint32_t gpio_no, uint32_t *val);	/*!< read gpio device val. */
} DEV_GPIO, * DEV_GPIO_PTR;



#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief	get an \ref dev_gpio "gpio device" by gpio device id.
 * 	For how to use gpio device hal refer to \ref dev_gpio "Functions in gpio device structure"
 * \param[in]	gpio_id	id of gpio, defined by user
 * \retval	!NULL	pointer to an \ref dev_gpio "gpio device structure"
 * \retval	NULL	failed to find the gpio device by \ref gpio_id
 * \note	need to implemented by user in user code
 */
extern DEV_GPIO_PTR gpio_get_dev(int32_t gpio_id);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _DEVICE_HAL_GPIO_H_ */
