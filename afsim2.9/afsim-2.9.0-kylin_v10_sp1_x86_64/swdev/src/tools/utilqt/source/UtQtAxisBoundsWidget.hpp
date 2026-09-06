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

#ifndef UT_QT_AXIS_BOUNDS_WIDGET_HPP
#define UT_QT_AXIS_BOUNDS_WIDGET_HPP

#include "utilqt_export.h"

#include <QGroupBox>

#include "ui_UtQtAxisBounds.h"

class UtQtGL2DPlot;

namespace ut
{
namespace qt
{
//! AxisBoundsWidget is a class designed to work with UtQtGL2DPlot to set the x and y axes.
//! The user may select between 2 or 3 types of axis controls:
//!    1) Auto:  Simply calls MakePlotPretty().
//!    2) Value: Allows the user to set min and max values.
//!              These values are passed to MakePlotPretty(...).
//!    3) Range: Allows the user to set a single value.
//!              The passes (max_data - range) and (max_data) to MakePlotPretty(...).
//!              This mode is only available for the X Axis.
//!
//! A separate AxisBoundsWidget is required for each axis.
//! Be sure to call Set???AxisControls() after construction. For some reason, calling them from the constructor does not work.
class UTILQT_EXPORT AxisBoundsWidget final : public QGroupBox
{
   Q_OBJECT

public:
   //! After construction, users should call Set__AxisControls()).
   //! For some reason, calling it inside the constructor does not work.
   AxisBoundsWidget(QWidget* aParent) noexcept;

   //! Sets the name of the group box to "X Axis".
   //! Ensures "Range" option is visible.
   void SetXAxisControls() noexcept;

   //! Sets the name of the group box to "Y Axis".
   //! Ensures "Range" option is not visible.
   void SetYAxisControls() noexcept;

   //! Sets the name of the group box to "Y2 Axis".
   //! Ensures "Range" option is not visible.
   void SetYAxisControls2() noexcept;

   //! Sets the range values that Min and Max can be.
   //! Range can be at most aHigh - aLow.
   //! E.g. SetValidRange(0, 100) means Min and Max must be somewhere between 0 and 100 inclusive.
   //! If aHigh <= aLow, does nothing.
   //! If Min or Max are outside this range, moves them inside.
   //! By default, range is [0, 999999].
   void SetValidRange(double aMinumum, double aMaximum) noexcept;

   //! Sets the current minimum and maximum values.
   //! Does not set the mode.
   //! @{
   void SetMin(double aMinimum) noexcept;
   void SetMax(double aMaximum) noexcept;
   void SetMinMax(double aMinimum, double aMaximum) noexcept;
   //! @}

   //! Sets the current range.
   //! Does not set the mode.
   void SetRange(double aRange) noexcept;

   //! Sets the current mode.
   //! If aSet == true, sets to named mode.
   //! If aSet == false, sets mode to next valid mode.
   //! @{
   void SetAutoMode(bool aSet = true) noexcept;
   void SetValueMode(bool aSet = true) noexcept;
   void SetRangeMode(bool aSet = true) noexcept;
   //! @}

   //! Returns the valid min/max range.
   //! @{
   double GetMinValid() const noexcept;
   double GetMaxValid() const noexcept;
   //! @}

   //! Returns the max valid Range value.
   double GetValidRange() const noexcept;

   //! Returns the current settings.
   //! These do not take into account the current mode.
   //! @{
   double GetMin() const noexcept;
   double GetMax() const noexcept;

   double GetRange() const noexcept;
   //! @}

   //! Poll about the current mode.
   //! @{
   bool IsAutoMode() const noexcept;
   bool IsValueMode() const noexcept;
   bool IsRangeMode() const noexcept;
   //! @}

   //! Sets the X or Y axis bounds of the plot based on the current settings.
   //! @{
   void SetXAxisBounds(UtQtGL2DPlot& aPlot);
   void SetYAxisBounds(UtQtGL2DPlot& aPlot);
   void SetYAxisBounds2(UtQtGL2DPlot& aPlot);
   //! @}

signals:
   //! Signal emitted when min/max changes by user interaction
   //!    AND widget is in Value mode.
   void MinMaxEdited(double aMinimum, double aMaximum);

   //! Signal emitten when range changes by user interaction
   //!    AND widget is in Range mode.
   void RangeEdited(double aRange);

   //! Signal emitted when the mode changes.
   void ModeChanged();

   //! Catch-all signal emitted whenever the bounds change.
   void BoundsChanged();

private:
   void MinimumChanged(double aValue);
   void MaximumChanged(double aValue);

   void RangeChanged(double aValue);

   bool NoModeSelected() const noexcept;

   Ui::UtQtAxisBoundsWidget mUi;
};
} // namespace qt
} // namespace ut

#endif
