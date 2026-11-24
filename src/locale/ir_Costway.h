// Copyright 2019-2026 David Conran
/// @file
/// @brief Support for Costway A/C protocols.

#ifndef IR_COSTWAY_H_
#define IR_COSTWAY_H_

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include "IRremoteESP8266.h"
#include "IRsend.h"
#ifdef UNIT_TEST
#include "IRsend_test.h"
#endif

// State length (13 bytes, matching the Electra/AUX 104-bit frame)
const uint16_t kCostwayAcStateLength = 13;
const uint16_t kCostwayAcBits = kCostwayAcStateLength * 8;
const uint16_t kCostwayAcMinRepeat = 0;

/// Native representation of a Costway A/C message.
union CostwayProtocol {
  uint8_t raw[kCostwayAcStateLength];  ///< The state of the IR remote
  struct {
    // Byte 0
    uint8_t             :8;
    // Byte 1
    uint8_t SwingV      :3;
    uint8_t Temp        :5;
    // Byte 2
    uint8_t             :5;
    uint8_t SwingH      :3;
    // Byte 3
    uint8_t             :6;
    uint8_t SensorUpdate:1;
    uint8_t             :1;
    // Byte 4
    uint8_t             :5;
    uint8_t Fan         :3;
    // Byte 5
    uint8_t             :6;
    uint8_t Turbo       :1;
    uint8_t Quiet       :1;
    // Byte 6
    uint8_t             :3;
    uint8_t IFeel       :1;
    uint8_t             :1;
    uint8_t Mode        :3;
    // Byte 7
    uint8_t SensorTemp  :8;
    // Byte 8
    uint8_t             :8;
    // Byte 9
    uint8_t             :2;
    uint8_t Clean       :1;
    uint8_t             :2;
    uint8_t Power       :1;
    uint8_t             :2;
    // Byte 10
    uint8_t             :1;  // Bit 0: always 0 in captured frames
    uint8_t TempF       :6;  // Bits 1..6: Fahrenheit value offset (0x04 = 60F)
    uint8_t Fahrenheit  :1;  // Bit 7: 1 when Fahrenheit mode is active (0x80)
    // Byte 11
    uint8_t LightToggle :8;
    // Byte 12
    uint8_t Sum         :8;
  };
};

// Constants
const uint8_t kCostwayAcMinTemp = 16;   // 16C
const uint8_t kCostwayAcMaxTemp = 32;   // 32C
const uint8_t kCostwayAcTempDelta = 8;
const uint8_t kCostwayAcMinTempF = 60;  // 60F
const uint8_t kCostwayAcMaxTempF = 86;  // 86F
const uint8_t kCostwayAcTempFOffset = 0x04;  // (0x88 & 0x7E) >> 1 = 4 for 60F

const uint8_t kCostwayAcSwingOn =  0b000;
const uint8_t kCostwayAcSwingOff = 0b111;

const uint8_t kCostwayAcFanAuto =  0b101;
const uint8_t kCostwayAcFanLow =   0b011;
const uint8_t kCostwayAcFanMed =   0b010;
const uint8_t kCostwayAcFanHigh =  0b001;

const uint8_t kCostwayAcAuto =     0b000;
const uint8_t kCostwayAcCool =     0b001;
const uint8_t kCostwayAcDry =      0b010;
const uint8_t kCostwayAcHeat =     0b100;
const uint8_t kCostwayAcFan =      0b110;

const uint8_t kCostwayAcLightToggleOn  = 0x15;
const uint8_t kCostwayAcLightToggleMask = 0x11;
const uint8_t kCostwayAcLightToggleOff = 0x08;

const uint8_t kCostwayAcSensorTempDelta = 0x4A;
const uint8_t kCostwayAcSensorMinTemp = 0;    // 0C
const uint8_t kCostwayAcSensorMaxTemp = 50;   // 50C

// Classes
/// Class for handling detailed Costway A/C messages.
class IRCostwayAc {
 public:
  explicit IRCostwayAc(const uint16_t pin, const bool inverted = false,
                       const bool use_modulation = true);
  void stateReset(void);
#if SEND_COSTWAY_AC
  void send(const uint16_t repeat = kCostwayAcMinRepeat);
  /// Run the calibration to calculate uSec timing offsets for this platform.
  /// @return The uSec timing offset needed per modulation of the IR Led.
  int8_t calibrate(void) { return _irsend.calibrate(); }
#endif  // SEND_COSTWAY_AC
  void begin(void);
  void on(void);
  void off(void);
  void setPower(const bool on);
  bool getPower(void) const;
  void setMode(const uint8_t mode);
  uint8_t getMode(void) const;
  void setTemp(const uint8_t temp, const bool fahrenheit = false);
  uint8_t getTemp(void) const;
  bool getFahrenheit(void) const;
  void setFan(const uint8_t speed);
  uint8_t getFan(void) const;
  void setSwingV(const bool on);
  bool getSwingV(void) const;
  void setSwingH(const bool on);
  bool getSwingH(void) const;
  void setClean(const bool on);
  bool getClean(void) const;
  void setLightToggle(const bool on);
  bool getLightToggle(void) const;
  void setTurbo(const bool on);
  bool getTurbo(void) const;
  void setQuiet(const bool on);
  bool getQuiet(void) const;
  void setIFeel(const bool on);
  bool getIFeel(void) const;
  void setSensorUpdate(const bool on);
  bool getSensorUpdate(void) const;
  void setSensorTemp(const uint8_t temp);
  uint8_t getSensorTemp(void) const;
  uint8_t* getRaw(void);
  void setRaw(const uint8_t new_code[],
              const uint16_t length = kCostwayAcStateLength);
  static bool validChecksum(const uint8_t state[],
                            const uint16_t length = kCostwayAcStateLength);
  static uint8_t calcChecksum(const uint8_t state[],
                              const uint16_t length = kCostwayAcStateLength);
  String toString(void) const;
  static uint8_t convertMode(const stdAc::opmode_t mode);
  static uint8_t convertFan(const stdAc::fanspeed_t speed);
  static stdAc::opmode_t toCommonMode(const uint8_t mode);
  static stdAc::fanspeed_t toCommonFanSpeed(const uint8_t speed);
  stdAc::state_t toCommon(void) const;

#ifndef UNIT_TEST
 private:
  IRsend _irsend;  ///< instance of the IR send class
#else
  /// @cond IGNORE
  IRsendTest _irsend;  ///< instance of the testing IR send class
  /// @endcond
#endif
  CostwayProtocol _;
  void checksum(const uint16_t length = kCostwayAcStateLength);
};

#endif  // IR_COSTWAY_H_
