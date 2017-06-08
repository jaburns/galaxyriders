FROM alpine

RUN mkdir -p /app/src
WORKDIR /app

COPY external ./external
COPY src/shared ./src/shared
COPY src/server ./src/server
COPY build_server ./build_server

RUN apk add --no-cache --update tini libstdc++ g++ && \
    ./build_server && \
    apk del g++

EXPOSE 12345

CMD ["tini", "--", "./server"]
