import os

from constants import *

LIST_OF_MOUNTS_FOR_DEV = [
    f'{PROJECT_ROOT_DIR_HOST}:{PROJECT_ROOT_DIR_CONT}',
    f'{DOCKER_SOCK}:{DOCKER_SOCK}',
    f'{USER_HOST_GIT_CONFIG}:{DOCKER_GIT_CONFIG}',
    f'{HOST_SSH_AUTH_SOCK}:{DOCKER_SSH_AUTH_SOCK}',
]


# Docker management
def docker_run_args_list(mounts_list: list[str]) -> list[str]:
    args_list = RUN_ARGS
    for mounts in mounts_list:
        args_list += ['-v', mounts]
    return args_list

def run_dev_image_cmd() -> str:
    args_list = docker_run_args_list(LIST_OF_MOUNTS_FOR_DEV)
    project_root = os.path.abspath(os.curdir)
    args_list = args_list + ['--env',f'PROJECT_ROOT_DIR={project_root}']
    cmd_words = DOCKER_RUN_CMD + args_list + DEV_IMAGE_NAME
    return ' '.join(cmd_words)

def build_dev_image_cmd(fast: bool) -> str:
    cache_flag = [] if fast is True else ['--no-cache']
    return ' '.join(['docker', 'build'] + cache_flag + ['-t'] + DEV_IMAGE_NAME + ['--ssh','default','.'])

def rm_dev_image_cmd() -> str:
    return ' '.join(['docker', 'image', 'rm'] + DEV_IMAGE_NAME)

def run_dev_image() -> None:
    cmd = run_dev_image_cmd()
    os.system(f'{cmd}')

def build_dev_image(fast: bool) -> None:
    cmd = build_dev_image_cmd(fast)
    os.system(f'{cmd}')

def rm_dev_image() -> None:
    cmd = rm_dev_image_cmd()
    os.system(f'{cmd}')

def run_postgres_container() -> None:
    os.system(f'docker run -d --name {POSTGRES_CONTAINER_NANE} \
                              --network {BACKEND_NETWORK_NAME} \
                              --ip {POSTGRES_IPADDR} \
                              -e POSTGRES_PASSWORD={POSTGRES_ADMIN_PASS} \
                              {POSTGRES_IMAGE_VERSION}')

def shutdown_postgres_container() -> None:
    os.system(f'docker stop {POSTGRES_CONTAINER_NANE}')
    os.system(f'docker rm {POSTGRES_CONTAINER_NANE}')

def setup_testbench_containers() -> None:
    os.makedirs(f'{ARTIFACTS_DIR}/server', exist_ok = True)
    os.makedirs(f'{ARTIFACTS_DIR}/client1', exist_ok = True)
    os.makedirs(f'{ARTIFACTS_DIR}/client2', exist_ok = True)
    os.system('PROJECT_ROOT_DIR=${PROJECT_ROOT_DIR} docker compose up -d --build')

def destroy_testbench_containers() -> None:
    os.system('docker compose down')
    os.system('docker image rm $(docker images messenger* -q)')


# Project build
def clear_project() -> None:
    if os.path.exists(f'{BUILD_DIR}'):
        os.system(f'rm -rf {BUILD_DIR}')
    if os.path.exists(f'{ARTIFACTS_DIR}'):
        os.system(f'rm -rf {ARTIFACTS_DIR}')

def build_project() -> None:
    if not os.path.exists(f'{BUILD_DIR}'):
        os.mkdir(f'{BUILD_DIR}')
    os.chdir(f'{BUILD_DIR}')
    os.system(f'cmake -DCLANG_TIDY_CHECK={CLANG_CHECK} \
                      -DCMAKE_TOOLCHAIN_FILE=../{CMAKE_TOOLCHAIN_FILE} ..; \
                      cmake --build . -j {BUILD_THREAD_NUM}')
