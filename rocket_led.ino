/* sketch 1
  turn on a LED when the button is pressed
  turn it off when the button is not pressed (or released)
*/
#include "NeoFire.h"
#include "params.h"

const int buttonCount = 2;                                      //total number of buttons to be connected
int buttonState[buttonCount] = {0, 0};                          //initial states of each button
int pinButton[buttonCount] = {2, 3};                            //the pins where we connect the buttons
#ifdef DEBUG
int LED = LED_BUILTIN;                                          //the pin for the Arduino's built in LED, usually 13
#endif

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
  fire.Begin();                                                 //initialize the NeoPixels
}

///
/// Main loop
///
void loop() {
  for (int i = 0; i < buttonCount; i++) {
    buttonState[i] = digitalRead(pinButton[i]);
  }
  if (buttonState[0] == 0) {                                    //if the first button is pressed
    if (fire.getState() == fire.States::OFF) {                  //if the fire is OFF
      fire.setState(fire.States::ON);                           //  turn it ON
#ifdef DEBUG
      digitalWrite(LED, 1);                                     //turn on the built in LED
#endif
    }
  } else {                                                      //if first button is not pressed
    if (fire.getState() == fire.States::ON) {                   //if the fire is ON
      fire.setState(fire.States::OFF);                          //  turn it OFF
#ifdef DEBUG
      digitalWrite(LED, 0);                                     //turn off the built in LED
#endif
    }
  }
  if (buttonState[1] == 0) {                                    //if the second button is pressed
    if (fire.getState() == fire.States::ON) {                   //  AND if the fire is ON
      fire.setState(fire.States::BOOST);                        //  turn it to BOOST
    }
  }

  fire.Update();                                                //update the NeoPixels
}
