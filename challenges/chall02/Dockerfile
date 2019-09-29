FROM ubuntu:18.04 AS build

RUN apt-get update --fix-missing && apt-get -y upgrade
RUN apt-get install -y build-essential
RUN mkdir /build
ADD src /build/src
RUN cd /build/src && make

FROM pwnable:latest

COPY --from=build /build/src/chall2 /home/ctf/chall
ADD deploy/.config.json /home/ctf/.config.json
ADD deploy/flag /home/ctf/flag
RUN chown root:ctf /home/ctf/chall /home/ctf/.config.json /home/ctf/flag
RUN chmod 750 /home/ctf/chall
RUN chmod 400 /home/ctf/.config.json
RUN chmod 440 /home/ctf/flag
