FROM alpine

RUN mkdir -p /app/src
WORKDIR /app

COPY external ./external
COPY src/shared ./src/shared
COPY src/server ./src/server

RUN apk add --no-cache --update tini libstdc++ g++ && \
    g++ -o server_bin \
        src/server/*.cpp \
        src/shared/*.cpp \
        -Iexternal/glm \
        -std=c++14 \
        -lstdc++ && \
    apk del g++

EXPOSE 12345

CMD ["tini", "--", "./server_bin"]
