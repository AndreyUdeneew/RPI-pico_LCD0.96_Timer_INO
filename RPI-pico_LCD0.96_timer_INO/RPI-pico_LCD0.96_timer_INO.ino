#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SPI.h>              // Arduino SPI library#include <Adafruit_GFX.h>    // Core graphics library

#define TFT_CS 9    // define chip select pin
#define TFT_DC 8    // define data/command pin
#define TFT_RST 12  // define reset pin, or set to -1 and connect to Arduino RESET pin
#define TFT_MOSI 11
#define TFT_SCLK 10
#define TFT_BL 25
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

boolean button1WasUp = true;  // переменные для считывания нажатий кнопок
boolean button2WasUp = true;
boolean button3WasUp = true;
boolean button4WasUp = true;

byte w = 0;
uint8_t ButtonDebounce = 30;

int SEC = 0;  // переменные для отсчета минут и секунд
int MIN = 0;
unsigned long timer;

void setup() {
  tft.setSPISpeed(40000000);
  tft.initR(INITR_MINI160x80);
  tft.setRotation(3);
  tft.invertDisplay(true);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.fillRoundRect(0, 0, 100, 40, 0, ST77XX_BLACK);
  tft.setCursor(5, 5);
  tft.setTextSize(5);
  tft.print("TEST!");


  pinMode(2, INPUT_PULLUP);  //The sketch works only when pinmodes are after the tft init... подключаем тактовые кнопки
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  pinMode(6, OUTPUT);  // подключаем светодиод и зуммер
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
}

void loop() {
  boolean button1IsUp = digitalRead(2);  // узнаем текущее состояние кнопок
  boolean button2IsUp = digitalRead(3);
  boolean button3IsUp = digitalRead(4);
  boolean button4IsUp = digitalRead(5);

  // при нажатии на первую кнопку вычитаем минуту и выводим на дисплей
  if (button1WasUp && !button1IsUp) {
    delay(ButtonDebounce);
    button1IsUp = digitalRead(2);
    if (!button1IsUp) {
      MIN = MIN - 1;
      SEC = 0;
      if (MIN < 0) { MIN = 0; }
      tft.fillRoundRect(0, 0, 100, 40, 0, ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.print("TIMER STOP");
      tft.setCursor(5, 0);
      tft.print(MIN);
      tft.print(" : ");
      tft.print(SEC);
    }
  }
  button1WasUp = button1IsUp;  // запоминаем состояние кнопки 1

  // при нажатии на вторую кнопку прибавляем минуту и выводим на дисплей
  if (button2WasUp && !button2IsUp) {
    delay(ButtonDebounce);
    button2IsUp = digitalRead(3);
    if (!button2IsUp) {
      MIN = MIN + 1;
      SEC = 0;
      tft.fillRoundRect(0, 0, 100, 40, 0, ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.print("TIMER STOP");
      tft.setCursor(5, 0);
      tft.print(MIN);
      tft.print(" : ");
      tft.print(SEC);
    }
  }
  button2WasUp = button2IsUp;  // запоминаем состояние кнопки 2

  // при нажатии на третью кнопку выключаем светодиод и переходим в цикл while
  if (button3WasUp && !button3IsUp && MIN > 0) {
    delay(ButtonDebounce);
    button3IsUp = digitalRead(6);
    if (!button3IsUp) {
      if (SEC == 0) {
        SEC = 60;
        MIN = MIN - 1;
      }
      if (MIN < 0) { MIN = 0; }
      digitalWrite(10, LOW);
      w = 1;
    }
  }
  button3WasUp = button3IsUp;  // запоминаем состояние кнопки 3

  // при нажатии на четвертую кнопку обнуляем счетчик и включаем светодиод
  if (button4WasUp && !button4IsUp) {
    delay(ButtonDebounce);
    button4IsUp = digitalRead(5);
    if (!button4IsUp) {
      MIN = 0;
      SEC = 0;
      tft.fillRoundRect(0, 0, 100, 40, 0, ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.print("TIMER STOP");
      tft.setCursor(5, 1);
      tft.print(MIN);
      tft.print(" : ");
      tft.print(SEC);
    }
  }
  button4WasUp = button4IsUp;  // запоминаем состояние кнопки 4

  while (w == 1) {
    // если прошло 1000 мс - вычитаем одну секунду от переменной SEC
    if (millis() - timer > 1000) {
      timer = millis();
      SEC = SEC - 1;

      // если отсчет закончился - включаем сигнал и выходим из цикла
      if (SEC == 0 && MIN == 0) {
        tft.fillRoundRect(0, 0, 100, 40, 0, ST77XX_BLACK);
        tft.setCursor(0, 0);
        tft.print("TIMER STOP");
        tft.setCursor(5, 1);
        tft.print(MIN);
        tft.print(" : ");
        tft.print(SEC);
        digitalWrite(10, HIGH);
        tone(12, 100);
        delay(500);
        noTone(12);
        w = 0;
      }

      // если секунды дошли до нуля - вычитаем одну минуту
      if (SEC == 0) {
        SEC = 59;
        MIN = MIN - 1;
        if (MIN < 0) { MIN = 0; }
      }

      // если из цикла while еще не вышли - выводим информацию на дисплей
      if (w == 1) {
        tft.fillRoundRect(0, 0, 100, 40, 0, ST77XX_BLACK);
        tft.setCursor(2, 0);
        tft.print("TIMER START");
        tft.setCursor(5, 1);
        tft.print(MIN);
        tft.print(" : ");
        tft.print(SEC);
      }
    }

    // если была нажата третья кнопка, выходим из цикла без обнуления счетчика
    boolean button3IsUp = digitalRead(4);
    if (button3WasUp && !button3IsUp) {
      delay(ButtonDebounce);
      button3IsUp = digitalRead(4);
      if (!button3IsUp) {
        tft.setCursor(2, 0);
        tft.print("TIMER  STOP");
        w = 0;
      }
    }
    button3WasUp = button3IsUp;  // запоминаем состояние кнопки 3
  }
}
