# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# =============================================================================
#            This script runs a sequence of automated tests.
#
# Directories starting with "auto_" contain test cases. Text files within the
# directory that start with "test_" are consider the individual tests.
# =============================================================================

from __future__ import print_function

import fnmatch
import optparse
import os
import subprocess
import sys
import threading
import time
from glob import glob

os.environ['AUTO_TEST'] = '1'

parser = optparse.OptionParser(description='run tests')
parser.add_option('-v', action='store_true', help='Display a message as each directory or file is processed.')
parser.add_option('-p', action='store_true', help='Display pass messages as well as fail messages.')
parser.add_option('-d', metavar="dir-pat", action='append', default=[],
                  help='The pattern indicating the directories to be processed. This value should be quoted if it '
                       'contains wild cards. May be specified multiple times.')
parser.add_option('-f', metavar="file-pat", action='append', default=[],
                  help='The pattern indicating the files to be processed (without the .txt extension). This value '
                       'should be quoted if it contains wild cards.  May be specified multiple times.')
parser.add_option('-j', metavar='jobs', help='Number of concurrent tests', default=1)
parser.add_option('-x', metavar="prog", help='The name of the executable to be tested')
parser.add_option('--run-valgrind', action='store_true')
parser.add_option('--vg-extra',metavar="vg-extra",action='store', default='',
                  help='Pass in user specific flags for Valgrind. Flags for Valgrind must be surrounded'
                       ' by quote marks after an equals symbol such as --vg_extra="-v --keep-debuginfo=yes".  The'
                       ' quote marked string will be split on spaces and each token will be passed as a flag to'
                       ' Valgrind.')

args, _ = parser.parse_args()
args = vars(args)
# print(args)
verbose = args['v']
show_passes = args['p']
dir_patterns = args['d']
run_valgrind = args['run_valgrind']
vg_extra = []
if len(args['vg_extra']) > 0:
    vg_extra = args['vg_extra'].split(' ')
# Use this if parsing valgrind output in .lis files
# valgrindPassRE = re.compile('==[0-9]*== ERROR SUMMARY: 0 error')

if not len(dir_patterns):
    dir_patterns = ['auto_*']
file_patterns = args['f']
if not len(file_patterns):
    file_patterns = ['test_*']
test_program = args['x']
if test_program is None:
    print('-x option required')
    sys.exit(1)
test_program_arg = test_program

test_program = os.path.abspath(test_program)

if not os.path.exists(test_program):
    print('-FAIL- Test program', args['x'], 'does not exist')
    sys.exit(1)

print("-TEST- Using", test_program)

for d in dir_patterns:
    print("-TEST- directory", d)


# Locate the directory containing this script and form the name
# of the execution interface scripts needed to execute the tests.
def path_to_auto_test():
    import __main__
    return os.path.abspath(__main__.__file__)


proc_fullname = path_to_auto_test()

allTests = set()
# For each tests root directory pattern
for dirPattern in dir_patterns:
    # print('Checking dir pattern',dirPattern)
    # Get each test root directory that matches the pattern
    for d in glob(dirPattern):
        # print('  Checking dir',d)
        # For each test file name pattern
        for filePattern in file_patterns:
            # Look through the root and all sub directories
            #  for any files with a name that matches the
            # file pattern and add them to the tests list
            for root, dirNames, fileNames in os.walk(d):
                for f in fnmatch.filter(fileNames, filePattern + '.txt'):
                    # print('   Found file ',os.path.join(root, f))
                    allTests.add(os.path.join(root, f))
allTests = sorted(list(allTests))
print('Running', len(allTests), 'tests')

testResults = {}


def get_test_output(test):
    base, file = os.path.split(test)
    file_base, ext = os.path.splitext(file)
    lis_file = os.path.join(base, file_base + '.lis')
    return lis_file


allProcs = []

threadedPrintLock = threading.Lock()

def process_test(test):
    lis_file = get_test_output(test)
    base, file = os.path.split(test)
    file_base, ext = os.path.splitext(file)
    has_auto_exe = os.path.exists(base + '/auto_execute.py')
    if os.path.exists(lis_file):
        os.remove(lis_file)
    start_time = time.time()
    return_code = 0
    fail_lines = []
    pass_lines = []
    test_ran_valgrind = False
    with open(lis_file, 'w') as of:
        try:
            args = [test_program, file]
            if has_auto_exe:
                # if a auto_execute.py script is present, do that instead of running the program
                # valgrind can't run with these kinds of tests
                args = [sys.executable, 'auto_execute.py', test_program, file]
            elif run_valgrind:
                vgxml_file = file_base + '.vg.xml'
                # TODO check to see if valgrind version >= 3.14, and add --keep-debuginfo flag.
                # otherwise valgrind won't tell us where the leaks occurred whenever they occurred in a plugin, since we dlclose the plugins
                # during program shutdown, and valgrind unloads their debug information during the dlclose.
                args = ['valgrind', '--trace-children=yes', '--read-inline-info=yes', '--num-callers=24',
                        '--leak-check=full', '--error-exitcode=42', '--xml=yes', '--xml-file=' + vgxml_file] + vg_extra + args
                test_ran_valgrind = True
            if verbose:
                with threadedPrintLock:
                    print('running: ./' + base + '$ ' + ' '.join(args))
                    sys.stdout.flush()
            # on windows, there are problems when not using a shell
            # on linux, there are problems when using a shell
            use_shell = os.name == 'nt'
            proc = subprocess.Popen(args, cwd=base, stdout=of, stderr=of, shell=use_shell)
            allProcs.append(proc)
            proc.wait()
            return_code = proc.returncode
        except OSError:
            fail_lines.append('-FAIL- Failed to execute program')
        except IOError:
            fail_lines.append('-FAIL- IO error with' + lis_file)

    time_delta = time.time() - start_time

    # have the completed text be printed in the same print statement
    # as the success/fail flag to allow correct output parsing during
    # parallel runs
    testoutput = ''
    if verbose:
        testoutput ='Completed: ' + test + ' :  '+str(round(time_delta,1))+' s\n'

    try:
        with open(lis_file, 'r') as f:
            lis_text = f.readlines()
            found_valgrind_pass = False
            for line in lis_text:
                if line.find('-FAIL-') != -1:
                    fail_lines.append(line.rstrip())
                if line.find('-PASS-') != -1:
                    pass_lines.append(line.rstrip())
                if test_ran_valgrind:
                    # Use following line if parsing .lis files
                    # if valgrindPassRE.search(line) is not None:
                    if return_code != 42:
                        found_valgrind_pass = True
            if test_ran_valgrind and not found_valgrind_pass:
                fail_lines.append('-VALGRIND Errors detected-')
            if return_code != 0:
                fail_lines.append('-FAIL- Exited with status ' + str(return_code))
            if len(fail_lines):
                testResults[test] = (False, fail_lines, time_delta)
                testoutput += 'x'
            else:
                testoutput += '.'
                testResults[test] = (True, pass_lines, time_delta)
    except Exception as e:
        testoutput += e + '\n' + 'Failed to open lis file: ' + lis_file + '\n'

    with threadedPrintLock:
        print(testoutput, end = '')
        sys.stdout.flush()


def run_all_tests():
    for test in allTests:
        lis_file = get_test_output(test)
        if os.path.exists(lis_file):
            os.remove(lis_file)
    start_time = time.time()
    threading = args['j']
    threading = min(int(threading), len(allTests))
    if threading != 1 and len(allTests) > 0:
        # only use this dependency if requested
        from multiprocessing.pool import ThreadPool
        pool = ThreadPool(threading)
        # chunk size=1 to ensure maximum parallelism of tests
        pool.map_async(process_test, allTests,1).get(999999)
    else:
        for t in allTests:
            process_test(t)

    print('\nTests Complete in', round(time.time() - start_time, 1), 's')

    # Clean up any files created by the tests with extension .testout*
    for d in dir_patterns:
        for f in glob('%s/**/*.testout*' % d, recursive=True):
            os.remove(f)

    passes, fails = 0, 0
    for test in allTests:
        if test not in testResults:
            print('Failed to run test: ' + test)
        else:
            result, lines, time_delta = testResults[test]
            if result:
                passes += 1
            else:
                fails += 1
            if len(lines) and (not result or show_passes and result is True):
                result_text = 'PASSED' if result else 'FAILED'
                print('-TEST- ' + result_text + ': ' + test, end='')
                if not result:
                    print(' see', get_test_output(test))
                for line in lines:
                    print(line)

    print('---------------------------------------------------')
    print(passes, 'tests passed', end='')
    if fails:
        print(',', fails, 'tests failed')
    else:
        print()
    if fails:
        print('-FAIL-', fails, 'total error(s) detected')
    else:
        print('-PASS- No errors detected')
    return not fails


# On exception, stop jobs
try:
    if not run_all_tests():
        sys.exit(1)
except Exception:
    print('Stopping test runs...')
    for proc in allProcs:
        if proc.returncode is None:
            proc.terminate()
    raise
