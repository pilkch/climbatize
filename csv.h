#pragma once

#include <fstream>
#include <sstream>

#include "utils.h"

namespace climbatize {

class cCSV {
public:
  template <typename... Targs>
  bool OpenAppend(const std::string& sFilePath, const std::string& field, Targs... otherFields);

  template <typename... Targs>
  void WriteLine(const std::string& field, Targs... otherFields);

private:
  template <typename... Targs>
  void WriteField(std::ostringstream& o, const std::string& field, Targs... otherFields);

  void WriteField(std::ostringstream& o, const std::string& field);

  std::ofstream f;
};


template <typename... Targs>
bool cCSV::OpenAppend(const std::string& sFilePath, const std::string& field, Targs... otherFields)
{
  const bool bExists = TestFileExists(sFilePath);

  f.open(sFilePath, std::ostream::out | std::ofstream::app);

  if (!f.good()) return false;

  // Write the header if this is a new file
  if (!bExists) WriteLine(field, otherFields...);

  return true;
}

template <typename... Targs>
void cCSV::WriteLine(const std::string& field, Targs... otherFields)
{
  std::ostringstream o;

  o<<field;

  WriteField(o, otherFields...);
 
  f<<o.str()<<std::endl;
}

template <typename... Targs>
void cCSV::WriteField(std::ostringstream& o, const std::string& field, Targs... otherFields)
{
  o<<","<<field;
  WriteField(o, otherFields...);
}

inline void cCSV::WriteField(std::ostringstream& o, const std::string& field)
{
  o<<","<<field;
}

}

