import tools.py3.cmd_shortcuts as docker_cmds

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
