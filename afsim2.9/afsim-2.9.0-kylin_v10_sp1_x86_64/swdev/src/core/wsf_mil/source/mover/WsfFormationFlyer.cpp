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

#include "WsfFormationFlyer.hpp"

#include <cassert>
#include <iostream>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

//#define DEBUG_ON
#ifdef DEBUG_ON
#define DIAG_ON
#define DO_DEBUG(A) A
#else
#define DO_DEBUG(A) /* A */
#endif

//#define DIAG_ON
#ifdef DIAG_ON
#define DO_DIAG(A) A
#else
#define DO_DIAG(A) /* A */
#endif

/* for debug only...
namespace {

   WsfFormationFlyer* debuggedMoverPtr = 0;
   double cumDT    = 0.0;

   bool SetMe(WsfFormationFlyer* me)
   {
      bool result = false;
      if (debuggedMoverPtr == 0)
      {
         debuggedMoverPtr = me;
         cumDT = 0.0;
         result = true;
         //cout << "Platform of interest was set to " << me->GetPlatform()->GetName() << endl;
      }
      return result;
   }

   bool ClearMe(WsfFormationFlyer* me)
   {
      bool result = false;
      if (debuggedMoverPtr == me)
      {
         result = true;
         debuggedMoverPtr = 0;
         //cout << "CumDT is = " << cumDT << endl;
         //cout << "(Platform of interest was) " << me->GetPlatform()->GetName() << endl;
      }
      return result;
   }

   bool IsMe(WsfFormationFlyer* me)
   {
      return debuggedMoverPtr == me;
   }
} */

WsfFormationFlyer::WsfFormationFlyer(WsfScenario& aScenario)
   : WsfFollower(aScenario)
   , mLeadNameId(nullptr)
   , mLeadIndex(0)
   , mLeadPlatPtr(nullptr)
   , mFormationRolls(false)
   , mNegateLateralOffset(false)
   , mInitializeAtOffset(false)
{
   UtVec3d::Set(mOffsetFromLeadECS, 0.0);
   mThreadSafe = false;
}

WsfFormationFlyer::WsfFormationFlyer(const WsfFormationFlyer& aSrc)
   : WsfFollower(aSrc)
   , mLeadNameId(aSrc.mLeadNameId)
   , mLeadIndex(aSrc.mLeadIndex)
   , mLeadPlatPtr(aSrc.mLeadPlatPtr)
   , mFormationRolls(aSrc.mFormationRolls)
   , mNegateLateralOffset(aSrc.mNegateLateralOffset)
   , mInitializeAtOffset(aSrc.mInitializeAtOffset)
{
   // We only allow the cloning of non-initialized movers.
   assert(!IsInitialized());

   UtVec3d::Set(mOffsetFromLeadECS, aSrc.mOffsetFromLeadECS);
}

// Clone this mover (used to create an instance from a type).
// virtual
WsfMover* WsfFormationFlyer::Clone() const
{
   return new WsfFormationFlyer(*this);
}
//
// // ============================================================================
// void WsfFormationFlyer::SetLeadAircraft(int aLeadNameId)
// {
//    WsfFollower::DontFollowThis();  // temporary, until a new Update() occurs.
//    mLeadNameId  = aLeadNameId;
//    mLeadIndex   = 0;
//    mLeadPlatPtr = GetSimulation()->GetPlatformByName(mLeadNameId);
//    if (mLeadPlatPtr != 0)
//    {
//       mLeadIndex = mLeadPlatPtr->GetIndex();
//    }
// }

// ============================================================================
void WsfFormationFlyer::SetLeadAircraft(WsfPlatform* aPlatformPtr)
{
   WsfFollower::DontFollowThis(); // temporary, until a new Update() occurs.
   mLeadPlatPtr = aPlatformPtr;
   mLeadNameId  = aPlatformPtr->GetNameId();
   mLeadIndex   = aPlatformPtr->GetIndex();
}

// ============================================================================
void WsfFormationFlyer::SetOffsetFromLeadECS(const double aOffsetFromLeadECS[3])
{
   UtVec3d::Set(mOffsetFromLeadECS, aOffsetFromLeadECS);
   mNegateLateralOffset = false; // Reset to false
}

// ============================================================================
void WsfFormationFlyer::GetOffsetFromLeadECS(double aOffsetFromLeadECS[3]) const
{
   UtVec3d::Set(aOffsetFromLeadECS, mOffsetFromLeadECS);

   if (mNegateLateralOffset)
   {
      aOffsetFromLeadECS[1] *= -1.0;
   }
}

// ============================================================================
const char* WsfFormationFlyer::GetScriptClassName() const
{
   return "WsfFormationFlyer";
}

// ============================================================================
bool WsfFormationFlyer::Initialize(double aSimTime)
{
   assert(GetPlatform() != nullptr);
   if (mLeadNameId != 0)
   {
      mLeadPlatPtr = GetSimulation()->GetPlatformByName(mLeadNameId);
      if (mLeadPlatPtr != nullptr)
      {
         // Get the index of the lead platform
         mLeadIndex = mLeadPlatPtr->GetIndex();

         if (mInitializeAtOffset)
         {
            // Determine our position and orientation in the formation
            // based on our formation offset and the lead platform's
            // position and orientation.
            double locWCS[3] = {0.0};
            mLeadPlatPtr->GetLocationWCS(locWCS);

            // Get the orientation of the lead platform
            double heading(0.0);
            double pitch(0.0);
            double roll(0.0);
            mLeadPlatPtr->GetOrientationNED(heading, pitch, roll);

            // Get the ECS offset and negate the lateral
            // value if necessary
            double offset[3] = {0.0};
            UtVec3d::Set(offset, mOffsetFromLeadECS);
            if (mNegateLateralOffset)
            {
               offset[1] *= -1.0;
            }

            // Find the offset in the NED frame; assume zero pitch and roll; only use heading
            double offsetNED[3] = {0.0};
            offsetNED[0]        = ::cos(heading) * offset[0] - ::sin(heading) * offset[1];
            offsetNED[1]        = ::sin(heading) * offset[0] + ::cos(heading) * offset[1];
            offsetNED[2]        = offset[2];

            // Convert from WCS to LLA
            double locLLA[3] = {0.0};
            mLeadPlatPtr->ConvertNEDToLLA(offsetNED, locLLA[0], locLLA[1], locLLA[2]);

            // Set the location, speed, and heading of this platform
            SetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
            SetSpeed(mLeadPlatPtr->GetSpeed());
            SetHeading(heading);
         }
      }
   }

   // Saved this return value so the output could be
   // printed after the mover is fully initialized
   bool initialized(WsfFollower::Initialize(aSimTime));
   DO_DIAG(OutputDiagnostic(aSimTime, mLeadPlatPtr));
   return initialized;
}

// ============================================================================
// virtual
bool WsfFormationFlyer::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());

   if (command == "lead_aircraft")
   {
      std::string leadName;
      aInput.ReadValue(leadName);
      mLeadNameId = leadName;
   }
   else if (command == "formation_rolls_with_lead")
   {
      mFormationRolls = true;
   }
   else if (command == "offset_forward_from_lead")
   {
      double offset;
      aInput.ReadValueOfType(offset, UtInput::cLENGTH);
      mOffsetFromLeadECS[0] = offset;
   }
   else if (command == "offset_right_from_lead")
   {
      double offset;
      aInput.ReadValueOfType(offset, UtInput::cLENGTH);
      mOffsetFromLeadECS[1] = offset;
   }
   else if (command == "offset_down_from_lead")
   {
      double offset;
      aInput.ReadValueOfType(offset, UtInput::cLENGTH);
      mOffsetFromLeadECS[2] = offset;
   }
   else if (command == "initialize_at_offset")
   {
      mInitializeAtOffset = true;
   }
   else
   {
      myCommand = WsfFollower::ProcessInput(aInput);
   }

   return myCommand;
}

// ============================================================================
// virtual
void WsfFormationFlyer::Update(double aSimTime)
{
   assert(GetPlatform() != nullptr);
   assert(IsInitialized());

   // The first job of this class is to see if the Lead aircraft
   // is present and Updated to current location.  If so, refresh
   // the "follow" position and velocity in the base class:
   UpdateLeadInfo(aSimTime);

   // Request the base class to do its thing...
   WsfFollower::Update(aSimTime);
}

// ============================================================================
void WsfFormationFlyer::UpdateLeadInfo(double aSimTime)
{
   if (mLeadIndex != 0)
   {
      if (GetSimulation()->PlatformExists(mLeadIndex))
      {
         // The Lead aircraft (continues to be) present:
         UpdateLeadPosAndVel(aSimTime);
      }
      else
      {
         // Lead aircraft disappeared:
         mLeadNameId  = nullptr;
         mLeadIndex   = 0;
         mLeadPlatPtr = nullptr;
         DontFollowThis();
      }
   }
   else if (mLeadNameId != 0)
   {
      mLeadPlatPtr = GetSimulation()->GetPlatformByName(mLeadNameId);
      if (mLeadPlatPtr != nullptr)
      {
         // The Lead aircraft just appeared:
         mLeadIndex = mLeadPlatPtr->GetIndex();
         UpdateLeadPosAndVel(aSimTime);
      }
   }
}

// ============================================================================
void WsfFormationFlyer::UpdateLeadPosAndVel(double aSimTime)
{
   // Update the position of the lead platform
   mLeadPlatPtr->Update(aSimTime);

   // Get the velocity vector of the lead platform
   double velNED[3] = {0.0};
   mLeadPlatPtr->GetVelocityNED(velNED);

   // Get the ECS offset and negate the lateral
   // value if necessary
   double offset[3] = {0.0};
   UtVec3d::Set(offset, mOffsetFromLeadECS);
   if (mNegateLateralOffset)
   {
      offset[1] *= -1.0;
   }

   // If the formation rolls with the lead aircraft
   double resultWCS[3] = {0.0};
   if (mFormationRolls)
   {
      // Get the lead platform's location in the WCS frame
      double platformWCS[3] = {0.0};
      mLeadPlatPtr->GetLocationWCS(platformWCS);

      // Get the offset as a WCS vector
      double deltaWCS[3] = {0.0};
      mLeadPlatPtr->ConvertECSVectorToWCS(deltaWCS, offset);

      // Find the follower's location in the WCS frame
      UtVec3d::Add(resultWCS, platformWCS, deltaWCS);
   }
   else
   {
      // Find the offset in the NED frame
      double heading      = ::atan2(velNED[1], velNED[0]);
      double offsetNED[3] = {0.0};
      offsetNED[0]        = ::cos(heading) * offset[0] - ::sin(heading) * offset[1];
      offsetNED[1]        = ::sin(heading) * offset[0] + ::cos(heading) * offset[1];
      offsetNED[2]        = offset[2];
      mLeadPlatPtr->ConvertNEDToWCS(offsetNED, resultWCS);
   }

   DO_DIAG(OutputDiagnostic(aSimTime, mLeadPlatPtr));
   WsfFollower::FollowThis(aSimTime, resultWCS, velNED);
}

// ============================================================================
#ifdef DIAG_ON

void WsfFormationFlyer::OutputDiagnostic(double aSimTime, WsfPlatform* aleadPlatPtr)
{
   cout << endl << "T=" << aSimTime << " Formation Flyer" << endl;

   // Lead location
   if (aleadPlatPtr != 0)
   {
      cout << "== Lead [" << aleadPlatPtr->GetName() << "]" << endl;
      PlatformDiagnostic(aleadPlatPtr);
   }

   cout << "== Offset From Lead" << endl;
   OutputVec3d("   Location    ECS [m]    =", mOffsetFromLeadECS);

   cout << "== Follower [" << GetPlatform()->GetName() << "]" << endl;
   PlatformDiagnostic(GetPlatform());
}

void WsfFormationFlyer::PlatformDiagnostic(WsfPlatform* aPlatformPtr)
{
   double dLocLLA[3] = {0.0};
   aPlatformPtr->GetLocationLLA(dLocLLA[0], dLocLLA[1], dLocLLA[2]);
   OutputVec3d("   Location    LLA [d,m]  =", dLocLLA);

   double heading(0.0);
   double pitch(0.0);
   double roll(0.0);
   aPlatformPtr->GetOrientationNED(heading, pitch, roll);
   cout << "   Orientation NED [deg]  =";
   cout << " " << (heading * UtMath::cDEG_PER_RAD);
   cout << " " << (pitch * UtMath::cDEG_PER_RAD);
   cout << " " << (roll * UtMath::cDEG_PER_RAD);
   cout << endl;

   double dVelNED[3] = {0.0};
   aPlatformPtr->GetVelocityNED(dVelNED);
   OutputVec3d("   Velocity    NED [m]    =", dVelNED);

   double dVelECS[3] = {0.0};
   aPlatformPtr->GetVelocityECS(dVelECS);
   OutputVec3d("   Velocity    ECS [m]    =", dVelECS);

   double dAccECS[3] = {0.0};
   aPlatformPtr->GetAccelerationECS(dAccECS);
   OutputVec3d("   Accel       ECS [m/s^2]=", dAccECS);

   cout << "   Speed           [m/s]  = " << aPlatformPtr->GetSpeed() << endl;
}

#endif
