#!/bin/sh

# how it works:
# on server boot, this script is launched as daemon thanks to supervisor (see /etc/supervisor/conf.d/cameraman_observer.conf)

/usr/bin/php -q /var/www/nabezky/scripts/rgb2gs.php

while inotifywait -e modify /var/www/nabezky/sites/default/files/ftp; do
     /usr/bin/php -q /var/www/nabezky/scripts/rgb2gs.php
done
