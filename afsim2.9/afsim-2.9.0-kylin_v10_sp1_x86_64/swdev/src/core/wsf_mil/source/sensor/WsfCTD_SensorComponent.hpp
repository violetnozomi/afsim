// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCTD_SENSORCOMPONENT_HPP
#define WSFCTD_SENSORCOMPONENT_HPP

#include "wsf_mil_export.h"

#include <unordered_set>
#include <vector>

#include "WsfMilComponentRoles.hpp"
class WsfScenario;
#include "WsfSensor.hpp"
#include "WsfSensorComponent.hpp"
#include "WsfSensorResult.hpp"

//! The Close Target Detection (CTD) sensor component that will be attached to all sensor systems.
class WSF_MIL_EXPORT WsfCTD_SensorComponent : public WsfSensorComponent
{
public:
   static void                    RegisterComponentFactory(WsfScenario& aScenario);
   static WsfCTD_SensorComponent* Find(const WsfSensor& aSensor);
   static WsfCTD_SensorComponent* FindOrCreate(WsfSensor& aSensor);

   WsfCTD_SensorComponent()                                   = default;
   WsfCTD_SensorComponent(const WsfCTD_SensorComponent& aSrc) = default;
   WsfCTD_SensorComponent& operator=(const WsfCTD_SensorComponent&) = delete;
   ~WsfCTD_SensorComponent() override                               = default;

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Interface from WsfSensorComponent.
   //@{
   bool Initialize(double aSimTime) override;
   bool ProcessBeamInput(UtInput& aInput, WsfSensorBeam& aSensorBeam) override;

   void SelectMode(double aSimTime, WsfStringId aModeNameId) override;
   void DeselectMode(double aSimTime, WsfStringId aModeNameId) override;

   void AttemptToDetect(double aSimTime, WsfSensorResult& aResult) override;

   void PostAttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) override;

   bool TrackerAllowTracking(double                 aSimTime,
                             const TrackerSettings& aSettings,
                             const WsfTrackId&      aRequestId,
                             size_t                 aObjectId,
                             WsfTrack*              aTrackPtr,
                             WsfSensorResult&       aResult) override;

   // Callback from WsfSensorTracker implementation when dropping a track.
   void TrackerDropTrack(double                 aSimTime,
                         const TrackerSettings& aSettings,
                         const WsfTrackId&      aRequestId,
                         size_t                 aObjectId,
                         WsfSensorMode*         aModePtr,
                         WsfTrack*              aTrackPtr) override;
   //@}

   void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;
   void PlatformDeleted(WsfPlatform* aPlatformPtr) override;

   void TurnOff(double aSimTime) override;

   //! WsfCTD_SensorComponent methods.
   //! These are called explicitly called by various components of the CTD model
   //@{
   //@}

   using CTD_ResultsMap = std::map<WsfEM_Xmtr*, WsfSensorResult>;

   //! The component extensions to a sensor beam.
   class CTD_Beam
   {
   public:
      CTD_Beam() = default;

      struct Deltas
      {
         double mAzimuthDelta{-1.0};
         double mElevationDelta{-1.0};
         double mRangeDelta{-1.0};
      };

      size_t                       mBeamIndex{0};
      Deltas                       mAcquireDeltas;
      Deltas                       mReacquireDeltas;
      size_t                       mTargetIndex{0}; // prevent circular references
      std::unordered_set<size_t>   mCTD_Platforms;
      std::vector<WsfSensorResult> mCTD_Results;
   };

   //! The CTD component extensions to a sensor mode.
   class CTD_Mode
   {
   public:
      CTD_Mode();
      CTD_Mode(const CTD_Mode& aSrc);
      CTD_Mode& operator=(const CTD_Mode& aRhs);
      ~CTD_Mode()       = default;

      std::vector<CTD_Beam> mBeamList;

      struct TargetResult
      {
         bool   mTrackingCloseTarget{false};
         bool   mReacquirePossible{true};
         bool   mTargetReacquired{false};
         size_t mLastTargetResult{0};
      };
      std::map<size_t, TargetResult> mTargetResults;
   };

private:
   void ProcessDeltaInputs(UtInput& aInput, CTD_Beam::Deltas& aDeltas);

   WsfSensorMode* GetMode(WsfStringId aModeNameId) const;
   CTD_Mode&      GetOrCreateCTD_Mode(const WsfSensorMode& aSensorMode);
   CTD_Beam&      GetOrCreateCTD_Beam(const WsfSensorBeam& aSensorBeam);

   void ResetPlatforms();

   std::vector<CTD_Mode> mModeList;
   CTD_Mode              mTemplate;
   bool                  mIsWsfRadarSensor{false}; //!< true if the sensor is a WsfRadarSensor
   bool                  mDebugEnabled{false};
   bool                  mEnabled{false}; //!< true if the close_target_detection input block is present
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfCTD_SensorComponent, cWSF_COMPONENT_CTD_SENSOR)

#endif
