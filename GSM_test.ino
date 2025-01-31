#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define GSM_TX_PIN 8
#define GSM_RX_PIN 7

static const uint32_t GPSBaud = 9600;

#define GPS_TX_PIN 4
#define GPS_RX_PIN 2


SoftwareSerial gsmSerial(GSM_TX_PIN, GSM_RX_PIN); // GSM connection
SoftwareSerial GPS(GPS_TX_PIN, GPS_RX_PIN); // GSM connection


  String phoneNumber[] = {"+9779865519287", "9865519287", "+9779824083176", "9824083176"};
  float Latitude=27.707706, Longitude=85.325290;

// The TinyGPS++ object
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  GPS.begin(GPSBaud);

  delay(2000);

  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  Serial.println("Initiated");
  pinMode(9, INPUT);
}

void loop() {
  while (GPS.available() > 0) {
    Serial.println("GPS available");
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

  String incomingPhoneNumber = getIncomingCallPhoneNumber();

  if (incomingPhoneNumber != "") {
    Serial.print("Incoming call detected! Phone number: ");
    Serial.println(incomingPhoneNumber);
    if (isPhoneNumberValid(incomingPhoneNumber.c_str(), phoneNumber, 4)) {
        delay(30000);
       sendLocation(Latitude, Longitude, incomingPhoneNumber);
        delay(3000);
        Serial.println("Calling: ");
        gsmSerial.print("ATD" + incomingPhoneNumber + ";\r");
        delay(20000);
        gsmSerial.println("ATH"); // Hang up the call
        Serial.println("Call Ended: ");
    }
    // Add your code to handle the incoming call here
  } 
}
bool isPhoneNumberValid(const char* incomingPhoneNumber, const String phoneNumber[], int size) {
  for (int i = 0; i < size; ++i) {
    if (strcmp(incomingPhoneNumber, phoneNumber[i].c_str()) == 0) {
      return true;  // Match found
    }
  }
  return false; // No match found
}

void sendLocation(float lat, float lon, String phNumber) {
  // Create SMS message
  String message = "Help! I'm in danger. My live location is: https://www.google.com/maps/place/";
  message += String(lat, 6);
  message += ",";
  message += String(lon, 6);

  // Send SMS
  gsmSerial.println("AT+CMGS=\"" + phNumber + "\"");
  delay(1000);
  gsmSerial.println(message);
  delay(100);
  gsmSerial.write(0x1A); // End of message
  delay(1000);

  // Check for "OK" response
  if (gsmSerial.find("OK")) {
    Serial.println("Message sent successfully.");
  } else {
    Serial.println("Failed to send message.");
    Serial.println("Check GSM module and network.");
  }
}
String getIncomingCallPhoneNumber() {
  gsmSerial.println("AT+CLCC");
  delay(100);

  if (gsmSerial.find("+CLCC: 1,1")) {
    gsmSerial.readStringUntil(',');
    gsmSerial.readStringUntil('"'); // Skip the opening quote
    String phoneNumber = gsmSerial.readStringUntil('"'); // Read the phone number
    return phoneNumber;
  }

  return "";
}
