About:  
This project is a monitoring application for the DHT22 temperature monitor for an Orange Pi (And maybe Raspberry Pi etc.) based on code from http://www.piprojects.xyz/temperature-humidity-sensor-orange-pi/  
Basically it is run by crontab at regular intervals (Say, every 15 minutes), opens the DHT22 device, tries to take a reading and outputs the time, humidity and temperature to a CSV file.

Build:
```bash
make
```

Run:
```bash
sudo ./climatize
```

OR

1) 
```bash
sudo crontab -e
```
2) Add climatize:
```bash
*/5 * * * * /home/myuser/backup.sh
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
