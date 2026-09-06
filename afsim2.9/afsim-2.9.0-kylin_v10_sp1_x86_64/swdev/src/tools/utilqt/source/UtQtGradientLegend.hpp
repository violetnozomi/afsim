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

#ifndef UTQTGRADIENTLEGEND_HPP
#define UTQTGRADIENTLEGEND_HPP

#include "utilqt_export.h"

#include <QVector>
#include <QWidget>

#include "ui_UtQtGradientLegend.h"

class QLabel;
class UtQtGradientBar;

//! A GUI for showing a gradient bar with tick values and a label. This is used
//! as a color legend or key for e.g. data values that have been plotted with
//! color on a plot.
class UTILQT_EXPORT UtQtGradientLegend : public QWidget
{
   Q_OBJECT

public:
   explicit UtQtGradientLegend(QWidget* aParent = nullptr);
   ~UtQtGradientLegend() override = default;

   UtQtGradientBar* GradientBar();
   QString          VariableName() const;
   void             SetVariableName(const QString& aName);
   void             SetDataBounds(double aMinDataValue, double aMaxDataValue);
   void             SetPrecision(int aPrecision);
   void             SetBoundsAndPrecision(double aMinDataValue, double aMaxDataValue, int aPrecision);

private:
   void Relabel();

   QVector<QLabel*>         mTickLabels;
   Ui::UtQtGradientLegendUi mUi;
   double                   mMinValue{0.0};
   double                   mMaxValue{1.0};
   int                      mPrecision{6};
};

#endif // UTQTGRADIENTLEGEND_HPP
