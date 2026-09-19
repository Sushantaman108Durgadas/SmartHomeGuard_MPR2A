<div align="center">

# 🏘️ Smart Home Guard

### *From a Semester Mini Project to a Smart Home Guard System*

**Embedded C++ • ESP32 • STM32 • Arduino IDE • Wokwi • RPi • Firebase • MIT App**

[![Stars](https://img.shields.io/github/stars/Sushantaman108Durgadas/SmartHomeGuard_MPR2A?style=for-the-badge)](https://github.com/Sushantaman108Durgadas/SmartHomeGuard_MPR2A/stargazers)
[![Forks](https://img.shields.io/github/forks/Sushantaman108Durgadas/SmartHomeGuard_MPR2A?style=for-the-badge)](https://github.com/Sushantaman108Durgadas/SmartHomeGuard_MPR2A/network)
[![Issues](https://img.shields.io/github/issues/Sushantaman108Durgadas/SmartHomeGuard_MPR2A?style=for-the-badge)](https://github.com/Sushantaman108Durgadas/SmartHomeGuard_MPR2A/issues)
[![License](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)](LICENSE)

**A Smart Home Guard System with Face Detection and MIT App Integration using OOPs and Finite State Machines (FSM)**

</div>

---

# 📖 Overview

This repository documents the complete evolution of our **Semester 5 Mini Project**.

This project began with a lot of pondering regarding sensor selections, system architecture, and hardware orchestration. We created a custom-made Magnetic Switch Sensor in Wokwi for simulation testing before deploying logic directly onto microcontrollers.

---

# 📌 System Architecture

The overall hardware and software system uses an event-driven flow across local state machines, connectivity modules, and computer vision integration:

![System Architecture](assets/architecture.png)

### Core Architectural Flow:
1. **Authorization Layer:** Raspberry Pi executes face detection/recognition pipeline and passes verification signals to the ESP32[cite: 1].
2. **Embedded Security Layer:** ESP32 reads real-time triggers (PIR, magnetic reed switch)[cite: 1], runs the core **Security State FSM**[cite: 1], and relays commands over **UART** to the STM32[cite: 1].
3. **Connectivity & Cloud Layer:** ESP32 handles telemetry to Firebase[cite: 1], while STM32 connects directly to the MIT App Inventor mobile application via Bluetooth[cite: 1].

---

# 🎬 Project Demo & Presentation


<video src="assets/project_presentation.mp4" controls width="100%"></video>

---

# 🚀 Project Evolution

| Version | Platform | Major Contribution |
| :--- | :--- | :--- |
| 🟢 Base Simulation | ESP32 Simulation on Wokwi | Door Security system using OOPs, FSM, and interrupts. Explored Door States & Transitions |
| 🔵 Hardware Implementation | STM32, ESP32 and RPi based Home Security System | Integrated MIT Bluetooth App, Firebase telemetry, and Raspberry Pi communication pipeline |
| 🟣 Advanced Simulation | ESP32 Simulation on Wokwi | Robust OOPs Architecture (Class-in-class structure) |

---

# 🧠 Engineering Evolution

```text
Base Simulation
│
├── OOPs, FSM, interrupts
├── Door States, Door Transitions
└── Custom Made messages for Transitions and States
        │
        ▼
Hardware Implementation
│
├── Wi-Fi and Bluetooth Connectivity
├── MIT App and Firebase Dashboard
├── Face Detection pipeline integration (In Progress)
└── MIT app control to monitor Home Security System
        │
        ▼
Advanced Simulation
│
├── More Robust OOPs Architecture
├── Emphasize on Home Security FSM logic
└── Custom Made messages for States

```

---

# 📁 Repository Directory Structure

```text
Directory structure:
└── sushantaman108durgadas-smarthomeguard_mpr2a/
    ├── README.md
    ├── LICENSE
    ├── adv-simul/
    │   ├── diagram.json
    │   ├── libraries.txt
    │   ├── reedswitch.chip.c
    │   ├── reedswitch.chip.json
    │   └── sketch.ino
    ├── base-simul/
    │   ├── diagram.json
    │   ├── reedswitch.chip.c
    │   ├── reedswitch.chip.json
    │   ├── sketch.ino
    │   └── wokwi-project.txt
    └── src/
        ├── ESP32_HomeGuard.ino
        └── STM32_HomeGuard.ino


```

---

# ✨ Features

## 🤖 Automation & Hardware

* Door opening detection via magnetic reed switch


* Motion and intruder detection using PIR sensor


* Inter-MCU UART communication between ESP32 and STM32


* Mobile application control via Bluetooth link



## 📡 IoT & Dashboards

* Real-time mobile interface with MIT App Inventor


* Telemetry syncing with Firebase Realtime Database



## 🧠 Intelligent Logic

* Object-Oriented C++ Architecture
* Finite State Machine (FSM) engine eliminating race conditions


* Hardware interrupt-driven state triggers



## 💻 Simulation

* Complete Wokwi simulation for hardware-independent verification

---

# 🚧 Roadmap

* [ ] **Raspberry Pi Face Recognition:** Completing local OpenCV/face-recognition script integration to send identity verification payloads to ESP32 via UART.



---

# ⚙ Technologies Used

### Hardware

* ESP32, STM32, Raspberry Pi, PIR Motion Sensor, Reed Switches, DHT Sensor



### Software & Protocols

* Embedded C++, Python, Arduino IDE, UART, Bluetooth, Wi-Fi



### Cloud & Mobile

* Firebase Realtime Database, MIT App Inventor



### Simulation

* Wokwi



---

# 🚀 Getting Started

1. Clone the repository:
```bash
git clone [https://github.com/Sushantaman108Durgadas/SmartHomeGuard_MPR2A.git](https://github.com/Sushantaman108Durgadas/SmartHomeGuard_MPR2A.git)
cd SmartHomeGuard_MPR2A

```


2. Open the desired version (`esp32` or `stm32`) inside **Arduino IDE**.
3. Install required libraries (e.g., DHT library, Firebase ESP Client).
4. Select appropriate board target (ESP32 or STM32) and flash the firmware.
5. Install `SMARTHOMEGUARD.apk` on your Android device to connect over Bluetooth.

---

# 👥 Team

- **Suyash Subodh Shirsat**
- **Devesh Vikrant Shelatkar**
- **Priyanka Amit Vaidya**

---


# 🤝 Contributing

Contributions are welcome.

If you'd like to improve the project,

1. Fork the repository
2. Create a new branch
3. Commit your changes
4. Push the branch
5. Open a Pull Request

---

# 📜 License

This project is licensed under the **MIT License**.

See the **LICENSE** file for details.

---

<div align="center">

### ⭐ If you found this project interesting, consider giving it a star!

*"Engineering is rarely about building the perfect system on the first try. It's about continuously improving each iteration."*

</div>
