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
#include "xio/WsfXIO_RIPRExtension.hpp"

#include <cassert>

#include "UtInput.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_JobBoardPacketRegistry.hpp"
#include "xio/WsfXIO_JobBoardService.hpp"

WsfXIO_RIPRExtension::WsfXIO_RIPRExtension(WsfXIO_Interface* aInterfacePtr)
   : mInterfacePtr(aInterfacePtr)
   , mJobBoardServicePtr(0)
   , mRIPRManagerServicePtr(0)
{
   // Initialize the packets recognized by RIPR XIO.
   WsfXIO_JobBoardPacketRegistry::Initialize((PakProcessor&)*aInterfacePtr);

   // Initialize the process that listens for remote application RIPR requests
   mJobBoardServicePtr    = new WsfXIO_JobBoardService(aInterfacePtr);
   mRIPRManagerServicePtr = new WsfXIO_RIPRManagerService(aInterfacePtr);
}

WsfXIO_RIPRExtension::~WsfXIO_RIPRExtension()
{
   delete mJobBoardServicePtr;
   delete mRIPRManagerServicePtr;
}

bool WsfXIO_RIPRExtension::Initialize()
{
   return true;
}

WsfXIO_Interface* WsfXIO_RIPRExtension::GetInterface() const
{
   return mInterfacePtr;
}
