/*
    This example plays a random file (001 to 010) forever
    It uses the playFileAndWait() command so no extra code is needed, it will play another file as soon as the previous one finishes
    If it doesn't work, try reversing the RX/TX wires as that's probably going to be the most common mistake
    Also check that the player's BUSY line is connected to Arduino pin 3
    Finally check that the player has a speaker connected as well as ground and VCC
*/

#include "SoftwareSerial.h"
#include "MP3FLASH16P.h"
MP3FLASH16P myPlayer;

#define PIN_voice_LED     6
#define PIN_pulse_LED     5
#define PIN_sound         A4
#define PIN_trigger       A3
#define number_of_sounds  5

void setup() {
    myPlayer.init(2); // Init the player with the busy pin connected to Arduino pin 3
    pinMode(PIN_voice_LED, OUTPUT);
    pinMode(PIN_pulse_LED, OUTPUT); 
    pinMode(PIN_sound, INPUT);
    pinMode(PIN_trigger, INPUT);
    randomSeed(analogRead(0));
    Serial.begin(9600);
}

int voiceBrightness;
int pulseBrightness;
void loop() {
    // Checking for trigger going LOW
    if(digitalRead(PIN_trigger) == LOW){
        if(!myPlayer.isBusy()){
            myPlayer.playFile(random(1, number_of_sounds+1));
            delay(100);
        }
    }
    
    // Voice light
    if(myPlayer.isBusy()){
        // Sets the brightness of the light based on the loudness of the voice
        voiceBrightness = constrain(map(analogRead(PIN_sound), 700, 1024, 0, 255), 0, 255);
        analogWrite(PIN_voice_LED, voiceBrightness);
    }else{
        // No voice playing, light is off
        analogWrite(PIN_voice_LED, 0);
    }
    
    // Slow pulsing light on the side of BB-8's head
    pulseBrightness = 170+(sin(millis()/400.00)*80);
    analogWrite(PIN_pulse_LED, pulseBrightness);
}
