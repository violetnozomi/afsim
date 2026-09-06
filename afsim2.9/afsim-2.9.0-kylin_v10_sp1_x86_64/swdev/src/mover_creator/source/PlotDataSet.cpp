// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "PlotDataSet.hpp"

#include "UtLog.hpp"

namespace Designer
{

bool PlotDataSet::AddPoint(std::pair<double, double> aPoint, size_t aIndex)
{
   auto iter = mPlotData.begin();
   for (size_t i = 0; i < aIndex; ++i)
   {
      iter++;

      if (iter == mPlotData.end())
      {
         return false;
      }
   }

   mPlotData.insert(iter, aPoint);
   return true;
}

bool PlotDataSet::CreatePointAtX(double aXCoord)
{
   std::pair<double, double> newPoint;
   newPoint.first = aXCoord;
   auto iter1 = mPlotData.begin();

   // If new point is before the beginning, add it to the front
   if (aXCoord <= iter1->first)
   {
      newPoint.second = iter1->second;
      mPlotData.insert(mPlotData.begin(), newPoint);
      return true;
   }

   // If new point is past the end, put it on the back
   if (aXCoord > (--mPlotData.end())->first)
   {
      newPoint.second = iter1->second;
      mPlotData.push_back(newPoint);
      return true;
   }

   // Figure out which two points the new point is between, calculate its slope, and stick it there
   while (iter1 != --mPlotData.end())
   {
      auto prev = iter1;
      ++iter1;

      if (aXCoord > prev->first && aXCoord <= iter1->first)
      {
         newPoint.second = (aXCoord - prev->first) + prev->second;
         break;
      }
   }

   auto iter2 = mPlotData.begin();
   while (iter2 != --mPlotData.end())
   {
      auto prev = iter2;
      ++iter2;

      if (aXCoord > prev->first && aXCoord <= iter2->first)
      {
         mPlotData.insert(iter2, newPoint);
         return true;
      }
   }
   return false;
}

bool PlotDataSet::DeletePoint(size_t aIndex)
{
   auto iter = mPlotData.begin();

   for (size_t i = 0; i < aIndex; ++i)
   {
      iter++;

      if (iter == mPlotData.end())
      {
         return false;
      }
   }

   mPlotData.erase(iter);
   return true;
}

bool PlotDataSet::SetPoint(const std::pair<double, double>& aCoords, size_t aIndex)
{
   if (mPlotData.size() == 0)
   {
      return false;
   }

   auto iter = mPlotData.begin();

   for (size_t i = 0; i < aIndex; ++i)
   {
      iter++;

      if (iter == mPlotData.end())
      {
         return false;
      }
   }

   *iter = aCoords;
   return true;
}

void PlotDataSet::SetPlotData(std::vector<std::pair<double, double>>&& aPointVec)
{
   mPlotData = std::move(aPointVec);
}

double PlotDataSet::GetYValueForX(double aX)
{
   // Loop through the slope points, if this exact x is found, return the corresponding y, else calculate the y intercept given the slope of the points the given x value falls between

   double xLowerBound = 0.0;
   double yLowerBound = 0.0;
   double xUpperBound = 0.0;
   double yUpperBound = 0.0;

   bool lowerBoundOkay = false;
   bool upperBoundOkay = false;
   for (auto& pointIter : mPlotData)
   {
      std::pair<double, double> curPt = pointIter;
      if (curPt.first == aX)
      {
         // We have a match, so return it
         return curPt.second;
      }
      else if (curPt.first < aX)
      {
         xLowerBound = curPt.first;
         yLowerBound = curPt.second;
         lowerBoundOkay = true;
      }
      else if (curPt.first > aX)
      {
         xUpperBound = curPt.first;
         yUpperBound = curPt.second;
         upperBoundOkay = true;
         break;
      }
   }

   if (lowerBoundOkay && upperBoundOkay)
   {
      // At this point, the spline has been narrowed down to two points within which the given aX falls between.
      // Calculate the slope between the points: m = (y2-y1)/(x2-x)
      double m = (yUpperBound - yLowerBound) / (xUpperBound - xLowerBound);

      // Find the y intercept b = y - mx
      double yIntercept = yUpperBound - (m * xUpperBound);

      // Using slope-intercept form, get the y for our given aX: y = mx+b
      double yVal = (m * aX) + yIntercept;

      return yVal;

      // double t = (aX - mKnots[i].coordinates.first) / (mKnots[i + 1].coordinates.first - mKnots[i].coordinates.first);
      // double y = (1 - t)*mKnots[i].coordinates.second + t*mKnots[i + 1].coordinates.second + t*(1 - t)*(a*(1 - t) + b*t);
   }
   else
   {
      if (!lowerBoundOkay && !upperBoundOkay)
      {
         ut::log::error() << "Upper and lower bounds failed in PlotDataSet::GetYValueForX().";
      }
      else if (!lowerBoundOkay)
      {
         ut::log::error() << "Lower bound failed in PlotDataSet::GetYValueForX().";
      }
      if (!upperBoundOkay)
      {
         ut::log::error() << "Upper bound failed in PlotDataSet::GetYValueForX().";
      }
   }

   return 0.0; // Default return value
}

} // namespace Designer
