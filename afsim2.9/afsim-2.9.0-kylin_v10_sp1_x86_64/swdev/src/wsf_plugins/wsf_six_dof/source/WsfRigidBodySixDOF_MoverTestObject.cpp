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

#include "WsfRigidBodySixDOF_MoverTestObject.hpp"

#include "UtMath.hpp"
#include "WsfRigidBodySixDOF_AeroCoreObject.hpp"
#include "WsfRigidBodySixDOF_Mover.hpp"
#include "WsfRigidBodySixDOF_PilotManager.hpp"
#include "WsfRigidBodySixDOF_PropulsionSystem.hpp"
#include "WsfSixDOF_Environment.hpp"

wsf::six_dof::RigidBodyMoverTestObject::RigidBodyMoverTestObject(RigidBodyMover* aParentMover)
   : mParentMover(aParentMover)
{
}

wsf::six_dof::Mover* wsf::six_dof::RigidBodyMoverTestObject::GetParentVehicle() const
{
   return mParentMover;
}

void wsf::six_dof::RigidBodyMoverTestObject::Autotune()
{
   if (mParentMover != nullptr)
   {
      utils::Autotune(mParentMover);
   }
}

void wsf::six_dof::RigidBodyMoverTestObject::CreateAutopilotConfigFile(const std::string& aFilename)
{
   if (mParentMover != nullptr)
   {
      utils::CreateAutopilotConfigFile(mParentMover, aFilename);
   }
}

wsf::six_dof::utils::CanFlyAtReturnValue wsf::six_dof::RigidBodyMoverTestObject::CanFlyAt(double aAltitude_ft,
                                                                                          double aVelocity_fps,
                                                                                          double aThrottle,
                                                                                          double aFlaps)
{
   utils::CanFlyAtReturnValue retVal = utils::CAN_NOT_FLY;
   if (mParentMover != nullptr)
   {
      retVal = utils::CanFlyAt(mParentMover, aAltitude_ft, aVelocity_fps, aThrottle, aFlaps);
   }
   return retVal;
}

double wsf::six_dof::RigidBodyMoverTestObject::TurnRateAt(double aSpecificExcessPower_fps,
                                                          double aTrueSpeed_fps,
                                                          double aThrottleSetting)
{
   double retVal = 0.0;

   if (mParentMover == nullptr)
   {
      ut::log::error() << "Null parent mover in RigidBodyMoverTestObject::TurnRateAt().";
      return retVal;
   }

   SetupForTestingMode(true);

   double alt_m  = mParentMover->GetAltitudeMSL_m();
   double alt_ft = UtMath::cFT_PER_M * alt_m;

   const Environment* env = GetEnvironment();
   if (env == nullptr)
   {
      ut::log::error() << "Null environment in RigidBodyMoverTestObject::TurnRateAt().";
      return retVal;
   }

   double mach = env->CalcMachFromFps(alt_m, aTrueSpeed_fps);

   // Ps = V * (T - D) / W
   // Given W (Weight) in lbs.
   //       T (Thrust) in lbs.
   //       D (Drag) in lbs.
   //       V (Velocity) in f/s.
   double Ps = aSpecificExcessPower_fps;
   double V  = aTrueSpeed_fps;
   double W  = mParentMover->GetCurrentWeight_lbs();
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
   PropulsionSystem* propulsion = mParentMover->GetPropulsionSystem();

   int numEngines = static_cast<int>(propulsion->GetNumThrustProducers());

   // We assume all engines on the vehicle are the same, and get the first engine
   ThrustProducerObject* engine = nullptr;
   if (numEngines > 0)
   {
      engine = propulsion->GetThrustProducerByIndex(0);
   }

   double singleEngineThrust_lbs = 0.0;
   if (engine != nullptr)
   {
      singleEngineThrust_lbs = ThrustAtThrottleSetting(engine, env, alt_ft, mach, aThrottleSetting);
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
         double CL = mParentMover->GetActiveAeroObject()->CL_AlphaBetaMach(mach, alpha_rad, 0.0);

         double dynPressure_psf = env->CalcDynamicPressure_psf(UtMath::cM_PER_FT * alt_ft, aTrueSpeed_fps);
         double lift            = 0.0;
         if (mParentMover->GetActiveAeroObject()->UsesRefArea())
         {
            lift = dynPressure_psf * mParentMover->GetRefArea_sqft() * CL;
         }
         else
         {
            lift = dynPressure_psf * mParentMover->GetWingArea_sqft() * CL;
         }

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

double wsf::six_dof::RigidBodyMoverTestObject::TurnRateWithConditions(double aSpecificExcessPower_fps,
                                                                      double aCurrentMass_lbs,
                                                                      double aMaxGLoad,
                                                                      double aAltitude_ft,
                                                                      double aAirspeed_fps,
                                                                      double aThrottleSetting)
{
   double retVal = 0.0;

   if (mParentMover == nullptr)
   {
      ut::log::error() << "Null parent mover in RigidBodyMoverTestObject::TurnRateWithConditions().";
      return retVal;
   }

   SetupForTestingMode(true);

   const Environment* env = GetEnvironment();
   if (env == nullptr)
   {
      ut::log::error() << "Null environment in RigidBodyMoverTestObject::TurnRateWithConditions().";
      return retVal;
   }

   double mach = env->CalcMachFromFps(UtMath::cM_PER_FT * aAltitude_ft, aAirspeed_fps);

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
   PropulsionSystem* propulsion = mParentMover->GetPropulsionSystem();

   int numEngines = static_cast<int>(propulsion->GetNumThrustProducers());

   // We assume all engines on the vehicle are the same, and get the first engine
   ThrustProducerObject* engine = nullptr;
   if (numEngines > 0)
   {
      engine = propulsion->GetThrustProducerByIndex(0);
   }

   double singleEngineThrust_lbs = 0.0;
   if (engine != nullptr)
   {
      singleEngineThrust_lbs = ThrustAtThrottleSetting(engine, env, aAltitude_ft, mach, aThrottleSetting);
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
         double CL = mParentMover->GetActiveAeroObject()->CL_AlphaBetaMach(mach, alpha_rad, 0.0);

         // Get the dynamic pressure (lbs/ft2)
         double dynPressure_psf = env->CalcDynamicPressure_psf(UtMath::cM_PER_FT * aAltitude_ft, aAirspeed_fps);

         // Calc the lift (lbs)
         double lift = 0.0;
         if (mParentMover->GetActiveAeroObject()->UsesRefArea())
         {
            lift = dynPressure_psf * mParentMover->GetRefArea_sqft() * CL;
         }
         else
         {
            lift = dynPressure_psf * mParentMover->GetWingArea_sqft() * CL;
         }

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

double wsf::six_dof::RigidBodyMoverTestObject::GetPitchingMoment(double aAltitude_ft,
                                                                 double aMach,
                                                                 double aAlpha_deg,
                                                                 double aStickBack)
{
   SetupForTestingMode(true);

   double coreCmArea     = 0.0;
   double controlsCmArea = 0.0;
   double thrustOut_lbs  = 0.0;

   mParentMover->CalculateCmAreaForCoreAndControls(aMach,
                                                   aAlpha_deg * UtMath::cRAD_PER_DEG,
                                                   aStickBack,
                                                   true,
                                                   thrustOut_lbs,
                                                   coreCmArea,
                                                   controlsCmArea);

   // Get the atmosphere
   const Environment* env = GetEnvironment();
   if (env == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   double altitude_m      = UtMath::cM_PER_FT * aAltitude_ft;
   double speed_fps       = env->CalcFpsFromMach(altitude_m, aMach);
   double dynPressure_psf = env->CalcDynamicPressure_psf(altitude_m, speed_fps);

   RestoreFromTestingMode();

   return dynPressure_psf * (coreCmArea + controlsCmArea);
}

void wsf::six_dof::RigidBodyMoverTestObject::CreateAutopilotSupportFile(const std::string& aFilename)
{
   if (mParentMover != nullptr)
   {
      utils::CreateAutopilotSupportFile(mParentMover, aFilename);
   }
}

void wsf::six_dof::RigidBodyMoverTestObject::CreateAutopilotSupportFileWithMaxMach(const std::string& aFilename,
                                                                                   const double&      aMaxMach)
{
   if (mParentMover != nullptr)
   {
      utils::CreateAutopilotSupportFileWithMaxMach(mParentMover, aFilename, aMaxMach);
   }
}

void wsf::six_dof::RigidBodyMoverTestObject::LimitAlphaByMaxGLoad(double  aMach,
                                                                  double  aMaxGLoad,
                                                                  double  aDynPressure_ppsqft,
                                                                  double& aAlpha_rad)
{
   // In addition to limiting alpha, we may also need to limit the g-load.
   // If necessary, we will limit alpha to stay under the g-load limit.

   double CL = mParentMover->GetActiveAeroObject()->CL_AlphaBetaMach(aMach, aAlpha_rad, 0.0);

   double lift_lbs = 0.0;
   if (mParentMover->GetActiveAeroObject()->UsesRefArea())
   {
      lift_lbs = aDynPressure_ppsqft * mParentMover->GetRefArea_sqft() * CL;
   }
   else
   {
      lift_lbs = aDynPressure_ppsqft * mParentMover->GetWingArea_sqft() * CL;
   }
   double gLoad = lift_lbs / mParentMover->GetCurrentWeight_lbs();
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
         CL = mParentMover->GetActiveAeroObject()->CL_AlphaBetaMach(aMach, tempAlpha_rad, 0.0);

         if (mParentMover->GetActiveAeroObject()->UsesRefArea())
         {
            lift_lbs = aDynPressure_ppsqft * mParentMover->GetRefArea_sqft() * CL;
         }
         else
         {
            lift_lbs = aDynPressure_ppsqft * mParentMover->GetWingArea_sqft() * CL;
         }
         gLoad      = lift_lbs / mParentMover->GetCurrentWeight_lbs();
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

void wsf::six_dof::RigidBodyMoverTestObject::LimitAlphaByMaxGLoad(double  aMach,
                                                                  double  aMaxGLoad,
                                                                  double  aDynPressure_ppsqft,
                                                                  double  aCurrentMass_lbs,
                                                                  double& aAlpha_rad)
{
   // In addition to limiting alpha, we may also need to limit the g-load.
   // If necessary, we will limit alpha to stay under the g-load limit.

   double CL = mParentMover->GetActiveAeroObject()->CL_AlphaBetaMach(aMach, aAlpha_rad, 0.0);

   double refArea_ft2 = 0.0;
   if (mParentMover->GetActiveAeroObject()->UsesRefArea())
   {
      refArea_ft2 = mParentMover->GetRefArea_sqft();
   }
   else
   {
      refArea_ft2 = mParentMover->GetWingArea_sqft();
   }
   double lift_lbs = CL * aDynPressure_ppsqft * refArea_ft2;
   double gLoad    = lift_lbs / aCurrentMass_lbs;

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
         CL = mParentMover->GetActiveAeroObject()->CL_AlphaBetaMach(aMach, tempAlpha_rad, 0.0);

         lift_lbs   = CL * aDynPressure_ppsqft * refArea_ft2;
         gLoad      = lift_lbs / aCurrentMass_lbs;
         gLoadError = aMaxGLoad - gLoad;

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

void wsf::six_dof::RigidBodyMoverTestObject::SetVehicleMass(double aMass_lbs)
{
   mParentMover->SetCurrentWeight_lbs(aMass_lbs);
}

double wsf::six_dof::RigidBodyMoverTestObject::MaxInstantaneousGLoad(double aAltitude_ft, double aMach, double aCurrentMass_lbs)
{
   RigidBodyPilotManager* pilotMgr = mParentMover->GetRigidBodyPilotManager();
   if (pilotMgr == nullptr)
   {
      return 0.0;
   }

   const RigidBodyCommonController* controller = pilotMgr->GetSpecialRigidBodyCommonController();
   if (controller == nullptr)
   {
      return 0.0;
   }

   // Get the atmosphere
   const Environment* env = GetEnvironment();
   if (env == nullptr)
   {
      RestoreFromTestingMode();
      return 0.0;
   }

   // Calc the speed and dynamic pressure
   double altitude_m      = UtMath::cM_PER_FT * aAltitude_ft;
   double speed_fps       = env->CalcFpsFromMach(altitude_m, aMach);
   double dynPressure_psf = env->CalcDynamicPressure_psf(altitude_m, speed_fps);

   if (dynPressure_psf <= 0.0)
   {
      ut::log::error()
         << "Dynamic pressure must be greater than zero in RigidBodyMoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   // Get the reference area
   double refArea_sqft = 0.0;

   if (mParentMover->GetActiveAeroObject()->UsesRefArea())
   {
      refArea_sqft = mParentMover->GetRefArea_sqft();
   }
   else
   {
      refArea_sqft = mParentMover->GetWingArea_sqft();
   }

   if (refArea_sqft <= 0.0)
   {
      ut::log::error()
         << "Reference area must be greater than zero in RigidBodyMoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   // Get the max CL
   double maxCL = 0.0;
   bool   valid = controller->GetCLMaxMach(aMach, maxCL);
   if (!valid)
   {
      ut::log::error() << "Unable to get max CL in RigidBodyMoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   // Calc the alpha to with the required CL
   double alphaToFly_deg = 0.0;
   valid                 = controller->GetAlphaVsMachCL(aMach, maxCL, alphaToFly_deg);

   if (!valid)
   {
      ut::log::error()
         << "Unable to calculate alpha via GetAlphaVsMachCL() in RigidBodyMoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   // Convert to radians
   double alphaToFly_rad = alphaToFly_deg * UtMath::cRAD_PER_DEG;

   // Get the stick position for the given Mach and alpha
   double stickBack = 0.0;
   valid            = controller->GetStickForZeroMomentVsMachAlpha(aMach, alphaToFly_rad, 0.0, stickBack);

   if (!valid)
   {
      ut::log::error() << "Unable to calculate stick via GetStickForZeroMomentVsMachAlpha() in "
                          "RigidBodyMoverTestObject::MaxInstantaneousGLoad().";
      return 0.0;
   }

   // Calc the lift
   double lift_lbs = maxCL * dynPressure_psf * refArea_sqft;

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

double wsf::six_dof::RigidBodyMoverTestObject::AlphaAtDesiredDrag(double aAlt_ft, double aSpeed_fps, double aDrag_lbs, bool& aOk)
{
   // Default is a solution that is not valid
   aOk = false;

   // Get the atmosphere
   const Environment* env = GetEnvironment();
   if (env == nullptr)
   {
      return 0;
   }

   RigidBodyPilotManager* pilotMgr = mParentMover->GetRigidBodyPilotManager();
   if (pilotMgr == nullptr)
   {
      return 0;
   }

   const RigidBodyCommonController* controller = pilotMgr->GetSpecialRigidBodyCommonController();
   if (controller == nullptr)
   {
      return 0;
   }

   // Get the mach for the current speed (ft/sec) at the current altitude
   double mach = env->CalcMachFromFps(UtMath::cM_PER_FT * aAlt_ft, aSpeed_fps);

   // Get the maximum alpha (stall angle)
   double maxAlpha_deg = 0.0;
   if (!controller->GetAlphaMaxMach(mach, maxAlpha_deg))
   {
      return 0.0;
   }

   double maxAlpha_rad = maxAlpha_deg * UtMath::cRAD_PER_DEG;

   // Get the maximum allowable g-load
   AutopilotLimitsAndSettings limits   = controller->GetCurrentLimitsAndSettings();
   double                     maxGLoad = limits.pitchGLoad_Max;

   // Calculate the current dynamic pressure
   double dynPressure_psf = env->CalcDynamicPressure_psf(UtMath::cM_PER_FT * aAlt_ft, aSpeed_fps);

   // Calculate the "target" Cd to produce the desired drag
   // Cd = drag_lbs / (q * Area_sqft);
   double target_cd = 0.0;
   if (mParentMover->GetActiveAeroObject()->UsesRefArea())
   {
      target_cd = aDrag_lbs / (dynPressure_psf * mParentMover->GetRefArea_sqft());
   }
   else
   {
      target_cd = aDrag_lbs / (dynPressure_psf * mParentMover->GetWingArea_sqft());
   }

   // Get the Cd at the stall angle. This is essentially the max Cd that will be considered
   double cdAtStallAngle = mParentMover->GetActiveAeroObject()->Cd_AlphaBetaMach(mach, maxAlpha_rad, 0.0);

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
      LimitAlphaByMaxGLoad(mach, maxGLoad, dynPressure_psf, alpha_rad);
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

      double cd = mParentMover->GetActiveAeroObject()->Cd_AlphaBetaMach(mach, alpha_rad, 0.0);

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
         LimitAlphaByMaxGLoad(mach, maxGLoad, dynPressure_psf, alpha_rad);

         // Now, we need to verify that there is sufficient control authority to achieve the alpha
         //         double stickBack   = 0.0;
         double CoreCm        = 0.0;
         double ControlsCm    = 0.0;
         double ThrustOut_lbs = 0.0;
         //         double controlHigh = 1.0;
         //         double controlLow  = -1.0;

         // Try full stick back
         mParentMover->CalculateCmAreaForCoreAndControls(mach, alpha_rad, 1.0, true, ThrustOut_lbs, CoreCm, ControlsCm);
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

bool wsf::six_dof::RigidBodyMoverTestObject::AlphaAtDesiredDrag(double  aAlt_ft,
                                                                double  aMach,
                                                                double  aDrag_lbs,
                                                                double  aCurrentMass_lbs,
                                                                double  aMaxGLoad,
                                                                double& aAlpha_rad)
{
   // Set default alpha value
   aAlpha_rad = -0.1;

   // Get the atmosphere
   const Environment* env = GetEnvironment();
   if (env == nullptr)
   {
      return false;
   }

   RigidBodyPilotManager* pilotMgr = mParentMover->GetRigidBodyPilotManager();
   if (pilotMgr == nullptr)
   {
      return false;
   }

   const RigidBodyCommonController* controller = pilotMgr->GetSpecialRigidBodyCommonController();
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
   double altitude_m      = UtMath::cM_PER_FT * aAlt_ft;
   double speed_fps       = env->CalcFpsFromMach(altitude_m, aMach);
   double dynPressure_psf = env->CalcDynamicPressure_psf(altitude_m, speed_fps);

   // Calculate the "target" Cd to produce the desired drag
   // Cd = drag_lbs / (q * Area_sqft);
   double target_cd = 0.0;
   if (mParentMover->GetActiveAeroObject()->UsesRefArea())
   {
      target_cd = aDrag_lbs / (dynPressure_psf * mParentMover->GetRefArea_sqft());
   }
   else
   {
      target_cd = aDrag_lbs / (dynPressure_psf * mParentMover->GetWingArea_sqft());
   }

   // Get the Cd at the stall angle. This is essentially the max Cd that will be considered
   double cdAtStallAngle = mParentMover->GetActiveAeroObject()->Cd_AlphaBetaMach(aMach, maxAlpha_rad, 0.0);

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
      LimitAlphaByMaxGLoad(aMach, aMaxGLoad, dynPressure_psf, aCurrentMass_lbs, alpha_rad);

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

      double cd = mParentMover->GetActiveAeroObject()->Cd_AlphaBetaMach(aMach, alpha_rad, 0.0);

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
         LimitAlphaByMaxGLoad(aMach, aMaxGLoad, dynPressure_psf, aCurrentMass_lbs, alpha_rad);

         // Now, we need to verify that there is sufficient control authority to achieve the alpha
         double CoreCm        = 0.0;
         double ControlsCm    = 0.0;
         double ThrustOut_lbs = 0.0;

         // Try full stick back
         mParentMover->CalculateCmAreaForCoreAndControls(aMach, alpha_rad, 1.0, true, ThrustOut_lbs, CoreCm, ControlsCm);
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

bool wsf::six_dof::RigidBodyMoverTestObject::CalcLevelFlightDragAndAlpha(double  aAltitude_ft,
                                                                         double  aMach,
                                                                         double  aGrossWeight_lbs,
                                                                         double& aDrag_lbs,
                                                                         double& aAlpha_deg)
{
   RigidBodyPilotManager* pilotMgr = mParentMover->GetRigidBodyPilotManager();
   if (pilotMgr == nullptr)
   {
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   const RigidBodyCommonController* controller = pilotMgr->GetSpecialRigidBodyCommonController();

   if (controller == nullptr)
   {
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Get the atmosphere
   const Environment* env = GetEnvironment();
   if (env == nullptr)
   {
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Calc the speed and dynamic pressure
   double speed_fps       = env->CalcFpsFromMach(UtMath::cM_PER_FT * aAltitude_ft, aMach);
   double dynPressure_psf = env->CalcDynamicPressure_psf(UtMath::cM_PER_FT * aAltitude_ft, speed_fps);

   if (dynPressure_psf <= 0.0)
   {
      if (dynPressure_psf < 0.0)
      {
         ut::log::error() << "Dynamic pressure must be greater than or equal to zero in "
                             "RigidBodyMoverTestObject::CalcLevelFlightDragAndAlpha().";
      }
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Get the reference area
   double refArea_sqft = 0.0;

   if (mParentMover->GetActiveAeroObject()->UsesRefArea())
   {
      refArea_sqft = mParentMover->GetRefArea_sqft();
   }
   else
   {
      refArea_sqft = mParentMover->GetWingArea_sqft();
   }

   if (refArea_sqft <= 0.0)
   {
      ut::log::error()
         << "Reference area must be greater than zero in RigidBodyMoverTestObject::CalcLevelFlightDragAndAlpha().";
      aDrag_lbs  = 0.0;
      aAlpha_deg = 0.0;
      return false;
   }

   // Calculate the CL required to fly
   double requiredCL = aGrossWeight_lbs / (refArea_sqft * dynPressure_psf);

   double maxCL = 0.0;
   bool   valid = controller->GetCLMaxMach(aMach, maxCL);
   if (!valid)
   {
      ut::log::error() << "Unable to get valid max CL in RigidBodyMoverTestObject::CalcLevelFlightDragAndAlpha().";
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
                          "RigidBodyMoverTestObject::CalcLevelFlightDragAndAlpha().";
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
                          "RigidBodyMoverTestObject::CalcLevelFlightDragAndAlpha().";
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
   mParentMover->CalculateCdAreaForCoreAndControls(aMach, alphaToFly_rad, stickBack, coreCdArea, controlsCdArea);
   double totalCdArea = coreCdArea + controlsCdArea;

   aDrag_lbs  = dynPressure_psf * totalCdArea;
   aAlpha_deg = alphaToFly_deg;

   return true;
}
