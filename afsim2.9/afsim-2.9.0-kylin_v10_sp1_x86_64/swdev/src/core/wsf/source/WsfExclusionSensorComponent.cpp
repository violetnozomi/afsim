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

#include "WsfExclusionSensorComponent.hpp"

#include <array>

#include "UtCalendar.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMoon.hpp"
#include "UtSolarSystem.hpp"
#include "UtSun.hpp"
#include "WsfDateTime.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

namespace
{
class ExclusionSensorComponentFactory : public WsfComponentFactory<WsfSensor>
{
public:
   void PreInput(WsfSensor& aParent) override { WsfExclusionSensorComponent::FindOrCreate(aParent); }
};
} // namespace

//! Called by WsfScenario::CreateTypeLists on initialization
void WsfExclusionSensorComponent::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<ExclusionSensorComponentFactory>());
}

//! Returns the parent sensor's component, or nullptr if not found
//! @param   aParent   The sensor that we are checking for components
WsfExclusionSensorComponent* WsfExclusionSensorComponent::Find(const WsfSensor& aParent)
{
   WsfExclusionSensorComponent* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<WsfExclusionSensorComponent>(componentPtr);
   return componentPtr;
}

//! Returns the parent sensor's component, and creates one if not found
//! @param aParent   The sensor that we are checking for components
WsfExclusionSensorComponent* WsfExclusionSensorComponent::FindOrCreate(WsfSensor& aParent)
{
   WsfExclusionSensorComponent* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      auto newComponentPtr = ut::make_unique<WsfExclusionSensorComponent>();
      // Since AddComponent only conditionally takes ownership of the component (via a raw
      // pointer), we use unique_ptr to manage it until confirmed the component was added.
      if (aParent.GetComponents().AddComponent(newComponentPtr.get()))
      {
         // Ownership is not released to the caller, but was transferred to the parent sensor.
         // This is a mirage, but a convenient way to do two things at once.
         componentPtr = newComponentPtr.release();
      }
   }
   return componentPtr;
}

bool WsfExclusionSensorComponent::Initialize(double aSimTime)
{
   // If the mode list is empty then the single implicit mode defined by the template is used.
   if (mModes.empty())
   {
      mModes.push_back(mTemplate);
   }

   // Expand the local mode list to match the sensor mode list size. In cases where the modes are generated
   // from user input then they will be the same. In the case of WsfCompositeSensor that isn't true as its
   // mode list is derived from the constituent sensors.
   mModes.resize(GetSensor()->GetModeCount(), mTemplate);

   return true;
}

//! Enables component and sets exclusion angles if commands are found
//! @param   aInput      The input object to read from
//! @param   aSensorMode The current sensor mode that the input controls
bool WsfExclusionSensorComponent::ProcessModeInput(UtInput& aInput, WsfSensorMode& aSensorMode)
{
   ExclusionMode& mode(GetOrCreateExclusionMode(aSensorMode));

   std::string command(aInput.GetCommand());
   if (command == "solar_exclusion_angle")
   {
      mEnabled = true;
      double angle;
      aInput.ReadValueOfType(angle, UtInput::cANGLE);
      mode.SetSolar(angle);
      return true;
   }
   else if (command == "lunar_exclusion_angle")
   {
      mEnabled = true;
      double angle;
      aInput.ReadValueOfType(angle, UtInput::cANGLE);
      mode.SetLunar(angle);
      return true;
   }
   return false;
}

//! Calculates if exclusion has occurred for the target platform, and sets aResult respectively.
//! @param   aSimTime   The simulation time.
//! @param   aTargetPtr The platform that detection is attempted on.
//! @param   aResult    The sensor result that holds success/failure details.
void WsfExclusionSensorComponent::PostAttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   if (!mEnabled)
   {
      return;
   }
   size_t modeIndex = GetSensor()->GetCurrentMode()->GetModeIndex();
   if (modeIndex < mModes.size())
   {
      auto& mode = mModes[modeIndex];

#undef GetCurrentTime // Avoid conflict with Windows macro
      UtCalendar now = GetSimulation()->GetDateTime().GetCurrentTime(aSimTime);
      UtVec3d    platformLoc;
      UtVec3d    sensorLoc;
      UtVec3d    bodyLoc;

      aTargetPtr->GetLocationECI(platformLoc.GetData());
      GetSensor()->GetLocationECI(sensorLoc.GetData());
      UtVec3d platformVector = platformLoc - sensorLoc;

      if (mode.GetType() & cSOLAR)
      {
         UtSolarSystem::GetSunLocationECI(now, bodyLoc);
         UtVec3d bodyVector = UtVec3d(bodyLoc) - UtVec3d(sensorLoc);

         double halfApparentSize = asin(UtSun::cMEAN_RADIUS / bodyVector.Magnitude());
         double angleFromSun =
            acos(bodyVector.DotProduct(platformVector) / (bodyVector.Magnitude() * platformVector.Magnitude()));

         if (angleFromSun <= mode.GetSolar() + halfApparentSize)
         {
            aResult.mFailedStatus |= WsfSensorResult::cEXCLUSION_SOLAR;
         }
      }
      if (mode.GetType() & cLUNAR)
      {
         UtSolarSystem::GetMoonLocationECI(now, bodyLoc);
         UtVec3d bodyVector = UtVec3d(bodyLoc) - UtVec3d(sensorLoc);

         double halfApparentSize = asin(UtMoon::cMEAN_RADIUS / bodyVector.Magnitude());
         double angleFromMoon =
            acos(bodyVector.DotProduct(platformVector) / (bodyVector.Magnitude() * platformVector.Magnitude()));

         if (angleFromMoon <= mode.GetLunar() + halfApparentSize)
         {
            aResult.mFailedStatus |= WsfSensorResult::cEXCLUSION_LUNAR;
         }
      }
   }
}

//! Clones the component and returns it in a base class pointer
WsfComponent* WsfExclusionSensorComponent::CloneComponent() const
{
   return new WsfExclusionSensorComponent(*this);
}

//! Returns the name of the component
WsfStringId WsfExclusionSensorComponent::GetComponentName() const
{
   return "component_exclusion";
}

//! Returns the roles of the component
const int* WsfExclusionSensorComponent::GetComponentRoles() const
{
   static const std::array<int, 3> roles = {cWSF_COMPONENT_SENSOR_EXCLUSION,
                                            cWSF_COMPONENT_SENSOR_COMPONENT,
                                            cWSF_COMPONENT_NULL};
   return roles.data();
}

//! Returns a pointer to the object if aRole matches it, else returns nullptr
void* WsfExclusionSensorComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_SENSOR_EXCLUSION)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_SENSOR_COMPONENT)
   {
      return this;
   }
   return nullptr;
}

WsfExclusionSensorComponent::ExclusionMode& WsfExclusionSensorComponent::GetOrCreateExclusionMode(const WsfSensorMode& aSensorMode)
{
   // The local modes are maintained in the same order as the real mode list. If the
   // new mode index is larger than the current list size then expand our list as needed.

   // See if a mode_template is being used and return the mTemplate if so.
   if (&aSensorMode == aSensorMode.GetSensor()->GetModeList()->GetModeTemplate())
   {
      return mTemplate;
   }

   // Newly created modes are copies of the template.
   size_t modeIndex(aSensorMode.GetModeIndex());
   if (modeIndex >= mModes.size())
   {
      mModes.resize(modeIndex + 1, mTemplate);
   }
   return mModes[modeIndex];
}

//! Sets the solar angle, and sets the solar exclusion flag to true.
void WsfExclusionSensorComponent::ExclusionMode::SetSolar(double aAngle)
{
   mSolarAngle = aAngle;
   mType |= cSOLAR;
}

//! Sets the lunar angle, and sets the lunar exclusion flag to true.
void WsfExclusionSensorComponent::ExclusionMode::SetLunar(double aAngle)
{
   mLunarAngle = aAngle;
   mType |= cLUNAR;
}
