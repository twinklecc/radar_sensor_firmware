#include "calterah_algo.h"

void bubble_sort(float *arr,        // data array to be sorted
                 int size,          // data array size
                 int *sorted_ind    // sorted data in terms of index in original array
                 )
{
        int i, j;
        float tmp;
        int tmp_ind;
        for (i = 0; i < size; i++) {
                sorted_ind[i] = i;
        }
        for (i = 0; i < size - 1; i++) {
                for (j = 0; j < size - i - 1; j++) {
                        if (arr[j] > arr[j+1]) {
                                tmp = arr[j];
                                arr[j] = arr[j+1];
                                arr[j+1] = tmp;

                                tmp_ind = sorted_ind[j];
                                sorted_ind[j] = sorted_ind[j+1];
                                sorted_ind[j+1] = tmp_ind;
                        }
                }
        }
}
