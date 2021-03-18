# bike_computer_esp32

## TODO:
Add travis CI
Docker image for build environment?
clang-tidy?
Calibrate altitude
## Connect to uart
screen /dev/ttyUSB0 115200

exit: ctrl+a, k

or
idf.py -p /dev/ttyUSB0 monitor
exit: ctrl+]
## Basic build
cd esp-idf
. ./export.sh
cd bike_computer_esp32
idf.py build (can be skipped)
idf.py -p /dev/ttyUSB0 flash

### One liner:
idf.py --ccache -p /dev/ttyUSB0 flash  monitor