#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <avr/io.h>
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

//Konstanta pin
#define inpButton A5

#define segment_a 2
#define segment_b 3
#define segment_c 6
#define segment_d 7
#define segment_e 8
#define segment_f 4
#define segment_g 5
#define decoder0 A0
#define decoder1 A1

int ButtonState;
int x1=25, x2=27, x3=26;
int y1=8, y2=7, y3=9;
SemaphoreHandle_t xMutex;

// define tasks
void TaskWall( void *pvParameters );
void TaskBird( void *pvParameters );

void setup(void){
  pinMode(inpButton, INPUT);
  
  //initialize TimerOne’s interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize( 2000 ); //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( ScanDMD ); //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  
  //clear/init the DMD pixels held in RAM
  dmd.clearScreen( true ); //true is normal (all pixels off), false is negative (all pixels on)
  Serial.begin(9600); // Serial print to check score

  xMutex = xSemaphoreCreateMutex();

  if(xMutex != NULL) {
    //  xTaskCreate(TaskWall,(const portCHAR *)"Display Wall",128,NULL,1,NULL);
    xTaskCreate(TaskBird,(const portCHAR *)"Display Bird",128,NULL,1,NULL);
    xTaskCreate(TaskButton,(const portCHAR *)"Read Button",128,NULL,3,NULL);
    
    vTaskStartScheduler();
  }
}

void loop(void){

}

void TaskButton(void *pvParameters) { // This is a task.
    
  for (;;) {
    ButtonState = debounceButton(ButtonState, inpButton);

    if(ButtonState == HIGH) {
      xSemaphoreTake(xMutex, portMAX_DELAY);
      {
        dmd.clearScreen( true );
        y1 = y1 + 1;
        y2 = y2 + 1;
        y3 = y3 + 1;
        dmd.drawLine( x1, y1, x2, y1, GRAPHICS_NORMAL);
        dmd.drawLine( x3, y2, x3, y3, GRAPHICS_NORMAL);
      }
      xSemaphoreGive(xMutex);
    }
    
    vTaskDelay( 50 / portTICK_PERIOD_MS ); // wait for 50ms
  }
}

void TaskBird(void *pvParameters) { // This is a task.
  for (;;) {    
    xSemaphoreTake(xMutex, portMAX_DELAY);
    {
      dmd.clearScreen( true );
      y1 = y1 - 1;
      y2 = y2 - 1;
      y3 = y3 - 1;
      dmd.drawLine( x1, y1, x2, y1, GRAPHICS_NORMAL);
      dmd.drawLine( x3, y2, x3, y3, GRAPHICS_NORMAL);
    }
    xSemaphoreGive(xMutex); 
    vTaskDelay( 250 / portTICK_PERIOD_MS ); // wait for 250ms
  }
}

void TaskWall(void *pvParameters) { // This is a task.
  int xWall1, xWall2, xWall3, xWall4; // variable for scrolling the wall
  int skor; // each time a wall vanished, score is incremented
  int tinggibwh1, tinggibwh2, tinggibwh3, tinggibwh4; // the height of the low part of the wall
  int lebar = 3; // lebar wall 3 pixel
  int tinggi = 16; // tinggi total wall
  int delaytime = 250; // jika dibutuhkan

  // inisiasi tinggi awal dan posisi awal dari setiap wall
  tinggibwh1 = random(1, 10);
  tinggibwh2 = random(1, 10);
  tinggibwh3 = random(1, 10);
  tinggibwh4 = random(1, 10);
  xWall1 = 0;
  xWall2 = xWall1-8;
  xWall3 = xWall1-16;
  xWall4 = xWall1-24;

  // inisiasi skor awal game
  skor = 0;
  
  for (;;) {
    // hilangkan wall untuk nanti dimunculkan lagi pada loop berikutnya
    dmd.clearScreen( true );
    
    Serial.println(skor);
    // gambar wall pertama
    dmd.drawFilledBox(xWall1,0,xWall1+2,tinggibwh1,GRAPHICS_NORMAL);
    dmd.drawFilledBox(xWall1,(tinggibwh1 + 6),xWall1+2,15,GRAPHICS_NORMAL);
    // gambar wall kedua
    dmd.drawFilledBox(xWall2,0,xWall2+2,tinggibwh2,GRAPHICS_NORMAL);
    dmd.drawFilledBox(xWall2,(tinggibwh2 + 6),xWall2+2,15,GRAPHICS_NORMAL);
    // gambar wall ketiga
    dmd.drawFilledBox(xWall3,0,xWall3+2,tinggibwh3,GRAPHICS_NORMAL);
    dmd.drawFilledBox(xWall3,(tinggibwh3 + 6),xWall3+2,15,GRAPHICS_NORMAL);
    // gambar wall keempat
    dmd.drawFilledBox(xWall4,0,xWall4+2,tinggibwh4,GRAPHICS_NORMAL);
    dmd.drawFilledBox(xWall4,(tinggibwh4 + 6),xWall4+2,15,GRAPHICS_NORMAL);
    
    // increment masing-masing posisi wall
    xWall1++;
    xWall2++;
    xWall3++;
    xWall4++;
    /* atur kasus agar setiap kali posisi wall mencapai maksimal, maka ia kembali ke posisi 
     * awal x = 0 saat kembali ke posisi x = 0, tinggi dari setiap wall juga diubah dengan 
     * tinggi yang baru setiap kali ada wall yang kembali ke posisi x = 0, skor bertambah 1
     */
    if(xWall1 >= 32) {
      xWall1 = 0;
      tinggibwh1 = random(1, 10);
      skor++;
    }
    if(xWall2 >= 32) {
      xWall2 = 0;
      tinggibwh2 = random(1, 10);
      skor++;
    }
    if(xWall3 >= 32) {
      xWall3 = 0;
      tinggibwh3 = random(1, 10);
      skor++;
    }
    if(xWall4 >= 32) {
      xWall4 = 0;
      tinggibwh4 = random(1, 10);
      skor++;
    }
    vTaskDelay( 250 / portTICK_PERIOD_MS ); // wait for 250ms
  }
}

int debounceButton(int stateButton, int pin) {
  int stateNow = digitalRead(pin);
  if(stateButton != stateNow) {
    delay(100);
    stateNow = digitalRead(pin);
  }
  return stateNow;
}
