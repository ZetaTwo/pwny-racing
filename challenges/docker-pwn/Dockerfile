FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive
EXPOSE 1337

# Update and install dependencies
RUN apt-get update && apt-get -y upgrade
RUN apt-get install -y xinetd

RUN groupadd -r ctf && useradd -r -g ctf ctf

# Add common chall items
ADD ctf.xinetd /etc/xinetd.d/ctf
ADD redir.sh /home/ctf/redir.sh
ADD flag_submitter /home/ctf/flag_submitter

# Set permissions for common items 
RUN chown -R root:ctf /home/ctf
RUN chmod 750 /home/ctf/redir.sh
RUN chmod 4111 /home/ctf/flag_submitter

ENTRYPOINT ["/usr/sbin/xinetd", "-dontfork"]
