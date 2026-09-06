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

#include "ext/WsfExtEntityId.hpp"

#include <iostream>

std::ostream& operator<<(std::ostream& aOut, const WsfExtEntityId& aId)
{
   aOut << aId.GetSite() << '.' << aId.GetApplication() << '.' << aId.GetEntity();
   return aOut;
}

bool WsfExtEntityId::operator<(const WsfExtEntityId& aRhs) const
{
   if (mSite < aRhs.mSite)
   {
      return true;
   }
   if (mSite > aRhs.mSite)
   {
      return false;
   }
   if (mApplication < aRhs.mApplication)
   {
      return true;
   }
   if (mApplication > aRhs.mApplication)
   {
      return false;
   }
   if (mEntity < aRhs.mEntity)
   {
      return true;
   }
   return false;
}

bool WsfExtEntityId::operator==(const WsfExtEntityId& aRhs) const
{
   return mSite == aRhs.mSite && mApplication == aRhs.mApplication && mEntity == aRhs.mEntity;
}
