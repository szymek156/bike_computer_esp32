# bike_computer_esp32

## TODO:
[x] Assemble a prototype, sensors, display, buttons
[x] Read from GPS, ALT/TEMP sensors
[x] Provide an architecture (MVP) for handling GUI
[x] Create a tool to do GUI mockups fast
[ ] Support FIT Activity files
[ ] Find out faster way for display to show now only 1fps is effective
[ ] Frame transfer takes 200ms, consider transferring only dirty regions
[ ] Add ghost runner functionality
[ ] Support FIT Workout files
[ ] Support FIT Course files
[ ] Add travis CI
[ ] clang-tidy?
[ ] Calibrate altitude
[ ] Read about temp, seems to be off by 2*
[ ] Consider making paint class more cache friendly
[ ] Create a tool to make Workout FIT files (WASM?)
[ ] Create charts (WASM?)
[ ] Use FIT files on the device
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
