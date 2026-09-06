// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvMemoryPrefObject.hpp"

rv::MemoryPrefObject::MemoryPrefObject(QObject* parent)
   : wkf::PrefObjectT<MemoryPrefData>(parent, cNAME)
{
}

void rv::MemoryPrefObject::SetPagesInCache(unsigned int aPages)
{
   if (mCurrentPrefs.mPagesInCache != aPages)
   {
      mCurrentPrefs.mPagesInCache = aPages;
   }
}

rv::MemoryPrefData rv::MemoryPrefObject::ReadSettings(QSettings& aSettings) const
{
   MemoryPrefData pData;
   pData.mPagesInCache = aSettings.value("pagesInCache", mDefaultPrefs.mPagesInCache).toUInt();
   return pData;
}

void rv::MemoryPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("pagesInCache", mCurrentPrefs.mPagesInCache);
}
