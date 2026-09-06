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
#ifndef GENPIPE_HPP
#define GENPIPE_HPP

#include "genio_export.h"

#include <cstddef> // for size_t

//! Represents one end of an anonymous pipe.
//! Pipes created are non-blocking
class GENIO_EXPORT GenPipe
{
public:
   GenPipe()
      : mPipeHandle(0)
   {
   }

   static bool Create(GenPipe& aReadPipe, GenPipe& aWritePipe);

   size_t GetHandle() const { return mPipeHandle; }

   int Write(const void* aBuffer, unsigned int aBytes);

   int Read(void* aBuffer, unsigned int aBufferBytes);

   void Close();

private:
   GenPipe(size_t aPipeHandle);

   size_t mPipeHandle;
};

#endif
