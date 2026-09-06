#!/usr/bin/env python3
# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# CMAKE commands to exercise script
#   ADD_CUSTOM_TARGET(DOCUMENTATION COMMAND ${PYTHON_EXECUTABLE} build_doc.py -o html -b ${CMAKE_BINARY_DIR}/documentation -d \"${WSF_ROOT}/wsf\" ${WSF_OPTIONAL_PROJECT_DOCDIRS}\" WORKING_DIRECTORY ${WSF_ROOT}/wsf/sphinx)
#   ADD_CUSTOM_TARGET(DOCUMENTATION_clean COMMAND ${PYTHON_EXECUTABLE} build_doc.py clean -b ${CMAKE_BINARY_DIR}/documentation WORKING_DIRECTORY ${WSF_ROOT}/wsf/sphinx)

import argparse
import os
import shutil
import subprocess
import sys

def safe_path_string(path):
   # Emit the repr representation of a path string without surrounding quotes. This is fairly
   # equivalent to doing `return path.replace('\\', '/')` but hopefully more robust.
   return repr(path)[1:-1]

def clean(top):
   top = os.path.abspath(top) # get absolute resolved path for clarity in output messages
   if os.path.isfile(top) or os.path.islink(top):
      print('Cleaning: ' + top)
      os.remove(top)
   elif os.path.isdir(top):
      # Don't clean the entire directory, that wipes the containing source directory by default!
      cleaning_candidates = ["dirhtml", "docs", "doctrees", "html", "htmlhelp", "latex", "site_index.rst"]
      to_clean = [entry for entry in os.listdir(top) if entry in cleaning_candidates]
      if len(to_clean) > 0:
         print('Cleaning generated content from within: ' + top)
      for item in to_clean:
         item_path = os.path.join(top, item)
         print('Cleaning ' + item_path)
         if os.path.isfile(item_path) or os.path.islink(item_path):
            os.remove(item_path)
         elif os.path.isdir(item_path):
            shutil.rmtree(item_path)
         else:
            print("Error: don't know how to clean directory entry " + item)
            sys.exit(1)

def make_pdf(arg, outputDir):
   cwd=os.getcwd()
   os.chdir(outputDir)
   cmd = ['make', arg]
   if verbose:
      print('Running: ' + " ".join(cmd))
   subprocess.run(cmd, check=True)
   os.chdir(cwd)

def generate_sphinx_conf(build_directory):
   # Emulate CMake generation of the Sphinx conf.py into the specified build directory (base for all output formats).
   # This allows having just a single conf.py.in while still preserving the non-CMake doc generation handling.
   conf_template_replacements = {
      ("@AFSIM_COMPANYNAME@", "United States Air Force (USAF)"),
      ("@AFSIM_LEGALCOPYRIGHT@", "Multiple"),
      ("@AFSIM_LEGALTRADEMARKS@", "All Rights Reserved"),
      ("@AFSIM_VERSION@", "direct-unversioned"),
      ("@AFSIM_VERSION_MAJOR@.@AFSIM_VERSION_MINOR@", "unversioned"),
      ("'@SPHINX_BUILD_DIR@'", "os.environ.get('SPHINX_BUILD_DIR', os.path.dirname(os.path.realpath(__file__)))"),
      ("'@SPHINX_DOC_PATHS@'", "os.environ.get('SPHINX_DOC_PATHS', '" +
         safe_path_string(os.path.normpath(os.path.join(script_dir, "../../.."))) + "')"),
      ("@SPHINX_THEME_DIR@", safe_path_string(script_dir)),
      ("@SPHINX_THEME_NAME@", "wsftheme"),
      ("@SPHINX_EXTENSIONS_DIR@", safe_path_string(os.path.join(script_dir, "extensions"))),
      ("@SPHINX_SIDEBARS_EXTS@", ""), # CMake collected sidebar entries aren't available
      ("@WSF_SPHINX_DIR@", safe_path_string(script_dir)),
      ("@WSF_SPHINX_FAVICON@", safe_path_string(os.path.join(script_dir, "_static", "favicon.ico"))),
      ("'@WSF_SPHINX_PYTHON_MODULES@'", "os.environ.get('WSF_SPHINX_PYTHON_MODULES', '" +
         safe_path_string(os.path.normpath(os.path.join(script_dir, "../../../mystic/python"))) + "')"),
      ("@WSF_SPHINX_SIDEBAR_LOGO@", safe_path_string(os.path.join(script_dir, "_static", "sidebar_logo.png"))),
   }
   with open(os.path.join(script_dir, 'conf.py.in'), 'r') as conf_template:
      conf_content = conf_template.read()
      for (search, replace) in conf_template_replacements:
         conf_content = conf_content.replace(search, replace)
   if not os.path.isdir(build_directory):
      os.makedirs(build_directory)
   with open(os.path.join(build_directory, 'conf.py'), 'w') as build_conf:
      build_conf.write(conf_content)

script_dir = os.path.dirname(os.path.realpath(__file__))

parser = argparse.ArgumentParser(description='Generate AFSIM documentation')
parser.add_argument('-v', '--verbose', default=False, action='store_true', help='Display verbose output.')
parser.add_argument('-d', '--directory', dest="input_dirs", metavar="INPUT-DIR", action='append', default=[],
   help='One or more directories within which to search for documentation content. ' +
   'Value may contain glob patterns (in which case the value should be quoted). (default: auto-detected)')
parser.add_argument('-b', '--build-directory', metavar="OUTPUT-DIR", default=os.path.join(script_dir, 'generated-docs'),
   help=f'Output directory in which to generate documentation. (default: {script_dir})')
parser.add_argument('-x', '--sphinx-executable', metavar="EXECUTABLE", default='sphinx-build',
   help='Sphinx executable name to use. (default: sphinx-build)')
parser.add_argument('-o', '--builder', metavar="BUILDER", default='html',
   help='Output format for generated documentation (i.e. Sphinx builder). (default: html)')
parser.add_argument('-s', '--sphinx-arg', dest="extra_args", metavar="ARG", action='append', default=[],
   help='Argument to pass directly to sphinx. This argument may be specified multiple times.')
parser.add_argument('-t', '--tag', dest="tags", metavar="TAG", action='append', default=[],
   help='Content tag for AFSIM documentation type. ' +
   'Valid tags include "afsim-documentation", "afsim-user-manual", and "afsim-changelog". ' +
   '(default: afsim-documentation)')
parser.add_argument('command', nargs='?', choices=('build', 'clean'), default='build',
      help='Action to take. ' +
      'The "build" command will generate documentation according to provided tags. ' +
      'The "clean" command will delete all generated content, including intermediate output. ' +
      '(default: build)')
args = parser.parse_args()
args = vars(args)
#print(args)

verbose = args['verbose']
dir_patterns = args['input_dirs']
if not len(dir_patterns):
   # The repo from which the script is running may not always be named 'afsim'; use the actual name.
   # (The '../../..' is sensitive to relative location of this script within the 'afsim' repo.)
   base_dir, containing_repo = os.path.split(os.path.normpath(os.path.join(script_dir, '../../..')))

   # Try to infer a reasonable default doc set by assuming a common/typical AFSIM source tree layout.
   # This is best-effort - with alternate source naming or layout this may not match anything.
   candidate_directories = [containing_repo, 'afsim_shared', 'demos', 'tools', 'training']
   for candidate in [os.path.join(base_dir, c) for c in candidate_directories]:
      if os.path.isdir(candidate):
         dir_patterns.append(candidate)
   if not len(dir_patterns):
      print('No documentation sources were automatically detected, use "-d INPUT-DIR" to specify documentation input!')
      sys.exit(1)
else:
   # Resolve arguments which may have been specified as relative path input into absolute paths.
   # This is needed since the sphinx-build execution will be done with a different working directory (i.e. in the output directory).
   dir_patterns = [os.path.realpath(dir) for dir in dir_patterns]
os.environ['SPHINX_DOC_PATHS']=" ".join(dir_patterns)

sphinx_cmd = args['sphinx_executable']
sphinx_builder = args['builder']
build_dir = args['build_directory']

command = args['command']
if command == 'build':
   pass
elif command == 'clean':
   clean(build_dir)
   sys.exit(0)
else:
   print("Unrecognized command provided: " + command)
   sys.exit(1)

generate_sphinx_conf(build_dir)

warning_log_file = os.path.join(build_dir, "warning-log-" + sphinx_builder + ".txt")

base_options=["-j", "auto", "-n", "-c", build_dir, "-w", warning_log_file, "-d", os.path.join(build_dir, "doctrees")]

# Default to building the afsim-documentation tag if no other tags are specified.
if len(args['tags']) == 0:
   args['tags'].append('afsim-documentation')

# Note: This is a bit of a kludge that mimics the CMake support for alternate build directories for select doc targets.
#       There is ripe opportunity for improvement, but depends on refactoring the copy/intermediate processing in conf.py.
#       This is just good enough for now, and defers as much tag validation as possible to the shared conf.py processing.
intermediate_src_dir = "unmatched-afsim-doc-tag"
if "afsim-documentation" in args['tags']:
   intermediate_src_dir = build_dir
elif "afsim-changelog" in args['tags']:
   intermediate_src_dir = os.path.join(build_dir, "docs", "changelog")
elif "afsim-user-manual" in args['tags']:
   intermediate_src_dir = os.path.join(build_dir, "docs", "user_manual")
else:
   print("No AFSIM documentation flag recognized, unknown output location!")
   sys.exit(1)

if not os.path.isdir(intermediate_src_dir):
   os.makedirs(intermediate_src_dir)

extra_options=[]
if args['tags']:
   for tag in args['tags']:
      extra_options.extend(['-t', tag])

if verbose:
   extra_options.append('-v')
else:
   extra_options.append('-q')

# Append user specified sphinx options after other options to permit commandline override
extra_options.extend(args['extra_args'])

output_dir=os.path.join(build_dir, sphinx_builder)

cmd = [sphinx_cmd, '-b', sphinx_builder] + base_options + extra_options + [intermediate_src_dir] + [output_dir]

if verbose:
   print('Running: ' + " ".join(cmd))

proc = subprocess.run(cmd)
returnCode = proc.returncode

if returnCode == 0:
   if sphinx_builder == 'html' and build_dir != script_dir:
      # Copy the index.html that will redirect to the buried main_page.html
      shutil.copy(os.path.join(script_dir, 'index.html'), build_dir)
   elif (sphinx_builder == 'latex'):
      make_pdf('all-pdf', output_dir)
   print("\nBuild finished successfully. The generated content is in " + output_dir)
else:
   print('\nBuild Finished with ERRORS!')
