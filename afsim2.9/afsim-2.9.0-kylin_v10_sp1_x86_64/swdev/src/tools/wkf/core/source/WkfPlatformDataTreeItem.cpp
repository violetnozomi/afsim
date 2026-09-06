// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfPlatformDataTreeItem.hpp"

#include "WkfDefines.hpp"

void wkf::PlatformDataTreeItem::SetHidden(bool aState, HideCause aCause /*= PROGRAMMATIC*/)
{
   if (aState)
   {
      mHideCause |= aCause;
   }
   else
   {
      mHideCause &= ~aCause;
   }
   setHidden(mHideCause != 0);
}

void wkf::PlatformDataTreeItem::SetUnhideable(bool aValue /*= true*/)
{
   setData(0, wkf::UNHIDEABLE, aValue);
}

bool wkf::PlatformDataTreeItem::PreferenceHideable() const
{
   return !data(0, wkf::UNHIDEABLE).toBool();
}

void wkf::PlatformDataTreeItem::SetDefaultHidden(bool aValue /*= true*/)
{
   setData(0, wkf::DEFAULT_HIDDEN, aValue);
}

bool wkf::PlatformDataTreeItem::DefaultHidden() const
{
   return data(0, wkf::DEFAULT_HIDDEN).toBool();
}

void wkf::PlatformDataTreeItem::SetPlottable(bool aValue /*= true*/)
{
   mPlottable = aValue;
}

bool wkf::PlatformDataTreeItem::Plottable() const
{
   return mPlottable;
}
