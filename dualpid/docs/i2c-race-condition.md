# I2C Race Condition: Problem Analysis and Solutions

## Problem Description

`SlowPWMBase` (in `src/outputdrv.cpp`) spawns a FreeRTOS task pinned to the alternate CPU core. This task drives the relay by calling the virtual `_on()` / `_off()` methods on a tight timing loop using `delay()`. For GPIO-based outputs (`SlowPWMDriver`) this is fine. But two output drivers call I2C from within this task:

- `UnitSSRDriver::_on/off()` → `_ssr.setRelay()` → I2C write
- `UnitACSSRDriver::_on/off()` → `_ssr.setRelay()` → I2C write
- `GP8413Driver::set()` → `_dac.setOutput()` → I2C write (called from main loop, but still concurrent)

Meanwhile, the **main loop on core 0** runs input drivers that also use the same I2C bus:

- `SHT31TDriver` / `SHT31RHDriver` → SHT3x I2C reads
- `MCP9600Driver` → MCP9600 I2C reads
- `MLX90614Driver` → MLX90614 I2C reads
- `SprintIRDriver` → SprintIR reads
- `M5KMeterDriver` → M5KMeter reads

The ESP32's I2C peripheral is not reentrant. Concurrent access from two cores/tasks produces bus collisions, corrupted transfers, and hard-to-reproduce sensor read failures.

### Relevant files

- `src/outputdrv.h` / `src/outputdrv.cpp` — SlowPWMBase task, UnitSSRDriver, UnitACSSRDriver, GP8413Driver
- `src/inputdrv.h` / `src/inputdrv.cpp` — all sensor drivers
- `src/pidloop.cpp` — main control loop, calls `do_input()` and `do_output()` from main loop
- `src/main.cpp` (or equivalent) — main loop: button scan → LVGL tick → PID loops → settings save
- `lib/lib-TwoWireDevice/src/TwoWireDevice.cpp` — base class for all I2C devices

---

## Option A — FreeRTOS mutex inside TwoWireDevice

Add a static `SemaphoreHandle_t` mutex to `TwoWireDevice`. Every `writereg` / `readreg` call takes the mutex before the transfer and releases it after. Completely transparent to all callers — no changes above `TwoWireDevice`.

**Pros:**
- Minimal change — one file touched (`TwoWireDevice.cpp/.h`)
- Fixes all current and future I2C users automatically
- No changes to application architecture

**Cons:**
- The SlowPWM task blocks on the mutex if a sensor read is in progress, introducing jitter on relay toggles. For windows of 1–60 s this is irrelevant; for a 1 s window it could matter
- If a sensor read hangs (missing device, bus stuck), the SlowPWM task blocks indefinitely — `xSemaphoreTake` must use a timeout
- Does not fix the underlying architectural mismatch (I2C from two contexts)

---

## Option B — State flag: SlowPWM task sets flag, main loop performs I2C

The SlowPWM task no longer calls `_on()` / `_off()` directly. Instead it sets an atomic `_desired_state` boolean. The main loop (already on core 0 alongside all sensor I2C) reads that flag each cycle and calls the relay / DAC method.

**Pros:**
- All I2C traffic stays on one core — zero locking needed
- Small, contained change to `SlowPWMBase` and the main loop (`do_output()` or a new `tick()`)

**Cons:**
- Relay toggle is delayed by up to one main-loop cycle (100 ms). For a 1 s window this is 10% duty-cycle error; for longer windows it's negligible
- Adds a `tick()` call to the main loop that must be remembered for every future I2C output driver
- Still uses the SlowPWM FreeRTOS task (just strips the I2C call out of it)

---

## Option C — Dedicated I2C command queue (producer-consumer)

Introduce a FreeRTOS queue. I2C-bound output drivers post write commands (`{addr, reg, value}`) to the queue instead of writing directly. A consumer (a dedicated I2C task or the main loop) drains the queue and executes writes sequentially. Input reads can go through the same queue (with a response slot) or be done directly when the queue is idle.

**Pros:**
- Fully serialized I2C — scales well as more devices are added
- Clean separation of I2C scheduling from application logic

**Cons:**
- Significant complexity — input drivers need either a blocking wait-for-response pattern or an async callback model
- Adds latency to sensor reads (they queue behind pending writes)
- Overengineered for the current scale of this application

---

## Option D — millis()-based state machine in main loop; no task for I2C outputs

Replace the FreeRTOS task + `delay()` pattern with a millis()-based state machine in `do_output()`, identical in spirit to how `_next_pid` / `_next_input` are tracked in `pidloop.cpp`. No FreeRTOS task at all for I2C-based drivers. GPIO-based `SlowPWMDriver` keeps its task.

**Pros:**
- No concurrency for I2C outputs — zero race conditions by construction
- Right abstraction for relay/DAC: they don't benefit from sub-100 ms precision
- Eliminates the FreeRTOS task overhead for I2C drivers

**Cons:**
- Main-loop jitter (LVGL rendering time is variable) maps directly to PWM duty-cycle error. For a 5 s window, a 100–200 ms late toggle is 2–4% error; for a 60 s window it's negligible; for a 1 s window it becomes significant
- Requires two code paths: millis() state machine for I2C outputs, FreeRTOS task for GPIO — `SlowPWMBase` hierarchy gets split
- Medium refactor scope

---

## Option E — Remove the FreeRTOS task entirely (all outputs driven from main loop)

Same as D but applied to GPIO SlowPWM as well. All output toggling is done with a millis()-based state machine from the main loop. No FreeRTOS task at all.

**Pros:**
- Simplest code — no RTOS task management
- I2C stays on one core by construction
- Eliminates all task-related overhead and stack usage

**Cons:**
- **GUI jitter causes timing error for all slow PWM outputs.** LVGL rendering is variable; this was the original motivation for the dedicated task. Already observed in practice — jitter was significant enough to motivate the task approach
- For GPIO SlowPWM with short window times (1–2 s), timing quality degrades noticeably
- `delay()` cannot be used (would freeze LVGL) — requires millis() state machine throughout
- Not viable as a general replacement for the current task approach given observed GUI latency

---

## Option F — Move entire PID control loop to a dedicated FreeRTOS task on core 1

Restructure the main loop so that **core 0 only handles GUI + button scan**, and **core 1 runs a single "control task"** that executes at fixed `looptime` intervals: read sensors → run PID → write output. All I2C traffic (both sensor reads and output writes) lives in this one task. The existing per-channel SlowPWM sub-tasks are folded into the control task or replaced with a tight timing loop within it.

**Pros:**
- All I2C in one task — sensor reads and output writes are sequential by construction, zero bus collisions, zero mutex needed
- PID timing is fully decoupled from LVGL jitter — the control loop gets the precision the original SlowPWM task was designed for
- Matches the standard embedded architecture pattern: control task on one core, UI task on the other
- SlowPWM timing can live inside the control task (tighter loop than main-loop granularity)
- Eliminates the per-channel SlowPWM sub-tasks (folded in)

**Cons:**
- Largest refactor — the main loop currently owns the PID call sequence (`pidloop.loop()` etc.)
- **Settings must be shared safely** between GUI task (core 0, writes from menu) and control task (core 1, reads every iteration). Mitigation: control task re-reads settings at start of each iteration; a single `memcpy` under a lightweight mutex on menu close is sufficient
- **LVGL graph updates** (pushing new data points from PID output) currently happen inside the control path; LVGL is not thread-safe. Mitigation: small lock-free ring buffer or a flag that core 0 checks each tick to pull new data points
- The existing SlowPWM tasks are pinned to the alternate core; if the control task is also on core 1, those sub-tasks must be consolidated or removed

---

## Option G — Hardware timer interrupt drives SlowPWM toggles

Replace the FreeRTOS `delay()`-based task with an ESP32 hardware timer ISR. The timer is programmed with the next toggle time and fires at that exact moment. The ISR either acts directly or signals something to act.

The ESP32 has four hardware timers, none of which are used by this project (no WiFi/BT).

### What an ISR can and cannot do

**GPIO outputs** (`SlowPWMDriver`): `digitalWrite()` is interrupt-safe. The ISR can call it directly, with ~1 µs hardware accuracy, completely decoupled from the FreeRTOS scheduler and LVGL. This is the best possible timing quality.

**I2C outputs** (`UnitSSRDriver`, `UnitACSSRDriver`, `GP8413Driver`): the Wire library and I2C peripheral cannot be called from an ISR — it uses DMA and internal queues and is not reentrant. Calling it from an ISR will crash or corrupt the bus. The ISR must defer the I2C write via one of two mechanisms:

- **Set an atomic flag**, let the main loop or task pick it up — same execution as Option B, but the *intended* toggle time is captured precisely by the hardware
- **`xSemaphoreGiveFromISR()`** to wake a dedicated high-priority FreeRTOS task that immediately does the I2C write — the relay toggles within microseconds of the ISR, but the race condition with sensor reads on the same bus still exists unless combined with Option A (mutex) or Option F (control task)

### Timing quality for I2C outputs

The ISR fires at the exact right moment, but the relay toggles later — after the I2C transaction completes. Actual latency depends on the deferral method:

- Deferred to main loop: up to one loop iteration (~50 ms worst case) — identical execution to Option B, though the recorded intent time is precise
- Deferred to woken task + mutex (G+A): I2C write executes within ~1–2 ms of the ISR, bounded only by bus turnaround time

### ISR code requirements

ISR handlers on ESP32 must be placed in IRAM using the `IRAM_ATTR` attribute, otherwise a cache miss during interrupt handling causes a crash. This is a minor but easy-to-forget requirement.

**Pros:**
- GPIO toggle timing: hardware-accurate (~1 µs), completely immune to LVGL and FreeRTOS scheduler jitter — the best timing quality possible
- No FreeRTOS task and no `delay()` for GPIO outputs; lower stack and CPU overhead
- The ISR records the exact intended toggle time even for I2C outputs; combined with Option A the actual relay latency is bounded to bus turnaround (~1–2 ms) rather than the main loop period
- Straightforward to implement for GPIO; well-understood ESP-IDF pattern

**Cons:**
- **Does not fix the I2C race condition on its own** — must be combined with A (mutex), or F (control task) for I2C outputs
- I2C cannot be called from ISR context — GPIO and I2C outputs need different code paths (same split as Option D)
- ISR functions require `IRAM_ATTR`; forgetting it causes intermittent crashes
- If the I2C bus is busy when the ISR-woken task runs, the task still blocks on the bus, partially negating timing precision for I2C outputs

### Best combinations

- **G alone**: fixes GPIO timing only; I2C race condition unresolved
- **G + A**: hardware timer precision for all outputs + mutex safety for I2C; low refactor cost, good practical result
- **G + F**: ISR fires → notifies control task on core 1 which does I2C; precise timing and single-threaded I2C — the most complete solution

---

## GUI jitter — sources and magnitude

The main loop in `src/main.cpp` has no fixed period — no `delay()` or `vTaskDelay()` at the end. It runs as fast as its contents allow, and iteration time varies significantly.

### Dominant cost: `lv_timer_handler()`

`gui.loop()` calls `lv_timer_handler()` which, when LVGL has dirty regions, calls the flush callback `lv_disp_cb()`. This calls `_lgfx.pushImageDMA()` — a **synchronous, blocking** DMA transfer. `lv_disp_flush_ready()` is called only after DMA completes, so LVGL blocks the main loop for the entire transfer duration.

**M5Core buffer math:**
- `LV_BUF_SIZE = DISPLAY_HEIGHT × DISPLAY_WIDTH / 10` = 240 × 320 / 10 = 7,680 pixels = **15,360 bytes** (16bpp)
- Single buffer only (`SOOGH_DOUBLEBUF` not defined for M5Core target)
- SPI at 40 MHz: one buffer transfer ≈ 15,360 × 8 / 40,000,000 ≈ **3 ms** (transfer only; add CPU rendering time)
- Full-screen redraw: 10 cycles ≈ **30–50 ms**
- Partial redraw (labels only): 1–3 cycles ≈ **5–15 ms**

### `lv_timer_handler()` is called twice per main loop iteration

`gui.handle(e)` calls `gui.loop()` internally (`soogh-gui.cpp:91`), and `main.cpp` calls `gui.loop()` again on line 65. LVGL flushes up to twice per loop cycle — redundant work.

### Sensor reads add blocking I2C time

`pid->loop()` → `do_input()` does blocking I2C reads. SHT31 takes ~4–8 ms per read (I2C clock stretching). Two PID channels = up to **~15 ms** of blocking I2C per main loop cycle, interleaved with LVGL.

### Estimated jitter magnitude

| Condition | Approx. loop time |
|---|---|
| Mostly static screen, no graph update | 15–25 ms |
| Live value labels updating | 25–50 ms |
| Full screen transition / menu open | 50–100 ms |
| Menu open + spinbox active (LVGL animations) | 80–120 ms |

### Is GUI jitter fixable?

Yes, substantially. Four concrete improvements:

1. **Async DMA flush (biggest gain):** Call `lv_disp_flush_ready()` from the LovyanGFX DMA completion callback instead of inline. LVGL can then overlap CPU rendering of the next dirty region with the hardware transfer of the current one. LovyanGFX exposes a completion callback for this. Potential saving: **10–30 ms per frame**.

2. **Remove duplicate `gui.loop()` call:** The standalone `gui.loop()` on `main.cpp:65` is redundant — `gui.handle()` already calls it. One line removed.

3. **Enable double-buffering on M5Core:** `SOOGH_DOUBLEBUF` is already defined and used for WT32-SC01 but not M5Core. With double buffer, DMA sends buffer A while the CPU renders into buffer B. Requires testing for stability on M5Core.

4. **Option F (sensor reads off main loop):** Moving PID to core 1 removes the 5–15 ms I2C stall from the main loop entirely.

With fixes 1 + 2 + 4 combined, main loop jitter for a running PID screen (mostly static labels) could drop from ~50 ms to **~5–10 ms**, which makes Options D and E viable for window times ≥ 1 s.

---

## Comparison Summary

| | A | B | C | D | E | F | G (alone) | G+A |
|---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Fixes race condition | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✗ | ✓ |
| GPIO timing quality | ✓ task | partial | ✓ task | partial | ✗ GUI jitter | ✓ | ✓✓ best | ✓✓ best |
| I2C timing quality | ✓ task | partial | ✓ task | partial | ✗ | ✓ | partial | ✓ bounded |
| I2C on one thread | ✗ | ✓ | ✓ | ✓ | ✓ | ✓ | ✗ | ✗ |
| Refactor scope | tiny | small | large | medium | medium | large | medium | medium |
| Mutex / sync needed | in TwoWireDevice | none | queue | none | none | settings + graph | none | in TwoWireDevice |
| Eliminates SlowPWM task | ✗ | ✗ | ✗ | partial | yes | yes (folded in) | yes | yes |
| Viable for GPIO SlowPWM | ✓ | ✓ | ✓ | ✗ 2 paths | ✗ jitter | ✓ | ✓ | ✓ |

---

## Recommendation

**Short term:** Option A — add a mutex to `TwoWireDevice`. One file, immediate fix, no architecture changes. Use a timeout on `xSemaphoreTake` to avoid indefinite blocking if the bus hangs.

**Long term:** Option F — move the full PID control loop to core 1. This is the architecturally correct solution: all I2C in one task, precise timing independent of LVGL, and a clean GUI/control separation. The main cost is making the settings handoff (menu write → control task read) and the LVGL graph data updates thread-aware, both of which are well-understood patterns.

**For GPIO timing specifically:** Option G is a worthwhile independent improvement regardless of which I2C solution is chosen. G+A is a practical combination: hardware timer precision everywhere, mutex safety for I2C, modest refactor cost.

Option D is a reasonable intermediate step: no task for I2C-based outputs, keeps the task for GPIO. Acceptable if the window-time floor stays at ≥ 2 s, or after the GUI jitter fixes above are applied.

Option E is not recommended in its current form given the observed GUI jitter, but becomes viable if the async DMA flush and duplicate `gui.loop()` fixes are applied first.
