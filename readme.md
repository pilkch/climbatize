## About

This project is a monitoring application for the DHT22 temperature monitor for an Orange Pi (And maybe Raspberry Pi etc.) based on code from http://www.piprojects.xyz/temperature-humidity-sensor-orange-pi/  
Basically it is run by crontab at regular intervals (Say, every 15 minutes), opens the DHT22 device, tries to take a reading and outputs the time, humidity and temperature to a CSV file.

## Board information

[Board information](doc/board.md)

## Requirements

- [wiringOP](https://github.com/zhaolei/WiringOP)
- libjson-c

## Building

Build and install wiringOP:
```bash
git clone https://github.com/zhaolei/WiringOP
cd WiringOP/
./build clean
./build
```

Install libjson-c

Ubuntu:
```bash
sudo apt install libjson-c libjson-c-dev
```

Fedora:
```bash
sudo dnf install json-c json-c-devel
```

Double check the wiring diagram for your board and which pins wiringOP uses (The example climbatize.json files uses wiringOP pin 2 which is physical pin 7 on an OrangePI One board):
```bash
sudo gpio readall
```

Build:
```bash
make
```

Install it:
```bash
sudo make install
```

Create the output directory (You can change this, but you'll need to use a JSON config file and specify where it goes):
```bash
sudo mkdir -p /root/.config/climbatize/
sudo cp ./climbatize.json /root/.config/climbatize/climbatize.json
```

Edit the output CSV file path and WiringPi pin to read from:
```bash
sudo vi /root/.config/climbatize/climbatize.json
```

Test it can run and temperatures are read correctly:
```bash
sudo climbatize
```

## Cron

Create a cron job to run it every 15 minutes:
1) 
```bash
sudo crontab -e
```
2) Add climbatize:
```bash
*/15 * * * * /usr/local/bin/climbatize
```
3) Check it was added:
```bash
crontab -l
```

## Removal

Remove crontab file:
```bash
crontab -r
```

Remove binary and configuration file:
```bash
sudo make uninstall
```
