#!/usr/bin/python3

import os
from argparse import ArgumentParser
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


if __name__ == "__main__":

    parser = ArgumentParser()
    parser.add_argument("-rm","--remove", help="Remove dev_env image", action="store_true")
    parser.add_argument("-b","--build", help="Build dev_env image from scratch", action="store_true")
    parser.add_argument("-r","--run", help="Run dev_env image", action="store_true")
    parser.add_argument("-u","--update", help="Update dev_env image", action="store_true")
    args = parser.parse_args()

    if args.remove:
        rm_dev_image()
    elif args.build:
        build_dev_image(False)
    elif args.update:
        build_dev_image(True)
    elif args.run:
        run_dev_image()
    else:
        run_dev_image()
