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

// GenOBuf - This class is used to get and put data into and out of a buffer

#ifndef GEN_BUF_IO_HPP
#define GEN_BUF_IO_HPP

#include "genio_export.h"

#include "GenBufXferI.hpp"
#include "GenBufXferO.hpp"

class GENIO_EXPORT GenBufIO : public GenBufXferI, public GenBufXferO
{
public:
   // Constructor will prepare class for a buffer of type "aBufType"
   GenBufIO(BufType aBufType, unsigned long aSize, unsigned int aAllowGrowthOnPuts = 0);

   // Constructor will use the user supplied buffer "aUserBuffer"
   GenBufIO(BufType aBufType, unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes = 0);

   ~GenBufIO() override;

   // Change buffer currently being used and set current byte offset to zero.
   void SetBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes = 0);

   // Get a pointer to the buffer
   unsigned char* GetBuffer();
};

inline void GenBufIO::SetBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes)
{
   SetLocalBuffer(aUserBuffer, aSizeInBytes, aNumValidBytes);
}

inline unsigned char* GenBufIO::GetBuffer()
{
   return GetLocalBuffer();
}

#endif
