#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

void ScanDMD(){
  dmd.scanDisplayBySPI();
}

void setup(void){
  //initialize TimerOne’s interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize( 2000 ); //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( ScanDMD ); //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  
  //clear/init the DMD pixels held in RAM
  dmd.clearScreen( true ); //true is normal (all pixels off), false is negative (all pixels on)
  Serial.begin(9600); // Serial print to check score

}

void loop(void){
  int x1, x2, x3, x4; // variable for scrolling the wall
  int skor; // each time a wall vanished, score is incremented
  int tinggibwh1, tinggibwh2, tinggibwh3, tinggibwh4; // the height of the low part of the wall
  int lebar = 3; // lebar wall 3 pixel
  int tinggi = 16; // tinggi total wall
  int delaytime = 200; // jika dibutuhkan

  // inisiasi tinggi awal dan posisi awal dari setiap wall
  tinggibwh1 = random(1, 10);
  tinggibwh2 = random(1, 10);
  tinggibwh3 = random(1, 10);
  tinggibwh4 = random(1, 10);
  x1 = 0;
  x2 = x1-8;
  x3 = x1-16;
  x4 = x1-24;

  // inisiasi skor awal game
  skor = 0;

  // loop tak hingga
  for( ;; ){
    Serial.println(skor);
    // gambar wall pertama
    dmd.drawFilledBox(x1,0,x1+2,tinggibwh1,GRAPHICS_NORMAL);
    dmd.drawFilledBox(x1,(tinggibwh1 + 6),x1+2,15,GRAPHICS_NORMAL);
    // gambar wall kedua
    dmd.drawFilledBox(x2,0,x2+2,tinggibwh2,GRAPHICS_NORMAL);
    dmd.drawFilledBox(x2,(tinggibwh2 + 6),x2+2,15,GRAPHICS_NORMAL);
    // gambar wall ketiga
    dmd.drawFilledBox(x3,0,x3+2,tinggibwh3,GRAPHICS_NORMAL);
    dmd.drawFilledBox(x3,(tinggibwh3 + 6),x3+2,15,GRAPHICS_NORMAL);
    // gambar wall keempat
    dmd.drawFilledBox(x4,0,x4+2,tinggibwh4,GRAPHICS_NORMAL);
    dmd.drawFilledBox(x4,(tinggibwh4 + 6),x4+2,15,GRAPHICS_NORMAL);
    
    // Tentukan pergeseran wall setiap berapa detik
    delay(delaytime);
    // hilangkan wall untuk nanti dimunculkan lagi pada loop berikutnya
    dmd.clearScreen( true ); 
    // increment masing-masing posisi wall
    x1++;
    x2++;
    x3++;
    x4++;
    /* atur kasus agar setiap kali posisi wall mencapai maksimal, maka ia kembali ke posisi 
     * awal x = 0 saat kembali ke posisi x = 0, tinggi dari setiap wall juga diubah dengan 
     * tinggi yang baru setiap kali ada wall yang kembali ke posisi x = 0, skor bertambah 1
     */
    if(x1 >= 32) {
      x1 = 0;
      tinggibwh1 = random(1, 10);
      skor++;
    }
    if(x2 >= 32) {
      x2 = 0;
      tinggibwh2 = random(1, 10);
      skor++;
    }
    if(x3 >= 32) {
      x3 = 0;
      tinggibwh3 = random(1, 10);
      skor++;
    }
    if(x4 >= 32) {
      x4 = 0;
      tinggibwh4 = random(1, 10);
      skor++;
    }
  }
}
