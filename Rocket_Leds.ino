/* sketch 1
  turn on a LED when the button is pressed
  turn it off when the button is not pressed (or released)
*/

#include "df.h"
#include "params.h"
#include "neoFire.h"

const int buttonCount = 3;                                      //total number of buttons to be connected
int buttonState[buttonCount] = {0, 0, 0};                       //initial states of each button
int pinButton[buttonCount] = {2, 3, 4};                         //the pins where we connect the buttons
int debounce[buttonCount] = {0, 0, 0};                          //initial debounce state for each button
int pinVolume = 14;                                             //the pin where we connect the volume potentiometer
#ifdef DEBUG
int LED = LED_BUILTIN;                                          //the pin for the Arduino's built in LED, usually 13
#endif

bool mute = false;                                              //initial mute state
uint8_t vol;                                                    //initial volume level

// instance a DFMiniMp3 object,
// defined with the above notification class and the hardware serial class
//
//DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial1);

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definition and uncomment these lines
SoftwareSerial secondarySerial(10, 11); // RX, TX               //create a serial communications channel on pins 10 and 11
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);      //create the MP3 player object using the serial communications channel

NeoFire fire(strip);                                            //create the fire object

///
/// Setup
///
void setup()
{
  for (int i = 0; i < buttonCount; i++) {
    pinMode(pinButton[i], INPUT_PULLUP);                        //set all of the button pins as INPUT_PULLUP
  }
#ifdef DEBUG
  pinMode(LED, OUTPUT);                                         //set the built in LED pin as OUTPUT
#endif
  pinMode(PIN, OUTPUT);                                         //set the NeoPixel data pin as OUTPUT
  pinMode(pinVolume, INPUT_PULLUP);                             //set the volume pin is INPUT_PULLUP
  fire.Begin();                                                 //initialize the NeoPixels
  mp3.begin();                                                  //initialize the MP3 player
  mp3.setVolume(24);                                            //set initial volume
  mp3.setRepeatPlay(false);                                      //all sounds play until stopped, so turn on loop play
}

///
/// Main loop
///
void loop() {
////  Volume Control - currently disabled; may revisit at a later date
//  if (!mute) {                                                    //if mute is not true
//    int currentVol = map(analogRead(pinVolume), 0, 1023, 0, 30); //check the volume potentiometer position
//    if (vol != currentVol) {                                      //if volume has changed
//      vol = currentVol;                                           //  update the value
//      mp3.setVolume(vol);                                         //  and set the volume to the new level
//    }
//  }

  for (int i = 0; i < buttonCount; i++) {                       //cycle through all of the pins
    buttonState[i] = digitalRead(pinButton[i]);                 //  update the states
  }
  if (buttonState[0] == 0) {                                    //if the first button is pressed
    if (debounce[0] == 0) {                                     //if the debounce flag is not set
      debounce[0] = 1;                                          //set the debounce flag for this pin
      if (fire.getState() == fire.States::OFF) {                //if the fire is OFF
        fire.setState(fire.States::STARTUP);                    //  turn it ON
        if (!mute) {                                            //  if mute is not true
          mp3.stop();                                           //    stop any current sounds
          mp3.playMp3FolderTrack(1);                            //    play the ON sound
        }
#ifdef DEBUG
        digitalWrite(LED, 1);                                   //turn on the built in LED
#endif
      }
    }
  } else {                                                      //if first button is not pressed
    debounce[0] = 0;                                            //clear the debounce flag
    if (fire.getState() == fire.States::RUN) {                  //if the fire is ON
      fire.setState(fire.States::OFF);                          //  turn it OFF
      mp3.stop();                                               //  stop playing sound
#ifdef DEBUG
      digitalWrite(LED, 0);                                     //turn off the built in LED
#endif
    }
  }
  if (buttonState[1] == 0) {                                    //if the second button is pressed
    if (debounce[0] == 0) {                                     //if the debounce flag is not set
      debounce[1] = 1;                                          //set the debounce flag for this pin
      if (fire.getState() == fire.States::RUN) {                 //  AND if the fire is ON
        fire.setState(fire.States::BOOST);                      //  turn it to BOOST
        if (!mute) {                                            //  if mute is not true
          mp3.stop();                                           //    stop any current sounds
          mp3.playMp3FolderTrack(2);                            //    play the BOOST sound
        }
      }
    }
  } else {                                                      //if the second button is NOT pressed
    debounce[1] = 0;                                            //clear the debounce flag
  }
  if (buttonState[2] == 0) {                                    //if the third button (switch) is closed
    if (debounce[2] == 0) {                                     //if the debounce flag is not set
      debounce[2] = 1;                                          //set the debounce flag
      mute = true;                                              //  turn off sound
      mp3.stop();
    }
  } else {                                                      //if the third button (switch) is open
    debounce[2] = 0;                                             //clear the debounce flag
    mute = false;                                               //  turn on sound
  }

  fire.Update();                                                //update the NeoPixels
  mp3.loop();                                                   //check for MP3 notifications
}
