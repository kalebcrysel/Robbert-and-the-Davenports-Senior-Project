#include <TimeProfiler.h>
#include <WiFi.h>
#include <AsyncUDP.h>

const char * ssid = "esp32_ssid";
const char * password = "password";

char intensity[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
int intensity_values[8] = {0, 130, 155, 175, 190, 205, 220, 255};
//int pins[5] = {9, 10, 3, 5, 6};
int pins[5] = {6, 5, 3, 10, 9};
int port = 1234;

AsyncUDP udp;


void setup() {
  for(int i = 0; i<5; i++){
    pinMode(pins[i], OUTPUT);
  }
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); // Make this the client (the server is WIFI_AP)
  WiFi.begin(ssid, password);

    Serial.print("Connecting...");
    // Display a period every 0.5 s to show the user something is happening.
    while (WiFi.waitForConnectResult() != WL_CONNECTED) { 
      Serial.print(".");
      delay(500);
    }
    Serial.println("");

    if(udp.listen(port)) {
        udp.onPacket([](AsyncUDPPacket packet) {
          Serial.print("Received data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
        });
    }
}

void loop() {
  
  TIMEPROFILE_BEGIN(one); // profiler
  
  if(Serial.available()){
    
    SCOPED_TIMEPROFILE(all); // begin scoped profiler
    
    int vals[5] = {0,0,0,0,0};
    for(int i = 0; i<5; i++){
      char c = Serial.read();
      Serial.println(c);
      TIMEPROFILE_END(one); // end "one"
      int j = 0;
      for(j = 0; j < 8; j++){
        if(c == intensity[j])
          break; 
      }
      vals[i] = intensity_values[j];
    }
    
    // Print profiler
    Serial.print("all : ");
    Serial.println(TimeProfiler.getProfile("all"));
    Serial.print("one : ");
    Serial.println(TimeProfiler.getProfile("one"));
    
    for(int i = 0; i<5; i++){
      ledcWrite(pins[i], vals[i]);
    }
    
  }
}
