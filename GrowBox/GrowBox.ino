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

/*
 * Сенсор температуры 
 * 
 * 1) Синий провод - GND
 * 2) Зеленый провод +5V
 * 3) Красный провод TEMPERATURE_SENSOR_PIN
 */ 
#define TEMPERATURE_SENSOR_PIN 2




#define FAN_PIN 13

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas growBoxTemperature ICs)
OneWire* oneWire = NULL;

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature* sensors = NULL;

#define ON LOW
#define OFF HIGH

void turnFans(int flag)
{
  digitalWrite(FAN_PIN, flag);
}

void setup()
{
  Serial.println("qwerty");
    
  oneWire = new OneWire(TEMPERATURE_SENSOR_PIN);
  
  
  sensors = new DallasTemperature(oneWire);
 
  Serial.println("qwerty -1");
  
  pinMode(FAN_PIN, OUTPUT);   
  turnFans(OFF);

  // начинаем передавать 
  Serial.begin(9600);
 
  // Начинаем получать данные от датчика
  sensors->begin();
}

/*
 * Диапазон температур от -55°C до +125°C
 * Погрешность преобразования меньше 0,5°C в диапазоне контролируемых температур -10°C до +85°С
 * Источник http://arduino-ua.com/prod190-Datchik_temperatyri_DS18B20
 *
 * @return возвращает температуру в градусах цельсия
 */
float growBoxTemperature()
{ 
  sensors->requestTemperatures(); 
  
  uint8_t countOfDevices = sensors->getDeviceCount();
  
  // запрашиваем обновленную информацию по первому сенсору
  float temperature = sensors->getTempCByIndex(0);
  
  Serial.print("Temperature is: "); Serial.println(temperature);
  
  return temperature;
}

const static float maxAllowedTemperature = 26.; // °C 

void loop()
{
  float temperature = growBoxTemperature();
    
  if( temperature > maxAllowedTemperature )
  {
    turnFans(ON);
  }
  else
  {
    turnFans(OFF);
  }
}
