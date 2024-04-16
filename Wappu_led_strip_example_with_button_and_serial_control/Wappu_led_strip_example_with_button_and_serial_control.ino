#include <FastLED.h>

FASTLED_USING_NAMESPACE

// --------------------------------- LED CONFIG ---------------------------------
#define DATA_PIN    3      // Digital pin connected to the data line of the LED strip
#define LED_TYPE    WS2811 // Type of LED chip used in the LED strip
#define COLOR_ORDER GRB    // Color order of the LED strip (Green-Red-Blue)
#define NUM_LEDS    10     // Total number of LEDs in the LED strip

#define BRIGHTNESS         200     // Overall brightness level of the LED strip (0-255)
#define FRAMES_PER_SECOND  120    // Desired frame rate for the animation (frames per second)
// ------------------------------------------------------------------------------


// --------------------------------- EFFECT CONFIG ------------------------------
#define EFFECT_DURATION_S 10 // How long one effect lasts in seconds

// List all effect function prototypes
// Add all the effec function prototypes first here and then define them after main loop function.
void rainbow();
void rainbowWithGlitter();
void confetti();
void sinelon();
void bpm();
void juggle();
void staticRed();

typedef void (*SimplePatternList[])();

// List the used effect functions
// Add the effects you want in the order you want them to cycle trough here
SimplePatternList effect_functions = { staticRed, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
// ------------------------------------------------------------------------------


// --------------------------------- BUTTON CONFIG ------------------------------
#define BUTTON_PIN 2
// ------------------------------------------------------------------------------

// --------------------------------- SERIAL CONFIG ------------------------------
#define SERIAL_RATE 115200
#define SERIAL_INSTRUCTION_MESSAGE "WELCOME: send number of effect to set"
#define SERIAL_EFFECT_NUM_OUT_OF_BOUNDS_ERROR "ERROR: effect number is too large"
// ------------------------------------------------------------------------------

// --------------------------------- EFFECT GLOBALS -----------------------------
uint8_t current_effect = 0; // Index number of which pattern is current
uint8_t hue = 0; // rotating "base color" used by many of the effects
// ------------------------------------------------------------------------------

// --------------------------------- LED GLOBALS --------------------------------
CRGB leds[NUM_LEDS];
// ------------------------------------------------------------------------------


void setup() {
    // Arduino setup function that is run first when the program
    // put your setup code here, to run once:
    delay(3000); // 3 second delay for recovery

    // Set the button pin as input with internal pull-up resistor
    pinMode(BUTTON_PIN, INPUT_PULLUP); 
    // Attach interrupt to the button pin on falling edge
    // This means that the button is connected between BUTTON_PIN and ground
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING); 

    // Initialize serial communication
    Serial.begin(115200);
    // Send the instructions trough serial
    Serial.println(SERIAL_INSTRUCTION_MESSAGE);

    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);
}


void loop() {
    tryReadEffectNumberFromSerial();

    // Call the current pattern function once, updating the 'leds' array
    effect_functions[current_effect]();

    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND); 

    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { hue++; } // slowly cycle the "base color" through the rainbow
}



// --------------------------------- EFFECT FUNCTIONS ---------------------------
// --------------------------------- EFFECT HELPERS -----------------------------
/*This macro calculates the number of elements in an array at compile time.
 * It divides the total size of the array by the size of the first element
 */
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern() {
    // add one to the current pattern number, and wrap around at the end
    current_effect = (current_effect + 1) % ARRAY_SIZE( effect_functions);
}

void addGlitter( fract8 chanceOfGlitter) {
    if( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
}
// ------------------------------------------------------------------------------

void rainbow() {
    // FastLED's built-in rainbow generator
    fill_rainbow( leds, NUM_LEDS, hue, 7);
}

void rainbowWithGlitter() {
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}


void confetti() {
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV(hue + random8(64), 200, 255);
}

void sinelon() {
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy( leds, NUM_LEDS, 20);
    int pos = beatsin16( 13, 0, NUM_LEDS-1 );
    leds[pos] += CHSV(hue, 255, 192);
}

void bpm() {
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for( int i = 0; i < NUM_LEDS; i++) { //9948
      leds[i] = ColorFromPalette(palette,hue+(i*2), beat-hue+(i*10));
    }
}

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy( leds, NUM_LEDS, 20);
    uint8_t dothue = 0;
    for( int i = 0; i < 8; i++) {
      leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
      dothue += 32;
    }
}

void staticRed() {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
}

// ------------------------------------------------------------------------------

// --------------------------------- SERIAL FUNCTIONS ---------------------------
/**
* If there is serial data avaivable tries
*/
void tryReadEffectNumberFromSerial() {
    // Check for incoming serial data
    if (Serial.available() == 0) return;
    
    int incoming_byte = Serial.parseInt(); // Read the incoming byte as an integer
    if (incoming_byte < 0 || incoming_byte > ARRAY_SIZE(effect_functions)) {
      Serial.print(SERIAL_EFFECT_NUM_OUT_OF_BOUNDS_ERROR);
      Serial.print(" MAX is: ");
      Serial.print(String(ARRAY_SIZE(effect_functions)-1));
      Serial.println();
    }
  
    current_effect = incoming_byte; // Set the current effect based on the incoming byte   

    // Clear the serial buffer
    while (Serial.available() > 0) {
        Serial.read();
    }
}
// ------------------------------------------------------------------------------

// --------------------------------- BUTTON FUNCTIOS ----------------------------
void buttonInterrupt() {
  nextPattern();
}
// ------------------------------------------------------------------------------