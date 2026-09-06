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

#ifndef WKFHUDPREFOBJECTHPP
#define WKFHUDPREFOBJECTHPP

#include "wkf_air_combat_common_export.h"

#include "WkfPrefObject.hpp"

class QSettings;

namespace wkf
{
struct HUD_PrefData
{
   int mVerticalFieldOfView{90};

   unsigned int mResX{960};
   unsigned int mResY{540};
};

// The HUD::PrefObject provides the preferences object with
// which the users can set their preferences for the HeadsUpDisplay plugin.
class WKF_AIR_COMBAT_COMMON_EXPORT HUD_PrefObject : public PrefObjectT<wkf::HUD_PrefData>
{
   Q_OBJECT

public:
   explicit HUD_PrefObject(QObject* parent = nullptr);

   unsigned int GetResolutionX() const { return mCurrentPrefs.mResX; }
   unsigned int GetResolutionY() const { return mCurrentPrefs.mResY; }
   void         SetResolution(unsigned int aResX, unsigned int aResY);

signals:
   void ResolutionChanged(unsigned int aResX, unsigned int aResY);

private:
   void EmitAllSignals();

   void              Apply() override;
   wkf::HUD_PrefData ReadSettings(QSettings& aSettings) const override;
   void              SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::HUD_PrefData)
#endif
