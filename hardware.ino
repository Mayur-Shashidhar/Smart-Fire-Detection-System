#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ================== WIFI ==================
const char* ssid = "Sam";
const char* password = "9108495939";
const char* serverURL = "http://10.235.184.5:3000/api/data";

// ================== DHT ==================
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define MQ135_PIN 34
#define TRIG_PIN 5
#define ECHO_PIN 18
#define PIR_PIN 19
#define BUZZER_PIN 23
#define TEMP_THRESHOLD     40.0
#define GAS_THRESHOLD      30
#define DISTANCE_THRESHOLD 150  


float lastValidTemp = 0.0;
float lastValidHumidity = 0.0;
int consecutiveDHTFails = 0;


void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(DHTPIN, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  dht.begin();
  delay(2000);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 20) {
    delay(500);
    Serial.print(".");
    wifiAttempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n WiFi Connected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n WiFi Failed — running offline");
  }
}

bool readDHT(float &temp, float &humidity) {
  for (int i = 0; i < 3; i++) {
    temp     = dht.readTemperature();
    humidity = dht.readHumidity();

    if (!isnan(temp) && !isnan(humidity)) {
      lastValidTemp       = temp;
      lastValidHumidity   = humidity;
      consecutiveDHTFails = 0;
      return true;
    }
    delay(500);
  }
  consecutiveDHTFails++;
  temp     = lastValidTemp;
  humidity = lastValidHumidity;
  Serial.println("DHT read failed (" + String(consecutiveDHTFails) + " times) — using last known values");
  return false;
}


long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}


void handleBuzzer(String priority) {
  if (priority == "HIGH") {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
  } else if (priority == "LOW") {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}


void sendData(float temp, float humidity, int gas, long distance, bool motion, bool occupied, String priority, int score) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected — attempting reconnect");
    WiFi.reconnect();
    delay(3000);
    return;
  }
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);
  String json = "{";
  json += "\"temperature\":" + String(temp, 1) + ",";
  json += "\"humidity\":" + String(humidity, 1) + ",";
  json += "\"gas\":" + String(gas) + ",";
  json += "\"distance\":" + String(distance) + ",";
  json += "\"motion\":" + String(motion ? "true" : "false") + ",";
  json += "\"occupied\":" + String(occupied ? "true" : "false") + ",";
  json += "\"priority\":\"" + priority + "\",";
  json += "\"score\":" + String(score);
  json += "}";
  Serial.println(" Sending: " + json);
  int res = http.POST(json);
  if (res > 0) {
    Serial.println("📡 Response: " + String(res));
    if (res == 400) Serial.println(" Server returned 400 — check server expects these fields");
  } else {
    Serial.println(" HTTP error: " + String(res));
  }
  http.end();
}


void loop() {
  float temp, humidity;
  bool dhtOk = readDHT(temp, humidity);
  int gasRaw     = analogRead(MQ135_PIN);
  int gasPercent = map(gasRaw, 0, 4095, 0, 100);
  long distance = getDistance();
  bool motion   = digitalRead(PIR_PIN);
  bool occupied = (distance < DISTANCE_THRESHOLD || motion);
  bool fire     = (temp > TEMP_THRESHOLD || gasPercent > GAS_THRESHOLD);
  if (!dhtOk && gasPercent <= GAS_THRESHOLD) fire = false;
  String priority = "SAFE";
  int score = 10;
  if (fire && occupied) {
    priority = "HIGH";
    score = 80;
  } else if (fire) {
    priority = "LOW";
    score = 50;
  }
  handleBuzzer(priority);
  Serial.println("----------- SENSOR READINGS -----------");
  Serial.println("Temp:     " + (dhtOk ? String(temp) + " °C" : "FAILED"));
  Serial.println("Humidity: " + (dhtOk ? String(humidity) + " %" : "FAILED"));
  Serial.println("Gas:      " + String(gasPercent) + " %");
  Serial.println("Distance: " + String(distance) + " cm");
  Serial.println("Motion:   " + String(motion));
  Serial.println("Occupied: " + String(occupied));
  Serial.println("Fire:     " + String(fire));
  Serial.println("Priority: " + priority);
  Serial.println("Score:    " + String(score));
  Serial.println("---------------------------------------");
  sendData(temp, humidity, gasPercent, distance, motion, occupied, priority, score);
  delay(3000);
}
