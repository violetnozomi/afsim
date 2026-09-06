// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFOVERLAYUPDATERPREFOBJECT_HPP
#define WKFOVERLAYUPDATERPREFOBJECT_HPP

#include "wkf_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
struct OverlayUpdaterPrefData
{
   std::map<QString, QFont>  mFontMap;
   std::map<QString, QColor> mColorMap;
};

class WKF_EXPORT OverlayUpdaterPrefObject : public PrefObjectT<OverlayUpdaterPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "OverlayUpdaterPreferences";

   OverlayUpdaterPrefObject(QObject* parent = nullptr);
   ~OverlayUpdaterPrefObject() override = default;

   void          AddColor(const QString& aName, const QColor& aColor);
   void          AddFont(const QString& aName, const QFont& aFont);
   const QColor& LookupColor(const QString& aName) const;
   const QFont&  LookupFont(const QString& aName) const;

signals:
   void OverlayUpdatersChanged();

private:
   void                   Apply() override;
   OverlayUpdaterPrefData ReadSettings(QSettings& aSettings) const override;
   void                   SaveSettingsP(QSettings& aSettings) const override;

   QColor mDefaultColor;
   QFont  mDefaultFont;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::OverlayUpdaterPrefData)
#endif
