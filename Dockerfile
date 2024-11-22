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
    apt install -y g++

# Install git
RUN apt install -y git

# Install Python tools and utilities
RUN apt install -y python3-doit

# Install data base server
RUN export DEBIAN_FRONTEND=noninteractive && \
    apt install -y postgresql && \
    apt install -y libpq-dev

# Install basic text editor
RUN apt install -y nano

# Set work directory
WORKDIR /workdir/messenger

# Allow git to do manipulations inside the container
RUN git config --global --add safe.directory /workdir/messenger
ENTRYPOINT /bin/bash
