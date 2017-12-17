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
#define segment_c 4
#define segment_d 5
#define segment_e A2
#define segment_f A3
#define segment_g A4

#define decoder0 A0
#define decoder1 A1

int ButtonState;
int x1=25, x2=27, x3=26;
int y1=8, y2=7, y3=9;
int score = 0;
SemaphoreHandle_t xMutex;

// define tasks
void TaskWall( void *pvParameters );
void TaskBird( void *pvParameters );
void TaskButton( void *pvParameters );
void TaskScore( void *pvParameters );

void setup(void){
  //Input
  pinMode(inpButton, INPUT);
  //Output
  pinMode(segment_a, OUTPUT);
  pinMode(segment_b, OUTPUT);
  pinMode(segment_c, OUTPUT);
  pinMode(segment_d, OUTPUT);
  pinMode(segment_e, OUTPUT);
  pinMode(segment_f, OUTPUT);
  pinMode(segment_g, OUTPUT);
  
  pinMode(decoder0, OUTPUT);
  pinMode(decoder1, OUTPUT);
  
  //initialize TimerOne’s interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize( 2000 ); //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( ScanDMD ); //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  
  //clear/init the DMD pixels held in RAM
  dmd.clearScreen( true ); //true is normal (all pixels off), false is negative (all pixels on)
  Serial.begin(9600); // Serial print to check score

  xMutex = xSemaphoreCreateMutex();

  if(xMutex != NULL) {
    xTaskCreate(TaskWall,(const portCHAR *)"Display Wall",128,NULL,2,NULL);
    xTaskCreate(TaskBird,(const portCHAR *)"Display Bird",128,NULL,2,NULL);
    xTaskCreate(TaskButton,(const portCHAR *)"Read Button",128,NULL,3,NULL);
    xTaskCreate(TaskScore,(const portCHAR *)"Display Score",128,NULL,1,NULL);
    
    vTaskStartScheduler();
  }
}

void loop(void){
}

void TaskScore(void *pvParameters) { // This is a task.
  int digit = 0;
  for (;;) {
    digitalWrite(decoder0, LOW);
    digitalWrite(decoder1, LOW);
    bcd(score%10);
    delay(1);
    digitalWrite(decoder0, HIGH);
    digitalWrite(decoder1, LOW);
    bcd((score%100)/10);
    delay(1);
    digitalWrite(decoder0, LOW);
    digitalWrite(decoder1, HIGH);
    bcd(score/100);
    delay(1);
    
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for 1ms
  }
}

void TaskButton(void *pvParameters) { // This is a task.
  for (;;) {
    ButtonState = debounceButton(ButtonState, inpButton);
    if(ButtonState == HIGH) {
      xSemaphoreTake(xMutex, portMAX_DELAY);
      {
        dmd.drawLine( x1, y1, x2, y1, GRAPHICS_INVERSE);
        dmd.drawLine( x3, y2, x3, y3, GRAPHICS_INVERSE);
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
      dmd.drawLine( x1, y1, x2, y1, GRAPHICS_INVERSE);
      dmd.drawLine( x3, y2, x3, y3, GRAPHICS_INVERSE);
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
  
  for (;;) {
    // hilangkan wall untuk nanti dimunculkan lagi pada loop berikutnya
    dmd.clearScreen( true );
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
//    if(xWall1 = 30) {
//      score++;
//    } 
//    else if(xWall2 = 30) {
//      score++;
//    }
//    else if(xWall3 = 30) {
//      score++;
//    }
//    else if(xWall4 = 30) {
//      score++;
//    }
     
    if(xWall1 >= 32) {
      xWall1 = 0;
      tinggibwh1 = random(1, 10);
    }
    if(xWall2 >= 32) {
      xWall2 = 0;
      tinggibwh2 = random(1, 10);
    }
    if(xWall3 >= 32) {
      xWall3 = 0;
      tinggibwh3 = random(1, 10);
    }
    if(xWall4 >= 32) {
      xWall4 = 0;
      tinggibwh4 = random(1, 10);
    }
    vTaskDelay( 250 / portTICK_PERIOD_MS ); // wait for 250ms
  }
}

void bcd(byte bil)  {
  byte D3, D2, D1, D0, A, B, C, D, E, F, G;
  
  D3 = (bil & 0b00001000);
  D2 = (bil & 0b00000100);
  D1 = (bil & 0b00000010);
  D0 = (bil & 0b00000001);

  A = (D3 || !D2 || D1 || D0) && (D3 || D2 || D1 || !D0); 
  B = (D3 || !D2 || D1 || !D0) && (D3 || !D2 || !D1 || D0); 
  C = D3 || D2 || !D1 || D0; 
  D = (D3 || !D2 || D1 || D0) && (D3 || D2 || D1 || !D0) && (D3 || !D2 || !D1 || !D0); 
  E = (!D2 && !D1 && !D0) || (D1 && !D0); 
  F = (D3 || D2 || !D0) && (D3 || D2 || !D1) && (D3 || !D1 || !D0); 
  G = (D3 || D2 || D1) && (D3 || !D2 || !D1 || !D0); 

  digitalWrite(segment_a, !A);
  digitalWrite(segment_b, !B);
  digitalWrite(segment_c, !C);
  digitalWrite(segment_d, !D);
  digitalWrite(segment_e, !E);
  digitalWrite(segment_f, !F);
  digitalWrite(segment_g, !G);
}

int debounceButton(int stateButton, int pin) {
  int stateNow = digitalRead(pin);
  if(stateButton != stateNow) {
    delay(100);
    stateNow = digitalRead(pin);
  }
  return stateNow;
}
