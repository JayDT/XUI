#!/usr/bin/env python
#coding: utf-8

import os, distutils.dir_util, getopt, sys

def current_folder():
    return os.path.basename(os.getcwd())

def template_project_path():
    return os.path.join(os.environ['CODE'], 'cmake', 'template_project')

def make_folders():
    distutils.dir_util.copy_tree(template_project_path(), os.getcwd())

def fill(path, dic):
    with open(path, 'r') as f:
        text = f.read()
    text = text % dic
    with open(path, 'w') as f:
        f.write(text)

def make_root_cmake():
    fill('CMakeLists.txt', {'project_name': current_folder()})

def make_config_cmake(): pass

def make_files():
    make_root_cmake()
    #make_config_cmake()

def usage():
    s = """prepare_cmake_project will copy all files in the template_project
directory and replace some fields in the cmake files"""
    print(s)

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'h', ['help'])
    except getopt.GetoptError, e:
        print str(e)
        usage()
        sys.exit(2)

    for o, a in opts:
        if o in ['-h', '--help']:
            usage()
            sys.exit()

    make_folders()
    make_files()
    #make_symbol_links()

if __name__ == '__main__':
    main()
