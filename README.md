# Modern Activity Tracker done right

The goal is to create a device that will last on the battery as long as possible, be able to communicate with sensors, like cadence, HR, store activity to de-facto standard file type called FIT. All of it with help of widely accessible HW components.


Currently focus is on (indoor) cycling activity, hence the project name.

TODO list is available [here](TODO.md)

Currently device looks like this, switched to Memory In Display LCD

<img src="https://user-images.githubusercontent.com/1136779/144249966-40f97204-0211-4b5a-9904-d35a4ad28dda.jpg" alt="drawing" width="300"/>

There is a demo of it, when display was an e-ink
https://www.youtube.com/watch?v=1cKFIr01Nvw

## Main highlights of current state
* Hardware is wired and works
* SW architecture is chosen and implemented
* Created a GUI for Activities, Statistics
* Able to select from several Activity types
* Activity starts, shows summary data, records are stored in FIT file
* Bluetooth Low Energy GATTS server is running, exposing characteristic to list files, and fetch selected file
* Currently WIP: Add GUI for BLE

