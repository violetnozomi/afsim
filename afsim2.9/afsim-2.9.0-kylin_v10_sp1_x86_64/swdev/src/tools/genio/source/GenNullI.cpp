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

// GenNullI - A derived class of GenI that does input from a null device.

#include "GenNullI.hpp"

#include "GenBufIManaged.hpp"

GenNullI::GenNullI()
   : GenI("GenNullI")
{
   mBuf = new GenBufIManaged(GenBuf::Null, 0);

   SetGenBufXferI(mBuf);
}

GenNullI::~GenNullI()
{
   delete mBuf;
}

int GenNullI::Receive(int /* aWaitTimeInMicroSec */)
{
   ClearInputErrorStatus();

   // No Data read
   return -1;
}


// ****************************************************************************


// ****************************************************************************

// Virtual
int GenNullI::HandleGetError(int /* aStatus */, unsigned long /* aVariableSize */)
{
   // This function will be called on every get.  Since this is
   // a null device and there is place to get the data, we won't
   // tell the user about any errors or try to fix anything.

   return 0;
}
