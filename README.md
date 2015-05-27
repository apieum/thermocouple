# Thermocouple
Arduino lib for computing thermocouples measures and compensate cold junction voltage based on NIST tables and [mosaic industries work](http://www.mosaic-industries.com/embedded-systems/microcontroller-projects/temperature-measurement/thermocouple/microcontroller).


## Installation Arduino Ide

Download source:
```
$ git clone https://github.com/apieum/thermocouple.git
```

Open Arduino Ide, in menu Sketch -> import lib click on "add library", select thermocouple folder (containing sources).

Then in menu sketch -> import lib you should see "thermocouple" entry.

## Usage:

Add the following at the begining of your sketch:

```
#include <Thermo.h>

```


Defaults units are millivolts and degree Celsius (Kelvin also available).
To initialise a thermocouple you have to specify its type and optionnaly temperature unit.

*Set a type E thermocouple with kelvin units*
```
Thermocouple<TypeE, Kelvin> thermo;

```
*Set a type E thermocouple with default unit (Celsius)*
```
Thermocouple<TypeE> thermo;

```

Then to compute temperature from voltage (mV) with cold junction compensation (20.0°c by default) call method temperature(double voltage, double cold_junction_temperature=20.0)

```
Thermocouple<TypeE> thermo;
double temp = thermo.temperature(4.20, 25.0);
Serial.print("temperature: ");
Serial.println(temp);
# print something like:
temperature: 91.0

```

If you don't need cold junction compensation call hot_volt_to_temp(double voltage)

```
Thermocouple<TypeE> thermo;
double temp = thermo.hot_volt_to_temp(4.20);
Serial.print("temperature: ");
Serial.println(temp);
# print something like:
temperature: 68.0

```

## Development:

To develop you need at least arduinounit and ino.
I compile tests locally to have rapid feedback if you need help about it contact me.

*Install ino:*

```
$ sudo pip install ino
```

*Use the file make.py to build and upload tests:*

```
$ ./make.py build-tests
$ ./make.py upload-tests

```




