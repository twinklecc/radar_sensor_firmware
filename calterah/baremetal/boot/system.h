#ifndef _SYSTEM_H_
#define _SYSTEM_H_


#ifdef SYSTEM_UART_LOG_EN
#define LOG_MSG(file, func, pos, cause)	(\
		(((file) & 0xFF) << 24) | \
		(((func) & 0xFF) << 16) | \
		(((pos & 0xFF) << 8) | \
		(((cause) & 0xFF)) )

#define SYSTEM_LOG_START_FLAG	0x53474f4c //0x4C4F4753
#define SYSTEM_LOG_END_FLAG	0x45474f4c //0x4C4F4745

#define UART_FF_DEFAULT	\
{.data_bits = UART_CHAR_8BITS, .parity = UART_NO_PARITY, .stop_bits = UART_STOP_1BIT }
#endif

typedef void (*tick_callback)(void *);
typedef void (*next_image_entry)(void);

void gen_crc_table(void);
unsigned int update_crc(unsigned int crc_accum, \
		unsigned char *datap, unsigned int datak_size);

#ifdef SYSTEM_TICK
void system_tick_init(void);
uint64_t system_ticks_get(void);
void chip_hw_udelay(uint32_t us);
void chip_hw_mdelay(uint32_t ms);
void set_current_cpu_freq(uint32_t freq);
void system_tick_timer_callback_register(tick_callback func);
#else
static inline void system_tick_init(void) { }
static inline uint64_t system_ticks_get(void) { return 0; }
static inline void chip_hw_udelay(uint32_t us){ }
#ifdef BOOT_SPLIT
void chip_hw_mdelay(uint32_t ms);
#else
static inline void chip_hw_mdelay(uint32_t ms) { }
#endif
static inline void set_current_cpu_freq(uint32_t freq) { }
static inline void system_tick_timer_callback_register(tick_callback func) { }
#endif

void uart_print(uint32_t info);
void flash_xip_init_early(void);
int32_t normal_boot(void);
#ifdef BOOT_SPLIT
void system_ota_entry(void);
#endif

#endif
