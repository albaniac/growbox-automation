growbox-automation
==================

Arduino Sketch.

Пример кода для подключения однопроводного датчика температуры DS18B20 и экрана Nokia5110 к Arduino.

Необходимые библиотеки:
* OneWire
* DallasTemperature_360
* LCD5110_Graph

Их необходимо импортировать через меню Arduino.app -> Sketch -> Import Library... -> Add library...

Цель - автоматизация гроубокса (growbox) описаного в боге [Экзотический сад дома](http://exotic-garden-at-home.blogspot.com "Экзотический сад дома").
Поддержание температуры внутри гроубокса в заданом диапазоне температур осуществляется путем включение кулеров [статья](http://exotic-garden-at-home.blogspot.com/2013/09/grow-box-2.html "статья").

