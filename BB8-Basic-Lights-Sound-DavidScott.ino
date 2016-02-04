/*
    //===================================================================================================
    //==================  BB-8 Basic Lights and Sound using Arduino & MP3-FLASH-16P =====================
    //=========================================  v0.04  =================================================
    //===================================================================================================

    //Versions History
    //0.01 - Original Sketch by DavidScott
    //0.02 - r0n_dL added annotations & variable for PIN_sound_BUSY as suggested by DavidScott
	  //0.03 - r0n_dL reassigned pins to align with Padawan control system
		   - Changed SoftwareSerial pins (RX/TX) 10/9 to 8/4
    //0.04 - Change pinMode for PIN_trigger to INPUT_PULLUP to resolve issue using with Pro Micro

    //r0n_dL IMPORTANT NOTE & RECOGNITON: This sketch was written by DavidScott. Only thing I did was added 
      more annotations

    //DavidScott's original comment below
    This example plays a random file (001 to 010) forever
    It uses the playFileAndWait() command so no extra code is needed, it will play another file as soon as 
    the previous one finishes. If it doesn't work, try reversing the RX/TX wires as that's probably going to be
    the most common mistake. Also check that the player's BUSY line is connected to Arduino pin 3
    Finally check that the player has a speaker connected as well as ground and VCC

    // r0n_dL NOTES:  Basic sketch provided by DavidScott to give sounds and synchronized voice lights for 
                      scale BB-8 Domes

    // REQUIRED: MP3FLASH16P Library.  Library is located at
    // https://github.com/Critters/MP3FLASH16P
    // (that's an Arduino Library - it needs to be downloaded and extracted into your
    // Libraries folder for this sketch to work)
    
    // IMPORTANT: By default MP3FLASH16P library assumes using Software Serial (RX/TX) ports 10/11.
                  Change this pin assignments if using different pins in the .cpp file.  I'm using 8/4.

    // r0n_dL PARTS USED
    
    1 Arduino Pro Micro
    1 MP3-FLASH-16P MP3 Soundboard
    1 Red 3mm Flanged LED (Radar Eye)*
    1 Blue 3mm Flanged LED (Side Logic Display)*
    1 White 3mm Flanged LED (Top Front Logic Display)*
    1 White 5mm Flanged LED (PSI/Voice LED)*
    1 5v power supply
    2 1K ohm Resistors*
    2 220 ohm Resistors*
    1 56 ohm Resistor*
    1 Switch - local or remote

    *There are actually MORE LEDs required if using BB-8 Club spec (as of 4 Jan 16 T&M beta v033) pg 42
     If using additional LEDs, more resistors and/or different value resistors will be required.

     May want to consider adding a LED to the HP as well which will also require changes to the resistor 
     & wiring

    Example:  2 Front (Top & Bottom) Logic Lenses have 5 round 3mm LEDs each
              1 Side Logic Lens has 4 round 3mm LEDs

    // r0n_dL BASIC WIRING

 Switch | 5v Power | Pro Micro | MP3-FLASH-16P | Speaker | LEDs
        |          |  8*(1K R) |      TX       |         |     
        |          |  4 (1K R) |      RX       |         |     
        |          |  6        |               |         | + (5mm Wht 220 R)
        |          |  9        |               |         | + (3mm Wht 220 R)
        |          |  3        |     BUSY      |         |     
        |          |  7        |     SPK1      |   +     |      
        |          |           |     SPK2      |   -     |     
        |   POS    |  VCC      |     VCC       |         | + (3mm Red)- +(3mm Blue) 56 R
    1   |   NEG    |  GND      |     GND       |         | GND all LEDs
    2   |          |  A3       |               |         |
    
 NOTE: 3mm Red LED and 3mm Blue LED wired in series with 56 ohm Resistor
 * MP3 TX to Pin 8 not required.  Do not use pin 8 for anything else
 
 Future Pins:
 
 PS2 Controller
	PS2ATT  10
	PS2CMD  16
	PS2DAT  14
	Ps2CLK  15
 
 For Body Signal
	BODYRX RX1
	
 For WTV020SD-16P
	WTV1BSY A0
	WTV1CLK A1
	WTV1DAT A2

RESERVED
	8 (Analog) - RX (SoftwareSerial)
	4 TX (SoftwareSerial)
FREE
	TX0
	2 (Analog)
	5 (Digital)

	
 SoftwareSerial Limitations:
 The library has the following known limitations:
	If using multiple software serial ports, only one can receive data at a time.
	
	Not all pins on the Mega and Mega 2560 support change interrupts, so only the following can be used for 
	RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), 
	A14 (68), A15 (69).

	Not all pins on the Leonardo and Micro support change interrupts, so only the following can be used for
	RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
*/

#include "SoftwareSerial.h"
#include "MP3FLASH16P.h"
MP3FLASH16P myPlayer;

//Arduino Pin Assignments - Change to suit your needs
#define PIN_voice_LED     6  
#define PIN_pulse_LED     9  
#define PIN_sound         7 
#define PIN_trigger       A3
#define PIN_sound_BUSY    3
#define number_of_sounds  5
//Default Pin Assignments based off of the BASIC WIRING in NOTES - Original Pins below
//PIN_voice_LED     6  //Voice LED - 5mm White & 220 Ohm Resistor
//PIN_pulse_LED     5  //Logic LED - 3mm White & 220 Ohm Resistor
//PIN_sound         A2 //Speaker lead for sound & LED sync
//PIN_trigger       A3 //Connect to Switch
//PIN_sound_BUSY    3  //Connect to BUSY on MP3-FLASH-16P

void setup() 
{
    myPlayer.init(PIN_sound_BUSY); // Init the player with the MP3 BUSY pin connected to Arduino pin defined
    pinMode(PIN_voice_LED, OUTPUT);
    pinMode(PIN_pulse_LED, OUTPUT); 
    pinMode(PIN_sound, INPUT);
    pinMode(PIN_trigger, INPUT_PULLUP);
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
