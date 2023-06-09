# Walking_bot
Задача: Основанный на Ардуино бот управляемый NRF24 радио модулем.

Продукт: Было принято решение делать околошагающего робота:
![IMG_2795](https://github.com/TortAlert/fabrika-bot/assets/60708873/5c82d76b-5b91-48b7-9e87-184a2eaebb29)
![IMG_2796](https://github.com/TortAlert/fabrika-bot/assets/60708873/9cf12e98-99e9-434d-9d29-72c06f33367d)
![IMG_2797](https://github.com/TortAlert/fabrika-bot/assets/60708873/df22c7e5-0a35-438e-928f-0ba4cfa5db4b)



https://github.com/TortAlert/fabrika-bot/assets/60708873/d4f3c4af-00cf-46fe-9ab5-31391a0af2ce



Необходимые элементы:
- NRF,
- NRF+,
- x2 5v to 3.3 v,
- Motor Shield,
- Arduino Leonardo/Amperka Iskra Neo,
- x2 18650,
- x2 DC motor,
- x2 joystick,
- RGB LED tape (1x8),
- potentiometer,
- button,
- powerbank,
- servo 180,
- needle.

Все детали корпуса были вырезаны лазерной гравировкой по дереву из фонеры и склеены термоклеем.

1. Joystick code: 

``` c++
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);
int data[7];  

const int xPin = A0;
const int yPin = A1;
const int buttonPin_left = 4;
const int buttonPin_right = 7;
const int ptmrPin = A2;
//const int tumblrPin = 0;

int xPosition = 0;
int yPosition = 0;
int buttonState_left = 0;
int buttonState_right = 0;
//int tumblrState = 0;
int ptmrState = 0;

int speed_left     = 0;
int speed_rigt     = 0;
int direction_left = 0;
int direction_rigt = 0;
                                
void setup(){
    radio.begin();
    radio.setChannel(10); // Channel (from 0 to 127), 5 - freq 2,405 GHz (only 1 receiver on a single channel and up to 6 transmitters)
    radio.setDataRate     (RF24_1MBPS); // Speed (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.setPALevel      (RF24_PA_HIGH); // Transmitter power (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openWritingPipe (0x1234567890LL); 
    
    pinMode(xPin, INPUT);
    pinMode(yPin, INPUT);
    Serial.begin(9600);
    pinMode(buttonPin_left, INPUT_PULLUP);
    pinMode(buttonPin_right, INPUT_PULLUP);
    //pinMode(tumblrPin, INPUT);
    pinMode(ptmrPin, INPUT);
}
void loop(){
    xPosition = analogRead(xPin);
    yPosition = analogRead(yPin);
    buttonState_left = digitalRead(buttonPin_left);
    buttonState_right = digitalRead(buttonPin_right);
    //tumblrState = digitalRead(tumblrPin);
    ptmrState = analogRead(ptmrPin);
    

    if (xPosition > 520) {
      direction_left = 0; // 0 - Forward, 1 - Backward
      speed_left = map(xPosition, 510, 1023, 5, 200);
    }
    else if (xPosition < 500) {
      direction_left = 1;
      speed_left = map(xPosition, 510, 0, 5, 200);
    }
    else {
      direction_left = 2;
      speed_left = 0;
    }

    if (yPosition > 490) {
      
      direction_rigt = 0; // 0 - Forward, 1 - Backward
      speed_rigt = map(yPosition, 475, 1023, 5, 160); 
    }
    else if (yPosition < 460) {
      
      direction_rigt = 1;
      speed_rigt = map(yPosition, 475, 0, 5, 160);
      
    }
    else {
      direction_rigt = 2;
      speed_rigt = 0;
    }

    data[0] = direction_left;
    data[1] = direction_rigt;
    data[2] = speed_left;
    data[3] = speed_rigt;
    data[4] = buttonState_left;
    data[5] = buttonState_right;
    data[6] = ptmrState;
    //data[6] = tmblrState;
    radio.write(&data, sizeof(data)); // check delivery: if(radio.write(&data, sizeof(data)))

    Serial.println();
    Serial.println(yPosition);
    Serial.println(speed_rigt);
    Serial.println(data[6]);
    //Serial.println(speed);
    delay(200);
}
```

2. Bot code:

``` c++
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

```
