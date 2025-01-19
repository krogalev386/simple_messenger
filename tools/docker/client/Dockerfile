FROM alpine:latest
RUN apk update && \
    apk add --no-cache clang16 make cmake
# Install protobuf
RUN apk add protobuf-dev

WORKDIR /test_bench
# copy project files from upper level directory
COPY . .

# Setup image specific paths to LLVM files
ENV C_COMPILER /usr/bin/clang-16
ENV CXX_COMPILER /usr/bin/clang++-16
ENV LLVM_INCLUDE_PATH /usr/lib/llvm16/include
ENV LLVM_LIB_PATH /usr/lib/llvm16/lib

RUN rm -rf build/ && mkdir build && \
    cmake -DCMAKE_TOOLCHAIN_FILE=CMakeToolchain.cmake -B build . && \
    cmake --build build --target client_app -j 4

FROM alpine:latest
RUN apk update && \
    apk add --no-cache libstdc++ && \
    apk add gdb && \
    apk add util-linux && \
    apk add protobuf

WORKDIR /test_bench
COPY --from=0 /test_bench/build .
#COPY --from=0 /usr/lib/x86_64-linux-gnu/libprotobuf.so /usr/lib/x86_64-linux-gnu/libprotobuf.so
ENTRYPOINT /bin/sh
