About:  
This project is a monitoring application for the DHT22 temperature monitor for an Orange Pi (And maybe Raspberry Pi etc.) based on code from http://www.piprojects.xyz/temperature-humidity-sensor-orange-pi/  
Basically it is run by crontab at regular intervals (Say, every 15 minutes), opens the DHT22 device, tries to take a reading and outputs the time, humidity and temperature to a CSV file.

[Board information](doc/board.md)

Build:
```bash
make
```

Install it:
```bash
sudo make install
```

Run it:
```bash
sudo climbatize
```

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

Uninstall:  
Remove crontab file:
```bash
crontab -r
```

Remove binary and configuration file:
```bash
sudo make uninstall
```
