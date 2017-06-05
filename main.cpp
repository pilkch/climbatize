#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include <iostream>

#include <syslog.h>

#include <json-c/json.h>
#include <wiringPi.h>

#include "csv.h"
#include "dht22.h"
#include "utils.h"

bool ReadFileIntoString(const std::string& sFilePath, size_t nMaxFileSizeBytes, std::string& contents)
{
  std::ifstream f(sFilePath);

  // Get the size of the file
  f.seekg(0, std::ios::end);
  const size_t nFileSizeBytes = f.tellg();
  if (nFileSizeBytes == 0) {
    std::cerr<<"Climbatize Empty config file \""<<sFilePath<<"\""<<std::endl;
    syslog(LOG_ERR, "Climbatize Empty config file \"%s\"", sFilePath.c_str());
    return false;
  } else if (nFileSizeBytes > nMaxFileSizeBytes) {
    std::cerr<<"Climbatize Config file \""<<sFilePath<<"\" is too large"<<std::endl;
    syslog(LOG_ERR, "Climbatize Config file \"%s\" is too large", sFilePath.c_str());
    return false;
  }

  f.seekg(0, std::ios::beg);

  contents.reserve(nFileSizeBytes);

  contents.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());

  return true;
}

class cSettings {
public:
  void Clear() { sCSVFilePath.clear(); }

  std::string sCSVFilePath;
};

bool JSONParse(json_object& jobj, cSettings& settings)
{
  json_object_object_foreach(&jobj, key, val) {
    enum json_type type = json_object_get_type(val);
    if ((type == json_type_object) && (strcmp(key, "settings") == 0)) {
      json_object_object_foreach(val, child_key, child_val) {
        enum json_type type = json_object_get_type(child_val);
        if ((type == json_type_string) && (strcmp(child_key, "csv_file") == 0)) {
          settings.sCSVFilePath = json_object_get_string(child_val);
          std::cout<<"Found settings, csv_file, value: \""<<settings.sCSVFilePath<<"\""<<std::endl;
          return true;
        }
      }
    }
  }

  return false;
}

bool ReadJSONConfig(const std::string& sFilePath, cSettings& settings)
{
  settings.Clear();

  const size_t nMaxFileSizeBytes = 2048;
  std::string contents;
  if (!ReadFileIntoString(sFilePath, nMaxFileSizeBytes, contents)) return false;

  json_object* jobj = json_tokener_parse(contents.c_str());
  if (jobj == nullptr) {
    std::cerr<<"Climbatize Invalid JSON config \""<<sFilePath<<"\""<<std::endl;
    syslog(LOG_ERR, "Climbatize Invalid JSON config \"%s\"", sFilePath.c_str());
    return false;
  }

  // Parse the JSON tree
  return JSONParse(*jobj, settings);
}

int main(int argc, char **argv)
{
  openlog(nullptr, LOG_PID | LOG_CONS, LOG_USER | LOG_LOCAL0);
  std::cout<<"Climbatize"<<std::endl;

  // Read the configuration
  cSettings settings;
  // TODO: Get the home directory, place this JSON file in the root folder
  if (!ReadJSONConfig("/home/elsa/climbatize/climbatize.json", settings)) {
    std::cerr<<"Climbatize Failed to load JSON configuration, exiting"<<std::endl;
    return EXIT_FAILURE;
  }

  // Initialise wiringPi
  if (wiringPiSetup() < 0) {
    std::cerr<<"Climbatize Failed to load wiringPi library, exiting"<<std::endl;
    return EXIT_FAILURE;
  }

  climbatize::cDHT22 dht;

  climbatize::cCSV csv;
  if (!csv.OpenAppend(settings.sCSVFilePath, "timestamp", "humidity", "temperature")) {
    std::cerr<<"Climbatize Failed to open CSV file \""<<settings.sCSVFilePath<<"\""<<std::endl;
    return EXIT_FAILURE;
  }

  // Keep trying to read a humidity and temperature until we succeed, then log it and exit
  while (true) {
    float fHumidity = 0.0f;
    float fTemperatureCelcius = 0.0f;
    if (dht.Read(fHumidity, fTemperatureCelcius)) {
      climbatize::LogHumidityAndTemperature(csv, fHumidity, fTemperatureCelcius);
      break;
    } else {
      std::cerr<<"Climbatize Data not good, dropping it"<<std::endl;
      syslog(LOG_ERR, "Climbatize Data not good, dropping it");
    }

    // Wait 2 seconds between each read
    delay( 2000 );
  }

  std::cout<<"Climbatize Finished, exiting"<<std::endl;
  closelog();

  return EXIT_SUCCESS;
}

