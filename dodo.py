import sys, os
sys.path.append( f'{os.getcwd()}/tools/py3' )

import cmd_shortcuts as docker_cmds

DOIT_CONFIG = {'default_tasks': []}

def task_build_dev_env():
    return {
        'actions': [docker_cmds.build_dev_image],
    }

def task_rm_dev_env():
    return {
        'actions': [docker_cmds.rm_dev_image],
    }

def task_run_dev_env():
    task_build_dev_env()
    return {
        'actions': [
            docker_cmds.rm_dev_image,
            docker_cmds.build_dev_image,
            docker_cmds.run_dev_image,
        ],
    }

def task_build_project():
    return {
        'actions': [docker_cmds.build_project],
    }

def task_clear_project():
    return {
        'actions': [docker_cmds.clear_project],
    }

def task_rebuild_project():
    return {
        'actions': [
            docker_cmds.clear_project,
            docker_cmds.build_project,
        ],
    }

def task_setup_psql():
    return {
        'actions': [docker_cmds.run_postgres_container],
    }

def task_shutdown_psql():
    return {
        'actions': [docker_cmds.shutdown_postgres_container],
    }

def task_setup_testbench():
    return {
        'actions' :[docker_cmds.setup_testbench_containers],
    }

def task_destroy_testbench():
    return {
        'actions' :[docker_cmds.destroy_testbench_containers],
    }

def task_build_app_base_image():
    return {
        'actions' :[docker_cmds.build_app_base_image],
    }

def task_remove_app_base_image():
    return {
        'actions' :[docker_cmds.remove_app_base_image],
    }

def task_update_app_base_image():
    return {
        'actions' :[docker_cmds.update_app_base_image],
    }
