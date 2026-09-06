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

#ifndef WSFNAMED_HPP
#define WSFNAMED_HPP

#include "wsf_export.h"

#include "WsfStringId.hpp"

//! A base object for objects that have a 'name'.
//!
//! This object provides the methods for maintain the 'name' of an object.
//! Objects that possess both a 'name' and a 'type' should use WsfObject.
class WSF_EXPORT WsfNamed
{
public:
   WsfNamed();
   WsfNamed(WsfStringId aName);
   WsfNamed(const WsfNamed&) = default;
   WsfNamed& operator=(const WsfNamed&) = default;
   virtual ~WsfNamed()                  = default;

   const std::string& GetName() const;

   //! Get the string ID of the name of the object.
   //! @returns an integer containing the string ID of the name of the object.
   WsfStringId GetNameId() const { return mName; }

   //! Set the name of the object.
   //! @param aName a const reference to a WsfStringId containing the name of the object.
   void SetName(WsfStringId aName) { mName = aName; }

private:
   WsfStringId mName;
};

#endif
