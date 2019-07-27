FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y apt-transport-https
RUN apt-get install -y xinetd qemu qemu-user qemu-user-static
RUN apt-get install -y 'binfmt*'
RUN apt-get install -y libc6-armhf-armel-cross
RUN mkdir /etc/qemu-binfmt
RUN ln -s /usr/arm-linux-gnueabihf /etc/qemu-binfmt/arm
RUN apt-get install -y lib32z1

# Add user
RUN groupadd -r ctf && useradd -r -g ctf ctf

# Add common chall items
ADD files/ctf.xinetd /etc/xinetd.d/ctf
ADD files/redir.sh /home/ctf/redir.sh
ADD files/flag_submitter /home/ctf/flag_submitter

# Set permissions for common items 
RUN chmod 750 /home/ctf/redir.sh

# Add challenge specific items
ADD files/chall /home/ctf/chall
ADD files/.config.json /home/ctf/.config.json

# Set permissions for specific items
RUN chown -R root:ctf /home/ctf
RUN chmod 750 /home/ctf/chall
RUN chmod 400 /home/ctf/.config.json
RUN chmod 4111 /home/ctf/flag_submitter

# Start serving
RUN service xinetd restart
CMD ["/usr/sbin/xinetd", "-dontfork"]
