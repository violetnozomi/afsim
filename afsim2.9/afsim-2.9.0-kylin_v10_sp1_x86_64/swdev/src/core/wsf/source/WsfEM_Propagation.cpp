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

#include "WsfEM_Propagation.hpp"

#include "UtInput.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEM_XmtrRcvr.hpp"

// =================================================================================================
//! Constructor
WsfEM_Propagation::WsfEM_Propagation()
   : WsfObject()
   , mDebugEnabled(false)
{
}

// =================================================================================================
//! Copy constructor
WsfEM_Propagation::WsfEM_Propagation(const WsfEM_Propagation& aSrc)
   : WsfObject(aSrc)
   , mDebugEnabled(aSrc.mDebugEnabled)
{
}


// =================================================================================================
//! Initialize the propagation object.
//! @param aXmtrRcvrPtr The transmitter/receiver to which the propagation object is attached.
// virtual
bool WsfEM_Propagation::Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr)
{
   return true;
}

// =================================================================================================
//! Initialize the propagation object.
//! @param aXmtrPtr The transmitter to which the propagation object is attached.
//! @note This method is deprecated and is provided simply so existing models will continue to
//! function. All new models should use the form that accepts a WsfEM_XmtrRcvr.
// virtual
bool WsfEM_Propagation::Initialize(WsfEM_Xmtr* aXmtrPtr)
{
   return Initialize(static_cast<WsfEM_XmtrRcvr*>(aXmtrPtr));
}

// =================================================================================================
// virtual
bool WsfEM_Propagation::ProcessInput(UtInput& aInput)
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
