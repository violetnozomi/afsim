// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SOSM_CoreBase.hpp"

#include "UtInput.hpp"

// =================================================================================================
SOSM_CoreBase::SOSM_CoreBase()
   : SOSM_SpectralObject()
   , mModificationTime(-1.0)
   , mTypeName()
   , mCachePrefix()
{
}

// =================================================================================================
SOSM_CoreBase::SOSM_CoreBase(const SOSM_CoreBase& aSrc)
   : SOSM_SpectralObject(aSrc)
   , mModificationTime(aSrc.mModificationTime)
   , mTypeName(aSrc.mTypeName)
   , mCachePrefix(aSrc.mCachePrefix)
{
}

// =================================================================================================
// virtual
SOSM_CoreBase::~SOSM_CoreBase() {}

// =================================================================================================
// virtual
bool SOSM_CoreBase::Initialize()
{
   return true;
}

// =================================================================================================
//! Return the name of the cache prefix for this object.
std::string SOSM_CoreBase::GetCachePrefix() const
{
   if (!mCachePrefix.empty())
   {
      return mCachePrefix;
   }
   return mTypeName;
}

// =================================================================================================
// virtual
void SOSM_CoreBase::InputComplete() {}

// =================================================================================================
// virtual
bool SOSM_CoreBase::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "cache_prefix")
   {
      aInput.ReadValue(mCachePrefix);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
void SOSM_CoreBase::UpdateModificationTime(double aModificationTime)
{
   if (aModificationTime > mModificationTime)
   {
      mModificationTime = aModificationTime;
   }
}
