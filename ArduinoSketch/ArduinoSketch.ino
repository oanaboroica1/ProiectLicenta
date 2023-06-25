#include <SoftwareSerial.h>
#include <DHT.h>
#include <Wire.h>

#define DHT_PIN 2             
#define pomp 11       
#define light 12     
#define fan 13       
#define humidity1 A0   
#define humidity2 A1   

#define min_temp 22.0   	
#define max_temp 26.0  		
#define min_humid 40.0       
#define max_humid 70.0

#define sensor_defection 15

DHT dht(DHT_PIN, DHT11);

int x=0;

struct Bin {
  long temperatura;
  long moisture1;
  long moisture2;
  long state_pomp;
  long state_light;
  long state_fan;
  long error;
};

Bin binData = {0, 0, 0, 0, 0, 0, 0};

void setup() {
  pinMode(light, OUTPUT);
  pinMode(pomp, OUTPUT);
  pinMode(fan, OUTPUT);

  Wire.begin(8);                
  Wire.onRequest(requestEvent); 
  Wire.onReceive(receiveEvent); 
  Serial.begin(9600);         

  dht.begin();
}

void loop() {

  binData.temperatura = dht.readTemperature();

  binData.moisture1 = 100 - map(analogRead(humidity1), 0, 1023, 0, 100);
  binData.moisture2 = 100 - map(analogRead(humidity2), 0, 1023, 0, 100);

  Serial.println(x);
  if(x==1){
   digitalWrite(light, HIGH);
   digitalWrite(fan, LOW);
   digitalWrite(pomp, LOW); 
    binData.state_light = 1;
    binData.state_fan = 0;
    binData.state_pomp = 0;
 } else {
  if (binData.temperatura > max_temp) {
    digitalWrite(fan, HIGH);      
    digitalWrite(light, LOW);   
    binData.state_light = 0;
    binData.state_fan = 1;
  } else if (binData.temperatura < min_temp) {
    digitalWrite(fan, LOW);      
    digitalWrite(light, HIGH);   
    binData.state_light = 1;
    binData.state_fan = 0;
  } else {
    if (binData.state_light) {
      digitalWrite(light, HIGH);
      digitalWrite(fan, LOW);
      binData.state_light = 1;
      binData.state_fan = 0;
    } else {
      digitalWrite(light, LOW);
      digitalWrite(fan, HIGH);
      binData.state_light = 0;
      binData.state_fan = 1;
    }
  }
 }

  if (binData.moisture1 - binData.moisture2 >= sensor_defection || binData.moisture2 - binData.moisture1 >= sensor_defection) {
    binData.error = 1;
  } else {
    binData.error = 0;

    if (binData.moisture1 < min_humid || binData.moisture2 < min_humid) {
      digitalWrite(pomp, HIGH);    
      binData.state_pomp = 1;
    } else if (binData.moisture1 > max_humid || binData.moisture2 > max_humid) {
      digitalWrite(pomp, LOW);   
      binData.state_pomp = 0;
    } else {
      if (binData.state_pomp) {
        digitalWrite(pomp, HIGH);
        binData.state_pomp = 1;
      } else {
        digitalWrite(pomp, LOW);
        binData.state_pomp = 0;
      }
    }
  }
delay(2000); 
}

void requestEvent() { 

Serial.print("Datele sunt:");
Serial.print(binData.temperatura);
Serial.print(",");
Serial.print(binData.moisture1);
Serial.print(",");
Serial.print(binData.moisture2); 
Serial.print(",");
Serial.print(binData.state_pomp);
Serial.print(",");
Serial.print(binData.state_light);
Serial.print(",");
Serial.print(binData.state_fan); 
Serial.print(",");
Serial.println(binData.error);

Wire.write((byte*)&binData, sizeof(binData));
delay(2000);

}

void receiveEvent(int howMany) {
  while (1 < Wire.available()) { 
    char c = Wire.read();     
  }
  x = Wire.read();    
  Serial.println(x);     
}