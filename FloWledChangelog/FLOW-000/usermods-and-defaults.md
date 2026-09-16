# Ticket: Enable Usermods + Custom Defaults (FloWled)

Follow-up to `TICKET-custom-hardware-config.md`. Adds usermod enablement and a set of
custom default settings. Target chip: **ESP32-WROOM-32E** (base env `esp32dev`).

## Summary of requested changes
1. Enable the **audioreactive** and **Battery** usermods.
2. Device/web name → **FloWled**; AP password → **flowled1234**.
3. Default **40 LEDs**, **one segment** over the whole strip, **mirror ON**.
4. **AP always on**, even when connected to WiFi.

## Where each setting lives (important: 3 different layers)
| Setting | Layer | File / key |
|---|---|---|
| Device name "FloWled" | compile-time | `my_config.h` → `SERVERNAME` |
| mDNS `flowled.local` | compile-time | `my_config.h` → `MDNS_NAME` |
| AP SSID/pass | compile-time | `my_config.h` → `WLED_AP_SSID` / `WLED_AP_PASS` |
| Default 40 LEDs | compile-time | `my_config.h` → `DEFAULT_LED_COUNT` |
| Usermod inclusion | build config | `platformio_override.ini` → `custom_usermods` |
| Usermod options | build config | `platformio_override.ini` → `build_flags -D...` |
| **AP always-on** | **runtime** | `cfg.json` → `ap.behav = 2` |
| **Segment mirror ON** | **compile-time (source edit)** | `FX.h` Segment constructor → `options( ... | MIRROR)` |
| LED bus pin + length | runtime (recommended) | `cfg.json` → `hw.led.ins[0]` |

Why the split: `apBehavior` (`wled.h:348`) and the segment `mirror` flag have **no
`#ifndef` override macro**. AP-always-on still ships as runtime JSON (`cfg.json`). Mirror
was instead made a compile-time default by editing the Segment constructor directly (see
below), so no boot preset is needed.

## Files created / changed
- **`../../wled00/my_config.h`** — added device identity, AP credentials, and `DEFAULT_LED_COUNT 40`.
- **`../../wled00/FX.h`** — Segment default constructor: added `MIRROR` to the `options(...)`
  initializer so every auto-created segment defaults to mirror ON. (Core source edit —
  see caveat below.)
- **`platformio_override.ini`** — new `[env:flowled]` extending `esp32dev`:
  - `custom_usermods = ${env:esp32dev.custom_usermods} audioreactive Battery`
  - `-D UM_AUDIOREACTIVE_ENABLE` (audioreactive on by default)
  - Battery pack tuning for single 18650 Li-Ion (see below).
- **`../../flowled-runtime-config/cfg.json`** — `ap.behav=2` (AP always on) + one LED bus on
  GPIO4 @ 40 LEDs. (No presets.json needed — mirror is now compile-time.)

## How to build & flash
1. **Firmware:** `pio run -e flowled` → flash the resulting binary.
2. **Runtime config:** on the fresh device, upload `cfg.json` from
   `../../flowled-runtime-config` via the device's `/edit` file manager (or bundle it into the
   LittleFS image). This applies AP-always-on and pins the LED bus to GPIO4 @ 40 LEDs.
   Mirror and LED count are already baked into the firmware.

## Battery usermod — options run-through (defaults in `../../usermods/Battery/battery_defaults.h`)
| Define | Default | Meaning |
|---|---|---|
| `USERMOD_BATTERY_MEASUREMENT_PIN` | 35 (we set **32**) | ADC pin reading the battery divider (use ADC1: GPIO32-39) |
| `USERMOD_BATTERY_INITIAL_DELAY` | 10000 ms | Wait after power-on before first reading (lets voltage settle) |
| `USERMOD_BATTERY_MEASUREMENT_INTERVAL` | 30000 ms | How often voltage is re-read |
| `USERMOD_BATTERY_DEFAULT_TYPE` | 0 | Chemistry: 0=unknown, 1=LiPo, 2=Li-Ion/18650 |
| `USERMOD_BATTERY_*_MIN/MAX_VOLTAGE` | 3.3/4.2 (unknown), 3.2/4.2 (LiPo), 2.6/4.2 (Li-Ion) | Empty(0%) / Full(100%) voltages per chemistry |
| `USERMOD_BATTERY_VOLTAGE_MULTIPLIER` | 2.0 (ESP32) | Divider ratio: scales pin voltage back to real battery voltage |
| `USERMOD_BATTERY_AVERAGING_ALPHA` | 0.1 | Smoothing (lower = smoother/slower) |
| `USERMOD_BATTERY_CALIBRATION` | 0 | Voltage offset for fine-tuning |
| `USERMOD_BATTERY_AUTO_OFF_ENABLED` | true | Auto master-off at low charge |
| `USERMOD_BATTERY_AUTO_OFF_THRESHOLD` | 10 % | Charge % at which LEDs auto-shutoff |
| `USERMOD_BATTERY_LOW_POWER_INDICATOR_ENABLED` | true | Low-battery visual warning |
| `USERMOD_BATTERY_LOW_POWER_INDICATOR_PRESET` | 0 | Preset played as the warning |
| `USERMOD_BATTERY_LOW_POWER_INDICATOR_THRESHOLD` | 20 % | Charge % that triggers the warning |
| `USERMOD_BATTERY_LOW_POWER_INDICATOR_DURATION` | 5 s | How long the warning shows |

### Battery values set for this build (single 18650 Li-Ion, 3.7V, BMS + 5V step-up)
- `USERMOD_BATTERY_MEASUREMENT_PIN=32` — ADC1 pin (AIN0).
- `USERMOD_BATTERY_DEFAULT_TYPE=2` — Li-Ion/18650 curve (2.6V empty / 4.2V full).
- `USERMOD_BATTERY_VOLTAGE_MULTIPLIER=2.0` — assumes a **halving divider** (e.g. two equal
  resistors like 100k/100k) so the 4.2V cell reads ~2.1V at the pin, safely under the
  ADC's ~3.3V limit. **Verify against your actual resistors** — if they aren't equal, this
  ratio is wrong and % will be off.
- `USERMOD_BATTERY_AUTO_OFF_THRESHOLD=10` — soft LED shutoff at 10% charge.

Important wiring notes for your topology:
- The gauge must measure the **raw 18650 cell voltage (3.0–4.2V) BEFORE the 5V step-up**,
  not the 5V rail. The step-up output is regulated and tells you nothing about charge.
- The **BMS** enforces the hard low-voltage cutoff to protect the cell; WLED's auto-off is
  a softer, earlier LED shutoff sitting above that — they don't conflict.

## Notes / caveats
- `esp32dev` env comment warns analog-mic audioreactive was removed on ESP32 V5 builds.
  You use a **digital I2S mic** (WS/SCK/SD 25/26/27) — unaffected.
- AP password must be ≥ 8 chars ("flowled1234" is fine). WLED forces `WLED_AP_SSID` to be
  set whenever `WLED_AP_PASS` is set (both are in `my_config.h`).
- `hw.led.ins[0].type = 22` = WS2812/SK6812. Change if your strip differs.
- Uploading `cfg.json`/`presets.json` overwrites existing device config — do it on a
  fresh device or when you intend to reset these settings.

## Caveat: core source edit
Mirror-on was done by editing `../../wled00/FX.h` (the Segment constructor) directly. This is a
change to WLED core, not an isolated override — it will show up as a local diff and may
need re-applying if you merge upstream WLED updates. All *new* segments will default to
mirrored (not just segment 0); disable per-segment in the UI if ever needed.

## Status
Complete. Battery vars finalized for single 18650 Li-Ion; mirror baked into firmware; LED
count 40 via my_config.h; AP-always-on via cfg.json. Next: build `-e flowled`, flash, and
upload `../../flowled-runtime-config/cfg.json`. Only open item: confirm the voltage-divider
resistors match the 2.0 multiplier.
