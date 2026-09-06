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

#include "UtQtAxisBoundsWidget.hpp"

#include "UtQtGL2DPlot.hpp"

ut::qt::AxisBoundsWidget::AxisBoundsWidget(QWidget* aParent) noexcept
   : QGroupBox(aParent)
{
   mUi.setupUi(this);
   SetAutoMode();

   SetMin(0);
   SetMax(60);
   SetRange(60);

   connect(mUi.autoCheckBox, &QCheckBox::clicked, this, &AxisBoundsWidget::SetAutoMode);
   connect(mUi.minMaxGroupBox, &QGroupBox::clicked, this, &AxisBoundsWidget::SetValueMode);
   connect(mUi.rangeGroupBox, &QGroupBox::clicked, this, &AxisBoundsWidget::SetRangeMode);

   const auto sig = QOverload<double>::of(&QDoubleSpinBox::valueChanged);

   connect(mUi.minSpinBox, sig, this, &AxisBoundsWidget::MinimumChanged);
   connect(mUi.maxSpinBox, sig, this, &AxisBoundsWidget::MaximumChanged);
   connect(mUi.rangeSpinBox, sig, this, &AxisBoundsWidget::RangeChanged);
}

void ut::qt::AxisBoundsWidget::SetXAxisControls() noexcept
{
   mUi.rangeGroupBox->setVisible(true);
   setTitle("X Axis");
}

void ut::qt::AxisBoundsWidget::SetYAxisControls() noexcept
{
   mUi.rangeGroupBox->setVisible(false);
   if (IsRangeMode())
   {
      SetAutoMode();
   }

   setTitle("Y Axis");
}

void ut::qt::AxisBoundsWidget::SetYAxisControls2() noexcept
{
   mUi.rangeGroupBox->setVisible(false);
   if (IsRangeMode())
   {
      SetAutoMode();
   }

   setTitle("Y2 Axis");
}

void ut::qt::AxisBoundsWidget::SetValidRange(double aMinimum, double aMaximum) noexcept
{
   if (aMinimum < aMaximum)
   {
      if (aMinimum >= GetMaxValid())
      {
         mUi.maxSpinBox->setMaximum(aMaximum);
         mUi.minSpinBox->setMinimum(aMinimum);
      }
      else
      {
         mUi.minSpinBox->setMinimum(aMinimum);
         mUi.maxSpinBox->setMaximum(aMaximum);
      }

      mUi.rangeSpinBox->setMaximum(aMaximum - aMinimum);
      emit BoundsChanged();
   }
}

void ut::qt::AxisBoundsWidget::SetMin(double aMinimum) noexcept
{
   mUi.minSpinBox->setValue(aMinimum);
   mUi.maxSpinBox->setMinimum(aMinimum);
   emit BoundsChanged();
}

void ut::qt::AxisBoundsWidget::SetMax(double aMaximum) noexcept
{
   mUi.maxSpinBox->setValue(aMaximum);
   mUi.minSpinBox->setMaximum(aMaximum);
   emit BoundsChanged();
}

void ut::qt::AxisBoundsWidget::SetMinMax(double aMinimum, double aMaximum) noexcept
{
   if (aMinimum < aMaximum && aMinimum >= GetMinValid() && aMaximum <= GetMaxValid())
   {
      mUi.minSpinBox->setValue(GetMinValid());
      mUi.maxSpinBox->setMinimum(GetMinValid());

      mUi.maxSpinBox->setValue(aMaximum);
      mUi.minSpinBox->setMaximum(aMaximum);

      mUi.minSpinBox->setValue(aMinimum);
      mUi.maxSpinBox->setMinimum(aMinimum);

      emit BoundsChanged();
   }
}

void ut::qt::AxisBoundsWidget::SetRange(double aRange) noexcept
{
   mUi.rangeSpinBox->setValue(aRange);
   emit BoundsChanged();
}

void ut::qt::AxisBoundsWidget::SetAutoMode(bool aSet) noexcept
{
   if (aSet)
   {
      mUi.autoCheckBox->setChecked(true);
      mUi.minMaxGroupBox->setChecked(false);
      mUi.rangeGroupBox->setChecked(false);

      emit ModeChanged();
      emit BoundsChanged();
   }
   else if (IsAutoMode() || NoModeSelected())
   {
      SetValueMode();
   }
}

void ut::qt::AxisBoundsWidget::SetValueMode(bool aSet) noexcept
{
   if (aSet)
   {
      mUi.autoCheckBox->setChecked(false);
      mUi.minMaxGroupBox->setChecked(true);
      mUi.rangeGroupBox->setChecked(false);

      emit ModeChanged();
      emit BoundsChanged();
   }
   else if (IsValueMode() || NoModeSelected())
   {
      SetAutoMode();
   }
}

void ut::qt::AxisBoundsWidget::SetRangeMode(bool aSet) noexcept
{
   if (aSet && mUi.rangeGroupBox->isVisible())
   {
      mUi.autoCheckBox->setChecked(false);
      mUi.minMaxGroupBox->setChecked(false);
      mUi.rangeGroupBox->setChecked(true);

      emit ModeChanged();
      emit BoundsChanged();
   }
   else if (IsRangeMode() || NoModeSelected())
   {
      SetAutoMode();
   }
}

double ut::qt::AxisBoundsWidget::GetMinValid() const noexcept
{
   return mUi.minSpinBox->minimum();
}

double ut::qt::AxisBoundsWidget::GetMaxValid() const noexcept
{
   return mUi.maxSpinBox->maximum();
}

double ut::qt::AxisBoundsWidget::GetValidRange() const noexcept
{
   return mUi.rangeSpinBox->maximum();
}

double ut::qt::AxisBoundsWidget::GetMin() const noexcept
{
   return mUi.minSpinBox->value();
}

double ut::qt::AxisBoundsWidget::GetMax() const noexcept
{
   return mUi.maxSpinBox->value();
}

double ut::qt::AxisBoundsWidget::GetRange() const noexcept
{
   return mUi.rangeSpinBox->value();
}

bool ut::qt::AxisBoundsWidget::IsAutoMode() const noexcept
{
   return mUi.autoCheckBox->isChecked();
}

bool ut::qt::AxisBoundsWidget::IsValueMode() const noexcept
{
   return mUi.minMaxGroupBox->isChecked();
}

bool ut::qt::AxisBoundsWidget::IsRangeMode() const noexcept
{
   return mUi.rangeGroupBox->isChecked();
}

void ut::qt::AxisBoundsWidget::SetXAxisBounds(UtQtGL2DPlot& aPlot)
{
   if (IsValueMode())
   {
      aPlot.MakePlotPrettyX(GetMin(), GetMax());
   }
   else if (IsRangeMode())
   {
      double dummy, maxX;
      aPlot.GetDataMinMax(dummy, dummy, maxX, dummy);
      aPlot.MakePlotPrettyX(maxX - GetRange(), maxX);
   }
   else
   {
      aPlot.MakePlotPrettyX();
   }
}

void ut::qt::AxisBoundsWidget::SetYAxisBounds(UtQtGL2DPlot& aPlot)
{
   if (IsValueMode())
   {
      // Unfortunately, UtQtGL2DPlot does not have a MakePlotPretty that only affects one of the Y axes.
      double min2, max2;
      aPlot.GetYAxisBounds2(min2, max2);
      aPlot.MakePlotPrettyY(GetMin(), GetMax(), min2, max2);
   }
   else
   {
      aPlot.MakePlotPrettyY();
   }
}

void ut::qt::AxisBoundsWidget::SetYAxisBounds2(UtQtGL2DPlot& aPlot)
{
   if (IsValueMode())
   {
      // Unfortunately, UtQtGL2DPlot does not have a MakePlotPretty that only affects one of the Y axes.
      double min1, max1;
      aPlot.GetYAxisBounds(min1, max1);
      aPlot.MakePlotPrettyY(min1, max1, GetMin(), GetMax());
   }
   else
   {
      aPlot.MakePlotPrettyY();
   }
}

void ut::qt::AxisBoundsWidget::MinimumChanged(double aValue)
{
   SetMin(aValue);
   if (IsValueMode())
   {
      emit MinMaxEdited(GetMin(), GetMax());
   }
}

void ut::qt::AxisBoundsWidget::MaximumChanged(double aValue)
{
   SetMax(aValue);
   if (IsValueMode())
   {
      emit MinMaxEdited(GetMin(), GetMax());
   }
}

void ut::qt::AxisBoundsWidget::RangeChanged(double aValue)
{
   SetRange(aValue);
   if (IsRangeMode())
   {
      emit RangeEdited(GetRange());
   }
}

bool ut::qt::AxisBoundsWidget::NoModeSelected() const noexcept
{
   return !(IsAutoMode() || IsValueMode() || IsRangeMode());
}
