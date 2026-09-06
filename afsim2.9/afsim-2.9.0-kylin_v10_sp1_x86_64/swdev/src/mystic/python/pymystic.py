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

"""
PyMystic
========

PyMystic is a Python module that may be used to read AER files.  It is written entirely in Python 3
and has no additional requirements. PyMystic primarily facilitates Design of Experiments analysis and batch 
processing of AFSIM Monte-Carlo runs. It may also aid in Mystic capability prototyping and custom data extraction.

PyMystic uses the schema embedded in the read AER file.  This should make it cross-compatible with any
version of AFSIM, so long as the rules underlying the schema itself have not changed.  The contents of
a schema will change between versions, so a script written using PyMystic is not necessarily
cross-version-compatible.  The contents of the event-pipe messages are documented by the
:command:`event_pipe` page from the version of AFSIM from which the file was created.

Usage
-----

PyMystic defines a Reader class that can read Messages from an event-pipe file.  In general a pymystic
script is structured as follows:

.. code:: python

   import pymystic

   with pymystic.Reader('somefilename.aer') as reader # create a reader
      for msg in reader: # Loop over every message in the AER file
         # process messages here
         pass

Messages are represented as Python dictionaries, where each key is tied to a value. For some keys, the associated value may itself be a dictionary.

API Documentation
-----------------
"""

import io     # for file IO, and string/byte streams
import re     # for regular expression substitution while parsing schemas
import struct # for unpacking bytes into basic python types

class Schema:
   """The Schema class consumes a schema string and generates local definitions of the schema."""

   def _ProcessType(self, aCommand, aInput):
      """Process type takes a command (string aCommand), and a tokenized definition (list[string] aInput), it will return a schema definition of the data described."""
      if (aCommand == 'BasicType'):            # its a basic type
         return self._ProcessBasicType(aInput)
      elif (aCommand == 'List'):               # its a list type
         return self._ProcessListType(aInput)
      elif (aCommand == 'Enum'):               # its an enum type
         return self._ProcessEnumType(aInput)
      elif (aCommand == 'Struct'):             # its a struct type
         return self._ProcessStructType(aInput)
      elif (aCommand == 'Union'):              # it is a union type
         return self._ProcessUnionType(aInput)
      else:
         raise Exception('pymystic.Schema._ProcessType: unknown command ', aCommand)
         return


   def _ProcessBasicType(self, aInput):
      """Reads a tokenized definition (list[string] aInput) of a basic type from the schema and returns it in definition form."""
      retval = dict()
      retval['_command'] = 'basic'
      i = 0
      size = 0
      pytype = ''
      while i < len(aInput) - 1:                   # read all the tokens, these are a series of commands (ending with ':') and values
         inputcmd = aInput[i];
         inputcmd = inputcmd[0:inputcmd.find(':')] # strip the ':' off the command
         if (inputcmd == 'size'):
            size = int(aInput[i + 1])  # convert size to int, so the consumer doesn't have to
         elif (not inputcmd == 'id'):              # skip id, it is used as the key in the typelist
            retval[inputcmd] = aInput[i + 1]
         if (inputcmd == 'cpp_type'):              # cpp types' values are allowed spaces, so we need to look for the next command
            j = i + 1
            cpp_type = aInput[j]
            j += 1
            while (not aInput[j].endswith(':')):
               cpp_type += ' '
               cpp_type += aInput[j]
               j += 1
            retval['cpp_type'] = cpp_type.lstrip('\"').rstrip('\"') # some types ("std::string") show in quotes, strip these off
            if (cpp_type == 'double'):
               pytype = 'd'
            elif (cpp_type == 'float'):
               pytype = 'f'
            elif (cpp_type == 'signed char' or cpp_type == 'char'):
               pytype = 'b'
            elif (cpp_type == 'unsigned char'):
               pytype = 'B'
            elif (cpp_type == 'short'):
               pytype = 'h'
            elif (cpp_type == 'unsigned short'):
               pytype = 'H'
            elif (cpp_type == 'int'):
               pytype = 'i'
            elif (cpp_type == 'unsigned int'):
               pytype = 'I'
            elif (cpp_type == 'long long'):
               pytype = 'q'
            i = j
         # NOTE: we are ignoring the encoding value.  It is derivative of the cpp_type, since UtPack doesn't support anything
         #       but bit (for bit), varbytes (for std::string), or le (little-endian for everthing else).  No big-endian.
         else:
            i += 2
      if (pytype):
         retval['pytype'] = pytype, size
      else:
         retval['pytype'] = cpp_type, size

      return retval


   def _ProcessListType(self, aInput):
      """Reads a tokenized defintion (list[string] aInput) of a list type from the schema and returns it in definition form."""
      retval = dict()
      retval['_command'] = 'list'
      i = 0
      while i < len(aInput) - 1:                    # read all the tokens, these are a series of commands (ending with ':') and values
         inputcmd = aInput[i];
         inputcmd = inputcmd[0:inputcmd.find(':')]  # strip the ':' off the command
         if (not inputcmd == 'id'):                 # get everything but id
            retval[inputcmd] = aInput[i + 1]
         i += 2
      return retval


   def _ProcessEnumType(self, aInput):
      """Reads a tokenized defintion (list[string] aInput) of a enumeration type from the schema and returns it in definition form."""
      retval = dict()
      retval['_command'] = 'enum'
      i = 0
      vdict = dict()
      while i < len(aInput):
         if (aInput[i] == 'Value'):                 # read values to make the dictionary
            end = aInput[i+2:].index('}')           # find the end of the value list
            valueInput = aInput[i+2:i+2+end]        # and limit the string to that range
            i = i + 2 + end + 1                     # adjust i for when we continue reading from aInput
            j = 0
            n = ''                                  # the enum name
            v = ''                                  # the enum value
            while j < len(valueInput) - 1:
               if (valueInput[j] == 'id:'):
                  n = valueInput[j + 1]             # grab the name
               elif (valueInput[j] == 'value:'):
                  v = valueInput[j + 1]             # grab the value
               else:
                  raise Exception('pymystic.Schema._ProcessEnumType: Error reading enumeration value')
               j += 2
            if (n and v):
               vdict[int(v)] = n                    # if we got a name and a value, capture it
         elif (aInput[i] == 'id:'):
            i += 2                                  # we ignore the id of the value (it is the type's key)
         else:
            raise Exception('pymystic.Schema._ProcessEnumType: Error reading enumeration')
            return
      retval['data'] = vdict;
      return retval

   def _ProcessUnionType(self, aInput):
      """Reads a tokenized definition (list[string] aInput) of a union type from the schema and returns it in definition form."""
      retval = dict()
      retval['_command'] = 'union'
      i = 0
      fielddata = dict()
      typeid = ''
      bitsetsize = 0
      
      while i < len(aInput) - 1:
         inputcmd = aInput[i]
         if (not inputcmd == 'id:'):
            if (inputcmd == 'Field'):
               field = dict()
               end = aInput[i+2:].index('}')
               fieldInput = aInput[i+2:i+2+end]
               i = i + 2 + end + 1
               j = 0
               id = ''
               optionFound = 0
               while j < len(fieldInput) - 1:                  # Traverse the field's data
                  if (fieldInput[j] == 'id:'):
                     id = fieldInput[j + 1]                    # capture the field id
                  else:
                     fieldcmd = fieldInput[j];
                     fieldcmd = fieldcmd[0:fieldcmd.find(':')]
                     if (fieldcmd == 'bit' or fieldcmd == 'option'):
                        if (fieldcmd == 'option'):
                           optionFound = 1
                        bitsetsize += 1
                        field[fieldcmd] = int(fieldInput[j+1]) # we handle 'bit' and 'option' apart because we want the value cast to int for the consumer
                     else:
                        field[fieldcmd] = fieldInput[j+1]      # otherwise, just store the attribute
                  j += 2
               if not optionFound:                             # union fields must have an option bit
                  raise Exception('pymystic.Schema._ProcessUnionType: Field ' + id + ' in union ' + typeid + ' must be optional.')
               elif (id):
                  fielddata[id] = field   
         else:
            typeid = aInput[i + 1]
            i += 2  
      retval['fields'] = fielddata                             # create the field dictionary
      if (bitsetsize):
         retval['bitsetsize'] = int((bitsetsize + 7) / 8)      # calculate the bitmap size in bytes so the consumer doesn't have to
      return retval

   def _ProcessStructType(self, aInput):
      """Reads a tokenized defintion (list[string] aInput) of a struct type from the schema and returns it in definition form."""
      retval = dict()
      retval['_command'] = 'struct'
      i = 0
      subdata = dict()                              # this dictionary, if necessary will hold locally scoped type definitions
      fielddata = dict()                            # this dictionary will hold the structures fields
      bitsetsize = 0                                # this will keep track of how many bits are needed in the bitmap
      while i < len(aInput) - 1:                    # read all the tokens, these are a series of commands (ending with ':') and values
         inputcmd = aInput[i];
         if (not inputcmd == 'id:'):                # we ignore the id, since it is the key to the type
            if (inputcmd == 'Field'):               # capture fields
               field = dict()
               end = aInput[i+2:].index('}')
               fieldInput = aInput[i+2:i+2+end]     # find the subset of tokens representing this field
               i = i + 2 + end + 1                  # adjust i for the next time we need to read from aInput
               j = 0                                # j is our iterator for the field's tokens (fieldInput)
               id = ''
               while j < len(fieldInput) - 1:
                  if (fieldInput[j] == 'id:'):      # capture the field's id
                     id = fieldInput[j + 1]
                  else:
                     fieldcmd = fieldInput[j];
                     fieldcmd = fieldcmd[0:fieldcmd.find(':')]
                     if (fieldcmd == 'bit' or fieldcmd == 'option'):
                        bitsetsize += 1
                        field[fieldcmd] = int(fieldInput[j+1]) # we handle 'bit' and 'option' apart because we want the value cast to int for the consumer
                     else:
                        field[fieldcmd] = fieldInput[j+1]      # otherwise, just store the attribute
                  j += 2
               if (id):
                  fielddata[id] = field                        # put the field into the field dictionary
            elif (inputcmd == 'Struct' or inputcmd == 'List' or inputcmd == 'Enum' or inputcmd == 'Union' or inputcmd == 'BasicType'): # capture locally scoped types
               i += 2
               start = i
               depth = 1
               while (depth > 0):
                  if (aInput[i] == '{'):
                     depth += 1
                  elif (aInput[i] == '}'):
                     depth -= 1
                  elif ((aInput[i] == 'id:') and (depth == 1)):
                     id = aInput[i + 1]
                  i += 1
               end = i - 1                                     # find the scoped tokens and process the scoped type
               subdata[id] = self._ProcessType(inputcmd, aInput[start:end]);
            else:                                              # capture anything else
               inputcmd = inputcmd[0:inputcmd.find(':')]
               retval[inputcmd] = aInput[i + 1]
               i += 2
         else:
            i += 2
      if (subdata):
         retval['subdata'] = subdata                   # only create the subdata dictionary if necessary, this has performance benefit
      retval['fields'] = fielddata                     # create the field dictionary
      if (bitsetsize):
         retval['bitsetsize'] = int((bitsetsize + 7) / 8) # calculate the bitmap size in bytes so the consumer doesn't have to
      return retval


   def __init__(self, aSchemaStr):
      """
      The schema constructor takes a schema string and builds up a definition representation of the schema, and
      a map from message id to structure name

      :param aSchemaStr: The schema definition string
      :type aSchemaStr: str
      """

      self.mTypeList = {}        # Definition of data types
      self.mMessages = {}        # Map from message id to message name
      self.mMessageNameSpace = 0 # this value is bit-or'ed with message_id to isolate messages in different domains
      self.mDoubleTime = 0       # this value indicates if 'time' type is float (0) or double (1) defaulting to float

      reader = io.StringIO(aSchemaStr + ' ')   # space needed at end to prevent data loss
	                                           # here we clean up the string

      ########################################## Clean and tokenize the string
      l = reader.readline()
      text = ''
      while (l):
         l = l[0:l.find('//')]                 # strip off comments
         if (len(l) > 0):
            text += l
         l = reader.readline()
      text = text.replace('{', ' { ')          # add spaces around scope changes to ensure proper tokenization
      text = text.replace('}', ' } ')
      text = re.sub('([^:]):([^:])', '\\1: \\2', text) # add space after a singular ':'
      tokened = text.split()                   # tokenize

      ########################################## Parse the token list
      i = 0
      while (i < len(tokened)):                # start reading
         id = ''
         command = tokened[i]                  # the first token inside of the scope tells us what kind of data we are dealing with
         i += 1
         if (tokened[i] == '{'):               # if we enter a scope, find the range of it and peek at the id
            start = i + 1
            depth = 1
            i += 1
            while (depth > 0):
               if (tokened[i] == '{'):
                  depth += 1
               elif (tokened[i] == '}'):
                  depth -= 1
               elif ((tokened[i] == 'id:') and (depth == 1)):
                  id = tokened[i + 1]           # Capture the id
               i += 1
            end = i - 1
         if (command == 'Settings'):
            t = start
            while (t < end):
               if (tokened[t] == 'message_id_or:'):
                  if (t + 1 < end):
                     self.mMessageNameSpace = int(tokened[t + 1]) # extract the 'namespace' value
               elif (tokened[t] == 'double_time:'):
                  if (t + 1 < end):
                     self.mDoubleTime = int(tokened[t + 1]) # indicates if time is represented as a double
               t = t + 1
         if (id):                               # settings, which have no id, will not be read.  They contain nothing needed here
            data = self._ProcessType(command, tokened[start:end]) # read the data type from the subset of the data
            self.mTypeList[id] = data
            if ('message' in data.keys()):                     # if it is a message, catch the lookup id
               if (not data['message'] == 'virtual'):
                  self.mMessages[int(data['message'])] = id    # if it is a message with a messageId, map it


class Reader:
   """
   The Reader class will open an AER file, generate the schema from the header, and then allow the reading of messages.

   This class may be used as an iterator, in which case it will yeild the messages in the loaded file in order.
   """

   def __init__(self, filename, debug=False):
      """
      This method will load a file of the given *filename*, which may be processed with other Reader methods.
      Files must have the *.aer* extension.
      The *filename* may include a relative or absolute path to the file.

      You may use this class as a context manager as follows:

      .. code:: python

         with Reader("filename.aer") as reader:
            # Use the reader here
            pass

      :param filename: The name and extension of the input file. 
      :param debug: Print debug strings while parsing the AER file.
      :type debug: bool, optional
      """
      self._fileobj = open(filename, 'rb') # open the file
      magic = self._fileobj.read(11)        # check the magic-number
      if (magic != b'\x00\x00WSF_PIPE\n'):
         raise Exception('pymystic.Reader.load: This is not an AER file: ', filename)
      else:
         schemastr = ''
         a = self._fileobj.read(1)
         while (a != b'\x00'):              # pull the schema text (till the next /0)
            schemastr += a.decode("utf-8")
            a = self._fileobj.read(1)
         self.mSchema = Schema(schemastr)   # construct the schema

      self._scope = []            # A stack of the currently used scopes
      self.mDebug = debug         # Debug flag, set to true to print read messages and read fields


   # Context manager interface   
   def __enter__(self):
      return self
      
   def __exit__(self, exc_type, exc_value, traceback):
      return self._fileobj.__exit__()

   # Iterator interface
   def __iter__(self):
      return self
   def __next__(self):
      msg = self.read()
      if not msg:
         raise StopIteration
      return msg

   
   def scan(self):
      """
      This method reads only the header data of the next message in the file, allowing the user to filter messages by type.
      In order to parse the data into a dictionary, pass the output to :py:meth:`pymystic.Reader.process`.

      Returns: 
         A tuple containing:
         
         * The message id (int)
         * The message type (string)
         * The message (bytes)
      """
      msgsizeraw = self._fileobj.read(4)          # message start with a size
      if (not msgsizeraw):
         return                                   # couldn't read anymore, return null
      msgsize = struct.unpack('i', msgsizeraw)[0] # convert the messagesize to an int
      msgbytes = io.BytesIO(self._fileobj.read(msgsize)) # read the next message's memory block
      if (len(msgbytes.getvalue()) < msgsize):    # if we couldn't read enough, inform the user that the file ends with an incomplete message
         print('file ended with an incomplete message!') # not an exception, there may be valid reasons to read this sort of thing
      else:
         id = struct.unpack('i', msgbytes.read(4))[0] # read the id (unsigned int) of the message
         try:
            msgtype = self.mSchema.mMessages[id]      # look up the message name
         except LookupError:
            raise Exception('Unknown message: id=', retval['id'], ' size=', msgsize)
         return id, msgtype, msgbytes


   def scan_iter(self):
      """
      Yields an iterator over the messages in the AER file. In order to parse the data into a dictionary, 
      pass the output to :py:meth:`pymystic.Reader.process`.

      Yields: 
         a tuple containing:

         * The message id (int)
         * The message type (string)
         * The message (bytes)
      """
      msg = self.scan()
      while msg:
         yield msg
         msg = self.scan()
         

   def process(self, msgdata):
      """
      This method converts MessageData into a Message.

      :param msgdata: A MessageData tuple output by either :py:meth:`pymystic.Reader.scan` or :py:meth:`pymystic.Reader.scan_iter`.

      :return: A dictionary, representing the message.
      """
      retval = {}
      retval['id'] = msgdata[0]
      retval['msgtype'] = msgdata[1]            # store the name
      if (self.mDebug):                         # if the debug flag is set, print the name
         print('DEBUG: reading message type ', msgdata[1])
      try:
         if ('subdata' in self.mSchema.mTypeList[msgdata[1]].keys()): # if a type doesn't define sub data, it is irrelevant to looking up types
            self._scope.append(self.mSchema.mTypeList[msgdata[1]])  # push the message definition onto the scope stack
         self._ProcessStruct(self.mSchema.mTypeList[msgdata[1]], msgdata[2], retval) # construct the message
         if ('subdata' in self.mSchema.mTypeList[msgdata[1]].keys()):
            self._scope.pop()                                    # restore the scope stack
      except LookupError:
         raise Exception('pymystic.reader.read: Could not find message structure in schema : ', msgtype)
      return retval


   def read(self):
      """
      This method will read and return the next message in the loaded file.

      :return: A dictionary, representing next message in the currently loaded file, or None if EOF has been reached.
      """
      d = self.scan()
      if d:
         return self.process(d)
         
   def close(self):
      """
      This method will close the loaded replay file. This allows users to explicitly release the file 
      handle in the event of an error.
      """
      self._fileobj.close()


   def _ScopeLookup(self, aScopeStruct, aNameList):
      """
      If an type address makes use of a path, this allows us to navigate to it
      definition   aScopeStruct - The current scope 
      list[string] aNameList    - The path of the data looked for
      """
      
      if (not aNameList):
         return aScopeStruct # if everything is popped off of the path, we found our target
      else:
         if ('subdata' in aScopeStruct.keys()): # otherwise dive deeper into subdata
            if (aNameList[0] in aScopeStruct['subdata'].keys()):   #if it is there
               return self._ScopeLookup(aScopeStruct['subdata'][aNameList[0]], aNameList[1:]) # pop it off the path, and dive deeper
      raise Exception('pymystic.Reader._ScopeLookup: Could not find scoped type')


   def _LookupType(self, aTypeName):
      """
      Returns a schema type (defintion).
      string aTypeName - The name of the definition looked for
      """
      typePath = aTypeName.split('.')                       # split up into a path, schema allows pathed structures
      for scope in self._scope[::-1]:                       # traverse in reverse through scopes (most specific scopes are on the end of the list)
         if (typePath[0] in scope['subdata'].keys()):    # and the type is defined their
            return self._ScopeLookup(scope['subdata'][typePath[0]], typePath[1:]) # follow the given path from here (pop off the first element)
      try:
         return self._ScopeLookup(self.mSchema.mTypeList[typePath[0]], typePath[1:]) # dive deeper into the type path as necessary
      except LookupError:
         raise Exception('pymystic.Reader._LookupType: could not find type ', aTypeName, ' in scopes', self._scope)


   def _ReadType(self, aTypeDict, aMsgBytes, aBitNum, aBitMap):
      """
      Interprets a value based on the schema.
      defintition aTypeDict - The schema definition we want to use to interpret the data
      io.Bytes    aMsgBytes - The memory block we are reading from
      int         aBitNum   - If this is a bit, this will tell us where in the bitmap to find it
      int         aBitMap   - The local bitmap for storing bits and options
      """
      cmd = aTypeDict['_command']

      ############################################################## Read a basic type
      if (cmd == 'basic'):
         pytype = aTypeDict['pytype']
         if (pytype[0] == 'std::string'):                           # its a string
            bytesToRead = struct.unpack('I', aMsgBytes.read(4))[0] # read the length
            return aMsgBytes.read(bytesToRead).decode("utf-8")     # read the string
         elif (pytype[0] == 'bit'):                                 # its a bit, read it from the bitmap
            return bool(1 & (aBitMap >> aBitNum))
         else:
            try:
               return struct.unpack(pytype[0], aMsgBytes.read(pytype[1]))[0]
            except:
               raise Exception('pymystic.Reader._ReadType: unknown basic type of pytype: ', pytype)

      ############################################################## Read a structure
      elif (cmd == 'struct'):
         s = {}                                                    # create an empty object
         if ('subdata' in aTypeDict.keys()):                       # if a type doesn't define sub data, it is irrelevant to looking up types
            self._scope.append(aTypeDict)                          # add the definition to the scope list
         self._ProcessStruct(aTypeDict, aMsgBytes, s)              # read the structure
         if ('subdata' in aTypeDict.keys()):
            self._scope.pop()                                      # restore the previous scope list
         return s

      ############################################################## Read a list
      elif (cmd == 'list'):
         entryToRead = struct.unpack('I', aMsgBytes.read(4))[0]    # read the number of entries
         typeToRead = aTypeDict['type']                            # read the type name
         i = 0
         if (not ((typeToRead == 'time') or (typeToRead == 'index'))):
            t = self._LookupType(typeToRead)                       # look up the type (as long as it isn't special case time or index
         list = []
         while (i < entryToRead):                                  # for every entry
            if (typeToRead == 'time'):
               if (self.mSchema.mDoubleTime == 1):
                  list.append(struct.unpack('d', aMsgBytes.read(8))[0]) 
               else:
                  list.append(struct.unpack('f', aMsgBytes.read(4))[0]) 
            elif (typeToRead == 'index'):
               list.append(struct.unpack('I', aMsgBytes.read(4))[0])
            else:
               v = self._ReadType(t, aMsgBytes, 0, aBitMap)        # read it
               if (v):
                  list.append(v)                                   # add it to the list
            i += 1
         return list

      ############################################################## Read an enumeration
      elif (cmd == 'enum'):
         return struct.unpack('B', aMsgBytes.read(1))[0] # NOTE: should we cast this to a string?  Proper python enumerations?  This is a good first step.
      elif (cmd == 'union'):
         fieldType = struct.unpack('B', aMsgBytes.read(1))[0]
         if fieldType != 255:
            for fieldName in aTypeDict['fields']:
               field = aTypeDict['fields'][fieldName]
               if field['option'] == fieldType:
                  lookupType = self._LookupType(field['type'])
                  retval = self._ReadType(lookupType, aMsgBytes, aBitNum, aBitMap)
                  return retval
         else:
            return
         raise Exception('pymystic.Reader._ReadType: union requested field out of range')
      else:
         raise Exception('pymystic.Reader._ReadType: unsupported type can not be read :', cmd, ' field = ', field)


   def _ProcessStruct(self, aStructDef, aMsgBytes, aOutput):
      """
      Read a structure from the file
      definition aStructDef - a definition from the schema describing the data
      io.BytesIO aMsgBytes  - The memory block we are reading from
      definition aOutput    - The definition we want to write to
      """

      ###################################################### Handle the bitmap, for bit and option storage
      bitmap = 0
      if ('bitsetsize' in aStructDef.keys()):              # if there are things that are packed in a bitmap
         bitmap = aMsgBytes.read(aStructDef['bitsetsize']) # read an appropriate sized bitmap
         bitmap = int.from_bytes(bitmap[::-1], 'big')      # reverse it and convert to int for easy lookup
      if ('base' in aStructDef.keys()):                    # if it has a base 'class'
         base = aStructDef['base']                         # get the name of the base
         try:
            self._ProcessStruct(self.mSchema.mTypeList[base], aMsgBytes, aOutput) # process the base class
         except LookupError:
            raise Exception('pymystic.Reader._ProcessStruct: Could not find base type : ', base)

      ###################################################### Handle data fields
      if ('fields' in aStructDef.keys()):                  # if there are fields
         for field in aStructDef['fields']:                # for every field
            fieldStruct = aStructDef['fields'][field]      # get the field's definition
            if (self.mDebug):
               print('DEBUG:   reading field:', field)     # DEBUG print
            optioned = True                                # by default everything will write unless otherwise indicated
            if ('option' in fieldStruct.keys()):           # if the field is optional, read the option state
               validtext = field + 'Valid'
               bitnum = fieldStruct['option']              # get the bit
               optioned = bool(1 & (bitmap >> bitnum))     # pull the desired bit from the bitmap
               aOutput[validtext] = optioned               # store the option value
            if (optioned):                                 # if it was not 'optioned' out
               try:
                  typeName = fieldStruct['type']
                  if (typeName == 'time'):                 # if its a time (float or double)
                     if (self.mSchema.mDoubleTime == 1):
                        time = struct.unpack('d', aMsgBytes.read(8))[0]
                        aOutput[field] = time
                     else:
                        time = struct.unpack('f', aMsgBytes.read(4))[0]
                        aOutput[field] = time
                  elif (typeName == 'index'):              # if its an index (int)
                     index = struct.unpack('I', aMsgBytes.read(4))[0]
                     aOutput[field] = index
                  else:                                    # otherwise it is defined in the schema
                     t = self._LookupType(typeName)        # look up the type
                     bitnum = 0
                     if ('bit' in fieldStruct.keys()):
                        bitnum = fieldStruct['bit']        # if it has a defined bit, ReadType will need to know about that
                     v = self._ReadType(t, aMsgBytes, bitnum, bitmap) # convert bytes and schema definitions into data definitions
                     aOutput[field] = v                    # set the field value
               except LookupError:
                  raise Exception('pymystic.Reader._ProcessStruct: could not find type in field')
