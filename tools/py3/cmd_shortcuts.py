import os

PROJECT_ROOT_DIR_HOST = '.'
PROJECT_ROOT_DIR_CONT = '/workdir/messenger'
DOCKER_SOCK = '/var/run/docker.sock'

LIST_OF_MOUNTS_FOR_DEV = [
    f'{PROJECT_ROOT_DIR_HOST}:{PROJECT_ROOT_DIR_CONT}',
    f'{DOCKER_SOCK}:{DOCKER_SOCK}'
]

DOCKER_RUN_CMD = ['docker', 'run']
DEV_IMAGE_NAME = ['dev_image']
DEFAULT_RUN_ARGS = ['--rm', '-it', '--security-opt seccomp=unconfined']
CUSTOM_RUN_ARGS = []
RUN_ARGS = DEFAULT_RUN_ARGS + CUSTOM_RUN_ARGS

# Docker management
def docker_run_args_list(mounts_list: list[str]) -> list[str]:
    args_list = RUN_ARGS
    for mounts in mounts_list:
        args_list += ['-v', mounts]
    return args_list

def run_dev_image_cmd() -> str:
    args_list = docker_run_args_list(LIST_OF_MOUNTS_FOR_DEV)
    cmd_words = DOCKER_RUN_CMD + args_list + DEV_IMAGE_NAME
    return ' '.join(cmd_words)

def build_dev_image_cmd() -> str:
    return ' '.join(['docker', 'build', '-t'] + DEV_IMAGE_NAME + ['.'])

def rm_dev_image_cmd() -> str:
    return ' '.join(['docker', 'image', 'rm'] + DEV_IMAGE_NAME)

def run_dev_image() -> None:
    cmd = run_dev_image_cmd()
    os.system(f'{cmd}')

def build_dev_image() -> None:
    cmd = build_dev_image_cmd()
    os.system(f'{cmd}')

def rm_dev_image() -> str:
    cmd = rm_dev_image_cmd()
    os.system(f'{cmd}')


# Project build

BUILD_DIR = './build'
CMAKE_TOOLCHAIN_FILE = 'CMakeToolchain.cmake'
CLANG_CHECK = 1
BUILD_THREAD_NUM = 4

def clear_project() -> None:
    os.system(f'rm -rf {BUILD_DIR}')

def build_project() -> None:
    if not os.path.exists(f'{BUILD_DIR}'):
        os.mkdir(f'{BUILD_DIR}')
    os.chdir(f'{BUILD_DIR}')
    os.system(f'cmake -DCLANG_TIDY_CHECK={CLANG_CHECK} \
                      -DCMAKE_TOOLCHAIN_FILE=../{CMAKE_TOOLCHAIN_FILE} ..; \
                      cmake --build . -j {BUILD_THREAD_NUM}')
