# CHANGELOG

## v2.7
### 2026-11-16 Show base MAC at startup
- The factory eFuse base MAC (the device's only unique ID on the classic ESP32) is read with `esp_efuse_mac_get_default()` and logged with `DBG()` in `setup()`, and shown on the boot screen below the version, for identifying units
- Boot label height is now `LV_SIZE_CONTENT` (was fixed 50 px) to fit the second line
- Holding any key keeps the boot screen up (timeout suspended from the first key event on); the final `KEY_RELEASED` goes to the main screen, so a short press still skips the delay. The switch now happens on release instead of on press, so the main screen no longer receives that press's release events

### 2026-11-16 Sensor input offset (PR #89)
- New per-channel `input_offset` setting ("Input Offset" spinbox, -99..99, 2 decimals), added to the sensor value before it goes into the PID; an unused `temp_offset_c` field was added to the settings struct as well
- Fix compile error on case-sensitive filesystems
- Settings format: still v6, but the settings struct grew without a settings-format bump: settings stored by an older firmware fail the blob-size check and defaults are loaded

## v2.6
### 2026-11-13 Fix lost key presses while settings are unsaved
- `MainScreen::loop()` cleared the border's HIDDEN flag every 100 ms while dirty; LVGL invalidates the full-screen border on every such call, so the whole screen was redrawn each loop and short key presses were missed (menu couldn't be reopened until the auto-save). The flag is now only changed when the dirty state changes

### 2026-11-13 Graph autoscale and setpoint range fixes
- `GraphPanel::autoScale()` only includes the setpoint when the channel uses it (`mode() > CONTROL_MODE_SENSOR`); sensor-only channels scale on their data
- `PIDLoop::begin()` clamps the stored setpoint to the input driver's `setpoint_min/max()`, so a driver-type change (e.g. temperature → pressure) doesn't leave an out-of-range setpoint

### 2026-11-13 Add BME280 input driver
- New local library [lib/lib-BME280](lib/lib-BME280) (TwoWireDevice-based, SHT3x style; forced mode by default, rejects BMP280 chip ID)
- New input drivers `BME280TDriver` / `BME280RHDriver` / `BME280PDriver` sharing one sensor (0x76, fallback 0x77), enabled by `INPUTDRV_BME280_ENABLED`
- The input driver runs the sensor in NORMAL mode (continuous, non-blocking reads): oversampling T x2 / P x1 / H x4, standby 125 ms, IIR filter x2
- New measurement type `TYPE_PRESSURE` (enum group 400, unit hPa, setpoint range 300..1100, 1 decimal, purple)

### 2026-04-22 — Merge lib-UnitSSR into lib-M5Unit-SSR; drop phantom Unit SSR I2C driver
- Library folder renamed to `lib-M5Unit-SSR`; old remote dep `lib-UnitSSR` removed from [platformio.ini](platformio.ini)
- `UnitSSR` + `UnitACSSR` classes collapsed into a single `M5UnitSSR` class covering Unit ACSSR / Unit DCSSR (0x50, with RGB LED / version / address-change)
- Removed `UnitSSRDriver`, `OUTPUT_DRIVER_SLOWPWM_M5SSR`, `OUTPUTDRV_UNITSSR_ENABLED`, and the `unitssr` union member — M5Stack has no I2C Unit SSR at 0x25 (the old "Unit SSR" BT136S is GPIO-only). Enum value 12 reserved in [src/outputdrv.h](src/outputdrv.h) so existing NVS settings for `M5ACSSR` (=13) stay stable; any device with the stale `M5SSR` value saved will fall through to `NoneOutputDriver` until reconfigured
- Menu label for the remaining driver changed from "Unit ACSSR" to "Unit AC/DC SSR"
- Settings format: v6 (output driver enum renumbered, handled by the version bump to 2.6 instead of a settings-format bump)

## v2.5
### 2026-04-21 Add M5Unit-Hbridge output driver
- New `lib-M5Unit-HBridge` (full-featured TwoWireDevice-based port of the M5Stack Unit H-Bridge: direction, 8/16-bit speed, PWM frequency, 8/12-bit ADC, motor current, I2C readdressing)
- New `M5UnitHbridgeDriver` output driver (SlowPWMBase-based) that uses the H-bridge as a switch via 0%/100% duty on `setSpeed8()`
- Wired into the Setup→Output menu alongside UnitSSR/UnitACSSR with I2C address + windowtime selectors

<!-- Entries below were reconstructed from the git history. Before v1.9 the version was not kept in
     version.h: the version string was the git branch plus build date. The numbers below come from the
     git tags (v2.0a, v2.1) and the branch name v1.8-LVGL8; v1.9/v1.10 came after them, so v2.x was taken
     and v1.10 was renumbered to v2.5. "Settings format" is the NVS settings data version (_data_version in settings.cpp). -->

## Previous versions
### 2026-04-20 — v1.10 — Output invert option (2026-04-20)
- New per-channel output *invert* setting for all output drivers (SlowPWM IO, FastPWM, GP8413, Unit SSR); changes the stored settings, hence the version bump
- SlowPWM output no longer runs in its own task; fixed on/off levels that were swapped
- Menu: a double press now also acts as a normal press; no "dirty" mark in the version string
- Settings format: still v6, but the stored output settings changed (invert added): settings from older firmware are not valid anymore

### 2026-04-17 — v1.9 — LVGL9 port, version from version.h (2026-03-30 .. 2026-04-17)
- First version number kept in [include/version.h](include/version.h) (`VERSION_MAJOR`/`VERSION_MINOR`), git branch and dirty state detected at build time, generated version header (`version_auto`)
- GUI ported from LVGL 8 to LVGL 9 (lib-soogh upstream), display double buffering
- All git submodules replaced by PlatformIO `lib_deps`

### 2026-04-07 — v1.8 — LVGL8 line (2023-09-07 .. 2026-04-07, branch v1.8-LVGL8)
- Input sensors: MCP9600 (thermocouple), MAX31865 (PT100), MLX90614 (IR, untested); sensors abstracted as `InputDriver`
- Outputs abstracted as `OutputDriver`, selectable in the menu: SlowPWM (IO, in a task), FastPWM, GP8413 DAC, M5Stack Unit SSR; min/max output limits
- Control modes (DISABLED / SENSOR / INACTIVE / PID / FIXED), fixed output value, lock window in percent, `STATUS_LOCKED_WAIT` and `STATUS_ERROR` (non-finite sensor value), PID scaled with the actual loop time
- BM8563 RTC with clock on the main screen and date/time menu; rudimentary serial CLI; orange border on the main screen while settings are unsaved
- Long press A/C toggles a channel, ABC enters (permanent) expert mode; double press is ESC in the menu
- WT32-SC01 device support; `-Wall -Werror`
- Settings format: v6 (2023-11-28: input sensor type stored in the settings)

### 2023-08-31 — v2.1 — CO2 sensor and input filters (tag v2.1, 2023-08-22 .. 2023-08-31)
- SprintIR CO2 sensor and M5Stack KMeter (thermocouple) inputs; sensor inputs abstracted
- PID input filter, D-term filter and feed-forward offset (lib-FPID)
- PID panel shows number and precision per sensor; default graph delta 100
- License changed to GPL
- Settings format: v5

### 2023-08-22 — v2.0a — DualPID (tag v2.0a, 2018-12-06 .. 2023-08-22)
- Started 2018 as an Arduino humidity PID controller; 2020 ported to PlatformIO / ESP32 on the M5Stack with an SHT30/SHT31 sensor and settings in EEPROM/NVS
- 2021: two independent PID channels, LVGL GUI (lib-soogh) with boot screen, main screen with status panels, output bars and graph, and a menu
- 2022: lib-FPID instead of MiniPID, graph autoscale and time axis, sensor error detection (outputs off, NaN shown), output pins, PID loop time and window time configurable in the menu, settings upgrade on version change
- 2023: LVGL 8.3, dynamic menus with expert mode, configurable graph speed, lock window and locked/error status colors, PID output normalized to 0..100 %, renamed to DualPID
- Settings format: v4
