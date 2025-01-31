# Adaptive Headlight System for Bikes  

## Overview  
This project implements an **Adaptive Headlight System** for motorcycles, where the headlight adjusts its direction based on the bike's handlebar movement. The system enhances visibility and safety, especially during turns.  

## Features  
- **GPS Tracking:** Tracks the real-time location of the bike.  
- **GSM Messaging:** Sends notifications via SMS.  
- **Microcontroller:** Powered by **Arduino Uno**.  
- **Headlight Control:** A **beam light** attached to a **servo motor** adjusts its direction.  
- **Gyro-Based Rotation Detection:** Uses a **GY-50 Gyroscope sensor** to detect handlebar rotation.  

## Components Used  
- **Arduino Uno** (Microcontroller)  
- **GY-50 Gyroscope Sensor** (Detects handle movement)  
- **GPS Module** (Tracks location)  
- **GSM Module** (Sends SMS alerts)  
- **Servo Motor** (Adjusts headlight direction)  
- **Beam Light** (Primary headlamp)  

## Working Principle  
1. The **GY-50 gyro sensor** detects the bike handle's rotation.  
2. The **Arduino Uno** processes this data and controls the **servo motor**, adjusting the headlight’s direction accordingly.  
3. The **GPS module** tracks the bike’s location.  
4. The **GSM module** sends location-based alerts via SMS if necessary.  

## Applications  
- Improved night-time visibility on curved roads.  
- Enhanced rider safety by ensuring the headlight points in the travel direction.  
- Can be extended for use in autonomous or smart vehicles.  

## Author  
**Gagpa**  

---
