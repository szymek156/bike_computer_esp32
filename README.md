# bike_computer_esp32

## TODO:
- [x] Assemble a prototype, sensors, display, buttons
- [x] Read from GPS, ALT/TEMP sensors
- [x] Provide an architecture (MVP) for handling GUI
- [x] Create a tool to do GUI mockups fast
- [ ] Support FIT Activity files
- [ ] Find out faster way for display to show now only 1fps is effective
- [ ] Frame transfer takes 200ms, consider transferring only dirty regions
- [ ] Add ghost runner functionality
- [ ] Support FIT Workout files
- [ ] Support FIT Course files
- [ ] Add travis CI
- [ ] Add OTA support (2 partitions + factory)
- [ ] clang-tidy?
- [ ] Calibrate altitude
- [ ] Read about temp, seems to be off by 2*
- [ ] Consider making paint class more cache friendly
- [ ] Create a tool to make Workout FIT files (WASM?)
- [ ] Create charts (WASM?)
- [ ] Use FIT files on the device


## Very Quick & Dirty notes for FIT_SDK
To compile C:
Make sure ```#define FIT_USE_STDINT_H``` is set in fit_config.h

```
~/Downloads/FIT_SDK/c/examples/encode$
gcc -I../../ ../../fit.c ../../fit_example.c ../../fit_crc.c ../../fit_convert.c encode.c
```

To compile C++:
```
~/Downloads/FIT_SDK/cpp/examples/encode$
g++ -I../../ ../../*.cpp encode.cpp
```

There is a FitGen.exe binary (can be launched from wine!) which takes messages definitions from config.csv and generates C code. Can be used to reduce binary size. Currently it works only for C, it has no effect on C++.
```
 wine FitGen.exe -c
```
## Partition table:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html
https://github.com/espressif/esp-idf/blob/master/examples/storage/spiffs/main/spiffs_example_main.c

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
