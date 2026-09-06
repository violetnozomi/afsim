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

#ifndef WSFRADARMTD_SIGNALPROCESSOR
#define WSFRADARMTD_SIGNALPROCESSOR

#include "wsf_export.h"

#include <vector>

#include "UtTable.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorSignalProcessor.hpp"

class WsfMTI_Filter;
class WsfPD_Filter;

//! A class derived from WsfSensorSignalProcessor that defines a MTD signal
//! processing technique for a radar sensor.

class WsfRadarMTD_SignalProcessor : public WsfSensorSignalProcessor
{
public:
   //! Allowed MTD types
   enum MTD_Type
   {
      cMTDT_ALARM,
      cMTDT_DATA_TABLE
   };

   WsfRadarMTD_SignalProcessor() = delete;
   WsfRadarMTD_SignalProcessor(MTD_Type aMTD_Type);
   WsfRadarMTD_SignalProcessor(const WsfRadarMTD_SignalProcessor& aSrc);
   ~WsfRadarMTD_SignalProcessor() override;
   WsfRadarMTD_SignalProcessor& operator=(const WsfRadarMTD_SignalProcessor&) = delete;

   static WsfSensorSignalProcessor* ObjectFactory(const std::string& aTypeName);

   WsfSensorSignalProcessor* Clone() const override;

   bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, size_t aBeamIndex = 0) override;

   bool ProcessInput(UtInput& aInput) override;

   void Execute(double aSimTime, WsfSensorResult& aResult) override;

private:
   //! Computes an absolute MTD response value that will be applied to a sensor detection result
   double ComputeMTD_Response(const double               aTgtBearing,
                              const double               aTgtSlantRange,
                              const double               aTgtDopplerFrequency,
                              const std::vector<double>& aPulseRepetitionFrequencies,
                              const int                  aTypeFlag,
                              bool                       aZVF_Only = false);

   //! Returns the MTD response from the table for the specified Doppler frequency
   double GetTableResponse(const double aTargetDopplerFrequency);

   //! If 'true' the ownship velocity will be filtered (not included) included in the Doppler computations.
   bool mFilterOwnshipFromDoppler;

   //! Defines the type of MTD response
   MTD_Type mMTD_Type;

   //! Defines the minimum response of the MTD algorithm
   //! converted from dB to absolute when read from the config file
   double mMTD_MinResponse; // dB

   //! The minimum slant ranges at which MTD response begins
   // std::vector<double>      mMTD_MinRanges;      // meters

   ////! The maximum slant ranges at which the MTD response if calculated
   // std::vector<double>      mMTD_MaxRanges;      // meters

   //! The minimum angles at which MTD response begins
   // std::vector<double>      mMTD_MinAngles;      // radians

   ////! The maximum angles at which the MTD response if calculated
   // std::vector<double>      mMTD_MaxAngles;      // radians

   //! The table provides MTD response for the slant range
   UtTable::Curve mResponseTable;

   std::vector<WsfMTI_Filter*> mMTI_Filters;
   WsfPD_Filter*               mPD_Filter;
   WsfPD_Filter*               mZVF_Filter;
};

#endif
