// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UtQtGL3DPlotWidget_HPP
#define UtQtGL3DPlotWidget_HPP

#include "utilqt_export.h"

#include <iostream>

#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>

#include "UtQt3DTrajectoryPlot.hpp"
#include "UtQtGLWidgetBase.hpp"


class UTILQT_EXPORT UtQtGL3DPlotWidget : public UtQtGLWidgetBase
{
   Q_OBJECT
   Q_PROPERTY(QColor gridColor READ GetGridColor WRITE SetGridColor)
   Q_PROPERTY(QColor borderColor READ GetBorderColor WRITE SetBorderColor)
   Q_PROPERTY(QColor textColor READ GetTextColor WRITE SetTextColor)

public:
   UtQtGL3DPlotWidget(QWidget* aParent = nullptr);
   ~UtQtGL3DPlotWidget() override;

   const UtQt3DTrajectoryPlot& GetTrajectoryItem() const { return mTrajectoryPlot; }

   void EnableZoom(bool aEnabled) { mAllowZoom = aEnabled; }
   void SetYaw(double aYaw) { mYaw_deg = aYaw; }
   void SetPitch(double aPitch) { mPitch_deg = aPitch; }

   void   SetGridColor(QColor aColor);
   void   SetBorderColor(QColor aColor);
   void   SetTextColor(QColor aColor);
   QColor GetGridColor() const;
   QColor GetBorderColor() const;
   QColor GetTextColor() const;

protected:
   // Qt reimplemented
   void initializeGL() override;
   void Draw() override;
   void mousePressEvent(QMouseEvent* event) override;
   void mouseReleaseEvent(QMouseEvent* event) override;
   void mouseMoveEvent(QMouseEvent* event) override;
   void wheelEvent(QWheelEvent* event) override;

private:
   double mScale;
   double mDeltaX;
   double mDeltaY;
   int    mMouseX;
   int    mMouseY;
   bool   mButtonDown;

   bool mAllowZoom;

   double mYaw_deg;
   double mPitch_deg;
   double mRoll_deg;

   UtQt3DTrajectoryPlot mTrajectoryPlot;
};

#endif // WAVEGLCHARTVIEW_HPP
