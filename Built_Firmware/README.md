## Supported devices of the Miniscope DAQ firmware
The Miniscope DAQ firmware now supports a range of devices with a single firmware .img file instead of needing a different firmware file for each device. This means you can now program your Miniscope DAQ once and it will support all Miniscope related devices without you needing to do anything more.

* You will use the "128K_EEPROM.img" file if you have a v3.2 DAQ (or one with an EEPROM mounted into the 4 x 2 socket on the PCB).
* You will use the "256K_EEPROM.img" file if you have a v3.3 DAQ or MiniDAQ (or any DAQ with a small surface mount EEPROM IC).

The list of currently supported devices are:
* V3 Miniscope 
  * Resolution: 752px X 480px
* V4 Miniscope 
  * Resolution: 608px X 608px
* Minicam
  * Resolution: 2592px X 1944px, 1296px X 972px, 1024px X 768px, 800px X 800px
* MiniLFM v2
  * Resolution: 2592px X 1944px, 1296px X 972px, 1024px X 768px, 800px X 800px
* LFOV Miniscope
  * Resolution: 2592px X 1944px, 1296px X 972px, 1024px X 768px, 800px X 800px