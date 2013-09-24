/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Created by Max Lunin <nut.code/monkey@gmail.com> on 15/09/13.
 *
 * Arduino Sketch.
 * Groubox automation described at http://exotic-garden-at-home.blogspot.com blog
 *
 * Required OneWire library
 * Required DallasTemperature_360 library
 * Required LCD5110_Graph library
 */
  
#include <OneWire.h>
#include <DallasTemperature.h>
/* Подключение датчика температуры:
 *
 * Диапазон температур от -55°C до +125°C. Погрешность преобразования меньше 0,5°C в диапазоне контролируемых температур -10°C до +85°С
 * Источник http://arduino-ua.com/prod190-Datchik_temperatyri_DS18B20
 *
 * wire 1 (Синий провод) - GND
 * wire 2 (Зеленый провод) - +3.3 V
 * wire 3 (Красный провод) - TEMPERATURE_SENSOR_PIN (2) */
static const uint8_t TEMPERATURE_SENSOR_PIN = 2;


/* Подключение Реле:
 * Источник http://arduino-ua.com/prod203-8mi_kanalnii_modyl_rele_5V_10A
 * Плата реле /  Arduino
 *
 * VCC - +5 V
 * GND - GND
 * IN1 - FAN_PIN (3) */
static const uint8_t FAN_PIN = 3;

typedef class Fan
{
    const uint8_t pin;

public:
    
    static const uint8_t ON = HIGH;
    static const uint8_t OFF  = LOW;

    inline void turn(uint8_t mode) const
    {
        if(digitalRead(pin) != mode)
        {
            digitalWrite(pin, mode);
        }
    }

    Fan(uint8_t fanPin) : pin(fanPin)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, OFF);// sic! 
        digitalWrite(pin, ON); // устанавливаем высокий уровень для инициализации реле
    };
    
    inline bool isTurnedOff() const
    {
        return digitalRead(pin) == OFF;
    }
};


#include <LCD5110_Graph.h>
/* Модуль Nokia 5110 LCD
 * Источник http://arduino-ua.com/prod407-Nokia5110_LCD_modyl */
static const uint8_t LCD_RST_PIN  = 8;  // LCD RST .... Pin 1 - Reset
static const uint8_t LCD_CS_PIN   = 7;  // LCD CE  .... Pin 2 - выбор чипа
static const uint8_t LCD_DC_PIN   = 6;  // LCD DC ..... Pin 3 - переключатель данные/инструкции
static const uint8_t LCD_MOSI_PIN = 5;  // LCD Din .... Pin 4 - Serial Data Line
static const uint8_t LCD_SCK_PIN  = 4;  // LCD Clk .... Pin 5 - Serial Clock Line
                                        // LCD Vcc .... Pin 6 - (+3.3V) питание
                                        // LCD BL ..... Pin 7 - подсветка
                                        // LCD Gnd .... Pin 8 - земля
class GrowBox
{
public:
    DallasTemperature* termoSensors;
    Fan* fans;
    LCD5110* display;
    
    float minTemperature;
    float maxTemperature;
   
private:
    float getTemperature(bool& canFetch)
    {
        uint8_t countOfDevices = termoSensors->getDeviceCount();
        float temperature = 0;
        if (countOfDevices > 0)
        {
            termoSensors->requestTemperatures();
            // запрашиваем обновленную информацию по первому сенсору
            temperature = termoSensors->getTempCByIndex(0);
        }
        canFetch = (countOfDevices > 0) && (fabs(temperature) > 0.000001);
        return temperature;
    }

    void displayNoConnectedSensors()
    {
        display->clrScr();
        display->print("No sensor", CENTER, 0);
        display->print("connected", CENTER, 15);
        display->update();
    }

public:
    void displayCurrentTemperature()
    {
        bool canFetchTemperature;
        float temperature = getTemperature(canFetchTemperature);
        
        if (canFetchTemperature)
        {
            display->clrScr();
            display->print("Temperature:", CENTER, 0);
            
            String currentTemperature = String(temperature);
            display->print((char*)currentTemperature.c_str(), CENTER, 12);
            
            String separator = fans->isTurnedOff() ? String(" -> ") : String(" <- ");
            String interval = String(minTemperature, 1) += separator += String(maxTemperature, 1);
            display->print((char*)interval.c_str(), CENTER, 25);
            
            String isTurned = String("Fan: ") += (fans->isTurnedOff() ? "off" : "on");
            display->print((char*)isTurned.c_str(), CENTER, 37);
            
            display->update();
        }
        else
        {
            displayNoConnectedSensors();
            fans->turn(Fan::ON);
        }
    }
    
    void updateFansState()
    {
        bool canFetchTemperature;
        float temperature = getTemperature( canFetchTemperature );
        if (canFetchTemperature)
        {
             if (temperature < minTemperature)
             {
                 fans->turn( Fan::OFF );
                 delay(100); // 
             }
             else if (temperature > maxTemperature)
             {
                 fans->turn( Fan::ON );
             }
        }
        else
        {
            displayNoConnectedSensors();
            fans->turn( Fan::ON );
        }
    }
};

static GrowBox growBox;

void setup()
{
//    Serial.begin(9600);
    growBox.fans = new Fan(FAN_PIN);
    
    growBox.minTemperature = 27.5;
    growBox.maxTemperature = 28.;

    // Инициализируем датчик температуры
    OneWire* oneWire = new OneWire(TEMPERATURE_SENSOR_PIN);
    growBox.termoSensors = new DallasTemperature(oneWire);
    // Начинаем получать данные от датчика
    growBox.termoSensors->begin();
    
    growBox.display = new LCD5110(LCD_SCK_PIN, LCD_MOSI_PIN, LCD_DC_PIN, LCD_RST_PIN, LCD_CS_PIN);
    growBox.display->InitLCD();

    extern uint8_t SmallFont[]; 
    growBox.display->setFont(SmallFont);
}

void loop()
{
    growBox.displayCurrentTemperature();
    growBox.updateFansState();
}
