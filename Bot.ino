#include <SPI.h>                                         
#include <nRF24L01.h>                                    
#include <RF24.h>
#include <Servo.h>
#include <FastLED.h>

#define LED_PIN    2
#define NUM_LEDS    8
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 1000

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


#define SPEED_1      5 
#define DIR_1        4
 
#define SPEED_2      6
#define DIR_2        7

RF24 radio(8, 9); // nRF24L01+ (CE, CSN)
int data[7]; 

Servo rotServo;
int servoAngle;

void setup(){
  delay( 3000 );
  Serial.begin(9600);
    
  radio.begin();                                        
  radio.setChannel(10);
  radio.setDataRate (RF24_1MBPS);
  radio.setPALevel (RF24_PA_HIGH);
  radio.openReadingPipe (1, 0x1234567890LL);
  radio.startListening ();                          
  // motor pins
  for (int i = 4; i < 8; i++) {     
    pinMode(i, OUTPUT);
  }

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );


  rotServo.attach(10);
}

void loop(){


    if(radio.available()){                                
        radio.read(&data, sizeof(data));

        
        int tmblrState = 1;//data[6];
        int LGBTnState = data[4];
        int RightBTNState = data[5];
        int ptmrVal = data[6];
        servoAngle = map(ptmrVal, 0, 1023, 0, 180); 
        rotServo.write(servoAngle); 
        //Serial.println(ptmrVal);

          if (LGBTnState == 1) {
            currentPalette = RainbowStripeColors_p;   
            currentBlending = LINEARBLEND;
          }
          else 
            if (RightBTNState == 1) {
              currentPalette = myRedWhiteBluePalette_p; 
              currentBlending = NOBLEND;
            }
            else {
              fill_solid( currentPalette, 16, CRGB::Black);
            }
          
          static uint8_t startIndex = 0;
          startIndex = startIndex + 6;  //motion speed 
    
          FillLEDsFromPaletteColors( startIndex);
    
          FastLED.show();
          FastLED.delay(1000 / UPDATES_PER_SECOND);
        int tmp = 3;
   
        // 0 - Forward, 1 - backward, 2 - right, 3 - left
        switch (data[0]) {
          case 0: {
            digitalWrite(DIR_1, HIGH); // set direction
            analogWrite(SPEED_1, data[2]); // set speed
            tmp = 0;
            break;
          }
          case 1: {
            digitalWrite(DIR_1, LOW);
            analogWrite(SPEED_1, data[2]); 
            tmp = 1;
            break;
          }
          case 2: {
            analogWrite(SPEED_1, 0);

            break;
          }
        }
        switch (data[1]) {
          case 0: {
            digitalWrite(DIR_2, HIGH); 
            analogWrite(SPEED_2, data[3]); 

            break;
          }
          case 1: {
            digitalWrite(DIR_2, LOW);
            analogWrite(SPEED_2, data[3]); 

            break;
          }
          case 2: {
            analogWrite(SPEED_2, 0);

            break;
          }
        }
        
        Serial.println();
        Serial.println(data[0]);
        Serial.println(tmp);       
    }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{    
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Red,
    CRGB::Black


};
