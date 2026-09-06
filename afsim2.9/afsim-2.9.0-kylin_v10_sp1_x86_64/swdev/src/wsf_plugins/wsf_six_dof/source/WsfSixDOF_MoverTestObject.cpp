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

#include "WsfSixDOF_MoverTestObject.hpp"

#include "UtAtmosphere.hpp"
#include "UtMath.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfSixDOF_PilotManager.hpp"
#include "WsfSixDOF_PilotObject.hpp"
#include "WsfSixDOF_PropulsionSystem.hpp"
#include "WsfSixDOF_TunerGAManager.hpp"

std::unique_ptr<wsf::six_dof::Engine>
wsf::six_dof::MoverTestObject::GetTestEngineFromThrustProducer(ThrustProducerObject* aEngine) const
{
   std::unique_ptr<Engine> engine{nullptr};

   if (aEngine->GetEngine())
   {
      engine.reset(aEngine->GetEngine()->Clone());
      engine->SetFuelTank(aEngine->GetEngine()->GetFuelTank()->GetName());
   }

   return engine;
}

void wsf::six_dof::MoverTestObject::Update(double aSimTime)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->Update(aSimTime);
   }
}

double wsf::six_dof::MoverTestObject::GetRangeSinceStart_km() const
{
   double retVal = 0.0;
   if (GetParentVehicle() != nullptr)
   {
      retVal = GetParentVehicle()->GetRangeSinceStart_km();
   }
   return retVal;
}

void wsf::six_dof::MoverTestObject::SetMasterNoLagTesting(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->testingNoLag = aEnable;
   }
}

void wsf::six_dof::MoverTestObject::SetMasterFreezeLocation(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->location = aEnable;
   }
}

void wsf::six_dof::MoverTestObject::SetMasterFreezeAltitude(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->altitude = aEnable;
   }
}

void wsf::six_dof::MoverTestObject::SetMasterFreezeVerticalSpeed(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->verticalSpeed = aEnable;
   }
}

void wsf::six_dof::MoverTestObject::SetMasterFreezePitch(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->pitch = aEnable;
   }
}

void wsf::six_dof::MoverTestObject::SetMasterFreezeRoll(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->roll = aEnable;
   }
}

void wsf::six_dof::MoverTestObject::SetMasterFreezeYaw(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->yaw = aEnable;
   }
}

void wsf::six_dof::MoverTestObject::SetMasterFreezeFuelBurn(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->fuelBurn = aEnable;
   }
}

void wsf::six_dof::MoverTestObject::SetMasterFreezeSpeed(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->speed = aEnable;
   }
}

void wsf::six_dof::MoverTestObject::SetMasterNoAlphaTesting(bool aEnable)
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->testingNoAlpha = aEnable;
   }
}

double wsf::six_dof::MoverTestObject::GetLevelFlightHorizontalThrust(double aAltitude_ft,
                                                                     double aMach,
                                                                     double aThrottle,
                                                                     double aGrossWeight_lbs)
{
   SetupForTestingMode(true);

   double drag_lbs  = 0.0;
   double alpha_deg = 0.0;
   CalcLevelFlightDragAndAlpha(aAltitude_ft, aMach, aGrossWeight_lbs, drag_lbs, alpha_deg);

   // Get the atmosphere
   const Environment* env = GetEnvironment();
   if (env == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   PropulsionSystem* propulsionSystem = GetParentVehicle()->GetPropulsionSystem();
   if (propulsionSystem == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   int numEngines = static_cast<int>(propulsionSystem->GetNumThrustProducers());
   if (numEngines == 0)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   // Get the first engine -- we assume all are the same
   auto engine = propulsionSystem->GetThrustProducerByIndex(0);
   if (engine == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   double singleEngineThrust_lbs = ThrustAtThrottleSetting(engine, env, aAltitude_ft, aMach, aThrottle);

   RestoreFromTestingMode();

   return numEngines * singleEngineThrust_lbs * cos(alpha_deg * UtMath::cRAD_PER_DEG);
}

double wsf::six_dof::MoverTestObject::GetLevelFlightDrag(double aAltitude_ft, double aMach, double aGrossWeight_lbs)
{
   SetupForTestingMode(true);

   double drag_lbs  = 0.0;
   double alpha_deg = 0.0;

   bool valid = CalcLevelFlightDragAndAlpha(aAltitude_ft, aMach, aGrossWeight_lbs, drag_lbs, alpha_deg);

   if (!valid)
   {
      return 0.0;
   }

   RestoreFromTestingMode();

   return drag_lbs;
}

double wsf::six_dof::MoverTestObject::GetLevelFlightFuelBurnRate(double aAltitude_ft, double aMach, double aGrossWeight_lbs)
{
   SetupForTestingMode(true);

   double drag_lbs  = 0.0;
   double alpha_deg = 0.0;

   if (!CalcLevelFlightDragAndAlpha(aAltitude_ft, aMach, aGrossWeight_lbs, drag_lbs, alpha_deg))
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   if ((alpha_deg >= 90.0) || (alpha_deg <= -90.0))
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   double thrustRequired_lbs = drag_lbs / cos(alpha_deg * UtMath::cRAD_PER_DEG);

   PropulsionSystem* propulsionSystem = GetParentVehicle()->GetPropulsionSystem();
   if (propulsionSystem == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   int numEngines = static_cast<int>(propulsionSystem->GetNumThrustProducers());
   if (numEngines == 0)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   // Adjust for number of engines
   thrustRequired_lbs /= numEngines;

   // Get the first engine -- we assume all are the same
   auto engine = propulsionSystem->GetThrustProducerByIndex(0);
   if (engine == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   // Get the atmosphere
   const Environment* env = GetEnvironment();
   if (env == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   double fuelBurn_pph = numEngines * FuelBurnAtRequiredThrust(engine, env, aAltitude_ft, aMach, thrustRequired_lbs);

   RestoreFromTestingMode();

   return fuelBurn_pph;
}

void wsf::six_dof::MoverTestObject::FlyStraightLevelFullThrottle()
{
   if (GetParentVehicle() != nullptr)
   {
      double alpha_deg = 0.0;
      GetParentVehicle()->CalculateAlphaAtSpecifiedGLoad_deg(1.0, alpha_deg);

      PilotObject* pilot = GetParentVehicle()->GetActivePilotObject();
      if (pilot)
      {
         pilot->TakeTestControl();

         pilot->SetTestFlapsControllerPosition(0.0);
         pilot->SetTestStickBackControllerPosition(0.0);
         // The stick and rudder values are +/-1 while all others are 0-1.
         // Throttle assumes that 0 is idle, 1 is full power without
         // augmentation/afterburner (military power), and 2 is full power
         // with full augmentation/afterburner (full afterburner).
         pilot->SetTestThrottleControllerPosition(1.0);
      }

      GetParentVehicle()->SetAttitudeNED_rad(0, alpha_deg * UtMath::cRAD_PER_DEG, 0);
   }
}

double wsf::six_dof::MoverTestObject::GetMaxGLoad()
{
   if (GetParentVehicle() == nullptr)
   {
      return 0.0;
   }

   PilotManager* pilotMgr = GetParentVehicle()->GetPilotManager();
   if (pilotMgr == nullptr)
   {
      return 0.0;
   }

   PilotObject* pilot = pilotMgr->GetActivePilot();
   if (pilot != nullptr)
   {
      CommonController* controller = pilot->GetCommonController();
      if (controller == nullptr)
      {
         return 0.0;
      }

      AutopilotLimitsAndSettings limits = controller->GetCurrentLimitsAndSettings();
      return limits.pitchGLoad_Max;
   }

   return 0.0;
}

const wsf::six_dof::Environment* wsf::six_dof::MoverTestObject::GetEnvironment()
{
   // Get the atmosphere
   const Environment* environment = GetParentVehicle()->GetEnvironment();

   if (environment == nullptr)
   {
      ut::log::error() << "No atmosphere in MoverTestObject::GetEnvironment()";
   }

   return environment;
}

void wsf::six_dof::MoverTestObject::SetupForTestingMode(bool aFreezeFuelBurn)
{
   if (GetParentVehicle() != nullptr)
   {
      // Remember old flags
      mTestingNoLag          = GetParentVehicle()->GetFreezeFlags()->testingNoLag;
      mTestingFreezeFuelBurn = GetParentVehicle()->GetFreezeFlags()->fuelBurn;

      // Always set no lag
      GetParentVehicle()->GetFreezeFlags()->testingNoLag = true;

      // Set fuel freeze if commanded
      if (aFreezeFuelBurn)
      {
         GetParentVehicle()->GetFreezeFlags()->fuelBurn = true;
      }
   }
}

void wsf::six_dof::MoverTestObject::RestoreFromTestingMode()
{
   if (GetParentVehicle() != nullptr)
   {
      GetParentVehicle()->GetFreezeFlags()->testingNoLag = mTestingNoLag;
      GetParentVehicle()->GetFreezeFlags()->fuelBurn     = mTestingFreezeFuelBurn;
   }
}

double wsf::six_dof::MoverTestObject::ThrottleSettingToProduceThrust(ThrustProducerObject* aEngine,
                                                                     const Environment*    aEnv,
                                                                     double                aAltitude_ft,
                                                                     double                aMach,
                                                                     double                aRequiredThrust_lbs)
{
   double speed_fps       = aEnv->CalcFpsFromMach(UtMath::cM_PER_FT * aAltitude_ft, aMach);
   double dynPressure_psf = 0.0;
   double statPress_psf   = 0.0;
   double density_slugft3 = 0.0;

   aEnv->GetAeroParameters(aAltitude_ft, speed_fps, density_slugft3, dynPressure_psf, statPress_psf);

   // First, try max throttle
   double throttle             = 1.0;
   bool   afterburnerAvailable = aEngine->AfterburnerIsPresent();
   if (afterburnerAvailable)
   {
      throttle = 2.0;
   }

   double fuelBurnRate_pps = 0.0;
   double fuelBurned_lbs   = 0.0;
   double thrust_lbs       = 0.0;

   aEngine->SetThrottlePosition(throttle);

   auto engine = GetTestEngineFromThrustProducer(aEngine);
   engine->CalculateThrust(0.01,
                           aAltitude_ft,
                           dynPressure_psf,
                           statPress_psf,
                           speed_fps,
                           aMach,
                           0.0,
                           0.0,
                           thrust_lbs,
                           fuelBurnRate_pps,
                           fuelBurned_lbs);

   if (thrust_lbs > aRequiredThrust_lbs)
   {
      // We can produce the required thrust, now we need to determine the throttle position

      double maxThrottle = throttle;
      double minThrottle = 0.0;

      double thrustAtMinThrottle = 0.0;
      double thrustAtMaxThrottle = thrust_lbs;

      // Test min throttle to start our search
      engine->SetThrottlePosition(minThrottle);
      engine->CalculateThrust(0.01,
                              aAltitude_ft,
                              dynPressure_psf,
                              statPress_psf,
                              speed_fps,
                              aMach,
                              0.0,
                              0.0,
                              thrustAtMinThrottle,
                              fuelBurnRate_pps,
                              fuelBurned_lbs);

      double thrustErrorAtMinThrottle = aRequiredThrust_lbs - thrustAtMinThrottle;
      double thrustErrorAtMaxThrottle = aRequiredThrust_lbs - thrustAtMaxThrottle;

      double currentThrottle = 0.0;
      for (size_t attempt = 0; attempt < 100; ++attempt)
      {
         currentThrottle =
            utils::NextGuessFromFalsePosition(minThrottle, thrustErrorAtMinThrottle, maxThrottle, thrustErrorAtMaxThrottle);

         engine->SetThrottlePosition(currentThrottle);
         engine->CalculateThrust(0.01,
                                 aAltitude_ft,
                                 dynPressure_psf,
                                 statPress_psf,
                                 speed_fps,
                                 aMach,
                                 0.0,
                                 0.0,
                                 thrust_lbs,
                                 fuelBurnRate_pps,
                                 fuelBurned_lbs);

         double thrustError = aRequiredThrust_lbs - thrust_lbs;
         utils::UpdateFalsePositionBounds(minThrottle,
                                          thrustErrorAtMinThrottle,
                                          maxThrottle,
                                          thrustErrorAtMaxThrottle,
                                          currentThrottle,
                                          thrustError);

         double deltaThrottle = fabs(maxThrottle - minThrottle);
         if (fabs(deltaThrottle) < 0.01 || fabs(thrustError) < 2.0 * DBL_EPSILON)
         {
            break;
         }
      }

      return currentThrottle;
   }

   // If we are here, we are unable to produce the required thrust, so return zero
   return 0.0;
}

double wsf::six_dof::MoverTestObject::FuelBurnAtRequiredThrust(ThrustProducerObject* aEngine,
                                                               const Environment*    aEnv,
                                                               double                aAltitude_ft,
                                                               double                aMach,
                                                               double                aRequiredThrust_lbs)
{
   double speed_fps       = aEnv->CalcFpsFromMach(UtMath::cM_PER_FT * aAltitude_ft, aMach);
   double dynPressure_psf = 0.0;
   double statPress_psf   = 0.0;
   double density_slugft3 = 0.0;

   aEnv->GetAeroParameters(aAltitude_ft, speed_fps, density_slugft3, dynPressure_psf, statPress_psf);

   // First, try max throttle
   double throttle             = 1.0;
   bool   afterburnerAvailable = aEngine->AfterburnerIsPresent();
   if (afterburnerAvailable)
   {
      throttle = 2.0;
   }

   double fuelBurnRate_pps = 0.0;
   double fuelBurned_lbs   = 0.0;
   double thrust_lbs       = 0.0;

   aEngine->SetThrottlePosition(throttle);

   auto engine = GetTestEngineFromThrustProducer(aEngine);
   engine->CalculateThrust(0.01,
                           aAltitude_ft,
                           dynPressure_psf,
                           statPress_psf,
                           speed_fps,
                           aMach,
                           0.0,
                           0.0,
                           thrust_lbs,
                           fuelBurnRate_pps,
                           fuelBurned_lbs);

   if (thrust_lbs > aRequiredThrust_lbs)
   {
      // We can produce the required thrust, now we need to determine the throttle position

      double maxThrottle = throttle;
      double minThrottle = 0.0;

      double thrustAtMinThrottle = 0.0;
      double thrustAtMaxThrottle = thrust_lbs;

      // Test min throttle to start our search
      engine->SetThrottlePosition(minThrottle);
      engine->CalculateThrust(0.01,
                              aAltitude_ft,
                              dynPressure_psf,
                              statPress_psf,
                              speed_fps,
                              aMach,
                              0.0,
                              0.0,
                              thrustAtMinThrottle,
                              fuelBurnRate_pps,
                              fuelBurned_lbs);

      double thrustErrorAtMinThrottle = aRequiredThrust_lbs - thrustAtMinThrottle;
      double thrustErrorAtMaxThrottle = aRequiredThrust_lbs - thrustAtMaxThrottle;

      double currentThrottle = 0.0;
      for (size_t attempt = 0; attempt < 100; ++attempt)
      {
         currentThrottle =
            utils::NextGuessFromFalsePosition(minThrottle, thrustErrorAtMinThrottle, maxThrottle, thrustErrorAtMaxThrottle);

         engine->SetThrottlePosition(currentThrottle);
         engine->CalculateThrust(0.01,
                                 aAltitude_ft,
                                 dynPressure_psf,
                                 statPress_psf,
                                 speed_fps,
                                 aMach,
                                 0.0,
                                 0.0,
                                 thrust_lbs,
                                 fuelBurnRate_pps,
                                 fuelBurned_lbs);

         double thrustError = aRequiredThrust_lbs - thrust_lbs;
         utils::UpdateFalsePositionBounds(minThrottle,
                                          thrustErrorAtMinThrottle,
                                          maxThrottle,
                                          thrustErrorAtMaxThrottle,
                                          currentThrottle,
                                          thrustError);

         double deltaThrottle = fabs(maxThrottle - minThrottle);
         if (fabs(deltaThrottle) < 0.01 || fabs(thrustError) < 2.0 * DBL_EPSILON)
         {
            break;
         }
      }

      return fuelBurnRate_pps * 3600.0;
   }

   // If we are here, we are unable to produce the required thrust, so return zero fuel burn
   return 0.0;
}

double wsf::six_dof::MoverTestObject::FuelBurnAtThrottleSetting(ThrustProducerObject* aEngine,
                                                                const Environment*    aEnv,
                                                                double                aAltitude_ft,
                                                                double                aMach,
                                                                double                aThrottle)
{
   double speed_fps       = aEnv->CalcFpsFromMach(UtMath::cM_PER_FT * aAltitude_ft, aMach);
   double dynPressure_psf = 0.0;
   double statPress_psf   = 0.0;
   double density_slugft3 = 0.0;

   aEnv->GetAeroParameters(aAltitude_ft, speed_fps, density_slugft3, dynPressure_psf, statPress_psf);

   double fuelBurnRate_pps = 0.0;
   double fuelBurned_lbs   = 0.0;
   double thrust_lbs       = 0.0;

   aEngine->SetThrottlePosition(aThrottle);

   auto engine = GetTestEngineFromThrustProducer(aEngine);
   engine->CalculateThrust(0.01,
                           aAltitude_ft,
                           dynPressure_psf,
                           statPress_psf,
                           speed_fps,
                           aMach,
                           0.0,
                           0.0,
                           thrust_lbs,
                           fuelBurnRate_pps,
                           fuelBurned_lbs);

   // Convert to lbs/hr
   return fuelBurnRate_pps * 3600.0;
}

double wsf::six_dof::MoverTestObject::ThrustAtThrottleSetting(ThrustProducerObject* aEngine,
                                                              const Environment*    aEnv,
                                                              double                aAltitude_ft,
                                                              double                aMach,
                                                              double                aThrottle)
{
   double speed_fps       = aEnv->CalcFpsFromMach(UtMath::cM_PER_FT * aAltitude_ft, aMach);
   double dynPressure_psf = 0.0;
   double statPress_psf   = 0.0;
   double density_slugft3 = 0.0;

   aEnv->GetAeroParameters(aAltitude_ft, speed_fps, density_slugft3, dynPressure_psf, statPress_psf);

   double fuelBurnRate_pps = 0.0;
   double fuelBurned_lbs   = 0.0;
   double thrust_lbs       = 0.0;

   aEngine->SetThrottlePosition(aThrottle);

   auto engine = GetTestEngineFromThrustProducer(aEngine);
   engine->CalculateThrust(0.01,
                           aAltitude_ft,
                           dynPressure_psf,
                           statPress_psf,
                           speed_fps,
                           aMach,
                           0.0,
                           0.0,
                           thrust_lbs,
                           fuelBurnRate_pps,
                           fuelBurned_lbs);

   return thrust_lbs;
}
