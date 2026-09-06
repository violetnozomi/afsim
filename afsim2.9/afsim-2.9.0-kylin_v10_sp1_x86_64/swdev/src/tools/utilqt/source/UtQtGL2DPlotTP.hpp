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

#ifndef UtQtGL2DPlotTP_hpp
#define UtQtGL2DPlotTP_hpp

#include "UtQtGL2DPlot.hpp"
#include "UtUnitTypes.hpp"

// ************************************************************************************ //
// UtQtGL2DPlotTP extends UtQtGL2DPlot to provide a specialized plot for                //
// showing Turn Performance charts for aircraft.                                        //
// ************************************************************************************ //

class UTILQT_EXPORT UtQtGL2DPlotTP : public UtQtGL2DPlot
{
public:
   UtQtGL2DPlotTP();

   void Reset() override;

   void SetSpeedUnits(UtUnitSpeed::BaseUnit aSpeedUnit);
   void SetSpeedMultiplier(double aSpeedMultiplier);

   void SetShowBackgroundCurves(bool aShow) { mShowBackgroundCurves = aShow; }

protected:
   void ComputeCurveLimits();
   bool GetMaxXandYPoints(double& aXMaxX, double& aXMaxY, double& aYMaxX, double& aYMaxY);

   struct MinMaxPair
   {
      MinMaxPair(double aMin, double aMax)
         : min(aMin)
         , max(aMax)
      {
      }

      double min;
      double max;
   };

   void DrawUnderlayLines() override;

   // Note this function assumes that the data points within mG_Curves & mRadius_Curves
   //  have monotonically increasing values for X
   //  (This should be safe since we generated the data values for the curves)
   void DrawSeriesSubset(const Series& aSeries, MinMaxPair aXLimits);

   void PopulateG_Curves();
   void PopulateRadiusCurves();

   bool mShowBackgroundCurves;

   double mSpeedFPS_Multiplier;

   MinMaxPair mPreviousX;
   MinMaxPair mPreviousY;

   std::map<int, Series*>    mG_Curves;
   std::map<int, double>     mG_Values;
   std::map<int, MinMaxPair> mG_CurvesLimits;

   std::map<int, Series*>    mRadius_Curves;
   std::map<int, double>     mRadius_Values;
   std::map<int, MinMaxPair> mRadius_CurvesLimits;
};

#endif