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

#ifndef WSFSOSM_SENSOR_HPP
#define WSFSOSM_SENSOR_HPP

#include "wsf_sosm_export.h"

class SOSM_SensorTarget;
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
class WsfPlatform;
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
class WsfSensorResult;
class WsfSOSM_Interface;

//! A sensor that utilizes the 'Spectral Optical Detection Model' (SOSM).
class WSF_SOSM_EXPORT WsfSOSM_Sensor : public WsfSensor
{
public:
   WsfSOSM_Sensor(WsfScenario& aScenario);
   WsfSOSM_Sensor(const WsfSOSM_Sensor& aSrc);
   WsfSOSM_Sensor& operator=(const WsfSOSM_Sensor&) = delete;

   WsfSensor*  Clone() const override;
   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;
   bool        Initialize(double aSimTime) override;
   bool        ProcessInput(UtInput& aInput) override;
   void        Update(double aSimTime) override;
   void        PlatformDeleted(WsfPlatform* aPlatformPtr) override;

   WsfSOSM_Interface* GetSOSM_Interface() const { return mSOSM_InterfacePtr; }

protected:
   class SOSM_Mode : public WsfSensorMode
   {
   public:
      SOSM_Mode();
      SOSM_Mode(const SOSM_Mode& aSrc);
      ~SOSM_Mode() override;
      SOSM_Mode& operator=(const SOSM_Mode&) = delete;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;
      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;
      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;
      void PlatformDeleted(WsfPlatform* aPlatformPtr);

      void               UpdateTrackReportingFlags(double aSimTime, WsfTrack* aTrackPtr) override;
      WsfSOSM_Interface* GetSOSM_Interface() const;
      WsfStringId        mSOSM_SensorType;
      WsfEM_Antenna      mAntenna;
      WsfEM_Rcvr         mRcvr;

      using TargetMap = std::map<size_t, SOSM_SensorTarget*>;

      TargetMap mTargetMap;

      double mRangingTime;
      double mRangingTimeTrackQuality;
   };

   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<SOSM_Mode*> mSOSM_ModeList;

   WsfSOSM_Interface* mSOSM_InterfacePtr;
};

#endif
