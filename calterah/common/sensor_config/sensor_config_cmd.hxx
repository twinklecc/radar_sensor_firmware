        
        const char *param1, *param2;
        BaseType_t len1, len2;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
        sensor_config_t *cfg;
        if (param1 != NULL) {
                cfg = sensor_config_get_cur_cfg(); 
                if (strncmp(param1, "fmcw_startfreq", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fmcw_startfreq = (double) strtod(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_startfreq = %.4f\n\r", cfg->fmcw_startfreq);
                        return pdFALSE;
                }
                if (strncmp(param1, "fmcw_bandwidth", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fmcw_bandwidth = (double) strtod(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_bandwidth = %.4f\n\r", cfg->fmcw_bandwidth);
                        return pdFALSE;
                }
                if (strncmp(param1, "fmcw_chirp_rampup", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fmcw_chirp_rampup = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_chirp_rampup = %.4f\n\r", cfg->fmcw_chirp_rampup);
                        return pdFALSE;
                }
                if (strncmp(param1, "fmcw_chirp_down", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fmcw_chirp_down = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_chirp_down = %.4f\n\r", cfg->fmcw_chirp_down);
                        return pdFALSE;
                }
                if (strncmp(param1, "fmcw_chirp_period", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fmcw_chirp_period = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_chirp_period = %.4f\n\r", cfg->fmcw_chirp_period);
                        return pdFALSE;
                }
                if (strncmp(param1, "nchirp", len1) == 0) {
                        if (param2 != NULL)
                                cfg->nchirp = (int32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "nchirp = %d\n\r", cfg->nchirp);
                        return pdFALSE;
                }
                if (strncmp(param1, "adc_freq", len1) == 0) {
                        if (param2 != NULL)
                                cfg->adc_freq = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "adc_freq = %u\n\r", cfg->adc_freq);
                        return pdFALSE;
                }
                if (strncmp(param1, "dec_factor", len1) == 0) {
                        if (param2 != NULL)
                                cfg->dec_factor = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "dec_factor = %u\n\r", cfg->dec_factor);
                        return pdFALSE;
                }
                if (strncmp(param1, "adc_sample_start", len1) == 0) {
                        if (param2 != NULL)
                                cfg->adc_sample_start = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "adc_sample_start = %.4f\n\r", cfg->adc_sample_start);
                        return pdFALSE;
                }
                if (strncmp(param1, "adc_sample_end", len1) == 0) {
                        if (param2 != NULL)
                                cfg->adc_sample_end = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "adc_sample_end = %.4f\n\r", cfg->adc_sample_end);
                        return pdFALSE;
                }
                if (strncmp(param1, "tx_groups", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < MAX_NUM_TX) {
                                cfg->tx_groups[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "tx_groups = [");
                        for(cnt = 0; cnt < MAX_NUM_TX; cnt++) {
                                if (cnt != MAX_NUM_TX - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->tx_groups[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->tx_groups[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "rng_win", len1) == 0) {
                        if (param2 != NULL)
                                snprintf(cfg->rng_win, MIN(len2, 16-1)+1, "%s", param2);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rng_win = %s\n\r", cfg->rng_win);
                        return pdFALSE;
                }
                if (strncmp(param1, "vel_win", len1) == 0) {
                        if (param2 != NULL)
                                snprintf(cfg->vel_win, MIN(len2, 16-1)+1, "%s", param2);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "vel_win = %s\n\r", cfg->vel_win);
                        return pdFALSE;
                }
                if (strncmp(param1, "rng_win_params", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 3) {
                                cfg->rng_win_params[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "rng_win_params = [");
                        for(cnt = 0; cnt < 3; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->rng_win_params[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->rng_win_params[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "vel_win_params", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 3) {
                                cfg->vel_win_params[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "vel_win_params = [");
                        for(cnt = 0; cnt < 3; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->vel_win_params[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->vel_win_params[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "rng_nfft", len1) == 0) {
                        if (param2 != NULL)
                                cfg->rng_nfft = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rng_nfft = %u\n\r", cfg->rng_nfft);
                        return pdFALSE;
                }
                if (strncmp(param1, "vel_nfft", len1) == 0) {
                        if (param2 != NULL)
                                cfg->vel_nfft = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "vel_nfft = %u\n\r", cfg->vel_nfft);
                        return pdFALSE;
                }
                if (strncmp(param1, "rng_fft_scalar", len1) == 0) {
                        if (param2 != NULL)
                                cfg->rng_fft_scalar = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rng_fft_scalar = 0x%X\n\r", cfg->rng_fft_scalar);
                        return pdFALSE;
                }
                if (strncmp(param1, "vel_fft_scalar", len1) == 0) {
                        if (param2 != NULL)
                                cfg->vel_fft_scalar = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "vel_fft_scalar = 0x%X\n\r", cfg->vel_fft_scalar);
                        return pdFALSE;
                }
                if (strncmp(param1, "fft_nve_bypass", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fft_nve_bypass = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fft_nve_bypass = %d\n\r", cfg->fft_nve_bypass);
                        return pdFALSE;
                }
                if (strncmp(param1, "fft_nve_shift", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fft_nve_shift = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fft_nve_shift = %u\n\r", cfg->fft_nve_shift);
                        return pdFALSE;
                }
                if (strncmp(param1, "fft_nve_ch_mask", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fft_nve_ch_mask = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fft_nve_ch_mask = 0x%X\n\r", cfg->fft_nve_ch_mask);
                        return pdFALSE;
                }
                if (strncmp(param1, "fft_nve_default_value", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fft_nve_default_value = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fft_nve_default_value = %.4f\n\r", cfg->fft_nve_default_value);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_pk_en", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_pk_en = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_pk_en = %u\n\r", cfg->cfar_pk_en);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_pk_win_size1", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_pk_win_size1[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_pk_win_size1 = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_pk_win_size1[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_pk_win_size1[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_pk_win_size2", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_pk_win_size2[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_pk_win_size2 = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_pk_win_size2[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_pk_win_size2[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_pk_threshold", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_pk_threshold[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_pk_threshold = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_pk_threshold[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_pk_threshold[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_sliding_win", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_sliding_win = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_sliding_win = %u\n\r", cfg->cfar_sliding_win);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_recwin_decimate", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_recwin_decimate = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_recwin_decimate = %u\n\r", cfg->cfar_recwin_decimate);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_recwin_msk", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_RECWIN_MSK_LEN) {
                                cfg->cfar_recwin_msk[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_recwin_msk = [");
                        for(cnt = 0; cnt < CFAR_MAX_RECWIN_MSK_LEN; cnt++) {
                                if (cnt != CFAR_MAX_RECWIN_MSK_LEN - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_recwin_msk[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_recwin_msk[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_region_algo_type", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_region_algo_type = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_region_algo_type = %u\n\r", cfg->cfar_region_algo_type);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_os_rnk_ratio", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_os_rnk_ratio[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_os_rnk_ratio = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_os_rnk_ratio[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_os_rnk_ratio[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_os_rnk_sel", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_os_rnk_sel[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_os_rnk_sel = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_os_rnk_sel[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_os_rnk_sel[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_os_tdec", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_os_tdec[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_os_tdec = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_os_tdec[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_os_tdec[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_os_alpha", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_os_alpha[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_os_alpha = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_os_alpha[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_os_alpha[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_combine_dirs", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_combine_dirs = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_combine_dirs = %u\n\r", cfg->cfar_combine_dirs);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_combine_thetas", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < MAX_CFAR_DIRS) {
                                cfg->cfar_combine_thetas[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_combine_thetas = [");
                        for(cnt = 0; cnt < MAX_CFAR_DIRS; cnt++) {
                                if (cnt != MAX_CFAR_DIRS - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_combine_thetas[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_combine_thetas[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_combine_phis", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < MAX_CFAR_DIRS) {
                                cfg->cfar_combine_phis[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_combine_phis = [");
                        for(cnt = 0; cnt < MAX_CFAR_DIRS; cnt++) {
                                if (cnt != MAX_CFAR_DIRS - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_combine_phis[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_combine_phis[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_crswin_rng_size", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_crswin_rng_size = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_crswin_rng_size = %u\n\r", cfg->cfar_crswin_rng_size);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_crswin_rng_skip", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_crswin_rng_skip = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_crswin_rng_skip = %u\n\r", cfg->cfar_crswin_rng_skip);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_crswin_vel_size", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_crswin_vel_size = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_crswin_vel_size = %u\n\r", cfg->cfar_crswin_vel_size);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_crswin_vel_skip", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_crswin_vel_skip = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_crswin_vel_skip = %u\n\r", cfg->cfar_crswin_vel_skip);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_mimo_win", len1) == 0) {
                        if (param2 != NULL)
                                snprintf(cfg->cfar_mimo_win, MIN(len2, 16-1)+1, "%s", param2);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_mimo_win = %s\n\r", cfg->cfar_mimo_win);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_mimo_win_params", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 3) {
                                cfg->cfar_mimo_win_params[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_mimo_win_params = [");
                        for(cnt = 0; cnt < 3; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_mimo_win_params[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_mimo_win_params[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_noise_type", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cfar_noise_type = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_noise_type = %u\n\r", cfg->cfar_noise_type);
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_nr_alpha", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_nr_alpha[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_alpha = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_nr_alpha[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_nr_alpha[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_nr_beta1", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_nr_beta1[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_beta1 = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_nr_beta1[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_nr_beta1[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_nr_beta2", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_nr_beta2[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_beta2 = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_nr_beta2[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_nr_beta2[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_nr_rnk_ratio", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_nr_rnk_ratio[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_rnk_ratio = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_nr_rnk_ratio[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_nr_rnk_ratio[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_nr_rnk_sel", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_nr_rnk_sel[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_rnk_sel = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_nr_rnk_sel[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_nr_rnk_sel[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_nr_scheme_sel", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_nr_scheme_sel[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_scheme_sel = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_nr_scheme_sel[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_nr_scheme_sel[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_nr_tdec", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_nr_tdec[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_tdec = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_nr_tdec[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_nr_tdec[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_region_sep_rng", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 3) {
                                cfg->cfar_region_sep_rng[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_region_sep_rng = [");
                        for(cnt = 0; cnt < 3; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_region_sep_rng[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_region_sep_rng[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_region_sep_vel", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 8) {
                                cfg->cfar_region_sep_vel[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_region_sep_vel = [");
                        for(cnt = 0; cnt < 8; cnt++) {
                                if (cnt != 8 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_region_sep_vel[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_region_sep_vel[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_sogo_alpha", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_sogo_alpha[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_sogo_alpha = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_sogo_alpha[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_sogo_alpha[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_sogo_i", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_sogo_i[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_sogo_i = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_sogo_i[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_sogo_i[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_sogo_mask", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_sogo_mask[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_sogo_mask = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "0x%X, ", cfg->cfar_sogo_mask[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "0x%X]\n\r", cfg->cfar_sogo_mask[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_ca_alpha", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_ca_alpha[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_ca_alpha = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_ca_alpha[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_ca_alpha[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "cfar_ca_n", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < CFAR_MAX_GRP_NUM) {
                                cfg->cfar_ca_n[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_ca_n = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_ca_n[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_ca_n[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "doa_mode", len1) == 0) {
                        if (param2 != NULL)
                                cfg->doa_mode = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_mode = %u\n\r", cfg->doa_mode);
                        return pdFALSE;
                }
                if (strncmp(param1, "doa_num_groups", len1) == 0) {
                        if (param2 != NULL)
                                cfg->doa_num_groups = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_num_groups = %u\n\r", cfg->doa_num_groups);
                        return pdFALSE;
                }
                if (strncmp(param1, "doa_fft_mux", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < MAX_BFM_GROUP_NUM) {
                                cfg->doa_fft_mux[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "doa_fft_mux = [");
                        for(cnt = 0; cnt < MAX_BFM_GROUP_NUM; cnt++) {
                                if (cnt != MAX_BFM_GROUP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->doa_fft_mux[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->doa_fft_mux[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "combined_doa_fft_mux", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 4) {
                                cfg->combined_doa_fft_mux[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "combined_doa_fft_mux = [");
                        for(cnt = 0; cnt < 4; cnt++) {
                                if (cnt != 4 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->combined_doa_fft_mux[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->combined_doa_fft_mux[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "doa_method", len1) == 0) {
                        if (param2 != NULL)
                                cfg->doa_method = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_method = %u\n\r", cfg->doa_method);
                        return pdFALSE;
                }
                if (strncmp(param1, "doa_npoint", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < MAX_BFM_GROUP_NUM) {
                                cfg->doa_npoint[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "doa_npoint = [");
                        for(cnt = 0; cnt < MAX_BFM_GROUP_NUM; cnt++) {
                                if (cnt != MAX_BFM_GROUP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->doa_npoint[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->doa_npoint[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "doa_samp_space", len1) == 0) {
                        if (param2 != NULL)
                                cfg->doa_samp_space = (char) param2[0];
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_samp_space = %c\n\r", cfg->doa_samp_space);
                        return pdFALSE;
                }
                if (strncmp(param1, "doa_max_obj_per_bin", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < MAX_BFM_GROUP_NUM) {
                                cfg->doa_max_obj_per_bin[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "doa_max_obj_per_bin = [");
                        for(cnt = 0; cnt < MAX_BFM_GROUP_NUM; cnt++) {
                                if (cnt != MAX_BFM_GROUP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->doa_max_obj_per_bin[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->doa_max_obj_per_bin[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_peak_scalar", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 3) {
                                cfg->bfm_peak_scalar[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_peak_scalar = [");
                        for(cnt = 0; cnt < 3; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->bfm_peak_scalar[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->bfm_peak_scalar[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_noise_level_scalar", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 3) {
                                cfg->bfm_noise_level_scalar[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_noise_level_scalar = [");
                        for(cnt = 0; cnt < 3; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->bfm_noise_level_scalar[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->bfm_noise_level_scalar[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_snr_thres", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 2) {
                                cfg->bfm_snr_thres[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_snr_thres = [");
                        for(cnt = 0; cnt < 2; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->bfm_snr_thres[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->bfm_snr_thres[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_az_left", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bfm_az_left = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_az_left = %.4f\n\r", cfg->bfm_az_left);
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_az_right", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bfm_az_right = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_az_right = %.4f\n\r", cfg->bfm_az_right);
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_ev_up", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bfm_ev_up = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_ev_up = %.4f\n\r", cfg->bfm_ev_up);
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_ev_down", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bfm_ev_down = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_ev_down = %.4f\n\r", cfg->bfm_ev_down);
                        return pdFALSE;
                }
                if (strncmp(param1, "doa_win", len1) == 0) {
                        if (param2 != NULL)
                                snprintf(cfg->doa_win, MIN(len2, 16-1)+1, "%s", param2);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_win = %s\n\r", cfg->doa_win);
                        return pdFALSE;
                }
                if (strncmp(param1, "doa_win_params", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 3) {
                                cfg->doa_win_params[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "doa_win_params = [");
                        for(cnt = 0; cnt < 3; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->doa_win_params[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->doa_win_params[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_raw_search_step", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bfm_raw_search_step = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_raw_search_step = %u\n\r", cfg->bfm_raw_search_step);
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_fine_search_range", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bfm_fine_search_range = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_fine_search_range = %u\n\r", cfg->bfm_fine_search_range);
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_iter_search", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bfm_iter_search = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_iter_search = %d\n\r", cfg->bfm_iter_search);
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_mode", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bfm_mode = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_mode = %u\n\r", cfg->bfm_mode);
                        return pdFALSE;
                }
                if (strncmp(param1, "bfm_group_idx", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bfm_group_idx = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_group_idx = %u\n\r", cfg->bfm_group_idx);
                        return pdFALSE;
                }
                if (strncmp(param1, "ant_info_from_flash", len1) == 0) {
                        if (param2 != NULL)
                                cfg->ant_info_from_flash = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "ant_info_from_flash = %d\n\r", cfg->ant_info_from_flash);
                        return pdFALSE;
                }
                if (strncmp(param1, "ant_info_flash_addr", len1) == 0) {
                        if (param2 != NULL)
                                cfg->ant_info_flash_addr = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "ant_info_flash_addr = 0x%X\n\r", cfg->ant_info_flash_addr);
                        return pdFALSE;
                }
                if (strncmp(param1, "ant_pos", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < MAX_ANT_ARRAY_SIZE) {
                                if (cnt % 2 == 0)
                                        cfg->ant_pos[cnt/2].x = (float) strtof(param2, NULL);
                                else
                                        cfg->ant_pos[cnt/2].y = (float) strtof(param2, NULL);
                                cnt++;
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "ant_pos = [");
                        for(cnt = 0; cnt < MAX_ANT_ARRAY_SIZE && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != MAX_ANT_ARRAY_SIZE - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "(%.2f, %.2f), ", cfg->ant_pos[cnt].x, cfg->ant_pos[cnt].y);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "(%.2f, %.2f)]\n\r", cfg->ant_pos[cnt].x, cfg->ant_pos[cnt].y);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "ant_comps", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < MAX_ANT_ARRAY_SIZE) {
                                cfg->ant_comps[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "ant_comps = [");
                        for(cnt = 0; cnt < MAX_ANT_ARRAY_SIZE; cnt++) {
                                if (cnt != MAX_ANT_ARRAY_SIZE - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->ant_comps[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->ant_comps[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "bpm_mode", len1) == 0) {
                        if (param2 != NULL)
                                cfg->bpm_mode = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bpm_mode = %d\n\r", cfg->bpm_mode);
                        return pdFALSE;
                }
                if (strncmp(param1, "phase_scramble_on", len1) == 0) {
                        if (param2 != NULL)
                                cfg->phase_scramble_on = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "phase_scramble_on = %u\n\r", cfg->phase_scramble_on);
                        return pdFALSE;
                }
                if (strncmp(param1, "phase_scramble_init_state", len1) == 0) {
                        if (param2 != NULL)
                                cfg->phase_scramble_init_state = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "phase_scramble_init_state = 0x%X\n\r", cfg->phase_scramble_init_state);
                        return pdFALSE;
                }
                if (strncmp(param1, "phase_scramble_tap", len1) == 0) {
                        if (param2 != NULL)
                                cfg->phase_scramble_tap = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "phase_scramble_tap = 0x%X\n\r", cfg->phase_scramble_tap);
                        return pdFALSE;
                }
                if (strncmp(param1, "phase_scramble_comp", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 4) {
                                cfg->phase_scramble_comp[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "phase_scramble_comp = [");
                        for(cnt = 0; cnt < 4; cnt++) {
                                if (cnt != 4 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->phase_scramble_comp[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->phase_scramble_comp[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "freq_hopping_on", len1) == 0) {
                        if (param2 != NULL)
                                cfg->freq_hopping_on = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "freq_hopping_on = %u\n\r", cfg->freq_hopping_on);
                        return pdFALSE;
                }
                if (strncmp(param1, "freq_hopping_init_state", len1) == 0) {
                        if (param2 != NULL)
                                cfg->freq_hopping_init_state = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "freq_hopping_init_state = 0x%X\n\r", cfg->freq_hopping_init_state);
                        return pdFALSE;
                }
                if (strncmp(param1, "freq_hopping_tap", len1) == 0) {
                        if (param2 != NULL)
                                cfg->freq_hopping_tap = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "freq_hopping_tap = 0x%X\n\r", cfg->freq_hopping_tap);
                        return pdFALSE;
                }
                if (strncmp(param1, "freq_hopping_deltaf", len1) == 0) {
                        if (param2 != NULL)
                                cfg->freq_hopping_deltaf = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "freq_hopping_deltaf = %.4f\n\r", cfg->freq_hopping_deltaf);
                        return pdFALSE;
                }
                if (strncmp(param1, "chirp_shifting_on", len1) == 0) {
                        if (param2 != NULL)
                                cfg->chirp_shifting_on = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "chirp_shifting_on = %u\n\r", cfg->chirp_shifting_on);
                        return pdFALSE;
                }
                if (strncmp(param1, "chirp_shifting_init_state", len1) == 0) {
                        if (param2 != NULL)
                                cfg->chirp_shifting_init_state = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "chirp_shifting_init_state = %u\n\r", cfg->chirp_shifting_init_state);
                        return pdFALSE;
                }
                if (strncmp(param1, "chirp_shifting_init_tap", len1) == 0) {
                        if (param2 != NULL)
                                cfg->chirp_shifting_init_tap = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "chirp_shifting_init_tap = %u\n\r", cfg->chirp_shifting_init_tap);
                        return pdFALSE;
                }
                if (strncmp(param1, "chirp_shifting_delay", len1) == 0) {
                        if (param2 != NULL)
                                cfg->chirp_shifting_delay = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "chirp_shifting_delay = %.4f\n\r", cfg->chirp_shifting_delay);
                        return pdFALSE;
                }
                if (strncmp(param1, "fsm_on", len1) == 0) {
                        if (param2 != NULL)
                                cfg->fsm_on = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fsm_on = %d\n\r", cfg->fsm_on);
                        return pdFALSE;
                }
                if (strncmp(param1, "agc_mode", len1) == 0) {
                        if (param2 != NULL)
                                cfg->agc_mode = (int32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_mode = %d\n\r", cfg->agc_mode);
                        return pdFALSE;
                }
                if (strncmp(param1, "agc_code", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < AGC_CODE_ENTRY_NUM) {
                                cfg->agc_code[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "agc_code = [");
                        for(cnt = 0; cnt < AGC_CODE_ENTRY_NUM; cnt++) {
                                if (cnt != AGC_CODE_ENTRY_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->agc_code[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->agc_code[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "agc_tia_thres", len1) == 0) {
                        if (param2 != NULL)
                                cfg->agc_tia_thres = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_tia_thres = %.4f\n\r", cfg->agc_tia_thres);
                        return pdFALSE;
                }
                if (strncmp(param1, "agc_vga1_thres", len1) == 0) {
                        if (param2 != NULL)
                                cfg->agc_vga1_thres = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_vga1_thres = %.4f\n\r", cfg->agc_vga1_thres);
                        return pdFALSE;
                }
                if (strncmp(param1, "agc_vga2_thres", len1) == 0) {
                        if (param2 != NULL)
                                cfg->agc_vga2_thres = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_vga2_thres = %.4f\n\r", cfg->agc_vga2_thres);
                        return pdFALSE;
                }
                if (strncmp(param1, "agc_align_en", len1) == 0) {
                        if (param2 != NULL)
                                cfg->agc_align_en = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_align_en = %d\n\r", cfg->agc_align_en);
                        return pdFALSE;
                }
                if (strncmp(param1, "adc_comp_en", len1) == 0) {
                        if (param2 != NULL)
                                cfg->adc_comp_en = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "adc_comp_en = %d\n\r", cfg->adc_comp_en);
                        return pdFALSE;
                }
                if (strncmp(param1, "rf_tia_gain", len1) == 0) {
                        if (param2 != NULL)
                                cfg->rf_tia_gain = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_tia_gain = %u\n\r", cfg->rf_tia_gain);
                        return pdFALSE;
                }
                if (strncmp(param1, "rf_vga1_gain", len1) == 0) {
                        if (param2 != NULL)
                                cfg->rf_vga1_gain = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_vga1_gain = %u\n\r", cfg->rf_vga1_gain);
                        return pdFALSE;
                }
                if (strncmp(param1, "rf_vga2_gain", len1) == 0) {
                        if (param2 != NULL)
                                cfg->rf_vga2_gain = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_vga2_gain = %u\n\r", cfg->rf_vga2_gain);
                        return pdFALSE;
                }
                if (strncmp(param1, "rf_hpf1", len1) == 0) {
                        if (param2 != NULL)
                                cfg->rf_hpf1 = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_hpf1 = %u\n\r", cfg->rf_hpf1);
                        return pdFALSE;
                }
                if (strncmp(param1, "rf_hpf2", len1) == 0) {
                        if (param2 != NULL)
                                cfg->rf_hpf2 = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_hpf2 = %u\n\r", cfg->rf_hpf2);
                        return pdFALSE;
                }
                if (strncmp(param1, "de_vel_amb", len1) == 0) {
                        if (param2 != NULL)
                                cfg->de_vel_amb = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "de_vel_amb = %d\n\r", cfg->de_vel_amb);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_fps", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_fps = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fps = %u\n\r", cfg->track_fps);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_fov_az_left", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_fov_az_left = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fov_az_left = %.4f\n\r", cfg->track_fov_az_left);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_fov_az_right", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_fov_az_right = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fov_az_right = %.4f\n\r", cfg->track_fov_az_right);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_fov_ev_down", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_fov_ev_down = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fov_ev_down = %.4f\n\r", cfg->track_fov_ev_down);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_fov_ev_up", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_fov_ev_up = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fov_ev_up = %.4f\n\r", cfg->track_fov_ev_up);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_near_field_thres", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_near_field_thres = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_near_field_thres = %.4f\n\r", cfg->track_near_field_thres);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_capture_delay", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_capture_delay = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_capture_delay = %.4f\n\r", cfg->track_capture_delay);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_drop_delay", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_drop_delay = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_drop_delay = %.4f\n\r", cfg->track_drop_delay);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_vel_pos_ind_portion", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_vel_pos_ind_portion = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_vel_pos_ind_portion = %.4f\n\r", cfg->track_vel_pos_ind_portion);
                        return pdFALSE;
                }
                if (strncmp(param1, "track_obj_snr_sel", len1) == 0) {
                        if (param2 != NULL)
                                cfg->track_obj_snr_sel = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_obj_snr_sel = %u\n\r", cfg->track_obj_snr_sel);
                        return pdFALSE;
                }
                if (strncmp(param1, "tx_phase_value", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 4) {
                                cfg->tx_phase_value[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "tx_phase_value = [");
                        for(cnt = 0; cnt < 4; cnt++) {
                                if (cnt != 4 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->tx_phase_value[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->tx_phase_value[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "spk_en", len1) == 0) {
                        if (param2 != NULL)
                                cfg->spk_en = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_en = %d\n\r", cfg->spk_en);
                        return pdFALSE;
                }
                if (strncmp(param1, "spk_buf_len", len1) == 0) {
                        if (param2 != NULL)
                                cfg->spk_buf_len = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_buf_len = %u\n\r", cfg->spk_buf_len);
                        return pdFALSE;
                }
                if (strncmp(param1, "spk_set_zero", len1) == 0) {
                        if (param2 != NULL)
                                cfg->spk_set_zero = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_set_zero = %d\n\r", cfg->spk_set_zero);
                        return pdFALSE;
                }
                if (strncmp(param1, "spk_ovr_num", len1) == 0) {
                        if (param2 != NULL)
                                cfg->spk_ovr_num = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_ovr_num = %u\n\r", cfg->spk_ovr_num);
                        return pdFALSE;
                }
                if (strncmp(param1, "spk_thres_dbl", len1) == 0) {
                        if (param2 != NULL)
                                cfg->spk_thres_dbl = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_thres_dbl = %d\n\r", cfg->spk_thres_dbl);
                        return pdFALSE;
                }
                if (strncmp(param1, "spk_min_max_sel", len1) == 0) {
                        if (param2 != NULL)
                                cfg->spk_min_max_sel = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_min_max_sel = %d\n\r", cfg->spk_min_max_sel);
                        return pdFALSE;
                }
                if (strncmp(param1, "zero_doppler_cancel", len1) == 0) {
                        if (param2 != NULL)
                                cfg->zero_doppler_cancel = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "zero_doppler_cancel = %d\n\r", cfg->zero_doppler_cancel);
                        return pdFALSE;
                }
                if (strncmp(param1, "anti_velamb_en", len1) == 0) {
                        if (param2 != NULL)
                                cfg->anti_velamb_en = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "anti_velamb_en = %d\n\r", cfg->anti_velamb_en);
                        return pdFALSE;
                }
                if (strncmp(param1, "anti_velamb_delay", len1) == 0) {
                        if (param2 != NULL)
                                cfg->anti_velamb_delay = (uint32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "anti_velamb_delay = %u\n\r", cfg->anti_velamb_delay);
                        return pdFALSE;
                }
                if (strncmp(param1, "anti_velamb_qmin", len1) == 0) {
                        if (param2 != NULL)
                                cfg->anti_velamb_qmin = (int32_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "anti_velamb_qmin = %d\n\r", cfg->anti_velamb_qmin);
                        return pdFALSE;
                }
                if (strncmp(param1, "high_vel_comp_en", len1) == 0) {
                        if (param2 != NULL)
                                cfg->high_vel_comp_en = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "high_vel_comp_en = %d\n\r", cfg->high_vel_comp_en);
                        return pdFALSE;
                }
                if (strncmp(param1, "high_vel_comp_method", len1) == 0) {
                        if (param2 != NULL)
                                cfg->high_vel_comp_method = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "high_vel_comp_method = %u\n\r", cfg->high_vel_comp_method);
                        return pdFALSE;
                }
                if (strncmp(param1, "vel_comp_usr", len1) == 0) {
                        if (param2 != NULL)
                                cfg->vel_comp_usr = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "vel_comp_usr = %.4f\n\r", cfg->vel_comp_usr);
                        return pdFALSE;
                }
                if (strncmp(param1, "rng_comp_usr", len1) == 0) {
                        if (param2 != NULL)
                                cfg->rng_comp_usr = (float) strtof(param2, NULL);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rng_comp_usr = %.4f\n\r", cfg->rng_comp_usr);
                        return pdFALSE;
                }
                if (strncmp(param1, "dml_2dsch_start", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 2) {
                                cfg->dml_2dsch_start[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_2dsch_start = [");
                        for(cnt = 0; cnt < 2; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->dml_2dsch_start[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->dml_2dsch_start[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "dml_2dsch_step", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 2) {
                                cfg->dml_2dsch_step[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_2dsch_step = [");
                        for(cnt = 0; cnt < 2; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->dml_2dsch_step[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->dml_2dsch_step[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "dml_2dsch_end", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 2) {
                                cfg->dml_2dsch_end[cnt++] = (uint32_t) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_2dsch_end = [");
                        for(cnt = 0; cnt < 2; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->dml_2dsch_end[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->dml_2dsch_end[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "dml_extra_1d_en", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 2) {
                                cfg->dml_extra_1d_en[cnt++] = (bool) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_extra_1d_en = [");
                        for(cnt = 0; cnt < 2; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%d, ", cfg->dml_extra_1d_en[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%d]\n\r", cfg->dml_extra_1d_en[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "dml_p1p2_en", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 2) {
                                cfg->dml_p1p2_en[cnt++] = (bool) strtol(param2, NULL, 0);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_p1p2_en = [");
                        for(cnt = 0; cnt < 2; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%d, ", cfg->dml_p1p2_en[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%d]\n\r", cfg->dml_p1p2_en[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "dml_respwr_coef", len1) == 0) {
                        uint32_t cnt = 0;
                        while(param2 != NULL && cnt < 10) {
                                cfg->dml_respwr_coef[cnt++] = (float) strtof(param2, NULL);
                                param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                        }
                        int32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_respwr_coef = [");
                        for(cnt = 0; cnt < 10; cnt++) {
                                if (cnt != 10 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->dml_respwr_coef[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->dml_respwr_coef[cnt]);
                        }
                        
                        return pdFALSE;
                }
                if (strncmp(param1, "acc_rng_hw", len1) == 0) {
                        if (param2 != NULL)
                                cfg->acc_rng_hw = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "acc_rng_hw = %d\n\r", cfg->acc_rng_hw);
                        return pdFALSE;
                }
                if (strncmp(param1, "acc_vel_hw", len1) == 0) {
                        if (param2 != NULL)
                                cfg->acc_vel_hw = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "acc_vel_hw = %d\n\r", cfg->acc_vel_hw);
                        return pdFALSE;
                }
                if (strncmp(param1, "acc_angle_hw", len1) == 0) {
                        if (param2 != NULL)
                                cfg->acc_angle_hw = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "acc_angle_hw = %d\n\r", cfg->acc_angle_hw);
                        return pdFALSE;
                }
                if (strncmp(param1, "cas_obj_merg_typ", len1) == 0) {
                        if (param2 != NULL)
                                cfg->cas_obj_merg_typ = (uint8_t) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cas_obj_merg_typ = %u\n\r", cfg->cas_obj_merg_typ);
                        return pdFALSE;
                }
                if (strncmp(param1, "sv_read_from_flash", len1) == 0) {
                        if (param2 != NULL)
                                cfg->sv_read_from_flash = (bool) strtol(param2, NULL, 0);
                        snprintf(pcWriteBuffer, xWriteBufferLen, "sv_read_from_flash = %d\n\r", cfg->sv_read_from_flash);
                        return pdFALSE;
                }
        } else {
                static int cc = 0;
                cfg = sensor_config_get_config(cc);
                static uint32_t count = 0;
                uint32_t cnt = 0;
                int32_t offset = 0;
                switch(count) {
                case 0 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_startfreq = %.4f\r\n", cfg->fmcw_startfreq);
                        count++;
                        return pdTRUE;
                case 1 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_bandwidth = %.4f\r\n", cfg->fmcw_bandwidth);
                        count++;
                        return pdTRUE;
                case 2 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_chirp_rampup = %.4f\r\n", cfg->fmcw_chirp_rampup);
                        count++;
                        return pdTRUE;
                case 3 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_chirp_down = %.4f\r\n", cfg->fmcw_chirp_down);
                        count++;
                        return pdTRUE;
                case 4 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fmcw_chirp_period = %.4f\r\n", cfg->fmcw_chirp_period);
                        count++;
                        return pdTRUE;
                case 5 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "nchirp = %d\r\n", cfg->nchirp);
                        count++;
                        return pdTRUE;
                case 6 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "adc_freq = %u\r\n", cfg->adc_freq);
                        count++;
                        return pdTRUE;
                case 7 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "dec_factor = %u\r\n", cfg->dec_factor);
                        count++;
                        return pdTRUE;
                case 8 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "adc_sample_start = %.4f\r\n", cfg->adc_sample_start);
                        count++;
                        return pdTRUE;
                case 9 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "adc_sample_end = %.4f\r\n", cfg->adc_sample_end);
                        count++;
                        return pdTRUE;
                case 10 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "tx_groups = [");
                        for(cnt = 0; cnt < MAX_NUM_TX && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != MAX_NUM_TX - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->tx_groups[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->tx_groups[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 11 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rng_win = %s\r\n", cfg->rng_win);
                        count++;
                        return pdTRUE;
                case 12 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "vel_win = %s\r\n", cfg->vel_win);
                        count++;
                        return pdTRUE;
                case 13 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "rng_win_params = [");
                        for(cnt = 0; cnt < 3 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->rng_win_params[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->rng_win_params[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 14 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "vel_win_params = [");
                        for(cnt = 0; cnt < 3 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->vel_win_params[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->vel_win_params[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 15 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rng_nfft = %u\r\n", cfg->rng_nfft);
                        count++;
                        return pdTRUE;
                case 16 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "vel_nfft = %u\r\n", cfg->vel_nfft);
                        count++;
                        return pdTRUE;
                case 17 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rng_fft_scalar = 0x%X\r\n", cfg->rng_fft_scalar);
                        count++;
                        return pdTRUE;
                case 18 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "vel_fft_scalar = 0x%X\r\n", cfg->vel_fft_scalar);
                        count++;
                        return pdTRUE;
                case 19 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fft_nve_bypass = %d\r\n", cfg->fft_nve_bypass);
                        count++;
                        return pdTRUE;
                case 20 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fft_nve_shift = %u\r\n", cfg->fft_nve_shift);
                        count++;
                        return pdTRUE;
                case 21 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fft_nve_ch_mask = 0x%X\r\n", cfg->fft_nve_ch_mask);
                        count++;
                        return pdTRUE;
                case 22 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fft_nve_default_value = %.4f\r\n", cfg->fft_nve_default_value);
                        count++;
                        return pdTRUE;
                case 23 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_pk_en = %u\r\n", cfg->cfar_pk_en);
                        count++;
                        return pdTRUE;
                case 24 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_pk_win_size1 = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_pk_win_size1[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_pk_win_size1[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 25 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_pk_win_size2 = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_pk_win_size2[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_pk_win_size2[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 26 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_pk_threshold = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_pk_threshold[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_pk_threshold[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 27 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_sliding_win = %u\r\n", cfg->cfar_sliding_win);
                        count++;
                        return pdTRUE;
                case 28 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_recwin_decimate = %u\r\n", cfg->cfar_recwin_decimate);
                        count++;
                        return pdTRUE;
                case 29 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_recwin_msk = [");
                        for(cnt = 0; cnt < CFAR_MAX_RECWIN_MSK_LEN && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_RECWIN_MSK_LEN - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_recwin_msk[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_recwin_msk[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 30 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_region_algo_type = %u\r\n", cfg->cfar_region_algo_type);
                        count++;
                        return pdTRUE;
                case 31 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_os_rnk_ratio = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_os_rnk_ratio[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_os_rnk_ratio[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 32 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_os_rnk_sel = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_os_rnk_sel[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_os_rnk_sel[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 33 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_os_tdec = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_os_tdec[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_os_tdec[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 34 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_os_alpha = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_os_alpha[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_os_alpha[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 35 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_combine_dirs = %u\r\n", cfg->cfar_combine_dirs);
                        count++;
                        return pdTRUE;
                case 36 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_combine_thetas = [");
                        for(cnt = 0; cnt < MAX_CFAR_DIRS && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != MAX_CFAR_DIRS - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_combine_thetas[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_combine_thetas[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 37 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_combine_phis = [");
                        for(cnt = 0; cnt < MAX_CFAR_DIRS && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != MAX_CFAR_DIRS - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_combine_phis[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_combine_phis[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 38 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_crswin_rng_size = %u\r\n", cfg->cfar_crswin_rng_size);
                        count++;
                        return pdTRUE;
                case 39 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_crswin_rng_skip = %u\r\n", cfg->cfar_crswin_rng_skip);
                        count++;
                        return pdTRUE;
                case 40 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_crswin_vel_size = %u\r\n", cfg->cfar_crswin_vel_size);
                        count++;
                        return pdTRUE;
                case 41 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_crswin_vel_skip = %u\r\n", cfg->cfar_crswin_vel_skip);
                        count++;
                        return pdTRUE;
                case 42 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_mimo_win = %s\r\n", cfg->cfar_mimo_win);
                        count++;
                        return pdTRUE;
                case 43 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_mimo_win_params = [");
                        for(cnt = 0; cnt < 3 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_mimo_win_params[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_mimo_win_params[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 44 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_noise_type = %u\r\n", cfg->cfar_noise_type);
                        count++;
                        return pdTRUE;
                case 45 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_alpha = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_nr_alpha[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_nr_alpha[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 46 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_beta1 = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_nr_beta1[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_nr_beta1[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 47 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_beta2 = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_nr_beta2[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_nr_beta2[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 48 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_rnk_ratio = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_nr_rnk_ratio[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_nr_rnk_ratio[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 49 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_rnk_sel = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_nr_rnk_sel[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_nr_rnk_sel[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 50 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_scheme_sel = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_nr_scheme_sel[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_nr_scheme_sel[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 51 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_nr_tdec = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_nr_tdec[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_nr_tdec[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 52 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_region_sep_rng = [");
                        for(cnt = 0; cnt < 3 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_region_sep_rng[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_region_sep_rng[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 53 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_region_sep_vel = [");
                        for(cnt = 0; cnt < 8 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 8 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_region_sep_vel[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_region_sep_vel[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 54 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_sogo_alpha = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_sogo_alpha[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_sogo_alpha[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 55 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_sogo_i = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_sogo_i[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_sogo_i[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 56 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_sogo_mask = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "0x%X, ", cfg->cfar_sogo_mask[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "0x%X]\n\r", cfg->cfar_sogo_mask[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 57 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_ca_alpha = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->cfar_ca_alpha[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->cfar_ca_alpha[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 58 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "cfar_ca_n = [");
                        for(cnt = 0; cnt < CFAR_MAX_GRP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != CFAR_MAX_GRP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->cfar_ca_n[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->cfar_ca_n[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 59 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_mode = %u\r\n", cfg->doa_mode);
                        count++;
                        return pdTRUE;
                case 60 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_num_groups = %u\r\n", cfg->doa_num_groups);
                        count++;
                        return pdTRUE;
                case 61 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "doa_fft_mux = [");
                        for(cnt = 0; cnt < MAX_BFM_GROUP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != MAX_BFM_GROUP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->doa_fft_mux[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->doa_fft_mux[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 62 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "combined_doa_fft_mux = [");
                        for(cnt = 0; cnt < 4 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 4 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->combined_doa_fft_mux[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->combined_doa_fft_mux[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 63 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_method = %u\r\n", cfg->doa_method);
                        count++;
                        return pdTRUE;
                case 64 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "doa_npoint = [");
                        for(cnt = 0; cnt < MAX_BFM_GROUP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != MAX_BFM_GROUP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->doa_npoint[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->doa_npoint[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 65 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_samp_space = %c\r\n", cfg->doa_samp_space);
                        count++;
                        return pdTRUE;
                case 66 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "doa_max_obj_per_bin = [");
                        for(cnt = 0; cnt < MAX_BFM_GROUP_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != MAX_BFM_GROUP_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->doa_max_obj_per_bin[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->doa_max_obj_per_bin[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 67 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_peak_scalar = [");
                        for(cnt = 0; cnt < 3 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->bfm_peak_scalar[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->bfm_peak_scalar[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 68 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_noise_level_scalar = [");
                        for(cnt = 0; cnt < 3 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->bfm_noise_level_scalar[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->bfm_noise_level_scalar[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 69 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_snr_thres = [");
                        for(cnt = 0; cnt < 2 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->bfm_snr_thres[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->bfm_snr_thres[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 70 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_az_left = %.4f\r\n", cfg->bfm_az_left);
                        count++;
                        return pdTRUE;
                case 71 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_az_right = %.4f\r\n", cfg->bfm_az_right);
                        count++;
                        return pdTRUE;
                case 72 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_ev_up = %.4f\r\n", cfg->bfm_ev_up);
                        count++;
                        return pdTRUE;
                case 73 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_ev_down = %.4f\r\n", cfg->bfm_ev_down);
                        count++;
                        return pdTRUE;
                case 74 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "doa_win = %s\r\n", cfg->doa_win);
                        count++;
                        return pdTRUE;
                case 75 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "doa_win_params = [");
                        for(cnt = 0; cnt < 3 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 3 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->doa_win_params[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->doa_win_params[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 76 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_raw_search_step = %u\r\n", cfg->bfm_raw_search_step);
                        count++;
                        return pdTRUE;
                case 77 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_fine_search_range = %u\r\n", cfg->bfm_fine_search_range);
                        count++;
                        return pdTRUE;
                case 78 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_iter_search = %d\r\n", cfg->bfm_iter_search);
                        count++;
                        return pdTRUE;
                case 79 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_mode = %u\r\n", cfg->bfm_mode);
                        count++;
                        return pdTRUE;
                case 80 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bfm_group_idx = %u\r\n", cfg->bfm_group_idx);
                        count++;
                        return pdTRUE;
                case 81 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "ant_info_from_flash = %d\r\n", cfg->ant_info_from_flash);
                        count++;
                        return pdTRUE;
                case 82 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "ant_info_flash_addr = 0x%X\r\n", cfg->ant_info_flash_addr);
                        count++;
                        return pdTRUE;
                case 83 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "ant_pos = [");
                        for(cnt = 0; cnt < MAX_ANT_ARRAY_SIZE && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != MAX_ANT_ARRAY_SIZE - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "(%.2f, %.2f), ", cfg->ant_pos[cnt].x, cfg->ant_pos[cnt].y);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "(%.2f, %.2f)]\n\r", cfg->ant_pos[cnt].x, cfg->ant_pos[cnt].y);
                        }
                        count++;
                        return pdTRUE;
                case 84 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "ant_comps = [");
                        for(cnt = 0; cnt < MAX_ANT_ARRAY_SIZE && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != MAX_ANT_ARRAY_SIZE - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->ant_comps[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->ant_comps[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 85 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "bpm_mode = %d\r\n", cfg->bpm_mode);
                        count++;
                        return pdTRUE;
                case 86 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "phase_scramble_on = %u\r\n", cfg->phase_scramble_on);
                        count++;
                        return pdTRUE;
                case 87 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "phase_scramble_init_state = 0x%X\r\n", cfg->phase_scramble_init_state);
                        count++;
                        return pdTRUE;
                case 88 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "phase_scramble_tap = 0x%X\r\n", cfg->phase_scramble_tap);
                        count++;
                        return pdTRUE;
                case 89 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "phase_scramble_comp = [");
                        for(cnt = 0; cnt < 4 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 4 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->phase_scramble_comp[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->phase_scramble_comp[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 90 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "freq_hopping_on = %u\r\n", cfg->freq_hopping_on);
                        count++;
                        return pdTRUE;
                case 91 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "freq_hopping_init_state = 0x%X\r\n", cfg->freq_hopping_init_state);
                        count++;
                        return pdTRUE;
                case 92 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "freq_hopping_tap = 0x%X\r\n", cfg->freq_hopping_tap);
                        count++;
                        return pdTRUE;
                case 93 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "freq_hopping_deltaf = %.4f\r\n", cfg->freq_hopping_deltaf);
                        count++;
                        return pdTRUE;
                case 94 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "chirp_shifting_on = %u\r\n", cfg->chirp_shifting_on);
                        count++;
                        return pdTRUE;
                case 95 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "chirp_shifting_init_state = %u\r\n", cfg->chirp_shifting_init_state);
                        count++;
                        return pdTRUE;
                case 96 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "chirp_shifting_init_tap = %u\r\n", cfg->chirp_shifting_init_tap);
                        count++;
                        return pdTRUE;
                case 97 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "chirp_shifting_delay = %.4f\r\n", cfg->chirp_shifting_delay);
                        count++;
                        return pdTRUE;
                case 98 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "fsm_on = %d\r\n", cfg->fsm_on);
                        count++;
                        return pdTRUE;
                case 99 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_mode = %d\r\n", cfg->agc_mode);
                        count++;
                        return pdTRUE;
                case 100 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "agc_code = [");
                        for(cnt = 0; cnt < AGC_CODE_ENTRY_NUM && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != AGC_CODE_ENTRY_NUM - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->agc_code[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->agc_code[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 101 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_tia_thres = %.4f\r\n", cfg->agc_tia_thres);
                        count++;
                        return pdTRUE;
                case 102 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_vga1_thres = %.4f\r\n", cfg->agc_vga1_thres);
                        count++;
                        return pdTRUE;
                case 103 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_vga2_thres = %.4f\r\n", cfg->agc_vga2_thres);
                        count++;
                        return pdTRUE;
                case 104 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "agc_align_en = %d\r\n", cfg->agc_align_en);
                        count++;
                        return pdTRUE;
                case 105 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "adc_comp_en = %d\r\n", cfg->adc_comp_en);
                        count++;
                        return pdTRUE;
                case 106 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_tia_gain = %u\r\n", cfg->rf_tia_gain);
                        count++;
                        return pdTRUE;
                case 107 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_vga1_gain = %u\r\n", cfg->rf_vga1_gain);
                        count++;
                        return pdTRUE;
                case 108 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_vga2_gain = %u\r\n", cfg->rf_vga2_gain);
                        count++;
                        return pdTRUE;
                case 109 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_hpf1 = %u\r\n", cfg->rf_hpf1);
                        count++;
                        return pdTRUE;
                case 110 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rf_hpf2 = %u\r\n", cfg->rf_hpf2);
                        count++;
                        return pdTRUE;
                case 111 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "de_vel_amb = %d\r\n", cfg->de_vel_amb);
                        count++;
                        return pdTRUE;
                case 112 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fps = %u\r\n", cfg->track_fps);
                        count++;
                        return pdTRUE;
                case 113 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fov_az_left = %.4f\r\n", cfg->track_fov_az_left);
                        count++;
                        return pdTRUE;
                case 114 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fov_az_right = %.4f\r\n", cfg->track_fov_az_right);
                        count++;
                        return pdTRUE;
                case 115 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fov_ev_down = %.4f\r\n", cfg->track_fov_ev_down);
                        count++;
                        return pdTRUE;
                case 116 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_fov_ev_up = %.4f\r\n", cfg->track_fov_ev_up);
                        count++;
                        return pdTRUE;
                case 117 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_near_field_thres = %.4f\r\n", cfg->track_near_field_thres);
                        count++;
                        return pdTRUE;
                case 118 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_capture_delay = %.4f\r\n", cfg->track_capture_delay);
                        count++;
                        return pdTRUE;
                case 119 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_drop_delay = %.4f\r\n", cfg->track_drop_delay);
                        count++;
                        return pdTRUE;
                case 120 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_vel_pos_ind_portion = %.4f\r\n", cfg->track_vel_pos_ind_portion);
                        count++;
                        return pdTRUE;
                case 121 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "track_obj_snr_sel = %u\r\n", cfg->track_obj_snr_sel);
                        count++;
                        return pdTRUE;
                case 122 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "tx_phase_value = [");
                        for(cnt = 0; cnt < 4 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 4 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->tx_phase_value[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->tx_phase_value[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 123 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_en = %d\r\n", cfg->spk_en);
                        count++;
                        return pdTRUE;
                case 124 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_buf_len = %u\r\n", cfg->spk_buf_len);
                        count++;
                        return pdTRUE;
                case 125 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_set_zero = %d\r\n", cfg->spk_set_zero);
                        count++;
                        return pdTRUE;
                case 126 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_ovr_num = %u\r\n", cfg->spk_ovr_num);
                        count++;
                        return pdTRUE;
                case 127 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_thres_dbl = %d\r\n", cfg->spk_thres_dbl);
                        count++;
                        return pdTRUE;
                case 128 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "spk_min_max_sel = %d\r\n", cfg->spk_min_max_sel);
                        count++;
                        return pdTRUE;
                case 129 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "zero_doppler_cancel = %d\r\n", cfg->zero_doppler_cancel);
                        count++;
                        return pdTRUE;
                case 130 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "anti_velamb_en = %d\r\n", cfg->anti_velamb_en);
                        count++;
                        return pdTRUE;
                case 131 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "anti_velamb_delay = %u\r\n", cfg->anti_velamb_delay);
                        count++;
                        return pdTRUE;
                case 132 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "anti_velamb_qmin = %d\r\n", cfg->anti_velamb_qmin);
                        count++;
                        return pdTRUE;
                case 133 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "high_vel_comp_en = %d\r\n", cfg->high_vel_comp_en);
                        count++;
                        return pdTRUE;
                case 134 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "high_vel_comp_method = %u\r\n", cfg->high_vel_comp_method);
                        count++;
                        return pdTRUE;
                case 135 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "vel_comp_usr = %.4f\r\n", cfg->vel_comp_usr);
                        count++;
                        return pdTRUE;
                case 136 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "rng_comp_usr = %.4f\r\n", cfg->rng_comp_usr);
                        count++;
                        return pdTRUE;
                case 137 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_2dsch_start = [");
                        for(cnt = 0; cnt < 2 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->dml_2dsch_start[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->dml_2dsch_start[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 138 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_2dsch_step = [");
                        for(cnt = 0; cnt < 2 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->dml_2dsch_step[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->dml_2dsch_step[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 139 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_2dsch_end = [");
                        for(cnt = 0; cnt < 2 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u, ", cfg->dml_2dsch_end[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%u]\n\r", cfg->dml_2dsch_end[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 140 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_extra_1d_en = [");
                        for(cnt = 0; cnt < 2 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%d, ", cfg->dml_extra_1d_en[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%d]\n\r", cfg->dml_extra_1d_en[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 141 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_p1p2_en = [");
                        for(cnt = 0; cnt < 2 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 2 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%d, ", cfg->dml_p1p2_en[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%d]\n\r", cfg->dml_p1p2_en[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 142 :
                        offset = snprintf(pcWriteBuffer, xWriteBufferLen, "dml_respwr_coef = [");
                        for(cnt = 0; cnt < 10 && offset < cmdMAX_OUTPUT_SIZE; cnt++) {
                                if (cnt != 10 - 1)
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f, ", cfg->dml_respwr_coef[cnt]);
                                else
                                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%.4f]\n\r", cfg->dml_respwr_coef[cnt]);
                        }
                        count++;
                        return pdTRUE;
                case 143 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "acc_rng_hw = %d\r\n", cfg->acc_rng_hw);
                        count++;
                        return pdTRUE;
                case 144 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "acc_vel_hw = %d\r\n", cfg->acc_vel_hw);
                        count++;
                        return pdTRUE;
                case 145 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "acc_angle_hw = %d\r\n", cfg->acc_angle_hw);
                        count++;
                        return pdTRUE;
                case 146 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "cas_obj_merg_typ = %u\r\n", cfg->cas_obj_merg_typ);
                        count++;
                        return pdTRUE;
                case 147 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "sv_read_from_flash = %d\r\n", cfg->sv_read_from_flash);
                        count++;
                        return pdTRUE;
                default :
                        count = 0;
                        snprintf(pcWriteBuffer, xWriteBufferLen, "-----------CFG-EOF %d----------\r\n", cc);
                        if (cc == NUM_FRAME_TYPE-1) {
                                cc = 0;
                                return pdFALSE;        
                        } else {
                                cc++;
                                return pdTRUE;
                        }
                }
        }
        return pdFALSE;
        