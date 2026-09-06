// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFTRIMSIM_PROCESSOR_HPP
#define WSFTRIMSIM_PROCESSOR_HPP

#include "wsf_nx_export.h"

#include <memory>

#include "UtMatrix.hpp"
#include "WsfSensorComponent.hpp"
#include "WsfSensorErrorModel.hpp"
#include "WsfSensorProcessor.hpp"
class WsfTRIMSIM_SensorComponent;

//! This processor implements the TRIMSIM geopositioning error model
//! similar to the SUPPRESSOR TDOA Target Locating Sensor capability.
class WSF_NX_EXPORT WsfTRIMSIM_Processor : public WsfSensorProcessor
{
public:
   //! TRIMSIM requires at least four platforms supplying sensor
   //! measurement data in order to compute a 3-D location error covariance
   static constexpr unsigned int cDEFAULT_MINIMUM_DETECTIONS = 4U;

   //! Detection results
   struct Detection
   {
      WsfSensor* mSensorPtr;
      size_t     mTargetPlatformIndex;
      double     mSensorLocWCS[3];
      double     mSensorVelNED[3];
      double     mSensorVelWCS[3];
      double     mTimeOfDetection;
      double     mTrueRange;
      double     mTrueUnitVecNED[3];
   };
   using Detections = std::vector<Detection>;

   explicit WsfTRIMSIM_Processor(WsfScenario& aScenario);
   ~WsfTRIMSIM_Processor() override = default;

   //! @name Common framework methods
   //@{
   WsfProcessor* Clone() const override;
   bool          ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Status methods.
   //@{
   void TurnOn(double aSimTime) override;
   //@}

   bool AttemptToDetect(double aSimTime, WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

   bool ComputeMeasurementErrors(WsfSensor& aSensor);

   //! @name Public Detection information
   //@{
   const Detections& GetDetections() const { return mDetectionList; }
   size_t            GetDetectionCount() const { return mDetectionList.size(); }
   //@}

   static void PrintMatrix(const std::string& aTitle, UtMatrixd& aMatrix);

private:
   WsfTRIMSIM_Processor(const WsfTRIMSIM_Processor& aSrc);
   WsfTRIMSIM_Processor& operator=(const WsfTRIMSIM_Processor&) = delete;

   //! @name Sensor adding/removal
   //@{
   //! Check the number of sensors to ensure the minimum are available
   //! in order to perform the TRIMSIM error model processing. This is
   //! done any time the processor is turned on or a sensor is added or removed.
   void CheckSensors();
   bool SensorAdded(WsfSensor* aSensorPtr) override;
   bool SensorRemoved(WsfSensor* aSensorPtr) override;
   //@}

   //! @name Private Detection information
   //@{
   void AddDetection(double aSimTime, WsfSensor* aSensorPtr, size_t aTargetPlatformIndex, WsfSensorResult& aResult);
   void ClearDetections() { mDetectionList.clear(); }
   //@}

   void PrintTDOAInfo(WsfSensor* aSensorPtr);

   //! The minimum number of detections for successful Geolocation
   unsigned int mMinimumDetections{cDEFAULT_MINIMUM_DETECTIONS};

   //! The list of detections from the contributing sensors.
   std::vector<Detection> mDetectionList;

   //! List of sensors available for use by this processor
   std::vector<WsfSensor*> mSensorPtrs;
};

#include "WsfNonExportableComponentRoles.hpp"

//! A sensor component for the TRIMSIM/SUPPRESSOR TDOA Sensor Component for allowing extended
//! capability in the base sensor class functions.
class WSF_NX_EXPORT WsfTRIMSIM_SensorComponent : public WsfSensorComponent
{
public:
   static void                        RegisterComponentFactory(WsfScenario& aScenario);
   static WsfTRIMSIM_SensorComponent* Find(const WsfSensor& aSensor);
   static WsfTRIMSIM_SensorComponent* FindOrCreate(WsfSensor& aSensor);

   WsfTRIMSIM_SensorComponent();
   WsfTRIMSIM_SensorComponent(const WsfTRIMSIM_SensorComponent& aSrc);
   ~WsfTRIMSIM_SensorComponent() override = default;
   WsfTRIMSIM_SensorComponent& operator=(const WsfTRIMSIM_SensorComponent&) = delete;

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Interface from WsfSensorComponent.
   //@{
   void PostAttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) override;

   void UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) override;
   //@}

   bool GetMeasurementIsValid() const { return mMeasurementIsValid; }
   void SetMeasurementIsValid(bool aIsValid) { mMeasurementIsValid = aIsValid; }

   UtMatrixd& GetMeasurement() { return mMeasurement; }
   void       SetMeasurement(const UtMatrixd& aMeasurement) { mMeasurement = aMeasurement; }

   UtMatrixd& GetMeasurementErrorCovariance() { return mMeasurementErrorCovariance; }
   void       SetMeasurementErrorCovariance(const UtMatrixd& aMeasurementErrorCovariance)
   {
      mMeasurementErrorCovariance = aMeasurementErrorCovariance;
   }

   UtMatrixd& GetMeasurementTransition() { return mMeasurementTransition; }
   void       SetMeasurementTransition(const UtMatrixd& aMeasurementTransition)
   {
      mMeasurementTransition = aMeasurementTransition;
   }

   void CorrectForAltitude(double       aTargetLocWCS[3], // in / out
                           WsfPlatform* aTargetPtr,
                           double       aDefaultAltitude = 0.0);

   void SetProcessor(WsfTRIMSIM_Processor* aTRIMSIM_ProcessorPtr) { mTRIMSIM_ProcessorPtr = aTRIMSIM_ProcessorPtr; }

private:
   void ConvertMatrix(UtMatrixd& aMatrix, WsfTrack* aTrackPtr);

   void ApplyMeasurementErrors(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

   //! @name Computed Measurement Data
   //@{
   UtMatrixd mMeasurement;                //! [nM,1]
   UtMatrixd mMeasurementErrorCovariance; //! [nM,nM]
   UtMatrixd mMeasurementTransition;      //! [nM,nS]
   bool      mMeasurementIsValid;
   //@}

   WsfTRIMSIM_Processor* mTRIMSIM_ProcessorPtr;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfTRIMSIM_SensorComponent, cWSF_COMPONENT_TRIMSIM_SENSOR)

//! A sensor error model for the TRIMSIM/SUPPRESSOR TDOA Error Model functionality.
//! @note This class only holds the TDOA errors; it does not actually compute or apply errors.
class WSF_NX_EXPORT WsfTRIMSIM_ErrorModel : public wsf::SensorErrorModelBase
{
public:
   WsfTRIMSIM_ErrorModel();

   static std::unique_ptr<wsf::SensorErrorModelBase> ObjectFactory(const std::string& aTypeName);

   WsfTRIMSIM_ErrorModel* Clone() const override;
   bool                   ProcessInput(UtInput& aInput) override;
   bool                   Initialize(WsfSensorMode* aSensorModePtr) override;

   void ApplyMeasurementErrors(WsfSensorResult& aResult) override
   {
      // This class does not apply errors; it is done in WsfTRIMSIM_Processor::ApplyMeasurementErrors.
   }
   bool IsNullModel() const override { return true; }

   //! TRIMSIM Errors
   //@{
   bool                       ComputeErrors() const { return mComputeErrors; }
   const std::vector<double>& GetErrors() const { return mErrors; }
   double                     GetGroundTargetAltitudeError() const { return mGroundTargetAltitudeError; }
   //@}

   static const unsigned int cTDOA_ERROR_SIZE;
   static const unsigned int cTDOA_STATE_SIZE;

protected:
   WsfTRIMSIM_ErrorModel(const WsfTRIMSIM_ErrorModel& aSrc) = default;
   WsfTRIMSIM_ErrorModel& operator=(const WsfTRIMSIM_ErrorModel&) = delete;

private:
   // double  mTDOA_Errors[7];
   // north position error sigma (m)
   // east position error sigma (m)
   // down position error sigma (m)
   // reference time error (s)
   // inter-system time error (s)
   // sensor timing error (s)
   // atmospheric refraction error

   bool                mComputeErrors;             //! True if processor can compute geolocation errors
   std::vector<double> mErrors;                    //! Vector of errors
   double              mGroundTargetAltitudeError; //! If supplied, allow 3-platform geolocation calculation
};

#endif
