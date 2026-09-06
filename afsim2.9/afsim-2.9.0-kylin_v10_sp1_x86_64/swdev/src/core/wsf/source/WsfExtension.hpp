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

#ifndef WSFEXTENSION_HPP
#define WSFEXTENSION_HPP

#include "wsf_export.h"

#include <string>

// Forward declarations for exceptions.
class WsfDuplicationExtension;
class WsfUnknownExtension;

//! The base class for application, scenario and simulation extensions.
class WSF_EXPORT WsfExtension
{
public:
   virtual ~WsfExtension() = default;

   //! Returns the name of this extension.
   //! Only valid after WsfApplication::RegisterExtension is called
   const std::string& GetExtensionName() const { return mExtensionName; }

protected:
   void InitializeExtensionName(const std::string& aName);

private:
   std::string mExtensionName;
};

#endif
