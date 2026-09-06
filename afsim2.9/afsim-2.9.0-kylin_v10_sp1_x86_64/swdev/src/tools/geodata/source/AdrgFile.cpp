// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "AdrgFile.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>

// ----------------------------------------------------------------------------
//
// Notes on the ADRG file format vs. the documentation.
//
// The files are described as being in 'ISO 8211' format, which describes the
// file in terms of 'records', 'fields' and 'sub-fields'.  Unfortunately the
// actual data we are interested in is encapsulated by a bunch of other stuff
// that is dictated by the ISO 8211 standard.  This standard provides the
// definition of a 'self-describing' file.  I will now describe the format
// as much as needed to get our task done:
//
// Each logical record is composed of three separate pieces:
//
//   a) Leader
//   b) Directory
//   c) Field area
//
// Leader
//   (LEN) Length (5 numeric characters)
//     Length of the logical record in bytes.  The length includes the length
//     of this item.  Adding this value to the file position of the start
//     of this record gives the file position of the next logical record.
//   Ident (7 characters)
//   (BAD) Base Data Address (5 numeric characters)
//     Offset from the start of the record the start of the 'Field Area'.
//   Unknown (3 characters (blanks))
//   Entry Map (4 characters)
//     This is used to decipher directory entries.
//     (FLS) Size of field length   (1 numeric character)
//     (FPS) Size of field position (1 numeric character)
//     Unused                 (1 numeric character ('0'))
//     (FTS) Size of field tag      (1 numeric character)
//
//   The total size of the leader is 24 bytes.
//
// Directory
//   The directory indicates which fields are present in the record.  The
//   number of directory entries is:
//
//   (BAD - 24 - 1) / (FTS + FLS + FPS)
//
//   The '- 1' in the numerator is for the presence of a '0x1E' delimiter.
//   For ADRG the denominator will be 10.
//
//   The format of each directory entry is:
//     Field tag      ('FTS' bytes) (For ADRG, FTS = 3)
//       The tag for the i'th field.
//     Field length   ('FLS' bytes) (For ADRG, FLS = 3)
//       The length of the i'th field.  This includes the 1 extra byte for
//       the field terminator character (0x1E).
//     Field position ('FPS' bytes) (For ADRG, FPS = 4)
//       Offset from the start of the field area to the first position of the
//       field.
//
// Fields
//   Each field that is present in the directory is present here in the same
//   order.
//
// Within the file itself, the first record is a Data Description Record (DDR)
// describes the types and format of data that may be present within the file.
// We are not interested in this with ADRG as we know what is in the file.
//
// ----------------------------------------------------------------------------

AdrgFile::AdrgFile(FILE* aFilePtr)
   : mFilePtr(aFilePtr)
{
}

AdrgFile::~AdrgFile() {}

// Convert a string of 'aSize' characters in '+/-DDDMMSS.HH' format to
// a double.  The string does not have to be null-terminated.
double AdrgFile::StrToDeg(const char* aStr, unsigned int aSize)
{
   char tempStr[12];

   assert(aSize < sizeof(tempStr));
   ::memcpy(tempStr, aStr, aSize);
   tempStr[aSize] = '\0';
   double value   = ::atof(tempStr);
   double sign    = 1;
   if (value < 0.0)
   {
      value = -value;
      sign  = -1.0;
   }
   int dddmmsshh = static_cast<int>(value * 100.0);
   int ddd       = dddmmsshh / 1000000;
   int mm        = (dddmmsshh / 10000) % 100;
   int sshh      = dddmmsshh % 10000;
   value         = sign * (ddd + (mm / 60.0) + (sshh / 360000.0));
   return value;
}

// The following is used only for debugging...
const char* AdrgFile::ToStr(void* aValue, unsigned int aLength)
{
   static char tempBuf[512];

   assert(aLength < (sizeof(tempBuf) - 1));
   ::memcpy(tempBuf, aValue, aLength);
   tempBuf[aLength] = '\0';
   return tempBuf;
}
