#ifndef _VERSION_H_
#define _VERSION_H_

#ifndef MAJOR_VERSION_ID
#define MAJOR_VERSION_ID	(0)
#endif

#ifndef MINOR_VERSION_ID
#define MINOR_VERSION_ID	(1)
#endif

#ifndef STAGE_VERSION_ID
#define STAGE_VERSION_ID	(0)
#endif

#ifndef COMPILE_DATE_YEAR
#define COMPILE_DATE_YEAR	(2020)
#endif

#ifndef COMPILE_DATE_MONTH
#define COMPILE_DATE_MONTH	(01)
#endif

#ifndef COMPILE_DATE_DAY
#define COMPILE_DATE_DAY	(01)
#endif

#ifndef SYS_NAME_STR
#define SYS_NAME_STR		"Calterah_Radar_System_CA"
#endif

#ifndef BUILD_COMMIT_ID
#define BUILD_COMMIT_ID		"0000000"
#endif

#define COMPILE_DATE_LEN_MAX	(16)
#define CIMPILE_TIME_LEN_MAX	(16)
#define SYSTEM_NAME_LEN_MAX		(24)
typedef struct {
	uint8_t major_ver_id;
	uint8_t minor_ver_id;
	uint8_t stage_ver_id;
	uint8_t reserverd0;
	uint8_t date[COMPILE_DATE_LEN_MAX];
	uint8_t time[CIMPILE_TIME_LEN_MAX];
	uint8_t info[SYSTEM_NAME_LEN_MAX];
} sw_version_t;

#endif
