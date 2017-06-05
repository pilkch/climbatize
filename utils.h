#pragma once

#include <string>

namespace climbatize {

class cCSV;

std::string GetDateTimeUTC();

bool TestFileExists(const std::string& sFilePath);

float CelciusToFarenheit(float fCelcius);

void LogHumidityAndTemperature(cCSV& csv, float fHumidity, float fTemperatureCelcius);

}
