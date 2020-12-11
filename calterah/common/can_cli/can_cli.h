#ifndef _CAN_CLI_H_
#define _CAN_CLI_H_

typedef int32_t (*can_cli_callback)(uint8_t *data, uint32_t len);

int32_t can_cli_init(void);
int32_t can_cli_register(uint32_t msg_id, can_cli_callback func);

#endif
