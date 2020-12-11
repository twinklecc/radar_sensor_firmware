#include <stdlib.h>
#include <string.h>

#include "embARC.h"
#include "embARC_debug.h"
#include "console.h"

#include "FreeRTOS.h"

#include "FreeRTOS_CLI.h"
#include "command.h"
#include "cascade.h"
#include "baseband_cas.h"

//#define REPORT_PORT_CAN		(1)

#define CLI_COMMAND_LEN_MAX	(cmdMAX_INPUT_SIZE)
#define CLI_COMMAND_ECHO_CNT	(8)

/* output 32 blanks to termimal.... */
#define CLI_LINE_CLEANUP_STR	\
	"                                                      "
#define CLI_KEY_UP(buf, index)	\
	(((buf)[index] == 0x1b) && ((buf)[index + 1] == 0x5b) && ((buf)[index + 2] == 0x41))
#define CLI_KEY_DOWN(buf, index) \
	(((buf)[index] == 0x1b) && ((buf)[index + 1] == 0x5b) && ((buf)[index + 2] == 0x42))
#define CLI_KEY_RIGHT(buf, index) \
	(((buf)[index] == 0x1b) && ((buf)[index + 1] == 0x5b) && ((buf)[index + 2] == 0x43))
#define CLI_KEY_LEFT(buf, index) \
	(((buf)[index] == 0x1b) && ((buf)[index + 1] == 0x5b) && ((buf)[index + 2] == 0x44))

#define CLI_CHAR_BS	(0x08) /* BACKSPACE */
#define CLI_CHAR_NL	(0xA) /* \n */
#define CLI_CHAR_CR	(0xD) /* \r */
#define CLI_CHAR_DEL	(0x7F) /* DELET */

#define CLI_DIR_1	(0x1b)
#define CLI_DIR_2	(0x5b)
#define CLI_DIR_UP	(0x41)
#define CLI_DIR_DOWN	(0x42)
#define CLI_DIR_RIGHT	(0x43)
#define CLI_DIR_LEFT	(0x44)

struct cli_echo_control {
	unsigned char cmd_order;
	uint32_t len;
	char *cmd_ptr;
};

static uint8_t cur_cmd_cnt = 0;
static uint8_t cur_cmd_order = 0;

struct cli_echo_control *cli_echo_ctl_ptr = NULL;
static uint32_t local_buffer_offset = 0;

static char local_buffer[cmdMAX_INPUT_SIZE];

static TaskHandle_t cli_task_handle = NULL;

static struct cli_echo_control *cli_last_cmd(void);
static struct cli_echo_control *cli_next_cmd(void);
static void cli_history_cmd_update(char *cmd_ptr, uint32_t len);
int32_t cli_insert_str(char *input_string, char *cli_dir, uint32_t cur_cursor, uint32_t total_size);


void freertos_cli_init(void)
{
    void *mem_addr = NULL;

    mem_addr = pvPortMalloc((sizeof(struct cli_echo_control) + CLI_COMMAND_LEN_MAX) * CLI_COMMAND_ECHO_CNT);
    if (!mem_addr) {
        EMBARC_PRINTF("[error]: lack of memory!!!\r\n");
    } else {
	int i = 0;
	struct cli_echo_control *echo_ctl_ptr = NULL;
        uint32_t buf_base = (((uint32_t)mem_addr) + CLI_COMMAND_ECHO_CNT * sizeof(struct cli_echo_control));

	cli_echo_ctl_ptr = (struct cli_echo_control *)mem_addr;
	echo_ctl_ptr = cli_echo_ctl_ptr;
	for (; i < CLI_COMMAND_ECHO_CNT; i++) {
	    echo_ctl_ptr->cmd_order = 0xFF;
	    echo_ctl_ptr->len = 0;
	    echo_ctl_ptr->cmd_ptr = (char *)(buf_base + (i * CLI_COMMAND_LEN_MAX));
	    echo_ctl_ptr++;
	}
    }

    if (pdPASS != xTaskCreate(command_interpreter_task, \
	"cli_task", 256, (void *)1, (configMAX_PRIORITIES - 1), &cli_task_handle)) {
	EMBARC_PRINTF("create cli task error\r\n");
    }
}

void command_interpreter_task(void *parameters)
{
	long cmd_result = 0;

	uint32_t char_count = 0;
	int32_t local_buffer_index = 0;
	int32_t input_string_index = 0;

	char *buf_ptr = NULL;

	char input_char;
	static char input_string[cmdMAX_INPUT_SIZE];
	static char output_string[cmdMAX_OUTPUT_SIZE];

	static struct cli_echo_control *last_cmd = NULL;
	static struct cli_echo_control *next_cmd = NULL;
	static int32_t cur_cursor = 0;

	uint32_t cli_dir_idx = 0;
	static char cli_dir[4];

	/* Just to prevent compiler warnings. */
	(void)parameters;

	for ( ; ; ) {
		/* wait a new line. */
		//char_count = console_io_wait_newline();
		buf_ptr = (char *)(((uint32_t)local_buffer) + local_buffer_offset);

#ifdef CHIP_CASCADE
                if (cascade_spi_cmd_wait()) { // spi cmd rx
                        uint32_t cmd_len;
                        char * pcCommandString = cascade_spi_cmd_info(&cmd_len);

                        if (cmd_len < (cmdMAX_INPUT_SIZE - local_buffer_offset))
                                memcpy(buf_ptr, pcCommandString, cmd_len);

                        char_count = cmd_len;

                } else { // uart cmd rx
                        char_count = console_wait_newline((uint8_t *)buf_ptr, \
                                     cmdMAX_INPUT_SIZE - local_buffer_offset);
                }
#else
                char_count = console_wait_newline((uint8_t *)buf_ptr, \
                             cmdMAX_INPUT_SIZE - local_buffer_offset);
#endif

		if (char_count > 0) {
			uint32_t data_uplimit;
			local_buffer_index = local_buffer_offset;
			data_uplimit = char_count + local_buffer_index;

			while(local_buffer_index < data_uplimit) {
				input_char = local_buffer[local_buffer_index];
				switch (input_char) {
					case CLI_CHAR_NL:
					case CLI_CHAR_CR:
						if (cli_dir_idx > 0) {
							cli_dir[cli_dir_idx] = '\0';
							cli_insert_str(input_string, cli_dir, cur_cursor, input_string_index);
							cur_cursor += cli_dir_idx;
							input_string_index += cli_dir_idx;
						}

						EMBARC_PRINTF("\r\n");
						if (input_string_index == 0) {
							local_buffer_index++;
							continue;
						}

						input_string[input_string_index++] = '\0';
						do {
							cmd_result = FreeRTOS_CLIProcessCommand(input_string, output_string, cmdMAX_OUTPUT_SIZE);
							if (0 != output_string[0]) {
								EMBARC_PRINTF("%s", output_string);
							}
							memset(output_string, 0x00, cmdMAX_OUTPUT_SIZE);
						} while (cmd_result != pdFALSE);

						/* update history command... */
						cli_history_cmd_update(input_string, input_string_index);
						cur_cmd_order = 0;
						local_buffer_offset = 0;

						/* All the strings generated by the command processing
						have been sent.  Clear the input string ready to receive
						the next command. */
						if (!strstr(input_string, "scan start")) {
							EMBARC_PRINTF("<EOF>\r\n");
						}

						cur_cursor = 0;
						input_string_index = 0;
						memset(input_string, 0x00, cmdMAX_INPUT_SIZE);

						cli_dir_idx = 0;
						memset(cli_dir, '\0', 4);
						break;

					case CLI_CHAR_DEL:
					case CLI_CHAR_BS:
						if (cli_dir_idx > 0) {
							cli_dir[cli_dir_idx] = '\0';
							cli_insert_str(input_string, cli_dir, cur_cursor, input_string_index);
							cur_cursor += cli_dir_idx;
							input_string_index += cli_dir_idx;
						}
						if (cur_cursor > 0) {
							if (cur_cursor < input_string_index) {
								int i = 0;
								char tmp_string[CLI_COMMAND_LEN_MAX];
								char tmp_string1[CLI_COMMAND_LEN_MAX];

								for (; i < cur_cursor - 1; ) {
									tmp_string1[i++] = ' ';
								}
								tmp_string1[i] = '\0';

								memcpy(tmp_string, input_string, cur_cursor - 1);
								tmp_string[cur_cursor - 1] = '\0';

								memcpy(&input_string[cur_cursor - 1], &input_string[cur_cursor], input_string_index - cur_cursor);
								input_string[input_string_index - 1] = '\0';
								EMBARC_PRINTF("\r%s\r%s%s\r%s", CLI_LINE_CLEANUP_STR, tmp_string1, &input_string[cur_cursor - 1], tmp_string);

								cur_cursor--;
							} else {
								EMBARC_PRINTF("%c", input_char);
								input_string[--cur_cursor] = '\0';
							}
							input_string_index--;
						}

						memset(cli_dir, '\0', 4);
						cli_dir_idx = 0;
						break;

					case CLI_DIR_1:
						cli_dir[cli_dir_idx++] = input_char;
						break;
					case CLI_DIR_2:
						if (0x1b == cli_dir[0]) {
							cli_dir[cli_dir_idx++] = input_char;
						} else {
							cli_dir[cli_dir_idx++] = input_char;
							cli_dir[cli_dir_idx++] = '\0';
							cli_insert_str(input_string, cli_dir, cur_cursor, input_string_index);
							cur_cursor++;
							input_string_index++;
							memset(cli_dir, '\0', 4);
							cli_dir_idx = 0;
						}
						break;
					case CLI_DIR_UP:
						if ((0x1b == cli_dir[0]) && (0x5b == cli_dir[1])) {
							last_cmd = cli_last_cmd();
							if (last_cmd) {
								EMBARC_PRINTF("\r%s\r%s", CLI_LINE_CLEANUP_STR, last_cmd->cmd_ptr);
								memcpy(input_string, last_cmd->cmd_ptr, last_cmd->len);
								input_string_index = last_cmd->len - 1;
							} else {
								EMBARC_PRINTF("\r%s\r", CLI_LINE_CLEANUP_STR);
								input_string_index = 0;
							}
							cur_cursor = input_string_index;
						} else {
							cli_dir[cli_dir_idx++] = input_char;
							cli_dir[cli_dir_idx++] = '\0';
							EMBARC_PRINTF("%s", cli_dir);
							cli_insert_str(input_string, cli_dir, cur_cursor, input_string_index);
							cur_cursor += cli_dir_idx - 1;
							input_string_index += cli_dir_idx - 1;
						}
						memset(cli_dir, '\0', 4);
						cli_dir_idx = 0;
						break;

					case CLI_DIR_DOWN:
						if ((0x1b == cli_dir[0]) && (0x5b == cli_dir[1])) {
							next_cmd = cli_next_cmd();
							if (next_cmd) {
								EMBARC_PRINTF("\r%s\r%s", CLI_LINE_CLEANUP_STR, next_cmd->cmd_ptr);
								memcpy(input_string, next_cmd->cmd_ptr, next_cmd->len);
								input_string_index = next_cmd->len - 1;
							} else {
								EMBARC_PRINTF("\r%s\r", CLI_LINE_CLEANUP_STR);
								input_string_index = 0;
							}
							cur_cursor = input_string_index;
						} else {
							cli_dir[cli_dir_idx++] = input_char;
							cli_dir[cli_dir_idx++] = '\0';
							EMBARC_PRINTF("%s", cli_dir);
							cli_insert_str(input_string, cli_dir, cur_cursor, input_string_index);
							cur_cursor += cli_dir_idx - 1;
							input_string_index += cli_dir_idx - 1;
						}
						memset(cli_dir, '\0', 4);
						cli_dir_idx = 0;
						break;

					case CLI_DIR_RIGHT:
					case CLI_DIR_LEFT:
						cli_dir[cli_dir_idx++] = input_char;
						cli_dir[cli_dir_idx++] = '\0';
						if ((0x1b == cli_dir[0]) && (0x5b == cli_dir[1])) {
							if ((CLI_DIR_RIGHT == input_char) && (cur_cursor < input_string_index)) {
								cur_cursor++;
								EMBARC_PRINTF("%s", cli_dir);
							}
							if ((CLI_DIR_LEFT == input_char) && (cur_cursor > 0)) {
								cur_cursor--;
								EMBARC_PRINTF("%s", cli_dir);
							}
						} else {
							EMBARC_PRINTF("%s", cli_dir);
							cli_insert_str(input_string, cli_dir, cur_cursor, input_string_index);
							cur_cursor += cli_dir_idx - 1;
							input_string_index += cli_dir_idx - 1;
						}
						memset(cli_dir, '\0', 4);
						cli_dir_idx = 0;
						break;

					default:
						if (cur_cursor >= input_string_index) {
							EMBARC_PRINTF("%c", input_char);
						} else {
							int i = 0;
							char tmp_string[CLI_COMMAND_LEN_MAX];
							char tmp_string1[CLI_COMMAND_LEN_MAX];

							for (; i < cur_cursor + 1; ) {
								tmp_string1[i++] = ' ';
							}
							tmp_string1[i] = '\0';

							memcpy(tmp_string, input_string, cur_cursor);
							tmp_string[cur_cursor] = input_char;
							tmp_string[cur_cursor + 1] = '\0';
							input_string[input_string_index] = '\0';
							EMBARC_PRINTF("\r%s\r%s%s\r%s", CLI_LINE_CLEANUP_STR, tmp_string1, &input_string[cur_cursor], tmp_string);
						}

						cli_dir[cli_dir_idx++] = input_char;
						cli_dir[cli_dir_idx++] = '\0';
						cli_insert_str(input_string, cli_dir, cur_cursor, input_string_index);
						cur_cursor += cli_dir_idx - 1;
						input_string_index += cli_dir_idx - 1;
						memset(cli_dir, '\0', 4);
						cli_dir_idx = 0;
						break;
				} /* switch */

				//local_buffer_offset = local_buffer_index;

				local_buffer_index++;
			}

		} else {
			/* back to wait a new line, and sleep again... */
			taskYIELD();
		}
	}
}

/*
 * get last command from history command buffer. in history command buffer,
 * command order range is {1, CLI_COMMAND_ECHO_CNT}, current command order
 * is 0.
 * */
static struct cli_echo_control *cli_last_cmd(void)
{
	struct cli_echo_control *r_ptr = NULL;

	int cmd_count, cnt = CLI_COMMAND_ECHO_CNT;
	struct cli_echo_control *echo_ctl_ptr = cli_echo_ctl_ptr;

	if (cur_cmd_cnt < CLI_COMMAND_ECHO_CNT) {
		cmd_count = cur_cmd_cnt;
	} else {
		cmd_count = cnt;
	}

	if (cur_cmd_order < cmd_count) {
		while (cnt--) {
			if (echo_ctl_ptr->cmd_order == cur_cmd_order + 1) {
				r_ptr = echo_ctl_ptr;
				break;
			}
			echo_ctl_ptr++;
		}
		cur_cmd_order++;
	} else {
		/* limit current order below or equal CLI_COMMAND_ECHO_CNT.
		 * so user can still find next command. */
		cur_cmd_order = cmd_count + 1;
	}

	return r_ptr;
}

static struct cli_echo_control *cli_next_cmd(void)
{
	struct cli_echo_control *r_ptr = NULL;

	int cnt = CLI_COMMAND_ECHO_CNT;
	struct cli_echo_control *echo_ctl_ptr = cli_echo_ctl_ptr;

	if (cur_cmd_order > 0) {
		while (cnt--) {
			if (echo_ctl_ptr->cmd_order == cur_cmd_order - 1) {
				r_ptr = echo_ctl_ptr;
				break;
			}
			echo_ctl_ptr++;
		}
		cur_cmd_order--;
	} else {
		/* user can still find last command. */
		//cur_cmd_order = 1;
	}

	return r_ptr;
}

static void cli_history_cmd_update(char *cmd_ptr, uint32_t len)
{
	int idx, insert_flag = 0;
	struct cli_echo_control *echo_ctl_ptr = cli_echo_ctl_ptr;

	if (len > CLI_COMMAND_LEN_MAX) {
		len = CLI_COMMAND_LEN_MAX;
	}

	for (idx = 0; idx < CLI_COMMAND_ECHO_CNT; idx++) {
		if (echo_ctl_ptr[idx].cmd_order == 0xFF) {
			if (0 == insert_flag) {
				echo_ctl_ptr[idx].cmd_order = 1;
				echo_ctl_ptr[idx].len = len;
				memcpy(echo_ctl_ptr[idx].cmd_ptr, cmd_ptr, len);
				insert_flag = 1;
				cur_cmd_cnt++;
			}
			break;
		} else if(echo_ctl_ptr[idx].cmd_order == CLI_COMMAND_ECHO_CNT) {
			echo_ctl_ptr[idx].cmd_order = 1;
			echo_ctl_ptr[idx].len = len;
			memcpy(echo_ctl_ptr[idx].cmd_ptr, cmd_ptr, len);
			insert_flag = 1;
		} else {
			echo_ctl_ptr[idx].cmd_order += 1;
		}
	}
}
int32_t cli_insert_str(char *input_string, char *cli_dir, uint32_t cur_cursor, uint32_t total_size)
{
	int32_t ret = 0;

	uint32_t cnt = 0;
	char *tmp_str = cli_dir;
	while (*tmp_str++) {
		cnt++;
	}
	if ((total_size + cnt) >= CLI_COMMAND_LEN_MAX) {
		cnt = CLI_COMMAND_LEN_MAX - total_size - 1;
		ret = -1;
	}

	if (cur_cursor < total_size) {
		while (total_size >= cur_cursor) {
			input_string[total_size + cnt] = input_string[total_size];
			total_size--;
		}
	}
	memcpy(&input_string[cur_cursor], cli_dir, cnt);

	return ret;
}
