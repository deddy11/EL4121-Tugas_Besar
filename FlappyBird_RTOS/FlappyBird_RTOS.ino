#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <avr/io.h>
#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "Arial14.h"

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
int x1=6, x2=8, x3=7;
int y1=8, y2=7, y3=9;
int score = 0;
boolean isStop1 = false;
boolean isStop2 = false;
int xWall1, xWall2, xWall3, xWall4; // variable for scrolling the wall
int tinggibwh1, tinggibwh2, tinggibwh3, tinggibwh4; // the height of the low part of the wall
SemaphoreHandle_t xMutex;

// define tasks
void TaskWall( void *pvParameters );
void TaskBird( void *pvParameters );
void TaskButton( void *pvParameters );
void TaskScore( void *pvParameters );

// define task handle
//TaskHandle_t Handle_Main;
TaskHandle_t Handle_Wall;
TaskHandle_t Handle_Bird;
TaskHandle_t Handle_Button;
TaskHandle_t Handle_Score;

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
  dmd.clearScreen( true );  //clear/init the DMD pixels held in RAM
  Serial.begin(9600); // Serial print to check score
//  xTaskCreate(TaskMain,(const portCHAR *)"Display Main Menu",128,NULL,3,&Handle_Main);
  
  xMutex = xSemaphoreCreateMutex();
  if(xMutex != NULL) {
    xTaskCreate(TaskButton,(const portCHAR *)"Read Button",128,NULL,3,&Handle_Button); 
    xTaskCreate(TaskBird,(const portCHAR *)"Display Bird",128,NULL,2,&Handle_Bird);     
    xTaskCreate(TaskWall,(const portCHAR *)"Display Wall",128,NULL,2,&Handle_Wall);
    xTaskCreate(TaskScore,(const portCHAR *)"Display Score",128,NULL,1,&Handle_Score);
    vTaskStartScheduler();
  }
}

void loop(void){
}

void TaskMain(void *pvParameters) { // This is a task.
  vTaskSuspend(Handle_Score);
  vTaskSuspend(Handle_Wall);
  int x = 30, y = 5;
  dmd.clearScreen( true );  //clear/init the DMD pixels held in RAM
  dmd.selectFont(Arial_14);
  dmd.drawChar(  1,  2, 'P', GRAPHICS_NORMAL );
  dmd.drawChar(  8,  2, 'L', GRAPHICS_NORMAL );
  dmd.drawChar( 15,  2, 'A', GRAPHICS_NORMAL );
  dmd.drawChar( 22,  2, 'Y', GRAPHICS_NORMAL );
  dmd.writePixel(22, 9, GRAPHICS_NORMAL, 0x01);
  dmd.writePixel(22, 10, GRAPHICS_NORMAL, 0x01);
  dmd.writePixel(23, 11, GRAPHICS_NORMAL, 0x01);
  dmd.writePixel(23, 12, GRAPHICS_NORMAL, 0x01);
  for (;;) {
    Serial.println("Main is running");
    ButtonState = debounceButton(ButtonState, inpButton);
    if(ButtonState == HIGH) {
      vTaskResume(Handle_Score);
      vTaskResume(Handle_Wall);
    }
    vTaskDelay( 50 / portTICK_PERIOD_MS ); // wait for 50ms     
  }
}

void TaskScore(void *pvParameters) { // This is a task.
//  vTaskDelete(Handle_Main);
  int digit = 0;
  for (;;) {
//    Serial.println("Score is running");
    vTaskSuspendAll();
    {
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
    }
    xTaskResumeAll();
  }
}

void TaskButton(void *pvParameters) { // This is a task.
  for (;;) {
//    Serial.println("Button is running");
    ButtonState = debounceButton(ButtonState, inpButton);
    if(ButtonState == HIGH) {
      xSemaphoreTake(xMutex, portMAX_DELAY);
      {
        dmd.drawLine( x1, y1, x2, y1, GRAPHICS_INVERSE);
        dmd.drawLine( x3, y2, x3, y3, GRAPHICS_INVERSE);
        if(isStop1 == true) {
          y1=8; y2=7; y3=9;
          score = 0;
          isStop1 = false;
        }
        y1 = y1 - 1;
        y2 = y2 - 1;
        y3 = y3 - 1;
        dmd.drawLine( x1, y1, x2, y1, GRAPHICS_NORMAL);
        dmd.drawLine( x3, y2, x3, y3, GRAPHICS_NORMAL);

        if((xWall1<9)&&(xWall1>5))  {
          if(y2<=tinggibwh1) {
            isStop1 = true;
            isStop2 = true;
          }
        }
        else if((xWall2<9)&&(xWall2>5))  {
          if(y2<=tinggibwh2) {
            isStop1 = true;
            isStop2 = true;
          }
        }
        else if((xWall3<9)&&(xWall3>5))  {
          if(y2<=tinggibwh3) {
            isStop1 = true;
            isStop2 = true;
          }
        }
        else if((xWall4<9)&&(xWall4>5))  {
          if(y2<=tinggibwh4) {
            isStop1 = true;
            isStop2 = true;
          }
        }
        
        if(y2 < 0) {
            isStop1 = true;
            isStop2 = true;
        }
      }
      xSemaphoreGive(xMutex);
    }
    vTaskDelay( 50 / portTICK_PERIOD_MS ); // wait for 50ms
  }
}

void TaskBird(void *pvParameters) { // This is a task.
  for (;;) {    
//    Serial.println("Bird is running");
    xSemaphoreTake(xMutex, portMAX_DELAY);
    {
      dmd.drawLine( x1, y1, x2, y1, GRAPHICS_INVERSE);
      dmd.drawLine( x3, y2, x3, y3, GRAPHICS_INVERSE);
      if(isStop1 == true) {
        y1=8; y2=7; y3=9;
        score = 0;
        isStop1 = false;
      }
      y1 = y1 + 1;
      y2 = y2 + 1;
      y3 = y3 + 1;
      dmd.drawLine( x1, y1, x2, y1, GRAPHICS_NORMAL);
      dmd.drawLine( x3, y2, x3, y3, GRAPHICS_NORMAL);

      if((xWall1<9)&&(xWall1>5))  {
          if(y3>(tinggibwh1+6)){
            isStop1 = true;
            isStop2 = true;
          }
        }
        else if((xWall2<9)&&(xWall2>5))  {
          if(y3>(tinggibwh2+6)){
            isStop1 = true;
            isStop2 = true;
          }
        }
        else if((xWall3<9)&&(xWall3>5))  {
          if(y3>(tinggibwh3+6)){
            isStop1 = true;
            isStop2 = true;
          }
        }
        else if((xWall4<9)&&(xWall4>5))  {
          if(y3>(tinggibwh4+6)){
            isStop1 = true;
            isStop2 = true;
          }
        }
        
        if(y3 > 15) {
          isStop1 = true;
          isStop2 = true;
        }
    }
    xSemaphoreGive(xMutex); 
    vTaskDelay( 250 / portTICK_PERIOD_MS ); // wait for 250ms
  }
}

void TaskWall(void *pvParameters) { // This is a task.
  int lebar = 2; // lebar wall 3 pixel
  int tinggiCelah = 7; // tinggi total wall

  // inisiasi tinggi awal dan posisi awal dari setiap wall
  tinggibwh1 = random(2, 7);
  tinggibwh2 = random(2, 7);
  tinggibwh3 = random(2, 7);
  tinggibwh4 = random(2, 7);
  xWall1 = 33;
  xWall2 = xWall1+9;
  xWall3 = xWall1+18;
  xWall4 = xWall1+27;
  
  for (;;) {
//    Serial.println("Wall is running");
    xSemaphoreTake(xMutex, portMAX_DELAY);
    {
      if(isStop2 == true) {
         // hilangkan wall untuk nanti dimunculkan lagi pada loop berikutnya
        dmd.drawFilledBox(xWall1,0,xWall1+lebar,tinggibwh1,GRAPHICS_INVERSE);
        dmd.drawFilledBox(xWall1,(tinggibwh1 + tinggiCelah),xWall1+lebar,15,GRAPHICS_INVERSE);
        // gambar wall kedua
        dmd.drawFilledBox(xWall2,0,xWall2+lebar,tinggibwh2,GRAPHICS_INVERSE);
        dmd.drawFilledBox(xWall2,(tinggibwh2 + tinggiCelah),xWall2+lebar,15,GRAPHICS_INVERSE);
        // gambar wall ketiga
        dmd.drawFilledBox(xWall3,0,xWall3+lebar,tinggibwh3,GRAPHICS_INVERSE);
        dmd.drawFilledBox(xWall3,(tinggibwh3 + tinggiCelah),xWall3+lebar,15,GRAPHICS_INVERSE);
        // gambar wall keempat
        dmd.drawFilledBox(xWall4,0,xWall4+lebar,tinggibwh4,GRAPHICS_INVERSE);
        dmd.drawFilledBox(xWall4,(tinggibwh4 + tinggiCelah),xWall4+lebar,15,GRAPHICS_INVERSE);
        dmd.drawFilledBox(0,0,0,15,GRAPHICS_INVERSE);
        
        xWall1 = 33;
        xWall2 = xWall1+9;
        xWall3 = xWall1+18;
        xWall4 = xWall1+27;
        isStop2 = false;
      }
      // hilangkan wall untuk nanti dimunculkan lagi pada loop berikutnya
      dmd.drawFilledBox(xWall1,0,xWall1+lebar,tinggibwh1,GRAPHICS_INVERSE);
      dmd.drawFilledBox(xWall1,(tinggibwh1 + tinggiCelah),xWall1+lebar,15,GRAPHICS_INVERSE);
      // gambar wall kedua
      dmd.drawFilledBox(xWall2,0,xWall2+lebar,tinggibwh2,GRAPHICS_INVERSE);
      dmd.drawFilledBox(xWall2,(tinggibwh2 + tinggiCelah),xWall2+lebar,15,GRAPHICS_INVERSE);
      // gambar wall ketiga
      dmd.drawFilledBox(xWall3,0,xWall3+lebar,tinggibwh3,GRAPHICS_INVERSE);
      dmd.drawFilledBox(xWall3,(tinggibwh3 + tinggiCelah),xWall3+lebar,15,GRAPHICS_INVERSE);
      // gambar wall keempat
      dmd.drawFilledBox(xWall4,0,xWall4+lebar,tinggibwh4,GRAPHICS_INVERSE);
      dmd.drawFilledBox(xWall4,(tinggibwh4 + tinggiCelah),xWall4+lebar,15,GRAPHICS_INVERSE);

      xWall1--;
      xWall2--;
      xWall3--;
      xWall4--;
      
      // gambar wall pertama
      dmd.drawFilledBox(xWall1,0,xWall1+lebar,tinggibwh1,GRAPHICS_NORMAL);
      dmd.drawFilledBox(xWall1,(tinggibwh1 + tinggiCelah),xWall1+lebar,15,GRAPHICS_NORMAL);
      // gambar wall kedua
      dmd.drawFilledBox(xWall2,0,xWall2+lebar,tinggibwh2,GRAPHICS_NORMAL);
      dmd.drawFilledBox(xWall2,(tinggibwh2 + tinggiCelah),xWall2+lebar,15,GRAPHICS_NORMAL);
      // gambar wall ketiga
      dmd.drawFilledBox(xWall3,0,xWall3+lebar,tinggibwh3,GRAPHICS_NORMAL);
      dmd.drawFilledBox(xWall3,(tinggibwh3 + tinggiCelah),xWall3+lebar,15,GRAPHICS_NORMAL);
      // gambar wall keempat
      dmd.drawFilledBox(xWall4,0,xWall4+lebar,tinggibwh4,GRAPHICS_NORMAL);
      dmd.drawFilledBox(xWall4,(tinggibwh4 + tinggiCelah),xWall4+lebar,15,GRAPHICS_NORMAL);
      
      // increment masing-masing posisi wall
      
      /* atur kasus agar setiap kali posisi wall mencapai maksimal, maka ia kembali ke posisi 
       * awal x = 0 saat kembali ke posisi x = 0, tinggi dari setiap wall juga diubah dengan 
       * tinggi yang baru setiap kali ada wall yang kembali ke posisi x = 0, skor bertambah 1
       */
      if(xWall1 == 3) {
        score++;
      } 
      else if(xWall2 == 3) {
        score++;
      }
      else if(xWall3 == 3) {
        score++;
      }
      else if(xWall4 == 3) {
        score++;
      }
       
      if(xWall1 <= -3) {
        xWall1 = 33;
        tinggibwh1 = random(2, 7);
      }
      if(xWall2 <= -3) {
        xWall2 = 33;
        tinggibwh2 = random(2, 7);
      }
      if(xWall3 <= -3) {
        xWall3 = 33;
        tinggibwh3 = random(2, 7);
      }
      if(xWall4 <= -3) {
        xWall4 = 33;
        tinggibwh4 = random(2, 7);
      }
    }
    xSemaphoreGive(xMutex); 
    
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
