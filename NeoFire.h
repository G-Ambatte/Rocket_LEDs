#ifndef NEOFIRE_H_
#define NEOFIRE_H_

#include <Adafruit_NeoPixel.h>
//#include "params.h"

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT, PIN, NEO_GRB + NEO_KHZ800);

///
/// Fire simulator
///
class NeoFire
{
    //Adafruit_NeoPixel &strip;

  public:

    NeoFire(Adafruit_NeoPixel&);
    void Begin();
    void Draw(uint32_t color);
    void Clear(bool update_afterwards = false);
    enum States { OFF, ON, BOOST };
    int getState();
    void setState(States target_state);
    void Update();

  private:

    Adafruit_NeoPixel &strip;
    void AddColor(uint8_t position, uint32_t color);
    void SubstractColor(uint8_t position, uint32_t color);
    uint32_t Blend(uint32_t color1, uint32_t color2);
    uint32_t Substract(uint32_t color1, uint32_t color2);
    unsigned long delay_draw;
    unsigned long delay_boost;
    States state;
    uint32_t fire_color;
    uint32_t off_color;
    uint32_t boost_color;

};

///
/// Constructor
///
NeoFire::NeoFire(Adafruit_NeoPixel& n_strip)
  : strip (n_strip)
{
  state = States::OFF;
  delay_draw = millis();
  fire_color = strip.Color (75, 30, 0);
  off_color  = strip.Color (0, 0, 0);
  boost_color = strip.Color (30, 30, 75);
}

void NeoFire::Begin()
{
  strip.begin();                                            //Initialize the strip
  Clear();                                                  //Turn off all pixels
}

int NeoFire::getState() {
  return state;                                             //Report state without exposing it
}

void NeoFire::setState(States target_state) {
  state = target_state;                       
  switch (state) {                                          //When setting a new state
    case States::OFF:                                       //if new state is OFF
      Clear(true);                                          //  clear the NeoPixels
    case States::BOOST:                                     //if new state is BOOST
      delay_boost = millis() + random(1000, 5000);          //  set boost timer to somewhere between 1 and 5 seconds
  }
}

void NeoFire::Update() {

  switch (state) {                                          //Fire State Machine
    case States::ON:                                        //if fire is ON
      if (millis() > delay_draw)                            //AND if the flame update timer has expired
      {
        Draw(fire_color);                                   //  show the fire!
        delay_draw = millis() + random(5, 60);              //  set a new flame update delay between 5 and 60ms
      }
      break;
    case States::BOOST:                                     //if fire is BOOST
      Draw(boost_color);                                    //  show the BOOST flame!
      if (millis() > delay_boost)                           //if boost timer is expired
      {
        state = States::ON;                                 //  set state to ON
      }
      break;
    case States::OFF:                                       //if fire is OFF
      break;                                                //  do nothing
    default:                                                //if somehow the fire is in none of the above states, which SHOULD be impossible
      state = States::OFF;                                  //  set it to OFF
      break;
  }
}
///
/// Set all colors
///
void NeoFire::Draw(uint32_t color)
{
  Clear();

  for (int i = 0; i < strip.numPixels(); i++)
  {
    AddColor(i, color);
    int r = random(70);
    uint32_t diff_color = strip.Color ( r, r / 2, r / 2);
    SubstractColor(i, diff_color);
  }

  strip.show();

}

///
/// Set color of LED
///
void NeoFire::AddColor(uint8_t position, uint32_t color)
{
  uint32_t blended_color = Blend(strip.getPixelColor(position), color);
  strip.setPixelColor(position, blended_color);
}

///
/// Set color of LED
///
void NeoFire::SubstractColor(uint8_t position, uint32_t color)
{
  uint32_t blended_color = Substract(strip.getPixelColor(position), color);
  strip.setPixelColor(position, blended_color);
}

///
/// Color blending
///
uint32_t NeoFire::Blend(uint32_t color1, uint32_t color2)
{
  uint8_t r1, g1, b1;
  uint8_t r2, g2, b2;
  uint8_t r3, g3, b3;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  return strip.Color(constrain(r1 + r2, 0, 255), constrain(g1 + g2, 0, 255), constrain(b1 + b2, 0, 255));
}

///
/// Color blending
///
uint32_t NeoFire::Substract(uint32_t color1, uint32_t color2)
{
  uint8_t r1, g1, b1;
  uint8_t r2, g2, b2;
  uint8_t r3, g3, b3;
  int16_t r, g, b;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  r = (int16_t)r1 - (int16_t)r2;
  g = (int16_t)g1 - (int16_t)g2;
  b = (int16_t)b1 - (int16_t)b2;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  return strip.Color(r, g, b);
}

///
/// Every LED to black
///
void NeoFire::Clear(bool update_afterwards)
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, off_color);
  }
  if (update_afterwards)
  {
    strip.show();
  }
}

#endif
