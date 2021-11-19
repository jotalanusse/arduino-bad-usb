/*
 * Author: jotalanusse
 *         https://github.com/jotalanusse
 *
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

// #include <HID-Settings.h>
// #include <HID-Project.h>
#include <Keyboard.h>
#include <SD.h>
#include <SPI.h>
#include <string.h>

// CHARACTER GROUPS:
const char ALL_END_CHARS[] PROGMEM = {'-', ' ', '\n', '\r'};
const char LINE_END_CHARS[] PROGMEM = {'\n', '\r'};

// CONFIGURATION:
// Pins
const int SD_PIN PROGMEM = 4;
const int PINS[] PROGMEM = {5, 6, 7, 8, 9};

// Files
const int MAX_READ_LENGTH PROGMEM = 1024;
const char FILE_EXTENSION[] PROGMEM = ".txt";

// Delays
const int INITIAL_DELAY PROGMEM = 800;
int DEFUALT_COMMAND_DELAY = 0;
const int KEYSTROKE_COMBOS_DELAY PROGMEM = 5;

// STRUCTS:
struct ReadResult {
  String result = ""; // TODO: CHeck if this even works
  char endChar = "";  // TODO: Chek if this can be an empty char
  bool byteLimitReached = false;
};

void setup() {
  int fileIndex = 0;

  // Set up each pin as an input
  for (int i = 0; i < sizeof(PINS); i += 1) {
    int pin = PINS[i];

    pinMode(pin, INPUT_PULLUP);
  }

  // If the pin is in a state of HIGH, we add to the index
  for (int i = 0; i < sizeof(PINS); i += 1) {
    int pin = PINS[i];
    int pinWeight = sizeof(PINS) - i - 1; // TODO: Find a cleaner way to do this

    if (digitalRead(pin) == HIGH) {
      fileIndex += pow(2, pinWeight); // Calculate the pin's weight
    }
  }

  // Parse the index to a file name
  String fileName = String(fileIndex) + FILE_EXTENSION;

  // Start the SD card on the specified pin
  if (!SD.begin(SD_PIN)) {
    return; // If the SD card fails to start, we exit
  }

  // OPen the SD file and get to work
  File file = SD.open(fileName);
  if (file) {
    Keyboard.begin();

    delay(INITIAL_DELAY); // Wait for the keyboard to initialize

    processFile(file);
    file.close();

    Keyboard.end();
  }
}

// Read a file until and end character is found or a byte limit is reached
ReadResult readFile(File file, char endCharacters[], int byteLimit = 0) {
  ReadResult readResult;

  while (file.available()) {
    char character = file.read();

    // If the character is present in our end characters array
    for (int i = 0; i < sizeof(endCharacters); i += 1) {
      char endChar = endCharacters[i];

      if (character == endChar) {
        readResult.endChar = endChar;

        return readResult;
      }
    }

    readResult.result += character;

    // If we've reached our byte limit for our read operation return the result
    if (readResult.result.length() == byteLimit) {
      readResult.byteLimitReached = true;

      return readResult;
    }
  }

  return readResult;
}

// Read a file and process the commands it contains
void processFile(File file) {
  while (file.available()) {
    ReadResult readResult = readFile(file, ALL_END_CHARS);
    String command = readResult.result;

    if (command != "") {
      delay(DEFUALT_COMMAND_DELAY); // Default delay between commands

      if (command == F("REM")) {
        /*
         * REM: The REM command is used to comment out a line of code,
         * so when we parse the script we just ignore it's content. This
         * can be helpfull to document the script, and make it easier to
         * read.
         */

        // TODO: Read comment until end of line
      } else if (command == F("STRING") || command == F("STR")) {
        /*
         * STRING: The STRING command is used to read a piece of text
         * from the script file, and send it as individual keystrokes to
         * the target computer. This is used to open links, programs,
         * execute commands, or even write secripts on the go.
         */

        stringCommand(file);
      } else if (command == F("DELAY")) {
        /*
         * DELAY: The DELAY command works by waiting for a certain
         * amount  of time (specified in milliseconds) before continuing
         * with the script. This can be used to slow down the script,
         * and thats' useful when waiting for GUI operations to complete.
         * Like menus, or prompts.
         */

        delayCommand(file);
      } else if (command == F("DEFAULT_DELAY") || command == F("DEFAULTDELAY")) {
        /*
         * DEFAULT_DELAY: The DEFAULT_DELAY command specifies the default
         * amount of time (in milliseconds) to wait before performing
         * each subsequent command. A lower value will result in a faster
         * execution, this command is mostly useful for debugging.
         */

        defaultDelayCommand(file);
      } else {
        /*
         * KEYSTROKES: Finally we have the keystrokes command, which is
         * not actually a command, but a keystroke itself (SHIFT, CTRL, f).
         * We use this to navigate through the GUI to get where we want to,
         * almost every keystroke can be simulated, which gives us a lot of
         * flexibility.
         */

        // Rewind the file to the start of the command because in this case
        // the argument is the command itself
        file.seek(file.position() - command.length());

        keystrokeCommand(file);
      }
    }
  }
}

// Convert the string into a set of keystrokes
void stringCommand(File file) {
  bool bytesAvailable = true;
  while (bytesAvailable) {
    ReadResult readResult = readFile(file, LINE_END_CHARS, MAX_READ_LENGTH);

    String string = readResult.result;
    char endChar = readResult.endChar;

    Keyboard.print(string);

    // If the read operation ended with an end character we stop
    if (endChar != "") { // TODO: Check if this can be an empty char
      bytesAvailable = false;
    }
  }
}

// Delay the script for x milliseconds
void delayCommand(File file) {
  ReadResult readResult = readFile(file, ALL_END_CHARS);

  String delayString = readResult.result;
  int delayTime = delayString.toInt();

  delay(delayTime);
}

// Delay the script for x milliseconds before each command
void defaultDelayCommand(File file) {
  ReadResult readResult = readFile(file, ALL_END_CHARS);

  String delayString = readResult.result;
  int delayTime = delayString.toInt();

  DEFUALT_COMMAND_DELAY = delayTime;
}

// Convert the string into a combination of keystrokes
void keystrokeCommand(File file) {
  bool keystrokesAvailable = true;
  while (keystrokesAvailable) {
    ReadResult readResult = readFile(file, ALL_END_CHARS);

    String key = readResult.result;
    char endChar = readResult.endChar;

    pressKey(key);

    // TODO: Use a for loop to iterate through the line end characters?
    if (endChar == '\n' || endChar == '\r') {
      keystrokesAvailable = false;
    }

    delay(KEYSTROKE_COMBOS_DELAY);
  }

  Keyboard.releaseAll();
}

// TODO: Make this function more pretty
void pressKey(String key) {
  if (key.length() == 1) {
    char character = key.charAt(0); // TODO: Use [] instead?
    Keyboard.press(character);
  } else {
    if (key == F("ENTER"))
      Keyboard.press(KEY_RETURN);
    if (key == F("CTRL") || key == F("CONTROL"))
      Keyboard.press(KEY_LEFT_CTRL);
    if (key == F("SHIFT"))
      Keyboard.press(KEY_LEFT_SHIFT);
    if (key == F("ALT"))
      Keyboard.press(KEY_LEFT_ALT);
    if (key == F("GUI") || key == F("WINDOWS") || F("WIN"))
      Keyboard.press(KEY_LEFT_GUI);
    if (key == F("UP") || key == F("UPARROW"))
      Keyboard.press(KEY_UP_ARROW);
    if (key == F("DOWN") || key == F("DOWNARROW"))
      Keyboard.press(KEY_DOWN_ARROW);
    if (key == F("LEFT") || key == F("LEFTARROW"))
      Keyboard.press(KEY_LEFT_ARROW);
    if (key == F("RIGHT") || key == F("RIGHTARROW"))
      Keyboard.press(KEY_RIGHT_ARROW);
    if (key == F("DELETE"))
      Keyboard.press(KEY_DELETE);
    if (key == F("PAGEUP"))
      Keyboard.press(KEY_PAGE_UP);
    if (key == F("PAGEDOWN"))
      Keyboard.press(KEY_PAGE_DOWN);
    if (key == F("HOME"))
      Keyboard.press(KEY_HOME);
    if (key == F("ESC"))
      Keyboard.press(KEY_ESC);
    if (key == F("INSERT"))
      Keyboard.press(KEY_INSERT);
    if (key == F("TAB"))
      Keyboard.press(KEY_TAB);
    if (key == F("END"))
      Keyboard.press(KEY_END);
    if (key == F("CAPSLOCK"))
      Keyboard.press(KEY_CAPS_LOCK);
    if (key == F("F1"))
      Keyboard.press(KEY_F1);
    if (key == F("F2"))
      Keyboard.press(KEY_F2);
    if (key == F("F3"))
      Keyboard.press(KEY_F3);
    if (key == F("F4"))
      Keyboard.press(KEY_F4);
    if (key == F("F5"))
      Keyboard.press(KEY_F5);
    if (key == F("F6"))
      Keyboard.press(KEY_F6);
    if (key == F("F7"))
      Keyboard.press(KEY_F7);
    if (key == F("F8"))
      Keyboard.press(KEY_F8);
    if (key == F("F9"))
      Keyboard.press(KEY_F9);
    if (key == F("F10"))
      Keyboard.press(KEY_F10);
    if (key == F("F11"))
      Keyboard.press(KEY_F11);
    if (key == F("F12"))
      Keyboard.press(KEY_F12);
  }
}

void loop() {
  // Nothing to do here
}
