// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtQtGL2DPlotTP.hpp"

#include "UtCast.hpp"
#include "UtMath.hpp"

UtQtGL2DPlotTP::UtQtGL2DPlotTP()
   : UtQtGL2DPlot()
   , mShowBackgroundCurves(false)
   , mSpeedFPS_Multiplier(1.0)
   , mPreviousX(MinMaxPair(0, DBL_MAX))
   , mPreviousY(MinMaxPair(0, DBL_MAX))
{
   PopulateG_Curves();
   PopulateRadiusCurves();
}

void UtQtGL2DPlotTP::Reset()
{
   mShowBackgroundCurves = false;
   mSpeedFPS_Multiplier  = 1.0;
   mPreviousX            = MinMaxPair(0, DBL_MAX);
   mPreviousY            = MinMaxPair(0, DBL_MAX);

   UtQtGL2DPlot::Reset();
}

void UtQtGL2DPlotTP::SetSpeedUnits(UtUnitSpeed::BaseUnit aSpeedUnit)
{
   // set current unit and convert the X values on the curves.
   double value = UtUnitSpeed::ConvertToStandard(1.0, UtUnitSpeed::cFEET_PER_SECOND);
   SetSpeedMultiplier(UtUnitSpeed::ConvertFromStandard(value, aSpeedUnit));
}

void UtQtGL2DPlotTP::SetSpeedMultiplier(double aSpeedMultiplier)
{
   if (aSpeedMultiplier != 0.0 && aSpeedMultiplier != mSpeedFPS_Multiplier)
   {
      double multiplier = aSpeedMultiplier / mSpeedFPS_Multiplier;

      for (auto& series : mG_Curves)
      {
         for (auto& dataPt : series.second->data)
         {
            dataPt.x *= multiplier;
         }
      }

      for (auto& series : mRadius_Curves)
      {
         for (auto& dataPt : series.second->data)
         {
            dataPt.x *= multiplier;
         }
      }

      mSpeedFPS_Multiplier = aSpeedMultiplier;
   }
}

void UtQtGL2DPlotTP::ComputeCurveLimits()
{
   double yMaxX, yMaxY, xMaxX, xMaxY;
   // Find the X value of max Y value
   // Find the Y value of the max X value.
   if (GetMaxXandYPoints(xMaxX, xMaxY, yMaxX, yMaxY))
   {
      // Compute the radius for these two points.
      //  xValue = Vel and yValue = TurnRate
      //  R = (360 / 2PI) * (Vel / TurnRate)
      double temp       = 360 / (2 * UtMath::cPI);
      double xMaxRadius = temp * xMaxX / xMaxY;
      double yMaxRadius = temp * yMaxX / yMaxY;

      int yMaxIndex = -1; // This is the min Radius line necessary for the plot
      int xMaxIndex = -1; // The is the max Radius line necessary for the plot

      for (auto& pair : mRadius_Values)
      {
         if (yMaxRadius < pair.second)
         {
            yMaxIndex = pair.first;
            break;
         }
      }
      for (auto& pair : mRadius_Values)
      {
         if (xMaxRadius > pair.second)
         {
            xMaxIndex = pair.first;
         }
      }

      yMaxIndex = std::max(yMaxIndex - 2, 1); // Draw two lines prior to what is needed to make plot look "nice"
      xMaxIndex = std::min(xMaxIndex + 2, mRadius_Values.rbegin()->first); // Draw two lines after

      for (auto& curve : mRadius_Curves)
      {
         curve.second->mVisible = (curve.first >= yMaxIndex && curve.first <= xMaxIndex);
      }
   }
}

bool UtQtGL2DPlotTP::GetMaxXandYPoints(double& aXMaxX, double& aXMaxY, double& aYMaxX, double& aYMaxY)
{
   bool xLocated = false;
   bool yLocated = false;

   if (ContainsData())
   {
      aXMaxX = -DBL_MAX;
      aYMaxY = -DBL_MAX;

      for (auto& i : mData)
      {
         if (i.second->mVisible)
         {
            UtQtGL2DPlot::Series* series = dynamic_cast<UtQtGL2DPlot::Series*>(i.second);
            if (series != nullptr)
            {
               // Go in reverse order because it is more likely max X is at end of data list, and therefore less operations
               for (DataPtList::reverse_iterator iter = series->data.rbegin(); iter != series->data.rend(); ++iter)
               {
                  if (iter->x > aXMaxX)
                  {
                     aXMaxX   = iter->x;
                     aXMaxY   = iter->y;
                     xLocated = true;
                  }

                  if (iter->y > aYMaxY)
                  {
                     aYMaxX   = iter->x;
                     aYMaxY   = iter->y;
                     yLocated = true;
                  }
               }
            }
         }
      }
   }
   return xLocated && yLocated;
}

void UtQtGL2DPlotTP::DrawUnderlayLines()
{
   if (mShowBackgroundCurves)
   {
      double x1 = (double)(mPlotLeft_pixels + mDisplayWidth / 2);
      double x2 = x1 + (double)(mPlotRight_pixels - mPlotLeft_pixels);
      double y1 = (double)(mPlotBottom_pixels + mDisplayHeight / 2);
      double y2 = y1 + (double)(mPlotTop_pixels - mPlotBottom_pixels);

      // Create the plane equations
      GLdouble equationX1[] = {1.0, 0.0, 0.0, -x1};
      GLdouble equationX2[] = {-1.0, 0.0, 0.0, x2};
      GLdouble equationY1[] = {0.0, 1.0, 0.0, -y1};
      GLdouble equationY2[] = {0.0, -1.0, 0.0, y2};

      // Load the plane equations into the clippers
      glClipPlane(GL_CLIP_PLANE0, equationX1);
      glClipPlane(GL_CLIP_PLANE1, equationX2);
      glClipPlane(GL_CLIP_PLANE2, equationY1);
      glClipPlane(GL_CLIP_PLANE3, equationY2);

      // Enable the clipping planes
      glEnable(GL_CLIP_PLANE0);
      glEnable(GL_CLIP_PLANE1);
      glEnable(GL_CLIP_PLANE2);
      glEnable(GL_CLIP_PLANE3);

      // Check to see if we need to change the limits for the series.
      //  Only do this when the plot data changes
      double minX, maxX, minY, maxY;
      GetDataMinMax(minX, maxX, minY, maxY);
      if (mPreviousX.min != minX || mPreviousX.max != maxX || mPreviousY.min != minY || mPreviousY.max != maxY)
      {
         ComputeCurveLimits();
      }

      for (auto& seriesPair : mG_Curves)
      {
         DrawSeriesSubset(*seriesPair.second, mG_CurvesLimits.at(seriesPair.first));
      }

      for (auto& seriesPair : mRadius_Curves)
      {
         DrawSeriesSubset(*seriesPair.second, mRadius_CurvesLimits.at(seriesPair.first));
      }

      // Disable the clipping planes
      glDisable(GL_CLIP_PLANE0);
      glDisable(GL_CLIP_PLANE1);
      glDisable(GL_CLIP_PLANE2);
      glDisable(GL_CLIP_PLANE3);
   }
}

void UtQtGL2DPlotTP::DrawSeriesSubset(const Series& aSeries, MinMaxPair aXLimits)
{
   if (aSeries.mVisible)
   {
      DataPtList::const_iterator iter = aSeries.data.begin();

      // Initialize the Previous Point to the first point
      if (iter != aSeries.data.end())
      {
         glPushMatrix();
         {
            glTranslated((double)(mDisplayWidth)*0.5, (double)(mDisplayHeight)*0.5, 0.0);

            glLineWidth(1);
            {
               glColor4f(0, 0, 0, .3f);

               glEnable(GL_LINE_SMOOTH);
               glBegin(GL_LINE_STRIP);
               {
                  for (; iter != aSeries.data.end(); ++iter)
                  {
                     DataPt pt = *iter;
                     if (pt.x >= aXLimits.min && pt.x <= aXLimits.max)
                     {
                        double translatedX, translatedY;
                        TranslatePointFromRealToScene(aSeries, pt.x, pt.y, translatedX, translatedY);
                        if (mUsePointColor)
                        {
                           glColor4ubv((unsigned char*)&pt.rgba);
                        }
                        glVertex2i(translatedX, translatedY);
                     }
                  }
               }
               glEnd();
            }
            DisableStippling();
         }
         glPopMatrix();
      }
   }
}

void UtQtGL2DPlotTP::PopulateG_Curves()
{
   mG_Values.insert(std::pair<int, double>(1, 1.1));
   mG_Values.insert(std::pair<int, double>(2, 1.5));
   mG_Values.insert(std::pair<int, double>(3, 2.0));
   mG_Values.insert(std::pair<int, double>(4, 2.5));
   mG_Values.insert(std::pair<int, double>(5, 3.0));
   mG_Values.insert(std::pair<int, double>(6, 4.0));
   mG_Values.insert(std::pair<int, double>(7, 5.0));
   mG_Values.insert(std::pair<int, double>(8, 6.0));
   mG_Values.insert(std::pair<int, double>(9, 7.0));
   mG_Values.insert(std::pair<int, double>(10, 8.0));
   mG_Values.insert(std::pair<int, double>(11, 9.0));
   mG_Values.insert(std::pair<int, double>(12, 10.0));

   for (auto& gValue : mG_Values)
   {
      double g           = gValue.second;
      int    seriesIndex = gValue.first;

      UtQtGL2DPlot::Series* series = new UtQtGL2DPlot::Series(seriesIndex, UtQtGL2DPlot::LINE);

      // calculate turn rate
      //  G_Lateral (using a^2 + b^2 = c^2) is sqrt(g*g - 1^2) // 1g vertical to maintain level flight
      //  Accel = G_Lateral * 32.1741
      //  TurnRate(deg/s) = 360 * accel / (2*PI * vel)

      double accel = sqrt(g * g - 1) * 32.1741;
      double temp  = (360 * accel) / (2 * UtMath::cPI);
      // Need a higher data resolution at lower velocities
      for (double vel = 100; vel < 450; vel += 25)
      {
         series->data.emplace_back(vel, temp / vel);
      }
      // Don't need as high of resolution at high velocities
      for (double vel = 450; vel <= 3500; vel += 50)
      {
         series->data.emplace_back(vel, temp / vel);
      }
      series->SetName("G = " + QString::number(g));
      mG_Curves.insert(std::make_pair(seriesIndex, series));
      mG_CurvesLimits.insert(std::make_pair(seriesIndex, MinMaxPair(0, DBL_MAX)));
   }
}

void UtQtGL2DPlotTP::PopulateRadiusCurves()
{
   // Note this may not look as pretty as PopulateG_Curves() but it is more efficient run time.

   mRadius_Values.insert(std::pair<int, double>(1, 200));
   mRadius_Values.insert(std::pair<int, double>(2, 400));
   mRadius_Values.insert(std::pair<int, double>(3, 600));
   mRadius_Values.insert(std::pair<int, double>(4, 800));
   mRadius_Values.insert(std::pair<int, double>(5, 1000));
   mRadius_Values.insert(std::pair<int, double>(6, 1500));
   mRadius_Values.insert(std::pair<int, double>(7, 2000));
   mRadius_Values.insert(std::pair<int, double>(8, 2500));
   mRadius_Values.insert(std::pair<int, double>(9, 3000));
   mRadius_Values.insert(std::pair<int, double>(10, 3500));
   mRadius_Values.insert(std::pair<int, double>(11, 4000));
   mRadius_Values.insert(std::pair<int, double>(12, 4500));
   mRadius_Values.insert(std::pair<int, double>(13, 5000));
   mRadius_Values.insert(std::pair<int, double>(14, 6000));
   mRadius_Values.insert(std::pair<int, double>(15, 7000));
   mRadius_Values.insert(std::pair<int, double>(16, 8000));
   mRadius_Values.insert(std::pair<int, double>(17, 10000));
   mRadius_Values.insert(std::pair<int, double>(18, 15000));
   mRadius_Values.insert(std::pair<int, double>(19, 20000));
   mRadius_Values.insert(std::pair<int, double>(20, 30000));
   mRadius_Values.insert(std::pair<int, double>(21, 40000));
   mRadius_Values.insert(std::pair<int, double>(22, 50000));

   UtQtGL2DPlot::Series* r200   = new UtQtGL2DPlot::Series(1, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r400   = new UtQtGL2DPlot::Series(2, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r600   = new UtQtGL2DPlot::Series(3, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r800   = new UtQtGL2DPlot::Series(4, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r1000  = new UtQtGL2DPlot::Series(5, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r1500  = new UtQtGL2DPlot::Series(6, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r2000  = new UtQtGL2DPlot::Series(7, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r2500  = new UtQtGL2DPlot::Series(8, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r3000  = new UtQtGL2DPlot::Series(9, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r3500  = new UtQtGL2DPlot::Series(10, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r4000  = new UtQtGL2DPlot::Series(11, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r4500  = new UtQtGL2DPlot::Series(12, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r5000  = new UtQtGL2DPlot::Series(13, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r6000  = new UtQtGL2DPlot::Series(14, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r7000  = new UtQtGL2DPlot::Series(15, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r8000  = new UtQtGL2DPlot::Series(16, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r10000 = new UtQtGL2DPlot::Series(17, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r15000 = new UtQtGL2DPlot::Series(18, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r20000 = new UtQtGL2DPlot::Series(19, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r30000 = new UtQtGL2DPlot::Series(20, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r40000 = new UtQtGL2DPlot::Series(21, UtQtGL2DPlot::LINE);
   UtQtGL2DPlot::Series* r50000 = new UtQtGL2DPlot::Series(22, UtQtGL2DPlot::LINE);

   r200->data.emplace_back(100, 28.64788976);
   r400->data.emplace_back(100, 14.32394488);
   r600->data.emplace_back(100, 9.549296586);
   r800->data.emplace_back(100, 7.161972439);
   r1000->data.emplace_back(100, 5.729577951);
   r1500->data.emplace_back(100, 3.819718634);
   r2000->data.emplace_back(100, 2.864788976);
   r2500->data.emplace_back(100, 2.291831181);
   r3000->data.emplace_back(100, 1.909859317);
   r3500->data.emplace_back(100, 1.637022272);
   r4000->data.emplace_back(100, 1.432394488);
   r4500->data.emplace_back(100, 1.273239545);
   r5000->data.emplace_back(100, 1.14591559);
   r6000->data.emplace_back(100, 0.954929659);
   r7000->data.emplace_back(100, 0.818511136);
   r8000->data.emplace_back(100, 0.716197244);
   r10000->data.emplace_back(100, 0.572957795);
   r15000->data.emplace_back(100, 0.381971863);
   r20000->data.emplace_back(100, 0.286478898);
   r30000->data.emplace_back(100, 0.190985932);
   r40000->data.emplace_back(100, 0.143239449);
   r50000->data.emplace_back(100, 0.114591559);

   r200->data.emplace_back(3500, 1002.676141);
   r400->data.emplace_back(3500, 501.3380707);
   r600->data.emplace_back(3500, 334.2253805);
   r800->data.emplace_back(3500, 250.6690354);
   r1000->data.emplace_back(3500, 200.5352283);
   r1500->data.emplace_back(3500, 133.6901522);
   r2000->data.emplace_back(3500, 100.2676141);
   r2500->data.emplace_back(3500, 80.21409132);
   r3000->data.emplace_back(3500, 66.8450761);
   r3500->data.emplace_back(3500, 57.29577951);
   r4000->data.emplace_back(3500, 50.13380707);
   r4500->data.emplace_back(3500, 44.56338407);
   r5000->data.emplace_back(3500, 40.10704566);
   r6000->data.emplace_back(3500, 33.42253805);
   r7000->data.emplace_back(3500, 28.64788976);
   r8000->data.emplace_back(3500, 25.06690354);
   r10000->data.emplace_back(3500, 20.05352283);
   r15000->data.emplace_back(3500, 13.36901522);
   r20000->data.emplace_back(3500, 10.02676141);
   r30000->data.emplace_back(3500, 6.68450761);
   r40000->data.emplace_back(3500, 5.013380707);
   r50000->data.emplace_back(3500, 4.010704566);

   mRadius_Curves.insert(std::make_pair(1, r200));
   mRadius_Curves.insert(std::make_pair(2, r400));
   mRadius_Curves.insert(std::make_pair(3, r600));
   mRadius_Curves.insert(std::make_pair(4, r800));
   mRadius_Curves.insert(std::make_pair(5, r1000));
   mRadius_Curves.insert(std::make_pair(6, r1500));
   mRadius_Curves.insert(std::make_pair(7, r2000));
   mRadius_Curves.insert(std::make_pair(8, r2500));
   mRadius_Curves.insert(std::make_pair(9, r3000));
   mRadius_Curves.insert(std::make_pair(10, r3500));
   mRadius_Curves.insert(std::make_pair(11, r4000));
   mRadius_Curves.insert(std::make_pair(12, r4500));
   mRadius_Curves.insert(std::make_pair(13, r5000));
   mRadius_Curves.insert(std::make_pair(14, r6000));
   mRadius_Curves.insert(std::make_pair(15, r7000));
   mRadius_Curves.insert(std::make_pair(16, r8000));
   mRadius_Curves.insert(std::make_pair(17, r10000));
   mRadius_Curves.insert(std::make_pair(18, r15000));
   mRadius_Curves.insert(std::make_pair(19, r20000));
   mRadius_Curves.insert(std::make_pair(20, r30000));
   mRadius_Curves.insert(std::make_pair(21, r40000));
   mRadius_Curves.insert(std::make_pair(22, r50000));

   // Note this 22 much be the same as the highest index for the series' above
   for (int i = 1; i <= 22; ++i)
   {
      mRadius_CurvesLimits.insert(std::make_pair(i, MinMaxPair(0, DBL_MAX)));
   }
}
