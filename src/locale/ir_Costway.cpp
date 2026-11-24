/// Set the temperature.
/// @param[in] temp The temperature in degrees Celsius or Fahrenheit.
/// @param[in] fahrenheit True if the temperature is in Fahrenheit, false for Celsius.
void IRCostwayAc::setTemp(const uint8_t temp, const bool fahrenheit) {
  if (fahrenheit) {
    uint8_t f_temp = std::max(kCostwayAcMinTempF,
                              std::min(kCostwayAcMaxTempF, temp));
    _.Fahrenheit = 1;
    // Maps 60F -> 0x04 (resulting in Byte 10 = 0x88), incrementing +1 per deg F
    _.TempF = (f_temp - kCostwayAcMinTempF) + kCostwayAcTempFOffset;

    // Populate Byte 1 with the rounded Celsius equivalent expected by the A/C
    // Matches captured mapping: 60-61F->16C, 62-63F->17C, 64-65F->18C, etc.
    uint8_t c_equiv = ((f_temp - 32) * 5 + 4) / 9;
    c_equiv = std::max(kCostwayAcMinTemp, std::min(kCostwayAcMaxTemp, c_equiv));
    _.Temp = c_equiv - kCostwayAcTempDelta;
  } else {
    _.Fahrenheit = 0;
    _.TempF = 0;
    uint8_t c_temp = std::max(kCostwayAcMinTemp,
                              std::min(kCostwayAcMaxTemp, temp));
    _.Temp = c_temp - kCostwayAcTempDelta;
  }
}

/// Get the current temperature setting.
/// @return The current setting for temp. in degrees (C or F depending on mode).
uint8_t IRCostwayAc::getTemp(void) const {
  if (_.Fahrenheit) {
    return (_.TempF - kCostwayAcTempFOffset) + kCostwayAcMinTempF;
  }
  return _.Temp + kCostwayAcTempDelta;
}

/// Get the current temperature unit mode.
/// @return True if the temperature is in Fahrenheit, false if in Celsius.
bool IRCostwayAc::getFahrenheit(void) const {
  return _.Fahrenheit;
}

/// Convert the current internal state into a human readable string.
/// @return A human readable string.
String IRCostwayAc::toString(void) const {
  String result = "";
  result.reserve(130);
  result += addBoolToString(_.Power, kPowerStr, false);
  result += addModeToString(_.Mode, kCostwayAcAuto, kCostwayAcCool,
                            kCostwayAcHeat, kCostwayAcDry, kCostwayAcFan);
  result += addTempToString(getTemp(), _.Fahrenheit);
  result += addFanToString(_.Fan, kCostwayAcFanHigh, kCostwayAcFanLow,
                           kCostwayAcFanAuto, kCostwayAcFanAuto,
                           kCostwayAcFanMed);
  result += addIntToString(_.SwingV, kSwingVStr);
  result += kSpaceLBraceStr;
  switch (_.SwingV) {
    case kCostwayAcSwingOff:
      result += kOffStr;
      break;
    case kCostwayAcSwingOn:
      result += kOnStr;
      break;
    default:
      result += kUnknownStr;
  }
  result += ')';
  result += addIntToString(_.SwingH, kSwingHStr);
  result += kSpaceLBraceStr;
  switch (_.SwingH) {
    case kCostwayAcSwingOff:
      result += kOffStr;
      break;
    case kCostwayAcSwingOn:
      result += kOnStr;
      break;
    default:
      result += kUnknownStr;
  }
  result += ')';
  result += addBoolToString(_.Clean, kCleanStr);
  result += addBoolToString(_.Turbo, kTurboStr);
  result += addBoolToString(_.IFeel, kIFeelStr);
  if (_.SensorUpdate) {
    result += addTempToString(getSensorTemp(), _.Fahrenheit, true);
  }
  return result;
}
