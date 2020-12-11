#include "vel_deamb_MF.h"

#if VEL_DEAMB_MF_EN

/*
 * About chirp parameters configurations:
 *
 * To enable multi-frame velocity de-ambiguity function, two frames with different chirp periods are required.
 * Except for chirp period, it is suggested that the FMCW parameters are set to be the same so that
 * targets generated in two frames can be easily matched.
 *
 * The velocity ambiguity is usually caused by virtual array which increased the chirp periods. It is
 * suggested that the first frame is responsible for generating as many targets as possible to match
 * the second frame targets and the second frame is responsible for virtual array to increase angle resolution.
 *
 * A pair of already verified configurations are list below as an example:
 *
 * ----------frame type0 configs:------------
 * fmcw_startfreq = 76.800
 * fmcw_bandwidth = 350.00
 * fmcw_chirp_rampup = 14.00
 * fmcw_chirp_down = 2.00
 * fmcw_chirp_period = 47.00
 * nchirp = 256
 * adc_freq = 50
 * dec_factor = 1
 * adc_sample_start = 2.00
 * adc_sample_end = 13.00
 * tx_groups = [1, 0, 0, 0]
 * rng_nfft = 512
 * vel_nfft = 256 // higher velocity resolution helps to detect more targets
 * doa_fft_mux = [15, 4369, 33825] // the first value 16 = 0xf means 4 channels used in DoA
 *
 * ----------frame type1 configs:-------------
 * fmcw_startfreq = 76.800
 * fmcw_bandwidth = 350.00
 * fmcw_chirp_rampup = 14.00
 * fmcw_chirp_down = 2.00
 * fmcw_chirp_period = 33.00
 * nchirp = 256
 * adc_freq = 50
 * dec_factor = 1
 * adc_sample_start = 2.00
 * adc_sample_end = 13.00
 * tx_groups = [1, 0, 0, 16]  // TX0 and TX3 TDM virtual array
 * rng_nfft = 512
 * vel_nfft = 128
 * doa_fft_mux = [255, 292, 33825] // the first value 256 = 0xff means 8 channels used in DoA

*/

/*
 * Function: Search Doppler frequency domain wrap number
 *
 * Description:
 * For 2 different chirp periods, doppler frequency domain wrapping satisfies:
 *
 * v_ind1/vel_nfft1*1/Tc1 + n1*1/Tc1 = v_ind2/vel_nfft2*1/Tc2 + n2*1/Tc2
 *
 * That is:
 *
 * v_ind1*vel_nfft2*Tc2 + n1*vel_nfft1*vel_nfft2*Tc2
 * = v_ind2*vel_nfft1*Tc1 + n2*vel_nfft1*vel_nfft2*Tc1
 *
 * This function is to search n1, n2 to minimize the following difference:
 *
 * Diff = v_ind1*vel_nfft2*Tc2 - v_ind2*vel_nfft1*Tc1
 *       + (n1*Tc2 - n2*Tc1)*vel_nfft1*vel_nfft2
 *
 * Tips:
 * Given different n1, n2 in a constrained range (e.g. [-3,3]),
 * to avoid the same 'Diff' value, do not choose Tc1 and Tc2 that let
 * n1*Tc2 - n2*Tc1 has the same value .
 * A bad example is Tc2 : Tc1 = 4 : 5, thus (n1, n2) = (-3, -3) and (n1, n2) = (2, 1)
 * will result in the same 'Diff' value.
*/
void search_vel_wrap_num (uint32_t v_ind1, uint32_t v_nfft1, uint32_t Tc1, \
                          uint32_t v_ind2, uint32_t v_nfft2, uint32_t Tc2, \
                          signed char * wrp_num1, signed char * wrp_num2)
{
        signed char n1_tmp = 0, n2_tmp = 0;
        signed int diff_init = v_ind1 * Tc2 * v_nfft2 - v_ind2 * Tc1 * v_nfft1;
        signed int diff_min = (diff_init >= 0) ? diff_init : (- diff_init);
        uint32_t v_nfft2_mult_v_nfft1 = v_nfft2 * v_nfft1;
        //OPTION_PRINT("diff_init = %d, diff_min = %d, v_nfft2_mult_v_nfft1 = %d \n", diff_init, diff_min, v_nfft2_mult_v_nfft1);
        for (signed char n1 = - MAX_WRAP_NUM - 1; n1 <= MAX_WRAP_NUM - 1; n1++) {   // - 1 is because negative velocity's value is usually larger than positive velocity's
                for (signed char n2 = - MAX_WRAP_NUM - 1; n2 <= MAX_WRAP_NUM - 1; n2++) {
                        signed int tmp = (diff_init + (n1 * Tc2 - n2 * Tc1) * v_nfft2_mult_v_nfft1);
                        //OPTION_PRINT("tmp = %d, n1 = %d, n2 = %d \n", tmp, n1, n2);
                        tmp = (tmp >= 0) ? tmp : - tmp;
                        //OPTION_PRINT("tmp = %d, n1 = %d, n2 = %d \n", tmp, n1, n2);
                        if (diff_min > tmp) {
                                diff_min = tmp;
                                n1_tmp = n1;
                                n2_tmp = n2;
                        }
                }
        }
        *wrp_num1 = n1_tmp;
        *wrp_num2 = n2_tmp;
        //OPTION_PRINT("diff_min = %d, wrp_num1 = %d, wrp_num2 = %d \n", diff_min, n1_tmp, n2_tmp);
        return;
}

/*
 * Insert main obj (frame type1 obj) matching info node to candi obj(frame type0 obj)'s
 * potential matching objs chain.
 *  */
void insert_main_obj_info_node(pair_candi_obj_t *frame_type_0_obj, uint8_t candi_obj_ind, pair_main_obj_info_t * main_obj)
{
        pair_main_obj_info_t * tmp = frame_type_0_obj[candi_obj_ind].head;

        if(tmp == NULL) { // When head and tail all point to NULL, insert the first node
//      OPTION_PRINT("frame_type_0_obj head, tail init!\n");
                frame_type_0_obj[candi_obj_ind].head = main_obj;
                frame_type_0_obj[candi_obj_ind].tail = main_obj;
        } else if(tmp->weighted_diff_exp >= main_obj->weighted_diff_exp) { // When head point to a main object whose weighted_diff_exp
                                                                 // is larger than main_obj's weighted_diff_exp
                                        main_obj->next = tmp;
                                        frame_type_0_obj[candi_obj_ind].head = main_obj;// insert as the first node in the chain
        } else {
                // Find insert position in weighted_diff_exp increasing order
                while(tmp->next != NULL && tmp->next->weighted_diff_exp < main_obj->weighted_diff_exp) {
                        tmp = tmp->next;
                }
                if(tmp->next == NULL) { // Add node to the tail of chain
                        tmp->next = main_obj;
                        frame_type_0_obj[candi_obj_ind].tail = main_obj;
                } else {   // Insert node
                        main_obj->next = tmp->next;
                        tmp->next = main_obj;
                }
        }
        frame_type_0_obj[candi_obj_ind].main_obj_num ++; // length of the ith frame type 0 object's potentially matching object chain increased
}

/*
 * Insert candi obj (frame type0 obj) matching info node to main obj(frame type1 obj)'s
 * potential matching objs chain.
 *  */
void insert_candi_obj_info_node(pair_main_obj_t *frame_type_1_obj, uint8_t main_obj_ind, pair_candi_obj_info_t * candi_obj)
{
        pair_candi_obj_info_t * tmp1 = frame_type_1_obj[main_obj_ind].head;
        if(tmp1 == NULL) { // When head and tail all point to NULL
//      OPTION_PRINT("frame_type_1_obj head, tail init!\n");
                frame_type_1_obj[main_obj_ind].head = candi_obj;
                frame_type_1_obj[main_obj_ind].tail = candi_obj;
        } else if(tmp1->weighted_diff_exp >= candi_obj->weighted_diff_exp) { // When head point to a candidate object whose weighted_diff_exp
                                                                  // is larger than candi_obj's weighted_diff_exp
                candi_obj->next = tmp1;
                frame_type_1_obj[main_obj_ind].head = candi_obj;// insert as the first node in the chain
        } else {
                // Find insert position in weighted_diff_exp increasing order
                while(tmp1->next != NULL && tmp1->next->weighted_diff_exp < candi_obj->weighted_diff_exp) {
                        tmp1 = tmp1->next;
                }
                if(tmp1->next == NULL) { // Add node to the tail of chain
                        tmp1->next = candi_obj;
                        frame_type_1_obj[main_obj_ind].tail = candi_obj;
                } else {   // Insert candi_obj
                        candi_obj->next = tmp1->next;
                        tmp1->next = candi_obj;
                }
        }
        frame_type_1_obj[main_obj_ind].candi_obj_num ++; // length of the jth frame type 1 object's potentially matching object chain increased
}

/*
 * Function: Recursively pair frame type0 and type1 objects
 *
 * Description: For details, refer to 'Example Matching Algorithm' for velocity de-ambiguity
 * in chapter 13.6.2.2 in ALPS-MP baseband user guide.
 *
*/

bool recur_pair_main_obj(pair_main_obj_t *frame_type_1_obj, pair_candi_obj_t *frame_type_0_obj, uint8_t main_obj_ind){
        //OPTION_PRINT("// Recur Pair main_obj %d //\n",main_obj_ind);
        while (frame_type_1_obj[main_obj_ind].head != NULL && frame_type_1_obj[main_obj_ind].match_flag != true ) {
                uint8_t closest_candi_obj_ind = frame_type_1_obj[main_obj_ind].head->candi_obj_index;
                uint8_t closest_main_obj_ind = frame_type_0_obj[closest_candi_obj_ind].head->main_obj_index;
                if (closest_main_obj_ind == main_obj_ind){ // Successfully paired! && frame_type_0_obj[closest_candi_obj_ind].match_flag != true;
                        frame_type_0_obj[closest_candi_obj_ind].match_flag = true;
                        frame_type_1_obj[main_obj_ind].match_flag = true;
                } else {
                        recur_pair_candi_obj(frame_type_1_obj, frame_type_0_obj, closest_candi_obj_ind); //search the smallest weighted_diff and pair
                        if (frame_type_1_obj[main_obj_ind].match_flag != true) { // frame_type_1_obj[main_obj_ind].match_flag may turn to true during
                                                                                     // previous pairing process in recur_pair_candi_obj()
                                frame_type_1_obj[main_obj_ind].head = frame_type_1_obj[main_obj_ind].head->next;  // Try to match with next candi_obj
                        }
                }
        }

        if (frame_type_1_obj[main_obj_ind].match_flag == true){
                return true;
        } else { // frame_type_1_obj[main_obj_ind].head == NULL, not matched to any one of its candidate object
                return false;
        }

}

bool recur_pair_candi_obj(pair_main_obj_t *frame_type_1_obj, pair_candi_obj_t *frame_type_0_obj, uint8_t candi_obj_ind){
        //OPTION_PRINT("// Recur Pair candi_obj %d //\n",candi_obj_ind);
        while (frame_type_0_obj[candi_obj_ind].head != NULL && frame_type_0_obj[candi_obj_ind].match_flag != true ) {
                uint8_t closest_main_obj_ind = frame_type_0_obj[candi_obj_ind].head->main_obj_index;
                uint8_t closest_candi_obj_ind = frame_type_1_obj[closest_main_obj_ind].head->candi_obj_index;  // frame_type_1_obj[closest_main_obj_ind].head couldn't be NULL
                if (closest_candi_obj_ind == candi_obj_ind){ // Successfully paired! && frame_type_0_obj[closest_candi_obj_ind].match_flag != true;
                        frame_type_0_obj[candi_obj_ind].match_flag = true;
                        frame_type_1_obj[closest_main_obj_ind].match_flag = true;
                } else {
                        recur_pair_main_obj(frame_type_1_obj, frame_type_0_obj, closest_main_obj_ind); //search the smallest weighted_diff and pair
                        if (frame_type_0_obj[candi_obj_ind].match_flag != true) { // frame_type_0_obj[candi_obj_ind].match_flag may turn to true during
                                                                                      // previous pairing process in recur_pair_main_obj()
                                frame_type_0_obj[candi_obj_ind].head = frame_type_0_obj[candi_obj_ind].head->next; // Try to match with next main_obj
                        }
                }
        }

        if (frame_type_0_obj[candi_obj_ind].match_flag == true){
                return true;
        } else { // frame_type_0_obj[candi_obj_ind].head == NULL, not matched to any one of its candidate object
                return false;
        }

}

#endif

