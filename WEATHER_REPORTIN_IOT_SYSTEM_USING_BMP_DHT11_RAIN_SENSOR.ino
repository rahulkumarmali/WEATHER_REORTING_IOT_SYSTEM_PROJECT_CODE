// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include<ESP8266WiFi.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"

// These pressure and temperature sensors use I2C to communicate, 2 pins
// are required to interface
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Analog 5/esp_pin_D1
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Analog 4/esp_pin_D2

#define DHTPIN D5     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14
#define Rain_Sensor_Pin D6

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
#define LEDonBoard 2

DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp;

String apiKey = "1MNS4NU8RPGF0DGW"; //--> Enter your Write API key from ThingSpeak

//----------------------------------------SSID and Password of your WiFi router
const char* ssid = "R";
const char* password = "llllllll";
//----------------------------------------
//----------------------------------------ThingSpeak Server
const char* server = "api.thingspeak.com";

WiFiClient client;

void setup(){
  // put your setup code here, to run once:
  // Initialize the Serial to communicate with the Serial Monitor.
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");
  if (!bmp.begin()) 
  {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
  pinMode(Rain_Sensor_Pin, INPUT);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(LEDonBoard, LOW);
    delay(250);
    digitalWrite(LEDonBoard, HIGH);
    delay(250);
    //----------------------------------------
  }
  
  //----------------------------------------If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
}

void loop() {
  // put your main code here, to run repeatedly:
  int rain_state = digitalRead(Rain_Sensor_Pin);
  Serial.println(rain_state);
  Serial.println("...");
  if(rain_state == HIGH)
  {
    Serial.println("The rain is not detected");
  }
  else
  {
    Serial.println("The rain is detected");
  }

    float bmpst = bmp.readTemperature();
    Serial.print("BMP_Sensor_Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    float bmpp = bmp.readPressure();
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal

    float bmpa = bmp.readAltitude();
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");
  
   // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  //Serial.println(" ");
  Serial.print(" Humidity: ");
  Serial.print(h);
  Serial.println("%");

  //Serial.print(" ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("°C ");

  //Serial.print(" ");
  Serial.print("Temperature in Fahrenheit: ");
  Serial.print(f);
  Serial.print("F");

  Serial.println("..");

  if (client.connect(server,80))  {   //--> "184.106.153.149" or api.thingspeak.com  
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(t);
    postStr +="&field2=";
    postStr += String(h);
    postStr +="&field3=";
    postStr += String(f);
    postStr +="&field4=";
    postStr += String(bmpst);
    postStr +="&field5=";
    postStr += String(bmpp);
    postStr +="&field6=";
    postStr += String(bmpa);
    postStr +="&field7=";
    postStr += String(rain_state);
    postStr += "\r\n\r\n";
 
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
   }

    client.stop();
  //Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates

  delay(1000);


}
