# coding: utf-8

import os
import sys
import serial
from serial.tools import list_ports
import subprocess
from subprocess import DEVNULL


if sys.platform == 'win32':
    GDB_EXE = 'arc-elf32-gdb.exe'
    ARC_GNU = r'C:\arc_gnu'
elif sys.platform.startswith('linux'):
    GDB_EXE = 'arc-elf32-gdb'
    ARC_GNU = '/opt/arc_gnu'
else:
    raise NotImplementedError


def kill_gdb():
    if sys.platform == 'win32':
        subprocess.call("taskkill /F /T /IM arc-elf32-gdb.exe",stdout=DEVNULL,stderr=DEVNULL)
    elif sys.platform.startswith('linux'):
        os.system('killall {}'.format(GDB_EXE))
    else:
        raise NotImplementedError


def find_devices(comports=None, vid=None, pid=None):
    if vid is None:
        vid = 0x0403
    vid = (vid, ) if isinstance(vid, int) else vid

    if pid is None:
        pid = (0x6010, 0x6001)
    pid = (pid, ) if isinstance(pid, int) else pid

    comports = comports or list_ports.comports()
    devices = []
    for info in comports:
        if info.vid in vid and info.pid in pid:
            devices.append(info)
    return devices


def find_available_comport(comports=None, vid=None, pid=None):
    devices = find_devices(comports, vid, pid)
    if len(devices) == 0:
        return None
    elif len(devices) == 1:
        comport = devices[0].device
    else:
        raise Exception("Can't find device.")
    return comport


def make_run(fw_src_dir, cleanup=True):
    cmd = 'make run ACC_BB_BOOTUP=1'
    if cleanup:
        cmd = 'make clean && ' + cmd
    print(cmd)
    process = subprocess.Popen(cmd, cwd=fw_src_dir, shell=True)
    return process
    
def make_clean(fw_src_dir):
    cmd = 'make clean'
    print(cmd)
    process = subprocess.Popen(cmd, cwd=fw_src_dir, shell=True)
    
def get_cwd():
    cwd = os.getcwd()
    return cwd


def main(comport, baudrate, fw_src_dir):
    kill_gdb()
    ser = serial.Serial()
    ser.port = comport
    ser.baudrate = baudrate
    ser.open()
    process = make_run(fw_src_dir, cleanup=True)
    started = False
    try:
        data = b''
        while True:
            line = ser.readline()
            if b"if(bb_hw->frame_type_id == 0){" in line:
                started = True
            if b'<EOF>' in line:
                break
            if started:
                data += line
        with open(os.path.join(fw_src_dir, "calterah", "common", "baseband", "baseband_bb_bootup.h"), "wb") as f:
            f.write(data)
    finally:
        ser.close()
        make_clean(fw_src_dir)
        process.terminate()
        kill_gdb()



if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument('--comport', default=find_available_comport())
    parser.add_argument('--baudrate', type=int, default=3000000)
    parser.add_argument('--src-dir', help="specify firmware source dir.", default=get_cwd())
    args = parser.parse_args()

    main(args.comport, args.baudrate, args.src_dir)
