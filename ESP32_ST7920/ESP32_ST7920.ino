#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <OneWire.h>
#include <DallasTemperature.h>

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 23, /* CS=*/ 5, /* reset=*/ 22);

/******************************************* Defind variable; **************************************************/
//adc value
int adc1, adc2, adc3;
int _adc1, _adc2, _adc3;
//function menu
volatile int _case = 0;
//switch pin
#define SW1 27
//DS18B20 Onewire
const int oneWireBus = 26; 
//LED Set
#define LED1 15
#define LED2 2
#define LED3 4
#define LED4 19
#define LED5 21
#define LED6 13
#define LED7 12
#define LED8 14
int LEDSET[] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8};
byte fade[] = {0, 0, 0, 0, 200, 255, 200, 150, 100, 50, 10};
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void IRAM_ATTR isr() {
  _case++;
  if(_case > 2){
    _case = 0;
  }
}

void setup(void) {
  Serial.begin(115200);
  sensors.begin();
  pinMode(2, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  attachInterrupt(SW1, isr, FALLING);
//  LEDC setup
  ledcSetup(0, 5000, 8);          
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);
  ledcSetup(3, 5000, 8);
  ledcSetup(4, 5000, 8);          
  ledcSetup(5, 5000, 8);
  ledcSetup(6, 5000, 8);
  ledcSetup(7, 5000, 8);
  ledcAttachPin(LED1, 0);         
  ledcAttachPin(LED2, 1);
  ledcAttachPin(LED3, 2);
  ledcAttachPin(LED4, 3);
  ledcAttachPin(LED5, 4);        
  ledcAttachPin(LED6, 5);
  ledcAttachPin(LED7, 6);
  ledcAttachPin(LED8, 7);
  u8g2.begin();
}

void loop() {
  if(_case == 0){
    sensors.requestTemperatures(); 
    float temperatureC = sensors.getTempCByIndex(0);
    float humid = random(7000,10000) / 100.0;
    Serial.print(temperatureC);
    Serial.println("ºC");
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(56,10,"24/10/64 0:14");
    u8g2.drawStr(57,20,"Temperature");
    u8g2.setCursor(75 ,33);
    u8g2.print(String(temperatureC) + " C");
    u8g2.drawStr(64,43,"Humidity");
    u8g2.setCursor(75 ,55);
    u8g2.print(String(humid) + " %");
    if(temperatureC < 28 and humid > 85){
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(5, 55, 67);
    }
    else{
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(5, 55, 69);
    }
    u8g2.sendBuffer();
    delay(500);
  }
  else if(_case == 1){
    for(int i = 0; i <= 8; i++){
      digitalWrite(LEDSET[i], 0);
    }
    readADC();
    delay(100);
    _adc1 = map(adc1, 0, 4095, 0, 36);
    _adc2 = map(adc2, 0, 4095, 0, 36);
    _adc3 = map(adc3, 0, 4095, 0, 36);
    u8g2.clearBuffer();         
    u8g2.setFont(u8g2_font_ncenB08_tr); 
    u8g2.drawStr(20,10,"Electronic ENG.");
    u8g2.drawStr(35,20,"Read ADC");
    u8g2.drawStr(5,35,"ADC0");
    u8g2.drawStr(50,35,"ADC1");
    u8g2.drawStr(93,35,"ADC2");
    u8g2.drawLine(0, 40, 128, 40);
    u8g2.drawLine(42, 25, 42, 64);
    u8g2.drawLine(85, 25, 85, 64);
    u8g2.setCursor(8, 52);
    u8g2.print(adc1);
    ProgressbarADC(1, 53, 40, 10, _adc1);
    u8g2.setCursor(53,52);
    u8g2.print(adc2);
    ProgressbarADC(44, 53, 40, 10, _adc2);
    u8g2.setCursor(95,52);
    u8g2.print(adc3);
    ProgressbarADC(87, 53, 40, 10, _adc3);
    u8g2.sendBuffer();
  }else if(_case == 2){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr); 
    u8g2.drawStr(20,10,"Electronic ENG.");
    u8g2.drawStr(30,20,"LED Raining");
    u8g2.drawLine(0, 25, 128, 25);
    u8g2.sendBuffer();
    for(int i = 0; i < 11; i++) {
      ledcWrite(0, fade[i]);
      ledcWrite(1, fade[i + 1]);
      ledcWrite(2, fade[i + 2]);
      ledcWrite(3, fade[i + 3]);
      ledcWrite(4, fade[i + 4]);
      ledcWrite(5, fade[i + 5]);
      ledcWrite(6, fade[i + 6]);
      ledcWrite(7, fade[i + 7]);
    delay(75);
  }
 }
}

void readADC(){
  adc1 = analogRead(34);
  adc2 = analogRead(35);
  adc3 = analogRead(32);
}

int ProgressbarADC(int x, int y, int w, int h, int _adc){
    u8g2.drawFrame(x, y, w, h);
    u8g2.drawBox(x + 2, y + 2, _adc, h - 4);
}
