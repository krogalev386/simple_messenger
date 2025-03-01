FROM alpine:latest
RUN apk update && \
    apk add --no-cache g++ clang16 make cmake && \
    apk add --no-cache libpq-dev git

RUN apk add --no-cache linux-headers
WORKDIR /test_bench

# Install grpc and protobuf
COPY ./tools/grpc_setup ./grpc_setup
RUN chmod u+x ./grpc_setup/install_grpc.sh
RUN ./grpc_setup/install_grpc.sh && rm -rf grpc_setup

# Setup image specific paths to LLVM files
ENV C_COMPILER /usr/bin/clang-16
ENV CXX_COMPILER /usr/bin/clang++-16
ENV LLVM_INCLUDE_PATH /usr/lib/llvm16/include
ENV LLVM_LIB_PATH /usr/lib/llvm16/lib
