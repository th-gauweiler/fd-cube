#include <Wire.h>
#include <hcsr04.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <Servo.h>
#include <IRremote.hpp>
#include <TinyMPU6050.h>
#include <DHT.h>

#define TEST_VERSION "1.0.1"


#define OLED_I2C_ADDRESS 0x3C
#define OLED_RST_PIN -1
#define LED_RED_PIN 12
#define LED_YELLOW_PIN 11
#define LED_GREEN_PIN 10
#define BUTTON_PIN 9
#define PIR_PIN 8
#define SPEAKER_PIN 7
#define HCSR04_TRIG_PIN 6
#define HCSR04_ECHO_PIN 5
#define LED_RING_PIN 4
#define LED_RING_NUM 12
#define SERVO_PIN 3
#define REMOTE_CONTROL_PIN 2
#define LDR_PIN A7
#define POTI_PIN A6

SSD1306AsciiWire oled;
HCSR04 hcsr04(HCSR04_TRIG_PIN, HCSR04_ECHO_PIN, 20, 1500);
Adafruit_NeoPixel pixels(LED_RING_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);
Servo servo;
MPU6050 mpu (Wire);
DHT dht11(A3, DHT11);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  setupOled();
  setupLED();
  setupPir();
  setupSpeaker();
  setupUltraSonic();
  setupLedRing();
  setupServo();
  setupRC();
  setupLdr();
  setupPoti();
  setupMpu();
  setupDht();

  reset();
}

void setupOled() {
  Wire.begin();
  Wire.setClock(400000L);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS, OLED_RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
#endif // RST_PIN >= 0
}

void setupLED() {
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
}

void setupPir() {
  pinMode(PIR_PIN, INPUT);
}

void setupSpeaker() {
}

void setupUltraSonic() {
}

void setupLedRing() {
  pixels.begin();
}

void setupServo() {
  servo.attach(3);
}

void setupRC() {
}

void setupLdr() {
  pinMode(LDR_PIN, INPUT);
}

void setupPoti() {
  pinMode(POTI_PIN, INPUT);
}

void setupMpu() {
  mpu.Initialize();
}

void setupDht() {
}


int testLevel = 0;

void loop() {
  if (testLevel == 1)
    testLED();
  if (testLevel == 2)
    testPir();
  if (testLevel == 3)
    testSpeaker();
  if (testLevel == 4)
    testUltraSonic();
  if (testLevel == 5)
    testLedRing();
  if (testLevel == 6)
    testServo();
  if (testLevel == 7)
    testRC();
  if (testLevel == 8)
    testLdr();
  if (testLevel == 9)
    testPoti();
  if (testLevel == 10)
    testMpu();
  if (testLevel == 11)
    testDht();
  if (isButtonPressed()) {
    digitalWrite(LED_BUILTIN, HIGH);
    testLevel = ++testLevel % 12;
    while (isButtonPressed())
      delay(50);
    reset();
  }
}

void reset() {
  digitalWrite(LED_BUILTIN, LOW);
  outputsOff();
  printTitleOnOled();
}

void printTitleOnOled() {
  oled.clear();
  oled.setScrollMode(SCROLL_MODE_AUTO);
  oled.setFont(TimesNewRoman13);
  oled.print("Fun");
  oled.setFont(TimesNewRoman13_italic);
  oled.print("duino ");
  oled.setFont(TimesNewRoman13);
  oled.println("CUBE");
  oled.setFont(Adafruit5x7);
  oled.print("Test ");
  oled.println(TEST_VERSION);
  oled.println("---------------------");
}

void testLED() {
  int millis = 250;
  oled.println("LED test");
  digitalWrite(LED_RED_PIN, HIGH);
  delay(millis);
  digitalWrite(LED_YELLOW_PIN, HIGH);
  delay(millis);
  digitalWrite(LED_GREEN_PIN, HIGH);
  delay(millis);
  digitalWrite(LED_RED_PIN, LOW);
  delay(millis);
  digitalWrite(LED_YELLOW_PIN, LOW);
  delay(millis);
  digitalWrite(LED_GREEN_PIN, LOW);
  delay(millis);
}

void testPir() {
  oled.println("PIR test");
  while (!isButtonPressed()) {
    if (digitalRead(PIR_PIN) == HIGH) {
      digitalWrite(LED_GREEN_PIN, HIGH);
    }
    delay(200);
    digitalWrite(LED_GREEN_PIN, LOW);
  }
}

void testSpeaker() {
  oled.println("Speaker test");
  while (!isButtonPressed()) {
    digitalWrite(LED_YELLOW_PIN, HIGH);
    for (int i=100; i<500 && !isButtonPressed(); i+=100) {
      tone(SPEAKER_PIN, i);
      printfRepeatedValue("", i);
      delay(analogRead(POTI_PIN));
    }
    noTone(SPEAKER_PIN);
    digitalWrite(LED_YELLOW_PIN, LOW);
    delay(1000);
  }
}

void testUltraSonic() {
  oled.println("UltraSonic test");
  while (!isButtonPressed()) {
    printfRepeatedValue("Millimeter: ", hcsr04.distanceInMillimeters());
    delay(250);
  }
}

void testLedRing() {
  oled.println("LED RING test");
  pixels.clear();
  oled.println("bunt");
  for(int i=0; i<LED_RING_NUM; i++) {
    if (i % 3 == 0)
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    if (i % 3 == 1)
      pixels.setPixelColor(i, pixels.Color(0, 0, 150));
    if (i % 3 == 2)
      pixels.setPixelColor(i, pixels.Color(150, 0, 0));
    pixels.show();
    delay(analogRead(POTI_PIN));
    if (isButtonPressed()) {
      return;
    }
  }
  uint32_t color = pixels.Color(150, 0, 0);
  oled.println("rot");
  if (!isButtonPressed()) {
    setLEDRingFor(color);
  }
  color = pixels.Color(0, 150, 0);
  oled.println("gruen");
  if (!isButtonPressed()) {
    setLEDRingFor(color);
  }
  color = pixels.Color(0, 0, 150);
  oled.println("blau");
  if (!isButtonPressed()) {
    setLEDRingFor(color);
  }
  color = pixels.Color(150, 150, 150);
  oled.println("weiss");
  if (!isButtonPressed()) {
    setLEDRingFor(color);
  }
  color = pixels.Color(0, 0, 0);
  oled.println("aus");
  if (!isButtonPressed()) {
    setLEDRingFor(color);
  }
}

void setLEDRingFor(uint32_t color) {
  for(int i=0; i<LED_RING_NUM; i++) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(analogRead(POTI_PIN));
    if (digitalRead(BUTTON_PIN) == HIGH) {
      return;
    }
  }
}

void testServo() {
  oled.println("Servo test");
  while (!isButtonPressed()) {
      for (int angle=0; angle < 180 && !isButtonPressed() ; angle += 45) {
        setServoAngle(angle);
      }
      for (int angle=180; angle > 0 && !isButtonPressed() ; angle -= 45) {
        setServoAngle(angle);
      }
  }
}

void setServoAngle(int angle) {
  digitalWrite(LED_GREEN_PIN, HIGH);
  oled.print("Winkel: ");
  oled.print(angle);
  oled.clearToEOL();
  oled.setCol(0);
  servo.write(angle);
  delay(50);
  digitalWrite(LED_GREEN_PIN, LOW);
  delay(450 + analogRead(POTI_PIN));
}

void testRC() {
  oled.println("RemoteControl test");
  IrReceiver.begin(REMOTE_CONTROL_PIN, ENABLE_LED_FEEDBACK);
  while (!isButtonPressed()) {
    if (IrReceiver.decode()) {
      oled.print("Wert: ");
      char command = convertIRCommand(IrReceiver.decodedIRData.command);
      oled.print(command);
      blinkLedRing(command);
      oled.clearToEOL();
      oled.setCol(0);
      IrReceiver.resume();
    }
  }
  IrReceiver.stop();
}

void blinkLedRing(char command) {
  int ledNo = calcLedNo(command);
  if (ledNo < 0)
    return;
  pixels.clear();
  pixels.setPixelColor(ledNo, pixels.Color(0, 150, 0));
  pixels.show();
  delay(analogRead(POTI_PIN));
  pixels.clear();
}

int calcLedNo(char command) {
  if (command >= '0' && command <= '9')
    return command - '0';
  if (command == 'A')
    return 10;
  if (command == 'B')
    return 11;
  return -1;
}

void testLdr() {
  oled.println("LDR test");
  while (!isButtonPressed()) {
    oled.print(analogRead(LDR_PIN));
    oled.clearToEOL();
    oled.setCol(0);
    delay(500);
  }
}

void testPoti() {
  oled.println("Potentiometer test");
  while (!isButtonPressed()) {
    printfRepeatedValue("", analogRead(POTI_PIN));
    delay(500);
  }
}

void testMpu() {
  oled.println("MPU test");
  oled.println("Kalibrierung beginnt,");
  oled.println("nicht wackeln!!!");
  oled.println("bitte warten!");
  mpu.Calibrate();

  oled.println("Kalibrierung abgeschlossen");
  oled.println("x/y/z");
  while (!isButtonPressed()) {
    mpu.Execute();
    oled.print(mpu.GetAngX());
    oled.print(" / ");
    oled.print(mpu.GetAngY());
    oled.print(" / ");
    oled.print(mpu.GetAngZ());
    oled.clearToEOL();
    oled.setCol(0);
    delay(250);
  }
}

void testDht() {
  oled.println("DHT test");
  dht11.begin();
  oled.println("Temperatur in Celsius");
  oled.println("Feuchtigkeit in %");
  while (!isButtonPressed()) {
    oled.print("T: ");
    oled.print(dht11.readTemperature());
    oled.print("C   F: ");
    oled.print(dht11.readHumidity());
    oled.print("%");
    oled.clearToEOL();
    oled.setCol(0);
    delay(1000);
  }
}

bool isButtonPressed() {
  return digitalRead(BUTTON_PIN) == HIGH;
}

void outputsOff() {
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_YELLOW_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, LOW);
  noTone(SPEAKER_PIN);
  pixels.clear();pixels.show();
}

void printfRepeatedValue(String name, int value) {
  oled.print(name);
  oled.print(value);
  oled.clearToEOL();
  oled.setCol(0);
}

char convertIRCommand(int command) {
  switch(command) {
    case 70:
      return 'U'; // arrow up
    case 68:
      return 'L'; // arrow left
    case 64:
      return 'X';
    case 67:
      return 'R'; // arrow right
    case 21:
      return 'D'; // arrow down
    case 69:
      return 'A';
    case 71:
      return 'B';
    case 9:
      return 'C';
    case 7:
      return '0';
    case 22:
      return '1';
    case 25:
      return '2';
    case 13:
      return '3';
    case 12:
      return '4';
    case 24:
      return '5';
    case 94:
      return '6';
    case 8:
      return '7';
    case 28:
      return '8';
    case 90:
      return '9';
  }
}