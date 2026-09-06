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

#ifndef WSF_SOLAR_ILLUMINATION_COMPONENT_HPP
#define WSF_SOLAR_ILLUMINATION_COMPONENT_HPP

#include "wsf_export.h"

#include "WsfComponentRoles.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorComponent.hpp"

//! Component that checks if the sensor's vision of target has appropriate sunlight angles
//! Created for every EOIR, Geometric, IRST, and Optical sensor, but is only enabled by the target_solar_illumination_angle command
class WSF_EXPORT WsfSolarIlluminationComponent : public WsfSensorComponent
{
public:
   static void                           RegisterComponentFactory(WsfScenario& aScenario);
   static WsfSolarIlluminationComponent* Find(const WsfSensor& aParent);
   static WsfSolarIlluminationComponent* FindOrCreate(WsfSensor& aParent);

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

   //! Class that stores the defined solar illumination bounds for each individual mode
   class IlluminationMode
   {
   public:
      double GetMin() const { return mMinAngle; }
      double GetMax() const { return mMaxAngle; }
      void   SetBounds(double aMinAngle, double aMaxAngle);
      bool   IsEnabled() const { return mEnabled; }

   private:
      double mMinAngle{0.0};
      double mMaxAngle{0.0};
      bool   mEnabled{false};
   };

private:
   IlluminationMode& GetOrCreateIlluminationMode(const WsfSensorMode& aSensorMode);

   std::vector<IlluminationMode> mModes;
   IlluminationMode              mTemplate;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfSolarIlluminationComponent, cWSF_COMPONENT_SOLAR_ILLUMINATION)

#endif
