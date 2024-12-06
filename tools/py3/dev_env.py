#!/usr/bin/python3

import cmd_shortcuts as docker_cmds
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("-rm","--remove", help="Remove dev_env image", action="store_true")
parser.add_argument("-b","--build", help="Build dev_env image from scratch", action="store_true")
parser.add_argument("-r","--run", help="Run dev_env image", action="store_true")
parser.add_argument("-u","--update", help="Update dev_env image", action="store_true")
args = parser.parse_args()

if args.remove:
    docker_cmds.rm_dev_image()
elif args.build:
    docker_cmds.build_dev_image(False)
elif args.update:
    docker_cmds.build_dev_image(True)
elif args.run:
    docker_cmds.run_dev_image()
else:
    docker_cmds.run_dev_image()
