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

#include "WsfMoonLOS_SensorComponent.hpp"

#include <array>

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
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
class MoonLOSSensorComponentFactory : public WsfComponentFactory<WsfSensor>
{
public:
   void PreInput(WsfSensor& aParent) override { WsfMoonLOS_SensorComponent::FindOrCreate(aParent); }
};
} // namespace

//! Called by WsfScenario::CreateTypeLists on initialization
void WsfMoonLOS_SensorComponent::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<MoonLOSSensorComponentFactory>());
}

//! Returns the parent sensor's component, or nullptr if not found
//! @param   aParent   The sensor that we are checking for components
WsfMoonLOS_SensorComponent* WsfMoonLOS_SensorComponent::Find(const WsfSensor& aParent)
{
   WsfMoonLOS_SensorComponent* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<WsfMoonLOS_SensorComponent>(componentPtr);
   return componentPtr;
}

//! Returns the parent sensor's component, or creates one if not found
//! @param aParent   The sensor that we are checking for components
WsfMoonLOS_SensorComponent* WsfMoonLOS_SensorComponent::FindOrCreate(WsfSensor& aParent)
{
   WsfMoonLOS_SensorComponent* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      auto newComponentPtr = ut::make_unique<WsfMoonLOS_SensorComponent>();
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

bool WsfMoonLOS_SensorComponent::Initialize(double aSimTime)
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
bool WsfMoonLOS_SensorComponent::ProcessModeInput(UtInput& aInput, WsfSensorMode& aSensorMode)
{
   std::string command(aInput.GetCommand());
   if (command == "enable_moon_los_block")
   {
      bool enabled;
      aInput.ReadValue(enabled);
      // Sets the template as enabled if the template is the current mode
      if (&aSensorMode == aSensorMode.GetSensor()->GetModeList()->GetModeTemplate())
      {
         mTemplate = enabled;
      }
      else
      {
         // Resizes the mode enable list if the current mode value is not saved yet
         size_t modeIndex(aSensorMode.GetModeIndex());
         if (modeIndex >= mModes.size())
         {
            mModes.resize(modeIndex + 1, mTemplate);
         }
         mModes[modeIndex] = enabled;
      }
      return true;
   }
   return false;
}

//! Calculates if the sensor and target LOS is blocked by the moon, and sets aResult respectively.
//! @param   aSimTime   The simulation time.
//! @param   aTargetPtr The platform that detection is attempted on.
//! @param   aResult    The sensor result that holds success/failure details.
void WsfMoonLOS_SensorComponent::PostAttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   size_t modeIndex = GetSensor()->GetCurrentMode()->GetModeIndex();
   if ((modeIndex < mModes.size() && mModes[modeIndex]))
   {
#undef GetCurrentTime // Avoid conflict with Windows macro
      UtCalendar now = GetSimulation()->GetDateTime().GetCurrentTime(aSimTime);
      UtVec3d    platformLoc;
      UtVec3d    sensorLoc;
      UtVec3d    moonLoc;

      aTargetPtr->GetLocationECI(platformLoc.GetData());
      GetSensor()->GetLocationECI(sensorLoc.GetData());
      UtSolarSystem::GetMoonLocationECI(now, moonLoc);

      if (!ut::CentralBody::LineOfSight(sensorLoc, platformLoc, moonLoc, UtMoon::cMEAN_RADIUS))
      {
         aResult.mFailedStatus |= WsfSensorResult::cMOON_BLOCKED;
      }
   }
}

//! Clones the component and returns it in a base class pointer
WsfComponent* WsfMoonLOS_SensorComponent::CloneComponent() const
{
   return new WsfMoonLOS_SensorComponent(*this);
}

//! Returns the name of the component
WsfStringId WsfMoonLOS_SensorComponent::GetComponentName() const
{
   return "component_moon_los";
}

//! Returns the roles of the component
const int* WsfMoonLOS_SensorComponent::GetComponentRoles() const
{
   static const std::array<int, 3> roles = {cWSF_COMPONENT_SENSOR_MOON_LOS,
                                            cWSF_COMPONENT_SENSOR_COMPONENT,
                                            cWSF_COMPONENT_NULL};
   return roles.data();
}

//! Returns a pointer to the object if aRole matches it, else returns nullptr
void* WsfMoonLOS_SensorComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_SENSOR_MOON_LOS)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_SENSOR_COMPONENT)
   {
      return this;
   }
   return nullptr;
}
