// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfP6DOF_MoverTestObject.hpp"

#include "P6DofAeroCoreObject.hpp"
#include "P6DofFreezeFlags.hpp"
#include "P6DofPilotManager.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofScenario.hpp"
#include "P6DofTunerGAManager.hpp"
#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"

// Autotuning helper functions
const P6DOF::SinglePidValueData& GetPID_Value(P6DOF::Pid::Type aPid, const P6DOF::AutopilotPidGroupValueData& aPID_Values);
void                             GetRelevantPids(P6DofVehicle* aVehicle, std::vector<P6DOF::Pid::Type>& aPidVector);
void                             SendAutotuneCommand(P6DofPilotObject* aPilotObject, P6DOF::Pid::Type aPid);

WsfP6DOF_MoverTestObject::WsfP6DOF_MoverTestObject(WsfP6DOF_Mover* aParentMover)
   : mParentMover(aParentMover)
{
}

void WsfP6DOF_MoverTestObject::Update(double aSimTime)
{
   if (mParentMover != nullptr)
   {
      mParentMover->Update(aSimTime);
   }
}

double WsfP6DOF_MoverTestObject::GetRangeSinceStart_km() const
{
   double retVal = 0.0;
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      retVal = mParentMover->GetP6DofVehicle()->GetRangeSinceStart_km();
   }
   return retVal;
}

void WsfP6DOF_MoverTestObject::SetMasterNoLagTesting(bool aEnable)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      mParentMover->GetP6DofVehicle()->GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(aEnable);
   }
}

void WsfP6DOF_MoverTestObject::SetMasterFreezeLocation(bool aEnable)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      mParentMover->GetP6DofVehicle()->GetScenario()->GetFreezeFlags()->SetMasterFreezeLocation(aEnable);
   }
}

void WsfP6DOF_MoverTestObject::SetMasterFreezeAltitude(bool aEnable)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      mParentMover->GetP6DofVehicle()->GetScenario()->GetFreezeFlags()->SetMasterFreezeAltitude(aEnable);
   }
}

void WsfP6DOF_MoverTestObject::SetMasterFreezeVerticalSpeed(bool aEnable)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      mParentMover->GetP6DofVehicle()->GetScenario()->GetFreezeFlags()->SetMasterFreezeVerticalSpeed(aEnable);
   }
}

void WsfP6DOF_MoverTestObject::SetMasterFreezePitch(bool aEnable)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      mParentMover->GetP6DofVehicle()->GetScenario()->GetFreezeFlags()->SetMasterFreezePitch(aEnable);
   }
}

void WsfP6DOF_MoverTestObject::SetMasterFreezeRoll(bool aEnable)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      mParentMover->GetP6DofVehicle()->GetScenario()->GetFreezeFlags()->SetMasterFreezeRoll(aEnable);
   }
}

void WsfP6DOF_MoverTestObject::SetMasterFreezeYaw(bool aEnable)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      mParentMover->GetP6DofVehicle()->GetScenario()->GetFreezeFlags()->SetMasterFreezeYaw(aEnable);
   }
}

void WsfP6DOF_MoverTestObject::SetMasterFreezeFuelBurn(bool aEnable)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      mParentMover->GetP6DofVehicle()->GetScenario()->GetFreezeFlags()->SetMasterFreezeFuelBurn(aEnable);
   }
}

void WsfP6DOF_MoverTestObject::SetMasterNoAlphaTesting(bool aEnable)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      mParentMover->GetP6DofVehicle()->GetScenario()->GetFreezeFlags()->SetMasterNoAlphaTesting(aEnable);
   }
}

void WsfP6DOF_MoverTestObject::Autotune()
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      P6DofUtils::Autotune(mParentMover->GetP6DofVehicle());
   }
}

void WsfP6DOF_MoverTestObject::CreateAutopilotConfigFile(const std::string& aFilename)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      P6DofUtils::CreateAutopilotConfigFile(mParentMover->GetP6DofVehicle(), aFilename);
   }
}

P6DofUtils::CanFlyAtReturnValue
WsfP6DOF_MoverTestObject::CanFlyAt(double aAltitude_ft, double aVelocity_fps, double aThrottle, double aFlaps)
{
   P6DofUtils::CanFlyAtReturnValue retVal = P6DofUtils::CAN_NOT_FLY;
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      retVal = P6DofUtils::CanFlyAt(mParentMover->GetP6DofVehicle(), aAltitude_ft, aVelocity_fps, aThrottle, aFlaps);
   }
   return retVal;
}

double WsfP6DOF_MoverTestObject::TurnRateAt(double aSpecificExcessPower_fps, double aTrueSpeed_fps, double aThrottleSetting)
{
   double retVal = 0.0;

   if (mParentMover == nullptr)
   {
      ut::log::error() << "Null parent mover in WsfP6DOF_MoverTestObject::TurnRateAt().";
      return retVal;
   }

   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      ut::log::error() << "Null vehicle in WsfP6DOF_MoverTestObject::TurnRateAt().";
      return retVal;
   }

   SetupForTestingMode(true);

   double alt_ft = vehicle->GetAlt_m() * UtMath::cFT_PER_M;

   P6DofAtmosphere* atm = GetAtmosphere();
   if (atm == nullptr)
   {
      ut::log::error() << "Null atmosphere in WsfP6DOF_MoverTestObject::TurnRateAt().";
      return retVal;
   }

   double mach = atm->CalcMachFromFps(alt_ft, aTrueSpeed_fps);

   // Ps = V * (T - D) / W
   // Given W (Weight) in lbs.
   //       T (Thrust) in lbs.
   //       D (Drag) in lbs.
   //       V (Velocity) in f/s.
   double Ps = aSpecificExcessPower_fps;
   double V  = aTrueSpeed_fps;
   double W  = vehicle->GetCurrentWeight_lbs();
   double T  = 0;

   // Limit/bound throttle setting
   if (aThrottleSetting < 0.0)
   {
      aThrottleSetting = 0.0;
   }
   if (aThrottleSetting > 2.0)
   {
      aThrottleSetting = 2.0;
   }

   // Get propulsion system
   P6DofPropulsionSystem* propulsion = vehicle->GetPropulsionSystem();

   int numEngines = (int)propulsion->GetNumThrustProducers();

   // We assume all engines on the vehicle are the same, and get the first engine
   P6DofThrustProducerObject* engine = nullptr;
   if (numEngines > 0)
   {
      engine = propulsion->GetThrustProducerByIndex(0);
   }

   double singleEngineThrust_lbs = 0.0;
   if (engine != nullptr)
   {
      singleEngineThrust_lbs = ThrustAtThrottleSetting(engine, atm, alt_ft, mach, aThrottleSetting);
   }

   // Total thrust is sum of all engines
   T = singleEngineThrust_lbs * numEngines;

   // D = T - (Ps * W / V);
   double D = T - (Ps * W / V);

   // No alpha value can achieve negative drag
   if (D > 0)
   {
      bool   ok        = false;
      double alpha_rad = AlphaAtDesiredDrag(alt_ft, aTrueSpeed_fps, D, ok);

      if (ok)
      {
         double CL = 0.0;
         if (vehicle->GetActiveAeroObject()->UseLegacyAero())
         {
            CL = vehicle->GetActiveAeroObject()->CL_AlphaMach(mach, alpha_rad);
         }
         else
         {
            CL = vehicle->GetActiveAeroObject()->CL_AlphaBetaMach(mach, alpha_rad, 0.0);
         }

         double q    = atm->CalcDynamicPressure_lbft2(alt_ft, aTrueSpeed_fps);
         double lift = q * vehicle->GetWingArea_sqft() * CL;

         // If lift is not greater than weight, there is no solution
         if (lift >= W)
         {
            double G_total    = lift / W;
            double G_lateral  = std::sqrt(G_total * G_total - 1); // level flight
            double accel_fps2 = G_lateral * 32.1741;
            // TurnRate = (360 * Accel) / ( 2 * PI * Velocity)
            retVal = 360 * accel_fps2 / (2 * UtMath::cPI * V);
         }
      }
   }

   RestoreFromTestingMode();

   return retVal;
}

double WsfP6DOF_MoverTestObject::TurnRateWithConditions(double aSpecificExcessPower_fps,
                                                        double aCurrentMass_lbs,
                                                        double aMaxGLoad,
                                                        double aAltitude_ft,
                                                        double aAirspeed_fps,
                                                        double aThrottleSetting)
{
   double retVal = 0.0;

   if (mParentMover == nullptr)
   {
      ut::log::error() << "Null parent mover in WsfP6DOF_MoverTestObject::TurnRateWithConditions().";
      return retVal;
   }

   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      ut::log::error() << "Null vehicle in WsfP6DOF_MoverTestObject::TurnRateWithConditions().";
      return retVal;
   }

   SetupForTestingMode(true);

   P6DofAtmosphere* atm = GetAtmosphere();
   if (atm == nullptr)
   {
      ut::log::error() << "Null atmosphere in WsfP6DOF_MoverTestObject::TurnRateWithConditions().";
      return retVal;
   }

   double mach = atm->CalcMachFromFps(aAltitude_ft, aAirspeed_fps);

   // Ps = V * (T - D) / W
   // Given W (Weight) in lbs.
   //       T (Thrust) in lbs.
   //       D (Drag) in lbs.
   //       V (Velocity) in f/s.
   double Ps = aSpecificExcessPower_fps;
   double V  = aAirspeed_fps;
   double W  = aCurrentMass_lbs;
   double T  = 0;

   // Limit/bound throttle setting
   if (aThrottleSetting < 0.0)
   {
      aThrottleSetting = 0.0;
   }
   if (aThrottleSetting > 2.0)
   {
      aThrottleSetting = 2.0;
   }

   // Get propulsion system
   P6DofPropulsionSystem* propulsion = vehicle->GetPropulsionSystem();

   int numEngines = (int)propulsion->GetNumThrustProducers();

   // We assume all engines on the vehicle are the same, and get the first engine
   P6DofThrustProducerObject* engine = nullptr;
   if (numEngines > 0)
   {
      engine = propulsion->GetThrustProducerByIndex(0);
   }

   double singleEngineThrust_lbs = 0.0;
   if (engine != nullptr)
   {
      singleEngineThrust_lbs = ThrustAtThrottleSetting(engine, atm, aAltitude_ft, mach, aThrottleSetting);
   }

   // Total thrust is sum of all engines
   T = singleEngineThrust_lbs * numEngines;

   // D = T - (Ps * W / V);
   double D = T - (Ps * W / V);

   // No alpha value can achieve negative drag
   if (D > 0)
   {
      double alpha_rad = 0.0;
      bool   ok        = AlphaAtDesiredDrag(aAltitude_ft, mach, D, aCurrentMass_lbs, aMaxGLoad, alpha_rad);

      if (ok)
      {
         double CL = 0.0;
         if (vehicle->GetActiveAeroObject()->UseLegacyAero())
         {
            CL = vehicle->GetActiveAeroObject()->CL_AlphaMach(mach, alpha_rad);
         }
         else
         {
            CL = vehicle->GetActiveAeroObject()->CL_AlphaBetaMach(mach, alpha_rad, 0.0);
         }

         // Get the dynamic pressure (lbs/ft2)
         double q = atm->CalcDynamicPressure_lbft2(aAltitude_ft, aAirspeed_fps);

         // Calc the lift (lbs)
         double lift = q * vehicle->GetWingArea_sqft() * CL;

         // The thrust contributes (some) to the lift
         lift += T * sin(alpha_rad);

         // If lift is not greater than weight, there is no solution
         if (lift >= W)
         {
            double G_total    = lift / W;
            double G_lateral  = std::sqrt(G_total * G_total - 1); // level flight
            double accel_fps2 = G_lateral * 32.1741;

            // TurnRate = (360 * Accel) / ( 2 * PI * Velocity)
            retVal = 360 * accel_fps2 / (2 * UtMath::cPI * V);
         }
      }
   }

   RestoreFromTestingMode();

   return retVal;
}

double WsfP6DOF_MoverTestObject::GetLevelFlightHorizontalThrust(double aAltitude_ft,
                                                                double aMach,
                                                                double aThrottle,
                                                                double aGrossWeight_lbs)
{
   SetupForTestingMode(true);

   double drag_lbs  = 0.0;
   double alpha_deg = 0.0;
   CalcLevelFlightDragAndAlpha(aAltitude_ft, aMach, aGrossWeight_lbs, drag_lbs, alpha_deg);

   // Get the atmosphere
   P6DofAtmosphere* atm = GetAtmosphere();
   if (atm == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   P6DofPropulsionSystem* propulsionSystem = vehicle->GetPropulsionSystem();
   if (propulsionSystem == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   int numEngines = (int)propulsionSystem->GetNumThrustProducers();
   if (numEngines == 0)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   // Get the first engine -- we assume all are the same
   P6DofThrustProducerObject* engine = propulsionSystem->GetThrustProducerByIndex(0);
   if (engine == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   double singleEngineThrust_lbs = ThrustAtThrottleSetting(engine, atm, aAltitude_ft, aMach, aThrottle);

   RestoreFromTestingMode();

   return numEngines * singleEngineThrust_lbs * cos(alpha_deg * UtMath::cRAD_PER_DEG);
}

double WsfP6DOF_MoverTestObject::GetLevelFlightDrag(double aAltitude_ft, double aMach, double aGrossWeight_lbs)
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

double WsfP6DOF_MoverTestObject::GetPitchingMoment(double aAltitude_ft, double aMach, double aAlpha_deg, double aStickBack)
{
   SetupForTestingMode(true);

   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   double coreCmArea     = 0.0;
   double controlsCmArea = 0.0;
   double thrustOut_lbs  = 0.0;

   vehicle->CalculateCmAreaForCoreAndControls(aMach,
                                              aAlpha_deg * UtMath::cRAD_PER_DEG,
                                              aStickBack,
                                              true,
                                              thrustOut_lbs,
                                              coreCmArea,
                                              controlsCmArea);

   // Get the atmosphere
   P6DofAtmosphere* atm = GetAtmosphere();
   if (atm == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   double speed_fps       = atm->CalcFpsFromMach(aAltitude_ft, aMach);
   double dynPressure_psf = atm->CalcDynamicPressure_lbft2(aAltitude_ft, speed_fps);

   RestoreFromTestingMode();

   return dynPressure_psf * (coreCmArea + controlsCmArea);
}

double WsfP6DOF_MoverTestObject::GetLevelFlightFuelBurnRate(double aAltitude_ft, double aMach, double aGrossWeight_lbs)
{
   SetupForTestingMode(true);

   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

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

   P6DofPropulsionSystem* propulsionSystem = vehicle->GetPropulsionSystem();
   if (propulsionSystem == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   int numEngines = (int)propulsionSystem->GetNumThrustProducers();
   if (numEngines == 0)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   // Adjust for number of engines
   thrustRequired_lbs /= numEngines;

   // Get the first engine -- we assume all are the same
   P6DofThrustProducerObject* engine = propulsionSystem->GetThrustProducerByIndex(0);
   if (engine == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   // Get the atmosphere
   P6DofAtmosphere* atm = GetAtmosphere();
   if (atm == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   double fuelBurn_pph = numEngines * FuelBurnAtRequiredThrust(engine, atm, aAltitude_ft, aMach, thrustRequired_lbs);

   RestoreFromTestingMode();

   return fuelBurn_pph;
}

void WsfP6DOF_MoverTestObject::FlyStraightLevelFullThrottle()
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      double alpha_deg = 0.0;
      mParentMover->GetP6DofVehicle()->CalculateAlphaAtSpecifiedGLoad_deg(1.0, alpha_deg);
      double stickBack = 0.0;
      mParentMover->GetP6DofVehicle()->CalculateStickBackForZeroMoment(alpha_deg, stickBack);

      P6DofPilotObject* pilot = mParentMover->GetP6DofVehicle()->GetPilotObject();
      if (pilot)
      {
         pilot->TakeTestControl();

         pilot->SetTestFlapsControllerPosition(0.0);
         pilot->SetTestStickBackControllerPosition(stickBack);
         // The stick and rudder values are +/-1 while all others are 0-1.
         // Throttle assumes that 0 is idle, 1 is full power without
         // augmentation/afterburner (military power), and 2 is full power
         // with full augmentation/afterburner (full afterburner).
         pilot->SetTestThrottleControllerPosition(1.0);
      }

      mParentMover->GetP6DofVehicle()->SetAttitudeNED(0, alpha_deg * UtMath::cRAD_PER_DEG, 0);
   }
}

void WsfP6DOF_MoverTestObject::CreateAutopilotSupportFile(const std::string& aFilename)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      P6DofUtils::CreateAutopilotSupportFile(mParentMover->GetP6DofVehicle(), aFilename);
   }
}

void WsfP6DOF_MoverTestObject::CreateAutopilotSupportFileWithMaxMach(const std::string& aFilename, const double& aMaxMach)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      P6DofUtils::CreateAutopilotSupportFileWithMaxMach(mParentMover->GetP6DofVehicle(), aFilename, aMaxMach);
   }
}

void WsfP6DOF_MoverTestObject::LimitAlphaByMaxGLoad(P6DofVehicle* aVehicle,
                                                    double        aMach,
                                                    double        aMaxGLoad,
                                                    double        aDynPressure_ppsqft,
                                                    double&       aAlpha_rad)
{
   // In addition to limiting alpha, we may also need to limit the g-load.
   // If necessary, we will limit alpha to stay under the g-load limit.

   double CL = 0.0;
   if (aVehicle->GetActiveAeroObject()->UseLegacyAero())
   {
      CL = aVehicle->GetActiveAeroObject()->CL_AlphaMach(aMach, aAlpha_rad);
   }
   else
   {
      CL = aVehicle->GetActiveAeroObject()->CL_AlphaBetaMach(aMach, aAlpha_rad, 0.0);
   }

   double lift_lbs = CL * aDynPressure_ppsqft * aVehicle->GetWingArea_sqft();
   double gLoad    = lift_lbs / aVehicle->GetCurrentWeight_lbs();
   if (gLoad > aMaxGLoad)
   {
      // Need to back-off to stay under g-limit

      double tempAlpha_rad      = aAlpha_rad;
      double maxAlpha_rad       = aAlpha_rad;
      double minAlpha_rad       = 0;
      double gLoadTolerance_rad = 0.001;
      double gLoadError         = gLoad - aMaxGLoad;
      int    iterCount          = 0;

      // Loop until the g-limited alpha setting is found
      bool gLoadLimitedAlphaSettingFound = false;
      while (!gLoadLimitedAlphaSettingFound)
      {
         if (aVehicle->GetActiveAeroObject()->UseLegacyAero())
         {
            CL = aVehicle->GetActiveAeroObject()->CL_AlphaMach(aMach, tempAlpha_rad);
         }
         else
         {
            CL = aVehicle->GetActiveAeroObject()->CL_AlphaBetaMach(aMach, tempAlpha_rad, 0.0);
         }

         lift_lbs   = CL * aDynPressure_ppsqft * aVehicle->GetWingArea_sqft();
         gLoad      = lift_lbs / aVehicle->GetCurrentWeight_lbs();
         gLoadError = gLoad - aMaxGLoad;

         if (fabs(gLoadError) < gLoadTolerance_rad)
         {
            gLoadLimitedAlphaSettingFound = true;
         }
         else
         {
            if (gLoadError > 0)
            {
               maxAlpha_rad  = tempAlpha_rad;
               tempAlpha_rad = 0.5 * (tempAlpha_rad + minAlpha_rad);
            }
            else
            {
               minAlpha_rad  = tempAlpha_rad;
               tempAlpha_rad = 0.5 * (tempAlpha_rad + maxAlpha_rad);
            }

            // Limit the number of iterations to 100
            ++iterCount;
            if (iterCount > 100)
            {
               gLoadLimitedAlphaSettingFound = true;
            }
         }
      }

      // Set the new, g-limited alpha value
      aAlpha_rad = tempAlpha_rad;
   }
}

void WsfP6DOF_MoverTestObject::LimitAlphaByMaxGLoad(P6DofVehicle* aVehicle,
                                                    double        aMach,
                                                    double        aMaxGLoad,
                                                    double        aDynPressure_ppsqft,
                                                    double        aCurrentMass_lbs,
                                                    double&       aAlpha_rad)
{
   // In addition to limiting alpha, we may also need to limit the g-load.
   // If necessary, we will limit alpha to stay under the g-load limit.

   double CL = 0.0;
   if (aVehicle->GetActiveAeroObject()->UseLegacyAero())
   {
      CL = aVehicle->GetActiveAeroObject()->CL_AlphaMach(aMach, aAlpha_rad);
   }
   else
   {
      CL = aVehicle->GetActiveAeroObject()->CL_AlphaBetaMach(aMach, aAlpha_rad, 0.0);
   }

   double wingArea_ft2 = aVehicle->GetWingArea_sqft();
   double lift_lbs     = CL * aDynPressure_ppsqft * wingArea_ft2;
   double gLoad        = lift_lbs / aCurrentMass_lbs;

   if (gLoad > aMaxGLoad)
   {
      // Need to back-off to stay under g-limit

      double tempAlpha_rad      = aAlpha_rad;
      double maxAlpha_rad       = aAlpha_rad;
      double minAlpha_rad       = 0;
      double gLoadTolerance_rad = 0.001;
      double gLoadError         = gLoad - aMaxGLoad;
      int    iterCount          = 0;

      // Loop until the g-limited alpha setting is found
      bool gLoadLimitedAlphaSettingFound = false;
      while (!gLoadLimitedAlphaSettingFound)
      {
         if (aVehicle->GetActiveAeroObject()->UseLegacyAero())
         {
            CL = aVehicle->GetActiveAeroObject()->CL_AlphaMach(aMach, tempAlpha_rad);
         }
         else
         {
            CL = aVehicle->GetActiveAeroObject()->CL_AlphaBetaMach(aMach, tempAlpha_rad, 0.0);
         }

         lift_lbs   = CL * aDynPressure_ppsqft * wingArea_ft2;
         gLoad      = lift_lbs / aCurrentMass_lbs;
         gLoadError = gLoad - aMaxGLoad;

         if (fabs(gLoadError) < gLoadTolerance_rad)
         {
            gLoadLimitedAlphaSettingFound = true;
         }
         else
         {
            if (gLoadError > 0)
            {
               maxAlpha_rad  = tempAlpha_rad;
               tempAlpha_rad = 0.5 * (tempAlpha_rad + minAlpha_rad);
            }
            else
            {
               minAlpha_rad  = tempAlpha_rad;
               tempAlpha_rad = 0.5 * (tempAlpha_rad + maxAlpha_rad);
            }

            // Limit the number of iterations to 100
            ++iterCount;
            if (iterCount > 100)
            {
               gLoadLimitedAlphaSettingFound = true;
            }
         }
      }

      // Set the new, g-limited alpha value
      aAlpha_rad = tempAlpha_rad;
   }
}

void WsfP6DOF_MoverTestObject::SetVehicleMass(double aMass_lbs)
{
   mParentMover->GetP6DofVehicle()->SetCurrentWeight_lbs(aMass_lbs);
}

double WsfP6DOF_MoverTestObject::GetMaxGLoad()
{
   if (mParentMover == nullptr)
   {
      return 0.0;
   }

   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      return 0.0;
   }

   P6DofPilotManager* pilotMgr = vehicle->GetPilotManager();
   if (pilotMgr == nullptr)
   {
      return 0.0;
   }

   // Try for synthetic pilot first
   P6DofPilotObject* pilot = pilotMgr->GetActiveSyntheticPilot();
   if (pilot != nullptr)
   {
      P6DofCommonController* controller = pilot->GetCommonController();
      if (controller == nullptr)
      {
         return 0.0;
      }

      P6DOF::AutopilotLimitsAndSettings limits = controller->GetCurrentLimitsAndSettings();
      return limits.pitchGLoad_Max;
   }

   // Try augmented manual pilot next
   pilot = pilotMgr->GetActiveManualPilot();
   if (pilot != nullptr)
   {
      P6DofCommonController* controller = pilot->GetCommonController();
      if (controller == nullptr)
      {
         return 0.0;
      }

      P6DOF::AutopilotLimitsAndSettings limits = controller->GetCurrentLimitsAndSettings();
      return limits.pitchGLoad_Max;
   }

   // Try guidance pilot next
   pilot = pilotMgr->GetActiveGuidanceAutopilot();
   if (pilot != nullptr)
   {
      P6DofCommonController* controller = pilot->GetCommonController();
      if (controller == nullptr)
      {
         return 0.0;
      }

      P6DOF::AutopilotLimitsAndSettings limits = controller->GetCurrentLimitsAndSettings();
      return limits.pitchGLoad_Max;
   }

   // Try hardware autopilot
   pilot = pilotMgr->GetActiveHardwareAutopilot();
   if (pilot != nullptr)
   {
      P6DofCommonController* controller = pilot->GetCommonController();
      if (controller == nullptr)
      {
         return 0.0;
      }

      P6DOF::AutopilotLimitsAndSettings limits = controller->GetCurrentLimitsAndSettings();
      return limits.pitchGLoad_Max;
   }

   return 0.0;
}

double WsfP6DOF_MoverTestObject::MaxInstantaneousGLoad(double aAltitude_ft, double aMach, double aCurrentMass_lbs)
{
   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      return 0.0;
   }

   P6DofPilotManager* pilotMgr = vehicle->GetPilotManager();
   if (pilotMgr == nullptr)
   {
      return 0.0;
   }

   const P6DofCommonController* controller = pilotMgr->GetSpecialCommonController();

   if (controller == nullptr)
   {
      return 0.0;
   }

   // Get the atmosphere
   P6DofAtmosphere* atm = GetAtmosphere();
   if (atm == nullptr)
   {
      return 0.0;
   }

   // Calc the speed and dynamic pressure
   double speed_fps           = atm->CalcFpsFromMach(aAltitude_ft, aMach);
   double dynamicPressure_psf = atm->CalcDynamicPressure_lbft2(aAltitude_ft, speed_fps);

   if (dynamicPressure_psf <= 0.0)
   {
      ut::log::error()
         << "Dynamic pressure must be greater than zero in WsfP6DOF_MoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   // Get the reference area
   double refArea_sqft = 0.0;

   if (vehicle->GetActiveAeroObject()->UsesRefArea())
   {
      refArea_sqft = vehicle->GetRefArea_sqft();
   }
   else
   {
      refArea_sqft = vehicle->GetWingArea_sqft();
   }

   if (refArea_sqft <= 0.0)
   {
      ut::log::error()
         << "Reference area must be greater than zero in WsfP6DOF_MoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   // Get the max CL
   double maxCL = 0.0;
   bool   valid = controller->GetCLMaxMach(aMach, maxCL);
   if (!valid)
   {
      ut::log::error() << "Unable to get max CL in WsfP6DOF_MoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   // Calc the alpha to with the required CL
   double alphaToFly_deg = 0.0;
   valid                 = controller->GetAlphaVsMachCL(aMach, maxCL, alphaToFly_deg);

   if (!valid)
   {
      ut::log::error()
         << "Unable to calculate alpha via GetAlphaVsMachCL() in WsfP6DOF_MoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   // Convert to radians
   double alphaToFly_rad = alphaToFly_deg * UtMath::cRAD_PER_DEG;

   // Get the stick position for the given Mach and alpha
   double stickBack = 0.0;
   valid            = controller->GetStickForZeroMomentVsMachAlpha(aMach, alphaToFly_rad, 0.5, stickBack);

   if (!valid)
   {
      ut::log::error() << "Unable to calculate stick via GetStickForZeroMomentVsMachAlpha() in "
                          "WsfP6DOF_MoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   if ((stickBack >= 1.0) || (stickBack <= -1.0))
   {
      // Not controllable
      return 0.0;
   }

   // Calc the lift
   double lift_lbs = maxCL * dynamicPressure_psf * refArea_sqft;

   // Calc the g-load
   double gLoad = lift_lbs / aCurrentMass_lbs;

   // Limit by max g-load
   double maxGLoad = GetMaxGLoad();
   if (gLoad > maxGLoad)
   {
      gLoad = maxGLoad;
   }

   return gLoad;
}

double WsfP6DOF_MoverTestObject::AlphaAtDesiredDrag(double aAlt_ft, double aSpeed_fps, double aDrag_lbs, bool& aOk)
{
   // Default is a solution that is not valid
   aOk = false;

   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      return 0;
   }

   // Get the atmosphere
   P6DofAtmosphere* atm = GetAtmosphere();
   if (atm == nullptr)
   {
      return 0;
   }

   P6DofPilotManager* pilotMgr = vehicle->GetPilotManager();
   if (pilotMgr == nullptr)
   {
      return 0;
   }

   const P6DofCommonController* controller = pilotMgr->GetSpecialCommonController();
   if (controller == nullptr)
   {
      return 0;
   }

   // Get the mach for the current speed (ft/sec) at the current altitude
   double mach = atm->CalcMachFromFps(aAlt_ft, aSpeed_fps);

   // Get the maximum alpha (stall angle)
   double maxAlpha_deg = 0.0;
   if (!controller->GetAlphaMaxMach(mach, maxAlpha_deg))
   {
      return 0.0;
   }

   double maxAlpha_rad = maxAlpha_deg * UtMath::cRAD_PER_DEG;

   // Get the maximum allowable g-load
   P6DOF::AutopilotLimitsAndSettings limits   = controller->GetCurrentLimitsAndSettings();
   double                            maxGLoad = limits.pitchGLoad_Max;

   // Calculate the current dynamic pressure
   double q = atm->CalcDynamicPressure_lbft2(aAlt_ft, aSpeed_fps);

   // Calculate the "target" Cd to produce the desired drag
   // Cd = drag_lbs / (q * Area_sqft);
   double target_cd = aDrag_lbs / (q * vehicle->GetWingArea_sqft());

   // Get the Cd at the stall angle. This is essentially the max Cd that will be considered
   double cdAtStallAngle = 0.0;
   if (vehicle->GetActiveAeroObject()->UseLegacyAero())
   {
      cdAtStallAngle = vehicle->GetActiveAeroObject()->Cd_AlphaMach(mach, maxAlpha_rad);
   }
   else
   {
      cdAtStallAngle = vehicle->GetActiveAeroObject()->Cd_AlphaBetaMach(mach, maxAlpha_rad, 0.0);
   }

   double alpha_rad           = 0;
   double initialStepSize_rad = 1.0;
   double step_rad            = initialStepSize_rad;
   double converge_rad        = 0.000001;

   // Before attempting to converge on a solution, we need to examine whether the
   // target Cd is greater than the Cd at stall. If it is, simply return the stall alpha,
   // since that is an acceptable solution.
   if (target_cd > cdAtStallAngle)
   {
      aOk       = true; // Indicate that the solution is valid
      alpha_rad = maxAlpha_rad;

      // If necessary, we will limit alpha to stay under the g-load limit
      LimitAlphaByMaxGLoad(vehicle, mach, maxGLoad, q, alpha_rad);
      return alpha_rad;
   }

   while (true)
   {
      // If it cannot converge on a solution, return an alpha of zero
      if (step_rad < converge_rad)
      {
         return 0;
      }

      // Check for excessive alpha -- if alpha is too large (>60 deg), we can not achieve a solution
      if (alpha_rad > 1.000)
      {
         return 0;
      }

      double cd = 0.0;
      if (vehicle->GetActiveAeroObject()->UseLegacyAero())
      {
         cd = vehicle->GetActiveAeroObject()->Cd_AlphaMach(mach, alpha_rad);
      }
      else
      {
         cd = vehicle->GetActiveAeroObject()->Cd_AlphaBetaMach(mach, alpha_rad, 0.0);
      }

      if ((cd >= target_cd - 0.00001) && (cd <= target_cd + 0.00001))
      {
         // Indicate that the solution is valid
         aOk = true;

         // Check for limiting alpha to the max alpha of the aircraft.
         // Note: Reducing the alpha will result in a reduced drag. This means that
         // we will not produce the desired drag and hence not produce the desired Ps.
         // This is due to the fact that a lower alpha will no longer produce the
         // desired drag (it will be less). However, this is a correct approach for
         // turn performance charts.
         if (alpha_rad > maxAlpha_rad)
         {
            alpha_rad = maxAlpha_rad;
         }

         // If necessary, we will limit alpha to stay under the g-load limit.
         LimitAlphaByMaxGLoad(vehicle, mach, maxGLoad, q, alpha_rad);

         // Now, we need to verify that there is sufficient control authority to achieve the alpha
         //         double stickBack   = 0.0;
         double CoreCm        = 0.0;
         double ControlsCm    = 0.0;
         double ThrustOut_lbs = 0.0;
         //         double controlHigh = 1.0;
         //         double controlLow  = -1.0;

         // Try full stick back
         vehicle->CalculateCmAreaForCoreAndControls(mach, alpha_rad, 1.0, true, ThrustOut_lbs, CoreCm, ControlsCm);
         double deltaCm = CoreCm + ControlsCm;

         // We need a positive deltaCm if the controls can pull the needed alpha
         if (deltaCm < 0.0)
         {
            auto out = ut::log::warning() << "Cannot get nose to desired alpha.";
            out.AddNote() << "Desired: " << alpha_rad * UtMath::cDEG_PER_RAD << " deg";
            aOk = false; // Indicate unable to attain solution
            return 0.0;
         }

         /* Uncomment this section if we need to find the exact control position
         // We have a controllable case -- find the exact stick position
         // Set a tolerance that is "good enough" for the controls Cm
         double toleranceCm = fabs(CoreCm)*0.0001; // Get to within 1/100th of a percent
         int numAttempts = 0;
         bool controlSettingFound = false;
         while(!controlSettingFound)
         {
            vehicle->CalculateCmAreaForCoreAndControls(mach, alpha_rad, stickBack, CoreCm, ControlsCm);
            deltaCm = CoreCm + ControlsCm;

            if (fabs(deltaCm) < toleranceCm)
            {
               // We have sufficient control authority to achieve the alpha
               controlSettingFound = true;
            }
            else
            {
               if (deltaCm < 0.0)
               {
                  // Need more stick
                  controlLow = stickBack;
                  stickBack = (controlHigh + stickBack)*0.5;
               }
               else
               {
                  // Need less stick
                  controlHigh = stickBack;
                  stickBack = (controlLow + stickBack)*0.5;
               }
            }

            ++numAttempts;
            if (numAttempts > 100)
            {
               // Good enough
               controlSettingFound = true;
            }
         }
         */

         return alpha_rad;
      }
      else if (cd > target_cd)
      {
         step_rad  = step_rad / 2;
         alpha_rad = alpha_rad - step_rad;
      }
      else if (cd < target_cd)
      {
         if (step_rad >= initialStepSize_rad)
         {
            alpha_rad += step_rad;
         }
         else
         {
            step_rad  = step_rad / 2;
            alpha_rad = alpha_rad + step_rad;
         }
      }
   }

   // We lack a solution
   aOk = false;
   return 0.0;
}

bool WsfP6DOF_MoverTestObject::AlphaAtDesiredDrag(double  aAlt_ft,
                                                  double  aMach,
                                                  double  aDrag_lbs,
                                                  double  aCurrentMass_lbs,
                                                  double  aMaxGLoad,
                                                  double& aAlpha_rad)
{
   // Set default alpha value
   aAlpha_rad = 0.0;

   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      return false;
   }

   // Get the atmosphere
   P6DofAtmosphere* atm = GetAtmosphere();
   if (atm == nullptr)
   {
      return false;
   }

   P6DofPilotManager* pilotMgr = vehicle->GetPilotManager();
   if (pilotMgr == nullptr)
   {
      return false;
   }

   const P6DofCommonController* controller = pilotMgr->GetSpecialCommonController();
   if (controller == nullptr)
   {
      return false;
   }

   // Get the maximum alpha (stall angle)
   double maxAlpha_deg = 0.0;
   if (!controller->GetAlphaMaxMach(aMach, maxAlpha_deg))
   {
      return false;
   }
   double maxAlpha_rad = maxAlpha_deg * UtMath::cRAD_PER_DEG;

   // Calculate the current dynamic pressure
   double speed_fps = atm->CalcFpsFromMach(aAlt_ft, aMach);
   double q         = atm->CalcDynamicPressure_lbft2(aAlt_ft, speed_fps);

   // Calculate the "target" Cd to produce the desired drag
   // Cd = drag_lbs / (q * Area_sqft);
   double target_cd = aDrag_lbs / (q * vehicle->GetWingArea_sqft());

   // Get the Cd at the stall angle. This is essentially the max Cd that will be considered
   double cdAtStallAngle = 0.0;
   if (vehicle->GetActiveAeroObject()->UseLegacyAero())
   {
      cdAtStallAngle = vehicle->GetActiveAeroObject()->Cd_AlphaMach(aMach, maxAlpha_rad);
   }
   else
   {
      cdAtStallAngle = vehicle->GetActiveAeroObject()->Cd_AlphaBetaMach(aMach, maxAlpha_rad, 0.0);
   }

   double alpha_rad           = 0;
   double initialStepSize_rad = 1.0;
   double step_rad            = initialStepSize_rad;
   double converge_rad        = 0.000001;

   // Before attempting to converge on a solution, we need to examine whether the
   // target Cd is greater than the Cd at stall. If it is, simply return the stall alpha,
   // since that is an acceptable solution.
   if (target_cd > cdAtStallAngle)
   {
      alpha_rad = maxAlpha_rad;

      // If necessary, we will limit alpha to stay under the g-load limit
      LimitAlphaByMaxGLoad(vehicle, aMach, aMaxGLoad, q, aCurrentMass_lbs, alpha_rad);

      // Set return value
      aAlpha_rad = alpha_rad;

      return true;
   }

   while (true)
   {
      // If it cannot converge on a solution, return an alpha of zero
      if (step_rad < converge_rad)
      {
         return false;
      }

      // Check for excessive alpha -- if alpha is too large (>60 deg), we can not achieve a solution
      if (alpha_rad > 1.000)
      {
         return false;
      }

      double cd = 0.0;
      if (vehicle->GetActiveAeroObject()->UseLegacyAero())
      {
         cd = vehicle->GetActiveAeroObject()->Cd_AlphaMach(aMach, alpha_rad);
      }
      else
      {
         cd = vehicle->GetActiveAeroObject()->Cd_AlphaBetaMach(aMach, alpha_rad, 0.0);
      }

      if ((cd >= target_cd - 0.00001) && (cd <= target_cd + 0.00001))
      {
         // Check for limiting alpha to the max alpha of the aircraft.
         // Note: Reducing the alpha will result in a reduced drag. This means that
         // we will not produce the desired drag and hence not produce the desired Ps.
         // This is due to the fact that a lower alpha will no longer produce the
         // desired drag (it will be less). However, this is a correct approach for
         // turn performance charts.
         if (alpha_rad > maxAlpha_rad)
         {
            alpha_rad = maxAlpha_rad;
         }

         // If necessary, we will limit alpha to stay under the g-load limit.
         LimitAlphaByMaxGLoad(vehicle, aMach, aMaxGLoad, q, aCurrentMass_lbs, alpha_rad);

         // Now, we need to verify that there is sufficient control authority to achieve the alpha
         double CoreCm        = 0.0;
         double ControlsCm    = 0.0;
         double ThrustOut_lbs = 0.0;

         // Try full stick back
         vehicle->CalculateCmAreaForCoreAndControls(aMach, alpha_rad, 1.0, true, ThrustOut_lbs, CoreCm, ControlsCm);
         double deltaCm = CoreCm + ControlsCm;

         // We need a positive deltaCm if the controls can pull the needed alpha
         if (deltaCm < 0.0)
         {
            auto out = ut::log::warning() << "Cannot get nose to desired alpha.";
            out.AddNote() << "Desired: " << alpha_rad * UtMath::cDEG_PER_RAD << " deg";
            // Indicate unable to attain solution
            return false;
         }

         /* Uncomment this section if we need to find the exact control position
         // We have a controllable case -- find the exact stick position
         // Set a tolerance that is "good enough" for the controls Cm
         double toleranceCm = fabs(CoreCm)*0.0001; // Get to within 1/100th of a percent
         int numAttempts = 0;
         bool controlSettingFound = false;
         while(!controlSettingFound)
         {
         vehicle->CalculateCmAreaForCoreAndControls(mach, alpha_rad, stickBack, CoreCm, ControlsCm);
         deltaCm = CoreCm + ControlsCm;

         if (fabs(deltaCm) < toleranceCm)
         {
         // We have sufficient control authority to achieve the alpha
         controlSettingFound = true;
         }
         else
         {
         if (deltaCm < 0.0)
         {
         // Need more stick
         controlLow = stickBack;
         stickBack = (controlHigh + stickBack)*0.5;
         }
         else
         {
         // Need less stick
         controlHigh = stickBack;
         stickBack = (controlLow + stickBack)*0.5;
         }
         }

         ++numAttempts;
         if (numAttempts > 100)
         {
         // Good enough
         controlSettingFound = true;
         }
         }
         */

         // Set return value
         aAlpha_rad = alpha_rad;

         return true;
      }
      else if (cd > target_cd)
      {
         step_rad  = step_rad / 2;
         alpha_rad = alpha_rad - step_rad;
      }
      else if (cd < target_cd)
      {
         if (step_rad >= initialStepSize_rad)
         {
            alpha_rad += step_rad;
         }
         else
         {
            step_rad  = step_rad / 2;
            alpha_rad = alpha_rad + step_rad;
         }
      }
   }

   // We lack a solution
   return false;
}

P6DofAtmosphere* WsfP6DOF_MoverTestObject::GetAtmosphere()
{
   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      ut::log::error() << "Null vehicle in WsfP6DOF_MoverTestObject::GetAtmosphere().";
      return nullptr;
   }

   // Get the atmosphere
   P6DofAtmosphere* atm      = nullptr;
   P6DofScenario*   scenario = vehicle->GetScenario();
   if (scenario != nullptr)
   {
      atm = scenario->GetAtmosphere();
   }

   if (atm == nullptr)
   {
      ut::log::error() << "Null atmosphere in WsfP6DOF_MoverTestObject::GetAtmosphere().";
   }

   return atm;
}

void WsfP6DOF_MoverTestObject::SetupForTestingMode(bool aFreezeFuelBurn)
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();

      // Remember old flags
      mTestingNolag          = vehicle->GetScenario()->GetFreezeFlags()->GetMasterNoLagTesting();
      mTestingFreezeFuelBurn = vehicle->GetScenario()->GetFreezeFlags()->GetMasterFreezeFuelBurn();

      // Always set no lag
      vehicle->GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(true);

      // Set fuel freeze if commanded
      if (aFreezeFuelBurn)
      {
         vehicle->GetScenario()->GetFreezeFlags()->SetMasterFreezeFuelBurn(true);
      }
   }
}

void WsfP6DOF_MoverTestObject::RestoreFromTestingMode()
{
   if (mParentMover != nullptr && mParentMover->GetP6DofVehicle() != nullptr)
   {
      P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();

      vehicle->GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(mTestingNolag);
      vehicle->GetScenario()->GetFreezeFlags()->SetMasterFreezeFuelBurn(mTestingFreezeFuelBurn);
   }
}

bool WsfP6DOF_MoverTestObject::CalcLevelFlightDragAndAlpha(double  aAltitude_ft,
                                                           double  aMach,
                                                           double  aGrossWeight_lbs,
                                                           double& aDrag_lbs,
                                                           double& aAlpha_deg)
{
   P6DofVehicle* vehicle = mParentMover->GetP6DofVehicle();
   if (vehicle == nullptr)
   {
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   P6DofPilotManager* pilotMgr = vehicle->GetPilotManager();
   if (pilotMgr == nullptr)
   {
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   const P6DofCommonController* controller = pilotMgr->GetSpecialCommonController();

   if (controller == nullptr)
   {
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Get the atmosphere
   P6DofAtmosphere* atm = GetAtmosphere();
   if (atm == nullptr)
   {
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Calc the speed and dynamic pressure
   double speed_fps           = atm->CalcFpsFromMach(aAltitude_ft, aMach);
   double dynamicPressure_psf = atm->CalcDynamicPressure_lbft2(aAltitude_ft, speed_fps);

   if (dynamicPressure_psf <= 0.0)
   {
      if (dynamicPressure_psf < 0.0)
      {
         ut::log::error() << "Dynamic pressure must be greater than or equal to zero in "
                             "WsfP6DOF_MoverTestObject::CalcLevelFlightDragAndAlpha().";
      }
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Get the reference area
   double refArea_sqft = 0.0;

   if (vehicle->GetActiveAeroObject()->UsesRefArea())
   {
      refArea_sqft = vehicle->GetRefArea_sqft();
   }
   else
   {
      refArea_sqft = vehicle->GetWingArea_sqft();
   }

   if (refArea_sqft <= 0.0)
   {
      ut::log::error()
         << "Reference area must be greater than zero in WsfP6DOF_MoverTestObject::CalcLevelFlightDragAndAlpha().";
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Calculate the CL required to fly
   double requiredCL = aGrossWeight_lbs / (refArea_sqft * dynamicPressure_psf);

   double maxCL = 0.0;
   bool   valid = controller->GetCLMaxMach(aMach, maxCL);
   if (!valid)
   {
      ut::log::error() << "Unable to get valid max CL in WsfP6DOF_MoverTestObject::CalcLevelFlightDragAndAlpha().";
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   if (requiredCL > maxCL)
   {
      // This is not an error case, but we need to return zero values to indicate an unattainable condition
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Calc the alpha to with the required CL
   double alphaToFly_deg = 0.0;
   valid                 = controller->GetAlphaVsMachCL(aMach, requiredCL, alphaToFly_deg);

   if (!valid)
   {
      ut::log::error() << "Unable to calculate alpha via GetAlphaVsMachCL() in "
                          "WsfP6DOF_MoverTestObject::CalcLevelFlightDragAndAlpha().";
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Convert to radians
   double alphaToFly_rad = alphaToFly_deg * UtMath::cRAD_PER_DEG;

   // Get the stick position for the given Mach and alpha
   double stickBack = 0.0;
   valid            = controller->GetStickForZeroMomentVsMachAlpha(aMach, alphaToFly_rad, 0.5, stickBack);

   if (!valid)
   {
      ut::log::error() << "Unable to calculate stick via GetStickForZeroMomentVsMachAlpha() in "
                          "WsfP6DOF_MoverTestObject::CalcLevelFlightDragAndAlpha().";
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Check that the stick is valid
   if ((stickBack >= 1.0) || (stickBack <= -1.0))
   {
      // Not controllable
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // We now need to calculate the drag
   double coreCdArea     = 0.0;
   double controlsCdArea = 0.0;
   vehicle->CalculateCdAreaForCoreAndControls(aMach, alphaToFly_rad, stickBack, coreCdArea, controlsCdArea);
   double totalCdArea = coreCdArea + controlsCdArea;

   aDrag_lbs  = dynamicPressure_psf * totalCdArea;
   aAlpha_deg = alphaToFly_deg;

   return true;
}

double WsfP6DOF_MoverTestObject::ThrottleSettingToProduceThrust(P6DofThrustProducerObject* aEngine,
                                                                P6DofAtmosphere*           aAtm,
                                                                double                     aAltitude_ft,
                                                                double                     aMach,
                                                                double                     aRequiredThrust_lbs)
{
   double speed_fps         = aAtm->CalcFpsFromMach(aAltitude_ft, aMach);
   double dynPressure_psf   = aAtm->CalcDynamicPressure_lbft2(aAltitude_ft, speed_fps);
   double statPress_lbssqft = aAtm->StaticPressureAtAltitude(aAltitude_ft);

   // First, try max throttle
   double throttle             = 1.0;
   bool   afterburnerAvailable = aEngine->AfterburnerIsPresent();
   if (afterburnerAvailable)
   {
      throttle = 2.0;
   }

   double                     fuelBurnRate_pps = 0.0;
   double                     fuelBurned_lbs   = 0.0;
   P6DofForceAndMomentsObject forceAndMoment;

   aEngine->SetThrottlePosition(throttle);
   aEngine->CalculateThrust(0.01,
                            aAltitude_ft,
                            dynPressure_psf,
                            statPress_lbssqft,
                            speed_fps,
                            aMach,
                            0.0,
                            0.0,
                            forceAndMoment,
                            fuelBurnRate_pps,
                            fuelBurned_lbs);
   double thrust_lbs = forceAndMoment.GetForceMagnitude_lbs();

   if (thrust_lbs > aRequiredThrust_lbs)
   {
      // We can produce the required thrust, now we need to determine the throttle position

      double maxThrottle     = throttle;
      double minThrottle     = 0.0;
      double currentThrottle = 0.5 * (maxThrottle + minThrottle);

      bool   done          = false;
      double deltaThrottle = 2.0;
      while (!done)
      {
         aEngine->SetThrottlePosition(currentThrottle);
         aEngine->CalculateThrust(0.01,
                                  aAltitude_ft,
                                  dynPressure_psf,
                                  statPress_lbssqft,
                                  speed_fps,
                                  aMach,
                                  0.0,
                                  0.0,
                                  forceAndMoment,
                                  fuelBurnRate_pps,
                                  fuelBurned_lbs);
         thrust_lbs = forceAndMoment.GetForceMagnitude_lbs();

         if (thrust_lbs > aRequiredThrust_lbs)
         {
            // Decrease the throttle
            maxThrottle         = currentThrottle;
            double lastThrottle = currentThrottle;
            currentThrottle     = 0.5 * (maxThrottle + minThrottle);
            deltaThrottle       = currentThrottle - lastThrottle;
         }
         else if (thrust_lbs < aRequiredThrust_lbs)
         {
            // Increase the throttle
            minThrottle         = currentThrottle;
            double lastThrottle = currentThrottle;
            currentThrottle     = 0.5 * (maxThrottle + minThrottle);
            deltaThrottle       = lastThrottle - currentThrottle;
         }
         else
         {
            done = true;
         }

         if (fabs(deltaThrottle) < 0.01)
         {
            done = true;
         }
      }

      return currentThrottle;
   }

   // If we are here, we are unable to produce the required thrust, so return zero
   return 0.0;
}

double WsfP6DOF_MoverTestObject::FuelBurnAtRequiredThrust(P6DofThrustProducerObject* aEngine,
                                                          P6DofAtmosphere*           aAtm,
                                                          double                     aAltitude_ft,
                                                          double                     aMach,
                                                          double                     aRequiredThrust_lbs)
{
   double speed_fps         = aAtm->CalcFpsFromMach(aAltitude_ft, aMach);
   double dynPressure_psf   = aAtm->CalcDynamicPressure_lbft2(aAltitude_ft, speed_fps);
   double statPress_lbssqft = aAtm->StaticPressureAtAltitude(aAltitude_ft);

   // First, try max throttle
   double throttle             = 1.0;
   bool   afterburnerAvailable = aEngine->AfterburnerIsPresent();
   if (afterburnerAvailable)
   {
      throttle = 2.0;
   }

   double                     fuelBurnRate_pps = 0.0;
   double                     fuelBurned_lbs   = 0.0;
   P6DofForceAndMomentsObject forceAndMoment;

   aEngine->SetThrottlePosition(throttle);
   aEngine->CalculateThrust(0.01,
                            aAltitude_ft,
                            dynPressure_psf,
                            statPress_lbssqft,
                            speed_fps,
                            aMach,
                            0.0,
                            0.0,
                            forceAndMoment,
                            fuelBurnRate_pps,
                            fuelBurned_lbs);
   double thrust_lbs = forceAndMoment.GetForceMagnitude_lbs();

   if (thrust_lbs > aRequiredThrust_lbs)
   {
      // We can produce the required thrust, now we need to determine the throttle position

      double maxThrottle         = throttle;
      double minThrottle         = 0.0;
      double currentThrottle     = 0.5 * (maxThrottle + minThrottle);
      double currentFuelBurn_pph = 0.0;

      bool   done          = false;
      double deltaThrottle = 2.0;
      while (!done)
      {
         aEngine->SetThrottlePosition(currentThrottle);
         aEngine->CalculateThrust(0.01,
                                  aAltitude_ft,
                                  dynPressure_psf,
                                  statPress_lbssqft,
                                  speed_fps,
                                  aMach,
                                  0.0,
                                  0.0,
                                  forceAndMoment,
                                  fuelBurnRate_pps,
                                  fuelBurned_lbs);
         thrust_lbs          = forceAndMoment.GetForceMagnitude_lbs();
         currentFuelBurn_pph = fuelBurnRate_pps * 3600.0;

         if (thrust_lbs > aRequiredThrust_lbs)
         {
            // Decrease the throttle
            maxThrottle         = currentThrottle;
            double lastThrottle = currentThrottle;
            currentThrottle     = 0.5 * (maxThrottle + minThrottle);
            deltaThrottle       = currentThrottle - lastThrottle;
         }
         else if (thrust_lbs < aRequiredThrust_lbs)
         {
            // Increase the throttle
            minThrottle         = currentThrottle;
            double lastThrottle = currentThrottle;
            currentThrottle     = 0.5 * (maxThrottle + minThrottle);
            deltaThrottle       = lastThrottle - currentThrottle;
         }
         else
         {
            done = true;
         }

         if (fabs(deltaThrottle) < 0.005)
         {
            done = true;
         }
      }

      return currentFuelBurn_pph;
   }

   // If we are here, we are unable to produce the required thrust, so return zero fuel burn
   return 0.0;
}

double WsfP6DOF_MoverTestObject::FuelBurnAtThrottleSetting(P6DofThrustProducerObject* aEngine,
                                                           P6DofAtmosphere*           aAtm,
                                                           double                     aAltitude_ft,
                                                           double                     aMach,
                                                           double                     aThrottle)
{
   double speed_fps         = aAtm->CalcFpsFromMach(aAltitude_ft, aMach);
   double dynPressure_psf   = aAtm->CalcDynamicPressure_lbft2(aAltitude_ft, speed_fps);
   double statPress_lbssqft = aAtm->StaticPressureAtAltitude(aAltitude_ft);

   double                     fuelBurnRate_pps = 0.0;
   double                     fuelBurned_lbs   = 0.0;
   P6DofForceAndMomentsObject forceAndMoment;

   aEngine->SetThrottlePosition(aThrottle);
   aEngine->CalculateThrust(0.01,
                            aAltitude_ft,
                            dynPressure_psf,
                            statPress_lbssqft,
                            speed_fps,
                            aMach,
                            0.0,
                            0.0,
                            forceAndMoment,
                            fuelBurnRate_pps,
                            fuelBurned_lbs);

   // Convert to lbs/hr
   return fuelBurnRate_pps * 3600.0;
}

double WsfP6DOF_MoverTestObject::ThrustAtThrottleSetting(P6DofThrustProducerObject* aEngine,
                                                         P6DofAtmosphere*           aAtm,
                                                         double                     aAltitude_ft,
                                                         double                     aMach,
                                                         double                     aThrottle)
{
   double speed_fps         = aAtm->CalcFpsFromMach(aAltitude_ft, aMach);
   double dynPressure_psf   = aAtm->CalcDynamicPressure_lbft2(aAltitude_ft, speed_fps);
   double statPress_lbssqft = aAtm->StaticPressureAtAltitude(aAltitude_ft);

   double                     fuelBurnRate_pps = 0.0;
   double                     fuelBurned_lbs   = 0.0;
   P6DofForceAndMomentsObject forceAndMoment;

   aEngine->SetThrottlePosition(aThrottle);
   aEngine->CalculateThrust(0.01,
                            aAltitude_ft,
                            dynPressure_psf,
                            statPress_lbssqft,
                            speed_fps,
                            aMach,
                            0.0,
                            0.0,
                            forceAndMoment,
                            fuelBurnRate_pps,
                            fuelBurned_lbs);

   return forceAndMoment.GetForceMagnitude_lbs();
}
