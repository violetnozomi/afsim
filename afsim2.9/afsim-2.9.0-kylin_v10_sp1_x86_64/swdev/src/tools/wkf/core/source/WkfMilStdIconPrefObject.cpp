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

#include "WkfMilStdIconPrefObject.hpp"

namespace
{
const QString cUSE_SYMBOLOGY   = "use_symbology";
const QString cVELOCITY_VECTOR = "show_vector";
const QString cORIENT          = "orient_heading";
const QString cFRIEND          = "friend_teams";
const QString cHOSTILE         = "hostile_teams";
const QString cNEUTRAL         = "neutral_teams";
} // namespace

wkf::MilStdIconPrefObject::MilStdIconPrefObject(QObject* parent)
   : wkf::PrefObjectT<MilStdIconPrefData>(parent, cNAME)
{
}

void wkf::MilStdIconPrefObject::Apply()
{
   if (mUseSymbologyChanged)
   {
      emit UseSymbologyChanged();
      mUseSymbologyChanged = false;
   }
   if (mShowVelocityVectorChanged)
   {
      emit ShowVelocityVectorChanged();
      mShowVelocityVectorChanged = false;
   }
   if (mTeamMappingsChanged)
   {
      emit TeamMappingsChanged();
      mTeamMappingsChanged = false;
   }
}

wkf::MilStdIconPrefData wkf::MilStdIconPrefObject::ReadSettings(QSettings& aSettings) const
{
   auto ReadMappings = [&](const QString& aArrayName, std::set<QString>& aSet)
   {
      aSet.clear();
      int size = aSettings.beginReadArray(aArrayName);
      for (int i = 0; i < size; ++i)
      {
         aSettings.setArrayIndex(i);
         aSet.insert(aSettings.value("team").toString());
      }
      aSettings.endArray();
      return (size > 0);
   };

   MilStdIconPrefData pData;
   pData.mUseSymbology       = aSettings.value(cUSE_SYMBOLOGY, mDefaultPrefs.mUseSymbology).toBool();
   pData.mShowVelocityVector = aSettings.value(cVELOCITY_VECTOR, mDefaultPrefs.mShowVelocityVector).toBool();
   pData.mOrientToHeading    = aSettings.value(cORIENT, mDefaultPrefs.mOrientToHeading).toBool();
   bool friendDefined        = ReadMappings(cFRIEND, pData.mFriendTeams);
   bool hostileDefined       = ReadMappings(cHOSTILE, pData.mHostileTeams);
   bool neutralDefined       = ReadMappings(cNEUTRAL, pData.mNeutralTeams);
   // If no teams are assigned, just force the default
   if (!(friendDefined || hostileDefined || neutralDefined))
   {
      pData.mFriendTeams  = mDefaultPrefs.mFriendTeams;
      pData.mHostileTeams = mDefaultPrefs.mHostileTeams;
   }
   return pData;
}

void wkf::MilStdIconPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   auto SaveTeamMappings = [&](const QString& aArrayName, const std::set<QString>& aTeamList)
   {
      int i = 0;
      aSettings.beginWriteArray(aArrayName);
      for (const auto& it : aTeamList)
      {
         aSettings.setArrayIndex(i++);
         aSettings.setValue("team", it);
      }
      aSettings.endArray();
   };

   aSettings.setValue(cUSE_SYMBOLOGY, mCurrentPrefs.mUseSymbology);
   aSettings.setValue(cVELOCITY_VECTOR, mCurrentPrefs.mShowVelocityVector);
   aSettings.setValue(cORIENT, mCurrentPrefs.mOrientToHeading);
   SaveTeamMappings(cFRIEND, mCurrentPrefs.mFriendTeams);
   SaveTeamMappings(cHOSTILE, mCurrentPrefs.mHostileTeams);
   SaveTeamMappings(cNEUTRAL, mCurrentPrefs.mNeutralTeams);
}

void wkf::MilStdIconPrefObject::SetPreferenceDataP(const MilStdIconPrefData& aPrefData)
{
   mUseSymbologyChanged       = aPrefData.mUseSymbology != mCurrentPrefs.mUseSymbology;
   mShowVelocityVectorChanged = aPrefData.mShowVelocityVector != mCurrentPrefs.mShowVelocityVector;
   mTeamMappingsChanged       = aPrefData.mFriendTeams != mCurrentPrefs.mFriendTeams ||
                          aPrefData.mHostileTeams != mCurrentPrefs.mHostileTeams ||
                          aPrefData.mNeutralTeams != mCurrentPrefs.mNeutralTeams;
   BasePrefObject::SetPreferenceDataP(aPrefData);
}
