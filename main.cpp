#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include "dht22.h"
#include "utils.h"

int main(int argc, char **argv)
{
  std::cout<<"Climbatize"<<std::endl;

  // Initialise wiringPi
  if (wiringPiSetup() < 0) {
    std::cerr<<"Climbatize Failed to load wiringPi library, exiting"<<std::endl;
    return EXIT_FAILURE;
  }

  climbatize::cDHT22 dht;

  while (true) {
    float fHumidity = 0.0f;
    float fCelcius = 0.0f;
    if (dht.Read(fHumidity, fCelcius)) {
      const float fFarenheit = climbatize::CelciusToFarenheit(fCelcius);
      std::cout.precision(1);
      std::cout<<"Climbatize Humidity = "<<fHumidity<<" %% Temperature = "<<fCelcius<<" *C ("<<fFarenheit<<" *F)"<<std::endl;
    } else {
      std::cerr<<"Climbatize Data not good, dropping it"<<std::endl;
    }

    // Wait 2 seconds between each read
    delay( 2000 );
  }

  std::cout<<"Climbatize Finished, exiting"<<std::endl;

  return EXIT_SUCCESS;
}
