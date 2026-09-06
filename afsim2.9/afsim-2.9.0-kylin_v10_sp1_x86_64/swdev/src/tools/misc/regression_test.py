# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# =================================================#
# This script runs a sequence of regression tests. #
# =================================================#

from __future__ import print_function

import atexit
import fnmatch
import optparse
import os
import subprocess
import sys
import time
import re
from glob import glob

os.environ['AUTO_TEST']='1'

parser = optparse.OptionParser(description='run regression tests')
parser.add_option('-v', action='store_true', help='Display a message as each directory or file is processed.')
parser.add_option('-p', action='store_true', help='Display pass messages as well as fail messages.')
parser.add_option('-d', metavar="dir-pat", default=[], help='The list file input root directory.')
parser.add_option('-o', metavar="dir-pat", default=[], help='The output directory for comparison data.')
parser.add_option('-l', metavar="file-pat", action='append', default=[], help='The pattern indicating the input list file containing list of test files.   May be specified multiple times.')
parser.add_option('-j', metavar='jobs', help='Number of concurrent tests', default=1)
parser.add_option('-x', metavar="prog", help='The name of the executable to be tested')
parser.add_option('-t', metavar="prog", help='The name of the tool to be tested')
parser.add_option('--run-valgrind', action='store_true')

args, _ = parser.parse_args()
args = vars(args)
#print(args)
verbose = args['v']
tool = args['t']
show_passes = args['p']
run_valgrind = args['run_valgrind']
# Use this if parsing valgrind output in .lis files
#valgrindPassRE = re.compile('==[0-9]*== ERROR SUMMARY: 0 error')

output_dir = args['o']
if output_dir is None:
    print('-o option required')
    sys.exit(1)
input_file_list = args['l']
if not len(input_file_list):
    print('-FAIL- input file lists', args['l'], 'does not exist')
    sys.exit(1)
test_program = args['x']
if test_program is None:
    print('-x option required')
    sys.exit(1)
test_program_arg = test_program

output_dir = os.path.abspath(output_dir)
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

test_program = os.path.abspath(test_program)
if not os.path.exists(test_program):
    print('-FAIL- Test program', args['x'], 'does not exist')
    sys.exit(1)

print("-REGRESSION TEST- Using", test_program)

#
## Locate the directory containing this script and form the name
## of the execution interface scripts needed to execute the tests.
#

def path_to_auto_test():
    import __main__
    return os.path.abspath(__main__.__file__)
proc_fullname = path_to_auto_test()

allTests = set()
for inputFile in input_file_list:
   input_file = os.path.abspath(inputFile)
   f = open(input_file, 'r')
   for line in f:
      input_dir = os.path.dirname(input_file)
      line = line.rstrip('\n')
      testBase, testFile = os.path.split(os.path.normpath(line))
      testDir = os.path.abspath(os.path.join(input_dir, '../' + testBase))
      outDir = os.path.abspath(os.path.join(output_dir, testBase))
      if not os.path.exists(outDir):
         os.makedirs(outDir)
      allTests.add((testDir, outDir, testFile))
allTests = sorted(list(allTests))
print('Running',len(allTests),'tests')

def create_output_items(outDir, fileBase):
   regFile = os.path.join(outDir, "regression_extra_" + fileBase + ".txt")
   if not os.path.isfile(regFile):
      ef = open(regFile, 'w')
      if tool == 'mission':
         ef.write("non-realtime\n")
         ef.write("dis_interface\n   connections\n   end_connections\nend_dis_interface\n\n")
         ef.write("event_pipe\n   file NULL\nend_event_pipe\n\n")
         #ef.write("csv_event_output\n   file ")
         #ef.write(os.path.join(outDir, fileBase + ".csv"))
         #ef.write("\n   enable all\nend_csv_event_output\n\n")
         ef.write("csv_event_output\n   file NULL\nend_csv_event_output\n\n")
         ef.write("event_output\n   file ")
         ef.write(os.path.join(outDir, fileBase + ".evt"))
         ef.write("\n   enable all\nend_event_output\n\n")
         #ef.write("event_output\n   file NULL\nend_event_output\n\n")
      ef.close()
   lisFile = os.path.join(outDir, fileBase + '.lis')
   return regFile, lisFile

testResults = {}
allProcs = []
def process_test(test):
   inDir = test[0]
   outDir = test[1]
   file = test[2]
   inPath = os.path.join(inDir, file)
   fileBase, ext = os.path.splitext(file)
   regFile, lisFile = create_output_items(outDir, fileBase)
   hasAutoExe = os.path.exists(inDir + '/auto_execute.py')
   if os.path.exists(lisFile):
       os.remove(lisFile)
   tStart = time.time()
   returnCode = 0
   failLines = []
   passLines = []
   testRanValgrind = False
   with open(lisFile,'w') as of:
      try:
         args = [test_program, inPath, regFile]
         if hasAutoExe:
             # if a auto_execute.py script is present, do that instead of running the program
             # valgrind can't run with these kinds of tests
             args = [sys.executable, 'auto_execute.py', test_program, inPath, regFile]
         elif run_valgrind:
             vgxmlFile = os.path.join(outDir, fileBase + '.vg.xml')
             args = ['valgrind', '--trace-children=yes', '--read-inline-info=yes', '--num-callers=24', '--leak-check=full', '--error-exitcode=42', '--xml=yes', '--xml-file=' + vgxmlFile] + args
             testRanValgrind = True
         if verbose:
             print('running: ./' + inPath + '$ ' + ' '.join(args))
         # on windows, there are problems when not using a shell
         # on linux, there are problems when using a shell
         useShell = shell=os.name=='nt'
         proc = subprocess.Popen(args, cwd=outDir, stdout=of, stderr=of, shell=useShell)
         allProcs.append(proc)
         proc.wait()
         returnCode = proc.returncode
      except OSError:
         failLines.append('-FAIL- Failed to execute program')
      except IOError:
         failLines.append('-FAIL- IO error with' + lisFile)

   tDelta = time.time() - tStart
   try:
     lisText = open(lisFile,'r').readlines()
     foundValgrindPass = False
     for line in lisText:
         if line.find('-FAIL-') != -1:
             failLines.append(line.rstrip())
         if line.find('-PASS-') != -1:
             passLines.append(line.rstrip())
         if testRanValgrind:
             # Use following line if parsing .lis files
             #if valgrindPassRE.search(line) is not None:
             if returnCode is not 42:
                 foundValgrindPass = True
     if testRanValgrind and not foundValgrindPass:
         failLines.append('-VALGRIND Errors detected-')
     if returnCode != 0:
         failLines.append('-FAIL- Exited with status ' + str(returnCode))
     if len(failLines):
         testResults[inPath] = (False, failLines, tDelta)
         print('x',end='')
     else:
         print('.',end='')
         testResults[inPath] = (True, passLines, tDelta)
   except:
     print('Failed to open lis file: ', lisFile)
   sys.stdout.flush()
   if os.path.exists(regFile):
      os.remove(regFile)

def run_all_tests():
   startTime = time.time()
   threading = args['j']
   if threading != 1:
      # only use this dependency if requested
      from multiprocessing.pool import ThreadPool
      pool = ThreadPool(int(threading))
      pool.map_async(process_test, allTests, 1).get(999999)
   else:
      for t in allTests:
         process_test(t)

   print('\nTests Complete in', round(time.time() - startTime, 1), 's')

   passes, fails = 0, 0
   for test in allTests:
      inDir = test[0]
      outDir = test[1]
      file = test[2]
      inPath = os.path.join(inDir, file)
      if inPath not in testResults:
         print('Failed to run test: ' + inPath)
      else:
         result, lines, tDelta = testResults[inPath]
         if result: passes+=1
         else: fails += 1
         if len(lines) and (not result or show_passes and result is True):
             resultText = 'PASSED' if result else 'FAILED'
             print('-TEST- ' + resultText + ': ' + inPath, end='')
             if not result:
                 print(' see output .lis file for more information')
             for line in lines:
                 print(line)

   print('---------------------------------------------------')
   print(passes, 'tests passed',end='')
   if fails: print(',', fails, 'tests failed')
   else: print()
   if fails: print('-FAIL-', fails, 'total error(s) detected')
   else: print('-PASS- No errors detected')
   return not fails

# On exception, stop jobs
try:
    if not run_all_tests():
        sys.exit(1)
except:
    print('Stopping test runs...')
    for proc in allProcs:
        if proc.returncode is None: proc.terminate()
    raise
