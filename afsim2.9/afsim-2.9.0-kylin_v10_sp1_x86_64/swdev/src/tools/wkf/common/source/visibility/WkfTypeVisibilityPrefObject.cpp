// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfTypeVisibilityPrefObject.hpp"

#include <QMetaType>

#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"

namespace
{
const QString cTYPEFILTERLIST = "platformTypeFilterList";
const QString cTYPEFILTERITEM = "filterListItem";
} // namespace

wkf::TypeVisibilityPrefObject::TypeVisibilityPrefObject(QObject* parent)
   : wkf::PrefObjectT<TypeVisibilityPrefData>(parent, cNAME)
{
   mVisibilityFilterId = wkfEnv.RegisterPlatformVisibilityFilter(
      [this](const wkf::Platform& aPlatform) -> bool
      {
         for (const auto& type : aPlatform.GetTypeList())
         {
            if (!IsVisible(type))
            {
               return false;
            }
         }
         return true;
      });
}

wkf::TypeVisibilityPrefObject::~TypeVisibilityPrefObject()
{
   wkfEnv.UnregisterPlatformVisibilityFilter(mVisibilityFilterId);
}

bool wkf::TypeVisibilityPrefObject::IsVisible(const std::string& aTypeName) const
{
   return (!mCurrentPrefs.mHiddenTypes.contains(QString::fromStdString(aTypeName)));
}

void wkf::TypeVisibilityPrefObject::ExternalAddPreference(const QString& aTypeName, bool aVisible)
{
   if (aVisible)
   {
      mCurrentPrefs.mHiddenTypes.remove(aTypeName);
   }
   else
   {
      mCurrentPrefs.mHiddenTypes.insert(aTypeName);
   }
   mPrefsChanged = true;
   BasePrefObject::SetPreferenceDataP(mCurrentPrefs);
   Apply();
   SaveSettings(wkfEnv.GetUserSettingsFile(), wkfEnv.GetSharedSettingsFile());
}

void wkf::TypeVisibilityPrefObject::Apply()
{
   if (mPrefsChanged)
   {
      mPrefsChanged = false;
      emit TypeVisibilityChanged();
      // We registered a visibility filter with the environment, need to notify subscribers to the environment
      //  that they need to re-evaluate the visibility
      emit wkfEnv.PlatformVisibilityChanged();
   }
}

wkf::TypeVisibilityPrefData wkf::TypeVisibilityPrefObject::ReadSettings(QSettings& aSettings) const
{
   // note: The next line of code cannot be used, because the associated line in SaveSettingsP causes a crash on exit
   // mCurrentPrefs.mHiddenTypes = aSettings.value(cTYPEFILTERLIST,
   // QVariant::fromValue(mDefaultPrefs.mHiddenTypes)).value<TypeSet>();;

   TypeVisibilityPrefData pData;
   aSettings.beginGroup(cTYPEFILTERLIST);
   int size = aSettings.beginReadArray(cTYPEFILTERITEM);
   for (int i = 0; i < size; ++i)
   {
      aSettings.setArrayIndex(i);
      pData.mHiddenTypes.insert(aSettings.value("type").toString());
   }
   aSettings.endArray();
   aSettings.endGroup();
   return pData;
}

void wkf::TypeVisibilityPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   // note: Why does the following code cause a crash on exit? (the metatype stream operators would normally be in the
   // constructor) qRegisterMetaTypeStreamOperators<TypeSet>("TypeSet"); aSettings.setValue(cTYPEFILTERLIST,
   // QVariant::fromValue(mCurrentPrefs.mHiddenTypes));

   aSettings.beginGroup(cTYPEFILTERLIST);
   aSettings.remove(cTYPEFILTERITEM);
   aSettings.beginWriteArray(cTYPEFILTERITEM, (int)mCurrentPrefs.mHiddenTypes.size());
   int i = 0;
   for (const auto& type : mCurrentPrefs.mHiddenTypes)
   {
      aSettings.setArrayIndex(i++);
      aSettings.setValue("type", type);
   }
   aSettings.endArray();
   aSettings.endGroup();
}

void wkf::TypeVisibilityPrefObject::SetPreferenceDataP(const TypeVisibilityPrefData& aPrefData)
{
   // This does not determine if only Visibility changed or only Team Color changed
   if (!(mCurrentPrefs.mHiddenTypes == aPrefData.mHiddenTypes))
   {
      mPrefsChanged = true;
   }
   BasePrefObject::SetPreferenceDataP(aPrefData);
}
