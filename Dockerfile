FROM alpine:latest
RUN apk update && \
    apk add --no-cache g++ make cmake
WORKDIR /test_bench
COPY . .
RUN rm -rf build/ && mkdir build && cmake -B build . && cmake --build build

FROM alpine:latest
RUN apk update && \
    apk add --no-cache libstdc++ && \
    apk add gdb && \
    apk add util-linux
WORKDIR /test_bench
COPY --from=0 /test_bench/build .
ENTRYPOINT /bin/sh 
