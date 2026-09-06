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

#include "script/WsfScriptTowedMoverClass.hpp"

#include <iostream>
#include <map>

#include "UtMemory.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfStringId.hpp"
#include "WsfTowedMover.hpp"
#include "script/WsfScriptContext.hpp"

WsfScriptTowedMoverClass::WsfScriptTowedMoverClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMoverClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTowedMover");

   AddMethod(ut::make_unique<SetTowLength>());

   AddMethod(ut::make_unique<SetStartReelInTimeAbsolute>());

   AddMethod(ut::make_unique<SetStartReelInTimeRelative>());

   AddMethod(ut::make_unique<SetReelInTimeAfterDeploymentRelative>());

   AddMethod(ut::make_unique<SetReelInRate>());

   AddMethod(ut::make_unique<SetReelOutRate>());

   AddMethod(ut::make_unique<State>());
   AddMethod(ut::make_unique<State_1>("State"));

   AddMethod(ut::make_unique<ReelInRate>());
   AddMethod(ut::make_unique<ReelInRate_1>("ReelInRate"));

   AddMethod(ut::make_unique<ReelOutRate>());
   AddMethod(ut::make_unique<ReelOutRate_1>("ReelOutRate"));

   AddMethod(ut::make_unique<StartReelInTime>());
   AddMethod(ut::make_unique<StartReelInTime_1>("StartReelInTime"));

   AddMethod(ut::make_unique<ReelInTimeAfterDeploymentRelative>());
   AddMethod(ut::make_unique<ReelInTimeAfterDeploymentRelative_1>("ReelInTimeAfterDeploymentRelative"));

   AddMethod(ut::make_unique<Distance>());

   // AddMethod(ut::make_unique<Help>());
   // AddMethod(ut::make_unique<Help_1>("Help"));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, SetTowLength, 1, "void", "double")
{
   // Argument 0: Absolute simulation time to start reeling in the towed asset.
   double reelInTime = aVarArgs[0].GetDouble();

   aObjectPtr->SetTowLength(reelInTime);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, SetStartReelInTimeAbsolute, 1, "void", "double")
{
   // Argument 0: Absolute simulation time to start reeling in the towed asset.
   double reelInTime = aVarArgs[0].GetDouble();

   aObjectPtr->SetReelInTime(reelInTime);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, SetStartReelInTimeRelative, 1, "void", "double")
{
   // Argument 0: Relative simulation time to start reeling in the towed asset.
   double simTime            = WsfScriptContext::GetTIME_NOW(aContext);
   double relativeReelInTime = aVarArgs[0].GetDouble();

   aObjectPtr->SetReelInTime((simTime + relativeReelInTime));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, SetReelInTimeAfterDeploymentRelative, 1, "void", "double")
{
   // Argument 0: Relative simulation time to start reeling in the towed asset.
   double relativeReelInTime = aVarArgs[0].GetDouble();

   aObjectPtr->SetReelInTimeAfterDeployment(relativeReelInTime);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, SetReelInRate, 1, "void", "double")
{
   // Argument 0: Set the reel in rate units MPS.
   double reelInRate = aVarArgs[0].GetDouble();

   aObjectPtr->SetReelInRate(reelInRate);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, SetReelOutRate, 1, "void", "double")
{
   // Argument 0: Set the reel in rate units MPS.
   double reelOutRate = aVarArgs[0].GetDouble();

   aObjectPtr->SetReelOutRate(reelOutRate);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, State, 0, "string", "")
{
   // Argument 0: Set the reel in rate units MPS.
   WsfTowedMover::DeploymentState deploymentState       = aObjectPtr->GetState();
   std::string                    deploymentStateString = "STOWED";

   switch (deploymentState)
   {
   case WsfTowedMover::cREELING_OUT:
   {
      deploymentStateString = "REELING_OUT";
   }
   break;
   case WsfTowedMover::cDEPLOYED:
   {
      deploymentStateString = "DEPLOYED";
   }
   break;
   case WsfTowedMover::cREELING_IN:
   {
      deploymentStateString = "REELING_IN";
   }
   default:
      break;
   }
   aReturnVal.SetString(deploymentStateString);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, State_1, 1, "string", "WsfPlatform")
{
   std::string deploymentStateString = "STOWED";
   // the platform is the towed asset in this case, so it has a WsfTowedMover type
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   if (platformPtr != nullptr)
   {
      WsfMover*      moverPtr      = platformPtr->GetMover();
      WsfTowedMover* towedMoverPtr = dynamic_cast<WsfTowedMover*>(moverPtr);
      if (towedMoverPtr != nullptr)
      {
         // Argument 0: Set the reel in rate units MPS.
         WsfTowedMover::DeploymentState deploymentState = aObjectPtr->GetState();

         switch (deploymentState)
         {
         case WsfTowedMover::cREELING_OUT:
         {
            deploymentStateString = "REELING_OUT";
         }
         break;
         case WsfTowedMover::cDEPLOYED:
         {
            deploymentStateString = "DEPLOYED";
         }
         break;
         case WsfTowedMover::cREELING_IN:
         {
            deploymentStateString = "REELING_IN";
         }
         default:
            break;
         }
      }
      else
      {
         deploymentStateString = "UNKNOWN";
      }
   }
   else
   {
      deploymentStateString = "UNKNOWN";
   }
   WsfStringId deploymentStateId(deploymentStateString);
   aReturnVal.SetString(deploymentStateId);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, StartReelInTime, 0, "double", "")
{
   double startReelInTimeSeconds = aObjectPtr->GetReelInTimeAbsolute();
   aReturnVal.SetDouble(startReelInTimeSeconds);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, StartReelInTime_1, 1, "double", "WsfPlatform")
{
   double startReelInTimeSeconds = -1.0;
   // the platform is the towed asset in this case, so it has a WsfTowedMover type
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   if (platformPtr != nullptr)
   {
      WsfMover*      moverPtr      = platformPtr->GetMover();
      WsfTowedMover* towedMoverPtr = dynamic_cast<WsfTowedMover*>(moverPtr);
      if (towedMoverPtr != nullptr)
      {
         if (towedMoverPtr->GetReelInTimeAbsolute() > 0.0)
         {
            startReelInTimeSeconds = towedMoverPtr->GetReelInTimeAbsolute();
         }
      }
   }

   aReturnVal.SetDouble(startReelInTimeSeconds);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, ReelInTimeAfterDeploymentRelative, 0, "double", "")
{
   double reelInTimeSeconds = aObjectPtr->GetReelInTimeAfterDeployment();
   aReturnVal.SetDouble(reelInTimeSeconds);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, ReelInTimeAfterDeploymentRelative_1, 1, "double", "WsfPlatform")
{
   double reelInTimeSeconds = -1.0;
   // the platform is the towed asset in this case, so it has a WsfTowedMover type
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   if (platformPtr != nullptr)
   {
      WsfMover*      moverPtr      = platformPtr->GetMover();
      WsfTowedMover* towedMoverPtr = dynamic_cast<WsfTowedMover*>(moverPtr);
      if (towedMoverPtr != nullptr)
      {
         if (towedMoverPtr->GetReelInTimeAbsolute() > 0.0)
         {
            reelInTimeSeconds = towedMoverPtr->GetReelInTimeAfterDeployment();
         }
      }
   }

   aReturnVal.SetDouble(reelInTimeSeconds);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, ReelInRate, 0, "double", "")
{
   double reelInRate = -1.0;
   if (aObjectPtr->GetReelInRate() > 0.0)
   {
      reelInRate = aObjectPtr->GetReelInRate();
   }

   aReturnVal.SetDouble(reelInRate);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, ReelInRate_1, 1, "double", "WsfPlatform")
{
   double reelInRate = -1.0;
   // the platform is the towed asset in this case, so it has a WsfTowedMover type
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   if (platformPtr != nullptr)
   {
      WsfMover*      moverPtr      = platformPtr->GetMover();
      WsfTowedMover* towedMoverPtr = dynamic_cast<WsfTowedMover*>(moverPtr);
      if (towedMoverPtr != nullptr)
      {
         if (towedMoverPtr->GetReelInRate() > 0.0)
         {
            reelInRate = towedMoverPtr->GetReelInRate();
         }
      }
   }

   aReturnVal.SetDouble(reelInRate);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, ReelOutRate, 0, "double", "")
{
   double reelOutRate = -1.0;
   if (aObjectPtr->GetReelOutRate() > 0.0)
   {
      reelOutRate = aObjectPtr->GetReelOutRate();
   }

   aReturnVal.SetDouble(reelOutRate);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, ReelOutRate_1, 1, "double", "WsfPlatform")
{
   double reelOutRate = -1.0;
   // the platform is the towed asset in this case, so it has a WsfTowedMover type
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   if (platformPtr != nullptr)
   {
      WsfMover*      moverPtr      = platformPtr->GetMover();
      WsfTowedMover* towedMoverPtr = dynamic_cast<WsfTowedMover*>(moverPtr);
      if (towedMoverPtr != nullptr)
      {
         if (towedMoverPtr->GetReelOutRate() > 0.0)
         {
            reelOutRate = towedMoverPtr->GetReelOutRate();
         }
      }
   }

   aReturnVal.SetDouble(reelOutRate);
}

// Returns distance between towed platform and its parent platform (meters)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTowedMoverClass, WsfTowedMover, Distance, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDistanceMeters());
}
