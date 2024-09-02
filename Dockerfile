FROM alpine:latest
RUN apk update && \
    apk add --no-cache clang make cmake
WORKDIR /test_bench
COPY . .
RUN rm -rf build/ && mkdir build && \
    cmake -DCMAKE_TOOLCHAIN_FILE=CMakeToolchain.cmake -B build . && \
    cmake --build build -j 4

FROM alpine:latest
RUN apk update && \
    apk add --no-cache libstdc++ && \
    apk add gdb && \
    apk add util-linux
WORKDIR /test_bench
COPY --from=0 /test_bench/build .
ENTRYPOINT /bin/sh 
