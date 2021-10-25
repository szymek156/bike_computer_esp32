# bike_computer_esp32

## TODO:
- [x] Assemble a prototype, sensors, display, buttons
- [x] Read from GPS, ALT/TEMP sensors
- [x] Provide an architecture (MVP) for handling GUI
- [x] Create a tool to do GUI mockups fast
- [x] Figure out how to create and read/write partitions
- [ ] Figure out BLE communication
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
- [ ] Looks like workouts are written in ZWO file format https://github.com/h4l/zwift-workout-file-reference
- [ ] https://medium.com/hackernoon/cloning-zwift-on-ios-part-2-reverse-engineering-a-workout-9d4ffabc29e8

## Very Quick & Dirty notes for FIT_SDK
TODO:
* Familiarize with Scale/Offset feature
* Dynamic Fields
* Components
* Common fields
* Keep in mind while writing the encoder about Best Practices chapter
* While writing Decoder: FIT message conversion chapter, and Plugin Framework chapter
* Download activities from strava and convert them to see the file structure.
### FitCSVTool
Under ```java/FitCSVTool.jar``` allows to decode binary FIT to the CSV.
```
java -jar FitCSVTool.jar
```
### Defining local FIT profile
There is a FitGen.exe binary (can be launched from wine!) which takes messages definitions from config.csv and generates C code. Can be used to reduce binary size. Currently it works only for C, it has no effect on C++.
```
 wine FitGen.exe -c
```

### Compiling C example inside FIT module
Changed encode.c code that shows how to create example FIT files. Can be used as a reference for FIT creation on the target. Note encode.c changed to encode.cpp, to allow lambdas in the macro.

```
~/source/esp/bike_computer_esp32/components/FIT/c/examples/encode$

g++ -I../../ ../../fit.c ../../fit_example.c ../../fit_crc.c ../../fit_convert.c encode.cpp
```
### Compilation of FIT_SDK
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


### FIT File definition
Described here: https://developer.garmin.com/fit/protocol/

The data records in the FIT file are the main content and purpose of the FIT protocol. There are two kinds of data records:

* **Definition message**
these define the upcoming data messages. A definition message will define a local message type and associate it to a specific FIT message, and then designate the byte alignment and field contents of the upcoming data message.

* **Data message**
these contain a local message type and populated data fields in the format described by the preceding definition message. The definition message and its associated data messages will have matching **local message types**.

All records contain a __1 byte Record Header__ that indicates whether the Record Content is a definition message, a normal data message or a compressed timestamp data message (Figure 6.b). The lengths of the records vary in size depending on the number and size of fields within them.


All data messages are handled locally, and the definition messages are used to associate local data message types to the global FIT message profile. For example, a definition message may specify that data messages of local message type 0 are Global FIT ‘lap’ messages (Figure 7). The definition message also specifies which of the ‘lap’ fields are included in the data messages (start_time, start_position_lat, start_position_long, end_position_lat, end_position_long), and the format of the data in those fields. __As a result, data messages can be optimized to contain only data, and the local message type is referenced in the header.__ Data messages are referenced to local message type.

### Header
|   Bit   |     Value     |  Description                                         |
|:-------:|:-------------:|------------------------------------------------------|
|    7    |       0       |  Normal Header                                       |
|    6    |     0 or 1    |  Message Type 1: Definition Message 0: Data Message  |
|    5    |  0 (default)  |  Message Type Specific                               |
|    4    |       0       |  Reserved                                            |
|  0 - 3  |     0 - 15    |  Local Message Type                                  |

Compressed header type not described for brevity.

Local message value helps with reducing FIT file size. For example if it's known that some message (like "lap") will be used very often, then create a Message Definition record and set local message to 0 for example. Later in the file (as long as local message is not overwritten by other Message Definition), Data Message refers in it's header to local message == 0. Data Messages can be interleaved with different local message values, as long as Definition Message was written before use.

Example:
```
Definition Message: "lap", local_message = 0, fields,....
Data Message: local_message = 0 ("lap"), data,.....
Data Message: local_message = 0 ("lap"), data,.....
Definition Message: local_message = 1 ("track"), fields,.....
Data Message: local_message = 1 ("track"), data,.....
Data Message: local_message = 0 ("lap"), data,..... // local_message = 0 is already known, no need to write Definition Message once again.
Data Message: local_message = 1 ("track"), data,.....
Data Message: local_message = 1 ("track"), data,.....
Data Message: local_message = 0 ("lap"), data,.....
Data Message: local_message = 0 ("lap"), data,.....
```
Here FIT file contains only two Definition Messages records, no need to write it before every single Data Message.

Header definition implies there can be 16 local messages defined at once. It may happen that FIT file will use more than that. Then Message Definition will have to be overwritten.

### Message types
1) __Definition Message__: this describes the architecture, format, and fields of upcoming data messages
```| Header | Reserved 1byte | Arch 1byte | Global Msg Number 2 bytes | #of fields 1byte | Field definition 3 bytes per field |```

    Optionally, record may be extended after Field definitions with:

    ```|#of Dev fields 1 byte|Dev field definition 3 bytes per field|```

    Definition message associates Global Msg Number from Global FIT profile to the local message (set in the header) in local profile. It additionally selects which fields from Global FIT are used in this local instance, and also, if there is such need, may define developer custom fields.

    Field definition is structured as follows:
    ```| Field def. number 1 byte | Size 1 byte | Base type 1 byte |```

    * Field def. number is the number from global FIT profile
    * Size describes length of the field in bytes. Multiples of it indicates that's an array. That's actually strange, that should be a number of elements of length defined for underlying Base type, not size in bytes.
    * Base type describes which type field holds, what number refers to what type is described in fit.h file. It also describes length in bytes for each type.

        Base type field is destructured further to single bits definition, but is skipped here for brevity.

2) __Data Message__: this contains data that is formatted according to a preceding definition message. A data message must start with a normal or compressed timestamp header indicating its local message type, and the record content must be formatted according to the definition message of matching local message type.



### FIT Messages
Defined and described in
* https://developer.garmin.com/fit/file-types/activity/
* https://developer.garmin.com/fit/file-types/course/
* https://developer.garmin.com/fit/file-types/workout/



## Partition table:
- https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html
- https://github.com/espressif/esp-idf/blob/master/examples/storage/spiffs/main/spiffs_example_main.c

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
