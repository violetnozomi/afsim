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

// GenBufIManaged - This class is used to get data from a input buffer that this
// class owns.

#ifndef GEN_BUF_I_MANAGED_HPP
#define GEN_BUF_I_MANAGED_HPP

#include "genio_export.h"

#include "GenBufXferI.hpp"

class GENIO_EXPORT GenBufIManaged : public GenBufXferI
{
public:
   // Constructor will prepare class for a buffer of type "aBufType"
   GenBufIManaged(BufType aBufType, unsigned long aSize, bool aAllowGrowth = false);

   ~GenBufIManaged() override;

   // Get a access to the buffer.  User is responsible to not overflow the
   // buffer
   unsigned char* GetBuffer();
};

inline unsigned char* GenBufIManaged::GetBuffer()
{
   return GetLocalBuffer();
}

#endif
