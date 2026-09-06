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
#ifndef GENPIPEI_HPP
#define GENPIPEI_HPP

#include "genio_export.h"

#include "GenI.hpp"
#include "GenPipe.hpp"
class GenBufIO;

class GENIO_EXPORT GenPipeI : public GenI
{
public:
   // Move any unread data to the front of the buffer and then refill the buffer.
   // Return value: >= 0, number of bytes in the input buffer.
   //                < 0, error (such as EOF)
   // NOTE: It is not necessary to call this function.  The Get()
   //       methods will refill the buffer as necessary.
   int Receive(int aWaitTimeInMicroSec) override;

   GenPipeI(GenBuf::BufType aBufType, GenPipe& aPipe);

   ~GenPipeI() override;

private:
   GenPipe   mPipe;
   GenBufIO* mBuffer;
};

#endif
