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

#include "UtQt3DTrajectoryPlot.hpp"

#include <iostream>
#include <limits>

#include <QLayout>
#include <QMatrix4x4>

#include "UtMath.hpp"

//#include "WaveVaEnvironment.hpp"

// defining MULTISAMPLE for anti-aliasing in case it is not already defined in the GL lib being used
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

UtQt3DTrajectoryPlot::UtQt3DTrajectoryPlot()
   : mScale(.5)
   , mWidth(400)
   , mHeight(400)
   , mDepth(400)
   , mFont("Arial", 10.0, 10)
   , mFontSize(10)
   , mSignificantDigits(6)
   , mDirty(false)
   , mXMin((std::numeric_limits<double>::max)())
   , mXMax(-(std::numeric_limits<double>::max)())
   , mYMin((std::numeric_limits<double>::max)())
   , mYMax(-(std::numeric_limits<double>::max)())
   , mZMin((std::numeric_limits<double>::max)())
   , mZMax(-(std::numeric_limits<double>::max)())
   , mDrawGrid(true)
   , mDrawLegend(true)
   , mDrawOrigin(true)
{
   mXYPlane._border  = 0;
   mXYPlane._grid    = 0;
   mXAxis._min       = 0.0;
   mXAxis._max       = 0.0;
   mXAxis._increment = 0.0;

   mYZPlane._border  = 0;
   mYZPlane._grid    = 0;
   mYAxis._min       = 0.0;
   mYAxis._max       = 0.0;
   mYAxis._increment = 0.0;

   mXZPlane._border  = 0;
   mXZPlane._grid    = 0;
   mZAxis._min       = 0.0;
   mZAxis._max       = 0.0;
   mZAxis._increment = 0.0;

   mTrajectoryPlot = 0;

   mTextColor   = QColor();
   mBorderColor = QColor();
   mGridColor   = QColor(128, 128, 128, 128);
}

UtQt3DTrajectoryPlot::~UtQt3DTrajectoryPlot()
{
   glDeleteLists(0, 8);
}

void UtQt3DTrajectoryPlot::Initialize()
{
   BuildAxisPlanes();
}

void UtQt3DTrajectoryPlot::Paint(QPaintDevice* aPaintDevice)
{
   glEnable(GL_POLYGON_SMOOTH); // Using this can cause problems with poly-seams
   glEnable(GL_LINE_SMOOTH);
   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
   glEnable(GL_CULL_FACE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   if (mDirty)
   {
      BuildAxisPlanes();
      BuildPlot();
   }

   glPushMatrix();
   glLineWidth(1.0);
   glPointSize(2.0);

   // XY Plane
   glPushMatrix();
   glRotated(90.0, 1.0, 0.0, 0.0);
   if (mDrawGrid)
   {
      glCallList(mXYPlane._grid);
   }
   glCallList(mXYPlane._border);
   glPopMatrix();

   // YZ Plane
   glPushMatrix();
   glTranslated(mWidth, 0.0, 0.0);
   glRotated(-90.0, 0.0, 1.0, 0.0);
   if (mDrawGrid)
   {
      glCallList(mYZPlane._grid);
   }
   glCallList(mYZPlane._border);
   glPopMatrix();

   // ZX Plane
   glPushMatrix();
   glTranslated(0.0, 0.0, mDepth);
   if (mDrawGrid)
   {
      glCallList(mXZPlane._grid);
   }
   glCallList(mXZPlane._border);
   glPopMatrix();

   PaintOrigin();
   PaintTrajectory();

   // Axis Labels
   if (mYaw_deg < 80 || mPitch_deg > 10)
   {
      // X Axis on the xy plane goes in the x direction
      glPushMatrix();
      glTranslated(mWidth * 0.5, mHeight + 25.0, mDepth + 40.0);
      PaintText(aPaintDevice, mXAxis._label, X_AXIS);

      if (mXAxis._increment != 0)
      {
         float currInc = GetMinIncrement(mXAxis._min, mXAxis._increment);
         int   x       = UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mWidth));

         while (x <= mWidth)
         {
            glPushMatrix();
            glTranslated(x - (mWidth * 0.5), -20.0f, -40.0f);
            QString intervalStr = QString::number(currInc, 'g', mSignificantDigits);
            PaintText(aPaintDevice, intervalStr, X_AXIS);
            glPopMatrix();

            currInc += mXAxis._increment;
            x = UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mWidth));
         }
      }
      glPopMatrix();
   }

   if ((mYaw_deg >= 0 && mPitch_deg > 10) || (mYaw_deg > 10 && mPitch_deg < 10))
   {
      // y axis on the xy plane goes in the z direction
      glPushMatrix();
      glTranslated(mWidth + 25.0, mHeight + 25.0, mDepth * 0.5);
      PaintText(aPaintDevice, mYAxis._label, Y_AXIS, Qt::AlignRight | Qt::AlignVCenter);

      if (mYAxis._increment != 0)
      {
         float currInc = GetMinIncrement(mYAxis._min, mYAxis._increment);
         int   z       = UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));

         while (z <= mDepth)
         {
            glPushMatrix();
            glTranslated(-25.0f, -20.0f, z - (mDepth * 0.5));
            QString intervalStr = QString::number(currInc, 'g', mSignificantDigits);
            PaintText(aPaintDevice, intervalStr, Y_AXIS, Qt::AlignRight | Qt::AlignVCenter);
            glPopMatrix();

            currInc += mYAxis._increment;
            z = UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));
         }
      }
      glPopMatrix();
   }

   if (mPitch_deg < 80)
   {
      // z axis on the xz plane goes in the y direction
      glPushMatrix();
      glTranslated(mWidth + 25.0, mHeight * 0.5, -25.0);
      PaintText(aPaintDevice, mZAxis._label, Z_AXIS, Qt::AlignRight | Qt::AlignVCenter);

      if (mZAxis._increment != 0)
      {
         float currInc = GetMinIncrement(mZAxis._min, mZAxis._increment);
         int   y       = mHeight - UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));

         while (y >= 0)
         {
            glPushMatrix();
            glTranslated(-25.0f, y - ((mHeight * 0.5) + 10.0f), 20.0f); // -10 on the y axis to make the labels show
                                                                        // above the interval rather than below
            QString intervalStr = QString::number(currInc, 'g', mSignificantDigits);
            PaintText(aPaintDevice, intervalStr, Z_AXIS, Qt::AlignRight | Qt::AlignVCenter);

            currInc += mZAxis._increment;
            y = mHeight - UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));
            glPopMatrix();
         }
      }
      glPopMatrix();
   }

   glPopMatrix();
}

void UtQt3DTrajectoryPlot::PaintOrigin()
{
   if (mDrawOrigin)
   {
      double originX = mWidth - UtMath::Round(UtMath::Lerp(0, mXAxis._min, mXAxis._max, 0, mWidth));
      double originY = mDepth - UtMath::Round(UtMath::Lerp(0, mYAxis._min, mYAxis._max, 0, mDepth));
      double originZ = UtMath::Round(UtMath::Lerp(0, mZAxis._min, mZAxis._max, 0, mHeight));

      glPushMatrix();
      glTranslated(originX, originZ, originY);
      glColor4f(1.0, 0.0, 0.0, 1.0);
      glLineWidth(1.0 * mScale);
      glBegin(GL_LINES);
      glVertex3d(-4, 0, 0);
      glVertex3d(4, 0, 0);

      glVertex3d(0, -4, 0);
      glVertex3d(0, 4, 0);

      glVertex3d(0, 0, -4);
      glVertex3d(0, 0, 4);
      glEnd();
      glPopMatrix();
   }
}

void UtQt3DTrajectoryPlot::PaintTrajectory()
{
   glPushMatrix();
   glColor3f(0.5f, 0.5f, 0.5f); // TODO: change color per plot
   glCallList(mTrajectoryPlot);
   glPopMatrix();
}

void UtQt3DTrajectoryPlot::PaintText(QPaintDevice*  aPaintDevice,
                                     const QString& aText,
                                     Axis           aAxis,
                                     Qt::Alignment  aAlign /*= Qt::AlignLeft | Qt::AlignVCenter*/)
{
   double proj[16];
   double model[16];

   glGetDoublev(GL_MODELVIEW_MATRIX, model);
   glGetDoublev(GL_PROJECTION_MATRIX, proj);

   mFont.setPointSizeF(mFontSize);

   QPainter painter;
   painter.begin(aPaintDevice);
   painter.setViewport(0, 0, aPaintDevice->width(), aPaintDevice->height());
   // painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
   painter.setRenderHints(QPainter::TextAntialiasing);
   painter.setClipping(false);
   painter.setFont(mFont);
   painter.setPen(Qt::black);

   QVector3D textLoc(0.0, 0.0, 0.0);
   // QMatrix4x4 modelMat(model); //This constructor is not supported in Qt5
   // QMatrix4x4 projMat(proj);   //This constructor is not supported in Qt5
   QMatrix4x4 modelMat(model[0],
                       model[1],
                       model[2],
                       model[3],
                       model[4],
                       model[5],
                       model[6],
                       model[7],
                       model[8],
                       model[9],
                       model[10],
                       model[11],
                       model[12],
                       model[13],
                       model[14],
                       model[15]);
   QMatrix4x4 projMat(proj[0],
                      proj[1],
                      proj[2],
                      proj[3],
                      proj[4],
                      proj[5],
                      proj[6],
                      proj[7],
                      proj[8],
                      proj[9],
                      proj[10],
                      proj[11],
                      proj[12],
                      proj[13],
                      proj[14],
                      proj[15]);
   QMatrix4x4 result  = modelMat.transposed() * projMat.transposed();
   QVector3D  textXYZ = result * textLoc;

   double x            = aPaintDevice->width() - textXYZ.x();
   double y            = textXYZ.y();
   double textBoxWidth = mWidth;

   QTextOption to;
   to.setAlignment(aAlign);

   switch (aAxis)
   {
   case X_AXIS:
      // do nothing for now
      break;
   case Y_AXIS:
      x += -(textBoxWidth * mScale);
      break;
   case Z_AXIS:
      x += -(textBoxWidth * mScale);
      break;
   }

   painter.translate(x, y);
   //       painter.setPen(Qt::green);
   //       painter.drawRect(QRectF(0,0,textBoxWidth*scale,20*scale));
   painter.setPen(mTextColor);
   painter.drawText(QRectF(0, 0, textBoxWidth * mScale, 32), aText, to);
   painter.end();
}

void UtQt3DTrajectoryPlot::DefineAxis(QString aTitle, double aMin, double aMax, double aIncrement, Axis aAxis)
{
   SetLabel(aTitle, aAxis);

   switch (aAxis)
   {
   case X_AXIS:
      mXAxis._min       = aMin;
      mXAxis._max       = aMax;
      mXAxis._increment = aIncrement;
      break;
   case Y_AXIS:
      mYAxis._min       = aMin;
      mYAxis._max       = aMax;
      mYAxis._increment = aIncrement;
      break;
   case Z_AXIS:
      mZAxis._min       = aMin;
      mZAxis._max       = aMax;
      mZAxis._increment = aIncrement;
      break;
   }

   mDirty = true;
}

void UtQt3DTrajectoryPlot::SetLabel(QString aTitle, Axis aAxis)
{
   switch (aAxis)
   {
   case X_AXIS:
      mXAxis._label = aTitle;
      break;
   case Y_AXIS:
      mYAxis._label = aTitle;
      break;
   case Z_AXIS:
      mZAxis._label = aTitle;
      break;
   }
}

void UtQt3DTrajectoryPlot::BuildPlot()
{
   glDeleteLists(mTrajectoryPlot, 1);

   mTrajectoryPlot = glGenLists(1);
   glNewList(mTrajectoryPlot, GL_COMPILE);
   {
      glLineWidth(2.0 * mScale);
      for (auto& mapIter : mTrajectoryRawDataMap)
      {
         QColor color = GetSeriesColor(mapIter.first);
         glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
         glBegin(GL_LINE_STRIP);
         {
            for (auto& trajIter : mapIter.second)
            {
               int x = mWidth - UtMath::Round(UtMath::Lerp(trajIter.x(), mXAxis._min, mXAxis._max, 0, mWidth));
               int y = mDepth - UtMath::Round(UtMath::Lerp(trajIter.y(), mYAxis._min, mYAxis._max, 0, mDepth));
               int z = UtMath::Round(UtMath::Lerp(trajIter.z(), mZAxis._min, mZAxis._max, 0, mHeight));
               glVertex3i(x, z, y); // in opengl, y is up
            }
         }
         glEnd();
      }
   }
   glEndList();
}

void UtQt3DTrajectoryPlot::BuildAxisPlanes()
{
   BuildXYPlane();
   BuildYZPlane();
   BuildXZPlane();

   mDirty = false;
}

void UtQt3DTrajectoryPlot::BuildXYPlane()
{
   glDeleteLists(mXYPlane._border, 1);
   glDeleteLists(mXYPlane._grid, 1);

   // border and ticks
   mXYPlane._border = glGenLists(1);
   glNewList(mXYPlane._border, GL_COMPILE);
   glPushMatrix();
   glLineWidth(2.0 * mScale);
   glColor4f(mBorderColor.redF(), mBorderColor.greenF(), mBorderColor.blueF(), mBorderColor.alphaF());
   glBegin(GL_LINE_LOOP);
   glVertex2f(0.0, 0.0);
   glVertex2f(mWidth, 0.0);
   glVertex2f(mWidth, mDepth);
   glVertex2f(0.0, mDepth);
   glEnd();

   if (mXAxis._increment != 0)
   {
      // tick marks
      // find the smallest and largest multiple of increment
      double currInc = GetMinIncrement(mXAxis._min, mXAxis._increment);
      int    x       = mWidth - UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mWidth));

      while (x >= 0)
      {
         glBegin(GL_LINES);
         glVertex2i(x, 0);
         glVertex2i(x, 5);
         glEnd();

         currInc += mXAxis._increment;
         x = mWidth - UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mWidth));
      }
   }
   if (mYAxis._increment != 0)
   {
      double currInc = GetMinIncrement(mYAxis._min, mYAxis._increment);
      int    y       = mDepth - UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));

      while (y >= 0)
      {
         glBegin(GL_LINES);
         glVertex2i(0, y);
         glVertex2i(5, y);
         glEnd();

         currInc += mYAxis._increment;
         y = mDepth - UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));
      }
   }
   glPopMatrix();
   glEndList();

   // grid
   mXYPlane._grid = glGenLists(1);
   glNewList(mXYPlane._grid, GL_COMPILE);
   glPushMatrix();
   glLineWidth(0.5 * mScale);
   glColor4f(mGridColor.redF(), mGridColor.greenF(), mGridColor.blueF(), mGridColor.alphaF());

   if (mXAxis._increment != 0)
   {
      // find the smallest and largest multiple of increment
      double currInc = GetMinIncrement(mXAxis._min, mXAxis._increment);
      int    x       = mWidth - UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mWidth));

      while (x >= 0)
      {
         glBegin(GL_LINES);
         glVertex2i(x, 0);
         glVertex2i(x, mDepth);
         glEnd();

         currInc += mXAxis._increment;
         x = mWidth - UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mWidth));
      }
   }

   if (mYAxis._increment != 0)
   {
      double currInc = GetMinIncrement(mYAxis._min, mYAxis._increment);
      int    y       = mDepth - UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));

      while (y >= 0)
      {
         glBegin(GL_LINES);
         glVertex2i(0, y);
         glVertex2i(mWidth, y);
         glEnd();

         currInc += mYAxis._increment;
         y = mDepth - UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));
      }
   }
   glPopMatrix();
   glEndList();
}

void UtQt3DTrajectoryPlot::BuildYZPlane()
{
   glDeleteLists(mYZPlane._border, 1);
   glDeleteLists(mYZPlane._grid, 1);

   // border and ticks
   mYZPlane._border = glGenLists(1);
   glNewList(mYZPlane._border, GL_COMPILE);
   glPushMatrix();
   glLineWidth(2.0 * mScale);
   glColor4f(mBorderColor.redF(), mBorderColor.greenF(), mBorderColor.blueF(), mBorderColor.alphaF());
   glBegin(GL_LINE_LOOP);
   glVertex2f(0.0, 0.0);
   glVertex2f(mDepth, 0.0);
   glVertex2f(mDepth, mHeight);
   glVertex2f(0.0, mHeight);
   glEnd();

   // tick marks
   // find the smallest and largest multiple of increment
   //          double currInc = GetMinIncrement(mYAxis._min, mYAxis._increment);
   //          int x = mDepth - UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));
   //
   //          while (x >= 0)
   //          {
   //             glBegin(GL_LINES);
   //                glVertex2i(x, 0);
   //                glVertex2i(x, 5);
   //             glEnd();
   //
   //             currInc += mYAxis._increment;
   //             x = mDepth - UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));
   //          }
   //
   //          currInc = GetMinIncrement(mZAxis._min, mZAxis._increment);
   //          int y = UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));
   //
   //          while (y <= mHeight)
   //          {
   //             glBegin(GL_LINES);
   //                glVertex2i(0, y);
   //                glVertex2i(5, y);
   //             glEnd();
   //
   //             currInc += mZAxis._increment;
   //             y = UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));
   //          }
   glPopMatrix();
   glEndList();

   // grid
   mYZPlane._grid = glGenLists(1);
   glNewList(mYZPlane._grid, GL_COMPILE);
   glPushMatrix();
   glLineWidth(0.5 * mScale);
   glColor4f(mGridColor.redF(), mGridColor.greenF(), mGridColor.blueF(), mGridColor.alphaF());

   // find the smallest and largest multiple of increment
   if (mYAxis._increment != 0)
   {
      double currInc = GetMinIncrement(mYAxis._min, mYAxis._increment);
      int    x       = mDepth - UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));

      while (x >= 0)
      {
         glBegin(GL_LINES);
         glVertex2i(x, 0);
         glVertex2i(x, mHeight);
         glEnd();

         currInc += mYAxis._increment;
         x = mDepth - UtMath::Round(UtMath::Lerp(currInc, mYAxis._min, mYAxis._max, 0, mDepth));
      }
   }
   if (mZAxis._increment != 0)
   {
      double currInc = GetMinIncrement(mZAxis._min, mZAxis._increment);
      int    y       = UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));

      while (y <= mHeight)
      {
         glBegin(GL_LINES);
         glVertex2i(0, y);
         glVertex2i(mDepth, y);
         glEnd();

         currInc += mZAxis._increment;
         y = UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));
      }
   }
   glPopMatrix();
   glEndList();
}

void UtQt3DTrajectoryPlot::BuildXZPlane()
{
   glDeleteLists(mXZPlane._border, 1);
   glDeleteLists(mXZPlane._grid, 1);

   // border and ticks
   mXZPlane._border = glGenLists(1);
   glNewList(mXZPlane._border, GL_COMPILE);
   glPushMatrix();
   glLineWidth(2.0 * mScale);
   glColor4f(mBorderColor.redF(), mBorderColor.greenF(), mBorderColor.blueF(), mBorderColor.alphaF());
   glBegin(GL_LINE_LOOP);
   glVertex2f(0.0, 0.0);
   glVertex2f(mWidth, 0.0);
   glVertex2f(mWidth, mHeight);
   glVertex2f(0.0, mHeight);
   glEnd();

   // tick marks
   // find the smallest and largest multiple of increment
   //          double currInc = GetMinIncrement(mXAxis._min, mXAxis._increment);
   //          int x = mWidth - UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mWidth));
   //
   //          while (x >= 0)
   //          {
   //             glBegin(GL_LINES);
   //                glVertex2i(x, 0);
   //                glVertex2i(x, 5);
   //             glEnd();
   //
   //             currInc += mXAxis._increment;
   //             x = mWidth - UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mWidth));
   //          }

   if (mZAxis._increment != 0)
   {
      double currInc = GetMinIncrement(mZAxis._min, mZAxis._increment);
      int    y       = UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));

      while (y <= mHeight)
      {
         glBegin(GL_LINES);
         glVertex2i(0, y);
         glVertex2i(5, y);
         glEnd();

         currInc += mZAxis._increment;
         y = UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));
      }
   }
   glPopMatrix();
   glEndList();

   // grid
   mXZPlane._grid = glGenLists(1);
   glNewList(mXZPlane._grid, GL_COMPILE);
   glPushMatrix();
   glLineWidth(0.5 * mScale);
   glColor4f(mGridColor.redF(), mGridColor.greenF(), mGridColor.blueF(), mGridColor.alphaF());

   // find the smallest and largest multiple of increment
   if (mXAxis._increment != 0)
   {
      double currInc = GetMinIncrement(mXAxis._min, mXAxis._increment);
      int    x       = mWidth - UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mDepth));

      while (x >= 0)
      {
         glBegin(GL_LINES);
         glVertex2i(x, 0);
         glVertex2i(x, mHeight);
         glEnd();

         currInc += mXAxis._increment;
         x = mWidth - UtMath::Round(UtMath::Lerp(currInc, mXAxis._min, mXAxis._max, 0, mDepth));
      }
   }

   if (mZAxis._increment != 0)
   {
      double currInc = GetMinIncrement(mZAxis._min, mZAxis._increment);
      int    y       = UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));

      while (y <= mHeight)
      {
         glBegin(GL_LINES);
         glVertex2i(0, y);
         glVertex2i(mDepth, y);
         glEnd();

         currInc += mZAxis._increment;
         y = UtMath::Round(UtMath::Lerp(currInc, mZAxis._min, mZAxis._max, 0, mHeight));
      }
   }
   glPopMatrix();
   glEndList();
}

// return the lowest multiple of increment that is within the given range
double UtQt3DTrajectoryPlot::GetMinIncrement(double min, double increment)
{
   int    n1     = UtMath::Round(min / increment);
   double result = increment * (double)n1;

   // if the result is still outside of the range, add 1 and multiply again
   if (result < min)
   {
      result = increment * (double)(n1 + 1);
   }

   return result;
}

double UtQt3DTrajectoryPlot::GetMaxIncrement(double max, double increment)
{
   return increment * UtMath::Round(max / increment);
}

void UtQt3DTrajectoryPlot::SetPlotData(QList<double>& aXAxis,
                                       QList<double>& aYAxis,
                                       QList<double>& aZAxis,
                                       int            aSeriesNum,
                                       const double   aUnitConversionFactor)
{
   mTrajectoryRawDataMap[aSeriesNum].clear();

   for (int i = 0; i < aXAxis.count(); ++i)
   {
      double x = aXAxis.at(i) * aUnitConversionFactor;
      double y = aYAxis.at(i) * aUnitConversionFactor;
      double z = aZAxis.at(i); // ft

      mXMin = (std::min)(x, mXMin);
      mYMin = (std::min)(y, mYMin);
      mZMin = (std::min)(z, mZMin);

      mXMax = (std::max)(x, mXMax);
      mYMax = (std::max)(y, mYMax);
      mZMax = (std::max)(z, mZMax);

      mTrajectoryRawDataMap[aSeriesNum].push_back(QVector3D(x, y, z));
   }

   mDirty = true;
   // BuildPlot();
}

void UtQt3DTrajectoryPlot::RemoveDataSeries(int aSeriesNum)
{
   mTrajectoryRawDataMap.erase(aSeriesNum);
   mSeriesColorMap.erase(aSeriesNum);
   mDirty = true;
}

bool UtQt3DTrajectoryPlot::GetXMinMax(double& aXMin, double& aXMax)
{
   aXMin = mXMin;
   aXMax = mXMax;

   return !mTrajectoryRawDataMap.empty();
}

bool UtQt3DTrajectoryPlot::GetYMinMax(double& aYMin, double& aYMax)
{
   aYMin = mYMin;
   aYMax = mYMax;

   return !mTrajectoryRawDataMap.empty();
}

bool UtQt3DTrajectoryPlot::GetZMinMax(double& aZMin, double& aZMax)
{
   aZMin = mZMin;
   aZMax = mZMax;

   return !mTrajectoryRawDataMap.empty();
}

bool UtQt3DTrajectoryPlot::GetMinMax(double& aXMin, double& aXMax, double& aYMin, double& aYMax, double& aZMin, double& aZMax)
{
   GetXMinMax(aXMin, aXMax);
   GetYMinMax(aYMin, aYMax);
   GetZMinMax(aZMin, aZMax);

   return !mTrajectoryRawDataMap.empty();
}

void UtQt3DTrajectoryPlot::Clear()
{
   glDeleteLists(mTrajectoryPlot, 1);
   mTrajectoryRawDataMap.clear();

   mXMin = (std::numeric_limits<double>::max)();
   mXMax = -(std::numeric_limits<double>::max)();
   mYMin = (std::numeric_limits<double>::max)();
   mYMax = -(std::numeric_limits<double>::max)();
   mZMin = (std::numeric_limits<double>::max)();
   mZMax = -(std::numeric_limits<double>::max)();
}

void UtQt3DTrajectoryPlot::SetGridColor(QColor aColor)
{
   mGridColor = aColor;
   mDirty     = true;
}

void UtQt3DTrajectoryPlot::SetBorderColor(QColor aColor)
{
   mBorderColor = aColor;
   mDirty       = true;
}

void UtQt3DTrajectoryPlot::SetTextColor(QColor aColor)
{
   mTextColor = aColor;
   mDirty     = true;
}

void UtQt3DTrajectoryPlot::SetSeriesColor(QColor aColor, int aSeriesNum)
{
   mSeriesColorMap[aSeriesNum] = aColor;
   mDirty                      = true;
}

QColor UtQt3DTrajectoryPlot::GetSeriesColor(int aSeriesNum)
{
   if (mSeriesColorMap.count(aSeriesNum) > 0)
   {
      return mSeriesColorMap[aSeriesNum];
   }
   else
   {
      return QColor();
   }
}
