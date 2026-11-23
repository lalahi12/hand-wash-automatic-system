// Kelompok 10

// =========================
// PIN DEFINISI
// =========================
const int sensor1 = 2;      // Sensor IR 1 (Untuk Trigger Relay 1)
const int sensor2 = 9;      // Sensor IR 2

const int relay1  = 3;      // Relay 1 (Pompa/IN1) - Nyala 200 detik
const int relay2  = 8;      // Relay 2
const int relay3  = 11;     // Relay 3 (KIPAS) - Ultrasonik

const int trigPin = 13;     // Ultrasonic Trigger
const int echoPin = 12;     // Ultrasonic Echo

// =========================
// KONFIGURASI PENGATURAN
// =========================
const int OBJECT_DETECTED = LOW;   
const int RELAY_ON  = HIGH;
const int RELAY_OFF = LOW;

// 1. PENGATURAN JARAK KIPAS (20 cm)
const int DISTANCE_THRESHOLD = 20; 

// 2. PENGATURAN DURASI RELAY 1
const unsigned long RELAY1_DURATION = 200; 

// =========================
// VARIABEL SISTEM
// =========================
bool lastSensor1State = HIGH;

// Variabel untuk Timer Relay 1 (Non-blocking)
unsigned long relay1StartTime = 0;
bool isRelay1On = false;

void setup() {
  Serial.begin(9600);

  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(relay1, RELAY_OFF);
  digitalWrite(relay2, RELAY_OFF);
  digitalWrite(relay3, RELAY_OFF);

  Serial.println("=== Sistem Aktif ===");
  Serial.println("Note: Relay 1 akan menyala 200 detik tanpa mengganggu Kipas.");
}

void loop() {
  unsigned long currentMillis = millis(); // Ambil waktu saat ini

  // =========================
  // 1. LOGIKA SENSOR IR 1 (TIMER 200 DETIK)
  // =========================
  int baca1 = digitalRead(sensor1);

  // Jika mendeteksi tangan (dari tidak ada menjadi ada) DAN Relay sedang mati
  if (lastSensor1State == HIGH && baca1 == OBJECT_DETECTED) {
    if (!isRelay1On) {
      Serial.println("Sensor 1 TERDETEKSI -> Relay 1 ON (Mulai 200 Detik)");
      digitalWrite(relay1, RELAY_ON);
      relay1StartTime = currentMillis; // Catat waktu mulai
      isRelay1On = true;
    }
  }
  lastSensor1State = baca1;

  // Cek apakah durasi 200 Milli detik sudah habis?
  if (isRelay1On) {
    if (currentMillis - relay1StartTime >= RELAY1_DURATION) {
      Serial.println("Waktu Habis -> Relay 1 OFF");
      digitalWrite(relay1, RELAY_OFF);
      isRelay1On = false;
    }
  }

  // =========================
  // 2. SENSOR IR 2 -> RELAY 2
  // =========================
  int baca2 = digitalRead(sensor2);
  if (baca2 == OBJECT_DETECTED) {
    digitalWrite(relay2, RELAY_ON);
  } else {
    digitalWrite(relay2, RELAY_OFF);
  }

  // =========================
  // 3. ULTRASONIK -> KIPAS (RELAY 3)
  // =========================
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 25000); 
  int distance = (duration > 0) ? duration * 0.034 / 2 : 999;

  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.print(" cm | Relay 1 Status: ");
  Serial.println(isRelay1On ? "ON (Hitung Mundur)" : "OFF");

  // Logika Kipas (Jarak <= 20 cm)
  if (distance <= DISTANCE_THRESHOLD) {
    digitalWrite(relay3, RELAY_ON);
  } else {
    digitalWrite(relay3, RELAY_OFF);
  }

  delay(100);
}