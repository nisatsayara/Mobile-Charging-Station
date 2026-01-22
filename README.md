
# Mobile Charging Station

## 📌 Project Overview

This repository contains the complete implementation of the **Mobile Charging Station** project developed as part of the **ETE 320 – Microprocessor and Microcontroller Sessional** course at the Department of Electronics and Telecommunication Engineering, Chittagong University of Engineering & Technology (CUET).

The project focuses on designing a **coin-based multi-port mobile charging system** with optional **RFID-based authenticated access**, intended for use in public environments such as universities, transportation hubs, and commercial areas.

---

## 🧠 Abstract

The increasing demand for public mobile charging facilities requires solutions that ensure controlled usage, secure access, and efficient power management. This project presents a coin-based multi-port mobile charging station built around an **ESP32 DevKit microcontroller**. The system integrates a **CH616 multi-coin acceptor** for time-bound, coin-operated charging and an **RFID module** for authenticated access to a dedicated charging port. Relay-controlled USB charging ports enable multiple users to charge devices simultaneously, while an LCD interface provides real-time system feedback. The design is modular, scalable, and suitable for real-world deployment.

---

## 🎯 Objectives

* Design a controlled public mobile charging system
* Implement coin-operated, time-bound charging logic
* Integrate RFID-based authenticated charging access
* Enable independent control of multiple charging ports
* Ensure safe and efficient power delivery

---

## 🛠 Components Used

* **ESP32 DevKitC** – Main controller
* **CH616 Multi-Coin Acceptor** – Coin detection and validation
* **4-Channel Relay Module** – Independent port switching
* **USB Charging Modules** – Regulated power output
* **RFID Module** – Secure authenticated access
* **LCD Display (I2C)** – System status and time display
* **Push Button** – Port selection and user input
* **Power Adapter** – AC to DC conversion

---

## ⚙️ System Operation

### Coin-Based Charging Mode

1. User inserts a valid coin.
2. Coin acceptor generates pulses detected by the ESP32.
3. User selects a charging port.
4. Corresponding relay activates and charging starts.
5. LCD displays remaining charging time.
6. Charging automatically stops when time expires.

### RFID-Based Charging Mode

* Authorized RFID card activates a dedicated charging port.
* Provides secure, contactless access without coin insertion.

---

## 🧪 Simulation & Implementation

* **Simulation Platform**: Wokwi (coin-based logic)
* Verified correct sequencing of coin detection, port selection, timing, and automatic shutdown.
* **Practical Implementation**: Hardware prototype using ESP32, relay modules, coin acceptor, LCD, and charging ports.

---

## 📂 Repository Structure

```
Mobile-Charging-Station/
│
├── Code/
│   └── ESP32 Firmware
│
├── Circuit Diagram/
│   ├── Coin-Based Charging
│   └── RFID-Based Charging
│
├── Simulation/
│   └── Wokwi Files
│
├── Report/
│   └── Project Report (PDF)
│
└── README.md
```

---

## ⚠️ Limitations

* Possible false coin pulses due to mechanical or electrical noise
* Relay-based switching introduces mechanical wear
* Fixed charging current (no adaptive power management)
* No digital or mobile payment integration
* Breadboard-based prototype not suitable for long-term deployment

---

## 🔮 Future Improvements

* Integration of mobile financial services (MFS)
* Smart power management and adaptive charging
* Solid-state switching instead of mechanical relays
* Enclosed, rugged hardware design
* IoT-based monitoring and analytics

---

## 👩‍💻 Team Members

* **Wahida Zannat (2108003)** – Project Lead, Firmware & Integration
* **Puspita Barua (2108007)** – Hardware Implementation & Testing
* **Nisat Sayara (2108041)** – Documentation & Circuit Debugging
* **Maowa Alam Khan (2108045)** – Hardware Setup & Testing
* **Nabila Sultana (2108060)** – Programming Support & Presentation

---

## 📚 Course Information

* **Course Code**: ETE 320
* **Course Title**: Microprocessor and Microcontroller Sessional
* **Department**: Electronics and Telecommunication Engineering
* **University**: Chittagong University of Engineering & Technology

---

## 📜 License

This project is developed for academic purposes. Reuse is allowed with proper attribution.
