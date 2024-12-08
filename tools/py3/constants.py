# Backend network settings
BACKEND_NETWORK_NAME = 'messenger_backend'
BACKEND_NETWORK_GATEWAY = '11.0.0.1'
BACKEND_NETWORK_SUBNET = '11.0.0.0/24'

# PostgreSQL database settings
POSTGRES_IMAGE_VERSION = 'postgres:14-alpine3.20'
POSTGRES_CONTAINER_NANE = 'postgres_database'
POSTGRES_ADMIN_PASS = 'admin'
POSTGRES_IPADDR = '11.0.0.3'

# Build spec settings
ARTIFACTS_DIR = './out'
BUILD_DIR = './build'
CMAKE_TOOLCHAIN_FILE = 'CMakeToolchain.cmake'
CLANG_CHECK = 1
BUILD_THREAD_NUM = 4

# Dev env containter settings
PROJECT_ROOT_DIR_HOST = '.'
PROJECT_ROOT_DIR_CONT = '/workdir/messenger'
DOCKER_SOCK = '/var/run/docker.sock'
USER_HOST_GIT_CONFIG = '~/.gitconfig'
DOCKER_GIT_CONFIG = '/etc/gitconfig'
HOST_SSH_AUTH_SOCK = '$SSH_AUTH_SOCK'
DOCKER_SSH_AUTH_SOCK = '/ssh-agent'
DOCKER_RUN_CMD = ['docker', 'run']
DEV_IMAGE_NAME = ['dev_image']
DEFAULT_RUN_ARGS = ['--rm', '-it',
                    '--security-opt seccomp=unconfined',
                    '--env',f'SSH_AUTH_SOCK={DOCKER_SSH_AUTH_SOCK}']
NETWORK_RUN_ARGS = ['--network ' + BACKEND_NETWORK_NAME, '--ip 11.0.0.2']
RUN_ARGS = DEFAULT_RUN_ARGS + NETWORK_RUN_ARGS
