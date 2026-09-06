// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfAltLocComponent.hpp"

#include "UtEllipsoidalEarth.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLength.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "WsfAltLocSimulationExtension.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

namespace wsf
{
namespace altloc
{

Component::Component()
   : WsfObject()
   , WsfPlatformComponent()
   , mBlockType(BlockType::NONE)
   , mReferenceName()
   , mReferenceComponent(nullptr)
   , mOffset({})
   , mLocations()
   , mDraw(cINVALID_DRAW)
   , mWeightsSum(0.0)
   , mLocationIndex(cINVALID_LOCATION_INDEX)
   , mDebugType(DebugType::GLOBAL)
{
   SetName(cNAME);
}

Component::Component(const Component& aSrc)
   : WsfObject(aSrc)
   , WsfPlatformComponent(aSrc)
   , mBlockType(aSrc.mBlockType)
   , mReferenceName(aSrc.mReferenceName)
   , mReferenceComponent(aSrc.mReferenceComponent)
   , mOffset(aSrc.mOffset)
   , mLocations(aSrc.mLocations)
   , mDraw(aSrc.mDraw)
   , mWeightsSum(aSrc.mWeightsSum)
   , mLocationIndex(aSrc.mLocationIndex)
   , mDebugType(aSrc.mDebugType)
{
}

const int* Component::GetComponentRoles() const
{
   static const int roles[] = {cCOMPONENT, cWSF_COMPONENT_NULL};
   return roles;
}

// virtual
WsfComponent* Component::CloneComponent() const
{
   return new Component(*this);
}

// virtual
void* Component::QueryInterface(int aRole)
{
   if (aRole == cCOMPONENT)
   {
      return this;
   }
   else
   {
      return nullptr;
   }
}

// virtual
bool Component::ProcessInput(UtInput& aInput)
{
   std::string command;
   aInput.GetCommand(command);

   // 4. Start reading the command
   if (command == cCOMMAND)
   {
      Component* referenceComponent = nullptr;

      std::string  blockCommand;
      UtInputBlock block(aInput);

      // 4.1 Start reading commands
      while (block.ReadCommand(blockCommand))
      {
         if (blockCommand == "reference")
         {
            if (referenceComponent)
            {
               throw UtInput::BadValue(aInput,
                                       std::string{"Can not specify multiple reference platforms in an "} + cCOMMAND +
                                          " block");
            }

            if (mBlockType == BlockType::LOCAL)
            {
               mBlockType = BlockType::REF_LOC;
            }

            // 4.2 Add to Deferred input
            std::string platformName;
            aInput.ReadCommand(platformName);
            WsfScenario& scenario = WsfScenario::FromInput(aInput);
            scenario.GetDeferredInput().Requires("platform", platformName);

            WsfPlatform* platformPtr = scenario.GetInputPlatformByName(platformName);
            assert(platformPtr != nullptr);
            WsfPlatformComponentList& componentList = const_cast<WsfPlatformComponentList&>(platformPtr->GetComponents());
            referenceComponent                      = static_cast<Component*>(componentList.FindComponent(cNAME));
            if (!referenceComponent)
            {
               throw UtInput::BadValue(aInput,
                                       std::string{"Could not find an "} + cCOMMAND + " block on reference platform " +
                                          platformName);
            }

            if (referenceComponent->mReferenceName != WsfStringId())
            {
               throw UtInput::BadValue(
                  aInput,
                  std::string{"An "} + cCOMMAND +
                     " block cannot reference another platform which references another platform in its " + cCOMMAND +
                     " block");
            }

            for (auto const& location : mLocations)
            {
               if (location.wgtSet)
               {
                  throw UtInput::BadValue(
                     aInput,
                     "alternate_locations blocks which reference another platform may not include weighted locations.");
               }
            }

            mReferenceName = WsfStringId(platformName);
         }
         else if (blockCommand == "location")
         {
            if (mBlockType != BlockType::OFF_ARA && mBlockType != BlockType::OFF_LLA)
            {
               if (!referenceComponent)
               {
                  mBlockType = BlockType::LOCAL;
               }
               else
               {
                  mBlockType = BlockType::REF_LOC;
               }
               WeightedLocation loc;
               ReadAltLocLine(aInput, loc);
               mLocations.push_back(loc);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Locations cannot be mixed with offsets");
            }
         }
         else if (blockCommand == "offset_lla")
         {
            if (mBlockType == BlockType::NONE)
            {
               mBlockType = BlockType::OFF_LLA;
               aInput.ReadValueOfType(mOffset.LLA.lat, UtInput::cLATITUDE);
               aInput.ReadValueOfType(mOffset.LLA.lon, UtInput::cLONGITUDE);
               std::string offsetCommand;

               while (aInput.ReadCommand(offsetCommand))
               {
                  if (offsetCommand == "altitude")
                  {
                     aInput.ReadValueOfType(mOffset.LLA.alt, UtInput::cLENGTH);
                     mOffset.LLA.altSet = true;
                  }
                  else if (offsetCommand == "heading")
                  {
                     aInput.ReadValueOfType(mOffset.LLA.hed, UtInput::cANGLE);
                     mOffset.LLA.hedSet = true;
                  }
                  else
                  {
                     aInput.PushBack(offsetCommand);
                     break;
                  }
               }
            }
            else if (mBlockType == BlockType::OFF_ARA || mBlockType == BlockType::OFF_LLA)
            {
               throw UtInput::BadValue(aInput, "An alternate_locations block may only contain a single offset");
            }
            else
            {
               throw UtInput::BadValue(aInput, "Offsets cannot be mixed with locations");
            }
         }
         else if (blockCommand == "offset_ara")
         {
            if (mBlockType == BlockType::NONE)
            {
               mBlockType = BlockType::OFF_ARA;
               aInput.ReadValueOfType(mOffset.ARA.angle, UtInput::cANGLE);
               aInput.ReadValueOfType(mOffset.ARA.radius, UtInput::cLENGTH);
               std::string offsetCommand;

               while (aInput.ReadCommand(offsetCommand))
               {
                  if (offsetCommand == "altitude")
                  {
                     aInput.ReadValueOfType(mOffset.ARA.alt, UtInput::cLENGTH);
                     mOffset.ARA.altSet = true;
                  }
                  else if (offsetCommand == "heading")
                  {
                     aInput.ReadValueOfType(mOffset.ARA.hed, UtInput::cANGLE);
                     mOffset.ARA.hedSet = true;
                  }
                  else
                  {
                     aInput.PushBack(offsetCommand);
                     break;
                  }
               }
            }
            else if (mBlockType == BlockType::OFF_ARA || mBlockType == BlockType::OFF_LLA)
            {
               throw UtInput::BadValue(aInput, "An alternate_locations block may only contain a single offset");
            }
            else
            {
               throw UtInput::BadValue(aInput, "Offsets cannot be mixed with locations");
            }
         }
         else if (ReadPlatformDebugFlag(aInput))
         {
         }
         else
         {
            throw UtInput::BadValue(aInput);
         }
      }

      if (mBlockType == BlockType::NONE)
      {
         throw UtInput::BadValue(aInput, std::string{"An "} + cCOMMAND + " block cannot be empty");
      }
      else if (mBlockType != BlockType::OFF_ARA && mBlockType != BlockType::OFF_LLA)
      {
         if (!referenceComponent)
         {
            // 4.3 Validate total of weights
            bool weightSet = false;
            for (auto const& location : mLocations)
            {
               weightSet |= location.wgtSet;
            }
            if (!weightSet)
            {
               for (auto& location : mLocations)
               {
                  location.weight = 1.0 / mLocations.size();
               }
               mWeightsSum = 1.0;
            }
            else
            {
               for (const auto& location : mLocations)
               {
                  mWeightsSum += location.weight;
               }
            }
            if (mWeightsSum <= 0.0)
            {
               throw UtInput::BadValue(aInput,
                                       std::string{"The weights in an "} + cCOMMAND +
                                          " block must sum to a positive, nonzero value");
            }
         }
         else
         {
            if (mLocations.size() != referenceComponent->mLocations.size())
            {
               throw UtInput::BadValue(aInput,
                                       std::string{"The number of locations in this "} + cCOMMAND +
                                          " block must match the number of locations in the " + cCOMMAND +
                                          " block for referenced platform " + mReferenceName);
            }
            else
            {
               mWeightsSum = referenceComponent->mWeightsSum;
            }
         }
      }
      else if (!referenceComponent)
      {
         throw UtInput::BadValue(aInput,
                                 std::string{"An "} + cCOMMAND +
                                    " block which makes use of an offset must reference another platform");
      }
   }

   return true;
}

// ==========================================================================
//! Find the instance of this component attached to the specified platform.
Component* Component::Find(const WsfPlatform& aParent)
{
   Component* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<Component>(componentPtr);
   return componentPtr;
}

// ==========================================================================
//! Find the instance of this component attached to the specified platform,
//! and create it if it doesn't exist.
Component* Component::FindOrCreate(WsfPlatform& aParent)
{
   Component* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      componentPtr = new Component();
      const_cast<WsfPlatformComponentList&>(aParent.GetComponents()).AddComponent(componentPtr);
   }
   return componentPtr;
}

WsfObject* Component::Clone() const
{
   return new Component(*this);
}

WsfStringId Component::GetComponentName() const
{
   return GetNameId();
}

int Component::GetRandomLocation()
{
   if (mLocationIndex == cINVALID_LOCATION_INDEX)
   {
      assert(!mLocations.empty());

      // draw a random value to choose the location
      auto& extension = SimulationExtension::Get(*GetComponentParent()->GetSimulation());

      mDraw = extension.GetDraw() * mWeightsSum;

      // if the random draw is <= current weight sum (tSum), use i as the
      // index for the alt-location to use
      double tSum = 0.0;
      for (unsigned int i = 0; i < mLocations.size(); ++i)
      {
         tSum += mLocations[i].weight;
         if (mDraw <= tSum)
         {
            mLocationIndex = i;
            break;
         }
      }
      if (mLocationIndex == cINVALID_LOCATION_INDEX)
      {
         mLocationIndex = 0;
      }
   }

   return mLocationIndex;
}

bool Component::PreInitialize(double aSimTime)
{
   WsfPlatform*     platform   = GetComponentParent();
   WsfSimulation*   simulation = platform->GetSimulation();
   WeightedLocation location   = {};

   assert(mBlockType != BlockType::NONE);

   if (mBlockType == BlockType::LOCAL)
   {
      GetRandomLocation();
      if (mLocationIndex == cINVALID_LOCATION_INDEX)
      {
         return false;
      }
      location = mLocations[mLocationIndex];
   }
   else
   {
      auto referencePlatPtr = simulation->GetPlatformByName(mReferenceName);

      if (referencePlatPtr)
      {
         mReferenceComponent =
            static_cast<Component*>(referencePlatPtr->FindComponentByRole(cCOMPONENT_ROLE<Component>()));

         assert(mReferenceComponent);

         mLocationIndex = mReferenceComponent->GetRandomLocation();
      }
      else
      {
         // Parent platform does not exist. As it was checked for validity in ProcessInput this is likely due to
         // platform availability not instantiating the platform into the simulation.
         auto log = ut::log::error() << "Referenced platform: " << mReferenceName
                                     << " from alternate locations platform: " << platform->GetName()
                                     << " does not exist!";
         log.AddNote() << "This is often caused when the referenced platform is disabled by platform_availability.";

         return false;
      }

      if (mLocationIndex == cINVALID_LOCATION_INDEX)
      {
         return false;
      }
      mDraw = mReferenceComponent->mDraw;

      switch (mBlockType)
      {
      case BlockType::REF_LOC:
      {
         location = mLocations[mLocationIndex];
         break;
      }
      case BlockType::OFF_LLA:
      {
         location = mReferenceComponent->mLocations[mLocationIndex];
         location.lat += mOffset.LLA.lat;
         location.lon += mOffset.LLA.lon;
         if (mOffset.LLA.altSet)
         {
            location.alt += mOffset.LLA.alt;
            location.altSet = true;
         }
         if (mOffset.LLA.hedSet)
         {
            location.hed    = mOffset.LLA.hed;
            location.hedSet = true;
         }
         break;
      }
      case BlockType::OFF_ARA:
      {
         location = mReferenceComponent->mLocations[mLocationIndex];
         UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(location.lat,
                                                             location.lon,
                                                             (mOffset.ARA.angle * UtMath::cDEG_PER_RAD),
                                                             mOffset.ARA.radius,
                                                             location.lat,
                                                             location.lon);
         if (mOffset.ARA.altSet)
         {
            location.alt += mOffset.ARA.alt;
            location.altSet = true;
         }
         if (mOffset.ARA.hedSet)
         {
            location.hed    = mOffset.ARA.hed;
            location.hedSet = true;
         }
         break;
      }
      default:
         // RAII block
         {
            auto out = ut::log::fatal() << "Invalid BlockType.";
            out.AddNote() << "In: altloc::Component::PreInitialize";
         }
         assert(!"Invalid BlockType");
         return false;
      }
   }

   if (location.altRef == AltitudeReference::AGL)
   {
      wsf::Terrain terrain(simulation->GetTerrainInterface());
      float        elevation;
      terrain.GetElevInterp(location.lat, location.lon, elevation);
      location.alt += (double)elevation;
   }

   platform->SetLocationLLA(location.lat, location.lon, location.alt);

   double yaw, pitch, roll;
   platform->GetOrientationNED(yaw, pitch, roll);
   platform->SetOrientationNED(location.hedSet ? location.hed : yaw, 0.0, 0.0);

   // debug print statements

   const auto& extension = SimulationExtension::Get(*platform->GetSimulation());

   if (mDebugType == DebugType::ON || (mDebugType == DebugType::GLOBAL && extension.GetGlobalDebugEnabled()))
   {
      auto out = ut::log::debug() << "Alternate Locations Pre-Initialization:";
      out.AddNote() << "Platform: " << platform->GetName();
      if (mReferenceComponent)
      {
         const WeightedLocation& baseLocation = mReferenceComponent->mLocations[mLocationIndex];
         auto                    baseNote     = out.AddNote() << "Base Location:";
         baseNote.AddNote() << "Reference: " << mReferenceName;
         baseNote.AddNote() << "Weighted Draw: " << mDraw;

         auto baseLocNote = baseNote.AddNote() << "Location-" << mLocationIndex + 1 << ":";
         baseLocNote.AddNote() << "Lat: " << UtLatPos(baseLocation.lat);
         baseLocNote.AddNote() << "Lon: " << UtLonPos(baseLocation.lon);
         if (baseLocation.altSet)
         {
            baseLocNote.AddNote() << "Alt: " << UtLength(baseLocation.alt)
                                  << (baseLocation.altRef == AltitudeReference::MSL ? " MSL" : " AGL");
         }
         if (baseLocation.hedSet)
         {
            baseLocNote.AddNote() << "Heading: " << baseLocation.hed * UtMath::cDEG_PER_RAD << " deg";
         }
      }
      switch (mBlockType)
      {
      case BlockType::LOCAL:
      case BlockType::REF_LOC:
         for (unsigned int i = 0; i < mLocations.size(); ++i)
         {
            const WeightedLocation& loc  = mLocations[i];
            auto                    note = out.AddNote() << "Location-" << i + 1 << ':';
            if (!mReferenceComponent)
            {
               note.AddNote() << "Weight: " << loc.weight;
            }
            note.AddNote() << "Lat: " << UtLatPos(loc.lat);
            note.AddNote() << "Lon: " << UtLonPos(loc.lon);
            if (loc.altSet)
            {
               note.AddNote() << "Alt: " << UtLength(loc.alt) << (loc.altRef == AltitudeReference::MSL ? " MSL" : " AGL");
            }
            if (loc.hedSet)
            {
               note.AddNote() << "Heading: " << loc.hed * UtMath::cDEG_PER_RAD << " deg";
            }
         }
         if (!mReferenceComponent)
         {
            out.AddNote() << "Weighted Draw: " << mDraw;
         }
         break;
      case BlockType::OFF_LLA:
      {
         auto offsetNote = out.AddNote() << "Offset:";
         offsetNote.AddNote() << "Lat: " << UtLatPos(mOffset.LLA.lat);
         offsetNote.AddNote() << "Lon: " << UtLonPos(mOffset.LLA.lon);
         if (mOffset.LLA.altSet)
         {
            offsetNote.AddNote() << "Alt: " << UtLength(mOffset.LLA.alt);
         }
         if (mOffset.LLA.hedSet)
         {
            offsetNote.AddNote() << "Heading: " << mOffset.LLA.hed * UtMath::cDEG_PER_RAD << " deg";
         }
      }
      break;
      case BlockType::OFF_ARA:
      {
         auto offsetNote = out.AddNote() << "Offset:";
         offsetNote.AddNote() << "Angle: " << mOffset.ARA.angle * UtMath::cDEG_PER_RAD << " deg";
         offsetNote.AddNote() << "Radius: " << UtLength(mOffset.ARA.radius);
         if (mOffset.ARA.altSet)
         {
            offsetNote.AddNote() << "Alt: " << UtLength(mOffset.ARA.alt);
         }
         if (mOffset.ARA.hedSet)
         {
            offsetNote.AddNote() << "Heading: " << mOffset.ARA.hed * UtMath::cDEG_PER_RAD << " deg";
         }
      }
      break;
      default:
         out.Send();
         // RAII block
         {
            auto error = ut::log::fatal() << "Invalid BlockType.";
            error.AddNote() << "In: altloc::Component::PreInitialize";
         }
         assert(!"Invalid BlockType");
         return false;
      }

      double lat, lon, alt;
      platform->GetLocationLLA(lat, lon, alt);
      platform->GetOrientationNED(yaw, pitch, roll);

      auto locationNote = out.AddNote() << "Location";
      if (mBlockType != BlockType::OFF_ARA && mBlockType != BlockType::OFF_LLA)
      {
         locationNote << '-' << mLocationIndex + 1;
      }
      locationNote << ':';

      locationNote.AddNote() << "Lat: " << UtLatPos(lat);
      locationNote.AddNote() << "Lon: " << UtLonPos(lon);
      if (location.altSet)
      {
         locationNote.AddNote() << "Alt: " << UtLength(alt);
      }
      if (location.hedSet)
      {
         locationNote.AddNote() << "Heading: " << yaw * UtMath::cDEG_PER_RAD << " deg";
      }
   }
   WsfObserver::Event(simulation)(aSimTime, platform, this);

   return true;
}

// =============================================================================
// Process input utility methods

// Reads a set of values in the form: <lat-value> <lon-value> <length-value> [ msl | agl ]. Examples:
//
//    0n 0e 1 km
//    30:00:00s 60:00:00w 0 m agl
//
// Defaults to msl if neither msl nor agl is given.
void Component::ReadAltLocLine(UtInput& aInput, WeightedLocation& aLoc)
{
   // Required Lat and Lon
   aInput.ReadValueOfType(aLoc.lat, UtInput::cLATITUDE);
   aInput.ReadValueOfType(aLoc.lon, UtInput::cLONGITUDE);

   std::string command;
   // Optional commands
   while (aInput.ReadCommand(command))
   {
      if (command == "altitude")
      {
         aInput.ReadValueOfType(aLoc.alt, UtInput::cLENGTH);

         std::string altReferenceString;
         aInput.ReadValue(altReferenceString);
         if (altReferenceString == "msl")
         {
            aLoc.altRef = AltitudeReference::MSL;
         }
         else if (altReferenceString == "agl")
         {
            aLoc.altRef = AltitudeReference::AGL;
         }
         else
         {
            aLoc.altRef = AltitudeReference::MSL;
            aInput.PushBack(altReferenceString);
         }
         aLoc.altSet = true;
      }
      else if (command == "heading")
      {
         aInput.ReadValueOfType(aLoc.hed, UtInput::cANGLE);
         aLoc.hedSet = true;
      }
      else if (command == "weight")
      {
         if (mReferenceName != WsfStringId())
         {
            throw UtInput::BadValue(
               aInput,
               "alternate_locations blocks which reference another platform may not include weighted locations.");
         }
         aInput.ReadValue(aLoc.weight);
         aInput.ValueGreaterOrEqual(aLoc.weight, 0.0);
         aLoc.wgtSet = true;
      }
      else
      {
         aInput.PushBack(command);
         break;
      }
   }
}

bool Component::ReadPlatformDebugFlag(UtInput& aInput)
{
   std::string command = aInput.GetCommand();

   if (command == "debug")
   {
      bool result = false;
      aInput.ReadValue(result);
      if (result)
      {
         mDebugType = DebugType::ON;
      }
      else
      {
         mDebugType = DebugType::OFF;
      }
      return true;
   }
   return false;
}

} // namespace altloc
} // namespace wsf
