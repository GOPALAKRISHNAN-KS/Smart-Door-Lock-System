# 🔐 Door Lock System Using STM32F446RE

A **password-protected door lock system** built using the **STM32F446RE microcontroller**, a **4x3 matrix keypad**, and an **LCD display**.  
When the **correct password** is entered, the **door opens**.  
When the **wrong password** is entered, the **door remains closed**.  
The system also allows the **user to change the password** directly from the keypad.

---

## 🧠 Project Overview

This project demonstrates an **embedded system** integrating keypad scanning, LCD interfacing, and password verification using the **STM32F446RE** board.

It’s a great example for learning:
- GPIO input/output configuration  
- LCD 16x2 interfacing (4-bit mode)  
- 4x3 keypad scanning technique  
- Simple password management logic  
- Register-level programming on STM32  

---

## ⚙️ Hardware Requirements

| Component | Description |
|------------|-------------|
| **STM32F446RE** | ARM Cortex-M4 based MCU (Nucleo or bare chip) |
| **Keypad (4x3)** | For password input |
| **LCD (16x2)** | To display user messages |
| **Power Supply (5V)** | For powering the circuit |
| **Door Control (LED / Relay / Motor)** | Output to simulate door status |

---

## 🧩 Pin Configuration

### 🔸 LCD (connected to GPIOA)
| LCD Pin | STM32 Pin | Description |
|----------|------------|-------------|
| D4 | PA0 | Data Line 4 |
| D5 | PA1 | Data Line 5 |
| D6 | PA2 | Data Line 6 |
| D7 | PA3 | Data Line 7 |
| RS | PA4 | Register Select |
| EN | PA5 | Enable |

### 🔸 Keypad (connected to GPIOB)
| Keypad Pin | STM32 Pin | Type |
|-------------|------------|------|
| R1–R4 | PB0–PB3 | Row Outputs |
| C1–C3 | PB4–PB6 | Column Inputs (Pull-Up Enabled) |

---

## 💡 Features

✅ Enter a 4-digit password to unlock the door  
✅ LCD shows `Password:` and displays `*` for each entered digit  
✅ Press `*` → Verify password  
✅ Press `#` → Enter new password mode  
✅ Displays `OPEN` or `CLOSED` states  
✅ Shows `"Wrong Try"` for incorrect entries  
✅ Supports password change functionality  

---

## 🧮 Default Logic

| Function | Key |
|-----------|-----|
| **Enter password** | Type digits (0–9) |
| **Check password** | Press `*` |
| **Change password** | Press `#` → Enter new password → Press `#` again to save |

- **Default Password:** `1234`  
- **Initial Door State:** Closed  

---

## 🧰 Software Requirements

- **STM32CubeIDE** or **Keil µVision**  
- **STM32 HAL Drivers** *(optional — not used here, this project uses direct register access)*  
- **ST-Link Utility** *(for flashing)*  

---

## 🧾 Working Principle

1. System initializes LCD and keypad.  
2. LCD prompts: `Password:`  
3. User enters the password using keypad.  
4. If password matches → LCD displays `OPEN`.  
5. If wrong → LCD displays `Wrong Try`.  
6. Press `#` to change password → enter new digits → press `#` again to save.  
7. Door state toggles between open/closed when the correct password is entered.

---
