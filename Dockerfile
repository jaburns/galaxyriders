FROM alpine

RUN mkdir -p /app/src
WORKDIR /app

COPY external ./external
COPY src/shared ./src/shared
COPY src/server ./src/server

RUN apk add --no-cache --update tini libstdc++ g++ && \
    g++ -o server \
        src/server/*.cpp \
        src/shared/*.cpp \
        -Iexternal/glm \
        -std=c++14 \
        -lstdc++ && \
    apk del g++

CMD ["tini", "--", "./server"]
