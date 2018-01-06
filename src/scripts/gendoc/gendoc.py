#!/usr/bin/env python3
# encoding: utf-8
"""
Efl documentation generator

Use this script without arguments to generate the full documentation of the Efl
namespace in a folder called 'dokuwiki' (-v to see all generated files)

  --help to see all other options

"""
import os
import sys
import argparse
import atexit


# Use .eo files from the source tree (not the installed ones)
script_path = os.path.dirname(os.path.realpath(__file__))
root_path = os.path.abspath(os.path.join(script_path, '..', '..', '..'))
SCAN_FOLDER = os.path.join(root_path, 'src', 'lib')


# Use pyolian from source (not installed)
pyolian_path = os.path.join(root_path, 'src', 'scripts')
sys.path.insert(0, pyolian_path)
from pyolian import eolian
from pyolian.generator import Template


# parse args
parser = argparse.ArgumentParser(description='Pyolian DocuWiki generator.')
parser.add_argument('--root-path', '-r', metavar='FOLDER', default='dokuwiki',
                    help='where to write files to (root of dokuwiki) '
                         'default to: "./dokuwiki"')
parser.add_argument('--verbose', '-v', action='store_true',
                    help='print a line for each rendered file')
parser.add_argument('--namespace', '-n', metavar='ROOT', default='Efl',
                    help='root namespace of the docs. (default to "Efl")')
_choices = ['start', 'classes', 'enums', 'structs', 'aliases']
parser.add_argument('--step', '-s', metavar='STEP', default=None,
                    choices=_choices,
                    help='A single step to run (default to all), '
                         'valid choises: '+ ', '.join(_choices))
args = parser.parse_args()


# load the whole eolian db (from .eo files in source tree)
eolian_db = eolian.Eolian()
if not isinstance(eolian_db, eolian.Eolian):
    raise(RuntimeError('Eolian, failed to create Eolian state'))

if not eolian_db.directory_scan(SCAN_FOLDER):
    raise(RuntimeError('Eolian, failed to scan source directory'))

if not eolian_db.all_eot_files_parse():
    raise(RuntimeError('Eolian, failed to parse all EOT files'))
    
if not eolian_db.all_eo_files_parse():
    raise(RuntimeError('Eolian, failed to parse all EO files'))


# cleanup the database on exit
def cleanup_db():
    global eolian_db
    del eolian_db
atexit.register(cleanup_db)


# calculate the full path for the txt page of the given object
def page_path_for_object(obj):
    path = ['data', 'pages', 'develop', 'api']
    for ns in obj.namespaces:
        path.append(ns.lower())
    output_file = obj.name.lower() + '.txt'
    return os.path.join(args.root_path, *path, output_file)


# render the main start.txt page
if args.step in ('start', None):
    t = Template('doc_start.template')
    output_file = os.path.join(args.root_path,'data','pages','develop','api','start.txt')
    t.render(output_file, args.verbose, nspaces=eolian_db.all_namespaces)

# render a page for each Class
if args.step in ('classes', None):
    t = Template('doc_class.template')
    for cls in eolian_db.all_classes:
        if cls.full_name.startswith(args.namespace):
            output_file = page_path_for_object(cls)
            t.render(output_file, args.verbose, cls=cls.full_name)

# render a page for each Enum
if args.step in ('enums', None):
    t = Template('doc_enum.template')
    for enum in eolian_db.typedecl_all_enums:
        if enum.full_name.startswith(args.namespace):
            output_file = page_path_for_object(enum)
            t.render(output_file, args.verbose, enum=enum.full_name)

# render a page for each Struct
if args.step in ('structs', None):
    t = Template('doc_struct.template')
    for struct in eolian_db.typedecl_all_structs:
        if struct.full_name.startswith(args.namespace):
            output_file = page_path_for_object(struct)
            t.render(output_file, args.verbose, struct=struct.full_name)

# render a page for each Alias
if args.step in ('aliases', None):
    t = Template('doc_alias.template')
    for alias in eolian_db.typedecl_all_aliases:
        if alias.full_name.startswith(args.namespace):
            output_file = page_path_for_object(alias)
            t.render(output_file, args.verbose, alias=alias.full_name)