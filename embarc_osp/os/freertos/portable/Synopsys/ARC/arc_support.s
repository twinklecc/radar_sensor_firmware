/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/**
 * \file
 * \ingroup OS_FREERTOS
 * \brief  freertos support for arc processor
 *	like task dispatcher, interrupt handler
 */
/** @cond OS_FREERTOS_ASM_ARC_SUPPORT */

/*
 * core-dependent part in assemble language (for arc)
 */
#define __ASSEMBLY__
#include "arc.h"
#include "arc_asm_common.h"

/*
 *  task dispatcher
 *
 */
	.text
	.align 4
	.global dispatch
dispatch:
/*
 *  the pre-conditions of this routine are task context, CPU is
 *  locked, dispatch is enabled.
 */
	SAVE_NONSCRATCH_REGS		/* save callee save registers */
	mov	r1, dispatch_r
	PUSH	r1			/* save return address */
	ld	r0, [pxCurrentTCB]
	b	dispatcher

/* return routine when task dispatch happened in task context */
dispatch_r:
	RESTORE_NONSCRATCH_REGS		/* recover registers */
	j	[blink]

/*
 *  start dispatch
 */
	.global start_dispatch
	.align 4
start_dispatch:
/*
 *  this routine is called in the non-task context during the startup of the kernel
 *  , and all the interrupts are locked.
 *
 *  when the dispatcher is called, the cpu is locked, no nest exception (CPU exception/interrupt).
 *  In target_initialize, all interrupt priority mask should be cleared, cpu should be
 *  locked, the interrupts outside the kernel such as fiq can be
 *  enabled.
 */
	clri
	mov	r0, 0
	st	r0, [exc_nest_count]
	b	dispatcher_0
/*
 *  dispatcher
 */
dispatcher:
	ld	r1, [ulCriticalNesting]
	PUSH	r1			/* save critical nesting */
	st	sp, [r0]		/* save stack pointer of current task, r0->pxCurrentTCB */
	jl	vTaskSwitchContext	/* change the value of pxCurrentTCB */
/*
 *  before dispatcher is called, task context | cpu locked | dispatch enabled
 *  should be satisfied. In this routine, the processor will jump
 *  into the entry of next to run task
 *
 *  i.e. kernel mode, IRQ disabled, dispatch enabled
 */
dispatcher_0:
	ld	r1, [pxCurrentTCB]
	ld	sp, [r1]	/* recover task stack */
	POP	r0		/* get critical nesting */
	st	r0, [ulCriticalNesting]
	POP	r0		/* get return address  */
	j	[r0]

/*
 *  task startup routine
 *
 */
	.text
	.global start_r
	.align 4
start_r:
	seti	/* unlock cpu */
	mov	blink, vPortEndTask	/* set return address */
	POP	r1			/* get task function body */
	POP	r0			/* get task parameters */
	j	[r1]

/****** exceptions and interrupts handing ******/
/****** entry for exception handling ******/
	.global exc_entry_cpu
	.align 4
exc_entry_cpu:

	EXCEPTION_PROLOGUE

	mov	blink,	sp
	mov	r3, sp		/* as exception handler's para(p_excinfo) */

	ld	r0, [exc_nest_count]
	add	r1, r0, 1
	st	r1, [exc_nest_count]
	cmp	r0, 0
	bne	exc_handler_1
/* change to exception stack if interrupt happened in task context */
	mov	sp, _e_stack
exc_handler_1:
	PUSH	blink

	lr	r0, [AUX_ECR]
	lsr	r0, r0, 16
	mov	r1, exc_int_handler_table
	ld.as	r2, [r1, r0]

	mov	r0, r3
	jl	[r2]		/* !!!!jump to exception handler where interrupts are not allowed! */

/* interrupts are not allowed */
ret_exc:
	POP	sp
	mov	r1, exc_nest_count
	ld	r0, [r1]
	sub	r0, r0, 1
	cmp	r0, 0
	bne.d	ret_exc_1
	st	r0, [r1]

	ld	r0, [context_switch_reqflg]
	cmp	r0, 0
	bne	ret_exc_2
ret_exc_1:	/* return from non-task context, interrupts or exceptions are nested */

	EXCEPTION_EPILOGUE
	rtie

/* there is a dispatch request */
ret_exc_2:
	/* clear dispatch request */
	mov	r0, 0
	st	r0, [context_switch_reqflg]

	ld	r0, [pxCurrentTCB]
	cmp	r0, 0
	beq	ret_exc_r_1

	SAVE_CALLEE_REGS	/* save callee save registers */

	lr	r0, [AUX_STATUS32]
	bclr	r0, r0, AUX_STATUS_BIT_AE	/* clear exception bit */
	kflag	r0

	mov	r1, ret_exc_r	/* save return address */
	PUSH	r1

	b	dispatcher	/* r0->pxCurrentTCB */

ret_exc_r:
	/* recover exception status */
	lr	r0, [AUX_STATUS32]
	bset	r0, r0, AUX_STATUS_BIT_AE
	kflag	r0

	RESTORE_CALLEE_REGS	/* recover registers */
ret_exc_r_1:
	EXCEPTION_EPILOGUE
	rtie

/****** entry for normal interrupt exception handling ******/
	.global exc_entry_int	/* entry for interrupt handling */
	.align 4
exc_entry_int:
	clri	/* disable interrupt */

#if ARC_FEATURE_FIRQ == 1
#if ARC_FEATURE_RGF_NUM_BANKS > 1
	lr	r0, [AUX_IRQ_ACT]			/*  check whether it is P0 interrupt */
	btst	r0, 0
	jnz	exc_entry_firq
#else
	PUSH	r10
	lr	r10, [AUX_IRQ_ACT]
	btst	r10, 0
	POP	r10
	jnz	exc_entry_firq
#endif
#endif
	INTERRUPT_PROLOGUE

	mov	blink, sp

	ld	r3, [exc_nest_count]
	add	r2, r3, 1
	st	r2, [exc_nest_count]
	cmp	r3, 0
	bne	irq_handler_1
/* change to exception stack if interrupt happened in task context */
	mov	sp, _e_stack
irq_handler_1:
	PUSH	blink

	lr	r0, [AUX_IRQ_CAUSE]
	sr	r0, [AUX_IRQ_SELECT]
	mov	r1, exc_int_handler_table
	ld.as	r2, [r1, r0]	/* r2 = exc_int_handler_table + irqno *4 */
/* handle software triggered interrupt */
	lr	r3, [AUX_IRQ_HINT]
	cmp	r3, r0
	bne.d irq_hint_handled
	xor	r3, r3, r3
	sr	r3, [AUX_IRQ_HINT]
irq_hint_handled:
	lr	r3, [AUX_IRQ_PRIORITY]
	PUSH	r3		/* save irq priority */
	seti	/* enable higher priority interrupt */
	jl	[r2]		/* jump to interrupt handler */
/* no interrupts are allowed from here */
ret_int:
	clri	/* disable interrupt */
	POP	r3		/* irq priority */
	POP	sp
	mov	r1, exc_nest_count
	ld	r0, [r1]
	sub	r0, r0, 1
	cmp	r0, 0
	bne.d	ret_int_1
	st	r0, [r1]

	ld	r0, [context_switch_reqflg]
	cmp	r0, 0
	bne	ret_int_2
ret_int_1:	/* return from non-task context */
	INTERRUPT_EPILOGUE
	rtie
/* there is a dispatch request */
ret_int_2:
	/* clear dispatch request */
	mov	r0, 0
	st	r0, [context_switch_reqflg]

	ld	r0, [pxCurrentTCB]
	cmp	r0, 0
	beq	ret_int_r_1

	lr	r10, [AUX_IRQ_ACT]
	PUSH	r10
	bclr	r10, r10, r3	/* clear related bits in IRQ_ACT */
	sr	r10, [AUX_IRQ_ACT]

	SAVE_CALLEE_REGS	/* save callee save registers */
	mov	r1, ret_int_r	/* save return address */
	PUSH	r1

	b	dispatcher	/* r0->pxCurrentTCB */

ret_int_r:
	RESTORE_CALLEE_REGS	/* recover registers */
ret_int_r_1:
	/*
	 * whether to call the task exception routine
	 * as it is jumped from dispatch, TCB is already in r1
	 */
	/* recover AUX_IRQ_ACT */
	POPAX	AUX_IRQ_ACT
	INTERRUPT_EPILOGUE
	rtie
/** @endcond */
