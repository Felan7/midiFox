#include <Arduino.h>
#include <MIDIUSB.h>
#include "SPI.h"
#define CHIP_SELECT_PIN 10
#define MOSI_PIN 16
#define SCK_PIN 15
#define GATE_OUT_PIN 7

//define Pin Layout


struct vector2{
  byte x;
  byte y;
};

vector2 data;

/** Look-Up Table
 * look-up table that contains the digital number corresponding to the input of the DACs for every voltage output.
 * This may need to be configured manually to your needs.
 * */
const int noteTable[] = {
    0,    //0 C 0 

    33,   //1 C#0
    66,   //2 D 0
    99,  //3 D#0
    132,  //4 E 0
    165,  //5 F 0
    198,   //6 F#0
    231,   //7 G 0
    263,   //8 G#0
    297,   //9 A 0
    330,   //10 A#0
    363,   //11 B 0
    
    400,    //12 C 1
    462,   //13 C#1
    495,   //14 D 1
    528,  //15 D#1
    561,  //16 E 1
    594,  //17 F 1
    627,   //18 F#1
    660,   //19 G 1
    726,   //20 G#1
    759,   //21 A 1
    792,   //22 A#1
    852,   //23 B 1

    858,    //24 C 2
    891,    //25 C#2
    924,    //26 D 2
    957,    //27 D#2
    990,    //28 E 2
    1023,   //29 F 2
    1056,   //30 F#2
    1089,   //31 G 2
    1122,   //32 G#2
    1254,   //33 A 2
    1287,   //34 A#2
    1320,   //35 B 2

    1353,   //36 C 3  
    1386,   //37 C#3
    1419,   //38 D 3
    1452,   //39 D#3
    1485,   //40 E 3
    1518,   //41 F 3
    1551,   //42 F#3
    1584,   //43 G 3
    1617,   //44 G#3
    1650,   //45 A 3
    1683,   //46 A#3
    1716,   //47 B 3
    1749,   //48 C 3

    1782,   //49 C 4 
    1815,   //50 C#4
    1848,   //51 D 4
    1881,   //52 D#4
    1914,   //53 E 4
    1947,   //54 F 4
    1980,   //55 F#4
    2013,   //56 G 4
    2046,   //57 G#4
    2079,   //58 A 4
    2112,   //59 A#4
    2145,   //60 B 4
    2178,   //61 C 4

};



void setup() {
  //start Serial Monitor
  Serial.begin(9600);
  while (!Serial) {
    delay(10); 
   }
  Serial.println("SERIAL BEGIN");

  //define pin modes
  pinMode(GATE_OUT_PIN,    OUTPUT); //gate out (digital)
  pinMode(CHIP_SELECT_PIN, OUTPUT); //SPI Chip Select

  //SPI
  SPI.begin(); //Begin SPI Comunication
  SPI.setBitOrder(MSBFIRST);
}

/** Build byte A
 * Building byte a by use of the look-up table, than splitting the result 
 * into two bits, which are passed in the form of a vector2 struct.
 */
vector2 buildA(byte noteByte){
  struct vector2 returnValue;
  returnValue.x = 80;
  int tableValue = noteTable[noteByte];
  returnValue.x += tableValue >> 8;
  returnValue.y = (byte) tableValue;
  return returnValue;
}

/** Build byte B
 * Building byte a by multipling, than splitting the result 
 * into two bits, which are passed in the form of a vector2 struct.
 */
vector2 buildB(byte data){
  struct vector2 returnValue;
  returnValue.x = 208 + (data>>4);
  returnValue.y = data<<4;
  return returnValue;
}

void sendBytes(vector2 data){

  digitalWrite(CHIP_SELECT_PIN, LOW); //activate DAC Communication

  SPI.transfer(data.x); //send first byte
  SPI.transfer(data.y); //send second byte

  digitalWrite(CHIP_SELECT_PIN, HIGH); //deactivate DAC Communication
}


void loop() {
  //read and split MIDI packet 
  midiEventPacket_t event = MidiUSB.read(); //read Midi Signal to 'event'
  byte header = event.header;
  byte byteTwo = event.byte2;
  byte byteThree = event.byte3;

  //if midi on
  if(header == 0x09){

    Serial.println("MIDI On");

    //TO-DO: add velocity =! 0 check
    digitalWrite(GATE_OUT_PIN, HIGH);

    sendBytes(buildA(byteTwo));
   
    sendBytes(buildB(byteThree));

  }

  //if midi off
  if(header == 0x08){

    Serial.println("MIDI Off");
    
    digitalWrite(GATE_OUT_PIN, LOW);
  
  }
}


