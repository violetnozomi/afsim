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
#ifndef WKF_LOG_CONSOLE_PREF_WIDGET_HPP
#define WKF_LOG_CONSOLE_PREF_WIDGET_HPP

#include "WkfLogConsolePrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfLogConsolePrefWidget.h"
#include "ui_WkfLogPatternWidget.h"

namespace wkf
{
namespace log
{
class WKF_COMMON_EXPORT PatternWidget final : public QWidget
{
   Q_OBJECT

public:
   PatternWidget(QWidget* aParent, int aIndex) noexcept;
   ~PatternWidget() noexcept override = default;

   ColorPattern GetPattern() const;
   void         SetPattern(const ColorPattern& aPattern);

   void IncrementIndex() { mIndex++; }
   void DecrementIndex() { mIndex--; }

signals:
   void AddClicked(int aIndex);
   void RemoveClicked(int aIndex);

private:
   //! Colors the line edit text red if "Use Regex" is enabled
   //!    AND the pattern is an invalid regex.
   //! Removes custom colors otherwise.
   void ValidateRegex();

   int                     mIndex = 0;
   Ui::WkfLogPatternWidget mUi;
};

class WKF_COMMON_EXPORT ConsolePrefWidget final : public wkf::PrefWidgetT<ConsolePrefObject>
{
   Q_OBJECT

public:
   ConsolePrefWidget(QWidget* aParent = nullptr) noexcept;
   ~ConsolePrefWidget() noexcept override = default;

   QString GetCategoryHint() const override;

private:
   void showEvent(QShowEvent* aEvent) override;
   void SetExampleTextSize(int aSize);

   void ReadPreferenceData(const ConsolePrefData& aPrefData) override;
   void WritePreferenceData(ConsolePrefData& aPrefData) override;

   void OnAddClicked(int aIndex);
   void OnRemoveClicked(int aIndex);
   void OnAppendClicked();

   void OnRegexHelpClicked() const;
   void OnScrollRangeChanged(int aMin, int aMax);

   PatternWidget* MakeWidget(int aIndex);

   //! Contains references to pattern widgets.
   //! DOES NOT OWN THE WIDGETS!
   QList<PatternWidget*>       mPatternWidgets;
   Ui::WkfLogConsolePrefWidget mUi;
   bool                        mScrollToBottom = false;
};
} // namespace log
} // namespace wkf

#endif
