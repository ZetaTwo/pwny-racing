FROM ubuntu:16.04 AS build

RUN apt-get update && apt-get install -y build-essential gcc-4.8-arm-linux-gnueabihf
RUN mkdir /build
ADD src /build/src
RUN cd /build/src && make clean && make

FROM pwnable:latest

RUN apt-get update && apt-get install -y qemu-user libc6-armhf-cross
COPY --from=build /build/src/chall11 /home/ctf/chall
RUN mkdir /etc/qemu-binfmt && ln -s /usr/arm-linux-gnueabihf /etc/qemu-binfmt/arm
ADD deploy/.config.json /home/ctf/.config.json
ADD deploy/flag /home/ctf/flag
RUN chown root:ctf /home/ctf/chall /home/ctf/.config.json /home/ctf/flag
RUN chmod 750 /home/ctf/chall
RUN chmod 400 /home/ctf/.config.json
RUN chmod 440 /home/ctf/flag
