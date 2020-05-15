/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <TheThingsNetwork.h>


//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 433500000

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define loraSerial Serial1
#define debugSerial Serial
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaBattery;
String LoRaPressure;
String LoRaTemperature;

const uint8_t vbatPin = 35;
const char* ssid = "isbees";
const char* password =  "38937389698294142558";


void setup() {   
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA RECEIVER ");
  display.display();
  
  //initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("LoRa Receiver Test");
  
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.println("LoRa Initializing OK!");
  display.display();  
  
  //initialize vbat pin
  pinMode(vbatPin, INPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
}

void loop() {

  //try to parse Voltage Package
  float vbat = (float)(analogRead(vbatPin)) / 4095*2*3.3*1.1;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    Serial.print("Received packet ");

    //read packet
    while (LoRa.available()) {
      String LoRaPacket = LoRa.readString();
      if (LoRaPacket.substring(0,4)=="Batt"){
        LoRaBattery = LoRaPacket;
        Serial.print("Battery detected");
        Serial.print(LoRaBattery);
      }
      if (LoRaPacket.substring(0,4)=="Temp"){
        LoRaTemperature = LoRaPacket;
        Serial.print("Temperature detected");
        Serial.print(LoRaTemperature);
      }
      if (LoRaPacket.substring(0,4)=="Pres"){
        LoRaPressure = LoRaPacket;
        Serial.print("pressure detected");
        Serial.print(LoRaPressure);
      }
      
      Serial.print(LoRaPacket);
    }
  }

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");    
    Serial.println(rssi);

   // Dsiplay information
   display.clearDisplay();
   display.setCursor(0,0);
   display.print("LoRa Weather");
   display.setCursor(0,10);
   display.print(LoRaBattery);
   display.setCursor(0,20);
   display.print(LoRaPressure);
   display.setCursor(0,30);
   display.print(LoRaTemperature);
   display.setCursor(0,40);
   display.print("RSSI:");
   display.setCursor(30,40);
   display.print(rssi);
   display.setCursor(0,50);
   display.print("LocalBattery:");
   display.setCursor(80,50);
   display.print(vbat);
   display.display();   
}
