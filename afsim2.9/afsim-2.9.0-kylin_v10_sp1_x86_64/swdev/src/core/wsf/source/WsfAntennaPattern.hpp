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

#ifndef WSFANTENNAPATTERN_HPP
#define WSFANTENNAPATTERN_HPP

#include "wsf_export.h"

#include <memory>
#include <mutex>

#include "TblLookup.hpp"
class UtInput;
#include "UtReferenceTracked.hpp"
#include "WsfObject.hpp"
class WsfSimulation;

//! Represents the gain of an antenna as a function of azimuth and elevation.
//!
//! Antenna patterns are used by transmitters and receivers of electromagnetic
//! radiation to compute the gain of the transmitted or received signal.
//!
//! A given antenna pattern object will be shared amongst all objects that
//! utilize the pattern.

class WSF_EXPORT WsfAntennaPattern : public WsfObject
{
public:
   static const char* cTYPE_KIND;

   class WSF_EXPORT GainAdjustmentTable
   {
   public:
      TblIndVarU<double> mFrequency;
      TblDepVar1<double> mAdjustment;
   };

   class WSF_EXPORT BaseData : public UtReferenceCounted
   {
   public:
      BaseData();
      BaseData(const BaseData& aSrc) = default;
      ~BaseData() override           = default;

      virtual bool ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput);
      virtual bool Initialize(WsfAntennaPattern& aAntennaPattern);

      virtual double GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El);

      virtual double PerformGainAdjustment(double aFrequency, double aGain);

      virtual void InitializeAverageGain(double aFrequency);

      //! The minimum gain that should be returned by any antenna pattern.
      double mMinimumGain;

      //! A simple gain adjustment.
      double mGainAdjustment;

      //! Frequency-dependent gain adjustment table.
      GainAdjustmentTable mGainAdjustmentTable;

      //! Initialization flag for the shared data that is to be initialized only once to speed up
      //! the overall initialization of the simulation.
      bool mInitialized;

      //! @name Data used by the GetGainThreshold function.
      //@{
      bool                 mAvgGainInitialized;
      bool                 mShowAvgGain;
      std::recursive_mutex mAvgGainMutex;
      std::vector<double>  mAvgGain;
      //! The peak gain of the sample. This *SHOULD* be the peak gain of the pattern.
      double mSampledPeakGain;
      //@}
   };

   WsfAntennaPattern();
   WsfAntennaPattern(BaseData* aBasePtr);
   ~WsfAntennaPattern() override;

   WsfAntennaPattern* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfAntennaPattern"; }

   virtual double GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El);

   WSF_DEPRECATED virtual double GetAzimuthBeamwidth(double aFrequency) const final;
   virtual double GetAzimuthBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const;
   WSF_DEPRECATED virtual double GetElevationBeamwidth(double aFrequency) const final;
   virtual double GetElevationBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const;

   virtual double                     GetMinimumGain() const;
   virtual double                     GetPeakGain(double aFrequency) const;
   virtual const GainAdjustmentTable& GetGainAdjustmentTable() const;
   virtual double                     GetGainAdjustment() const;

   virtual bool Initialize(WsfSimulation* aSimulationPtr);

   bool ProcessInput(UtInput& aInput) override;

   virtual double
   GetGainThresholdFraction(double aGainThreshold, double aPeakGain, double aMinAz, double aMaxAz, double aFrequency);

   virtual void SetAntennaBeamCount(unsigned int aBeamCount){};

protected:
   WsfAntennaPattern(const WsfAntennaPattern& aSrc);
   WsfAntennaPattern& operator=(const WsfAntennaPattern& aRhs);

   //! A shared pointer to the data shared between this and derived classes.
   //! @note Provides a method for sharing data among pattern instances via pointers within each derived class.
   // std::shared_ptr<BaseData>  mSharedDataPtr;

   BaseData* mSharedDataPtr;

   virtual double ApplyEBS(double aBeamwidth, double aEBS_Az, double aEBS_El) const;
};

#endif
