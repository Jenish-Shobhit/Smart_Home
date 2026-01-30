# Smart Home Code Guide ✨

## Big Picture 🌟
- **Goal**: secure entry + automated home responses.
- **Inputs**: PIR, gas, light, soil, steam sensors + 2 buttons.
- **Outputs**: LCD, buzzer, LEDs, fan/relay, door and window servos.

## Sketch Flow 🧭
1. **Boot** → LCD shows “Enter Password”.
2. **Password mode** → dots collected, submit on Enter.
3. **Active mode** → sensors update, alarms react, LCD rotates, motion alerts.

## Inside the Code 🧩

### 1) Pin + Threshold Map (top of file)
- All wiring and tuning live here.
- Edit `LIGHT_THRESHOLD`, `GAS_THRESHOLD`, `SOIL_THRESHOLD`, `STEAM_THRESHOLD` to fit your sensors.

### 2) `setup()`
- Starts LCD, sets pin modes, attaches servos.
- Door and window start closed.
- LCD prints password instructions.

### 3) `loop()`
- Always calls `handlePassword()` for input.
- When active:
  - **Sensors** refresh every 500 ms (`updateSensors()`).
  - **Alarms** run right after readings (`handleAlarms()`).
  - **LCD** refreshes every 1000 ms (`updateDisplay()`).
  - **Motion** is checked continuously (`handleMotion()`).

## Core Functions Explained 🪄

### `handlePassword()`
- BTN1 short press adds a dot (`.`).
- BTN2 submits the code.
- Codes:
  - `..` → activate system + open door.
  - `...` → play Birthday melody.
  - `---` → play Ode to Joy.
  - `##` → close door (reserved).
- Success and error tones give instant feedback.

### `updateSensors()`
- Reads analog sensors and PIR.
- Stores values in a single `sensors` struct.

### `handleAlarms()`
- **Gas high**:
  - Yellow LED on, fan + relay on, buzzer alert.
  - LCD switches to alert state.
- **Low light**:
  - White LED turns on automatically.
- **Steam/Rain high**:
  - Window closes smoothly.
  - Reopens when steam drops below `STEAM_THRESHOLD - 100`.

### `updateDisplay()`
Rotates 6 screens:
1. Gas value
2. Light value + low warning
3. Motion status
4. Soil value + watering advice
5. Door open/closed
6. Steam value + rain warning

If `alarmActive`, LCD shows the alert screen instead.

### `handleMotion()`
- Triggers only on PIR state change.
- Plays a short melody on detection.
- 3-second cooldown to avoid spam.

## Quick Tuning 💡
- **Thresholds**: change near the top of the file.
- **Servo angles**: edit the door/window loops to fit your mechanics.
- **LCD I2C address**: update `LiquidCrystal_I2C lcd(0x27, 16, 2);` if needed.

## Known Quirks ⚠️
- `##` is coded but `#` input is not currently possible with dot-only entry.
- `PIR_DELAY` is defined but unused.
- Frequent `lcd.clear()` can cause flicker.
