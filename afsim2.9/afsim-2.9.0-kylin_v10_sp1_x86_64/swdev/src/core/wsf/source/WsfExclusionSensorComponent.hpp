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

#ifndef WSF_EXCLUSION_SENSOR_COMPONENT_HPP
#define WSF_EXCLUSION_SENSOR_COMPONENT_HPP

#include "wsf_export.h"

#include "WsfComponentRoles.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorComponent.hpp"

//! Component that checks if the sensor's line-of-sight is blocked by the Sun or Moon
//! Created for every sensor, but has no affect unless solar_exclusion_angle or lunar_exclusion_angle commands are specified
class WSF_EXPORT WsfExclusionSensorComponent : public WsfSensorComponent
{
public:
   enum
   {
      cNONE  = 0,
      cSOLAR = 1,
      cLUNAR = 2,
      cBOTH  = cSOLAR | cLUNAR
   };

   static void                         RegisterComponentFactory(WsfScenario& aScenario);
   static WsfExclusionSensorComponent* Find(const WsfSensor& aParent);
   static WsfExclusionSensorComponent* FindOrCreate(WsfSensor& aParent);

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

   //! Class that stores the defined exclusion angles for each individual mode
   class ExclusionMode
   {
   public:
      unsigned int GetType() const { return mType; }
      double       GetSolar() const { return mSolarAngle; }
      double       GetLunar() const { return mLunarAngle; }
      void         SetSolar(double aAngle);
      void         SetLunar(double aAngle);

   private:
      unsigned int mType{cNONE};
      double       mSolarAngle;
      double       mLunarAngle;
   };

private:
   ExclusionMode& GetOrCreateExclusionMode(const WsfSensorMode& aSensorMode);

   std::vector<ExclusionMode> mModes;
   ExclusionMode              mTemplate;
   bool                       mEnabled{false};
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfExclusionSensorComponent, cWSF_COMPONENT_SENSOR_EXCLUSION)

#endif
