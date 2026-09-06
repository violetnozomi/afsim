// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfLogConsolePrefObject.hpp"

#include "UtQtSettingsScopes.hpp"

wkf::log::ConsolePrefData::ConsolePrefData() noexcept
{
   mPatterns.append(ColorPattern{"***** FATAL:", "red"});
   mPatterns.append(ColorPattern{"***** ERROR:", "red"});
   mPatterns.append(ColorPattern{"***** WARNING:", "orange"});
   mPatterns.append(ColorPattern{"***** DEVELOPER:", "cyan"});
}

wkf::log::ConsolePrefObject::ConsolePrefObject(QObject* aParent /* = nullptr */) noexcept
   : wkf::PrefObjectT<ConsolePrefData>(aParent, cNAME)
{
}

wkf::log::PatternList wkf::log::ConsolePrefObject::GetPatterns() const noexcept
{
   if (mCurrentPrefs.mPatternsEnabled)
   {
      PatternList retval;
      retval.reserve(mCurrentPrefs.mPatterns.size());
      for (const auto& pattern : mCurrentPrefs.mPatterns)
      {
         retval.push_back(&pattern);
      }
      return retval;
   }
   return {};
}

void wkf::log::ConsolePrefObject::Apply()
{
   if (mFontSizeChanged)
   {
      emit FontSizeChanged(mCurrentPrefs.mFontSize);
      mFontSizeChanged = false;
   }
   if (mHtmlChanged)
   {
      emit HtmlChanged();
      mHtmlChanged = false;
   }
}

void wkf::log::ConsolePrefObject::SetPreferenceDataP(const ConsolePrefData& aPrefData)
{
   if (mCurrentPrefs.mFontSize != aPrefData.mFontSize)
   {
      mFontSizeChanged = true;
   }
   if (mCurrentPrefs.mLineCap != aPrefData.mLineCap)
   {
      mHtmlChanged = true;
   }
   if (mCurrentPrefs.mPatternsEnabled != aPrefData.mPatternsEnabled)
   {
      mHtmlChanged = true;
   }
   if (mCurrentPrefs.mPatterns != aPrefData.mPatterns)
   {
      mHtmlChanged = true;
   }

   mCurrentPrefs = aPrefData;
}

wkf::log::ConsolePrefData wkf::log::ConsolePrefObject::ReadSettings(QSettings& aSettings) const
{
   ConsolePrefData retval;
   retval.mFontSize        = aSettings.value("fontSize", mDefaultPrefs.mFontSize).toInt();
   retval.mLineCap         = aSettings.value("lineCap", mDefaultPrefs.mLineCap).toInt();
   retval.mPatternsEnabled = aSettings.value("patternsEnabled", mDefaultPrefs.mPatternsEnabled).toBool();

   if (aSettings.childGroups().contains("patterns"))
   {
      retval.mPatterns.clear();
   }

   ut::qt::ReadArrayScope ras{aSettings, "patterns"};
   while (ras.Next())
   {
      ColorPattern pattern;

      pattern.mRawPattern     = aSettings.value("pattern").toString();
      pattern.mColor          = aSettings.value("color").value<QColor>();
      pattern.mMatchCase      = aSettings.value("matchCase", false).toBool();
      pattern.mMatchWholeWord = aSettings.value("wholeWord", false).toBool();
      pattern.mUseRegex       = aSettings.value("useRegex", false).toBool();

      pattern.Update();

      retval.mPatterns.append(pattern);
   }

   return retval;
}

void wkf::log::ConsolePrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("fontSize", mCurrentPrefs.mFontSize);
   aSettings.setValue("lineCap", mCurrentPrefs.mLineCap);
   aSettings.setValue("patternsEnabled", mCurrentPrefs.mPatternsEnabled);

   ut::qt::WriteArrayScope was{aSettings, "patterns"};
   for (const auto& pattern : mCurrentPrefs.mPatterns)
   {
      was.Next();

      aSettings.setValue("pattern", pattern.mRawPattern);
      aSettings.setValue("color", pattern.mColor);
      aSettings.setValue("matchCase", pattern.mMatchCase);
      aSettings.setValue("wholeWord", pattern.mMatchWholeWord);
      aSettings.setValue("useRegex", pattern.mUseRegex);
   }
}
