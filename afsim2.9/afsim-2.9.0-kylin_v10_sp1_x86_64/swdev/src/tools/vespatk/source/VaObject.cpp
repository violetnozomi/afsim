// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaObject.hpp"

#include <algorithm>

#include "VaEnvironment.hpp"
#include "VaSystemDefines.hpp"

namespace vespa
{
//! This is the constructor for the VaObject class.
VaObject::VaObject()
   : VaUniqueId()
   , mName("")
   , mType("")
   , mTypeList()
   , mData()
{
}

//! Copy constructor (for Clone())
// protected
VaObject::VaObject(const VaObject& aSrc, bool aCreateNewUniqueId /* = true*/)
   : VaUniqueId(aSrc, aCreateNewUniqueId)
   , mName(aSrc.mName)
   , mType(aSrc.mType)
   , mTypeList(aSrc.mTypeList)
   , mData(aSrc.mData)
{
}

UtVariant VaObject::GetData(const std::string& aTag) const
{
   if (DataExists(aTag))
   {
      return mData.at(aTag);
   }
   return UtVariant();
}

//! Equals operator.
VaObject& VaObject::operator=(const VaObject& aRhs)
{
   if (this != &aRhs)
   {
      mName     = aRhs.mName;
      mType     = aRhs.mType;
      mTypeList = aRhs.mTypeList;
      mData     = aRhs.mData;
   }
   return *this;
}

} // end namespace vespa.
