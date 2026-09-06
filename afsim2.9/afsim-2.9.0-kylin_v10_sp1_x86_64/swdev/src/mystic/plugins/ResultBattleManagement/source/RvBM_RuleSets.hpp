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

#ifndef RV_BM_RULE_SETS_HPP
#define RV_BM_RULE_SETS_HPP

#include "statistics/WkfBM_ResourceRuleSet.hpp"
#include "statistics/WkfBM_RuleSet.hpp"

namespace RvBM
{
class RuleSetDamageFactor final : public wkf::bm::RuleSet
{
public:
   explicit RuleSetDamageFactor(QWidget* aParent) noexcept;
   QVariant GetValue(const wkf::Platform* aPlatform) const override;
   void     SetDefaults() override;
};

class RuleSetFuel final : public wkf::bm::ResourceRuleSet
{
public:
   explicit RuleSetFuel(QWidget* aParent) noexcept;
   QVariant             GetValue(const wkf::Platform* aPlatform) const override;
   ut::optional<double> GetInitialValue(const wkf::Platform* aPlatform) const override;
};

class RuleSetWeaponCount final : public wkf::bm::ResourceRuleSet
{
public:
   explicit RuleSetWeaponCount(QWidget* aParent) noexcept;
   QVariant             GetValue(const wkf::Platform* aPlatform) const override;
   ut::optional<double> GetInitialValue(const wkf::Platform* aPlatform) const override;
};

class AuxDataRuleSet final : public wkf::bm::RuleSet
{
public:
   AuxDataRuleSet(QWidget* aParent, QString aKey, wkf::bm::RuleSet::Mode aMode) noexcept;

   QString  GetDisplayLabel() const override;
   QVariant GetValue(const wkf::Platform* aPlatform) const override;
   QString  GetDisplayValue(const wkf::Platform* aPlatform) const override;
   void     SetDefaults() override;

private:
   std::string mKey;
};

class AuxDataResourceRuleSet final : public wkf::bm::ResourceRuleSet
{
public:
   AuxDataResourceRuleSet(QWidget* aParent, QString aNumerator, QString aDenominator) noexcept;

   QString              GetDisplayLabel() const override;
   QVariant             GetValue(const wkf::Platform* aPlatform) const override;
   ut::optional<double> GetInitialValue(const wkf::Platform* aPlatform) const override;

private:
   std::string mNumeratorKey;
   std::string mDenominatorKey;
};
} // namespace RvBM

#endif
