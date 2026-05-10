# Expressive Robot Eyes (ESP8266)

Cute OLED eyes with sound effects, button control, auto-demo, and a boot-time resource monitor. Built for Wemos D1 Mini (ESP8266) with a 128x64 SSD1306 display and a piezo buzzer.

## Hardware
- MCU: Wemos D1 Mini (ESP8266)
- Display: SSD1306 128x64 I2C @ 0x3C (SDA=D2, SCL=D1)
- Buzzer: D5
- Button: D6 (INPUT_PULLUP)
- Power: 5V USB (OLED on 3.3V logic)

## Features
- 16 eye expressions with smooth animation: idle, happy, laugh, thinking, cool, sleepy, love (hearts), wink, shock, angry, sad, confused, dizzy, clown, bored, stone (moai bitmap).
- Sound effects via `BuzzerFX` matched to expressions.
- Manual control: button cycles expressions.
- Auto-demo: serial `a` toggles looping through all expressions.
- Direct serial select: send `0-9` to jump to an expression index.
- Boot resource monitor on OLED: RAM and Flash usage bars with percentages.
- Non-blocking updates: animations and buzzer run in loop without delays (except short boot display).

## Pinout (default)
- OLED SDA → D2
- OLED SCL → D1
- Buzzer → D5
- Button → D6 (pull-up enabled)

## Build & Upload (PlatformIO)
1. Open the folder in VS Code with PlatformIO.
2. Ensure USB port is set (e.g., `COM18`).
3. Task: `PlatformIO: Upload` (or `platformio run --target upload`).

## How to Use
- Power the board and OLED; on boot you will see the RAM/Flash bars for a moment, then the eyes start in idle.
- Button control: press the button on D6 to cycle expressions. Each press advances once. Pressing also disables auto-demo if it was on.
- Serial control (115200 baud):
  - Send `a` to toggle auto-demo (loop through all expressions).
  - Send digits `0-9` to jump directly to an expression index from the demo list.
- Sound plays automatically with each expression via the buzzer on D5.

## Using the Code (main.cpp)
- Expressions: call any eye expression directly, e.g.:
  ```cpp
  eyes.expr_angry();   // Angry face
  eyes.expr_happy();   // Happy face
  eyes.expr_stone();   // Moai stone mode
  eyes.expr_love();    // Hearts mode
  ```
- Boot resource screen: already shown in `setup()` via `ramMonitor.drawBootStats(&display);`. You can change the delay after it (currently 10s) or remove it if you want faster startup.
- Auto-demo: controlled by `autoMode` and the `demoSeq` list. You can reorder or edit `demoSeq` to change the default sequence and durations.
- Button behavior: wired to D6 with `INPUT_PULLUP`; a LOW press cycles expressions. Adjust `BUTTON_PIN` if you rewire.
- Serial control: handled in `loop()`—send `a` to toggle auto mode, `0-9` to jump to a specific `demoSeq` index. Extend this block to add more commands if needed.

### Extending `demoSeq`
Add a new entry to the `demoSeq` array in `main.cpp` with the expression function, a label, and a duration (ms):
```cpp
DemoExpr demoSeq[] = {
  // ...existing entries...
  {[](){ eyes.expr_angry(); }, "😡 Angry", 2000},
  {[](){ eyes.expr_stone(); }, "🗿 Stone Mode", 3000},
  {[](){ eyes.expr_love();  }, "😍 Love", 2000},
  // Add yours here:
  {[](){ eyes.expr_sleepy(); }, "😴 Sleepy", 2000}
};
```
Remember to update `DEMO_COUNT` if you add or remove items (it is currently derived from the array size, so it auto-updates).

## Runtime Controls
- Button (D6): advance to next expression; also exits auto-demo.
- Serial (115200):
  - `a` toggles auto-demo on/off.
  - `0-9` selects expression by index.

## Boot Display
- Shows two bars:
  - RAM: percentage of heap used.
  - Flash: percentage of flash used (sketch size / chip size).
- After the boot screen delay, eyes start in idle.

## Files of Interest
- `src/main.cpp` – wiring, setup, loop, button/serial control, demo list.
- `src/EyeAnimation.*` – eye expressions and drawing logic (includes moai bitmap & hearts).
- `src/BuzzerFX.*` – sound effect patterns.
- `src/seeRam.*` – boot resource monitor (RAM/Flash bars).

## Notes
- The RAM total is assumed as 81920 bytes (ESP8266 typical). Percentages use integer math.
- Keep the boot delay modest if you want faster startup; current code shows the monitor briefly, then proceeds.
