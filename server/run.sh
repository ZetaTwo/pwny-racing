#!/bin/sh
docker run -ti -p 9090:9090 --network pwnyracing --name pwnyracing-nodecg pwnyracing-nodecg
docker run -ti -p 80:80 -p 443:443 --network pwnyracing --name pwnyracing-nginx pwnyracing-nginx
