# simple-messenger
Brand new super-puper-duper instant messages application

## What is it?
~Just a hobby project/a test site for playing with random stuff that I am trying to learn~
An unique example of messanger service that offers the worst user experience one can imagine!

If you are looking for:
* safe
* secure
* easy to use
* pleasent for eyes
* reliable
* and blazing fast

enviroment for communication - you can find it anywhere but there. At least today.
Don't worry, we will notify you when you can come back here for all this ~(No, we won't)~.

Currently the project consists of the more-or-less functioning C++ server application side
and the rudimentary C++ client application side that plays a role of the testing interface for the server.

The server application uses gRPC framework for facilitation of communication between clients
and PostgreSQL database for auxiliary tasks like authentification and - if future - long-term user data storage.

## Requirements
To build the project, you need to have installed:
1) Linux-based OS (or WSL if you use Windows 10/11);
2) Docker Engine (v27.5.1, older versions might not work without tweaks);
3) git;
4) Python3;
5) ssh-agent (if you want to operate git inside the development container)

That's all. Project is designed to ship all necessary development instruments automatically
during build of the development Docker container.

### Tools and libs shipped after the dev container build
- clang-format and clang-tidy: for C++ code formatting and static diagnostics;
- Python3;
- doit: Python utility which allows to create custom CLI shortcuts;
- PostgreSQL database engine and its C++ API library libpq-dev;
- docker-compose: for deploying the virtual testbench network.
- gRPC and protobuf.

The base image of the dev container is Ubuntu 22.04 LTS image (Jammy Jellyfish).

## Build
To build the dev container image, inside main folder type
```
python3 tools/py3/dev_env.py -b
```
**NB 1**: _you should be able to run ```docker``` without ```sudo``` before running_
_this script, otherwise check if you have ```docker``` user group and your user added_
_to this group: https://docs.docker.com/engine/install/linux-postinstall/_

**NB 2**: _the first build run can take a lot of time, mainly because of compilation_
_of gRPC libraries from source code. Subsequent re-builds and updates_
_will be speeded up by local Docker image caching mechanism._

To run and enter inside the dev container, type
```
# temporarly workaround for the first container run; will be fixed soon
docker network create -d bridge messenger_backend --gateway 11.0.0.1 --subnet 11.0.0.0/24

python3 tools/py3/dev_env.py
```

To build both server and client C++ applications, inside the dev container type
```
doit build_project
```

To remove or re-build apps, use ```clear_project``` and ```rebuild_project``` doit targets respectively.

For managing of PostgreSQL database, there are the following doit targets:
```
doit setup_psql - start PostgreSQL container
doit shutdown_psql - stop and derstroy PostgreSQL container
```

## Execution
Currently the usage is limited by running the compiled applications inside the
dev container or inside the "testbench" - the virtual network of containers
deployed via ```docker-compose```. The testbench consists of:
- 3 containers with the C++ client apps inside;
- The container with the server app;
- The container with PostgreSQL database.

To deploy the testbench, use ```doit``` target ```setup_testbench``` - and ```destroy_testbench``` to shutdown it.

The server and database containers run in non-interactive mode; one can (and it is assumed that one will) attach to them by the following commands:
```
docker attach messenger-client1-1
docker attach messenger-client2-1
docker attach messenger-client3-1
```
