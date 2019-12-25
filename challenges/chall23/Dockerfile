FROM ubuntu:18.04 AS build

RUN apt-get update && apt-get install -y build-essential gcc-avr avr-libc
RUN mkdir /build
ADD src /build/src
RUN cd /build/src && make clean && make

FROM ubuntu:18.04
RUN mkdir /build
COPY --from=build /build/src/chall23.hex /build/chall.hex
COPY --from=build /build/src/writeFlagToEEPROM.hex /build/flash-flag.hex
