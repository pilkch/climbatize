#pragma once

#include <array>

namespace climbatize {

class cDHT22 {
  bool Read(float& fHumidity, float& fCelcius);

private:
  bool ValidateChecksum(const std::array<int, 5>& data) const;
};

}
