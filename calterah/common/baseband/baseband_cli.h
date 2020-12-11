#ifndef BASEBAND_CLI_H
#define BASEBAND_CLI_H

void baseband_cli_commands( void );
bool baseband_stream_on_dmp_mid();
bool baseband_stream_on_dmp_fnl();
bool baseband_stream_on_fft1d();
bool baseband_scan_stop_req();
bool baseband_stream_off_req();
//avoid DC interference and elevate precision
#define RNG_START_SEARCH_IDX                (4)
#define MAX_VALUE_SEARCH_RANGE_LEN          (16)

#endif
