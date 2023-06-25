#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// const char ssid[] = "Larisa_2.4_Ghz";
// const char pass[] = "canon1234";

const char ssid[] = "username";
const char pass[] = "password";

const char post_server_name[] = "http://192.168.0.165:8000/api/v1/sensors_data";
const char get_server_name[] = "http://192.168.0.165:8000/api/v1/light_state";

void setup() {
  Wire.begin(); 
  Serial.begin(9600);  

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }

  Serial.println("Connected to Wi-Fi!");
}

struct Bin {
  int temperatura;
  int moisture1;
  int moisture2;
  int state_pomp;
  int state_light;
  int state_fan;
  int error;
};

Bin binData= {0, 0, 0, 0, 0, 0, 0};

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to Wi-Fi, reconnecting...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to Wi-Fi...");
    }
    Serial.println("Connected to Wi-Fi!");
    delay(1000);
  }

  Wire.requestFrom(8, sizeof(Bin)); 
  static bool post = false;

  while (Wire.available()) {
    Wire.readBytes((byte*)&binData, sizeof(binData));
    if(post){
      WiFiClient client;
      HTTPClient http;
      char json_format[200];

      sprintf(json_format, 
            "{\"temperature\": %f, \"soil_humidity1\": %f, \"soil_humidity2\": %f, \"pump_state\": %d, \"light_state\": %d, \"vent_state\": %d, \"errors\": %d}", 
            (float)binData.temperatura, (float)binData.moisture1, (float)binData.moisture2, binData.state_pomp, binData.state_light, binData.state_fan, binData.error);
      Serial.println(json_format);
      http.begin(client, post_server_name);

      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(String(json_format));
      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode);
      http.end();
      post = false;
    } else 
    {
      String light = httpGETRequest(get_server_name);

      Serial.print("Starea becului este:");
      Serial.println(light);
      JSONVar myObject = JSON.parse(light);
  
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

      Serial.print("JSON object = ");
      Serial.println(myObject);
      
      Serial.print("Ligh Status: ");
      Serial.println(myObject["light_state"]);
      int light_state = myObject["light_state"];
       Wire.beginTransmission(8); 
       Wire.write(light_state);  
       Wire.endTransmission();   

      post = true;
    }
  }
  delay(1000);
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();

  return payload;
}