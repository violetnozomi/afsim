// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtQtGradientLegend.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

UtQtGradientLegend::UtQtGradientLegend(QWidget* aParent)
   : QWidget(aParent)
{
   mUi.setupUi(this);
   mTickLabels.append(mUi.tickLabel1);
   mTickLabels.append(mUi.tickLabel2);
   mTickLabels.append(mUi.tickLabel3);
   mTickLabels.append(mUi.tickLabel4);
   mTickLabels.append(mUi.tickLabel5);
   mTickLabels.append(mUi.tickLabel6);
   mTickLabels.append(mUi.tickLabel7);
   mTickLabels.append(mUi.tickLabel8);
   mTickLabels.append(mUi.tickLabel9);
   mTickLabels.append(mUi.tickLabel10);
   mTickLabels.append(mUi.tickLabel11);
}

UtQtGradientBar* UtQtGradientLegend::GradientBar()
{
   return mUi.gradientBar;
}

QString UtQtGradientLegend::VariableName() const
{
   return mUi.variableLabel->text();
}

//! Sets the text of the label that will appear to the right of the gradient bar.
void UtQtGradientLegend::SetVariableName(const QString& aName)
{
   mUi.variableLabel->setText(aName);
}

//! Updates the tick labels according to the given data bounds. For instance,
//! 0.0 and 1.0 should be passed in for PD data, so the tick labels will be
//! 0.0, 0.1, 0.2, ... 1.0.
void UtQtGradientLegend::SetDataBounds(double aMinDataValue, double aMaxDataValue)
{
   mMinValue = aMinDataValue;
   mMaxValue = aMaxDataValue;
   Relabel();
}

void UtQtGradientLegend::SetPrecision(int aPrecision)
{
   mPrecision = aPrecision;
   Relabel();
}

void UtQtGradientLegend::SetBoundsAndPrecision(double aMinDataValue, double aMaxDataValue, int aPrecision)
{
   mMinValue  = aMinDataValue;
   mMaxValue  = aMaxDataValue;
   mPrecision = aPrecision;
   Relabel();
}

void UtQtGradientLegend::Relabel()
{
   const double stepSize = std::abs(mMaxValue - mMinValue) / 10.0;
   for (int i = 0; i < mTickLabels.count(); ++i)
   {
      const double      tickValue = mMinValue + (i * stepSize);
      std::stringstream msg;
      msg << std::fixed << std::setprecision(mPrecision) << tickValue;
      mTickLabels[i]->setText(QString::fromStdString(msg.str()));
   }
}
