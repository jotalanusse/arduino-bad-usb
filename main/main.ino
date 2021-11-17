/*
 * Author: jotalanusse
 *         https://github.com/jotalanusse
 * GIT:
 *         https://github.com/jotalanusse/arduino-bad-usb
 *
 * Attributions:
 *         Special thanks to Seytonic for developing the original
 *         version of this script. Please go and check out his
 *         work:
 *
 *         https://twitter.com/seytonic
 *         https://www.youtube.com/seytonic
 */

#include "Keyboard.h"
#include <SD.h>
#include <SPI.h>
#include <string.h>

// Constants
const char newlineChar = '\n';
const char carriageReturnChar = '\r';
const char spaceChar = ' ';

// Global variables
const int pins[] = {5, 6, 7, 8, 9}; // Pins to be used as input

void setup() {
  int fileIndex = 0; // Index of the file to be read

  // Set up each pin as an input
  for (int i = 0; i < sizeof(pins); i = i + 1) {
    int pin = pins[i];
    pinMode(pin, INPUT_PULLUP);
  }

  // If the pin is in a state of HIGH, we add to the index
  for (int i = 0; i < sizeof(pins); i = i + 1) {
    int pin = pins[i];
    int pinWeight =  sizeof(pins) - i -1; // We have to subtract 1 because the for loop starts at 0

    if (digitalRead(pin) == HIGH) {
      fileIndex += pow(2, pinWeight);
    }
  }

  // Start the SD card on pin 4
  if (!SD.begin(4)) {
    return;
  }

  // Open the file
  String fileName = String(fileIndex) + ".txt";
  File scriptFile = SD.open(fileName);

  // If our file exists
  if (scriptFile) {
    Keyboard.begin();

    String line = "";

    // While there's is still bytes available in the file
    while (scriptFile.available()) {
      char character = scriptFile.read();

      // If the character is not a newline or a carriage return
      if (character != newlineChar && character != carriageReturnChar) {
        line += character;
      } else {
        processLine(line);
        line = "";
      }
    }
    processLine(line);

    scriptFile.close();
    Keyboard.end();
  }
}

void processLine(String line) {
  int spaceIndex = line.indexOf(spaceChar);

  // If a space exists this is a complex line (command + arguments, or set of keystrokes), otherwise it's a simple keystroke
  if (spaceIndex > -1) {
    String command = line.substring(0, spaceIndex);
    String argument = line.substring(spaceIndex + 1);

    if (command == "REM") { // This is a comment, so we just ignore it
      // Hello there!
    } else  if (command ==  "STRING") { // This will send a string of text as ke]yboard input
      Keyboard.print(argument);
    } else if (command ==  "DELAY") { // This will delay the script for a certain amount of time
      int delayTime = argument.toInt();
      delay(delayTime);
    } else { // This is a set of keystrokes we have to chain together
      String remain = argument;

      while (remain.length() > 0) {
        int latestSpace = remain.indexOf(spaceChar);

        if (latestSpace == -1) {
          pressKey(remain);
          remain = "";
        } else {
          pressKey(remain.substring(0, latestSpace));
          remain = remain.substring(latestSpace + 1);
        }

        delay(5);
      }
    }
  } else {
    pressKey(line);
  }

  Keyboard.releaseAll();
}

// TODO: Make this function more pretty
// TODO: Use a switch statement instead of if statements?
void pressKey(String key) {
  // Check if the key is a simple or special key
  if (key.length() == 1) {
    char character = key.charAt(0); // Get the character
    Keyboard.press(character); // Press the character
  } else {
    if (key == "ENTER") Keyboard.press(KEY_RETURN);
    if (key == "CTRL") Keyboard.press(KEY_LEFT_CTRL);
    if (key == "SHIFT") Keyboard.press(KEY_LEFT_SHIFT);
    if (key == "ALT") Keyboard.press(KEY_LEFT_ALT);
    if (key == "GUI") Keyboard.press(KEY_LEFT_GUI);
    if (key == "UP" || key == "UPARROW") Keyboard.press(KEY_UP_ARROW);
    if (key == "DOWN" || key == "DOWNARROW") Keyboard.press(KEY_DOWN_ARROW);
    if (key == "LEFT" || key == "LEFTARROW")  Keyboard.press(KEY_LEFT_ARROW);
    if (key == "RIGHT" || key == "RIGHTARROW")Keyboard.press(KEY_RIGHT_ARROW);
    if (key == "DELETE") Keyboard.press(KEY_DELETE);
    if (key == "PAGEUP") Keyboard.press(KEY_PAGE_UP);
    if (key == "PAGEDOWN") Keyboard.press(KEY_PAGE_DOWN);
    if (key == "HOME") Keyboard.press(KEY_HOME);
    if (key == "ESC")  Keyboard.press(KEY_ESC);
    if (key == "INSERT")  Keyboard.press(KEY_INSERT);
    if (key == "TAB") Keyboard.press(KEY_TAB);
    if (key == "END")Keyboard.press(KEY_END);
    if (key == "CAPSLOCK")  Keyboard.press(KEY_CAPS_LOCK);
    if (key == "F1") Keyboard.press(KEY_F1);
    if (key == "F2")  Keyboard.press(KEY_F2);
    if (key == "F3")  Keyboard.press(KEY_F3);
    if (key == "F4")  Keyboard.press(KEY_F4);
    if (key == "F5")  Keyboard.press(KEY_F5);
    if (key == "F6") Keyboard.press(KEY_F6);
    if (key == "F7") Keyboard.press(KEY_F7);
    if (key == "F8")  Keyboard.press(KEY_F8);
    if (key == "F9") Keyboard.press(KEY_F9);
    if (key == "F10")   Keyboard.press(KEY_F10);
    if (key == "F11") Keyboard.press(KEY_F11);
    if (key == "F12") Keyboard.press(KEY_F12);
  }
}

void loop() {
  // Nothing to do here
}