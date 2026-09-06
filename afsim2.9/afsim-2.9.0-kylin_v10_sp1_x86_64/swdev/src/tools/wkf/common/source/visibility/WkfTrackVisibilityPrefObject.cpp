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
#include "WkfTrackVisibilityPrefObject.hpp"

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfTrack.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::TrackVisibilityPrefObject::TrackVisibilityPrefObject(QObject* parent)
   : PrefObjectT<TrackVisibilityPrefData>(parent, cNAME)
   , mTrackVisibilityChanged(false)
   , mTrackIconChanged(false)
   , mTrackImageChanged(false)
   , mTrackColorChanged(false)
{
}

const QString& wkf::TrackVisibilityPrefObject::GetDomainIconPath(SpatialDomain aDomain)
{
   if (0 <= aDomain && aDomain < SPATIAL_DOMAIN_COUNT)
   {
      return mCurrentPrefs.mDomainIcons[aDomain];
   }
   return mCurrentPrefs.mDomainIcons[SPATIAL_DOMAIN_UNKNOWN];
}

void wkf::TrackVisibilityPrefObject::SetPreferenceDataP(const wkf::TrackVisibilityPrefData& aPrefData)
{
   mTrackVisibilityChanged = aPrefData.mTrackVisibility != mCurrentPrefs.mTrackVisibility;
   mTrackIconChanged =
      aPrefData.mShowColor != mCurrentPrefs.mShowColor || aPrefData.mShowDirection != mCurrentPrefs.mShowDirection ||
      aPrefData.mTrackScale != mCurrentPrefs.mTrackScale || aPrefData.mFont != mCurrentPrefs.mFont ||
      aPrefData.mIconType != mCurrentPrefs.mIconType || aPrefData.mUsePlatformIcon != mCurrentPrefs.mUsePlatformIcon;
   mTrackImageChanged =
      aPrefData.mCustomIcon != mCurrentPrefs.mCustomIcon || aPrefData.mDomainIcons != mCurrentPrefs.mDomainIcons;
   mTrackColorChanged = aPrefData.mShowColor != mCurrentPrefs.mShowColor;

   BasePrefObject::SetPreferenceDataP(aPrefData);
}

wkf::TrackVisibilityPrefData wkf::TrackVisibilityPrefObject::ReadSettings(QSettings& aSettings) const
{
   TrackVisibilityPrefData pData;
   pData.mTrackVisibility  = aSettings.value("trackVis", mDefaultPrefs.mTrackVisibility).value<tracks::Visibility>();
   pData.mSelectedTeam     = aSettings.value("trackTeam", mDefaultPrefs.mSelectedTeam).toString();
   pData.mShowRemoteTracks = aSettings.value("remoteTracks", mDefaultPrefs.mShowRemoteTracks).toBool();
   pData.mShowColor        = aSettings.value("trackShowColor", mDefaultPrefs.mShowColor).toBool();
   pData.mShowLabel        = aSettings.value("trackShowLabel", mDefaultPrefs.mShowLabel).toBool();
   pData.mShowDirection    = aSettings.value("trackShowDirection", mDefaultPrefs.mShowDirection).toBool();
   pData.mTrackScale       = aSettings.value("trackScale", mDefaultPrefs.mTrackScale).toInt();
   pData.mFont             = aSettings.value("font", mDefaultPrefs.mFont).value<QFont>();
   pData.mCustomIcon       = aSettings.value("iconPath", mDefaultPrefs.mCustomIcon).toString();
   pData.mIconType =
      aSettings.value("iconType", QVariant::fromValue(mDefaultPrefs.mIconType)).value<wkf::tracks::IconType>();

   pData.mUsePlatformIcon = aSettings.value("usePlatformIcon", mDefaultPrefs.mUsePlatformIcon).toBool();

   auto readDomainIcon = [&](SpatialDomain aDomain)
   {
      pData.mDomainIcons[aDomain] = aSettings
                                       .value(QString::fromStdString(wkf::SpatialDomainToString(aDomain)) + "Icon",
                                              mDefaultPrefs.mDomainIcons[aDomain])
                                       .toString();
   };

   readDomainIcon(SPATIAL_DOMAIN_LAND);
   readDomainIcon(SPATIAL_DOMAIN_AIR);
   readDomainIcon(SPATIAL_DOMAIN_SURFACE);
   readDomainIcon(SPATIAL_DOMAIN_SUBSURFACE);
   readDomainIcon(SPATIAL_DOMAIN_SPACE);
   readDomainIcon(SPATIAL_DOMAIN_UNKNOWN);

   return pData;
}

void wkf::TrackVisibilityPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("trackVis", mCurrentPrefs.mTrackVisibility);
   aSettings.setValue("trackTeam", mCurrentPrefs.mSelectedTeam);
   aSettings.setValue("remoteTracks", mCurrentPrefs.mShowRemoteTracks);
   aSettings.setValue("trackShowColor", mCurrentPrefs.mShowColor);
   aSettings.setValue("trackShowLabel", mCurrentPrefs.mShowLabel);
   aSettings.setValue("trackShowDirection", mCurrentPrefs.mShowDirection);
   aSettings.setValue("trackScale", mCurrentPrefs.mTrackScale);
   aSettings.setValue("font", mCurrentPrefs.mFont);
   aSettings.setValue("iconPath", mCurrentPrefs.mCustomIcon);
   aSettings.setValue("iconType", mCurrentPrefs.mIconType);

   aSettings.setValue("usePlatformIcon", mCurrentPrefs.mUsePlatformIcon);

   auto saveDomainIcon = [&](SpatialDomain aDomain)
   {
      aSettings.setValue(QString::fromStdString(wkf::SpatialDomainToString(aDomain)) + "Icon",
                         mCurrentPrefs.mDomainIcons[aDomain]);
   };

   saveDomainIcon(SPATIAL_DOMAIN_LAND);
   saveDomainIcon(SPATIAL_DOMAIN_AIR);
   saveDomainIcon(SPATIAL_DOMAIN_SURFACE);
   saveDomainIcon(SPATIAL_DOMAIN_SUBSURFACE);
   saveDomainIcon(SPATIAL_DOMAIN_SPACE);
   saveDomainIcon(SPATIAL_DOMAIN_UNKNOWN);
}

void wkf::TrackVisibilityPrefObject::Apply()
{
   if (mTrackVisibilityChanged)
   {
      emit TrackVisibilityChanged();
      mTrackVisibilityChanged = false;
   }
   if (mTrackIconChanged)
   {
      emit TrackIconChanged();
      mTrackIconChanged = false;
   }
   if (mTrackImageChanged)
   {
      emit TrackImageChanged();
      mTrackImageChanged = false;
   }
   if (mTrackColorChanged)
   {
      emit TrackColorChanged();
      mTrackColorChanged = false;
   }
}
