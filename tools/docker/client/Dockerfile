FROM build-base-image

# copy project files from upper level directory
COPY . .

RUN rm -rf build/ && mkdir build && \
    cmake -DCMAKE_TOOLCHAIN_FILE=CMakeToolchain.cmake -B build . && \
    cmake --build build --target client_app -j 4

FROM alpine:3.21.3
RUN apk update && \
    apk add --no-cache libstdc++ && \
    apk add gdb && \
    apk add util-linux

WORKDIR /test_bench
COPY --from=0 /test_bench/build .
ENTRYPOINT /bin/sh
