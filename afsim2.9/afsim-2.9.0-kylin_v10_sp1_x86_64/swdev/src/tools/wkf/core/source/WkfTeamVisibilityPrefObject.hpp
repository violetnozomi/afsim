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
#ifndef WKFTEAMVISIBILITYPREFOBJECT_HPP
#define WKFTEAMVISIBILITYPREFOBJECT_HPP

#include "wkf_export.h"

#include <map>
#include <unordered_map>

#include <QColor>

#include "UtColor.hpp"
#include "WkfPlatform.hpp"
#include "WkfPrefObject.hpp"

namespace wkf
{
struct TeamVisibilityPrefData
{
   TeamVisibilityPrefData();

   //! Assigns aOther to *this, but respects team visibility lock.
   void SafeAssign(const TeamVisibilityPrefData& aOther);

   static constexpr const char* cDEFAULT_TEAM = "default";
   struct TeamInfo
   {
      bool   visible{true};
      QColor color{128, 128, 128, 255};
      bool   operator==(const TeamInfo& rhs) const { return (visible == rhs.visible && color == rhs.color); }
   };

   using TeamInfoMap = std::map<QString, TeamInfo>;
   TeamInfoMap mTeamInfo;
   bool        mLockTeamVisibility{false};
   bool        mReadInProgress{false};
};

class WKF_EXPORT TeamVisibilityPrefObject : public PrefObjectT<TeamVisibilityPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "TeamOptions";

   TeamVisibilityPrefObject(QObject* parent = nullptr);
   ~TeamVisibilityPrefObject() override;

   bool                    IsTeamVisible(const QString& aTeamName) const;
   QColor                  GetTeamColor(const QString& aTeamName) const;
   UtColor                 GetTeamUtColor(const std::string& aTeamName) const;
   void                    SetTeamVisibilityLocked(bool aLocked) { mCurrentPrefs.mLockTeamVisibility = aLocked; }
   std::map<QString, bool> GetTeamVisibilityMap() const;
   void                    SetTeamVisible(const QString& aTeamName, const bool aVisible);
   bool                    AddTeam(const QString& aSide, const TeamVisibilityPrefData::TeamInfo& aTeamInfo);
   void                    RemoveTeam(const QString& aSide);

signals:
   void TeamColorChanged();
   void TeamVisibilityChanged();

private:
   void                   Apply() override;
   QColor                 LookUpDefaultTeamColor(const std::string& aTeamColor) const;
   void                   UpdateColorDialog();
   TeamVisibilityPrefData ReadSettings(QSettings& aSettings) const override;
   void                   SaveSettingsP(QSettings& aSettings) const override;
   void                   SetPreferenceDataP(const TeamVisibilityPrefData& aPrefData) override;

   bool   mTeamColorChanged{false};
   bool   mTeamVisibilityChanged{false};
   size_t mVisibilityFilterId{0};
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::TeamVisibilityPrefData)
#endif
