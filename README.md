# Bidirectional Visitor Counter with Automatic Appliance Control

![Platform](https://img.shields.io/badge/Platform-Arduino-blue)
![Status](https://img.shields.io/badge/Status-Active-green)
![License](https://img.shields.io/badge/License-MIT-orange)

## 📖 Project Overview
This project is an embedded system designed for smart building automation and energy conservation. It utilizes an **Arduino UNO** and dual **Infrared (IR) sensors** to accurately count the number of people entering and exiting a room through a single doorway.

Beyond simple counting, the system functions as an **Automatic Room Controller**. It monitors occupancy in real-time:
* **Occupied State:** When visitors are present (Count > 0), the system automatically activates the room's appliances (Fan and Lights).
* **Empty State:** When the last person leaves (Count reaches 0), the system automatically cuts power to the appliances and triggers an audio alert, ensuring zero energy wastage.

## ✨ Key Features
* **Bidirectional Counting:** Intelligent logic distinguishes between entry and exit movements using dual-sensor sequencing.
* **Real-Time Dashboard:** Displays the current visitor count and system status on a 20x4 I2C LCD screen.
* **Smart Automation:** Controls high-current loads (via L293D Driver) based on live occupancy data.
* **Audio Feedback:** Provides distinct buzzer alerts for entry detection, exit detection, and "Room Empty" shutdown events.
* **Cost-Effective:** Built using standard, widely available components.

## 🛠️ Hardware Requirements
To replicate this project, you will need the following components:

* **Microcontroller:** Arduino UNO (or compatible board)
* **Sensors:** 2x IR Obstacle Avoidance Sensor Modules
* **Display:** 20x4 LCD with I2C Serial Interface Adapter
* **Motor Driver:** L293D IC (for controlling DC fans)
* **Actuators:**
    * DC Motor (Fan)
    * LED (simulating Room Light)
    * Piezo Buzzer
* **Power:** 9V Battery (for the Motor Driver) & USB Cable (for Arduino logic)
* **Misc:** Breadboard, Jumper Wires, 10k Potentiometer

## 🔌 Pin Configuration
Wire your components to the Arduino according to this mapping to match the project logic:

| Component | Pin Label | Arduino Pin | Function |
| :--- | :--- | :--- | :--- |
| **IR Sensor 1** | OUT | `D2` | Entry Detection (Outer Sensor) |
| **IR Sensor 2** | OUT | `D3` | Exit Detection (Inner Sensor) |
| **L293D Driver** | Input 1 | `D8` | Fan Control Signal A |
| **L293D Driver** | Input 2 | `D9` | Fan Control Signal B |
| **Buzzer** | + | `D10` | Audio Alert Output |
| **LED** | Anode | `D11` | Light Control Output |
| **LCD Display** | SDA | `A4` | I2C Data Line |
| **LCD Display** | SCL | `A5` | I2C Clock Line |

> **⚠️ Wiring Tip:** Ensure that the Ground (GND) pin of the Arduino is connected to the negative terminal of the 9V battery (Common Ground).

## 💻 Software Dependencies
You will need the **Arduino IDE** to upload the code. Additionally, the following library is required:
* **LiquidCrystal_I2C** by Frank de Brabander (Available in the Arduino Library Manager).

## 🚀 Usage Guide
1.  **Mount the Sensors:** Place Sensor 1 on the outside of the door frame and Sensor 2 on the inside. Ensure they are aligned and not facing direct sunlight.
2.  **Power Up:** Connect the battery and the Arduino. The LCD should initialize.
3.  **Entry:** Walk past Sensor 1 then Sensor 2. The count will increase, and the fan/light will turn ON.
4.  **Exit:** Walk past Sensor 2 then Sensor 1. The count will decrease.
5.  **Shutdown:** When the count reaches 0, the appliances turn OFF and the buzzer sounds a shutdown tone.

## 📄 License
This project is open-source and available under the MIT License.