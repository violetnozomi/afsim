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

#ifndef WSFRADARSTC_SIGNALPROCESSOR_HPP
#define WSFRADARSTC_SIGNALPROCESSOR_HPP

#include "wsf_export.h"

#include "UtTable.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorSignalProcessor.hpp"

//! A class derived from WsfSensorSignalProcessor that defines a sensitivity time control
//! processing technique for a radar sensor.
//!
//! Sensitivity Time Control (STC) attempts to adjust receiver gain with time during a
//! single pulse repetition period to get the best visibility of nearby and distant
//! targets. It is used to attenuate the very strong signals returned from nearby
//! ground clutter close to a Radar receiver. Without this signal attenuation, the
//! receiver would routinely saturate due to the strong signals. STC effects are typically
//! limited to around 50 miles, beyond that the receiver operates normally. Excessive STC
//! can blank out potential returns of low RCS targets close to the receiver.
//!
//! This signal processor is only applicable to radar sensors and was developed to be
//! compatible with the SUPPRESSOR method of STC processing. It adjusts the received
//! power, clutter power, and jamming power of a sensor detection attempt result.

class WsfRadarSTC_SignalProcessor : public WsfSensorSignalProcessor
{
public:
   WsfRadarSTC_SignalProcessor();
   WsfRadarSTC_SignalProcessor(const WsfRadarSTC_SignalProcessor& aSrc) = default;
   ~WsfRadarSTC_SignalProcessor() override                              = default;
   WsfRadarSTC_SignalProcessor& operator=(const WsfRadarSTC_SignalProcessor&) = delete;

   static WsfSensorSignalProcessor* ObjectFactory(const std::string& aTypeName);

   WsfSensorSignalProcessor* Clone() const override;

   virtual bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   bool ProcessInput(UtInput& aInput) override;

   void Execute(double aSimTime, WsfSensorResult& aResult) override;

private:
   //! Allowed STC response types
   enum SensitivityTimeContolType
   {
      cSTC_MIN_RANGE,
      cSTC_MAX_RANGE,
      cSTC_MIN_MAX_RANGE,
      cSTC_DATA_TABLE
   };

   //! Defines the type of STC response
   SensitivityTimeContolType mSTC_Type;

   //! Defines the minimum response of the STC algorithm
   //! converted from dB to absolute when read from the config file
   double mSTC_MinResponse; // dB

   //! The minimum slant range at which STC response begins
   double mSTC_MinRange; // meters

   //! The maximum slant range at which the STC response if calculated
   double mSTC_MaxRange; // meters

   //! The order of the STC response curve, typically 3 or 4
   double mSTC_Order;

   //! The table provides STC response for the slant range
   UtTable::Curve mResponseTable;

   //! Computes an absolute STC response value that will be applied to a sensor detection result
   double ComputeSTC_Response(const double aSlantRange, const double aPulseRepetitionFrequency);

   //! Returns the STC response from the table for the specified range
   double GetTableResponse(const double aSlantRange);
};

#endif
