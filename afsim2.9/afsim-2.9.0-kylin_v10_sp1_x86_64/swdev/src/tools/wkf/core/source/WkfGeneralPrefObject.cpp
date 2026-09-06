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
#include "WkfGeneralPrefObject.hpp"

#include <QApplication>
#include <QIcon>
#include <QPalette>
#include <QPixmapCache> //Included for a workaround to known Qt bug
#include <QStatusBar>
#include <QStyle>
#include <QStyleFactory>
#include <QToolTip>

#include "UtQt.hpp"
#include "UtoRawShape.hpp"
#include "VaObserver.hpp"
#include "VaOverlay.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::GeneralPrefObject::GeneralPrefObject(QObject* parent)
   : PrefObjectT<GeneralPrefData>(parent, cNAME)
   , mThemeUpdated(true)
   , mOverlayBannerText("")
{
   CreatePaletteMap();
   mCallbacks.Add(vespa::VaObserver::ViewerInitialized.Connect(&GeneralPrefObject::ViewerInitializedCB, this));
}

void wkf::GeneralPrefObject::SetOverlayBannerText(const std::string& aBannerText, const UtColor& aClassificationColor)
{
   mOverlayBannerText   = aBannerText;
   mClassificationColor = aClassificationColor;

   // Update the text in any viewers
   auto& viewers = vaEnv.GetViewerManager()->GetViewers();
   for (auto& viewer : viewers)
   {
      CreateOrUpdateOverlayText(viewer);
   }
}

void wkf::GeneralPrefObject::SetSimulationName(std::string aSimName)
{
   mSimulationName = std::move(aSimName);
   Apply();
}

void wkf::GeneralPrefObject::CreatePaletteMap()
{
   QPalette lightPalette = qApp->style()->standardPalette();
   lightPalette.setColor(QPalette::Link, QColor(0, 99, 177));
   lightPalette.setColor(QPalette::Highlight, QColor(0, 99, 177));
   mPaletteMap.insert("Light", lightPalette);

   QPalette darkPalette(lightPalette);
   darkPalette.setColor(QPalette::Active, QPalette::Base, QColor(30, 30, 32));
   darkPalette.setColor(QPalette::Inactive, QPalette::Base, QColor(30, 30, 32));
   darkPalette.setColor(QPalette::Disabled, QPalette::Base, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::Button, QColor(45, 45, 47));
   darkPalette.setColor(QPalette::Inactive, QPalette::Button, QColor(45, 45, 47));
   darkPalette.setColor(QPalette::Disabled, QPalette::Button, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::ButtonText, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Inactive, QPalette::ButtonText, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(40, 40, 42));
   darkPalette.setColor(QPalette::Active, QPalette::Light, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Inactive, QPalette::Light, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Disabled,
                        QPalette::Light,
                        QColor(220, 220, 222, 0)); // alpha set to 0 to remove unwanted shadow effect for disabled menu items
   darkPalette.setColor(QPalette::Active, QPalette::Text, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Inactive, QPalette::Text, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::Window, QColor(45, 45, 47));
   darkPalette.setColor(QPalette::Inactive, QPalette::Window, QColor(45, 45, 47));
   darkPalette.setColor(QPalette::Disabled, QPalette::Window, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::WindowText, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Inactive, QPalette::WindowText, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::ToolTipText, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::ToolTipBase, QColor(220, 220, 222));

   mPaletteMap.insert("Dark", darkPalette);
}

void wkf::GeneralPrefObject::ViewerInitializedCB(vespa::VaViewer* aViewer)
{
   if (aViewer)
   {
      CreateOrUpdateOverlayText(aViewer);
   }
}

void wkf::GeneralPrefObject::CreateOrUpdateOverlayText(vespa::VaViewer* aViewer)
{
   if (aViewer->IsInitialized())
   {
      OverlayBanner* overlay = aViewer->FindFirstOverlayOfType<wkf::UniqueBanner>();

      if (mCurrentPrefs.mShowClassificationOverlay)
      {
         if (mOverlayBannerText.empty())
         {
            if (overlay)
            {
               overlay->SetVisible(false);
            }
         }
         else
         {
            if (!overlay)
            {
               overlay = new UniqueBanner();
               aViewer->AddOverlay(overlay);
            }
            overlay->SetVisible(true);
            QFont f;
            f.setPointSize(mCurrentPrefs.mOverlayBannerFont);
            QColor color(mClassificationColor[0] * 255, mClassificationColor[1] * 255, mClassificationColor[2] * 255);
            overlay->Setup(QString::fromStdString(mOverlayBannerText), color, f);
         }
      }
      else if (overlay)
      {
         overlay->SetVisible(false);
      }
   }
}

void wkf::GeneralPrefObject::SetPreferenceDataP(const GeneralPrefData& aPrefData)
{
   if (aPrefData.mThemeName != mCurrentPrefs.mThemeName)
   {
      mThemeUpdated = true;
   }
   BasePrefObject::SetPreferenceDataP(aPrefData);
}

wkf::GeneralPrefData wkf::GeneralPrefObject::ReadSettings(QSettings& aSettings) const
{
   GeneralPrefData pData;
   pData.mThemeName             = aSettings.value("currentPalette", mDefaultPrefs.mThemeName).toString();
   pData.mNumberOfRecentConfigs = aSettings.value("numConfig", mDefaultPrefs.mNumberOfRecentConfigs).toInt();
   pData.mShowStatusBar         = aSettings.value("statusBar", mDefaultPrefs.mShowStatusBar).toBool();

   pData.mAppBannerBgColor      = aSettings.value("appBgColor", mDefaultPrefs.mAppBannerBgColor).value<QColor>();
   pData.mAppBannerColor        = aSettings.value("appColor", mDefaultPrefs.mAppBannerColor).value<QColor>();
   pData.mAppBannerFont         = aSettings.value("appFont", mDefaultPrefs.mAppBannerFont).toDouble();
   pData.mAppBannerText         = aSettings.value("appText", mDefaultPrefs.mAppBannerText).toString();
   pData.mUseSimNameAsAppBanner = aSettings.value("useSimName", mDefaultPrefs.mUseSimNameAsAppBanner).toBool();
   pData.mShowAppBanner         = aSettings.value("showApp", mDefaultPrefs.mShowAppBanner).toBool();

   pData.mOverlayBannerBgColor = aSettings.value("overlayBgColor", mDefaultPrefs.mOverlayBannerBgColor).value<QColor>();
   pData.mOverlayBannerColor   = aSettings.value("overlayColor", mDefaultPrefs.mOverlayBannerColor).value<QColor>();
   pData.mOverlayBannerFont    = aSettings.value("overlayFont", mDefaultPrefs.mOverlayBannerFont).toDouble();
   pData.mOverlayBannerText    = aSettings.value("overlayText", mDefaultPrefs.mOverlayBannerText).toString();

   pData.mOverlayBannerFont = aSettings.value("overlayFont", mDefaultPrefs.mOverlayBannerFont).toDouble();
   pData.mShowClassificationOverlay =
      aSettings.value("showClassification", mDefaultPrefs.mShowClassificationOverlay).toBool();

   return pData;
}

void wkf::GeneralPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("currentPalette", mCurrentPrefs.mThemeName);
   aSettings.setValue("numConfig", mCurrentPrefs.mNumberOfRecentConfigs);
   aSettings.setValue("statusBar", mCurrentPrefs.mShowStatusBar);

   aSettings.setValue("appBgColor", mCurrentPrefs.mAppBannerBgColor);
   aSettings.setValue("appColor", mCurrentPrefs.mAppBannerColor);
   aSettings.setValue("appFont", mCurrentPrefs.mAppBannerFont);
   aSettings.setValue("appText", mCurrentPrefs.mAppBannerText);
   aSettings.setValue("useSimName", mCurrentPrefs.mUseSimNameAsAppBanner);
   aSettings.setValue("showApp", mCurrentPrefs.mShowAppBanner);

   aSettings.setValue("overlayFont", mCurrentPrefs.mOverlayBannerFont);
   aSettings.setValue("showClassification", mCurrentPrefs.mShowClassificationOverlay);
}

void wkf::GeneralPrefObject::EmitAllSignals()
{
   emit NumRecentConfigsChanged(mCurrentPrefs.mNumberOfRecentConfigs);
}

void wkf::GeneralPrefObject::Apply()
{
   // due to Apply being called from the Environment directly on launch to apply the them, check to see if MainWindow is valid
   if (wkfEnv.GetMainWindow())
   {
      wkfEnv.GetMainWindow()->statusBar()->setVisible(mCurrentPrefs.mShowStatusBar);
   }
   EmitAllSignals();

   if (mThemeUpdated)
   {
      // QApplication::setPalette (same as aApp->setPalette()) changes the currently displayed palette to the desired palette
      //  but does not emit the paletteChanged signal.
      // QGuiApplication::setPalette emits the paletteChanged() signal but does not update the displayed palette.
      // When both of these setPalette functions are called with the same palette argument,
      //  only the first one called recognizes the palette is different, and executes.
      // Thus we have to manually emit the paletteChanged() signal
      QPalette palette = mPaletteMap.value(mCurrentPrefs.mThemeName);
      qApp->setPalette(palette);
      emit qApp->paletteChanged(palette);

      // This is a workaround for QTBUG-65475 (https://bugreports.qt.io/browse/QTBUG-65475)
      // When using fusion style, application palette changes at runtime are not propagated
      // correctly to all widgets. The cause is the pixmap caching done inside the style.
      QPixmapCache::clear();

      qApp->setStyleSheet(QString("                                \
      QAbstractItemView::indicator,                                \
      QCheckBox::indicator,                                        \
      QGroupBox::indicator,                                        \
      QRadioButton::indicator                                      \
      {                                                            \
          height: 12px;                                            \
          width:  12px;                                            \
      }                                                            \
                                                                   \
      QMenu::separator, QToolBar::separator                        \
      {                                                            \
          background: palette(Midlight);                           \
          width:   1px;                                            \
          height:  1px;                                            \
          margin:  4px;                                            \
      }                                                            \
                                                                   \
      QTabBar::tab:hover, QToolBox::tab:hover                      \
      {                                                            \
          color: palette(Highlight);                               \
      }                                                            \
                                                                   \
      QTabBar::close-button                                        \
      {                                                            \
         image: url(:/icons/%1/64x64/close.png);                   \
         subcontrol-position : right;                              \
      }                                                            \
                                                                   \
      QTabBar::close-button:hover                                  \
      {                                                            \
         image: url(:/icons/%1/64x64/close_hover.png);             \
      }                                                            \
                                                                   \
      QAbstractItemView::indicator::checked,                       \
      QCheckBox::indicator:checked,                                \
      QGroupBox::indicator:checked                                 \
      {                                                            \
          image: url(:/icons/%1/64x64/checkbox_checked.png);       \
      }                                                            \
                                                                   \
      QAbstractItemView::indicator::unchecked,                     \
      QCheckBox::indicator:unchecked,                              \
      QGroupBox::indicator:unchecked                               \
      {                                                            \
          image: url(:/icons/%1/64x64/checkbox_unchecked.png);     \
      }                                                            \
                                                                   \
      QAbstractItemView::indicator::indeterminate,                 \
      QCheckBox::indicator:indeterminate,                          \
      QGroupBox::indicator:indeterminate                           \
      {                                                            \
          image: url(:/icons/%1/64x64/checkbox_indeterminate.png); \
      }                                                            \
                                                                   \
      QRadioButton::indicator:checked                              \
      {                                                            \
          image: url(:/icons/%1/64x64/radiobutton_checked.png);    \
      }                                                            \
                                                                   \
      QRadioButton::indicator:unchecked                            \
      {                                                            \
          image: url(:/icons/%1/64x64/radiobutton_unchecked.png);  \
      }                                                            \
      ")
                             .arg(mCurrentPrefs.mThemeName));

      QIcon::setThemeName(mCurrentPrefs.mThemeName);
      QToolTip::setPalette(palette);
   }
   mThemeUpdated = false;

   if (wkfEnv.GetMainWindow())
   {
      if (mCurrentPrefs.mShowAppBanner)
      {
         QString appBanner = mCurrentPrefs.mAppBannerText;
         if (mCurrentPrefs.mUseSimNameAsAppBanner && !mSimulationName.empty())
         {
            appBanner = QString::fromStdString(mSimulationName);
         }
         wkfEnv.GetMainWindow()->ShowBannerText(appBanner,
                                                mCurrentPrefs.mAppBannerColor,
                                                mCurrentPrefs.mAppBannerBgColor,
                                                mCurrentPrefs.mAppBannerFont);
      }
      else
      {
         wkfEnv.GetMainWindow()->HideBannerText();
      }
   }

   auto& viewers = vaEnv.GetViewerManager()->GetViewers();
   for (auto& viewer : viewers)
   {
      CreateOrUpdateOverlayText(viewer);
   }
}

wkf::UniqueBanner::UniqueBanner()
   : wkf::OverlayBanner("wkfUniqueBanner")
{
   SetType<UniqueBanner>();
}
