#include <SoftwareSerial.h>
SoftwareSerial smsSerial(2,3); // RX and TX pins to communicate with GSM module
String number ="9413440235"; // Recipient's number - see text
#include <String.h>
#include <Wire.h>  // Wire library - used for I2C communication

int ADXL345 = 0x53; // The ADXL345 sensor I2C address

float X_out, Y_out, Z_out;  // Outputs
int led_pin = 5;
void setup()
{
   
  Serial.begin(9600);
  smsSerial.begin(9600);
     Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(100);
  pinMode(led_pin, OUTPUT);
   
   }
void loop()
{    // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  X_out = X_out/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  Y_out = Y_out/256;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Z_out = Z_out/256;
  Serial.print("Xa= ");
  Serial.print(X_out);
  Serial.print("   Ya= ");
  Serial.print(abs(Y_out));
  Serial.print("   Za= ");
  Serial.println(Z_out);
  
  
  if (abs(Y_out) > 0.80 ) // Check if the sense_pin sets off
   {digitalWrite(led_pin, HIGH);
     smsSerial.println("AT+CMGF=1"); // Set to Text Mode
      delay(500);
      smsSerial.println("AT+CMGS=\"+91"+number+"\""); // Specify recipient's number in international format
      delay(500);
      smsSerial.print("Warning! Your Vehicle has fall on its side"); // Enter the alert message 
      delay(500);
      smsSerial.write((byte)0x1A); // End of message character 0x1A (CTRL+Z)
      delay(500);
      smsSerial.println();
      }
  if (abs(Y_out)<0.80)
  {digitalWrite(led_pin, LOW);

   }
  
}
