#include <SPI.h>
#include <MFRC522.h>
#include "define.h"
#include "myProto.h" 

#define RST_PIN   9
#define SS_PIN    10
MFRC522 mfrc522(SS_PIN, RST_PIN);

uint8_t readCounter = 0;
uint8_t blockAddr = 0;
byte resp;
MFRC522::MIFARE_Key key;
byte buff[18];
byte sz = sizeof(buff);
byte strBuff[32];

// Number of known default keys (hard-coded)
// NOTE: Synchronize the NR_KNOWN_KEYS define with the defaultKeys[] array
#define NR_KNOWN_KEYS   8
// Known keys, see: https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys
byte knownKeys[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] =  {
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF = factory default
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
    {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
    {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
    {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
    {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
};

void setup() {
	Serial.begin(9600);   // Initialize serial communications with the PC
	while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();      // Init SPI bus
	mfrc522.PCD_Init(); // Init MFRC522
	myInit();
  for (byte i = 0; i < MFRC522::MF_KEY_SIZE; ++i) key.keyByte[i] = 0xFF;
  GREEN_LED_ON;
}

void loop() {
  if( recvPkt.forProcessing ){
    GREEN_LED_OFF;
    RED_LED_OFF;
    if( recvPkt.cmd >= CMD_READ_UID_AND_TYPE){
      ORANGE_LED_ON;
      // усли пришла комманда на работу с картой, то даем 3 секудны, чтоб ее приложить к считывателю
      readCounter = 0;
      bool findFlag = false;
      while( readCounter < 30 ){
        if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ){
          findFlag = true;
          break;
        }else{
          readCounter++;
          delay(100);
        }
      }
      if( !findFlag ){
        resp = sprintf( strBuff, "Card not found\n" );
        MyProto_send(CMD_ERROR,strBuff,resp);
        recvPkt.cmd = 0;
        RED_LED_ON;
      }else{
        GREEN_LED_ON;  
      }
      ORANGE_LED_OFF;
    }
    switch ( recvPkt.cmd ){
      case CMD_SET_ADDRESS: setAddress(); break;
      case CMD_READ_UID_AND_TYPE:
        readUID();
        delay(100);
        readType();
      break;
      case CMD_READ_VERSION: readVersion(); break;
      case CMD_READ_UID: readUID(); break;
      case CMD_READ_BLOCK:
        readType();
        resp = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, blockAddr, &key, &(mfrc522.uid));
        if ( resp == MFRC522::STATUS_OK ) {
          resp = mfrc522.MIFARE_Read(blockAddr, buff, &sz);
          if ( resp == MFRC522::STATUS_OK ) {
            switch( mfrc522.PICC_GetType(mfrc522.uid.sak) ){
              case 0x04: MyProto_send(CMD_READ_BLOCK, buff, 16); break;
              default: MyProto_send(CMD_READ_BLOCK, buff, sz); break;
            }
          }else{
            resp = sprintf( strBuff, "MIFARE_Read failed\n" );
            //resp = sprintf( strBuff, "MIFARE_Read failed: %s\n", getStatusCodeName(resp) );
            MyProto_send(CMD_ERROR,strBuff,resp);
          }
        }else{
          resp = sprintf( strBuff, "PCD_Auth failed\n" );
          //resp = sprintf( strBuff, "PCD_Auth failed: %s\n", getStatusCodeName(resp) );
          MyProto_send(CMD_ERROR,strBuff,resp);
        }
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
      break;
      case CMD_READ_PICC_TYPE: readType(); break;
      case CMD_WRITE_UID:
		mfrc522.MIFARE_UnbrickUidSector(false);
        if ( mfrc522.MIFARE_SetUid(recvPkt.data, (byte)4, true) ) {
          resp = sprintf( strBuff, "MIFARE_Write UID completed\n" );
          MyProto_send(CMD_SUCCESS,strBuff,resp);
        }else{
          resp = sprintf( strBuff, "MIFARE_Write UID failed!\n" );
          MyProto_send(CMD_ERROR,strBuff,resp);
        }
        mfrc522.PICC_HaltA();
        if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ){
			//mfrc522.PCD_Init();
			break;
		}
        /*
        Serial.println(">: Processing CMD_WRITE_UID");
        if ( mfrc522.MIFARE_SetUid(recvPkt.data, (byte)recvPkt.len, true) ) Serial.println(F("Wrote new UID to card."));
        mfrc522.PICC_HaltA();
        if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) break;
        */
      break;
      case CMD_WRITE_BLOCK:
        resp = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, blockAddr, &key, &(mfrc522.uid));
        if ( resp == MFRC522::STATUS_OK ) {
          resp = mfrc522.MIFARE_Write(blockAddr, recvPkt.data, recvPkt.len);
          if ( resp == MFRC522::STATUS_OK ) {
            resp = sprintf( strBuff, "MIFARE_Write completed\n" );
            MyProto_send(CMD_SUCCESS,strBuff,resp);
          }else{
            resp = sprintf( strBuff, "MIFARE_Write failed! [%d]\n", resp );
            MyProto_send(CMD_ERROR,strBuff,resp);
          }
        }else{
          resp = sprintf( strBuff, "PCD_Auth failed! [%d]\n", resp );
          MyProto_send(CMD_ERROR,strBuff,resp);
        }
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
      break;
      case CMD_DUMP_DATA: dumpData(); break;
    }
    recvPkt.forProcessing = 0;
  }
}

void readVersion()
{
  uint8_t MiniBuff[1];
  MiniBuff[0] = (uint8_t) mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  MyProto_send(CMD_READ_VERSION, MiniBuff, 1);
  delay(100);
  resp = sprintf( strBuff, "MIFARE_Read OK\n" );
  MyProto_send(CMD_SUCCESS,strBuff,resp);
}

void readUID()
{
  MyProto_send(CMD_READ_UID, mfrc522.uid.uidByte, mfrc522.uid.size);
  delay(100);
  resp = sprintf( strBuff, "MIFARE_Read OK\n" );
  MyProto_send(CMD_SUCCESS,strBuff,resp);
}

void readType()
{
  uint8_t MiniBuff[1];
  MiniBuff[0] = (uint8_t) mfrc522.PICC_GetType(mfrc522.uid.sak);
  MyProto_send(CMD_READ_PICC_TYPE, MiniBuff, 1);
  /*
  // Try the known default keys
  MFRC522::MIFARE_Key key;
  for (byte k = 0; k < NR_KNOWN_KEYS; k++) {
      // Copy the known key into the MIFARE_Key structure
      for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
          key.keyByte[i] = knownKeys[k][i];
      }
      // Try the key
      if (try_key(&key)) {
          // Found and reported on the key and block,
          // no need to try other keys for this PICC
          break;
      }
  }
  */
}

void setAddress()
{
  if( recvPkt.len == 1 ) blockAddr = recvPkt.data[0];
  uint8_t MiniBuff[1];
  MiniBuff[0] = blockAddr;
  MyProto_send(CMD_SET_ADDRESS, MiniBuff, 1);
  delay(100);
  resp = sprintf( strBuff, "MIFARE_Set address OK\n" );
  MyProto_send(CMD_SUCCESS,strBuff,resp);
}

void dumpData()
{
  uint8_t blocks;
  switch( mfrc522.PICC_GetType(mfrc522.uid.sak) ){
    case 0x03: blocks = 19; break;
    case 0x04: blocks = 63; break;
    case 0x05: blocks = 255; break;
    default: blocks = 0; break;
  }  
  for( uint8_t blockAddr = 0; blockAddr <= blocks; blockAddr++ ){
    resp = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, blockAddr, &key, &(mfrc522.uid));
    if ( resp == MFRC522::STATUS_OK ) {
      resp = mfrc522.MIFARE_Read(blockAddr, buff, &sz);
      if ( resp == MFRC522::STATUS_OK ) {
        buff[16] = blockAddr;
        MyProto_send(CMD_DUMP_DATA,buff,17);
      }else{
        resp = sprintf( strBuff, "MIFARE_Read failed! [%d]", resp );
        buff[++resp] = blockAddr;
        MyProto_send(CMD_DUMP_DATA,strBuff,resp);
      }
    }else{
      resp = sprintf( strBuff,"PCD_Auth failed! [%d]", resp );
      buff[++resp] = blockAddr;
      MyProto_send(CMD_DUMP_DATA,strBuff,resp);
    }
    delay(100);
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  resp = sprintf( strBuff, "Dump data completed\n" );
  MyProto_send(CMD_SUCCESS,strBuff,resp);
}

void serialEvent() {
	static uint8_t counter = 0;
	static uint8_t uartBuff[37];

	while (Serial.available()) {
		uint8_t sym = (uint8_t)Serial.read();
		uartBuff[counter++] = sym;
		if( MyProto_parsData( recvPkt, uartBuff, counter  ) || counter == sizeof(uartBuff) ) counter = 0;
    //for(uint8_t i = 0; i< counter; i++){
    //Serial.print(buff[i]);
    //}
    //Serial.print(pkt.forProcessing);
    //Serial.print("  ");
    //Serial.println(sym,HEX);
	}
}

/*
const char* getStatusCodeName(byte code)
{
  switch (code) {
    case 1: return "Success."; break;
    case 2: return "Error in communication."; break;
    case 3: return "Collission detected."; break;
    case 4: return "Timeout in communication."; break;
    case 5: return "A buffer is not big enough."; break;
    case 6: return "Internal error in the code. Should not happen."; break;
    case 7: return "Invalid argument."; break;
    case 8: return "The CRC_A does not match."; break;
    case 9: return "A MIFARE PICC responded with NAK."; break;
    default: return "Unknown error"; break;
  }
}
*/
