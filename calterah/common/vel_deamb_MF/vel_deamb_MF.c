/*
 *  Velocity deambiguity using two frames interleaving
 */

#include "vel_deamb_MF.h"

#if VEL_DEAMB_MF_EN

static uint32_t fram0_fft_peak_mem[TRACK_NUM_CDI][MAX_NUM_RX]; // To store the first frame FFT peak values
void NO_PRINTF(const char *format,...){}

static uint32_t previous_chrp_prd, previous_rng_nfft, previous_vel_nfft;
static uint32_t current_chrp_prd, current_rng_nfft, current_vel_nfft;
static uint32_t next_chrp_prd, next_rng_nfft, next_vel_nfft;
static uint8_t previous_frame_type, current_frame_type, next_frame_type;
static uint8_t BPM_NUM = 0;
static uint8_t obj_num_0, obj_num_1; // Numbers of detected objects in type 0 and type 1 frames

/*
 * nodes storing matching info of frame type 1 and frame type 0 objects
 * 'main_obj' indicates frame type 1 object
 * 'candi_obj' indicates frame type 0 object
 * */
static pair_main_obj_info_t main_obj_buf[TRACK_NUM_CDI*MAX_PAIR_CANDI]; // store frame type 1 objects' matching info
static pair_candi_obj_info_t candi_obj_buf[TRACK_NUM_CDI*MAX_PAIR_CANDI];// store frame type 0 objects' matching info

/*
 * Container of potentially matching objects chain
 * Note that frame type 1 objects' chain nodes stores frame type 0 objects' matching info
 * while frame type 0 objects' chain nodes stores frame type 1 objects' matching info
 * */
static pair_main_obj_t frame_type_1_obj[TRACK_NUM_CDI]; // Container of frame type 1 objects' potentially matched object chain
static pair_candi_obj_t frame_type_0_obj[TRACK_NUM_CDI];// Container of frame type 0 objects' potentially matched object chain

bool Is_main_obj_match( unsigned char obj_ind )
{
        return frame_type_1_obj[obj_ind].match_flag;
}

bool Is_candi_obj_match( unsigned char obj_ind )
{
        return frame_type_0_obj[obj_ind].match_flag;
}

signed char main_obj_wrap_num( unsigned char obj_ind )
{
        uint8_t closest_candi_obj_ind = frame_type_1_obj[obj_ind].head->candi_obj_index;
        //signed char wrp_num_0 = frame_type_1_obj[j].head->candi_vel_wrap_num;
        signed char wrp_num_1 = frame_type_0_obj[closest_candi_obj_ind].head->main_vel_wrap_num;
        return wrp_num_1;
}

void print_main_obj_info(uint8_t main_obj_ind)
{
        pair_candi_obj_info_t * tmp = frame_type_1_obj[main_obj_ind].head;
        OPTION_PRINT("Main_obj[%d] has %d pair candi_obj\n",main_obj_ind,frame_type_1_obj[main_obj_ind].candi_obj_num);
        while(tmp != NULL) {
                OPTION_PRINT("Pair candi_obj_info for main_obj[%d] is index: %d, weighted_diff_exp: %d \n", main_obj_ind, tmp->candi_obj_index, tmp->weighted_diff_exp);
                tmp = tmp->next;
        }
        return;
        OPTION_PRINT("\n");
}

void print_candi_obj_info(uint8_t candi_obj_ind)
{
        pair_main_obj_info_t * tmp = frame_type_0_obj[candi_obj_ind].head;
        OPTION_PRINT("Candi_obj[%d] has %d pair main_obj\n",candi_obj_ind,frame_type_0_obj[candi_obj_ind].main_obj_num);
        while(tmp != NULL) {
                OPTION_PRINT("Pair main_obj_info for candi_obj[%d] is index: %d, weighted_diff_exp: %d \n", candi_obj_ind, tmp->main_obj_index, tmp->weighted_diff_exp);
                tmp = tmp->next;
        }
        return;
        OPTION_PRINT("\n");
}
/* Update frame type related parameters */
void frame_type_params_update(baseband_t *bb)
{
        baseband_hw_t *bb_hw = &bb->bb_hw;
        sensor_config_t* cfg = (sensor_config_t*)CONTAINER_OF(bb_hw, baseband_t, bb_hw)->cfg;

        BPM_NUM = baseband_read_reg(bb_hw, BB_REG_SYS_SIZE_BPM); // get virtual array size
        next_chrp_prd = (baseband_read_reg(bb_hw, BB_REG_SYS_SIZE_RNG_PRD) + 1)*(BPM_NUM + 1);
        next_chrp_prd = (int)(cfg->fmcw_chirp_period*10)*(BPM_NUM + 1);// chirp_period (unit:us) is scaled by 10
        next_rng_nfft = baseband_read_reg(bb_hw, BB_REG_SYS_SIZE_RNG_FFT) + 1;
        next_vel_nfft = baseband_read_reg(bb_hw, BB_REG_SYS_SIZE_VEL_FFT) + 1;
        next_frame_type = baseband_read_reg(bb_hw, BB_REG_FDB_SYS_BNK_ACT);

        previous_chrp_prd = current_chrp_prd;
        previous_rng_nfft = current_rng_nfft;
        previous_vel_nfft = current_vel_nfft;
        current_chrp_prd = next_chrp_prd;
        current_rng_nfft = next_rng_nfft;
        current_vel_nfft = next_vel_nfft;

        previous_frame_type = current_frame_type;
        current_frame_type = next_frame_type;
        OPTION_PRINT("current_frame_type = %d \n", current_frame_type);

}
/*
 * Pairing objects detected in type 0 and type 1 frames
 * */
void obj_pair(baseband_t *bb)
{
        baseband_hw_t *bb_hw = &bb->bb_hw;
        //OPTION_PRINT("//===================Current frame_type is %d ====================//\n",current_frame_type);
        OPTION_PRINT("sizeof(pair_main_obj_info_t) is %d\n", sizeof(pair_main_obj_info_t));
        OPTION_PRINT("sizeof(pair_candi_obj_info_t) is %d\n", sizeof(pair_candi_obj_info_t));
        OPTION_PRINT("sizeof(pair_main_obj_t) is %d\n", sizeof(pair_main_obj_t));
        OPTION_PRINT("sizeof(pair_candi_obj_t) is %d\n", sizeof(pair_candi_obj_t));
        // Read detected objects number
        OPTION_PRINT("BB_REG_FDB_SYS_BNK_ACT is %d. \n",baseband_read_reg(NULL, BB_REG_FDB_SYS_BNK_ACT));
        EMBARC_PRINTF("BB_REG_SYS_BNK_ACT is %d. \n",baseband_read_reg(NULL, BB_REG_SYS_BNK_ACT));
        if (current_frame_type == 0) {
                obj_num_0 = BB_READ_REG(bb_hw, CFR_NUMB_OBJ);
                if (obj_num_0 > TRACK_NUM_CDI)
                obj_num_0 = TRACK_NUM_CDI;
                OPTION_PRINT("obj_num_0 is %d\n", obj_num_0);
                FFT_mem_buf2rlt(bb_hw);
        } else if (current_frame_type == 1) {
                obj_num_1 = BB_READ_REG(bb_hw, CFR_NUMB_OBJ);
                if (obj_num_1 > TRACK_NUM_CDI)
                obj_num_1 = TRACK_NUM_CDI;
                OPTION_PRINT("obj_num_1 is %d\n", obj_num_1);
        }

        // Pairing at the end of type 1 frame
        if (current_frame_type == 1) {
                //OPTION_PRINT("//===================Read obj info=====================//\n");
                uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_RLT);
                uint32_t mem_rlt_offset = 1 * (1<<SYS_SIZE_OBJ_WD_BNK) * RESULT_SIZE;// according to track_read()
                //OPTION_PRINT("//===================mem_rlt_offset is %d ====================//\n",mem_rlt_offset);
                volatile obj_info_t *obj_info_0 = (volatile obj_info_t *)(BB_MEM_BASEADDR + 0);// Starting address of type 0 frame detected objects infos
                volatile obj_info_t *obj_info_1 = (volatile obj_info_t *)(BB_MEM_BASEADDR + mem_rlt_offset);// Starting address of type 1 frame detected objects infos

                /*
                 * STEP 1 : Initialization frame type 0 and frame type 1 object chain container
                 * */
                for (uint8_t i = 0; i < obj_num_0; i++) {
                        frame_type_0_obj[i].main_obj_num = 0;
                        frame_type_0_obj[i].match_flag = false;
                        frame_type_0_obj[i].head = NULL;
                        frame_type_0_obj[i].tail = NULL;
                }
                for (uint8_t j = 0; j < obj_num_1; j++) {
                        frame_type_1_obj[j].candi_obj_num = 0;
                        frame_type_1_obj[j].match_flag = false;
                        frame_type_1_obj[j].head = NULL;
                        frame_type_1_obj[j].tail = NULL;
                }
                uint8_t main_obj_buf_ind = 0;
                uint8_t candi_obj_buf_ind = 0;

                OPTION_PRINT("In type 0 frame, obj_num is: %d\n", obj_num_0);
                for (uint8_t i = 0; i < obj_num_0; i++) {
                        OPTION_PRINT("The %dth object info is:\n",i);
                        int hw_rng = obj_info_0[i].rng_idx; //In fact return value is unsigned integer.
                        int hw_vel = obj_info_0[i].vel_idx;
                        int hw_sig = obj_info_0[i].doa[0].sig_0;
                        int hw_noi = obj_info_0[i].noi;
                        int hw_ang = obj_info_0[i].doa[0].ang_idx_0;
                        OPTION_PRINT("hw_rng is %d, hw_vel is %d, hw_sig is %d, hw_noi is %d, hw_ang is %d \n",
                                        hw_rng, hw_vel, hw_sig, hw_noi, hw_ang);
                }

                OPTION_PRINT("\n In type 1 frame, obj_num is: %d\n", obj_num_1);
                for (uint8_t i = 0; i < obj_num_1; i++) {
                        OPTION_PRINT("The %dth object info is:\n",i);
                        int hw_rng = obj_info_1[i].rng_idx; //In fact return value is unsigned integer.
                        int hw_vel = obj_info_1[i].vel_idx;
                        int hw_sig = obj_info_1[i].doa[0].sig_0;
                        int hw_noi = obj_info_1[i].noi;
                        int hw_ang = obj_info_1[i].doa[0].ang_idx_0;
                        OPTION_PRINT("hw_rng is %d, hw_vel is %d, hw_sig is %d, hw_noi is %d, hw_ang is %d\n",
                                        hw_rng, hw_vel, hw_sig, hw_noi, hw_ang);
                }

                /* Set difference threshold for pairing in range, velocity, and angle domain,
                 * FIXME: power difference need to consider, may be influenced by
                 * channel numbers during CFAR combination
                 * */
                float raw_rng_diff;
                float SNR_diff;
                float raw_vel_diff;
                float corr_coe;
                float weighted_diff;// recording total weighted difference between 2 frames' objects
                uint8_t nxt_rng_pair_bgn_ind = 0;
                bool rng_match_flag = false;// reduce search time by recording rng_idx of the first range matching object

                /*
                 * STEP 2 : Create potentially matching object chains for both frame types' objects
                 * */
                for (uint8_t i = 0; i < obj_num_0; i++) { // For each frame type 0 object, find its potentially matching objects from type 1 frame
                        int hw_rng_0 = obj_info_0[i].rng_idx;
                        int hw_vel_0 = obj_info_0[i].vel_idx;
                        rng_match_flag = false;
                        radar_sys_params_t* sys_params_0 = bb->track->radar_params[0];
                        float raw_rng_0, raw_vel_0;
                        radar_param_fft2rv_nowrap(sys_params_0, hw_rng_0, hw_vel_0, &raw_rng_0, &raw_vel_0);

                        int hw_sig_0 = obj_info_0[i].doa[0].sig_0;
                        int hw_noi_0 = obj_info_0[i].noi;
                        float tmpS = fl_to_float(hw_sig_0, 15, 1, false, 5, false);
                        float tmpN = fl_to_float(hw_noi_0, 15, 1, false, 5, false);
                        float SNR0 = 10*log10f(tmpS/tmpN); // TODO: simplify the SNR calculation by exploiting our flr representation

                        OPTION_PRINT("\n >>> Pair the %dth frame type 0 object... \n", i);
//                      OPTION_PRINT("i %d\n", i);
                        for (uint8_t j = nxt_rng_pair_bgn_ind; j < obj_num_1; j++) { // search in from type 1 frame objects set
//                              OPTION_PRINT("j %d\n", j);
                                // read the jth frame type 1 object information
                                int hw_rng_1 = obj_info_1[j].rng_idx;
                                int hw_vel_1 = obj_info_1[j].vel_idx;

                                radar_sys_params_t* sys_params_1 = bb->track->radar_params[1];
                                float raw_rng_1, raw_vel_1;
                                radar_param_fft2rv(sys_params_1, hw_rng_1, hw_vel_1, &raw_rng_1, &raw_vel_1);

                                int hw_sig_1 = obj_info_1[j].doa[0].sig_0;
                                int hw_noi_1 = obj_info_1[j].noi;
                                float tmpS = fl_to_float(hw_sig_1, 15, 1, false, 5, false);
                                float tmpN = fl_to_float(hw_noi_1, 15, 1, false, 5, false);
                                float SNR1 = 10*log10f(tmpS/tmpN);

                                OPTION_PRINT("raw_rng_0 is %4.2f, raw_vel_0 is %4.2f, SNR0 is %4.2f\n",
                                                raw_rng_0, raw_vel_0, SNR0);
                                OPTION_PRINT("raw_rng_1 is %4.2f, raw_vel_1 is %4.2f, SNR1 is %4.2f\n",
                                                raw_rng_1, raw_vel_1, SNR1);

                                /*
                                 *  (STEP 2) Sub-step A:
                                 *  Evaluate the info differences between the ith frame type 0 object
                                 *  and the jth frame type 1 object
                                 * */
                                /* Evaluate the range difference */
                                raw_rng_diff = raw_rng_0 - raw_rng_1;
                                OPTION_PRINT("raw_rng_diff is %4.2f\n", raw_rng_diff);
                                if (raw_rng_diff < -RNG_DIFF_THRES) { // indicating j is too large, since objects are arranged in range ascending order in RAM MEM_RLT
//                                      OPTION_PRINT("break \n");
                                        break;// switch to next (i+1)th frame type 0 object to find its potentially matching objects of type 1 frame
                                } else if (raw_rng_diff > RNG_DIFF_THRES) {
//                                      OPTION_PRINT("continue \n");
                                        continue;// switch to next (j+1)th frame type 1 object to evaluate the info difference
                                } else if ( rng_match_flag == false ) {
//                                      OPTION_PRINT("rng_match first \n");
                                        nxt_rng_pair_bgn_ind = j;// In next i for loop , j starts with initial value "nxt_rng_pair_bgn_ind"
                                        rng_match_flag = true;
                                }
                                /* Evaluate the SNR difference */
                                SNR_diff = fabs(SNR0 - SNR1);
                                OPTION_PRINT("SNR_diff is %4.2f\n", SNR_diff);
                                if (SNR_diff > SNR_DIFF_THRES) {
                                        continue;
                                }
                                /* Evaluate the correlation coefficient of RXs FFT peak values */
                                uint32_t fft_vec[MAX_NUM_RX];
                                get_fft_vec( bb_hw, fft_vec, hw_rng_1, hw_vel_1);
                                corr_coe = corr_coe_cal(fram0_fft_peak_mem[i], fft_vec, MAX_NUM_RX);
                                OPTION_PRINT("corr_coe = %2.4f.\n", corr_coe);
                                if (corr_coe < CORR_COE_THRES) {
                                        continue;
                                }
                                /* Evaluate the velocity difference
                                 * velocity difference is evaluated at last due to possible ambiguity
                                 * */
                                raw_vel_diff = fabs(raw_vel_0 - raw_vel_1);
                                OPTION_PRINT("abs raw_vel_diff is %4.2f\n", raw_vel_diff);
                                signed char wrp_num_0 = 0;
                                signed char wrp_num_1 = 0;
                                if (raw_vel_diff > VEL_DIFF_THRES ) { // If velocity difference is larger than threshold, velocity ambiguity is assumed to occur first
                                        // search the wrap numbers to minimize doppler frequency difference
                                        search_vel_wrap_num ( hw_vel_0, previous_vel_nfft, previous_chrp_prd,
                                                        hw_vel_1, current_vel_nfft, current_chrp_prd,
                                                        &wrp_num_0, &wrp_num_1);
                                        // compensate velocity ambiguity
                                        int hw_vel_0_compen = hw_vel_0 + wrp_num_0*previous_vel_nfft;
                                        int hw_vel_1_compen = hw_vel_1 + wrp_num_1*current_vel_nfft;
                                        // update velocity difference after velocity ambiguity compensation
                                        float raw_vel_0_compen = hw_vel_0_compen*sys_params_0->v_coeff;
                                        float raw_vel_1_compen = hw_vel_1_compen*sys_params_1->v_coeff;
                                        raw_vel_diff = fabs(raw_vel_0_compen - raw_vel_1_compen);
                                        OPTION_PRINT("Compensated abs raw_vel_diff is %4.2f \n", raw_vel_diff);
                                        if (raw_vel_diff > VEL_DIFF_THRES) {
                                                continue; // velocity difference is still large even after ambiguity compensation
                                        }
                                }

                                /* If all types of thresholds are satisfied,
                                 * then calculate total weighted info difference
                                 */
                                /* FIXME perhaps need to add weightings */
                                weighted_diff = fabs(raw_rng_diff) + 0.5*SNR_diff + raw_vel_diff + fabs(1/corr_coe -1); // corr_coe is a positive value

                                OPTION_PRINT("Compensated Weighted_diff is %7.2f\n", weighted_diff);

                                /* In order to compress the data size of object info node,
                                 * transfer float type 'weighted_diff' to pseudo floating point type 'weighted_diff_exp'
                                 */
                                unsigned char weighted_diff_exp = 0;
                                // bit width of mantissa in weighted_diff_exp
                                unsigned char MANTI_W = 4;
                                // bit width of exponent in weighted_diff_exp
                                unsigned char EXP_W = 8 * sizeof(weighted_diff_exp) - (MANTI_W % 8);// 8 is the bit width of a byte
                                OPTION_PRINT("sizeof(weighted_diff_exp) is %d.\n", sizeof(weighted_diff_exp));
                                OPTION_PRINT("MANTI_W is %d, EXP_W is %d.\n", MANTI_W, EXP_W);
                                //weighted_diff is supposed smaller than pow(2,pow(2,EXP_W)-SCAL_EXP))
                                unsigned char SCAL_EXP = 13;
                                int weighted_diff_scl = (int)(weighted_diff*pow(2,SCAL_EXP));// Scaling and rounding

                                // down scale mantissa, increase exponent
                                while(weighted_diff_scl > pow(2,MANTI_W)-1) {
                                        weighted_diff_exp++;
                                        weighted_diff_scl = weighted_diff_scl >> 1;
                                }
                                OPTION_PRINT("weighted_diff_exp is %d\n", weighted_diff_exp); // final exponent
                                OPTION_PRINT("weighted_diff_scl is %d\n", weighted_diff_scl);// final mantissa
                                unsigned char exp_bit_mask = (unsigned char)(pow(2,EXP_W)-1);
                                weighted_diff_exp = (( weighted_diff_exp & exp_bit_mask ) << MANTI_W) + weighted_diff_scl;
                                OPTION_PRINT("weighted_diff_exp is %d\n", weighted_diff_exp);

                                /* (STEP 2) Sub-step B:
                                 * Create the jth frame type 1 object info node
                                 * */
                                pair_main_obj_info_t * main_obj = &(main_obj_buf[main_obj_buf_ind++]);
                                if (main_obj == NULL || main_obj_buf_ind >= TRACK_NUM_CDI*MAX_PAIR_CANDI) {
                                        EMBARC_PRINTF("Memory for main_obj is not enough! \n");
                                        exit(1);
                                        continue;
                                }
//                              OPTION_PRINT("main_obj vaule is %d\n",main_obj);
                                main_obj->main_obj_index = j;
                                main_obj->weighted_diff_exp = weighted_diff_exp;
                                main_obj->main_vel_wrap_num = wrp_num_1;
                                main_obj->next = NULL;
                                /* (STEP 2) Sub-step C:
                                 * Insert Created node to the ith frame type 0 object's potential matching object chain
                                 * */
                                insert_main_obj_info_node(frame_type_0_obj, i, main_obj);

                                /* (STEP 2) Sub-step D:
                                 * Create the ith frame type 0 object info node
                                 * */
                                pair_candi_obj_info_t * candi_obj = &(candi_obj_buf[candi_obj_buf_ind++]);
                                if (candi_obj == NULL || candi_obj_buf_ind >= TRACK_NUM_CDI*MAX_PAIR_CANDI) {
                                        EMBARC_PRINTF("Memory for candi_obj is not enough!\n");
                                        exit(1);
                                        continue;
                                }
                                candi_obj->candi_obj_index = i;
                                candi_obj->weighted_diff_exp = weighted_diff_exp;
                                candi_obj->candi_vel_wrap_num = wrp_num_0;
                                candi_obj->next = NULL;

                                /* (STEP 2) Sub-step E:
                                 * Insert Created node to the jth frame type 1 object's potential matching object chain
                                 * */
                                insert_candi_obj_info_node(frame_type_1_obj, j, candi_obj);

                                OPTION_PRINT("Created nodes! \n");
                        }
                }
                /**/
                OPTION_PRINT("\n");
                OPTION_PRINT("<-----------Print candidate pair objects infos---------->\n");
                for (uint8_t i = 0; i < obj_num_0; i++) {
                        print_candi_obj_info(i);
                }
                OPTION_PRINT("\n");

                OPTION_PRINT("<-----------Print main pair objects infos---------->\n");
                for (uint8_t j = 0; j < obj_num_1; j++) {
                        print_main_obj_info(j);
                }
                OPTION_PRINT("\n");

                /* STEP 3 : Recursively pair frame type 0 and frame type 1 objects using their
                 * potentially matching objects chains
                 * */
                for (uint8_t j = 0; j < obj_num_1; j++) {
                        if (frame_type_1_obj[j].match_flag != true && frame_type_1_obj[j].head != NULL) {
                                recur_pair_main_obj(frame_type_1_obj, frame_type_0_obj, j);
                        }
                }
                /*---------- Print pair results and correct velocity ambiguity ----------*/
//              OPTION_PRINT("\n");
                OPTION_PRINT("<-----------Print pair results infos---------->\n");
                //uint8_t trk_cdi_ind = 0;
                for (uint8_t j = 0; j < obj_num_1; j++) {
                        EMBARC_PRINTF("j = %d\n",j);
                        if (frame_type_1_obj[j].match_flag == true) {
                                uint8_t closest_candi_obj_ind = frame_type_1_obj[j].head->candi_obj_index;
                                signed char wrp_num_0 = frame_type_1_obj[j].head->candi_vel_wrap_num;
                                signed char wrp_num_1 = frame_type_0_obj[closest_candi_obj_ind].head->main_vel_wrap_num;
//                              OPTION_PRINT("candi_obj_num is %d\n", candi_obj_num);

                                int hw_rng_0 = obj_info_0[closest_candi_obj_ind].rng_idx;
                                int hw_vel_0 = obj_info_0[closest_candi_obj_ind].vel_idx;
                                int hw_rng_1 = obj_info_1[j].rng_idx;
                                int hw_vel_1 = obj_info_1[j].vel_idx;
                                EMBARC_PRINTF("Read hw_vel_0 is %d, hw_vel_1 is %d\n", hw_vel_0, hw_vel_1);
                                int hw_vel_0_compen = hw_vel_0 + wrp_num_0*previous_vel_nfft;
                                int hw_vel_1_compen = hw_vel_1 + wrp_num_1*current_vel_nfft;
                                EMBARC_PRINTF("wrp_num_0 is %d, wrp_num_1 is %d\n", wrp_num_0, wrp_num_1);
//                              OPTION_PRINT("Compensated hw_vel_0 is %d, hw_vel_1 is %d\n", hw_vel_0_compen, hw_vel_1_compen);
                                OPTION_PRINT("obj_info_1[%d].amb_idx; %d\n", j, obj_info_1[j].amb_idx);
                                OPTION_PRINT("obj_info_1[%d].noi; %d\n", j, obj_info_1[j].noi);
                                uint32_t noi_val = obj_info_1[j].noi;
                                uint32_t * p_tmp = (uint32_t *)(&(obj_info_1[j]));
                                //wrp_num_1 = 1; // for test
                                unsigned char amb_idx_offset = 1;// memory storage offset of amb_idx in obj_info structure, unit : word
                                if (hw_vel_1 >= current_vel_nfft/2) { // additional -1 unwrap is done in track
                                        // write wrp_num_1 to obj_info_1[j].amb_idx, reorganize as noi(24:5), amb_idx(4:0) due to R/W difference
                                        *(p_tmp + amb_idx_offset) = (noi_val<<5) + (unsigned char)((wrp_num_1 + 1)&0x1F);
                                } else {
                                        *(p_tmp + amb_idx_offset) = (noi_val<<5) + (unsigned char)((wrp_num_1)&0x1F);
                                }
                                OPTION_PRINT("after change, obj_info_1[%d].amb_idx; %d\n", j, obj_info_1[j].amb_idx);
                                OPTION_PRINT("after change, obj_info_1[%d].noi; %d\n", j, obj_info_1[j].noi);
                                // trk_cdi_ind = track_read_1_obj(bb->track, obj_info_1, j, hw_vel_1_compen, trk_cdi_ind, current_frame_type);
                                if(abs(wrp_num_0) > 1 || abs(wrp_num_1) > 1) {
                                        EMBARC_PRINTF("Match info: type_0 frame obj_ind: %d, type_1 frame obj_ind: %d\n", closest_candi_obj_ind, j);
                                        EMBARC_PRINTF("Compensated hw_rng_0 is %d, hw_vel_0 is %d, hw_rng_1 is %d, hw_vel_1 is %d\n",
                                                        hw_rng_0, hw_vel_0_compen, hw_rng_1, hw_vel_1_compen);
                                }
                        }
                }
                baseband_switch_mem_access(bb_hw, old);
        }
        return;
}

/*
 * Store FFT values
 * */
static void get_fft_vec(baseband_hw_t * bb_hw, uint32_t * fft_vec, int rng_index, int vel_index)
{
        uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
        for(uint8_t ch_index = 0; ch_index < MAX_NUM_RX; ch_index++) {
                fft_vec[ch_index] = baseband_hw_get_fft_mem(bb_hw, ch_index, rng_index, vel_index, 0);
        }
        baseband_switch_mem_access(bb_hw, old);
}
/* Store RXs FFT vectors of all CFAR detected peaks from MEM_BUF.
 * The vector size is MAX_NUM_RX, since only data generated
 * by the first TX in VAM mode are stored.
 *  */
static void FFT_mem_buf2rlt(baseband_hw_t *bb_hw)
{
        uint8_t bb_bank_act_id = baseband_read_reg(NULL, BB_REG_SYS_BNK_ACT);
        OPTION_PRINT("\n bb_bank_act_id is %d \n", bb_bank_act_id);
        uint8_t obj_num = BB_READ_REG(bb_hw, CFR_NUMB_OBJ);
        if( obj_num > TRACK_NUM_CDI )
        obj_num = TRACK_NUM_CDI;
        uint32_t old = baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_RLT);
        volatile obj_info_t *obj_info = (volatile obj_info_t *)(BB_MEM_BASEADDR +
                        bb_bank_act_id * (1<<SYS_SIZE_OBJ_WD_BNK) * RESULT_SIZE);

        uint32_t fft_mem[MAX_NUM_RX];
        int rng_index, vel_index;
        // uint32_t * p_tmp;
        for(uint8_t obj_ind = 0; obj_ind < obj_num; obj_ind ++)
        {
                rng_index = obj_info[obj_ind].rng_idx;
                vel_index = obj_info[obj_ind].vel_idx;
                //EMBARC_PRINTF("obj_ind is %d, rng_index is %d, vel_index is %d. \n", obj_ind, rng_index, vel_index);
                baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_BUF);
                get_fft_vec(bb_hw, fram0_fft_peak_mem[obj_ind], rng_index, vel_index);

                for( uint8_t ch_index = 0; ch_index < MAX_NUM_RX; ch_index++ ) {
                        // OPTION_PRINT("\n fram0_fft_peak_mem[%d][%d] = %d \n", obj_ind, ch_index, fram0_fft_peak_mem[obj_ind][ch_index]);
                        fft_mem[ch_index] = baseband_hw_get_fft_mem(bb_hw, ch_index, rng_index, vel_index, 0);
                        fram0_fft_peak_mem[obj_ind][ch_index] = fft_mem[ch_index];
                        // OPTION_PRINT("\n fram0_fft_peak_mem[%d][%d] = %d \n", obj_ind, ch_index, fram0_fft_peak_mem[obj_ind][ch_index]);
                        //EMBARC_PRINTF("\n fft_mem[%d] = %d \n", ch_index, fft_mem[ch_index]);
                }

                baseband_switch_mem_access(bb_hw, SYS_MEM_ACT_RLT);
                /*
                 * // Try to store in MEM_RLT but failed.

                 p_tmp = (uint32_t *)(&(obj_info[obj_ind].doa[1]));
                 for(uint8_t ch_index = 0; ch_index < MAX_NUM_RX-1; ch_index++){
                 *( p_tmp + ch_index )= fft_mem[ch_index];
                 EMBARC_PRINTF("\n stored value is %d \n", obj_info[obj_ind].doa[ch_index].sig_0);
                 }
                 */
        }
        baseband_switch_mem_access(bb_hw, old);
}
/* Calculate correlation coefficient of 2 FFT peak vectors */
float corr_coe_cal(uint32_t * fft_mem1, uint32_t * fft_mem2, uint8_t len)
{
        complex_t init_val;
        init_val.r = 0;
        init_val.i = 0;
        complex_t a;
        complex_t b;
        complex_t innerProd_a_b = init_val;
        complex_t innerProd_a_a = init_val;
        complex_t innerProd_b_b = init_val;
        for(uint8_t i = 0; i < len; i++)
        {
                a = cfl_to_complex(*(fft_mem1+i), 14, 14, true, 4, false);
                b = cfl_to_complex(*(fft_mem2+i), 14, 14, true, 4, false);
                OPTION_PRINT("\n a[%d].r = %5.4f, a[%d].i = %5.4f, ", i, a.r, i, a.i);
                OPTION_PRINT("b[%d].r = %5.4f, b[%d].i = %5.4f.  \n", i, b.r, i, b.i);
                cmult_conj_cum(&a, &b, &innerProd_a_b);
                OPTION_PRINT("\n a_b.r = %5.4f, a_b.i = %5.4f, ", innerProd_a_b.r, innerProd_a_b.i);
                cmult_conj_cum(&a, &a, &innerProd_a_a);
                OPTION_PRINT("\n a_a.r = %5.4f, a_a.i = %5.4f, ", innerProd_a_a.r, innerProd_a_a.i);
                cmult_conj_cum(&b, &b, &innerProd_b_b);
                OPTION_PRINT("\n b_b.r = %5.4f, b_b.i = %5.4f, ", innerProd_b_b.r, innerProd_b_b.i);
        }
        return ( mag_sqr(&innerProd_a_b) / sqrt( mag_sqr(&innerProd_a_a) * mag_sqr(&innerProd_b_b) ) );
}

#endif /* VEL_DEAMB_MF_EN */
