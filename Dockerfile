FROM ubuntu:latest as build

USER connection

EXPOSE 8889

RUN apt-get update && \
    apt-get install -y \
       g++ \
      libevent-dev \
      cmake

COPY . /app
COPY . /var/www

WORKDIR /app/build

RUN cmake .. && cmake --build .

CMD ./highload_server