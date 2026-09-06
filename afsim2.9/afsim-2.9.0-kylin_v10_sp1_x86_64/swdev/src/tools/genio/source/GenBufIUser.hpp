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

// GenBufIUser - This class is used to get data from a user input buffer

#ifndef GEN_BUF_I_USER_HPP
#define GEN_BUF_I_USER_HPP

#include "genio_export.h"

#include "GenBufXferI.hpp"

class GENIO_EXPORT GenBufIUser : public GenBufXferI
{
public:
   // Constructor will use the provided "aUserBuffer" of type "aBufType".
   GenBufIUser(BufType aBufType, const unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes);

   ~GenBufIUser() override;

   // Change buffer currently being used and set current byte offset to zero.
   void SetBuffer(const unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes);

   // Get a pointer to the buffer, but this should not be needed,
   // since the user gave us the pointer.
   const unsigned char* GetBuffer();
};

inline void GenBufIUser::SetBuffer(const unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes)
{
   // Since we are in charge of the base class we know this buffer
   // will not be changed, so we can cast away the const
   SetLocalBuffer(const_cast<unsigned char*>(aUserBuffer), aSizeInBytes, aNumValidBytes);
}

inline const unsigned char* GenBufIUser::GetBuffer()
{
   return GetLocalBuffer();
}

#endif
