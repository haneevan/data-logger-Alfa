#include <SPI.h>
#include <SD.h>
#include <Wire.h>  // Wire library - used for I2C communication
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Outputs
#include "RTClib.h"
const int chipSelect = 10;
File myFile;
RTC_DS1307 rtc;
char daysOfTheWeek[7][21]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
void setup() {
  Serial.begin(9600); // Initiate serial communication for printing the results on the Serial monitor
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);
Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  if (SD.exists("example2.txt")) {
    Serial.println("example2.txt exists.");
  } else {
    Serial.println("example2.txt doesn't exist.");
  }
  Serial.println("Creating example.txt...");
  myFile = SD.open("example2.txt", FILE_WRITE);
  myFile.close();

  if (SD.exists("example2.txt")) {
    Serial.println("example2.txt exists.");
  } else {
    Serial.println("example2.txt doesn't exist.");
  }
}  

void loop() {
DateTime now = rtc.now();
String dataString = "";
dataString += String(now.year(), DEC);
dataString += String('/');
dataString += String(now.month(), DEC);
dataString += String('/');
dataString += String(now.day(), DEC);
dataString += String("(");
dataString += String(daysOfTheWeek[now.dayOfTheWeek()]);
dataString += String(")");
dataString += String(now.hour(), DEC);
dataString += String(':');
dataString += String(now.minute(), DEC);
dataString += String(':');
dataString += String(now.second(), DEC);
dataString += String(" Response: ");
  // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  X_out = X_out/64; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  Y_out = Y_out/64;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Z_out = Z_out/64;
  Serial.print("Xa= ");
  Serial.print(X_out);
  Serial.print("   Ya= ");
  Serial.print(Y_out);
  Serial.print("   Za= ");
  Serial.println(Z_out);
 
  myFile = SD.open("example2.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (myFile) {
      myFile.print(dataString);
      myFile.print(",");
      myFile.print(X_out);
      myFile.print(",");
      myFile.print(Y_out);
      myFile.print(",");
      myFile.println(Z_out);
      myFile.close();
  delay(500);
    }
}
