#pragma once

#include <string>

namespace climbatize {

class cCSV;

std::string GetDateTimeUTC();

bool TestFileExists(const std::string& sFilePath);
size_t GetFileSizeBytes(const std::string& sFilePath);
void RotateLogs(const std::string& sFilePath);

std::string GetConfigFolder(const std::string& sApplicationNameLower);

float CelciusToFarenheit(float fCelcius);

void LogHumidityAndTemperature(cCSV& csv, float fHumidity, float fTemperatureCelcius);

}
