// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFGENERALPREFOBJECT_HPP
#define WKFGENERALPREFOBJECT_HPP

#include "wkf_export.h"

#include <string>

#include <QColor>
#include <QMap>
#include <QString>
#include <osg/MatrixTransform>

#include "UtColor.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfOverlayBanner.hpp"
#include "WkfPrefObject.hpp"

namespace vespa
{
class VaViewer;
};

namespace wkf
{
struct GeneralPrefData
{
   QString mThemeName{"Dark"};

   int  mNumberOfRecentConfigs{5};
   bool mShowStatusBar{true};

   bool    mShowAppBanner{false};
   bool    mUseSimNameAsAppBanner{false};
   int     mAppBannerFont{12};
   QColor  mAppBannerColor{0, 0, 0, 255};
   QColor  mAppBannerBgColor{0, 255, 0, 200};
   QString mAppBannerText{};

   bool    mShowClassificationOverlay{true};
   int     mOverlayBannerFont{12};
   QColor  mOverlayBannerColor{0, 240, 0, 255};
   QColor  mOverlayBannerBgColor{0, 0, 0, 64};
   QString mOverlayBannerText{};
};

class WKF_EXPORT GeneralPrefObject : public PrefObjectT<GeneralPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "GeneralOptions";

   GeneralPrefObject(QObject* parent = nullptr);
   ~GeneralPrefObject() override = default;

   int GetNumberOfRecentConfigs() const { return mCurrentPrefs.mNumberOfRecentConfigs; }

   QStringList        GetPaletteKeys() const { return mPaletteMap.keys(); }
   const std::string& GetSimulationName() const noexcept { return mSimulationName; }

   void SetOverlayBannerText(const std::string& aBannerText, const UtColor& aClassificationColor);
   void SetSimulationName(std::string aSimName);

signals:
   void NumRecentConfigsChanged(int aSize);

private:
   void CreatePaletteMap();

   void ViewerInitializedCB(vespa::VaViewer* aViewer);

   void CreateOrUpdateOverlayText(vespa::VaViewer* aViewer);

   void SetPreferenceDataP(const GeneralPrefData& aPrefData) override;
   void Apply() override;

   GeneralPrefData ReadSettings(QSettings& aSettings) const override;
   void            SaveSettingsP(QSettings& aSettings) const override;

   void EmitAllSignals();

   QMap<QString, QPalette> mPaletteMap;
   bool                    mThemeUpdated;
   vespa::VaCallbackHolder mCallbacks;
   std::string             mOverlayBannerText;
   std::string             mSimulationName;
   UtColor                 mClassificationColor;
};

// create a new overlay type here since we want it to be the only overlay of its type
// this protects us in case someone adds another wkf::OverlayBanner down the line
class UniqueBanner : public wkf::OverlayBanner
{
public:
   UniqueBanner();
   ~UniqueBanner() override = default;
};

} // namespace wkf
VA_DECLARE_OBJECT_TYPE(wkf::UniqueBanner)
Q_DECLARE_METATYPE(wkf::GeneralPrefData)
#endif
