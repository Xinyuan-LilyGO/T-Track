<h1 align = "center"> 🌟T-Track🌟</h1>


## Pinout

<img width="500" align="middle"  src=image/T-Track.jpg>
<img width="500" align="middle"  src=image/T-Track-Pin.jpg>


## Quick Start

The board uses USB as the JTAG upload port. When printing serial port information on USB, CDC_ON_BOOT configuration needs to be turned on. 
If the port cannot be found when uploading the program or the USB has been used for other functions, the port does not appear. 
Please enter the upload mode manually. 
-  Press and hold the BOOT button when the power is off, the button is located behind the RST button.
-  When the USB is connected, first press and hold the BOOT button, then click reset and then release the BOOT button

### Arduino

1. Inastall [Arduino 2.x](https://www.arduino.cc/en/software)
2. In Arduino Preferences, on the Settings tab, enter the `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` URL in the `Additional boards manager URLs` input box. 
3. Click OK and the software will install. 
4. Search for ESP32 in Tools → Board Manager and install ESP32-Arduino SDK (version 2.0.3 or above)
5. Copy everything from this repository lib folder to [Arduino library folder](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries#manual-installation) (e.g. `C:\Users\YourName\Documents\Arduino\libraries`)
6. Select the correct settings in the Tools menu as shown below.

| Setting                  | Value                            |
| :----------------------- | :------------------------------- |
| Board                    | ESP32 S3 Dev Module              |
| USB CDC On Boot          | Enabled                          |
| CPU Frequency            | 240MHz (WiFi)                    |
| Core Debug Level         | None                             |
| USB DFU On Boot          | Enabled                          |
| Events Run On            | Core 1                           |
| Flash Mode               | QIO 80MHz                        |
| Flash Size               | 16MB (128Mb)                     |
| JTAG Adapter             | Integrated USB JTAG              |
| Arduino Runs On          | Core 1                           |
| USB Firmware MSC On Boot | Disabled                         |
| Partition Scheme         | 16M Flash (3MB APP/9.9MB SPIFFS) |
| PSRAM                    | OPI PSRAM                        |
| USB Mode                 | Hardware CDC and JTAG            |

### PlatformIO

1. PlatformIO plug-in installation: click on the extension on the left column → search platformIO → install the first plug-in
2. Click Platforms → Embedded → search Espressif 32 in the input box → select the corresponding firmware installation

### Pins Map

| Name           | GPIO NUM |
| -------------- | -------- |
| G1             | 1        |
| G2             | 2        |
| G3             | 3        |
| G4             | 10       |
| AMOLED SCK     | 36       |
| AMOLED MISO    | 34       |
| AMOLED MOSI    | 35       |
| AMOLED RST     | 38       |
| AMOLED DC      | 46       |
| AMOLED CS      | 37       |
| Button1 (BOOT) | 0        |
| On Board LED   | 21       |
| Power Enable   | 4        |
| Battery ADC    | NC       |

- Power Enable: Control trackball and AMOLED power
- Battery ADC: No onboard battery ADC voltage divider resistor

## LED Description

| Function   | Color |
| ---------- | ----- |
| Charge LED | Red   |
| User LED   | Green |

### Charging Indicator LED Status

1. Flashes or dimly lights when no battery is connected.
2. Remains on while charging.
3. Turns off when fully charged.

## Tips

1. When the USB is repeatedly disconnected, it means that the program is abnormal. At this time, you need to follow the steps below
   - Press and hold the **BOOT(In the middle of the trackball, press down)** button (without releasing it), then press the **RST** button, then release the **RST** button, and finally release the **BOOT(In the middle of the trackball, press down)** button
   - Click the upload button in the IDE and wait for the upload to complete
   - Press the **RST** button to exit the download mode
