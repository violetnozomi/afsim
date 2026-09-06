// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SOLAR_ELEVATION_AT_TARGET_SENSOR_COMPONENT_HPP
#define WSF_SOLAR_ELEVATION_AT_TARGET_SENSOR_COMPONENT_HPP

#include "wsf_export.h"

#include <vector>

#include "WsfComponentRoles.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorComponent.hpp"

//! Component that checks if the solar elevation to the target is within a specified range.
//! Created for every EOIR, Geometric, IRST, and Optical sensor, but is only enabled by the solar_elevation_at_target command
class WSF_EXPORT WsfSolarElevationAtTargetSensorComponent : public WsfSensorComponent
{
public:
   //! Called by WsfScenario::CreateTypeLists on initialization.
   static void RegisterComponentFactory(WsfScenario& aScenario);
   //! Returns the parent sensor's component, or nullptr if not found.
   static WsfSolarElevationAtTargetSensorComponent* Find(const WsfSensor& aParent);
   //! Returns the parent sensor's component, or creates one if not found.
   static WsfSolarElevationAtTargetSensorComponent* FindOrCreate(WsfSensor& aParent);

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

   //! Class that stores the defined elevation angles for each individual mode
   class ElevationMode
   {
   public:
      bool   IsEnabled() const { return mEnabled; }
      double GetMinAngle() const { return mMinAngle; }
      double GetMaxAngle() const { return mMaxAngle; }
      void   SetBounds(double aMinAngle, double aMaxAngle);

   private:
      double mMinAngle = 0;
      double mMaxAngle = 0;
      bool   mEnabled  = false;
   };

private:
   ElevationMode& GetOrCreateElevationMode(const WsfSensorMode& aSensorMode);

   std::vector<ElevationMode> mModes;
   ElevationMode              mTemplate;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfSolarElevationAtTargetSensorComponent, cWSF_COMPONENT_SOLAR_ELEVATION_AT_TARGET)

#endif
