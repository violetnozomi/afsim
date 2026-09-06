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

#include "WkfLogConsolePrefWidget.hpp"

#include <random>

#include <QMessageBox>
#include <QScrollBar>

static QColor GetRandomBrightColor()
{
   static std::default_random_engine  engine{std::random_device()()};
   std::uniform_int_distribution<int> dist{30, 225};

   return QColor(dist(engine), dist(engine), dist(engine));
}

wkf::log::PatternWidget::PatternWidget(QWidget* aParent, int aIndex) noexcept
   : QWidget(aParent)
   , mIndex(aIndex)
{
   mUi.setupUi(this);

   mUi.colorButton->SetColor(GetRandomBrightColor());

   connect(mUi.patternEdit, &QLineEdit::textEdited, this, &PatternWidget::ValidateRegex);
   connect(mUi.regexButton, &QPushButton::clicked, this, &PatternWidget::ValidateRegex);

   connect(mUi.addButton, &QToolButton::clicked, this, [this]() { emit AddClicked(mIndex); });
   connect(mUi.removeButton, &QToolButton::clicked, this, [this]() { emit RemoveClicked(mIndex); });
}

wkf::log::ColorPattern wkf::log::PatternWidget::GetPattern() const
{
   ColorPattern retval;

   retval.mRawPattern     = mUi.patternEdit->text();
   retval.mColor          = mUi.colorButton->GetColor();
   retval.mMatchCase      = mUi.matchCaseButton->isChecked();
   retval.mMatchWholeWord = mUi.matchWordButton->isChecked();
   retval.mUseRegex       = mUi.regexButton->isChecked();

   retval.Update();

   return retval;
}

void wkf::log::PatternWidget::SetPattern(const ColorPattern& aPattern)
{
   mUi.patternEdit->setText(aPattern.mRawPattern);
   mUi.colorButton->SetColor(aPattern.mColor);
   mUi.matchCaseButton->setChecked(aPattern.mMatchCase);
   mUi.matchWordButton->setChecked(aPattern.mMatchWholeWord);
   mUi.regexButton->setChecked(aPattern.mUseRegex);

   ValidateRegex();
}

void wkf::log::PatternWidget::ValidateRegex()
{
   if (mUi.regexButton->isChecked() && !QRegularExpression(mUi.patternEdit->text()).isValid())
   {
      mUi.patternEdit->setStyleSheet("color: red;");
   }
   else
   {
      mUi.patternEdit->setStyleSheet({});
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

wkf::log::ConsolePrefWidget::ConsolePrefWidget(QWidget* aParent) noexcept
   : wkf::PrefWidgetT<ConsolePrefObject>(aParent)
{
   mUi.setupUi(this);

   connect(mUi.fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ConsolePrefWidget::SetExampleTextSize);

   connect(mUi.appendColorButton, &QPushButton::clicked, this, &ConsolePrefWidget::OnAppendClicked);
   connect(mUi.regexHelpButton, &QToolButton::clicked, this, &ConsolePrefWidget::OnRegexHelpClicked);
   connect(mUi.scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged, this, &ConsolePrefWidget::OnScrollRangeChanged);
}

QString wkf::log::ConsolePrefWidget::GetCategoryHint() const
{
   return "Simulation Logs";
}

void wkf::log::ConsolePrefWidget::showEvent(QShowEvent* aEvent)
{
   SetExampleTextSize(mPrefObjectPtr->GetFontSize());
}

void wkf::log::ConsolePrefWidget::SetExampleTextSize(int aSize)
{
   QTextCursor cursor = mUi.exampleText->textCursor();
   mUi.exampleText->selectAll();
   mUi.exampleText->setFontPointSize(aSize);
   mUi.exampleText->setTextCursor(cursor);
}

void wkf::log::ConsolePrefWidget::ReadPreferenceData(const ConsolePrefData& aPrefData)
{
   mUi.fontSizeSpinBox->setValue(aPrefData.mFontSize);
   mUi.lineCapSpinBox->setValue(aPrefData.mLineCap);
   mUi.colorPatternsGroupBox->setChecked(aPrefData.mPatternsEnabled);

   // Clear current pattern widget list.
   for (int i = 0; i < mPatternWidgets.size(); i++)
   {
      auto item = mUi.patternList->takeAt(0);
      delete item->widget();
      delete item;
   }
   mPatternWidgets.clear();

   // Create new pattern widget list.
   for (int i = 0; i < aPrefData.mPatterns.size(); i++)
   {
      MakeWidget(i)->SetPattern(aPrefData.mPatterns[i]);
   }
}

void wkf::log::ConsolePrefWidget::WritePreferenceData(ConsolePrefData& aPrefData)
{
   aPrefData.mFontSize        = mUi.fontSizeSpinBox->value();
   aPrefData.mLineCap         = mUi.lineCapSpinBox->value();
   aPrefData.mPatternsEnabled = mUi.colorPatternsGroupBox->isChecked();
   aPrefData.mPatterns.clear();

   for (const auto* patternWidget : mPatternWidgets)
   {
      aPrefData.mPatterns.append(patternWidget->GetPattern());
   }
}

void wkf::log::ConsolePrefWidget::OnAddClicked(int aIndex)
{
   MakeWidget(aIndex);
}

void wkf::log::ConsolePrefWidget::OnRemoveClicked(int aIndex)
{
   auto item = mUi.patternList->takeAt(aIndex);
   delete item->widget();
   delete item;
   mPatternWidgets.removeAt(aIndex);

   for (int i = aIndex; i < mPatternWidgets.size(); i++)
   {
      mPatternWidgets[i]->DecrementIndex();
   }
}

void wkf::log::ConsolePrefWidget::OnAppendClicked()
{
   MakeWidget(mPatternWidgets.size());
   mScrollToBottom = true;
}

void wkf::log::ConsolePrefWidget::OnRegexHelpClicked() const
{
   QMessageBox msgBox{QMessageBox::Icon::NoIcon,
                      "Regex Help",
                      R"regex(Perl Regular Expression Quick Reference Guide

Characters:
   a          Just an 'a' character
   abc        The string 'abc'
   .          Any character except new line
   ^          Beginning of line (except in [])
   $          End of line
   \b         Word boundary
   (...)      Grouping
   abc|def    The string 'abc' OR 'def'
   \.         The period character
              or other control character

Character Classes:
   [abc]      Either 'a', 'b', or 'c'
   [a-f]      The same as [abcdef]
   [^abc]     The opposite of [abc]
   \w         Word characters: [a-zA-Z0-9_]
   \d         Digit characters: [0-9]
   \s         Whitespace characters: [\f\t\n\r ]
   \W         [^\w]
   \D         [^\d]
   \S         [^\s]

Quantifiers:
   a?         0-1 'a' characters
   a+         1 or more 'a' characters
   a*         0 or more 'a' characters)regex"};
   msgBox.setFont(QFont{"Courier New"});
   msgBox.exec();
}

void wkf::log::ConsolePrefWidget::OnScrollRangeChanged(int aMin, int aMax)
{
   if (mScrollToBottom)
   {
      mUi.scrollArea->verticalScrollBar()->setValue(aMax);
      mScrollToBottom = false;
   }
}

wkf::log::PatternWidget* wkf::log::ConsolePrefWidget::MakeWidget(int aIndex)
{
   auto patternWidget = new PatternWidget(nullptr, aIndex);
   mUi.patternList->insertWidget(aIndex, patternWidget);
   mPatternWidgets.insert(aIndex, patternWidget);

   connect(patternWidget, &PatternWidget::AddClicked, this, &ConsolePrefWidget::OnAddClicked);
   connect(patternWidget, &PatternWidget::RemoveClicked, this, &ConsolePrefWidget::OnRemoveClicked);

   for (int i = aIndex + 1; i < mPatternWidgets.size(); i++)
   {
      mPatternWidgets[i]->IncrementIndex();
   }
   patternWidget->show();
   return patternWidget;
}
