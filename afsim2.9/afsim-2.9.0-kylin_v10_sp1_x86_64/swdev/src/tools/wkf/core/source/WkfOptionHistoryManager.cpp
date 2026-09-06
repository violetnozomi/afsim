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

#include "WkfOptionHistoryManager.hpp"

#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"

wkf::OptionHistoryManager::OptionHistoryManager(const QString& aPluginName, QObject* aParent)
   : QObject(aParent)
   , mPluginName(aPluginName)
   , mMaxId(0)
{
   mCallbacks.Add(Observer::LoadSettings.Connect(&OptionHistoryManager::Load, this));
   mCallbacks.Add(Observer::SaveSettings.Connect(&OptionHistoryManager::Save, this));
   mCallbacks.Add(Observer::PlatformsAdded.Connect(&OptionHistoryManager::ApplyOptions, this));
   connect(&wkfEnv, &Environment::ClearPlatformSettings, this, &wkf::OptionHistoryManager::Clear);
}

int wkf::OptionHistoryManager::AddOption(const QString& aName, int aParentType)
{
   const auto& it = mStringToId.find(aName);
   if (it != mStringToId.end())
   {
      return it->second;
   }
   else
   {
      int val            = mMaxId;
      mIdToString[val]   = aName;
      mStringToId[aName] = val;
      SetParentOption(val, aParentType);
      mMaxId++;
      return val;
   }
}

void wkf::OptionHistoryManager::DefaultOptionTrue(const QString& aName)
{
   mDefaultOptions.emplace(aName);
}

QString wkf::OptionHistoryManager::OptionName(int aOptionId)
{
   const auto& it = mIdToString.find(aOptionId);
   if (it != mIdToString.end())
   {
      return it->second;
   }
   return "";
}
