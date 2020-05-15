//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//libraries for weather station
#include <Adafruit_MPL115A2.h>

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

//packet counter
int counter = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

const uint8_t vbatPin = 35;

//pressure and temperature sensor
Adafruit_MPL115A2 mpl115a2;


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
  display.print("LORA SENDER ");
  display.display();
  
  //initialize Serial Monitor
  Serial.begin(115200);
  
  Serial.println("LoRa Sender Test");

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
  display.print("LoRa Initializing OK!");
  display.display();

  //initialize vbat pin
  pinMode(vbatPin, INPUT);

  //initialize mp115a2
  mpl115a2.begin();
  delay(2000);
}

void loop() {
   
  Serial.print("Sending packet: ");
  float vbat = (float)(analogRead(vbatPin)) / 4095*2*3.3*1.1;
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("Battery: ");
  LoRa.print(vbat);
  LoRa.print("v");
  LoRa.endPacket();

  delay(5000);
  float pressureKPA = mpl115a2.getPressure();
  LoRa.beginPacket();
  LoRa.print("Pressure: ");
  LoRa.print(pressureKPA);
  LoRa.print("kpa");
  LoRa.endPacket();

  delay(5000);
  float temperatureC = mpl115a2.getTemperature();
  LoRa.beginPacket();
  LoRa.print("Temperature: ");
  LoRa.print(temperatureC);
  LoRa.print(" C");
  LoRa.endPacket();

  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("LORA Weather Station");
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("LoRa packages sent.");
  display.setCursor(0,30);
  display.print("Counter:");
  display.setCursor(50,30);
  display.print(counter*3);      
  display.display();

  counter++;
  
  delay(50000);
}
