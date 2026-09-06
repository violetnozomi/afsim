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

#include "CubicSpline.hpp"

#include <algorithm>
#include <iterator>

#include "UtLog.hpp"

namespace Designer
{

CubicSpline::CubicSpline()
   : mPointsBetweenKnots(0)
{
}

CubicSpline::~CubicSpline()
{
}

std::vector<std::pair<double, double>> CubicSpline::GetKnotPoints()
{
   std::vector<std::pair<double, double>> temp;

   std::transform(mKnots.begin(), mKnots.end(), std::back_inserter(temp), [](const Knot& k) {return k.coordinates; });

   return temp;
}

CubicSpline::Knot CubicSpline::GetKnot(size_t aIndex)
{
   return mKnots[aIndex];
}

void CubicSpline::AddKnot(CubicSpline::Knot aKnot)
{
   mKnots.push_back(aKnot);
   SetPointCountBetweenKnots(mPointsBetweenKnots);
}

bool CubicSpline::AddKnot(CubicSpline::Knot aKnot, size_t aIndex)
{
   auto iter = mKnots.begin();
   for (size_t i = 0; i < aIndex; ++i)
   {
      iter++;

      if (iter == mKnots.end())
      {
         return false;
      }
   }

   mKnots.insert(iter, aKnot);
   SetPointCountBetweenKnots(mPointsBetweenKnots);
   return true;
}

bool CubicSpline::CreateKnotAtX(double aXCoord)
{
   CubicSpline::Knot newKnot;
   newKnot.coordinates.first = aXCoord;
   newKnot.BCSpecification = CubicSpline::cFIRST_DERIV;
   auto iter1 = mSpline.begin();

   // If new knot is before the beginning, add it to the front
   if (aXCoord <= iter1->first)
   {
      newKnot.coordinates.second = iter1->second;
      newKnot.boundaryCondition = 0;
      mKnots.insert(mKnots.begin(), newKnot);
      SetPointCountBetweenKnots(mPointsBetweenKnots);
      return true;
   }

   // If new knot is past the end, put it on the back
   if (aXCoord > (--mSpline.end())->first)
   {
      newKnot.coordinates.second = iter1->second;
      newKnot.boundaryCondition = 0;
      mKnots.push_back(newKnot);
      SetPointCountBetweenKnots(mPointsBetweenKnots);
      return true;
   }

   // Figure out which two knots the new knot is between, calculate its slope, and stick it there
   while (iter1 != --mSpline.end())
   {
      auto prev = iter1;
      ++iter1;

      if (aXCoord > prev->first && aXCoord <= iter1->first)
      {
         newKnot.boundaryCondition = (iter1->second - prev->second) / (iter1->first - prev->first);
         newKnot.coordinates.second = newKnot.boundaryCondition * (aXCoord - prev->first) + prev->second;
         break;
      }
   }

   auto iter2 = mKnots.begin();
   while (iter2 != --mKnots.end())
   {
      auto prev = iter2;
      ++iter2;

      if (aXCoord > prev->coordinates.first && aXCoord <= iter2->coordinates.first)
      {
         mKnots.insert(iter2, newKnot);
         return true;
      }
   }
   return false;
}

bool CubicSpline::DeleteKnot(size_t aIndex)
{
   auto iter = mKnots.begin();

   for (size_t i = 0; i < aIndex; ++i)
   {
      iter++;

      if (iter == mKnots.end())
      {
         return false;
      }
   }

   mKnots.erase(iter);
   GenerateSpline();
   return true;
}

bool CubicSpline::SetKnotPoint(const std::pair<double, double>& aCoords, size_t aIndex)
{
   if (mKnots.size() == 0)
   {
      return false;
   }

   auto iter = mKnots.begin();

   for (size_t i = 0; i < aIndex; ++i)
   {
      iter++;

      if (iter == mKnots.end())
      {
         return false;
      }
   }

   iter->coordinates = aCoords;
   GenerateSpline();
   return true;
}

bool CubicSpline::SetKnot(const CubicSpline::Knot& aKnot, size_t aIndex)
{
   mKnots[aIndex] = aKnot;
   GenerateSpline();
   return true;
}

void CubicSpline::SetKnotVec(std::vector<CubicSpline::Knot>& aKnotVec)
{
   mKnots = aKnotVec;
   SetPointCountBetweenKnots(mPointsBetweenKnots);
}

void CubicSpline::SetPointCountBetweenKnots(size_t aPointCount)
{
   mPointsBetweenKnots = aPointCount;
   GenerateSpline();
}

void CubicSpline::GenerateSpline()
{
   mSpline.clear();

   if (!mKnots.empty())
   {
      for (size_t i = 0; i < mKnots.size() - 1; ++i)
      {
         mSpline.push_back(mKnots[i].coordinates);
         if (mKnots[i].BCSpecification == cFIRST_DERIV &&
             mKnots[i + 1].BCSpecification == cFIRST_DERIV)
         {
            double a = mKnots[i].boundaryCondition * (mKnots[i + 1].coordinates.first - mKnots[i].coordinates.first)
                       - (mKnots[i + 1].coordinates.second - mKnots[i].coordinates.second);

            double b = -mKnots[i + 1].boundaryCondition * (mKnots[i + 1].coordinates.first - mKnots[i].coordinates.first)
                       + (mKnots[i + 1].coordinates.second - mKnots[i].coordinates.second);

            for (size_t j = 1; j <= mPointsBetweenKnots; ++j)
            {
               double x = ((mKnots[i + 1].coordinates.first - mKnots[i].coordinates.first) /
                           (mPointsBetweenKnots + 1)) * j + mKnots[i].coordinates.first;

               double t = (x - mKnots[i].coordinates.first) /
                          (mKnots[i + 1].coordinates.first - mKnots[i].coordinates.first);

               double y = (1 - t) * mKnots[i].coordinates.second + t * mKnots[i + 1].coordinates.second +
                          t * (1 - t) * (a * (1 - t) + b * t);

               mSpline.push_back({ x, y });
            }
         }
      }
      mSpline.push_back(mKnots[mKnots.size() - 1].coordinates);
   }
}

double CubicSpline::GetYValueForX(double aX)
{
   // Loop through the slope points, if this exact x is found, return the corresponding y, else calculate the y intercept given the slope of the points the given x value falls between

   double xLowerBound = 0.0;
   double yLowerBound = 0.0;
   double xUpperBound = 0.0;
   double yUpperBound = 0.0;

   bool lowerBoundOkay = false;
   bool upperBoundOkay = false;
   for (auto& pointIter : mSpline)
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
         ut::log::error() << "Upper and lower bounds failed in CubicSpline::GetYValueForX().";
      }
      else if (!lowerBoundOkay)
      {
         ut::log::error() << "Lower bound failed in CubicSpline::GetYValueForX().";
      }
      if (!upperBoundOkay)
      {
         ut::log::error() << "Upper bound failed in CubicSpline::GetYValueForX().";
      }
   }

   return 0.0; // Default return value
}

} // namespace Designer
