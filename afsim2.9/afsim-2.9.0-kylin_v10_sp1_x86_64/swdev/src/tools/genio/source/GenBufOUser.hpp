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

// GenBufOUser - This class is used to put data into a user output buffer

#ifndef GEN_BUF_O_USER_HPP
#define GEN_BUF_O_USER_HPP

#include "genio_export.h"

#include "GenBufXferO.hpp"

class GENIO_EXPORT GenBufOUser : public GenBufXferO
{
public:
   // Constructor will use the provided "aUserBuffer" of type "aBufType".
   GenBufOUser(BufType aBufType, unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes);

   ~GenBufOUser() override;

   // Change buffer currently being used.
   void SetBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes);

   // Get a pointer to the buffer, but this should not be needed,
   // since the user gave us the pointer.
   unsigned char* GetBuffer();
};

inline void GenBufOUser::SetBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes)
{
   SetLocalBuffer(aUserBuffer, aSizeInBytes, aNumValidBytes);
}

inline unsigned char* GenBufOUser::GetBuffer()
{
   return GetLocalBuffer();
}

#endif
