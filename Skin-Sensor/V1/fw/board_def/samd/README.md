# Arduino Core for SAMD21 (E variant) CPU

This repository containts the source code and configuration files of the Arduino Core
for Atmel's SAMD21 (E variant) processor, which is used on the skin sensor boards.

## Installation on Arduino IDE

First, you should have Arduino > 1.6

To satisfy the dependencies, just open the "Boards Manager" and install the package called:

"Arduino SAMD Boards (32-bit ARM Cortex-M0+)"

Then drag this folder (`board_def`) to the `hardware` folder in the sketch directory of your Arduino installation (or make a symlink). Restart Arduino and you should see the board `Sensor-Sticker V1` listed. Select this board and upload the sketch `Skin-Sensor-AIAA`.


