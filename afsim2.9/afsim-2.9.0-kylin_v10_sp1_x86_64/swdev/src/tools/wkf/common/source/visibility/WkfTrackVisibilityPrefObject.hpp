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
#ifndef WKFTRACKVISIBILITYPREFOBJECT_HPP
#define WKFTRACKVISIBILITYPREFOBJECT_HPP

#include "wkf_common_export.h"

#include <array>

#include "WkfDefines.hpp"
#include "WkfPrefObject.hpp"

namespace wkf
{
namespace tracks
{
Q_NAMESPACE

enum Visibility
{
   cNONE,
   cLOCAL_ONLY,
   cALL_VISIBLE_TEAMS,
   cSELECTED_TEAM
};
Q_ENUM_NS(Visibility);

enum IconType
{
   cWEDGE_ICON,
   cCUSTOM_ICON,
   cRULES_ICON
};
Q_ENUM_NS(IconType);
} // namespace tracks

struct WKF_COMMON_EXPORT TrackVisibilityPrefData
{
   // Control which tracks are visible
   // {
   tracks::Visibility mTrackVisibility = tracks::Visibility::cLOCAL_ONLY;
   QString            mSelectedTeam;
   bool               mShowRemoteTracks = true;
   // }

   // Control how tracks are displayed
   // {
   bool             mShowColor     = true;
   bool             mShowLabel     = false;
   bool             mShowDirection = true;
   int              mTrackScale    = 100;
   QFont            mFont;
   tracks::IconType mIconType = tracks::IconType::cWEDGE_ICON;

   QString mCustomIcon;

   std::array<QString, SPATIAL_DOMAIN_COUNT> mDomainIcons;
   bool                                      mUsePlatformIcon = true;

   // }
};

class WKF_COMMON_EXPORT TrackVisibilityPrefObject : public PrefObjectT<TrackVisibilityPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "TrackOptions";

   TrackVisibilityPrefObject(QObject* parent = nullptr);
   ~TrackVisibilityPrefObject() override = default;

   tracks::Visibility GetTrackVisibility() const { return mCurrentPrefs.mTrackVisibility; }
   tracks::IconType   GetTrackIconType() const { return mCurrentPrefs.mIconType; }

   const QString& GetSelectedTeamForTracks() const { return mCurrentPrefs.mSelectedTeam; }
   bool           GetShowRemoteTracks() const { return mCurrentPrefs.mShowRemoteTracks; }
   bool           GetShowColor() const { return mCurrentPrefs.mShowColor; }
   bool           GetShowLabel() const { return mCurrentPrefs.mShowLabel; }
   const QFont&   GetTrackFont() const { return mCurrentPrefs.mFont; }
   bool           GetShowDirection() const { return mCurrentPrefs.mShowDirection; }
   int            GetTrackScale() const { return mCurrentPrefs.mTrackScale; }
   bool           GetPlatformIcons() const { return mCurrentPrefs.mUsePlatformIcon; }

   const QString& GetCustomIconPath() const { return mCurrentPrefs.mCustomIcon; }
   const QString& GetDomainIconPath(SpatialDomain aDomain);

signals:
   void TrackVisibilityChanged();
   void TrackIconChanged();
   void TrackImageChanged();
   void TrackColorChanged();

private:
   void Apply() override;
   void SetPreferenceDataP(const TrackVisibilityPrefData& aPrefData) override;

   TrackVisibilityPrefData ReadSettings(QSettings& aSettings) const override;
   void                    SaveSettingsP(QSettings& aSettings) const override;

   bool mTrackVisibilityChanged;
   bool mTrackIconChanged;
   bool mTrackImageChanged;
   bool mTrackColorChanged;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::TrackVisibilityPrefData)
#endif
