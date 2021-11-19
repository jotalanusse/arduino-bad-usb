
# Arduino BadUSB
A **Ducky Script** interpreter that runs on an **Arduino**, decodes and executes scripts saved on a **microSD** card.

The goal of this project is to recreate an affordable and DIY alternative to the legendary [Rubber Ducky](https://hak5.org/products/usb-rubber-ducky-deluxe) from Hak5. Ideally capable of executing most if not all Ducky Script payloads already available out there, without the need of parsing/converting them beforehand.

This project is based on the [original version](https://github.com/Seytonic/Duckduino-microSD) from [Seytonic](https://twitter.com/seytonic), kudos to him for the inspiration and the initial implementation.

## Hardware:
### Components:
There are 3 basic components required for this project:

 - **Arduino board**
 - **Micro SD module**
 - **DIP switch**

**Arduino board:** The Arduino board it's where the code for our BadUSB will be stored and processed, it will be the one in charge of parsin our scripts and connecting with the target computer.

 For this particular implementation the [Pro Micro - 5V/16MHz](https://www.sparkfun.com/products/12640) from SparkFun is used, featuring an 8-bit ATmega32U4 and Arduino support out of the box. The original board can get quite expensive, so I recommend getting a third party implementation from AliExpress for a fraction of the cost (~$6).

<p align="center">
  <img src="./img/pro-micro.png" alt="SparkFun Pro Micro - 5V/16MHz"/>
</p>

**Micro SD module:** Our micro SD card module will be used to store our different scripts, this saves us from having to reprogram/reupload our code into the Arduino every time we want to modify our script/s.

There's no specific micro SD card module that has to be used for it to work with our board, just make sure that it is Arduino compatible (it has to support the SPI communication protocol), but most manufacturers will simply specify that it works with Arduino in the product description. These modules are also sold in AliExpress for a very low cost (~$0.45)

<p align="center">
  <img src="./img/sd-module.png" alt="Micro SD card reader module"/>
</p>

**DIP switch:** The DIP switch is what will enable us to select our script from the variety of payloads stored in our SD without the need of having to edit our code to do so.

Once again there is no specific DIP switch that has to be used, actually you don't even have to use a DIP switch at all. You could use a combination of individual switches and put them together, but given that we want to minimize the size of our USB I recommend getting one. These switches are also available in AliExpress at a very low cost (~$1) for 5 switches.

<p align="center">
  <img src="./img/dip-switch.png" alt="DIP switch of 10 positions"/>
</p>

## Setup:
TODO

## TODO:
**Disclaimer:** This project is in a very early stage so there are a lot of things to get done, lot's of things may change during development and that's okay. But for now this are the things that need to be edited/added at the moment:

- [ ] Migrate to the **NicoHood HID** library.
- [ ] Add number pad support.
- [ ] Add support for the **DEFAULT_DELAY** command.
- [ ] Add an initial delay for every script to avoid errors
- [ ] Add support for the **PRINTSCREEN** key.
- [ ] Change how the script file variable is accessed
