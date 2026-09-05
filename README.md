# 🤖 Glove-Controlled Robotic Arm — Master-Slave Architecture

A real-time IoT-based robotic arm system that mirrors hand and wrist movements from a wearable sensor glove. The system uses ESP32 microcontrollers with MQTT communication to control 7 servo motors in real-time. Fully simulated and validated in **Wokwi** with transition to physical hardware using **ESP-NOW**.

---

## 📋 Overview

This project demonstrates a professional **master-slave architecture** where:
- **Master (Glove):** Reads sensor data (5 flex sensors + accelerometer) and transmits it
- **Slave (Arm):** Receives data and drives 7 servo motors
- **Communication:** MQTT over Wi-Fi (simulated), ESP-NOW (real hardware)

The system successfully mirrors finger, wrist, and elbow movements from the glove to the robotic arm in real-time.

---

## ✨ Key Features

✅ **Real-Time Motion Mirroring**
- 5 fingers (thumb, index, middle, ring, pinky)
- Wrist rotation control
- Elbow bend via accelerometer

✅ **Master-Slave Architecture**
- Genuinely independent circuits
- Wireless communication (MQTT in simulation, ESP-NOW planned for hardware)
- Scalable IoT-style design

✅ **Dual Simulation Stages**
- **Stage 1:** Combined concept in Tinkercad (Arduino Uno)
- **Stage 2:** Separated master-slave in Wokwi (ESP32 × 2)

✅ **Production-Ready Design**
- Validated communication protocol
- Sensor-to-servo mapping verified
- Pin architecture documented for hardware transition

---

## 🏗️ System Architecture

### Master Circuit (Glove) - ESP32
**Sensors:**
| Sensor | Pin | Purpose |
|--------|-----|---------|
| Thumb flex | GPIO 32 | Thumb bend |
| Index flex | GPIO 33 | Index bend |
| Middle flex | GPIO 34 | Middle bend |
| Ring flex | GPIO 35 | Ring bend |
| Pinky flex | GPIO 36 (VP) | Pinky bend |
| Wrist potentiometer | GPIO 39 (VN) | Wrist rotation |
| MPU6050 accelerometer | GPIO 21/22 (I2C) | Elbow angle |

**Communication:**
- Publishes JSON every 100ms: `{"a":[thumb, index, middle, ring, pinky, wrist, elbow]}`
- Uses MQTT protocol to broker.hivemq.com
- Wi-Fi enabled for real-time data transmission

### Slave Circuit (Arm) - ESP32
**Actuators:**
| Joint | Servo Motor | Pin |
|-------|-------------|-----|
| Thumb | SG90 | GPIO 13 |
| Index | SG90 | GPIO 12 |
| Middle | SG90 | GPIO 14 |
| Ring | SG90 | GPIO 27 |
| Pinky | SG90 | GPIO 26 |
| Wrist | SG90 | GPIO 25 |
| Elbow | SG90 | GPIO 33 |

**Power:**
- Servo power (V+) → ESP32 VIN (5V)
- All servo grounds → common ground with ESP32
- Rationale: 3V3 cannot supply 7 simultaneous servo loads

### Communication Protocol
```
Master publishes to: glove/arm/joints
Slave subscribes to: glove/arm/joints

Message format (JSON):
{
  "a": [angle1, angle2, angle3, angle4, angle5, angle6, angle7]
}
```

---

## 🔧 Hardware Components

**Master (Glove Circuit):**
- 1× ESP32 microcontroller
- 5× Flex sensors (or potentiometer stand-ins in simulation)
- 1× Potentiometer (wrist)
- 1× MPU6050 accelerometer/gyroscope
- Breadboard, jumper wires, USB power

**Slave (Arm Circuit):**
- 1× ESP32 microcontroller
- 7× SG90 servo motors
- Breadboard, jumper wires
- External 5V power supply (for servos)

**Simulation Tools:**
- **Wokwi** - Circuit simulation and testing
- **HiveMQ Public Broker** - MQTT message broker (broker.hivemq.com)
- **Arduino IDE** - Code development and upload

---

## 📁 Project Structure

```
robotic-arm/
├── README.md                 # Project documentation
├── .gitignore               # Git ignore file
├── master_glove/
│   ├── glove_master.ino     # Master circuit code
│   └── circuit_diagram.json # Wokwi circuit file
├── slave_arm/
│   ├── arm_slave.ino        # Slave circuit code
│   └── circuit_diagram.json # Wokwi circuit file
├── simulation_report.pdf    # Detailed project report
└── docs/
    ├── hardware_setup.md
    ├── communication_protocol.md
    └── future_upgrades.md
```

---

## 🚀 Getting Started

### Prerequisites
- ESP32 development boards (2)
- Arduino IDE or PlatformIO
- Wokwi account (free) for simulation
- Required libraries:
  - `PubSubClient` (MQTT)
  - `ArduinoJson` (JSON parsing)
  - `Adafruit_MPU6050` (accelerometer)

### Setup Instructions

#### 1. Simulation (Wokwi)

**Access the simulations:**
- Master circuit: [Wokwi Project](https://wokwi.com) - Search for "glove_master" by tamannameh23
- Slave circuit: [Wokwi Project](https://wokwi.com) - Search for "arm_slave" by tamannameh23

**Or create your own:**
1. Go to [wokwi.com](https://wokwi.com)
2. Create new ESP32 project
3. Add components:
   - For master: 5 potentiometers + MPU6050 + ESP32
   - For slave: 7 servo motors + ESP32
4. Wire according to pin diagrams above
5. Copy code from `master_glove.ino` and `arm_slave.ino`
6. Run simulation in Wokwi

#### 2. Real Hardware Setup

**Master (Glove):**
```bash
# Install required libraries
arduino-cli lib install PubSubClient
arduino-cli lib install ArduinoJson
arduino-cli lib install Adafruit_MPU6050

# Upload code
arduino-cli upload -b esp32:esp32:esp32 -p /dev/ttyUSB0 master_glove/glove_master.ino
```

**Slave (Arm):**
```bash
# Same libraries as master

# Upload code
arduino-cli upload -b esp32:esp32:esp32 -p /dev/ttyUSB0 slave_arm/arm_slave.ino
```

---

## 📊 How It Works

### Sensor Data Flow

```
Glove Sensors
    ↓
ADC Conversion (12-bit)
    ↓
Sensor Mapping (0-180° servo range)
    ↓
JSON Encoding
    ↓
MQTT Publish (every 100ms)
    ↓
HiveMQ Broker
    ↓
MQTT Subscribe
    ↓
JSON Parsing
    ↓
Servo Angle Mapping
    ↓
Servo Motor Control
    ↓
Robotic Arm Movement
```

### Calibration

**Flex Sensors → Servo Angles:**
```cpp
// Example mapping (adjust based on sensor range)
int sensorMin = 0;      // ADC reading when relaxed
int sensorMax = 4095;   // ADC reading when fully bent
int servoMin = 0;       // Servo angle when relaxed
int servoMax = 180;     // Servo angle when fully bent

int angle = map(sensorValue, sensorMin, sensorMax, servoMin, servoMax);
```

---

## 🧪 Testing & Validation

✅ **Master Circuit Tests:**
- All 7 sensors verified responsive via Serial Monitor
- MQTT connection to HiveMQ confirmed
- JSON message publishing every 100ms validated

✅ **Slave Circuit Tests:**
- Power and ground wiring verified
- Servo control signals confirmed on all 7 pins
- MQTT subscription and JSON parsing tested

✅ **End-to-End Integration:**
- Master and slave run simultaneously
- Sensor movement on glove produces proportional servo movement
- Real-time response confirmed with <200ms latency
- No signal loss or data corruption observed

---

## 🔮 Future Scope

### Hardware Upgrades
- [ ] Replace placeholder ADC calibration with real flex sensor min/max values
- [ ] Design and 3D print physical arm structure
- [ ] Source external regulated 5V power supply for servos
- [ ] Implement actual flex sensors with voltage divider circuits

### Communication Enhancement
- [ ] Replace MQTT/Wi-Fi with **ESP-NOW** for lower latency (peer-to-peer)
- [ ] Remove dependency on internet-connected broker
- [ ] Add data encryption for security

### Software Improvements
- [ ] Implement sensor smoothing/filtering to reduce servo jitter
- [ ] Add calibration routine for sensor range detection
- [ ] Implement gesture recognition (hand gestures → robotic arm movements)
- [ ] Add telemetry and logging for debugging

### Advanced Features
- [ ] Add force feedback from arm servos back to glove (haptic feedback)
- [ ] Implement trajectory planning for smooth arm movements
- [ ] Multi-user control (multiple gloves controlling one arm)
- [ ] Mobile app for remote control

---

## 📈 Project Evolution

### Stage 1: Conceptual Design (Tinkercad)
- Single combined circuit on Arduino Uno
- 5 flex sensors + 1 potentiometer → 6 servos
- Proof of concept for sensor-to-servo mapping
- ✅ Validated core idea

### Stage 2: Master-Slave Architecture (Wokwi)
- Two independent ESP32 circuits
- Separate sensor reading and servo control
- MQTT communication over Wi-Fi
- ✅ Validated independent operation
- ✅ Ready for physical hardware transition

---

## 📚 Technical Specifications

| Parameter | Value |
|-----------|-------|
| **Microcontroller** | ESP32 (2×) |
| **Operating Voltage** | 3.3V (ESP32), 5V (servos) |
| **ADC Resolution** | 12-bit (4096 levels) |
| **Communication Protocol** | MQTT (simulated), ESP-NOW (planned) |
| **Message Frequency** | 100ms (10 updates/second) |
| **Servo Range** | 0-180° |
| **Servo Control** | PWM (50Hz) |
| **Latency (Simulated)** | <200ms |
| **Total Joints** | 7 (5 fingers + wrist + elbow) |
| **Power Consumption** | ~2A (all servos active) |

---

## 🔌 Pin Reference

### Master (Glove) - ESP32
```
GPIO 32  → Thumb flex sensor (ADC)
GPIO 33  → Index flex sensor (ADC)
GPIO 34  → Middle flex sensor (ADC)
GPIO 35  → Ring flex sensor (ADC)
GPIO 36  → Pinky flex sensor (ADC)
GPIO 39  → Wrist potentiometer (ADC)
GPIO 21  → MPU6050 SDA (I2C)
GPIO 22  → MPU6050 SCL (I2C)
GND      → Common ground
3V3      → Sensor power
```

### Slave (Arm) - ESP32
```
GPIO 13  → Thumb servo (PWM)
GPIO 12  → Index servo (PWM)
GPIO 14  → Middle servo (PWM)
GPIO 27  → Ring servo (PWM)
GPIO 26  → Pinky servo (PWM)
GPIO 25  → Wrist servo (PWM)
GPIO 33  → Elbow servo (PWM)
GND      → Common ground
VIN      → Servo power (5V external)
```

---

## 🛠️ Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| Servos don't move | No Wi-Fi/MQTT connection | Check Wi-Fi credentials and broker URL |
| Glove sensors unresponsive | ADC pin not configured | Verify GPIO pins and ADC1 channels |
| Servo jitter | Sensor noise | Add smoothing filter in code |
| Power issues | Insufficient supply | Use external 5V PSU for servos |
| JSON parsing error | Malformed message | Check JSON format in master code |

---

## 📖 Documentation

- **Simulation Report:** `simulation_report.pdf` - Full project documentation with circuit diagrams
- **Hardware Setup Guide:** See `/docs/hardware_setup.md`
- **Communication Protocol:** See `/docs/communication_protocol.md`

---

## 🎓 Learning Outcomes

This project demonstrates:
- ✅ IoT master-slave architecture design
- ✅ Real-time sensor data acquisition and processing
- ✅ MQTT protocol implementation
- ✅ JSON message serialization/deserialization
- ✅ PWM servo motor control
- ✅ Multi-board wireless communication
- ✅ Circuit design and simulation
- ✅ Embedded systems integration

---

## 🤝 Contributing

This is an academic/portfolio project. Suggestions and improvements are welcome!

Feel free to:
- Fork the repository
- Improve the hardware design
- Optimize communication latency
- Add new features
- Submit pull requests

---

## 📄 License

This project is provided for educational purposes. Feel free to use, modify, and share.

---

## 👤 Author

**Tamanna Mehra**
- B.Tech Electronics & Communication Engineering (GGSIPU)
- B.S. Data Science (IIT Madras - Dual Degree)
- Embedded Systems & Robotics Enthusiast

---

## ⭐ Highlights

🌟 **Why This Project Stands Out:**
- Professional master-slave IoT architecture
- Real-time gesture mirroring with <200ms latency
- Fully validated in simulation before hardware transition
- Scalable design for expansion (more joints, force feedback)
- Complete documentation with circuit diagrams
- Perfect portfolio piece for robotics/IoT careers

---

## 📞 Support & Questions

For questions, issues, or suggestions, please open an issue on GitHub.

---

**Star this repository if you find it useful!** ⭐

*Last Updated: September 2026*
