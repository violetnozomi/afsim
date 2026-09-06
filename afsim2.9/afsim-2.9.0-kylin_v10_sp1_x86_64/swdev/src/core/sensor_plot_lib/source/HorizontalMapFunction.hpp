// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef HORIZONTALMAPFUNCTION_HPP
#define HORIZONTALMAPFUNCTION_HPP

#include <string>
#include <vector>

#include "ContourFilter2D.hpp"
#include "MapPlotFunction.hpp"
#include "SupTMProjection.hpp"
#include "Target.hpp"
#include "UtColor.hpp"
class WsfPlatform;
class WsfSensor;

class HorizontalMapFunction : public MapPlotFunction
{
public:
   //! The list of values at a specified sample point
   using Point = std::vector<double>;

   static const double cUNDEFINED;

   explicit HorizontalMapFunction(const WsfScenario& aScenario);

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

protected:
   //! The function object for comparing two points at a specific variable index.
   struct PointCompare
   {
      explicit PointCompare(size_t aVarIndex)
         : mVarIndex(aVarIndex)
      {
      }

      bool operator()(const Point& p1, const Point& p2) { return (p1[mVarIndex] < p2[mVarIndex]); }

   private:
      size_t mVarIndex;
   };

   //! A contour level for sedit contour level output.
   struct ContourLevel
   {
      double  mLevel{0.5};
      UtColor mLineColor{1.0F, 0.5F, 0.0F};
      int     mLineWidth{3};
      int     mLineStyle{0};
   };

   //! The 'functor' for the contour filter.
   class ContourFunction : public ContourFilter2D::Function2D
   {
   public:
      ContourFunction(const std::vector<double>& aRowValues,
                      const std::vector<double>& aColValues,
                      const std::vector<Point>&  aDataValues,
                      size_t                     aVarIndex)
         : mRowValues(aRowValues)
         , mColValues(aColValues)
         , mDataValues(aDataValues)
         , mVarIndex(aVarIndex)
      {
      }

      double GetX(size_t aRowIndex, size_t aColIndex) override { return mRowValues[aRowIndex]; }
      double GetY(size_t aRowIndex, size_t aColIndex) override { return mColValues[aColIndex]; }
      double GetValue(size_t aRowIndex, size_t aColIndex) override
      {
         size_t gridIndex = (aRowIndex * mColValues.size()) + aColIndex;
         return mDataValues[gridIndex][mVarIndex];
      }

   private:
      const std::vector<double>& mRowValues;
      const std::vector<double>& mColValues;
      const std::vector<Point>&  mDataValues;
      size_t                     mVarIndex;
   };

   class Selector
   {
   public:
      enum SelectorType
      {
         cSENSOR_TYPE,
         cSENSOR_CATEGORY,
         cPLATFORM_TYPE,
         cPLATFORM_CATEGORY
      };
      SelectorType mType;
      WsfStringId  mValue;
   };

   struct AnalysisMapOptions
   {
      AnalysisMapOptions()
         : mFileName()
         , mColorRanges()
         , mDataTitleStr("Unknown")
         , mUnitsStr("Unknown")
      {
      }

      //! A color rule for analysis map output.
      struct ColorRange
      {
         bool operator<(const ColorRange& aRhs) const
         {
            return (std::make_pair(mMinValue, mMaxValue) < std::make_pair(aRhs.mMinValue, aRhs.mMaxValue));
         }

         double  mMinValue;
         double  mMaxValue;
         UtColor mColor; // RGBA
      };

      std::string             mFileName;
      std::vector<ColorRange> mColorRanges;
      std::string             mDataTitleStr;
      std::string             mUnitsStr;
   };

   using SelectorList = std::vector<Selector>;

   bool BuildContours(size_t aPdVarIndex);
   bool LoopOverTargetAltitude(size_t aPdVarIndex);
   bool LoopOverTargetPitch(size_t aPdVarIndex);
   bool LoopOverTargetRoll(size_t aPdVarIndex);

   void ConvertCartesianToSpherical(double aDownRange, double aCrossRange, double& aLatitude, double& aLongitude);

   void ConvertSphericalToCartesian(double aLatitude, double aLongitude, double& aDownRange, double& aCrossRange);

   void EvaluatePoint(double aSimTime, double aRowValue, double aColValue, std::vector<double>& aDataValues);

   void InitializeSensorPlatforms(double aSimTime);

   void CreateSensorList(WsfSimulation& aSimulation);

   bool InSelectorList(WsfSensor* aSensorPtr, const SelectorList& aSelectorList);

   bool ProcessTargetRegion(UtInput& aInput);

   void ReadSelector(UtInput& aInput, SelectorList& aSelectorList);

   void WriteDefendedAreaReport(const std::vector<double>& aAreaByPd);

   void WriteGnuPlotFile(const std::vector<double>& aRowValues,
                         const std::vector<double>& aColValues,
                         const std::vector<Point>&  aVarValues);

   void WriteGnuPlotPlayerFile();

   void WriteKML_File(const std::vector<double>& aRowValues,
                      const std::vector<double>& aColValues,
                      const std::vector<Point>&  aVarValues,
                      size_t                     aVarIndex);

   void WritePdMapFile(const std::vector<double>& aRowValues,
                       const std::vector<double>& aColValues,
                       const std::vector<Point>&  aVarValues,
                       size_t                     aVarIndex);

   void WriteSeditContours(const std::vector<double>& aRowValues,
                           const std::vector<double>& aColValues,
                           const std::vector<Point>&  aVarValues,
                           size_t                     aVarIndex);

   void WriteWSFContours(const std::vector<double>& aRowValues,
                         const std::vector<double>& aColValues,
                         const std::vector<Point>&  aVarValues,
                         size_t                     aVarIndex);

   void WriteShapefile(const std::vector<double>& aRowValues,
                       const std::vector<double>& aColValues,
                       const std::vector<Point>&  aVarValues,
                       size_t                     aVarIndex);

   void WriteAnalysisMapFile(const std::vector<double>& aRowValues,
                             const std::vector<double>& aColValues,
                             const std::vector<Point>&  aVarValues,
                             size_t                     aVarIndex);

   void WriteTiffFile(const std::vector<double>& aRowValues,
                      const std::vector<double>& aColValues,
                      const std::vector<Point>&  aVarValues) const;

   // Allow derived classes to write output in custom formats.
   // Default implementation does nothing.
   virtual void WriteCustomOutputFormats(const std::vector<double>& aRowValues,
                                         const std::vector<double>& aColValues,
                                         const std::vector<Point>&  aVarValues,
                                         size_t                     aVarIndex)
   {
   }

   std::string GetPdMapFileJsonMetadata() const;

   WsfSimulation* mSimulationPtr;

   double      mSensorPlatformYaw;
   double      mSensorPlatformPitch;
   double      mSensorPlatformRoll;
   std::string mModeName;
   size_t      mModeIndex;
   Target      mTarget;
   double      mTargetAlt;
   bool        mTargetAltAGL;
   double      mTargetAltMin;
   double      mTargetAltMax;
   double      mTargetAltStep;
   bool        mTargetAltRangeAGL;
   bool        mAutomaticTargetCueing;
   bool        mLineOfSightMasking;

   WsfStringId mTargetPlatformId;

   double mLatMin;
   double mLatMax;
   double mLatStep;
   double mLonMin;
   double mLonMax;
   double mLonStep;

   double      mDownRangeMin;
   double      mDownRangeMax;
   double      mDownRangeStep;
   std::string mDownRangeUnits;
   double      mCrossRangeMin;
   double      mCrossRangeMax;
   double      mCrossRangeStep;
   std::string mCrossRangeUnits;

   //! Used for converting between X/Y and Lat/Lon.
   //! Only used when use_tm_projection input is true.
   SupTMProjection mProjection;

   double mHeading;
   double mCenterLat;
   double mCenterLon;
   double mCenterEarthRadius;

   bool mUseTM_Projection;

   std::string               mGnuPlotFile;
   std::string               mGnuPlotPlayerFile;
   std::string               mPdMapFile;
   std::string               mHeaderLine1;
   std::string               mHeaderLine2;
   std::string               mHeaderLine3;
   size_t                    mOutputColumnLimit;
   std::string               mKML_File;
   std::string               mShapefile;
   std::string               mSeditFile;
   std::string               mWSF_File;
   std::vector<ContourLevel> mContourLevels;
   std::string               mContourVariable{"pd"};
   std::string               mDefendedAreaReportFile;

   AnalysisMapOptions mAnalysisMapOptions;
   std::string        mTiffFilename;

   SelectorList mExclusionList;
   SelectorList mInclusionList;

   std::vector<WsfSensor*> mSensors;
   std::vector<WsfSensor*> mExcludedSensors;

   double mSensorToENU_Transform[3][3];
   bool   mFirstWrite;

   static void GreatCircleHeadingAndDistance(double  aBegLat,
                                             double  aBegLon,
                                             double  aEndLat,
                                             double  aEndLon,
                                             double& aHeading,
                                             double& aDistance);
};

#endif
