FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive
EXPOSE 1337

#update
RUN apt-get update --fix-missing && apt-get -y upgrade

#system deps
RUN apt-get install -y lib32z1 libseccomp-dev xinetd

#create ctf-user
RUN groupadd -r ctf && useradd -r -g ctf ctf

### IF CHROOT
#RUN cp -R /lib* /home/ctf
#RUN cp -R /usr/lib* /home/ctf
#RUN mkdir /home/ctf/dev
#RUN mknod /home/ctf/dev/null c 1 3
#RUN mknod /home/ctf/dev/zero c 1 5
#RUN mknod /home/ctf/dev/random c 1 8
#RUN mknod /home/ctf/dev/urandom c 1 9
#RUN chmod 666 /home/ctf/dev/*
#RUN mkdir /home/ctf/bin
#RUN cp /bin/sh /home/ctf/bin
#RUN cp /bin/ls /home/ctf/bin
#RUN cp /bin/cat /home/ctf/bin
### IF CHROOT

# Add common chall items
ADD ctf.xinetd /etc/xinetd.d/ctf
ADD chall_init.sh /etc/chall_init.sh
ADD redir.sh /home/ctf/redir.sh
ADD flag_submitter /home/ctf/flag_submitter

# Set permissions for common items 
RUN chmod 750 /home/ctf/redir.sh
RUN chmod 500 /etc/chall_init.sh

# Add challenge specific items
ADD chall /home/ctf/chall
ADD .config.json /home/ctf/.config.json

# Set permissions for specific items
RUN chown -R root:ctf /home/ctf
RUN chmod 750 /home/ctf/chall
RUN chmod 400 /home/ctf/.config.json
RUN chmod 4111 /home/ctf/flag_submitter

RUN service xinetd restart

### IF NETWORK-DEBUG
#RUN apt-get -y install netcat
#RUN apt-get -y install net-tools
### IF NETWORK-DEBUG

ENTRYPOINT ["/etc/chall_init.sh"]
