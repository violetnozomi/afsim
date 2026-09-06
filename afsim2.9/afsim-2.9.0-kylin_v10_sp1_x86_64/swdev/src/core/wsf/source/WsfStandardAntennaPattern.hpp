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

#ifndef WSFSTANDARDANTENNAPATTERN_HPP
#define WSFSTANDARDANTENNAPATTERN_HPP

#include "wsf_export.h"

#include <memory>

class UtAzElTable;
#include "WsfAntennaPattern.hpp"

//! The implementation of 'standard' antenna patterns.
//!
//! The class provides an implementation for several common antenna patterns
//!
//! - sin x/x circular aperture
//! - sin x/x elliptical and rectangular aperture
//! - uniform or constant
//! - an inline or external table of gain vs. az/el.

class WSF_EXPORT WsfStandardAntennaPattern : public WsfAntennaPattern
{
public:
   enum PatternType
   {
      cUNKNOWN,
      cTABLE,
      cCIRCULAR_PATTERN,
      cCOSECANT_SQUARED_PATTERN,
      cSINE_PATTERN,
      cUNIFORM_PATTERN
   };

   class StandardData : public WsfAntennaPattern::BaseData
   {
   public:
      StandardData();
      ~StandardData() override;

      bool ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput) override;
      bool Initialize(WsfAntennaPattern& aAntennaPattern) override;

      double GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El) override;

      void ResetPatternType();

      static double BesselJ1(double aX);

      PatternType  mPatternType;
      UtAzElTable* mTablePtr;

      double mPeakGain;
      double mInputAzBeamwidth;
      double mInputElBeamwidth;
      double mAzBeamwidth;
      double mElBeamwidth;

      //! @name addition input values for the 'sine' parameters
      //@{
      double mSineAngleModificationFactor;
      int    mSineFunctionType;
      //@}

      //! @name Additional input values for the 'cosecant_squared' pattern.
      //@{
      double mCscMinElForPeakGain;
      double mCscElOfPeakCsc2Boundary;
      double mCscMaxElForCsc2;
      //@}

      //! @name Derived values for the 'cosecant_squared' pattern that are computed when it is first used.
      //@{
      double mCscMinimumGainAdjusted;
      double mCscPeakGainAdjusted;
      double mCscK_Az;
      double mCscK_El;
      double mCscWithinCsc2Factor;
      double mCscAboveCsc2Factor;
      //@}

      bool mFirstTime;


   protected:
      static double CircularPattern(double aAzimuth, double aElevation, double aPeakGain, double aBeamwidth);

      double CosecantSquaredPattern(double aAzimuth, double aElevation);

      static double SinePattern(double aAzimuth,
                                double aElevation,
                                double aPeakGain,
                                double aAzimuthBeamwidth,
                                double aElevationBeamwidth,
                                double aOffset,
                                double aFact,
                                int    aType);

      void ProcessCircularInput(WsfAntennaPattern& aPattern, UtInput& aInput);
      void ProcessCosecantSquaredInput(WsfAntennaPattern& aPattern, UtInput& aInput);
      void ProcessPatternTableInput(WsfAntennaPattern& aPattern, UtInput& aInput);
      void ProcessSineInput(WsfAntennaPattern& aPattern, UtInput& aInput);
      void ProcessUniformInput(WsfAntennaPattern& aPattern, UtInput& aInput);
   };

   WsfStandardAntennaPattern();
   WsfStandardAntennaPattern& operator=(const WsfStandardAntennaPattern& aRhs) = delete;
   ~WsfStandardAntennaPattern() override;

   WsfAntennaPattern* Clone() const override;

   double GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El) override;

   double GetAzimuthBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const override;
   double GetElevationBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const override;
   double GetPeakGain(double aFrequency) const override;

   //! @name Private data access.
   //! These methods return data that should not be considered part of the public interface.
   //! They were provided to support External Services.
   //! Use at your own risk!
   //@{
   double         GetSineAngleModificationFactor() const { return GetStandardData()->mSineAngleModificationFactor; }
   int            GetSineFunctionType() const { return GetStandardData()->mSineFunctionType; }
   int            GetStandardPatternType() const { return GetStandardData()->mPatternType; }
   UtAzElTable*   GetUtAzElTable() const { return GetStandardData()->mTablePtr; }
   double         GetCscMinElForPeakGain() const { return GetStandardData()->mCscMinElForPeakGain; }
   double         GetCscElOfPeakCsc2Boundary() const { return GetStandardData()->mCscElOfPeakCsc2Boundary; }
   double         GetCscMaxElForCsc2() const { return GetStandardData()->mCscMaxElForCsc2; }
   virtual double GetInputAzimuthBeamwidth() const { return GetStandardData()->mInputAzBeamwidth; }
   virtual double GetInputElevationBeamwidth() const { return GetStandardData()->mInputElBeamwidth; }
   StandardData*  GetStandardData() const { return static_cast<StandardData*>(mSharedDataPtr); }
   //@}

protected:
   WsfStandardAntennaPattern(const WsfStandardAntennaPattern& aSrc);

private:
   //! Pointer to the data which is shared in the base WsfAntenna class via a shared pointer..
   // StandardData* mStandardDataPtr;
};

#endif
