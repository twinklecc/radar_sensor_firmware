#include "stdio.h"
#include "CuTest.h"

extern CuSuite* math_conversion_get_suite();
extern CuSuite* fmcw_radio_get_suite();
extern CuSuite* radar_sys_params_get_suite();
extern CuSuite* fft_window_get_suite();
extern CuSuite* steering_vector_get_suite();

void RunAllTests(void) {
        CuString *output = CuStringNew();
        CuSuite* suite = CuSuiteNew();

        CuSuiteAddSuite(suite, math_conversion_get_suite());
        CuSuiteAddSuite(suite, fmcw_radio_get_suite());
        CuSuiteAddSuite(suite, radar_sys_params_get_suite());
        CuSuiteAddSuite(suite, fft_window_get_suite());
        CuSuiteAddSuite(suite, steering_vector_get_suite());

        CuSuiteRun(suite);
        CuSuiteSummary(suite, output);
        CuSuiteDetails(suite, output);
        printf("%s\n", output->buffer);
}

int main(void) {
        RunAllTests();
}
