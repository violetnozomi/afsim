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

#include "WsfCarltonLethality.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "WsfPlatform.hpp"
#include "WsfStringId.hpp"
#include "WsfWeaponEngagement.hpp"

//! Default constructor.
WsfCarltonLethality::WsfCarltonLethality(WsfScenario& aScenario)
   : WsfExplicitWeaponEffects(aScenario)
   , mLAMap()
   , mD0(0.5)
{
   // We are going to set and use ONLY the Intercept Value of Pk for this class.
   SetUseInterceptPk();
}

// virtual
WsfWeaponEffects* WsfCarltonLethality::Clone() const
{
   return new WsfCarltonLethality(*this);
}

// virtual
bool WsfCarltonLethality::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "d_zero")
   {
      double dzValue;
      aInput.ReadValue(dzValue);
      aInput.ValueInClosedRange(dzValue, 0.0, 1.0);
      SetD0(dzValue);
   }
   else if (command == "lethal_area")
   {
      std::string tgtTypeName;
      double      lethalArea;
      aInput.ReadValue(tgtTypeName);
      aInput.ReadValueOfType(lethalArea, UtInput::cAREA);
      aInput.ValueGreater(lethalArea, 0.0);
      WsfStringId tgtTypeId = tgtTypeName;
      if (!AddPlatformType(tgtTypeId, lethalArea))
      {
         throw UtInput::BadValue(aInput, "Duplicate lethal_area");
      }
   }
   else if ((command == "use_launch_pk") || (command == "launch_pk") || (command == "intercept_pk"))
   {
      std::string scream = "The " + command + " command is incompatible with a WsfCarltonLethality object.";
      throw UtInput::BadValue(aInput, scream);
   }
   else
   {
      // Test the possibility that command is for the base class.
      myCommand = WsfExplicitWeaponEffects::ProcessInput(aInput);
   }

   return myCommand;
}

//! Add into consideration a new platform type, by defining the Lethal Area for it.
//! @param aPlatformTypeID identifies the platform type for which to add consideration.
//! @param aLethalArea is the vulnerable area intrinsic to the target platform type.
//! @note If no lethal area is defined for a platform type, then the munition will have
//! no effect against it.
bool WsfCarltonLethality::AddPlatformType(WsfStringId aPlatformTypeID, double aLethalArea)
{
   bool result = false;

   auto myIter(mLAMap.find(aPlatformTypeID));
   if (myIter == mLAMap.end())
   {
      // Success, the tgtTypeId has not already
      // been inserted, so do it now.
      mLAMap.insert(std::pair<WsfStringId, double>(aPlatformTypeID, aLethalArea));
      result = true;
   }

   return result;
}

// virtual
void WsfCarltonLethality::CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr)
{
   // Get the TYPE of the platform in order
   // to determine its lethal area:

   WsfStringId                             tgtTypeId = aTargetPtr->GetTypeId();
   std::map<WsfStringId, double>::iterator myIter;
   myIter = mLAMap.find(tgtTypeId);

   double thePk = 0.0;

   if (myIter != mLAMap.end())
   {
      double lethalArea = myIter->second;

      // Now we can calculate the geometry of the engagement.
      double missRange;
      double missDeflection;
      double angleOfFall;

      // Get the weapon and target locations at the time of detonation.
      // These may be different that the current locations (see WsfWeaponEngagement and WsfWeaponFuse);

      double targetLocWCS[3];
      GetEngagement()->GetTargetLocationWCS(aTargetPtr, targetLocWCS);
      UtEntity target(*aTargetPtr);
      target.SetLocationWCS(targetLocWCS);

      WsfPlatform* weaponPtr = GetEngagement()->GetWeaponPlatform();
      double       weaponLocWCS[3];
      GetEngagement()->GetWeaponLocationWCS(weaponPtr, weaponLocWCS);
      UtEntity weapon(*weaponPtr);
      weapon.SetLocationWCS(weaponLocWCS);
      double weaponVelNED[3];
      weapon.GetVelocityNED(weaponVelNED);

      double offsetNED[3];
      target.ConvertWCSToNED(weaponLocWCS, offsetNED);

      // Orient the X and Y offsets to the weapon, from the target's frame of reference
      double grndHeading = atan2(weaponVelNED[1], weaponVelNED[0]);

      missRange      = cos(grndHeading) * offsetNED[0] + sin(grndHeading) * offsetNED[1];
      missDeflection = cos(grndHeading) * offsetNED[1] - sin(grndHeading) * offsetNED[0];

      double weaponGrndSpeed = sqrt(weaponVelNED[0] * weaponVelNED[0] + weaponVelNED[1] * weaponVelNED[1]);
      angleOfFall            = atan2(weaponVelNED[2], weaponGrndSpeed);

      double RDR = 1.0 / (1.0 - 0.8 * cos(angleOfFall));

      // Probability of Damage =
      thePk = mD0 * exp(-(mD0 * UtMath::cPI / lethalArea) *
                        (missRange * missRange * RDR + missDeflection * missDeflection * (1.0 / RDR)));
   }

   SetInterceptPk(thePk);
}
