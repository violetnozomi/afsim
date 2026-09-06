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

#include "WsfATG_WeaponLAR.hpp"

#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfZone.hpp"
#include "WsfZoneReference.hpp"

WsfATG_WeaponLAR::WsfATG_WeaponLAR()
   : WsfWeaponLAR()
   , mZonePtr(nullptr)
   , mDeltaAlt(0.0)
   , mLaunchSpd(0.0)
   , mSens10mAlt(0.0)
   , mSens10msVel(0.0)
   , mSensGradient(0.0)
{
   SetType(WsfStringId("WSF_ATG_WEAPON_LAR"));
}

WsfATG_WeaponLAR::WsfATG_WeaponLAR(const WsfATG_WeaponLAR& aSrc)
   : WsfWeaponLAR(aSrc)
   , mZonePtr(nullptr)
   , mDeltaAlt(aSrc.mDeltaAlt)
   , mLaunchSpd(aSrc.mLaunchSpd)
   , mSens10mAlt(aSrc.mSens10mAlt)
   , mSens10msVel(aSrc.mSens10msVel)
   , mSensGradient(aSrc.mSensGradient)
{
   SetType(WsfStringId("WSF_ATG_WEAPON_LAR"));
   if (aSrc.mZonePtr != nullptr)
   {
      delete mZonePtr;
      mZonePtr = aSrc.mZonePtr->Clone();
   }
}

// static
WsfStringId WsfATG_WeaponLAR::GetTypeId()
{
   return UtStringIdLiteral("WSF_ATG_WEAPON_LAR");
}

// virtual
WsfATG_WeaponLAR::~WsfATG_WeaponLAR()
{
   delete mZonePtr;
}

// virtual
WsfWeaponLAR* WsfATG_WeaponLAR::Clone() const
{
   return new WsfATG_WeaponLAR(*this);
}

// virtual
bool WsfATG_WeaponLAR::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "delta_altitude")
   {
      aInput.ReadValueOfType(mDeltaAlt, UtInput::cLENGTH);
   }
   else if (command == "use_zone")
   {
      std::string globalZoneName;
      aInput.ReadValue(globalZoneName);

      WsfZone*    zonePtr = WsfScenario::FromInput(aInput).FindTypeT<WsfZone>(globalZoneName);
      std::string as;       // This is a throwaway string which should contain "as",
      aInput.ReadValue(as); // just for a more readable syntax.
      std::string localZoneName;
      aInput.ReadValue(localZoneName);

      if (zonePtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Global Zone not found: " + globalZoneName);
      }
      else
      {
         std::unique_ptr<WsfZoneReference> zoneRefPtr(new WsfZoneReference(globalZoneName));
         if (zoneRefPtr.get() == nullptr)
         {
            throw UtInput::BadValue(aInput, "Could not create Zone reference.");
         }
         zoneRefPtr->SetName(localZoneName);
         zoneRefPtr->SetGlobalName(globalZoneName);
         // If we already have a zone in use, and we are
         // replacing it with a new one, delete the old.
         delete mZonePtr;
         mZonePtr = zoneRefPtr.release();
      }
   }
   else if (command == "launch_speed")
   {
      aInput.ReadValueOfType(mLaunchSpd, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mLaunchSpd, 0.0);
   }
   else if (command == "sensitivity_range_per_10m_altitude")
   {
      aInput.ReadValueOfType(mSens10mAlt, UtInput::cLENGTH);
   }
   else if (command == "sensitivity_range_per_10ms_velocity")
   {
      aInput.ReadValueOfType(mSens10msVel, UtInput::cLENGTH);
   }
   else if (command == "sensitivity_range_per_percent_gradient")
   {
      aInput.ReadValueOfType(mSensGradient, UtInput::cLENGTH);
   }
   else
   {
      myCommand = WsfWeaponLAR::ProcessInput(aInput);
   }

   return myCommand;
}

void WsfATG_WeaponLAR::ProcessInputBlock(UtInputBlock& aInputBlock)
{
   while (aInputBlock.ReadCommand())
   {
      ProcessInput(aInputBlock.GetInput());
   }
}

// virtual
bool WsfATG_WeaponLAR::Initialize(WsfSimulation& aSimulation)
{
   bool result = true;

   if (mZonePtr == nullptr)
   {
      auto out = ut::log::error() << "Zone was not defined.";
      out.AddNote() << "Zone: " << GetName();
      result = false;
   }
   else
   {
      mZonePtr->Initialize(aSimulation.GetScenario());
   }

   if ((result) && (!WsfWeaponLAR::Initialize(aSimulation)))
   {
      result = false;
   }

   return result;
}


// virtual
bool WsfATG_WeaponLAR::IsInside(const double aTargetPointWCS[3],
                                const double aLaunchPointWCS[3],
                                const double aLaunchHeading,
                                const double aLaunchSpeed,
                                const double aDeltaAltitude,
                                const double aLaunchGamma) const
{
   // Calculate a LAR shift due to off-design conditions.
   // (Hopefully these are only SMALL deltas off-design.)

   double deltaDownRange = (aLaunchSpeed - mLaunchSpd) * mSens10msVel / 10.0 +
                           (aDeltaAltitude - mDeltaAlt) * mSens10mAlt / 10.0 + aLaunchGamma * mSensGradient;

   bool result =
      mZonePtr->PointIsInside(GetSimulation(), aTargetPointWCS, aLaunchPointWCS, aLaunchHeading, deltaDownRange);
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "WsfATG_WeaponLAR::IsInside(): " << std::boolalpha << result;
      out.AddNote() << "Launch Speed - Actual (m/s): " << aLaunchSpeed;
      out.AddNote() << "Launch Speed - LAR (m/s): " << mLaunchSpd;
      out.AddNote() << "Delta Alt - Actual (m): " << aDeltaAltitude;
      out.AddNote() << "Delta Alt - LAR (m): " << mDeltaAlt;
      out.AddNote() << "Launch Gamma - Actual (radians): " << aLaunchGamma;
      out.AddNote() << "Delta Down Range (m): " << deltaDownRange;
   }

   return result;
}
