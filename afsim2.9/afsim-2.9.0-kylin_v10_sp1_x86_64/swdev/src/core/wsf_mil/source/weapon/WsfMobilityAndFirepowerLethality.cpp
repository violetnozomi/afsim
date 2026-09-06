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

#include "WsfMobilityAndFirepowerLethality.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "UtVec3dX.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponFuse.hpp"
#include "WsfWeaponObserver.hpp"

namespace
{
// TODO-VS2015 The two comments below only work in VS2015 and gcc 5.x compilers
// TODO-VS2015 The other stuff below this was a workaround for VS2013
// TODO-VS2015 const WsfStringId cDEFAULT("default");
// TODO-VS2015 const WsfStringId cMY_TYPE("WSF_MOBILITY_AND_FIREPOWER_LETHALITY");

WsfStringId cDEFAULT;
WsfStringId cMY_TYPE;
void        InitializeStatics()
{
   if (cDEFAULT.Empty())
   {
      cDEFAULT = "default";
      cMY_TYPE = "WSF_MOBILITY_AND_FIREPOWER_LETHALITY";
   }
}
} // namespace

// static
const double WsfMobilityAndFirepowerLethality::cSLIGHT_DAMAGE   = 0.25;
const double WsfMobilityAndFirepowerLethality::cMODERATE_DAMAGE = 0.50;

// ===============================================================================================
//! Default constructor.
WsfMobilityAndFirepowerLethality::WsfMobilityAndFirepowerLethality(WsfScenario& aScenario)
   : WsfExplicitWeaponEffects(aScenario)
   , mSharedMFK_TablesPtr(nullptr)
   , mSharedTargetTypesPtr(nullptr)
   , mUpdateInterval(1.0)
   , mTargetTypeInUsePtr(nullptr)
   , mCM_StatePtrs()
   , mCallbacks()
{
   // We are going to set and use ONLY the Intercept Value of Pk for this class.
   // (This is for the legacy implementation only.)
   SetUseInterceptPk();
   SetInterceptPk(1.0);

   InitializeStatics();
}

// ===============================================================================================
//! Copy constructor (for Clone())
// protected
WsfMobilityAndFirepowerLethality::WsfMobilityAndFirepowerLethality(const WsfMobilityAndFirepowerLethality& aSrc)
   : WsfExplicitWeaponEffects(aSrc)
   , mSharedMFK_TablesPtr(aSrc.mSharedMFK_TablesPtr)
   , mSharedTargetTypesPtr(aSrc.mSharedTargetTypesPtr)
   , mUpdateInterval(aSrc.mUpdateInterval)
   , mTargetTypeInUsePtr(aSrc.mTargetTypeInUsePtr)
   , mCM_StatePtrs()
   , mCallbacks()
{
}

// ===============================================================================================
//! Destructor
// virtual
WsfMobilityAndFirepowerLethality::~WsfMobilityAndFirepowerLethality()
{
   for (size_t i = 0; i != mCM_StatePtrs.size(); ++i)
   {
      delete mCM_StatePtrs[i];
   }
}

// ===============================================================================================
// virtual
WsfWeaponEffects* WsfMobilityAndFirepowerLethality::Clone() const
{
   return new WsfMobilityAndFirepowerLethality(*this);
}

// ===============================================================================================
// virtual
std::string WsfMobilityAndFirepowerLethality::MyFullName() const
{
   const WsfWeaponEngagement* engPtr = GetEngagement();
   if (engPtr != nullptr)
   {
      WsfPlatform* missilePtr = engPtr->GetWeaponPlatform();
      return missilePtr->GetName() + ":" + GetName();
   }
   return cMY_TYPE;
}

// ===============================================================================================
// virtual
bool WsfMobilityAndFirepowerLethality::Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   bool ok = WsfExplicitWeaponEffects::Initialize(aSimTime, aEngagementPtr);
   if (ok)
   {
      // Here we enforce that ONE lethality Table must _ALWAYS_ be of type "default".
      // So if we do not yet have one, create it, and use default parameters for it.
      // (Use the probability values in the default constructor.)
      if (GetTable(cDEFAULT) == nullptr)
      {
         Add(MFK_Table(cDEFAULT));
         auto out = ut::log::warning() << "Weapon is automatically inserting a 'default' probablity table.";
         out.AddNote() << "Weapon: " << MyFullName();
         out.AddNote() << "Type: " << cMY_TYPE;
      }

      // Here we also enforce that ONE TargetType must _ALWAYS_ be for Target Type "default".
      // So if we do not yet have one, create it, and use default parameters for it.
      if (GetTargetType(cDEFAULT) == nullptr)
      {
         Add(TargetType(cDEFAULT));
         auto out = ut::log::info() << "Weapon is automatically inserting a 'default' TargetType lethality.";
         out.AddNote() << "Weapon: " << MyFullName();
         out.AddNote() << "Type: " << cMY_TYPE;
      }

      // Assure that all tables used in this engagement are properly initialized.
      // This may cause the tables to get initialized more than once; they should
      // deal with this internally.
      std::vector<MFK_Table>::iterator iter = (*mSharedMFK_TablesPtr).begin();
      while (iter != (*mSharedMFK_TablesPtr).end())
      {
         if (!iter->Initialize(aSimTime, this))
         {
            auto out = ut::log::info() << "MFK Table intialized:";
            out.AddNote() << "Name: " << MyFullName();
            ok = false;
         }
         ++iter;
      }

      std::vector<TargetType>::iterator iter2 = (*mSharedTargetTypesPtr).begin();
      while (iter2 != (*mSharedTargetTypesPtr).end())
      {
         if (!iter2->Initialize(aSimTime, this))
         {
            auto out = ut::log::info() << "Target Type intialized:";
            out.AddNote() << "Name: " << MyFullName();
            ok = false;
         }
         ++iter2;
      }
   }

   bool doScheduleCM_Checks = false;

   if (ok)
   {
      // Determine which target type table we will use for this engagement.
      // Note that it may in fact be the 'default' target type lethality:

      // Supply default value in case nothing else matches:
      mTargetTypeInUsePtr = &(*(mSharedTargetTypesPtr->begin()));

      WsfPlatform* tgtPtr = aEngagementPtr->GetTargetPlatform();
      if (tgtPtr != nullptr)
      {
         const WsfObject::TypeList& tgtTypeIds = tgtPtr->GetTypeList();
         for (size_t i = 0; i != tgtTypeIds.size(); ++i)
         {
            if (FindTargetType(tgtTypeIds[i], mTargetTypeInUsePtr))
            {
               break;
            }
         }
      }
      assert(mTargetTypeInUsePtr != nullptr);

      if (mTargetTypeInUsePtr->ContainsVulnerabilities())
      {
         doScheduleCM_Checks = true;
      }

      mCallbacks.Add(
         WsfObserver::WeaponFired(GetSimulation()).Connect(&WsfMobilityAndFirepowerLethality::WeaponFired, this));
      mCallbacks.Add(
         WsfObserver::PlatformDeleted(GetSimulation()).Connect(&WsfMobilityAndFirepowerLethality::PlatformDeleted, this));

      if (doScheduleCM_Checks)
      {
         AddInitialCM_States(aSimTime);

         // Queue a recurring Event to re-visit the CMs in progress during the engagement.
         GetSimulation()->AddEvent(ut::make_unique<UpdateEvent>(aSimTime + mUpdateInterval, this));
      }
   }

   return ok;
}

// ===============================================================================================
// virtual
bool WsfMobilityAndFirepowerLethality::AddCM_State(double         aSimTime,
                                                   WsfStringId    aCM_TypeId,
                                                   WsfPlatform*   aCM_PlatPtr,
                                                   WsfPlatform*   aMissilePtr,
                                                   WsfPlatform*   aTargetPtr,
                                                   Vulnerability* aEffectPtr)
{
   if (aCM_PlatPtr == nullptr)
   {
      return false;
   }
   if (aMissilePtr == nullptr)
   {
      return false;
   }
   if (aTargetPtr == nullptr)
   {
      return false;
   }
   if (aEffectPtr == nullptr)
   {
      return false;
   }

   size_t index = aCM_PlatPtr->GetIndex();

   for (size_t i = 0; i != mCM_StatePtrs.size(); ++i)
   {
      if (mCM_StatePtrs[i]->PlatIndex() == index)
      {
         // Cannot add a duplicate.
         return false;
      }
   }

   mCM_StatePtrs.push_back(
      new CM_State(aSimTime, aCM_TypeId, aCM_PlatPtr, aMissilePtr, aTargetPtr, aEffectPtr, GetSimulation()));

   return true;
}

// ===============================================================================================
// virtual
bool WsfMobilityAndFirepowerLethality::IsTypeMatch(const std::vector<WsfStringId>& aPlatTypes, WsfStringId aCM_Type)
{
   // string toMatch = aCM_Type.GetString();
   //  Test these two arrays, to see if they match:
   for (size_t j = 0; j != aPlatTypes.size(); ++j)
   {
      // string platType = aPlatTypes[j].GetString();
      if (aPlatTypes[j] == aCM_Type)
      {
         return true;
      }
   }
   return false;
}

// ===============================================================================================
// virtual
void WsfMobilityAndFirepowerLethality::AddInitialCM_States(double aSimTime)
{
   // Go through all currently existing platforms, and test each of them
   // to see if CMs that 'this' weapon is vulnerable to are already being
   // used in the simulation.  If so, know that 'this' effectiveness is
   // potentially getting degraded from the very beginning of employment
   // in the Sim (exposure duration is tracked and compared to a threshold time):

   for (unsigned int i = 0; i != GetSimulation()->GetPlatformCount(); ++i)
   {
      WsfPlatform* cmPlatPtr = GetSimulation()->GetPlatformEntry(i);

      // March through all types that this platform inherits from, and find
      // out if this is a CM <platform> type 'this' is vulnerable to.
      const WsfObject::TypeList& cmTypeIds = cmPlatPtr->GetTypeList();
      for (size_t j = 0; j != cmTypeIds.size(); ++j)
      {
         // The given platform may inherit from any of a number of types...
         WsfStringId    cmTypeId = cmTypeIds[j];
         Vulnerability* vulnerabilityPtr;

         if (mTargetTypeInUsePtr->FindVulnerabilityTo(cmTypeId, vulnerabilityPtr))
         {
            WsfPlatform* missilePtr = GetEngagement()->GetWeaponPlatform();
            WsfPlatform* targetPtr  = GetEngagement()->GetTargetPlatform();

            // The "add" might not be successful if we already have that type added.  This is OK.
            /*bool added =*/
            AddCM_State(aSimTime, cmTypeId, cmPlatPtr, missilePtr, targetPtr, vulnerabilityPtr);

            // Break on this type, and go on to the next platform in the Sim:
            break;
         }
      }
   }
}

// ===============================================================================================
// virtual
void WsfMobilityAndFirepowerLethality::CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr)
{
   // Does nothing, but must override the parent method, which uses other PkTable types ...
}

// ===============================================================================================
std::string WsfMobilityAndFirepowerLethality::KillTypeName(KillType aType) const
{
   // Provides a human-readable string for the enumeration.
   switch (aType)
   {
   case cNO_KILL:
      return "NO_DAMAGE";
   case cFIREPOWER:
      return "FIREPOWER_KILL";
   case cMOBILITY:
      return "MOBILITY_KILL";
   case cFIREPOWER_AND_MOBILITY:
      return "FIREPOWER_AND_MOBILITY_KILL";
   case cCATASTROPHIC_KILL:
      return "CATASTROPHIC_KILL";
   }
   return "UNKNOWN_KILL";
}

// ===============================================================================================
// virtual
bool WsfMobilityAndFirepowerLethality::ProcessInput(UtInput& aInput)
{
   // Read in new table values for this class to use.
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   MFK_Table*  tablePtr      = nullptr;
   TargetType* targetTypePtr = nullptr;

   if ((command == "use_launch_pk") || (command == "launch_pk") || (command == "intercept_pk"))
   {
      std::string msg = "The '" + command + "' command is incompatible with " + cMY_TYPE + ".";
      throw UtInput::BadValue(aInput, msg);
   }
   else if (command == "update_interval")
   {
      aInput.ReadValueOfType(mUpdateInterval, UtInput::cTIME);
      aInput.ValueGreater(mUpdateInterval, 0.0);
   }
   else if (MFK_Table::LoadInstance(aInput, tablePtr))
   {
      if (!Add(*tablePtr))
      {
         std::string msg = "Could not add lethality table " + tablePtr->GetName() + ", as it already exists.";
         delete tablePtr;
         throw UtInput::BadValue(aInput, msg);
      }
      delete tablePtr;
      tablePtr = nullptr;
   }
   else if (TargetType::LoadInstance(aInput, targetTypePtr))
   {
      if (!Add(*targetTypePtr))
      {
         std::string msg = "Could not add TargetType type " + targetTypePtr->GetName() + ", as it already exists.";
         delete targetTypePtr;
         throw UtInput::BadValue(aInput, msg);
      }
      delete targetTypePtr;
      targetTypePtr = nullptr;
   }
   else
   {
      // Test the possibility that command is for the base class.
      myCommand = WsfExplicitWeaponEffects::ProcessInput(aInput);
   }

   return myCommand;
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::Add(const MFK_Table& aTable)
{
   if (!mSharedMFK_TablesPtr)
   {
      mSharedMFK_TablesPtr = std::make_shared<TableCollection>();
   }

   // Add values only if there is no ambiguity.  (Do not permit duplicates.)
   TableCollection::iterator it = mSharedMFK_TablesPtr->begin();
   while (it != mSharedMFK_TablesPtr->end())
   {
      if (it->GetNameId() == aTable.GetNameId())
      {
         // We already have one of these, cannot add another.
         return false;
      }
      ++it;
   }

   // The add operation is successful.
   mSharedMFK_TablesPtr->push_back(aTable);
   return true;
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::Add(const TargetType& aTargetType)
{
   if (!mSharedTargetTypesPtr)
   {
      mSharedTargetTypesPtr = std::make_shared<TargetTypeCollection>();
   }

   // Add values only if there is no ambiguity.  (Do not permit duplicates.)
   TargetTypeCollection::iterator it = mSharedTargetTypesPtr->begin();
   while (it != mSharedTargetTypesPtr->end())
   {
      if (it->GetNameId() == aTargetType.GetNameId())
      {
         // We already have one of these, cannot add another.
         return false;
      }
      ++it;
   }

   mSharedTargetTypesPtr->push_back(aTargetType);
   return true;
}

// ===============================================================================================
// virtual
WsfMobilityAndFirepowerLethality::TargetType* WsfMobilityAndFirepowerLethality::GetTargetType(WsfStringId aTargetTypeId)
{
   if (!mSharedTargetTypesPtr)
   {
      return nullptr;
   }

   // Search our collection for the table for the given platform type.
   TargetTypeCollection::iterator it = mSharedTargetTypesPtr->begin();
   while (it != mSharedTargetTypesPtr->end())
   {
      // string the type = it->GetNameId().GetString();
      if (it->GetNameId() == aTargetTypeId)
      {
         return &(*it);
      }
      ++it;
   }
   return nullptr;
}

// ===============================================================================================
WsfMobilityAndFirepowerLethality::MFK_Table* WsfMobilityAndFirepowerLethality::GetTable(WsfStringId aTableNameId)
{
   if (!mSharedMFK_TablesPtr)
   {
      return nullptr;
   }

   // Search our collection for the table with the given name.
   TableCollection::iterator it = mSharedMFK_TablesPtr->begin();
   while (it != mSharedMFK_TablesPtr->end())
   {
      // string name = it->GetNameId().GetString();
      if (it->GetNameId() == aTableNameId)
      {
         return &(*it);
      }
      ++it;
   }
   return nullptr;
}

// ===============================================================================================
void WsfMobilityAndFirepowerLethality::Update(double aSimTime)
{
   // If any CM Update() returns true, that means we should detonate this flyout.
   bool detonate = false;
   for (size_t i = 0; i != mCM_StatePtrs.size(); ++i)
   {
      if (mCM_StatePtrs[i]->Update(aSimTime))
      {
         detonate = true;
      }
   }
   if (detonate)
   {
      Detonate(aSimTime);
   }
}

// ===============================================================================================
void WsfMobilityAndFirepowerLethality::WeaponFired(double                     aSimTime,
                                                   const WsfWeaponEngagement* aEngagementPtr,
                                                   const WsfTrack*            aTrackPtr)
{
   // For this weapon firing to be of interest to us, the firing must be explicit,
   // and the weapon platform type must be of the countermeasure types that this
   // is vulnerable to.  (I thought about considering CMs fired only from the target
   // platform, but we will allow a tactic where a wing-man fires CMs to attempt to
   // protect his buddy.)  (I also thought about requiring the target to be my
   // weaponPlatform, but assume CM's do not have sensor/trackers, and so are not
   // fired to defeat a particular WsfTrack.  Target is probably unknown to the CM itself.)

   WsfPlatform* cmPlatPtr = aEngagementPtr->GetWeaponPlatform();
   if (cmPlatPtr != nullptr)
   {
      // Explicit Weapon Engagement.
      bool attemptedToAdd = false;

      const WsfObject::TypeList& cmTypeIds = cmPlatPtr->GetTypeList();
      for (size_t i = 0; ((!attemptedToAdd) && (i != cmTypeIds.size())); ++i)
      {
         Vulnerability* vulPtr;
         if (mTargetTypeInUsePtr->FindVulnerabilityTo(cmTypeIds[i], vulPtr))
         {
            WsfPlatform* missilePtr = GetEngagement()->GetWeaponPlatform();
            WsfPlatform* targetPtr  = GetEngagement()->GetTargetPlatform();
            if (missilePtr != nullptr && targetPtr != nullptr)
            {
               attemptedToAdd = true;
               /*bool added = */
               AddCM_State(aSimTime, cmTypeIds[i], cmPlatPtr, missilePtr, targetPtr, vulPtr);
            }
         }
      }
   }
}

// ===============================================================================================
void WsfMobilityAndFirepowerLethality::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   CM_StatePtrCollection::iterator it = mCM_StatePtrs.begin();
   while (it != mCM_StatePtrs.end())
   {
      if (((*it)->CM_Ptr() == aPlatformPtr) || ((*it)->TgtPtr() == aPlatformPtr))
      {
         (*it)->Null();
      }
      ++it;
   }
}

// ===============================================================================================
void WsfMobilityAndFirepowerLethality::GetTableAndScalarFor(WsfPlatform*      aTargetPtr,
                                                            const MFK_Table*& aTablePtr,
                                                            double&           aPkFactor)
{
   aPkFactor = 1.0;
   TargetType*                targetTypePtr(nullptr);
   const WsfObject::TypeList& tgtTypeIds = aTargetPtr->GetTypeList();
   for (size_t i = 0; i != tgtTypeIds.size(); ++i)
   {
      targetTypePtr = GetTargetType(tgtTypeIds[i]);
      if (targetTypePtr != nullptr)
      {
         break;
      }
   }
   if (targetTypePtr == nullptr)
   {
      targetTypePtr = GetTargetType(cDEFAULT);
   }
   assert(targetTypePtr != nullptr);

   aTablePtr = targetTypePtr->GetTable();

   // Now loop through the CM states (known active CMs), and see if they are
   // "effective", meaning that geometry and time have been favorable to their
   // having an impact or effect on this engagement.  Multiple CM types can be
   // "effective" at the same time, but _ONLY_ if the effect is to reduce Pk
   // by a factor...  we cannot simultaneously apply more than one table!
   for (size_t i = 0; i != mCM_StatePtrs.size(); ++i)
   {
      CM_State* ptr = mCM_StatePtrs[i];
      if (ptr->IsEffective())
      {
         const MFK_Table* tPtr = ptr->GetVulnerability()->GetAlternateTablePtr();
         if (tPtr != nullptr)
         {
            aTablePtr = tPtr; // Only one table can be active at a time!
         }
         else if (ptr->GetVulnerability()->GetPkFactor() != 1.0)
         {
            // But multiple Pk degrades can be applied to the same engagement.
            aPkFactor *= ptr->GetVulnerability()->GetPkFactor();
         }
      }
   }
}

// ===============================================================================================
void WsfMobilityAndFirepowerLethality::Detonate(double aSimTime)
{
   // Find the weapon fuse, and cause it to detonate:
   WsfPlatform* wpnPlatPtr = GetEngagement()->GetWeaponPlatform();
   if (wpnPlatPtr != nullptr)
   {
      for (WsfComponentList::RoleIterator<WsfProcessor> iter(*wpnPlatPtr); !iter.AtEnd(); ++iter)
      {
         WsfWeaponFuse* fusePtr = dynamic_cast<WsfWeaponFuse*>(*iter);
         if (fusePtr != nullptr)
         {
            fusePtr->Detonate(aSimTime);
            break;
         }
      }
   }
}

// ===============================================================================================
// protected virtual
void WsfMobilityAndFirepowerLethality::ApplyEffectTo(double aSimTime, WsfPlatform* aOtherPlatformPtr)
{
   bool   affectAlreadyApplied    = false;
   double platformDamageIncrement = 0.0;

   // Traverse the list of object type IDs to find the first match.
   // If no match, then use the "default" type as the lethality.
   const MFK_Table* tablePtr = nullptr;
   double           pkScalar = 1.0;
   GetTableAndScalarFor(aOtherPlatformPtr, tablePtr, pkScalar);

   // We will not use the Pk obtained from GetPkConstraint() here,
   // but we need to know if it has been zeroed or degraded elsewhere:
   double pk = GetPkConstraint();
   if (Defeated())
   {
      pk = 0.0;
   }

   if (pk > 0.0) // Only process non-zero Pk as zero is always a miss
   {
      if (aOtherPlatformPtr->IsIndestructible())
      {
         // Use the base class implementation
         WsfWeaponEffects::ApplyEffectTo(aSimTime, aOtherPlatformPtr);
         affectAlreadyApplied = true;
      }
      else
      {
         // We already know that the weapon fly-by platform is in lethal range, or we would not be here.
         // Throw the dice, and get a number between zero and one...
         // If the dice value is less than the PK, we have some kind of a kill:
         // Reduce the dice throw (Pk degrade) if the above pk value is less than one.

         double   draw = DrawUniformPk();
         KillType kill = cNO_KILL;
         if (tablePtr != nullptr)
         {
            kill = tablePtr->KillTypeGivenUniformDraw(draw, pkScalar);
         }

         double damageLimit = cUNHARMED;
         switch (kill)
         {
         case cFIREPOWER:
            damageLimit = cSLIGHT_DAMAGE;
            break;
         case cMOBILITY:
            damageLimit = cSLIGHT_DAMAGE;
            break;
         case cFIREPOWER_AND_MOBILITY:
            damageLimit = cMODERATE_DAMAGE;
            break;
         case cCATASTROPHIC_KILL:
            damageLimit = cMORTALLY_WOUNDED;
         default:
            break;
         }

         if (DebugEnabled())
         {
            auto out = ut::log::debug();
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Weapon: " << MyFullName();
            if (kill != KillType::cNO_KILL)
            {
               out << "Weapon has killed platform.";
               out.AddNote() << "Kill Type: " << KillTypeName(kill);
               out.AddNote() << "Affected Platform: " << aOtherPlatformPtr->GetName();
            }
            else
            {
               out << "Weapon did not kill platform.";
               out.AddNote() << "Affected Platform: " << aOtherPlatformPtr->GetName();
            }
         }

         // We have just inflicted (Zero, M, F, MF, K) kill upon an adversary.
         // The kill amount will never decrease, but may not increase, if that
         // kill type has already been inflicted upon the target.  So first get
         // his current kill status:
         double initialPlatformDamage = aOtherPlatformPtr->GetDamageFactor();

         // Compute if we did any further damage or not:
         platformDamageIncrement = (initialPlatformDamage >= damageLimit) ? 0.0 : damageLimit - initialPlatformDamage;

         if (kill == cFIREPOWER || kill == cFIREPOWER_AND_MOBILITY)
         {
            // This is a new firepower kill.  Initial implementation is to kill all weapons on the platform.
            for (WsfComponentList::RoleIterator<WsfWeapon> iter(*aOtherPlatformPtr); !iter.AtEnd(); ++iter)
            {
               double currentDamage = iter->GetDamageFactor();
               if (currentDamage < cMORTALLY_WOUNDED)
               {
                  // Initial implementation is to kill all weapons on the platform.
                  iter->SetDamageFactor(aSimTime, 1.0);
               }
            }
         }

         if (kill == cMOBILITY || kill == cFIREPOWER_AND_MOBILITY)
         {
            WsfMover* moverPtr = aOtherPlatformPtr->GetMover();
            if (moverPtr != nullptr)
            {
               if (moverPtr->GetDamageFactor() < cMORTALLY_WOUNDED)
               {
                  // This is a new mobility kill.
                  aOtherPlatformPtr->GetMover()->SetDamageFactor(aSimTime, 1.0);
               }
            }
         }

         if ((kill != cNO_KILL) && (platformDamageIncrement == 0.0))
         {
            // If our kill type was non-trivial, but this damage level was already
            // sustained by the target, then there is danger that this will be
            // considered a MISS, so continue to sustain SOME amount of damage:
            platformDamageIncrement = 0.01;
         }
      } // end if IsIndestructible
   }

   if (!affectAlreadyApplied)
   {
      ApplyEffectIncrement(aSimTime, aOtherPlatformPtr, platformDamageIncrement);
   }
}

// =========================================== SUB_CLASS TABLE ==========================================
// =========================================== SUB_CLASS TABLE ==========================================
// static
bool WsfMobilityAndFirepowerLethality::MFK_Table::LoadInstance(UtInput& aInput, MFK_Table*& aTablePtr)
{
   bool        loaded = false;
   std::string command(aInput.GetCommand());

   if ((command == "table") || (command == "mfk_table"))
   {
      // This is a new Lethality table for a new type of platform.
      loaded = true;
      delete aTablePtr;
      std::string typeName;
      aInput.ReadValue(typeName);
      aTablePtr = new MFK_Table(typeName);
      UtInputBlock block(aInput, "end_" + command);
      while (block.ReadCommand())
      {
         if (!aTablePtr->ProcessInput(block.GetInput()))
         {
            throw UtInput::UnknownCommand(block.GetInput());
         }
      }
   }
   return loaded;
}

// ===============================================================================================
WsfMobilityAndFirepowerLethality::MFK_Table::MFK_Table(WsfStringId aTypeNameId)
   : WsfNamed(aTypeNameId)
   , mIsInitialized(false)
   , mPdGivenProximity(0.50)
   , mP_MK_GivenDamage(0.15)
   , mP_FK_GivenDamage(0.15)
   , mP_KK_GivenDamage(0.50)
{
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::MFK_Table::Initialize(double aSimTime, WsfMobilityAndFirepowerLethality* aLethalityPtr)
{
   // If this gets called more than once, the other calls are redundant.
   if (mIsInitialized)
   {
      return true;
   }
   double total   = mP_MK_GivenDamage + mP_FK_GivenDamage + mP_KK_GivenDamage;
   mIsInitialized = total <= 1.0;
   if (!mIsInitialized)
   {
      ut::log::error() << "MFK Table specified probabilities that do not sum to Unity.";
   }
   return mIsInitialized;
}

// ===============================================================================================================
// virtual
bool WsfMobilityAndFirepowerLethality::MFK_Table::ProcessInput(UtInput& aInput)
{
   // Read values into the table for each probability needed.
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "probability_of_damage_given_proximity_hit")
   {
      aInput.ReadValue(mPdGivenProximity);
      aInput.ValueInClosedRange(mPdGivenProximity, 0.0, 1.0);
   }
   else if (command == "probability_of_mk_given_damage")
   {
      aInput.ReadValue(mP_MK_GivenDamage);
      aInput.ValueInClosedRange(mP_MK_GivenDamage, 0.0, 1.0);
   }
   else if (command == "probability_of_fk_given_damage")
   {
      aInput.ReadValue(mP_FK_GivenDamage);
      aInput.ValueInClosedRange(mP_FK_GivenDamage, 0.0, 1.0);
   }
   else if (command == "probability_of_kk_given_damage")
   {
      aInput.ReadValue(mP_KK_GivenDamage);
      aInput.ValueInClosedRange(mP_KK_GivenDamage, 0.0, 1.0);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ===============================================================================================================
WsfMobilityAndFirepowerLethality::KillType
WsfMobilityAndFirepowerLethality::MFK_Table::KillTypeGivenUniformDraw(double aUniformDrawValue, double aPkFactor) const
{
   KillType result = cNO_KILL;

   // Assemble "Kill Thermometer" values:
   double Threshold_No_Kill = 1.0 - (mPdGivenProximity * aPkFactor);
   double Threshold_M_Kill  = Threshold_No_Kill + mP_MK_GivenDamage * mPdGivenProximity;
   double Threshold_F_Kill  = Threshold_M_Kill + mP_FK_GivenDamage * mPdGivenProximity;
   double remainingProb     = (1.0 - (mP_MK_GivenDamage + mP_FK_GivenDamage + mP_KK_GivenDamage));
   double Threshold_MF_Kill = Threshold_F_Kill + remainingProb * mPdGivenProximity;

   // If the uniform draw exceeds increasing thresholds, the kill type elevates.
   if (aUniformDrawValue < Threshold_No_Kill)
   {
      result = cNO_KILL;
   }
   else if (aUniformDrawValue < Threshold_M_Kill)
   {
      // Statement to handle Mobility Kill
      result = cMOBILITY;
   }
   else if (aUniformDrawValue < Threshold_F_Kill)
   {
      // Statement to handle Firepower Kill
      result = cFIREPOWER;
   }
   else if (aUniformDrawValue < Threshold_MF_Kill)
   {
      // Statement to handle Mobility and Firepower Kill
      result = cFIREPOWER_AND_MOBILITY;
   }
   else
   {
      // Statement to handle Catastrophic Kill
      result = cCATASTROPHIC_KILL;
   }

   return result;
}

// ===================================== SUB_CLASS Vulnerability ====================================
// ===================================== SUB_CLASS Vulnerability ====================================
// static
bool WsfMobilityAndFirepowerLethality::Vulnerability::LoadInstance(UtInput& aInput, Vulnerability*& aCM_EffectPtr)
{
   bool        loaded = false;
   std::string command(aInput.GetCommand());

   if (command == "vulnerability")
   {
      // This is a new Lethality table for a new type of platform.
      loaded = true;
      delete aCM_EffectPtr;
      std::string typeName;
      aInput.ReadValue(typeName);
      aCM_EffectPtr = new Vulnerability(typeName);
      UtInputBlock block(aInput, "end_" + command);
      while (block.ReadCommand())
      {
         if (!aCM_EffectPtr->ProcessInput(block.GetInput()))
         {
            throw UtInput::UnknownCommand(block.GetInput());
         }
      }
   }
   return loaded;
}

// ===============================================================================================
WsfMobilityAndFirepowerLethality::Vulnerability::Vulnerability(WsfStringId aTypeNameId)
   : WsfNamed(aTypeNameId)
   , mIsInitialized(false)
   , mAlternateTableId(nullptr)
   , mAlternateTablePtr(nullptr)
   , mPkFactor(1.0)
   , mProbWpnDefeat(0.0)
   //,mMinimumQuantity(1)
   , mMinimumDuration(0.0)
   , mMaximumHalfCone(179.0 * UtMath::cRAD_PER_DEG)
   , mMinimumDistance(0.0)
   , mMaximumDistance(DBL_MAX)
   , mDistValue(cMSL_TO_TGT)
{
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::Vulnerability::Initialize(double                            aSimTime,
                                                                 WsfMobilityAndFirepowerLethality* aLethalityPtr)
{
   // If this gets called more than once, the other calls are redundant.
   if (mIsInitialized)
   {
      return true;
   }
   mAlternateTablePtr = aLethalityPtr->GetTable(mAlternateTableId);
   bool useTable      = (mAlternateTablePtr != nullptr);
   bool usePk         = mPkFactor != 1.0;
   // Here we enforce that we can use an alternate table, or a Pk-Factor, but not both:
   mIsInitialized = (useTable || usePk) && (!(usePk && useTable));
   if (!mIsInitialized)
   {
      ut::log::error() << "CM Vulnerabilities may use PkFactor or Table, but not both.";
   }
   return mIsInitialized;
}

// ================================================================================================
// virtual
bool WsfMobilityAndFirepowerLethality::Vulnerability::ProcessInput(UtInput& aInput)
{
   // Read values into the table for each probability needed.
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "minimum_duration")
   {
      aInput.ReadValueOfType(mMinimumDuration, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mMinimumDuration, 0.0);
   }
   else if (command == "maximum_half_cone_angle")
   {
      aInput.ReadValueOfType(mMaximumHalfCone, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mMaximumHalfCone, 0.0);
   }
   else if (command == "maximum_distance")
   {
      aInput.ReadValueOfType(mMaximumDistance, UtInput::cLENGTH);
   }
   else if (command == "minimum_distance")
   {
      aInput.ReadValueOfType(mMinimumDistance, UtInput::cLENGTH);
   }
   else if (command == "distance_value")
   {
      std::string distValue;
      aInput.ReadValue(distValue);
      if (distValue == "missile_to_target")
      {
         mDistValue = cMSL_TO_TGT;
      }
      else if (distValue == "countermeasure_to_target")
      {
         mDistValue = cCM_TO_TGT;
      }
      else if (distValue == "missile_to_countermeasure")
      {
         mDistValue = cMSL_TO_CM;
      }
      else
      {
         std::string a   = "Unknown 'distance_value':  options are missile_to_target, ";
         std::string b   = "countermeasure_to_target, or missile_to_countermeasure.";
         std::string msg = a + b;
         throw UtInput::BadValue(aInput, msg);
      }
   }
   else if (command == "pk_factor")
   {
      aInput.ReadValue(mPkFactor);
      aInput.ValueInClosedRange(mPkFactor, 0.0, 1.0);

      // If a pk_factor is provided, then _remove_ the cDEFAULT table name...
      if (mAlternateTableId == cDEFAULT)
      {
         mAlternateTableId = nullptr;
      }
   }
   else if (command == "probability_of_weapon_defeat")
   {
      aInput.ReadValue(mProbWpnDefeat);
      aInput.ValueInClosedRange(mProbWpnDefeat, 0.0, 1.0);
   }
   else if (command == "alternate_mfk_table_name")
   {
      std::string altTableName;
      aInput.ReadValue(altTableName);
      mAlternateTableId.SetString(altTableName);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::FindTargetType(WsfStringId aTargetTypeId, TargetType*& aTargetTypePtr)
{
   TargetTypeCollection::iterator ttIter = mSharedTargetTypesPtr->begin();
   while (ttIter != mSharedTargetTypesPtr->end())
   {
      if (ttIter->GetTargetTypeId() == aTargetTypeId)
      {
         aTargetTypePtr = &(*ttIter);
         return true;
      }
      ++ttIter;
   }
   return false;
}

// =========================================== SUB_CLASS TargetType ==========================================
// =========================================== SUB_CLASS TargetType ==========================================
// static
bool WsfMobilityAndFirepowerLethality::TargetType::LoadInstance(UtInput& aInput, TargetType*& aTargetTypePtr)
{
   bool        loaded = false;
   std::string command(aInput.GetCommand());

   if (command == "target_type")
   {
      // This is a new Lethality table for a new type of platform.
      loaded = true;
      delete aTargetTypePtr;
      std::string typeName;
      aInput.ReadValue(typeName);
      aTargetTypePtr = new TargetType(typeName);
      UtInputBlock block(aInput, "end_" + command);
      while (block.ReadCommand())
      {
         if (!aTargetTypePtr->ProcessInput(block.GetInput()))
         {
            throw UtInput::UnknownCommand(block.GetInput());
         }
      }
   }
   return loaded;
}

// ===============================================================================================
WsfMobilityAndFirepowerLethality::TargetType::TargetType(WsfStringId aTypeNameId)
   : WsfNamed(aTypeNameId)
   , mIsInitialized(false)
   , mCMsNotUsedTableNameId(cDEFAULT)
   , mCMsNotUsedTablePtr()
   , mVulnerabilities()
{
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::TargetType::Initialize(double                            aSimTime,
                                                              WsfMobilityAndFirepowerLethality* aLethalityPtr)
{
   // If this gets called more than once, the other calls are redundant.
   if (mIsInitialized)
   {
      return true;
   }

   mIsInitialized      = true;
   mCMsNotUsedTablePtr = aLethalityPtr->GetTable(mCMsNotUsedTableNameId);
   if (mCMsNotUsedTablePtr == nullptr)
   {
      auto out = ut::log::error() << "Table was not found.";
      out.AddNote() << "Name: " << GetName();
      mIsInitialized = false;
   }

   if (mIsInitialized)
   {
      for (size_t i = 0; i != mVulnerabilities.size(); ++i)
      {
         if (!mVulnerabilities[i].Initialize(aSimTime, aLethalityPtr))
         {
            mIsInitialized = false;
         }
      }
   }
   return mIsInitialized;
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::TargetType::Add(const Vulnerability& aVulnerability)
{
   // Add values only if there is no ambiguity.  (Do not permit duplicates.)
   Vulnerability* vulPtrNotUsed = nullptr;
   if (FindVulnerabilityTo(aVulnerability.GetCM_TypeId(), vulPtrNotUsed))
   {
      // if pointer is non-zero, cannot add, as aVulnerability is a duplicate.
      return false;
   }
   mVulnerabilities.push_back(aVulnerability);
   return true;
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::TargetType::FindVulnerabilityTo(
   WsfStringId                                       aCM_TypeId,
   WsfMobilityAndFirepowerLethality::Vulnerability*& aVulnerabilityPtr)
{
   // Search our collection for the vulnerability to a given CM type.
   for (size_t i = 0; i != mVulnerabilities.size(); ++i)
   {
      if (mVulnerabilities[i].GetCM_TypeId() == aCM_TypeId)
      {
         aVulnerabilityPtr = &mVulnerabilities[i];
         return true;
      }
   }
   return false;
}

// ================================================================================================
// virtual
bool WsfMobilityAndFirepowerLethality::TargetType::ProcessInput(UtInput& aInput)
{
   // Read values into the table for each probability needed.
   bool           myCommand = true;
   std::string    command(aInput.GetCommand());
   Vulnerability* cmEffectPtr = nullptr;

   if (command == "mfk_table_name")
   {
      std::string theName;
      aInput.ReadValue(theName);
      mCMsNotUsedTableNameId.SetString(theName);
   }
   else if (Vulnerability::LoadInstance(aInput, cmEffectPtr))
   {
      if (!Add(*cmEffectPtr))
      {
         std::string msg = "Could not add Vulnerability type " + cmEffectPtr->GetName() + ", as it already exists.";
         delete cmEffectPtr;
         throw UtInput::BadValue(aInput, msg);
      }
      delete cmEffectPtr;
      cmEffectPtr = nullptr;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =========================================== SUB_CLASS CM_State ==========================================
// =========================================== SUB_CLASS CM_State ==========================================
WsfMobilityAndFirepowerLethality::CM_State::CM_State(double               aSimTime,
                                                     WsfStringId          aCM_TypeId,
                                                     WsfPlatform*         aCountermeasurePtr,
                                                     WsfPlatform*         aMissilePtr,
                                                     WsfPlatform*         aTargetPtr,
                                                     const Vulnerability* aCM_EffectPtr,
                                                     WsfSimulation*       aSimulationPtr)
   : mCM_Index(aCountermeasurePtr->GetIndex())
   , mCM_TypeId(aCM_TypeId)
   , mCM_Ptr(aCountermeasurePtr)
   , mTargetPtr(aTargetPtr)
   , mMissilePtr(aMissilePtr)
   , mEffectPtr(aCM_EffectPtr)
   , mSimulationPtr(aSimulationPtr)
   , mIsEffective(false)
   , mIsFinalized(false)
   , mCumEffectiveTime(0.0)
   , mLastUpdate(aSimTime)
{
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::CM_State::Update(double aSimTime)
{
   if (mLastUpdate >= aSimTime)
   {
      return false;
   }

   // If Nulled, do nothing further.
   if (mIsFinalized)
   {
      return false;
   }
   if (mCM_Ptr == nullptr)
   {
      return false;
   }

   assert(mTargetPtr != nullptr);
   assert(mMissilePtr != nullptr);
   assert(mEffectPtr != nullptr);

   // Now, to find out if a countermeasure has an effect on the engagement, there
   // are four/five things we must check that are satisfied simultaneously:
   // Consider the target-to-missile-to-CM geometry.  Look at distances, angles,
   // and durations, and if all are satisfied simultaneously, set the IsEffective flag.

   // Get the three geometric points we are interested in:

   UtVec3dX missileLocWCS;
   mMissilePtr->Update(aSimTime);
   mMissilePtr->GetLocationWCS(missileLocWCS.GetData());

   UtVec3dX targetLocWCS;
   mTargetPtr->Update(aSimTime);
   mTargetPtr->GetLocationWCS(targetLocWCS.GetData());

   UtVec3dX counterLocWCS;
   mCM_Ptr->Update(aSimTime);
   mCM_Ptr->GetLocationWCS(counterLocWCS.GetData());

   UtVec3dX mslToTgt = targetLocWCS - missileLocWCS;
   UtVec3dX mslToCM  = counterLocWCS - missileLocWCS;
   UtVec3dX cmToTgt;

   double halfCone = mslToTgt.AngleBetween(mslToCM);
   double distance = 0.0;
   switch (mEffectPtr->GetDistEnum())
   {
   case Vulnerability::cMSL_TO_TGT:
      distance = mslToTgt.Magnitude();
      break;
   case Vulnerability::cCM_TO_TGT:
      cmToTgt  = targetLocWCS - counterLocWCS;
      distance = cmToTgt.Magnitude();
      break;
   case Vulnerability::cMSL_TO_CM:
      distance = mslToCM.Magnitude();
      break;
   default:
      break;
   }

   bool distIsOk  = (mEffectPtr->GetMinDist() <= distance) && (distance <= mEffectPtr->GetMaxDist());
   bool angleIsOk = halfCone <= mEffectPtr->GetMaxHalfCone();

   // Now, if is currently in constraints, then increment the amount of time
   // in constraints.  If the amount of time in constraints exceeds threshold,
   // flag as valid.
   if (distIsOk && angleIsOk)
   {
      mCumEffectiveTime += (aSimTime - mLastUpdate);
   }

   bool detonate = false;
   if (mCumEffectiveTime >= mEffectPtr->GetMinDuration())
   {
      mIsEffective = true;
      detonate     = Finalize();
   }

   mLastUpdate = aSimTime;

   return detonate;
}

// ===============================================================================================
bool WsfMobilityAndFirepowerLethality::CM_State::Finalize()
{
   if (mIsFinalized)
   {
      return false;
   }

   // It is now that we throw the dice (if needed), to determine if
   // this CM defeats the guidance of the incoming missile.
   // If so, detonate it:
   bool   detonate = false;
   double Pwd      = mEffectPtr->GetProbabilityOfDefeat();
   if (Pwd > 0.0)
   {
      if (mSimulationPtr->GetRandom().Bernoulli(Pwd))
      {
         // This will cause a detonation.
         detonate = true;
      }
   }
   mIsFinalized = true;
   Null();

   return detonate;
}

// ===============================================================================================
void WsfMobilityAndFirepowerLethality::CM_State::Null()
{
   mCM_Ptr    = nullptr;
   mTargetPtr = nullptr;
   // mEffectPtr = 0; // Do not do this.
}

// ===================================== SUB_CLASS UpdateEvent ====================================
// ===================================== SUB_CLASS UpdateEvent ====================================
WsfMobilityAndFirepowerLethality::UpdateEvent::UpdateEvent(double aSimTime, WsfMobilityAndFirepowerLethality* aLethalityPtr)
   : WsfEvent(aSimTime)
   , mPlatIndex(0)
   , mLethalityPtr(aLethalityPtr)
{
   mPlatIndex = mLethalityPtr->GetEngagement()->GetWeaponPlatform()->GetIndex();
}

// ===============================================================================================
WsfEvent::EventDisposition WsfMobilityAndFirepowerLethality::UpdateEvent::Execute()
{
   if (GetSimulation()->PlatformExists(mPlatIndex))
   {
      double t  = GetTime();
      double dt = mLethalityPtr->UpdateInterval();
      mLethalityPtr->Update(t);
      SetTime(t + dt);
      return WsfEvent::cRESCHEDULE;
   }
   return WsfEvent::cDELETE;
}
