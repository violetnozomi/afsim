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

// GenNullI - A derived class of GenI that inputs from a null device.

#ifndef GEN_NULL_I_HPP
#define GEN_NULL_I_HPP

#include "genio_export.h"

#include "GenI.hpp"

class GenBufIManaged;

class GENIO_EXPORT GenNullI : public GenI
{
public:
   // Open the file.
   //   BufType  : is the numeric representation.
   GenNullI();

   ~GenNullI() override;

   // Always returns a -1 to say no data received
   int Receive(int aWaitTimeInMicroSec) override;

protected:
   int HandleGetError(int aStatus, unsigned long aVariableSize) override;

private:
   GenBufIManaged* mBuf;
};

#endif
