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

// GenNullIO - A derived class of GenIO that does I/O to a null device.

#include "GenNullIO.hpp"

#include "GenBufIO.hpp"

GenNullIO::GenNullIO()
   : GenIO("GenNullIO", "GenNullIO")
{
   // Make a buffer with no space and take the default to not
   // grow as weird as that seems
   mBuf = new GenBufIO(GenBuf::Null, 0);

   SetGenBufXferI(mBuf);
}

GenNullIO::~GenNullIO()
{
   delete mBuf;
}

int GenNullIO::Receive(int /* aWaitTimeInMicroSec */)
{
   ClearInputErrorStatus();

   // No Data read
   return -1;
}

int GenNullIO::Send()
{
   ClearOutputErrorStatus();

   // No Data written
   return -1;
}

// ****************************************************************************


// ****************************************************************************

// Virtual
int GenNullIO::HandleGetError(int /* aStatus */, unsigned long /* aVariableSize */)
{
   // This function will be called on every get.  Since this is
   // a null device and there is place to get the data, we won't
   // tell the user about any errors or try to fix anything.

   return 0;
}

// Virtual
int GenNullIO::HandlePutError(int /* aStatus */, unsigned long /* aVariableSize */)
{
   // This function will be called on every put.  Since this is
   // a null device and there is place to put the data, we won't
   // tell the user about any errors or try to fix anything.

   return 0;
}
