#define __ASSEMBLY__
#include <target_mem_config.h>

MEMORY
{
#if (ICCM_SIZE != 0)
	ICCM :		ORIGIN = ICCM_START,	LENGTH = ICCM_SIZE
#endif
#if (DCCM_SIZE !=0)
	DCCM :		ORIGIN = DCCM_START,	LENGTH = DCCM_SIZE
#endif
#if (EXT_ROM_SIZE != 0)
	EXT_ROM :	ORIGIN = EXT_ROM_START,	LENGTH = EXT_ROM_SIZE
#endif
#if (EXT_TEXT_XIP_SIZE != 0)
    EXT_TEXT_XIP : ORIGIN = EXT_TEXT_XIP_START, LENGTH = EXT_TEXT_XIP_SIZE
#endif
#if (EXT_RAM_SIZE != 0)
	EXT_RAM :	ORIGIN = EXT_RAM_START,	LENGTH = EXT_RAM_SIZE
#endif
}

ENTRY(_start)

SECTIONS
{
	.init :
	{
		_f_init = .;
		KEEP (*(.init_vector))
		KEEP (*(.init_bootstrap))
		_e_init = .;
	} > REGION_ROM

	.vector : ALIGN(1024)
	{
		_f_vector = .;
		*(.vector)
		_e_vector = .;
	} > REGION_ROM

#if (EXT_TEXT_XIP_SIZE != 0)
	.text_xip : ALIGN(4)
	{
		_f_text_xip = .;
		*flash.o (.text .text.* .gnu.linkonce.t.*)
		*dw_ssi_obj.o (.text .text.* .gnu.linkonce.t.*)
		*clkgen.o (.text .text.* .gnu.linkonce.t.*)
		*dw_ssi.o (.text .text.* .gnu.linkonce.t.*)
		*s25fls.o (.text .text.* .gnu.linkonce.t.*)
		*alps_timer.o (.text .text.* .gnu.linkonce.t.*)
		*arc_timer.o (.text .text.* .gnu.linkonce.t.*)
		*xip_hal.o (.text .text.* .gnu.linkonce.t.*)
		*arc_exception.o (.text .text.* .gnu.linkonce.t.*)
		_e_text_xip = .;
	} > EXT_TEXT_XIP AT > REGION_ROM
#endif

	.text : ALIGN(4)
	{
		_f_text = .;
		*(.text .text.* .gnu.linkonce.t.*)
		_e_text = .;
	} > REGION_ROM

	.rodata : ALIGN(4)
	{
		_f_rodata = .;

		. = ALIGN(4);
		__CTOR_LIST__ = .;
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
		KEEP(*(SORT_BY_NAME(".ctors*")))
		LONG(0)
		__CTOR_END__ = .;

		. = ALIGN(4);
		__init_array_start = .;
		KEEP(*(SORT_BY_NAME(".init_array*")))
		__init_array_end = .;

		. = ALIGN(4);
		__DTOR_LIST__ = .;
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
		KEEP(*(SORT_BY_NAME(".dtors*")))
		LONG(0)
		__DTOR_END__ = .;

		*(.rodata .rodata.* .gnu.linkonce.r.*)
		_e_rodata = .;
	} > REGION_ROM

	.data : ALIGN(4)
	{
		_f_data = .;
		*(.data .data.* .gnu.linkonce.d.*)
		_f_sdata = .;
		__SDATA_BEGIN__ = .;
		*(.sdata .sdata.* .gnu.linkonce.s.*)
		_e_sdata = .;
		_e_data = .;
	} > REGION_RAM AT > REGION_ROM

	.bss (NOLOAD) : ALIGN(8)
	{
		PROVIDE (__sbss_start = .);
		PROVIDE (___sbss_start = .);
		_f_bss = .;
		_f_sbss = .;
		*(.dynsbss)
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
		_e_sbss = .;
		PROVIDE (__sbss_end = .);
		PROVIDE (___sbss_end = .);

		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		_e_bss = .;
	} > REGION_RAM

	.stack (NOLOAD) :
	{
		. = ALIGN(4);
		_f_stack = .;
		. = . + _STACKSIZE;
		_e_stack = .;
	} > REGION_RAM

	.heap (NOLOAD) :
	{
		. = ALIGN(4);
		__start_heap = . ;
		_f_heap = .;
		/* . = . + _HEAPSIZE; */
		_e_heap = .;
		__end_heap = . ;
		end = . ;
	} > REGION_RAM

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
#if (EXT_TEXT_XIP_SIZE != 0)
	_load_addr_text_xip = LOADADDR(.text_xip);
#endif
}
