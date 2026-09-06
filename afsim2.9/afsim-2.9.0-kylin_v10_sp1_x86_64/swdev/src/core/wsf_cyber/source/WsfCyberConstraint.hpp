// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERCONSTRAINT_HPP
#define WSFCYBERCONSTRAINT_HPP

#include "wsf_cyber_export.h"

#include <string>

#include "UtRandom.hpp"
#include "UtScriptBasicTypes.hpp"
#include "WsfComponent.hpp"
#include "WsfCyberComponentRoles.hpp"
#include "WsfCyberRandom.hpp"
#include "WsfNamed.hpp"
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfPlatform.hpp"
#include "script/WsfScriptObjectClass.hpp"

namespace wsf
{
namespace cyber
{
//! A class that models the concept of a platform having unique factors that can restrict or degrade outgoing cyber
//! attacks. The class also has script hooks to allow for external script level C2 logic. For example a platform may
//! only conduct a particular attack x times/hour or have x concurrent attacks. If a platform does not have sufficient
//! resources available for an attack that requires resources then the attack will not be initiated.
class WSF_CYBER_EXPORT Constraint : public WsfPlatformComponent, public WsfObject
{
public:
   Constraint();

   //! @name Component infrastructure methods.
   //@{
   WsfStringId        GetComponentName() const override { return GetNameId(); }
   const int*         GetComponentRoles() const override;
   WsfComponent*      CloneComponent() const override;
   void*              QueryInterface(int aRole) override;
   static Constraint* Find(const WsfPlatform& aPlatform);
   static Constraint* FindOrCreate(WsfPlatform& aPlatform);
   //@}

   //! @name Framework methods.
   //@{
   bool        ProcessInput(UtInput& aInput) override;
   Constraint* Clone() const override { return new Constraint(*this); }
   //@}

   //! @name External C2 monitoring methods.
   //@{
   size_t GetConcurrentAttacks(const std::string& aAttackType) const;
   void   AddConcurrentAttack(const std::string& aAttackType, size_t aEngagementID);
   void   RemoveConcurrentAttack(const std::string& aAttackType, size_t aEngagementID);
   void   AddAttackTime(const std::string& aAttackType, double aSimTime);
   size_t GetAttackCountAfterTime(const std::string& aAttackType, double aSimTime) const;
   //@}

   //! @name Cyber resource methods
   //@{
   double GetCurrentResources() const { return mResources; }
   double GetTotalResources() const { return mTotalResources; }
   bool   RestoreResources(double aQuantity);
   bool   RemoveResources(double aQuantity);
   //@}

private:
   struct AttackInfo
   {
      std::vector<size_t> mConcurrentEngagements;
      // Multiset is used here in order to allow for multiple entries of the same value as well as keeping the data
      // structure stored in order
      std::multiset<double> mAttackTimes;
   };

   std::map<std::string, AttackInfo> mAttackData;

   double mResources{0.0};
   double mTotalResources{0.0};
   bool   mDefaultDefined{false};
};

class ScriptConstraintClass : public WsfScriptObjectClass
{
public:
   ScriptConstraintClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(CurrentResources);
   UT_DECLARE_SCRIPT_METHOD(TotalResources);

   UT_DECLARE_SCRIPT_METHOD(ConcurrentAttacks);
   UT_DECLARE_SCRIPT_METHOD(AttackCountAfterTime);
};

} // namespace cyber

} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::cyber::Constraint, cWSF_COMPONENT_CYBER_CONSTRAINT)

#endif
