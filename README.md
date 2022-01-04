
# My Clock Project

  Early sleep clock. it does:
  - different color for late night. like: blue/green for normal, red after 9pm
  - color flashing. like: do red blue flashing after 11pm, high frequency flashing after 11:30pm
  - beep notify, like: do beep after 12pm, high frequency beep after 12:30pm
  - sleep mode: light blue/green when sleep
  - use light sensor to detect slept or not.

## Commands Note

setup arduino cli environment:
```sh
# list all boards connected to PC
~/opt/bin/arduino-cli board list

~/opt/bin/arduino-cli core install arduino:avr

~/opt/bin/arduino-cli core update-index
```

arduino project management:
```sh
# Create/Compile/Upload a project
~/opt/bin/arduino-cli sketch new MyBlink
~/opt/bin/arduino-cli compile --fqbn arduino:avr:nano MyBlink
~/opt/bin/arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano MyBlink
```

