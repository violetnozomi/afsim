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

#ifndef MapHoverInfoPREFOBJECT_HPP
#define MapHoverInfoPREFOBJECT_HPP

#include <QObject>
#include <QStringList>

#include "WkfPrefObject.hpp"

namespace MapHoverInfo
{
struct PrefData
{
   bool mShowTooltips{true};
   bool mShowNames{true};
   bool mShowItemLabels{true};

   QStringList mPlatformUpdaterList{"Position", "Altitude", "Heading", "Speed"};
   QStringList mTrackUpdaterList{"Side", "Type", "Heading"};

   QStringList mPlatformEventList{"Event Type", "Time", "Position", "Altitude"};
   QStringList mWeaponEventList{"Event Type", "Time", "Position", "Altitude"};
};

class PrefObject : public wkf::PrefObjectT<PrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "MapHoverInfoObject";

   PrefObject(QObject* parent = nullptr);
   ~PrefObject() override = default;

   bool GetShowNames() const { return mCurrentPrefs.mShowNames; }
   bool GetShowLabels() const { return mCurrentPrefs.mShowItemLabels; }
   bool GetShowTooltips() const { return mCurrentPrefs.mShowTooltips; }

   const QStringList& GetPlatformUpdaterList() const { return mCurrentPrefs.mPlatformUpdaterList; }
   const QStringList& GetTrackUpdaterList() const { return mCurrentPrefs.mTrackUpdaterList; }

   const QStringList& GetPlatformEventList() const { return mCurrentPrefs.mPlatformEventList; }
   const QStringList& GetWeaponEventList() const { return mCurrentPrefs.mWeaponEventList; }

signals:
   void HoverPrefsChanged();

private:
   void     Apply() override;
   PrefData ReadSettings(QSettings& aSettings) const override;
   void     SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace MapHoverInfo

Q_DECLARE_METATYPE(MapHoverInfo::PrefData)
#endif
