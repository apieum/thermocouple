#include <ArduinoUnit.h>
#include <Thermo.h>

test(Thermocouple_TypeE_returns_coefs0_when_temp_is_lower_than_minus_100) {
    Thermocouple<TypeE, Celsius> thermo;
    bool are_equals = (thermo.coefs_from_temp(-100) == TypeE.hot[0]);
    assertTrue(are_equals);
}

test(Thermocouple_TypeE_returns_coefs4_when_temp_is_higher_than_max_temp) {
    Thermocouple<TypeE, Celsius> thermo;
    bool are_equals = (thermo.coefs_from_temp(1000.0001) == TypeE.hot[4]);
    assertTrue(are_equals);
}

test(Thermocouple_TypeE_returns_coefs0_when_temp_is_lower_than_min_temp) {
    Thermocouple<TypeE, Celsius> thermo;
    bool are_equals = (thermo.coefs_from_temp(-270.0001) == TypeE.hot[0]);
    assertTrue(are_equals);
}

test(coefs_from_temp_automatically_convert_unit) {
    Kelvin _temp = Celsius(10);
    Thermocouple<TypeE, Kelvin> thermo;
    bool are_equals = (thermo.coefs_from_temp(_temp) == TypeE.hot[1]);
    assertTrue(are_equals);
}

test(Thermocouple_TypeE_returns_coefs0_when_voltage_is_lower_than_coef1_minimum) {
    Thermocouple<TypeE, Celsius> thermo;
    bool are_equals = (thermo.coefs_from_volt(-5.237) == TypeE.hot[0]);
    assertTrue(are_equals);
}

test(Thermocouple_TypeE_returns_coefs4_when_voltage_is_higher_than_maximum) {
    Thermocouple<TypeE, Celsius> thermo;
    bool are_equals = (thermo.coefs_from_volt(76.3731) == TypeE.hot[4]);
    assertTrue(are_equals);
}

test(Thermocouple_TypeE_returns_coefs0_when_voltage_is_lower_than_minimum) {
    Thermocouple<TypeE, Celsius> thermo;
    bool are_equals = (thermo.coefs_from_volt(-9.836) == TypeE.hot[0]);
    assertTrue(are_equals);
}

test(it_converts_cold_junction_temperature_into_voltage) {
    Thermocouple<TypeE, Celsius> thermo;
    double voltage = floor(thermo.cold_temp_to_volt(68)*100 + 0.5)/100;
    assertEqual(4.20, voltage);
}

test(it_converts_hot_junction_voltage_into_temperature) {
    Thermocouple<TypeE, Celsius> thermo;
    double temperature = floor(thermo.hot_volt_to_temp(4.20)+0.5);
    assertEqual(68.0, temperature);
}

test(it_returns_hot_junction_temp_from_voltage_and_cold_junction_temp) {
    Thermocouple<TypeE> thermo;
    double temperature = floor(thermo.temperature(4.20, 25.0)+0.5);
    assertEqual(91.0, temperature);
}

void setup() {}


void loop() { Test::run(); }

