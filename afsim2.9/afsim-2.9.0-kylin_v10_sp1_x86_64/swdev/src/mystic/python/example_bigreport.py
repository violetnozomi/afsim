# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# example_bigreport will read a list of aerfiles in 'aerlist.txt' and print a
# report of the version and time of execution for every file in the list.  This
# demonstrates using pymystic for batch processing.
#
# It is run with python:
# > python example_bigreport

import pymystic

def testfile(filename):
   print('TESTING ', filename)
   with pymystic.Reader(filename) as r:
      a = r.read()
      if (a):
         if (a['msgtype'] == 'MsgExecData'):
            if ('version' in a):
               print('   version:  ', a['version'])
            if ('timeStamp' in a):
               print('   executed: ', a['timeStamp'])
            return
         print('   ***unversioned***')
         return

list = open('aerlist.txt')
s = list.readline()
while (s):
   s = s.strip()
   testfile(s)
   s = list.readline()
