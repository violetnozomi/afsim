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
#ifndef WKFORBITPREFOBJECT_HPP
#define WKFORBITPREFOBJECT_HPP

class QSettings;
#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
struct OrbitPrefData
{
   int mFadeout{600};
   enum ColorMode
   {
      eWHITE,
      eNAMEHASH,
      eSCENARIO,
      eTEAM
   };
   ColorMode    mColorMode{eSCENARIO};
   int          mLineWidth{1};
   unsigned int mPeriods{1};
};

class WKF_COMMON_EXPORT OrbitPrefObject : public wkf::PrefObjectT<OrbitPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "OrbitOptions";
   OrbitPrefObject(QObject* parent = nullptr);

   int  GetFadeout() const { return mCurrentPrefs.mFadeout; }
   void SetFadeout(int aFadeout);

   OrbitPrefData::ColorMode GetColorMode() const { return mCurrentPrefs.mColorMode; }
   void                     SetColorMode(OrbitPrefData::ColorMode aMode);

   int  GetLineWidth() const { return mCurrentPrefs.mLineWidth; }
   void SetLineWidth(int aWidth);

   unsigned int GetPeriods() const { return mCurrentPrefs.mPeriods; }
   void         SetPeriods(unsigned int aPeriods);

signals:
   void FadeoutChanged(int aTime);
   void ColorModeChanged(OrbitPrefData::ColorMode aShow);
   void LineWidthChanged(int aWidth);
   void PeriodsChanged(unsigned int aPeriods);

private:
   void EmitAllSignals();

   void Apply() override;

   OrbitPrefData ReadSettings(QSettings& aSettings) const override;
   void          SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::OrbitPrefData)
#endif // WKFORBITPREFOBJECT_HPP
