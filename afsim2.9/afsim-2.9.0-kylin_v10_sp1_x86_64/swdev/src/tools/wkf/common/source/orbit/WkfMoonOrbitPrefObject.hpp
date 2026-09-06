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
#ifndef WKFMOONORBITPREFOBJECT_HPP
#define WKFMOONORBITPREFOBJECT_HPP

class QSettings;
#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
struct MoonOrbitPrefData
{
   bool   mVisible{false};
   int    mLineWidth{1};
   QColor mColor{Qt::white};
};

class WKF_COMMON_EXPORT MoonOrbitPrefObject : public wkf::PrefObjectT<MoonOrbitPrefData>
{
   Q_OBJECT
public:
   MoonOrbitPrefObject(QObject* parent = nullptr);

   bool GetVisibility() const { return mCurrentPrefs.mVisible; }
   void SetVisibility(bool aVisible);

   QColor GetColor() const { return mCurrentPrefs.mColor; }
   void   SetColor(const QColor& aColor);

   int  GetLineWidth() const { return mCurrentPrefs.mLineWidth; }
   void SetLineWidth(int aWidth);

signals:
   void VisibilityChanged(bool aVisible);
   void ColorChanged(const QColor& aColor);
   void LineWidthChanged(int aWidth);

private:
   void              EmitAllSignals();
   void              Apply() override;
   MoonOrbitPrefData ReadSettings(QSettings& aSettings) const override;
   void              SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::MoonOrbitPrefData)
#endif
