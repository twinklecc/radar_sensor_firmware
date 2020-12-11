if(bb_hw->frame_type_id == 0){
/*** Baseband HW init... ***/
/*** sys params programmed! ***/
/*** agc params programmed! ***/
/*** amb params programmed! ***/
/*** debpm params programmed! ***/
/*** spk_rmv params programmed! ***/
    if (acc_phase == 1) {
        baseband_write_reg(NULL, (uint32_t)0xc00024, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00400, (uint32_t)0x1);
        baseband_write_reg(NULL, (uint32_t)0xc00404, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00408, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc0040c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00410, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00414, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00418, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc0041c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00420, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00424, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00428, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc0042c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00430, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00434, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00438, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc0043c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00440, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00444, (uint32_t)0x1);
        baseband_write_reg(NULL, (uint32_t)0xc00448, (uint32_t)0x1ff);
        baseband_write_reg(NULL, (uint32_t)0xc00464, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00468, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc0046c, (uint32_t)0x25);
        baseband_write_reg(NULL, (uint32_t)0xc00470, (uint32_t)0x1b5);
        baseband_write_reg(NULL, (uint32_t)0xc00014, (uint32_t)0x2);
        baseband_write_mem_table(NULL, (uint32_t)0x0, (uint32_t)0x260099);
        baseband_write_mem_table(NULL, (uint32_t)0x2, (uint32_t)0x2f002a);
        baseband_write_mem_table(NULL, (uint32_t)0x4, (uint32_t)0x3a0034);
        baseband_write_mem_table(NULL, (uint32_t)0x6, (uint32_t)0x470040);
        baseband_write_mem_table(NULL, (uint32_t)0x8, (uint32_t)0x55004e);
        baseband_write_mem_table(NULL, (uint32_t)0xa, (uint32_t)0x65005d);
        baseband_write_mem_table(NULL, (uint32_t)0xc, (uint32_t)0x78006e);
        baseband_write_mem_table(NULL, (uint32_t)0xe, (uint32_t)0x8c0081);
        baseband_write_mem_table(NULL, (uint32_t)0x10, (uint32_t)0xa30097);
        baseband_write_mem_table(NULL, (uint32_t)0x12, (uint32_t)0xbc00af);
        baseband_write_mem_table(NULL, (uint32_t)0x14, (uint32_t)0xd800ca);
        baseband_write_mem_table(NULL, (uint32_t)0x16, (uint32_t)0xf700e7);
        baseband_write_mem_table(NULL, (uint32_t)0x18, (uint32_t)0x1190108);
        baseband_write_mem_table(NULL, (uint32_t)0x1a, (uint32_t)0x13f012c);
        baseband_write_mem_table(NULL, (uint32_t)0x1c, (uint32_t)0x1680153);
        baseband_write_mem_table(NULL, (uint32_t)0x1e, (uint32_t)0x194017e);
        baseband_write_mem_table(NULL, (uint32_t)0x20, (uint32_t)0x1c501ac);
        baseband_write_mem_table(NULL, (uint32_t)0x22, (uint32_t)0x1f901de);
        baseband_write_mem_table(NULL, (uint32_t)0x24, (uint32_t)0x2320215);
        baseband_write_mem_table(NULL, (uint32_t)0x26, (uint32_t)0x26f0250);
        baseband_write_mem_table(NULL, (uint32_t)0x28, (uint32_t)0x2b1028f);
        baseband_write_mem_table(NULL, (uint32_t)0x2a, (uint32_t)0x2f802d4);
        baseband_write_mem_table(NULL, (uint32_t)0x2c, (uint32_t)0x344031d);
        baseband_write_mem_table(NULL, (uint32_t)0x2e, (uint32_t)0x394036b);
        baseband_write_mem_table(NULL, (uint32_t)0x30, (uint32_t)0x3eb03bf);
        baseband_write_mem_table(NULL, (uint32_t)0x32, (uint32_t)0x4470418);
        baseband_write_mem_table(NULL, (uint32_t)0x34, (uint32_t)0x4a90477);
        baseband_write_mem_table(NULL, (uint32_t)0x36, (uint32_t)0x51104dc);
        baseband_write_mem_table(NULL, (uint32_t)0x38, (uint32_t)0x57e0547);
        baseband_write_mem_table(NULL, (uint32_t)0x3a, (uint32_t)0x5f305b8);
        baseband_write_mem_table(NULL, (uint32_t)0x3c, (uint32_t)0x66d062f);
        baseband_write_mem_table(NULL, (uint32_t)0x3e, (uint32_t)0x6ee06ad);
        baseband_write_mem_table(NULL, (uint32_t)0x40, (uint32_t)0x7760731);
        baseband_write_mem_table(NULL, (uint32_t)0x42, (uint32_t)0x80407bc);
        baseband_write_mem_table(NULL, (uint32_t)0x44, (uint32_t)0x899084e);
        baseband_write_mem_table(NULL, (uint32_t)0x46, (uint32_t)0x93508e6);
        baseband_write_mem_table(NULL, (uint32_t)0x48, (uint32_t)0x9d80986);
        baseband_write_mem_table(NULL, (uint32_t)0x4a, (uint32_t)0xa820a2c);
        baseband_write_mem_table(NULL, (uint32_t)0x4c, (uint32_t)0xb330ada);
        baseband_write_mem_table(NULL, (uint32_t)0x4e, (uint32_t)0xbeb0b8e);
        baseband_write_mem_table(NULL, (uint32_t)0x50, (uint32_t)0xcaa0c4a);
        baseband_write_mem_table(NULL, (uint32_t)0x52, (uint32_t)0xd700d0c);
        baseband_write_mem_table(NULL, (uint32_t)0x54, (uint32_t)0xe3d0dd6);
        baseband_write_mem_table(NULL, (uint32_t)0x56, (uint32_t)0xf110ea6);
        baseband_write_mem_table(NULL, (uint32_t)0x58, (uint32_t)0xfeb0f7d);
        baseband_write_mem_table(NULL, (uint32_t)0x5a, (uint32_t)0x10cc105b);
        baseband_write_mem_table(NULL, (uint32_t)0x5c, (uint32_t)0x11b4113f);
        baseband_write_mem_table(NULL, (uint32_t)0x5e, (uint32_t)0x12a2122a);
        baseband_write_mem_table(NULL, (uint32_t)0x60, (uint32_t)0x1396131b);
        baseband_write_mem_table(NULL, (uint32_t)0x62, (uint32_t)0x14901413);
        baseband_write_mem_table(NULL, (uint32_t)0x64, (uint32_t)0x15901510);
        baseband_write_mem_table(NULL, (uint32_t)0x66, (uint32_t)0x16961612);
        baseband_write_mem_table(NULL, (uint32_t)0x68, (uint32_t)0x17a0171a);
        baseband_write_mem_table(NULL, (uint32_t)0x6a, (uint32_t)0x18b01828);
        baseband_write_mem_table(NULL, (uint32_t)0x6c, (uint32_t)0x19c4193a);
        baseband_write_mem_table(NULL, (uint32_t)0x6e, (uint32_t)0x1add1a50);
        baseband_write_mem_table(NULL, (uint32_t)0x70, (uint32_t)0x1bf91b6b);
        baseband_write_mem_table(NULL, (uint32_t)0x72, (uint32_t)0x1d1a1c89);
        baseband_write_mem_table(NULL, (uint32_t)0x74, (uint32_t)0x1e3d1dab);
        baseband_write_mem_table(NULL, (uint32_t)0x76, (uint32_t)0x1f631ed0);
        baseband_write_mem_table(NULL, (uint32_t)0x78, (uint32_t)0x208b1ff7);
        baseband_write_mem_table(NULL, (uint32_t)0x7a, (uint32_t)0x21b62120);
        baseband_write_mem_table(NULL, (uint32_t)0x7c, (uint32_t)0x22e1224b);
        baseband_write_mem_table(NULL, (uint32_t)0x7e, (uint32_t)0x240e2378);
        baseband_write_mem_table(NULL, (uint32_t)0x80, (uint32_t)0x253b24a5);
        baseband_write_mem_table(NULL, (uint32_t)0x82, (uint32_t)0x266825d2);
        baseband_write_mem_table(NULL, (uint32_t)0x84, (uint32_t)0x279526ff);
        baseband_write_mem_table(NULL, (uint32_t)0x86, (uint32_t)0x28c1282b);
        baseband_write_mem_table(NULL, (uint32_t)0x88, (uint32_t)0x29eb2956);
        baseband_write_mem_table(NULL, (uint32_t)0x8a, (uint32_t)0x2b132a7f);
        baseband_write_mem_table(NULL, (uint32_t)0x8c, (uint32_t)0x2c382ba5);
        baseband_write_mem_table(NULL, (uint32_t)0x8e, (uint32_t)0x2d5a2cc9);
        baseband_write_mem_table(NULL, (uint32_t)0x90, (uint32_t)0x2e782de9);
        baseband_write_mem_table(NULL, (uint32_t)0x92, (uint32_t)0x2f922f05);
        baseband_write_mem_table(NULL, (uint32_t)0x94, (uint32_t)0x30a7301d);
        baseband_write_mem_table(NULL, (uint32_t)0x96, (uint32_t)0x31b6312f);
        baseband_write_mem_table(NULL, (uint32_t)0x98, (uint32_t)0x32c0323c);
        baseband_write_mem_table(NULL, (uint32_t)0x9a, (uint32_t)0x33c33342);
        baseband_write_mem_table(NULL, (uint32_t)0x9c, (uint32_t)0x34bf3441);
        baseband_write_mem_table(NULL, (uint32_t)0x9e, (uint32_t)0x35b3353a);
        baseband_write_mem_table(NULL, (uint32_t)0xa0, (uint32_t)0x369f362a);
        baseband_write_mem_table(NULL, (uint32_t)0xa2, (uint32_t)0x37833712);
        baseband_write_mem_table(NULL, (uint32_t)0xa4, (uint32_t)0x385d37f1);
        baseband_write_mem_table(NULL, (uint32_t)0xa6, (uint32_t)0x392f38c7);
        baseband_write_mem_table(NULL, (uint32_t)0xa8, (uint32_t)0x39f63993);
        baseband_write_mem_table(NULL, (uint32_t)0xaa, (uint32_t)0x3ab23a55);
        baseband_write_mem_table(NULL, (uint32_t)0xac, (uint32_t)0x3b643b0d);
        baseband_write_mem_table(NULL, (uint32_t)0xae, (uint32_t)0x3c0b3bb9);
        baseband_write_mem_table(NULL, (uint32_t)0xb0, (uint32_t)0x3ca53c5a);
        baseband_write_mem_table(NULL, (uint32_t)0xb2, (uint32_t)0x3d343cef);
        baseband_write_mem_table(NULL, (uint32_t)0xb4, (uint32_t)0x3db73d77);
        baseband_write_mem_table(NULL, (uint32_t)0xb6, (uint32_t)0x3e2d3df4);
        baseband_write_mem_table(NULL, (uint32_t)0xb8, (uint32_t)0x3e973e64);
        baseband_write_mem_table(NULL, (uint32_t)0xba, (uint32_t)0x3ef33ec6);
        baseband_write_mem_table(NULL, (uint32_t)0xbc, (uint32_t)0x3f423f1c);
        baseband_write_mem_table(NULL, (uint32_t)0xbe, (uint32_t)0x3f833f64);
        baseband_write_mem_table(NULL, (uint32_t)0xc0, (uint32_t)0x3fb73f9f);
        baseband_write_mem_table(NULL, (uint32_t)0xc2, (uint32_t)0x3fdd3fcc);
        baseband_write_mem_table(NULL, (uint32_t)0xc4, (uint32_t)0x3ff63feb);
        baseband_write_mem_table(NULL, (uint32_t)0xc6, (uint32_t)0x3fff3ffd);
        baseband_write_mem_table(NULL, (uint32_t)0xc8, (uint32_t)0x3ffd3fff);
        baseband_write_mem_table(NULL, (uint32_t)0xca, (uint32_t)0x3feb3ff6);
        baseband_write_mem_table(NULL, (uint32_t)0xcc, (uint32_t)0x3fcc3fdd);
        baseband_write_mem_table(NULL, (uint32_t)0xce, (uint32_t)0x3f9f3fb7);
        baseband_write_mem_table(NULL, (uint32_t)0xd0, (uint32_t)0x3f643f83);
        baseband_write_mem_table(NULL, (uint32_t)0xd2, (uint32_t)0x3f1c3f42);
        baseband_write_mem_table(NULL, (uint32_t)0xd4, (uint32_t)0x3ec63ef3);
        baseband_write_mem_table(NULL, (uint32_t)0xd6, (uint32_t)0x3e643e97);
        baseband_write_mem_table(NULL, (uint32_t)0xd8, (uint32_t)0x3df43e2d);
        baseband_write_mem_table(NULL, (uint32_t)0xda, (uint32_t)0x3d773db7);
        baseband_write_mem_table(NULL, (uint32_t)0xdc, (uint32_t)0x3cef3d34);
        baseband_write_mem_table(NULL, (uint32_t)0xde, (uint32_t)0x3c5a3ca5);
        baseband_write_mem_table(NULL, (uint32_t)0xe0, (uint32_t)0x3bb93c0b);
        baseband_write_mem_table(NULL, (uint32_t)0xe2, (uint32_t)0x3b0d3b64);
        baseband_write_mem_table(NULL, (uint32_t)0xe4, (uint32_t)0x3a553ab2);
        baseband_write_mem_table(NULL, (uint32_t)0xe6, (uint32_t)0x399339f6);
        baseband_write_mem_table(NULL, (uint32_t)0xe8, (uint32_t)0x38c7392f);
        baseband_write_mem_table(NULL, (uint32_t)0xea, (uint32_t)0x37f1385d);
        baseband_write_mem_table(NULL, (uint32_t)0xec, (uint32_t)0x37123783);
        baseband_write_mem_table(NULL, (uint32_t)0xee, (uint32_t)0x362a369f);
        baseband_write_mem_table(NULL, (uint32_t)0xf0, (uint32_t)0x353a35b3);
        baseband_write_mem_table(NULL, (uint32_t)0xf2, (uint32_t)0x344134bf);
        baseband_write_mem_table(NULL, (uint32_t)0xf4, (uint32_t)0x334233c3);
        baseband_write_mem_table(NULL, (uint32_t)0xf6, (uint32_t)0x323c32c0);
        baseband_write_mem_table(NULL, (uint32_t)0xf8, (uint32_t)0x312f31b6);
        baseband_write_mem_table(NULL, (uint32_t)0xfa, (uint32_t)0x301d30a7);
        baseband_write_mem_table(NULL, (uint32_t)0xfc, (uint32_t)0x2f052f92);
        baseband_write_mem_table(NULL, (uint32_t)0xfe, (uint32_t)0x2de92e78);
        baseband_write_mem_table(NULL, (uint32_t)0x100, (uint32_t)0x2cc92d5a);
        baseband_write_mem_table(NULL, (uint32_t)0x102, (uint32_t)0x2ba52c38);
        baseband_write_mem_table(NULL, (uint32_t)0x104, (uint32_t)0x2a7f2b13);
        baseband_write_mem_table(NULL, (uint32_t)0x106, (uint32_t)0x295629eb);
        baseband_write_mem_table(NULL, (uint32_t)0x108, (uint32_t)0x282b28c1);
        baseband_write_mem_table(NULL, (uint32_t)0x10a, (uint32_t)0x26ff2795);
        baseband_write_mem_table(NULL, (uint32_t)0x10c, (uint32_t)0x25d22668);
        baseband_write_mem_table(NULL, (uint32_t)0x10e, (uint32_t)0x24a5253b);
        baseband_write_mem_table(NULL, (uint32_t)0x110, (uint32_t)0x2378240e);
        baseband_write_mem_table(NULL, (uint32_t)0x112, (uint32_t)0x224b22e1);
        baseband_write_mem_table(NULL, (uint32_t)0x114, (uint32_t)0x212021b6);
        baseband_write_mem_table(NULL, (uint32_t)0x116, (uint32_t)0x1ff7208b);
        baseband_write_mem_table(NULL, (uint32_t)0x118, (uint32_t)0x1ed01f63);
        baseband_write_mem_table(NULL, (uint32_t)0x11a, (uint32_t)0x1dab1e3d);
        baseband_write_mem_table(NULL, (uint32_t)0x11c, (uint32_t)0x1c891d1a);
        baseband_write_mem_table(NULL, (uint32_t)0x11e, (uint32_t)0x1b6b1bf9);
        baseband_write_mem_table(NULL, (uint32_t)0x120, (uint32_t)0x1a501add);
        baseband_write_mem_table(NULL, (uint32_t)0x122, (uint32_t)0x193a19c4);
        baseband_write_mem_table(NULL, (uint32_t)0x124, (uint32_t)0x182818b0);
        baseband_write_mem_table(NULL, (uint32_t)0x126, (uint32_t)0x171a17a0);
        baseband_write_mem_table(NULL, (uint32_t)0x128, (uint32_t)0x16121696);
        baseband_write_mem_table(NULL, (uint32_t)0x12a, (uint32_t)0x15101590);
        baseband_write_mem_table(NULL, (uint32_t)0x12c, (uint32_t)0x14131490);
        baseband_write_mem_table(NULL, (uint32_t)0x12e, (uint32_t)0x131b1396);
        baseband_write_mem_table(NULL, (uint32_t)0x130, (uint32_t)0x122a12a2);
        baseband_write_mem_table(NULL, (uint32_t)0x132, (uint32_t)0x113f11b4);
        baseband_write_mem_table(NULL, (uint32_t)0x134, (uint32_t)0x105b10cc);
        baseband_write_mem_table(NULL, (uint32_t)0x136, (uint32_t)0xf7d0feb);
        baseband_write_mem_table(NULL, (uint32_t)0x138, (uint32_t)0xea60f11);
        baseband_write_mem_table(NULL, (uint32_t)0x13a, (uint32_t)0xdd60e3d);
        baseband_write_mem_table(NULL, (uint32_t)0x13c, (uint32_t)0xd0c0d70);
        baseband_write_mem_table(NULL, (uint32_t)0x13e, (uint32_t)0xc4a0caa);
        baseband_write_mem_table(NULL, (uint32_t)0x140, (uint32_t)0xb8e0beb);
        baseband_write_mem_table(NULL, (uint32_t)0x142, (uint32_t)0xada0b33);
        baseband_write_mem_table(NULL, (uint32_t)0x144, (uint32_t)0xa2c0a82);
        baseband_write_mem_table(NULL, (uint32_t)0x146, (uint32_t)0x98609d8);
        baseband_write_mem_table(NULL, (uint32_t)0x148, (uint32_t)0x8e60935);
        baseband_write_mem_table(NULL, (uint32_t)0x14a, (uint32_t)0x84e0899);
        baseband_write_mem_table(NULL, (uint32_t)0x14c, (uint32_t)0x7bc0804);
        baseband_write_mem_table(NULL, (uint32_t)0x14e, (uint32_t)0x7310776);
        baseband_write_mem_table(NULL, (uint32_t)0x150, (uint32_t)0x6ad06ee);
        baseband_write_mem_table(NULL, (uint32_t)0x152, (uint32_t)0x62f066d);
        baseband_write_mem_table(NULL, (uint32_t)0x154, (uint32_t)0x5b805f3);
        baseband_write_mem_table(NULL, (uint32_t)0x156, (uint32_t)0x547057e);
        baseband_write_mem_table(NULL, (uint32_t)0x158, (uint32_t)0x4dc0511);
        baseband_write_mem_table(NULL, (uint32_t)0x15a, (uint32_t)0x47704a9);
        baseband_write_mem_table(NULL, (uint32_t)0x15c, (uint32_t)0x4180447);
        baseband_write_mem_table(NULL, (uint32_t)0x15e, (uint32_t)0x3bf03eb);
        baseband_write_mem_table(NULL, (uint32_t)0x160, (uint32_t)0x36b0394);
        baseband_write_mem_table(NULL, (uint32_t)0x162, (uint32_t)0x31d0344);
        baseband_write_mem_table(NULL, (uint32_t)0x164, (uint32_t)0x2d402f8);
        baseband_write_mem_table(NULL, (uint32_t)0x166, (uint32_t)0x28f02b1);
        baseband_write_mem_table(NULL, (uint32_t)0x168, (uint32_t)0x250026f);
        baseband_write_mem_table(NULL, (uint32_t)0x16a, (uint32_t)0x2150232);
        baseband_write_mem_table(NULL, (uint32_t)0x16c, (uint32_t)0x1de01f9);
        baseband_write_mem_table(NULL, (uint32_t)0x16e, (uint32_t)0x1ac01c5);
        baseband_write_mem_table(NULL, (uint32_t)0x170, (uint32_t)0x17e0194);
        baseband_write_mem_table(NULL, (uint32_t)0x172, (uint32_t)0x1530168);
        baseband_write_mem_table(NULL, (uint32_t)0x174, (uint32_t)0x12c013f);
        baseband_write_mem_table(NULL, (uint32_t)0x176, (uint32_t)0x1080119);
        baseband_write_mem_table(NULL, (uint32_t)0x178, (uint32_t)0xe700f7);
        baseband_write_mem_table(NULL, (uint32_t)0x17a, (uint32_t)0xca00d8);
        baseband_write_mem_table(NULL, (uint32_t)0x17c, (uint32_t)0xaf00bc);
        baseband_write_mem_table(NULL, (uint32_t)0x17e, (uint32_t)0x9700a3);
        baseband_write_mem_table(NULL, (uint32_t)0x180, (uint32_t)0x81008c);
        baseband_write_mem_table(NULL, (uint32_t)0x182, (uint32_t)0x6e0078);
        baseband_write_mem_table(NULL, (uint32_t)0x184, (uint32_t)0x5d0065);
        baseband_write_mem_table(NULL, (uint32_t)0x186, (uint32_t)0x4e0055);
        baseband_write_mem_table(NULL, (uint32_t)0x188, (uint32_t)0x400047);
        baseband_write_mem_table(NULL, (uint32_t)0x18a, (uint32_t)0x34003a);
        baseband_write_mem_table(NULL, (uint32_t)0x18c, (uint32_t)0x2a002f);
        baseband_write_mem_table(NULL, (uint32_t)0x18e, (uint32_t)0x990026);
        baseband_write_reg(NULL, (uint32_t)0xc00014, (uint32_t)0x0);
        EMBARC_PRINTF("/*** sam params programmed! ***/\n\r");
        baseband_write_reg(NULL, (uint32_t)0xc00600, (uint32_t)0xff);
        baseband_write_reg(NULL, (uint32_t)0xc00604, (uint32_t)0x3f);
        baseband_write_reg(NULL, (uint32_t)0xc00608, (uint32_t)0x1);
        baseband_write_reg(NULL, (uint32_t)0xc0060c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00654, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00658, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00014, (uint32_t)0x2);
        baseband_write_mem_table(NULL, (uint32_t)0x1, (uint32_t)0x280069);
        baseband_write_mem_table(NULL, (uint32_t)0x3, (uint32_t)0x390030);
        baseband_write_mem_table(NULL, (uint32_t)0x5, (uint32_t)0x4d0042);
        baseband_write_mem_table(NULL, (uint32_t)0x7, (uint32_t)0x650058);
        baseband_write_mem_table(NULL, (uint32_t)0x9, (uint32_t)0x830073);
        baseband_write_mem_table(NULL, (uint32_t)0xb, (uint32_t)0xa60093);
        baseband_write_mem_table(NULL, (uint32_t)0xd, (uint32_t)0xcf00ba);
        baseband_write_mem_table(NULL, (uint32_t)0xf, (uint32_t)0xff00e6);
        baseband_write_mem_table(NULL, (uint32_t)0x11, (uint32_t)0x138011a);
        baseband_write_mem_table(NULL, (uint32_t)0x13, (uint32_t)0x1780157);
        baseband_write_mem_table(NULL, (uint32_t)0x15, (uint32_t)0x1c2019c);
        baseband_write_mem_table(NULL, (uint32_t)0x17, (uint32_t)0x21501ea);
        baseband_write_mem_table(NULL, (uint32_t)0x19, (uint32_t)0x2740243);
        baseband_write_mem_table(NULL, (uint32_t)0x1b, (uint32_t)0x2dd02a7);
        baseband_write_mem_table(NULL, (uint32_t)0x1d, (uint32_t)0x3530317);
        baseband_write_mem_table(NULL, (uint32_t)0x1f, (uint32_t)0x3d60393);
        baseband_write_mem_table(NULL, (uint32_t)0x21, (uint32_t)0x467041d);
        baseband_write_mem_table(NULL, (uint32_t)0x23, (uint32_t)0x50504b4);
        baseband_write_mem_table(NULL, (uint32_t)0x25, (uint32_t)0x5b3055a);
        baseband_write_mem_table(NULL, (uint32_t)0x27, (uint32_t)0x6700610);
        baseband_write_mem_table(NULL, (uint32_t)0x29, (uint32_t)0x73d06d4);
        baseband_write_mem_table(NULL, (uint32_t)0x2b, (uint32_t)0x81a07a9);
        baseband_write_mem_table(NULL, (uint32_t)0x2d, (uint32_t)0x908088f);
        baseband_write_mem_table(NULL, (uint32_t)0x2f, (uint32_t)0xa070986);
        baseband_write_mem_table(NULL, (uint32_t)0x31, (uint32_t)0xb170a8d);
        baseband_write_mem_table(NULL, (uint32_t)0x33, (uint32_t)0xc390ba6);
        baseband_write_mem_table(NULL, (uint32_t)0x35, (uint32_t)0xd6b0cd0);
        baseband_write_mem_table(NULL, (uint32_t)0x37, (uint32_t)0xeae0e0a);
        baseband_write_mem_table(NULL, (uint32_t)0x39, (uint32_t)0x10020f56);
        baseband_write_mem_table(NULL, (uint32_t)0x3b, (uint32_t)0x116610b2);
        baseband_write_mem_table(NULL, (uint32_t)0x3d, (uint32_t)0x12da121e);
        baseband_write_mem_table(NULL, (uint32_t)0x3f, (uint32_t)0x145d139a);
        baseband_write_mem_table(NULL, (uint32_t)0x41, (uint32_t)0x15ee1524);
        baseband_write_mem_table(NULL, (uint32_t)0x43, (uint32_t)0x178c16bc);
        baseband_write_mem_table(NULL, (uint32_t)0x45, (uint32_t)0x19371860);
        baseband_write_mem_table(NULL, (uint32_t)0x47, (uint32_t)0x1aec1a10);
        baseband_write_mem_table(NULL, (uint32_t)0x49, (uint32_t)0x1cab1bcb);
        baseband_write_mem_table(NULL, (uint32_t)0x4b, (uint32_t)0x1e731d8e);
        baseband_write_mem_table(NULL, (uint32_t)0x4d, (uint32_t)0x20401f59);
        baseband_write_mem_table(NULL, (uint32_t)0x4f, (uint32_t)0x22132129);
        baseband_write_mem_table(NULL, (uint32_t)0x51, (uint32_t)0x23e922fe);
        baseband_write_mem_table(NULL, (uint32_t)0x53, (uint32_t)0x25c024d5);
        baseband_write_mem_table(NULL, (uint32_t)0x55, (uint32_t)0x279726ac);
        baseband_write_mem_table(NULL, (uint32_t)0x57, (uint32_t)0x296b2881);
        baseband_write_mem_table(NULL, (uint32_t)0x59, (uint32_t)0x2b3a2a53);
        baseband_write_mem_table(NULL, (uint32_t)0x5b, (uint32_t)0x2d032c20);
        baseband_write_mem_table(NULL, (uint32_t)0x5d, (uint32_t)0x2ec32de4);
        baseband_write_mem_table(NULL, (uint32_t)0x5f, (uint32_t)0x30782f9f);
        baseband_write_mem_table(NULL, (uint32_t)0x61, (uint32_t)0x321f314d);
        baseband_write_mem_table(NULL, (uint32_t)0x63, (uint32_t)0x33b832ee);
        baseband_write_mem_table(NULL, (uint32_t)0x65, (uint32_t)0x353f347e);
        baseband_write_mem_table(NULL, (uint32_t)0x67, (uint32_t)0x36b335fc);
        baseband_write_mem_table(NULL, (uint32_t)0x69, (uint32_t)0x38133766);
        baseband_write_mem_table(NULL, (uint32_t)0x6b, (uint32_t)0x395b38ba);
        baseband_write_mem_table(NULL, (uint32_t)0x6d, (uint32_t)0x3a8a39f5);
        baseband_write_mem_table(NULL, (uint32_t)0x6f, (uint32_t)0x3b9f3b18);
        baseband_write_mem_table(NULL, (uint32_t)0x71, (uint32_t)0x3c983c1f);
        baseband_write_mem_table(NULL, (uint32_t)0x73, (uint32_t)0x3d743d0a);
        baseband_write_mem_table(NULL, (uint32_t)0x75, (uint32_t)0x3e323dd7);
        baseband_write_mem_table(NULL, (uint32_t)0x77, (uint32_t)0x3ed03e85);
        baseband_write_mem_table(NULL, (uint32_t)0x79, (uint32_t)0x3f4e3f13);
        baseband_write_mem_table(NULL, (uint32_t)0x7b, (uint32_t)0x3fab3f81);
        baseband_write_mem_table(NULL, (uint32_t)0x7d, (uint32_t)0x3fe63fcd);
        baseband_write_mem_table(NULL, (uint32_t)0x7f, (uint32_t)0x3fff3ff7);
        baseband_write_reg(NULL, (uint32_t)0xc00014, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc0065c, (uint32_t)0x107);
        baseband_write_reg(NULL, (uint32_t)0xc00664, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00670, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00660, (uint32_t)0x133);
        baseband_write_reg(NULL, (uint32_t)0xc00668, (uint32_t)0xffff);
        baseband_write_reg(NULL, (uint32_t)0xc0066c, (uint32_t)0xd1b6e);
        EMBARC_PRINTF("/*** fft params programmed! ***/\n\r");
    }
/*** inteference params programmed! ***/
/*** velamb params programmed! ***/
    if (acc_phase == 2) {
        baseband_write_reg(NULL, (uint32_t)0xc00800, (uint32_t)0x3f);
        baseband_write_reg(NULL, (uint32_t)0xc00810, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00804, (uint32_t)0x1);
        baseband_write_reg(NULL, (uint32_t)0xc00808, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc0080c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00834, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc00838, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc0083c, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc00814, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc00818, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc0081c, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc00820, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc00824, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc00828, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc0082c, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc00830, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc00960, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc0090c, (uint32_t)0xfff00600);
        baseband_write_reg(NULL, (uint32_t)0xc00910, (uint32_t)0xc0180300);
        baseband_write_reg(NULL, (uint32_t)0xc00914, (uint32_t)0x600c0180);
        baseband_write_reg(NULL, (uint32_t)0xc00918, (uint32_t)0x600fff);
        baseband_write_reg(NULL, (uint32_t)0xc008fc, (uint32_t)0xffffffff);
        baseband_write_reg(NULL, (uint32_t)0xc00900, (uint32_t)0xffffffff);
        baseband_write_reg(NULL, (uint32_t)0xc00904, (uint32_t)0xffffffff);
        baseband_write_reg(NULL, (uint32_t)0xc00908, (uint32_t)0x1ffffff);
        baseband_write_reg(NULL, (uint32_t)0xc008ec, (uint32_t)0x1ffe00);
        baseband_write_reg(NULL, (uint32_t)0xc008f0, (uint32_t)0xc0180300);
        baseband_write_reg(NULL, (uint32_t)0xc008f4, (uint32_t)0x600c0180);
        baseband_write_reg(NULL, (uint32_t)0xc008f8, (uint32_t)0x7ff800);
        baseband_write_reg(NULL, (uint32_t)0xc008dc, (uint32_t)0x1fffff);
        baseband_write_reg(NULL, (uint32_t)0xc008e0, (uint32_t)0xffffffff);
        baseband_write_reg(NULL, (uint32_t)0xc008e4, (uint32_t)0xffffffff);
        baseband_write_reg(NULL, (uint32_t)0xc008e8, (uint32_t)0x1fff800);
        baseband_write_reg(NULL, (uint32_t)0xc008cc, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc008d0, (uint32_t)0xc0080100);
        baseband_write_reg(NULL, (uint32_t)0xc008d4, (uint32_t)0x200400ff);
        baseband_write_reg(NULL, (uint32_t)0xc008d8, (uint32_t)0x1c00000);
        baseband_write_reg(NULL, (uint32_t)0xc008bc, (uint32_t)0x3ff);
        baseband_write_reg(NULL, (uint32_t)0xc008c0, (uint32_t)0xffffffff);
        baseband_write_reg(NULL, (uint32_t)0xc008c4, (uint32_t)0xffffffff);
        baseband_write_reg(NULL, (uint32_t)0xc008c8, (uint32_t)0x1c00000);
        baseband_write_reg(NULL, (uint32_t)0xc008ac, (uint32_t)0x803ffe00);
        baseband_write_reg(NULL, (uint32_t)0xc008b0, (uint32_t)0xc0180300);
        baseband_write_reg(NULL, (uint32_t)0xc008b4, (uint32_t)0x600c0180);
        baseband_write_reg(NULL, (uint32_t)0xc008b8, (uint32_t)0x7ffc01);
        baseband_write_reg(NULL, (uint32_t)0xc0089c, (uint32_t)0xff901);
        baseband_write_reg(NULL, (uint32_t)0xc008a0, (uint32_t)0x20240480);
        baseband_write_reg(NULL, (uint32_t)0xc008a4, (uint32_t)0x90120240);
        baseband_write_reg(NULL, (uint32_t)0xc008a8, (uint32_t)0x9ff000);
        baseband_write_reg(NULL, (uint32_t)0xc00844, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc0093c, (uint32_t)0xff);
        baseband_write_reg(NULL, (uint32_t)0xc0091c, (uint32_t)0x739c0);
        baseband_write_reg(NULL, (uint32_t)0xc00940, (uint32_t)0x1f);
        baseband_write_reg(NULL, (uint32_t)0xc00920, (uint32_t)0x739c0);
        baseband_write_reg(NULL, (uint32_t)0xc00944, (uint32_t)0x1f);
        baseband_write_reg(NULL, (uint32_t)0xc00924, (uint32_t)0x739c0);
        baseband_write_reg(NULL, (uint32_t)0xc00948, (uint32_t)0x1f);
        baseband_write_reg(NULL, (uint32_t)0xc00928, (uint32_t)0x739c0);
        baseband_write_reg(NULL, (uint32_t)0xc0094c, (uint32_t)0x1f);
        baseband_write_reg(NULL, (uint32_t)0xc0092c, (uint32_t)0x739c0);
        baseband_write_reg(NULL, (uint32_t)0xc00950, (uint32_t)0x1f);
        baseband_write_reg(NULL, (uint32_t)0xc00930, (uint32_t)0x739c0);
        baseband_write_reg(NULL, (uint32_t)0xc00954, (uint32_t)0x1f);
        baseband_write_reg(NULL, (uint32_t)0xc00934, (uint32_t)0x739c0);
        baseband_write_reg(NULL, (uint32_t)0xc00958, (uint32_t)0x1f);
        baseband_write_reg(NULL, (uint32_t)0xc00938, (uint32_t)0x739c0);
        baseband_write_reg(NULL, (uint32_t)0xc0095c, (uint32_t)0x1f);
        baseband_write_reg(NULL, (uint32_t)0xc00840, (uint32_t)0x0);
    *(uint32_t *)0xc0084c = 0x2; 
    *(uint32_t *)0xc00858 = 0x28a; 
    *(uint32_t *)0xc0084c = 0x102; 
    *(uint32_t *)0xc00858 = 0xa2a8a; 
    *(uint32_t *)0xc0084c = 0x8102; 
    *(uint32_t *)0xc00854 = 0x28a; 
    *(uint32_t *)0xc0084c = 0x408102; 
    *(uint32_t *)0xc00854 = 0xa2a8a; 
    *(uint32_t *)0xc00848 = 0x2; 
    *(uint32_t *)0xc00854 = 0x28aa2a8a; 
    *(uint32_t *)0xc00848 = 0x102; 
    *(uint32_t *)0xc00850 = 0x28a; 
    *(uint32_t *)0xc00848 = 0x8102; 
    *(uint32_t *)0xc00850 = 0xa2a8a; 
    *(uint32_t *)0xc00848 = 0x408102; 
    *(uint32_t *)0xc00850 = 0x28aa2a8a; 
        baseband_write_reg(NULL, (uint32_t)0xc00974, (uint32_t)0xff);
        EMBARC_PRINTF("/*** cfar params programmed! ***/\n\r");
        baseband_write_reg(NULL, (uint32_t)0xc00a24, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a2c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a30, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a34, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a3c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a38, (uint32_t)0x1);
        baseband_write_reg(NULL, (uint32_t)0xc00a4c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a50, (uint32_t)0x14);
        baseband_write_reg(NULL, (uint32_t)0xc00a7c, (uint32_t)0xf);
        baseband_write_reg(NULL, (uint32_t)0xc00a80, (uint32_t)0x1e);
        baseband_write_reg(NULL, (uint32_t)0xc00a84, (uint32_t)0x339);
        baseband_write_reg(NULL, (uint32_t)0xc00a88, (uint32_t)0x3c1);
        baseband_write_reg(NULL, (uint32_t)0xc00a8c, (uint32_t)0x3c1);
        baseband_write_reg(NULL, (uint32_t)0xc00a90, (uint32_t)0x3c1);
        baseband_write_reg(NULL, (uint32_t)0xc00014, (uint32_t)0x5);
        baseband_write_reg(NULL, (uint32_t)0xc00a40, (uint32_t)0x210);
        baseband_write_reg(NULL, (uint32_t)0xc00a48, (uint32_t)0x167);
        baseband_write_reg(NULL, (uint32_t)0xc00a54, (uint32_t)0x3);
        baseband_write_reg(NULL, (uint32_t)0xc00a58, (uint32_t)0x443);
        baseband_write_reg(NULL, (uint32_t)0xc00a5c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a60, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a64, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a68, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a6c, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a70, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00a74, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00bcc, (uint32_t)0x378);
        baseband_write_reg(NULL, (uint32_t)0xc00bd4, (uint32_t)0x4e0);
        baseband_write_reg(NULL, (uint32_t)0xc00bdc, (uint32_t)0x648);
        baseband_write_reg(NULL, (uint32_t)0xc00bd0, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00bd8, (uint32_t)0x0);
        baseband_write_reg(NULL, (uint32_t)0xc00be0, (uint32_t)0x0);
        baseband_write_mem_table(NULL, (uint32_t)0x840, (uint32_t)0xf78b26f);
        baseband_write_mem_table(NULL, (uint32_t)0x841, (uint32_t)0xe7c5f6c);
        baseband_write_mem_table(NULL, (uint32_t)0x842, (uint32_t)0x2782190);
        baseband_write_mem_table(NULL, (uint32_t)0x843, (uint32_t)0xd7f4965);
        baseband_write_mem_table(NULL, (uint32_t)0x844, (uint32_t)0xf593286);
        baseband_write_mem_table(NULL, (uint32_t)0x845, (uint32_t)0xeb3df94);
        baseband_write_mem_table(NULL, (uint32_t)0x846, (uint32_t)0x256e166);
        baseband_write_mem_table(NULL, (uint32_t)0x847, (uint32_t)0xd840979);
        baseband_write_mem_table(NULL, (uint32_t)0x848, (uint32_t)0xf3972a1);
        baseband_write_mem_table(NULL, (uint32_t)0x849, (uint32_t)0xeec5fb5);
        baseband_write_mem_table(NULL, (uint32_t)0x84a, (uint32_t)0x234e13e);
        baseband_write_mem_table(NULL, (uint32_t)0x84b, (uint32_t)0xd89098e);
        baseband_write_mem_table(NULL, (uint32_t)0x84c, (uint32_t)0xf19b2c1);
        baseband_write_mem_table(NULL, (uint32_t)0x84d, (uint32_t)0xf255fd1);
        baseband_write_mem_table(NULL, (uint32_t)0x84e, (uint32_t)0x212a117);
        baseband_write_mem_table(NULL, (uint32_t)0x84f, (uint32_t)0xd8e09a3);
        baseband_write_mem_table(NULL, (uint32_t)0x850, (uint32_t)0xef9f2e6);
        baseband_write_mem_table(NULL, (uint32_t)0x851, (uint32_t)0xf5f5fe7);
        baseband_write_mem_table(NULL, (uint32_t)0x852, (uint32_t)0x1efa0f3);
        baseband_write_mem_table(NULL, (uint32_t)0x853, (uint32_t)0xd9349b7);
        baseband_write_mem_table(NULL, (uint32_t)0x854, (uint32_t)0xeda3310);
        baseband_write_mem_table(NULL, (uint32_t)0x855, (uint32_t)0xf99dff6);
        baseband_write_mem_table(NULL, (uint32_t)0x856, (uint32_t)0x1cc60d1);
        baseband_write_mem_table(NULL, (uint32_t)0x857, (uint32_t)0xd9889cc);
        baseband_write_mem_table(NULL, (uint32_t)0x858, (uint32_t)0xebab340);
        baseband_write_mem_table(NULL, (uint32_t)0x859, (uint32_t)0xfd51ffe);
        baseband_write_mem_table(NULL, (uint32_t)0x85a, (uint32_t)0x1a860b2);
        baseband_write_mem_table(NULL, (uint32_t)0x85b, (uint32_t)0xd9dc9e1);
        baseband_write_mem_table(NULL, (uint32_t)0x85c, (uint32_t)0xe9b3374);
        baseband_write_mem_table(NULL, (uint32_t)0x85d, (uint32_t)0x10dfff);
        baseband_write_mem_table(NULL, (uint32_t)0x85e, (uint32_t)0x1842094);
        baseband_write_mem_table(NULL, (uint32_t)0x85f, (uint32_t)0xda349f6);
        baseband_write_mem_table(NULL, (uint32_t)0x860, (uint32_t)0xe7c33af);
        baseband_write_mem_table(NULL, (uint32_t)0x861, (uint32_t)0x4d1ffa);
        baseband_write_mem_table(NULL, (uint32_t)0x862, (uint32_t)0x15f6079);
        baseband_write_mem_table(NULL, (uint32_t)0x863, (uint32_t)0xda8ca0b);
        baseband_write_mem_table(NULL, (uint32_t)0x864, (uint32_t)0xe5d73ee);
        baseband_write_mem_table(NULL, (uint32_t)0x865, (uint32_t)0x89dfed);
        baseband_write_mem_table(NULL, (uint32_t)0x866, (uint32_t)0x13a2061);
        baseband_write_mem_table(NULL, (uint32_t)0x867, (uint32_t)0xdae8a1f);
        baseband_write_mem_table(NULL, (uint32_t)0x868, (uint32_t)0xe3ef433);
        baseband_write_mem_table(NULL, (uint32_t)0x869, (uint32_t)0xc71fd9);
        baseband_write_mem_table(NULL, (uint32_t)0x86a, (uint32_t)0x114604b);
        baseband_write_mem_table(NULL, (uint32_t)0x86b, (uint32_t)0xdb44a34);
        baseband_write_mem_table(NULL, (uint32_t)0x86c, (uint32_t)0xe20f47e);
        baseband_write_mem_table(NULL, (uint32_t)0x86d, (uint32_t)0x1049fbd);
        baseband_write_mem_table(NULL, (uint32_t)0x86e, (uint32_t)0xee2038);
        baseband_write_mem_table(NULL, (uint32_t)0x86f, (uint32_t)0xdba0a49);
        baseband_write_mem_table(NULL, (uint32_t)0x870, (uint32_t)0xe03b4ce);
        baseband_write_mem_table(NULL, (uint32_t)0x871, (uint32_t)0x1429f9a);
        baseband_write_mem_table(NULL, (uint32_t)0x872, (uint32_t)0xc7a027);
        baseband_write_mem_table(NULL, (uint32_t)0x873, (uint32_t)0xdc00a5e);
        baseband_write_mem_table(NULL, (uint32_t)0x874, (uint32_t)0xde6b524);
        baseband_write_mem_table(NULL, (uint32_t)0x875, (uint32_t)0x1809f6e);
        baseband_write_mem_table(NULL, (uint32_t)0x876, (uint32_t)0xa0a019);
        baseband_write_mem_table(NULL, (uint32_t)0x877, (uint32_t)0xdc60a72);
        baseband_write_mem_table(NULL, (uint32_t)0x878, (uint32_t)0xdcab57e);
        baseband_write_mem_table(NULL, (uint32_t)0x879, (uint32_t)0x1bf1f3b);
        baseband_write_mem_table(NULL, (uint32_t)0x87a, (uint32_t)0x79600e);
        baseband_write_mem_table(NULL, (uint32_t)0x87b, (uint32_t)0xdcc0a87);
        baseband_write_mem_table(NULL, (uint32_t)0x87c, (uint32_t)0xdaf35df);
        baseband_write_mem_table(NULL, (uint32_t)0x87d, (uint32_t)0x1fd5eff);
        baseband_write_mem_table(NULL, (uint32_t)0x87e, (uint32_t)0x51a006);
        baseband_write_mem_table(NULL, (uint32_t)0x87f, (uint32_t)0xdd24a9c);
        baseband_write_mem_table(NULL, (uint32_t)0x880, (uint32_t)0xd947644);
        baseband_write_mem_table(NULL, (uint32_t)0x881, (uint32_t)0x23b9eba);
        baseband_write_mem_table(NULL, (uint32_t)0x882, (uint32_t)0x29a002);
        baseband_write_mem_table(NULL, (uint32_t)0x883, (uint32_t)0xdd88ab0);
        baseband_write_mem_table(NULL, (uint32_t)0x884, (uint32_t)0xd7ab6af);
        baseband_write_mem_table(NULL, (uint32_t)0x885, (uint32_t)0x27a1e6e);
        baseband_write_mem_table(NULL, (uint32_t)0x886, (uint32_t)0x12000);
        baseband_write_mem_table(NULL, (uint32_t)0x887, (uint32_t)0xddf0ac5);
        baseband_write_mem_table(NULL, (uint32_t)0x888, (uint32_t)0xd61f71f);
        baseband_write_mem_table(NULL, (uint32_t)0x889, (uint32_t)0x2b81e18);
        baseband_write_mem_table(NULL, (uint32_t)0x88a, (uint32_t)0xfd86002);
        baseband_write_mem_table(NULL, (uint32_t)0x88b, (uint32_t)0xde58ad9);
        baseband_write_mem_table(NULL, (uint32_t)0x88c, (uint32_t)0xd4a3794);
        baseband_write_mem_table(NULL, (uint32_t)0x88d, (uint32_t)0x2f61dba);
        baseband_write_mem_table(NULL, (uint32_t)0x88e, (uint32_t)0xfaf6006);
        baseband_write_mem_table(NULL, (uint32_t)0x88f, (uint32_t)0xdec0aed);
        baseband_write_mem_table(NULL, (uint32_t)0x890, (uint32_t)0xd33780f);
        baseband_write_mem_table(NULL, (uint32_t)0x891, (uint32_t)0x333dd53);
        baseband_write_mem_table(NULL, (uint32_t)0x892, (uint32_t)0xf85e00f);
        baseband_write_mem_table(NULL, (uint32_t)0x893, (uint32_t)0xdf2cb02);
        baseband_write_mem_table(NULL, (uint32_t)0x894, (uint32_t)0xd1e388d);
        baseband_write_mem_table(NULL, (uint32_t)0x895, (uint32_t)0x3711ce3);
        baseband_write_mem_table(NULL, (uint32_t)0x896, (uint32_t)0xf5c601a);
        baseband_write_mem_table(NULL, (uint32_t)0x897, (uint32_t)0xdf9cb16);
        baseband_write_mem_table(NULL, (uint32_t)0x898, (uint32_t)0xd09f911);
        baseband_write_mem_table(NULL, (uint32_t)0x899, (uint32_t)0x3addc6b);
        baseband_write_mem_table(NULL, (uint32_t)0x89a, (uint32_t)0xf326029);
        baseband_write_mem_table(NULL, (uint32_t)0x89b, (uint32_t)0xe008b2a);
        baseband_write_mem_table(NULL, (uint32_t)0x89c, (uint32_t)0xcf6f999);
        baseband_write_mem_table(NULL, (uint32_t)0x89d, (uint32_t)0x3e9dbe9);
        baseband_write_mem_table(NULL, (uint32_t)0x89e, (uint32_t)0xf08603c);
        baseband_write_mem_table(NULL, (uint32_t)0x89f, (uint32_t)0xe078b3e);
        baseband_write_mem_table(NULL, (uint32_t)0x8a0, (uint32_t)0xce57a25);
        baseband_write_mem_table(NULL, (uint32_t)0x8a1, (uint32_t)0x4255b5e);
        baseband_write_mem_table(NULL, (uint32_t)0x8a2, (uint32_t)0xede2053);
        baseband_write_mem_table(NULL, (uint32_t)0x8a3, (uint32_t)0xe0ecb51);
        baseband_write_mem_table(NULL, (uint32_t)0x8a4, (uint32_t)0xcd57ab6);
        baseband_write_mem_table(NULL, (uint32_t)0x8a5, (uint32_t)0x4601aca);
        baseband_write_mem_table(NULL, (uint32_t)0x8a6, (uint32_t)0xeb3606d);
        baseband_write_mem_table(NULL, (uint32_t)0x8a7, (uint32_t)0xe160b65);
        baseband_write_mem_table(NULL, (uint32_t)0x8a8, (uint32_t)0xcc6fb4a);
        baseband_write_mem_table(NULL, (uint32_t)0x8a9, (uint32_t)0x49a1a2d);
        baseband_write_mem_table(NULL, (uint32_t)0x8aa, (uint32_t)0xe88e08b);
        baseband_write_mem_table(NULL, (uint32_t)0x8ab, (uint32_t)0xe1d4b79);
        baseband_write_mem_table(NULL, (uint32_t)0x8ac, (uint32_t)0xcb9fbe2);
        baseband_write_mem_table(NULL, (uint32_t)0x8ad, (uint32_t)0x4d2d987);
        baseband_write_mem_table(NULL, (uint32_t)0x8ae, (uint32_t)0xe5de0ad);
        baseband_write_mem_table(NULL, (uint32_t)0x8af, (uint32_t)0xe24cb8c);
        baseband_write_mem_table(NULL, (uint32_t)0x8b0, (uint32_t)0xcaebc7d);
        baseband_write_mem_table(NULL, (uint32_t)0x8b1, (uint32_t)0x50ad8d8);
        baseband_write_mem_table(NULL, (uint32_t)0x8b2, (uint32_t)0xe3320d2);
        baseband_write_mem_table(NULL, (uint32_t)0x8b3, (uint32_t)0xe2c4b9f);
        baseband_write_mem_table(NULL, (uint32_t)0x8b4, (uint32_t)0xca57d1b);
        baseband_write_mem_table(NULL, (uint32_t)0x8b5, (uint32_t)0x5415821);
        baseband_write_mem_table(NULL, (uint32_t)0x8b6, (uint32_t)0xe07e0fc);
        baseband_write_mem_table(NULL, (uint32_t)0x8b7, (uint32_t)0xe33cbb2);
        baseband_write_mem_table(NULL, (uint32_t)0x8b8, (uint32_t)0xc9dbdbc);
        baseband_write_mem_table(NULL, (uint32_t)0x8b9, (uint32_t)0x576d760);
        baseband_write_mem_table(NULL, (uint32_t)0x8ba, (uint32_t)0xddce12a);
        baseband_write_mem_table(NULL, (uint32_t)0x8bb, (uint32_t)0xe3b8bc5);
        baseband_write_mem_table(NULL, (uint32_t)0x8bc, (uint32_t)0xc97be5f);
        baseband_write_mem_table(NULL, (uint32_t)0x8bd, (uint32_t)0x5aad696);
        baseband_write_mem_table(NULL, (uint32_t)0x8be, (uint32_t)0xdb1a15c);
        baseband_write_mem_table(NULL, (uint32_t)0x8bf, (uint32_t)0xe438bd7);
        baseband_write_mem_table(NULL, (uint32_t)0x8c0, (uint32_t)0xc93ff04);
        baseband_write_mem_table(NULL, (uint32_t)0x8c1, (uint32_t)0x5dd55c4);
        baseband_write_mem_table(NULL, (uint32_t)0x8c2, (uint32_t)0xd86a192);
        baseband_write_mem_table(NULL, (uint32_t)0x8c3, (uint32_t)0xe4b4bea);
        baseband_write_mem_table(NULL, (uint32_t)0x8c4, (uint32_t)0xc91bfab);
        baseband_write_mem_table(NULL, (uint32_t)0x8c5, (uint32_t)0x60e54e9);
        baseband_write_mem_table(NULL, (uint32_t)0x8c6, (uint32_t)0xd5b61cc);
        baseband_write_mem_table(NULL, (uint32_t)0x8c7, (uint32_t)0xe538bfc);
        baseband_write_mem_table(NULL, (uint32_t)0x8c8, (uint32_t)0xc918053);
        baseband_write_mem_table(NULL, (uint32_t)0x8c9, (uint32_t)0x63d9406);
        baseband_write_mem_table(NULL, (uint32_t)0x8ca, (uint32_t)0xd30620b);
        baseband_write_mem_table(NULL, (uint32_t)0x8cb, (uint32_t)0xe5b8c0e);
        baseband_write_mem_table(NULL, (uint32_t)0x8cc, (uint32_t)0xc93c0fd);
        baseband_write_mem_table(NULL, (uint32_t)0x8cd, (uint32_t)0x66b131a);
        baseband_write_mem_table(NULL, (uint32_t)0x8ce, (uint32_t)0xd05624d);
        baseband_write_mem_table(NULL, (uint32_t)0x8cf, (uint32_t)0xe63cc20);
        baseband_write_mem_table(NULL, (uint32_t)0x8d0, (uint32_t)0xc97c1a6);
        baseband_write_mem_table(NULL, (uint32_t)0x8d1, (uint32_t)0x696d227);
        baseband_write_mem_table(NULL, (uint32_t)0x8d2, (uint32_t)0xcda6294);
        baseband_write_mem_table(NULL, (uint32_t)0x8d3, (uint32_t)0xe6c4c31);
        baseband_write_mem_table(NULL, (uint32_t)0x8d4, (uint32_t)0xc9e0250);
        baseband_write_mem_table(NULL, (uint32_t)0x8d5, (uint32_t)0x6c0512b);
        baseband_write_mem_table(NULL, (uint32_t)0x8d6, (uint32_t)0xcafa2e0);
        baseband_write_mem_table(NULL, (uint32_t)0x8d7, (uint32_t)0xe748c43);
        baseband_write_mem_table(NULL, (uint32_t)0x8d8, (uint32_t)0xca642fa);
        baseband_write_mem_table(NULL, (uint32_t)0x8d9, (uint32_t)0x6e7d028);
        baseband_write_mem_table(NULL, (uint32_t)0x8da, (uint32_t)0xc852330);
        baseband_write_mem_table(NULL, (uint32_t)0x8db, (uint32_t)0xe7d4c54);
        baseband_write_mem_table(NULL, (uint32_t)0x8dc, (uint32_t)0xcb0c3a2);
        baseband_write_mem_table(NULL, (uint32_t)0x8dd, (uint32_t)0x70d0f1e);
        baseband_write_mem_table(NULL, (uint32_t)0x8de, (uint32_t)0xc5ae384);
        baseband_write_mem_table(NULL, (uint32_t)0x8df, (uint32_t)0xe85cc64);
        baseband_write_mem_table(NULL, (uint32_t)0x8e0, (uint32_t)0xcbd444a);
        baseband_write_mem_table(NULL, (uint32_t)0x8e1, (uint32_t)0x7300e0d);
        baseband_write_mem_table(NULL, (uint32_t)0x8e2, (uint32_t)0xc30a3dd);
        baseband_write_mem_table(NULL, (uint32_t)0x8e3, (uint32_t)0xe8e8c75);
        baseband_write_mem_table(NULL, (uint32_t)0x8e4, (uint32_t)0xccc04ef);
        baseband_write_mem_table(NULL, (uint32_t)0x8e5, (uint32_t)0x750ccf4);
        baseband_write_mem_table(NULL, (uint32_t)0x8e6, (uint32_t)0xc06e43a);
        baseband_write_mem_table(NULL, (uint32_t)0x8e7, (uint32_t)0xe978c85);
        baseband_write_mem_table(NULL, (uint32_t)0x8e8, (uint32_t)0xcdd0593);
        baseband_write_mem_table(NULL, (uint32_t)0x8e9, (uint32_t)0x76ecbd5);
        baseband_write_mem_table(NULL, (uint32_t)0x8ea, (uint32_t)0xbdd249c);
        baseband_write_mem_table(NULL, (uint32_t)0x8eb, (uint32_t)0xea04c95);
        baseband_write_mem_table(NULL, (uint32_t)0x8ec, (uint32_t)0xcf04634);
        baseband_write_mem_table(NULL, (uint32_t)0x8ed, (uint32_t)0x78a4ab1);
        baseband_write_mem_table(NULL, (uint32_t)0x8ee, (uint32_t)0xbb3e502);
        baseband_write_mem_table(NULL, (uint32_t)0x8ef, (uint32_t)0xea98ca4);
        baseband_write_mem_table(NULL, (uint32_t)0x8f0, (uint32_t)0xd0586d2);
        baseband_write_mem_table(NULL, (uint32_t)0x8f1, (uint32_t)0x7a34986);
        baseband_write_mem_table(NULL, (uint32_t)0x8f2, (uint32_t)0xb8b256d);
        baseband_write_mem_table(NULL, (uint32_t)0x8f3, (uint32_t)0xeb28cb4);
        baseband_write_mem_table(NULL, (uint32_t)0x8f4, (uint32_t)0xd1cc76c);
        baseband_write_mem_table(NULL, (uint32_t)0x8f5, (uint32_t)0x7b94856);
        baseband_write_mem_table(NULL, (uint32_t)0x8f6, (uint32_t)0xb62a5dc);
        baseband_write_mem_table(NULL, (uint32_t)0x8f7, (uint32_t)0xebbccc2);
        baseband_write_mem_table(NULL, (uint32_t)0x8f8, (uint32_t)0xd364802);
        baseband_write_mem_table(NULL, (uint32_t)0x8f9, (uint32_t)0x7cc8721);
        baseband_write_mem_table(NULL, (uint32_t)0x8fa, (uint32_t)0xb3aa650);
        baseband_write_mem_table(NULL, (uint32_t)0x8fb, (uint32_t)0xec50cd1);
        baseband_write_mem_table(NULL, (uint32_t)0x8fc, (uint32_t)0xd51c893);
        baseband_write_mem_table(NULL, (uint32_t)0x8fd, (uint32_t)0x7dcc5e7);
        baseband_write_mem_table(NULL, (uint32_t)0x8fe, (uint32_t)0xb1326c9);
        baseband_write_mem_table(NULL, (uint32_t)0x8ff, (uint32_t)0xece8cdf);
        baseband_write_mem_table(NULL, (uint32_t)0x900, (uint32_t)0xd6f491f);
        baseband_write_mem_table(NULL, (uint32_t)0x901, (uint32_t)0x7ea44a9);
        baseband_write_mem_table(NULL, (uint32_t)0x902, (uint32_t)0xaec2746);
        baseband_write_mem_table(NULL, (uint32_t)0x903, (uint32_t)0xed80ced);
        baseband_write_mem_table(NULL, (uint32_t)0x904, (uint32_t)0xd8ec9a6);
        baseband_write_mem_table(NULL, (uint32_t)0x905, (uint32_t)0x7f44368);
        baseband_write_mem_table(NULL, (uint32_t)0x906, (uint32_t)0xac5e7c7);
        baseband_write_mem_table(NULL, (uint32_t)0x907, (uint32_t)0xee1ccfb);
        baseband_write_mem_table(NULL, (uint32_t)0x908, (uint32_t)0xdb04a26);
        baseband_write_mem_table(NULL, (uint32_t)0x909, (uint32_t)0x7fb8223);
        baseband_write_mem_table(NULL, (uint32_t)0x90a, (uint32_t)0xaa0284d);
        baseband_write_mem_table(NULL, (uint32_t)0x90b, (uint32_t)0xeeb8d08);
        baseband_write_mem_table(NULL, (uint32_t)0x90c, (uint32_t)0xdd38aa0);
        baseband_write_mem_table(NULL, (uint32_t)0x90d, (uint32_t)0x7ff40dc);
        baseband_write_mem_table(NULL, (uint32_t)0x90e, (uint32_t)0xa7ae8d7);
        baseband_write_mem_table(NULL, (uint32_t)0x90f, (uint32_t)0xef54d15);
        baseband_write_mem_table(NULL, (uint32_t)0x910, (uint32_t)0xdf88b13);
        baseband_write_mem_table(NULL, (uint32_t)0x911, (uint32_t)0x7ffff93);
        baseband_write_mem_table(NULL, (uint32_t)0x912, (uint32_t)0xa566966);
        baseband_write_mem_table(NULL, (uint32_t)0x913, (uint32_t)0xeff0d21);
        baseband_write_mem_table(NULL, (uint32_t)0x914, (uint32_t)0xe1f4b7e);
        baseband_write_mem_table(NULL, (uint32_t)0x915, (uint32_t)0x7fd3e48);
        baseband_write_mem_table(NULL, (uint32_t)0x916, (uint32_t)0xa32a9f9);
        baseband_write_mem_table(NULL, (uint32_t)0x917, (uint32_t)0xf090d2d);
        baseband_write_mem_table(NULL, (uint32_t)0x918, (uint32_t)0xe47cbe1);
        baseband_write_mem_table(NULL, (uint32_t)0x919, (uint32_t)0x7f73cfc);
        baseband_write_mem_table(NULL, (uint32_t)0x91a, (uint32_t)0xa0f6a91);
        baseband_write_mem_table(NULL, (uint32_t)0x91b, (uint32_t)0xf130d38);
        baseband_write_mem_table(NULL, (uint32_t)0x91c, (uint32_t)0xe718c3c);
        baseband_write_mem_table(NULL, (uint32_t)0x91d, (uint32_t)0x7ed7baf);
        baseband_write_mem_table(NULL, (uint32_t)0x91e, (uint32_t)0x9ed2b2c);
        baseband_write_mem_table(NULL, (uint32_t)0x91f, (uint32_t)0xf1d4d44);
        baseband_write_mem_table(NULL, (uint32_t)0x920, (uint32_t)0xe9ccc8e);
        baseband_write_mem_table(NULL, (uint32_t)0x921, (uint32_t)0x7e07a63);
        baseband_write_mem_table(NULL, (uint32_t)0x922, (uint32_t)0x9cbebcd);
        baseband_write_mem_table(NULL, (uint32_t)0x923, (uint32_t)0xf278d4e);
        baseband_write_mem_table(NULL, (uint32_t)0x924, (uint32_t)0xec94cd7);
        baseband_write_mem_table(NULL, (uint32_t)0x925, (uint32_t)0x7cff917);
        baseband_write_mem_table(NULL, (uint32_t)0x926, (uint32_t)0x9ab2c71);
        baseband_write_mem_table(NULL, (uint32_t)0x927, (uint32_t)0xf31cd58);
        baseband_write_mem_table(NULL, (uint32_t)0x928, (uint32_t)0xef70d17);
        baseband_write_mem_table(NULL, (uint32_t)0x929, (uint32_t)0x7bbf7cd);
        baseband_write_mem_table(NULL, (uint32_t)0x92a, (uint32_t)0x98bad19);
        baseband_write_mem_table(NULL, (uint32_t)0x92b, (uint32_t)0xf3c4d62);
        baseband_write_mem_table(NULL, (uint32_t)0x92c, (uint32_t)0xf25cd4c);
        baseband_write_mem_table(NULL, (uint32_t)0x92d, (uint32_t)0x7a43685);
        baseband_write_mem_table(NULL, (uint32_t)0x92e, (uint32_t)0x96cedc6);
        baseband_write_mem_table(NULL, (uint32_t)0x92f, (uint32_t)0xf46cd6c);
        baseband_write_mem_table(NULL, (uint32_t)0x930, (uint32_t)0xf554d78);
        baseband_write_mem_table(NULL, (uint32_t)0x931, (uint32_t)0x789353f);
        baseband_write_mem_table(NULL, (uint32_t)0x932, (uint32_t)0x94f2e76);
        baseband_write_mem_table(NULL, (uint32_t)0x933, (uint32_t)0xf514d74);
        baseband_write_mem_table(NULL, (uint32_t)0x934, (uint32_t)0xf85cd98);
        baseband_write_mem_table(NULL, (uint32_t)0x935, (uint32_t)0x76a73fd);
        baseband_write_mem_table(NULL, (uint32_t)0x936, (uint32_t)0x9326f2b);
        baseband_write_mem_table(NULL, (uint32_t)0x937, (uint32_t)0xf5c0d7d);
        baseband_write_mem_table(NULL, (uint32_t)0x938, (uint32_t)0xfb6cdae);
        baseband_write_mem_table(NULL, (uint32_t)0x939, (uint32_t)0x74832bf);
        baseband_write_mem_table(NULL, (uint32_t)0x93a, (uint32_t)0x916afe3);
        baseband_write_mem_table(NULL, (uint32_t)0x93b, (uint32_t)0xf66cd85);
        baseband_write_mem_table(NULL, (uint32_t)0x93c, (uint32_t)0xfe84db9);
        baseband_write_mem_table(NULL, (uint32_t)0x93d, (uint32_t)0x7227185);
        baseband_write_mem_table(NULL, (uint32_t)0x93e, (uint32_t)0x8fc309f);
        baseband_write_mem_table(NULL, (uint32_t)0x93f, (uint32_t)0xf718d8c);
        baseband_write_mem_table(NULL, (uint32_t)0x940, (uint32_t)0x1a0db9);
        baseband_write_mem_table(NULL, (uint32_t)0x941, (uint32_t)0x6f93050);
        baseband_write_mem_table(NULL, (uint32_t)0x942, (uint32_t)0x8e2f15e);
        baseband_write_mem_table(NULL, (uint32_t)0x943, (uint32_t)0xf7c4d93);
        baseband_write_mem_table(NULL, (uint32_t)0x944, (uint32_t)0x4bcdad);
        baseband_write_mem_table(NULL, (uint32_t)0x945, (uint32_t)0x6cc6f22);
        baseband_write_mem_table(NULL, (uint32_t)0x946, (uint32_t)0x8ca7221);
        baseband_write_mem_table(NULL, (uint32_t)0x947, (uint32_t)0xf874d99);
        baseband_write_mem_table(NULL, (uint32_t)0x948, (uint32_t)0x7dcd96);
        baseband_write_mem_table(NULL, (uint32_t)0x949, (uint32_t)0x69c6dfa);
        baseband_write_mem_table(NULL, (uint32_t)0x94a, (uint32_t)0x8b372e8);
        baseband_write_mem_table(NULL, (uint32_t)0x94b, (uint32_t)0xf924d9f);
        baseband_write_mem_table(NULL, (uint32_t)0x94c, (uint32_t)0xaf8d74);
        baseband_write_mem_table(NULL, (uint32_t)0x94d, (uint32_t)0x668ecd9);
        baseband_write_mem_table(NULL, (uint32_t)0x94e, (uint32_t)0x89db3b2);
        baseband_write_mem_table(NULL, (uint32_t)0x94f, (uint32_t)0xf9d8da4);
        baseband_write_mem_table(NULL, (uint32_t)0x950, (uint32_t)0xe0cd45);
        baseband_write_mem_table(NULL, (uint32_t)0x951, (uint32_t)0x631ebc0);
        baseband_write_mem_table(NULL, (uint32_t)0x952, (uint32_t)0x889347f);
        baseband_write_mem_table(NULL, (uint32_t)0x953, (uint32_t)0xfa88da9);
        baseband_write_mem_table(NULL, (uint32_t)0x954, (uint32_t)0x111cd0c);
        baseband_write_mem_table(NULL, (uint32_t)0x955, (uint32_t)0x5f7eab0);
        baseband_write_mem_table(NULL, (uint32_t)0x956, (uint32_t)0x875f54f);
        baseband_write_mem_table(NULL, (uint32_t)0x957, (uint32_t)0xfb3cdad);
        baseband_write_mem_table(NULL, (uint32_t)0x958, (uint32_t)0x141ccc6);
        baseband_write_mem_table(NULL, (uint32_t)0x959, (uint32_t)0x5baa9a9);
        baseband_write_mem_table(NULL, (uint32_t)0x95a, (uint32_t)0x863f622);
        baseband_write_mem_table(NULL, (uint32_t)0x95b, (uint32_t)0xfbf0db1);
        baseband_write_mem_table(NULL, (uint32_t)0x95c, (uint32_t)0x1710c75);
        baseband_write_mem_table(NULL, (uint32_t)0x95d, (uint32_t)0x57a28ac);
        baseband_write_mem_table(NULL, (uint32_t)0x95e, (uint32_t)0x85376f8);
        baseband_write_mem_table(NULL, (uint32_t)0x95f, (uint32_t)0xfca4db4);
        baseband_write_mem_table(NULL, (uint32_t)0x960, (uint32_t)0x19f4c19);
        baseband_write_mem_table(NULL, (uint32_t)0x961, (uint32_t)0x536a7ba);
        baseband_write_mem_table(NULL, (uint32_t)0x962, (uint32_t)0x84477d1);
        baseband_write_mem_table(NULL, (uint32_t)0x963, (uint32_t)0xfd5cdb6);
        baseband_write_mem_table(NULL, (uint32_t)0x964, (uint32_t)0x1cc0bb2);
        baseband_write_mem_table(NULL, (uint32_t)0x965, (uint32_t)0x4f066d3);
        baseband_write_mem_table(NULL, (uint32_t)0x966, (uint32_t)0x836b8ac);
        baseband_write_mem_table(NULL, (uint32_t)0x967, (uint32_t)0xfe14db8);
        baseband_write_mem_table(NULL, (uint32_t)0x968, (uint32_t)0x1f78b40);
        baseband_write_mem_table(NULL, (uint32_t)0x969, (uint32_t)0x4a725f8);
        baseband_write_mem_table(NULL, (uint32_t)0x96a, (uint32_t)0x82a7989);
        baseband_write_mem_table(NULL, (uint32_t)0x96b, (uint32_t)0xfeccdba);
        baseband_write_mem_table(NULL, (uint32_t)0x96c, (uint32_t)0x2218ac3);
        baseband_write_mem_table(NULL, (uint32_t)0x96d, (uint32_t)0x45b6529);
        baseband_write_mem_table(NULL, (uint32_t)0x96e, (uint32_t)0x81fba69);
        baseband_write_mem_table(NULL, (uint32_t)0x96f, (uint32_t)0xff84dba);
        baseband_write_mem_table(NULL, (uint32_t)0x970, (uint32_t)0x249ca3c);
        baseband_write_mem_table(NULL, (uint32_t)0x971, (uint32_t)0x40ce467);
        baseband_write_mem_table(NULL, (uint32_t)0x972, (uint32_t)0x8167b4a);
        baseband_write_mem_table(NULL, (uint32_t)0x973, (uint32_t)0x3cdba);
        baseband_write_mem_table(NULL, (uint32_t)0x974, (uint32_t)0x26fc9ab);
        baseband_write_mem_table(NULL, (uint32_t)0x975, (uint32_t)0x3bc23b3);
        baseband_write_mem_table(NULL, (uint32_t)0x976, (uint32_t)0x80efc2e);
        baseband_write_mem_table(NULL, (uint32_t)0x977, (uint32_t)0xf8dba);
        baseband_write_mem_table(NULL, (uint32_t)0x978, (uint32_t)0x2940910);
        baseband_write_mem_table(NULL, (uint32_t)0x979, (uint32_t)0x368e30d);
        baseband_write_mem_table(NULL, (uint32_t)0x97a, (uint32_t)0x808bd13);
        baseband_write_mem_table(NULL, (uint32_t)0x97b, (uint32_t)0x1b4db9);
        baseband_write_mem_table(NULL, (uint32_t)0x97c, (uint32_t)0x2b5c86d);
        baseband_write_mem_table(NULL, (uint32_t)0x97d, (uint32_t)0x313a276);
        baseband_write_mem_table(NULL, (uint32_t)0x97e, (uint32_t)0x8043df9);
        baseband_write_mem_table(NULL, (uint32_t)0x97f, (uint32_t)0x270db7);
        baseband_write_mem_table(NULL, (uint32_t)0x980, (uint32_t)0x2d547c0);
        baseband_write_mem_table(NULL, (uint32_t)0x981, (uint32_t)0x2bc61ee);
        baseband_write_mem_table(NULL, (uint32_t)0x982, (uint32_t)0x8017ee1);
        baseband_write_mem_table(NULL, (uint32_t)0x983, (uint32_t)0x32cdb5);
        baseband_write_mem_table(NULL, (uint32_t)0x984, (uint32_t)0x2f2070c);
        baseband_write_mem_table(NULL, (uint32_t)0x985, (uint32_t)0x2636175);
        baseband_write_mem_table(NULL, (uint32_t)0x986, (uint32_t)0x8003fca);
        baseband_write_mem_table(NULL, (uint32_t)0x987, (uint32_t)0x3e8db2);
        baseband_write_mem_table(NULL, (uint32_t)0x988, (uint32_t)0x30c4651);
        baseband_write_mem_table(NULL, (uint32_t)0x989, (uint32_t)0x208a10d);
        baseband_write_mem_table(NULL, (uint32_t)0x98a, (uint32_t)0x80080b4);
        baseband_write_mem_table(NULL, (uint32_t)0x98b, (uint32_t)0x4a4dae);
        baseband_write_mem_table(NULL, (uint32_t)0x98c, (uint32_t)0x323858e);
        baseband_write_mem_table(NULL, (uint32_t)0x98d, (uint32_t)0x1aca0b5);
        baseband_write_mem_table(NULL, (uint32_t)0x98e, (uint32_t)0x802c19f);
        baseband_write_mem_table(NULL, (uint32_t)0x98f, (uint32_t)0x564daa);
        baseband_write_mem_table(NULL, (uint32_t)0x990, (uint32_t)0x337c4c6);
        baseband_write_mem_table(NULL, (uint32_t)0x991, (uint32_t)0x14f206e);
        baseband_write_mem_table(NULL, (uint32_t)0x992, (uint32_t)0x806828a);
        baseband_write_mem_table(NULL, (uint32_t)0x993, (uint32_t)0x620da5);
        baseband_write_mem_table(NULL, (uint32_t)0x994, (uint32_t)0x34903f8);
        baseband_write_mem_table(NULL, (uint32_t)0x995, (uint32_t)0xf0a039);
        baseband_write_mem_table(NULL, (uint32_t)0x996, (uint32_t)0x80c0376);
        baseband_write_mem_table(NULL, (uint32_t)0x997, (uint32_t)0x6e0d9f);
        baseband_write_mem_table(NULL, (uint32_t)0x998, (uint32_t)0x3574326);
        baseband_write_mem_table(NULL, (uint32_t)0x999, (uint32_t)0x916015);
        baseband_write_mem_table(NULL, (uint32_t)0x99a, (uint32_t)0x8134461);
        baseband_write_mem_table(NULL, (uint32_t)0x99b, (uint32_t)0x7a0d99);
        baseband_write_mem_table(NULL, (uint32_t)0x99c, (uint32_t)0x362024f);
        baseband_write_mem_table(NULL, (uint32_t)0x99d, (uint32_t)0x316002);
        baseband_write_mem_table(NULL, (uint32_t)0x99e, (uint32_t)0x81c454d);
        baseband_write_mem_table(NULL, (uint32_t)0x99f, (uint32_t)0x860d91);
        baseband_write_mem_table(NULL, (uint32_t)0x9a0, (uint32_t)0x369c176);
        baseband_write_mem_table(NULL, (uint32_t)0x9a1, (uint32_t)0xfd12002);
        baseband_write_mem_table(NULL, (uint32_t)0x9a2, (uint32_t)0x8270638);
        baseband_write_mem_table(NULL, (uint32_t)0x9a3, (uint32_t)0x920d8a);
        baseband_write_mem_table(NULL, (uint32_t)0x9a4, (uint32_t)0x36dc09a);
        baseband_write_mem_table(NULL, (uint32_t)0x9a5, (uint32_t)0xf706014);
        baseband_write_mem_table(NULL, (uint32_t)0x9a6, (uint32_t)0x8338723);
        baseband_write_mem_table(NULL, (uint32_t)0x9a7, (uint32_t)0x9e0d81);
        baseband_write_mem_table(NULL, (uint32_t)0x9a8, (uint32_t)0x36ebfbd);
        baseband_write_mem_table(NULL, (uint32_t)0x9a9, (uint32_t)0xf0fe039);
        baseband_write_mem_table(NULL, (uint32_t)0x9aa, (uint32_t)0x841c80d);
        baseband_write_mem_table(NULL, (uint32_t)0x9ab, (uint32_t)0xaa0d78);
        baseband_write_mem_table(NULL, (uint32_t)0x9ac, (uint32_t)0x36bfee0);
        baseband_write_mem_table(NULL, (uint32_t)0x9ad, (uint32_t)0xeaf6070);
        baseband_write_mem_table(NULL, (uint32_t)0x9ae, (uint32_t)0x85208f6);
        baseband_write_mem_table(NULL, (uint32_t)0x9af, (uint32_t)0xb60d6e);
        baseband_write_mem_table(NULL, (uint32_t)0x9b0, (uint32_t)0x3657e03);
        baseband_write_mem_table(NULL, (uint32_t)0x9b1, (uint32_t)0xe4f60b9);
        baseband_write_mem_table(NULL, (uint32_t)0x9b2, (uint32_t)0x863c9dd);
        baseband_write_mem_table(NULL, (uint32_t)0x9b3, (uint32_t)0xc20d64);
        baseband_write_mem_table(NULL, (uint32_t)0x9b4, (uint32_t)0x35bbd27);
        baseband_write_mem_table(NULL, (uint32_t)0x9b5, (uint32_t)0xdf02115);
        baseband_write_mem_table(NULL, (uint32_t)0x9b6, (uint32_t)0x8774ac4);
        baseband_write_mem_table(NULL, (uint32_t)0x9b7, (uint32_t)0xce0d59);
        baseband_write_mem_table(NULL, (uint32_t)0x9b8, (uint32_t)0x34e7c4d);
        baseband_write_mem_table(NULL, (uint32_t)0x9b9, (uint32_t)0xd91a184);
        baseband_write_mem_table(NULL, (uint32_t)0x9ba, (uint32_t)0x88ccba8);
        baseband_write_mem_table(NULL, (uint32_t)0x9bb, (uint32_t)0xda0d4d);
        baseband_write_mem_table(NULL, (uint32_t)0x9bc, (uint32_t)0x33d7b77);
        baseband_write_mem_table(NULL, (uint32_t)0x9bd, (uint32_t)0xd346205);
        baseband_write_mem_table(NULL, (uint32_t)0x9be, (uint32_t)0x8a3cc8b);
        baseband_write_mem_table(NULL, (uint32_t)0x9bf, (uint32_t)0xe60d40);
        baseband_write_mem_table(NULL, (uint32_t)0x9c0, (uint32_t)0x3293aa5);
        baseband_write_mem_table(NULL, (uint32_t)0x9c1, (uint32_t)0xcd86298);
        baseband_write_mem_table(NULL, (uint32_t)0x9c2, (uint32_t)0x8bccd6b);
        baseband_write_mem_table(NULL, (uint32_t)0x9c3, (uint32_t)0xf20d33);
        baseband_write_mem_table(NULL, (uint32_t)0x9c4, (uint32_t)0x31179d8);
        baseband_write_mem_table(NULL, (uint32_t)0x9c5, (uint32_t)0xc7e233d);
        baseband_write_mem_table(NULL, (uint32_t)0x9c6, (uint32_t)0x8d78e49);
        baseband_write_mem_table(NULL, (uint32_t)0x9c7, (uint32_t)0xfe0d25);
        baseband_write_mem_table(NULL, (uint32_t)0x9c8, (uint32_t)0x2f67911);
        baseband_write_mem_table(NULL, (uint32_t)0x9c9, (uint32_t)0xc25e3f5);
        baseband_write_mem_table(NULL, (uint32_t)0x9ca, (uint32_t)0x8f3cf24);
        baseband_write_mem_table(NULL, (uint32_t)0x9cb, (uint32_t)0x10a0d16);
        baseband_write_mem_table(NULL, (uint32_t)0x9cc, (uint32_t)0x2d83850);
        baseband_write_mem_table(NULL, (uint32_t)0x9cd, (uint32_t)0xbcf64be);
        baseband_write_mem_table(NULL, (uint32_t)0x9ce, (uint32_t)0x9120ffd);
        baseband_write_mem_table(NULL, (uint32_t)0x9cf, (uint32_t)0x1160d06);
        baseband_write_mem_table(NULL, (uint32_t)0x9d0, (uint32_t)0x2b6b797);
        baseband_write_mem_table(NULL, (uint32_t)0x9d1, (uint32_t)0xb7b6598);
        baseband_write_mem_table(NULL, (uint32_t)0x9d2, (uint32_t)0x931d0d2);
        baseband_write_mem_table(NULL, (uint32_t)0x9d3, (uint32_t)0x121ccf6);
        baseband_write_mem_table(NULL, (uint32_t)0x9d4, (uint32_t)0x29236e7);
        baseband_write_mem_table(NULL, (uint32_t)0x9d5, (uint32_t)0xb29e683);
        baseband_write_mem_table(NULL, (uint32_t)0x9d6, (uint32_t)0x95351a4);
        baseband_write_mem_table(NULL, (uint32_t)0x9d7, (uint32_t)0x12dcce5);
        baseband_write_mem_table(NULL, (uint32_t)0x9d8, (uint32_t)0x26af640);
        baseband_write_mem_table(NULL, (uint32_t)0x9d9, (uint32_t)0xadb277e);
        baseband_write_mem_table(NULL, (uint32_t)0x9da, (uint32_t)0x9769273);
        baseband_write_mem_table(NULL, (uint32_t)0x9db, (uint32_t)0x1398cd3);
        baseband_write_mem_table(NULL, (uint32_t)0x9dc, (uint32_t)0x240b5a4);
        baseband_write_mem_table(NULL, (uint32_t)0x9dd, (uint32_t)0xa8f688a);
        baseband_write_mem_table(NULL, (uint32_t)0x9de, (uint32_t)0x99b933d);
        baseband_write_mem_table(NULL, (uint32_t)0x9df, (uint32_t)0x1458cc1);
        baseband_write_mem_table(NULL, (uint32_t)0x9e0, (uint32_t)0x213f512);
        baseband_write_mem_table(NULL, (uint32_t)0x9e1, (uint32_t)0xa46e9a5);
        baseband_write_mem_table(NULL, (uint32_t)0x9e2, (uint32_t)0x9c21404);
        baseband_write_mem_table(NULL, (uint32_t)0x9e3, (uint32_t)0x1514cad);
        baseband_write_mem_table(NULL, (uint32_t)0x9e4, (uint32_t)0x1e4b48c);
        baseband_write_mem_table(NULL, (uint32_t)0x9e5, (uint32_t)0xa01aacf);
        baseband_write_mem_table(NULL, (uint32_t)0x9e6, (uint32_t)0x9ea54c6);
        baseband_write_mem_table(NULL, (uint32_t)0x9e7, (uint32_t)0x15d0c99);
        baseband_write_mem_table(NULL, (uint32_t)0x9e8, (uint32_t)0x1b37413);
        baseband_write_mem_table(NULL, (uint32_t)0x9e9, (uint32_t)0x9c02c07);
        baseband_write_mem_table(NULL, (uint32_t)0x9ea, (uint32_t)0xa141584);
        baseband_write_mem_table(NULL, (uint32_t)0x9eb, (uint32_t)0x168cc85);
        baseband_write_mem_table(NULL, (uint32_t)0x9ec, (uint32_t)0x17ff3a6);
        baseband_write_mem_table(NULL, (uint32_t)0x9ed, (uint32_t)0x9826d4c);
        baseband_write_mem_table(NULL, (uint32_t)0x9ee, (uint32_t)0xa3f563c);
        baseband_write_mem_table(NULL, (uint32_t)0x9ef, (uint32_t)0x1748c6f);
        baseband_write_mem_table(NULL, (uint32_t)0x9f0, (uint32_t)0x14ab348);
        baseband_write_mem_table(NULL, (uint32_t)0x9f1, (uint32_t)0x948ae9f);
        baseband_write_mem_table(NULL, (uint32_t)0x9f2, (uint32_t)0xa6c16f0);
        baseband_write_mem_table(NULL, (uint32_t)0x9f3, (uint32_t)0x1800c59);
        baseband_write_mem_table(NULL, (uint32_t)0x9f4, (uint32_t)0x113f2f7);
        baseband_write_mem_table(NULL, (uint32_t)0x9f5, (uint32_t)0x912effd);
        baseband_write_mem_table(NULL, (uint32_t)0x9f6, (uint32_t)0xa9a979f);
        baseband_write_mem_table(NULL, (uint32_t)0x9f7, (uint32_t)0x18bcc42);
        baseband_write_mem_table(NULL, (uint32_t)0x9f8, (uint32_t)0xdbb2b5);
        baseband_write_mem_table(NULL, (uint32_t)0x9f9, (uint32_t)0x8e1b167);
        baseband_write_mem_table(NULL, (uint32_t)0x9fa, (uint32_t)0xaca5848);
        baseband_write_mem_table(NULL, (uint32_t)0x9fb, (uint32_t)0x1974c2a);
        baseband_write_mem_table(NULL, (uint32_t)0x9fc, (uint32_t)0xa23282);
        baseband_write_mem_table(NULL, (uint32_t)0x9fd, (uint32_t)0x8b4f2db);
        baseband_write_mem_table(NULL, (uint32_t)0x9fe, (uint32_t)0xafb98ec);
        baseband_write_mem_table(NULL, (uint32_t)0x9ff, (uint32_t)0x1a2cc12);
        baseband_write_mem_table(NULL, (uint32_t)0xa00, (uint32_t)0x67f25e);
        baseband_write_mem_table(NULL, (uint32_t)0xa01, (uint32_t)0x88cf458);
        baseband_write_mem_table(NULL, (uint32_t)0xa02, (uint32_t)0xb2e198a);
        baseband_write_mem_table(NULL, (uint32_t)0xa03, (uint32_t)0x1ae0bf9);
        baseband_write_mem_table(NULL, (uint32_t)0xa04, (uint32_t)0x2d324a);
        baseband_write_mem_table(NULL, (uint32_t)0xa05, (uint32_t)0x86975de);
        baseband_write_mem_table(NULL, (uint32_t)0xa06, (uint32_t)0xb621a22);
        baseband_write_mem_table(NULL, (uint32_t)0xa07, (uint32_t)0x1b98bdf);
        baseband_write_mem_table(NULL, (uint32_t)0xa08, (uint32_t)0xff1f246);
        baseband_write_mem_table(NULL, (uint32_t)0xa09, (uint32_t)0x84b376c);
        baseband_write_mem_table(NULL, (uint32_t)0xa0a, (uint32_t)0xb975ab3);
        baseband_write_mem_table(NULL, (uint32_t)0xa0b, (uint32_t)0x1c4cbc4);
        baseband_write_mem_table(NULL, (uint32_t)0xa0c, (uint32_t)0xfb6b252);
        baseband_write_mem_table(NULL, (uint32_t)0xa0d, (uint32_t)0x831b901);
        baseband_write_mem_table(NULL, (uint32_t)0xa0e, (uint32_t)0xbcddb3e);
        baseband_write_mem_table(NULL, (uint32_t)0xa0f, (uint32_t)0x1d00ba9);
        baseband_write_mem_table(NULL, (uint32_t)0xa10, (uint32_t)0xf7bb26e);
        baseband_write_mem_table(NULL, (uint32_t)0xa11, (uint32_t)0x81d7a9b);
        baseband_write_mem_table(NULL, (uint32_t)0xa12, (uint32_t)0xc055bc3);
        baseband_write_mem_table(NULL, (uint32_t)0xa13, (uint32_t)0x1db0b8d);
        baseband_write_mem_table(NULL, (uint32_t)0xa14, (uint32_t)0xf41329a);
        baseband_write_mem_table(NULL, (uint32_t)0xa15, (uint32_t)0x80e7c39);
        baseband_write_mem_table(NULL, (uint32_t)0xa16, (uint32_t)0xc3e5c40);
        baseband_write_mem_table(NULL, (uint32_t)0xa17, (uint32_t)0x1e60b70);
        baseband_write_mem_table(NULL, (uint32_t)0xa18, (uint32_t)0xf0772d5);
        baseband_write_mem_table(NULL, (uint32_t)0xa19, (uint32_t)0x804bddb);
        baseband_write_mem_table(NULL, (uint32_t)0xa1a, (uint32_t)0xc781cb7);
        baseband_write_mem_table(NULL, (uint32_t)0xa1b, (uint32_t)0x1f10b52);
        baseband_write_mem_table(NULL, (uint32_t)0xa1c, (uint32_t)0xeceb321);
        baseband_write_mem_table(NULL, (uint32_t)0xa1d, (uint32_t)0x8007f7f);
        baseband_write_mem_table(NULL, (uint32_t)0xa1e, (uint32_t)0xcb31d26);
        baseband_write_mem_table(NULL, (uint32_t)0xa1f, (uint32_t)0x1fbcb34);
        baseband_write_mem_table(NULL, (uint32_t)0xa20, (uint32_t)0xe97337c);
        baseband_write_mem_table(NULL, (uint32_t)0xa21, (uint32_t)0x8014125);
        baseband_write_mem_table(NULL, (uint32_t)0xa22, (uint32_t)0xcef1d8e);
        baseband_write_mem_table(NULL, (uint32_t)0xa23, (uint32_t)0x2068b15);
        baseband_write_mem_table(NULL, (uint32_t)0xa24, (uint32_t)0xe6133e6);
        baseband_write_mem_table(NULL, (uint32_t)0xa25, (uint32_t)0x807c2cb);
        baseband_write_mem_table(NULL, (uint32_t)0xa26, (uint32_t)0xd2c1def);
        baseband_write_mem_table(NULL, (uint32_t)0xa27, (uint32_t)0x2114af5);
        baseband_write_mem_table(NULL, (uint32_t)0xa28, (uint32_t)0xe2d345f);
        baseband_write_mem_table(NULL, (uint32_t)0xa29, (uint32_t)0x813c46f);
        baseband_write_mem_table(NULL, (uint32_t)0xa2a, (uint32_t)0xd69de48);
        baseband_write_mem_table(NULL, (uint32_t)0xa2b, (uint32_t)0x21bcad5);
        baseband_write_mem_table(NULL, (uint32_t)0xa2c, (uint32_t)0xdfb34e7);
        baseband_write_mem_table(NULL, (uint32_t)0xa2d, (uint32_t)0x8254612);
        baseband_write_mem_table(NULL, (uint32_t)0xa2e, (uint32_t)0xda85e99);
        baseband_write_mem_table(NULL, (uint32_t)0xa2f, (uint32_t)0x2264ab4);
        baseband_write_mem_table(NULL, (uint32_t)0xa30, (uint32_t)0xdcb757c);
        baseband_write_mem_table(NULL, (uint32_t)0xa31, (uint32_t)0x83c07b1);
        baseband_write_mem_table(NULL, (uint32_t)0xa32, (uint32_t)0xde79ee2);
        baseband_write_mem_table(NULL, (uint32_t)0xa33, (uint32_t)0x230ca92);
        baseband_write_mem_table(NULL, (uint32_t)0xa34, (uint32_t)0xd9e761e);
        baseband_write_mem_table(NULL, (uint32_t)0xa35, (uint32_t)0x858494b);
        baseband_write_mem_table(NULL, (uint32_t)0xa36, (uint32_t)0xe279f23);
        baseband_write_mem_table(NULL, (uint32_t)0xa37, (uint32_t)0x23b0a6f);
        baseband_write_mem_table(NULL, (uint32_t)0xa38, (uint32_t)0xd7436cc);
        baseband_write_mem_table(NULL, (uint32_t)0xa39, (uint32_t)0x87a0ae0);
        baseband_write_mem_table(NULL, (uint32_t)0xa3a, (uint32_t)0xe681f5c);
        baseband_write_mem_table(NULL, (uint32_t)0xa3b, (uint32_t)0x2450a4c);
        baseband_write_mem_table(NULL, (uint32_t)0xa3c, (uint32_t)0xd4cf786);
        baseband_write_mem_table(NULL, (uint32_t)0xa3d, (uint32_t)0x8a10c6f);
        baseband_write_mem_table(NULL, (uint32_t)0xa3e, (uint32_t)0xea95f8c);
        baseband_write_mem_table(NULL, (uint32_t)0xa3f, (uint32_t)0x24f0a28);
        baseband_write_mem_table(NULL, (uint32_t)0xa40, (uint32_t)0xd28f84b);
        baseband_write_mem_table(NULL, (uint32_t)0xa41, (uint32_t)0x8cd0df5);
        baseband_write_mem_table(NULL, (uint32_t)0xa42, (uint32_t)0xeeadfb5);
        baseband_write_mem_table(NULL, (uint32_t)0xa43, (uint32_t)0x2590a04);
        baseband_write_mem_table(NULL, (uint32_t)0xa44, (uint32_t)0xd08791a);
        baseband_write_mem_table(NULL, (uint32_t)0xa45, (uint32_t)0x8fe4f72);
        baseband_write_mem_table(NULL, (uint32_t)0xa46, (uint32_t)0xf2d1fd4);
        baseband_write_mem_table(NULL, (uint32_t)0xa47, (uint32_t)0x262c9df);
        baseband_write_mem_table(NULL, (uint32_t)0xa48, (uint32_t)0xcebb9f2);
        baseband_write_mem_table(NULL, (uint32_t)0xa49, (uint32_t)0x934d0e5);
        baseband_write_mem_table(NULL, (uint32_t)0xa4a, (uint32_t)0xf6f5fec);
        baseband_write_mem_table(NULL, (uint32_t)0xa4b, (uint32_t)0x26c89b9);
        baseband_write_mem_table(NULL, (uint32_t)0xa4c, (uint32_t)0xcd2bad1);
        baseband_write_mem_table(NULL, (uint32_t)0xa4d, (uint32_t)0x970124c);
        baseband_write_mem_table(NULL, (uint32_t)0xa4e, (uint32_t)0xfb21ffa);
        baseband_write_mem_table(NULL, (uint32_t)0xa4f, (uint32_t)0x2760992);
        baseband_write_mem_table(NULL, (uint32_t)0xa50, (uint32_t)0xcbd7bb8);
        baseband_write_mem_table(NULL, (uint32_t)0xa51, (uint32_t)0x9afd3a8);
        baseband_write_mem_table(NULL, (uint32_t)0xa52, (uint32_t)0xff4dfff);
        baseband_write_mem_table(NULL, (uint32_t)0xa53, (uint32_t)0x27f496b);
        baseband_write_mem_table(NULL, (uint32_t)0xa54, (uint32_t)0xcac3ca4);
        baseband_write_mem_table(NULL, (uint32_t)0xa55, (uint32_t)0x9f494f6);
        baseband_write_mem_table(NULL, (uint32_t)0xa56, (uint32_t)0x37dffd);
        baseband_write_mem_table(NULL, (uint32_t)0xa57, (uint32_t)0x2888943);
        baseband_write_mem_table(NULL, (uint32_t)0xa58, (uint32_t)0xc9f3d95);
        baseband_write_mem_table(NULL, (uint32_t)0xa59, (uint32_t)0xa3d9635);
        baseband_write_mem_table(NULL, (uint32_t)0xa5a, (uint32_t)0x7adff1);
        baseband_write_mem_table(NULL, (uint32_t)0xa5b, (uint32_t)0x291c91a);
        baseband_write_mem_table(NULL, (uint32_t)0xa5c, (uint32_t)0xc96be89);
        baseband_write_mem_table(NULL, (uint32_t)0xa5d, (uint32_t)0xa8ad765);
        baseband_write_mem_table(NULL, (uint32_t)0xa5e, (uint32_t)0xbddfdd);
        baseband_write_mem_table(NULL, (uint32_t)0xa5f, (uint32_t)0x29ac8f1);
        baseband_write_mem_table(NULL, (uint32_t)0xa60, (uint32_t)0xc923f7f);
        baseband_write_mem_table(NULL, (uint32_t)0xa61, (uint32_t)0xadc1885);
        baseband_write_mem_table(NULL, (uint32_t)0xa62, (uint32_t)0x100dfbf);
        baseband_write_mem_table(NULL, (uint32_t)0xa63, (uint32_t)0x2a388c8);
        baseband_write_mem_table(NULL, (uint32_t)0xa64, (uint32_t)0xc91c077);
        baseband_write_mem_table(NULL, (uint32_t)0xa65, (uint32_t)0xb311993);
        baseband_write_mem_table(NULL, (uint32_t)0xa66, (uint32_t)0x1439f99);
        baseband_write_mem_table(NULL, (uint32_t)0xa67, (uint32_t)0x2ac489d);
        baseband_write_mem_table(NULL, (uint32_t)0xa68, (uint32_t)0xc96416e);
        baseband_write_mem_table(NULL, (uint32_t)0xa69, (uint32_t)0xb89da90);
        baseband_write_mem_table(NULL, (uint32_t)0xa6a, (uint32_t)0x1861f6a);
        baseband_write_mem_table(NULL, (uint32_t)0xa6b, (uint32_t)0x2b4c872);
        baseband_write_mem_table(NULL, (uint32_t)0xa6c, (uint32_t)0xc9ec264);
        baseband_write_mem_table(NULL, (uint32_t)0xa6d, (uint32_t)0xbe61b79);
        baseband_write_mem_table(NULL, (uint32_t)0xa6e, (uint32_t)0x1c81f32);
        baseband_write_mem_table(NULL, (uint32_t)0xa6f, (uint32_t)0x2bd0847);
        baseband_write_mem_table(NULL, (uint32_t)0xa70, (uint32_t)0xcabc357);
        baseband_write_mem_table(NULL, (uint32_t)0xa71, (uint32_t)0xc451c4f);
        baseband_write_mem_table(NULL, (uint32_t)0xa72, (uint32_t)0x209def2);
        baseband_write_mem_table(NULL, (uint32_t)0xa73, (uint32_t)0x2c5481b);
        baseband_write_mem_table(NULL, (uint32_t)0xa74, (uint32_t)0xcbd0446);
        baseband_write_mem_table(NULL, (uint32_t)0xa75, (uint32_t)0xca71d10);
        baseband_write_mem_table(NULL, (uint32_t)0xa76, (uint32_t)0x24b1ea8);
        baseband_write_mem_table(NULL, (uint32_t)0xa77, (uint32_t)0x2cd47ee);
        baseband_write_mem_table(NULL, (uint32_t)0xa78, (uint32_t)0xcd28530);
        baseband_write_mem_table(NULL, (uint32_t)0xa79, (uint32_t)0xd0bddbd);
        baseband_write_mem_table(NULL, (uint32_t)0xa7a, (uint32_t)0x28bde56);
        baseband_write_mem_table(NULL, (uint32_t)0xa7b, (uint32_t)0x2d507c1);
        baseband_write_mem_table(NULL, (uint32_t)0xa7c, (uint32_t)0xcec0613);
        baseband_write_mem_table(NULL, (uint32_t)0xa7d, (uint32_t)0xd72de54);
        baseband_write_mem_table(NULL, (uint32_t)0xa7e, (uint32_t)0x2cbddfb);
        baseband_write_mem_table(NULL, (uint32_t)0xa7f, (uint32_t)0x2dcc793);
        baseband_write_mem_table(NULL, (uint32_t)0xa80, (uint32_t)0xd09c6ef);
        baseband_write_mem_table(NULL, (uint32_t)0xa81, (uint32_t)0xddb9ed5);
        baseband_write_mem_table(NULL, (uint32_t)0xa82, (uint32_t)0x30b5d98);
        baseband_write_mem_table(NULL, (uint32_t)0xa83, (uint32_t)0x2e44765);
        baseband_write_mem_table(NULL, (uint32_t)0xa84, (uint32_t)0xd2b47c3);
        baseband_write_mem_table(NULL, (uint32_t)0xa85, (uint32_t)0xe465f3f);
        baseband_write_mem_table(NULL, (uint32_t)0xa86, (uint32_t)0x349dd2c);
        baseband_write_mem_table(NULL, (uint32_t)0xa87, (uint32_t)0x2ebc736);
        baseband_write_mem_table(NULL, (uint32_t)0xa88, (uint32_t)0xd50888c);
        baseband_write_mem_table(NULL, (uint32_t)0xa89, (uint32_t)0xeb25f92);
        baseband_write_mem_table(NULL, (uint32_t)0xa8a, (uint32_t)0x3879cb8);
        baseband_write_mem_table(NULL, (uint32_t)0xa8b, (uint32_t)0x2f30706);
        baseband_write_mem_table(NULL, (uint32_t)0xa8c, (uint32_t)0xd79494b);
        baseband_write_mem_table(NULL, (uint32_t)0xa8d, (uint32_t)0xf1f9fcf);
        baseband_write_mem_table(NULL, (uint32_t)0xa8e, (uint32_t)0x3c45c3b);
        baseband_write_mem_table(NULL, (uint32_t)0xa8f, (uint32_t)0x2fa06d7);
        baseband_write_mem_table(NULL, (uint32_t)0xa90, (uint32_t)0xda549fd);
        baseband_write_mem_table(NULL, (uint32_t)0xa91, (uint32_t)0xf8d5ff3);
        baseband_write_mem_table(NULL, (uint32_t)0xa92, (uint32_t)0x4001bb6);
        baseband_write_mem_table(NULL, (uint32_t)0xa93, (uint32_t)0x300c6a6);
        baseband_write_mem_table(NULL, (uint32_t)0xa94, (uint32_t)0xdd48aa3);
        baseband_write_mem_table(NULL, (uint32_t)0xa95, (uint32_t)0xffbdfff);
        baseband_write_mem_table(NULL, (uint32_t)0xa96, (uint32_t)0x43adb2a);
        baseband_write_mem_table(NULL, (uint32_t)0xa97, (uint32_t)0x3074675);
        baseband_write_mem_table(NULL, (uint32_t)0xa98, (uint32_t)0xe06cb3b);
        baseband_write_mem_table(NULL, (uint32_t)0xa99, (uint32_t)0x6a1ff5);
        baseband_write_mem_table(NULL, (uint32_t)0xa9a, (uint32_t)0x4745a95);
        baseband_write_mem_table(NULL, (uint32_t)0xa9b, (uint32_t)0x30dc644);
        baseband_write_mem_table(NULL, (uint32_t)0xa9c, (uint32_t)0xe3b8bc4);
        baseband_write_mem_table(NULL, (uint32_t)0xa9d, (uint32_t)0xd89fd2);
        baseband_write_mem_table(NULL, (uint32_t)0xa9e, (uint32_t)0x4ac99f8);
        baseband_write_mem_table(NULL, (uint32_t)0xa9f, (uint32_t)0x3140612);
        baseband_write_mem_table(NULL, (uint32_t)0xaa0, (uint32_t)0xe728c3e);
        baseband_write_mem_table(NULL, (uint32_t)0xaa1, (uint32_t)0x1465f97);
        baseband_write_mem_table(NULL, (uint32_t)0xaa2, (uint32_t)0x4e3d954);
        baseband_write_mem_table(NULL, (uint32_t)0xaa3, (uint32_t)0x31a05e0);
        baseband_write_mem_table(NULL, (uint32_t)0xaa4, (uint32_t)0xeabcca8);
        baseband_write_mem_table(NULL, (uint32_t)0xaa5, (uint32_t)0x1b31f45);
        baseband_write_mem_table(NULL, (uint32_t)0xaa6, (uint32_t)0x51958a8);
        baseband_write_mem_table(NULL, (uint32_t)0xaa7, (uint32_t)0x32005ae);
        baseband_write_mem_table(NULL, (uint32_t)0xaa8, (uint32_t)0xee6cd02);
        baseband_write_mem_table(NULL, (uint32_t)0xaa9, (uint32_t)0x21ededb);
        baseband_write_mem_table(NULL, (uint32_t)0xaaa, (uint32_t)0x54d97f6);
        baseband_write_mem_table(NULL, (uint32_t)0xaab, (uint32_t)0x325c57a);
        baseband_write_mem_table(NULL, (uint32_t)0xaac, (uint32_t)0xf238d4a);
        baseband_write_mem_table(NULL, (uint32_t)0xaad, (uint32_t)0x2891e5a);
        baseband_write_mem_table(NULL, (uint32_t)0xaae, (uint32_t)0x580573b);
        baseband_write_mem_table(NULL, (uint32_t)0xaaf, (uint32_t)0x32b0547);
        baseband_write_mem_table(NULL, (uint32_t)0xab0, (uint32_t)0xf614d81);
        baseband_write_mem_table(NULL, (uint32_t)0xab1, (uint32_t)0x2f19dc1);
        baseband_write_mem_table(NULL, (uint32_t)0xab2, (uint32_t)0x5b1967b);
        baseband_write_mem_table(NULL, (uint32_t)0xab3, (uint32_t)0x3308513);
        baseband_write_mem_table(NULL, (uint32_t)0xab4, (uint32_t)0xf9fcda5);
        baseband_write_mem_table(NULL, (uint32_t)0xab5, (uint32_t)0x357dd13);
        baseband_write_mem_table(NULL, (uint32_t)0xab6, (uint32_t)0x5e155b3);
        baseband_write_mem_table(NULL, (uint32_t)0xab7, (uint32_t)0x33584df);
        baseband_write_mem_table(NULL, (uint32_t)0xab8, (uint32_t)0xfdf0db8);
        baseband_write_mem_table(NULL, (uint32_t)0xab9, (uint32_t)0x3bb9c4e);
        baseband_write_mem_table(NULL, (uint32_t)0xaba, (uint32_t)0x60f54e5);
        baseband_write_mem_table(NULL, (uint32_t)0xabb, (uint32_t)0x33a44aa);
        baseband_write_mem_table(NULL, (uint32_t)0xabc, (uint32_t)0x1e8db8);
        baseband_write_mem_table(NULL, (uint32_t)0xabd, (uint32_t)0x41cdb73);
        baseband_write_mem_table(NULL, (uint32_t)0xabe, (uint32_t)0x63b5411);
        baseband_write_mem_table(NULL, (uint32_t)0xabf, (uint32_t)0x33f0476);
        baseband_write_mem_table(NULL, (uint32_t)0xac0, (uint32_t)0x5dcda6);
        baseband_write_mem_table(NULL, (uint32_t)0xac1, (uint32_t)0x47ada83);
        baseband_write_mem_table(NULL, (uint32_t)0xac2, (uint32_t)0x665d336);
        baseband_write_mem_table(NULL, (uint32_t)0xac3, (uint32_t)0x3438440);
        baseband_write_mem_table(NULL, (uint32_t)0xac4, (uint32_t)0x9c8d82);
        baseband_write_mem_table(NULL, (uint32_t)0xac5, (uint32_t)0x4d59980);
        baseband_write_mem_table(NULL, (uint32_t)0xac6, (uint32_t)0x68e9256);
        baseband_write_mem_table(NULL, (uint32_t)0xac7, (uint32_t)0x347c40b);
        baseband_write_mem_table(NULL, (uint32_t)0xac8, (uint32_t)0xda8d4c);
        baseband_write_mem_table(NULL, (uint32_t)0xac9, (uint32_t)0x52c9868);
        baseband_write_mem_table(NULL, (uint32_t)0xaca, (uint32_t)0x6b51171);
        baseband_write_mem_table(NULL, (uint32_t)0xacb, (uint32_t)0x34bc3d5);
        baseband_write_mem_table(NULL, (uint32_t)0xacc, (uint32_t)0x1178d04);
        baseband_write_mem_table(NULL, (uint32_t)0xacd, (uint32_t)0x57fd73e);
        baseband_write_mem_table(NULL, (uint32_t)0xace, (uint32_t)0x6d9d086);
        baseband_write_mem_table(NULL, (uint32_t)0xacf, (uint32_t)0x34f839f);
        baseband_write_mem_table(NULL, (uint32_t)0xad0, (uint32_t)0x1530caa);
        baseband_write_mem_table(NULL, (uint32_t)0xad1, (uint32_t)0x5ced602);
        baseband_write_mem_table(NULL, (uint32_t)0xad2, (uint32_t)0x6fc8f96);
        baseband_write_mem_table(NULL, (uint32_t)0xad3, (uint32_t)0x3534368);
        baseband_write_mem_table(NULL, (uint32_t)0xad4, (uint32_t)0x18ccc40);
        baseband_write_mem_table(NULL, (uint32_t)0xad5, (uint32_t)0x61994b5);
        baseband_write_mem_table(NULL, (uint32_t)0xad6, (uint32_t)0x71d4ea2);
        baseband_write_mem_table(NULL, (uint32_t)0xad7, (uint32_t)0x3568332);
        baseband_write_mem_table(NULL, (uint32_t)0xad8, (uint32_t)0x1c48bc4);
        baseband_write_mem_table(NULL, (uint32_t)0xad9, (uint32_t)0x65f9358);
        baseband_write_mem_table(NULL, (uint32_t)0xada, (uint32_t)0x73c0da9);
        baseband_write_mem_table(NULL, (uint32_t)0xadb, (uint32_t)0x359c2fb);
        baseband_write_mem_table(NULL, (uint32_t)0xadc, (uint32_t)0x1fa0b39);
        baseband_write_mem_table(NULL, (uint32_t)0xadd, (uint32_t)0x6a0d1ec);
        baseband_write_mem_table(NULL, (uint32_t)0xade, (uint32_t)0x7588cac);
        baseband_write_mem_table(NULL, (uint32_t)0xadf, (uint32_t)0x35cc2c4);
        baseband_write_mem_table(NULL, (uint32_t)0xae0, (uint32_t)0x22cca9f);
        baseband_write_mem_table(NULL, (uint32_t)0xae1, (uint32_t)0x6dcd072);
        baseband_write_mem_table(NULL, (uint32_t)0xae2, (uint32_t)0x7730bab);
        baseband_write_mem_table(NULL, (uint32_t)0xae3, (uint32_t)0x35f428c);
        baseband_write_mem_table(NULL, (uint32_t)0xae4, (uint32_t)0x25c89f6);
        baseband_write_mem_table(NULL, (uint32_t)0xae5, (uint32_t)0x713ceec);
        baseband_write_mem_table(NULL, (uint32_t)0xae6, (uint32_t)0x78b4aa7);
        baseband_write_mem_table(NULL, (uint32_t)0xae7, (uint32_t)0x361c255);
        baseband_write_mem_table(NULL, (uint32_t)0xae8, (uint32_t)0x2894940);
        baseband_write_mem_table(NULL, (uint32_t)0xae9, (uint32_t)0x7454d5a);
        baseband_write_mem_table(NULL, (uint32_t)0xaea, (uint32_t)0x7a1499f);
        baseband_write_mem_table(NULL, (uint32_t)0xaeb, (uint32_t)0x364421d);
        baseband_write_mem_table(NULL, (uint32_t)0xaec, (uint32_t)0x2b2887e);
        baseband_write_mem_table(NULL, (uint32_t)0xaed, (uint32_t)0x7714bbd);
        baseband_write_mem_table(NULL, (uint32_t)0xaee, (uint32_t)0x7b50894);
        baseband_write_mem_table(NULL, (uint32_t)0xaef, (uint32_t)0x36641e5);
        baseband_write_mem_table(NULL, (uint32_t)0xaf0, (uint32_t)0x2d807af);
        baseband_write_mem_table(NULL, (uint32_t)0xaf1, (uint32_t)0x7978a18);
        baseband_write_mem_table(NULL, (uint32_t)0xaf2, (uint32_t)0x7c68787);
        baseband_write_mem_table(NULL, (uint32_t)0xaf3, (uint32_t)0x36801ae);
        baseband_write_mem_table(NULL, (uint32_t)0xaf4, (uint32_t)0x2f9c6d7);
        baseband_write_mem_table(NULL, (uint32_t)0xaf5, (uint32_t)0x7b8086a);
        baseband_write_mem_table(NULL, (uint32_t)0xaf6, (uint32_t)0x7d5c678);
        baseband_write_mem_table(NULL, (uint32_t)0xaf7, (uint32_t)0x369c175);
        baseband_write_mem_table(NULL, (uint32_t)0xaf8, (uint32_t)0x317c5f5);
        baseband_write_mem_table(NULL, (uint32_t)0xaf9, (uint32_t)0x7d286b6);
        baseband_write_mem_table(NULL, (uint32_t)0xafa, (uint32_t)0x7e2c566);
        baseband_write_mem_table(NULL, (uint32_t)0xafb, (uint32_t)0x36b013d);
        baseband_write_mem_table(NULL, (uint32_t)0xafc, (uint32_t)0x331450b);
        baseband_write_mem_table(NULL, (uint32_t)0xafd, (uint32_t)0x7e704fd);
        baseband_write_mem_table(NULL, (uint32_t)0xafe, (uint32_t)0x7ed4453);
        baseband_write_mem_table(NULL, (uint32_t)0xaff, (uint32_t)0x36c4105);
        baseband_write_mem_table(NULL, (uint32_t)0xb00, (uint32_t)0x346841a);
        baseband_write_mem_table(NULL, (uint32_t)0xb01, (uint32_t)0x7f5833f);
        baseband_write_mem_table(NULL, (uint32_t)0xb02, (uint32_t)0x7f5833f);
        baseband_write_mem_table(NULL, (uint32_t)0xb03, (uint32_t)0x36d40cd);
        baseband_write_mem_table(NULL, (uint32_t)0xb04, (uint32_t)0x3574323);
        baseband_write_mem_table(NULL, (uint32_t)0xb05, (uint32_t)0x7fdc180);
        baseband_write_mem_table(NULL, (uint32_t)0xb06, (uint32_t)0x7fb4229);
        baseband_write_mem_table(NULL, (uint32_t)0xb07, (uint32_t)0x36dc094);
        baseband_write_mem_table(NULL, (uint32_t)0xb08, (uint32_t)0x363c229);
        baseband_write_mem_table(NULL, (uint32_t)0xb09, (uint32_t)0x7ffffbf);
        baseband_write_mem_table(NULL, (uint32_t)0xb0a, (uint32_t)0x7fec113);
        baseband_write_mem_table(NULL, (uint32_t)0xb0b, (uint32_t)0x36e405c);
        baseband_write_mem_table(NULL, (uint32_t)0xb0c, (uint32_t)0x36b812b);
        baseband_write_mem_table(NULL, (uint32_t)0xb0d, (uint32_t)0x7fc3dfe);
        baseband_write_mem_table(NULL, (uint32_t)0xb0e, (uint32_t)0x7fffffd);
        baseband_write_mem_table(NULL, (uint32_t)0xb0f, (uint32_t)0x36e8023);
        baseband_write_mem_table(NULL, (uint32_t)0xb10, (uint32_t)0x36e802c);
        baseband_write_mem_table(NULL, (uint32_t)0xb11, (uint32_t)0x7f1fc3f);
        baseband_write_mem_table(NULL, (uint32_t)0xb12, (uint32_t)0x7fefee6);
        baseband_write_mem_table(NULL, (uint32_t)0xb13, (uint32_t)0x36ebfeb);
        baseband_write_mem_table(NULL, (uint32_t)0xb14, (uint32_t)0x36d3f2c);
        baseband_write_mem_table(NULL, (uint32_t)0xb15, (uint32_t)0x7e1ba82);
        baseband_write_mem_table(NULL, (uint32_t)0xb16, (uint32_t)0x7fb7dd0);
        baseband_write_mem_table(NULL, (uint32_t)0xb17, (uint32_t)0x36ebfb2);
        baseband_write_mem_table(NULL, (uint32_t)0xb18, (uint32_t)0x366fe2e);
        baseband_write_mem_table(NULL, (uint32_t)0xb19, (uint32_t)0x7cb78ca);
        baseband_write_mem_table(NULL, (uint32_t)0xb1a, (uint32_t)0x7f57cbb);
        baseband_write_mem_table(NULL, (uint32_t)0xb1b, (uint32_t)0x36e3f7a);
        baseband_write_mem_table(NULL, (uint32_t)0xb1c, (uint32_t)0x35c3d32);
        baseband_write_mem_table(NULL, (uint32_t)0xb1d, (uint32_t)0x7af3717);
        baseband_write_mem_table(NULL, (uint32_t)0xb1e, (uint32_t)0x7ed3ba6);
        baseband_write_mem_table(NULL, (uint32_t)0xb1f, (uint32_t)0x36d7f41);
        baseband_write_mem_table(NULL, (uint32_t)0xb20, (uint32_t)0x34cfc3a);
        baseband_write_mem_table(NULL, (uint32_t)0xb21, (uint32_t)0x78cf56c);
        baseband_write_mem_table(NULL, (uint32_t)0xb22, (uint32_t)0x7e27a93);
        baseband_write_mem_table(NULL, (uint32_t)0xb23, (uint32_t)0x36cbf09);
        baseband_write_mem_table(NULL, (uint32_t)0xb24, (uint32_t)0x3393b47);
        baseband_write_mem_table(NULL, (uint32_t)0xb25, (uint32_t)0x764f3c8);
        baseband_write_mem_table(NULL, (uint32_t)0xb26, (uint32_t)0x7d5b982);
        baseband_write_mem_table(NULL, (uint32_t)0xb27, (uint32_t)0x36b7ed0);
        baseband_write_mem_table(NULL, (uint32_t)0xb28, (uint32_t)0x320fa5a);
        baseband_write_mem_table(NULL, (uint32_t)0xb29, (uint32_t)0x737722e);
        baseband_write_mem_table(NULL, (uint32_t)0xb2a, (uint32_t)0x7c63872);
        baseband_write_mem_table(NULL, (uint32_t)0xb2b, (uint32_t)0x36a3e98);
        baseband_write_mem_table(NULL, (uint32_t)0xb2c, (uint32_t)0x304b975);
        baseband_write_mem_table(NULL, (uint32_t)0xb2d, (uint32_t)0x704309f);
        baseband_write_mem_table(NULL, (uint32_t)0xb2e, (uint32_t)0x7b4b765);
        baseband_write_mem_table(NULL, (uint32_t)0xb2f, (uint32_t)0x368be60);
        baseband_write_mem_table(NULL, (uint32_t)0xb30, (uint32_t)0x2e43899);
        baseband_write_mem_table(NULL, (uint32_t)0xb31, (uint32_t)0x6cbaf1b);
        baseband_write_mem_table(NULL, (uint32_t)0xb32, (uint32_t)0x7a0f65a);
        baseband_write_mem_table(NULL, (uint32_t)0xb33, (uint32_t)0x366fe28);
        baseband_write_mem_table(NULL, (uint32_t)0xb34, (uint32_t)0x2bff7c8);
        baseband_write_mem_table(NULL, (uint32_t)0xb35, (uint32_t)0x68dada5);
        baseband_write_mem_table(NULL, (uint32_t)0xb36, (uint32_t)0x78ab553);
        baseband_write_mem_table(NULL, (uint32_t)0xb37, (uint32_t)0x364fdf0);
        baseband_write_mem_table(NULL, (uint32_t)0xb38, (uint32_t)0x297f701);
        baseband_write_mem_table(NULL, (uint32_t)0xb39, (uint32_t)0x64aec3d);
        baseband_write_mem_table(NULL, (uint32_t)0xb3a, (uint32_t)0x772744e);
        baseband_write_mem_table(NULL, (uint32_t)0xb3b, (uint32_t)0x362bdb8);
        baseband_write_mem_table(NULL, (uint32_t)0xb3c, (uint32_t)0x26c7646);
        baseband_write_mem_table(NULL, (uint32_t)0xb3d, (uint32_t)0x6036ae4);
        baseband_write_mem_table(NULL, (uint32_t)0xb3e, (uint32_t)0x757f34d);
        baseband_write_mem_table(NULL, (uint32_t)0xb3f, (uint32_t)0x3603d81);
        baseband_write_mem_table(NULL, (uint32_t)0xb40, (uint32_t)0x23db599);
        baseband_write_mem_table(NULL, (uint32_t)0xb41, (uint32_t)0x5b7299b);
        baseband_write_mem_table(NULL, (uint32_t)0xb42, (uint32_t)0x73b7250);
        baseband_write_mem_table(NULL, (uint32_t)0xb43, (uint32_t)0x35d7d49);
        baseband_write_mem_table(NULL, (uint32_t)0xb44, (uint32_t)0x20bb4fa);
        baseband_write_mem_table(NULL, (uint32_t)0xb45, (uint32_t)0x566a864);
        baseband_write_mem_table(NULL, (uint32_t)0xb46, (uint32_t)0x71cb157);
        baseband_write_mem_table(NULL, (uint32_t)0xb47, (uint32_t)0x35abd12);
        baseband_write_mem_table(NULL, (uint32_t)0xb48, (uint32_t)0x1d73469);
        baseband_write_mem_table(NULL, (uint32_t)0xb49, (uint32_t)0x512273f);
        baseband_write_mem_table(NULL, (uint32_t)0xb4a, (uint32_t)0x6fbb062);
        baseband_write_mem_table(NULL, (uint32_t)0xb4b, (uint32_t)0x3577cdb);
        baseband_write_mem_table(NULL, (uint32_t)0xb4c, (uint32_t)0x1a073e9);
        baseband_write_mem_table(NULL, (uint32_t)0xb4d, (uint32_t)0x4b9a62d);
        baseband_write_mem_table(NULL, (uint32_t)0xb4e, (uint32_t)0x6d8ef72);
        baseband_write_mem_table(NULL, (uint32_t)0xb4f, (uint32_t)0x3543ca5);
        baseband_write_mem_table(NULL, (uint32_t)0xb50, (uint32_t)0x1673378);
        baseband_write_mem_table(NULL, (uint32_t)0xb51, (uint32_t)0x45da52f);
        baseband_write_mem_table(NULL, (uint32_t)0xb52, (uint32_t)0x6b3ee88);
        baseband_write_mem_table(NULL, (uint32_t)0xb53, (uint32_t)0x350bc6e);
        baseband_write_mem_table(NULL, (uint32_t)0xb54, (uint32_t)0x12c3319);
        baseband_write_mem_table(NULL, (uint32_t)0xb55, (uint32_t)0x3fe6445);
        baseband_write_mem_table(NULL, (uint32_t)0xb56, (uint32_t)0x68d2da2);
        baseband_write_mem_table(NULL, (uint32_t)0xb57, (uint32_t)0x34cfc38);
        baseband_write_mem_table(NULL, (uint32_t)0xb58, (uint32_t)0xefb2cb);
        baseband_write_mem_table(NULL, (uint32_t)0xb59, (uint32_t)0x39be371);
        baseband_write_mem_table(NULL, (uint32_t)0xb5a, (uint32_t)0x6646cc2);
        baseband_write_mem_table(NULL, (uint32_t)0xb5b, (uint32_t)0x348fc02);
        baseband_write_mem_table(NULL, (uint32_t)0xb5c, (uint32_t)0xb2328e);
        baseband_write_mem_table(NULL, (uint32_t)0xb5d, (uint32_t)0x33722b2);
        baseband_write_mem_table(NULL, (uint32_t)0xb5e, (uint32_t)0x639ebe7);
        baseband_write_mem_table(NULL, (uint32_t)0xb5f, (uint32_t)0x344bbcc);
        baseband_write_mem_table(NULL, (uint32_t)0xb60, (uint32_t)0x737264);
        baseband_write_mem_table(NULL, (uint32_t)0xb61, (uint32_t)0x2cfa20a);
        baseband_write_mem_table(NULL, (uint32_t)0xb62, (uint32_t)0x60dab13);
        baseband_write_mem_table(NULL, (uint32_t)0xb63, (uint32_t)0x3403b97);
        baseband_write_mem_table(NULL, (uint32_t)0xb64, (uint32_t)0x34724c);
        baseband_write_mem_table(NULL, (uint32_t)0xb65, (uint32_t)0x2666179);
        baseband_write_mem_table(NULL, (uint32_t)0xb66, (uint32_t)0x5df6a45);
        baseband_write_mem_table(NULL, (uint32_t)0xb67, (uint32_t)0x33bbb62);
        baseband_write_mem_table(NULL, (uint32_t)0xb68, (uint32_t)0xff4f246);
        baseband_write_mem_table(NULL, (uint32_t)0xb69, (uint32_t)0x1fb20ff);
        baseband_write_mem_table(NULL, (uint32_t)0xb6a, (uint32_t)0x5afa97d);
        baseband_write_mem_table(NULL, (uint32_t)0xb6b, (uint32_t)0x336bb2d);
        baseband_write_mem_table(NULL, (uint32_t)0xb6c, (uint32_t)0xfb5b252);
        baseband_write_mem_table(NULL, (uint32_t)0xb6d, (uint32_t)0x18ea09d);
        baseband_write_mem_table(NULL, (uint32_t)0xb6e, (uint32_t)0x57e68bc);
        baseband_write_mem_table(NULL, (uint32_t)0xb6f, (uint32_t)0x331baf9);
        baseband_write_mem_table(NULL, (uint32_t)0xb70, (uint32_t)0xf76f271);
        baseband_write_mem_table(NULL, (uint32_t)0xb71, (uint32_t)0x120e052);
        baseband_write_mem_table(NULL, (uint32_t)0xb72, (uint32_t)0x54b6802);
        baseband_write_mem_table(NULL, (uint32_t)0xb73, (uint32_t)0x32c7ac5);
        baseband_write_mem_table(NULL, (uint32_t)0xb74, (uint32_t)0xf38b2a1);
        baseband_write_mem_table(NULL, (uint32_t)0xb75, (uint32_t)0xb2a01f);
        baseband_write_mem_table(NULL, (uint32_t)0xb76, (uint32_t)0x517274f);
        baseband_write_mem_table(NULL, (uint32_t)0xb77, (uint32_t)0x3273a91);
        baseband_write_mem_table(NULL, (uint32_t)0xb78, (uint32_t)0xefbb2e3);
        baseband_write_mem_table(NULL, (uint32_t)0xb79, (uint32_t)0x43a004);
        baseband_write_mem_table(NULL, (uint32_t)0xb7a, (uint32_t)0x4e126a4);
        baseband_write_mem_table(NULL, (uint32_t)0xb7b, (uint32_t)0x3217a5e);
        baseband_write_mem_table(NULL, (uint32_t)0xb7c, (uint32_t)0xec03337);
        baseband_write_mem_table(NULL, (uint32_t)0xb7d, (uint32_t)0xfd4a002);
        baseband_write_mem_table(NULL, (uint32_t)0xb7e, (uint32_t)0x4a9e600);
        baseband_write_mem_table(NULL, (uint32_t)0xb7f, (uint32_t)0x31bba2c);
        baseband_write_mem_table(NULL, (uint32_t)0xb80, (uint32_t)0xe86339b);
        baseband_write_mem_table(NULL, (uint32_t)0xb81, (uint32_t)0xf65e017);
        baseband_write_mem_table(NULL, (uint32_t)0xb82, (uint32_t)0x4716563);
        baseband_write_mem_table(NULL, (uint32_t)0xb83, (uint32_t)0x315b9f9);
        baseband_write_mem_table(NULL, (uint32_t)0xb84, (uint32_t)0xe4e7410);
        baseband_write_mem_table(NULL, (uint32_t)0xb85, (uint32_t)0xef7e044);
        baseband_write_mem_table(NULL, (uint32_t)0xb86, (uint32_t)0x437a4cf);
        baseband_write_mem_table(NULL, (uint32_t)0xb87, (uint32_t)0x30f79c7);
        baseband_write_mem_table(NULL, (uint32_t)0xb88, (uint32_t)0xe18b494);
        baseband_write_mem_table(NULL, (uint32_t)0xb89, (uint32_t)0xe8aa089);
        baseband_write_mem_table(NULL, (uint32_t)0xb8a, (uint32_t)0x3fce442);
        baseband_write_mem_table(NULL, (uint32_t)0xb8b, (uint32_t)0x308f996);
        baseband_write_mem_table(NULL, (uint32_t)0xb8c, (uint32_t)0xde5b527);
        baseband_write_mem_table(NULL, (uint32_t)0xb8d, (uint32_t)0xe1e60e6);
        baseband_write_mem_table(NULL, (uint32_t)0xb8e, (uint32_t)0x3c0e3bd);
        baseband_write_mem_table(NULL, (uint32_t)0xb8f, (uint32_t)0x3027965);
        baseband_write_mem_table(NULL, (uint32_t)0xb90, (uint32_t)0xdb575c8);
        baseband_write_mem_table(NULL, (uint32_t)0xb91, (uint32_t)0xdb3e159);
        baseband_write_mem_table(NULL, (uint32_t)0xb92, (uint32_t)0x383e341);
        baseband_write_mem_table(NULL, (uint32_t)0xb93, (uint32_t)0x2fbb934);
        baseband_write_mem_table(NULL, (uint32_t)0xb94, (uint32_t)0xd883676);
        baseband_write_mem_table(NULL, (uint32_t)0xb95, (uint32_t)0xd4b21e3);
        baseband_write_mem_table(NULL, (uint32_t)0xb96, (uint32_t)0x345e2cd);
        baseband_write_mem_table(NULL, (uint32_t)0xb97, (uint32_t)0x2f4b904);
        baseband_write_mem_table(NULL, (uint32_t)0xb98, (uint32_t)0xd5e7730);
        baseband_write_mem_table(NULL, (uint32_t)0xb99, (uint32_t)0xce4a283);
        baseband_write_mem_table(NULL, (uint32_t)0xb9a, (uint32_t)0x3072261);
        baseband_write_mem_table(NULL, (uint32_t)0xb9b, (uint32_t)0x2ed78d5);
        baseband_write_mem_table(NULL, (uint32_t)0xb9c, (uint32_t)0xd37f7f6);
        baseband_write_mem_table(NULL, (uint32_t)0xb9d, (uint32_t)0xc806339);
        baseband_write_mem_table(NULL, (uint32_t)0xb9e, (uint32_t)0x2c7a1fe);
        baseband_write_mem_table(NULL, (uint32_t)0xb9f, (uint32_t)0x2e638a6);
        baseband_write_mem_table(NULL, (uint32_t)0xba0, (uint32_t)0xd1538c6);
        baseband_write_mem_table(NULL, (uint32_t)0xba1, (uint32_t)0xc1ee404);
        baseband_write_mem_table(NULL, (uint32_t)0xba2, (uint32_t)0x28761a4);
        baseband_write_mem_table(NULL, (uint32_t)0xba3, (uint32_t)0x2deb877);
        baseband_write_mem_table(NULL, (uint32_t)0xba4, (uint32_t)0xcf6399f);
        baseband_write_mem_table(NULL, (uint32_t)0xba5, (uint32_t)0xbc064e3);
        baseband_write_mem_table(NULL, (uint32_t)0xba6, (uint32_t)0x2466152);
        baseband_write_mem_table(NULL, (uint32_t)0xba7, (uint32_t)0x2d6f849);
        baseband_write_mem_table(NULL, (uint32_t)0xba8, (uint32_t)0xcdb3a80);
        baseband_write_mem_table(NULL, (uint32_t)0xba9, (uint32_t)0xb6525d6);
        baseband_write_mem_table(NULL, (uint32_t)0xbaa, (uint32_t)0x204e109);
        baseband_write_mem_table(NULL, (uint32_t)0xbab, (uint32_t)0x2cf381c);
        baseband_write_mem_table(NULL, (uint32_t)0xbac, (uint32_t)0xcc43b68);
        baseband_write_mem_table(NULL, (uint32_t)0xbad, (uint32_t)0xb0d26db);
        baseband_write_mem_table(NULL, (uint32_t)0xbae, (uint32_t)0x1c2e0c9);
        baseband_write_mem_table(NULL, (uint32_t)0xbaf, (uint32_t)0x2c737ef);
        baseband_write_mem_table(NULL, (uint32_t)0xbb0, (uint32_t)0xcb17c55);
        baseband_write_mem_table(NULL, (uint32_t)0xbb1, (uint32_t)0xab927f3);
        baseband_write_mem_table(NULL, (uint32_t)0xbb2, (uint32_t)0x180a092);
        baseband_write_mem_table(NULL, (uint32_t)0xbb3, (uint32_t)0x2bef7c3);
        baseband_write_mem_table(NULL, (uint32_t)0xbb4, (uint32_t)0xca2fd47);
        baseband_write_mem_table(NULL, (uint32_t)0xbb5, (uint32_t)0xa69291c);
        baseband_write_mem_table(NULL, (uint32_t)0xbb6, (uint32_t)0x13de063);
        baseband_write_mem_table(NULL, (uint32_t)0xbb7, (uint32_t)0x2b6b797);
        baseband_write_mem_table(NULL, (uint32_t)0xbb8, (uint32_t)0xc98fe3c);
        baseband_write_mem_table(NULL, (uint32_t)0xbb9, (uint32_t)0xa1d2a55);
        baseband_write_mem_table(NULL, (uint32_t)0xbba, (uint32_t)0xfae03e);
        baseband_write_mem_table(NULL, (uint32_t)0xbbb, (uint32_t)0x2ae376c);
        baseband_write_mem_table(NULL, (uint32_t)0xbbc, (uint32_t)0xc92ff32);
        baseband_write_mem_table(NULL, (uint32_t)0xbbd, (uint32_t)0x9d5ab9d);
        baseband_write_mem_table(NULL, (uint32_t)0xbbe, (uint32_t)0xb7e021);
        baseband_write_mem_table(NULL, (uint32_t)0xbbf, (uint32_t)0x2a57741);
        baseband_write_mem_table(NULL, (uint32_t)0xbc0, (uint32_t)0xc91802a);
        baseband_write_mem_table(NULL, (uint32_t)0xbc1, (uint32_t)0x992acf3);
        baseband_write_mem_table(NULL, (uint32_t)0xbc2, (uint32_t)0x74a00d);
        baseband_write_mem_table(NULL, (uint32_t)0xbc3, (uint32_t)0x29cb718);
        baseband_write_mem_table(NULL, (uint32_t)0xbc4, (uint32_t)0xc944120);
        baseband_write_mem_table(NULL, (uint32_t)0xbc5, (uint32_t)0x9546e57);
        baseband_write_mem_table(NULL, (uint32_t)0xbc6, (uint32_t)0x316002);
        baseband_write_mem_table(NULL, (uint32_t)0xbc7, (uint32_t)0x293b6ee);
        baseband_write_mem_table(NULL, (uint32_t)0xbc8, (uint32_t)0xc9b8215);
        baseband_write_mem_table(NULL, (uint32_t)0xbc9, (uint32_t)0x91aefc6);
        baseband_write_mem_table(NULL, (uint32_t)0xbca, (uint32_t)0xfee2000);
        baseband_write_mem_table(NULL, (uint32_t)0xbcb, (uint32_t)0x28ab6c6);
        baseband_write_mem_table(NULL, (uint32_t)0xbcc, (uint32_t)0xca70307);
        baseband_write_mem_table(NULL, (uint32_t)0xbcd, (uint32_t)0x8e6b141);
        baseband_write_mem_table(NULL, (uint32_t)0xbce, (uint32_t)0xfab2007);
        baseband_write_mem_table(NULL, (uint32_t)0xbcf, (uint32_t)0x281769e);
        baseband_write_mem_table(NULL, (uint32_t)0xbd0, (uint32_t)0xcb6c3f5);
        baseband_write_mem_table(NULL, (uint32_t)0xbd1, (uint32_t)0x8b772c6);
        baseband_write_mem_table(NULL, (uint32_t)0xbd2, (uint32_t)0xf682017);
        baseband_write_mem_table(NULL, (uint32_t)0xbd3, (uint32_t)0x2783676);
        baseband_write_mem_table(NULL, (uint32_t)0xbd4, (uint32_t)0xcca84de);
        baseband_write_mem_table(NULL, (uint32_t)0xbd5, (uint32_t)0x88d7453);
        baseband_write_mem_table(NULL, (uint32_t)0xbd6, (uint32_t)0xf25a02f);
        baseband_write_mem_table(NULL, (uint32_t)0xbd7, (uint32_t)0x26eb64f);
        baseband_write_mem_table(NULL, (uint32_t)0xbd8, (uint32_t)0xce205c0);
        baseband_write_mem_table(NULL, (uint32_t)0xbd9, (uint32_t)0x868b5e8);
        baseband_write_mem_table(NULL, (uint32_t)0xbda, (uint32_t)0xee36050);
        baseband_write_mem_table(NULL, (uint32_t)0xbdb, (uint32_t)0x264f629);
        baseband_write_mem_table(NULL, (uint32_t)0xbdc, (uint32_t)0xcfdc69a);
        baseband_write_mem_table(NULL, (uint32_t)0xbdd, (uint32_t)0x8497783);
        baseband_write_mem_table(NULL, (uint32_t)0xbde, (uint32_t)0xea1a079);
        baseband_write_mem_table(NULL, (uint32_t)0xbdf, (uint32_t)0x25b3604);
        baseband_write_mem_table(NULL, (uint32_t)0xbe0, (uint32_t)0xd1cc76c);
        baseband_write_mem_table(NULL, (uint32_t)0xbe1, (uint32_t)0x82fb924);
        baseband_write_mem_table(NULL, (uint32_t)0xbe2, (uint32_t)0xe6020ab);
        baseband_write_mem_table(NULL, (uint32_t)0xbe3, (uint32_t)0x25135df);
        baseband_write_mem_table(NULL, (uint32_t)0xbe4, (uint32_t)0xd3f8834);
        baseband_write_mem_table(NULL, (uint32_t)0xbe5, (uint32_t)0x81bbac9);
        baseband_write_mem_table(NULL, (uint32_t)0xbe6, (uint32_t)0xe1f60e5);
        baseband_write_mem_table(NULL, (uint32_t)0xbe7, (uint32_t)0x24735bb);
        baseband_write_mem_table(NULL, (uint32_t)0xbe8, (uint32_t)0xd6588f2);
        baseband_write_mem_table(NULL, (uint32_t)0xbe9, (uint32_t)0x80cfc70);
        baseband_write_mem_table(NULL, (uint32_t)0xbea, (uint32_t)0xddf6127);
        baseband_write_mem_table(NULL, (uint32_t)0xbeb, (uint32_t)0x23d3598);
        baseband_write_mem_table(NULL, (uint32_t)0xbec, (uint32_t)0xd8ec9a5);
        baseband_write_mem_table(NULL, (uint32_t)0xbed, (uint32_t)0x803be19);
        baseband_write_mem_table(NULL, (uint32_t)0xbee, (uint32_t)0xd9fe172);
        baseband_write_mem_table(NULL, (uint32_t)0xbef, (uint32_t)0x232f575);
        baseband_write_mem_table(NULL, (uint32_t)0xbf0, (uint32_t)0xdbaca4c);
        baseband_write_mem_table(NULL, (uint32_t)0xbf1, (uint32_t)0x8003fc3);
        baseband_write_mem_table(NULL, (uint32_t)0xbf2, (uint32_t)0xd6121c4);
        baseband_write_mem_table(NULL, (uint32_t)0xbf3, (uint32_t)0x2287553);
        baseband_write_mem_table(NULL, (uint32_t)0xbf4, (uint32_t)0xde98ae5);
        baseband_write_mem_table(NULL, (uint32_t)0xbf5, (uint32_t)0x802016c);
        baseband_write_mem_table(NULL, (uint32_t)0xbf6, (uint32_t)0xd23221f);
        baseband_write_mem_table(NULL, (uint32_t)0xbf7, (uint32_t)0x21e3532);
        baseband_write_mem_table(NULL, (uint32_t)0xbf8, (uint32_t)0xe1a8b71);
        baseband_write_mem_table(NULL, (uint32_t)0xbf9, (uint32_t)0x8098313);
        baseband_write_mem_table(NULL, (uint32_t)0xbfa, (uint32_t)0xce62281);
        baseband_write_mem_table(NULL, (uint32_t)0xbfb, (uint32_t)0x2137511);
        baseband_write_mem_table(NULL, (uint32_t)0xbfc, (uint32_t)0xe4e0bf0);
        baseband_write_mem_table(NULL, (uint32_t)0xbfd, (uint32_t)0x81644b7);
        baseband_write_mem_table(NULL, (uint32_t)0xbfe, (uint32_t)0xcaa22ea);
        baseband_write_mem_table(NULL, (uint32_t)0xbff, (uint32_t)0x208f4f1);
        baseband_write_mem_table(NULL, (uint32_t)0xc00, (uint32_t)0xe834c5f);
        baseband_write_mem_table(NULL, (uint32_t)0xc01, (uint32_t)0x8288658);
        baseband_write_mem_table(NULL, (uint32_t)0xc02, (uint32_t)0xc6f235b);
        baseband_write_mem_table(NULL, (uint32_t)0xc03, (uint32_t)0x1fe34d2);
        baseband_write_mem_table(NULL, (uint32_t)0xc04, (uint32_t)0xeba0cc0);
        baseband_write_mem_table(NULL, (uint32_t)0xc05, (uint32_t)0x84047f2);
        baseband_write_mem_table(NULL, (uint32_t)0xc06, (uint32_t)0xc34e3d4);
        baseband_write_mem_table(NULL, (uint32_t)0xc07, (uint32_t)0x1f334b3);
        baseband_write_mem_table(NULL, (uint32_t)0xc08, (uint32_t)0xef28d11);
        baseband_write_mem_table(NULL, (uint32_t)0xc09, (uint32_t)0x85d0987);
        baseband_write_mem_table(NULL, (uint32_t)0xc0a, (uint32_t)0xbfc2453);
        baseband_write_mem_table(NULL, (uint32_t)0xc0b, (uint32_t)0x1e83496);
        baseband_write_mem_table(NULL, (uint32_t)0xc0c, (uint32_t)0xf2bcd53);
        baseband_write_mem_table(NULL, (uint32_t)0xc0d, (uint32_t)0x87ecb15);
        baseband_write_mem_table(NULL, (uint32_t)0xc0e, (uint32_t)0xbc464d9);
        baseband_write_mem_table(NULL, (uint32_t)0xc0f, (uint32_t)0x1dd3479);
        baseband_write_mem_table(NULL, (uint32_t)0xc10, (uint32_t)0xf660d84);
        baseband_write_mem_table(NULL, (uint32_t)0xc11, (uint32_t)0x8a58c9a);
        baseband_write_mem_table(NULL, (uint32_t)0xc12, (uint32_t)0xb8de566);
        baseband_write_mem_table(NULL, (uint32_t)0xc13, (uint32_t)0x1d2345c);
        baseband_write_mem_table(NULL, (uint32_t)0xc14, (uint32_t)0xfa10da6);
        baseband_write_mem_table(NULL, (uint32_t)0xc15, (uint32_t)0x8d10e15);
        baseband_write_mem_table(NULL, (uint32_t)0xc16, (uint32_t)0xb58a5f9);
        baseband_write_mem_table(NULL, (uint32_t)0xc17, (uint32_t)0x1c6f441);
        baseband_write_mem_table(NULL, (uint32_t)0xc18, (uint32_t)0xfdc0db8);
        baseband_write_mem_table(NULL, (uint32_t)0xc19, (uint32_t)0x9014f87);
        baseband_write_mem_table(NULL, (uint32_t)0xc1a, (uint32_t)0xb24a693);
        baseband_write_mem_table(NULL, (uint32_t)0xc1b, (uint32_t)0x1bbb426);
        baseband_write_mem_table(NULL, (uint32_t)0xc1c, (uint32_t)0x174db9);
        baseband_write_mem_table(NULL, (uint32_t)0xc1d, (uint32_t)0x93610ed);
        baseband_write_mem_table(NULL, (uint32_t)0xc1e, (uint32_t)0xaf1e733);
        baseband_write_mem_table(NULL, (uint32_t)0xc1f, (uint32_t)0x1b0340c);
        baseband_write_mem_table(NULL, (uint32_t)0xc20, (uint32_t)0x524dab);
        baseband_write_mem_table(NULL, (uint32_t)0xc21, (uint32_t)0x96f1248);
        baseband_write_mem_table(NULL, (uint32_t)0xc22, (uint32_t)0xac0a7d9);
        baseband_write_mem_table(NULL, (uint32_t)0xc23, (uint32_t)0x1a4f3f3);
        baseband_write_mem_table(NULL, (uint32_t)0xc24, (uint32_t)0x8c8d8d);
        baseband_write_mem_table(NULL, (uint32_t)0xc25, (uint32_t)0x9ac5396);
        baseband_write_mem_table(NULL, (uint32_t)0xc26, (uint32_t)0xa90e884);
        baseband_write_mem_table(NULL, (uint32_t)0xc27, (uint32_t)0x19973da);
        baseband_write_mem_table(NULL, (uint32_t)0xc28, (uint32_t)0xc64d60);
        baseband_write_mem_table(NULL, (uint32_t)0xc29, (uint32_t)0x9ed94d6);
        baseband_write_mem_table(NULL, (uint32_t)0xc2a, (uint32_t)0xa62a935);
        baseband_write_mem_table(NULL, (uint32_t)0xc2b, (uint32_t)0x18df3c2);
        baseband_write_mem_table(NULL, (uint32_t)0xc2c, (uint32_t)0xff0d23);
        baseband_write_mem_table(NULL, (uint32_t)0xc2d, (uint32_t)0xa329607);
        baseband_write_mem_table(NULL, (uint32_t)0xc2e, (uint32_t)0xa35e9eb);
        baseband_write_mem_table(NULL, (uint32_t)0xc2f, (uint32_t)0x18233ab);
        baseband_write_mem_table(NULL, (uint32_t)0xc30, (uint32_t)0x1364cd8);
        baseband_write_mem_table(NULL, (uint32_t)0xc31, (uint32_t)0xa7b172a);
        baseband_write_mem_table(NULL, (uint32_t)0xc32, (uint32_t)0xa0aaaa6);
        baseband_write_mem_table(NULL, (uint32_t)0xc33, (uint32_t)0x176b395);
        baseband_write_mem_table(NULL, (uint32_t)0xc34, (uint32_t)0x16c0c7f);
        baseband_write_mem_table(NULL, (uint32_t)0xc35, (uint32_t)0xac7183d);
        baseband_write_mem_table(NULL, (uint32_t)0xc36, (uint32_t)0x9e12b66);
        baseband_write_mem_table(NULL, (uint32_t)0xc37, (uint32_t)0x16af37f);
        baseband_write_mem_table(NULL, (uint32_t)0xc38, (uint32_t)0x1a04c17);
        baseband_write_mem_table(NULL, (uint32_t)0xc39, (uint32_t)0xb161940);
        baseband_write_mem_table(NULL, (uint32_t)0xc3a, (uint32_t)0x9b92c2a);
        baseband_write_mem_table(NULL, (uint32_t)0xc3b, (uint32_t)0x15f336a);
        baseband_write_mem_table(NULL, (uint32_t)0xc3c, (uint32_t)0x1d24ba3);
        baseband_write_mem_table(NULL, (uint32_t)0xc3d, (uint32_t)0xb681a33);
        baseband_write_mem_table(NULL, (uint32_t)0xc3e, (uint32_t)0x992acf3);
        baseband_write_mem_table(NULL, (uint32_t)0xc3f, (uint32_t)0x1537356);
        baseband_write_mem_table(NULL, (uint32_t)0xc40, (uint32_t)0x2024b22);
        baseband_write_mem_table(NULL, (uint32_t)0xc41, (uint32_t)0xbbc9b14);
        baseband_write_mem_table(NULL, (uint32_t)0xc42, (uint32_t)0x96dedc0);
        baseband_write_mem_table(NULL, (uint32_t)0xc43, (uint32_t)0x1477342);
        baseband_write_mem_table(NULL, (uint32_t)0xc44, (uint32_t)0x2300a94);
        baseband_write_mem_table(NULL, (uint32_t)0xc45, (uint32_t)0xc13dbe3);
        baseband_write_mem_table(NULL, (uint32_t)0xc46, (uint32_t)0x94aee90);
        baseband_write_mem_table(NULL, (uint32_t)0xc47, (uint32_t)0x13bb330);
        baseband_write_mem_table(NULL, (uint32_t)0xc48, (uint32_t)0x25b09fc);
        baseband_write_mem_table(NULL, (uint32_t)0xc49, (uint32_t)0xc6d1ca1);
        baseband_write_mem_table(NULL, (uint32_t)0xc4a, (uint32_t)0x929af65);
        baseband_write_mem_table(NULL, (uint32_t)0xc4b, (uint32_t)0x12fb31e);
        baseband_write_mem_table(NULL, (uint32_t)0xc4c, (uint32_t)0x2838959);
        baseband_write_mem_table(NULL, (uint32_t)0xc4d, (uint32_t)0xcc85d4c);
        baseband_write_mem_table(NULL, (uint32_t)0xc4e, (uint32_t)0x909f03c);
        baseband_write_mem_table(NULL, (uint32_t)0xc4f, (uint32_t)0x123f30d);
        baseband_write_mem_table(NULL, (uint32_t)0xc50, (uint32_t)0x2a908ad);
        baseband_write_mem_table(NULL, (uint32_t)0xc51, (uint32_t)0xd251de5);
        baseband_write_mem_table(NULL, (uint32_t)0xc52, (uint32_t)0x8ec3117);
        baseband_write_mem_table(NULL, (uint32_t)0xc53, (uint32_t)0x117f2fc);
        baseband_write_mem_table(NULL, (uint32_t)0xc54, (uint32_t)0x2cb87f7);
        baseband_write_mem_table(NULL, (uint32_t)0xc55, (uint32_t)0xd839e6a);
        baseband_write_mem_table(NULL, (uint32_t)0xc56, (uint32_t)0x8cff1f4);
        baseband_write_mem_table(NULL, (uint32_t)0xc57, (uint32_t)0x10bf2ed);
        baseband_write_mem_table(NULL, (uint32_t)0xc58, (uint32_t)0x2eb073a);
        baseband_write_mem_table(NULL, (uint32_t)0xc59, (uint32_t)0xde35edd);
        baseband_write_mem_table(NULL, (uint32_t)0xc5a, (uint32_t)0x8b5b2d5);
        baseband_write_mem_table(NULL, (uint32_t)0xc5b, (uint32_t)0xfff2de);
        baseband_write_mem_table(NULL, (uint32_t)0xc5c, (uint32_t)0x3074676);
        baseband_write_mem_table(NULL, (uint32_t)0xc5d, (uint32_t)0xe441f3d);
        baseband_write_mem_table(NULL, (uint32_t)0xc5e, (uint32_t)0x89d33b7);
        baseband_write_mem_table(NULL, (uint32_t)0xc5f, (uint32_t)0xf3f2cf);
        baseband_write_mem_table(NULL, (uint32_t)0xc60, (uint32_t)0x32045ab);
        baseband_write_mem_table(NULL, (uint32_t)0xc61, (uint32_t)0xea55f8a);
        baseband_write_mem_table(NULL, (uint32_t)0xc62, (uint32_t)0x886349c);
        baseband_write_mem_table(NULL, (uint32_t)0xc63, (uint32_t)0xe7f2c2);
        baseband_write_mem_table(NULL, (uint32_t)0xc64, (uint32_t)0x33604db);
        baseband_write_mem_table(NULL, (uint32_t)0xc65, (uint32_t)0xf075fc3);
        baseband_write_mem_table(NULL, (uint32_t)0xc66, (uint32_t)0x8717583);
        baseband_write_mem_table(NULL, (uint32_t)0xc67, (uint32_t)0xdbb2b5);
        baseband_write_mem_table(NULL, (uint32_t)0xc68, (uint32_t)0x3480406);
        baseband_write_mem_table(NULL, (uint32_t)0xc69, (uint32_t)0xf699fea);
        baseband_write_mem_table(NULL, (uint32_t)0xc6a, (uint32_t)0x85e366b);
        baseband_write_mem_table(NULL, (uint32_t)0xc6b, (uint32_t)0xcfb2a9);
        baseband_write_mem_table(NULL, (uint32_t)0xc6c, (uint32_t)0x356c32e);
        baseband_write_mem_table(NULL, (uint32_t)0xc6d, (uint32_t)0xfcbdffd);
        baseband_write_mem_table(NULL, (uint32_t)0xc6e, (uint32_t)0x84cb755);
        baseband_write_mem_table(NULL, (uint32_t)0xc6f, (uint32_t)0xc3b29e);
        baseband_write_mem_table(NULL, (uint32_t)0xc70, (uint32_t)0x3620253);
        baseband_write_mem_table(NULL, (uint32_t)0xc71, (uint32_t)0x2ddffe);
        baseband_write_mem_table(NULL, (uint32_t)0xc72, (uint32_t)0x83d3840);
        baseband_write_mem_table(NULL, (uint32_t)0xc73, (uint32_t)0xb7b293);
        baseband_write_mem_table(NULL, (uint32_t)0xc74, (uint32_t)0x3698177);
        baseband_write_mem_table(NULL, (uint32_t)0xc75, (uint32_t)0x8f9fec);
        baseband_write_mem_table(NULL, (uint32_t)0xc76, (uint32_t)0x82f792c);
        baseband_write_mem_table(NULL, (uint32_t)0xc77, (uint32_t)0xabb289);
        baseband_write_mem_table(NULL, (uint32_t)0xc78, (uint32_t)0x36dc09a);
        baseband_write_mem_table(NULL, (uint32_t)0xc79, (uint32_t)0xf05fc7);
        baseband_write_mem_table(NULL, (uint32_t)0xc7a, (uint32_t)0x8237a19);
        baseband_write_mem_table(NULL, (uint32_t)0xc7b, (uint32_t)0x9f7280);
        baseband_write_mem_table(NULL, (uint32_t)0xc7c, (uint32_t)0x36ebfbd);
        baseband_write_mem_table(NULL, (uint32_t)0xc7d, (uint32_t)0x1509f91);
        baseband_write_mem_table(NULL, (uint32_t)0xc7e, (uint32_t)0x8193b06);
        baseband_write_mem_table(NULL, (uint32_t)0xc7f, (uint32_t)0x937277);
        baseband_write_mem_table(NULL, (uint32_t)0xc80, (uint32_t)0x36bfee1);
        baseband_write_mem_table(NULL, (uint32_t)0xc81, (uint32_t)0x1af9f48);
        baseband_write_mem_table(NULL, (uint32_t)0xc82, (uint32_t)0x810bbf4);
        baseband_write_mem_table(NULL, (uint32_t)0xc83, (uint32_t)0x87726f);
        baseband_write_mem_table(NULL, (uint32_t)0xc84, (uint32_t)0x365be07);
        baseband_write_mem_table(NULL, (uint32_t)0xc85, (uint32_t)0x20d5eee);
        baseband_write_mem_table(NULL, (uint32_t)0xc86, (uint32_t)0x809fce1);
        baseband_write_mem_table(NULL, (uint32_t)0xc87, (uint32_t)0x7b7268);
        baseband_write_mem_table(NULL, (uint32_t)0xc88, (uint32_t)0x35c3d30);
        baseband_write_mem_table(NULL, (uint32_t)0xc89, (uint32_t)0x2699e83);
        baseband_write_mem_table(NULL, (uint32_t)0xc8a, (uint32_t)0x804fdcf);
        baseband_write_mem_table(NULL, (uint32_t)0xc8b, (uint32_t)0x6f7262);
        baseband_write_mem_table(NULL, (uint32_t)0xc8c, (uint32_t)0x34f7c5c);
        baseband_write_mem_table(NULL, (uint32_t)0xc8d, (uint32_t)0x2c41e07);
        baseband_write_mem_table(NULL, (uint32_t)0xc8e, (uint32_t)0x801bebc);
        baseband_write_mem_table(NULL, (uint32_t)0xc8f, (uint32_t)0x63725c);
        baseband_write_mem_table(NULL, (uint32_t)0xc90, (uint32_t)0x33f7b8c);
        baseband_write_mem_table(NULL, (uint32_t)0xc91, (uint32_t)0x31cdd7b);
        baseband_write_mem_table(NULL, (uint32_t)0xc92, (uint32_t)0x8003fa8);
        baseband_write_mem_table(NULL, (uint32_t)0xc93, (uint32_t)0x57b257);
        baseband_write_mem_table(NULL, (uint32_t)0xc94, (uint32_t)0x32c3ac2);
        baseband_write_mem_table(NULL, (uint32_t)0xc95, (uint32_t)0x3739cdf);
        baseband_write_mem_table(NULL, (uint32_t)0xc96, (uint32_t)0x8004094);
        baseband_write_mem_table(NULL, (uint32_t)0xc97, (uint32_t)0x4bb252);
        baseband_write_mem_table(NULL, (uint32_t)0xc98, (uint32_t)0x31639fd);
        baseband_write_mem_table(NULL, (uint32_t)0xc99, (uint32_t)0x3c81c33);
        baseband_write_mem_table(NULL, (uint32_t)0xc9a, (uint32_t)0x802417f);
        baseband_write_mem_table(NULL, (uint32_t)0xc9b, (uint32_t)0x3ff24f);
        baseband_write_mem_table(NULL, (uint32_t)0xc9c, (uint32_t)0x2fd393f);
        baseband_write_mem_table(NULL, (uint32_t)0xc9d, (uint32_t)0x41a1b79);
        baseband_write_mem_table(NULL, (uint32_t)0xc9e, (uint32_t)0x805c268);
        baseband_write_mem_table(NULL, (uint32_t)0xc9f, (uint32_t)0x33f24c);
        baseband_write_mem_table(NULL, (uint32_t)0xca0, (uint32_t)0x2e17888);
        baseband_write_mem_table(NULL, (uint32_t)0xca1, (uint32_t)0x4699ab1);
        baseband_write_mem_table(NULL, (uint32_t)0xca2, (uint32_t)0x80b0351);
        baseband_write_mem_table(NULL, (uint32_t)0xca3, (uint32_t)0x283249);
        baseband_write_mem_table(NULL, (uint32_t)0xca4, (uint32_t)0x2c337d9);
        baseband_write_mem_table(NULL, (uint32_t)0xca5, (uint32_t)0x4b699dc);
        baseband_write_mem_table(NULL, (uint32_t)0xca6, (uint32_t)0x811c437);
        baseband_write_mem_table(NULL, (uint32_t)0xca7, (uint32_t)0x1c7247);
        baseband_write_mem_table(NULL, (uint32_t)0xca8, (uint32_t)0x2a23732);
        baseband_write_mem_table(NULL, (uint32_t)0xca9, (uint32_t)0x50098f9);
        baseband_write_mem_table(NULL, (uint32_t)0xcaa, (uint32_t)0x81a451c);
        baseband_write_mem_table(NULL, (uint32_t)0xcab, (uint32_t)0x10b246);
        baseband_write_mem_table(NULL, (uint32_t)0xcac, (uint32_t)0x27f3694);
        baseband_write_mem_table(NULL, (uint32_t)0xcad, (uint32_t)0x547d80a);
        baseband_write_mem_table(NULL, (uint32_t)0xcae, (uint32_t)0x82445ff);
        baseband_write_mem_table(NULL, (uint32_t)0xcaf, (uint32_t)0x4f246);
        baseband_write_mem_table(NULL, (uint32_t)0xcb0, (uint32_t)0x259f5ff);
        baseband_write_mem_table(NULL, (uint32_t)0xcb1, (uint32_t)0x58c170f);
        baseband_write_mem_table(NULL, (uint32_t)0xcb2, (uint32_t)0x82fc6e0);
        baseband_write_mem_table(NULL, (uint32_t)0xcb3, (uint32_t)0xff97246);
        baseband_write_mem_table(NULL, (uint32_t)0xcb4, (uint32_t)0x232b574);
        baseband_write_mem_table(NULL, (uint32_t)0xcb5, (uint32_t)0x5ccd60a);
        baseband_write_mem_table(NULL, (uint32_t)0xcb6, (uint32_t)0x83cc7be);
        baseband_write_mem_table(NULL, (uint32_t)0xcb7, (uint32_t)0xfedb246);
        baseband_write_mem_table(NULL, (uint32_t)0xcb8, (uint32_t)0x20974f3);
        baseband_write_mem_table(NULL, (uint32_t)0xcb9, (uint32_t)0x60a94fa);
        baseband_write_mem_table(NULL, (uint32_t)0xcba, (uint32_t)0x84b889a);
        baseband_write_mem_table(NULL, (uint32_t)0xcbb, (uint32_t)0xfe23248);
        baseband_write_mem_table(NULL, (uint32_t)0xcbc, (uint32_t)0x1deb47c);
        baseband_write_mem_table(NULL, (uint32_t)0xcbd, (uint32_t)0x64513e1);
        baseband_write_mem_table(NULL, (uint32_t)0xcbe, (uint32_t)0x85b8974);
        baseband_write_mem_table(NULL, (uint32_t)0xcbf, (uint32_t)0xfd6b249);
        baseband_write_mem_table(NULL, (uint32_t)0xcc0, (uint32_t)0x1b27411);
        baseband_write_mem_table(NULL, (uint32_t)0xcc1, (uint32_t)0x67c12be);
        baseband_write_mem_table(NULL, (uint32_t)0xcc2, (uint32_t)0x86cca4b);
        baseband_write_mem_table(NULL, (uint32_t)0xcc3, (uint32_t)0xfcb324c);
        baseband_write_mem_table(NULL, (uint32_t)0xcc4, (uint32_t)0x184f3b0);
        baseband_write_mem_table(NULL, (uint32_t)0xcc5, (uint32_t)0x6af5194);
        baseband_write_mem_table(NULL, (uint32_t)0xcc6, (uint32_t)0x87f8b1f);
        baseband_write_mem_table(NULL, (uint32_t)0xcc7, (uint32_t)0xfbff24f);
        baseband_write_mem_table(NULL, (uint32_t)0xcc8, (uint32_t)0x156335b);
        baseband_write_mem_table(NULL, (uint32_t)0xcc9, (uint32_t)0x6df5062);
        baseband_write_mem_table(NULL, (uint32_t)0xcca, (uint32_t)0x893cbef);
        baseband_write_mem_table(NULL, (uint32_t)0xccb, (uint32_t)0xfb47253);
        baseband_write_mem_table(NULL, (uint32_t)0xccc, (uint32_t)0x1267310);
        baseband_write_mem_table(NULL, (uint32_t)0xccd, (uint32_t)0x70b8f29);
        baseband_write_mem_table(NULL, (uint32_t)0xcce, (uint32_t)0x8a94cbd);
        baseband_write_mem_table(NULL, (uint32_t)0xccf, (uint32_t)0xfa93257);
        baseband_write_mem_table(NULL, (uint32_t)0xcd0, (uint32_t)0xf5f2d2);
        baseband_write_mem_table(NULL, (uint32_t)0xcd1, (uint32_t)0x7344dea);
        baseband_write_mem_table(NULL, (uint32_t)0xcd2, (uint32_t)0x8c00d87);
        baseband_write_mem_table(NULL, (uint32_t)0xcd3, (uint32_t)0xf9e325b);
        baseband_write_mem_table(NULL, (uint32_t)0xcd4, (uint32_t)0xc4f29f);
        baseband_write_mem_table(NULL, (uint32_t)0xcd5, (uint32_t)0x7594ca6);
        baseband_write_mem_table(NULL, (uint32_t)0xcd6, (uint32_t)0x8d80e4e);
        baseband_write_mem_table(NULL, (uint32_t)0xcd7, (uint32_t)0xf92f261);
        baseband_write_mem_table(NULL, (uint32_t)0xcd8, (uint32_t)0x937277);
        baseband_write_mem_table(NULL, (uint32_t)0xcd9, (uint32_t)0x77a8b5d);
        baseband_write_mem_table(NULL, (uint32_t)0xcda, (uint32_t)0x8f14f12);
        baseband_write_mem_table(NULL, (uint32_t)0xcdb, (uint32_t)0xf87f267);
        baseband_write_mem_table(NULL, (uint32_t)0xcdc, (uint32_t)0x61b25b);
        baseband_write_mem_table(NULL, (uint32_t)0xcdd, (uint32_t)0x7980a11);
        baseband_write_mem_table(NULL, (uint32_t)0xcde, (uint32_t)0x90bcfd1);
        baseband_write_mem_table(NULL, (uint32_t)0xcdf, (uint32_t)0xf7cf26d);
        baseband_write_mem_table(NULL, (uint32_t)0xce0, (uint32_t)0x2ff24b);
        baseband_write_mem_table(NULL, (uint32_t)0xce1, (uint32_t)0x7b208c1);
        baseband_write_mem_table(NULL, (uint32_t)0xce2, (uint32_t)0x927508d);
        baseband_write_mem_table(NULL, (uint32_t)0xce3, (uint32_t)0xf71f274);
        baseband_write_mem_table(NULL, (uint32_t)0xce4, (uint32_t)0xffe3245);
        baseband_write_mem_table(NULL, (uint32_t)0xce5, (uint32_t)0x7c8076e);
        baseband_write_mem_table(NULL, (uint32_t)0xce6, (uint32_t)0x9441146);
        baseband_write_mem_table(NULL, (uint32_t)0xce7, (uint32_t)0xf67327b);
        baseband_write_mem_table(NULL, (uint32_t)0xce8, (uint32_t)0xfccb24c);
        baseband_write_mem_table(NULL, (uint32_t)0xce9, (uint32_t)0x7da861a);
        baseband_write_mem_table(NULL, (uint32_t)0xcea, (uint32_t)0x961d1fa);
        baseband_write_mem_table(NULL, (uint32_t)0xceb, (uint32_t)0xf5c7283);
        baseband_write_mem_table(NULL, (uint32_t)0xcec, (uint32_t)0xf9b725d);
        baseband_write_mem_table(NULL, (uint32_t)0xced, (uint32_t)0x7e944c4);
        baseband_write_mem_table(NULL, (uint32_t)0xcee, (uint32_t)0x98092aa);
        baseband_write_mem_table(NULL, (uint32_t)0xcef, (uint32_t)0xf51b28c);
        baseband_write_mem_table(NULL, (uint32_t)0xcf0, (uint32_t)0xf6af279);
        baseband_write_mem_table(NULL, (uint32_t)0xcf1, (uint32_t)0x7f4436e);
        baseband_write_mem_table(NULL, (uint32_t)0xcf2, (uint32_t)0x9a05356);
        baseband_write_mem_table(NULL, (uint32_t)0xcf3, (uint32_t)0xf46f294);
        baseband_write_mem_table(NULL, (uint32_t)0xcf4, (uint32_t)0xf3af29f);
        baseband_write_mem_table(NULL, (uint32_t)0xcf5, (uint32_t)0x7fb8218);
        baseband_write_mem_table(NULL, (uint32_t)0xcf6, (uint32_t)0x9c0d3fe);
        baseband_write_mem_table(NULL, (uint32_t)0xcf7, (uint32_t)0xf3c729e);
        baseband_write_mem_table(NULL, (uint32_t)0xcf8, (uint32_t)0xf0bf2d0);
        baseband_write_mem_table(NULL, (uint32_t)0xcf9, (uint32_t)0x7ff80c3);
        baseband_write_mem_table(NULL, (uint32_t)0xcfa, (uint32_t)0x9e294a2);
        baseband_write_mem_table(NULL, (uint32_t)0xcfb, (uint32_t)0xf31f2a8);
        baseband_write_mem_table(NULL, (uint32_t)0xcfc, (uint32_t)0xeddf30b);
        baseband_write_mem_table(NULL, (uint32_t)0xcfd, (uint32_t)0x7ffff6f);
        baseband_write_mem_table(NULL, (uint32_t)0xcfe, (uint32_t)0xa051541);
        baseband_write_mem_table(NULL, (uint32_t)0xcff, (uint32_t)0xf27b2b2);
        baseband_write_mem_table(NULL, (uint32_t)0xd00, (uint32_t)0xeb0f34f);
        baseband_write_mem_table(NULL, (uint32_t)0xd01, (uint32_t)0x7fcbe1d);
        baseband_write_mem_table(NULL, (uint32_t)0xd02, (uint32_t)0xa2855dc);
        baseband_write_mem_table(NULL, (uint32_t)0xd03, (uint32_t)0xf1d72bd);
        baseband_write_mem_table(NULL, (uint32_t)0xd04, (uint32_t)0xe85339d);
        baseband_write_mem_table(NULL, (uint32_t)0xd05, (uint32_t)0x7f5fcce);
        baseband_write_mem_table(NULL, (uint32_t)0xd06, (uint32_t)0xa4c5672);
        baseband_write_mem_table(NULL, (uint32_t)0xd07, (uint32_t)0xf1332c8);
        baseband_write_mem_table(NULL, (uint32_t)0xd08, (uint32_t)0xe5af3f4);
        baseband_write_mem_table(NULL, (uint32_t)0xd09, (uint32_t)0x7ebfb82);
        baseband_write_mem_table(NULL, (uint32_t)0xd0a, (uint32_t)0xa715704);
        baseband_write_mem_table(NULL, (uint32_t)0xd0b, (uint32_t)0xf08f2d3);
        baseband_write_mem_table(NULL, (uint32_t)0xd0c, (uint32_t)0xe31f453);
        baseband_write_mem_table(NULL, (uint32_t)0xd0d, (uint32_t)0x7de7a39);
        baseband_write_mem_table(NULL, (uint32_t)0xd0e, (uint32_t)0xa96d792);
        baseband_write_mem_table(NULL, (uint32_t)0xd0f, (uint32_t)0xefef2df);
        baseband_write_mem_table(NULL, (uint32_t)0xd10, (uint32_t)0xe0a74bb);
        baseband_write_mem_table(NULL, (uint32_t)0xd11, (uint32_t)0x7cdf8f5);
        baseband_write_mem_table(NULL, (uint32_t)0xd12, (uint32_t)0xabd181b);
        baseband_write_mem_table(NULL, (uint32_t)0xd13, (uint32_t)0xef4f2ec);
        baseband_write_mem_table(NULL, (uint32_t)0xd14, (uint32_t)0xde4b52a);
        baseband_write_mem_table(NULL, (uint32_t)0xd15, (uint32_t)0x7ba37b5);
        baseband_write_mem_table(NULL, (uint32_t)0xd16, (uint32_t)0xae4189f);
        baseband_write_mem_table(NULL, (uint32_t)0xd17, (uint32_t)0xeeb32f9);
        baseband_write_mem_table(NULL, (uint32_t)0xd18, (uint32_t)0xdc0b5a0);
        baseband_write_mem_table(NULL, (uint32_t)0xd19, (uint32_t)0x7a3767a);
        baseband_write_mem_table(NULL, (uint32_t)0xd1a, (uint32_t)0xb0b591f);
        baseband_write_mem_table(NULL, (uint32_t)0xd1b, (uint32_t)0xee17306);
        baseband_write_mem_table(NULL, (uint32_t)0xd1c, (uint32_t)0xd9e761d);
        baseband_write_mem_table(NULL, (uint32_t)0xd1d, (uint32_t)0x789b545);
        baseband_write_mem_table(NULL, (uint32_t)0xd1e, (uint32_t)0xb33999a);
        baseband_write_mem_table(NULL, (uint32_t)0xd1f, (uint32_t)0xed7b314);
        baseband_write_mem_table(NULL, (uint32_t)0xd20, (uint32_t)0xd7e36a1);
        baseband_write_mem_table(NULL, (uint32_t)0xd21, (uint32_t)0x76cf416);
        baseband_write_mem_table(NULL, (uint32_t)0xd22, (uint32_t)0xb5c1a11);
        baseband_write_mem_table(NULL, (uint32_t)0xd23, (uint32_t)0xece3322);
        baseband_write_mem_table(NULL, (uint32_t)0xd24, (uint32_t)0xd5fb72a);
        baseband_write_mem_table(NULL, (uint32_t)0xd25, (uint32_t)0x74d72ee);
        baseband_write_mem_table(NULL, (uint32_t)0xd26, (uint32_t)0xb851a83);
        baseband_write_mem_table(NULL, (uint32_t)0xd27, (uint32_t)0xec4b330);
        baseband_write_mem_table(NULL, (uint32_t)0xd28, (uint32_t)0xd4377b8);
        baseband_write_mem_table(NULL, (uint32_t)0xd29, (uint32_t)0x72b71cc);
        baseband_write_mem_table(NULL, (uint32_t)0xd2a, (uint32_t)0xbae9af0);
        baseband_write_mem_table(NULL, (uint32_t)0xd2b, (uint32_t)0xebb333f);
        baseband_write_mem_table(NULL, (uint32_t)0xd2c, (uint32_t)0xd28f84c);
        baseband_write_mem_table(NULL, (uint32_t)0xd2d, (uint32_t)0x706b0b1);
        baseband_write_mem_table(NULL, (uint32_t)0xd2e, (uint32_t)0xbd85b58);
        baseband_write_mem_table(NULL, (uint32_t)0xd2f, (uint32_t)0xeb1f34e);
        baseband_write_mem_table(NULL, (uint32_t)0xd30, (uint32_t)0xd10b8e3);
        baseband_write_mem_table(NULL, (uint32_t)0xd31, (uint32_t)0x6df6f9e);
        baseband_write_mem_table(NULL, (uint32_t)0xd32, (uint32_t)0xc029bbc);
        baseband_write_mem_table(NULL, (uint32_t)0xd33, (uint32_t)0xea8b35d);
        baseband_write_mem_table(NULL, (uint32_t)0xd34, (uint32_t)0xcfa797f);
        baseband_write_mem_table(NULL, (uint32_t)0xd35, (uint32_t)0x6b5ee93);
        baseband_write_mem_table(NULL, (uint32_t)0xd36, (uint32_t)0xc2d5c1c);
        baseband_write_mem_table(NULL, (uint32_t)0xd37, (uint32_t)0xe9fb36d);
        baseband_write_mem_table(NULL, (uint32_t)0xd38, (uint32_t)0xce67a1e);
        baseband_write_mem_table(NULL, (uint32_t)0xd39, (uint32_t)0x689ed90);
        baseband_write_mem_table(NULL, (uint32_t)0xd3a, (uint32_t)0xc581c76);
        baseband_write_mem_table(NULL, (uint32_t)0xd3b, (uint32_t)0xe96b37d);
        baseband_write_mem_table(NULL, (uint32_t)0xd3c, (uint32_t)0xcd47ac0);
        baseband_write_mem_table(NULL, (uint32_t)0xd3d, (uint32_t)0x65bec95);
        baseband_write_mem_table(NULL, (uint32_t)0xd3e, (uint32_t)0xc835ccd);
        baseband_write_mem_table(NULL, (uint32_t)0xd3f, (uint32_t)0xe8db38d);
        baseband_write_mem_table(NULL, (uint32_t)0xd40, (uint32_t)0xcc4bb64);
        baseband_write_mem_table(NULL, (uint32_t)0xd41, (uint32_t)0x62beba3);
        baseband_write_mem_table(NULL, (uint32_t)0xd42, (uint32_t)0xcae9d1e);
        baseband_write_mem_table(NULL, (uint32_t)0xd43, (uint32_t)0xe84f39e);
        baseband_write_mem_table(NULL, (uint32_t)0xd44, (uint32_t)0xcb6fc0a);
        baseband_write_mem_table(NULL, (uint32_t)0xd45, (uint32_t)0x5f9eaba);
        baseband_write_mem_table(NULL, (uint32_t)0xd46, (uint32_t)0xcda1d6b);
        baseband_write_mem_table(NULL, (uint32_t)0xd47, (uint32_t)0xe7c73ae);
        baseband_write_mem_table(NULL, (uint32_t)0xd48, (uint32_t)0xcab7cb2);
        baseband_write_mem_table(NULL, (uint32_t)0xd49, (uint32_t)0x5c629d9);
        baseband_write_mem_table(NULL, (uint32_t)0xd4a, (uint32_t)0xd05ddb4);
        baseband_write_mem_table(NULL, (uint32_t)0xd4b, (uint32_t)0xe73b3c0);
        baseband_write_mem_table(NULL, (uint32_t)0xd4c, (uint32_t)0xca1fd5a);
        baseband_write_mem_table(NULL, (uint32_t)0xd4d, (uint32_t)0x590a902);
        baseband_write_mem_table(NULL, (uint32_t)0xd4e, (uint32_t)0xd31ddf8);
        baseband_write_mem_table(NULL, (uint32_t)0xd4f, (uint32_t)0xe6b33d1);
        baseband_write_mem_table(NULL, (uint32_t)0xd50, (uint32_t)0xc9abe03);
        baseband_write_mem_table(NULL, (uint32_t)0xd51, (uint32_t)0x5596835);
        baseband_write_mem_table(NULL, (uint32_t)0xd52, (uint32_t)0xd5dde37);
        baseband_write_mem_table(NULL, (uint32_t)0xd53, (uint32_t)0xe62f3e3);
        baseband_write_mem_table(NULL, (uint32_t)0xd54, (uint32_t)0xc95bead);
        baseband_write_mem_table(NULL, (uint32_t)0xd55, (uint32_t)0x520e770);
        baseband_write_mem_table(NULL, (uint32_t)0xd56, (uint32_t)0xd89de72);
        baseband_write_mem_table(NULL, (uint32_t)0xd57, (uint32_t)0xe5a73f5);
        baseband_write_mem_table(NULL, (uint32_t)0xd58, (uint32_t)0xc92bf56);
        baseband_write_mem_table(NULL, (uint32_t)0xd59, (uint32_t)0x4e6e6b6);
        baseband_write_mem_table(NULL, (uint32_t)0xd5a, (uint32_t)0xdb5dea9);
        baseband_write_mem_table(NULL, (uint32_t)0xd5b, (uint32_t)0xe527407);
        baseband_write_mem_table(NULL, (uint32_t)0xd5c, (uint32_t)0xc917ffe);
        baseband_write_mem_table(NULL, (uint32_t)0xd5d, (uint32_t)0x4aba605);
        baseband_write_mem_table(NULL, (uint32_t)0xd5e, (uint32_t)0xde1dedc);
        baseband_write_mem_table(NULL, (uint32_t)0xd5f, (uint32_t)0xe4a3419);
        baseband_write_mem_table(NULL, (uint32_t)0xd60, (uint32_t)0xc9240a5);
        baseband_write_mem_table(NULL, (uint32_t)0xd61, (uint32_t)0x46f655e);
        baseband_write_mem_table(NULL, (uint32_t)0xd62, (uint32_t)0xe0ddf0a);
        baseband_write_mem_table(NULL, (uint32_t)0xd63, (uint32_t)0xe42342c);
        baseband_write_mem_table(NULL, (uint32_t)0xd64, (uint32_t)0xc95414a);
        baseband_write_mem_table(NULL, (uint32_t)0xd65, (uint32_t)0x431e4c0);
        baseband_write_mem_table(NULL, (uint32_t)0xd66, (uint32_t)0xe39df34);
        baseband_write_mem_table(NULL, (uint32_t)0xd67, (uint32_t)0xe3a743f);
        baseband_write_mem_table(NULL, (uint32_t)0xd68, (uint32_t)0xc9a01ee);
        baseband_write_mem_table(NULL, (uint32_t)0xd69, (uint32_t)0x3f3a42d);
        baseband_write_mem_table(NULL, (uint32_t)0xd6a, (uint32_t)0xe65df5a);
        baseband_write_mem_table(NULL, (uint32_t)0xd6b, (uint32_t)0xe32b452);
        baseband_write_mem_table(NULL, (uint32_t)0xd6c, (uint32_t)0xca0c290);
        baseband_write_mem_table(NULL, (uint32_t)0xd6d, (uint32_t)0x3b463a3);
        baseband_write_mem_table(NULL, (uint32_t)0xd6e, (uint32_t)0xe919f7c);
        baseband_write_mem_table(NULL, (uint32_t)0xd6f, (uint32_t)0xe2af465);
        baseband_write_mem_table(NULL, (uint32_t)0xd70, (uint32_t)0xca9432f);
        baseband_write_mem_table(NULL, (uint32_t)0xd71, (uint32_t)0x374a323);
        baseband_write_mem_table(NULL, (uint32_t)0xd72, (uint32_t)0xebd1f9a);
        baseband_write_mem_table(NULL, (uint32_t)0xd73, (uint32_t)0xe237478);
        baseband_write_mem_table(NULL, (uint32_t)0xd74, (uint32_t)0xcb383cb);
        baseband_write_mem_table(NULL, (uint32_t)0xd75, (uint32_t)0x33422ad);
        baseband_write_mem_table(NULL, (uint32_t)0xd76, (uint32_t)0xee89fb3);
        baseband_write_mem_table(NULL, (uint32_t)0xd77, (uint32_t)0xe1bf48c);
        baseband_write_mem_table(NULL, (uint32_t)0xd78, (uint32_t)0xcbf8464);
        baseband_write_mem_table(NULL, (uint32_t)0xd79, (uint32_t)0x2f32241);
        baseband_write_mem_table(NULL, (uint32_t)0xd7a, (uint32_t)0xf141fc9);
        baseband_write_mem_table(NULL, (uint32_t)0xd7b, (uint32_t)0xe14749f);
        baseband_write_mem_table(NULL, (uint32_t)0xd7c, (uint32_t)0xccd04fa);
        baseband_write_mem_table(NULL, (uint32_t)0xd7d, (uint32_t)0x2b1a1de);
        baseband_write_mem_table(NULL, (uint32_t)0xd7e, (uint32_t)0xf3f1fdc);
        baseband_write_mem_table(NULL, (uint32_t)0xd7f, (uint32_t)0xe0d34b3);
        baseband_write_mem_table(NULL, (uint32_t)0xd80, (uint32_t)0xcdc458b);
        baseband_write_mem_table(NULL, (uint32_t)0xd81, (uint32_t)0x26fe185);
        baseband_write_mem_table(NULL, (uint32_t)0xd82, (uint32_t)0xf69dfea);
        baseband_write_mem_table(NULL, (uint32_t)0xd83, (uint32_t)0xe0634c7);
        baseband_write_mem_table(NULL, (uint32_t)0xd84, (uint32_t)0xcecc619);
        baseband_write_mem_table(NULL, (uint32_t)0xd85, (uint32_t)0x22e2136);
        baseband_write_mem_table(NULL, (uint32_t)0xd86, (uint32_t)0xf949ff5);
        baseband_write_mem_table(NULL, (uint32_t)0xd87, (uint32_t)0xdfef4db);
        baseband_write_mem_table(NULL, (uint32_t)0xd88, (uint32_t)0xcff06a3);
        baseband_write_mem_table(NULL, (uint32_t)0xd89, (uint32_t)0x1ebe0f0);
        baseband_write_mem_table(NULL, (uint32_t)0xd8a, (uint32_t)0xfbedffc);
        baseband_write_mem_table(NULL, (uint32_t)0xd8b, (uint32_t)0xdf7f4ef);
        baseband_write_mem_table(NULL, (uint32_t)0xd8c, (uint32_t)0xd124729);
        baseband_write_mem_table(NULL, (uint32_t)0xd8d, (uint32_t)0x1a9e0b3);
        baseband_write_mem_table(NULL, (uint32_t)0xd8e, (uint32_t)0xfe91fff);
        baseband_write_mem_table(NULL, (uint32_t)0xd8f, (uint32_t)0xdf13504);
        baseband_write_mem_table(NULL, (uint32_t)0xd90, (uint32_t)0xd2707aa);
        baseband_write_mem_table(NULL, (uint32_t)0xd91, (uint32_t)0x167e07f);
        baseband_write_mem_table(NULL, (uint32_t)0xd92, (uint32_t)0x129fff);
        baseband_write_mem_table(NULL, (uint32_t)0xd93, (uint32_t)0xdea7518);
        baseband_write_mem_table(NULL, (uint32_t)0xd94, (uint32_t)0xd3d0826);
        baseband_write_mem_table(NULL, (uint32_t)0xd95, (uint32_t)0x125e055);
        baseband_write_mem_table(NULL, (uint32_t)0xd96, (uint32_t)0x3c1ffc);
        baseband_write_mem_table(NULL, (uint32_t)0xd97, (uint32_t)0xde3b52d);
        baseband_write_mem_table(NULL, (uint32_t)0xd98, (uint32_t)0xd54089e);
        baseband_write_mem_table(NULL, (uint32_t)0xd99, (uint32_t)0xe42033);
        baseband_write_mem_table(NULL, (uint32_t)0xd9a, (uint32_t)0x651ff6);
        baseband_write_mem_table(NULL, (uint32_t)0xd9b, (uint32_t)0xddd3541);
        baseband_write_mem_table(NULL, (uint32_t)0xd9c, (uint32_t)0xd6c0910);
        baseband_write_mem_table(NULL, (uint32_t)0xd9d, (uint32_t)0xa2a01a);
        baseband_write_mem_table(NULL, (uint32_t)0xd9e, (uint32_t)0x8ddfec);
        baseband_write_mem_table(NULL, (uint32_t)0xd9f, (uint32_t)0xdd6b556);
        baseband_write_mem_table(NULL, (uint32_t)0xda0, (uint32_t)0xd85097d);
        baseband_write_mem_table(NULL, (uint32_t)0xda1, (uint32_t)0x61a009);
        baseband_write_mem_table(NULL, (uint32_t)0xda2, (uint32_t)0xb61fe0);
        baseband_write_mem_table(NULL, (uint32_t)0xda3, (uint32_t)0xdd0756b);
        baseband_write_mem_table(NULL, (uint32_t)0xda4, (uint32_t)0xd9f09e6);
        baseband_write_mem_table(NULL, (uint32_t)0xda5, (uint32_t)0x20e001);
        baseband_write_mem_table(NULL, (uint32_t)0xda6, (uint32_t)0xde1fd0);
        baseband_write_mem_table(NULL, (uint32_t)0xda7, (uint32_t)0xdca3580);
        baseband_write_mem_table(NULL, (uint32_t)0xda8, (uint32_t)0xdba0a49);
        baseband_write_mem_table(NULL, (uint32_t)0xda9, (uint32_t)0xfe0e001);
        baseband_write_mem_table(NULL, (uint32_t)0xdaa, (uint32_t)0x1059fbd);
        baseband_write_mem_table(NULL, (uint32_t)0xdab, (uint32_t)0xdc3f595);
        baseband_write_mem_table(NULL, (uint32_t)0xdac, (uint32_t)0xdd58aa6);
        baseband_write_mem_table(NULL, (uint32_t)0xdad, (uint32_t)0xfa12009);
        baseband_write_mem_table(NULL, (uint32_t)0xdae, (uint32_t)0x12c9fa7);
        baseband_write_mem_table(NULL, (uint32_t)0xdaf, (uint32_t)0xdbdf5aa);
        baseband_write_mem_table(NULL, (uint32_t)0xdb0, (uint32_t)0xdf1caff);
        baseband_write_mem_table(NULL, (uint32_t)0xdb1, (uint32_t)0xf626018);
        baseband_write_mem_table(NULL, (uint32_t)0xdb2, (uint32_t)0x1535f8f);
        baseband_write_mem_table(NULL, (uint32_t)0xdb3, (uint32_t)0xdb7f5bf);
        baseband_write_mem_table(NULL, (uint32_t)0xdb4, (uint32_t)0xe0ecb52);
        baseband_write_mem_table(NULL, (uint32_t)0xdb5, (uint32_t)0xf24202f);
        baseband_write_mem_table(NULL, (uint32_t)0xdb6, (uint32_t)0x1795f74);
        baseband_write_mem_table(NULL, (uint32_t)0xdb7, (uint32_t)0xdb235d4);
        baseband_write_mem_table(NULL, (uint32_t)0xdb8, (uint32_t)0xe2c4b9f);
        baseband_write_mem_table(NULL, (uint32_t)0xdb9, (uint32_t)0xee6a04e);
        baseband_write_mem_table(NULL, (uint32_t)0xdba, (uint32_t)0x19f1f56);
        baseband_write_mem_table(NULL, (uint32_t)0xdbb, (uint32_t)0xdac75e9);
        baseband_write_mem_table(NULL, (uint32_t)0xdbc, (uint32_t)0xe4a4be7);
        baseband_write_mem_table(NULL, (uint32_t)0xdbd, (uint32_t)0xea9e073);
        baseband_write_mem_table(NULL, (uint32_t)0xdbe, (uint32_t)0x1c45f36);
        baseband_write_mem_table(NULL, (uint32_t)0xdbf, (uint32_t)0xda6b5fe);
        baseband_write_mem_table(NULL, (uint32_t)0xdc0, (uint32_t)0xe688c2a);
        baseband_write_mem_table(NULL, (uint32_t)0xdc1, (uint32_t)0xe6de0a0);
        baseband_write_mem_table(NULL, (uint32_t)0xdc2, (uint32_t)0x1e91f13);
        baseband_write_mem_table(NULL, (uint32_t)0xdc3, (uint32_t)0xda13613);
        baseband_write_mem_table(NULL, (uint32_t)0xdc4, (uint32_t)0xe874c67);
        baseband_write_mem_table(NULL, (uint32_t)0xdc5, (uint32_t)0xe32e0d2);
        baseband_write_mem_table(NULL, (uint32_t)0xdc6, (uint32_t)0x20d5eee);
        baseband_write_mem_table(NULL, (uint32_t)0xdc7, (uint32_t)0xd9bb628);
        baseband_write_mem_table(NULL, (uint32_t)0xdc8, (uint32_t)0xea64c9f);
        baseband_write_mem_table(NULL, (uint32_t)0xdc9, (uint32_t)0xdf8e10c);
        baseband_write_mem_table(NULL, (uint32_t)0xdca, (uint32_t)0x2311ec7);
        baseband_write_mem_table(NULL, (uint32_t)0xdcb, (uint32_t)0xd96763d);
        baseband_write_mem_table(NULL, (uint32_t)0xdcc, (uint32_t)0xec58cd2);
        baseband_write_mem_table(NULL, (uint32_t)0xdcd, (uint32_t)0xdbfe14b);
        baseband_write_mem_table(NULL, (uint32_t)0xdce, (uint32_t)0x2545e9d);
        baseband_write_mem_table(NULL, (uint32_t)0xdcf, (uint32_t)0xd913652);
        baseband_write_mem_table(NULL, (uint32_t)0xdd0, (uint32_t)0xee50cff);
        baseband_write_mem_table(NULL, (uint32_t)0xdd1, (uint32_t)0xd87e190);
        baseband_write_mem_table(NULL, (uint32_t)0xdd2, (uint32_t)0x2771e72);
        baseband_write_mem_table(NULL, (uint32_t)0xdd3, (uint32_t)0xd8bf667);
        baseband_write_mem_table(NULL, (uint32_t)0xdd4, (uint32_t)0xf048d27);
        baseband_write_mem_table(NULL, (uint32_t)0xdd5, (uint32_t)0xd50e1db);
        baseband_write_mem_table(NULL, (uint32_t)0xdd6, (uint32_t)0x2991e44);
        baseband_write_mem_table(NULL, (uint32_t)0xdd7, (uint32_t)0xd86f67c);
        baseband_write_mem_table(NULL, (uint32_t)0xdd8, (uint32_t)0xf240d4b);
        baseband_write_mem_table(NULL, (uint32_t)0xdd9, (uint32_t)0xd1ae22b);
        baseband_write_mem_table(NULL, (uint32_t)0xdda, (uint32_t)0x2bade15);
        baseband_write_mem_table(NULL, (uint32_t)0xddb, (uint32_t)0xd81f691);
        baseband_write_mem_table(NULL, (uint32_t)0xddc, (uint32_t)0xf438d69);
        baseband_write_mem_table(NULL, (uint32_t)0xddd, (uint32_t)0xce5e281);
        baseband_write_mem_table(NULL, (uint32_t)0xdde, (uint32_t)0x2dbdde3);
        baseband_write_mem_table(NULL, (uint32_t)0xddf, (uint32_t)0xd7cf6a6);
        baseband_write_reg(NULL, (uint32_t)0xc00014, (uint32_t)0x0);
        EMBARC_PRINTF("/*** doa params programmed! ***/\n\r");
    }
/*** shadow bank programmed! ***/
/*** dc calib done! ***/
/*** interframe powersaving feature is on! ***/
/*** Baseband HW init done... ***/
}
