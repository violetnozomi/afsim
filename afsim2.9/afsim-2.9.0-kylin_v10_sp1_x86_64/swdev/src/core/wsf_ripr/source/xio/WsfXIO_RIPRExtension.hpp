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
#ifndef WSFXIO_RIPREXTENSION_HPP
#define WSFXIO_RIPREXTENSION_HPP

#include <map>

class UtInput;
class WsfXIO_Interface;
class WsfXIO_JobBoardService;
class WsfXIO_RIPRManagerService;

//! WsfXIO_RIPRExtension manages the RIPR XIO environment.  Behavior-wise, this class
//! imitates WsfXIO_SimStandard (instantiating services) and WsfXIO_Interface
//! (registering XIO packets).
class WsfXIO_RIPRExtension
{
protected:
   struct ConnectionData;

public:
   WsfXIO_RIPRExtension(WsfXIO_Interface* aInterfacePtr);
   ~WsfXIO_RIPRExtension();

   WsfXIO_Interface* GetInterface() const;

   WsfXIO_JobBoardService* JobBoardService() const { return mJobBoardServicePtr; }

   bool Initialize();

protected:
   WsfXIO_Interface*          mInterfacePtr;
   WsfXIO_JobBoardService*    mJobBoardServicePtr;
   WsfXIO_RIPRManagerService* mRIPRManagerServicePtr;
};

#endif
