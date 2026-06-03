#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS A0

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures();

  float tempC = sensors.getTempCByIndex(0);

  Serial.print("Temperatura: ");
  Serial.print(tempC);
  Serial.println(" °C");

  delay(1000);
}