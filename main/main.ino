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
const char NEWLINE_CHAR = '\n';
const char CARRIAGE_RETURN_CHAR = '\r';
const char SPACE_CHAR = ' ';

const char COMMAND_END_CHARS[] = {SPACE_CHAR, NEWLINE_CHAR, CARRIAGE_RETURN_CHAR};
const char LINE_END_CHARS[] = {NEWLINE_CHAR, CARRIAGE_RETURN_CHAR};

const int MAX_STRING_LENGTH = 1024;

// Global variables
const int pins[] = {5, 6, 7, 8, 9}; // Pins to be used as input

// The main function where the program is set up
void setup() {
  int fileIndex = 0; // Index of the file to be read

  // Set up each pin as an input
  for (int i = 0; i < sizeof(pins); i += 1) {
    int pin = pins[i];
    pinMode(pin, INPUT_PULLUP);
  }

  // If the pin is in a state of HIGH, we add to the index
  for (int i = 0; i < sizeof(pins); i += 1) {
    int pin = pins[i];
    int pinWeight =  sizeof(pins) - i -1; // We have to subtract 1 because the for loop starts at 0

    if (digitalRead(pin) == HIGH) {
      fileIndex += pow(2, pinWeight);
    }
  }

  // Parse the index to a file name
  String fileName = String(fileIndex) + ".txt"; // The file name is the index of the file to be read

  // Start the SD card on pin 4
  if (!SD.begin(4)) {
    return; // If the SD card fails to start, we exit
  }

  // Open the script file
  File file = SD.open(fileName);

  // If the file exist, process it
  if (file) {
    Keyboard.begin(); // Start the keyboard for sending keystrokes

    processFile(file); // Start processing the file
    file.close();

    Keyboard.end(); // End the keyboard
  }
}

// TODO: Rename end character
// Read a file until and end character is found or a byte limit is reached
String readFile(File file, char endCharacters[], int byteLimit = 0) {
    String result = ""; // The result of our read operation

    // While there's is still bytes available in the file
    while (file.available()) {
      // This is rather confusing, but let me explain.
      // By using "read()" instead of "peek()" we still
      // don't know if the character is an end character
      // or not, but the header moves by one byte anyway.
      //  
      // So any end character is unintentionally avoided
      // Not only from the current read operation, but also
      // from the next one.
      char character = file.read(); // Read the next character

      // If the character is not present in our end characters array
      for (int i = 0; i < sizeof(endCharacters); i += 1) {
        char endCharacter = endCharacters[i]; // Thee current end character we want to compare

        // If the character is an end character we return our current result
        if (character == endCharacter) {
           return result;
        }
      }

      result += character; // Add the character to our result and keep going

      // If we've reached our byte limit for our read operation return the result
      if (result.length() == byteLimit) {
        return result;
      }
    }

    return result; // Finally return the result
}

// Read a file and process the commands it contains
void processFile(File file) {
    // While there's is still bytes available in the file
    while (file.available()) {
      String command = readFile(file, COMMAND_END_CHARS); // Read until me encounter one or more end characters

      // Only process the command if it's not empty // TODO: Check if this is actually needed
      if (command != "") {
        processCommand(command, file); // Process the command accordingly
      }
    }
}

void processCommand(String command, File file) {
  if (command == "REM") { // This is a comment, so we just ignore it
    // Hello there!
  } else  if (command ==  "STRING") { // This will send a string of text as ke]yboard input
    processStringCommand(file);
  } else if (command ==  "DELAY") { // This will delay the script for a certain amount of time
    processDelayCommand(file);
  } else { // This is a single keystroke, or a set of keystrokes we have to chain together
  //   String remain = argument;

  //   while (remain.length() > 0) {
  //     int latestSpace = remain.indexOf(SPACE_CHAR);

  //     if (latestSpace == -1) {
  //       pressKey(remain);
  //       remain = "";
  //     } else {
  //       pressKey(remain.substring(0, latestSpace));
  //       remain = remain.substring(latestSpace + 1);
  //     }

  //     delay(5);
  //   }
  }
}

// Process the string command by converting the string set of keyboard inputs
void processStringCommand(File file) {
  // While there's is still bytes available in the file keep reading
  bool bytesAvailable = true;
  while (bytesAvailable) {
    String string = readFile(file, LINE_END_CHARS, MAX_STRING_LENGTH); // Read until me encounter an end character or we reach the byte limit

    // If the string is not empty, we still have things to process
    if (string != "") {
      Keyboard.print(string); // Send the string to the keyboard
    } else {
      bytesAvailable = false; // There are no bytes left
    }
  } 
}

void processDelayCommand(File file) {
  String delayString = readFile(file, LINE_END_CHARS); // Read until me encounter an end character

  int delayTime = delayString.toInt(); // Parse the delay time to an int
  delay(delayTime); // Wait
}

// void processLine(String line) {
//   int spaceIndex = line.indexOf(SPACE_CHAR);

//   // If a space exists this is a complex line (command + arguments, or set of keystrokes), otherwise it's a simple keystroke
//   if (spaceIndex > -1) {
//     String command = line.substring(0, spaceIndex);
//     String argument = line.substring(spaceIndex + 1);


//   } else {
//     pressKey(line);
//   }

//   Keyboard.releaseAll();
// }

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