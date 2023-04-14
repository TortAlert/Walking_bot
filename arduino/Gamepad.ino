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