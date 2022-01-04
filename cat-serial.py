#!/usr/bin/env python3

import os
import termios

os.system('stty -F /dev/ttyUSB0 9600')
fd = os.open('/dev/ttyUSB0', os.O_RDWR)

#attr = termios.tcgetattr(fd)
#attr[5] = attr[6] = termios.B9600
#termios.tcsetattr(fd, termios.TCSANOW, attr)

fh = open(fd)

while True:
    line = fh.readline()
    print(line.rstrip())
