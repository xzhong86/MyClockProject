#!/bin/sh

cli=$HOME/opt/bin/arduino-cli
fqbn=arduino:avr:nano
tty=/dev/ttyUSB0

$cli compile -b $fqbn .  || exit 1
$cli upload -b $fqbn -p $tty .  || exit 1

