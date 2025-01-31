#include <Wire.h>
#include <math.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define GSM_TX_PIN 8
#define GSM_RX_PIN 7
static const uint32_t GPSBaud = 9600;

SoftwareSerial gsmSerial(GSM_TX_PIN, GSM_RX_PIN); // GSM connection
HardwareSerial& GPS = Serial; // assuming your board has Serial1
String phoneNumber[] = {"+9779865519287", "9865519287", "+9779824083176", "9824083176"};
float Latitude=27.707706, Longitude=85.325290;

// Define and initialize variables related to the gyro
float x; // angular velocity around x-axis
float y; // angular velocity around y-axis
float z; // angular velocity around z-axis
float angleZ = 0; // angle of rotation around z-axis
unsigned long previousTime = 0;
int ldrpin=A0;
int ledpin=11;
int brightness;
int val;    // variable to read the value from the analog pin

// L3G4200D gyro address
int L3G4200D_Address = 105;

Servo myservo;
TinyGPSPlus gps;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  gsmSerial.begin(9600);
  GPS.begin(GPSBaud);

  delay(2000);
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  Serial.println("Initiated");

  pinMode(ldrpin, INPUT);
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin,LOW);
  pinMode(12,OUTPUT);
  digitalWrite(12,HIGH);
  myservo.attach(10);  // attaches the servo on pin 9 to the servo object
  delay(100);
  myservo.write(90);
  Serial.println("starting up L3G4200D");
  setupL3G4200D(2000); // Configure L3G4200  - 250, 500 or 2000 deg/sec
  delay(1500);
  
}

void loop() {
  unsigned long currentTime = millis();
  float elapsedTime = (currentTime - previousTime) / 1000.0; // Convert to seconds

  getGyroValues();  // This will update x, y, and z with new values

  // Integrate angular velocities to get angles of rotation
  if(z>=-10 && z<=10)z=0;
  angleZ += (z * elapsedTime);
  // Serial.println(z);
  
  float degree = map(angleZ, 1500,-1500,180,0);
  if(degree>180)degree=180;
  if(degree<0)degree=0;
  // Serial.print("   Angle : ");
  // Serial.println(degree);
  myservo.write(degree);
  previousTime = currentTime;

  int red=analogRead(ldrpin);
  if(red>800){
    analogWrite(ledpin,10);}
  else{
  brightness= map(red, 0, 800, 255, 0);

  analogWrite(ledpin,brightness);
  }
  if (GPS.available() > 0) {
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

void getGyroValues() {
  byte zMSB = readRegister(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister(L3G4200D_Address, 0x2C);
  z = ((zMSB << 8) | zLSB);
}

int setupL3G4200D(int scale) {
  //From  Jim Lindblom of Sparkfun's code

  // Enable x, y, z and turn off power down:
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b00001111);

  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00000000);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:

  if (scale == 250) {
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00000000);
  } else if (scale == 500) {
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00010000);
  } else {
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00000000);
}

void writeRegister(int deviceAddress, byte address, byte val) {
  Wire.beginTransmission(deviceAddress); // start transmission to device 
  Wire.write(address);       // send register address
  Wire.write(val);         // send value to write
  Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address) {

  int v;
  Wire.beginTransmission(deviceAddress);
  Wire.write(address); // register to read
  Wire.endTransmission();

  Wire.requestFrom(deviceAddress, 1); // read a byte

  while (!Wire.available()) {
    // waiting
  }

  v = Wire.read();
  return v;
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
  String message = "Hi, There. My live location is: https://www.google.com/maps/place/";
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