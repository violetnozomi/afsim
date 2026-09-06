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

#include "WsfTabulatedLethality.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

WsfTabulatedLethality::WsfTabulatedLethality(WsfScenario& aScenario)
   : WsfExplicitWeaponEffects(aScenario)
   , mTargets()
   , mInputTargetId()
{
   // Use ONLY the Intercept Pk value for this class:
   SetUseInterceptPk();
}

// virtual
WsfWeaponEffects* WsfTabulatedLethality::Clone() const
{
   return new WsfTabulatedLethality(*this);
}

// virtual
bool WsfTabulatedLethality::PreInitialize()
{
   bool result = WsfExplicitWeaponEffects::PreInitialize();

   if (mTargets.empty())
   {
      auto out = ut::log::error()
                 << "WsfTabulatedLethality did not speficity any targets as being vulnerable to this weapon type.";
      out.AddNote() << "WsfTabulatedLethality: " << GetName();
      result = false;
   }
   else
   {
      bool warned = false;

      // Traverse the whole Lethality Table, and assure that each
      // CEP versus Pk array (vector) is sorted in increasing CEP,
      // decreasing Pk order.  Print a Warning if not true.

      for (auto tgtIter(mTargets.begin()); (tgtIter != mTargets.end()) && (!warned); ++tgtIter)
      {
         // Note:  Consider "tgtIter->second" to be equivalent
         // to "aCell", as commented out below:
         // Targets aCell = tgtIter->second;
         if (tgtIter->second.empty())
         {
            auto out = ut::log::error() << "WsfTabulatedLethality did not specify any CEP  or Pk values for target.";
            out.AddNote() << "WsfTabulatedLethality: " << GetName();
            out.AddNote() << "Target: " << tgtIter->first;
            result = false;
         }
         else
         {
            // Sort the vector by increasing CEP.
            sort(tgtIter->second.begin(), tgtIter->second.end());

            if (!warned)
            {
               // Now assure that Pk goes down as CEP goes up.
               double prevPk = 1.0;
               for (auto& cellIter : tgtIter->second)
               {
                  if ((!warned) && (prevPk < cellIter.Pk()))
                  {
                     ut::log::error() << "WsfTabulatedLethality: Not all Pk values decrease with increasing CEP.";
                     warned = true;
                     result = false;
                  }
                  prevPk = cellIter.Pk();
               }
            } // if (! warned)
         }    // if (tgtIter->second.size() == 0)
      }       // for (Targets::iterator
   }          // if (mTargets.size() == 0)
   return result;
}

// virtual
bool WsfTabulatedLethality::Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   WsfWeaponEffects::Initialize(aSimTime, aEngagementPtr);
   return true;
}

// virtual
bool WsfTabulatedLethality::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());

   if (command == "target_type")
   {
      myCommand = true;
      aInput.ReadValue(mInputTargetId);
   }
   else if ((command == "use_launch_pk") || (command == "launch_pk") || (command == "intercept_pk"))
   {
      myCommand = true;
      throw UtInput::BadValue(aInput, "WsfTabulatedLethality is incompatible with the " + command + " option.");
   }
   else if (command == "pk_at_cep")
   {
      myCommand = true;
      double pk;
      double cep;

      aInput.ReadValue(pk);
      aInput.ValueInClosedRange(pk, 0.0, 1.0);

      aInput.ReadValueOfType(cep, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(cep, 0.0);

      // This is the new point we are going to add into a Pk vs. CEP array.
      Point newPoint(cep, pk);

      auto tgtIter(mTargets.find(mInputTargetId));

      if (tgtIter == mTargets.end())
      {
         // Target not found... Add this target's row into mTargets.
         // (If this target row does not exist, then neither can
         //  the Cell's Point, so add a Point into a Cell.)

         // typedef std::vector<Point>  Cell;
         // typedef std::map<int, Cell> Targets;

         Cell newCell;
         newCell.push_back(newPoint);

         std::pair<Targets::iterator, bool> result = mTargets.insert(Targets::value_type(mInputTargetId, newCell));

         if (!result.second)
         {
            throw UtInput::BadValue(aInput, "Adding value 'pk_at_cep' failed.");
         }
      }
      else
      {
         // Found... this Target has already been entered as a row in the table.
         // This (Target, Weapon) combination has already had a (CEP, Pk)
         // value placed in its array... simple... just push-back another one.
         tgtIter->second.push_back(newPoint);
      }
   }
   else
   {
      // Test the possibility that command is for the base class.
      myCommand = WsfExplicitWeaponEffects::ProcessInput(aInput);
   }

   return myCommand;
}

// virtual
void WsfTabulatedLethality::CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr)
{
   double thePk = 0.0;

   auto tgtIter(mTargets.find(aTargetPtr->GetTypeId()));

   if (tgtIter != mTargets.end())
   {
      // Array lookup for the values of CEP which bridge
      // the input value.  Do not extrapolate past the ends.

      if (tgtIter->second.front().CEP() > GetCEP())
      {
         thePk = tgtIter->second.front().Pk();
      }
      else if (tgtIter->second.back().CEP() < GetCEP())
      {
         thePk = tgtIter->second.back().Pk();
      }
      else
      {
         // We are in the middle of the array,
         // an interpolation is required.

         auto cellIterA(tgtIter->second.begin());
         auto cellIterB(cellIterA + 1);

         if (cellIterA != cellIterB)
         {
            bool done = false;
            while (!done)
            {
               if ((cellIterA->CEP() < GetCEP()) && (cellIterB->CEP() > GetCEP()))
               {
                  // This is the correct line segment.
                  // Standard linear interpolation into the vector...
                  double cep0   = cellIterA->CEP();
                  double cep1   = cellIterB->CEP();
                  double pk0    = cellIterA->Pk();
                  double pk1    = cellIterB->Pk();
                  double interp = (GetCEP() - cep0) / (cep1 - cep0);
                  thePk         = pk0 + interp * (pk1 - pk0);
                  done          = true;
               }
               else
               {
                  // Step over to the right one cell,
                  // and check for the correct region.
                  ++cellIterA;
                  ++cellIterB;
               }
            }
         }
      }
   }

   SetInterceptPk(thePk);
}
