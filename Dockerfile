FROM rikorose/gcc-cmake
RUN apt-get update && apt-get install -y libpng-dev

RUN git clone https://github.com/hect1995/UBIMET_Challenge.git

WORKDIR /UBIMET_Challenge

RUN mkdir build

WORKDIR build

RUN cmake ..

RUN make

RUN ./ubimet /UBIMET_Challenge/data/1706221600.29 output.csv