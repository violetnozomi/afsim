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

#include "WsfEM_Clutter.hpp"

#include "UtInput.hpp"

// =================================================================================================
//! Constructor
WsfEM_Clutter::WsfEM_Clutter()
   : WsfObject()
   , mDebugEnabled(false)
{
}

// =================================================================================================
//! Copy constructor
WsfEM_Clutter::WsfEM_Clutter(const WsfEM_Clutter& aSrc)
   : WsfObject(aSrc)
   , mDebugEnabled(aSrc.mDebugEnabled)
{
}

WsfEM_Clutter& WsfEM_Clutter::operator=(const WsfEM_Clutter& aRhs)
{
   if (this != &aRhs)
   {
      WsfObject::operator=(aRhs);
      mDebugEnabled      = aRhs.mDebugEnabled;
   }
   return *this;
}

// =================================================================================================
//! Initialize the clutter object.
//! @param aRcvrPtr The receiver to which the clutter object is attached.
// virtual
bool WsfEM_Clutter::Initialize(WsfEM_Rcvr* aRcvrPtr)
{
   return true;
}

// =================================================================================================
// virtual
bool WsfEM_Clutter::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else
   {
      myCommand = WsfObject::ProcessInput(aInput);
   }
   return myCommand;
}
