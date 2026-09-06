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

#include "WsfHEL_Lethality.hpp"

#include <cassert>
#include <iostream>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfException.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

WsfHEL_Lethality::WsfHEL_Lethality(WsfScenario& aScenario)
   : WsfWeaponEffects(aScenario)
   , mEntryMap()
   , mSequence(0)
   , mManageKills(true)
   , mUnharmedUntilKilled(false)
   , mEntryPtr(nullptr)
{
   // Use a Pk determined at intercept, not launch (although for a DE weapon
   // like this, it is essentially the same...
   SetUseInterceptPk();
   SetInterceptPk(1.0);
}

WsfHEL_Lethality::WsfHEL_Lethality(const WsfHEL_Lethality& aSrc)
   : WsfWeaponEffects(aSrc)
   , mEntryMap(aSrc.mEntryMap)
   , mSequence(aSrc.mSequence)
   , mManageKills(aSrc.mManageKills)
   , mUnharmedUntilKilled(aSrc.mUnharmedUntilKilled)
   , mEntryPtr(nullptr)
{
}

bool WsfHEL_Lethality::Entry::ProcessType(UtInput& aInput)
{
   std::string descriptor = aInput.GetCommand();
   bool        processed  = true;
   if (descriptor == "region")
   {
      mType = Entry::cPLATFORM_REGION;
      std::string platformType;
      aInput.ReadValue(platformType);
      mPlatformType = platformType;
      std::string region;
      aInput.ReadValue(region);

      // We need to be more specific than to just store the region.
      // For instance we might have region "IRST" in two definitions,
      // so mangle the platform type with the region name as an id.
      mRegion = platformType + "_" + region;
   }
   else if (descriptor == "category")
   {
      mType = Entry::cCATEGORY;
      std::string category;
      aInput.ReadValue(category);
      mCategory = category;
   }
   else if ((descriptor == "platform_type") || (descriptor == "target_type")) // "platform_type" is deprecated
   {
      mType = Entry::cPLATFORM_TYPE;
      std::string platformType;
      aInput.ReadValue(platformType);
      mPlatformType = platformType;
   }
   else if (GetId() == nullptr)
   {
      // DEPRECATED; we may be processing an old category without the "category" keyword
      // if this is actually invalid input, it will be caught where the energy or energy density is read.
      mType     = Entry::cCATEGORY;
      mCategory = descriptor;
   }
   else
   {
      processed = false;
   }

   return processed;
}

bool WsfHEL_Lethality::Effect::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "minimum_energy")
   {
      mKind = cENERGY;
      aInput.ReadValueOfType(mThreshold, UtInput::cENERGY);
   }
   else if ((command == "minimum_energy_density") || (command == "minimum_fluence"))
   {
      mKind = cENERGY_DENSITY;
      aInput.ReadValueOfType(mThreshold, UtInput::cFLUENCE);
   }
   else if (command == "pk_energy_table")
   {
      mKind = cPK_ENERGY;
      if (mPK_TablePtr == nullptr)
      {
         mPK_TablePtr = new UtMatrixd();
         WsfHEL_Lethality::ReadPK_EnergyTable(aInput, mPK_TablePtr);
      }
   }
   else if (command == "damage_radius")
   {
      aInput.ReadValueOfType(mRadius, UtInput::cLENGTH);
   }
   else if (command == "type")
   {
      aInput.ReadCommand(command);
      if (command == "lethal_platform")
      {
         mType = cLETHAL_PLATFORM;
      }
      else if (command == "lethal_platform_part") // NO_DOC | PENDING
      {
         mType = cLETHAL_PLATFORM_PART;
      }
      else if (command == "custom")
      {
         mType = cCUSTOM;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

bool WsfHEL_Lethality::Entry::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (ProcessType(aInput))
   {
   }
   else
   {
      Effect      effect;
      std::string command;
      while (effect.ProcessInput(aInput))
      {
         aInput.ReadCommand(command);
      }
      aInput.PushBack(command);
      myCommand = true;
      if (effect.mKind == Effect::cUNDEFINED)
      {
         // string what = "!***** Error: You must defined a lethal effect.";
         throw UtInput::EndOfData(aInput);
      }
      mEffects.push_back(effect);
   }
   return myCommand;
}

WsfWeaponEffects* WsfHEL_Lethality::Clone() const
{
   return new WsfHEL_Lethality(*this);
}

// virtual
bool WsfHEL_Lethality::ProcessInput(UtInput& aInput)
{
   bool        success = true;
   std::string command = aInput.GetCommand();
   if ((command == "region") || (command == "category") || (command == "target_type"))
   {
      Entry entry;
      aInput.PushBack(command); // re-read the effect type in Entry::ProcessInput.
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         success = entry.ProcessInput(block.GetInput());
         if (!success)
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (success && (!entry.mEffects.empty()))
      {
         for (unsigned num = 0; num < entry.mEffects.size(); ++num)
         {
            Effect effect = entry.mEffects[num];
            mEntryMap.Add(entry);
         }
      }
      else
      {
         auto out = ut::log::error() << "HEL_Lethality: No effect defined for entry.";
         out.AddNote() << "Entry: " << entry.GetId();
         throw UtInput::BadValue(aInput);
      }
   }
   // deprecated inputs
   else if (command == "min_power") // NO_DOC | DEPRECATED
   {
      aInput.ReadCommand(command);
      Entry entry;
      success = entry.ProcessType(aInput);
      if (success)
      {
         double minPower;
         aInput.ReadValueOfType(minPower, UtInput::cPOWER);
         Effect effect(Effect::cPOWER, minPower);
         entry.mEffects.push_back(effect);
         mEntryMap.Add(entry);
      }
   }
   else if (command == "min_energy") // NO_DOC | DEPRECATED
   {
      aInput.ReadCommand(command);
      Entry entry;
      success = entry.ProcessType(aInput);
      if (success)
      {
         double minEnergy;
         aInput.ReadValueOfType(minEnergy, UtInput::cENERGY);
         Effect effect(Effect::cENERGY, minEnergy);
         entry.mEffects.push_back(effect);
         mEntryMap.Add(entry);
      }
   }
   else if ((command == "min_energy_density") || (command == "min_fluence")) // NO_DOC | DEPRECATED
   {
      aInput.ReadCommand(command);
      Entry entry;
      success = entry.ProcessType(aInput);
      if (success)
      {
         double minFluence;
         aInput.ReadValueOfType(minFluence, UtInput::cFLUENCE);
         Effect effect(Effect::cENERGY_DENSITY, minFluence);
         entry.mEffects.push_back(effect);
         mEntryMap.Add(entry);
      }
   }
   else if (command == "unharmed_until_killed") // NO_DOC | DEPRECATED
   {
      aInput.ReadValue(mUnharmedUntilKilled);
   }
   // end deprecated inputs
   else if (command == "manage_kills")
   {
      aInput.ReadValue(mManageKills);
   }
   else if ((command == "use_launch_pk") || (command == "use_intercept_pk") || (command == "launch_pk") ||
            (command == "intercept_pk") || (command == "use_pk_table"))
   {
      auto out = ut::log::warning() << "The " + command + " command is incompatible with a WSF_HEL_LETHALITY object.";
      out.AddNote() << "Location: " << aInput.GetLocation();
   }
   else
   {
      success = WsfWeaponEffects::ProcessInput(aInput);
   }
   return success;
}

// private
void WsfHEL_Lethality::ReadPK_EnergyTable(UtInput& aInput, UtMatrixd* aTablePtr)
{
   // read the pk-table
   int numRows;
   aInput.ReadValue(numRows);
   if (numRows < 2)
   {
      ut::log::error() << "Pk-energy tables must have at least two rows.";
      throw(UtInput::BadValue(aInput));
   }
   UtMatrixd& PK_Table = *aTablePtr;
   PK_Table.SetSize(numRows, 2);
   int rowNum = 0;
   while (rowNum < numRows)
   {
      double pk;
      double energy;
      aInput.ReadValue(pk);
      aInput.ReadValueOfType(energy, UtInput::cENERGY);
      PK_Table(rowNum, 0) = pk;
      PK_Table(rowNum, 1) = energy;
      ++rowNum;
   }
   assert(rowNum == numRows);
   std::string line;
   aInput.ReadValue(line);
   if (line != "end_pk_energy_table")
   {
      throw(UtInput::BadValue(aInput, "Bad entries in pk_energy_table"));
   }
}

// private
//! Go through all categories of the target platform;
//! attempt to find a match in the given map (category to value map).
//! If no match is found, throw an exception.
const WsfHEL_Lethality::Entry* WsfHEL_Lethality::FindMappedEntry(WsfPlatform* aTargetPtr)
{
   // Is there a region of interest?
   std::string  name     = aTargetPtr->GetType();
   const Entry* entryPtr = nullptr;
   bool         found    = false;
   if (GetEngagement()->GetIntendedTargetOffsetName() != 0)
   {
      name = name + "_" + GetEngagement()->GetIntendedTargetOffsetName();
   }
   entryPtr = mEntryMap.Find(name);
   found    = (entryPtr != nullptr);

   if (!found)
   {
      // Region not found; attempt to find platform value:
      name     = aTargetPtr->GetType();
      entryPtr = mEntryMap.Find(name);
      found    = (entryPtr != nullptr);
   }

   if (!found)
   {
      // Otherwise, look for a category.
      const WsfCategoryList::CategoryList& categoryList = aTargetPtr->GetCategories().GetCategoryList();
      for (auto& catNum : categoryList)
      {
         entryPtr = mEntryMap.Find(catNum);
         if (entryPtr != nullptr)
         {
            break;
         }
      }
   }

   return entryPtr;
}

// virtual
bool WsfHEL_Lethality::Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   bool success = WsfWeaponEffects::Initialize(aSimTime, aEngagementPtr);

   if (success && (mEntryPtr == nullptr))
   {
      assert(aEngagementPtr->GetTargetPlatform() != nullptr);
      mEntryPtr = FindMappedEntry(aEngagementPtr->GetTargetPlatform());
      if (mEntryPtr != nullptr)
      {
         // Set the damage radius if one was specified for the effect.
         double damageRadius = mEntryPtr->mEffects[0].mRadius;
         if (damageRadius > 0.0)
         {
            SetDamageRadius(damageRadius);
         }
         else
         {
            // print a warning
            auto out = ut::log::warning() << "hel_lethality: No damage_radius was specified for target.";
            out.AddNote() << "Target: " << aEngagementPtr->GetTargetPlatform()->GetName();
         }
      }
   }

   return success;
}

// virtual
/** Calculate the weapon's effect on the target.
 */
void WsfHEL_Lethality::ApplyEffectTo(double aSimTime, WsfPlatform* aTargetPtr)
{
   // Note:  CalcInterceptPk(aSimTime, xxx) has already been done by the time we get here.

   double damageIncr = cUNHARMED;

   const auto* weapon = dynamic_cast<const WsfDirectedEnergyWeapon*>(GetEngagement()->GetWeaponSystem());
   if (weapon != nullptr)
   {
      const WsfDirectedEnergyWeapon::Beam* beamPtr = weapon->GetBeam();
      if (beamPtr != nullptr)
      {
         double energy        = beamPtr->GetEnergy();
         double energyDensity = beamPtr->GetEnergyDensity();
         auto   out           = ut::log::debug();
         if (DebugEnabled())
         {
            out << "WsfHEL_Lethality (Target): Energy:";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Target Platform: " << aTargetPtr->GetName();
            out.AddNote() << "Intended Target Offset: " << GetEngagement()->GetIntendedTargetOffsetName();
            out.AddNote() << "Energy Deposition: " << energy << " J";
         }
         double draw = DrawUniformPk();

         if (mEntryPtr == nullptr)
         {
            mEntryPtr = FindMappedEntry(aTargetPtr);
         }
         if (mEntryPtr != nullptr)
         {
            const Effect* effectPtr = &(mEntryPtr->mEffects[mSequence]);

            if (effectPtr != nullptr)
            {
               bool killed = false;

               // TODO revisit:  This is tricky; we can't set the damage radius for the effect until we know what the
               // effect is! (perhaps make base class method virtual, but, no other methods are virtual).
               SetDamageRadius(effectPtr->mRadius);

               if (effectPtr->mKind == Effect::cENERGY)
               {
                  killed = (energy > effectPtr->mThreshold) && (draw <= GetInterceptPk());
               }
               else if (effectPtr->mKind == Effect::cENERGY_DENSITY)
               {
                  killed = (energyDensity > effectPtr->mThreshold) && (draw <= GetInterceptPk());
               }
               else // pk-energy table result; we've already computed the pk, so
               {
                  killed = draw <= GetInterceptPk();
               }

               if (DebugEnabled())
               {
                  auto note = out.AddNote() << "WsfHEL_Lethality: Result for P-K draw:";
                  if (killed)
                  {
                     note.AddNote() << "Killed";
                  }
                  else
                  {
                     note.AddNote() << "Damaged";
                  }

                  if (GetInterceptPk() < 1.0)
                  {
                     note.AddNote() << "Draw: " << draw;
                     note.AddNote() << "P-K: " << GetInterceptPk();
                  }

                  if (mEntryPtr->mType == Entry::cPLATFORM_TYPE)
                  {
                     note.AddNote() << "Against platform type: " << mEntryPtr->mPlatformType;
                  }
                  else if (mEntryPtr->mType == Entry::cPLATFORM_REGION)
                  {
                     note.AddNote() << "Against platform type: " << mEntryPtr->mPlatformType;
                     note.AddNote() << "Region: " << mEntryPtr->mRegion;
                  }
                  else if (mEntryPtr->mType == Entry::cCATEGORY)
                  {
                     note.AddNote() << "Against platform category: " << mEntryPtr->mCategory;
                  }
                  if (effectPtr->mKind == Effect::cENERGY)
                  {
                     note.AddNote() << "Energy Deposition: " << energy / 1000.0 << " kJ";
                     note.AddNote() << "Threshold: " << effectPtr->mThreshold / 1000.0 << " kJ)";
                  }
                  else
                  {
                     note.AddNote() << "Energy Density: " << energyDensity / 1.0e+7 << " kJ/cm^2";
                     note.AddNote() << "Threshold: " << effectPtr->mThreshold / 1.0e+7 << " kJ/cm^2)";
                  }
               }

               if (killed)
               {
                  damageIncr = cMORTALLY_WOUNDED;
               }
               else if (!mUnharmedUntilKilled)
               {
                  damageIncr = 0.001; // Trivial damage
               }

               ApplyEffectIncrement(aSimTime, aTargetPtr, damageIncr);
            }
         }
         else
         {
            out.AddNote() << "WsfHEL_Lethality: Could not find a category match for target. Returning no kill.";
            out.AddNote() << "Target: " << aTargetPtr->GetName();
            out.AddNote() << "Target Type: " << aTargetPtr->GetType();
         }
      }
   }
}

//! Depending on configuration, perform base class' implementation or
//! a modified implementation that does not delete killed entities.
//! This latter implementation is used for virtual simulation (esp. for the API version of HELSim).
void WsfHEL_Lethality::ApplyEffectIncrement(double aSimTime, WsfPlatform* aOtherPlatformPtr, double aDamageAmount)
{
   if (mManageKills)
   {
      // This is a standalone simulation; use the default implementation
      // WsfWeaponEffects::ApplyEffectIncrement(aSimTime, aOtherPlatformPtr, aDamageAmount);
      // TODO Need to decide how to treat implicit weapons in base class: only change over base class method is that
      // we keep "killed" status for indestructible platforms, but we don't set damage or remove from simulation.
      // previous implementation was to mark the result as "damage" even if it would have been "killed"
      if (aOtherPlatformPtr->IsExternallyControlled())
      {
         ApplyEffectIncrementP(aSimTime, aOtherPlatformPtr, aDamageAmount);
      }
      else // Internal simulation effects
      {
         WsfWeaponEffects::ApplyEffectIncrement(aSimTime, aOtherPlatformPtr, aDamageAmount);
      }

      // Allow dis to reflect the new damaged state through a callback
      // This may result in the issue of a directed energy pdu
      const WsfDirectedEnergyWeapon* de_weaponPtr =
         static_cast<const WsfDirectedEnergyWeapon*>(GetEngagement()->GetWeaponSystem());
      WsfObserver::DirectedEnergyWeaponHit(GetSimulation())(aSimTime, de_weaponPtr, GetEngagement());
   }
   else
   {
      ApplyEffectIncrementP(aSimTime, aOtherPlatformPtr, aDamageAmount);
   }
}

//! Private
//! This is a slightly different implementation from the base class'.
//! The difference is that we allow a "killed" result to be applied to an
//! "invincible" target.  This change is to allow the user to remove targets,
//! rather than the underlying simulation.
void WsfHEL_Lethality::ApplyEffectIncrementP(double aSimTime, WsfPlatform* aOtherPlatformPtr, double aDamageAmount)
{
   double initialDamage = aOtherPlatformPtr->GetDamageFactor();
   double incremDamage  = std::max(cUNHARMED, aDamageAmount);
   double finalDamage   = std::min(cMORTALLY_WOUNDED, initialDamage + incremDamage);

   Result newResult = cMISSED;
   if (incremDamage > cUNHARMED)
   {
      // Target was hit. See if we've killed it.

      bool isExternal = aOtherPlatformPtr->IsExternallyControlled();
      if (!mUnharmedUntilKilled)
      {
         newResult = cDAMAGED;
      }
      if (finalDamage >= cMORTALLY_WOUNDED)
      {
         incremDamage = cMORTALLY_WOUNDED - initialDamage;
         finalDamage  = cMORTALLY_WOUNDED;
         newResult    = cKILLED;
      }

      // We cannot directly damage or remove a remotely controlled player.
      // But we can inform the others (through the observer interface) that
      // we THINK we hit the external player. It will be up to the remote
      // simulation to respond accordingly.
      if (!isExternal)
      {
         aOtherPlatformPtr->SetDamageFactor(finalDamage);
         if ((newResult == cKILLED) && DebugEnabled())
         {
            auto out = ut::log::debug() << "Platform determined killed, but not removed from simulation.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << aOtherPlatformPtr->GetName();
         }
      }
   }

   // Update the result of the engagement.
   bool isIntendedTgt = aOtherPlatformPtr->GetIndex() == GetEngagement()->GetTargetPlatformIndex();
   SetResult(newResult, isIntendedTgt);

   // Inform the observers about the weapon hit or miss.
   // This must be done after SetResult() so the observers have the result.
   if (newResult == cMISSED)
   {
      WsfObserver::WeaponMissed(GetSimulation())(aSimTime, GetEngagement(), aOtherPlatformPtr);
   }
   else
   {
      WsfObserver::WeaponHit(GetSimulation())(aSimTime, GetEngagement(), aOtherPlatformPtr);
   }
}

// virtual
void WsfHEL_Lethality::CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr)
{
   UtMatrixd* tablePtr = nullptr;
   if (mEntryPtr == nullptr)
   {
      mEntryPtr = FindMappedEntry(aTargetPtr);
   }
   if (mEntryPtr != nullptr)
   {
      tablePtr = mEntryPtr->mEffects[mSequence].mPK_TablePtr;
   }
   if (tablePtr == nullptr)
   {
      // If there is no table, we will be using a fixed input PK.
      return;
   }

   double pk = 0.0;

   const auto* weapon = dynamic_cast<const WsfDirectedEnergyWeapon*>(GetEngagement()->GetWeaponSystem());
   if (weapon != nullptr)
   {
      const WsfDirectedEnergyWeapon::Beam* beamPtr = weapon->GetBeam();
      if (beamPtr != nullptr)
      {
         double energy = beamPtr->GetEnergy();

         // Linear interpolation on energy.
         int        numRows  = tablePtr->GetRows();
         int        row      = 0;
         UtMatrixd& PK_Table = *tablePtr;

         for (; (row < numRows) && (energy >= PK_Table(row, 1)); ++row)
            ;
         if (row == numRows)
         {
            // clamp to last value
            pk = PK_Table((row - 1), 0);
         }
         else if (row > 0)
         {
            double delta = (energy - PK_Table((row - 1), 1)) / (PK_Table(row, 1) - PK_Table((row - 1), 1));
            pk           = PK_Table((row - 1), 0) + delta * (PK_Table(row, 0) - PK_Table((row - 1), 0));
            if (pk > 1.0)
            {
               pk = 1.0;
            }
         }
         // else row = 0, energy less than minimum energy threshold, pk = 0.0
      }
   }

   SetInterceptPk(pk);
}

// protected virtual
//! Different processing from base class if we decide to manage kills (used for virtual sim. with API).
bool WsfHEL_Lethality::IsVulnerable(WsfPlatform* aOtherPlatformPtr)
{
   if (mManageKills)
   {
      // This is a standalone simulation; use the default implementation
      return WsfWeaponEffects::IsVulnerable(aOtherPlatformPtr);
   }
   else
   {
      return IsVulnerableP(aOtherPlatformPtr);
   }
}

// private
//! Same a base class, but without some checks (esp. the check for mortally wounded).
bool WsfHEL_Lethality::IsVulnerableP(WsfPlatform* aOtherPlatformPtr)
{
   bool isMe       = aOtherPlatformPtr == GetEngagement()->GetWeaponPlatform();
   bool isMyParent = aOtherPlatformPtr == GetEngagement()->GetFiringPlatform();

   bool isVulnerable = true;
   if ((isMe) || (isMyParent) || (aOtherPlatformPtr->IsDeleted()) || (aOtherPlatformPtr->IsExternalShadow()))
   {
      isVulnerable = false;
   }
   return isVulnerable;
}
