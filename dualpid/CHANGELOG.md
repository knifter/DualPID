# CHANGELOG

### 2026-11-16 — v2.7 — Show base MAC at startup
- The factory eFuse base MAC (the device's only unique ID on the classic ESP32) is read with `esp_efuse_mac_get_default()` and logged with `DBG()` in `setup()`, and shown on the boot screen below the version, for identifying units
- Boot label height is now `LV_SIZE_CONTENT` (was fixed 50 px) to fit the second line
- Holding any key keeps the boot screen up (timeout suspended from the first key event on); the final `KEY_RELEASED` goes to the main screen, so a short press still skips the delay. The switch now happens on release instead of on press, so the main screen no longer receives that press's release events

### 2026-11-13 — v2.6 — Fix lost key presses while settings are unsaved
- `MainScreen::loop()` cleared the border's HIDDEN flag every 100 ms while dirty; LVGL invalidates the full-screen border on every such call, so the whole screen was redrawn each loop and short key presses were missed (menu couldn't be reopened until the auto-save). The flag is now only changed when the dirty state changes

### 2026-11-13 — v2.6 — Graph autoscale and setpoint range fixes
- `GraphPanel::autoScale()` only includes the setpoint when the channel uses it (`mode() > CONTROL_MODE_SENSOR`); sensor-only channels scale on their data
- `PIDLoop::begin()` clamps the stored setpoint to the input driver's `setpoint_min/max()`, so a driver-type change (e.g. temperature → pressure) doesn't leave an out-of-range setpoint

### 2026-11-13 — v2.6 — Add BME280 input driver
- New local library [lib/lib-BME280](lib/lib-BME280) (TwoWireDevice-based, SHT3x style; forced mode by default, rejects BMP280 chip ID)
- New input drivers `BME280TDriver` / `BME280RHDriver` / `BME280PDriver` sharing one sensor (0x76, fallback 0x77), enabled by `INPUTDRV_BME280_ENABLED`
- The input driver runs the sensor in NORMAL mode (continuous, non-blocking reads): oversampling T x2 / P x1 / H x4, standby 125 ms, IIR filter x2
- New measurement type `TYPE_PRESSURE` (enum group 400, unit hPa, setpoint range 300..1100, 1 decimal, purple)

### 2026-04-22 — v2.6 — Merge lib-UnitSSR into lib-M5Unit-SSR; drop phantom Unit SSR I2C driver
- Library folder renamed to `lib-M5Unit-SSR`; old remote dep `lib-UnitSSR` removed from [platformio.ini](platformio.ini)
- `UnitSSR` + `UnitACSSR` classes collapsed into a single `M5UnitSSR` class covering Unit ACSSR / Unit DCSSR (0x50, with RGB LED / version / address-change)
- Removed `UnitSSRDriver`, `OUTPUT_DRIVER_SLOWPWM_M5SSR`, `OUTPUTDRV_UNITSSR_ENABLED`, and the `unitssr` union member — M5Stack has no I2C Unit SSR at 0x25 (the old "Unit SSR" BT136S is GPIO-only). Enum value 12 reserved in [src/outputdrv.h](src/outputdrv.h) so existing NVS settings for `M5ACSSR` (=13) stay stable; any device with the stale `M5SSR` value saved will fall through to `NoneOutputDriver` until reconfigured
- Menu label for the remaining driver changed from "Unit ACSSR" to "Unit AC/DC SSR"

### 2026-04-21 — v2.5 — Add M5Unit-Hbridge output driver
- New `lib-M5Unit-HBridge` (full-featured TwoWireDevice-based port of the M5Stack Unit H-Bridge: direction, 8/16-bit speed, PWM frequency, 8/12-bit ADC, motor current, I2C readdressing)
- New `M5UnitHbridgeDriver` output driver (SlowPWMBase-based) that uses the H-bridge as a switch via 0%/100% duty on `setSpeed8()`
- Wired into the Setup→Output menu alongside UnitSSR/UnitACSSR with I2C address + windowtime selectors
