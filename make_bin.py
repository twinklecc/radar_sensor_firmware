import os,argparse,sys,subprocess

def set_aes(src_dir, aes_flag):
    tar_file = src_dir + "\\post.py"
    datafile = open(tar_file,"r")
    lines = datafile.readlines()
    lines[10] = "security_flag = %s\n" % str(aes_flag)
    datafile = open(tar_file, "w")
    datafile.writelines(lines)
    datafile.close()

def set_boot_stage(src_dir, stage):
    tar_file = src_dir + "\\post.py"
    datafile = open(tar_file,"r")
    lines = datafile.readlines()
    lines[11] = "system_boot_stage = %s\n" % str(stage)
    datafile = open(tar_file, "w")
    datafile.writelines(lines)
    datafile.close()

def set_boot_split(src_dir, split_flag):
    tar_file = src_dir + "\\post.py"
    datafile = open(tar_file,"r")
    lines = datafile.readlines()
    lines[12] = "boot_split = %s\n" % str(split_flag)
    datafile = open(tar_file, "w")
    datafile.writelines(lines)
    datafile.close()
    
def cmd_subprocess(src_dir, cmd):
    process = subprocess.Popen(cmd, cwd=src_dir, shell=True)
    return process

def main(args):
    fw_cwd = os.getcwd()
    bt_cwd = fw_cwd + "\\calterah\\baremetal\\"

    if args.toolchain.lower() == "mw":
        # Toolchain: mw
        if args.cascade.lower() == "true":
            # Cascade
            bt_cmd = "make clean TOOLCHAIN=mw && make bin TOOLCHAIN=mw "
            fw_cmd = "make clean TOOLCHAIN=mw BOARD=ref_cascade && make bin TOOLCHAIN=mw BOARD=ref_cascade "
            gen_header_fw_cmd = "python post.py obj_alpsMP_ref_cascade_v1_sensor/mw_arcem6/sensor_mw_arcem6.bin firmware.bin "
            gen_header_bt_cmd = "python post.py calterah/baremetal/obj_alpsMP_ref_design_v1_boot/mw_arcem6/boot_mw_arcem6.bin boot.bin ram boot"
            log = "board = ref_cascade\r\ntoolchain = mw\r\n"
        else:
            # Non-cascade
            bt_cmd = "make clean TOOLCHAIN=mw && make bin TOOLCHAIN=mw "
            fw_cmd = "make clean TOOLCHAIN=mw && make bin TOOLCHAIN=mw "
            gen_header_fw_cmd = "python post.py obj_alpsMP_ref_design_v1_sensor/mw_arcem6/sensor_mw_arcem6.bin firmware.bin "
            gen_header_bt_cmd = "python post.py calterah/baremetal/obj_alpsMP_ref_design_v1_boot/mw_arcem6/boot_mw_arcem6.bin boot.bin ram boot"
            log = "board = ref_design\r\ntoolchain = mw\r\n"
    else:
        # Toolchain: gnu
        if args.cascade.lower() == "true":
            # Cascade
            bt_cmd = "make clean && make bin "
            fw_cmd = "make clean BOARD=ref_cascade && make bin BOARD=ref_cascade "
            gen_header_fw_cmd = "python post.py obj_alpsMP_ref_cascade_v1_sensor/gnu_arcem6/sensor_gnu_arcem6.bin firmware.bin "
            gen_header_bt_cmd = "python post.py calterah/baremetal/obj_alpsMP_ref_design_v1_boot/gnu_arcem6/boot_gnu_arcem6.bin boot.bin ram boot"
            log = "board = ref_cascade\r\ntoolchain = gnu\r\n"
        else:
            # Non-cascade
            bt_cmd = "make clean && make bin "
            fw_cmd = "make clean && make bin "
            gen_header_fw_cmd = "python post.py obj_alpsMP_ref_design_v1_sensor/gnu_arcem6/sensor_gnu_arcem6.bin firmware.bin "
            gen_header_bt_cmd = "python post.py calterah/baremetal/obj_alpsMP_ref_design_v1_boot/gnu_arcem6/boot_gnu_arcem6.bin boot.bin ram boot"
            log = "board = ref_design\r\ntoolchain = gnu\r\n"

    if args.stage == "2":
        # 2 stage boot
        gen_header_bt_cmd = ""
        bt_cmd = ""
        if args.method.lower() == "xip":
            # 2 stage xip
            set_boot_stage(fw_cwd, 2)
            set_boot_split(fw_cwd, 0)
            fw_cmd += "FLASH_XIP=1 LOAD_XIP_TEXT_EN=1 "
            gen_header_fw_cmd += "xip"
            log += "mode = 2 stage xip "
        else:
            # 2 stage ram
            set_boot_stage(fw_cwd, 2)
            set_boot_split(fw_cwd, 0)
            gen_header_fw_cmd += "ram"
            log += "mode = 2 stage ram "
        if args.vendor != None:
            # 2 stage xip with vendor
            fw_cmd += "FLASH_TYPE=%s " %args.vendor.lower()  
    else:
        # 3 stage boot
        bt_cmd += "SYSTEM_BOOT_STAGE=3 "
        fw_cmd += "SYSTEM_BOOT_STAGE=3 "
        if args.split.lower() == "true":
            # 3 stage split xip
            set_boot_stage(fw_cwd,3)
            set_boot_split(fw_cwd,1)
            fw_cmd += "FLASH_XIP=1 LOAD_XIP_TEXT_EN=1 "
            bt_cmd += "ELF_2_MULTI_BIN=1 "
            gen_header_fw_cmd += "xip"
            log += "mode = 3 stage boot split xip "
            if args.vendor != None:
                # 3 stage split xip with vendor
                fw_cmd += "FLASH_TYPE=%s " %args.vendor.lower()
                bt_cmd += "FLASH_TYPE=%s " %args.vendor.lower()
        else:
            # 3 stage non-split
            set_boot_stage(fw_cwd,3)
            set_boot_split(fw_cwd,0)
            if args.method.lower() == "xip":
                # 3 stage non-split xip
                fw_cmd += "FLASH_XIP=1 LOAD_XIP_TEXT_EN=1 "
                gen_header_fw_cmd += "xip"
                log += "mode = 3 stage non-split xip "
            else:
                # 3 stage no-split ram
                gen_header_fw_cmd += "ram"
                log += "mode = 3 stage non-split ram "
        if args.vendor != None:
            # 3 stage non-split xip with vendor
            fw_cmd += "FLASH_TYPE=%s " %args.vendor.lower()
            bt_cmd += "FLASH_TYPE=%s " %args.vendor.lower()

    if args.bbacc.lower() == "true":
        fw_cmd += "ACC_BB_BOOTUP=2 "
        log += "bb_acc "
    if args.aes.lower() == "true":
        set_aes(fw_cwd, 1)
        log += "aes\r\n"
    else:
        set_aes(fw_cwd, 0)
        log += "\r\n"
    print(log)
    if args.test.lower() == "true":
        print("cd \\calterah\\baremetal\\")
        print(bt_cmd)
        print("cd ..\\..")
        print(fw_cmd)
        print(gen_header_fw_cmd)
        print(gen_header_bt_cmd)
    else:
        p1 = cmd_subprocess(fw_cwd, fw_cmd)
        p2 = cmd_subprocess(bt_cwd, bt_cmd)
        p1.wait()
        p2.wait()
        cmd_subprocess(fw_cwd, gen_header_fw_cmd)
        cmd_subprocess(fw_cwd, gen_header_bt_cmd)     
    


if __name__ == "__main__":
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument('-stage', '-s', help = "Specify 2-stage or 3-stage boot [2/3]", default = "2")
    parser.add_argument('-method', '-m', help = "Specify method of executing [ram/xip]", default = "ram")
    parser.add_argument('-split', '-sp', help = "Split boot or not [true/false]", default = "false")
    parser.add_argument('-vendor', '-v', help = "Specify flash vendor for xip [giga/winbond/micron/microchip/mxic/s25fls]")
    parser.add_argument('-bbacc', '-ba', help = "Specify baseband acceleration [true/false]", default = "false")
    parser.add_argument('-aes', help = "enable firmware aes [true/false]", default = "false")
    parser.add_argument('-toolchain', '-tc', help = "Specify toolchain [gnu/mw]", default = "gnu")
    parser.add_argument('-cascade', '-c', help = "Specify whether firmware is used for cascade [true/false]", default = "false")
    parser.add_argument('-test', help = "Print command", default = "false")
    args = parser.parse_args()
    main(args)