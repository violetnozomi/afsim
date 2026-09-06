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

#include "UtQtGL2DPlotWidget.hpp"

#include <cassert>
#include <cfloat> // DBL_MAX on Linux

#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QMatrix4x4>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSpacerItem>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QVector3D>
#include <QWheelEvent>
#include <qmath.h>

#include "UtQtColorButton.hpp"

UtQtGL2DPlotWidget::UtQtGL2DPlotWidget(QWidget* aParent, UtQtGL2DPlot* aPlot)
   : UtQtGLWidgetBase(aParent)
   , mPreventUpdates(false)
   , mEnableMouseZoom(true)
   , mMousePressLocation(QPoint(-1, -1))
   , mButtonDownZooming(false)
   , mButtonDownLegend(false)
   , mLegendSelectionXOffset(0)
   , mLegendSelectionYOffset(0)
   , mZoomed(false)
   , mBoundaryTabsEnabled(false)
   , mBoundaryTabsMouseEnabled(true)
   , mBoundaryTabValue1(0)
   , mBoundaryTabValue2(0)
   , mBoundaryTab1Selected(false)
   , mBoundaryTab2Selected(false)
   , mBoundaryTabLockedToTickmarks(false)
   , mBoundaryTabDetailedLabels(false)
   , mMouseCrosshairsEnabled(false)
   , mEditModeEnabled(false)
   , mEditModeLockX(false)
   , mEditModeLockY(false)
   , mMinIncrementX(0)
   , mMinIncrementY(0)
   , mMinIncrementY2(0)
   , mCurrentSelectedSeries(-1)
   , mDataPointSelected(false)
   , mDataPointIndex(-1)
   , mDrawFocusIndicators(false)
   , mUseApplicationPalette(true)
   , mContextMenu(nullptr)
   , mDataView(false)
   , mPlot(aPlot)
{
   if (mPlot == nullptr)
   {
      mPlot = new UtQtGL2DPlot();
   }

   setFocusPolicy(Qt::StrongFocus);
   setContextMenuPolicy(Qt::DefaultContextMenu);

   QVBoxLayout* mainLayout = new QVBoxLayout();
   setLayout(mainLayout);
   mDataScrollArea = new QScrollArea(this);
   mainLayout->addWidget(mDataScrollArea);
   mDataScrollArea->hide();

   connect(mPlot, &UtQtGL2DPlot::BoundsChanged, this, &UtQtGL2DPlotWidget::PlotBoundariesChangedCB);
   connect(mPlot, &UtQtGL2DPlot::SeriesRemoved, this, QOverload<>::of(&UtQtGL2DPlotWidget::update));
   connect(mPlot, &UtQtGL2DPlot::SeriesNameChanged, this, QOverload<>::of(&UtQtGL2DPlotWidget::update));
   connect(mPlot, &UtQtGL2DPlot::SeriesColorChanged, this, QOverload<>::of(&UtQtGL2DPlotWidget::update));
   connect(mPlot, &UtQtGL2DPlot::ItemModelSeriesUpdated, this, QOverload<>::of(&UtQtGL2DPlotWidget::update));
   connect(mPlot, &UtQtGL2DPlot::SeriesSelected, this, &UtQtGL2DPlotWidget::HandleSeriesSelected);
   connect(qApp, &QGuiApplication::paletteChanged, this, &UtQtGL2DPlotWidget::PaletteChangedCB);

   ApplyPalette(palette());

   mPlot->GetXAxisBounds(mPreZoomMinXBoundary, mPreZoomMaxXBoundary);
   mPlot->GetYAxisBounds(mPreZoomMinYBoundary, mPreZoomMaxYBoundary);
}

UtQtGL2DPlotWidget::~UtQtGL2DPlotWidget()
{
   // Deleting plot cleans up GL stuff
   makeCurrent();
   delete mPlot;

   if (mContextMenu != nullptr)
   {
      delete mContextMenu;
   }
}

void UtQtGL2DPlotWidget::Draw()
{
   if (!mDataView)
   {
      mPlot->Draw();
      DrawZoomBox();
      DrawBoundaryTabs();
      DrawFocusIndicators();
   }
}

void UtQtGL2DPlotWidget::QtDraw()
{
   if (!mDataView)
   {
      QPainter painter;
      painter.begin(this);
      DrawBoundaryTabsText(&painter);
      mPlot->QtDraw(&painter);
      painter.end();
   }
}

void UtQtGL2DPlotWidget::DrawZoomBox()
{
   if (mEnableMouseZoom && mButtonDownZooming)
   {
      glPushMatrix();
      {
         glTranslated(static_cast<double>(width()) * 0.5, static_cast<double>(height()) * 0.5, 0.0);

         glLineWidth(2);
         glColor4f(.98f, 0.0f, 0.0f, .75f);

         glBegin(GL_LINE_LOOP);
         {
            glVertex2i(mMousePressLocation.x(), mMousePressLocation.y());
            glVertex2i(mMousePressLocation.x(), mMouseCurrentLocation.y());
            glVertex2i(mMouseCurrentLocation.x(), mMouseCurrentLocation.y());
            glVertex2i(mMouseCurrentLocation.x(), mMousePressLocation.y());
         }
         glEnd();
      }
      glPopMatrix();
      // update();
   }
}

void UtQtGL2DPlotWidget::DrawBoundaryTabs()
{
   if (mBoundaryTabsEnabled)
   {
      glPushMatrix();
      {
         glTranslated(static_cast<double>(width()) * 0.5, static_cast<double>(height()) * 0.5, 0.0);

         double xBoundMin, xBoundMax, xValue1, xValue2, yDummy;

         // Limit the BoundaryTabValues to the min/max Boundaries of the Plot
         mPlot->GetXAxisBounds(xBoundMin, xBoundMax);
         if (mBoundaryTabValue1 < xBoundMin)
         {
            SetBoundaryTab1(xBoundMin);
         }
         else if (mBoundaryTabValue1 > xBoundMax)
         {
            SetBoundaryTab1(xBoundMax);
         }
         if (mBoundaryTabValue2 < xBoundMin)
         {
            SetBoundaryTab2(xBoundMin);
         }
         else if (mBoundaryTabValue2 > xBoundMax)
         {
            SetBoundaryTab2(xBoundMax);
         }

         mPlot->TranslatePointFromRealToScene(0, mBoundaryTabValue1, 0, xValue1, yDummy);
         mPlot->TranslatePointFromRealToScene(0, mBoundaryTabValue2, 0, xValue2, yDummy);

         double heightInc = mDisplayHeight * .01;
         double widthInc  = mDisplayWidth * .01;

         mBoundaryTab1Points.clear();
         mBoundaryTab1Points << QPointF(xValue1, mPlot->mPlotBottom_pixels)
                             << QPointF(xValue1 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc)
                             << QPointF(xValue1 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc)
                             << QPointF(xValue1 - 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc)
                             << QPointF(xValue1 - 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
         glLineWidth(1);
         glColor4f(0, 0, 0, .4f);
         // Draw the polygon that fills the tab
         glBegin(GL_POLYGON);
         {
            glVertex2i(xValue1, mPlot->mPlotBottom_pixels);
            glVertex2i(xValue1 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
            glVertex2i(xValue1 + 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc);
            glVertex2i(xValue1 - 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc);
            glVertex2i(xValue1 - 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
         }
         glEnd();

         mBoundaryTab2Points.clear();
         mBoundaryTab2Points << QPointF(xValue2, mPlot->mPlotBottom_pixels)
                             << QPointF(xValue2 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc)
                             << QPointF(xValue2 + 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc)
                             << QPointF(xValue2 - 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc)
                             << QPointF(xValue2 - 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
         glBegin(GL_POLYGON);
         {
            glVertex2i(xValue2, mPlot->mPlotBottom_pixels);
            glVertex2i(xValue2 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
            glVertex2i(xValue2 + 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc);
            glVertex2i(xValue2 - 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc);
            glVertex2i(xValue2 - 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
         }
         glEnd();
         // Draw the line that traces the tab
         glBegin(GL_LINE_LOOP);
         {
            glVertex2i(xValue1, mPlot->mPlotBottom_pixels);
            glVertex2i(xValue1 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
            glVertex2i(xValue1 + 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc);
            glVertex2i(xValue1 - 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc);
            glVertex2i(xValue1 - 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
         }
         glEnd();
         glBegin(GL_LINE_LOOP);
         {
            glVertex2i(xValue2, mPlot->mPlotBottom_pixels);
            glVertex2i(xValue2 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
            glVertex2i(xValue2 + 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc);
            glVertex2i(xValue2 - 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc);
            glVertex2i(xValue2 - 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
         }
         glEnd();
         // Draw Vertical lines
         glColor4f(0, 0, 0, .8f);
         glBegin(GL_LINES);
         {
            glVertex2i(xValue1, mPlot->mPlotBottom_pixels);
            glVertex2i(xValue1, mPlot->mPlotTop_pixels);
            glVertex2i(xValue2, mPlot->mPlotBottom_pixels);
            glVertex2i(xValue2, mPlot->mPlotTop_pixels);
         }
         glEnd();

         glColor4f(1, 0, 0, 1);
         // Line for the distance between tabs
         glBegin(GL_LINES);
         {
            glVertex2i(xValue1, mPlot->mPlotBottom_pixels + heightInc);
            glVertex2i(xValue2, mPlot->mPlotBottom_pixels + heightInc);
         }
         glEnd();

         double deltaX = std::abs(mBoundaryTabValue1 - mBoundaryTabValue2);

         // Now handle text displays
         const UtQtGL2DPlot::SeriesMap data = mPlot->GetData();
         QList<double>                 yValues;
         for (const auto& seriesIter : data)
         {
            const int                       seriesNum = seriesIter.first;
            const UtQtGL2DPlot::SeriesBase* series    = seriesIter.second;
            if (mPlot->GetSeriesVisible(seriesNum))
            {
               QList<double> yValues1, yValues2;
               mPlot->CalculateValueForSeries(seriesNum, mBoundaryTabValue1, yValues1);
               mPlot->CalculateValueForSeries(seriesNum, mBoundaryTabValue2, yValues2);

               for (QList<double>::iterator yIter = yValues1.begin(); yIter != yValues1.end(); ++yIter)
               {
                  if ((*yIter) <= mPlot->GetYMaxBoundary(series->mIsSecondaryAxis) &&
                      (*yIter) >= mPlot->GetYMinBoundary(series->mIsSecondaryAxis))
                  {
                     double yPosition = ((*yIter) - mPlot->GetYMinBoundary(series->mIsSecondaryAxis)) /
                                        (mPlot->GetYMaxBoundary(series->mIsSecondaryAxis) -
                                         mPlot->GetYMinBoundary(series->mIsSecondaryAxis));
                     yPosition =
                        yPosition * (mPlot->mPlotTop_pixels - mPlot->mPlotBottom_pixels) + mPlot->mPlotBottom_pixels;

                     // Display a Dot for the intersection for the 1st tab
                     QColor color = mPlot->GetSeriesColor(seriesIter.first);
                     glPointSize(series->mWidth + 4);
                     glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
                     glBegin(GL_POINTS);
                     {
                        glVertex2i(xValue1, yPosition);
                     }
                     glEnd();

                     QString text = QString::number(*yIter, 'g', mPlot->mSignificantDigits).toStdString().c_str();
                     if (mBoundaryTabDetailedLabels && yValues1.size() == 1 && yValues2.size() == 1)
                     {
                        double deltaY = yValues1.at(0) - yValues2.at(0);
                        if (xValue2 > xValue1)
                        {
                           deltaY = -1 * deltaY;
                        }
                        text = text + " dy:" + QString::number(deltaY) + " slope: " + QString::number(deltaY / deltaX);
                     }
                  }
               }

               for (QList<double>::iterator yIter = yValues2.begin(); yIter != yValues2.end(); ++yIter)
               {
                  if ((*yIter) <= mPlot->GetYMaxBoundary(series->mIsSecondaryAxis) &&
                      (*yIter) >= mPlot->GetYMinBoundary(series->mIsSecondaryAxis))
                  {
                     double yPosition = ((*yIter) - mPlot->GetYMinBoundary(series->mIsSecondaryAxis)) /
                                        (mPlot->GetYMaxBoundary(series->mIsSecondaryAxis) -
                                         mPlot->GetYMinBoundary(series->mIsSecondaryAxis));
                     yPosition =
                        yPosition * (mPlot->mPlotTop_pixels - mPlot->mPlotBottom_pixels) + mPlot->mPlotBottom_pixels;

                     // Display a Dot for the intersection for the 2nd tab
                     QColor color = mPlot->GetSeriesColor(seriesIter.first);
                     glPointSize(series->mWidth + 4);
                     glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
                     glBegin(GL_POINTS);
                     {
                        glVertex2i(xValue2, yPosition);
                     }
                     glEnd();

                     // Display Text for intersection for the 2nd tab
                     QString text = QString::number(*yIter, 'g', mPlot->mSignificantDigits).toStdString().c_str();
                     if (mBoundaryTabDetailedLabels && yValues1.size() == 1 && yValues2.size() == 1)
                     {
                        double deltaY = yValues2.at(0) - yValues1.at(0);
                        if (xValue1 > xValue2)
                        {
                           deltaY = -1 * deltaY;
                        }
                        text = text + " dy:" + QString::number(deltaY) + " slope: " + QString::number(deltaY / deltaX);
                     }
                  }
               }
            }
         }
      }
      glPopMatrix();
   }
}

void UtQtGL2DPlotWidget::DrawBoundaryTabsText(QPainter* aPainter)
{
   if (mBoundaryTabsEnabled)
   {
      QFont font = qApp->font();
      font.setPointSize(mPlot->cSeriesFontLabelFactor * mPlot->mLabelFontSize);
      aPainter->setFont(font);

      float tx = width() * 0.5;
      float ty = height() * 0.5;

      double xBoundMin, xBoundMax, xValue1, xValue2, yDummy;

      // Limit the BoundaryTabValues to the min/max Boundaries of the Plot
      mPlot->GetXAxisBounds(xBoundMin, xBoundMax);
      if (mBoundaryTabValue1 < xBoundMin)
      {
         SetBoundaryTab1(xBoundMin);
      }
      else if (mBoundaryTabValue1 > xBoundMax)
      {
         SetBoundaryTab1(xBoundMax);
      }
      if (mBoundaryTabValue2 < xBoundMin)
      {
         SetBoundaryTab2(xBoundMin);
      }
      else if (mBoundaryTabValue2 > xBoundMax)
      {
         SetBoundaryTab2(xBoundMax);
      }

      mPlot->TranslatePointFromRealToScene(0, mBoundaryTabValue1, 0, xValue1, yDummy);
      mPlot->TranslatePointFromRealToScene(0, mBoundaryTabValue2, 0, xValue2, yDummy);

      double heightInc = mDisplayHeight * .01;
      double widthInc  = mDisplayWidth * .01;

      mBoundaryTab1Points.clear();
      mBoundaryTab1Points << QPointF(xValue1, mPlot->mPlotBottom_pixels)
                          << QPointF(xValue1 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc)
                          << QPointF(xValue1 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc)
                          << QPointF(xValue1 - 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc)
                          << QPointF(xValue1 - 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);

      mBoundaryTab2Points.clear();
      mBoundaryTab2Points << QPointF(xValue2, mPlot->mPlotBottom_pixels)
                          << QPointF(xValue2 + 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc)
                          << QPointF(xValue2 + 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc)
                          << QPointF(xValue2 - 1 * widthInc, mPlot->mPlotBottom_pixels - 3 * heightInc)
                          << QPointF(xValue2 - 1 * widthInc, mPlot->mPlotBottom_pixels - heightInc);
      aPainter->setPen(Qt::red);

      double deltaX = std::abs(mBoundaryTabValue1 - mBoundaryTabValue2);
      mPlot->RenderText(aPainter,
                        (xValue1 + xValue2) / 2 + tx,
                        mPlot->mPlotBottom_pixels + heightInc + ty,
                        QString::number(deltaX, 'g', mPlot->mSignificantDigits),
                        Qt::AlignHCenter | Qt::AlignBottom,
                        0); // note that the translation is not handled here, yet

      // Now handle text displays
      const UtQtGL2DPlot::SeriesMap data = mPlot->GetData();
      QList<double>                 yValues;
      for (const auto& seriesIter : data)
      {
         const int                       seriesNum = seriesIter.first;
         const UtQtGL2DPlot::SeriesBase* series    = seriesIter.second;
         if (mPlot->GetSeriesVisible(seriesNum))
         {
            QList<double> yValues1, yValues2;
            mPlot->CalculateValueForSeries(seriesNum, mBoundaryTabValue1, yValues1);
            mPlot->CalculateValueForSeries(seriesNum, mBoundaryTabValue2, yValues2);

            for (QList<double>::iterator yIter = yValues1.begin(); yIter != yValues1.end(); ++yIter)
            {
               if ((*yIter) <= mPlot->GetYMaxBoundary(series->mIsSecondaryAxis) &&
                   (*yIter) >= mPlot->GetYMinBoundary(series->mIsSecondaryAxis))
               {
                  double yPosition =
                     ((*yIter) - mPlot->GetYMinBoundary(series->mIsSecondaryAxis)) /
                     (mPlot->GetYMaxBoundary(series->mIsSecondaryAxis) - mPlot->GetYMinBoundary(series->mIsSecondaryAxis));
                  yPosition = yPosition * (mPlot->mPlotTop_pixels - mPlot->mPlotBottom_pixels) + mPlot->mPlotBottom_pixels;

                  // Display a Dot for the intersection for the 1st tab
                  QColor color = mPlot->GetSeriesColor(seriesIter.first);
                  aPainter->setPen(color);

                  QString text = QString::number(*yIter, 'g', mPlot->mSignificantDigits).toStdString().c_str();
                  if (mBoundaryTabDetailedLabels && yValues1.size() == 1 && yValues2.size() == 1)
                  {
                     double deltaY = yValues1.at(0) - yValues2.at(0);
                     if (xValue2 > xValue1)
                     {
                        deltaY = -1 * deltaY;
                     }
                     text = text + " dy:" + QString::number(deltaY) + " slope: " + QString::number(deltaY / deltaX);
                  }

                  // Display Text for intersection for the 1st tab
                  if (xValue1 > (mPlot->mPlotLeft_pixels + mPlot->cRightLeftLabelTransition *
                                                              (mPlot->mPlotRight_pixels - mPlot->mPlotLeft_pixels)))
                  {
                     mPlot->RenderText(aPainter,
                                       xValue1 - mPlot->cSeriesFontLabelOffset + tx,
                                       yPosition + ty,
                                       text,
                                       Qt::AlignRight | Qt::AlignVCenter,
                                       0);
                  }
                  else
                  {
                     mPlot->RenderText(aPainter,
                                       xValue1 + mPlot->cSeriesFontLabelOffset + tx,
                                       yPosition + ty,
                                       text,
                                       Qt::AlignLeft | Qt::AlignVCenter,
                                       0);
                  }
               }
            }

            for (QList<double>::iterator yIter = yValues2.begin(); yIter != yValues2.end(); ++yIter)
            {
               if ((*yIter) <= mPlot->GetYMaxBoundary(series->mIsSecondaryAxis) &&
                   (*yIter) >= mPlot->GetYMinBoundary(series->mIsSecondaryAxis))
               {
                  double yPosition =
                     ((*yIter) - mPlot->GetYMinBoundary(series->mIsSecondaryAxis)) /
                     (mPlot->GetYMaxBoundary(series->mIsSecondaryAxis) - mPlot->GetYMinBoundary(series->mIsSecondaryAxis));
                  yPosition = yPosition * (mPlot->mPlotTop_pixels - mPlot->mPlotBottom_pixels) + mPlot->mPlotBottom_pixels;

                  // Display a Dot for the intersection for the 2nd tab
                  QColor color = mPlot->GetSeriesColor(seriesIter.first);
                  aPainter->setPen(color);

                  // Display Text for intersection for the 2nd tab
                  QString text = QString::number(*yIter, 'g', mPlot->mSignificantDigits).toStdString().c_str();
                  if (mBoundaryTabDetailedLabels && yValues1.size() == 1 && yValues2.size() == 1)
                  {
                     double deltaY = yValues2.at(0) - yValues1.at(0);
                     if (xValue1 > xValue2)
                     {
                        deltaY = -1 * deltaY;
                     }
                     text = text + " dy:" + QString::number(deltaY) + " slope: " + QString::number(deltaY / deltaX);
                  }

                  font.setPointSize(mPlot->cSeriesFontLabelFactor * mPlot->mLabelFontSize);
                  aPainter->setFont(font);
                  if (xValue2 > (mPlot->mPlotLeft_pixels + mPlot->cRightLeftLabelTransition *
                                                              (mPlot->mPlotRight_pixels - mPlot->mPlotLeft_pixels)))
                  {
                     mPlot->RenderText(aPainter,
                                       xValue2 - mPlot->cSeriesFontLabelOffset + tx,
                                       yPosition + ty,
                                       text,
                                       Qt::AlignRight | Qt::AlignVCenter,
                                       0);
                  }
                  else
                  {
                     mPlot->RenderText(aPainter,
                                       xValue2 + mPlot->cSeriesFontLabelOffset + tx,
                                       yPosition + ty,
                                       text,
                                       Qt::AlignLeft | Qt::AlignVCenter,
                                       0);
                  }
               }
            }
         }
      }
   }
}

void UtQtGL2DPlotWidget::DrawFocusIndicators()
{
   if (mDrawFocusIndicators && hasFocus())
   {
      glPushMatrix();
      {
         QColor color = GetBackgroundColor().darker();
         glColor4f(color.redF(), color.blueF(), color.greenF(), color.alphaF());

         glBegin(GL_TRIANGLES);
         {
            float size = (static_cast<float>(std::min(mDisplayWidth, mDisplayHeight))) / 25.0f;

            // Bottom left
            glVertex2i(0, 0);
            glVertex2i(size, 0);
            glVertex2i(0, size);
            // Bottom Right
            glVertex2i(mDisplayWidth, 0);
            glVertex2i(mDisplayWidth, size);
            glVertex2i(mDisplayWidth - size, 0);
            // Top Right
            glVertex2i(mDisplayWidth, mDisplayHeight);
            glVertex2i(mDisplayWidth - size, mDisplayHeight);
            glVertex2i(mDisplayWidth, mDisplayHeight - size);
            // Top Left
            glVertex2i(0, mDisplayHeight);
            glVertex2i(0, mDisplayHeight - size);
            glVertex2i(size, mDisplayHeight);
         }
         glEnd();
      }
      glPopMatrix();
   }
}

void UtQtGL2DPlotWidget::contextMenuEvent(QContextMenuEvent* event)
{
   if (mContextMenu != nullptr)
   {
      PopulateContextMenuActions();

      mContextMenu->move(event->globalPos());
      mContextMenu->show();
   }
}

void UtQtGL2DPlotWidget::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Delete)
   {
      if (mEditModeEnabled)
      {
         RemoveSelectedDataPoint();
      }
   }
   else if (event->key() == Qt::Key_Space)
   {
      if (mEditModeEnabled)
      {
         ChangeSelectedSeries();
      }
   }
   else if (event->key() == Qt::Key_A)
   {
      if (mEditModeEnabled)
      {
         AddDataPoint(ePOINT_FIRST);
      }
   }
   else if (event->key() == Qt::Key_S)
   {
      if (mEditModeEnabled)
      {
         AddDataPoint(ePOINT_MIDDLE);
      }
   }
   else if (event->key() == Qt::Key_D)
   {
      if (mEditModeEnabled)
      {
         AddDataPoint(ePOINT_LAST);
      }
   }
   else
   {
      event->ignore();
   }
}

void UtQtGL2DPlotWidget::mousePressEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton)
   {
      // If viewing data not of these action are applicable
      if (!mDataView)
      {
         if (!(BoundaryTabSelected(event) || DataPointSelected(event)))
         {
            // The legend's bounding box is return in Qt's Coordinate System which the top-left point is (0,0)
            int x = event->x();
            int y = event->y();

            // Check to see if this event should affect the legend
            if (mPlot->GetShowLegend() && mPlot->GetLegendRect().contains(x, y))
            {
               mButtonDownLegend       = true;
               mLegendSelectionXOffset = x - mPlot->GetLegendRect().topLeft().x();
               mLegendSelectionYOffset = y - mPlot->GetLegendRect().topLeft().y();
            }
            else
            {
               ZoomEvent(event);
            }
         }
      }
   }
}

bool UtQtGL2DPlotWidget::BoundaryTabSelected(QMouseEvent* event)
{
   if (mBoundaryTabsEnabled && mBoundaryTabsMouseEnabled)
   {
      int x = event->x() - mDisplayWidth / 2;
      int y = mDisplayHeight - event->y() - mDisplayHeight / 2;

      if (mBoundaryTab1Points.containsPoint(QPoint(x, y), Qt::OddEvenFill))
      {
         mBoundaryTab1Selected = true;
         return true;
      }
      else if (mBoundaryTab2Points.containsPoint(QPoint(x, y), Qt::OddEvenFill))
      {
         mBoundaryTab2Selected = true;
         return true;
      }
   }
   return false;
}

bool UtQtGL2DPlotWidget::DataPointSelected(QMouseEvent* event)
{
   if (mEditModeEnabled)
   {
      for (const auto& series : mPlot->GetData())
      {
         if (series.first == mCurrentSelectedSeries && series.second->mVisible)
         {
            int width = mPlot->GetSeriesWidth(series.first) +
                        3; // UtQtGL2DPlot::mPointWidthModifier/2 (needs a better way to do selections)
            int index = 0;

            int x = event->x() - mDisplayWidth / 2;
            int y = mDisplayHeight - event->y() - mDisplayHeight / 2;

            for (size_t i = 0; i < series.second->Size(); ++i)
            {
               double data_x = 0.0;
               double data_y = 0.0;
               series.second->Get(i, data_x, data_y);
               double pointX, pointY;

               mPlot->TranslatePointFromRealToScene(series.first, data_x, data_y, pointX, pointY);

               if (x >= pointX - width && x <= pointX + width && y >= pointY - width && y <= pointY + width)
               {
                  // Set point as selected
                  mDataPointSelected = true;
                  mDataPointIndex    = index;
                  mPlot->SetDataPointSelected(mDataPointSelected, mDataPointIndex, series.first);
                  update();
                  return true;
               }

               index++;
            }
            break;
         }
      }

      mDataPointSelected = false;
   }
   return false;
}

void UtQtGL2DPlotWidget::ZoomEvent(QMouseEvent* event)
{
   mButtonDownZooming = false;

   if (mEnableMouseZoom)
   {
      // translate x/y, plot coordinates have 0,0 in middle of screen
      int x = event->x() - mDisplayWidth / 2;
      int y = (mDisplayHeight - event->y()) - mDisplayHeight / 2;

      int plotLeft, plotRight, plotBottom, plotTop;
      mPlot->GetPlotBoundaries(plotLeft, plotRight, plotBottom, plotTop);

      // if left click occurred in the grid region
      if ((plotLeft <= x) && (x <= plotRight) && (plotBottom <= y) && (y <= plotTop))
      {
         mMousePressLocation   = QPoint(x, y);
         mMouseCurrentLocation = QPoint(x, y);
         mButtonDownZooming    = true;
      }
   }
   event->ignore(); // call ignore so the event propagates to the parent widget
}

void UtQtGL2DPlotWidget::mouseReleaseEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton)
   {
      mButtonDownLegend     = false;
      mBoundaryTab1Selected = false;
      mBoundaryTab2Selected = false;
      mDataPointSelected    = false;

      if (mEnableMouseZoom)
      {
         int releaseX = event->x() - mDisplayWidth / 2;
         int releaseY = mDisplayHeight - event->y() - mDisplayHeight / 2;

         if (mButtonDownZooming)
         {
            mButtonDownZooming = false;

            int plotLeft, plotRight, plotBottom, plotTop;
            mPlot->GetPlotBoundaries(plotLeft, plotRight, plotBottom, plotTop);

            if (releaseX < plotLeft)
            {
               releaseX = plotLeft;
            }
            else if (releaseX > plotRight)
            {
               releaseX = plotRight;
            }
            if (releaseY < plotBottom)
            {
               releaseY = plotBottom;
            }
            else if (releaseY > plotTop)
            {
               releaseY = plotTop;
            }

            // swap if X1 is greater than X2
            if (mMousePressLocation.x() > releaseX)
            {
               double temp = releaseX;
               releaseX    = mMousePressLocation.x();
               mMousePressLocation.setX(temp);
            }
            // swap if Y1 is greater than Y2
            if (mMousePressLocation.y() > releaseY)
            {
               double temp = releaseY;
               releaseY    = mMousePressLocation.y();
               mMousePressLocation.setY(temp);
            }

            // Limit zooming to 1/10th the plot per click. (10X zoom)
            if (std::abs(releaseY - mMousePressLocation.y()) > (.1 * (plotRight - plotLeft)) &&
                std::abs(releaseX - mMousePressLocation.x()) > (.1 * (plotTop - plotBottom)))
            {
               ZoomIn();
               mPreventUpdates = true;

               double minXBoundary, maxXBoundary, minYBoundary, maxYBoundary, minYBoundary2, maxYBoundary2;
               mPlot->GetXAxisBounds(minXBoundary, maxXBoundary);
               mPlot->GetYAxisBounds(minYBoundary, maxYBoundary);
               mPlot->GetYAxisBounds2(minYBoundary2, maxYBoundary2);

               double translatedX1 = ((maxXBoundary - minXBoundary) *
                                      (static_cast<double>(mMousePressLocation.x()) - static_cast<double>(plotLeft)) /
                                      static_cast<double>(plotRight - plotLeft)) +
                                     minXBoundary;
               double translatedX2 =
                  ((maxXBoundary - minXBoundary) * (static_cast<double>(releaseX) - static_cast<double>(plotLeft)) /
                   static_cast<double>(plotRight - plotLeft)) +
                  minXBoundary;
               double translatedY1 = ((maxYBoundary - minYBoundary) *
                                      (static_cast<double>(mMousePressLocation.y()) - static_cast<double>(plotBottom)) /
                                      static_cast<double>(plotTop - plotBottom)) +
                                     minYBoundary;
               double translatedY2 =
                  ((maxYBoundary - minYBoundary) * (static_cast<double>(releaseY) - static_cast<double>(plotBottom)) /
                   static_cast<double>(plotTop - plotBottom)) +
                  minYBoundary;
               double translatedY1_2 = ((maxYBoundary2 - minYBoundary2) *
                                        (static_cast<double>(mMousePressLocation.y()) - static_cast<double>(plotBottom)) /
                                        static_cast<double>(plotTop - plotBottom)) +
                                       minYBoundary2;
               double translatedY2_2 =
                  ((maxYBoundary2 - minYBoundary2) * (static_cast<double>(releaseY) - static_cast<double>(plotBottom)) /
                   static_cast<double>(plotTop - plotBottom)) +
                  minYBoundary2;
               mPlot->SetAxesBounds(translatedX1, translatedY1, translatedX2, translatedY2);
               mPlot->SetYAxisBounds2(translatedY1_2, translatedY2_2);

               // Keep the same number of tick marks as displayed prior to Zooming.
               if (mPreZoomUseTickIncrementX)
               {
                  mPlot->SetTickCountX(((mPreZoomMaxXBoundary - mPreZoomMinXBoundary) / mPreZoomTickIncrementX) + 1);
               }
               else
               {
                  mPlot->SetTickCountX(mPreZoomTickCountX);
               }
               if (mPreZoomUseTickIncrementY)
               {
                  mPlot->SetTickCountY(((mPreZoomMaxYBoundary - mPreZoomMinYBoundary) / mPreZoomTickIncrementY) + 1);
               }
               else
               {
                  mPlot->SetTickCountY(mPreZoomTickCountY);
               }

               if (mPreZoomUseTickIncrementY2)
               {
                  mPlot->SetTickCountY2(((mPreZoomMaxYBoundary2 - mPreZoomMinYBoundary2) / mPreZoomTickIncrementY2) + 1);
               }
               else
               {
                  mPlot->SetTickCountY2(mPreZoomTickCountY2);
               }
               mPreventUpdates = false;
            }
            update();
         }
      }
      event->ignore(); // call ignore so the event propagates to the parent widget
   }
}

void UtQtGL2DPlotWidget::mouseMoveEvent(QMouseEvent* event)
{
   // If viewing data not of these action are applicable
   if (!mDataView)
   {
      if (mMouseCrosshairsEnabled)
      {
         mPlot->SetCrosshairsPosition(static_cast<double>(event->x()) / mDisplayWidth);
      }

      if (mBoundaryTab1Selected || mBoundaryTab2Selected)
      {
         // Bound to the limits of the grid.
         int x = event->x() - mDisplayWidth / 2;

         int plotLeft, plotRight, plotBottom, plotTop;
         mPlot->GetPlotBoundaries(plotLeft, plotRight, plotBottom, plotTop);

         if (x < plotLeft)
         {
            x = plotLeft;
         }
         else if (x > plotRight)
         {
            x = plotRight;
         }

         double translatedX = 0;
         if (mBoundaryTabLockedToTickmarks)
         {
            double tickWidthOver2 = 0;
            if (mPlot->mXAxisTicks.size() > 2)
            {
               tickWidthOver2 = (mPlot->mXAxisTicks.at(1).first - mPlot->mXAxisTicks.at(0).first) / 2;
            }

            bool found = false;
            for (std::vector<UtQtGL2DPlot::Double2>::const_iterator iter = mPlot->mXAxisTicks.begin();
                 iter != mPlot->mXAxisTicks.end();
                 ++iter)
            {
               if ((iter->first + tickWidthOver2) >= x)
               {
                  translatedX = iter->second;
                  found       = true;
                  break;
               }
            }
            if (!found && !mPlot->mXAxisTicks.empty())
            {
               translatedX = mPlot->mXAxisTicks.rbegin()->second;
            }
         }
         else
         {
            double minXBoundary, maxXBoundary;
            mPlot->GetXAxisBounds(minXBoundary, maxXBoundary);
            translatedX = ((maxXBoundary - minXBoundary) * (static_cast<double>(x) - static_cast<double>(plotLeft)) /
                           static_cast<double>(plotRight - plotLeft)) +
                          minXBoundary;
         }

         if (mBoundaryTab1Selected)
         {
            SetBoundaryTab1(translatedX);
         }
         else if (mBoundaryTab2Selected)
         {
            SetBoundaryTab2(translatedX);
         }
      }

      if (mButtonDownLegend)
      {
         double xPercent = static_cast<double>(event->x() - mLegendSelectionXOffset) / mDisplayWidth;
         double yPercent = static_cast<double>(mDisplayHeight - event->y() + mLegendSelectionYOffset) / mDisplayHeight;

         // Prevent Legend from being dragged out of the view area.
         if (yPercent > 0.99)
         {
            yPercent = .99;
         }
         if (xPercent > 0.99)
         {
            xPercent = .99;
         }

         mPlot->SetLegendPosition(xPercent, yPercent);
      }

      if (mEditModeEnabled && mDataPointSelected)
      {
         // Bound to the limits of the grid.
         int x = event->x() - mDisplayWidth / 2;
         int y = mDisplayHeight - event->y() - mDisplayHeight / 2;

         int plotLeft, plotRight, plotBottom, plotTop;
         mPlot->GetPlotBoundaries(plotLeft, plotRight, plotBottom, plotTop);

         if (x < plotLeft)
         {
            x = plotLeft;
         }
         else if (x > plotRight)
         {
            x = plotRight;
         }
         if (y < plotBottom)
         {
            y = plotBottom;
         }
         else if (y > plotTop)
         {
            y = plotTop;
         }

         double minXBoundary, maxXBoundary, minYBoundary, maxYBoundary;
         mPlot->GetXAxisBounds(minXBoundary, maxXBoundary);

         if (mPlot->GetSeriesUseSecondaryAxis(mCurrentSelectedSeries))
         {
            mPlot->GetYAxisBounds2(minYBoundary, maxYBoundary);
         }
         else
         {
            mPlot->GetYAxisBounds(minYBoundary, maxYBoundary);
         }

         double translatedX = ((maxXBoundary - minXBoundary) * (static_cast<double>(x) - static_cast<double>(plotLeft)) /
                               static_cast<double>(plotRight - plotLeft)) +
                              minXBoundary;
         double translatedY = ((maxYBoundary - minYBoundary) * (static_cast<double>(y) - static_cast<double>(plotBottom)) /
                               static_cast<double>(plotTop - plotBottom)) +
                              minYBoundary;

         if (mMinIncrementX != 0)
         {
            translatedX = floor((translatedX + (mMinIncrementX * .5)) / mMinIncrementX) * mMinIncrementX;
         }
         if (mMinIncrementY != 0 && !mPlot->GetSeriesUseSecondaryAxis(mCurrentSelectedSeries))
         {
            translatedY = floor((translatedY + (mMinIncrementY * .5)) / mMinIncrementY) * mMinIncrementY;
         }
         else if (mMinIncrementY2 != 0 && mPlot->GetSeriesUseSecondaryAxis(mCurrentSelectedSeries))
         {
            translatedY = floor((translatedY + (mMinIncrementY2 * .5)) / mMinIncrementY2) * mMinIncrementY2;
         }

         std::pair<double, double> point;
         if (mEditModeLockX && GetDataPoint(mCurrentSelectedSeries, mDataPointIndex, point))
         {
            translatedX = point.first;
         }
         if (mEditModeLockY && GetDataPoint(mCurrentSelectedSeries, mDataPointIndex, point))
         {
            translatedY = point.second;
         }

         mPlot->ChangePoint(mDataPointIndex, translatedX, translatedY, mCurrentSelectedSeries);
      }
      else if (mEnableMouseZoom && mButtonDownZooming)
      {
         // Bound to the limits of the grid.
         int x = event->x() - mDisplayWidth / 2;
         int y = mDisplayHeight - event->y() - mDisplayHeight / 2;

         int plotLeft, plotRight, plotBottom, plotTop;
         mPlot->GetPlotBoundaries(plotLeft, plotRight, plotBottom, plotTop);

         if (x < plotLeft)
         {
            x = plotLeft;
         }
         else if (x > plotRight)
         {
            x = plotRight;
         }
         if (y < plotBottom)
         {
            y = plotBottom;
         }
         else if (y > plotTop)
         {
            y = plotTop;
         }

         mMouseCurrentLocation = QPoint(x, y);
      }

      update();
   }
}

void UtQtGL2DPlotWidget::wheelEvent(QWheelEvent* event)
{
   // If viewing data not of these action are applicable
   if (!mDataView)
   {
      if (mEnableMouseZoom && !mButtonDownZooming)
      {
         double numDegrees = event->delta() / 8.0;
         double numSteps   = numDegrees / 15.0;

         double xMin, xMax, yMin, yMax, yMin2, yMax2;
         mPlot->GetXAxisBounds(xMin, xMax);
         mPlot->GetYAxisBounds(yMin, yMax);
         mPlot->GetYAxisBounds2(yMin2, yMax2);

         // Get cursor position to center zoom on the cursor
         int releaseX = event->x() - mDisplayWidth / 2;
         int releaseY = mDisplayHeight - event->y() - mDisplayHeight / 2;

         int plotLeft, plotRight, plotBottom, plotTop;
         mPlot->GetPlotBoundaries(plotLeft, plotRight, plotBottom, plotTop);

         if (releaseX < plotLeft)
         {
            releaseX = plotLeft;
         }
         else if (releaseX > plotRight)
         {
            releaseX = plotRight;
         }
         if (releaseY < plotBottom)
         {
            releaseY = plotBottom;
         }
         else if (releaseY > plotTop)
         {
            releaseY = plotTop;
         }

         // Extremely fast scrolling back and forth can cause min/max to get out of sync.
         if (xMin > xMax)
         {
            double temp = xMin;
            xMin        = xMax;
            xMax        = temp;
         }
         if (yMin > yMax)
         {
            double temp = yMin;
            yMin        = yMax;
            yMax        = temp;
         }
         if (yMin2 > yMax2)
         {
            double temp = yMin2;
            yMin2       = yMax2;
            yMax2       = temp;
         }

         double multiplier = .4;                                          // zoom in 40%
         double xDelta     = xMax - xMin;                                 // current x range
         double yDelta     = yMax - yMin;                                 // current y range
         double yDelta2    = yMax2 - yMin2;                               // current y range
         double newXDelta  = xDelta - (multiplier * xDelta * numSteps);   // new x range
         double newYDelta  = yDelta - (multiplier * yDelta * numSteps);   // new y range
         double newYDelta2 = yDelta2 - (multiplier * yDelta2 * numSteps); // new y range
         double percentX =
            ((double)releaseX - plotLeft) / (plotRight - plotLeft); // release point is terms of % of x axis, 0 to 1
         double percentY =
            ((double)releaseY - plotBottom) / (plotTop - plotBottom); // release point is terms of % of y axis, 0 to 1
         double gridReleaseX  = percentX * xDelta + xMin;             // release point in range of xMin to xMax
         double gridReleaseY  = percentY * yDelta + yMin;             // release point in range of yMin to yMax
         double gridReleaseY2 = percentY * yDelta2 + yMin2;           // release point in range of yMin to yMax

         // compute the new min/max values
         xMin  = (gridReleaseX - percentX * newXDelta);
         xMax  = (gridReleaseX + (1 - percentX) * newXDelta);
         yMin  = (gridReleaseY - percentY * newYDelta);
         yMax  = (gridReleaseY + (1 - percentY) * newYDelta);
         yMin2 = (gridReleaseY2 - percentY * newYDelta2);
         yMax2 = (gridReleaseY2 + (1 - percentY) * newYDelta2);

         // if zooming out, check if new range is larger than the pre-zoom range, exit zoom mode and return to pre-zoom values
         if (numDegrees < 0 && // zooming out is negative
             (newXDelta >= mPreZoomMaxXBoundary - mPreZoomMinXBoundary ||
              newYDelta >= mPreZoomMaxYBoundary - mPreZoomMinYBoundary)) // Dont need to check Y2
         {
            // Exit zoom, since the zoom area is now larger than the original area.
            if (mZoomed)
            {
               EndZoom();
            }
         }
         // Else zoom in/out as long as greater than 1 / 1000th (max zoom of 1000x)
         //  Shouldn't need to check both X and Y since scaling is % base.
         else if (newXDelta > ((mPreZoomMaxXBoundary - mPreZoomMinXBoundary) / 1000))
         {
            ZoomIn(xMin, xMax, yMin, yMax, yMin2, yMax2);
         }

         update();
      }
   }
}

void UtQtGL2DPlotWidget::StorePreZoomValues()
{
   if (!mZoomed)
   {
      mPreZoomAutoUpdatePlot = mPlot->GetAutoScaledEnabled();

      mPlot->GetXAxisBounds(mPreZoomMinXBoundary, mPreZoomMaxXBoundary);
      mPlot->GetYAxisBounds(mPreZoomMinYBoundary, mPreZoomMaxYBoundary);
      mPlot->GetYAxisBounds2(mPreZoomMinYBoundary2, mPreZoomMaxYBoundary2);
      mPreZoomUseTickIncrementX  = mPlot->GetTickIncrementX() != 0;
      mPreZoomUseTickIncrementY  = mPlot->GetTickIncrementY() != 0;
      mPreZoomUseTickIncrementY2 = mPlot->GetTickIncrementY2() != 0;

      if (mPreZoomUseTickIncrementX)
      {
         mPreZoomTickIncrementX = mPlot->GetTickIncrementX();
      }
      else
      {
         mPreZoomTickCountX = mPlot->GetTickCountX();
      }

      if (mPreZoomUseTickIncrementY)
      {
         mPreZoomTickIncrementY = mPlot->GetTickIncrementY();
      }
      else
      {
         mPreZoomTickCountY = mPlot->GetTickCountY();
      }

      if (mPreZoomUseTickIncrementY2)
      {
         mPreZoomTickIncrementY2 = mPlot->GetTickIncrementY2();
      }
      else
      {
         mPreZoomTickCountY2 = mPlot->GetTickCountY2();
      }
   }
}

void UtQtGL2DPlotWidget::ZoomIn()
{
   StorePreZoomValues();
   mZoomed = true;
   mPlot->SetAutoScaleEnabled(false);
}

void UtQtGL2DPlotWidget::ZoomIn(double aMinX, double aMaxX, double aMinY, double aMaxY, double aMinY2, double aMaxY2)
{
   if (mZoomed)
   {
      EndZoom();
   }

   ZoomIn();
   mPreventUpdates = true;

   mPlot->MakePlotPrettyX(aMinX, aMaxX);
   mPlot->MakePlotPrettyY(aMinY, aMaxY, aMinY2, aMaxY2);

   mPreventUpdates = false;
}

void UtQtGL2DPlotWidget::EndZoom()
{
   if (mZoomed)
   {
      mZoomed         = false;
      mPreventUpdates = true;

      mPlot->SetAutoScaleEnabled(mPreZoomAutoUpdatePlot);
      mPlot->SetAxesBounds(mPreZoomMinXBoundary, mPreZoomMinYBoundary, mPreZoomMaxXBoundary, mPreZoomMaxYBoundary);
      mPlot->SetYAxisBounds2(mPreZoomMinYBoundary2, mPreZoomMaxYBoundary2);
      // restore tick mark properties.
      if (mPreZoomUseTickIncrementX)
      {
         mPlot->SetTickIncrementX(mPreZoomTickIncrementX);
      }
      else
      {
         mPlot->SetTickCountX(mPreZoomTickCountX);
      }

      if (mPreZoomUseTickIncrementY)
      {
         mPlot->SetTickIncrementY(mPreZoomTickIncrementY);
      }
      else
      {
         mPlot->SetTickCountY(mPreZoomTickCountY);
      }

      if (mPreZoomUseTickIncrementY2)
      {
         mPlot->SetTickIncrementY2(mPreZoomTickIncrementY2);
      }
      else
      {
         mPlot->SetTickCountY2(mPreZoomTickCountY2);
      }
      mPreventUpdates = false;
   }
}

void UtQtGL2DPlotWidget::resizeGL(int aWidth, int aHeight)
{
   mPlot->SetWindowSize(aWidth, aHeight);
   UtQtGLWidgetBase::resizeGL(aWidth, aHeight);
}

void UtQtGL2DPlotWidget::initializeGL()
{
   UtQtGLWidgetBase::initializeGL();
   mPlot->Initialize();
}

void UtQtGL2DPlotWidget::PaletteChangedCB(const QPalette& aPalette)
{
   if (mUseApplicationPalette)
   {
      ApplyPalette(aPalette);
   }
}

void UtQtGL2DPlotWidget::PlotBoundariesChangedCB()
{
   // If plot boundaries changed and was not due to it being set via this class
   if (!mPreventUpdates)
   {
      EndZoom();
      update();
   }
}

void UtQtGL2DPlotWidget::HandleSeriesSelected(unsigned int aSeriesNum)
{
   mCurrentSelectedSeries = aSeriesNum;
   update();
}

void UtQtGL2DPlotWidget::SetEnableEditMode(bool aInput)
{
   mEditModeEnabled = aInput;
   // Dont select a series by default, force user to select the series
   // if(mEditModeEnabled && mCurrentSelectedSeries == -1)
   //{
   // Highlight the a series by setting the points to be displayed.
   //    ChangeSelectedSeries();
   // }
}

void UtQtGL2DPlotWidget::AddDataPoint(pointPlacement aPlacement)
{
   int x, y, plotLeft, plotRight, plotBottom, plotTop;
   mPlot->GetPlotBoundaries(plotLeft, plotRight, plotBottom, plotTop);

   QPoint p = mapFromGlobal(QCursor::pos());
   x        = p.x() - mDisplayWidth / 2;
   y        = mDisplayHeight - p.y() - mDisplayHeight / 2;

   if (x < plotLeft)
   {
      x = plotLeft;
   }
   else if (x > plotRight)
   {
      x = plotRight;
   }
   if (y < plotBottom)
   {
      y = plotBottom;
   }
   else if (y > plotTop)
   {
      y = plotTop;
   }

   double minXBoundary, maxXBoundary, minYBoundary, maxYBoundary;
   mPlot->GetXAxisBounds(minXBoundary, maxXBoundary);

   if (mPlot->GetSeriesUseSecondaryAxis(mCurrentSelectedSeries))
   {
      mPlot->GetYAxisBounds2(minYBoundary, maxYBoundary);
   }
   else
   {
      mPlot->GetYAxisBounds(minYBoundary, maxYBoundary);
   }

   double translatedX =
      ((maxXBoundary - minXBoundary) * (x - plotLeft) / static_cast<double>(plotRight - plotLeft)) + minXBoundary;
   double translatedY =
      ((maxYBoundary - minYBoundary) * (y - plotBottom) / static_cast<double>(plotTop - plotBottom)) + minYBoundary;

   if (aPlacement == ePOINT_FIRST)
   {
      mDataPointSelected = true;
      mDataPointIndex    = 0;
      mPlot->InsertPoint(mDataPointIndex, translatedX, translatedY, mCurrentSelectedSeries);
      mPlot->SetDataPointSelected(mDataPointSelected, mDataPointIndex, mCurrentSelectedSeries);
   }
   else if (aPlacement == ePOINT_MIDDLE)
   {
      if (mDataPointIndex != -1 && mPlot->GetData().count(mCurrentSelectedSeries) != 0)
      {
         size_t i     = 0;
         int    count = 0;

         while (count != mDataPointIndex && i < mPlot->GetData().at(mCurrentSelectedSeries)->Size())
         {
            ++i;
            ++count;
         }

         if (i < mPlot->GetData().at(mCurrentSelectedSeries)->Size())
         {
            double x1 = 0.0;
            double y1 = 0.0;
            mPlot->GetData().at(mCurrentSelectedSeries)->Get(i, x1, y1);
            ++i;
            if (i < mPlot->GetData().at(mCurrentSelectedSeries)->Size())
            {
               double x2 = 0.0;
               double y2 = 0.0;
               mPlot->GetData().at(mCurrentSelectedSeries)->Get(i, x2, y2);

               mDataPointSelected = true;
               mDataPointIndex++;
               mPlot->InsertPoint(mDataPointIndex, (x1 + x2) / 2, (y1 + y2) / 2, mCurrentSelectedSeries);
               mPlot->SetDataPointSelected(mDataPointSelected, mDataPointIndex, mCurrentSelectedSeries);
            }
         }
      }
   }
   else
   {
      if (mPlot->GetData().count(mCurrentSelectedSeries) != 0)
      {
         mDataPointSelected = true;
         mDataPointIndex    = static_cast<int>(mPlot->GetData().at(mCurrentSelectedSeries)->Size());
         mPlot->AddPoint(translatedX, translatedY, mCurrentSelectedSeries);
         mPlot->SetDataPointSelected(mDataPointSelected, mDataPointIndex, mCurrentSelectedSeries);
      }
   }
   update();
}

void UtQtGL2DPlotWidget::RemoveSelectedDataPoint()
{
   int selectedIndex = mPlot->GetSelectedDataPoint(mCurrentSelectedSeries);
   if (mEditModeEnabled && selectedIndex != -1)
   {
      mPlot->RemovePoint(selectedIndex, mCurrentSelectedSeries);

      mDataPointIndex = -1;
      mPlot->SetSeriesSelected(true, mCurrentSelectedSeries);
      mPlot->SetSeriesShowDataPoints(true, mCurrentSelectedSeries);

      update();
   }
}

void UtQtGL2DPlotWidget::ChangeSelectedSeries()
{
   auto iter = mPlot->GetData().begin();
   // No data in plot
   if (iter == mPlot->GetData().end())
   {
      mCurrentSelectedSeries = -1;
   }
   else
   {
      // Identify the current selected series.
      for (; iter != mPlot->GetData().end(); ++iter)
      {
         if (iter->first == mCurrentSelectedSeries)
         {
            break;
         }
      }

      // Series was not found, select the first series
      if (iter == mPlot->GetData().end())
      {
         mCurrentSelectedSeries = mPlot->GetData().begin()->first;
         mPlot->SetSeriesSelected(true, mCurrentSelectedSeries);
         mPlot->SetSeriesShowDataPoints(true, mCurrentSelectedSeries);
      }
      else // Series was found
      {
         // Turn off show points
         mPlot->SetSeriesSelected(false, mCurrentSelectedSeries);
         mPlot->SetSeriesShowDataPoints(false, mCurrentSelectedSeries);
         // increment selected series
         ++iter;
         // If at end, set selected series to none
         if (iter == mPlot->GetData().end())
         {
            mCurrentSelectedSeries = -1;
         }
         else
         {
            mCurrentSelectedSeries = iter->first;
            // Turn on show points for selected series
            mPlot->SetSeriesSelected(true, mCurrentSelectedSeries);
            mPlot->SetSeriesShowDataPoints(true, mCurrentSelectedSeries);
         }
      }
   }
   update();
}

void UtQtGL2DPlotWidget::SetBoundaryTab1(double aValue)
{
   mBoundaryTabValue1 = aValue;
   emit BoundaryTabPositionChanged();
}

void UtQtGL2DPlotWidget::SetBoundaryTab2(double aValue)
{
   mBoundaryTabValue2 = aValue;
   emit BoundaryTabPositionChanged();
}

void UtQtGL2DPlotWidget::SetEnableCrosshairMouseControlled(bool aEnabled)
{
   setMouseTracking(aEnabled);
   mPlot->SetShowCrosshairs(aEnabled);
   mMouseCrosshairsEnabled = aEnabled;
}

bool UtQtGL2DPlotWidget::GetDataPoint(unsigned int aSeriesNum, unsigned int index, std::pair<double, double>& aPoint)
{
   if (mPlot->GetData().count(aSeriesNum) != 0)
   {
      if (mPlot->GetData().at(aSeriesNum)->Size() > index)
      {
         mPlot->GetData().at(aSeriesNum)->Get(index, aPoint.first, aPoint.second);
         return true;
      }
   }
   return false;
}

void UtQtGL2DPlotWidget::ApplyPalette(const QPalette& aPalette)
{
   const QColor& color = aPalette.color(QPalette::Background);
   SetBackgroundColor(color);
   mPlot->SetLegendBackgroundColor(color);
   mPlot->SetPlotBackgroundColor(color.lighter());
   mPlot->SetTextColor(aPalette.color(QPalette::Text));
}

void UtQtGL2DPlotWidget::DeleteContextMenu()
{
   if (mContextMenu != nullptr)
   {
      delete mContextMenu;
      mContextMenu = nullptr;
   }
}

QList<QAction*> UtQtGL2DPlotWidget::GetDefaultContextMenuActions()
{
   QList<QAction*> actions;

   // Create option to move to Plot View page
   QAction* dataViewAction = new QAction(this);
   actions.push_back(dataViewAction);
   connect(dataViewAction, &QAction::triggered, this, &UtQtGL2DPlotWidget::DataViewAction);

   // Create option to export data
   QAction* exportAction = new QAction(this);
   actions.push_back(exportAction);
   exportAction->setText("Export Data");
   connect(exportAction, &QAction::triggered, this, &UtQtGL2DPlotWidget::ExportDataAction);

   if (mDataView)
   {
      // Rename the dataViewAction appropriately
      dataViewAction->setText("Plot View");
   }
   else
   {
      // Rename the dataViewAction appropriately
      dataViewAction->setText("Data View");

      // Create option to toggle the legend
      QAction* legendAction = new QAction(this);
      if (mPlot->GetShowLegend())
      {
         legendAction->setText("Hide Legend");
      }
      else
      {
         legendAction->setText("Show Legend");
      }
      connect(legendAction, &QAction::triggered, this, &UtQtGL2DPlotWidget::LegendAction);
      actions.push_back(legendAction);

      // Create option to change series colors on plot
      QAction* changeSeriesColorsAction = new QAction(this);
      changeSeriesColorsAction->setText("Series Color(s)");
      connect(changeSeriesColorsAction, &QAction::triggered, this, &UtQtGL2DPlotWidget::ChangeSeriesColorsAction);
      actions.push_back(changeSeriesColorsAction);
   }
   return actions;
}


void UtQtGL2DPlotWidget::SetEnableDefaultContextMenu(bool aEnable)
{
   DeleteContextMenu();

   if (aEnable)
   {
      mContextMenu = new QMenu();
   }
}

void UtQtGL2DPlotWidget::PopulateContextMenuActions()
{
   mContextMenu->clear();
   mContextMenu->addActions(GetDefaultContextMenuActions());
}

void UtQtGL2DPlotWidget::LegendAction()
{
   mPlot->SetShowLegend(!mPlot->GetShowLegend());
   update();
}

void UtQtGL2DPlotWidget::DataViewAction()
{
   mDataView = !mDataView;
   if (mDataView)
   {
      PopulateDataTable();
      mDataScrollArea->show();
   }
   else
   {
      mDataScrollArea->hide();
   }
   update();
}

void UtQtGL2DPlotWidget::ExportDataAction()
{
   // Generate a default name to suggest to the user
   QString prefix = mPlot->GetLabelText(UtQtGL2DPlot::TITLE);
   if (prefix == "")
   {
      prefix = "DataExported";
   }
   QString filename = "./" + prefix + "_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".csv";
   filename.replace(" ", "_");

   // Ask user for file location and name
   filename = QFileDialog::getSaveFileName(this,
                                           QString("Export to CSV"),
                                           filename,
                                           QString("CSV (*.csv);; Text (*.txt);; All Files(*.*)"));

   // If filename is not empty attempt to WriteDataToFile(), if WriteDataToFile() fails, display error message.
   if (filename != "" && !mPlot->WriteDataToFile(filename))
   {
      // Display error message
      QMessageBox::warning(this, "File Write Error", "There was an error writing the file:\n" + filename + ".");
   }
}

// Creates a dialog box and populates the box with the currently selected series' and their
// respective colors which can be changed by clicking their buttons.
void UtQtGL2DPlotWidget::ChangeSeriesColorsAction()
{
   // Creates a dialog box and makes it modal(blocks any action outside the dialog)
   QDialog* seriesColorsDialog = new QDialog(this, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
   seriesColorsDialog->setWindowModality(Qt::WindowModality::WindowModal);
   seriesColorsDialog->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

   // Creates the tree widget that will contain all the series' and thier colors
   mSeriesColorsList = new QTreeWidget();
   mSeriesColorsList->setRootIsDecorated(false);
   mSeriesColorsList->setColumnCount(2);
   QStringList columnNames;
   columnNames << "Series Name"
               << "Color";
   mSeriesColorsList->setHeaderLabels(columnNames);

   // Loops through the selected series'
   for (const auto& series : mPlot->GetData())
   {
      // checks that there is data attached to the series
      if (mPlot->GetSeriesVisible(series.first) && !series.second->IsEmpty())
      {
         // adds series(treeWidgetItem) to tree, names the item after the series it represents, and then creates a color
         // button to allow user to change the color of the series.
         QTreeWidgetItem* seriesItem =
            new QTreeWidgetItem(mSeriesColorsList, QStringList(mPlot->GetSeriesName(series.first)));
         SetColor(series.first, seriesItem, mPlot->GetSeriesColor(series.first));
      }
   }

   mSeriesColorsList->resizeColumnToContents(0);
   mSeriesColorsList->resizeColumnToContents(1);

   // Creates the vertical layout
   QVBoxLayout* vertical_layout = new QVBoxLayout(seriesColorsDialog);
   vertical_layout->setObjectName(QStringLiteral("verticalboxlayout"));

   // Creates the horizontal layout
   QHBoxLayout* horizontal_layout = new QHBoxLayout(seriesColorsDialog);
   horizontal_layout->setObjectName(QStringLiteral("horizontalboxlayout"));

   // Creates the spacer and close button
   QSpacerItem* horizontal_spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
   QPushButton* closeButton       = new QPushButton(QString("Close"), seriesColorsDialog);

   // Connects the close button to a lambda that closes the dialog
   connect(closeButton, &QPushButton::clicked, [=]() { seriesColorsDialog->close(); });

   // adds spacer and button to the horizontal layout
   horizontal_layout->addItem(horizontal_spacer);
   horizontal_layout->addWidget(closeButton);

   // adds the selected series and respective colorbuttons(treeWidget/treeWidgetItems). Also adds the horizontal layout
   // containing the spacer and close button to the vertical layout.
   vertical_layout->addWidget(mSeriesColorsList);
   vertical_layout->addItem(horizontal_layout);

   // opens the dialog box.
   seriesColorsDialog->exec();
}

// Used to populate the 'Series Colors' dialog box with appropriately colored color buttons
// to change the respective series color.
void UtQtGL2DPlotWidget::SetColor(int seriesNum, QTreeWidgetItem* aItem, const QColor& aColor) const
{
   UtQtColorButton* colorButton = dynamic_cast<UtQtColorButton*>(mSeriesColorsList->itemWidget(aItem, 1));

   if (nullptr == colorButton)
   {
      colorButton = new UtQtColorButton(mSeriesColorsList);
      colorButton->setProperty("SeriesNum", seriesNum);
      mSeriesColorsList->setItemWidget(aItem, 1, colorButton);
      colorButton->SetColor(aColor);
   }
   if (nullptr != colorButton)
   {
      colorButton->SetColor(aColor);
   }

   // Connects a button's 'ColorChanged' signal to this lambda to change the color
   // of the repsective series.
   connect(colorButton,
           &UtQtColorButton::ColorChanged,
           [=](const QColor& color)
           {
              colorButton->SetColor(color);
              mPlot->SetSeriesColor(color, colorButton->property("SeriesNum").toInt());
           });
}

void UtQtGL2DPlotWidget::SetShowDataView(bool aShow)
{
   mDataView = aShow;
   if (mDataView)
   {
      PopulateDataTable();
      mDataScrollArea->show();
   }
   else
   {
      mDataScrollArea->hide();
   }
}

void UtQtGL2DPlotWidget::UpdateDataTable()
{
   if (mDataView)
   {
      PopulateDataTable();
   }
}

void UtQtGL2DPlotWidget::PopulateDataTable()
{
   QHBoxLayout* dataWidgetLayout = new QHBoxLayout();
   mDataWidget                   = new QWidget();

   mDataScrollArea->setWidgetResizable(true);
   mDataScrollArea->setWidget(mDataWidget);
   mDataWidget->setLayout(dataWidgetLayout);

   UtQtGL2DPlot::SeriesMap data = mPlot->GetData();
   for (const auto& series : data)
   {
      if (mPlot->GetSeriesVisible(series.first) && !series.second->IsEmpty())
      {
         QWidget*     widget = new QWidget(mDataWidget);
         QVBoxLayout* layout = new QVBoxLayout();
         widget->setLayout(layout);
         QLabel* label = new QLabel(widget);
         label->setText("Series: " + series.second->Name());
         label->setStyleSheet("QLabel { color : " + series.second->mColor.name() + "; }");
         layout->addWidget(label);
         mDataWidget->layout()->addWidget(widget);

         QTableWidget* table = new QTableWidget(widget);
         table->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
         widget->layout()->addWidget(table);
         table->setRowCount(static_cast<int>(series.second->Size()));

         bool threeColumns = series.second->HasZ();

         QStringList headerStrings;
         QString     xText = mPlot->GetLabelText(UtQtGL2DPlot::BOTTOM);
         if (xText == "")
         {
            xText = "X";
         }
         headerStrings.push_back(xText);

         QString yText = "";
         if (series.second->mIsSecondaryAxis)
         {
            yText = mPlot->GetLabelText(UtQtGL2DPlot::RIGHT);
         }
         else
         {
            yText = mPlot->GetLabelText(UtQtGL2DPlot::LEFT);
         }
         if (yText == "")
         {
            yText = "Y";
         }
         headerStrings.push_back(yText);

         if (threeColumns)
         {
            table->setColumnCount(3);
            headerStrings.push_back("Color Value");
         }
         else
         {
            table->setColumnCount(2);
         }
         table->setHorizontalHeaderLabels(headerStrings);

         int row = 0;
         for (size_t i = 0; i < series.second->Size(); ++i)
         {
            double x = 0.0;
            double y = 0.0;
            series.second->Get(i, x, y);
            table->setItem(row, 0, new QTableWidgetItem(QString::number(x)));
            table->setItem(row, 1, new QTableWidgetItem(QString::number(y)));
            if (threeColumns)
            {
               double z = 0.0;
               series.second->GetZ(i, z);
               table->setItem(row, 2, new QTableWidgetItem(QString::number(z)));
            }
            row++;
         }

         table->resizeColumnToContents(0);
         table->resizeColumnToContents(1);
         table->resizeColumnToContents(2);

         table->setMaximumWidth(18 + table->verticalHeader()->width() + table->columnWidth(0) + table->columnWidth(1) +
                                table->columnWidth(2));
      }
   }

   mDataWidget->layout()->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
   dataWidgetLayout->setStretch(2, 16);
}

QPointF UtQtGL2DPlotWidget::ConvertPositionToData(const QPoint& aPoint) const
{
   QPointF check = aPoint;
   int     left;
   int     right;
   int     top;
   int     bottom;
   mPlot->GetPlotBoundaries(left, right, bottom, top);
   left += width() * 0.5;
   right += width() * 0.5;
   top += height() * 0.5;
   bottom += height() * 0.5;
   check.setX((check.x() > left) ? check.x() : left);
   check.setX((check.x() < right) ? check.x() : right);
   check.setY((check.y() > bottom) ? check.y() : bottom);
   check.setY((check.y() < top) ? check.y() : top);
   double minX;
   double minY;
   double maxX;
   double maxY;
   mPlot->GetXAxisBounds(minX, maxX);
   mPlot->GetYAxisBounds(minY, maxY);
   check.setX((check.x() - left) / (right - left) * (maxX - minX) + minX);
   check.setY((check.y() - bottom) / (top - bottom) * (maxY - minY) + minY);
   return check;
}
