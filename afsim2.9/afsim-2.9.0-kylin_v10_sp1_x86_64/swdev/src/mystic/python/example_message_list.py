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

# example_message_list will read message from a provided AER file and generate
# a list of messages showing time, message-type, and if the message is a entity state
# the name of the relevant platform.  This example demonstrates using pymystic
# and pulling data out of specific message types
#
# It is run with python and a command line argument to an AER file:
# > python example_message_list path/myfile.aer

import sys
import pymystic

with pymystic.Reader(sys.argv[1]) as r:
   indexToName = {}
   for a in r:
      if (a['msgtype'] == 'MsgPlatformInfo'):
         indexToName.update({a['platformIndex'] : a['name']})
      if (a['msgtype'] == 'MsgEntityState'):
         print(a['simTime'], a['msgtype'], indexToName[a['state']['platformIndex']])
      else:
         print(a['simTime'], ': ', a['msgtype'])
