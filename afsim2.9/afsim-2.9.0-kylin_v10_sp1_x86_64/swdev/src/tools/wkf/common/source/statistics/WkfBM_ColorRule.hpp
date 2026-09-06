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

#ifndef WKF_BM_COLOR_RULE_HPP
#define WKF_BM_COLOR_RULE_HPP

#include "wkf_common_export.h"

#include <QDoubleValidator>
#include <QIntValidator>

#include "ui_WkfBM_ColorRule.h"

namespace wkf
{
namespace bm
{
struct ColorData;

class WKF_COMMON_EXPORT ColorRule : public QWidget
{
   Q_OBJECT

public:
   enum class Mode
   {
      Boolean,
      Number,
      Text //!< Case insensitive
   };

   ColorRule(QWidget* aParent, Mode aMode) noexcept;

   //! Returns the color in the rule.
   QColor GetColor() const noexcept;

   //! Sets the current settings for the value and comparison.
   //! This function will not change the mode.
   //! While in Mode::Boolean, aComparison is not used.
   void SetState(QColor aColor, const QString& aComparison, const QVariant& aValue);

   //! Returns the value in the line edit.
   //! The RuleSet gets an opportunity to transform it before calling TestValue.
   QString GetUserValue() const noexcept;

   //! Returns true if the value meets the conditions of the rule.
   //! This includes being of the required type.
   //! aUserValue is not used in Boolean mode.
   bool TestValue(const QVariant& aValue, const QVariant& aUserValue) const noexcept;

   //! Returns the rule's current mode.
   Mode GetMode() const noexcept;

   //! Sets the rule's current mode.
   //!  * Boolean: combo box disabled. showing check box.
   //!  * Integer & Numerical: combo box contains ==, !=, <, <=, >, >=. showing line edit.
   //!  * Text: combo box contains ==, !=. showing line edit.
   void SetMode(Mode aMode) noexcept;

   //! Reads or writes a rule to/from a QSettings object.
   //! @{
   ColorData ExportState() const;
   void      ImportState(const ColorData& aColor);
   //! @}

   void SetCustomValidator(const QValidator* aValidator);

signals:
   void RemoveRule(ColorRule* aRule);
   void InsertRule(ColorRule* aRule, bool aBefore);
   void RuleChanged();

private:
   void ContextMenu(const QPoint& aPoint);

   //! Returns 0 if empty or invalid.
   double GetDouble() const noexcept;

   bool TestBoolean(bool aValue) const noexcept;
   bool TestNumber(double aValue, double aUserValue) const noexcept;
   bool TestString(const QString& aValue, const QString& aUserValue) const noexcept;

   Ui::WkfBM_ColorRule mUi;
   QDoubleValidator    mNumberValidator;
   Mode                mMode;
   bool                mBoolTrue = true;
};
} // namespace bm
} // namespace wkf

#endif
