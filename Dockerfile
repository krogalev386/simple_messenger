# Developement container

FROM ubuntu:jammy

# Install docker stuff
RUN apt update && \
    apt install -y docker && \
    apt install -y docker.io && \
    apt install -y docker-compose-v2

# Install C++ utilities
RUN apt install -y clang && \
    apt install -y make && \
    apt install -y cmake && \
    apt install -y clang-tidy && \
    apt purge -y gcc

# Install git
RUN apt install -y git

# Install Python tools and utilities
RUN apt install -y python3-doit

# Install data base server
RUN export DEBIAN_FRONTEND=noninteractive && \
    apt install -y postgresql

WORKDIR /workdir/messenger
ENTRYPOINT /bin/bash
