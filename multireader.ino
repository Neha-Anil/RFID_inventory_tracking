/**
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from more than one PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 *
 * Example sketch/program showing how to read data from more than one PICC (that is: a RFID Tag or Card) using a
 * MFRC522 based RFID Reader on the Arduino SPI interface.
 *
 * Warning: This may not work! Multiple devices at one SPI are difficult and cause many trouble!! Engineering skill
 *          and knowledge are required!
 *
 * @license Released into the public domain.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS 1    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required **
 * SPI SS 2    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required **
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 */

#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_1_PIN = 10;   // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
constexpr uint8_t SS_2_PIN = 8;    // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1

constexpr uint8_t NR_OF_READERS = 2;
int led=7;
int led2=6;
byte ssPins[] = {SS_1_PIN, SS_2_PIN};
byte r1[775],r2[775],e[60];
byte i,j,n,m,i2=0,i1=0,x,y;
int count,flag=0,c=0;
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

/**
 * Initialize.
 */
void setup() {
  
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();   
  pinMode(led,OUTPUT);
  pinMode(led2,OUTPUT);
  // Init SPI bus

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }
}

/**
 * Main loop.
 */
void loop() {

  //byte r1[800],r2[800],e[100];
//byte r1[750],r2[750],e[500],i,j,z,n,m,i2=0,i1=0,x,y;
//byte i,j,n,m,i2=0,i1=0,x,y;

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println();
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
      Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));
      if(reader==0)
      {
        c++;
        if(flag==1)
        {
          for(j=0;j<(mfrc522[0].uid.size)/2;j++)
          {
            for(n=0;n<i1;n++)
            {
              if(r1[n]==mfrc522[0].uid.uidByte[j])
              {
                
                r1[n]=0;
               // i1--;
              }
            }
          }
        }
        if(flag==1)
        {
           count=count-2;
           digitalWrite(led2,HIGH);
           delay(2000);
           digitalWrite(led2,LOW);
           flag=0;
        }
        else
        {
          for (i = 0; i < (mfrc522[0].uid.size)/2; i++)
          {
           r1[i1] = mfrc522[0].uid.uidByte[i];
           e[i]=mfrc522[0].uid.uidByte[i];
           i1++;
           //Serial.print(r1[i1], HEX);
          }
        }
      Serial.println();
      }
      if(reader==1)
      {
        c++;
        for(j=0;j<(mfrc522[1].uid.size)/2;j++)
        {
          for(m=0;m<i2;m++)
          {
            if(r2[m]==mfrc522[1].uid.uidByte[j])
            {
            r2[m]=0;  
            flag=1;
            //i2=i2-1;
            }
          }
        }
        if(flag!=1)
        {
         for (j = 0; j < (mfrc522[1].uid.size)/2; j++)
         {
         
              r2[i2] = mfrc522[1].uid.uidByte[j];
              i2++;
             // Serial.print(r2[i2], HEX);
         }
        }
        if(flag!=1)
        {
        for(x=0;x<=(mfrc522[1].uid.size)-3;x++)
        {
          for(y=0;y<i2;y++)
          {
            if(e[x]==r2[y])
            {
              count++;
              digitalWrite(led,HIGH);
              delay(2000);
              digitalWrite(led,LOW);
            }
          }
        }
        }
      Serial.println();
      }
      
   
      if(c%2==0)
      {
      Serial.println("warehouse:");
      Serial.print(count/2);
      Serial.println();
      }
     
      delay(2000);
      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC
  } //for(uint8_t reader
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
