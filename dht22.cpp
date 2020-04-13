#include <wiringPi.h>

#include "dht22.h"

// From http://www.piprojects.xyz/temperature-humidity-sensor-orange-pi/

#define MAX_TIMINGS 850
#define DHT_PIN 2 // NOTE: This is physical pin 7, wiringpi pin 2

namespace climbatize {

bool cDHT22::ValidateChecksum(const std::array<int, 5>& data) const
{
  // Calculate the checksum
  const uint8_t checksum = ((data[0] + data[1] + data[2] + data[3]) & 0xFF);

  // Verify checksum in the last byte
  return (data[4] == checksum);
}

bool cDHT22::Read(float& fHumidity, float& fCelcius)
{
  fHumidity = 0.0f;
  fCelcius = 0.0f;

  std::array<int, 5> data = { 0, 0, 0, 0, 0 };

  // Pull pin down for 18 milliseconds
  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, LOW);
  delay(18);

  // Prepare to read the pin
  pinMode(DHT_PIN, INPUT);

  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0;

  // Detect change and read data
  for (uint8_t i = 0; i < MAX_TIMINGS; i++) {
    counter = 0;
    while (digitalRead(DHT_PIN) == laststate) {
      counter++;
      delayMicroseconds(1);

      if (counter == 255) break;
    }
    laststate = digitalRead(DHT_PIN);

    if (counter == 255) break;

    // Ignore first 4 transitions and every second read
    if ((i >= 4) && (i % 2 == 0)) {
      // Shove each bit into the storage bytes
      data[j / 8] <<= 1;

      if (counter > 16) data[j / 8] |= 1;

      j++;
    }
  }

  // Check we read 40 bits (8bit x 5 ) + verify checksum
  if ((j >= 40) && ValidateChecksum(data)) {
    // Read humidity
    fHumidity = float((data[0] << 8) + data[1]) / 10;
    if (fHumidity > 100) fHumidity = data[0]; // For DHT11

    // Read temperature
    fCelcius = float(((data[2] & 0x7F) << 8) + data[3]) / 10;
    if (fCelcius > 125) fCelcius = data[2]; // For DHT11

    if (data[2] & 0x80) fCelcius = -fCelcius;

    return true;
  }

  return false;
}

}
