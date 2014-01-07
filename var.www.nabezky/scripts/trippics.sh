#!/bin/sh

while inotifywait -e modify /home/sano/trippics; do
     mv /home/sano/trippics/*.* /var/www/nabezky/sites/default/files/trippics/$( date +%F_%T )
done
