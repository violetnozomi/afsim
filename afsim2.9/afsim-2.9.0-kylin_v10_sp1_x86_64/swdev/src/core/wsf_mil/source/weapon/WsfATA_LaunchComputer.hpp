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

#ifndef WSFATA_LAUNCHCOMPUTER_HPP
#define WSFATA_LAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

#include <vector>

class UtInput;
class UtInputBlock;
#include "WsfLaunchComputer.hpp"

//!   A WsfATALaunchComputer provides information supporting the decision to launch a
//!   guided airborne weapon against a track of another airborne platform.
//!
//!   All tracks will be obtained from the WsfPlatform WsfTrackManager.  The launch
//!   platform's Current Target will be used, unless one or more WsfTrackId are specified.
//!
//!   (FYI:  The seemingly large number of obscure public Set/Get methods of the class
//!   are necessary to allow automatic generation of the Launch Computer object by
//!   automated Weapon Tool software.  They may be ignored by the end user.)

class WSF_MIL_EXPORT WsfATA_LaunchComputer : public WsfLaunchComputer
{
public:
   //! The basic type name (needed by weapon_tools and launch computer processor)
   static std::string BaseTypeName() { return "WSF_ATA_LAUNCH_COMPUTER"; }

   WsfATA_LaunchComputer();
   WsfATA_LaunchComputer& operator=(const WsfATA_LaunchComputer&) = delete;
   ~WsfATA_LaunchComputer() override;

   WsfLaunchComputer* Clone() const override;
   bool               Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;
   bool               ProcessInput(UtInput& aInput) override;

   bool ToolOnlyInitialize(double aSimTime);

   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;

   //! This nested class represents an evenly-spaced array of floating-point
   //! numbers that identify the constrained range of an independent variable,
   //! such as elevation values for which a weapon may be launched.  Refer to
   //! "bins" that are zero-indexed, where the bin center is the floating point
   //! value that is half way between the upper and lower bounds of the bin.
   //! Example:  If there are 7 bins, there are 8 values (in total) bounding
   //! the upper and lower edges of all bins, and valid bin indices are 0 to 6.
   class WSF_MIL_EXPORT EvenlySpaced
   {
   public:
      EvenlySpaced() = default;

      void         SetNumBins(unsigned int aSize) { mBins = aSize; }
      unsigned int GetNumBins() const { return mBins; }
      void         SetDelta(double aValue) { mDelta = aValue; }
      double       Delta() const { return mDelta; }
      void         SetMin(double aValue) { mMin = aValue; }
      double       Min() const { return mMin; }
      double       Max() const { return mMin + mBins * mDelta; }
      bool         BinCenter(unsigned int aIndex, double& aValue) const;
      bool         BinLower(unsigned int aIndex, double& aValue) const;
      bool         BinUpper(unsigned int aIndex, double& aValue) const;
      bool         GetIndex(double aValue, unsigned int& aIndex) const;

   private:
      double       mMin{0.0};
      double       mDelta{0.0};
      unsigned int mBins{0};
   };

   //! Similar to above, this nested class represents a non-evenly-spaced
   //! array of floating-point values for a constrained independent variable.
   class WSF_MIL_EXPORT NotEvenlySpaced
   {
   public:
      void         SetNumBins(unsigned int aSize) { mValues.resize(aSize + 1, 0.0); }
      unsigned int GetNumBins() const { return static_cast<unsigned int>(mValues.size() - 1); }
      bool         SetValue(unsigned int aIndex, double aValue);
      bool         Delta(unsigned int aIndex, double& aValue) const;
      double       Min() const { return mValues[0]; }
      double       Max() const { return mValues[mValues.size() - 1]; }
      bool         BinCenter(unsigned int aIndex, double& aValue) const;
      bool         BinLower(unsigned int aIndex, double& aValue) const;
      bool         BinUpper(unsigned int aIndex, double& aValue) const;
      bool         GetIndex(double aValue, unsigned int& aIndex) const;

   private:
      std::vector<double> mValues;
   };

   //! @name The Methods used only by 'weapon_tools'.
   //! The following methods are provided only to assist the 'weapon_tools' program in the
   //! launch computer generation process.
   //@{

   void AllocateResults(unsigned int aSize);
   void ResetAllIndices();
   void SetResult(double aValue);
   bool IncrementRange();

   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   const EvenlySpaced& Aspects() { return mTableShotAspects; }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   const EvenlySpaced& Azimuths() { return mTableTargetAzimuths; }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   const EvenlySpaced& Elevations() { return mTableTargetElevations; }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   const EvenlySpaced& RangeRates() { return mTableRangeRates; }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   const NotEvenlySpaced& Ranges() { return mTableRanges; }

   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   unsigned int RangeIndex() const { return mRangeIndex; }

   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   unsigned int SizeOfResults() const { return SizeOfAspectsSet(); }

   unsigned int SizeOfAspectsSet() const;
   unsigned int SizeOfAzimuthsSet() const;
   unsigned int SizeOfElevationsSet() const;
   unsigned int SizeOfRangeRatesSet() const;
   unsigned int SizeOfRangeBinsSet() const;

   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   bool TableLookUpUsed() const { return !mTOFResults.empty(); }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   double GetResult() const { return mTOFResults.empty() ? 0.0 : mTOFResults[mIterIndex]; }

   double TableTargetRange() const;
   double TableTargetRangeRate() const;
   double TableTargetAspect() const;
   double TableTargetAzimuth() const;
   double TableTargetElevation() const;

   bool TableTargetRange(unsigned int aIndex, double& aValue) const;
   bool TableTargetRangeRate(unsigned int aIndex, double& aValue) const;
   bool TableTargetRelHeading(unsigned int aIndex, double& aValue) const;
   bool TableTargetAzimuth(unsigned int aIndex, double& aValue) const;
   bool TableTargetElevation(unsigned int aIndex, double& aValue) const;

   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   double TableMinRange() const { return mTableRanges.Min(); }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   double TableMaxRange() const { return mTableRanges.Max(); }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   double TableRange(unsigned int aIndex) const
   {
      double value;
      mTableRanges.BinCenter(aIndex, value);
      return value;
   }

   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   unsigned int RangeRateIndex() const { return mRangeRateIndex; }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   unsigned int TargetElIndex() const { return mTargetElIndex; }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   unsigned int TargetAzIndex() const { return mTargetAzIndex; }
   //! Used by the Weapon_Tool during the Launch Computer Generation process.
   unsigned int ShotAspectIndex() const { return mTableShotAspectIndex; }

   //@}

protected:
   WsfATA_LaunchComputer(const WsfATA_LaunchComputer& aSrc);

   void ProcessInterceptBlock(UtInputBlock& aInputBlock);
   void ProcessGenerateBlock(UtInputBlock& aInputBlock);
   bool CheckRanges();

   bool InitializeTTIData() override;

   double GetTOF(double aAspect, double aAzimuth, double aElevation, double aRangeRate, double aRange);

private:
   void SetIterIndex();
   bool IncrementRangeRate();
   bool IncrementTargetElevation();
   bool IncrementTargetAzimuth();
   bool IncrementShotAspect();

   unsigned int mTableShotAspectIndex;
   unsigned int mTargetAzIndex;
   unsigned int mTargetElIndex;
   unsigned int mRangeRateIndex;
   unsigned int mRangeIndex;
   unsigned int mIterIndex;

   EvenlySpaced    mTableShotAspects;
   EvenlySpaced    mTableTargetAzimuths;
   EvenlySpaced    mTableTargetElevations;
   EvenlySpaced    mTableRangeRates;
   NotEvenlySpaced mTableRanges;

   std::vector<double> mTOFResults;
};

#endif
