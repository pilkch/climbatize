#pragma once

#include <array>

namespace climbatize {

class cDHT22 {
public:
  explicit cDHT22(int wiringPiPin);

  bool Read(float& fHumidity, float& fCelcius);

private:
  bool ValidateChecksum(const std::array<int, 5>& data) const;

  int wiringPiPin; // The pin for reading from the temperature sensor
};

}
