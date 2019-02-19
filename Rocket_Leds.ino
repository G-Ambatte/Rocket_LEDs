#include "df.h"
#include "params.h"
#include "neoFire.h"

const int buttonCount = 3;                                          //total number of buttons to be connected
int buttonState[buttonCount] = {0, 0, 0};                           //initial states of each button
int pinButton[buttonCount] = {2, 3, 4};                             //the pins where we connect the buttons
int debounce[buttonCount] = {0, 0, 0};                              //initial debounce state for each button
int pinVolume = 14;                                                 //the pin where we connect the volume potentiometer
#ifdef DEBUG
int LED = LED_BUILTIN;                                              //the pin for the Arduino's built in LED, usually 13
#endif

int fire_rgb[3] = {75, 30, 0};
int ignite_rgb[3] = {128, 75, 30};

uint32_t fire_color = Adafruit_NeoPixel::Color (fire_rgb[0], fire_rgb[1], fire_rgb[2]);
uint32_t boost_color = Adafruit_NeoPixel::Color (30, 30, 75);

unsigned long timer;                                                //a timer for setting various delays
int delay_startup_sound = 900; // length of the startup sound in ms
int delay_run_sound = 1000; // length of the run sound in ms - not used, because we set loop mode
int delay_boost_sound = 1000; // length of the boost sound in ms - not used, because we set loop mode
int delay_shutdown_sound = 4150; // length of shutdown sound in ms

int STARTUP_SOUND = 1;                                              //track # for the STARTUP_SOUND
int RUN_SOUND = 2;                                                  //track # for the RUN_SOUND
int BOOST_SOUND = 3;                                                //track # for the BOOST_SOUND
int SHUTDOWN_SOUND = 4;                                             //track # for the SHUTDOWN_SOUND

bool mute = false;                                                  //initial mute state
uint8_t vol;                                                        //initial volume level

// instance a DFMiniMp3 object,
// defined with the above notification class and the hardware serial class
//
//DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial1);

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definition and uncomment these lines
SoftwareSerial secondarySerial(RX_PIN, TX_PIN); // RX, TX                   //create a serial communications channel on pins 10 and 11
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);          //create the MP3 player object using the serial communications channel

NeoFire fire(strip);                                                //create the fire object

///
/// Setup
///
void setup() {
  for (int i = 0; i < buttonCount; i++) {
    pinMode(pinButton[i], INPUT_PULLUP);                            //set all of the button pins as INPUT_PULLUP
  }
#ifdef DEBUG
  pinMode(LED, OUTPUT);                                             //set the built in LED pin as OUTPUT
#endif
  pinMode(PIN, OUTPUT);                                             //set the NeoPixel data pin as OUTPUT
  pinMode(pinVolume, INPUT_PULLUP);                                 //set the volume pin is INPUT_PULLUP
  fire.Begin();                                                     //initialize the NeoPixels
  mp3.begin();                                                      //initialize the MP3 player
  mp3.setRepeatPlay(false);                                         //single play only
}

///
/// Main loop
///
void loop() {
  ////  Volume Control - currently disabled; may revisit at a later date
  //  if (!mute) {                                                    //if mute is not true
  //    int currentVol = map(analogRead(pinVolume), 0, 1023, 0, 30);  //check the volume potentiometer position
  //    if (vol != currentVol) {                                      //if volume has changed
  //      vol = currentVol;                                           //  update the value
  //      mp3.setVolume(vol);                                         //  and set the volume to the new level
  //    }
  //  }

  for (int i = 0; i < buttonCount; i++) {                           //cycle through all of the pins
    buttonState[i] = digitalRead(pinButton[i]);                     //  update the states
  }
  if (buttonState[0] == 0) {                                        //if the first button is pressed
    if (debounce[0] == 0) {                                         //if the debounce flag is not set
      debounce[0] = 1;                                              //set the debounce flag for this pin
      switch (fire.getState()) {
        case (fire.States::OFF):                                    //if the fire is OFF
          fire.setState(fire.States::INIT_STARTUP);                 //  turn it INIT_STARTUP
          break;
        case (fire.States::SHUTDOWN):                               //if the fire is in SHUTDOWN
          fire.setState(fire.States::INIT_RUN);                     //  set fire to INIT_RUN
          break;
        default:
          break;
#ifdef DEBUG
          digitalWrite(LED, 1);                                     //turn on the built in LED
#endif
      }
    }
  } else {                                                          //if first button is not pressed
    if (debounce[0] == 1) {
      debounce[0] = 0;                                              //clear the debounce flag
      if (fire.getState() != fire.States::OFF                       //if the fire is not in OFF
          && fire.getState() != fire.States::BOOST) {               //    or BOOST
        fire.setState(fire.States::INIT_SHUTDOWN);                  //  turn it SHUTDOWN
#ifdef DEBUG
        digitalWrite(LED, 0);                                       //turn off the built in LED
//        Serial.println("- BUTTON 1 UP");
#endif
      }
    }
  }

  if (buttonState[1] == 0) {                                        //if the second button is pressed
    if (debounce[1] == 0) {                                         //if the debounce flag is not set
      debounce[1] = 1;                                              //set the debounce flag for this pin
      if (fire.getState() == fire.States::RUN) {                    //  AND if the fire is ON
        fire.setState(fire.States::INIT_BOOST);                     //  turn it to BOOST
      }
    }
  } else {                                                          //if the second button is NOT pressed
    if (debounce[1] == 1) {                                         //  if debounce flag is set
      debounce[1] = 0;                                              //    clear the debounce flag
#ifdef DEBUG
//      Serial.println("- BUTTON 2 UP");
#endif
    }
  };

  //  if (buttonState[2] == 0) {                                    //if the third button (switch) is closed
  //    if (debounce[2] == 0) {                                     //if the debounce flag is not set
  //      debounce[2] = 1;                                          //set the debounce flag
  //      mute = true;                                              //  turn off sound
  //      mp3.stop();
  //    }
  //  } else {                                                      //if the third button (switch) is open
  //    debounce[2] = 0;                                            //clear the debounce flag
  //    mute = false;                                               //  turn on sound
  //  }

  mp3.loop();                                                       //check for MP3 notifications

  //
  // Fire State Machine
  //

  switch (fire.getState()) {
    case fire.States::INIT_STARTUP:
      timer = millis() + delay_startup_sound;                       //  set a delay until end of start up sound
      if (!mute) {                                                  //  if mute is not true
        //mp3.stop();                                                 //    stop any current sounds
        //mp3.setRepeatPlay(false);                                   //    turn off loop playing
        mp3.playMp3FolderTrack(STARTUP_SOUND);                      //    play the STARTUP sound
      }
      fire.setState(fire.States::STARTUP);
    case fire.States::STARTUP:                                      //if the fire is STARTING UP
      if (millis() > fire.delay_draw) {                             //  AND if the flame update timer has expired
        float gamma = 1 - (1.0 * (timer - millis()) / delay_startup_sound);
        fire.Draw(Adafruit_NeoPixel::Color(gamma * ignite_rgb[0], gamma * ignite_rgb[1], gamma * ignite_rgb[2]));   //  show the fire!
        fire.delay_draw = millis() + random(5, 60);                 //  set a new flame update delay between 5 and 60ms
      }
      if (millis() >= timer) {                                      //  if the timer is up (STARTUP sound has completed playing)
        fire.setState(fire.States::INIT_RUN);                       //  set the fire state to INIT_RUN
      }
      break;
      
    case fire.States::INIT_RUN:
      fire.delay_draw = millis();                                   //  set fire flicker effect timer
      if (!mute) {                                                  //  if mute is not true
        //mp3.stop();                                                 //    stop anything that is still playing (shouldn't be necessary but including anyway)
        //mp3.setRepeatPlay(true);                                    //    turn on loop play
        mp3.playMp3FolderTrack(RUN_SOUND);                          //    play the RUN sound
      }
      fire.setState(fire.States::RUN);                              //  set the fire to RUN
    case fire.States::RUN:                                          //if fire is ON
      if (millis() > fire.delay_draw) {                             // AND if the flame update timer has expired
        fire.Draw(fire_color);                                      //  show the fire!
        fire.delay_draw = millis() + random(5, 60);                 //  set a new flame update delay between 5 and 60ms
      }
      break;
      
    case fire.States::INIT_BOOST:
      if (!mute) {                                                  //if mute is not true
        //mp3.stop();                                                 //  stop any current sounds
        //mp3.setRepeatPlay(true);                                    //  turn on loop play
        mp3.playMp3FolderTrack(BOOST_SOUND);                        //  play the BOOST sound
      }
      fire.setState(fire.States::BOOST);
    case fire.States::BOOST:                                        //if fire is BOOST
      fire.Draw(boost_color);                                       //  show the BOOST flame!
      if (millis() > fire.delay_boost) {                            //  if boost timer is expired
        fire.setState(fire.States::INIT_RUN);                       //    set state to INIT_RUN
        if (buttonState[0] == 1) {                                  //    if the button is not pressed
          debounce[0] = 1;                                          //    clear the debounce flag on button 1, which should trigger the SHUTDOWN transition on next loop
        }
      }
      break;
      
    case fire.States::INIT_SHUTDOWN:
      timer = millis() + delay_shutdown_sound;                      //set timer to end of SHUTDOWN
      fire.delay_draw = millis();                                   //set fire flicker timer
      if (!mute) {
        //mp3.stop();                                                 //  stop playing sound
        //mp3.setRepeatPlay(false);                                   //  turn off loop play
        mp3.playMp3FolderTrack(SHUTDOWN_SOUND);                     //  play the shutdown sound
      }
      fire.setState(fire.States::SHUTDOWN);
    case fire.States::SHUTDOWN:                                     //if fire is SHUTDOWN
      if (millis() > fire.delay_draw)                               //  AND if the flame update timer has expired
      {
        float gamma = 1.0 * (timer - millis()) / delay_shutdown_sound;
        fire.Draw(Adafruit_NeoPixel::Color(gamma * fire_rgb[0], gamma * fire_rgb[1], gamma * fire_rgb[2]));   //  show the fire!
        fire.delay_draw = millis() + random(5, 60);                 //  set a new flame update delay between 5 and 60ms
      }
      if (millis() > timer) {                                       //  if timer has expired
        fire.setState(fire.States::INIT_OFF);                       //    set state to OFF
      }
      break;
      
    case fire.States::INIT_OFF:
      mp3.stop();                                                   //stop any playing sounds
      fire.setState(fire.States::OFF);                              // and turn it OFF
    case fire.States::OFF:                                          //if fire is OFF
      break;                                                        //  do nothing
      
    default:                                                        //if somehow the fire is in none of the above states, which SHOULD be impossible
      fire.setState(fire.States::OFF);                              //  set it to OFF
      break;
  }
}
