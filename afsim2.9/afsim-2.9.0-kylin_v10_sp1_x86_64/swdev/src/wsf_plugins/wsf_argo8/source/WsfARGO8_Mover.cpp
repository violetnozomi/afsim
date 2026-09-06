// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfARGO8_Mover.hpp"

#include <climits>

#include "UtInput.hpp"
#include "UtPath.hpp"
#include "UtQuaternion.hpp"
#include "UtRandom.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"

#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfScenario.hpp"
#include "WsfApplication.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrack.hpp"
#include "WsfUtil.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfDraw.hpp"
#include "WsfSensor.hpp"


WsfARGO8_Mover::WsfARGO8_Mover(WsfScenario& aScenario)
   :  WsfMover(aScenario),
      mMissileType("standard"),
      mMissileLibrary(""),
      mGuidanceMethodTruth(false),
      mSeekerMethodTruth(false),
      mLoggingEnabled(false),
      mLogFilePath(""),
      mMissile(),
      mReference(),
      mEngagementState(cES_BOTHALIVE),
      mLastTrackLocation(),
      mLaunchTime(-DBL_MAX),
      mLastTrackUpdateTime(-DBL_MAX),
      mEndgameGees(0.0),
      mMissileState(cMS_COMPUTERHOLD),
      mTerminationCompleted(false),
      mSeekerOn(false),
      mFuzeOutput(false),
      mMissileTimeStep(0.01),
      mGuidanceTime(1.0),
      mGuidanceStep(0.0),
      mMissileStep(-1),
      mLastMissileUpdateTime(-1.0),
      mLastGuidLoc(),
      mLastGuidVel()
{
   mLastGuidLoc[0] = 0;
   mLastGuidLoc[1] = 0;
   mLastGuidLoc[2] = 0;
   mLastGuidVel[0] = 0;
   mLastGuidVel[1] = 0;
   mLastGuidVel[2] = 0;
}

WsfARGO8_Mover::WsfARGO8_Mover(const WsfARGO8_Mover& aSrc)
   :   WsfMover(aSrc),
       mMissileType(aSrc.mMissileType),
       mMissileLibrary(aSrc.mMissileLibrary),
       mGuidanceMethodTruth(aSrc.mGuidanceMethodTruth),
       mSeekerMethodTruth(aSrc.mSeekerMethodTruth),
       mLoggingEnabled(aSrc.mLoggingEnabled),
       mLogFilePath(aSrc.mLogFilePath),
       mMissile(aSrc.mMissile),
       mReference(aSrc.mReference),
       mEngagementState(aSrc.mEngagementState),
       mLastTrackLocation(),
       mLaunchTime(-DBL_MAX),
       mLastTrackUpdateTime(-DBL_MAX),
       mEndgameGees(aSrc.mEndgameGees),
       mMissileState(aSrc.mMissileState),
       mTerminationCompleted(aSrc.mTerminationCompleted),
       mSeekerOn(aSrc.mSeekerOn),
       mFuzeOutput(aSrc.mFuzeOutput),
       mMissileTimeStep(aSrc.mMissileTimeStep),
       mGuidanceTime(aSrc.mGuidanceTime),
       mGuidanceStep(aSrc.mGuidanceStep),
       mMissileStep(aSrc.mMissileStep),
       mLastMissileUpdateTime(aSrc.mLastMissileUpdateTime),
       mLastGuidLoc(),
       mLastGuidVel()
{
   UtVec3d::Set(mLastTrackLocation,aSrc.mLastTrackLocation);
   UtVec3d::Set(mLastGuidLoc,aSrc.mLastGuidLoc);
   UtVec3d::Set(mLastGuidVel, aSrc.mLastGuidVel);
}

WsfARGO8_Mover::~WsfARGO8_Mover()
{
}

WsfMover* WsfARGO8_Mover::Clone() const
{
   return new WsfARGO8_Mover(*this);
}

bool WsfARGO8_Mover::Initialize(double aSimTime)
{
   bool ok = true;
   if (mMissileType == "")
   {
       ok = false;
       std::cout << "***** ERROR: User must input ARGO missile model type using 'missile_type' input command." << std::endl;
   }
   ok &= mMissile.Initialize(mMissileType, mMissileLibrary);
   ok &= WsfMover::Initialize(aSimTime);
   if (ok)
   {
      mMissileTimeStep = mMissile.GetUpdateInterval();
      if (UseDeferredLaunching())
      {
         double updateInterval = GetSimulation()->GetTimestep();
         if (updateInterval <= 0.0)                  // Event driven
         {
            updateInterval = mMissileTimeStep;
            if (updateInterval < 0.010)              // Don't run faster than about 100 Hz
            {
               int stepsPerUpdate = static_cast<int>((0.010 / mMissileTimeStep) + 0.5);
               updateInterval = stepsPerUpdate * mMissileTimeStep;
            }
         }
         else if (updateInterval > mMissileTimeStep)
         {
            std::cout << "****WARNING: Simulation Timestep: " << updateInterval
                      << " is greater than the ARGO '" << mMissileType
                      << "' timestep: " << mMissileTimeStep << std::endl;
         }
         else if (fmod(mMissileTimeStep, updateInterval) != 0.0001)
         {
            std::cout << "****WARNING: Simulation Timestep: " << updateInterval
                      << " is not an integral step to the ARGO '" << mMissileType
                      << "' timestep: " << mMissileTimeStep << std::endl;
         }
         SetUpdateInterval(updateInterval);
      }
      mLaunchTime = aSimTime;
      mLastMissileUpdateTime = mLaunchTime - mMissileTimeStep;
      mMissileStep = -1;
      mMissile.SetMissileLaunchTime(aSimTime);
      if (mLoggingEnabled)
      {
         UtPath logPath(mLogFilePath);
         if (logPath.Stat())
         {
            mMissile.SetupLogging(true, mLogFilePath);
         }
         else
         {
            std::cout << "****WARNING: Log file path '" << mLogFilePath << "' invalid for '"
                      << mMissileType << '\'' << std::endl;
         }
      }
   }
   return ok;
}
bool WsfARGO8_Mover::Initialize2(double aSimTime)
{
   //Setting reference for NED and ESD conversions
   WsfPlatform* shooter = GetFiringPlatform();
   WsfPlatform* target = GetTargetPlatform();
   double lat, lon, alt, lat2, lon2;
   if (shooter != 0 && target != 0)
   {
      shooter->GetLocationLLA(lat, lon, alt);
      target->GetLocationLLA(lat2, lon2, alt);
      mReference.SetLocationLLA((lat+lat2)/2, (lon+lon2)/2, 0);
   }
   SetRailData(mGuidanceMethodTruth, aSimTime);
   return true;
}

bool WsfARGO8_Mover::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "missile_model") ||
       (command == "missile_type"))
   {
      aInput.ReadValue(mMissileType);
   }
   else if (command == "library_name")
   {
       aInput.ReadValue(mMissileLibrary);
   }
   else if (command == "argo_log_file_path")
   {
      mLoggingEnabled = true;
      aInput.ReadValueQuoted(mLogFilePath);
      mLogFilePath = aInput.SubstitutePathVariables(mLogFilePath);
      if ((mLogFilePath.back() != '\\') ||
          (mLogFilePath.back() != '/'))
      {
         mLogFilePath += '/';
      }
   }
   else if (command == "guidance_update_interval")
   {
       aInput.ReadValue(mGuidanceTime);
   }
   else if (command == "guidance_method")
   {
      std::string method;
      aInput.ReadValue(method);
      mGuidanceMethodTruth = (method == "truth" ? true : false);
   }
   else if (command == "seeker_method")
   {
      std::string method;
      aInput.ReadValue(method);
      mSeekerMethodTruth = (method == "truth" ? true : false);
   }
   else if (command == "output_fuze_data")
   {
       mFuzeOutput = true;
   }
   //endgame_gees Should never be used in regular engagements,
   //only as use in weapon_tools or other analysis
   else if (command == "endgame_gees")
   {
      if(GetScenario().GetApplication().GetApplicationName() == "mission")
      {
         std::cout << "ERROR: endgame_gees should not be specified for mission" << std::endl;
         std::cout << "endgame_gees should only be utilized in tools such as weapon_tools" << std::endl;
      }
      else
      {
      aInput.ReadValue(mEndgameGees);
      }
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

void WsfARGO8_Mover::Update(double aSimTime)
{
   if((mLastMissileUpdateTime + mMissileTimeStep) <= aSimTime)
   {
      if (!mTerminationCompleted && GetFiringPlatform() != 0 && GetTargetPlatform()!= 0)
      {
         ContinueFlight(aSimTime);
      }
      else if(!mTerminationCompleted)
      {
         CoastFlight(aSimTime);
      }
   }
   WsfMover::Update(aSimTime);
}
void WsfARGO8_Mover::ContinueFlight(double aSimTime)
{
   while ((mLastMissileUpdateTime + mMissileTimeStep) <= aSimTime)
   {
      int fuzeState(0);
      UpdateARGO8(mLastMissileUpdateTime + mMissileTimeStep);
      mMissileState = mMissile.Update(mLastMissileUpdateTime + mMissileTimeStep,
                                        fuzeState,
                                        GetPlatform()->GetName());
      UpdateMissile(mLastMissileUpdateTime + mMissileTimeStep);

      if (mMissileState == cMS_TERMINATED)
      {
         TerminateMissile(aSimTime, fuzeState, GetEngagementDistance(aSimTime, mEngagementState));
         break;
      }
      ++mMissileStep;
      mLastMissileUpdateTime = mLaunchTime + (mMissileStep * mMissileTimeStep);
   }
}

void WsfARGO8_Mover::CoastFlight(double aSimTime)
{
   int fuzeState(0);
   while ((mLastMissileUpdateTime + mMissileTimeStep) <= aSimTime)
   {
      if (mEngagementState == cES_BOTHALIVE)
      {
         //Dead Shooter
         if (GetTargetPlatform() != 0 && GetFiringPlatform() == 0)
         {
            mEngagementState = cES_DEADSHOOTER;
         }
         //Dead Target
         else if (GetFiringPlatform() != 0 && GetTargetPlatform() == 0)
         {
            mEngagementState = cES_DEADTARGET;
         }
      }
      //Both Dead
      if (GetFiringPlatform() == 0 && GetTargetPlatform() == 0)
      {
         mEngagementState = cES_BOTHDEAD;
      }
      UpdateARGO8Coast(mLastMissileUpdateTime + mMissileTimeStep, mEngagementState);
      mMissileState = mMissile.Update(mLastMissileUpdateTime + mMissileTimeStep,
                                        fuzeState,
                                        GetPlatform()->GetName());
      UpdateMissile(mLastMissileUpdateTime + mMissileTimeStep);

      if (mMissileState == cMS_TERMINATED)
      {
         TerminateMissile(aSimTime, fuzeState, GetEngagementDistance(aSimTime, mEngagementState));
         break;
      }
      ++mMissileStep;
      mLastMissileUpdateTime = mLaunchTime + (mMissileStep * mMissileTimeStep);
   }
}

void WsfARGO8_Mover::UpdateMissile(double aSimTime)
{
   double locLLA[3];
   double relPosNED[3];
   double relVelNED[3], velNED[3];
   double relAccNED[3], accNED[3];
   double Psi, Theta, Phi;
   mMissile.GetMissileState(aSimTime, relPosNED, relVelNED, relAccNED, Psi, Theta, Phi);

   // Location
   UtEntity temp(mReference);
   WsfPlatform* missile = GetPlatform();

   temp.ConvertNEDToLLA(relPosNED,locLLA[0],locLLA[1],locLLA[2]);
   missile->SetLocationLLA(locLLA[0],locLLA[1],-relPosNED[2]);

   // Velocity
   temp.GetVelocityNED(velNED);
   UtVec3d::Add(velNED, velNED, relVelNED);
   missile->SetVelocityNED(velNED);
   // Acceleration
   temp.GetAccelerationNED(accNED);
   UtVec3d::Add(accNED, accNED, relAccNED);
   missile->SetAccelerationNED(accNED);
   // Orientation
   missile->SetOrientationNED(Psi, Theta, Phi);
}

void WsfARGO8_Mover::UpdateARGO8Coast(double aSimTime,
                                      EngagementState aEngagementState)
{
   double tgtlocESD[3], tgtvESD[3], guidlocESD[3], guidvESD[3];
   bool update = false;
   if (aEngagementState == cES_DEADSHOOTER)
   {
      if (mMissile.GetMissileState() == cMS_ONRAIL)
      {
         WsfPlatform* missile = GetPlatform();
         missile->GetMover()->TurnOff(aSimTime);
         missile->SetDamageFactor(1.0);
         GetSimulation()->DeletePlatform(aSimTime+.01, GetPlatform());
      }
      else
      {
         WsfSensor* sensorPtr = GetPlatform()->GetComponent<WsfSensor>();

         if(sensorPtr !=0 && sensorPtr->IsTurnedOn())
         {
            mSeekerOn = true;
         }
         else if(sensorPtr !=0 && !sensorPtr->IsTurnedOn())
         {
            mSeekerOn = false;
         }
         else
         {
            mMissile.GetSeekerState(mSeekerOn);
         }
         WsfPlatform* target = GetTargetPlatform();
         const WsfTrack* trackPtr = GetTargetTrack();
         GetLocationESD(tgtlocESD, target);
         GetVelocityESD(tgtvESD, target);
         if(mSeekerMethodTruth)
         {
            UtVec3d::Set(guidlocESD,tgtlocESD);
            UtVec3d::Set(guidvESD,tgtvESD);
         }
         else if(trackPtr != 0 && HasTrackUpdated(trackPtr))
         {
            update = true;
            GetLocationESD(guidlocESD, trackPtr, aSimTime);
            GetVelocityESD(guidvESD, trackPtr);
         }
      }
      mMissile.SetDynamicData(tgtlocESD,tgtvESD,update,guidlocESD,guidvESD,true);
   }
   else if (aEngagementState == cES_DEADTARGET)
   {
      if (mMissile.GetMissileState() == cMS_ONRAIL)
      {
         SetRailData(false, aSimTime);
      }
      else
      {
         const WsfTrack* trackPtr = GetTargetTrack();
         if (trackPtr != 0)
         {
         GetLocationESD(guidlocESD, trackPtr, aSimTime);
         GetVelocityESD(guidvESD, trackPtr);
            if(HasTrackUpdated(trackPtr))
            {
               update = true;
            }
         mMissile.SetDynamicData(tgtlocESD,tgtvESD,update,guidlocESD,guidvESD,true);
         }
      }

   }
   else if (aEngagementState == cES_BOTHDEAD)
   {
      if (mMissile.GetMissileState() == cMS_ONRAIL)
      {
         WsfPlatform* missile = GetPlatform();
         missile->GetMover()->TurnOff(aSimTime);
         missile->SetDamageFactor(1.0);
         GetSimulation()->DeletePlatform(aSimTime+.01, missile);
      }
   }
}
void WsfARGO8_Mover::UpdateARGO8(double aSimTime)
{
   WsfPlatform* target = GetTargetPlatform();
   bool update;
   double guidlocESD[3], guidvESD[3], tgtlocESD[3], tgtvESD[3];

   GetLocationESD(tgtlocESD, target);
   GetVelocityESD(tgtvESD,   target);

   if(mMissile.GetMissileState() == cMS_ONRAIL)
   {
      SetRailData(mGuidanceMethodTruth, aSimTime);
   }
   else
   {
      //Seeker in AFSIM is prioritized over checking seeker status in ARGO
      //Not sure what behavior would happen if multiple sensors were put on missile
      WsfSensor* sensorPtr = GetPlatform()->GetComponent<WsfSensor>();
      if(sensorPtr !=0 && sensorPtr->IsTurnedOn())
      {
         mSeekerOn = true;
      }
      else if(sensorPtr !=0 && !sensorPtr->IsTurnedOn())
      {
         mSeekerOn = false;
      }
      else
      {
         mMissile.GetSeekerState(mSeekerOn);
      }
      mGuidanceStep -= mMissileTimeStep;
      if(mGuidanceStep <= 0 && mSeekerOn == false){
         mGuidanceStep = mGuidanceTime;
         update = true;
         if(!mGuidanceMethodTruth)
         {
            const WsfTrack* trackPtr = GetTargetTrack();
            if(trackPtr != 0 && HasTrackUpdated(trackPtr))
            {
               GetLocationESD(guidlocESD, trackPtr, aSimTime);
               GetVelocityESD(guidvESD, trackPtr);
               UtVec3d::Set(mLastGuidLoc, guidlocESD);
               UtVec3d::Set(mLastGuidVel, guidvESD);
            }
            else{update = false;}
         }
         else
         {
            UtVec3d::Set(guidlocESD, tgtlocESD);
            UtVec3d::Set(guidvESD, tgtvESD);
            UtVec3d::Set(mLastGuidLoc, guidlocESD);
            UtVec3d::Set(mLastGuidVel, guidvESD);
         }
      }
      else if(mSeekerOn == true){
         update = true;
         if(!mSeekerMethodTruth)
         {
            const WsfTrack* trackPtr = GetTargetTrack();
            if(trackPtr != 0 && HasTrackUpdated(trackPtr))
            {
               GetLocationESD(guidlocESD, trackPtr, aSimTime);
               GetVelocityESD(guidvESD, trackPtr);
               UtVec3d::Set(mLastGuidLoc, guidlocESD);
               UtVec3d::Set(mLastGuidVel, guidvESD);
            }
            else{update=false;}
         }
         else
         {
            UtVec3d::Set(guidlocESD, tgtlocESD);
            UtVec3d::Set(guidvESD, tgtvESD);
            UtVec3d::Set(mLastGuidLoc, guidlocESD);
            UtVec3d::Set(mLastGuidVel, guidvESD);
         }
      }
      else{update = false;}

      double lastGuidLoc[3];
      double lastGuidVel[3];
      UtVec3d::Set(lastGuidLoc, mLastGuidLoc);
      UtVec3d::Set(lastGuidVel, mLastGuidVel);
      mMissile.SetDynamicData(tgtlocESD,tgtvESD,update,lastGuidLoc,lastGuidVel,true);
   }
}

//pass in this instead of just using member variable for more control
//This function isn't always called using the member variable
void WsfARGO8_Mover::SetRailData(bool aTruthGuidance, double aSimTime)
{
   WsfPlatform* shooter = GetFiringPlatform();
   double phi, theta, psi;
   double lnch_a[3] = {0,0,0}, offset[3] = {0,0,0}, rotrat[3] = {0,0,0}, tgtsize = 2;
   double tgtlocESD[3], tgtvESD[3], shtlocESD[3], shtvESD[3];

   GetVelocityESD(shtvESD,shooter);
   GetLocationESD(shtlocESD,shooter);

   if (!aTruthGuidance)
   {
      const WsfTrack* trackPtr = GetTargetTrack();
      if (trackPtr != 0)
      {
         GetLocationESD(tgtlocESD,trackPtr, aSimTime);
         GetVelocityESD(tgtvESD,trackPtr);
      }
      else
      {
         UtVec3d::Set(tgtlocESD,0,0,0);
         UtVec3d::Set(tgtvESD,0,0,0);
      }
   }
   else
   {
      WsfPlatform* target = GetTargetPlatform();
      GetLocationESD(tgtlocESD,target);
      GetVelocityESD(tgtvESD,target);
   }

   //Getting separation Euler Angles
   shooter->GetOrientationNED(psi,theta,phi);
   mMissile.SetRailData(true, tgtsize, tgtlocESD, tgtvESD, shtlocESD, shtvESD, phi, theta, psi - UtMath::cPI/2, lnch_a, rotrat, offset);
}

WsfPlatform* WsfARGO8_Mover::GetTargetPlatform()
{
   if (GetPlatform() != 0)
   {
      WsfWeaponEngagement *engagement = WsfWeaponEngagement::Find(*GetPlatform());
      if (engagement != 0)
      {
         return engagement->GetTargetPlatform();
      }
   }
   return 0;
}

WsfPlatform* WsfARGO8_Mover::GetFiringPlatform()
{
   if (GetPlatform() != 0)
   {
      WsfWeaponEngagement* engagement = WsfWeaponEngagement::Find(*GetPlatform());
      if (engagement != 0)
      {
         return engagement->GetFiringPlatform();
      }
   }
   return 0;
}

const WsfTrack* WsfARGO8_Mover::GetTargetTrack()
{
   if (GetPlatform() != 0)
   {
      WsfTrackId trackId = WsfWeaponEngagement::Find(*GetPlatform())->GetTargetTrackId();
      if (trackId.GetLocalTrackNumber() != 0)
      {
            return GetPlatform()->GetTrackManager().GetCurrentTarget();
      }
   }
   return 0;
}

void WsfARGO8_Mover::GetLocationESD(double aLocESD[3], WsfPlatform *aPlatformPtr)
{
   double locNED[3], LLA[3];
   aPlatformPtr->GetLocationLLA(LLA[0],LLA[1],LLA[2]);
   if (mEngagementState != cES_BOTHALIVE)
   {
      UtVec3d::Set(mLastTrackLocation, LLA);
   }
   mReference.ConvertLLAToNED(LLA[0],LLA[1],LLA[2], locNED);
   UtVec3d::Set(aLocESD, locNED[1]*UtMath::cFT_PER_M, -locNED[0]*UtMath::cFT_PER_M, -LLA[2]*UtMath::cFT_PER_M);
}
void WsfARGO8_Mover::GetVelocityESD(double aVelESD[3], WsfPlatform *aPlatformPtr)
{
   double velNED[3];
   aPlatformPtr->GetVelocityNED(velNED);
   UtVec3d::Set(aVelESD, velNED[1]*UtMath::cFT_PER_M, -velNED[0]*UtMath::cFT_PER_M, velNED[2]*UtMath::cFT_PER_M);
}
//Also serves as function that sets old track location
void WsfARGO8_Mover::GetLocationESD(double aLocESD[3], const WsfTrack *aTrackPtr, double aSimTime)
{
   double locNED[3], locWCS[3], LLA[3];
   aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, locWCS);
   mReference.ConvertWCSToLLA(locWCS,LLA[0],LLA[1],LLA[2]);
   UtVec3d::Set(mLastTrackLocation, LLA);
   mReference.ConvertLLAToNED(LLA[0],LLA[1],LLA[2], locNED);
   UtVec3d::Set(aLocESD, locNED[1]*UtMath::cFT_PER_M, -locNED[0]*UtMath::cFT_PER_M, -LLA[2]*UtMath::cFT_PER_M);
}
void WsfARGO8_Mover::GetVelocityESD(double aVelESD[3], const WsfTrack *aTrackPtr)
{
   double velWCS[3], velNED[3];
   aTrackPtr->GetVelocityWCS(velWCS);
   mReference.ConvertWCSVectorToNED(velNED, velWCS);
   UtVec3d::Set(aVelESD, velNED[1]*UtMath::cFT_PER_M, -velNED[0]*UtMath::cFT_PER_M, velNED[2]*UtMath::cFT_PER_M);
}

double WsfARGO8_Mover::GetEngagementDistance(double aSimTime,
                                             EngagementState aEngagementState)
{
   double msl2TgtDist;
   if(aEngagementState == cES_BOTHALIVE || aEngagementState == cES_DEADSHOOTER)
   {
      double mslESD[3], tgtESD[3];
      WsfPlatform* target = GetTargetPlatform();
      GetLocationESD(tgtESD,target);
      mMissile.GetLocationESD(mslESD);
      UtVec3d::Subtract(tgtESD, tgtESD, mslESD);
      msl2TgtDist = UtVec3d::Magnitude(tgtESD)*UtMath::cM_PER_FT;
      return msl2TgtDist;
   }
   else if(aEngagementState == cES_BOTHDEAD || aEngagementState == cES_DEADTARGET)
   {
      double TargetWCS[3], FramePos[3];
      WsfPlatform* missile = GetPlatform();
      UtEntity TargetLoc = UtEntity();
      const WsfTrack* trackPtr = GetTargetTrack();
      if (trackPtr != 0)
      {
         trackPtr->GetLocationWCS(TargetWCS);
         TargetLoc.SetLocationWCS(TargetWCS);
      }
      else
      {
         TargetLoc.SetLocationLLA(mLastTrackLocation[0],mLastTrackLocation[1],mLastTrackLocation[2]);
      }
      TargetLoc.GetRelativeLocationWCS(missile, FramePos);
      msl2TgtDist = UtVec3d::Magnitude(FramePos);
      return msl2TgtDist;
   }
   return 0;
}

bool WsfARGO8_Mover::HasTrackUpdated(const WsfTrack* aTrackPtr)
{
   double guidlocLLA[3];
   aTrackPtr->GetLocationLLA(guidlocLLA[0],guidlocLLA[1],guidlocLLA[2]);
   if (!UtVec3d::Equals(guidlocLLA,mLastTrackLocation))
   {
      return true;
   }
   return false;
}

void WsfARGO8_Mover::TerminateMissile(double aSimTime,
                                     int    aFuzeState,
                                     double aMissDistance)
{
   if (mTerminationCompleted) return;                 // Only do this processing once...
   mTerminationCompleted = true;                      // ...
   std::string reasonStr;
   double gAvail;
   mMissile.GetgAvail(gAvail);
   if ((aFuzeState & cFS_PROXFUZEACTIVATED) != 0)
   {
      reasonStr += " Missile_ProxFuzeActivated";
   }
   if ((aFuzeState & cFS_SELFDESTRUCT) != 0)
   {
      reasonStr += " Missile_SelfDestruct";
   }
   if ((aFuzeState & cFS_FAILURE_GIMBALLIMIT) != 0)
   {
      reasonStr += " MissileFailure_GimbalLimit";
   }
   if ((aFuzeState & cFS_FAILURE_LOWMACH) != 0)
   {
      reasonStr += " MissileFailure_LowMach";
   }
   if ((aFuzeState & cFS_FAILURE_LOWCLOSINGVEL) != 0)
   {
      reasonStr += " MissileFailure_LowClosingVel";
   }
   if ((aFuzeState & cFS_LOWGAVAIL) != 0)
   {
      reasonStr += " MissileFailure_LowGsAvailable";
   }
   if ((aFuzeState & cFS_MAXTOFEXCEEDED) != 0)
   {
      reasonStr += " Missile_MaxTOFExceeded";
   }
   if ((aFuzeState & cFS_GROUNDIMPACT) != 0)
   {
      reasonStr += " Missile_GroundImpact";
   }
   if ((aFuzeState & cFS_DETONATE) != 0)
   {
      reasonStr += " Missile_Detonate";
   }
   if ((aFuzeState & cFS_FAILEDLAUNCH) != 0)
   {
      reasonStr += " Missile_FailedLaunch";
   }

   if (gAvail < mEndgameGees)
   {
      reasonStr += " Below_Endgame_gees";
      aMissDistance = 10000;
   }

   WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*GetPlatform());
   if (engagementPtr != 0)
   {
      engagementPtr->SetMissDistance(aMissDistance);
      //engagementPtr->SetEndgameGees(gAvail);
      if(mFuzeOutput)
      {
         std::cout << "T=" << aSimTime << ' ' << GetPlatform()->GetName() << ", fuze state "
                   << aFuzeState << ", " << reasonStr << ", Distance from Target: "
                   << aMissDistance << " meters" << std::endl;
      }
      engagementPtr->SetExtendedResult(reasonStr);

      if ((aFuzeState & cFS_DETONATE) != 0)
      {
         if ((aFuzeState & cFS_PROXFUZEACTIVATED) != 0 && gAvail >= mEndgameGees)
         {
            engagementPtr->Terminate(aSimTime, WsfWeaponEngagement::cTARGET_PROXIMITY_AIR_BURST);
         }
         else
         {
            engagementPtr->Terminate(aSimTime, WsfWeaponEngagement::cFAR_AWAY_GROUND_IMPACT);
         }
      }
      else
      {
         engagementPtr->Terminate(aSimTime, WsfWeaponEngagement::cFAR_AWAY_IN_AIR);
      }
   }
   else
   {
      // We should never get here!!!
      std::cout << "***** ERROR: T=" << aSimTime << ' ' << GetPlatform()->GetName()
                << "; ARGO object terminated but there was no engagement data!!!!" << std::endl;
      GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
   }
}
