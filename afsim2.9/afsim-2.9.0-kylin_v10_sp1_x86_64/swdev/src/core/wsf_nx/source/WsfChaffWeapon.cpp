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
#include "WsfChaffWeapon.hpp"

#include "UtLog.hpp"
#include "WsfChaffCloud.hpp"
#include "WsfChaffMover.hpp"
#include "WsfChaffRadarSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSignature.hpp"
#include "WsfScenario.hpp"
#include "WsfSignatureList.hpp"

// =================================================================================================
WsfChaffWeapon::WsfChaffWeapon(WsfScenario& aScenario)
   : WsfExplicitWeapon(aScenario)
   , mNextCloudId(0)
   , mCloudMap()
   , mCloudUpdateInterval(0.0)
   , mDrawCloudApproximations(false)
{
}

// =================================================================================================
// virtual
bool WsfChaffWeapon::DropChaffCloud(double aSimTime, ChaffOptions& aOptions)
{
   bool ok            = true;
   aOptions.mWeaponId = ++mNextCloudId;

   auto logger = ut::log::debug();
   if (DebugEnabled())
   {
      logger << "Requesting chaff cloud drop.";
      logger.AddNote() << "T = " << aSimTime;
      logger.AddNote() << "Platform: " << GetPlatform()->GetName();
      logger.AddNote() << "Weapon: " << GetName();
   }

   // Schedule a salvo for each ejector in list
   for (const auto& ejector : aOptions.mEjectors)
   {
      aOptions.mWeaponComponentName = ejector;

      WsfChaffEjector* ejectorPtr = GetEjectorByName(ejector);
      if (!ejectorPtr)
      {
         if (DebugEnabled())
         {
            logger.AddNote() << "Undefined Ejector: " << ejector;
         }

         ok = false;
      }
      else if (!WsfWeapon::FireSalvo(aSimTime, FireTarget(), aOptions))
      {
         if (DebugEnabled())
         {
            logger.AddNote() << "Unable to salvo from ejector: " << ejector;
         }

         ok = false;
      }
   }

   return ok;
}

// =================================================================================================
//! Standard Clone() method.
// virtual
WsfWeapon* WsfChaffWeapon::Clone() const
{
   return new WsfChaffWeapon(*this);
}

// =================================================================================================
// virtual
bool WsfChaffWeapon::Initialize(double aSimTime)
{
   // If launched platform type is not defined, set it to a default WSF_PLATFORM
   if (!GetLaunchedPlatformType())
   {
      SetLaunchedPlatformType("WSF_PLATFORM");
   }

   // Make sure at least one ejector is defined and initialize quantity remaining
   unsigned int ejectorCount = GetComponents().GetComponentCount<WsfChaffEjector>();
   if (ejectorCount == 0)
   {
      auto logger = ut::log::error() << "No chaff ejectors defined!";
      logger.AddNote() << "Platform: " << GetPlatform()->GetName();
      logger.AddNote() << "Weapon: " << GetName();
      return false;
   }

   unsigned int quantity = 0;
   for (unsigned int ejector = 0; ejector < ejectorCount; ++ejector)
   {
      quantity += GetComponents().GetComponentEntry<WsfChaffEjector>(ejector)->GetQuantityRemaining();
   }
   SetQuantityRemaining(quantity);

   // Make sure no mover or radar signature is defined on the launched platform type. These will be added after the
   // chaff is released in WeaponPlatformUserInitialize.
   WsfPlatform* platformPtr =
      static_cast<WsfPlatform*>(GetScenario().FindType("platform_type", GetLaunchedPlatformType()));
   if (platformPtr->GetMover())
   {
      auto logger = ut::log::error() << "Launched platform type, from chaff weapon, cannot have mover defined.";
      logger.AddNote() << "Launched Platform Type: " << GetLaunchedPlatformType();
      logger.AddNote() << "Platform: " << GetPlatform()->GetName();
      logger.AddNote() << "Weapon: " << GetName();
      return false;
   }

   auto* radarSigInterfacePtr = platformPtr->GetSignatureList().GetInterface(WsfRadarSignature::cSIGNATURE_INDEX);
   std::unique_ptr<WsfSignature> radarSigPtr(radarSigInterfacePtr->CloneSignature(GetScenario()));
   if (radarSigPtr)
   {
      auto logger =
         ut::log::error() << "Launched platform type, from chaff weapon, cannot have radar_signature defined.";
      logger.AddNote() << "Launched Platform Type: " << GetLaunchedPlatformType();
      logger.AddNote() << "Platform: " << GetPlatform()->GetName();
      logger.AddNote() << "Weapon: " << GetName();
      return false;
   }

   // Allow weapon to eject chaff from all ejectors simultaneously and prevent a cloud from being dropped until previous
   // cloud is complete
   SetMaximumRequestCount(ejectorCount);

   return WsfExplicitWeapon::Initialize(aSimTime);
   ;
}

bool WsfChaffWeapon::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "cloud_update_interval")
   {
      aInput.ReadValueOfType(mCloudUpdateInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mCloudUpdateInterval, 0.0);
   }
   else if (command == "draw_cloud_approximations")
   {
      aInput.ReadValue(mDrawCloudApproximations);
   }
   else
   {
      myCommand = WsfExplicitWeapon::ProcessInput(aInput);
   }
   return myCommand;
}

WsfChaffEjector* WsfChaffWeapon::GetEjectorByName(WsfStringId aEjectorName) const
{
   return GetComponents().GetComponent<WsfChaffEjector>(aEjectorName);
}

const char* WsfChaffWeapon::GetScriptClassName() const
{
   return "WsfChaffWeapon";
}

WsfWeapon::FireResult WsfChaffWeapon::Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings)
{
   WsfChaffEjector* ejectorPtr = GetEjectorByName(aSettings.mWeaponComponentName);

   if (!ejectorPtr)
   {
      if (DebugEnabled())
      {
         auto logger = ut::log::debug() << "Unable to dispense chaff from undefined ejector.";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << GetPlatform()->GetName();
         logger.AddNote() << "Weapon: " << GetName();
         logger.AddNote() << "Ejector: " << aSettings.mWeaponComponentName;
      }
      return false;
   }

   if (ejectorPtr->GetQuantityRemaining() == 0)
   {
      if (DebugEnabled())
      {
         auto logger = ut::log::debug() << "Unable to dispense chaff from ejector because no parcels remain.";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << GetPlatform()->GetName();
         logger.AddNote() << "Weapon: " << GetName();
         logger.AddNote() << "Ejector: " << ejectorPtr->GetName();
      }
      return false;
   }

   return WsfExplicitWeapon::Fire(aSimTime, aTarget, aSettings);
}


bool WsfChaffWeapon::AddWeaponToSimulation(double             aSimTime,
                                           WsfPlatform*       aWeaponPlatformPtr,
                                           const WsfTrack*    aLaunchingTrackPtr,
                                           const FireOptions& aSettings)
{
   auto* ejectorPtr = GetEjectorByName(aSettings.mWeaponComponentName);
   if (ejectorPtr) // Ejector should be valid because method is protected and called internally through Fire
   {
      // Create parcel of requested type
      auto parcelUP = ejectorPtr->EjectParcel(aSimTime);
      if (parcelUP)
      {
         // Add parcel as component to weapon platform (yielding ownership)
         auto* parcelPtr = parcelUP.release();
         if (aWeaponPlatformPtr->AddComponent(parcelPtr))
         {
            // Add chaff radar signature to platform
            WsfRadarSignature::SetSignature(aWeaponPlatformPtr, new WsfChaffRadarSignature);
            // Add weapon platform to simulation, which will initialize the platform and the parcel component
            if (WsfExplicitWeapon::AddWeaponToSimulation(aSimTime, aWeaponPlatformPtr, aLaunchingTrackPtr, aSettings))
            {
               // Find cloud that parcel is being added to, or create one if it doesn't exist yet...
               int  cloudId = aSettings.mWeaponId;
               auto it      = mCloudMap.find(cloudId);
               if (it == mCloudMap.end())
               {
                  it = mCloudMap
                          .emplace(cloudId, std::make_shared<WsfChaffCloud>(*GetSimulation(), mDrawCloudApproximations))
                          .first;
               }
               auto& cloudPtr = it->second;

               // ... and add parcel to the cloud
               parcelPtr->AddToCloud(cloudPtr);
               cloudPtr->AddParcel(parcelPtr);

               return true;
            }
         }
         else // Add component failed so caller retains ownership
         {
            delete parcelPtr;
         }
      }
   }

   return false;
}

double WsfChaffWeapon::GetQuantityRemaining() const
{
   return GetInitialStateQuantityRemaining() - GetTotalQuantityUsed();
}

void WsfChaffWeapon::SetLaunchState(double aSimTime, WsfPlatform* aPlatformPtr, const FireOptions& aOptions)
{
   auto* ejectorPtr = GetEjectorByName(aOptions.mWeaponComponentName);
   if (ejectorPtr) // Ejector should be valid because method is protected and called internally through Fire
   {
      ejectorPtr->SetLaunchState(aSimTime, aPlatformPtr);
   }
}

bool WsfChaffWeapon::WeaponPlatformUserInitialize(double aSimTime, WsfPlatform* aWeaponPlatformPtr, const WsfTrack* aTrackPtr)
{
   // Set the spatial domain of weapon platform to that of the dispensing platform.
   aWeaponPlatformPtr->SetSpatialDomain(GetPlatform()->GetSpatialDomain());

   // Give weapon platform an icon if it doesn't have one already
   if (aWeaponPlatformPtr->GetIconId().Empty())
   {
      aWeaponPlatformPtr->SetIcon("cloud");
   }

   // Add chaff mover to platform and set update interval
   WsfChaffMover* moverPtr = new WsfChaffMover(GetScenario());
   moverPtr->SetUpdateInterval(mCloudUpdateInterval);
   aWeaponPlatformPtr->SetMover(moverPtr);

   return WsfExplicitWeapon::WeaponPlatformUserInitialize(aSimTime, aWeaponPlatformPtr, aTrackPtr);
}
