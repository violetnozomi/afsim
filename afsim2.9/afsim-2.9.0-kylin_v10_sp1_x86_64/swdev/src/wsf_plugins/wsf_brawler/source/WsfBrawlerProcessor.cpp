// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfBrawlerProcessor.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <sstream>

#include "BrawlerCoordinateConversion.hpp"
#include "BrawlerEvaluation.hpp"
#include "BrawlerUtil.hpp"
#include "WsfBrawlerConsciousnessEvent.hpp"
#include "WsfBrawlerMover.hpp"
#include "WsfDraw.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMover.hpp"
#include "WsfPerceptionProcessor.hpp"
#include "WsfPlatform.hpp"
#include "WsfQuantumTaskerProcessor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfThreatProcessor.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeapon.hpp"


// =================================================================================================
WsfBrawlerProcessor::WsfBrawlerProcessor(WsfScenario& aScenario)
   : WsfQuantumTaskerProcessor(aScenario, "WsfBrawlerProcessor", "PROCESSOR")
   , BrawlerPlatform()
   , mPerceptionProcessorPtr(nullptr)
   , mThreatProcessorPtr(nullptr)
   , mFirstCE(true)
   , mNextConsciousnessEventTime(0)
   , mLastConsciousnessEventTime(0)
   , mUserConsciousnessEventDT(0)
   , mCLMaxVsMachTablePtr(nullptr)
   , mAlphaAtCLMaxVsMachTablePtr(nullptr)
   , mStructuralGLimitAboveDesignWeight_g(7.0)
   , mDebug(false)
   , mDebugDrawAlternatives(false)
   , mDebugDrawNominalStates(false)
   , mDrawPtr(nullptr)
   , mAlternativeMinScore(999999.9)
   , mAlternativeMaxScore(-999999.9)
{
}

WsfBrawlerProcessor::WsfBrawlerProcessor(const WsfBrawlerProcessor& aSrc)
   : WsfQuantumTaskerProcessor(aSrc)
   , BrawlerPlatform(aSrc)
   , mPerceptionProcessorPtr(nullptr)
   , mThreatProcessorPtr(nullptr)
   , mFirstCE(true)
   , mNextConsciousnessEventTime(0)
   , mLastConsciousnessEventTime(0)
   , mUserConsciousnessEventDT(aSrc.mUserConsciousnessEventDT)
   , mCLMaxVsMachTablePtr(nullptr)
   , mAlphaAtCLMaxVsMachTablePtr(nullptr)
   , mStructuralGLimitAboveDesignWeight_g(aSrc.mStructuralGLimitAboveDesignWeight_g)
   , mDebug(aSrc.mDebug)
   , mDebugDrawAlternatives(aSrc.mDebugDrawAlternatives)
   , mDebugDrawNominalStates(aSrc.mDebugDrawNominalStates)
   , mDrawPtr(nullptr)
   , mAlternativeMinScore(aSrc.mAlternativeMinScore)
   , mAlternativeMaxScore(aSrc.mAlternativeMaxScore)
   , mTargetEntity()
   , mThreatEntity()
{
   // Copy the CLmax versus Mach table
   if (aSrc.mCLMaxVsMachTablePtr != nullptr)
   {
      mCLMaxVsMachTablePtr = aSrc.mCLMaxVsMachTablePtr->Clone();
   }

   // Copy the Alpha at CLmax versus Mach table
   if (aSrc.mAlphaAtCLMaxVsMachTablePtr != nullptr)
   {
      mAlphaAtCLMaxVsMachTablePtr = aSrc.mAlphaAtCLMaxVsMachTablePtr->Clone();
   }
}

WsfBrawlerProcessor::~WsfBrawlerProcessor()
{
   delete mDrawPtr;
}

const int* WsfBrawlerProcessor::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_BRAWLER_PROCESSOR,
                               cWSF_COMPONENT_TASK_MANAGER,
                               cWSF_COMPONENT_PROCESSOR,
                               cWSF_COMPONENT_PLATFORM_PART,
                               cWSF_COMPONENT_NULL};
   return roles;
}

void* WsfBrawlerProcessor::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_BRAWLER_PROCESSOR)
   {
      return this;
   }
   return WsfQuantumTaskerProcessor::QueryInterface(aRole);
}

bool WsfBrawlerProcessor::Initialize(double aSimTime)
{
   bool ok = true;

   WsfQuantumTaskerProcessor::Initialize(aSimTime);

   assert(GetPlatform() != nullptr);

   // Find first instance of WsfPerceptionProcessor & WsfThreatProcessor on the platform.
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      if (mPerceptionProcessorPtr == nullptr)
      {
         WsfPerceptionProcessor* procPtr(dynamic_cast<WsfPerceptionProcessor*>(*iter));
         if (procPtr != nullptr)
         {
            mPerceptionProcessorPtr = procPtr;
            continue;
         }
      }
      if (mThreatProcessorPtr == nullptr)
      {
         WsfThreatProcessor* procPtr(dynamic_cast<WsfThreatProcessor*>(*iter));
         if (procPtr != nullptr)
         {
            mThreatProcessorPtr = procPtr;
            continue;
         }
      }
   }

   // Do parent initialization first
   // ok &= GetPlatform()->IsInitialized();

   Init(*GetPlatform(), GetSimulation()->GetRandom());

   // Verify the WsfBrawlerProcessor has all the necessary parts:
   // WSF_PERCEPTION_PROCESSOR
   // WSF_THREAT_PROCESSOR
   // WSF_BRAWLER_MOVER

   if (GetPerceptionProcessor() == nullptr)
   {
      auto logError = ut::log::error() << "WSF_BRAWLER_PLATFORM must have a WSF_PERCEPTION_PROCESSOR defined!";
      logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
      ok &= false;
   }

   if (GetThreatProcessor() == nullptr)
   {
      auto logError = ut::log::error() << "WSF_BRAWLER_PLATFORM must have a WSF_THREAT_PROCESSOR defined!";
      logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
      ok &= false;
   }

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if ((moverPtr == nullptr) || (moverPtr->GetType() != "WSF_BRAWLER_MOVER"))
   {
      auto logError = ut::log::error() << "WSF_BRAWLER_PLATFORM must have a WSF_BRAWLER_MOVER!";
      logError.AddNote() << "WSF_BRAWLER_PLATFROM: " << GetPlatform()->GetName();
      ok &= false;
   }

   if (ok)
   {
      // WsfPlatform initialization succeeded and we have the necessary
      // mover / processors. Do remaining initial set up here

      // For a brawler platform updates are controlled by the consciousness event time
      // Set update interval to zero for all processors to prevent update event
      // scheduling just in case there is an "update_interval" in the input files
      SetUpdateInterval(0.0);
      GetPerceptionProcessor()->SetUpdateInterval(0.0);
      GetPerceptionProcessor()->SetAssetUpdateInterval(0.0);
      GetPerceptionProcessor()->SetThreatUpdateInterval(0.0);
      GetThreatProcessor()->SetUpdateInterval(0.0);

      // TODO check other input to processors to throw errors if
      //      any values are set that should not be?

      // Set initial self called consciousness event time
      AddSelfCalledConsciousnessEvent(aSimTime + GetConsciousnessEventTime());
   }

   if (mDebugDrawAlternatives || mDebugDrawNominalStates)
   {
      mDrawPtr = new WsfDraw(*GetPlatform()->GetSimulation());
   }
#ifdef DEBUG_BRAWLER_TESTING
   else
   {
      mDrawPtr = new WsfDraw(*GetSimulation());
   }
#endif

   return ok;
}

bool WsfBrawlerProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "debug")
   {
      mDebug = true;
   }
   else if (command == "draw_alternatives")
   {
      mDebugDrawAlternatives = true;
   }
   else if (command == "draw_nominal_states")
   {
      mDebugDrawNominalStates = true;
   }
   else if (command == "consciousness_event_update_time")
   {
      double prcedt;
      aInput.ReadValueOfType(prcedt, UtInput::cTIME);
      // The range of values is the min/max used in gcetim.f (BRAWLER v7.5)
      aInput.ValueInClosedRange(prcedt, 0.25, 10.0);
      mUserConsciousnessEventDT = prcedt;
   }
   else if (command == "time_allowed_per_sector_search")
   {
      double val;
      aInput.ReadValueOfType(val, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(val, 0.0);
      SetTimeAllowedPerSectorSearch(val);
   }
   else if (command == "mind_file")
   {
      std::string filename;
      aInput.ReadValue(filename);
      filename = aInput.LocateFile(filename);
      if (!GetMIND()->LoadMindFile(filename))
      {
         std::string error_msg = "Could not load MIND file: " + filename;
         throw UtInput::BadValue(aInput, error_msg);
      }
   }
   else if (command == "clmax_vs_mach_table")
   {
      if (mCLMaxVsMachTablePtr != nullptr)
      {
         delete mCLMaxVsMachTablePtr;
      }
      mCLMaxVsMachTablePtr = new UtTable::Curve;
      mCLMaxVsMachTablePtr->ProcessInput(aInput,
                                         UtInput::cNON_DIMENSIONAL,
                                         "mach",
                                         UtTable::NoCheck(),
                                         UtInput::cNON_DIMENSIONAL,
                                         "clmax",
                                         UtTable::NoCheck());
   }
   else if (command == "alpha_at_clmax_vs_mach_table")
   {
      if (mAlphaAtCLMaxVsMachTablePtr != nullptr)
      {
         delete mAlphaAtCLMaxVsMachTablePtr;
      }
      mAlphaAtCLMaxVsMachTablePtr = new UtTable::Curve;
      mAlphaAtCLMaxVsMachTablePtr->ProcessInput(aInput,
                                                UtInput::cNON_DIMENSIONAL,
                                                "mach",
                                                UtTable::NoCheck(),
                                                UtInput::cNON_DIMENSIONAL,
                                                "alpha",
                                                UtTable::NoCheck());
   }
   else if (command == "structural_g_limit_above_design_wgt")
   {
      aInput.ReadValue(mStructuralGLimitAboveDesignWeight_g);
      aInput.ValueGreaterOrEqual(mStructuralGLimitAboveDesignWeight_g, 0.0);
   }
   else
   {
      // parent processor command
      myCommand = WsfQuantumTaskerProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

WsfProcessor* WsfBrawlerProcessor::Clone() const
{
   return new WsfBrawlerProcessor(*this);
}

WsfBrawlerProcessor* WsfBrawlerProcessor::Find(const WsfPlatform& aPlatform)
{
   WsfBrawlerProcessor* procPtr = nullptr;
   aPlatform.FindByRole(procPtr);
   return procPtr;
}

double WsfBrawlerProcessor::GetTime()
{
   return GetPlatform()->GetSimTime();
}

double WsfBrawlerProcessor::MindUpdate(double aSimTime)
{
   // following the design of BrawlerPlatform::MindUpdate()...

   // update our knowledge of all threats & assets:
   //  These metods take the place of BRAWLER mindin(), sensor(), inferl()
   //  and mindup(), called in conevt.f
   GetPerceptionProcessor()->Update(aSimTime);
   GetPerceptionProcessor()->PerceivedAssets(aSimTime);  // Called to update firendly perception
   GetPerceptionProcessor()->PerceivedThreats(aSimTime); // Called to update hostile perception
   GetThreatProcessor()->Update(aSimTime);               // Incoming hostile weapons

   // get maneuver value component statistics ready for this update
   InitializeStatistics();
   // update other mental model knowledge:
   UpdateMissileMode();
   // do simple projections
   NominalProjection(ProjectedTimeDelta());

   if (mDebugDrawNominalStates)
   {
      double locWCS[3] = {0, 0, 0};
      mDrawPtr->SetId(mDrawPtr->GetNewId());
      mDrawPtr->SetPointSize(3);
      mDrawPtr->SetDuration(30);
      mDrawPtr->BeginPoints();
      // my nominal state
      mDrawPtr->SetColor(0.0, 0.0, 1.0); // blue
      mMyNom.GetLocationWCS(locWCS);
      mDrawPtr->VertexWCS(locWCS); // Need two point because of VESPA bug?
      mDrawPtr->VertexWCS(locWCS); // Need two point because of VESPA bug?
      // targets nominal state
      mDrawPtr->SetColor(1.0, 0.0, 1.0); // lighter purple
      mTgNom.GetLocationWCS(locWCS);
      mDrawPtr->VertexWCS(locWCS); // Need two point because of VESPA bug?
      mDrawPtr->VertexWCS(locWCS); // Need two point because of VESPA bug?
      mDrawPtr->End();
   }


   // consider all alternatives (call appropriate evaluate...() method for each one):
   // This takes the place of BRAWLER modsel() called in conevt.f
   // alternatives should be on the behavior tree of the brawler processor (which is a quantum tasker derivative)
   Update(aSimTime);

   CalculateStatistics(); // after all alternatives evaluated

   // Based on the current event execution time and the interval to
   // the next event, schedule the new self called event time
   double dtime         = GetConsciousnessEventTime();
   double nextEventTime = aSimTime + dtime;
   SetNextConsciousnessEventTime(nextEventTime);
   return nextEventTime;
}

double WsfBrawlerProcessor::GetConsciousnessEventTime()
{
   // This method calls out what lines of gcetim.f are replicated
   // and which pieces are left out.

   // Time to new event, returned by gcetim()
   double dtime = 0.0;

   // Used by gcetim.f as array size in dtmods. See line 104/105.
   const int nummod = 10;

   // Used as a large value to initialize time and comapre against
   // Declared here to avoid "magic numbers"
   const double CE_TIME_MAX = 1000.0;

   // Used by gcetim to store possible mods to dtime
   // initialized to 1000. See line 106 of gcetim.f for decleration
   // and 117 for initialization.
   std::vector<double> dtmods(nummod, CE_TIME_MAX);

   // Lines 120 - 128 of gcetim.f check if this is SAM site
   // and schedule the next CE accordingly (smcetm()).
   // WsfBrawlerProcessor is not currently modeling this.

   // Lines 129 - 155 of gcetim.f
   // Set dtmods[1] based on range to nearest threat
   if (mUserConsciousnessEventDT == 0)
   {
      // No user supplied an update time (prcedt == 0)
      // Find the nearest threat
      /* BRAWLER uses:
       * closest "perceived" threat
       * closest "perceived" missile
       * closest truth threat from full list of platforms
       * closest truth missile targeting ownship from list of all missiles
       * large range of 10.e+12
       *
       * These five values are compared to find the smallest range.
       * For this simulation looping over the list of all missiles/threats
       * is not practical as the numbers could be in the thousands. We also
       * have alternate ways of updating position when things get close together
       * and don't need to schedule more frequent consciousness updates to update
       * our position. We will just use perceived threats.
       */
      std::vector<double> ranges(3, 1.0e+12);
      WsfTrack* nearestThreatPtr = GetPerceptionProcessor()->NearestThreat(GetPlatform()->GetSimulation()->GetSimTime());
      if (nearestThreatPtr != nullptr)
      {
         double threatRelativeWCS[3] = {0, 0, 0};
         double threatWCS[3]         = {0, 0, 0};
         nearestThreatPtr->GetLocationWCS(threatWCS);
         GetPlatform()->GetRelativeLocationWCS(threatWCS, threatRelativeWCS);
         ranges[0] = UtVec3d::Magnitude(threatRelativeWCS);
      }
      WsfTrack* nearestMissilePtr = GetThreatProcessor()->GetNearestThreat();
      if (nearestMissilePtr != nullptr && nearestMissilePtr != nearestThreatPtr)
      {
         double missileRelativeWCS[3] = {0, 0, 0};
         double missileWCS[3]         = {0, 0, 0};
         nearestThreatPtr->GetLocationWCS(missileWCS);
         GetPlatform()->GetRelativeLocationWCS(missileWCS, missileRelativeWCS);
         ranges[1] = UtVec3d::Magnitude(missileRelativeWCS);
      }
      // Find minimum range, meters
      double rangeMin = *min_element(ranges.begin(), ranges.end());

      // CE update time is
      // max consideration time for maneuver (decision level 3) plus (Time Per Sector Search minus
      // max consideration time for maneuver (decision level 3)) times a ramp function of the range min
      // between 75,000 and 150,000 ft.
      double maneuverReconsideration = GetMIND()->ManeuverReconsiderationMax();
      dtmods[1] =
         maneuverReconsideration + (TimeAllowedPerSectorSearch() - maneuverReconsideration) *
                                      BRAWLER::ramp(75.0e+3 * UtMath::cM_PER_FT, rangeMin, 150.0e+3 * UtMath::cM_PER_FT);
   }
   else // Lines 156 - 165 of gcetim.f
   {
      // The user did set an update time (prcedt > 0). BRAWLER code does checks
      // that we take care of on input to make sure prcedt is 10.0 or less
      dtmods[2] = mUserConsciousnessEventDT;
   }

   // Lines 169 - 171 of gcetim.f
   // Uses rules interval (prcivl) to set dtmods[3]
   // Not currently implemented for WsfBrawlerProcessor

   // Lines 173 - 197 of gcetim.f
   // Adjust for firing decisions, stored in dtmods[4]
   /*
   if Missile mode == 2 and selected weapon is DEW
   // DEW selected and in mslmd 2.
   // Use short time interval to allow reaction to acquisition
   dtmods(4) = 0.5
   else if (missile mode == 1 || missle mode == 2) and weapon selected is guns
   // Selected guns and in mslmd 1 or 2.  Go for a short time interval to
   // permit accurate aiming.
   dtmods(4) = 0.5
   */
   // These first two conditions are for directed energy weapons and guns
   // Don't currently have a good way to specify weapon type

   // else if missile mode == 1
   //  Following code makes the con event interval sensitive to
   //  the amount of time required to point at the target and to
   //  close to within max range.  The larger of the two is used
   //  since both constraints must be satisfied in order to fire.
   if (MissileMode() == BRAWLER::INTERCEPT)
   {
      std::list<WsfTask*>           weaponTasks = TaskListReceivedOfType("WEAPON");
      std::list<WsfTask*>::iterator tli         = weaponTasks.begin();
      for (; tli != weaponTasks.end(); ++tli)
      {
         WsfTask& task = *(*tli);
         // Get track ID
         const WsfTrackId& localTrackId = task.GetLocalTrackId();
         // Get location of the target track
         // Also could use:
         // const WsfTrack* currentTgt = GetPlatform()->GetTrackManager().GetCurrentTarget(); ??
         WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().FindTrack(localTrackId);
         if (localTrackPtr != nullptr)
         {
            // dtimex = the larger of 1.0 and (steering angle to fire/(15 * Radians per degree))
            double steeringAngleToFire = 0.0; // TODO how do we get this value?
            dtmods[4]                  = std::max(1.0, (steeringAngleToFire / (15 * UtMath::cRAD_PER_DEG)));
            // If range between me and my best target > RMAX2 and Range rate < 0 (closing)
            // if (rangeToTarget > mRMAX2 && closing)
            //{
            // }
            //  Target is outside missile envelope, but attacker is closing.  Add range limit:
            // dtimex = the larger of current dtimex and -(range between me and my best target - RMAX2) / range rate
            // w/best target dtmods(4) = dtimex
         }
      }
   }
   // else if missile mode == 2 (I have weapons pending or active)
   //   dtmods(4) = agemod(2,7)  <- max consideration time for decision level 7
   else if (MissileMode() == BRAWLER::AIMFIRE)
   {
      dtmods[4] = GetMIND()->WeaponReconsiderationMax();
   }


   // Lines 200 - 202 of gcetim.f
   // Adjust for large delta-v (500 ft/sec = 30 deg direction change at 1000 ft/sec speed)
   // TODO, need more info from Mover and BrawlerAircraft
   // if (difference between my current inertal velocity vector in ft/sec and final velocity vector as a result of
   // projected maneuver is greater than 500 ft/sec)
   //{
   // dtmods[5] = 5.0;
   //}

   // Line 204 of gcetim.f
   // Adjust for route, prevent too long a wait after crossing leg end
   // Calls gtmcap() and stores value in dtmods[6]
   // Not currently implemented for WsfBrawlerProcessor

   // Line 205 of gcetim.f (calls vmin())
   // Get the smallest next event time from dtmods, set in dtime
   dtime = *min_element(dtmods.begin(), dtmods.end());

   // Lines 211 - 215 of gcetim.f
   // This code to decouple conevt times from start time
   if (mFirstCE)
   {
      mFirstCE = false;
      dtime    = dtime * GetPlatform()->GetSimulation()->GetRandom().Uniform(0.0, 1.0);
   }
   else // Lines 216 - 221 of gcetim.f
   {
      // Prevent CE interval from being smaller than 0.25
      dtime = std::max(0.25, dtime);
   }

   // Lines 225 - 231 of gcetim.f
   // Modifies dtime when in validation mode and have an interactive pilot
   // Not implemented for WsfBrawlerProcessor

   // Lines 235 - 247 of gcetim.f
   // Modify dtime if there are pendinf IFF requests (calls giffs())
   // Not implemented for WsfBrawlerProcessor as IFF handling is completely
   // different from BRAWLER implementation.

   // return dtime as the amount of time to the next event
   return dtime;
}


void WsfBrawlerProcessor::AddSelfCalledConsciousnessEvent(double aEventTime)
{
   GetPlatform()->GetSimulation()->AddEvent(
      ut::make_unique<WsfBrawlerConsicousnessEvent>(aEventTime,
                                                    GetPlatform()->GetIndex(),
                                                    WsfBrawlerConsicousnessEvent::SELF_CALLED));
   SetNextConsciousnessEventTime(aEventTime);
}

double WsfBrawlerProcessor::EvaluateRates(UtVec3d& aAngularRates, UtVec3d& aLongitudinalAccel)
{
   // double score = 0.0;
   UtEntity  projection;
   double    dt       = ProjectedTimeDelta();
   WsfMover* moverPtr = GetPlatform()->GetMover();
   // double simtime = GetPlatform()->GetSimTime();
   // double projectedTime = simtime + dt;
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         brawlerMoverPtr->FlyRates(aAngularRates, aLongitudinalAccel); // set command
         brawlerMoverPtr->ProjectForward(dt, projection);
      }
      else
      {
         auto logError = ut::log::error()
                         << "Incompatible WSF_BRAWLER_PLATFORM.MOVER must be of a WSF_BRAWLER_MOVER type!";
         logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
         logError.AddNote() << "MOVER: " << moverPtr->GetType();
         logError.Send();
         assert(false);
      }
   }
   else
   {
      auto logError = ut::log::error() << "WSF_BRAWLER_PLATFORM must have a WSF_BRAWLER_MOVER defined!";
      logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
      logError.Send();
      assert(false);
   }
   return EvaluateAlternative(projection);
}

double WsfBrawlerProcessor::EvaluateHeadingSpeedAltitude(const double& aHeading,
                                                         const double& aSpeed,
                                                         const double& aAltitude,
                                                         const double& aMaxGees,
                                                         const double& aMaxClimb)
{
   // double score = 0.0;
   UtEntity  projection;
   double    dt       = ProjectedTimeDelta();
   WsfMover* moverPtr = GetPlatform()->GetMover();
   // double simtime = GetPlatform()->GetSimTime();
   // double projectedTime = simtime + dt;
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         brawlerMoverPtr->FlyHeadingSpeedAltitude(aHeading, aSpeed, aAltitude, aMaxGees, aMaxClimb); // set command
         brawlerMoverPtr->ProjectForward(dt, projection);
      }
      else
      {
         auto logError = ut::log::error()
                         << "Incompatible WSF_BRAWLER_PLATFORM.MOVER must be of a WSF_BRAWLER_MOVER type.";
         logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
         logError.AddNote() << "MOVER: " << moverPtr->GetType();
         logError.Send();
         assert(false);
      }
   }
   else
   {
      auto logError = ut::log::error() << "WSF_BRAWLER_PLATFORM must have a WSF_BRAWLER_MOVER defined!";
      logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
      logError.Send();
      assert(false);
   }
   return EvaluateAlternative(projection);
}

double WsfBrawlerProcessor::EvaluateTurnInPlaneWithSpeed(UtVec3d& aPlane, const double& aGees, const double& aSpeed)
{
   // double score = 0.0;
   UtEntity  projection;
   double    dt       = ProjectedTimeDelta();
   WsfMover* moverPtr = GetPlatform()->GetMover();
   // double simtime = GetPlatform()->GetSimTime();
   // double projectedTime = simtime + dt;
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         brawlerMoverPtr->TurnInPlaneWithSpeed(aPlane, aGees, aSpeed); // set command
         brawlerMoverPtr->ProjectForward(dt, projection);
      }
      else
      {
         auto logError = ut::log::error()
                         << "Incompatible WSF_BRAWLER_PLATFORM.MOVER must be of a WSF_BRAWLER_MOVER type!";
         logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
         logError.AddNote() << "MOVER: " << moverPtr->GetType();
         logError.Send();
         assert(false);
      }
   }
   else
   {
      auto logError = ut::log::error() << "WSF_BRAWLER_PLATFORM must have a WSF_BRAWLER_MOVER defined!";
      logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
      logError.Send();
      assert(false);
   }
   return EvaluateAlternative(projection);
}

double WsfBrawlerProcessor::EvaluateTurnInPlaneWithThrottle(UtVec3d&      aPlane,
                                                            const double& aGees,
                                                            const double& aThrottle) //[0-3]
{
   // double score = 0.0;
   UtEntity  projection;
   double    dt       = ProjectedTimeDelta();
   WsfMover* moverPtr = GetPlatform()->GetMover();
   // double simtime = GetPlatform()->GetSimTime();
   // double projectedTime = simtime + dt;
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         brawlerMoverPtr->TurnInPlaneWithThrottle(aPlane, aGees, aThrottle); // set command
         brawlerMoverPtr->ProjectForward(dt, projection);
      }
      else
      {
         auto logError = ut::log::error()
                         << "Incompatible WSF_BRAWLER_PLATFORM.MOVER must be of a WSF_BRAWLER_MOVER type!";
         logError.AddNote() << "WSF_BRAWLER_PLATFORM:" << GetPlatform()->GetName();
         logError.AddNote() << "MOVER: " << moverPtr->GetType();
         logError.Send();
         assert(false);
      }
   }
   else
   {
      auto logError = ut::log::error() << "WSF_BRAWLER_PLATFORM "
                                       << "must have a WSF_BRAWLER_MOVER defined.";
      logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
      logError.Send();
      assert(false);
   }
   return EvaluateAlternative(projection);
}

double WsfBrawlerProcessor::EvaluateVectorWithSpeed(UtVec3d& aVelocityVector, const double& aMaxGees, const double& aSpeed)
{
   // double score = 0.0;
   UtEntity  projection;
   double    dt       = ProjectedTimeDelta();
   WsfMover* moverPtr = GetPlatform()->GetMover();
   // double simtime = GetPlatform()->GetSimTime();
   // double projectedTime = simtime + dt;
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         brawlerMoverPtr->FlyVectorWithSpeed(aVelocityVector, aMaxGees, aSpeed); // set command
         brawlerMoverPtr->ProjectForward(dt, projection);
      }
      else
      {
         auto logError = ut::log::error() << "Incompatible WSF_BRAWLER_PLATFORM.MOVER "
                                          << "must be of a WSF_BRAWLER_MOVER type!";
         logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
         logError.AddNote() << "MOVER: " << moverPtr->GetType();
         logError.Send();
         assert(false);
      }
   }
   else
   {
      auto logError = ut::log::error() << "WSF_BRAWLER_PLATFORM "
                                       << "must have a WSF_BRAWLER_MOVER defined.";
      logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
      logError.Send();
      assert(false);
   }
   return EvaluateAlternative(projection);
}

double WsfBrawlerProcessor::EvaluateVectorWithThrottle(UtVec3d& aVelocityVector, const double& aMaxGees, const double& aThrottle)
{
   // double score = 0.0;
   UtEntity  projection;
   double    dt       = ProjectedTimeDelta();
   WsfMover* moverPtr = GetPlatform()->GetMover();
   // double simtime = GetPlatform()->GetSimTime();
   // double projectedTime = simtime + dt;
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         brawlerMoverPtr->FlyVectorWithThrottle(aVelocityVector, aMaxGees, aThrottle); // set command
         brawlerMoverPtr->ProjectForward(dt, projection);
      }
      else
      {
         auto logError = ut::log::error() << "Incompatible WSF_BRAWLER_PLATFORM.MOVER "
                                          << "must be of a WSF_BRAWLER_MOVER type!";
         logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
         logError.AddNote() << "MOVER: " << moverPtr->GetType();
         logError.Send();
         assert(false);
      }
   }
   else
   {
      auto logError = ut::log::error() << "WSF_BRAWLER_PLATFORM "
                                       << "must have a WSF_BRAWLER_MOVER defined.";
      logError.AddNote() << "WSF_BRAWLER_PLATFORM: " << GetPlatform()->GetName();
      logError.Send();
      assert(false);
   }
   return EvaluateAlternative(projection);
}

double WsfBrawlerProcessor::EvaluateAlternative(UtEntity& projectedState)
{
   BrawlerPlatform::SetProjectedState(projectedState);
   double score = BrawlerEvaluation::EvaluateAlternative(*this, GetTarget());
   if (score < mAlternativeMinScore)
   {
      mAlternativeMinScore = score;
   }
   if (score > mAlternativeMaxScore)
   {
      mAlternativeMaxScore = score;
   }
   if (mDebugDrawAlternatives)
   {
      double locLLA[3] = {0, 0, 0};
      projectedState.GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
      UtVec3d color = GetEvaluationColor(score);
      mDrawPtr->SetId(mDrawPtr->GetNewId());
      mDrawPtr->SetPointSize(3);
      mDrawPtr->SetDuration(10);
      mDrawPtr->SetColor(color[0], color[1], color[2]);
      mDrawPtr->BeginPoints();
      mDrawPtr->VertexLLA(locLLA[0], locLLA[1], locLLA[2]);
      mDrawPtr->End();
      // Build Alternative label and place it near the alternative point
      int ilevel = 0;
      int kalt   = 0;
      int icall  = 0;
      int lcall  = 0;
      GetAlternative(ilevel, kalt, icall, lcall);
      std::ostringstream oss;
      oss << ilevel << kalt << icall << lcall;
      std::string altID = oss.str();
      mDrawPtr->SetTextSize(5);
      mDrawPtr->BeginText(altID);
      mDrawPtr->VertexLLA(locLLA[0], locLLA[1], locLLA[2] + 10);
      mDrawPtr->End();
   }

   if (mDebug)
   {
      int ilevel = 0;
      int kalt   = 0;
      int icall  = 0;
      int lcall  = 0;
      GetAlternative(ilevel, kalt, icall, lcall);
      auto logDebug = ut::log::debug() << "WsfBrawlerProcessor::EvaluateAlternative";
      logDebug.AddNote() << "T = " << GetPlatform()->GetSimTime();
      logDebug.AddNote() << "Scored: " << score;
      auto logNote = logDebug.AddNote() << "Alt: ";
      logNote.AddNote() << "ilevel: " << ilevel;
      logNote.AddNote() << "kalt: " << kalt;
      logNote.AddNote() << "icall: " << icall;
      logNote.AddNote() << "lcall: " << lcall;
   }
   return score;
}

// virtual
double WsfBrawlerProcessor::SensorGimbalElevationLimit()
{
   // use sensor slew limit (slew = FOV + cue)
   if (GetPlatform()->GetComponentCount<WsfSensor>() <= 0)
   {
      return 0.0;
   }
   return GetPlatform()->GetComponentEntry<WsfSensor>(0)->GetMaxElSlew();

   ////TODO - make it more complicated later (find full sensor coverage)
   // double elmax = GetSensorEntry(0)->GetMaxElSlew();
   // double elmin = GetSensorEntry(0)->GetMinElSlew();
   // for (unsigned int i=1; i < GetSensorCount(); ++i)
   //{
   //    WsfSensor* sensorPtr = GetSensorEntry(i);
   //    double up = sensorPtr->GetMaxElSlew();
   //    double dn = sensorPtr->GetMinElSlew();
   //    //sensor must overlap existing sensor range
   //    if (up > elmax && dn < elmax)
   //    {
   //       elmax = up;
   //       //TODO - restart the loop to pick up sensors that might NOW overlap
   //    }
   //    if (dn < elmin && up > elmin)
   //    {
   //       elmin = dn;
   //       //TODO - restart the loop to pick up sensors that might NOW overlap
   //    }
   // }
   // return elmax;
}

// virtual
double WsfBrawlerProcessor::SensorGimbalAzimuthLimit()
{
   // use sensor slew limit (slew = FOV + cue)
   if (GetPlatform()->GetComponentCount<WsfSensor>() <= 0)
   {
      return 0.0;
   }
   return GetPlatform()->GetComponentEntry<WsfSensor>(0)->GetMaxAzSlew();

   // TODO - make it more complicated later (find full sensor coverage)
   //        see: SensorGimbalElevationLimit()
}

UtVec3d WsfBrawlerProcessor::GetEvaluationColor(double aScore)
{
   double ratio = (aScore - mAlternativeMinScore) / (mAlternativeMaxScore - mAlternativeMinScore);
   double red   = (ratio < 0.5) ? (1.0) : ((1.0 - ratio) * 2.0);
   double grn   = (ratio > 0.5) ? (1.0) : (ratio * 2.0);
   double blu   = 0.0;
   return UtVec3d(red, grn, blu);
}

void WsfBrawlerProcessor::PopulateEntityFromTrack(UtEntity& aEntity, WsfTrack* aTrackPtr)
{
   double temp[3] = {0, 0, 0};
   aTrackPtr->GetLocationWCS(temp);
   aEntity.SetLocationWCS(temp);
   aTrackPtr->GetVelocityWCS(temp);
   aEntity.SetVelocityWCS(temp);
}

WsfLocalTrack* WsfBrawlerProcessor::GetTargetTrack()
{
   std::list<WsfTask*> weaponTasks    = TaskListReceivedOfType("WEAPON");
   WsfLocalTrack*      targetTrackPtr = nullptr;
   if (!weaponTasks.empty())
   {
      // TODO - check for a particular target later, just use first one in list for now
      WsfTask weaponTask = *(weaponTasks.front());
      targetTrackPtr     = GetPerceptionProcessor()->PerceivedThreat(weaponTask.GetTargetName());
   }
   return targetTrackPtr;
}

// virtual
UtEntity* WsfBrawlerProcessor::GetTarget()
{
#ifdef DEBUG_BRAWLER_TESTING
   return &mTargetEntity;
#endif
   WsfLocalTrack* targetTrackPtr = GetTargetTrack();
   if (targetTrackPtr != nullptr)
   {
      PopulateEntityFromTrack(mTargetEntity, targetTrackPtr);
      return &mTargetEntity;
   }
   return nullptr;
}

// virtual
double WsfBrawlerProcessor::FirstObservedTime(UtEntity* aTgtPtr)
{
   // if (aTgt == 0)
   //{
   //    return -1.0;
   // }
   // if (aTgt == this)
   //{
   //    return 0.0;
   // }
   // WsfLocalTrack* targetTrack = GetPerceptionProcessor()->PerceivedThreat(aTgt->GetName());
   // if (targetTrack == 0)
   //{
   //    return -1.0;
   // }
   // return targetTrack->GetStartTime();
   return -1.0;
}

// virtual
UtEntity* WsfBrawlerProcessor::GetThreat()
{
   // Considering nearest threat as highest threat
   WsfTrack* threatTrackPtr = GetThreatProcessor()->GetNearestThreat();
   if (threatTrackPtr != nullptr)
   {
      PopulateEntityFromTrack(mThreatEntity, threatTrackPtr);
      return &mThreatEntity;
   }
   return nullptr;
}

// virtual
int WsfBrawlerProcessor::GetNumberThreats()
{
   return (int)GetThreatProcessor()->GetThreatList()->size();
}

// protected
// void WsfBrawlerProcessor::DoUpdate(double aSimTime)
//{
//   WsfPlatform::DoUpdate(aSimTime);
//
//#ifdef DEBUG_BRAWLER_TESTING
//
//   //double dt = ProjectedTimeDelta();
//   double dt = 2.0;  //for now
//   //double sphereRadiusNm = 0.4;  //less than 3000 ft???
//   double sphereRadiusNm = 3.0;  //less than 3000 ft???
//   static bool TestEvaluation = true;
//   //test brawler evaluation
//   if (TestEvaluation)
//   {
//      WsfMover* moverPtr = GetMover();
//      if (moverPtr == 0 || moverPtr->GetType() != "WSF_BRAWLER_MOVER")
//      {
//         ut::log::error << "debugging evaluation without a brawler mover!";
//         return;
//      }
//      WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
//
//      //iterate over 3d space and create hypothetical brawler platforms to evaluate against a hypothetical target
//      double gees = 6.0;
//      double speed = 250.0;      //m/s (~500 knots)
//      UtVec3d vec(speed, 0, 0);  //always north
//      UtVec3d vecFt(speed * UtMath::cFT_PER_M, 0, 0);
//
//      UtEntity::SetVelocityNED(vec.GetData());
//      mMyState.SetVelocityNED(vec.GetData());
//      brawlerMoverPtr->vel = vecFt;
//
//      //UtEntity test_target;
//      mTargetEntity.SetLocationLLA(0, 0, 10000 * UtMath::cM_PER_FT);
//      //double velNED[3] = {-250, 0, 0};  //south
//      double velNED[3] = { 250, 0, 0 };  //north
//      mTargetEntity.SetVelocityNED(velNED);
//
//      UtVec3d ned0 = BrawlerCoordinateConversion::ConvertLLAtoNED(UtVec3d(0, 0, 10000 * UtMath::cM_PER_FT));
//
//      double distance = 1852 * sphereRadiusNm * UtMath::cFT_PER_M;   //3nm (in feet)
//      vector<double> scores;
//      vector<UtVec3d> projs;
//      UtVec3d projWCS;
//      for (double az = 1.0; az < 359.0; az += 15.0)
//      {
//         for (double el = -90; el < 91.0; el += 15.0)
//         {
//            //for (double az = 1.0; az < 359.0; az += 180.0)
//            //{
//            //   double el = 0.0;  //for()
//            //   {
//            //for (double az = 260.0; az < 290.0; az += 20.0)
//            //{
//            //   double el = 0.0;  //for()
//            //   {
//            //calculate new test position
//            //positions are tied to "mTargetEntity" initial position of (0,0,10000) set above
//            UtVec3d ned;
//            ned[0] = ned0[0] + distance * cos(az * UtMath::cRAD_PER_DEG) * cos(el * UtMath::cRAD_PER_DEG);
//            ned[1] = ned0[1] + distance * sin(az * UtMath::cRAD_PER_DEG) * cos(el * UtMath::cRAD_PER_DEG);
//            ned[2] = ned0[2] - distance * sin(el * UtMath::cRAD_PER_DEG);
//
//            //set position of entity & mover
//            //ut::log::error() << "convert location NED: " << ned[0] << " " << ned[1] << " " << ned[2];
//            UtVec3d lla = BrawlerCoordinateConversion::ConvertNEDtoLLA(ned);
//            //ut::log::info() << "testing location LLA: " << lla[0] << " " << lla[1] << " " << lla[2];
//
//            if (lla[2] < 0)
//            {
//               continue;
//            }
//
//            UtEntity::SetLocationLLA(lla[0], lla[1], lla[2]);
//            mMyState.SetLocationLLA(lla[0], lla[1], lla[2]);
//            brawlerMoverPtr->pos = ned;
//
//            brawlerMoverPtr->perfrm(); //after each position change!!!
//
//            //just set it instead of calling UpdateMissileMode()
//            SetMissileMode(INTERCEPT);
//
//            //set projected position
//            NominalProjection(dt);
//
//            //alternative maneuver
//            UtEntity projection;
//            //brawlerMoverPtr->FlyVectorWithSpeed(vec, gees, speed); //set command
//
//            //copied & adapted from "behavior alt3421_roll_15_pull"
//            UtVec3d a0;   //# maneuver plane
//            UtVec3d dir0; //# direction vector
//            double gmx;   //# max Gs
//            double spd0;  //# throttle setting
//            double sd15 = 0.2588190;
//            double cd15 = 0.9659258;
//            dir0 = UtVec3d(0.0, sd15, -cd15);
//            dir0 = ConvertWindtoNED(dir0);
//            double gmxsu = MaxTacticalGLoad_g();
//            //#call manpln(dir0,gmxsu)
//            //#manpln() - maneuver in plane: replicated in 6 lines below
//            dir0 = vorth(dir0, VelocityNED());
//            dir0.Normalize();
//            //int iactn = 4;
//            gmx = gmxsu;
//            spd0 = 3.0;
//            //int spdmod = 2;   //thrttl
//            //### Project and Evaluate (Score) Maneuver Alternative
//            //double score = brawlerPlatform.EvaluateVectorWithThrottle(dir0, gmx, spd0, ilevel, kalt, icall, lcall);
//            brawlerMoverPtr->FlyVectorWithThrottle(dir0, gmx, spd0);
//
//            //do it
//            brawlerMoverPtr->ProjectForward(dt, projection);
//            BrawlerPlatform::SetProjectedState(projection);
//
//            //evaluate
//            double score = BrawlerEvaluation::EvaluateAlternative(*this, &mTargetEntity);
//            if (score < mAlternativeMinScore)
//            {
//               mAlternativeMinScore = score;
//            }
//            if (score > mAlternativeMaxScore)
//            {
//               mAlternativeMaxScore = score;
//            }
//
//            scores.push_back(score);
//
//            projection.GetLocationWCS(projWCS.GetData());
//            projs.push_back(projWCS);
//         }
//      }
//
//      //draw all color coded scores
//
//      double locWCS[3] = { 0, 0, 0 };
//      mDrawPtr->SetId(mDrawPtr->GetNewId());
//      mDrawPtr->SetPointSize(3);
//      mDrawPtr->SetDuration(30);
//      //mDrawPtr->BeginPoints();
//      mDrawPtr->BeginLines();
//
//      int i = 0;
//      for (double az = 1.0; az < 359.0; az += 15.0)
//      {
//         //double el = 0.0;  //for()
//         for (double el = -90; el < 91.0; el += 15.0)
//         {
//            //for (double az = 0.0; az < 359.0; az += 180.0)
//            //{
//            //   double el = 0.0;  //for()
//
//            //calculate new test position
//            //positions are tied to "mTargetEntity" initial position of (0,0,10000) set above
//            UtVec3d ned;
//            ned[0] = ned0[0] + distance * cos(az * UtMath::cRAD_PER_DEG) * cos(el * UtMath::cRAD_PER_DEG);
//            ned[1] = ned0[1] + distance * sin(az * UtMath::cRAD_PER_DEG) * cos(el * UtMath::cRAD_PER_DEG);
//            ned[2] = ned0[2] - distance * sin(el * UtMath::cRAD_PER_DEG);
//
//            UtVec3d lla = BrawlerCoordinateConversion::ConvertNEDtoLLA(ned);
//
//            if (lla[2] < 0)
//            {
//               continue;
//            }
//
//            UtEntity drawEntity;
//            drawEntity.SetLocationLLA(lla[0], lla[1], lla[2]);
//            drawEntity.GetLocationWCS(locWCS);
//
//            double score = scores.at(i);
//            UtVec3d projWCS = projs.at(i);
//
//            ++i;
//            UtVec3d c = GetEvaluationColor(score);
//            mDrawPtr->SetColor(c[0], c[1], c[2]);
//
//            mDrawPtr->VertexWCS(locWCS);
//            //mDrawPtr->VertexWCS(locWCS);  // Need two point because of VESPA bug?
//            mDrawPtr->VertexWCS(projWCS.GetData());
//         }
//      }
//      mDrawPtr->End();
//      TestEvaluation = false; //only do it once
//   }
//#endif
//}

// private
void WsfBrawlerProcessor::UpdateMissileMode()
{
   // TODO - add parts of mislmd.f ?

   // Check if I have any weapon tasks
   std::list<WsfTask*> weaponTasks = TaskListReceivedOfType("WEAPON");
   if (!weaponTasks.empty())
   {
      // If I do ( how to determine which one to use?)
      // I am at least in missile mode 1
      SetMissileMode(BRAWLER::INTERCEPT);

      std::list<WsfTask*>::iterator tli = weaponTasks.begin();
      for (; tli != weaponTasks.end(); ++tli)
      {
         WsfTask& task = *(*tli);
         // Get track ID and check then number of pending/active weapons
         // If we have a pending or active weapon missile mode is 2
         const WsfTrackId& localTrackId         = task.GetLocalTrackId();
         int               weaponsPendingActive = WsfWeapon::WeaponsActiveAndPendingFor(*GetPlatform(), localTrackId);
         if (weaponsPendingActive > 0)
         {
            SetMissileMode(BRAWLER::AIMFIRE);
         }
      }
   } // end weaponTasks != empty
   else
   {
      SetMissileMode(BRAWLER::NOTASK);
   }
}

// virtual
bool WsfBrawlerProcessor::HaveWeapons()
{
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      if (iter->GetQuantityRemaining() > 0.0)
      {
         return true;
      }
   }
   return false;
}


// virtual
void WsfBrawlerProcessor::SetLocationWCS(const double aLocationWCS[3])
{
   GetPlatform()->SetLocationWCS(aLocationWCS);
   mMyState.SetLocationWCS(aLocationWCS);

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if ((moverPtr != nullptr) && (moverPtr->GetType() == "WSF_BRAWLER_MOVER"))
   {
      UtVec3d lla;
      GetPlatform()->GetLocationLLA(lla[0], lla[1], lla[2]);
      dynamic_cast<WsfBrawlerMover*>(moverPtr)->pos = BrawlerCoordinateConversion::ConvertLLAtoNED(lla);
   }
}

// virtual
void WsfBrawlerProcessor::SetLocationECI(const double aLocationWCS[3])
{
   GetPlatform()->SetLocationECI(aLocationWCS);
   mMyState.SetLocationECI(aLocationWCS);

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if ((moverPtr != nullptr) && (moverPtr->GetType() == "WSF_BRAWLER_MOVER"))
   {
      UtVec3d lla;
      GetPlatform()->GetLocationLLA(lla[0], lla[1], lla[2]);
      dynamic_cast<WsfBrawlerMover*>(moverPtr)->pos = BrawlerCoordinateConversion::ConvertLLAtoNED(lla);
   }
}

// virtual
void WsfBrawlerProcessor::SetLocationLLA(double aLat, double aLon, double aAlt)
{
   GetPlatform()->SetLocationLLA(aLat, aLon, aAlt);
   mMyState.SetLocationLLA(aLat, aLon, aAlt);

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if ((moverPtr != nullptr) && (moverPtr->GetType() == "WSF_BRAWLER_MOVER"))
   {
      UtVec3d lla = UtVec3d(aLat, aLon, aAlt);
      dynamic_cast<WsfBrawlerMover*>(moverPtr)->pos =
         BrawlerCoordinateConversion::ConvertLLAtoNED(lla); // UtVec3d(aLat, aLon, aAlt));
   }
}

// -----------------------------------------------------------------------------------------------------

double WsfBrawlerProcessor::MaxForwardAcceleration_fps2()
{
   // return value
   double accel_fps2 = 0.0;

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         accel_fps2 = dynamic_cast<WsfBrawlerMover*>(moverPtr)->almaxg;
      }
   }
   return accel_fps2;
}

double WsfBrawlerProcessor::MinForwardAcceleration_fps2()
{
   double    accel_fps2 = 0.0;
   WsfMover* moverPtr   = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         accel_fps2 = dynamic_cast<WsfBrawlerMover*>(moverPtr)->alming;
      }
   }
   return accel_fps2;
}

double WsfBrawlerProcessor::MaxForwardAccelerationWithGravity_fps2()
{
   double    accel_fps2 = 0.0;
   WsfMover* moverPtr   = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         accel_fps2 = dynamic_cast<WsfBrawlerMover*>(moverPtr)->almax;
      }
   }
   return accel_fps2;
}

double WsfBrawlerProcessor::MinForwardAccelerationWithGravity_fps2()
{
   double    accel_fps2 = 0.0;
   WsfMover* moverPtr   = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         accel_fps2 = dynamic_cast<WsfBrawlerMover*>(moverPtr)->almin;
      }
   }
   return accel_fps2;
}

double WsfBrawlerProcessor::MaxInstantaneousGLoad_g()
{
   double    gLoad    = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         gLoad =
            brawlerMoverPtr->gliftm(brawlerMoverPtr->Mach(), brawlerMoverPtr->DynamicPressure(), brawlerMoverPtr->wate);
      }
   }
   return gLoad;
}

double WsfBrawlerProcessor::MinInstantaneousTurnRadius_ft()
{
   double radius_ft  = 0.0;
   double accel_g    = MaxInstantaneousGLoad_g();
   double accel_fps2 = accel_g * BRAWLER::cFtSec2PerG;

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         if (std::abs(accel_fps2) > std::numeric_limits<double>::epsilon())
         {
            double vel_fps = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Speed();
            radius_ft      = (vel_fps * vel_fps) / accel_fps2;
         }
      }
   }
   return radius_ft;
}

double WsfBrawlerProcessor::MaxInstantaneousTurnRate_rps()
{
   double    turnRate_rps = 0.0;
   WsfMover* moverPtr     = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         turnRate_rps = dynamic_cast<WsfBrawlerMover*>(moverPtr)->wmax;
      }
   }
   return turnRate_rps;
}

double WsfBrawlerProcessor::MaxSustainedGLoad_g()
{
   // In this case, we do not simply pull max-g, but pull-g to a level
   // where we could hold that g-load if using full power.

   double    gLoad    = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         gLoad = dynamic_cast<WsfBrawlerMover*>(moverPtr)->gmxsut;
      }
   }
   return gLoad;
}

double WsfBrawlerProcessor::MinSustainedTurnRadius_ft()
{
   double radius_ft  = 0.0;
   double accel_g    = MaxSustainedGLoad_g();
   double accel_fps2 = accel_g * BRAWLER::cFtSec2PerG;

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         if (std::abs(accel_fps2) > std::numeric_limits<double>::epsilon())
         {
            double vel_fps = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Speed();
            radius_ft      = (vel_fps * vel_fps) / accel_fps2;
         }
      }
   }
   return radius_ft;
}

double WsfBrawlerProcessor::MaxSustainedTurnRate_rps()
{
   double turnRate_rps = 0.0;
   double accel_g      = MaxSustainedGLoad_g();
   double accel_fps2   = accel_g * BRAWLER::cFtSec2PerG;

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         if (std::abs(accel_fps2) > std::numeric_limits<double>::epsilon())
         {
            double vel_fps   = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Speed();
            double radius_ft = (vel_fps * vel_fps) / accel_fps2;
            if (radius_ft > std::numeric_limits<double>::epsilon())
            {
               turnRate_rps = vel_fps / radius_ft;
            }
         }
      }
   }
   return turnRate_rps;
}


double WsfBrawlerProcessor::MaxTacticalGLoad_g()
{
   if ((MissileMode() == BRAWLER::INTERCEPT) || (MissileMode() == BRAWLER::AIMFIRE))
   {
      // c        --allow max turns because firing interest
      // gmxsu = gmxin;
      return MaxInstantaneousGLoad_g();
   }

   // double rhst  = SlantRange(GetTarget());   // closest range to [hostile] target
   double    rhst   = 0.0;
   UtEntity* tgtPtr = GetTarget();
   if (tgtPtr != nullptr)
   {
      rhst = SlantRange(*tgtPtr); // closest range to [hostile] target
   }
   // double rhstm = SlantRange(GetThreat());   // closest range to [hostile] missile
   double    rhstm     = 0.0;
   UtEntity* threatPtr = GetThreat();
   if (threatPtr != nullptr)
   {
      rhstm = SlantRange(*threatPtr); // closest range to [hostile] missile
   }

   if (BRAWLER::amin1(rhst, rhstm) * BRAWLER::ftnmi > 20.0)
   {
      // c        --no need to turn hard at longer range
      // gmxsu = gmxsut
      return MaxSustainedGLoad_g();
   }

   if (Mach() > 1.0 || GetSpeed() > (CorneringVelocity_fps() + 100.0))
   {
      // c        --in vicinity of hostiles, fast enough to use gmxin
      // gmxsu = gmxin;
      return MaxInstantaneousGLoad_g();
   }
   // else
   //{
   // c        --in vicinity of hostiles, but too slow to exceed gmxsut
   // c        --note that this won't apply if weapons firing interest
   // gmxsu = gmxsut
   return MaxSustainedGLoad_g();
   //}
}

double WsfBrawlerProcessor::MinTacticalTurnRadius_ft()
{
   double radius_ft = 0.0;

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         radius_ft = dynamic_cast<WsfBrawlerMover*>(moverPtr)->radmnp;
      }
   }
   return radius_ft;
}

double WsfBrawlerProcessor::MaxTacticalTurnRate_rps()
{
   double turnRate_rps = 0.0;
   double accel_g      = MaxSustainedGLoad_g();
   double accel_fps2   = accel_g * BRAWLER::cFtSec2PerG;

   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         if (std::abs(accel_fps2) > std::numeric_limits<double>::epsilon())
         {
            double vel_fps   = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Speed();
            double radius_ft = (vel_fps * vel_fps) / accel_fps2;
            if (radius_ft > std::numeric_limits<double>::epsilon())
            {
               turnRate_rps = vel_fps / radius_ft;
            }
         }
      }
   }
   return turnRate_rps;
}

double WsfBrawlerProcessor::CorneringVelocity_fps()
{
   double    corneringSpeed_fps = 0.0;
   WsfMover* moverPtr           = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         corneringSpeed_fps = dynamic_cast<WsfBrawlerMover*>(moverPtr)->cornrv;
      }
   }
   return corneringSpeed_fps;
}

double WsfBrawlerProcessor::SpecificEnergyRate_fps()
{
   double    specificEnergyRate_fps = 0.0;
   WsfMover* moverPtr               = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         specificEnergyRate_fps = dynamic_cast<WsfBrawlerMover*>(moverPtr)->SpecificEnergyRate();
      }
   }
   return specificEnergyRate_fps;
}

double WsfBrawlerProcessor::CalcTurnRate_rps(const double& aClMax,
                                             const double& aSpeed_fps,
                                             const double& aDynamicPressure_psf,
                                             const double& aWingArea_ft2)
{
   double turnRate_rps = 0.0;
   double gLoad_g      = aDynamicPressure_psf * aWingArea_ft2 * aClMax;
   double accel_fps2   = gLoad_g * BRAWLER::cFtSec2PerG;

   if (std::abs(accel_fps2) > std::numeric_limits<double>::epsilon())
   {
      double radius_ft = (aSpeed_fps * aSpeed_fps) / accel_fps2;

      //    How to calculate turn rate .....
      //    circumference_ft = TWO_PI * radius_ft;
      //    time_to_cover_circumference_sec = circumference_ft / aSpeed_fps;
      //    turnRate_rps = TWO_PI / time_to_cover_circumference_sec;
      //    turnRate_rps = TWO_PI / ((TWO_PI * radius_ft) / vel_fps);
      //    turnRate_rps = 1.0 / (radius_ft / vel_fps);

      if (radius_ft > std::numeric_limits<double>::epsilon())
      {
         turnRate_rps = aSpeed_fps / radius_ft;
      }
   }

   return turnRate_rps;
}

double WsfBrawlerProcessor::CurrentWeight_lbs()
{
   double    weight_lbs = 0.0;
   WsfMover* moverPtr   = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         weight_lbs = dynamic_cast<WsfBrawlerMover*>(moverPtr)->AircraftMass() * BRAWLER::grav;
      }
   }
   return weight_lbs;
}

double WsfBrawlerProcessor::EmptyWeight_lbs()
{
   double    weight_lbs = 0.0;
   WsfMover* moverPtr   = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         weight_lbs = dynamic_cast<WsfBrawlerMover*>(moverPtr)->EmptyMass() * BRAWLER::grav;
      }
   }
   return weight_lbs;
}

double WsfBrawlerProcessor::CurrentFuelWeight_lbs()
{
   double    weight_lbs = 0.0;
   WsfMover* moverPtr   = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         weight_lbs = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Gas() * BRAWLER::grav;
      }
   }
   return weight_lbs;
}

double WsfBrawlerProcessor::DesignWeightForMaxG_lbs()
{
   double    weight_lbs = 0.0;
   WsfMover* moverPtr   = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         weight_lbs = dynamic_cast<WsfBrawlerMover*>(moverPtr)->miss_mass;
      }
   }
   return weight_lbs;
}

double WsfBrawlerProcessor::CurrentStructuralGLimit_g()
{
   double    structuralGLimit_g = 0.0;
   WsfMover* moverPtr           = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         // gcap lookup
         structuralGLimit_g = BRAWLER::tbl1(brawlerMoverPtr->Mach(),
                                            &(brawlerMoverPtr->gcap[0][0]),
                                            brawlerMoverPtr->mchval,
                                            brawlerMoverPtr->nmach1);
      }
   }
   return structuralGLimit_g;
}

double WsfBrawlerProcessor::StructuralGLimitAtDesignWeight_g()
{
   double    structuralGLimit_g = 0.0;
   WsfMover* moverPtr           = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         structuralGLimit_g               = BRAWLER::tbl1(brawlerMoverPtr->Mach(),
                                            &(brawlerMoverPtr->gcap2[0][0]),
                                            brawlerMoverPtr->mchval,
                                            brawlerMoverPtr->nmach1);
      }
   }
   return structuralGLimit_g;
}

double WsfBrawlerProcessor::StructuralGLimitAboveDesignWeight_g()
{
   return mStructuralGLimitAboveDesignWeight_g;
}

double WsfBrawlerProcessor::DynamicPressure_psf()
{
   double    dynPressure_psf = 0.0;
   WsfMover* moverPtr        = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         dynPressure_psf = dynamic_cast<WsfBrawlerMover*>(moverPtr)->DynamicPressure();
      }
   }
   return dynPressure_psf;
}

double WsfBrawlerProcessor::Mach()
{
   double    mach     = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         mach = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Mach();
      }
   }
   return mach;
}

double WsfBrawlerProcessor::GetMaxCL()
{
   double    clmax    = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         clmax =
            BRAWLER::tbl1(brawlerMoverPtr->Mach(), brawlerMoverPtr->clmax, brawlerMoverPtr->fmvec1, brawlerMoverPtr->nfm1);
      }
   }
   return clmax;
}

double WsfBrawlerProcessor::GetMaxCL(const double& aMach)
{
   double    clmax    = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         clmax = BRAWLER::tbl1(aMach, brawlerMoverPtr->clmax, brawlerMoverPtr->fmvec1, brawlerMoverPtr->nfm1);
      }
   }
   return clmax;
}

double WsfBrawlerProcessor::GetMaxCL(const double& aAlt_ft, const double& aMach)
{
   (void)aAlt_ft; // Altitude is currently not used

   double    clmax    = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         clmax = BRAWLER::tbl1(aMach, brawlerMoverPtr->clmax, brawlerMoverPtr->fmvec1, brawlerMoverPtr->nfm1);
      }
   }
   return clmax;
}

double WsfBrawlerProcessor::GetAlphaAtMaxCL_deg()
{
   double    alpha    = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         double           maxCL           = GetMaxCL();
         // TODO need to check structual limit CL max? See BrawlerMover::aoamx1
         int    im = 0;
         double pm = 0.0;
         BRAWLER::srch(brawlerMoverPtr->fmvec2, brawlerMoverPtr->nfm2, brawlerMoverPtr->Mach(), im, pm);
         if (im == 0)
         {
            // The value to be interpolated is outside the limits of table
            if (brawlerMoverPtr->Mach() < brawlerMoverPtr->fmvec2[0])
            {
               // off the low end
               im = 1;
               pm = 0.0;
            }
            else
            {
               // off the high end
               im = brawlerMoverPtr->nfm2 - 1;
               pm = 1.0;
            }
         }
         int    ia = 0;
         double pa = 0.0;
         BRAWLER::invtb2(brawlerMoverPtr->clift, maxCL, brawlerMoverPtr->nfm2, im, pm, false, brawlerMoverPtr->nalph1, ia, pa);
         alpha = BRAWLER::param1(ia, pa, brawlerMoverPtr->alfv1);
      }
   }
   return alpha;
}

double WsfBrawlerProcessor::GetAlphaAtMaxCL_deg(const double& aMach)
{
   double    alpha    = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         double           maxCL           = GetMaxCL();
         int              im              = 0;
         double           pm              = 0.0;
         BRAWLER::srch(brawlerMoverPtr->fmvec2, brawlerMoverPtr->nfm2, aMach, im, pm);
         if (im == 0)
         {
            // The value to be interpolated is outside the limits of table
            if (aMach < brawlerMoverPtr->fmvec2[0])
            {
               // off the low end
               im = 1;
               pm = 0.0;
            }
            else
            {
               // off the high end
               im = brawlerMoverPtr->nfm2 - 1;
               pm = 1.0;
            }
         }
         int    ia = 0;
         double pa = 0.0;
         BRAWLER::invtb2(brawlerMoverPtr->clift, maxCL, brawlerMoverPtr->nfm2, im, pm, false, brawlerMoverPtr->nalph1, ia, pa);
         alpha = BRAWLER::param1(ia, pa, brawlerMoverPtr->alfv1);
      }
   }
   return alpha;
}

double WsfBrawlerProcessor::GetAlphaAtMaxCL_deg(const double& aAlt_ft, const double& aMach)
{
   double    alpha    = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         WsfBrawlerMover* brawlerMoverPtr = dynamic_cast<WsfBrawlerMover*>(moverPtr);
         double           maxCL           = GetMaxCL();
         int              im              = 0;
         double           pm              = 0.0;
         BRAWLER::srch(brawlerMoverPtr->fmvec2, brawlerMoverPtr->nfm2, aMach, im, pm);
         if (im == 0)
         {
            // The value to be interpolated is outside the limits of table
            if (aMach < brawlerMoverPtr->fmvec2[0])
            {
               // off the low end
               im = 1;
               pm = 0.0;
            }
            else
            {
               // off the high end
               im = brawlerMoverPtr->nfm2 - 1;
               pm = 1.0;
            }
         }
         int    ia = 0;
         double pa = 0.0;
         BRAWLER::invtb2(brawlerMoverPtr->clift, maxCL, brawlerMoverPtr->nfm2, im, pm, false, brawlerMoverPtr->nalph1, ia, pa);
         alpha = BRAWLER::param1(ia, pa, brawlerMoverPtr->alfv1);
      }
   }
   return alpha;
}

// replaces "wmax" from perfrm.f
double WsfBrawlerProcessor::MaxAngularRate_rps()
{
   // double wmax = MaxTacticalGLoad_g() * BRAWLER::grav / GetSpeed();
   return MaxTacticalGLoad_g() * BRAWLER::grav / GetSpeed();
}

double WsfBrawlerProcessor::Lift(const double& alpha, const double& speed, const double& altitude)
{
   double    lift     = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         lift = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Lift(alpha * UtMath::cRAD_PER_DEG,
                                                               speed * UtMath::cFT_PER_M,
                                                               altitude * UtMath::cFT_PER_M);
      }
   }
   return lift;
}

double WsfBrawlerProcessor::Thrust(const double& throttle, const double& speed, const double& altitude)
{
   double    thrust   = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         thrust = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Thrust(throttle,
                                                                   speed * UtMath::cFT_PER_M,
                                                                   altitude * UtMath::cFT_PER_M);
      }
   }
   return thrust;
}

double WsfBrawlerProcessor::Throttle() // setting [1-3]
{
   double    throttle = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         throttle = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Throttle();
      }
   }
   return throttle;
}

double WsfBrawlerProcessor::Drag(const double& alpha, const double& speed, const double& altitude)
{
   double    drag     = 0.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         drag = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Drag(alpha * UtMath::cRAD_PER_DEG,
                                                               speed * UtMath::cFT_PER_M,
                                                               altitude * UtMath::cFT_PER_M);
      }
   }
   return drag;
}

// TODO - grabbing throttle out of brawler is harder than I thought, do this later : )
// double WsfBrawlerProcessor::Throttle()
//{
//
// }

double WsfBrawlerProcessor::Area()
{
   double    area_sqft = 0.0;
   WsfMover* moverPtr  = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         area_sqft = dynamic_cast<WsfBrawlerMover*>(moverPtr)->ReferenceArea();
      }
   }
   return area_sqft;
}

double WsfBrawlerProcessor::Alpha()
{
   double    alpha_deg = 0.0;
   WsfMover* moverPtr  = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         alpha_deg = dynamic_cast<WsfBrawlerMover*>(moverPtr)->Alpha() * UtMath::cDEG_PER_RAD;
      }
   }
   return alpha_deg;
}

double WsfBrawlerProcessor::AlphaCL0()
{
   double    alphacl0_deg = 0.0;
   WsfMover* moverPtr     = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetType() == "WSF_BRAWLER_MOVER")
      {
         alphacl0_deg = dynamic_cast<WsfBrawlerMover*>(moverPtr)->AlphaCL0() * UtMath::cDEG_PER_RAD;
      }
   }
   return alphacl0_deg;
}
