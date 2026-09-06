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

#include "WsfSolarElevationAtTargetComponent.hpp"

#include "UtCalendar.hpp"
#include "UtEllipsoidalEarth.hpp"
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
class SolarElevationAtTargetComponentFactory : public WsfComponentFactory<WsfSensor>
{
   void PreInput(WsfSensor& aParent) override
   {
      if (aParent.IsA_TypeOf("WSF_GEOMETRIC_SENSOR") ||
          (aParent.IsClassPassive() && (aParent.IsClassInfrared() || aParent.IsClassVisual())))
      {
         WsfSolarElevationAtTargetSensorComponent::FindOrCreate(aParent);
      }
   }
};
} // namespace

// static
void WsfSolarElevationAtTargetSensorComponent::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<SolarElevationAtTargetComponentFactory>());
}

// static
WsfSolarElevationAtTargetSensorComponent* WsfSolarElevationAtTargetSensorComponent::Find(const WsfSensor& aParent)
{
   WsfSolarElevationAtTargetSensorComponent* retval = nullptr;
   aParent.GetComponents().FindByRole<WsfSolarElevationAtTargetSensorComponent>(retval);
   return retval;
}

// static
WsfSolarElevationAtTargetSensorComponent* WsfSolarElevationAtTargetSensorComponent::FindOrCreate(WsfSensor& aParent)
{
   auto componentPtr = WsfSolarElevationAtTargetSensorComponent::Find(aParent);
   if (!componentPtr)
   {
      auto newComponentPtr = ut::make_unique<WsfSolarElevationAtTargetSensorComponent>();
      if (aParent.GetComponents().AddComponent(newComponentPtr.get())) // Parent sensor takes ownership via raw pointer
      {
         componentPtr = newComponentPtr.release();
      }
   }
   return componentPtr;
}

bool WsfSolarElevationAtTargetSensorComponent::Initialize(double aSimTime)
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

bool WsfSolarElevationAtTargetSensorComponent::ProcessModeInput(UtInput& aInput, WsfSensorMode& aSensorMode)
{
   if (aInput.GetCommand() == "solar_elevation_at_target")
   {
      ElevationMode& mode = GetOrCreateElevationMode(aSensorMode);

      double minAngle, maxAngle;
      aInput.ReadValueOfType(minAngle, UtInput::cANGLE);
      aInput.ReadValueOfType(maxAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(minAngle, 0.0, maxAngle);
      aInput.ValueInClosedRange(maxAngle, 0.0, UtMath::cPI_OVER_2);

      mode.SetBounds(minAngle, maxAngle);
      return true;
   }
   return false;
}

void WsfSolarElevationAtTargetSensorComponent::PostAttemptToDetect(double           aSimTime,
                                                                   WsfPlatform*     aTargetPtr,
                                                                   WsfSensorResult& aResult)
{
   std::size_t modeIndex = GetSensor()->GetCurrentMode()->GetModeIndex();
   if (modeIndex < mModes.size())
   {
      auto& mode = mModes[modeIndex];
      if (!mode.IsEnabled())
      {
         return;
      }
#undef GetCurrentTime // Avoid conflict with Windows macro
      UtCalendar now = GetSimulation()->GetDateTime().GetCurrentTime(aSimTime);

      auto    targetPos = aTargetPtr->GetLocationWCS();
      UtVec3d sunPos;
      UtSun::GetSunLocationWCS(now, sunPos);

      const auto targetToSun = ut::coords::WCS(sunPos) - targetPos;

      double lat, lon, alt;
      aTargetPtr->GetLocationLLA(lat, lon, alt);
      const double          phi    = lat * UtMath::cRAD_PER_DEG;
      const double          lambda = lon * UtMath::cRAD_PER_DEG;
      const double          cosPhi = std::cos(phi);
      const ut::coords::WCS up{cosPhi * std::cos(lambda), cosPhi * std::sin(lambda), std::sin(phi)};

      const double cosNormal      = up.DotProduct(targetToSun) / targetToSun.Magnitude();
      const double elevationAngle = UtMath::cPI_OVER_2 - std::acos(cosNormal);

      if (elevationAngle < mode.GetMinAngle() || elevationAngle > mode.GetMaxAngle())
      {
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
      }
   }
}

WsfComponent* WsfSolarElevationAtTargetSensorComponent::CloneComponent() const
{
   return new WsfSolarElevationAtTargetSensorComponent(*this);
}

WsfStringId WsfSolarElevationAtTargetSensorComponent::GetComponentName() const
{
   return "component_solar_elevation_at_target";
}

const int* WsfSolarElevationAtTargetSensorComponent::GetComponentRoles() const
{
   static constexpr std::array<int, 3> roles = {cWSF_COMPONENT_SOLAR_ELEVATION_AT_TARGET,
                                                cWSF_COMPONENT_SENSOR_COMPONENT,
                                                cWSF_COMPONENT_NULL};
   return roles.data();
}

void* WsfSolarElevationAtTargetSensorComponent::QueryInterface(int aRole)
{
   switch (aRole)
   {
   case cWSF_COMPONENT_SOLAR_ELEVATION_AT_TARGET:
   case cWSF_COMPONENT_SENSOR_COMPONENT:
      return this;
   default:
      return nullptr;
   }
}

WsfSolarElevationAtTargetSensorComponent::ElevationMode&
WsfSolarElevationAtTargetSensorComponent::GetOrCreateElevationMode(const WsfSensorMode& aSensorMode)
{
   if (&aSensorMode == aSensorMode.GetSensor()->GetModeList()->GetModeTemplate())
   {
      return mTemplate;
   }

   const std::size_t modeIndex = aSensorMode.GetModeIndex();
   if (modeIndex >= mModes.size())
   {
      mModes.resize(modeIndex + 1, mTemplate);
   }
   return mModes[modeIndex];
}

void WsfSolarElevationAtTargetSensorComponent::ElevationMode::SetBounds(double aMinAngle, double aMaxAngle)
{
   mMinAngle = aMinAngle;
   mMaxAngle = aMaxAngle;
   mEnabled  = true;
}
