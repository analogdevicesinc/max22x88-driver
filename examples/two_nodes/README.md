# Stack with MAX22x88 example

This example demonstrates the use of MAX22088/MAX22288 drivers and protocol stack.

The application consistis of two MAX32670 microcontrollers connected via a MAX22088 evaluation kit.
The devices exchange data in response to input from the user.

## The Setup

The devies required for this example are:

| Name | Device |
| --- | --- |
| MCU 1 | MAX32670 EvKit Rev B |
| MCU 2 | MAX32670 EvKit Rev B |
| HBS | MAX22088 EvKit B |
| PS1 | 18 V / 200 mA power supply |
| PS2 | 6 V / 200 mA power supply |

The boards are configured as follows:

| Device | Configuration |
| --- | --- |
| HBS | JP1 Open<br>JP2 1-2<br>JP3 1-2<br>JP4 1-2<br>JP5 Open<br>JP6 Open<br>JP7 Open<br>JP8 1-2<br>JP9 1-2<br>JP11 Open<br>JP12 1-2<br>JP13 Open<br>JP14 Open
| MCU n | JP4 TX0<br>JP1 1-2<br>JP2 1-2 |

Make the following connections:

- MCU 1 is connected to HBS's master circuit
- MCU 2 is connected to HBS's remote circuit

| Device A | Pin A | Device B | Pin B |
| --- | --- | --- | --- |
| HBS | RST1 | MCU 1 | P0.26 |
| HBS | DIN1 | MCU 1 | P0.15 |
| HBS | DOUT1 | MCU 1 | P0.14 |
| HBS | GND1  | MCU 1 | GND |
| HBS | RST2 | MCU 2 | P0.26 |
| HBS | DIN2 | MCU 2 | P0.15 |
| HBS | DOUT2 | MCU 2 | P0.14 |
| HBS | GND2  | MCU 2 | GND |

HBS master's circuit provides the Home Bus line that powers all devices.
The HBS remote and master circuit should be connected with twisted pair cables as follows:

| Device A | Pin A | Device B | Pin B |
| --- | --- | --- | --- |
| HBS | H1+ | HBS | H2+ |
| HBS | H1- | HBS | H2- |

The power supplies are connected to HBS's master circuit.

| Device A | Pin A | Device B | Pin B|
| --- | --- | --- | --- |
| PS1 | + | HBS | XFMR_IN |
| PS1 | - | HBS | GND1 |
| PS2 | + | HBS | VRAW1 |
| PS2 | - | HBS | GND1 |

A device behaves as slave by default. Configure MCU 2 as the master with the following connection:

| Device A | Pin A | Device B | Pin B | Note |
| --- | --- | --- | --- | --- |
| MCU 2 | P0.12 | MCU 2 | GND | Configures device as master. |

Finally, power MCU 1 and MCU 2 through a PC and monitor the information sent via UART through a serial monitor application.

## The Application

The two devices connected via Home Bus take on different roles (master or slave) and demonstrate the exchange of data upon user interaction.
During execution, both devices provide text feedback via UART at 115200 baud.

The application defines two roles for the microcontrollers. MCU 1 is configured as the slave, and MCU 2 is configured as the master.
The user interact with each device via the SW3 general purpose switch on the MAX32670 evaluation kit.

The application execution flow is as follows:

- MCU 1 and MCU 2 boot up.
- MCU 1 is configured as the slave, MCU 2 is configured as the master.
- User presses SW3 on MCU 2.
- MCU 2 sends a request to MCU 1.
- MCU 1 processes the request, reads the state of its SW3 button.
- MCU 1 replies to MCU 2 with the button state
- MCU 2 processes the response from MCU 1.

During execution, detailed information is sent via UART and can be read from a PC.

## Building and Flashing

The application has been developed and tested with the [Analog Devices MSDK February 2024 Release](https://github.com/analogdevicesinc/msdk/releases/tag/v2024_02) and the build files are for the [VSCode-Maxim](https://github.com/analogdevicesinc/VSCode-Maxim) Visual Studio Code extension.
