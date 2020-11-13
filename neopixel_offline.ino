
// simple example code for POV poi on ESP8266, tested on D1 mini and ESP-01
// hardware: for testing, APA102 strip 36px long (144px/m) using pins defined below. Make sure to put a cap between +5v and GND, mine says 1000uf and works fine. 

  //as mentioned, i'll use a ws28-something, setting them as NEOPIXEL seems to work.

// Most of this code is the hard work of Tom Hastings www.bigtop.co.za
// Many thanks to Daniel Shiffman for open source code, as well as to Dylan and Brett Pillemer for inspiration..... 
// Thanks as well to all the FastLED and Arduino guys and gals of course. 
// example image is adapted from a Visual Poi program image, used for comparison with a commercial set of poi.


#include <FastLED.h>

#define BRIGHTNESS 220 // up to 255 but that's too bright. In pitch darkness use < 100 to save your eyes (and to save battery).
//#define NUM_LEDS 37 //always one more than the actual number of LED's in strip - for some reason I get a dud pixel at the end otherwise...
#define NUM_LEDS 16       //had this strange issue too, but let's see if we can fix this differently later in the code...

CRGB leds[NUM_LEDS];
//D1 mini:
//#define DATA_PIN D2
//#define CLOCK_PIN D1  // we only have 1 data line, i hope the rest of the code will work for these too.
                                //actually, we don't define any pins here, but directly when we add the leds. no idea why it didn't work this way.
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
    //tried to find a converter that spits out that array in the right format. i found png to unsigned, and since it's just one colour i thought i could 
    //just replace 248 with -8, but that didn't work, still all red. the startup pattern works though, so it's not the cables this time...since space seems 
    // less of an issue with only 16px, we probably might as well save the pattern directly and save the conversion?
uint8_t message1Data[] =
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,-8,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,-8,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,-8,-8,-8,0,0,0,0,0,0,0
,0,0,0,0,0,0,-8,-8,-8,0,0,0,0,0,0,0
,0,0,0,0,0,-8,-8,-8,-8,-8,0,0,0,0,0,0
,0,-8,-8,-8,-8,-8,0,-8,0,-8,-8,-8,-8,-8,0,0
,0,0,-8,-8,-8,-8,0,-8,0,-8,-8,-8,-8,0,0,0
,0,0,0,-8,-8,-8,0,-8,0,-8,-8,-8,0,0,0,0
,0,0,0,0,-8,-8,-8,-8,-8,-8,-8,0,0,0,0,0
,0,0,0,-8,-8,-8,-8,-8,-8,-8,-8,-8,0,0,0,0
,0,0,0,-8,-8,-8,-8,-8,-8,-8,-8,-8,0,0,0,0
,0,0,-8,-8,-8,-8,-8,0,-8,-8,-8,-8,-8,0,0,0
,0,0,-8,-8,-8,0,0,0,0,0,-8,-8,-8,0,0,0
,0,-8,-8,0,0,0,0,0,0,0,0,0,-8,-8,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct pattern message1 = {16, 16, message1Data}; //need this for multiple images later

byte X;

void setup() {
  //Serial.begin(115200);
  //Serial.println("Startup");
  FastLED.addLeds<NEOPIXEL, 6>(leds, NUM_LEDS); 
  FastLED.setBrightness(  BRIGHTNESS );
  startupPattern();
}




void loop()
{
  int counter = 0;
  for (int j = 0; j < 16; j++ ) { //number of rows
    for (int i = 0; i < 16; i++) { //number of LED's 
                                                          //played a bit around with these, still getting the one constant red led tom described
                                                          //will go for his solution tomorrow and see how that works.

      ///////////////////////////////convert byte to signed byte:////
      X = message1Data[counter++];
      ///////////////////////////////end convert byte to signed byte////

                                  //so, X is a 1 byte value


      // array compressed using R3G3B2 compression, uncompress here:
      byte R1 = (X & 0xE0);
                                  // so we do say 00001000 & 00011100000000 ? anyway, i'm tired now, more towmorrow!
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
  FastLED.delay(2);
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
