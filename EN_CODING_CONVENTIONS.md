# Coding Conventions - Gadget V1 (EN) 📝

Coding standards to maintain the cleanliness and stability of the Gadget V1 firmware.

## 🏷️ Naming
- **Class Managers**: Use PascalCase with the suffix `Manager`. Example: `DisplayManager`, `WiFiRepeaterManager`.
- **Global Objects**: Use camelCase. Example: `displayManager`, `rfManager`.
- **Files**: Filenames must match the class name. Example: `IRManager.h` & `IRManager.cpp`.
- **States**: Use SCREAMING_SNAKE_CASE with the prefix `STATE_`. Example: `STATE_MAIN_MENU`.

---

## 🏗️ Manager Class Structure
Each manager should have at least:
- **`begin()`**: Initial hardware initialization.
- **`update()`**: Main loop logic.
- **`stop()`**: Resource cleanup before switching modules.

---

## 🚫 Forbidden Patterns (Anti-Patterns)
1. **NO delay()**: Never use `delay()`. Use `millis()`-based timers instead.
2. **NO String Fragmentation**: Avoid using the `+` operator on `String` objects within the main loop. Use the `F()` macro for static text.
3. **NO Blocking Loops**: Avoid `while(true)` or long loops that do not call `yield()`. These will trigger a Watchdog Timer (WDT) reset.

---

## 📺 Rendering Style
- Always call `display.display()` at the end of a module's render function.
- Use *Partial Redraw*: Only update the parts of the screen that change to save CPU cycles.
- Use the `F()` macro for every text string printed to the OLED: `display.print(F("Text"));`.

---

## 💾 Memory-Safe Patterns
- **Flash Memory**: Store large data tables or static text in Flash using `PROGMEM`.
- **Heap Memory**: Avoid dynamic allocation (`new` / `malloc`) inside the loop. Allocate objects statically as manager class members.
