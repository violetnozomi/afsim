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

#include "WsfSolarIlluminationComponent.hpp"

#include <array>

#include "UtCalendar.hpp"
#include "UtSolarSystem.hpp"
#include "WsfDateTime.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

namespace
{
class SolarIlluminationComponentFactory : public WsfComponentFactory<WsfSensor>
{
public:
   void PreInput(WsfSensor& aParent) override
   {
      if (aParent.IsA_TypeOf("WSF_GEOMETRIC_SENSOR") ||
          (aParent.IsClassPassive() && (aParent.IsClassInfrared() || aParent.IsClassVisual())))
      {
         WsfSolarIlluminationComponent::FindOrCreate(aParent);
      }
   }
};
} // namespace

//! Called by WsfScenario::CreateTypeLists on initialization
void WsfSolarIlluminationComponent::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<SolarIlluminationComponentFactory>());
}

//! Returns the parent sensor's component, or nullptr if not found
//! @param   aParent   The sensor that we are checking for components
WsfSolarIlluminationComponent* WsfSolarIlluminationComponent::Find(const WsfSensor& aParent)
{
   WsfSolarIlluminationComponent* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<WsfSolarIlluminationComponent>(componentPtr);
   return componentPtr;
}

//! Returns the parent sensor's component, and creates one if not found
//! @param aParent   The sensor that we are checking for components
WsfSolarIlluminationComponent* WsfSolarIlluminationComponent::FindOrCreate(WsfSensor& aParent)
{
   WsfSolarIlluminationComponent* componentPtr = Find(aParent);
   if (!componentPtr)
   {
      auto newComponentPtr = ut::make_unique<WsfSolarIlluminationComponent>();
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

bool WsfSolarIlluminationComponent::Initialize(double aSimTime)
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

//! Enables component and sets min/max angles if commands are found
//! @param   aInput      The input object to read from
//! @param   aSensorMode The current sensor mode that the input controls
bool WsfSolarIlluminationComponent::ProcessModeInput(UtInput& aInput, WsfSensorMode& aSensorMode)
{
   IlluminationMode& mode(GetOrCreateIlluminationMode(aSensorMode));

   std::string command(aInput.GetCommand());
   if (command == "target_solar_illumination_angle")
   {
      double minAngle, maxAngle;
      aInput.ReadValueOfType(minAngle, UtInput::cANGLE);
      aInput.ReadValueOfType(maxAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(minAngle, 0.0, maxAngle);
      aInput.ValueInClosedRange(maxAngle, 0.0, UtMath::cPI);

      mode.SetBounds(minAngle, maxAngle);
      return true;
   }
   return false;
}

//! Calculates if solar illumination is sufficient to detect the target platform, and sets aResult respectively.
//! @param   aSimTime   The simulation time.
//! @param   aTargetPtr The platform that detection is attempted on.
//! @param   aResult    The sensor result that holds success/failure details.
void WsfSolarIlluminationComponent::PostAttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   size_t modeIndex = GetSensor()->GetCurrentMode()->GetModeIndex();
   if (modeIndex < mModes.size())
   {
      auto& mode = mModes[modeIndex];

      if (!mode.IsEnabled())
      {
         return;
      }

#undef GetCurrentTime // Avoid conflict with Windows macro
      UtCalendar now = GetSimulation()->GetDateTime().GetCurrentTime(aSimTime);

      UtVec3d targetPos;
      UtVec3d observerPos;
      UtVec3d sunPos;

      aTargetPtr->GetLocationECI(targetPos.GetData());
      GetSensor()->GetLocationECI(observerPos.GetData());

      UtSolarSystem::GetSunLocationECI(now, sunPos);

      UtVec3d obsToTarget = targetPos.Subtract(observerPos);
      UtVec3d sunToTarget = targetPos.Subtract(sunPos);

      double numerator       = obsToTarget.DotProduct(sunToTarget);
      double denominator     = obsToTarget.Magnitude() * sunToTarget.Magnitude();
      double solarIllumAngle = acos(numerator / denominator);

      if (solarIllumAngle <= mode.GetMin() || solarIllumAngle >= mode.GetMax())
      {
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
      }
   }
}

//! Clones the component and returns it in a base class pointer
WsfComponent* WsfSolarIlluminationComponent::CloneComponent() const
{
   return new WsfSolarIlluminationComponent(*this);
}

//! Returns the name of the component
WsfStringId WsfSolarIlluminationComponent::GetComponentName() const
{
   return "component_solar_illumination";
}

//! Returns the roles of the component
const int* WsfSolarIlluminationComponent::GetComponentRoles() const
{
   static const std::array<int, 3> roles = {cWSF_COMPONENT_SOLAR_ILLUMINATION,
                                            cWSF_COMPONENT_SENSOR_COMPONENT,
                                            cWSF_COMPONENT_NULL};
   return roles.data();
}

//! Returns a pointer to the object if aRole matches it, else returns nullptr
void* WsfSolarIlluminationComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_SOLAR_ILLUMINATION)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_SENSOR_COMPONENT)
   {
      return this;
   }
   return nullptr;
}

WsfSolarIlluminationComponent::IlluminationMode&
WsfSolarIlluminationComponent::GetOrCreateIlluminationMode(const WsfSensorMode& aSensorMode)
{
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

//! Sets the minimum and maximum solar illumination angles
void WsfSolarIlluminationComponent::IlluminationMode::SetBounds(double aMinAngle, double aMaxAngle)
{
   mMinAngle = aMinAngle;
   mMaxAngle = aMaxAngle;
   mEnabled  = true;
}
