# Miniscope-DAQ-Cypress-firmware
Firmware for Cypress USB Host Controller DAQ

## Setting up the source code and building the firmware

If you are interested in just using the firmware with your Minsicope system, there is no need to download the source code and get it compiling on our own. All you need is the correct pre-build .img file located in the "Built_Firmware" folder in this repository. 

If however you want to built it yourself or make modifications, follow the following steps:

1. Get the [Cypress SDK](https://www.cypress.com/documentation/software-and-drivers/ez-usb-fx3-software-development-kit)
2. Load the project in EZ USB Suite
3. Make sure the "built settings" in the project include the -i2cconf in the elf2img.exe commands. Picture below for reference.

![](img/Project_Prop_Build_Setting.PNG)
