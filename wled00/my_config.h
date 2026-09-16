#pragma once

/*
 * my_config.h — Custom hardware pin & settings definitions for THIS board.
 *
 * Target chip: ESP32-WROOM-32E
 *
 * This file sets WLED's COMPILE-TIME defaults. Each value below is guarded by an
 * #ifndef inside WLED, so anything defined here cleanly replaces the built-in default.
 *
 * IMPORTANT precedence note:
 *   These are DEFAULTS only. WLED reads a runtime "cfg.json" from flash on boot, and any
 *   value present there OVERRIDES the defaults below. On a fresh device (or after a factory
 *   reset) these values apply. Once you change settings in the web UI, cfg.json wins.
 *
 * DO NOT edit my_config_sample.h — edit THIS file (my_config.h).
 */

// Show a build-time confirmation that this file is actually compiled in.
#warning **** my_config.h: custom hardware pin map is being applied ****


// =============================================================================
//  DEVICE IDENTITY & ACCESS POINT  (core WLED)
// =============================================================================
// Device / UI name shown in the web interface ("server description").
#define SERVERNAME         "FloWled"
// mDNS hostname -> reachable at http://flowled.local
#define MDNS_NAME          "flowled"
// Fallback hotspot the device creates for setup / when AP is open.
//   IMPORTANT: WLED requires WLED_AP_SSID to be set whenever WLED_AP_PASS is set,
//   otherwise the build errors out — so both are defined together here.
//   AP password must be at least 8 characters.
#define WLED_AP_SSID       "FloWled-AP"
#define WLED_AP_PASS       "flowled1234"
// NOTE: "AP always on (even when connected to WiFi)" has NO compile-time knob.
//       It is set at runtime via cfg.json -> ap.behav = 2. See the shipped
//       cfg.json and the TICKET for details.


// =============================================================================
//  DEFAULT LED STRIP LENGTH  (core WLED)
// =============================================================================
// Default number of LEDs on a fresh device / after factory reset.
// (DEFAULT_LED_COUNT is a bare #define in const.h, so #undef first to avoid a
//  redefinition warning.)
#undef  DEFAULT_LED_COUNT
#define DEFAULT_LED_COUNT  40
// NOTE: a single segment covering all 40 LEDs is created automatically, but the
//       "mirror ON" default cannot be set here — it ships via presets.json as a
//       boot preset. See the TICKET.


// =============================================================================
//  ADDRESSABLE LED OUTPUTS  (core WLED)
// =============================================================================
// Two NeoPixel/addressable data outputs. WLED creates one digital bus per pin.
//   LED output 1 -> GPIO 4
//   LED output 2 -> GPIO 5
#define PIN_LED_OUTPUT_1   4
#define PIN_LED_OUTPUT_2   5

// DATA_PINS is the comma-separated list WLED expands into the default bus pins.
#define DATA_PINS          PIN_LED_OUTPUT_1, PIN_LED_OUTPUT_2


// =============================================================================
//  ONBOARD STATUS LED  (core WLED)
// =============================================================================
// Small onboard indicator LED. Defining STATUSLED compiles in status-LED support.
//   Status LED -> GPIO 2   (also a boot strapping pin; fine as an output indicator)
#define PIN_STATUS_LED     2
#define STATUSLED          PIN_STATUS_LED


// =============================================================================
//  I2C BUS  (core WLED — shared by accelerometer / other I2C sensors)
// =============================================================================
//   SDA (data)  -> GPIO 21
//   SCL (clock) -> GPIO 22
#define PIN_I2C_SDA        21
#define PIN_I2C_SCL        22
#define I2CSDAPIN          PIN_I2C_SDA
#define I2CSCLPIN          PIN_I2C_SCL


// =============================================================================
//  SPI BUS — MicroSD card  (core WLED defines the bus; SD usermod uses CS)
// =============================================================================
//   MOSI -> GPIO 23   (data ESP32 -> device)
//   MISO -> GPIO 19   (data device -> ESP32)
//   SCLK -> GPIO 18   (clock)
//   CS   -> GPIO 16   (chip select; consumed by the SD-card usermod, not core WLED)
#define PIN_SPI_MOSI       23
#define PIN_SPI_MISO       19
#define PIN_SPI_SCLK       18
#define PIN_SD_CS          16
#define SPIMOSIPIN         PIN_SPI_MOSI
#define SPIMISOPIN         PIN_SPI_MISO
#define SPISCLKPIN         PIN_SPI_SCLK


// =============================================================================
//  I2S DIGITAL MICROPHONE  (audioreactive usermod)
// =============================================================================
// Digital (I2S) microphone — NOT the SD card. These three are the mic interface:
//   WS  (Word Select / LR clock) -> GPIO 25
//   SCK (bit clock)              -> GPIO 26
//   SD  (serial data in)         -> GPIO 27
// Note: only takes effect when the audioreactive usermod is compiled in.
#define PIN_MIC_WS         25
#define PIN_MIC_SCK        26
#define PIN_MIC_SD         27
#define I2S_WSPIN          PIN_MIC_WS
#define I2S_CKPIN          PIN_MIC_SCK
#define I2S_SDPIN          PIN_MIC_SD
// Mic type: 1 = generic I2S digital microphone.
#define SR_DMTYPE          1


// =============================================================================
//  BATTERY VOLTAGE GAUGE  (Battery usermod)
// =============================================================================
// Analog input measuring battery voltage.
//   AIN0 -> GPIO 32   (ADC1 channel — correct choice; ADC2 is unusable with WiFi)
// Note: only takes effect when the Battery usermod is compiled in.
#define PIN_BATTERY_ADC    32
#define USERMOD_BATTERY_MEASUREMENT_PIN   PIN_BATTERY_ADC


// =============================================================================
//  ACCELEROMETER INTERRUPT  (future custom usermod — reserved, not yet wired)
// =============================================================================
// INT line: sensor signals the ESP32 that "something happened".
//   INT -> GPIO 14
// WLED has no native accelerometer support; this pin is reserved for a future usermod.
// The accelerometer's data/clock ride the shared I2C bus (SDA 21 / SCL 22) above.
#define PIN_ACCEL_INT      14
