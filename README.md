# 🚒 Smart Fire Detection & Rescue Prioritization System

## 📌 Overview

This project implements an **intelligent fire detection and rescue prioritization system** using an **ESP32 microcontroller** and multiple sensors.

Unlike traditional systems that rely only on smoke detection, this system uses **multi-sensor fusion** to:

* Detect fire conditions more accurately
* Identify human presence
* Assign **priority levels** for rescue operations

The system can operate in both:

* 🌐 **Online mode** (IoT-based monitoring via server)
* 🔌 **Offline mode** (standalone hardware alerts)

---

## 🎯 Key Features

* 🔥 Multi-sensor fire detection (Temperature + Gas)
* 👤 Human presence detection (PIR + Ultrasonic)
* 🚨 Priority-based alert system (HIGH / LOW / SAFE)
* 🔔 Buzzer alerts based on severity
* 🌐 IoT data transmission using HTTP (ESP32 WiFi)
* 🛡️ Fault-tolerant DHT readings (retry + fallback)
* ⚡ Real-time monitoring via Serial + Dashboard

---

## 🧠 System Architecture

```text
Sensors → ESP32 → Decision Logic → Alerts + IoT Transmission
```

### Logic Flow:

1. Read sensor data
2. Detect fire conditions
3. Detect human presence
4. Assign priority
5. Trigger buzzer + send data

---

## 🔌 Hardware Components

| Component          | Purpose                        |
| ------------------ | ------------------------------ |
| ESP32              | Main microcontroller           |
| DHT11              | Temperature & humidity sensing |
| MQ-135             | Gas / smoke detection          |
| HC-SR04            | Distance measurement           |
| PIR Sensor         | Motion detection               |
| Buzzer             | Alert system                   |
| Breadboard & Wires | Circuit connections            |

---

## 🔧 Circuit Diagram

<img width="960" height="1280" alt="WhatsApp Image 2026-04-21 at 12 22 13" src="https://github.com/user-attachments/assets/a018c0fc-9526-4b5d-b4a0-32ffb9f64f64" />

---

## ⚙️ Pin Configuration

| Sensor          | ESP32 Pin |
| --------------- | --------- |
| DHT11           | GPIO 4    |
| MQ-135 (Analog) | GPIO 34   |
| HC-SR04 TRIG    | GPIO 5    |
| HC-SR04 ECHO    | GPIO 18   |
| PIR Sensor      | GPIO 19   |
| Buzzer          | GPIO 23   |

---

## 🧪 Working Principle

### 🔥 Fire Detection

* Temperature > 40°C
* Gas level > threshold

### 👤 Occupancy Detection

* Distance < 150 cm
* OR motion detected

---

### 🚨 Priority Logic

```text
IF fire AND occupied → HIGH PRIORITY
IF fire ONLY         → LOW PRIORITY
ELSE                 → SAFE
```

---

## 🧾 Code Overview

The system integrates:

* Sensor reading with validation
* Fault-tolerant DHT handling
* Gas normalization (0–100%)
* Distance calculation using ultrasonic
* Priority computation
* HTTP-based IoT communication

---

## 🌐 IoT Integration

* ESP32 connects to WiFi
* Sends JSON data to backend server

### Sample Payload:

```json
{
  "temperature": 32.5,
  "humidity": 45,
  "gas": 60,
  "distance": 80,
  "motion": true,
  "occupied": true,
  "priority": "HIGH",
  "score": 80
}
```

---

## 🧪 Testing & Results

### Test Cases Performed:

| Test               | Expected Result |
| ------------------ | --------------- |
| Normal environment | SAFE            |
| Gas only           | LOW             |
| Motion only        | SAFE            |
| Fire + human       | HIGH            |

### Observations:

* MQ-135 requires ~90s warm-up
* DHT occasionally fails → handled via fallback logic
* Ultrasonic provides stable occupancy detection

---

## 📊 Output

### Serial Monitor:

```
Temp: 30°C
Gas: 45%
Distance: 60 cm
Occupied: YES
Priority: HIGH
```

### Hardware Output:

* 🔴 HIGH → Rapid buzzer alert
* 🟠 LOW → Slow buzzer
* 🟢 SAFE → No alert

---

## 🚀 Setup Instructions

### 1. Install Requirements

* Arduino IDE
* ESP32 Board Package
* DHT Library (Adafruit)

### 2. Configure Code

Update the following in the source file:

```cpp
const char* ssid      = "WiFi Name";
const char* password  = "WiFi Password";
const char* serverURL = "Live URL";
```

### 3. Upload Code

* Select Board: **ESP32 Dev Module**
* Select the correct **Port**
* Click **Upload**

### 4. Run System

* Open Serial Monitor at **115200 baud**
* Observe live sensor readings

---

## ⚠️ Limitations

* MQ-135 readings vary with environment
* No camera-based verification
* Limited to single-node deployment

---

## 🔮 Future Enhancements

* 📷 Camera + AI fire detection
* 🗺️ Multi-room monitoring system
* 📱 Mobile app integration
* ☁️ Cloud deployment (no IP dependency)
* 🔔 SMS/Email alerts

---

## 👨‍💻 Contributors

* Hardware Integration
* Embedded Systems Development
* IoT & Dashboard Design

---

## 📌 Conclusion

This project demonstrates a **practical, scalable, and intelligent fire safety system** that improves upon conventional approaches by incorporating:

* Multi-sensor fusion
* Human-aware prioritization
* Real-time IoT monitoring

It provides a strong foundation for **smart building safety systems** and future AI-integrated emergency response solutions.
