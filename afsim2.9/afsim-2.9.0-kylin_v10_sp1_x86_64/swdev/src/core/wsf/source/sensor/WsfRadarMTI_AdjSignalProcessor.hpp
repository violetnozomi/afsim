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

#ifndef WSFRADARMTI_ADJSIGNALPROCESSOR
#define WSFRADARMTI_ADJSIGNALPROCESSOR

#include "wsf_export.h"

#include "UtTable.hpp"
#include "WsfMTI_TableManager.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorSignalProcessor.hpp"

class RadarMode;
class Result;

//! A class derived from WsfSensorSignalProcessor that defines MTI signal
//! processing techniques for a radar sensor.

class WSF_EXPORT WsfRadarMTI_AdjSignalProcessor : public WsfSensorSignalProcessor
{
public:
   //! Allowed MTI types
   enum MTI_Type
   {
      cMTIT_DATA_TABLE,       //! Data table driven implementation
      cMTIT_ADJUSTMENT_TABLE, //! Target adjustment Data table driven implementation
      cMTIT_PROCESSOR         //! MTI Dual Delay Line Canceler implementation
   };

   WsfRadarMTI_AdjSignalProcessor(MTI_Type aMTI_Type);
   WsfRadarMTI_AdjSignalProcessor(const WsfRadarMTI_AdjSignalProcessor&) = default;
   WsfRadarMTI_AdjSignalProcessor& operator=(const WsfRadarMTI_AdjSignalProcessor&) = default;
   ~WsfRadarMTI_AdjSignalProcessor() override                                       = default;

   static WsfSensorSignalProcessor* ObjectFactory(const std::string& aTypeName);

   WsfSensorSignalProcessor* Clone() const override;

   bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, size_t aBeamIndex = 0) override;

   bool ProcessInput(UtInput& aInput) override;

   void Execute(double aSimTime, WsfSensorResult& aResult) override;

private:
   WsfRadarMTI_AdjSignalProcessor();

   bool ProcessDataTableInput(UtInput& aInput);
   bool ProcessAdjustmentTableInput(UtInput& aInput);
   bool ProcessMTI_ProcessorInput(UtInput& aInput);

   //! Computes an absolute MTI response value that will be applied to a sensor detection result
   double ComputeMTI_Response(WsfSensorResult& aResult, const int aTypeFlag);

   //! Returns the MTI response from the table for the specified range
   double GetTableResponse(const double aSlantRange);

   //! For External Services.
   WsfMTI_TableManager::MTI_AdjustmentTable* GetMTI_AdjustmentTable() { return mMTI_AdjustmentTablePtr.get(); }

   void ReadTable(UtInput& aInput, WsfMTI_TableManager::MTI_AdjustmentTable& aMTI_AdjustmentTable);

   double ComputeMTI_Effects(WsfSensorResult& aResult, const int aTypeFlag);

   // bool InitializeMTI(WsfSensorMode& aMode);

   // void ComputeClutterAttenuationFactor(WsfSensorMode& aMode);

   //! If 'true' the ownship velocity will be filtered (not included) included in the Doppler computations.
   bool mFilterOwnshipFromDoppler;

   //! Defines the type of MTI response
   MTI_Type mMTI_Type;

   //! Defines the minimum response of the MTI algorithm
   //! converted from dB to absolute when read from the config file
   double mMTI_MinResponse; // dB

   //! The minimum slant ranges at which MTI response begins
   // std::vector<double>    mMTI_MinRanges;      // meters

   ////! The maximum slant ranges at which the MTI response if calculated
   // std::vector<double>    mMTI_MaxRanges;      // meters

   //! The minimum angles at which MTI response begins
   // std::vector<double>    mMTI_MinAngles;      // radians

   ////! The maximum angles at which the MTI response if calculated
   // std::vector<double>    mMTI_MaxAngles;      // radians

   //! The table provides MTI response for the Doppler frequency
   UtTable::Curve mResponseTable; // future implementation

   //! @name GMTI Adjustment Table Parameters
   //@{
   //! Pointer to the MTI adjustment table.
   //! If this is non-null then it will be used for adjusting the SNR based on closing speed.
   std::shared_ptr<WsfMTI_TableManager::MTI_AdjustmentTable> mMTI_AdjustmentTablePtr;

   //! The MTI table will not be applied (no attentuaion) when the target is this far away.
   // ! Not to be confused with the MTI processing specific variable
   double mMTI_TableMaxRange;
   //@}

   //! @name MTI Processing Parameters
   //@{
   double mMTI_MaxRange; //!< MTI maximum range
   double mMTI_InterpulsePeriod_1;
   double mMTI_InterpulsePeriod_2;
   double mMTI_StabilityConstant;
   double mMTI_Weights[3];
   int    mMTI_NumberOfStages;
   bool   mMTI_ClutterLock;
   bool   mMTI_UpWind;
   double mClutterAttenuationFactor;
   double mInputClutterAttenuationFactor;
   //@}
};

#endif
