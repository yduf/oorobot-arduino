# Oorobot-arduino

This is the source code of the OoRoBoT firmware.
splitted in more understandable source parts (see ChANGELOG.md)

## How to build

- This fork use [Platform.io](https://platformio.org/) IDE which can be installed as an extension of [vscode](https://code.visualstudio.com/)
- Get the following libraries (using the Arduino IDE library manager)
	- **AccelStepper** v1.57.1 by Mike McCauley
	- **LiquidCrystal I2C** v1.1.2 by Frank de Brabander
- Open `main.cc` in the IDE
- You're all set: hit the regular build/download buttons

## Calibrating buttons

To calibrate the motors use the setting menu.

Press Setting menu (long press)
It display a menu, where you can configure
- distance for one move in cm,
- calibrate nb of step for 90° (1/4) rotation,
- calibrate nb of step for 10cm
- bluetooth identifier for robot (read only)

Press settings or play again to save this in eeprom.
Press cancel to abort saving.
