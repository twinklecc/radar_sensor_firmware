#ifndef CALTERAH_CLUSTER_H
#define CALTERAH_CLUSTER_H

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_toolchain.h"
#endif

typedef struct cluster {
        uint32_t dummy;
} cluster_t;

void cluster_init(cluster_t *cls);
void cluster_start(cluster_t *cls);
void cluster_stop(cluster_t *cls);

#endif
