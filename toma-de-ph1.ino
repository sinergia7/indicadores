/*
 * Medidor de pH – TODOMICRO
 * 
 * Con este programa vas a poder trabajar con el módulo medidor de pH.
 * Recuerda instalar las librerías Wire.h y LiquidCrystal_I2C.h
 * 
 * Todomicro, la tecnología en tus manos.
 */
float calibration_value = 21.34;
unsigned long int avgval;
int buffer_arr[10], temp;

void setup() {
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(A0);
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

  float volt = (float)avgval * 5.0 / 1024.0 / 6.0;
  float ph_act = -5.70 * volt + calibration_value;

  Serial.print("Voltaje: ");
  Serial.print(volt, 3);
  Serial.print(" V   pH: ");
  Serial.println(ph_act, 2);

  delay(1000);
}