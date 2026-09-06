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

#ifndef WSFRADARPD_SIGNALPROCESSOR
#define WSFRADARPD_SIGNALPROCESSOR

#include "wsf_export.h"

#include "UtTable.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorSignalProcessor.hpp"

class WsfPD_Filter;

//! A class derived from WsfSensorSignalProcessor that defines a Pulse Doppler signal
//! processing technique for a radar sensor.

class WsfRadarPD_SignalProcessor : public WsfSensorSignalProcessor
{
public:
   //! Allowed PD types
   enum PulseDopplerType
   {
      cPDT_ALARM,
      cPDT_DATA_TABLE
   };

   WsfRadarPD_SignalProcessor() = delete;
   WsfRadarPD_SignalProcessor(PulseDopplerType aPulseDopplerType);
   WsfRadarPD_SignalProcessor(const WsfRadarPD_SignalProcessor& aSrc);
   ~WsfRadarPD_SignalProcessor() override;
   WsfRadarPD_SignalProcessor& operator=(const WsfRadarPD_SignalProcessor&) = delete;

   static WsfSensorSignalProcessor* ObjectFactory(const std::string& aTypeName);

   WsfSensorSignalProcessor* Clone() const override;

   bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, size_t aBeamIndex = 0) override;

   bool ProcessInput(UtInput& aInput) override;

   void Execute(double aSimTime, WsfSensorResult& aResult) override;

private:
   //! Computes an absolute PD response value that will be applied to a sensor detection result
   double ComputePD_Response(const double               aTgtDopplerFrequency,
                             const std::vector<double>& aPulseRepetitionFrequencies,
                             const int                  aTypeFlag);

   //! Returns the PD response from the table for the specified range
   double GetTableResponse(const double aTargetDopplerFrequency);

   //! If 'true' the ownship velocity will be filtered (not included) included in the Doppler computations.
   bool mFilterOwnshipFromDoppler;

   //! Defines the type of PD response
   PulseDopplerType mPD_Type;

   //! Defines the minimum response of the PD algorithm
   //! converted from dB to absolute when read from the config file
   double mPD_MinResponse; // dB

   //! The table provides PD response for the Doppler frequency
   UtTable::Curve mResponseTable;

   WsfPD_Filter* mPD_Filter;
};

#endif
