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

#include "UtQtGL3DPlotWidget.hpp"

#include <QLayout>
#include <QMatrix4x4>

#include "GL/glu.h"
#include "UtMath.hpp"
#include "UtVec3.hpp"

// defining MULTISAMPLE for antialiasing in case it is not alread defined in the GL lib being used
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

UtQtGL3DPlotWidget::UtQtGL3DPlotWidget(QWidget* aParent /*= 0*/)
   : UtQtGLWidgetBase(aParent)
   , mScale(.75)
   , mMouseX(0)
   , mMouseY(0)
   , mButtonDown(false)
   , mAllowZoom(true)
   , mYaw_deg(45.0)
   , mPitch_deg(30.0)
   , mRoll_deg(0.0)
   , mTrajectoryPlot()
{
   setMouseTracking(false);
   setFocusPolicy(Qt::StrongFocus);
}

UtQtGL3DPlotWidget::~UtQtGL3DPlotWidget() {}

void UtQtGL3DPlotWidget::initializeGL()
{
   UtQtGLWidgetBase::initializeGL();
   mTrajectoryPlot.Initialize();
}

void UtQtGL3DPlotWidget::Draw()
{
   glPushMatrix();
   glTranslated((double)(width()) * 0.5, (double)(height()) * 0.5, 0.0);
   glRotated(mPitch_deg, -1.0, 0.0, 0.0);
   glRotated(mYaw_deg, 0.0, -1.0, 0.0);
   glScaled(mScale, mScale, mScale);

   mTrajectoryPlot.SetRotation(mYaw_deg, mPitch_deg);

   glPushMatrix();
   glTranslated(-mTrajectoryPlot.GetWidth() * 0.5, -mTrajectoryPlot.GetHeight() * 0.5, -mTrajectoryPlot.GetDepth() * 0.5);
   mTrajectoryPlot.Paint(this);
   glPopMatrix();

   glPopMatrix();
}

void UtQtGL3DPlotWidget::mousePressEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton)
   {
      mMouseX     = event->x();
      mMouseY     = event->y();
      mButtonDown = true;
   }
   event->ignore(); // call ignore so the event propagates to the parent WaveViewWidget
}

void UtQtGL3DPlotWidget::mouseReleaseEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton)
   {
      mDeltaX = (event->x() - mMouseX) * 1.15;
      mDeltaY = (event->y() - mMouseY) * 1.15;
      //       mDeltaX = event->x() - mMouseX;
      //       mDeltaY = event->y() - mMouseY;

      mYaw_deg += 0.1 * (double)mDeltaX;
      mYaw_deg = UtMath::Limit(mYaw_deg, 0.0, 90.0);
      mPitch_deg += 0.1 * -(double)mDeltaY;
      mPitch_deg = UtMath::Limit(mPitch_deg, 0.0, 90.0);

      mMouseX     = event->x();
      mMouseY     = event->y();
      mButtonDown = false;
   }

   update();
   event->ignore(); // call ignore so the event propagates to the parent WaveViewWidget
}

void UtQtGL3DPlotWidget::mouseMoveEvent(QMouseEvent* event)
{
   if (mButtonDown)
   {
      mDeltaX = (event->x() - mMouseX) * 1.15;
      mDeltaY = (event->y() - mMouseY) * 1.15;
      //       mDeltaX = event->x() - mMouseX;
      //       mDeltaY = event->y() - mMouseY;

      mYaw_deg += 0.1 * (double)mDeltaX;
      mYaw_deg = UtMath::Limit(mYaw_deg, 0.0, 90.0);
      mPitch_deg += 0.1 * -(double)mDeltaY;
      mPitch_deg = UtMath::Limit(mPitch_deg, 0.0, 90.0);

      mMouseX = event->x();
      mMouseY = event->y();
   }

   update();
}

void UtQtGL3DPlotWidget::wheelEvent(QWheelEvent* event)
{
   if (mAllowZoom)
   {
      double numDegrees = event->delta() / 8.0;
      double numSteps   = numDegrees / 15.0;
      mScale *= std::pow(1.125, numSteps);

      mScale = UtMath::Limit(mScale, 0.5, 2.0);

      mTrajectoryPlot.SetScale(mScale);

      update();
   }
}

void UtQtGL3DPlotWidget::SetGridColor(QColor aColor)
{
   mTrajectoryPlot.SetGridColor(aColor);
}

void UtQtGL3DPlotWidget::SetBorderColor(QColor aColor)
{
   mTrajectoryPlot.SetBorderColor(aColor);
}

void UtQtGL3DPlotWidget::SetTextColor(QColor aColor)
{
   mTrajectoryPlot.SetTextColor(aColor);
}

QColor UtQtGL3DPlotWidget::GetGridColor() const
{
   return mTrajectoryPlot.GetGridColor();
}

QColor UtQtGL3DPlotWidget::GetBorderColor() const
{
   return mTrajectoryPlot.GetBorderColor();
}

QColor UtQtGL3DPlotWidget::GetTextColor() const
{
   return mTrajectoryPlot.GetTextColor();
}
