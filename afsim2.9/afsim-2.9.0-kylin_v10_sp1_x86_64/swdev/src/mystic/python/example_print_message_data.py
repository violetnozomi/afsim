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

# example_print_message_data
# This example script formats and prints all data from each message of the input AER file.
# pymystic.reader.read returns messages as Python dictionaries. Some entries in a dictionary
# are themselves dictionaries
# This demonstrates using PyMystic to access all message data discretely, recursively
# parsing nested dictionary entries

import pymystic
import sys

# FormatMessage
# A recursive message formatting function
def FormatMessage(aMsg, aIndent = 0, aIndentChar = '\t', aIndentSize = 8, aColJust = 50, aLengthLimit = 200, aEntryNameDelimiter = ':'):
   formattedMessage = ''
   for entry in aMsg:
      indent = ''
      for i in range (0, aIndent):
         indent += aIndentChar
      entryName   = str(entry)
      entryOffset = len(aIndentSize * indent + entryName)
      formattedMessage += indent + entryName + aEntryNameDelimiter
      if isinstance(aMsg[entry], dict):
         formattedMessage += '\n' + FormatMessage(aMsg[entry], aIndent+1)
      else:
         entryJust  = str().ljust(aColJust - entryOffset)
         entryValue = str(aMsg[entry])[0:aLengthLimit]
         formattedMessage += entryJust + entryValue + '\n'
   return formattedMessage

with pymystic.Reader(sys.argv[1]) as r:
   for message in r:
      print(FormatMessage(message))
