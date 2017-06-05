Build:
make

Run:
sudo ./climatize

OR

1) sudo crontab -e
2) Add climatize:
*/5 * * * * /home/myuser/backup.sh
3) Check it was added:
crontab -l

Uninstall:
Remove crontab file:
crontab -r
