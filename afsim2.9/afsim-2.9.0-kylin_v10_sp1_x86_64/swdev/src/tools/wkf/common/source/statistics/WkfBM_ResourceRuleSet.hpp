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

#ifndef WKF_BM_COMMON_RULE_SETS_HPP
#define WKF_BM_COMMON_RULE_SETS_HPP

#include "UtOptional.hpp"
#include "WkfBM_RuleSet.hpp"

namespace wkf
{
namespace bm
{
//! Defines a rule set for a limited resource, such as fuel or weapons.
//! Values are displayed as "absolute (remaining%)".
//! The color rules may
class WKF_COMMON_EXPORT ResourceRuleSet : public wkf::bm::RuleSet
{
public:
   ResourceRuleSet(QWidget* aParent, QString aTitle, bool aRemovable = false) noexcept;

   //! Returns the initial value of the resource.
   //! If invalid, return an empty optional.
   virtual ut::optional<double> GetInitialValue(const wkf::Platform* aPlatform) const = 0;

   QString      GetDisplayValue(const wkf::Platform* aPlatform) const final;
   void         SetDefaults() final;
   QVariant     TransformUserValue(const QString& aUserValue) const final;
   ColorSetting GetColor(const wkf::Platform* aPlatform) const final;
   void         RuleAdded(wkf::bm::ColorRule* aRule) override;

private:
   struct Validator final : QValidator
   {
      State validate(QString& aInput, int& aPos) const override;
   } mValidator;
   mutable ut::optional<double> mInitial;
};
} // namespace bm
} // namespace wkf

#endif
