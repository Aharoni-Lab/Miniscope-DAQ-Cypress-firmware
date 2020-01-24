# Miniscope-DAQ-Cypress-firmware
Firmware for the UCLA Miniscope DAQ Box.

The Minsicope DAQ Firmware has been rewritten to both support the new [Miniscope DAQ Software](https://github.com/Aharoni-Lab/Miniscope-DAQ-QT-Software) and to increase its stability and performance. It is based off the [Cypress AN75779](https://www.cypress.com/documentation/application-notes/an75779-how-implement-image-sensor-interface-using-ez-usb-fx3-usb) UVC example project where the generic camera interface has been replaced with a flexible Minsicope interface. This firmware will only work with the new Miniscope Software so make sure to use the latet software release.

## Setting up the source code and building the firmware

If you are interested in just using the firmware with your Minsicope system, there is no need to download the source code and get it compiling on our own. All you need is the correct pre-build .img file located in the "Built_Firmware" folder in this repository. 

If however you want to built it yourself or make modifications, follow the following steps:

1. Get the [Cypress SDK](https://www.cypress.com/documentation/software-and-drivers/ez-usb-fx3-software-development-kit)
2. Load the project in EZ USB Suite
3. Make sure the "built settings" in the project include the -i2cconf in the elf2img.exe commands. Picture below for reference.

![](img/Project_Prop_Build_Setting.PNG)
