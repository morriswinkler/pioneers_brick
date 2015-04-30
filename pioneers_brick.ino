#include <SoftwareSerial.h>
#include <EEPROM.h>

// serial pins
#define rxPin 2
#define txPin 3

// initialice serial port
SoftwareSerial mySerial(rxPin, txPin);

// EEPROM read variable and last pos 
int zz;
int zzLast = 0;

// color of this brick between 1 and 252
const int myColor = 2;
const char myCharColor = myColor;


const int EEsize = 256; // size in bytes of attiny45 EEPROM

void setup()
{ 
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(4, OUTPUT);
  // Configure counter/timer0 for fast PWM on PB0 and PB1
  TCCR0A = 3<<COM0A0 | 3<<COM0B0 | 3<<WGM00;
  TCCR0B = 0<<WGM02 | 3<<CS00; // Optional; already set
  // Configure counter/timer1 for fast PWM on PB4
  GTCCR = 1<<PWM1B | 3<<COM1B0;
  TCCR1 = 3<<COM1A0 | 7<<CS10;
  
  mySerial.begin(9600);  //initilize serial line

  // initialize EEPROM if empty
  resetEE(true); 
  
 
}


/**
 * Software Pulse Width Modulation for RGB
 */
void rgbLed(int rVal, int gVal, int bVal) {
       *&OCR1B = 255 - rVal;
       *&OCR0B = 255 - gVal;
       *&OCR0A = 255 - bVal;      
}


void loop() {   
    for (int a=0; a<EEsize; a++) {
      
      zz = EEPROM.read(a);
      if( zz == 255) {
        zzLast = a;
        break;
      } else {
//        mySerial.print("EEPROM position: ");
//        mySerial.print(a);
//        mySerial.print(" contains ");
//        mySerial.println(zz);
          

          rgbLed(getColor(zz, 0), getColor(zz, 1), getColor(zz, 2));
        
          if(mySerial.available() && zzLast != 0) {
            int inChar = mySerial.read();
            mySerial.print("Catched char: ");
            mySerial.println(inChar);  
            EEPROM.write(zzLast, inChar);
            zzLast = 3;
            
            while(zzLast != 0) {
              
              rgbLed(0,0,0);
              delay(500);
              rgbLed(0,255,0);
              delay(500);
              zzLast--;
            }
            rgbLed(0,0,0);
            delay(500);
            mySerial.flush();
          }
            
        delay(1000);
        mySerial.println(myCharColor);  
      }
    
    }
    
    
}

 /*
 / get Color travels in 252 steps trough 24 bit color space
 /
 / 252,0,0 -> 252,252,0 -> 0,252,0 -> 0,252,252 -> 0,0,252, -> 252,0,252, -> reset    
 /
*/

int getColor(int val, int color) {
     if(val <=42) {
       switch (color) {
         case 0:
           return 252;
           break;
         case 1: 
           return 6 * val;
           break;
         case 2:
           return 0;
           break;
       }
     } else if(val <=84) {
       switch (color) {
         case 0:
           return 252 - (6*(val - 42));
           break;
         case 1:
           return 252;
           break;
         case 2:
           return 0;
           break;
       }
     } else if(val <=126) {
       switch (color) {
         case 0:
           return 0;
           break;
         case 1: 
           return 252;
           break;
         case 2:
           return 6 * (val - 84);
           break;
       }
     } else if(val <=168) {
       switch (color) {
         case 0:
           return 0;
           break;
         case 1:
           return 252 - (6*(val - 126));
           break;
         case 2:
           return 252;
           break;
       }
     } else if(val <=210) {
       switch (color) {
       case 0:
         return 6*(val - 168);
         break;
       case 1:
         return 0;
         break;
       case 2: 
         return 252;
         break;
       }
     } else if(val <= 252) {
       switch (color) {
         case 0:
           return 252;
           break;
         case 1:
           return 0;
           break;
         case 2:
           return 252 - (6*(val - 210));
           break;
       }
     }
}

//// reset EEPROM if 0 is not sed yet or force
void resetEE(boolean force) {
  zz = EEPROM.read(0);
  if( force || zz == 255) {
    for (int a=0; a<EEsize; a++) {
      if( a == 0) {
        EEPROM.write(a, myColor);
      } else {
        EEPROM.write(a, 255);
      }
    }
  }
}
