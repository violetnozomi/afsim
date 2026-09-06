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

// GenNullO - A derived class of GenO that does output to a null device.

#include "GenNullO.hpp"

#include "GenBufOManaged.hpp"

GenNullO::GenNullO()
   : GenO("GenNullO")
{
   // Make a buffer with no space and take the default to not
   // grow as weird as that seems
   mBuf = new GenBufOManaged(GenBuf::Null, 0);

   SetGenBufXferO(mBuf);
}

GenNullO::~GenNullO()
{
   delete mBuf;
}

int GenNullO::Send()
{
   ClearOutputErrorStatus();

   // No Data written
   return -1;
}

// ****************************************************************************


// ****************************************************************************


// Virtual
int GenNullO::HandlePutError(int /* aStatus */, unsigned long /* aVariableSize */)
{
   // This function will be called on every put.  Since this is
   // a null device and there is place to put the data, we won't
   // tell the user about any errors or try to fix anything.

   return 0;
}
