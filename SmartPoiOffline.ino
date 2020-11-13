
// simple example code for POV poi on ESP8266, tested on D1 mini and ESP-01
// hardware: for testing, APA102 strip 36px long (144px/m) using pins defined below. Make sure to put a cap between +5v and GND, mine says 1000uf and works fine. 

  //as mentioned, i'll use a ws28-something, setting them as NEOPIXEL seems to work.

// Most of this code is the hard work of Tom Hastings www.bigtop.co.za
// Many thanks to Daniel Shiffman for open source code, as well as to Dylan and Brett Pillemer for inspiration..... 
// Thanks as well to all the FastLED and Arduino guys and gals of course. 
// example image is adapted from a Visual Poi program image, used for comparison with a commercial set of poi.


#include "FastLED.h"

#define BRIGHTNESS 220 // up to 255 but that's too bright. In pitch darkness use < 100 to save your eyes (and to save battery).
//#define NUM_LEDS 37 //always one more than the actual number of LED's in strip - for some reason I get a dud pixel at the end otherwise...
#define NUM_LEDS 32       //had this strange issue too, but let's see if we can fix this differently later in the code...

CRGB leds[NUM_LEDS];
//D1 mini:
#define DATA_PIN D2
//#define CLOCK_PIN D1  // we only have 1 data line, i hope the rest of the code will work for these too.
//ESP-01:
//#define DATA_PIN 2
//#define CLOCK_PIN 0

struct dataRGB {
  byte r;
  byte g;
  byte b;
};

struct pattern {
  byte numLeds;
  byte numFrames;
  uint8_t *patData;
};


//the following Array is compressed using R3G3B2 method: each comma separated value comprises RGB values for one pixel, compressed into one signed byte.
//uncompressed uses 3x as much space, I have not seen any real difference in colour quality using this scheme.
    //being lazy as usual, i just took the original array and did a cut -f1-32...
uint8_t message1Data[] =
{ 63,	63,	31,	31,	22,	9,	0,	0,	-112,	-3,	-4,	-4,	-40,	68,	64,	-92,	-64,	-32,	-64,	-60,	-92,	96,	100,	-8,	-3,	-4,	-3,	-111,	0,	0,	5,	22,
  18,	63,	63,	63,	63,	18,	4,	0,	0,	-112,	-3,	-3,	-7,	-43,	100,	-128,	-60,	-64,	-96,	-92,	-128,	100,	-11,	-3,	-3,	-3,	112,	0,	0,	4,	18,	95,
  4,	18,	59,	63,	63,	63,	18,	4,	0,	32,	-112,	-3,	-3,	-3,	-44,	96,	-128,	-60,	-96,	96,	96,	-44,	-3,	-3,	-3,	112,	4,	0,	4,	17,	63,	63,
  0,	8,	18,	59,	31,	63,	63,	18,	4,	0,	36,	-112,	-3,	-3,	-7,	-44,	96,	96,	96,	100,	-44,	-3,	-3,	-35,	112,	0,	0,	4,	49,	95,	63,	63,
  0,	0,	5,	18,	63,	63,	63,	63,	14,	4,	0,	4,	-112,	-3,	-3,	-3,	-48,	68,	100,	-44,	-3,	-4,	-3,	-112,	0,	0,	4,	17,	63,	63,	63,	63,
  1,	0,	0,	5,	18,	59,	63,	31,	95,	18,	4,	0,	0,	-112,	-3,	-3,	-3,	-40,	-76,	-3,	-3,	-3,	112,	0,	0,	4,	18,	95,	63,	27,	63,	18,
  2,	2,	1,	0,	0,	50,	95,	63,	31,	63,	18,	4,	0,	0,	-112,	-3,	-4,	-4,	-4,	-4,	-3,	-112,	0,	0,	4,	50,	63,	31,	63,	63,	50,	5,
  3,	2,	1,	1,	0,	0,	18,	95,	31,	31,	63,	18,	0,	0,	0,	-80,	-3,	-4,	-4,	-3,	-112,	0,	0,	4,	50,	95,	63,	63,	59,	18,	5,	1,
  3,	3,	2,	2,	1,	0,	4,	50,	95,	31,	63,	63,	18,	4,	0,	0,	-80,	-3,	-3,	-112,	0,	0,	4,	50,	95,	63,	63,	59,	50,	5,	0,	1,
  3,	3,	3,	2,	2,	1,	0,	4,	18,	95,	63,	31,	63,	18,	4,	0,	36,	-112,	108,	4,	0,	4,	18,	95,	31,	63,	91,	50,	4,	0,	1,	1,
  3,	3,	3,	3,	2,	2,	1,	0,	4,	50,	59,	31,	63,	63,	18,	4,	0,	0,	0,	0,	4,	18,	63,	31,	63,	59,	50,	4,	0,	1,	2,	2,
  2,	2,	3,	3,	3,	2,	2,	1,	0,	4,	18,	63,	31,	63,	63,	18,	4,	0,	0,	4,	54,	63,	31,	31,	63,	18,	4,	0,	1,	2,	2,	3,
  34,	2,	2,	3,	3,	3,	2,	2,	1,	0,	5,	50,	59,	63,	63,	63,	17,	8,	9,	18,	63,	63,	31,	63,	18,	5,	0,	1,	2,	2,	3,	3,
  -121,	66,	34,	2,	3,	3,	3,	2,	2,	1,	0,	5,	18,	59,	63,	63,	95,	18,	22,	95,	63,	63,	63,	18,	4,	0,	1,	2,	2,	3,	3,	3,
  -57,	-93,	66,	34,	2,	3,	3,	3,	2,	2,	1,	0,	4,	50,	91,	63,	63,	63,	31,	31,	63,	95,	50,	4,	0,	1,	2,	3,	3,	3,	3,	2,
  -29,	-61,	-89,	66,	2,	2,	3,	3,	3,	2,	1,	0,	0,	4,	18,	63,	31,	31,	31,	31,	63,	18,	0,	0,	0,	1,	2,	3,	3,	3,	3,	2,
  -29,	-29,	-89,	98,	34,	2,	3,	3,	3,	3,	1,	1,	0,	0,	9,	59,	31,	31,	31,	31,	59,	9,	0,	0,	1,	2,	3,	3,	3,	3,	2,	2,
  -29,	-61,	-93,	66,	2,	3,	3,	3,	3,	2,	1,	0,	0,	4,	18,	63,	31,	31,	31,	31,	95,	18,	0,	0,	1,	2,	2,	3,	3,	3,	3,	2,
  -57,	-93,	98,	34,	2,	3,	3,	3,	2,	2,	1,	0,	4,	18,	63,	63,	63,	63,	63,	63,	27,	95,	18,	4,	0,	0,	1,	2,	3,	3,	3,	3,
  -89,	98,	34,	2,	3,	3,	3,	2,	2,	1,	0,	5,	50,	91,	63,	63,	59,	18,	22,	59,	63,	63,	95,	18,	4,	0,	0,	1,	3,	3,	3,	3,
  34,	34,	3,	3,	3,	3,	2,	2,	1,	0,	5,	18,	63,	63,	59,	95,	17,	4,	5,	18,	95,	59,	63,	63,	18,	9,	0,	1,	2,	2,	3,	3,
  2,	3,	3,	3,	3,	2,	1,	1,	0,	5,	18,	63,	63,	63,	59,	50,	4,	0,	0,	4,	13,	95,	63,	63,	63,	18,	5,	0,	1,	2,	2,	3,
  3,	3,	3,	3,	2,	1,	1,	0,	5,	18,	63,	63,	63,	59,	18,	0,	0,	0,	4,	0,	0,	50,	59,	63,	63,	27,	18,	5,	0,	1,	2,	2,
  3,	3,	3,	2,	1,	1,	0,	5,	50,	59,	63,	31,	63,	18,	4,	0,	36,	-112,	-112,	0,	0,	4,	50,	63,	31,	63,	63,	18,	4,	0,	1,	2,
  3,	3,	2,	1,	0,	0,	5,	50,	63,	63,	63,	95,	17,	4,	0,	4,	-112,	-3,	-3,	-112,	0,	0,	5,	18,	63,	31,	63,	63,	50,	4,	0,	1,
  3,	2,	1,	1,	0,	4,	18,	59,	63,	27,	95,	18,	4,	0,	4,	-75,	-3,	-3,	-3,	-3,	-108,	0,	0,	4,	18,	59,	63,	31,	59,	18,	5,	0,
  2,	2,	1,	0,	5,	18,	59,	63,	59,	95,	13,	4,	0,	0,	-112,	-3,	-4,	-4,	-4,	-3,	-3,	-108,	0,	0,	5,	18,	63,	63,	63,	59,	50,	5,
  1,	1,	0,	4,	18,	59,	63,	63,	91,	18,	4,	0,	36,	-112,	-3,	-3,	-3,	-44,	-40,	-4,	-4,	-3,	-112,	0,	0,	4,	18,	59,	63,	63,	59,	50,
  0,	0,	5,	50,	59,	63,	63,	91,	49,	0,	0,	36,	-112,	-3,	-3,	-3,	-48,	64,	100,	-80,	-4,	-4,	-3,	-112,	0,	0,	4,	17,	59,	63,	63,	91,
  0,	4,	50,	91,	63,	63,	95,	18,	0,	0,	4,	-112,	-3,	-3,	-7,	-48,	96,	96,	64,	100,	-48,	-4,	-4,	-3,	-112,	36,	0,	4,	18,	59,	63,	63,
  9,	18,	63,	63,	63,	91,	50,	4,	0,	4,	-112,	-3,	-4,	-3,	-48,	96,	-128,	-96,	-92,	96,	96,	-43,	-3,	-3,	-4,	-112,	36,	0,	4,	18,	63,	31,
  22,	63,	63,	31,	63,	18,	4,	0,	0,	-112,	-3,	-4,	-4,	-44,	100,	-128,	-64,	-64,	-64,	-96,	-128,	96,	-44,	-3,	-4,	-3,	-112,	0,	0,	5,	18,	63,
  63,	31,	31,	63,	18,	4,	0,	0,	-112,	-3,	-4,	-4,	-44,	68,	96,	-92,	-64,	-32,	-32,	-64,	-96,	96,	68,	-40,	-4,	-4,	-3,	-111,	0,	0,	4,	22,
};
struct pattern message1 = {32, 33, message1Data}; //need this for multiple images later

byte X;

void setup() {
  //Serial.begin(115200);
  //Serial.println("Startup");
  FastLED.addLeds<NEOPIXEL, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS); 
  FastLED.setBrightness(  BRIGHTNESS );
  startupPattern();
}




void loop()
{
  int counter = 0;
  for (int j = 0; j < 33; j++ ) { //number of rows
    for (int i = 0; i < 32; i++) { //number of LED's

      ///////////////////////////////convert byte to signed byte:////
      X = message1Data[counter++];
      ///////////////////////////////end convert byte to signed byte////

      // array compressed using R3G3B2 compression, uncompress here:
      byte R1 = (X & 0xE0);
      leds[i].r = R1; //
      byte G1 =  ((X << 3) & 0xE0);
      leds[i].g = G1;
      byte M1 = (X << 6);
      leds[i].b = M1;
      //FastLED.delay(1);
      //Serial.print(R1); Serial.print(", "); Serial.print(G1); Serial.print(", "); Serial.println(M1);
    }
    FastLED.show();
  }
  //FastLED.delay(2);
}

void startupPattern() {
  delay(100);
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.showColor(CRGB::Black);
  delay(100);
  FastLED.showColor(CRGB::Green);
  delay(1000);
  FastLED.showColor(CRGB::Black);
  delay(100);
  FastLED.showColor(CRGB::Blue);
  delay(1000);
  FastLED.showColor(CRGB::Black);
  delay(100);
  for (int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to whatever
    leds[i] = CRGB::Cyan;
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Black;
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to whatever
    leds[i] = CRGB::Yellow;
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Black;
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to whatever
    leds[i] = CRGB::Magenta;
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Magenta;
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
  delay(200);
  FastLED.showColor( CRGB::Black );
  delay(100);
}



