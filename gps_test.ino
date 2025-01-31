#include <SoftwareSerial.h>
#include <TinyGPS++.h>

HardwareSerial& GPS = Serial; // assuming your board has Serial1
TinyGPSPlus gps;


void setup() {
  Serial.begin(9600);
  GPS.begin(9600);
}

void loop() {
  // Read GPS data
  float Latitude, Longitude;
  while (GPS.available() > 0) {
    // Serial.println("GPS available");
    gps.encode(GPS.read());
    if (gps.location.isUpdated()) {
      Latitude = gps.location.lat();
      Longitude = gps.location.lng();
      Serial.print("Latitude= ");
      Serial.print(Latitude, 6);
      Serial.print(" Longitude= ");
      Serial.println(Longitude, 6);
    }
  }
}