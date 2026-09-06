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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfPProxyKey.hpp"

#include "UtCast.hpp"

WsfPProxyKey::WsfPProxyKey()
   : mIndex(ut::npos)
   , mKeyType(KeyType::cSTRING)
{
}

WsfPProxyKey::WsfPProxyKey(size_t i)
   : mIndex(i)
   , mKeyType(KeyType::cINDEX)
{
}

WsfPProxyKey::WsfPProxyKey(const std::string& s)
   : mMapKey(s)
   , mIndex(ut::npos)
   , mKeyType(KeyType::cSTRING)
{
}

std::string WsfPProxyKey::ToString() const
{
   if (IsString())
   {
      return mMapKey;
   }
   return std::to_string(mIndex);
}

bool WsfPProxyKey::IsString() const
{
   return mKeyType == KeyType::cSTRING;
}

bool WsfPProxyKey::IsIndex() const
{
   return mKeyType == KeyType::cINDEX;
}

bool WsfPProxyKey::operator<(const WsfPProxyKey& aRhs) const
{
   if (IsString() && aRhs.IsIndex())
   {
      return true;
   }
   if (IsIndex() && aRhs.IsString())
   {
      return false;
   }
   if (mIndex < aRhs.mIndex)
   {
      return true;
   }
   if (mIndex > aRhs.mIndex)
   {
      return false;
   }
   return mMapKey < aRhs.mMapKey;
}

bool WsfPProxyKey::operator==(const WsfPProxyKey& aRhs) const
{
   if (mIndex != aRhs.mIndex)
   {
      return false;
   }
   return (mKeyType != KeyType::cSTRING) || (mMapKey == aRhs.mMapKey);
}
