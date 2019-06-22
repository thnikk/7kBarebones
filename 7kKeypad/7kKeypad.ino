/************ 7K+ Keypad ****************/
/* This is a stripped-down version of the
7K keypad code.

-thnikk */

// M0 specific
#include <Arduino.h>
#include <Bounce2.h>
#include <HID-Project.h>
#include <Adafruit_NeoPixel.h>

#define numkeys 7
#if defined rgbw
  Adafruit_NeoPixel pixels = Adafruit_NeoPixel(numkeys, 5, NEO_GRBW + NEO_KHZ800);
#else
  Adafruit_NeoPixel pixels = Adafruit_NeoPixel(numkeys, 5, NEO_GRB + NEO_KHZ800);
#endif
// Trinket button pins
#ifdef testing
  const byte pins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
#else
  const byte pins[] = { 2, 3, 7, 9, 10, 11, 12, 4 };
#endif
//char initMapping[] = {"sdfjkl "};
byte initMapping[] = {115,100,102,106,107,108,32,27};
// Universal
Bounce * bounce = new Bounce[numkeys+1];
bool version = 1;
char mapping[numkeys][3];
bool pressedLock[numkeys];
// Cycle LED Mode
unsigned long cycleMillis;
unsigned long cycleSpeed = 10;
byte cycleWheel;
// Universal LED
byte ledMode = 0;
byte b = 127;  // Brightness
byte rgb[numkeys][3];
byte dsrgb[3];
byte numModes = 6;

void setup() {
	Serial.begin(9600);

	// Set pullups and debounce
	for (byte x=0; x<=numkeys; x++) {
  	pinMode(pins[x], INPUT_PULLUP);
  	bounce[x].attach(pins[x]);
  	bounce[x].interval(8);
	}

	NKROKeyboard.begin();

}


void loop() {

  // Refresh bounce values
  for(byte x=0; x<=numkeys; x++) bounce[x].update();

  keyboard();
  cycle();

}

void wheel(byte shortColor, byte key) { // Set RGB color with byte
  if (shortColor >= 0 && shortColor < 85) { rgb[key][0] = (shortColor * -3) +255; rgb[key][1] = shortColor * 3; rgb[key][2] = 0; }
  else if (shortColor >= 85 && shortColor < 170) { rgb[key][0] = 0; rgb[key][1] = ((shortColor - 85) * -3) +255; rgb[key][2] = (shortColor - 85) * 3; }
  else { rgb[key][0] = (shortColor - 170) * 3; rgb[key][1] = 0; rgb[key][2] = ((shortColor - 170) * -3) +255; }
}
// LED Modes
void cycle() {
  if ((millis() - cycleMillis) > cycleSpeed) {
    cycleWheel++; // No rollover needed since datatype is byte
    for(int x = 0; x < numkeys; x++) {
      if (x != 6) wheel(cycleWheel+x*20, x);
      else wheel(cycleWheel+3.5*20, x);
      setLED(x);
    }
    cycleMillis = millis();
  }
}
void setLED(byte key) {
  pixels.setPixelColor(key, pixels.Color(b*rgb[key][0]/255, b*rgb[key][1]/255, b*rgb[key][2]/255));
  pixels.show();
}

void keyboard(){

  for (byte x=0; x<=numkeys; x++){
    if (!bounce[x].read() && pressedLock[x]) { NKROKeyboard.press(initMapping[x]); pressedLock[x] = 0; }
    if (bounce[x].read() && !pressedLock[x]){ NKROKeyboard.release(initMapping[x]); pressedLock[x] = 1; }
  }


}
