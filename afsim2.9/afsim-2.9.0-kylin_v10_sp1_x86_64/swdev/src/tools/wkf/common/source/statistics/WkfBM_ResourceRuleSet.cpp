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

#include "WkfBM_ResourceRuleSet.hpp"

wkf::bm::ResourceRuleSet::ResourceRuleSet(QWidget* aParent, QString aTitle, bool aRemovable /*= false*/) noexcept
   : wkf::bm::RuleSet(aParent, Mode::Number, aTitle, aRemovable)
{
}

QString wkf::bm::ResourceRuleSet::GetDisplayValue(const wkf::Platform* aPlatform) const
{
   auto value = GetValue(aPlatform);
   mInitial   = GetInitialValue(aPlatform);

   if (mInitial.value_or(0) == 0)
   {
      mInitial.reset();
   }

   if (mInitial && value.isValid())
   {
      return QString{"%1 (%2%)"}.arg(value.toDouble(), 0, 'f', 2).arg(100 * value.toDouble() / *mInitial, 0, 'f', 2);
   }

   return "n/a";
}

void wkf::bm::ResourceRuleSet::SetDefaults()
{
   ClearRules();
   SetDefaultColor(ut::nullopt);

   AddRule()->SetState(QColor(0, 128, 0), ">", "50%");
   AddRule()->SetState(QColor(128, 128, 0), ">", "0%");
   AddRule()->SetState(QColor(128, 0, 0), "==", "0%");
}

QVariant wkf::bm::ResourceRuleSet::TransformUserValue(const QString& aUserValue) const
{
   if (aUserValue.endsWith("%"))
   {
      if (mInitial)
      {
         QString copy = aUserValue;
         copy.chop(1);
         return copy.toDouble() * *mInitial / 100;
      }
      return QVariant{};
   }
   return aUserValue.toDouble();
}

wkf::bm::ColorSetting wkf::bm::ResourceRuleSet::GetColor(const wkf::Platform* aPlatform) const
{
   if (GetInitialValue(aPlatform).value_or(0) > 0)
   {
      return RuleSet::GetColor(aPlatform);
   }
   return ut::nullopt;
}

void wkf::bm::ResourceRuleSet::RuleAdded(wkf::bm::ColorRule* aRule)
{
   aRule->SetCustomValidator(&mValidator);
}

QValidator::State wkf::bm::ResourceRuleSet::Validator::validate(QString& aInput, int& aPos) const
{
   bool ok = false;
   if (aInput.isEmpty())
   {
      return State::Intermediate;
   }
   else if (aInput.endsWith("%"))
   {
      QString copy = aInput;
      copy.chop(1);
      copy.toDouble(&ok);
   }
   else
   {
      aInput.toDouble(&ok);
   }

   return ok ? State::Acceptable : State::Invalid;
}
