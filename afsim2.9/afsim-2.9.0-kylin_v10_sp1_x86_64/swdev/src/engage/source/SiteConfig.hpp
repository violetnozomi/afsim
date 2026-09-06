// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef SITECONFIG_HPP
#define SITECONFIG_HPP

#include <string>
#include <vector>

class UtInput;


namespace engage
{

class Simulation;
class Task;

class SiteConfig
{
public:
   SiteConfig();

   bool Complete(bool aSiteGridUsed);

   bool CreatePlatforms(Simulation& aSimulation, const Task& aTask) const;

   bool ProcessInput(UtInput& aInput);
   bool ProcessSiteGridInput(UtInput& aInput);

   //! Returns the number of site points/cases.
   int GetPointCount() const { return mPointCount; }
   int GetGridDrCount() const { return mGridDownRangeCount; }
   int GetGridCrCount() const { return mGridCrossRangeCount; }

   double GetGridDrMin() const { return mGridDownRangeMin; }
   double GetGridDrMax() const { return mGridDownRangeMax; }
   double GetGridDrStep() const { return mGridDownRangeStep; }

   double GetGridCrMin() const { return mGridCrossRangeMin; }
   double GetGridCrMax() const { return mGridCrossRangeMax; }
   double GetGridCrStep() const { return mGridCrossRangeStep; }

   void GetRunPosition(int aTargetIndex, double& aX, double& aY) const;

   // Set the grid point count using external values during a site grid run
   void SetGridPointCount(int aCount) { mGridPointCount = aCount; }

   class Site
   {
   public:
      Site();

      bool ProcessInput(UtInput& aInput);

      double mX;
      double mY;
      double mZ;
      double mSpeed;
      double mHeading;
   };

   typedef std::vector<Site> SiteList;

private:
   std::string mLauncherType;
   std::string mTrackerType;
   SiteList    mSiteList;
   int         mPointCount;

   //! Site grid member variables
   //! @name Site grid
   //@{
   double      mGridDownRangeMin;
   double      mGridDownRangeMax;
   double      mGridDownRangeStep;
   std::string mGridDownRangeUnits;
   double      mGridCrossRangeMin;
   double      mGridCrossRangeMax;
   double      mGridCrossRangeStep;
   std::string mGridCrossRangeUnits;

   //! Computed values
   int mGridPointCount;
   int mGridDownRangeCount;
   int mGridCrossRangeCount;
   //@}
};

} // namespace engage

#endif
