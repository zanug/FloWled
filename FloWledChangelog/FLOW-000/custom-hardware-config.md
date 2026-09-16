# Ticket: Custom Hardware Config via `my_config.h`

## Goal
Define all custom-hardware pins in **one place** so WLED uses them instead of its
defaults. Chosen mechanism: **`../../wled00/my_config.h`** (compile-time defaults).
YAML source-of-truth idea deferred for now.

**Target chip:** ESP32-WROOM-32E (GPIO16 free — no PSRAM conflict, SD CS pin is fine).

## How it works
WLED does not read `.yml`. It configures via two layers:
1. **Compile-time defaults** — `../../wled00/my_config.h` (`#define`s), auto-included via
   `WLED_USE_MY_CONFIG` (`-D` flag already set in `platformio.ini`). `#ifndef`-guarded,
   so our values cleanly replace the built-in defaults.
2. **Runtime `cfg.json`** — on device flash (LittleFS). Read on boot and **overrides
   compile-time defaults** for any present key. Regenerated on any UI settings change.

So `my_config.h` sets the defaults that apply on a fresh device / after factory reset;
once you change settings in the UI, `cfg.json` wins on subsequent boots.

## Pin map (implemented in `my_config.h`)
| Function            | GPIO | Define(s) in my_config.h                        | Layer |
|---------------------|------|-------------------------------------------------|-------|
| LED output 1        | 4    | `PIN_LED_OUTPUT_1` → `DATA_PINS`               | core |
| LED output 2        | 5    | `PIN_LED_OUTPUT_2` → `DATA_PINS`               | core |
| Status LED          | 2    | `PIN_STATUS_LED` → `STATUSLED`                 | core |
| SD card MOSI        | 23   | `PIN_SPI_MOSI` → `SPIMOSIPIN`                  | core |
| SD card MISO        | 19   | `PIN_SPI_MISO` → `SPIMISOPIN`                  | core |
| SD card SCLK        | 18   | `PIN_SPI_SCLK` → `SPISCLKPIN`                  | core |
| SD card CS          | 16   | `PIN_SD_CS`                                     | SD usermod |
| I2C SDA             | 21   | `PIN_I2C_SDA` → `I2CSDAPIN`                    | core |
| I2C SCL             | 22   | `PIN_I2C_SCL` → `I2CSCLPIN`                    | core |
| I2S mic WS (LRCLK)  | 25   | `PIN_MIC_WS` → `I2S_WSPIN`                     | audioreactive |
| I2S mic SCK (BCLK)  | 26   | `PIN_MIC_SCK` → `I2S_CKPIN`                    | audioreactive |
| I2S mic SD (data)   | 27   | `PIN_MIC_SD` → `I2S_SDPIN`                     | audioreactive |
| Battery gauge (ADC) | 32   | `PIN_BATTERY_ADC` → `USERMOD_BATTERY_..._PIN` | Battery usermod |
| Accelerometer INT   | 14   | `PIN_ACCEL_INT` (reserved)                      | future usermod |

The advisor's I²S reading is correct: WS/SCK/SD (25/26/27) = digital microphone,
NOT the MicroSD card. MicroSD is the SPI group (23/19/18 + CS 16). Two different
"SD" meanings in the source table.

## Notes / dependencies
- **Mic pins** only take effect when the **audioreactive** usermod is compiled in.
- **Battery pin** only takes effect when the **Battery** usermod is compiled in.
- **Accelerometer** (I2C + INT 14): no native firmware. Pin reserved; needs a custom
  usermod later. Its data/clock ride the shared I2C bus (SDA 21 / SCL 22).
- GPIO16 (SD CS): fine on WROOM-32E (WROVER PSRAM conflict does not apply).
- GPIO2 status LED: boot strapping pin, fine as an output indicator.

## Deferred
- `custom_hardware.yml` + generator script → skipped for now (may revisit).
- Preloaded `cfg.json` generation → skipped; defaults live in `my_config.h`.

## Status
`../../wled00/my_config.h` created and ready to compile. Usermod-dependent pins require
the respective usermods to be enabled in the build.
