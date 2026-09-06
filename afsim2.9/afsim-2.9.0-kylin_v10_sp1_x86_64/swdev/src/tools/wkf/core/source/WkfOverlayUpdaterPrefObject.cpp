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

#include "WkfOverlayUpdaterPrefObject.hpp"

#include <QApplication>

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

wkf::OverlayUpdaterPrefObject::OverlayUpdaterPrefObject(QObject* parent /*= nullptr*/)
   : wkf::PrefObjectT<OverlayUpdaterPrefData>(parent, cNAME)
{
   mDefaultColor = wkfEnv.GetMainWindow()->palette().color(QPalette::WindowText);
   mDefaultFont  = QApplication::font();
   connect(qGuiApp,
           &QGuiApplication::paletteChanged,
           [this](const QPalette& aPalette)
           {
              mDefaultColor = aPalette.color(QPalette::WindowText);
              emit OverlayUpdatersChanged();
           });
}

void wkf::OverlayUpdaterPrefObject::AddColor(const QString& aName, const QColor& aColor)
{
   mCurrentPrefs.mColorMap[aName] = aColor;
   emit OverlayUpdatersChanged();
   emit PreferencesChanged(QVariant::fromValue(mCurrentPrefs));
}

void wkf::OverlayUpdaterPrefObject::AddFont(const QString& aName, const QFont& aFont)
{
   mCurrentPrefs.mFontMap[aName] = aFont;
   emit OverlayUpdatersChanged();
   emit PreferencesChanged(QVariant::fromValue(mCurrentPrefs));
}

const QColor& wkf::OverlayUpdaterPrefObject::LookupColor(const QString& aName) const
{
   std::map<QString, QColor>::const_iterator it = mCurrentPrefs.mColorMap.find(aName);
   if (it != mCurrentPrefs.mColorMap.end())
   {
      return it->second;
   }
   else
   {
      return mDefaultColor;
   }
}

const QFont& wkf::OverlayUpdaterPrefObject::LookupFont(const QString& aName) const
{
   std::map<QString, QFont>::const_iterator it = mCurrentPrefs.mFontMap.find(aName);
   if (it != mCurrentPrefs.mFontMap.end())
   {
      return it->second;
   }
   else
   {
      return mDefaultFont;
   }
}

void wkf::OverlayUpdaterPrefObject::Apply()
{
   emit OverlayUpdatersChanged();
}

wkf::OverlayUpdaterPrefData wkf::OverlayUpdaterPrefObject::ReadSettings(QSettings& aSettings) const
{
   OverlayUpdaterPrefData pData;
   // First copy all the defaults into the preferences.
   pData.mColorMap = mDefaultPrefs.mColorMap;
   pData.mFontMap  = mDefaultPrefs.mFontMap;
   // Then read the INI file for modifications.
   // Note: this will not catch if you remove a default (Are we concerned about this?)
   //       We may actually desire that the default teams come back after being deleted.
   int s = aSettings.beginReadArray("colors");
   for (int i = 0; i < s; ++i)
   {
      aSettings.setArrayIndex(i);
      QString itemName = aSettings.value("name", "").toString();
      QColor  color    = aSettings.value("color").value<QColor>();

      pData.mColorMap[itemName] = color;
   }
   aSettings.endArray();
   s = aSettings.beginReadArray("fonts");
   for (int i = 0; i < s; ++i)
   {
      aSettings.setArrayIndex(i);
      QString itemName = aSettings.value("name", "").toString();
      QFont   font     = aSettings.value("font").value<QFont>();

      pData.mFontMap[itemName] = font;
   }
   aSettings.endArray();
   return pData;
}

void wkf::OverlayUpdaterPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   unsigned int i = 0;
   aSettings.beginWriteArray("colors");
   for (auto&& it : mCurrentPrefs.mColorMap)
   {
      aSettings.setArrayIndex(i++);
      aSettings.setValue("name", it.first);
      aSettings.setValue("color", it.second);
   }
   aSettings.endArray();
   aSettings.beginWriteArray("fonts");
   i = 0;
   for (auto&& it : mCurrentPrefs.mFontMap)
   {
      aSettings.setArrayIndex(i++);
      aSettings.setValue("name", it.first);
      aSettings.setValue("font", it.second);
   }
   aSettings.endArray();
}
