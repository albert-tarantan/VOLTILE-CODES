#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <U8x8lib.h>

//#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 1
#define DHTTYPE    DHT22

char Light [5];
char Tile [5];
const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

DHT dht(7, DHTTYPE);

U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);

// Singleton instance of the radio driver
RH_ASK driver(2000, 2, 4, 5);
// Class to manage message delivery and receipt, using the driver declared above
RHDatagram manager(driver, SERVER_ADDRESS);

struct dataStruct{
  float lightValue;
  float tileValue;
  //unsigned long counter; 
}dataStructValue;

void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");

  dataStructValue.lightValue = 0;
  dataStructValue.tileValue = 0;

  dht.begin();
  Serial.begin(9600); 
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_helvB10_tf); 
  u8g2.setColorIndex(1);
  
  //SensorReadings.tempValue = 0;
  //SensorReadings.soilValue = 0;
  //SensorReadings.soilRaw = 0;
  //SensorReadings.counter = 0;
}

void loop() {
ReciveFromSensors();
}

void ReciveFromSensors()
{
  // Dont put this on the stack:
  uint8_t buf[sizeof(dataStructValue)];
  uint8_t from;
  uint8_t len = sizeof(buf);
  
  if (manager.available())
  {    
    // Wait for a message addressed to us from the client
    if (manager.recvfrom(buf, &len, &from))
    {
      int i;
      memcpy(&dataStructValue, buf, sizeof(dataStructValue));
      //Serial.println("--------------------------------------------");
      //Serial.print("Got message from unit: ");
      //Serial.println(from, DEC);
      //Serial.print("Transmission number: ");
      //Serial.println(SensorReadings.counter);
      //Serial.println("");
       
          u8g2.firstPage();
      do {   
        draw();
      } while( u8g2.nextPage() );
    }
  }
}

void draw(){
  readLight();
  readTile();
  
  u8g2.drawFrame(0,0,128,31);         
  u8g2.drawFrame(0,33,128,31);           
  
  u8g2.drawStr( 7, 13, "Current to Light");   
  u8g2.drawStr( 35, 28, Light);   
  //u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  u8g2.drawUTF8(76, 28, "A");

  u8g2.drawStr(7,46, "Current from Tile");         
  u8g2.drawStr( 15, 61, Tile); 
  u8g2.drawUTF8(35, 61, "A"); 
}

void readLight(){
  float t = dataStructValue.lightValue;
  dtostrf(t, 3, 1, Light);
}

void readTile(){
  float h = dataStructValue.tileValue;
  dtostrf(h, 3, 1, Tile);
}
