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
#ifndef WKF_LOG_CONSOLE_PREF_OBJECT_HPP
#define WKF_LOG_CONSOLE_PREF_OBJECT_HPP

#include "wkf_common_export.h"

#include "WkfLogPatternUtils.hpp"
#include "WkfPrefObject.hpp"

namespace wkf
{
namespace log
{
struct ConsolePrefData final
{
   // Default constructor initializes mPatterns with the following:
   //    - "*****FATAL:"     = red
   //    - "*****ERROR:"     = red
   //    - "*****WARNING:"   = orange
   //    - "*****DEVELOPER:" = cyan
   ConsolePrefData() noexcept;

   int                 mFontSize        = 8;
   int                 mLineCap         = 1000;
   bool                mPatternsEnabled = true;
   QList<ColorPattern> mPatterns;
};

class WKF_COMMON_EXPORT ConsolePrefObject final : public wkf::PrefObjectT<ConsolePrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "ConsoleOptions";

   ConsolePrefObject(QObject* aParent = nullptr) noexcept;
   ~ConsolePrefObject() noexcept override = default;

   int GetFontSize() const noexcept { return mCurrentPrefs.mFontSize; }
   int GetLineCap() const noexcept { return mCurrentPrefs.mLineCap; }

   //! Returns pointers to the patterns in preferences.
   //! If patterns are disabled, returns an empty list.
   PatternList GetPatterns() const noexcept;

signals:
   void FontSizeChanged(int aSize);
   //! Emitted when the patterns or line cap change.
   void HtmlChanged();

private:
   bool mFontSizeChanged = true;
   bool mHtmlChanged     = true;

   void Apply() override;

   void            SetPreferenceDataP(const ConsolePrefData& aPrefData) override;
   ConsolePrefData ReadSettings(QSettings& aSettings) const override;
   void            SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace log
} // namespace wkf

Q_DECLARE_METATYPE(wkf::log::ConsolePrefData);

#endif
