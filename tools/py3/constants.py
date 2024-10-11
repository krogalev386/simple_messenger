# Backend network settings
BACKEND_NETWORK_NAME = 'messenger_backend'
BACKEND_NETWORK_GATEWAY = '11.0.0.1'
BACKEND_NETWORK_SUBNET = '11.0.0.0/24'

# PostgreSQL database settings
POSTGRES_IMAGE_VERSION = 'postgres:14-alpine3.20'
POSTGRES_CONTAINER_NANE = 'postgres_database'
POSTGRES_ADMIN_PASS = 'admin'
POSTGRES_IPADDR = '11.0.0.3'

# Dev env containter settings
PROJECT_ROOT_DIR_HOST = '.'
PROJECT_ROOT_DIR_CONT = '/workdir/messenger'
DOCKER_SOCK = '/var/run/docker.sock'
DOCKER_RUN_CMD = ['docker', 'run']
DEV_IMAGE_NAME = ['dev_image']
DEFAULT_RUN_ARGS = ['--rm', '-it', '--security-opt seccomp=unconfined']
NETWORK_RUN_ARGS = ['--network ' + BACKEND_NETWORK_NAME, '--ip 11.0.0.2']
RUN_ARGS = DEFAULT_RUN_ARGS + NETWORK_RUN_ARGS
