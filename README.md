# Pacman Clock using ESP32 and 64x64 LED Matrix. 
Game controlled by PS4 Controller, or stand-alone plays by itself.
Modified Pacman Clock code from original by jnthas. For ESP32 D1 Mini.
WiFi setup for AP mode, but left original code for WiFiManager if you wish to enable.

To use PS4 Controllers you must install the "ESP32 Arduino + Bluepad32" Board setup.
You can use this YouTube Guide: https://www.youtube.com/watch?v=0jnY-XXiD8Q.

Place the following link, under Preferences into the Additional Boards Manager URL window:

https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json

Search and Install the new "esp32_bluepad32" Board Manager.
When complete, it will show up under Board Manager dropdown as "ESP32 Arduino + Bluepad32".
Compile the Pacman-Clock-with-PS4-Controller.ino program as normal.
*** The Pacman program will not compile without this added Board Manager! ***

Video: https://www.youtube.com/watch?v=4IPQbADvfUQ

Pacman Clock displays time and date.
Game play will occur upon bootup. Manual game play will be enabled when a 
PS4 Bluetooth controller is connected and the trigger button pushed.

The trigger button will toggle between auto game pla and manual game play.

Schematics, photos, etc. 
