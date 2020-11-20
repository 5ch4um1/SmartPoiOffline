
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
#define NUM_LEDS 32       //had this strange issue too, but let's see if we can fix this differently later in the code...

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
            //well, maybe later, for now i tried with rgb332, created a (mirrored) image of 32x16 px and converted it with this online converter: https://www.digole.com/tools/PicturetoC_Hex_converter.php
uint8_t message1Data[] =
{ 0,0,0,0,0,0,4,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,4,0,0,0,0,0,0
,0,0,0,0,0,0,8,20,20,16,0,0,0,0,0,0,0,0,0,0,0,0,16,20,20,8,0,0,0,0,0,0
,0,0,0,12,20,20,20,20,20,20,20,8,0,0,0,0,0,0,0,0,8,20,20,20,20,20,20,20,12,0,0,0
,0,0,0,12,20,20,20,20,121,121,153,40,0,0,0,0,0,0,0,0,40,153,121,121,20,20,20,20,12,0,0,0
,0,0,0,12,20,20,20,20,218,255,255,109,0,0,0,0,0,0,0,0,109,255,255,218,20,20,20,20,12,0,0,0
,214,214,0,0,0,0,109,255,255,218,0,0,0,0,73,255,255,73,0,0,0,0,218,255,255,109,0,0,0,0,214,214
,218,219,73,73,73,73,146,215,215,183,73,73,73,73,146,255,255,146,73,73,73,73,183,215,215,146,73,73,73,73,219,218
,219,255,255,255,255,255,183,79,79,115,255,255,255,255,255,255,255,255,255,255,255,255,115,79,79,183,255,255,255,255,255,219
,32,36,36,36,36,36,73,79,79,75,36,36,36,36,36,36,36,36,36,36,36,36,75,79,79,73,36,36,36,36,36,32
,0,0,0,0,0,0,37,79,79,75,0,0,0,0,0,0,0,0,0,0,0,0,75,79,79,37,0,0,0,0,0,0
,0,0,0,0,0,0,37,79,79,75,0,0,0,0,0,0,0,0,0,0,0,0,75,79,79,37,0,0,0,0,0,0
,0,0,0,0,0,74,75,79,79,75,74,37,0,0,0,0,0,0,0,0,37,74,75,79,79,75,74,0,0,0,0,0
,0,0,0,0,0,79,79,75,75,75,79,37,0,0,0,0,0,0,0,0,37,79,75,75,75,79,79,0,0,0,0,0
,0,0,0,0,0,79,38,0,0,0,111,37,0,0,0,0,0,0,0,0,37,111,0,0,0,38,79,0,0,0,0,0
,0,0,0,105,146,183,110,0,0,36,183,182,146,0,0,0,0,0,0,146,182,183,36,0,0,110,183,146,105,0,0,0
,0,0,0,146,255,255,146,0,0,36,255,255,255,0,0,0,0,0,0,255,255,255,36,0,0,146,255,255,146,0,0,0};
struct pattern message1 = {32, 16, message1Data}; //need this for multiple images later

byte X;

void setup() {
  //Serial.begin(115200);
  //Serial.println("Startup");
  FastLED.addLeds<NEOPIXEL, 4>(leds, NUM_LEDS); 
  FastLED.setBrightness(  BRIGHTNESS );
  startupPattern();
}




void loop()
{
  int counter = 0;
  for (int j = 0; j < 16; j++ ) { //number of rows
    for (int i = 0; i < 32; i++) { //number of LED's 
                                                          //played a bit around with these, still getting the one constant red led tom described
                                                          //will go for his solution tomorrow and see how that works.
                                                                // at the end i cut a strip of 33 leds, i wanted to sacrifice 1 to get better
                                                                // points for soldering, but then i decided to solder to the first led anyway,
                                                                // so this is kind of a "works on my box" thing...

      ///////////////////////////////convert byte to signed byte:////
      X = message1Data[counter++];
      ///////////////////////////////end convert byte to signed byte////

                                  //so, X is one line of that array i suppose... and it gets read byte by byte, in form of uint values.


      // array compressed using R3G3B2 compression, uncompress here:
      byte R1 = (X & 0xE0);
                                  // here it's suddenly hex? ok, i guess "byte" does that. so, we do an and operation of our X value with 0xE0?
      leds[i].r = R1; //
      byte G1 =  ((X << 3) & 0xE0);
                                  //here we shift X to the left by 3 bits and then do the and.
      leds[i].g = G1;
                                          
      byte M1 = (X << 6);             
                                  // and here just shifting to the left by 6 bits? 
      leds[i].b = M1;
      //FastLED.delay(42);
                                  //
      //Serial.print(R1); Serial.print(", "); Serial.print(G1); Serial.print(", "); Serial.println(M1);
    }
    FastLED.show();
    FastLED.clear();    
                                  // added the FastLED.clear here to turn off the leds
  }
  FastLED.delay(42);
                                  // adjust this delay for spacing, how long to pause between each time the array is shown.
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
