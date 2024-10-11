#!/usr/bin/python3

import os

from constants import (
    BACKEND_NETWORK_NAME,
    BACKEND_NETWORK_GATEWAY,
    BACKEND_NETWORK_SUBNET,
    POSTGRES_IMAGE_VERSION
)

if __name__ == "__main__":
    # 1) Create Docker backend subnet for access to backend services 
    #    both from dev and server app containers
    os.system(f'docker network create -d bridge {BACKEND_NETWORK_NAME} \
                --gateway {BACKEND_NETWORK_GATEWAY} \
                --subnet {BACKEND_NETWORK_SUBNET}')
    
    # 2) Download postgresql Docker container if it is not done yet
    os.system(f'docker pull {POSTGRES_IMAGE_VERSION}')
