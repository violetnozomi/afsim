// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_MOON_LOS_COMPONENT_HPP
#define WSF_MOON_LOS_COMPONENT_HPP

#include "wsf_export.h"

#include "WsfComponentRoles.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorComponent.hpp"

//! Component that checks if the sensor's line-of-sight to the target is blocked by the moon.
//! Created for every sensor, but has no effect unless enable_moon_los_block is specified in the mode.
class WSF_EXPORT WsfMoonLOS_SensorComponent : public WsfSensorComponent
{
public:
   static void                        RegisterComponentFactory(WsfScenario& aScenario);
   static WsfMoonLOS_SensorComponent* Find(const WsfSensor& aParent);
   static WsfMoonLOS_SensorComponent* FindOrCreate(WsfSensor& aParent);

   //! @name Interface from WsfSensorComponent.
   //@{
   bool Initialize(double aSimTime) override;
   bool ProcessModeInput(UtInput& aInput, WsfSensorMode& aSensorMode) override;

   void PostAttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) override;
   //@}

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}


private:
   std::vector<bool> mModes;
   bool              mTemplate = false;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMoonLOS_SensorComponent, cWSF_COMPONENT_SENSOR_MOON_LOS)

#endif
