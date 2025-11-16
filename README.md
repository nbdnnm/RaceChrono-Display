_You may [buy me a tea](https://buymeacoffee.com/yugene.ka) if you want to thank me. Also, [I can make the device on demand](mailto:yugene.ka@gmail.com)._

# RaceChrono Display

This project describes how to create an external display to show data from [RaceChrono](https://racechrono.com/) suitable for use on a race track or in daily applications.

The total price for the parts is about 50-70$/Euro (depends if you can print the case cheap)

![alt text](<pics/display_front_on.JPG>)

## Bill of materials
* ESP32 - the MCU with BLE to manage everything. I used a devkit 30-pin with USB-C, but you can adjust to any kind of ESP32, just reshape the holes and the pins soldering and mapping in the code.
* 3x3.5 inch ST7796 displays, I bought the one by Tenstar Robot from Aliexpress.
* a bit of 24awg wires.
* 12 M2x14mm screws + 12 M2x4mm heat inserts (OD 3.5) to keep everything in place.
* 1 heat insert 1/4-20 length 8mm (OD8mm) to mount on the windshield/dashboard by a GoPro compatible holder.


## Instructions

1. Print the bottom, the top of the box and the cap from the 3d models folder. Use anything up to your taste, or PETG if you don't know what to choose as a material for printing. Print the stick model two times with TPU. They are needed to keep the displays in place by pushing them against the back of the case.

2. Assemble the components as shown below.

![alt text](<pics/RaceChrono display assembling.jpg>)
Place the displays with pins in on the same side (on the right or the left).

![alt text](<pics/displays_wiring.JPG>)

![alt text](<pics/esp32_wiring.JPG>)

Solder the wires to the pins as shown below.

| Display1 | Display2 | Display3 | ESP32 |
|----------|----------|----------|-------|
| SDA      | SDA      | SDA      | 23    |
| SCL      | SCL      | SCL      | 18    |
| DC       | DC       | DC       | 17    |
| RST      | RST      | RST      | 16    |
| BL       | BL       | BL       | 15    |
| CS       |          |          | 25    |
|          | CS       |          | 26    |
|          |          | CS       | 27    |

So all the pins of the displays are connected sequentially to each other and then to ESP32, except the CS pins, which are connected to individual ESP32 pins.

The pins mapping can be found in the tft_setup.h file and main.cpp.

Put all the heat inserts and screws.

![alt text](<pics/case_back_2.JPG>)
![alt text](<pics/case_front.JPG>)
![alt text](<pics/case_back.JPG>)

3. Then download this repo, build and upload it with Visual Studio Code with the PlatformIO plugin.

```setRotation``` and ```onScreen``` in the main.cpp are responsible for manipulation the data on the screens. Play with it if you want to adjust it.

```monitors``` array define which data to get from RaceChrono. Put there anything you need and available in https://racechrono.com/support/equations/identifiers