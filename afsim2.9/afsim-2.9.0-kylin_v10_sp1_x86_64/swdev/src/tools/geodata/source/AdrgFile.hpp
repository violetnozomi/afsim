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

#ifndef ADRGFILE_HPP
#define ADRGFILE_HPP

#include "geodata_export.h"

#include <cstdio>

class GEODATA_EXPORT AdrgFile
{
public:
   AdrgFile(FILE* aFilePtr);
   ~AdrgFile();

   // Exception classes

   class InvalidFormat
   {
   };
   class IOError
   {
   };

   // The 'Get' methods read a value from the file and convert it to the
   // host machine format if necessary.  They throw and IOError if they
   // encounter an error.

   void Get(void* aValue, int aLength);
   void Seek(long int aOffset, int aWhence);

   // Convert a string of 'aSize' characters in '+/-DDDMMSS.HH' format to
   // a double.  The string does not have to be null-terminated.
   double StrToDeg(const char* aStr, unsigned int aSize);

   // The following is used only for debugging...
   const char* ToStr(void* aValue, unsigned int aLength);

private:
   FILE* mFilePtr;
};

inline void AdrgFile::Get(void* aValue, int aLength)
{
   if (::fread(aValue, aLength, 1, mFilePtr) != 1)
   {
      throw IOError();
   }
}
inline void AdrgFile::Seek(long int aOffset, int aWhence)
{
   if (::fseek(mFilePtr, aOffset, aWhence) != 0)
   {
      throw IOError();
   }
}

#endif
