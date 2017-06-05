#include <limits>
#include <iostream>

#include <sys/time.h>
#include <syslog.h>
#include <sys/stat.h>

#include "csv.h"
#include "utils.h"

namespace climbatize {

std::string ToStringTwoDecimalPlaces(float fValue)
{
  char buffer[std::numeric_limits<float>::max_exponent10 +
    2 + // Fixed precision of 2 decimal places
    3 // '-' + '.' + NULL
  ];
  sprintf(buffer, "%.2f", fValue);
  return buffer;
}

std::string GetDateTimeUTC()
{
  time_t now;
  time(&now);

  timeval curTime;
  gettimeofday(&curTime, nullptr);

  const int milliseconds = curTime.tv_usec / 1000;
  char buf[sizeof("2000-01-01-01T01:01:01") + 1];
  strftime(buf, sizeof(buf), "%FT%T", gmtime(&now));

  char buf2[sizeof("2000-01-01-01T01:01:01.23Z") + 1];
  sprintf(buf2, "%s%dZ", buf, milliseconds);

  return buf2;
}

bool TestFileExists(const std::string& sFilePath)
{
  struct stat s;
  return (stat(sFilePath.c_str(), &s) >= 0);
}

float CelciusToFarenheit(float fCelcius)
{
  return (fCelcius * 1.8f) + 32.0f;
}

void LogHumidityAndTemperature(cCSV& csv, float fHumidity, float fTemperatureCelcius)
{
  const float fTemperatureFarenheit = CelciusToFarenheit(fTemperatureCelcius);
  syslog(LOG_INFO, "Climbatize Humidity = %.02f %%, Temperature = %.02f C (%.02f F)", fHumidity, fTemperatureCelcius, fTemperatureFarenheit);

  csv.WriteLine(GetDateTimeUTC(), ToStringTwoDecimalPlaces(fHumidity), ToStringTwoDecimalPlaces(fTemperatureCelcius));
}

}

