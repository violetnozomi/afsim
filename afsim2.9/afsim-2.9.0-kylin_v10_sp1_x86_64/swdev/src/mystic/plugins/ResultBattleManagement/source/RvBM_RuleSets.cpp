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

#include "RvBM_RuleSets.hpp"

#include "RvEnvironment.hpp"
#include "RvEventPipeClasses.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "WkfPlatform.hpp"
#include "statistics/WkfBM_PlatformStatus.hpp"

static rv::ResultPlatform* GetRvPlatform(const wkf::Platform* aPlatform) noexcept
{
   auto* results = rvEnv.GetData();
   if (results)
   {
      auto* db = results->GetDb();
      if (db)
      {
         return db->FindPlatform(aPlatform->GetIndex());
      }
   }
   return nullptr;
}

static float GetRvSimTime() noexcept
{
   auto* results = rvEnv.GetData();
   if (results)
   {
      return results->GetSimTime();
   }
   return 0;
}

static QVariant GetAuxDataValue(const wkf::Platform* aPlatform, const std::string& aKey, wkf::bm::ColorRule::Mode aMode)
{
   auto* platform = GetRvPlatform(aPlatform);
   if (platform)
   {
      const auto* msg = platform->FindFirstBefore<rv::MsgAuxData>(GetRvSimTime());
      if (msg)
      {
         for (const auto& data : msg->auxData())
         {
            if (data.name() == aKey)
            {
               auto type = data.type();

               if (type == rv::AuxDataType::Boolean && aMode == wkf::bm::ColorRule::Mode::Boolean)
               {
                  return data.boolean();
               }
               else if (type == rv::AuxDataType::Integer && aMode == wkf::bm::ColorRule::Mode::Number)
               {
                  return data.integer();
               }
               else if (type == rv::AuxDataType::Real && aMode == wkf::bm::ColorRule::Mode::Number)
               {
                  return data.real();
               }
               else if (type == rv::AuxDataType::Text && aMode == wkf::bm::ColorRule::Mode::Text)
               {
                  return QString::fromStdString(data.text());
               }
               else
               {
                  return QVariant{};
               }
            }
         }
      }
   }
   return QVariant{};
}

RvBM::RuleSetDamageFactor::RuleSetDamageFactor(QWidget* aParent) noexcept
   : wkf::bm::RuleSet(aParent, Mode::Number, "Damage Factor")
{
}

QVariant RvBM::RuleSetDamageFactor::GetValue(const wkf::Platform* aPlatform) const
{
   auto* platform = GetRvPlatform(aPlatform);
   if (platform)
   {
      const auto* state = platform->FindFirstBefore<rv::MsgEntityState>(GetRvSimTime());
      if (state)
      {
         return state->state().damageFactor();
      }
   }
   return QVariant();
}

void RvBM::RuleSetDamageFactor::SetDefaults()
{
   ClearRules();
   SetDefaultColor(ut::nullopt);

   AddRule()->SetState(QColor(0, 128, 0), "==", 0);
   AddRule()->SetState(QColor(128, 128, 0), "<", 1);
   AddRule()->SetState(QColor(128, 0, 0), "==", 1);
}

RvBM::RuleSetFuel::RuleSetFuel(QWidget* aParent) noexcept
   : wkf::bm::ResourceRuleSet(aParent, "Fuel")
{
}

QVariant RvBM::RuleSetFuel::GetValue(const wkf::Platform* aPlatform) const
{
   auto* platform = GetRvPlatform(aPlatform);
   if (platform)
   {
      const auto* msg = platform->FindFirstBefore<rv::MsgEntityState>(GetRvSimTime());
      if (msg && msg->state().fuelCurrentValid())
      {
         return msg->state().fuelCurrent();
      }
   }
   return QVariant();
}

ut::optional<double> RvBM::RuleSetFuel::GetInitialValue(const wkf::Platform* aPlatform) const
{
   auto* platform = GetRvPlatform(aPlatform);
   if (platform)
   {
      const auto* msg = platform->FindFirstAfter<rv::MsgEntityState>(-1);
      if (msg && msg->state().fuelCurrentValid())
      {
         return msg->state().fuelCurrent();
      }
   }
   return ut::nullopt;
}

RvBM::RuleSetWeaponCount::RuleSetWeaponCount(QWidget* aParent) noexcept
   : wkf::bm::ResourceRuleSet(aParent, "Weapon Count")
{
}

QVariant RvBM::RuleSetWeaponCount::GetValue(const wkf::Platform* aPlatform) const
{
   auto* platform = GetRvPlatform(aPlatform);
   if (platform)
   {
      // TODO: Find weapons
   }
   return QVariant();
}

ut::optional<double> RvBM::RuleSetWeaponCount::GetInitialValue(const wkf::Platform* aPlatform) const
{
   auto* platform = GetRvPlatform(aPlatform);
   if (platform)
   {
      // TODO: Find weapons
   }
   return ut::nullopt;
}

RvBM::AuxDataRuleSet::AuxDataRuleSet(QWidget* aParent, QString aKey, wkf::bm::RuleSet::Mode aMode) noexcept
   : wkf::bm::RuleSet(aParent, aMode, wkf::bm::cAUX_DATA_PREFIX + aKey, true)
   , mKey(aKey.toStdString())
{
}

QString RvBM::AuxDataRuleSet::GetDisplayLabel() const
{
   return GetLabel().mid(wkf::bm::cAUX_DATA_PREFIX.size());
}

QVariant RvBM::AuxDataRuleSet::GetValue(const wkf::Platform* aPlatform) const
{
   return GetAuxDataValue(aPlatform, mKey, GetMode());
}

QString RvBM::AuxDataRuleSet::GetDisplayValue(const wkf::Platform* aPlatform) const
{
   auto value = GetValue(aPlatform);
   if (value.isValid())
   {
      if (GetMode() == Mode::Number)
      {
         return QString::number(value.toDouble(), 'f', 4);
      }
      else
      {
         return value.toString();
      }
   }
   return "n/a";
}

void RvBM::AuxDataRuleSet::SetDefaults()
{
   ClearRules();
   SetDefaultColor(ut::nullopt);

   if (GetMode() == Mode::Boolean)
   {
      AddRule()->SetState(QColor(0, 128, 0), QString{}, true);
      AddRule()->SetState(QColor(128, 0, 0), QString{}, false);
   }
}

RvBM::AuxDataResourceRuleSet::AuxDataResourceRuleSet(QWidget* aParent, QString aNumerator, QString aDenominator) noexcept
   : wkf::bm::ResourceRuleSet(aParent, wkf::bm::cAUX_DATA_PREFIX + aNumerator + " / " + aDenominator, true)
   , mNumeratorKey(aNumerator.toStdString())
   , mDenominatorKey(aDenominator.toStdString())
{
}

QString RvBM::AuxDataResourceRuleSet::GetDisplayLabel() const
{
   return GetLabel().mid(wkf::bm::cAUX_DATA_PREFIX.size());
}

QVariant RvBM::AuxDataResourceRuleSet::GetValue(const wkf::Platform* aPlatform) const
{
   return GetAuxDataValue(aPlatform, mNumeratorKey, GetMode());
}

ut::optional<double> RvBM::AuxDataResourceRuleSet::GetInitialValue(const wkf::Platform* aPlatform) const
{
   auto value = GetAuxDataValue(aPlatform, mDenominatorKey, GetMode());

   if (value.isValid())
   {
      return value.toDouble();
   }
   return ut::nullopt;
}
