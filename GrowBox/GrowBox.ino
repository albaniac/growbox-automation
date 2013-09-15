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

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define FAN 13

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup(void)
{
    pinMode(FAN, OUTPUT);   
    digitalWrite(FAN, LOW);
  
    // start serial port
    Serial.begin(9600);
    Serial.println("Dallas Temperature IC Control Library Demo");
    
    // Start up the library
    sensors.begin();
}

void loop(void)
{
    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");
    
    Serial.print("Temperature for the device 1 (index 0) is: ");
    
    float temperature = sensors.getTempCByIndex(0);
    
    Serial.println(temperature);
    
    digitalWrite(FAN, (temperature > 30) ? HIGH : LOW);
}
