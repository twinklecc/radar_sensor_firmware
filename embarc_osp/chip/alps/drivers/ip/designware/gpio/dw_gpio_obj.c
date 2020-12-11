#include "embARC_toolchain.h"
#include "alps/alps.h"
#include "dw_gpio.h"
#include "dbg_gpio_reg.h"

static dw_gpio_t dev_dw_gpio = {
        .base = REL_REGBASE_GPIO,
        .int_no = INTNO_GPIO
};

static void dw_gpio_resource_install(void)
{
        dw_gpio_install_ops(&dev_dw_gpio);
}

void *dw_gpio_get_dev(void)
{
        static uint32_t dw_gpio_install_flag = 0;

        if (0 == dw_gpio_install_flag) {
            dw_gpio_resource_install();
            dw_gpio_install_flag = 1;
        }

        return (void *)&dev_dw_gpio;
}
