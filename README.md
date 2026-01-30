# Smart Home Arduino 🏠

Smart home controller with password entry, sensors, alerts, and servo-controlled door/window.

## Highlights ✨
- 🔐 Password-gated access
- 🚪 Smooth door servo control
- 🧯 Gas alert with fan + relay + buzzer
- 💡 Auto light control
- 🕵️ Motion alert melody
- 🌧️ Rain/steam auto window
- 📟 LCD status rotation

## Quick Setup ⚡
1. Open `code/Smart_home/Smart_home.ino` in Arduino IDE.
2. Install `Servo`, `Wire`, `LiquidCrystal_I2C`.
3. Select board/port and upload.

## Pins 🔌
Digital: D2 PIR, D3 buzzer, D4 password, D5 relay, D6/D7 fan, D8 enter, D9 door servo, D10 window servo, D12 yellow LED, D13 white LED  
Analog: A0 gas, A1 light, A2 soil, A3 steam

## Codes 🎹
- `..` activate + open door

## Notes 📝
- LCD clears often (minor flicker).

More details for developers: `code/README.md`.
