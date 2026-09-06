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

#include "WsfNamed.hpp"

// =================================================================================================
//! Default constructor.
//! Using this constructor, the user must later call either the "SetName" methods.
WsfNamed::WsfNamed()
   : mName(nullptr)
{
}

// =================================================================================================
//! Construct with a specified name.
//! Use this constructor to automatically set the name.
//! @param aName a std::string with the desired object name.
WsfNamed::WsfNamed(WsfStringId aName)
   : mName(aName)
{
}

// =================================================================================================
//! Get the name of the object.
//! @returns a std::string containing the name of the object.
const std::string& WsfNamed::GetName() const
{
   return mName.GetString();
}
