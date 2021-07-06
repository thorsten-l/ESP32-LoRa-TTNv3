FROM ubuntu:20.04

RUN apt-get update && apt-get install -y --no-install-recommends wget unzip git make \
  srecord bc xz-utils gcc python3 curl python3-pip python3-dev build-essential 

RUN pip3 install -U platformio
RUN pio update
RUN pio upgrade
RUN git clone https://github.com/thorsten-l/ESP32-Arduino-Info.git

WORKDIR /ESP32-Arduino-Info
RUN /usr/local/bin/platformio run -e lolin32lite

WORKDIR /
RUN rm -fr /ESP32-Arduino-Info

RUN mkdir /workdir
WORKDIR /workdir

CMD ["/bin/bash"]
