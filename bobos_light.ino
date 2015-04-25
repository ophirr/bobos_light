#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define BUTTON_PIN  0    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.

#define PIN 1

int brightPin = 1;
int brightVal = 0;
int brightness = 0;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);

bool oldState = HIGH;
int oldBright = 0;
int showType = 0;

// timer 
unsigned long currentMillis;
unsigned long previousMillis; // will store last time pixel was updated
unsigned long main_currentMillis; // store last main loop time
unsigned long bedtimer_dim = 3600000; // dim down after 1 hour of use
int seconds; //seconds count in the timer

int bedtime_brightness = 40; // go down to 1...<40>...........255 brightness

int neoPixelToChange = 0; //track which neoPixel to change
int neoPixel_j = 0; //stores values for program cycles
 

uint32_t LIME = 0x00FF00;
uint32_t CORNFLOWERBLUE = 0x6495ED;
uint32_t REBECCAPURPLE = 0x663399;
uint32_t ORCHID = 0xDA70D6;
uint32_t GOLD = 0xFFD700;
uint32_t YELLOW = 0xFFFF00;
uint32_t WHITE = 0xFFFFFF;
uint32_t DODGERBLUE = 0x1E90FF;
uint32_t AQUA = 0x00FFFF;
uint32_t LAWNGREEN = 0x7CFC00;
uint32_t CHOCOLATE = 0xD2691E;
uint32_t GREENYELLOW = 0xADFF2F;

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code
  
  // initialize Timer1
    cli();          // disable global interrupts
    TCCR1A = 0;     // set entire TCCR1A register to 0
    TCCR1B = 0;     // same for TCCR1B
 
    // set compare match register to desired timer count:
    OCR1A = 15624;
    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
    // enable global interrupts:
    sei();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  //colorWipe(strip.Color(127,0,127), 50);
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
 
}

void loop() {
 
  // Get current button state.
  bool newState = digitalRead(BUTTON_PIN);
  
  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
      // reset neoPixelToChange AND neoPixel_j each time button is pressed
      neoPixelToChange = 0;
      neoPixel_j = 0;
      showType++;
      if (showType > 9  )
        showType=0;
      startShow(showType);
    }
  } else {
    startShow(showType);
  }

  // Set the last button state to the old state.
  oldState = newState;

 
 brightVal  = analogRead(brightPin); 
 if (oldBright != brightVal) { 
   brightness = map(brightVal, 0, 1023, 0, 255);
   strip.setBrightness(brightness);
   oldBright = brightVal;
 }
 
}

void startShow(int i) {
  switch(i){
    case 0: allColor(CORNFLOWERBLUE);  
            break;
    case 1: rainbowCycle(20); 
            break;
    case 2: allColor(ORCHID);
            break;
    case 3: allColor(AQUA);
            break;
    case 4: allColor(GOLD);
            break;
    case 5: allColor(LAWNGREEN);
            break;
    case 6: rainbow(20);
            break;
    case 7: allColor(REBECCAPURPLE);
            break;
    case 8: allColor(CHOCOLATE);
            break;
    case 9: allColor(GREENYELLOW);
            break;
  }
}


// Fill all the dots with one color
void allColor(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
  }
} // note for a random color, use:
  // allColor((unsigned long)random(0x01000000)); // random color
  
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  
  unsigned long currentMillis = millis();
  
  //only do this if some of the pixels still need to be lit
  if (neoPixelToChange <= strip.numPixels()){
    
    if(currentMillis - previousMillis > wait * neoPixelToChange) { //appears to be an exponential growth delay but works
      
      // save the last time you changed a NeoPixel 
      previousMillis = currentMillis;  
    
      //change a pixel
      strip.setPixelColor(neoPixelToChange, c);
      strip.show();
      neoPixelToChange++;
    }
  }
}

void rainbow(uint8_t wait) {
    
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > wait) {
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+neoPixel_j) & 255));
    }
    strip.show();
    neoPixel_j = (neoPixel_j + 1) % 255; //increment j until all colors are used, then start over
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > wait) {
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + neoPixel_j) & 255));
    }
    strip.show();
    neoPixel_j = (neoPixel_j + 1) % 1279; // 5 cycles of all colors on wheel, then start over
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

ISR(TIMER1_COMPA_vect)
{
    // We're hit the timer overflow... do something.
    seconds++;
    if (seconds == 3660)
    {
        seconds = 0;
        strip.setBrightness(bedtime_brightness); // an hour has passed...dim down.
    }
}

