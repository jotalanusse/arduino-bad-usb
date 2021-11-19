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

// TODO: Migrate to NicoHood HID
// TODO: Support hyphens between keystroke combinations
// TODO: Add numberpad support
// TODO: Add default delay between keystrokes command
// TODO: Add PRINTSCREEN support
// TODO: Add initial delay configuration
// TODO: Make the file variable global

// #include <HID-Settings.h>
// #include <HID-Project.h>
#include <Keyboard.h>
#include <SD.h>
#include <SPI.h>
#include <string.h>

// Character groups
const char ALL_END_CHARS[] PROGMEM = {' ', '\n', '\r'};
const char LINE_END_CHARS[] PROGMEM = {'\n', '\r'};

// Configuration
const int PINS[] PROGMEM = {5, 6, 7, 8, 9};
const int MAX_STRING_READ_LENGTH PROGMEM = 1024;
const char FILE_EXTENSION[] PROGMEM = ".txt";

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

  // Start the SD card on pin 4
  if (!SD.begin(4)) {
    return; // If the SD card fails to start, we exit
  }

  File file = SD.open(fileName);
  if (file) {
    Keyboard.begin();

    processFile(file);
    file.close();

    Keyboard.end();
  }
}

// Read a file until and end character is found or a byte limit is reached
String readFile(File file, char endCharacters[], int byteLimit = 0) {
  String result = ""; // The result of our read operation

  // While there's is still bytes available in the file
  while (file.available()) {
    // This is rather confusing, but let me explain.
    // By using "read()" instead of "peek()" we still
    // don't know if the character is an end character
    // or not, but the reader header moves by one byte
    // anyway.
    //
    // So any end character is unintentionally avoided
    // from being returned not only from the current
    // read operation, but also from the next one.
    char character = file.read(); // Read the next character

    // If the character is not present in our end characters array
    for (int i = 0; i < sizeof(endCharacters); i += 1) {
      char endCharacter =
          endCharacters[i]; // Thee current end character we want to compare

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
  while (file.available()) {
    String command = readFile(file, ALL_END_CHARS);

    if (command != "") {
      processCommand(command, file);
    }
  }
}

void processCommand(String command, File file) {
  if (command == F("REM")) { // This is a comment, so we just ignore it
    // Hello there!
  } else if (command == F("STRING")) { // Send a string of text to the keyboard
    stringCommand(file);
  } else if (command == F("DELAY")) { // Delay the script for x milliseconds
    delayCommand(file);
  } else { // Single keystroke, or a set of keystrokes
    keystrokeCommand(command, file);
  }
}

// Convert the string into a set of keystrokes
void stringCommand(File file) {
  bool bytesAvailable = true;
  while (bytesAvailable) {
    String string = readFile(file, LINE_END_CHARS, MAX_STRING_READ_LENGTH);

    if (string != "") {
      Keyboard.print(string);
    } else {
      bytesAvailable = false;
    }
  }
}

// Delay the script for x milliseconds
void delayCommand(File file) {
  String delayString = readFile(file, ALL_END_CHARS);

  int delayTime = delayString.toInt();
  delay(delayTime);
}

// Convert the string into a combination of keystrokes
void keystrokeCommand(String command, File file) {
  String keystrokes = command + ' ' + readFile(file, LINE_END_CHARS);

  while (keystrokes != "") {
    int spaceIndex = keystrokes.indexOf(' ');

    if (spaceIndex == -1) {
      pressKey(keystrokes);
      keystrokes = "";
    } else {
      pressKey(keystrokes.substring(0, spaceIndex));
      keystrokes = keystrokes.substring(spaceIndex + 1);
    }

    delay(5);
  }

  Keyboard.releaseAll();
}

// TODO: Make this function more pretty
void pressKey(String key) {
  if (key.length() == 1) {
    char character = key.charAt(0);
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
