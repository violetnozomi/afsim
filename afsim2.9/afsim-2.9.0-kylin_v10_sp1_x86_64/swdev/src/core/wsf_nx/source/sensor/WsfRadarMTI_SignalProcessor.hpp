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

#ifndef WSFRADARMTI_SIGNALPROCESSOR
#define WSFRADARMTI_SIGNALPROCESSOR

#include "wsf_export.h"

#include "UtTable.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorSignalProcessor.hpp"

class WsfMTI_Filter;

//! A class derived from WsfSensorSignalProcessor that defines MTI signal
//! processing techniques for a radar sensor.

class WsfRadarMTI_SignalProcessor : public WsfSensorSignalProcessor
{
public:
   //! Allowed MTI types
   enum MTI_Type
   {
      cMTIT_ALARM,      //! ALARM Type Implementation
      cMTIT_DATA_TABLE, //! Data table driven implementation
   };

   WsfRadarMTI_SignalProcessor(MTI_Type aMTI_Type);
   WsfRadarMTI_SignalProcessor(const WsfRadarMTI_SignalProcessor& aSrc);
   ~WsfRadarMTI_SignalProcessor() override;
   WsfRadarMTI_SignalProcessor& operator=(const WsfRadarMTI_SignalProcessor&) = delete;

   static WsfSensorSignalProcessor* ObjectFactory(const std::string& aTypeName);

   WsfSensorSignalProcessor* Clone() const override;

   bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, size_t aBeamIndex = 0) override;

   bool ProcessInput(UtInput& aInput) override;

   void Execute(double aSimTime, WsfSensorResult& aResult) override;

private:
   WsfRadarMTI_SignalProcessor();

   bool ProcessDataTableInput(UtInput& aInput);

   //! Computes an absolute MTI response value that will be applied to a sensor detection result
   double ComputeMTI_Response(const double               aTgtBearing,
                              const double               aTgtSlantRange,
                              const double               aTgtDopplerFrequency,
                              const std::vector<double>& aPulseRepetitionFrequencies,
                              const int                  aTypeFlag);

   //! Returns the MTI response from the table for the specified Doppler
   double GetTableResponse(const double aTargetDopplerFrequency);

   //! If 'true' the ownship velocity will be filtered (not included) included in the Doppler computations.
   bool mFilterOwnshipFromDoppler;

   //! Defines the type of MTI response
   MTI_Type mMTI_Type;

   //! Defines the minimum response of the MTI algorithm
   //! converted from dB to absolute when read from the config file
   double mMTI_MinResponse; // dB

   //! The minimum slant ranges at which MTI response begins
   // std::vector<double>      mMTI_MinRanges;      // meters

   ////! The maximum slant ranges at which the MTI response if calculated
   // std::vector<double>      mMTI_MaxRanges;      // meters

   //! The minimum angles at which MTI response begins
   // std::vector<double>      mMTI_MinAngles;      // radians

   ////! The maximum angles at which the MTI response if calculated
   // std::vector<double>      mMTI_MaxAngles;      // radians

   //! The table provides MTI response for the Doppler frequency
   UtTable::Curve mResponseTable; // future implementation

   WsfMTI_Filter* mMTI_Filter;
};

#endif
