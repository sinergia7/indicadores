#define TURBIDITY_PIN A1
#define TDS_PIN A2
#define VREF 5.0      // Voltaje de referencia del Arduino Uno

// ================= VARIABLES TURBIDEZ =================
// Calibración
float vA, vB, vC;
float nA = 0;    // Agua limpia
float nB = 5;    // Ligeramente turbia
float nC = 10;   // Muy turbia

void setup() {
  Serial.begin(9600);

  Serial.println("=== Calibracion del sensor de turbidez ===");

  Serial.println("Coloca la muestra A (agua limpia) y presiona ENTER...");
  while (!Serial.available());
  while (Serial.available()) Serial.read();
  vA = medirVoltajeTurbidez();
  Serial.print("Voltaje muestra A: ");
  Serial.println(vA, 3);

  Serial.println("Coloca la muestra B (ligeramente turbia) y presiona ENTER...");
  while (!Serial.available());
  while (Serial.available()) Serial.read();
  vB = medirVoltajeTurbidez();
  Serial.print("Voltaje muestra B: ");
  Serial.println(vB, 3);

  Serial.println("Coloca la muestra C (muy turbia) y presiona ENTER...");
  while (!Serial.available());
  while (Serial.available()) Serial.read();
  vC = medirVoltajeTurbidez();
  Serial.print("Voltaje muestra C: ");
  Serial.println(vC, 3);

  Serial.println("\nCalibracion completada");
}

void loop() {
  // ================= MEDICION TURBIDEZ =================
  float voltajeTurbidez = medirVoltajeTurbidez();
  float turbidezNTU = calcularNTU(voltajeTurbidez);

  Serial.println("========================");
  Serial.println("--- TURBIDEZ ---");
  Serial.print("Voltaje: ");
  Serial.print(voltajeTurbidez, 3);
  Serial.println(" V");

  Serial.print("Turbidez: ");
  Serial.print(turbidezNTU, 2);
  Serial.println(" NTU");

  if (turbidezNTU < 1) {
    Serial.println("AGUA LIMPIA");
  }
  else if (turbidezNTU >= 1 && turbidezNTU <= 5) {
    Serial.println("PRECAUCION: LIGERA TURBIDEZ");
  }
  else {
    Serial.println("ALERTA: AGUA MUY TURBIA");
  }

  // ================= MEDICION TDS =================
  Serial.println("\n--- SOLIDOS DISUELTOS (TDS) ---");
  int analogValue = analogRead(TDS_PIN);

  float voltageTDS = analogValue * VREF / 1024.0;

  // Fórmula común para sensores TDS DFRobot
  float tdsValue = (133.42 * voltageTDS * voltageTDS * voltageTDS
                   - 255.86 * voltageTDS * voltageTDS
                   + 857.39 * voltageTDS) * 0.5;

  Serial.print("Voltaje: ");
  Serial.print(voltageTDS, 2);
  Serial.print(" V  ");

  Serial.print("TDS: ");
  Serial.print(tdsValue, 0);
  Serial.println(" ppm");

  Serial.println("========================\n");

  delay(2000);
}

// ================= FUNCIONES TURBIDEZ =================

float medirVoltajeTurbidez() {
  long suma = 0;

  for (int i = 0; i < 10; i++) {
    suma += analogRead(TURBIDITY_PIN);
    delay(50);
  }

  float promedio = suma / 10.0;
  return (promedio * 5.0) / 1023.0;
}

float calcularNTU(float voltage) {
  float ntu;

  if (voltage >= vA)
    ntu = nA;
  else if (voltage <= vC)
    ntu = nC;
  else if (voltage < vB && voltage > vC)
    ntu = mapFloat(voltage, vB, vC, nB, nC);
  else
    ntu = mapFloat(voltage, vA, vB, nA, nB);

  return ntu;
}

float mapFloat(float x, float in_min, float in_max,
               float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) /
         (in_max - in_min) + out_min;
}
