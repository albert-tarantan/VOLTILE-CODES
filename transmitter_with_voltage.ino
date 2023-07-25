#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>


//RH communication
#define CLIENT_ADDRESS 2
#define SERVER_ADDRESS 1
#define ANALOG_IN_PIN A3
// Singleton instance of the radio driver
RH_ASK driver (2000, 4, 2, 5);
// Class to manage message delivery and receipt, using the driver declared above
RHDatagram manager(driver, CLIENT_ADDRESS);


struct dataStruct{
  float lightValue;
  //float tileValue;
  int voltageValue;
  unsigned long counter;
}dataStructValue;

byte buf[sizeof(dataStructValue)] = {0};

void setup() {
  Serial.begin(9600);
  
  //RF communication
  if (!manager.init())
    Serial.println("init failed");

  dataStructValue.lightValue=0;
  //dataStructValue.tileValue=0;
  dataStructValue.voltageValue=0;
}

void loop() {
  //Serial.println("------------------------------------");
  float lightValue;
  //float tileValue;
  int voltageValue;
  
  /*
  tile(); //Read temp sensor
  //delay(1000);
  delay(100);
  */

  light(); //Read hygrometer
  //delay(1000);
  delay(100);

  voltage(); //Read hygrometer
  //delay(1000);
  delay(100);

  
  SendValues(); //Send sensor values
  //delay(1000);
  delay(200);
  //Serial.println("------------------------------------");
}

/*
void tile() {
  unsigned int x=0;
  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,tileValue=0.0;

    for (int x = 0; x < 150; x++){ //Get 150 samples
    AcsValue = analogRead(A1);     //Read current sensor values   
    Samples = Samples + AcsValue;  //Add samples together
    delay (3); // let ADC settle before next sample 3ms
  }
  AvgAcs=Samples/150.0;//Taking Average of Samples

  //((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
  //2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
  //out to be 2.5 which is out offset. If your arduino is working on different voltage than 
  //you must change the offset according to the input voltage)
  //0.185v(185mV) is rise in output voltage when 1A current flows at input
  tileValue = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.185;

  Serial.print("current to tile: "); 
  Serial.println(tileValue);//Print the read current on Serial monitor
  dataStructValue.tileValue = tileValue;
}
*/

void light() {
  unsigned int x=0;
  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,lightValue=0.0;

    for (int x = 0; x < 150; x++){ //Get 150 samples
    AcsValue = analogRead(A0);     //Read current sensor values   
    Samples = Samples + AcsValue;  //Add samples together
    delay (3); // let ADC settle before next sample 3ms
  }
  AvgAcs=Samples/150.0;//Taking Average of Samples

  //((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
  //2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
  //out to be 2.5 which is out offset. If your arduino is working on different voltage than 
  //you must change the offset according to the input voltage)
  //0.185v(185mV) is rise in output voltage when 1A current flows at input
  lightValue = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.185;

  Serial.print("current to light: "); 
  Serial.println(lightValue);//Print the read current on Serial monitor
  dataStructValue.lightValue = lightValue;
}


void voltage() {
  // Floats for ADC voltage & Input voltage
  float adc_voltage = 0.0;
  float in_voltage = 0.0;
  float percent = 0.0;
  float voltageV = 0.0;
  int voltageValue = 0;
  
  // Floats for resistor values in divider (in ohms)
  float R1 = 30000.0;
  float R2 = 7500.0; 
  
  // Float for Reference Voltage
  float ref_voltage = 5.0;
  
  // Integer for ADC value
  int adc_value = 0;

  // Read the Analog Input
   adc_value = analogRead(A3);
   
   // Determine voltage at ADC input
   adc_voltage  = (adc_value * ref_voltage) / 1024.0; 
   
   // Calculate voltage at divider input
   in_voltage = adc_voltage / (R2/(R1+R2)) ; 
   
  voltageV = in_voltage + 0.5;
  percent = ((voltageV - 0.4) / 3.7) * 100;
  voltageValue = percent;


  // Print results to Serial Monitor to 2 decimal places
  Serial.print("Input Voltage = ");
  //Serial.print(in_voltage);
  Serial.println(voltageValue);
  //Serial.print(percent);
  //Serial.println("%");
  dataStructValue.voltageValue = voltageValue;
  // Short delay
  //delay(500);
}

//RF communication
void SendValues() {  
  //Load message into data-array
  //byte data[sizeof(sensorValues)];

  byte zize=sizeof(dataStructValue);
  memcpy (buf, &dataStructValue, zize);

  
  Serial.println("Sending to ask_datagram_server");
  int i = 0;
  int repeat = 10; //Times to repeat same message
    
  // Send a message to manager_server
  while (i<repeat)
  {
    if (manager.sendto(buf, zize, SERVER_ADDRESS))
    {
      Serial.println("Message sent");   
    }
    else
    {
      Serial.println("sendto failed");
    }
    delay(100);
    i = i+1;
  }
  dataStructValue.counter = dataStructValue.counter + 1;
}


