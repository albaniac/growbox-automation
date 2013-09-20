/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Created by Max Lunin <nut.code/monkey@gmail.com> on 15/09/13.
 *
 * Arduino Sketch.
 * Groubox automation described at http://exotic-garden-at-home.blogspot.com blog
 *
 * Required DallasTemperature_360 library
 */
  

#include <OneWire.h>
#include <DallasTemperature.h>
/* Подключение Сенсор температуры:
 *
 * Диапазон температур от -55°C до +125°C. Погрешность преобразования меньше 0,5°C в диапазоне контролируемых температур -10°C до +85°С
 * Источник http://arduino-ua.com/prod190-Datchik_temperatyri_DS18B20
 *
 * wire 1 (Синий провод) - GND
 * wire 2 (Зеленый провод) - +3.3 V
 * wire 3 (Красный провод) - TEMPERATURE_SENSOR_PIN (2) */
static const uint8_t TEMPERATURE_SENSOR_PIN = 2;

static DallasTemperature* sensors = NULL;



/* Подключение Реле:
 * Источник http://arduino-ua.com/prod203-8mi_kanalnii_modyl_rele_5V_10A
 * Плата реле /  Arduino
 *
 * VCC - +5 V
 * GND - GND
 * IN1 - FAN_PIN (3) */
static const uint8_t FAN_PIN = 3;


#include <LCD5110_Graph.h>
/*
 * Модуль Nokia 5110 LCD
 * Источник http://arduino-ua.com/prod407-Nokia5110_LCD_modyl
 */
static const uint8_t DISPLAY_RST  = 8;  // LCD RST .... Pin 1 - Reset
static const uint8_t DISPLAY_CS   = 7;  // LCD CE  .... Pin 2 - выбор чипа
static const uint8_t DISPLAY_DC   = 6;  // LCD DC ..... Pin 3 - переключатель данные/инструкции
static const uint8_t DISPLAY_MOSI = 5;  // LCD Din .... Pin 4 - Serial Data Line
static const uint8_t DISPLAY_SCK  = 4;  // LCD Clk .... Pin 5 - Serial Clock Line
                                        // LCD Vcc .... Pin 6 - (+3.3V) питание
                                        // LCD BL ..... Pin 7 - подсветка
                                        // LCD Gnd .... Pin 8 - земля                  
static LCD5110* display = NULL;

#define ON LOW
#define OFF HIGH

void turnFans(int flag)
{
    digitalWrite(FAN_PIN, flag);
}

void setup()
{
    pinMode(FAN_PIN, OUTPUT);
    turnFans(OFF);
    
    // Инициализируем датчик температуры
    OneWire* oneWire = new OneWire(TEMPERATURE_SENSOR_PIN);
    sensors = new DallasTemperature(oneWire);
    // Начинаем получать данные от датчика
    sensors->begin();
    
    display = new LCD5110(DISPLAY_SCK, DISPLAY_MOSI, DISPLAY_DC, DISPLAY_RST, DISPLAY_CS);
    display->InitLCD();

    extern uint8_t SmallFont[]; 
    display->setFont(SmallFont);
   
    Serial.begin(9600);
}

bool growBoxCurrentTemperature(float& temperature)
{
    uint8_t countOfDevices = sensors->getDeviceCount();

    if ( countOfDevices > 0 )
    {
        sensors->requestTemperatures();
        // запрашиваем обновленную информацию по первому сенсору
        temperature = sensors->getTempCByIndex(0);
    }
    
    return countOfDevices > 0;
}

const static float maxAllowedTemperature = 26.; // °C

void floatFraction(float f, int& integralPart, int& fractionalPart)
{
    integralPart = f;    
    fractionalPart = (f - (int)f) * 10;
}

void updateDisplayWithTemperature(float temperature)
{
    display->clrScr();
    display->print("Temperature:", 0, 0);
    
    char buffer[100];
    
    // В snprintf под AVR нельзя использовать %f
    // Problems with <stdio.h> and sprintf for floats : http://forum.arduino.cc/index.php/topic,40790.0.html 
    // поэтому вычленяем целую и дробные части как int и печатаем их с точкой

    int integralPart = 0;    
    int fractionalPart = 0;
    floatFraction(temperature, integralPart, fractionalPart);

    int cx = snprintf(buffer, sizeof(buffer), "%d.%d / ", integralPart, fractionalPart);
        
    floatFraction(maxAllowedTemperature, integralPart, fractionalPart);
    snprintf (buffer+cx, sizeof(buffer)-cx, "%d.%d", integralPart, fractionalPart);

    display->print(buffer, CENTER, 10);
    display->update();
}

void displayNoConnectedSensors()
{
    display->clrScr();
    display->print("No sensor", CENTER, 0);
    display->print("connected", CENTER, 15);
    display->update();
}

void loop()
{
    float growBoxTemperature;
    
    if( !growBoxCurrentTemperature(growBoxTemperature) )
    {
        displayNoConnectedSensors();
        turnFans(ON);
        Serial.print("not connected");
        return;
    }
    
//    Serial.print("Temperature is: ");Serial.println(growBoxTemperature);
    
    if( growBoxTemperature > maxAllowedTemperature )
    {
        turnFans(ON);
    }
    else
    {
        turnFans(OFF);
    }

    updateDisplayWithTemperature(growBoxTemperature);    
}
