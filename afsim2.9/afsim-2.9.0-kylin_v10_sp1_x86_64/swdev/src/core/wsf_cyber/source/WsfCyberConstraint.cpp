// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Radiance Technologies, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberConstraint.hpp"

#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfCyberConstraintTypes.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace cyber
{
wsf::cyber::Constraint::Constraint()
   : WsfPlatformComponent()
   , mAttackData()
   , mResources(0.0)
   , mTotalResources(0.0)
{
   SetName("WsfCyberConstraint");
}

const int* wsf::cyber::Constraint::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_CYBER_CONSTRAINT, cWSF_COMPONENT_NULL};
   return roles;
}

// virtual
WsfComponent* wsf::cyber::Constraint::CloneComponent() const
{
   return new Constraint(*this);
}

// virtual
void* wsf::cyber::Constraint::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_CYBER_CONSTRAINT)
   {
      return this;
   }
   else
   {
      return nullptr;
   }
}

// =================================================================================================
// static
Constraint* Constraint::Find(const WsfPlatform& aPlatform)
{
   return static_cast<Constraint*>(aPlatform.FindComponentByRole(cCOMPONENT_ROLE<Constraint>()));
}

Constraint* Constraint::FindOrCreate(WsfPlatform& aPlatform)
{
   Constraint* componentPtr = Find(aPlatform);
   if (componentPtr == nullptr)
   {
      componentPtr = new Constraint();
      const_cast<WsfPlatformComponentList&>(aPlatform.GetComponents()).AddComponent(componentPtr);
   }
   return componentPtr;
}

// virtual
bool wsf::cyber::Constraint::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "resources")
   {
      aInput.ReadValue(mTotalResources);
      mResources = mTotalResources;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

size_t Constraint::GetConcurrentAttacks(const std::string& aAttackType) const
{
   size_t concurrentAttacks = 0;

   auto attackList = mAttackData.find(aAttackType);

   if (attackList != mAttackData.end())
   {
      concurrentAttacks = attackList->second.mConcurrentEngagements.size();
   }

   return concurrentAttacks;
}

void Constraint::AddConcurrentAttack(const std::string& aAttackType, size_t aEngagementID)
{
   mAttackData[aAttackType].mConcurrentEngagements.push_back(aEngagementID);
}

void Constraint::RemoveConcurrentAttack(const std::string& aAttackType, size_t aEngagementID)
{
   auto attackList = mAttackData.find(aAttackType);

   if (attackList != mAttackData.end())
   {
      for (auto it = attackList->second.mConcurrentEngagements.begin();
           it != attackList->second.mConcurrentEngagements.end();)
      {
         if (aEngagementID == *it)
         {
            it = attackList->second.mConcurrentEngagements.erase(it);
         }
         else
         {
            ++it;
         }
      }
   }
}

void Constraint::AddAttackTime(const std::string& aAttackType, double aSimTime)
{
   mAttackData[aAttackType].mAttackTimes.insert(aSimTime);
}

size_t Constraint::GetAttackCountAfterTime(const std::string& aAttackType, double aSimTime) const
{
   auto& attackTimes = mAttackData.at(aAttackType).mAttackTimes;

   auto pos = attackTimes.rbegin();
   while (pos != attackTimes.rend())
   {
      if (aSimTime > *pos)
      {
         break;
      }
      else
      {
         ++pos;
      }
   }

   return std::distance(pos.base(), attackTimes.end());
}

bool wsf::cyber::Constraint::RestoreResources(double aQuantity)
{
   if ((mResources + aQuantity) > mTotalResources)
   {
      return false;
   }
   else
   {
      mResources += aQuantity;
      return true;
   }
}

bool wsf::cyber::Constraint::RemoveResources(double aQuantity)
{
   if ((mResources - aQuantity) < 0)
   {
      return false;
   }
   else
   {
      mResources -= aQuantity;
      return true;
   }
}

ScriptConstraintClass::ScriptConstraintClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCyberConstraint");

   AddMethod(ut::make_unique<CurrentResources>());
   AddMethod(ut::make_unique<TotalResources>());

   AddMethod(ut::make_unique<ConcurrentAttacks>());
   AddMethod(ut::make_unique<AttackCountAfterTime>());
}

//! Retrieve the quantity of available cyber resources.
UT_DEFINE_SCRIPT_METHOD(ScriptConstraintClass, Constraint, CurrentResources, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCurrentResources());
}

//! Retrieve the quantity of total cyber resources.
UT_DEFINE_SCRIPT_METHOD(ScriptConstraintClass, Constraint, TotalResources, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTotalResources());
}

UT_DEFINE_SCRIPT_METHOD(ScriptConstraintClass, Constraint, ConcurrentAttacks, 1, "int", "string")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetConcurrentAttacks(aVarArgs[0].GetString())));
}

UT_DEFINE_SCRIPT_METHOD(ScriptConstraintClass, Constraint, AttackCountAfterTime, 2, "int", "string, double")
{
   aReturnVal.SetInt(
      ut::cast_to_int(aObjectPtr->GetAttackCountAfterTime(aVarArgs[0].GetString(), aVarArgs[1].GetDouble())));
}

} // namespace cyber

} // namespace wsf
