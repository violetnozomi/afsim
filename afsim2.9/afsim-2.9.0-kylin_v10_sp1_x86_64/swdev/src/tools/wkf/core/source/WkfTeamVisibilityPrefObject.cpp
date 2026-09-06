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
#include "WkfTeamVisibilityPrefObject.hpp"

#include <set>

#include <QColorDialog>

#include "UtColor.hpp"
#include "UtQt.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPlatform.hpp"

namespace
{
const std::string                                  cNOT_FOUND             = "gray";
const std::unordered_map<std::string, std::string> cDefaultTeamToColorMap = {{"united_states", "blue"},
                                                                             {"us", "blue"},
                                                                             {"europe", "green"},
                                                                             {"china", "red"},
                                                                             {"russia", "red"},
                                                                             {"iran", "red"},
                                                                             {"iraq", "red"},
                                                                             {"skorea", "blue"},
                                                                             {"nkorea", "red"},
                                                                             {"neutral", "green"},
                                                                             {"red", "red"},
                                                                             {"blue", "blue"}};

const std::string& GetTeamMapping(const std::string& aTeamName)
{
   auto teamIt = cDefaultTeamToColorMap.find(aTeamName);
   return (teamIt != cDefaultTeamToColorMap.end()) ? teamIt->second : cNOT_FOUND;
}

bool TeamExistsInDefaultMap(const std::string& aTeamName)
{
   return cDefaultTeamToColorMap.find(aTeamName) != cDefaultTeamToColorMap.end() || UtColor::ColorExists(aTeamName);
}

// The colors themselves should also be treated as "teams".
QColor LookupTeamColor(const std::string& aTeamName)
{
   if (UtColor::ColorExists(aTeamName))
   {
      return UtQtColor(UtColor::GetColorMapping(aTeamName));
   }
   else
   {
      return UtQtColor(UtColor::GetColorMapping(GetTeamMapping(aTeamName)));
   }
}
} // namespace

wkf::TeamVisibilityPrefData::TeamVisibilityPrefData()
{
   mTeamInfo.emplace(QString{cDEFAULT_TEAM}, TeamInfo{});

   for (auto team : cDefaultTeamToColorMap)
   {
      mTeamInfo[team.first.c_str()].color = LookupTeamColor(team.second);
   }
}

void wkf::TeamVisibilityPrefData::SafeAssign(const TeamVisibilityPrefData& aOther)
{
   // If Team visibility is not locked, or if we are initializing all of the settings,
   //  Simply set the current mTeamInfo to aOther.mTeamInfo
   if (!mLockTeamVisibility || aOther.mReadInProgress)
   {
      mTeamInfo = aOther.mTeamInfo;
   }
   else
   {
      // Otherwise, only copy team color to prevent the visibility flag from changing.
      for (const auto& team : aOther.mTeamInfo)
      {
         mTeamInfo[team.first].color = team.second.color;
      }
   }
}

wkf::TeamVisibilityPrefObject::TeamVisibilityPrefObject(QObject* parent)
   : PrefObjectT<TeamVisibilityPrefData>(parent, cNAME)
{
   mVisibilityFilterId =
      wkfEnv.RegisterPlatformVisibilityFilter([this](const wkf::Platform& aPlatform) -> bool
                                              { return IsTeamVisible(QString::fromStdString(aPlatform.GetSide())); });
}

wkf::TeamVisibilityPrefObject::~TeamVisibilityPrefObject()
{
   wkfEnv.UnregisterPlatformVisibilityFilter(mVisibilityFilterId);
}

bool wkf::TeamVisibilityPrefObject::IsTeamVisible(const QString& aTeamName) const
{
   // Return the team's visibility
   auto it = mCurrentPrefs.mTeamInfo.find(aTeamName);
   if (it == mCurrentPrefs.mTeamInfo.end())
   {
      // If team is not in map, return the value of the default team
      it = mCurrentPrefs.mTeamInfo.find(PrefDataType::cDEFAULT_TEAM);
   }

   if (it != mCurrentPrefs.mTeamInfo.end())
   {
      return it->second.visible;
   }
   // if nothing in the map is applicable, then return true;
   return true;
}

QColor wkf::TeamVisibilityPrefObject::GetTeamColor(const QString& aTeamName) const
{
   // Return the team's visibility
   auto it = mCurrentPrefs.mTeamInfo.find(aTeamName);
   if (it != mCurrentPrefs.mTeamInfo.end())
   {
      return it->second.color;
   }
   else
   {
      // Team is not available in preferences
      return LookUpDefaultTeamColor(aTeamName.toStdString());
   }
}

UtColor wkf::TeamVisibilityPrefObject::GetTeamUtColor(const std::string& aTeamName) const
{
   return UtQtColor(GetTeamColor(QString::fromStdString(aTeamName)));
}

std::map<QString, bool> wkf::TeamVisibilityPrefObject::GetTeamVisibilityMap() const
{
   std::map<QString, bool> retVal;
   for (auto& i : mCurrentPrefs.mTeamInfo)
   {
      retVal[i.first] = i.second.visible;
   }
   return retVal;
}

void wkf::TeamVisibilityPrefObject::SetTeamVisible(const QString& aTeamName, const bool aVisible)
{
   TeamVisibilityPrefData newData = mCurrentPrefs;
   auto                   team    = newData.mTeamInfo.find(aTeamName);
   if (team != newData.mTeamInfo.end())
   {
      team->second.visible = aVisible;
      SetPreferenceDataP(newData);
      Apply();
   }
}

bool wkf::TeamVisibilityPrefObject::AddTeam(const QString& aSide, const TeamVisibilityPrefData::TeamInfo& aTeamInfo)
{
   bool retval{false};
   if (mCurrentPrefs.mTeamInfo.find(aSide) == mCurrentPrefs.mTeamInfo.end())
   {
      TeamVisibilityPrefData           newData     = mCurrentPrefs;
      TeamVisibilityPrefData::TeamInfo newTeamInfo = TeamVisibilityPrefData::TeamInfo(aTeamInfo);
      newTeamInfo.color                            = LookUpDefaultTeamColor(aSide.toStdString());
      newData.mTeamInfo.emplace(aSide, newTeamInfo);
      SetPreferenceDataP(newData);
      Apply();
      retval = true;
   }
   return retval;
}

void wkf::TeamVisibilityPrefObject::RemoveTeam(const QString& aSide)
{
   TeamVisibilityPrefData newData = mCurrentPrefs;
   if (!TeamExistsInDefaultMap(aSide.toStdString()))
   {
      newData.mTeamInfo.erase(aSide);
   }
   SetPreferenceDataP(newData);
   Apply();
}

QColor wkf::TeamVisibilityPrefObject::LookUpDefaultTeamColor(const std::string& aTeamName) const
{
   if (TeamExistsInDefaultMap(aTeamName))
   {
      return LookupTeamColor(aTeamName);
   }
   // Use the current preference for the default team
   else if (mCurrentPrefs.mTeamInfo.count(PrefDataType::cDEFAULT_TEAM) > 0)
   {
      return mCurrentPrefs.mTeamInfo.at(PrefDataType::cDEFAULT_TEAM).color;
   }
   // Lastly use the default preference for the default team
   else if (mDefaultPrefs.mTeamInfo.count(PrefDataType::cDEFAULT_TEAM) > 0)
   {
      return mDefaultPrefs.mTeamInfo.at(PrefDataType::cDEFAULT_TEAM).color;
   }
   else
   {
      return QColor(); // This should never happen
   }
}

wkf::TeamVisibilityPrefData wkf::TeamVisibilityPrefObject::ReadSettings(QSettings& aSettings) const
{
   TeamVisibilityPrefData pData;
   // Note: we do not store mLockTeamVisibility, as this should not be saved between instances of the application
   pData.mLockTeamVisibility = mDefaultPrefs.mLockTeamVisibility;

   // First copy all the defaults into the preferences.
   pData.mTeamInfo = mDefaultPrefs.mTeamInfo;
   // Then read the INI file for modifications.
   // Note: this will not catch if you remove a default (Are we concerned about this?)
   //       We may actually desire that the default teams come back after being deleted.
   int size = aSettings.beginReadArray("teams");
   for (int i = 0; i < size; ++i)
   {
      TeamVisibilityPrefData::TeamInfo info;

      aSettings.setArrayIndex(i);
      QString teamName = aSettings.value("name", "").toString();
      info.visible     = aSettings.value("show", true).toBool();

      int r      = aSettings.value("colorR", 0).toInt();
      int g      = aSettings.value("colorG", 0).toInt();
      int b      = aSettings.value("colorB", 0).toInt();
      int a      = aSettings.value("colorA", 255).toInt();
      info.color = QColor(r, g, b, a);

      pData.mTeamInfo[teamName] = info;
   }
   aSettings.endArray();
   pData.mReadInProgress = true;
   return pData;
}

void wkf::TeamVisibilityPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   // Note: we do not store mLockTeamVisibility, as this should not be saved between instances of the application

   int i = 0;
   aSettings.beginWriteArray("teams");
   for (const auto& iter : mCurrentPrefs.mTeamInfo)
   {
      aSettings.setArrayIndex(i++);
      aSettings.setValue("name", iter.first);
      aSettings.setValue("show", iter.second.visible);
      aSettings.setValue("colorR", iter.second.color.red());
      aSettings.setValue("colorG", iter.second.color.green());
      aSettings.setValue("colorB", iter.second.color.blue());
      aSettings.setValue("colorA", iter.second.color.alpha());
   }
   aSettings.endArray();
}

void wkf::TeamVisibilityPrefObject::SetPreferenceDataP(const TeamVisibilityPrefData& aPrefData)
{
   // This does not determine if only Visibility changed or only Team Color changed
   if (!(mCurrentPrefs.mTeamInfo == aPrefData.mTeamInfo))
   {
      mTeamVisibilityChanged = true;
      mTeamColorChanged      = true;
      // SafeAssign respects team visibility lock.
      mCurrentPrefs.SafeAssign(aPrefData);

      UpdateColorDialog();
   }
}

void wkf::TeamVisibilityPrefObject::Apply()
{
   if (mTeamVisibilityChanged)
   {
      // We registered a visibility filter with the environment, need to notify subscribers to the environment
      //  that they need to re-evaluate the visibility
      emit wkfEnv.PlatformVisibilityChanged();
      emit TeamVisibilityChanged();
      mTeamVisibilityChanged = false;
   }
   if (mTeamColorChanged)
   {
      emit TeamColorChanged();
      mTeamColorChanged = false;
   }
}

void wkf::TeamVisibilityPrefObject::UpdateColorDialog()
{
   std::set<QColor, utqt::QColorComp> uniqueColors;
   int                                index = 0;

   auto AddColor = [&](const QColor& aColor)
   {
      if (uniqueColors.emplace(aColor).second)
      {
         QColorDialog::setStandardColor(index, aColor);
         ++index;
      }
   };

   // explicitly add the mil-std colors to give some order to the start of the standard colors
   // ref: MIL-STD-2525D
   //      10 JUNE 2014
   //      p. 42
   // HOSTILE, SUSPECT, JOKER, FAKER - RED
   AddColor(QColor(200, 0, 0));
   AddColor(QColor(255, 48, 49));
   AddColor(QColor(255, 128, 128));
   // FRIEND, ASSUMED FRIEND - BLUE
   AddColor(QColor(0, 107, 140));
   AddColor(QColor(0, 168, 220));
   AddColor(QColor(128, 224, 255));
   // NEUTRAL - GREEN
   AddColor(QColor(0, 160, 0));
   AddColor(QColor(0, 226, 0));
   AddColor(QColor(170, 255, 170));
   // UNKNOWN, PENDING - YELLOW
   AddColor(QColor(225, 220, 0));
   AddColor(QColor(255, 255, 0));
   AddColor(QColor(255, 255, 128));
   // CIVILIAN - PURPLE
   AddColor(QColor(80, 0, 80));
   AddColor(QColor(128, 0, 128));
   AddColor(QColor(255, 161, 255));
   // end ref.

   for (const auto& teamColor : mCurrentPrefs.mTeamInfo)
   {
      AddColor(teamColor.second.color);
   }

   for (const auto& color : UtColor::GetColorMap())
   {
      AddColor(UtQtColor(color.second));
   }
}
