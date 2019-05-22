#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <DS1302.h>

#include "ps_2.h"

// initialize Sainsmart 1.8" TFT screen
// (connect pins accordingly or change these values)
#define TFT_DC            8     // Sainsmart RS/DC
#define TFT_RST           7     // Sainsmart RES
#define TFT_CS           6     // Sainsmart CS
// initialize screen with pins
static Adafruit_ST7735 TFT = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
// instead of using TFT.width() and TFT.height() set constant values
// (we can change the size of the game easily that way)
#define TFTW            128     // screen width
#define TFTH            128     // screen height
#define TFTW2            64     // half screen width
#define TFTH2            64     // half screen height

// background
const unsigned int BCKGRDCOL = TFT.Color565(138,235,244);
// bird
//const unsigned int BIRDCOL = TFT.Color565(255,254,174);
// pipe
//const unsigned int PIPECOL  = TFT.Color565(99,255,78);
// pipe highlight
//const unsigned int PIPEHIGHCOL  = TFT.Color565(250,255,250);
// pipe seam
//const unsigned int PIPESEAMCOL  = TFT.Color565(0,0,0);
// floor
//const unsigned int FLOORCOL = TFT.Color565(246,240,163);
// grass (col2 is the stripe color)
//const unsigned int GRASSCOL  = TFT.Color565(141,225,87);
//const unsigned int GRASSCOL2 = TFT.Color565(156,239,88);
//clk white
//sdi green
#define kCePin   5  // Chip Enable
#define kIoPin   3  // Input/Output
#define kSclkPin 4  // Serial Clock
// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);
//mode
unsigned char mode=0;
unsigned char clockMode=0;
bool first=true;
bool keyFlag=false;
unsigned char symLineNum=0;
unsigned char symKeyLineNum=0;
unsigned char lineNum=3;
unsigned char lineKeyNum=3;
unsigned char sec=0;
unsigned char xPosIn;
unsigned char xPosOut;
unsigned char yPosIn;
unsigned char yPosOut;
bool keyb=false;
unsigned char keySymNum;
unsigned char keyByte;
#define paddingRigth 2
#define pinButtonMode A5
#define pinButtonSet A4
#define pinButtonSelect A3
#define pinLedKeyBoard A2

// ---------------
// initial setup
// ---------------
void setup()
{
  pinMode(9,OUTPUT);
  digitalWrite(9,HIGH);
  
  pinMode(pinLedKeyBoard,OUTPUT);
  pinMode(pinButtonMode,INPUT);
  pinMode(pinButtonSelect,INPUT);
  // initialize a ST7735S chip, black tab
  TFT.initR(INITR_BLACKTAB);
  Serial.begin(9600);
  TFT.setRotation(TFT.getRotation()-1);
  rtc.writeProtect(false);
  rtc.halt(false);
  
  ps2_init();
  start();
}

// ---------------
// main loop
// ---------------
void loop()
{
  while(1){
    if(mode==0){uart();}
    if(mode==1){ds1302();}
    if(mode==2){ps2();}
    if(digitalRead(pinButtonMode)){
      for(unsigned char i=0;i<60;i++) delay(5);
      if(digitalRead(pinButtonMode)){
        clearScreen();
        first=true;
        mode++;
        if(mode>2) mode=0;
        clockMode=0;
        keyFlag=false;
      }
    }
  }
}

void clearScreen()
{
  TFT.fillScreen(BCKGRDCOL);
  TFT.setTextColor(ST7735_BLACK);
  TFT.setCursor(paddingRigth,3);
}

// ---------------
// start
// ---------------
void start()
{
  Serial.println("Starting...");
  TFT.fillScreen(ST7735_BLACK);
  TFT.fillRect(10, TFTH2 - 20, TFTW-20, 1, ST7735_WHITE);
  TFT.setTextColor(ST7735_WHITE);
  TFT.setTextSize(3);
  // half width - num char * char width in pixels
  TFT.setCursor( TFTW2-(6*9), TFTH2 - 16);
  TFT.println("--MY--");
  TFT.setTextSize(3);
  TFT.setCursor( TFTW2-(6*9), TFTH2 + 8);
  TFT.println("-UART-");
  TFT.setTextSize(0);
  TFT.setCursor( 10, TFTH2 - 28);
  TFT.println("ATMEGA328");
  TFT.setCursor( TFTW2 - (12*3) - 1, TFTH2 + 34);
  TFT.println("press button");
  while(1){
    if(digitalRead(pinButtonMode)) break;
  }
  clearScreen();
  first=true;
}

void uart()
{
  if(first){
    symLineNum=0;
    lineNum=3;
    lineKeyNum=100;
    symKeyLineNum=0;
    TFT.fillRect(paddingRigth,lineNum,TFTW-5,TFTH2+25,BCKGRDCOL);
    TFT.fillRect(paddingRigth,lineKeyNum,TFTW-5,30,BCKGRDCOL);
    TFT.setCursor(paddingRigth,lineNum);
    TFT.drawLine(10, TFTH2 + 32, TFTW-20, TFTH - 32, ST7735_BLACK);
    first=false;
    keyb=false;
    xPosIn=paddingRigth;
    xPosOut=paddingRigth;
    yPosIn=lineNum;
    yPosOut=100;
    keySymNum=0;
    keyByte=0;
  }
  while(Serial.available()){
    unsigned char sym=Serial.read();
    if(lineNum>=90){
      lineNum=3;
      TFT.fillRect(paddingRigth,lineNum,TFTW-5,TFTH2+25,BCKGRDCOL);
      TFT.setCursor(paddingRigth,lineNum);
    }
    if(keyb){
      xPosOut=TFT.getCursorX();
      yPosOut=TFT.getCursorY();
      TFT.setCursor(xPosIn,yPosIn);
      keyb=false;
    }
    
    if(sym<16) TFT.print("0");
    TFT.print(sym,HEX);
    if(symLineNum%2){TFT.print(" ");}
    
    symLineNum++;
    if(symLineNum==8){
      TFT.println("");
      symLineNum=0;
      lineNum+=10;
      TFT.setCursor(paddingRigth,lineNum);
    }
  }

  if(digitalRead(pinButtonSelect)){
      for(unsigned char i=0;i<50;i++) delay(3);
      if(digitalRead(pinButtonSelect)) first=true;
   }
   if(digitalRead(pinButtonSet)){
      for(unsigned char i=0;i<50;i++) delay(3);
      if(digitalRead(pinButtonSet)) first=true;
   }

  unsigned char keySym;
  keySym=keyboard_data;
  keyboard_data=0x00;
  if(keySym==0x00){
    return;
  }else{
    digitalWrite(pinLedKeyBoard,!digitalRead(pinLedKeyBoard));
  }

  if(keySym==0xF0){
    keyFlag=true;
    return;
  }
  if(keyFlag){
    keyFlag=false;
  }else{
    return;
  }
  if(!keyb){
      xPosIn=TFT.getCursorX();
      yPosIn=TFT.getCursorY();
      TFT.setCursor(xPosOut,yPosOut);
      keyb=true;
  }
  
  if(lineKeyNum>=130){
    lineKeyNum=100;
    TFT.fillRect(paddingRigth,lineKeyNum,TFTW-5,30,BCKGRDCOL);
    TFT.setCursor(paddingRigth,lineKeyNum);
  }

  switch(keySym){
    case 0x16: keySym=0x01; break;
    case 0x1E: keySym=0x02; break;
    case 0x26: keySym=0x03; break;
    case 0x25: keySym=0x04; break;
    case 0x2E: keySym=0x05; break;
    case 0x36: keySym=0x06; break;
    case 0x3D: keySym=0x07; break;
    case 0x3E: keySym=0x08; break;
    case 0x46: keySym=0x09; break;
    case 0x45: keySym=0x00; break;

    case 0x69: keySym=0x01; break;
    case 0x72: keySym=0x02; break;
    case 0x7A: keySym=0x03; break;
    case 0x6B: keySym=0x04; break;
    case 0x73: keySym=0x05; break;
    case 0x74: keySym=0x06; break;
    case 0x6C: keySym=0x07; break;
    case 0x75: keySym=0x08; break;
    case 0x7D: keySym=0x09; break;
    case 0x70: keySym=0x00; break;

    case 0x1C: keySym=0x0A; break;
    case 0x32: keySym=0x0B; break;
    case 0x21: keySym=0x0C; break;
    case 0x23: keySym=0x0D; break;
    case 0x24: keySym=0x0E; break;
    case 0x2B: keySym=0x0F; break;
    default: return;
  }

  if(keySymNum>=2){
    keySymNum=0;
    keyByte=0;
  }
  if(keySymNum<2){
    keyByte*=16;
    keyByte+=keySym;
    keySymNum++;
    if(keySymNum==1) return;
  }

  if(keyByte<16) TFT.print("0");
  Serial.write(keyByte);
  TFT.print(keyByte,HEX);
  if(symKeyLineNum%2){TFT.print(" ");}
  
  symKeyLineNum++;
  if(symKeyLineNum==8){
    TFT.println("");
    symKeyLineNum=0;
    lineKeyNum+=10;
    TFT.setCursor(paddingRigth,lineKeyNum);
  }
}

void ds1302()
{
  if(first){
    TFT.println("RTC DS1302");
    first=false;
  }
  
  Time t = rtc.time();
  if(sec!=t.sec){
    TFT.fillRect(paddingRigth,10,TFTW-10,50,BCKGRDCOL);
    sec=t.sec;
  }
  
  TFT.setCursor(paddingRigth+10,20);
  
  if(clockMode==0){TFT.print("");}
  if(clockMode==1){TFT.print("_");}
  if(clockMode==2){TFT.print("  __");}
  if(clockMode==3){TFT.print("     __");}
  if(clockMode==4){TFT.print("        ____");}
  
  
  char buf[14];
  snprintf(buf,sizeof(buf),"%d %02d.%02d.%04d",t.day,t.date,t.mon,t.yr);
  TFT.setCursor(paddingRigth+10,30);
  TFT.print(buf);

  TFT.setCursor(paddingRigth+10,40);
  if(clockMode==5){TFT.print("  __");}
  if(clockMode==6){TFT.print("     __");}
  
  TFT.setCursor(paddingRigth+10,50);
  snprintf(buf,sizeof(buf),"  %02d:%02d:%02d",t.hr,t.min,t.sec);
  TFT.print(buf);
  
  
  if(digitalRead(pinButtonSelect)){
      for(unsigned char i=0;i<50;i++) delay(3);
      if(digitalRead(pinButtonSelect)){
        clockMode++;
        if(clockMode>6) clockMode=0;
        TFT.fillRect(paddingRigth,10,TFTW-10,50,BCKGRDCOL);
      }
   }
   if(digitalRead(pinButtonSet)){
      for(unsigned char i=0;i<50;i++) delay(3);
      if(digitalRead(pinButtonSet)){
        unsigned char num;
        if(clockMode==1){num=t.day;if(num<7){num++;}else{num=1;}t.day=(Time::Day)num;}
        if(clockMode==2){num=t.date;if(num<31){num++;}else{num=1;}t.date=num;}
        if(clockMode==3){num=t.mon;if(num<12){num++;}else{num=1;}t.mon=num;}
        if(clockMode==4){int i=t.yr;if(i<2030){i++;}else{i=2000;}t.yr=i;}
        if(clockMode==5){num=t.hr;if(num<23){num++;}else{num=0;}t.hr=num;}
        if(clockMode==6){num=t.min;if(num<59){num++;}else{num=0;}t.min=num;}

        if(clockMode>0){t.sec=0;rtc.time(t);}
        TFT.fillRect(paddingRigth,10,TFTW-10,50,BCKGRDCOL);
      }
   }
   delay(500);
}

void ps2()
{
  if(first){
    TFT.println("PS/2");
    symLineNum=0;
    lineNum=10;
    TFT.fillRect(paddingRigth,lineNum,TFTW-5,TFTH-10,BCKGRDCOL);
    TFT.setCursor(paddingRigth,lineNum);
    first=false;
  }
  unsigned char sym;
  sym=keyboard_data;
  keyboard_data=0x00;
  if(sym==0x00){
    return;
  }else{
    digitalWrite(pinLedKeyBoard,!digitalRead(pinLedKeyBoard));
  }
  if(sym==0xF0){
    keyFlag=true;
    return;
  }
  if(keyFlag){
    keyFlag=false;
  }else{
    return;
  }
  
  if(lineNum>=130){
    lineNum=10;
    TFT.fillRect(paddingRigth,lineNum,TFTW-5,TFTH-10,BCKGRDCOL);
    TFT.setCursor(paddingRigth,lineNum);
  }

  if(sym<16) TFT.print("0");
  TFT.print(sym,HEX);
  if(symLineNum%2){TFT.print(" ");}
  
  symLineNum++;
  if(symLineNum==8){
    TFT.println("");
    symLineNum=0;
    lineNum+=10;
    TFT.setCursor(paddingRigth,lineNum);
  }
}
