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

// GenBufOManaged - This class is used to put data into a output buffer that this
//                  class owns.

#ifndef GEN_BUF_O_MANAGED_HPP
#define GEN_BUF_O_MANAGED_HPP

#include "genio_export.h"

#include "GenBufXferO.hpp"

class GENIO_EXPORT GenBufOManaged : public GenBufXferO
{
public:
   // Constructor will prepare class for a buffer of type "aBufType"
   GenBufOManaged(BufType aBufType, unsigned long aSize, unsigned int aAllowGrowthOnPuts = 0);

   ~GenBufOManaged() override;

   // Change buffer currently being used.
   // This function is not typically called and should be used with great care.
   void ChangeBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes);

   // Get a access to the buffer.
   const unsigned char* GetBuffer();
};

inline void GenBufOManaged::ChangeBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes)
{
   SetLocalBuffer(aUserBuffer, aSizeInBytes, aNumValidBytes);
}


inline const unsigned char* GenBufOManaged::GetBuffer()
{
   return GetLocalBuffer();
}

#endif
