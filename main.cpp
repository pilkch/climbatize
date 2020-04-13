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

namespace climbatize {

const std::string sVersion = "0.9";
const std::string sVersionDateTime = "2017/06/10 10:10PM";

bool ReadFileIntoString(const std::string& sFilePath, size_t nMaxFileSizeBytes, std::string& contents)
{
  if (!climbatize::TestFileExists(sFilePath)) {
    std::cerr<<"Climbatize Config file \""<<sFilePath<<"\" not found"<<std::endl;
    syslog(LOG_ERR, "Climbatize Config file \"%s\" not found", sFilePath.c_str());
    return false;
  }

  const size_t nFileSizeBytes = climbatize::GetFileSizeBytes(sFilePath);
  if (nFileSizeBytes == 0) {
    std::cerr<<"Climbatize Empty config file \""<<sFilePath<<"\""<<std::endl;
    syslog(LOG_ERR, "Climbatize Empty config file \"%s\"", sFilePath.c_str());
    return false;
  } else if (nFileSizeBytes > nMaxFileSizeBytes) {
    std::cerr<<"Climbatize Config file \""<<sFilePath<<"\" is too large"<<std::endl;
    syslog(LOG_ERR, "Climbatize Config file \"%s\" is too large", sFilePath.c_str());
    return false;
  }

  std::ifstream f(sFilePath);

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

void PrintVersion()
{
  std::cout<<"Climbatize v"<<sVersion<<", "<<sVersionDateTime<<std::endl;
}

void PrintUsage()
{
  std::cout<<"Usage:"<<std::endl;
  std::cout<<"climbatize [-v|--v|--version] [-h|--h|--help]"<<std::endl;
  std::cout<<"-v|--v|--version:\tPrint the version information"<<std::endl;
  std::cout<<"-h|--h|--help:\tPrint this usage information"<<std::endl;
}

}

int main(int argc, char **argv)
{
  openlog(nullptr, LOG_PID | LOG_CONS, LOG_USER | LOG_LOCAL0);
  std::cout<<"Climbatize"<<std::endl;

  if (argc >= 2) {
    for (size_t i = 1; i < size_t(argc); i++) {
      if (argv[i] != nullptr) {
        const std::string sAction = argv[i];
        if ((sAction == "-v") || (sAction == "-version") || (sAction == "--version")) climbatize::PrintVersion();
        else if ((sAction == "-h") || (sAction == "-help") || (sAction == "--help")) climbatize::PrintUsage();
        else {
          std::cerr<<"Unknown command line parameter \""<<sAction<<"\", exiting"<<std::endl;
          syslog(LOG_ERR, "Unknown command line parameter \"%s\", exiting", sAction.c_str());
          climbatize::PrintUsage();
          return -1;
        }
      }
    }

    return 0;
  }

  const std::string sConfigFolder = climbatize::GetConfigFolder("climbatize");
  if (sConfigFolder.empty()) {
    std::cerr<<"Climbatize Failed to get config folder for climbatize, exiting"<<std::endl;
    syslog(LOG_ERR, "Climbatize Failed to get config folder for climbatize, exiting");
    return EXIT_FAILURE;
  }

  // Read the configuration
  const std::string sSettingsFile = sConfigFolder + "/climbatize.json";
  climbatize::cSettings settings;
  if (!climbatize::ReadJSONConfig(sSettingsFile, settings)) {
    std::cerr<<"Climbatize Failed to load JSON configuration from \""<<sSettingsFile<<"\", exiting"<<std::endl;
    syslog(LOG_ERR, "Climbatize Failed to load JSON configuration from \"%s\", exiting", sSettingsFile.c_str());
    return EXIT_FAILURE;
  }

  // Initialise wiringPi
  if (wiringPiSetup() < 0) {
    std::cerr<<"Climbatize Failed to load wiringPi library, exiting"<<std::endl;
    syslog(LOG_ERR, "Climbatize Failed to load wiringPi library, exiting");
    return EXIT_FAILURE;
  }

  climbatize::cDHT22 dht;

  // Check if we need to rotate the logs
  const size_t nMaxBytes = 1 * 1024 * 1024; // 1 MB max per CSV file
  if (climbatize::GetFileSizeBytes(settings.sCSVFilePath) > nMaxBytes) {
    std::cout<<"Climbatize Rotating CSV file \""<<settings.sCSVFilePath<<"\", exiting"<<std::endl;
    syslog(LOG_INFO, "Climbatize Rotating CSV file \"%s\", exiting", settings.sCSVFilePath.c_str());
    climbatize::RotateLogs(settings.sCSVFilePath);
  }

  climbatize::cCSV csv;
  if (!csv.OpenAppend(settings.sCSVFilePath, "timestamp", "humidity", "temperature")) {
    std::cerr<<"Climbatize Failed to open CSV file \""<<settings.sCSVFilePath<<"\", exiting"<<std::endl;
    syslog(LOG_ERR, "Climbatize Failed to open CSV file \"%s\", exiting", settings.sCSVFilePath.c_str());
    return EXIT_FAILURE;
  }

  // Keep trying to read a humidity and temperature until we succeed, then log it and exit
  while (true) {
    float fHumidity = 0.0f;
    float fTemperatureCelcius = 0.0f;
    if (dht.Read(fHumidity, fTemperatureCelcius)) {
      std::cout<<"Climbatize Temperature read "<<fHumidity<<", "<<fTemperatureCelcius<<" C"<<std::endl;
      climbatize::LogHumidityAndTemperature(csv, fHumidity, fTemperatureCelcius);
      break;
    } else {
      std::cerr<<"Climbatize Data not good (Raw: "<<fHumidity<<", "<<fTemperatureCelcius<<"), dropping it"<<std::endl;
      syslog(LOG_ERR, "Climbatize Data not good, dropping it");
    }

    // Wait 2 seconds between each read
    delay( 2000 );
  }

  std::cout<<"Climbatize Finished, exiting"<<std::endl;
  closelog();

  return EXIT_SUCCESS;
}

