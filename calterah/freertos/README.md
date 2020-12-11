# Applications on FreeRTOS

Each application (APP) has its own subdirectory. Currently, only a general APP (sensor) is included. One special ``common`` subdirectory includes modules can be used in all APPs here.

## ``common``

List of modules:

1. CLI
   - It received Calterah specific command from UART and output command results if necessary.
2. DataStream
   - It re-direct data output to UART/CAN. The data output format could be binary or text. It mainly used for sensing results output.
