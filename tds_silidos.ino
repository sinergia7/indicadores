#define TDS_PIN A2
#define VREF 5.0      // Voltaje de referencia del Arduino Uno
#define SCOUNT 30

void setup() {
  Serial.begin(9600);
}

void loop() {
  int analogValue = analogRead(TDS_PIN);

  float voltage = analogValue * VREF / 1024.0;

  // Fórmula común para sensores TDS DFRobot
  float tdsValue = (133.42 * voltage * voltage * voltage
                   -255.86 * voltage * voltage
                   +857.39 * voltage) * 0.5;

  Serial.print("Voltaje: ");
  Serial.print(voltage, 2);
  Serial.print(" V  ");

  Serial.print("TDS: ");
  Serial.print(tdsValue, 0);
  Serial.println(" ppm");

  delay(1000);
}
