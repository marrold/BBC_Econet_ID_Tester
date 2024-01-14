#include <Arduino.h>
#include <TM1637Display.h>

// Define the pin numbers for the jumpers
const int jumperPins[8] = {9, 8, 7, 6, 5, 4, 3, 2};

const uint8_t animationSequence[] = {
  SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F
};

// Define the CLK and DIO pins for the TM1637
#define CLK A0
#define DIO A1

// Create a TM1637Display object
TM1637Display display(CLK, DIO);

// Array to store the states of the jumpers
bool jumperStates[8];

// Function to read the jumper positions
void readJumpers() {
  for (int i = 0; i < 8; i++) {
    jumperStates[i] = digitalRead(jumperPins[i]) == HIGH;
  }
}

// Function to convert jumper states to station ID
int getStationId() {
    int stationId = 0;
    for (int i = 0; i < 8; i++) {
        // 'X' (fitted jumper) is '0', and '0' (unfitted jumper) is '1'
        if (jumperStates[i]) { // '0' is represented as HIGH (true) in jumperStates
            stationId |= (1 << i);  // Set the bit at position i
        }
    }
    return stationId;
}


void setup() {
  // Initialize the display
  display.setBrightness(0x0f); // Set brightness to maximum
  display.clear();

  uint8_t allOn[] = { 0x7F, 0x7F, 0x7F, 0x7F }; 
  display.setSegments(allOn);

  // Wait for 1 seconds
  delay(1000);

  // Clear the display
  display.clear();

  // Wait for another 1 seconds
  delay(1000);

  // Initialize jumper pins as input with pull-up
  for (int i = 0; i < 8; i++) {
    pinMode(jumperPins[i], INPUT_PULLUP);
  }
}

void loop() {
  static int animationIndex = 0;

  // Read the jumper configuration and get the station ID
  readJumpers();
  int stationId = getStationId();

  // Prepare the display data
  uint8_t displayData[] = { 0x00, 0x00, 0x00, 0x00 };

  // Set the first digit to the current animation step
  displayData[0] = animationSequence[animationIndex];

  // Update the animation index
  animationIndex = (animationIndex + 1) % 6;

  if (stationId == 255) {
    // Display "---" on the second, third, and fourth digits
    displayData[1] = SEG_G;
    displayData[2] = SEG_G;
    displayData[3] = SEG_G;
  } else {
    // Manually construct the station ID display data for the last three digits
    for (int i = 0; i < 3; i++) {
      int digit = (stationId / int(pow(10, 2 - i))) % 10; // Extract each digit
      displayData[i + 1] = display.encodeDigit(digit);
    }
  }

  // Update the entire display
  display.setSegments(displayData);

  // Delay to control the speed of animation
  delay(100); // Adjust this delay to speed up or slow down the animation
}

