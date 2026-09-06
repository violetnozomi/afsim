# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
#!/usr/bin/env python3

from contextlib import contextmanager
import subprocess
import os

# Get root directory of script
script_path = os.path.dirname(os.path.realpath(__file__))
# Blacklist directories
blacklist = ['.git',
             'build',
             'images',
             'message_examples',
             'tspi',
             'engage_pk_tables',
             'dted',
             'FindPython']


def is_no_export_dir(path):
    children = os.listdir(path)
    for node in children:
        if node == 'NO_EXPORT.md':
            return True
    return False


def update_file_markings(path, no_export):
    cmd = ['perl', os.path.join(script_path, 'change_file_markings_itar.pl')]
    if no_export:
        cmd.append('--nx')
    cmd.append('%s/*.*' % path)
    subprocess.run(cmd)


def recurse_directories(cur_path, no_export):
    no_export = no_export or is_no_export_dir(cur_path)
    update_file_markings(cur_path, no_export)
    children = os.listdir(cur_path)
    for child in children:
        child_path = os.path.join(cur_path, child)
        if child not in blacklist and os.path.isdir(child_path):
            recurse_directories(child_path, no_export)


if __name__ == '__main__':
    recurse_directories(os.getcwd(), False)
