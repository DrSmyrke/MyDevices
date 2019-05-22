#include <OneWire.h>
#include "sound.h"
#define pinIbutton 8
#define pinLedModeRec A3
#define pinLedModeComp A2
#define pinLedRecOn A0
#define pinLedKey A1
#define pinButtonMode A5
#define pinButtonRec A4

OneWire ibutton (pinIbutton); // Подключаем ключ к 10 пину дуни

byte key[8]; //Буфер, хранящий в себе ид ключа
byte newKey[8];
byte compKey[8];
bool modeRec=false;
bool getKey=false;
bool newKeyFlag=false;

void ledKeyBlink(bool led){
 for(unsigned char i=0;i<10;i++){
  digitalWrite(pinLedKey,!digitalRead(pinLedKey));
  delay(100);
 }
 digitalWrite(pinLedKey,led);
}
void ledRecBlink(bool led){
 for(unsigned char i=0;i<10;i++){
  digitalWrite(pinLedRecOn,!digitalRead(pinLedRecOn));
  delay(100);
 }
 digitalWrite(pinLedRecOn,led);
}

void setup(){
 Serial.begin(9600);
 Serial.print("Init ... ");
 pinMode(pinLedModeRec,OUTPUT);
 pinMode(pinLedModeComp,OUTPUT);
 pinMode(pinLedRecOn,OUTPUT);
 pinMode(pinLedKey,OUTPUT);
 pinMode(pinButtonMode,INPUT);
 pinMode(pinButtonRec,INPUT);
 //testing
 digitalWrite(pinLedModeRec,!digitalRead(pinLedModeRec));delay(250);
 digitalWrite(pinLedModeRec,!digitalRead(pinLedModeRec));delay(250);
 digitalWrite(pinLedModeComp,!digitalRead(pinLedModeComp));delay(250);
 digitalWrite(pinLedModeComp,!digitalRead(pinLedModeComp));delay(250);
 ledKeyBlink(false);
 ledRecBlink(false);
 play(0);delay(1000);play(1);
 //end testing
 Serial.println("OK");
 Serial.println("=== help page ==========");
 Serial.println("m - change mode");
 Serial.println("r - reset key");
 Serial.println("k - new key");
 Serial.println("========================");
}

void loop(){
  //uart
  if(Serial.read()=='m') modeRec=!modeRec;
  if(Serial.read()=='r') getKey=false;
  if(Serial.read()=='k') newKeyFlag=true;
  if(newKeyFlag){
    bool first=true;
    for(unsigned char i=0;i<16;i++){
      unsigned char inChar = Serial.read();
      if(inChar>=48 and inChar<=57) inChar-=48;
      if(first){key[i/2]=inChar;}else{
        key[i/2]*=10;
        key[i/2]+=inChar;
      }
      first=!first;
    }
    ledKeyBlink(true);
    getKey=true;
    if(!modeRec){
      for(byte x = 0; x<8; x++){compKey[x]=key[x];}
    }else{
      for(byte x = 0; x<8; x++){newKey[x]=key[x];}
    }
    Serial.print("Key ");
    for (byte x = 0; x<8; x++){  
      Serial.print(key[x],HEX);
      Serial.print(" ");
    }
    Serial.println(" accepted");
  }
  //leds
  if(modeRec){
    digitalWrite(pinLedModeRec,HIGH);
    digitalWrite(pinLedModeComp,LOW);
  }else{
    digitalWrite(pinLedModeRec,LOW);
    digitalWrite(pinLedModeComp,HIGH);
  }
  if(getKey){digitalWrite(pinLedKey,HIGH);}else{digitalWrite(pinLedKey,LOW);}
  //buttons
  if(digitalRead(pinButtonMode)){
    for(unsigned char i=0;i<100;i++){delay(5);}
    if(digitalRead(pinButtonMode)) modeRec=!modeRec;
    getKey=false;
    delay(100);
  }
  if(digitalRead(pinButtonRec)){
    for(unsigned char i=0;i<100;i++){delay(5);}
    if(digitalRead(pinButtonRec)) getKey=false;
    delay(100);
  }
  //iButton
  if(!ibutton.search(key)){
    ibutton.reset_search();
    delay(200);
    return;
 }
 ledKeyBlink(true);
 //comp mode
 if(!modeRec){
   if(!getKey){
     getKey=true;
     for(byte x = 0; x<8; x++){compKey[x]=key[x];}
   }else{
     bool res=true;
     for(byte x = 0; x<8; x++){
       if(key[x]!=compKey[x]) res=false;
     }
     if(!res){play(0);}else{play(1);}
   }
 }else{
 //rec mode
  if(!getKey){
    getKey=true;
    for(byte x = 0; x<8; x++){newKey[x]=key[x];}
  }else{
    writeCode();
    ledRecBlink(false);
  }
 }
 Serial.print(">: ");
 for (byte x = 0; x<8; x++){  
   Serial.print(key[x],HEX); //Печатаем содержимое буфера в консоль 
   Serial.print(" ");
 }
 //compute crc//
 byte crc;
 crc = ibutton.crc8(key, 7);
 Serial.print("CRC: ");
 Serial.println(crc,HEX);
} 
void writeCode(){
  ibutton.skip();ibutton.reset();ibutton.write(0x33);
    Serial.print("  ID before write:");
    for (byte x=0; x<8; x++){
      Serial.print(' ');
      Serial.print(ibutton.read(), HEX);
    }
    // send reset
    ibutton.skip();
    ibutton.reset();
    // send 0xD1
    ibutton.write(0xD1);
    // send logical 0
    digitalWrite(pinIbutton, LOW); pinMode(pinIbutton, OUTPUT); delayMicroseconds(60);
    pinMode(pinIbutton, INPUT); digitalWrite(pinIbutton, HIGH); delay(10);
    
    Serial.print('\n');
    Serial.print("  Writing iButton ID:\n    ");
    //byte newID[8] = {0x01, 0x4F, 0x12, 0x08, 0xA0, 0x00, 0x00, 0x79}; //my
    //byte newID[8] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88};
                     
    ibutton.skip();
    ibutton.reset();
    ibutton.write(0xD5);
    for (byte x = 0; x<8; x++){
      writeByte(newKey[x]);
      Serial.print('*');
    }
    Serial.print('\n');
    ibutton.reset();
    // send 0xD1
    ibutton.write(0xD1);
    //send logical 1
    digitalWrite(pinIbutton, LOW); pinMode(pinIbutton, OUTPUT); delayMicroseconds(10);
    pinMode(pinIbutton, INPUT); digitalWrite(pinIbutton, HIGH); delay(10);
}
int writeByte(byte data){
  int data_bit;
  for(data_bit=0; data_bit<8; data_bit++){
    if (data & 1){
      digitalWrite(pinIbutton, LOW); pinMode(pinIbutton, OUTPUT);
      delayMicroseconds(60);
      pinMode(pinIbutton, INPUT); digitalWrite(pinIbutton, HIGH);
      delay(10);
    } else {
      digitalWrite(pinIbutton, LOW); pinMode(pinIbutton, OUTPUT);
      pinMode(pinIbutton, INPUT); digitalWrite(pinIbutton, HIGH);
      delay(10);
    }
    data = data >> 1;
  }
  return 0;
}
