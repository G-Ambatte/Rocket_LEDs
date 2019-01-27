#ifndef DF_H_
#define DF_H_

// this example will play a track and then
// every five seconds play another track
//
// it expects the sd card to contain these three mp3 files
// but doesn't care whats in them
//
// sd:/mp3/0001.mp3
// sd:/mp3/0002.mp3
// sd:/mp3/0003.mp3

#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

//#include "params.h"

// implement a notification class,
// its member methods will get called
//
class Mp3Notify
{
  public:
    static void OnError(uint16_t errorCode)
    {
      // see DfMp3_Error for code meaning
#ifdef DEBUG_MP3
      Serial.println();
      Serial.print("Com Error ");
      Serial.println(errorCode);
#endif
    }

    static void OnPlayFinished(uint16_t globalTrack)
    {
#ifdef DEBUG_MP3
      Serial.println();
      Serial.print("Play finished for #");
      Serial.println(globalTrack);
#endif
    }

    static void OnCardOnline(uint16_t code)
    {
#ifdef DEBUG_MP3
      Serial.println();
      Serial.print("Card online ");
      Serial.println(code);
#endif
    }

    static void OnCardInserted(uint16_t code)
    {
#ifdef DEBUG_MP3
      Serial.println();
      Serial.print("Card inserted ");
      Serial.println(code);
#endif
    }

    static void OnCardRemoved(uint16_t code)
    {
#ifdef DEBUG_MP3
      Serial.println();
      Serial.print("Card removed ");
      Serial.println(code);
#endif
    }
};



//void setup()
//{
//  Serial.begin(115200);
//
//  Serial.println("initializing...");
//
//  mp3.begin();
//
//  uint16_t volume = mp3.getVolume();
//  Serial.print("volume ");
//  Serial.println(volume);
//  mp3.setVolume(24);
//
//  uint16_t count = mp3.getTotalTrackCount();
//  Serial.print("files ");
//  Serial.println(count);
//
//  Serial.println("starting...");
//}
//
//void waitMilliseconds(uint16_t msWait)
//{
//  uint32_t start = millis();
//
//  while ((millis() - start) < msWait)
//  {
//    // calling mp3.loop() periodically allows for notifications
//    // to be handled without interrupts
//    mp3.loop();
//    delay(1);
//  }
//}
//
//void loop()
//{
//  Serial.println("track 1");
//  mp3.playMp3FolderTrack(1);  // sd:/mp3/0001.mp3
//
//  waitMilliseconds(5000);
//
//  Serial.println("track 2");
//  mp3.playMp3FolderTrack(2); // sd:/mp3/0002.mp3
//
//  waitMilliseconds(5000);
//
//  Serial.println("track 3");
//  mp3.playMp3FolderTrack(3); // sd:/mp3/0002.mp3
//
//  waitMilliseconds(5000);
//}

#endif
