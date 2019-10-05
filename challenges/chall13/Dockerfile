FROM ubuntu:18.04 AS build

RUN apt-get update && apt-get install -y build-essential gcc-multilib
RUN mkdir /build
ADD src /build/src
RUN cd /build/src && make clean && make

FROM pwnable:latest

RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y libc6:i386
COPY --from=build /build/src/chall13 /home/ctf/chall
ADD deploy/.config.json /home/ctf/.config.json
ADD deploy/flag /home/ctf/flag
RUN chown root:ctf /home/ctf/chall /home/ctf/.config.json /home/ctf/flag
RUN chmod 750 /home/ctf/chall
RUN chmod 400 /home/ctf/.config.json
RUN chmod 440 /home/ctf/flag
