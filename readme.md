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
