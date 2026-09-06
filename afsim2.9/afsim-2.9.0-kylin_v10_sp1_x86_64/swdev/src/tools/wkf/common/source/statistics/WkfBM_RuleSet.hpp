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

#ifndef WKF_BM_RULE_CONTAINER_HPP
#define WKF_BM_RULE_CONTAINER_HPP

#include "wkf_common_export.h"

#include <vector>

#include "WkfBM_ColorRule.hpp"
#include "WkfBM_ModelUtils.hpp"
#include "ui_WkfBM_RuleSet.h"

namespace wkf
{
class Platform;
namespace bm
{
struct ColumnData;

//! RuleSet contains the application logic and visual representation of the colors used to color a PlatformStatus column.
class WKF_COMMON_EXPORT RuleSet : public QWidget
{
   Q_OBJECT

public:
   using Mode = ColorRule::Mode;

   RuleSet(QWidget* aParent, Mode aMode, QString aLabel, bool aRemovable = false) noexcept;

   //! Can be overridden to lookup the value from a platform.
   virtual QVariant GetValue(const wkf::Platform* aPlatform) const = 0;

   //! Returns the value to display on the screen. By default, same as GetValue().toString().
   virtual QString GetDisplayValue(const wkf::Platform* aPlatform) const;

   //! Can be overridden to set the default colors.
   virtual void SetDefaults() = 0;

   //! Transforms the value from the ColorRule before passing it to TestValue.
   //! By default, does nothing.
   virtual QVariant TransformUserValue(const QString& aUserValue) const;

   //! Returns the color that a value would be assigned.
   virtual ColorSetting GetColor(const wkf::Platform* aPlatform) const;

   //! Returns the default color for the column.
   //! By default, uses an invalid/clear color.
   ColorSetting GetDefaultColor() const noexcept;

   //! Resets the default color to an invalid/clear color.
   void SetDefaultColor(ColorSetting aColor) noexcept;

   //! Adds a new rule at the end.
   ColorRule* AddRule();

   //! Adds a new rule at the specified index.
   ColorRule* InsertRule(int aIndex);

   //! Called when a ColorRule is inserted.
   virtual void RuleAdded(ColorRule* aRule){};

   //! Erases all existing rules.
   //! Does not affect default color.
   void ClearRules();

   //! Returns the number of rules.
   int RuleCount() const noexcept;

   //! Returns what type of value the rule set operates on.
   Mode GetMode() const noexcept;

   //! Returns the name of the rule set.
   const QString& GetLabel() const noexcept;

   //! Returns the string to be used as the column title.
   virtual QString GetDisplayLabel() const { return GetLabel(); }

   //! Saves the current RuleSet state to ColumnData.
   ColumnData ExportState() const;

   //! Resets the current column state to match aColumn.
   void ImportState(const ColumnData& aColumn);

   //! Returns true if the "Remove" button is enabled.
   bool IsRemovable() const noexcept;

   //! Makes the Show button visible.
   void EnableShowButton() noexcept;

   //! Ensures the column is visible.
   void ShowColumn();

signals:
   void RulesChanged(RuleSet* aRuleSet); // aRuleSet will always be this.
   void ShowClicked(RuleSet* aRuleSet);  // aRuleSet will always be this.

private:
   //! Called in the constructor.
   void InitializeDefaultColorButton();
   //! Called when a rule's remove button is clicked.
   void RemoveRuleCB(ColorRule* aRule);
   void InsertRuleCB(ColorRule* aRule, bool aBefore);

   Ui::WkfBM_RuleSet       mUi;
   const Mode              mMode;
   QString                 mLabel;
   std::vector<ColorRule*> mRules;
   ColorSetting            mDefaultColor;
   bool                    mRemovable = false;
};
} // namespace bm
} // namespace wkf

#endif
