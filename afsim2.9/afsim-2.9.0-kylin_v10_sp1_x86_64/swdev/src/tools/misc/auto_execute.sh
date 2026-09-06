# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
#!/bin/sh
# This is invoked by auto_test.pl to execute a test case under Unix.

if [ $# -ne 3 ] ; then
   echo "Usage auto_execute.sh <exe-file> <input-base> <output-file>"
   exit 1
fi

# Let the application know that it is running an automated test.
AUTO_TEST=true
export AUTO_TEST

# Limit execution to 60 seconds if not overriden
CPU_TIME_LIMIT=${AUTO_TEST_CPU_TIME_LIMIT:-60}
ulimit -t ${CPU_TIME_LIMIT}

sh_file=$2.sh
pl_file=$2.pl
if [ -f $sh_file ] ; then
   exec $sh_file $1 $2.txt >>$3 2>&1
elif [ -f $pl_file ] ; then
   exec perl $pl_file $1 $2.txt >>$3 2>&1
else
   exec "$1" $2.txt >>$3 2>&1
fi

