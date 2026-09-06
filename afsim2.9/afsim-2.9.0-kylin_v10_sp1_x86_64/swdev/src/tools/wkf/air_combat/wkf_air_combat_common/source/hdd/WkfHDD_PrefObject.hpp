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

#ifndef WKFHDD_PREFOBJECT_HPP
#define WKFHDD_PREFOBJECT_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfPrefObject.hpp"

class QSettings;

namespace HDD
{
// This holds our pref data.  Set defaults here.
// To get possible values look at PrefWidget.cpp where the widgets are populated.
struct PrefData
{
   // Resolution
   unsigned int mResX{960};
   unsigned int mResY{540};

   // Section 1 pages
   QString mPage1{"TSD-1"};
   QString mPage5{"NAV"};
   QString mPage6{"TFLIR"};

   // Section 2 pages
   QString mPage2{"FCS"};
   QString mPage7{"HUD"};
   QString mPage8{"ICAWS"};

   // Section 3 pages
   QString mPage3{"SMS"};
   QString mPage9{"ENG"};
   QString mPage10{"FUEL"};

   // Section 4 pages
   QString mPage4{"TWD"};
   QString mPage11{"FCS"};
   QString mPage12{"EFI"};
};

class WKF_AIR_COMBAT_COMMON_EXPORT PrefObject : public wkf::PrefObjectT<PrefData>
{
   Q_OBJECT

public:
   PrefObject(QObject* parent = nullptr);

   /** Get the X resolution. */
   unsigned int GetResolutionX() const { return mCurrentPrefs.mResX; }

   /** Get the Y resolution. */
   unsigned int GetResolutionY() const { return mCurrentPrefs.mResY; }

   /** Return a list of preferred pages.  The dock widget will
    * use this list when initializing.
    */
   QStringList const GetPageList() const;

   /** Set the resolution preference.
    * @param aResX X resolution.
    * @param aResY Y resolution.
    */
   void SetResolution(unsigned int aResX, unsigned int aResY);
signals:
   /** Signal for the resolution changing.
    * @param aResX new X resolution.
    * @param aResY new Y resolution.
    */
   void ResolutionChanged(unsigned int aResX, unsigned int aResY);

private:
   /** Emit all signals for this pref object. */
   void EmitAllSignals();

   /** Apply changes to this pref object. */
   void Apply() override;

   /** Read in settings.
    * @param aSettings settings to read from.
    */
   PrefData ReadSettings(QSettings& aSettings) const override;

   /** Write to settings.
    * @param aSettings settings to write to.
    */
   void SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace HDD
Q_DECLARE_METATYPE(HDD::PrefData)
#endif // WKFHDD_PREFOBJECT_HPP
