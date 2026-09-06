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
#ifndef WKFMilStdPREFOBJECT_HPP
#define WKFMilStdPREFOBJECT_HPP

#include "wkf_export.h"

#include <set>

#include "WkfPrefObject.hpp"

namespace wkf
{
struct MilStdIconPrefData
{
   bool mUseSymbology{false};
   bool mShowVelocityVector{false};
   bool mOrientToHeading{false};

   std::set<QString> mFriendTeams  = {"blue"};
   std::set<QString> mHostileTeams = {"red"};
   std::set<QString> mNeutralTeams;
};

class WKF_EXPORT MilStdIconPrefObject : public wkf::PrefObjectT<MilStdIconPrefData>
{
   Q_OBJECT
public:
   static constexpr const char* cNAME = "MilStdIconPrefObject";

   MilStdIconPrefObject(QObject* parent = nullptr);
   ~MilStdIconPrefObject() override = default;

   bool IsSharedPreference() const final { return true; }

   bool GetUseSymbology() const { return mCurrentPrefs.mUseSymbology; }
   bool GetShowVelocityVector() const
   {
      return mCurrentPrefs.mShowVelocityVector;
   } // Note: the "velocity" vector should actually be exclusively showing heading
   std::set<QString> GetFriendTeams() const { return mCurrentPrefs.mFriendTeams; }
   std::set<QString> GetHostileTeams() const { return mCurrentPrefs.mHostileTeams; }
   std::set<QString> GetNeutralTeams() const { return mCurrentPrefs.mNeutralTeams; }

signals:
   void UseSymbologyChanged();
   void ShowVelocityVectorChanged();
   void TeamMappingsChanged();

private:
   void               Apply() override;
   MilStdIconPrefData ReadSettings(QSettings& aSettings) const override;
   void               SaveSettingsP(QSettings& aSettings) const override;
   void               SetPreferenceDataP(const MilStdIconPrefData& aPrefData) override;

   bool mUseSymbologyChanged{false};
   bool mShowVelocityVectorChanged{false};
   bool mTeamMappingsChanged{false};
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::MilStdIconPrefData)
#endif
