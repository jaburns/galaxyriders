FROM alpine

RUN mkdir -p /app/src
WORKDIR /app

RUN apk add --no-cache --update g++

COPY src/shared ./src/shared
COPY src/server ./src/server
COPY external ./external

RUN g++ -o server \
    src/server/*.cpp \
    src/shared/*.cpp \
    -Iexternal/glm \
    -std=c++14 \
    -lstdc++

CMD ["./server"]
