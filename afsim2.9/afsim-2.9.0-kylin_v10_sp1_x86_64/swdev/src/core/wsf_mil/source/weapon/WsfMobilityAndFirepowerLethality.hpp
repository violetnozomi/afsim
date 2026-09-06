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

#ifndef WSFMOBILITYANDFIREPOWERLETHALITY_HPP
#define WSFMOBILITYANDFIREPOWERLETHALITY_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <vector>

#include "UtCallbackHolder.hpp"
#include "WsfEvent.hpp"
#include "WsfExplicitWeaponEffects.hpp"
#include "WsfNamed.hpp"

//! A specialization of the WsfLethality class which implements Firepower and Mobility Lethality.
//! Ref:  Game Programming Gems 4, ed. Andrew Kirmse (Charles River Media) ISBN I-58540-295-9
/*
From Ref, Chapter 3.1 - "Kills Come in Four Flavors".  Military simulations sometimes represent
four different types of kills often found in real-world combat. The first flavor is a mobility
kill in which the target is no longer able to move, but remains alive enough to fire its weapon
or communicate with other vehicles.  The second is a firepower kill in which the weapon is
damaged and cannot be fired, but the vehicle or person is still able to move and communicate.
The third is a mobility and firepower kill in which the vehicle or person is still alive, and may
observe, communicate, consume supplies, and may motivate a rescue mission, but cannot move or use
its weapon.   The final kill type is the catastrophic kill, essentially eliminating the platform.

Here, the lethality type is configurable by target type, and furthermore, each target type (or
his friends) may deploy countermeasures in the hopes of reducing or completely defeating the
incoming weapon.  All Weapon-Target lethality pairing Tables are kept in shared memory, to be
consulted during the flyout to determine the particular lethality values applicable to this
engagement.  Consult user documentation for configuration instructions.  Note that unlike most
engagements, this one will consume CPU periodically during the engagement to do timeline and
geometry updates, as the countermeasure effect is permitted to accumulate, gaining effectiveness
(decreasing the weapon lethality over time).

Synopsis:  1. A fired Weapon Platform has a Lethality (this).  2. The Lethality is against a
threat target type.  3.  The threat target type _may_ have a specific defined lethality to be
used during the engagement, but if one is not specified, a default lethality is selected anyway.
4.  Once a lethality is chosen for this engagement, there _may_ be mitigating countermeasures
types that are effective at reducing the previously expected lethality, but only after some
time, geometry, or proximity requirements are satisfied.

SPECIAL CAVEAT:  Model has been developed to be more easily exportable, to provide a notional
effect in an unclassified environment.  No sensor/signature or EM/EP techniques are modeled.
It is assumed here that <for example> the mere existence of FLARE platform(s) near the target
that satisfy constraints is sufficient to have a desired decoy effect.

ALSO NOTE:  The "vulnerability" tests are CPU expensive, and will not be made unless needed.
*/

class WSF_MIL_EXPORT WsfMobilityAndFirepowerLethality : public WsfExplicitWeaponEffects
{
public:
   enum KillType
   {
      cNO_KILL,
      cFIREPOWER,
      cMOBILITY,
      cFIREPOWER_AND_MOBILITY,
      cCATASTROPHIC_KILL
   };

   WsfMobilityAndFirepowerLethality(WsfScenario& aScenario);
   WsfMobilityAndFirepowerLethality(const WsfMobilityAndFirepowerLethality& aSrc);
   WsfMobilityAndFirepowerLethality& operator=(const WsfMobilityAndFirepowerLethality&) = delete;
   ~WsfMobilityAndFirepowerLethality() override;

   WsfWeaponEffects* Clone() const override;
   bool              Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr) override;
   bool              ProcessInput(UtInput& aInput) override;
   void              CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr) override;
   // Callback signatures:
   void Update(double aSimTime);
   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTrackPtr);
   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   std::string KillTypeName(KillType aType) const;
   std::string MyFullName() const;
   double      UpdateInterval() const { return mUpdateInterval; }

protected:
   static const double cSLIGHT_DAMAGE;
   static const double cMODERATE_DAMAGE;

   class UpdateEvent : public WsfEvent
   {
   public:
      UpdateEvent(double aSimTime, WsfMobilityAndFirepowerLethality* aLethalityPtr);
      EventDisposition Execute() override;

   private:
      size_t                            mPlatIndex;
      WsfMobilityAndFirepowerLethality* mLethalityPtr;
   };

   // MFK_Tables are shared by all instances of this class,
   // and an individual instance of this lethality will reference
   // the table type by nameId only.
   class MFK_Table : public WsfNamed
   {
   public:
      MFK_Table(WsfStringId aTypeNameId);
      static bool LoadInstance(UtInput& aInput, MFK_Table*& aTablePtr);
      bool        ProcessInput(UtInput& aInput);
      bool        Initialize(double aSimTime, WsfMobilityAndFirepowerLethality* aLethality);
      KillType    KillTypeGivenUniformDraw(double aUniformDrawValue, double aPkFactor = 1.0) const;
      double      PdGivenProximity() const { return mPdGivenProximity; }
      double      P_MK_GivenDamage() const { return mP_MK_GivenDamage; }
      double      P_FK_GivenDamage() const { return mP_FK_GivenDamage; }
      double      P_KK_GivenDamage() const { return mP_KK_GivenDamage; }

   private:
      bool   mIsInitialized;
      double mPdGivenProximity;
      double mP_MK_GivenDamage;
      double mP_FK_GivenDamage;
      double mP_KK_GivenDamage;
   };

   // Vulnerabilities are selected from a list, based FIRST upon the target type engaged
   // by this weapon, and SECOND, upon countermeasures types found in proximity to the
   // target of interest.  If countermeasures of the right type are present either close
   // enough, or within a tight enough half-cone of the target view, AND are present for
   // a long enough duration to be effective, then they will "re-route" the default
   // MFK_Table to one of lower probability (mAlternateTablePtr) of effectiveness.
   // (To prevent confusion, we should not use GetName(), GetNameId() on this class,
   //  but GetCM_TypeId() instead.)
   class Vulnerability : public WsfNamed // Name is CM _TYPE_ !!!
   {
   public:
      enum DistValue
      {
         cMSL_TO_TGT = 0,
         cCM_TO_TGT  = 1,
         cMSL_TO_CM  = 2
      };

      Vulnerability(WsfStringId aCM_TypeNameId);
      static bool      LoadInstance(UtInput& aInput, Vulnerability*& aCM_EffectPtr);
      bool             ProcessInput(UtInput& aInput);
      bool             Initialize(double aSimTime, WsfMobilityAndFirepowerLethality* aLethality);
      DistValue        GetDistEnum() const { return mDistValue; }
      double           GetMinDuration() const { return mMinimumDuration; }
      double           GetMaxHalfCone() const { return mMaximumHalfCone; }
      double           GetMaxDist() const { return mMaximumDistance; }
      double           GetMinDist() const { return mMinimumDistance; }
      double           GetPkFactor() const { return mPkFactor; }
      double           GetProbabilityOfDefeat() const { return mProbWpnDefeat; }
      WsfStringId      GetCM_TypeId() const { return GetNameId(); } // syntactical convenience
      const MFK_Table* GetAlternateTablePtr() const { return mAlternateTablePtr; }

   private:
      bool             mIsInitialized;
      WsfStringId      mAlternateTableId;  // Table or PkFactor are mutually exclusive.
      const MFK_Table* mAlternateTablePtr; // Table or PkFactor are mutually exclusive.
      double           mPkFactor;          // Table or PkFactor are mutually exclusive.
      double           mProbWpnDefeat;     // Probability this CM defeats a weapon intercept.
      double           mMinimumDuration;
      double           mMaximumHalfCone;
      double           mMinimumDistance;
      double           mMaximumDistance;
      DistValue        mDistValue;
   };

   // (To prevent confusion, we should not use GetName(), GetNameId() on this class,
   //  but GetTargetTypeId() instead.)
   class TargetType : public WsfNamed
   {
   public:
      static bool LoadInstance(UtInput& aInput, TargetType*& aTargetTypePtr);

      TargetType(WsfStringId aTypeNameId);

      bool ProcessInput(UtInput& aInput);

      bool Initialize(double aSimTime, WsfMobilityAndFirepowerLethality* aLethality);

      bool Add(const Vulnerability& aCM_Effect);

      bool FindVulnerabilityTo(WsfStringId aCM_TypeId, Vulnerability*& aVulnerabilityPtr);

      // bool FindCM_Type(WsfStringId     aCM_TypeId,
      //                  Vulnerability*& aVulnerabilityPtr);
      size_t         CM_EffectCount() { return mVulnerabilities.size(); }
      Vulnerability* CM_EffectEntry(size_t aIndex) { return &mVulnerabilities[aIndex]; }

      // KillType KillTypeGivenUniformDraw(double aUniformDrawValue);
      MFK_Table* GetTable() { return mCMsNotUsedTablePtr; }

      WsfStringId GetTargetTypeId() const { return GetNameId(); } // syntactical convenience

      bool ContainsVulnerabilities() const { return !mVulnerabilities.empty(); }

   private:
      bool                       mIsInitialized;
      WsfStringId                mCMsNotUsedTableNameId;
      MFK_Table*                 mCMsNotUsedTablePtr;
      std::vector<Vulnerability> mVulnerabilities;
   };

   class CM_State;
   using CM_StatePtrCollection = std::vector<CM_State*>;

   class CM_State
   {
   public:
      CM_State(double               aSimTime,
               WsfStringId          aCM_TypeId,
               WsfPlatform*         aCM_PlatPtr,
               WsfPlatform*         aMissilePtr,
               WsfPlatform*         aTargetPtr,
               const Vulnerability* aEffectPtr,
               WsfSimulation*       aSimulationPtr);

      // Return - true = Detonate;
      bool                 Update(double aSimTime);
      void                 Null();
      size_t               PlatIndex() const { return mCM_Index; }
      WsfStringId          CM_TypeId() const { return mCM_TypeId; }
      WsfPlatform*         CM_Ptr() { return mCM_Ptr; }
      WsfPlatform*         TgtPtr() { return mTargetPtr; }
      bool                 IsEffective() const { return mIsEffective; }
      const Vulnerability* GetVulnerability() const { return mEffectPtr; }

   private:
      // Return - true = Munition should Detonate.
      bool Finalize();

      size_t               mCM_Index;
      WsfStringId          mCM_TypeId;
      WsfPlatform*         mCM_Ptr;
      WsfPlatform*         mTargetPtr;
      WsfPlatform*         mMissilePtr;
      const Vulnerability* mEffectPtr;
      WsfSimulation*       mSimulationPtr;
      bool                 mIsEffective;
      bool                 mIsFinalized;
      double               mCumEffectiveTime;
      double               mLastUpdate;
      // CM_StatePtrCollection  mOtherStatePtrs;
   };

   bool FindTargetType(WsfStringId aTargetTypeId, TargetType*& aTargetTypePtr);

   // bool FindVulnerabilityTo(WsfStringId     aCM_TypeId,
   //                          Vulnerability*& aVulnerabilityPtr);

   void AddInitialCM_States(double aSimTime);

   bool AddCM_State(double         aSimTime,
                    WsfStringId    aCM_TypeId,
                    WsfPlatform*   aCM_PlatPtr,
                    WsfPlatform*   aMissilePtr,
                    WsfPlatform*   aTargetPtr,
                    Vulnerability* aEffectPtr);

   void GetTableAndScalarFor(WsfPlatform* aTargetPtr, const MFK_Table*& aTablePtr, double& aPkFactor);

   void Detonate(double aSimTime);

   using TableCollection      = std::vector<MFK_Table>;
   using TargetTypeCollection = std::vector<TargetType>;

   bool Add(const MFK_Table& aTable);
   bool Add(const TargetType& aTargetType);

   bool IsTypeMatch(const std::vector<WsfStringId>& aPlatTypes, WsfStringId aCM_Type);

   MFK_Table*  GetTable(WsfStringId aTableNameId);
   TargetType* GetTargetType(WsfStringId aTargetTypeId);

   void ApplyEffectTo(double aSimTime, WsfPlatform* aOtherPlatformPtr) override;

private:
   // void AddCM_State_P(CM_State* aCM_StatePtr);

   // ==== Class attributes: ====
   std::shared_ptr<TableCollection>      mSharedMFK_TablesPtr;
   std::shared_ptr<TargetTypeCollection> mSharedTargetTypesPtr; // This a a listing of target types which I have a
                                                                // lethality effect upon

   double                mUpdateInterval;
   TargetType*           mTargetTypeInUsePtr;
   CM_StatePtrCollection mCM_StatePtrs; // Unlike other _shared_ pointers, this is truly memory "owned" by 'this'.
   UtCallbackHolder      mCallbacks;
};

#endif
