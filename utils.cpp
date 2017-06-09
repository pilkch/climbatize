#include <limits>
#include <iostream>

#include <experimental/filesystem>

#include <pwd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

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

std::string GetHomeFolder()
{
  const char* szHomeFolder = getenv("HOME");
  if (szHomeFolder != nullptr) return szHomeFolder;

  struct passwd* pPasswd = getpwuid(getuid());
  if (pPasswd != nullptr) return pPasswd->pw_dir;

  return "";
}

std::string GetConfigFolder(const std::string& sApplicationNameLower)
{
  const std::string sHomeFolder = GetHomeFolder();
  if (sHomeFolder.empty()) return "";

  return sHomeFolder + "/.config/" + sApplicationNameLower;
}

bool TestFileExists(const std::string& sFilePath)
{
  struct stat s;
  return (stat(sFilePath.c_str(), &s) >= 0);
}

size_t GetFileSizeBytes(const std::string& sFilePath)
{
  struct stat s;
  if (stat(sFilePath.c_str(), &s) < 0) return 0;

  return s.st_size;
}

bool MoveFile(const std::string& sFilePathFrom, const std::string& sFilePathTo)
{
  const std::experimental::filesystem::path from(sFilePathFrom);
  const std::experimental::filesystem::path to(sFilePathTo);
  std::experimental::filesystem::rename(from, to);
  return TestFileExists(sFilePathTo);
}

void RotateLogs(const std::string& sFilePath)
{
  // Rotate the logs, starting with the last position and moving up to the second log
  for (size_t i = 9; i > 1; i--) {
    const std::string sFrom = sFilePath + "." + std::to_string(i - 1);
    if (TestFileExists(sFrom)) {
      const std::string sTo = sFilePath + "." + std::to_string(i);
      MoveFile(sFrom, sTo);
    }
  }

  // Now rotate the main log into the second rotation position
  if (TestFileExists(sFilePath)) {
    const std::string sTo = sFilePath + ".1";
    MoveFile(sFilePath, sTo);
  }
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

