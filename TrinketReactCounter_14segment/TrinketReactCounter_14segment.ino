// Adafruit Trinket React Counter Sketch - 14-segment quad alpha display
//
// Use a 14-segment quad alphanumeric LED backpack to display the 
// number of times a button has been pressed.  Great for building
// a physical  'like' or react button.  The value will be stored 
// in EEPROM so it will persist between power down/up.
//
// NOTE: As-is this sketch needs to run on a Trinket because it
// assumes the switch on pin #1 has a pull-down resistor to ground.
// If using another board without this pull-down you can explicitly
// add a ~10kohm resistor from digital #1 to ground.
//
// Author: Tony DiCola
// License: MIT (https://opensource.org/licenses/MIT)
#include <EEPROM.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"


// Uncomment the line below to reset the counter value in EEPROM to zero.
// After uncommenting reload the sketch and during the setup the counter
// will be reset.  Then comment the line again and reload to start again
// (if you don't comment it out then every time the board powers on it
// will reset back to zero!).
//#define RESET_COUNT

// Configuration:
#define LED_BACKPACK_ADDRESS   0x70   // I2C address of the backpack display.
                                      // Keep the default 0x70 unless you
                                      // change the backpack's address bridges.

#define COUNT_BUTTON_PIN       1      // Digital input connected to the button that
                                      // will increase the count. This line should
                                      // have a pull-down resistor to ground.  The
                                      // opposite side of the button should be
                                      // connected to a high level like 5V or 3.3V.

#define COUNT_ADDRESS          0      // Address in EEPROM to store the counter.
                                      // This will take 2 bytes (16-bit value).
                                      // You don't need to change this unless you
                                      // want to play with different EEPROM locations.

// 14-segment quad alphanumeric display
Adafruit_AlphaNum4 backpack = Adafruit_AlphaNum4();

void update_display() {
  // Get the count value from EEPROM and print it to the display.
  uint16_t count;
  EEPROM.get(COUNT_ADDRESS, count);
  // Use writeDigitRaw function to write out each digit for the thousands,
  // hundreds, tens, and ones place.
  int thousands = count / 1000;
  int remainder = count % 1000;
  int hundreds = remainder / 100;
  remainder = remainder % 100;
  int tens = remainder / 10;
  remainder = remainder % 10;
  // Check if the value is too large to display and just print dashes.
  if (thousands >= 10) {
    backpack.writeDigitAscii(0, '-');
    backpack.writeDigitAscii(1, '-');
    backpack.writeDigitAscii(2, '-');
    backpack.writeDigitAscii(3, '-');
  }
  else
  {
    // Print out the number starting from the first non-zero digit.
    if (thousands > 0) {
      backpack.writeDigitAscii(0, '0'+thousands);
      backpack.writeDigitAscii(1, '0'+hundreds);
      backpack.writeDigitAscii(2, '0'+tens);
      backpack.writeDigitAscii(3, '0'+remainder);
    }
    else if (hundreds > 0) {
      backpack.writeDigitAscii(1, '0'+hundreds);
      backpack.writeDigitAscii(2, '0'+tens);
      backpack.writeDigitAscii(3, '0'+remainder);
    }
    else if (tens > 0) {
      backpack.writeDigitAscii(2, '0'+tens);
      backpack.writeDigitAscii(3, '0'+remainder);
    }
    else {
      backpack.writeDigitAscii(3, '0'+remainder);
    }
  }
  backpack.writeDisplay();
}

void setup() {
  // Setup button inputs.
  pinMode(COUNT_BUTTON_PIN, INPUT);
  
  // Initialize the LED backpack display.
  backpack.begin(LED_BACKPACK_ADDRESS);

  // Clear the count in EEPROM if desired.
  #ifdef RESET_COUNT
    uint16_t count = 0;
    EEPROM.put(COUNT_ADDRESS, count);
  #endif

  // Update the display with the current count value.
  update_display();
}

void loop() {
  // Take a couple button readings with a small delay in between to detect when
  // the signal changes from high to low, i.e. the button was released.
  int firstCount = digitalRead(COUNT_BUTTON_PIN);
  delay(20);
  int secondCount = digitalRead(COUNT_BUTTON_PIN);

  // Check for count button release.
  if (firstCount == HIGH && secondCount == LOW) {
    // Button was released!
    // Increment the count value stored in EEPROM.
    uint16_t count;
    EEPROM.get(COUNT_ADDRESS, count);
    count += 1;
    EEPROM.put(COUNT_ADDRESS, count);
    // Update the display with the latest count value.
    update_display();
  }
}
