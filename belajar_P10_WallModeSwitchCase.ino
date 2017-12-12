/*–Includes——-*/
#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "SystemFont5x7.h"
#include "Arial_Black_16_ISO_8859_1.h"

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

/*
Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
called at the period set in Timer1.initialize();
*/
void ScanDMD(){
  dmd.scanDisplayBySPI();
}

/*————————————————————————-
setup
Called by the Arduino architecture before the main loop begins
————————————————————————-*/
void setup(void){

  //initialize TimerOne’s interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize( 2000 ); //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( ScanDMD ); //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  
  //clear/init the DMD pixels held in RAM
  dmd.clearScreen( true ); //true is normal (all pixels off), false is negative (all pixels on)
  Serial.begin(9600);
}
/*
 
 void GenerateWall(int a);{
  
}
*/
/*————————————————————————-
loop
Arduino architecture main loop
————————————————————————-*/
void loop(void){
  int i, j, k, l, m, n, o, row, col, tinggibwh, tinggibwh2, tinggibwh3, tinggibwh4, tinggibwh5;
  byte a = true;
  int lebar = 3;
  int tinggi = 16;
  int delaytime = 50000;
// fungsi untuk memunculkan 1 baris saja
/*
  for(row = 0; row<1; row++){
    for(col = 0; col<16; col++){
    dmd.writePixel(row, col, GRAPHICS_NORMAL, a);      
    }
  }
*/
  tinggibwh = random(0, 11);
  tinggibwh2 = random(0, 11);
  tinggibwh3 = random(0, 11);
  tinggibwh4 = random(0, 11);
  for(i=0;i<64;i++){
  j = i-9;
  k = i-18;
  l = i-27;
    switch (i){
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:{
        dmd.drawFilledBox(i,0,i+2,tinggibwh,GRAPHICS_NORMAL);
        dmd.drawFilledBox(i,(tinggibwh + 6),i+2,15,GRAPHICS_NORMAL);
        delay(200);
        dmd.clearScreen( true ); 
        break;
      }
      case 6:
      case 7:
      case 8:{
        dmd.drawFilledBox(i,0,i+2,tinggibwh,GRAPHICS_NORMAL);
        dmd.drawFilledBox(i,(tinggibwh + 6),i+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,0,j+2,tinggibwh2,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,(tinggibwh2 + 6),j+2,15,GRAPHICS_NORMAL);
        delay(200);
        dmd.clearScreen( true ); 
        break;
      }
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:{
        dmd.drawFilledBox(i,0,i+2,tinggibwh,GRAPHICS_NORMAL);
        dmd.drawFilledBox(i,(tinggibwh + 6),i+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,0,j+2,tinggibwh2,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,(tinggibwh2 + 6),j+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(k,0,k+2,tinggibwh3,GRAPHICS_NORMAL);
        dmd.drawFilledBox(k,(tinggibwh3 + 6),k+2,15,GRAPHICS_NORMAL);
        delay(200);
        dmd.clearScreen( true ); 
        break;
      }
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:{
        dmd.drawFilledBox(i,0,i+2,tinggibwh,GRAPHICS_NORMAL);
        dmd.drawFilledBox(i,(tinggibwh + 6),i+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,0,j+2,tinggibwh2,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,(tinggibwh2 + 6),j+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(k,0,k+2,tinggibwh3,GRAPHICS_NORMAL);
        dmd.drawFilledBox(k,(tinggibwh3 + 6),k+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(l,0,l+2,tinggibwh4,GRAPHICS_NORMAL);
        dmd.drawFilledBox(l,(tinggibwh4 + 6),l+2,15,GRAPHICS_NORMAL);
        delay(200);
        dmd.clearScreen( true ); 
        break;
      }
      case 27:
      case 28:
      case 29:
      case 30:
      case 31:{
        dmd.drawFilledBox(i,0,i+2,tinggibwh,GRAPHICS_NORMAL);
        dmd.drawFilledBox(i,(tinggibwh + 6),i+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,0,j+2,tinggibwh2,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,(tinggibwh2 + 6),j+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(k,0,k+2,tinggibwh3,GRAPHICS_NORMAL);
        dmd.drawFilledBox(k,(tinggibwh3 + 6),k+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(l,0,l+2,tinggibwh4,GRAPHICS_NORMAL);
        dmd.drawFilledBox(l,(tinggibwh4 + 6),l+2,15,GRAPHICS_NORMAL);
        delay(200);
        dmd.clearScreen( true ); 
        break;
      }
/*      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
      case 37:
      case 38:
      case 39:
      case 40:
      case 41:
      case 42:
      case 43:
      case 44:
      case 45:
      case 46:
      case 47:
      case 48:
      case 49:
      case 50:
      case 51:
      case 52:
      case 53:
      case 54:
      case 55:
      case 56:
      case 57:
      case 58:
      case 59:
      case 60:
      case 61:
      case 62:
      case 63:{
        dmd.drawFilledBox(i,0,i+2,tinggibwh,GRAPHICS_NORMAL);
        dmd.drawFilledBox(i,(tinggibwh + 6),i+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,0,j+2,tinggibwh2,GRAPHICS_NORMAL);
        dmd.drawFilledBox(j,(tinggibwh2 + 6),j+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(k,0,k+2,tinggibwh3,GRAPHICS_NORMAL);
        dmd.drawFilledBox(k,(tinggibwh3 + 6),k+2,15,GRAPHICS_NORMAL);
        dmd.drawFilledBox(l,0,l+2,tinggibwh4,GRAPHICS_NORMAL);
        dmd.drawFilledBox(l,(tinggibwh4 + 6),l+2,15,GRAPHICS_NORMAL);
        delay(delaytime);
        dmd.clearScreen( true ); 
        break;
      }*/
    }
  }
}
