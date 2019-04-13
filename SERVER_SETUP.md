# Setup instructions for Pwny Race server and host

## Summary

* Install nginx and libnginx-mod-rtmp
* Config stream endpoints
* Install teamspeak-server
* Config teamspeak-server

## Setup nginx RTMP relay

The setup script:

```
#!/bin/sh

set -e

# Install nginx and config RTMP server
apt-get install nginx libnginx-mod-rtmp
tee /etc/nginx/modules-available/60-mod-rtmp.conf <<EOF
rtmp {
    server {
        listen 1935;
        chunk_size 4000;
        notify_method get;

        application participant {
            live on;
            on_publish http://localhost:1936/auth_participant;
            on_play http://localhost:1936/auth_participant;
            record off;
        }

        application master {
            live on;
            on_publish http://localhost:1936/auth_master;
            record off;
            push rtmp://a.rtmp.youtube.com/live2/YOU_TUBE_STREAM_KEY;
            push rtmp://b.rtmp.youtube.com/live2/YOU_TUBE_STREAM_KEY?backup=1;
        }
    }
}
EOF

tee /etc/nginx/sites-available/rtmp-auth <<'EOF'
server {
  listen 127.0.0.1:1936;
  location /auth_participant {
    if ($arg_psk = 'CURRENT_PARTICIPANT_PASSWORD') {
        return 201;
    }
    return 404;
  }
  location /auth_master {
    if ($arg_psk = 'CURRENT_MASTER_PASSWORD') {
        return 201;
    }
    return 404;
  }
}
EOF
ln -s ../sites-available/rtmp-auth  /etc/nginx/sites-enabled/rtmp-auth
ln -s ../modules-available/60-mod-rtmp.conf /etc/nginx/modules-enabled/60-mod-rtmp.conf
service nginx reload
```

* Run setup script
* Update YOU_TUBE_STREAM_KEY, CURRENT_PARTICIPANT_PASSWORD and CURRENT_MASTER_PASSWORD
* Open port TCP 1935
* Point stream.pwny.racing to server

## Setup TeamSpeak

The setup script:

```
#!/bin/sh

set -e

# Based on: https://www.rosehosting.com/blog/how-to-set-up-a-teamspeak-server-on-ubuntu-16-04/
useradd --no-create-home --system --shell /usr/sbin/nologin teamspeak
curl -O https://files.teamspeak-services.com/releases/server/3.6.0/teamspeak3-server_linux_amd64-3.6.0.tar.bz2
tar xf teamspeak3-server_linux_amd64-3.6.0.tar.bz2
rm teamspeak3-server_linux_amd64-3.6.0.tar.bz2
mv teamspeak3-server_linux_amd64 /usr/local/teamspeak3
touch /usr/local/teamspeak3/.ts3server_license_accepted
chown -R teamspeak:teamspeak /usr/local/teamspeak3

tee /etc/systemd/system/teamspeak.service <<EOF
[Unit]
Description=TeamSpeak 3 Server
Documentation=http://www.teamspeak.com/?page=literature
After=network.target

[Service]
User=teamspeak
UMask=0027
WorkingDirectory=/usr/local/teamspeak3
ExecStart=/usr/local/teamspeak3/ts3server
Restart=on-failure

[Install]
WantedBy=multi-user.target
EOF

systemctl --system daemon-reload
service teamspeak start
```

* Run setup script
* Update participant and master password
* Open port TCP 10011,30033 and UDP 9987
* Point voice.pwny.racing to server


## Setup OBS

* Settings -> Stream -> Custom streaming server
  - URL: rtmp://stream.pwny.racing/master
  - Streaming key: ?psk=CURRENT_MASTER_PASSWORD
* Start streaming
* (Optional) Open a video player and point it to rtmp://stream.pwny.racing/master to verify that stream is working
