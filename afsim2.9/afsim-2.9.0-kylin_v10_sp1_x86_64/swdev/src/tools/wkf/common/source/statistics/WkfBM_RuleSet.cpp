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

#include "WkfBM_RuleSet.hpp"

#include <QAction>

#include "UtQtMemory.hpp"
#include "WkfBM_ColorRule.hpp"
#include "WkfBM_PlatformStatusPrefObject.hpp"
#include "misc/WkfExportHelper.hpp"

wkf::bm::RuleSet::RuleSet(QWidget* aParent, Mode aMode, QString aLabel, bool aRemovable /*= false*/) noexcept
   : QWidget(aParent)
   , mMode(aMode)
   , mLabel(std::move(aLabel))
   , mRemovable(aRemovable)
{
   mUi.setupUi(this);
   mUi.defaultColorButton->setAutoDefault(false);
   mUi.addColorButton->setAutoDefault(false);
   mUi.showButton->setAutoDefault(false);
   mUi.showButton->setHidden(true);

   InitializeDefaultColorButton();

   if (mMode == Mode::Boolean)
   {
      mUi.addColorButton->setVisible(false);
      mUi.defaultColorLabel->setVisible(false);
      mUi.defaultColorButton->setVisible(false);
   }

   connect(mUi.addColorButton, &QPushButton::clicked, this, &RuleSet::AddRule);
   connect(mUi.showButton, &QPushButton::clicked, this, &RuleSet::ShowColumn);
}

QString wkf::bm::RuleSet::GetDisplayValue(const wkf::Platform* aPlatform) const
{
   auto value = GetValue(aPlatform);
   if (value.isValid())
   {
      return value.toString();
   }
   return "n/a";
}

QVariant wkf::bm::RuleSet::TransformUserValue(const QString& aUserValue) const
{
   return aUserValue;
}

wkf::bm::ColorSetting wkf::bm::RuleSet::GetColor(const wkf::Platform* aPlatform) const
{
   auto value = GetValue(aPlatform);
   if (value.isValid())
   {
      for (const auto* rule : mRules)
      {
         if (rule->TestValue(value, TransformUserValue(rule->GetUserValue())))
         {
            return rule->GetColor();
         }
      }
      return mDefaultColor;
   }
   return ut::nullopt;
}

wkf::bm::ColorSetting wkf::bm::RuleSet::GetDefaultColor() const noexcept
{
   return mDefaultColor;
}

void wkf::bm::RuleSet::SetDefaultColor(wkf::bm::ColorSetting aColor) noexcept
{
   QSignalBlocker blocker(mUi.defaultColorButton);
   mUi.defaultColorButton->SetColor(aColor.value_or(QColor(0, 0, 0, 0)));
   mDefaultColor = aColor;
}

wkf::bm::ColorRule* wkf::bm::RuleSet::AddRule()
{
   return InsertRule(RuleCount());
}

wkf::bm::ColorRule* wkf::bm::RuleSet::InsertRule(int aIndex)
{
   auto it = mRules.begin() + aIndex;

   auto rule = ut::qt::make_qt_ptr<ColorRule>(this, mMode);
   mRules.emplace(it, rule);

   RuleAdded(rule);

   mUi.rulesLayout->insertWidget(aIndex, rule);

   connect(rule, &ColorRule::RuleChanged, [this]() { RulesChanged(this); });
   connect(rule, &ColorRule::RemoveRule, this, &RuleSet::RemoveRuleCB);
   connect(rule, &ColorRule::InsertRule, this, &RuleSet::InsertRuleCB);

   emit RulesChanged(this);

   return rule;
}

void wkf::bm::RuleSet::ClearRules()
{
   int initialCount = RuleCount();
   for (int i = 0; i < initialCount; i++)
   {
      auto* item = mUi.rulesLayout->takeAt(0);
      delete item->widget();
      delete item;
   }
   mRules.clear();

   emit RulesChanged(this);
}

int wkf::bm::RuleSet::RuleCount() const noexcept
{
   return static_cast<int>(mRules.size());
}

wkf::bm::RuleSet::Mode wkf::bm::RuleSet::GetMode() const noexcept
{
   return mMode;
}

const QString& wkf::bm::RuleSet::GetLabel() const noexcept
{
   return mLabel;
}

wkf::bm::ColumnData wkf::bm::RuleSet::ExportState() const
{
   ColumnData retval;

   retval.mName         = GetLabel();
   retval.mMode         = GetMode();
   retval.mDefaultColor = GetDefaultColor();
   retval.mHidden       = !mUi.showButton->isHidden();
   for (const auto* colorRule : mRules)
   {
      retval.mColors.append(colorRule->ExportState());
   }

   return retval;
}

void wkf::bm::RuleSet::ImportState(const ColumnData& aColumn)
{
   ClearRules();
   SetDefaultColor(aColumn.mDefaultColor);

   // Do nothing with name, mode, or hidden fields.

   for (const auto& color : aColumn.mColors)
   {
      AddRule()->ImportState(color);
   }
}

bool wkf::bm::RuleSet::IsRemovable() const noexcept
{
   return mRemovable;
}

void wkf::bm::RuleSet::EnableShowButton() noexcept
{
   mUi.showButton->setHidden(false);
}

void wkf::bm::RuleSet::ShowColumn()
{
   mUi.showButton->setHidden(true);
   emit ShowClicked(this);
}

void wkf::bm::RuleSet::InitializeDefaultColorButton()
{
   connect(mUi.defaultColorButton,
           &UtQtColorButton::ColorChanged,
           this,
           [this](const QColor& aColor)
           {
              mDefaultColor = aColor;
              emit RulesChanged(this);
           });

   auto noDefaultAction = new QAction("No Default", mUi.defaultColorButton);
   connect(noDefaultAction, &QAction::triggered, this, [this]() { this->SetDefaultColor(ut::nullopt); });
   mUi.defaultColorButton->insertAction(nullptr, noDefaultAction);

   SetDefaultColor(ut::nullopt);
}

void wkf::bm::RuleSet::RemoveRuleCB(ColorRule* aRule)
{
   aRule->hide();
   aRule->deleteLater();
   mUi.rulesLayout->removeWidget(aRule);

   for (auto it = mRules.begin(); it != mRules.end(); it++)
   {
      if (*it == aRule)
      {
         mRules.erase(it);
         break;
      }
   }
}

void wkf::bm::RuleSet::InsertRuleCB(ColorRule* aRule, bool aBefore)
{
   const int size = static_cast<int>(mRules.size());
   for (int i = 0; i < size; i++)
   {
      if (mRules[i] == aRule)
      {
         InsertRule(aBefore ? i : i + 1);
         emit RulesChanged(this);
         return;
      }
   }
}
