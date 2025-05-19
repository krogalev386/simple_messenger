import os
import docker

from constants import *


class DockerCliMngr:
    def __enter__(self):
        self.client = docker.from_env()
        return self.client
    def __exit__(self, exc_type, exc_value, traceback):
        self.client.close()


def run_postgres_container() -> None:
    with DockerCliMngr() as client:
        container = client.containers.run(
            POSTGRES_IMAGE_VERSION,
            detach=True,
            name=POSTGRES_CONTAINER_NANE,
            environment={
                'POSTGRES_PASSWORD': POSTGRES_ADMIN_PASS
            },
        )
        network = client.networks.get(BACKEND_NETWORK_NAME)
        network.connect(container, ipv4_address=POSTGRES_IPADDR)


def shutdown_postgres_container() -> None:
    with DockerCliMngr() as client:
        containers = client.containers.list(all=True, filters={'name': POSTGRES_CONTAINER_NANE})
        if containers != []:
            container = containers[0]
            container.stop()
            container.remove()
        else:
            print("No psql container is running")


def setup_testbench_containers() -> None:
    with DockerCliMngr() as client:
        print("Check if base image exists...")
        base_image = None
        if client.images.list(filters={"reference": APP_BUILD_BASE_IMAGE_NAME}) != []:
            base_image = client.images.get(APP_BUILD_BASE_IMAGE_NAME)
            print("base build image found")
        else:
            print("base image is not found, build image...")
            build_app_base_image()
        print("Base image exists.")
        print("Check if postgres container is init and up...")
        containers = client.containers.list(all=True, filters={'name': POSTGRES_CONTAINER_NANE})
        if containers == []:
            print("Postgres contaier does not exist. Booting it now...")
            run_postgres_container()

    os.makedirs(f'{ARTIFACTS_DIR}/server', exist_ok = True)
    os.makedirs(f'{ARTIFACTS_DIR}/client1', exist_ok = True)
    os.makedirs(f'{ARTIFACTS_DIR}/client2', exist_ok = True)
    os.system('PROJECT_ROOT_DIR=${PROJECT_ROOT_DIR} docker compose up -d --build')


def destroy_testbench_containers() -> None:
    os.system('docker compose down')
    os.system('docker image rm $(docker images messenger* -q)')


# Handle the base image for application assembly
def build_app_base_image() -> None:
    with DockerCliMngr() as client:
        print("Building base application image...")

        build_logs = client.api.build(path='.',
                                      dockerfile='./tools/docker/app_build_base/Dockerfile',
                                      tag=f'{APP_BUILD_BASE_IMAGE_NAME}:latest',
                                      decode=True,
                                      rm=True)

        for event in build_logs:
            if 'stream' in event:
                if "Step" in event['stream'].strip():
                    print(event['stream'].strip())
                else:
                    print(event['stream'].strip(), end="\r")


def remove_app_base_image() -> None:
    with DockerCliMngr() as client:
        if client.images.list(filters={"reference": APP_BUILD_BASE_IMAGE_NAME}) == []:
            print("base image does not exists")
            return
        client.images.remove(APP_BUILD_BASE_IMAGE_NAME)


def update_app_base_image() -> None:
    remove_app_base_image()
    build_app_base_image()


# Project build
def clear_project() -> None:
    if os.path.exists(f'{BUILD_DIR}'):
        os.system(f'rm -rf {BUILD_DIR}')
    if os.path.exists(f'{ARTIFACTS_DIR}'):
        os.system(f'rm -rf {ARTIFACTS_DIR}')
    if os.path.exists('logs'):
        os.system('rm -rf logs')

def build_project() -> None:
    if not os.path.exists(f'{BUILD_DIR}'):
        os.mkdir(f'{BUILD_DIR}')
    if not os.path.exists('logs'):
        os.mkdir('logs')
    os.chdir(f'{BUILD_DIR}')
    os.system(f'cmake -DCLANG_TIDY_CHECK={CLANG_CHECK} \
                      -DBUILD_GOOGLE_TESTS=0 \
                      -DCMAKE_TOOLCHAIN_FILE=../{CMAKE_TOOLCHAIN_FILE} ..; \
                      cmake --build . -j {BUILD_THREAD_NUM}')

def check_project() -> None:
    if not os.path.exists(f'{BUILD_DIR}'):
        os.mkdir(f'{BUILD_DIR}')
    os.chdir(f'{BUILD_DIR}')
    os.system(f'cmake -DBUILD_GOOGLE_TESTS=1 \
                      -DCMAKE_TOOLCHAIN_FILE=../{CMAKE_TOOLCHAIN_FILE} ..; \
                      cmake --build . --target ut_target \
                      -j {BUILD_THREAD_NUM}')
    os.system(f'ctest')
