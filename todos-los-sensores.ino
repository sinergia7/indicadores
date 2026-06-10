/*
 * SISTEMA UNIFICADO DE MEDICIÓN DE INDICADORES DE AGUA
 * Combina medición de: Turbidez, Sólidos Disueltos (TDS) y pH
 * 
 * Pines utilizados:
 * A0: Sensor de pH
 * A1: Sensor de Turbidez
 * A2: Sensor de TDS (Sólidos Disueltos)
 */

// ================= DEFINICIONES DE PINES Y CONSTANTES =================
#define TURBIDITY_PIN A1
#define TDS_PIN A2
#define PH_PIN A0
#define VREF 5.0      // Voltaje de referencia del Arduino Uno

// ================= VARIABLES TURBIDEZ =================
float vA, vB, vC;
float nA = 0;    // Agua limpia
float nB = 5;    // Ligeramente turbia
float nC = 10;   // Muy turbia

// ================= VARIABLES pH =================
float calibration_value = 21.34;
unsigned long int avgval;
int buffer_arr[10], temp;

void setup() {
  Serial.begin(9600);

  Serial.println("\n=== SISTEMA UNIFICADO DE MEDICIÓN DE AGUA ===");
  Serial.println("Calibrando sensores...\n");

  // ================= CALIBRACIÓN TURBIDEZ =================
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
  Serial.println("\nIniciando mediciones continuas...\n");
}

void loop() {
  // ================= MEDICIÓN TURBIDEZ =================
  float voltajeTurbidez = medirVoltajeTurbidez();
  float turbidezNTU = calcularNTU(voltajeTurbidez);

  // ================= MEDICIÓN TDS =================
  int analogValueTDS = analogRead(TDS_PIN);
  float voltageTDS = analogValueTDS * VREF / 1024.0;
  float tdsValue = (133.42 * voltageTDS * voltageTDS * voltageTDS
                   - 255.86 * voltageTDS * voltageTDS
                   + 857.39 * voltageTDS) * 0.5;

  // ================= MEDICIÓN pH =================
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(PH_PIN);
    delay(30);
  }

  // Ordenamiento para filtrar ruido
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  avgval = 0;
  for (int i = 2; i < 8; i++) {
    avgval += buffer_arr[i];
  }

  float voltPH = (float)avgval * 5.0 / 1024.0 / 6.0;
  float phActual = -5.70 * voltPH + calibration_value;

  // ================= MOSTRAR RESULTADOS =================
  Serial.println("========================================");
  Serial.println("--- TURBIDEZ ---");
  Serial.print("Voltaje: ");
  Serial.print(voltajeTurbidez, 3);
  Serial.println(" V");
  Serial.print("Turbidez: ");
  Serial.print(turbidezNTU, 2);
  Serial.println(" NTU");

  if (turbidezNTU < 1) {
    Serial.println("Estado: AGUA LIMPIA");
  }
  else if (turbidezNTU >= 1 && turbidezNTU <= 5) {
    Serial.println("Estado: PRECAUCION - LIGERA TURBIDEZ");
  }
  else {
    Serial.println("Estado: ALERTA - AGUA MUY TURBIA");
  }

  Serial.println("\n--- SOLIDOS DISUELTOS (TDS) ---");
  Serial.print("Voltaje: ");
  Serial.print(voltageTDS, 2);
  Serial.println(" V");
  Serial.print("TDS: ");
  Serial.print(tdsValue, 0);
  Serial.println(" ppm");

  Serial.println("\n--- pH ---");
  Serial.print("Voltaje: ");
  Serial.print(voltPH, 3);
  Serial.println(" V");
  Serial.print("pH: ");
  Serial.println(phActual, 2);

  Serial.println("========================================\n");

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
