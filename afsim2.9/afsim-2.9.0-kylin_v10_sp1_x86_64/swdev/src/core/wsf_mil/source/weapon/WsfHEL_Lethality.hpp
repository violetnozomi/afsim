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

#ifndef WSFHELLETHALITY_HPP
#define WSFHELLETHALITY_HPP

#include "wsf_mil_export.h"

#include <map>

#include "UtGenericMappedList.hpp"
#include "UtMatrix.hpp"
#include "WsfDirectedEnergyWeapon.hpp"
class WsfPlatform;
class WsfWeapon;
#include "WsfWeaponEffects.hpp"

/** A simple weapon effect calculating the lethal effect from a high-energy
   laser (HEL).  The conditions for a lethal effect are that it
   must be on target for a minimum time above a certain intensity threshold.
   We are not yet calculating beam size or aspect effects.  We tacitly assume
   that these parameters are sufficient to obtain the desired effect.
*/
class WSF_MIL_EXPORT WsfHEL_Lethality : public WsfWeaponEffects
{
   // TODO Cache data at initialization based on weapon type in order to save memory
   //  (currently databases are cloned as needed)
public:
   WsfHEL_Lethality(WsfScenario& aScenario);
   WsfHEL_Lethality(const WsfHEL_Lethality& aLethality);
   ~WsfHEL_Lethality() override = default;

   WsfWeaponEffects* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr) override;

protected:
   //! Disable use.
   WsfHEL_Lethality& operator=(const WsfHEL_Lethality& aRhs) = delete;

   void ApplyEffectTo(double aSimTime, WsfPlatform* aTargetPtr) override;

   void ApplyEffectIncrement(double aSimTime, WsfPlatform* aOtherPlatformPtr, double aDamageAmount) override;

   void CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr) override;

   bool IsVulnerable(WsfPlatform* aOtherPlatformPtr) override;

   class Entry;
   const Entry* FindMappedEntry(WsfPlatform* aTargetPtr);


   class Effect
   {
   public:
      enum Type
      {
         cLETHAL_PLATFORM,
         cLETHAL_PLATFORM_PART,
         cCUSTOM // basically, do something on callback.
      };

      enum Kind
      {
         cUNDEFINED,
         cENERGY,
         cPK_ENERGY,
         cENERGY_DENSITY,
         cPOWER // Deprecated; no longer used
      };

      Effect() = default;

      Effect(Kind aKind, double aThreshold, double aRadius = 0.0, Type aType = cLETHAL_PLATFORM)
         : mKind(aKind)
         , mThreshold(aThreshold)
         , mPK_TablePtr(nullptr)
         , mRadius(aRadius)
         , mType(aType)
      {
      }

      Effect(const Effect& aSrc)
         : mKind(aSrc.mKind)
         , mThreshold(aSrc.mThreshold)
         , mPK_TablePtr(nullptr)
         , mRadius(aSrc.mRadius)
         , mType(aSrc.mType)
      {
         if (aSrc.mPK_TablePtr != nullptr)
         {
            mPK_TablePtr = new UtMatrixd(*aSrc.mPK_TablePtr);
         }
      }

      Effect& operator=(const Effect& aSrc)
      {
         if (this != &aSrc)
         {
            mKind      = aSrc.mKind;
            mThreshold = aSrc.mThreshold;
            delete mPK_TablePtr;
            mPK_TablePtr = nullptr;
            if (aSrc.mPK_TablePtr != nullptr)
            {
               mPK_TablePtr = new UtMatrixd(*aSrc.mPK_TablePtr);
            }
            mRadius = aSrc.mRadius;
            mType   = aSrc.mType;
         }
         return *this;
      }
      ~Effect() { delete mPK_TablePtr; }

      bool ProcessInput(UtInput& aInput);

      Kind       mKind{cUNDEFINED};
      double     mThreshold{0.0};         // Power, Energy, or Energy Density
      UtMatrixd* mPK_TablePtr{nullptr};   // PK-energy
      double     mRadius{0.0};            // zero is not defined
      Type       mType{cLETHAL_PLATFORM}; // Is this effect lethal for the platform
   };

   class Entry
   {
   public:
      enum Type
      {
         cCATEGORY,
         cPLATFORM_TYPE,
         cPLATFORM_REGION
      };

      Entry()
         : mType(cCATEGORY)
         , mCategory(nullptr)
         , mPlatformType(nullptr)
         , mRegion(nullptr)
         , mEffects()
      {
      }

      bool ProcessInput(UtInput& aInput);

      bool ProcessType(UtInput& aInput);

      WsfStringId GetId() const
      {
         if (mType == cCATEGORY)
         {
            return mCategory;
         }
         else if (mType == cPLATFORM_TYPE)
         {
            return mPlatformType;
         }
         else // cPLATFORM_REGION
         {
            assert(mType == cPLATFORM_REGION);
            return mRegion;
         }
      }

      Type        mType;
      WsfStringId mCategory;
      WsfStringId mPlatformType;
      WsfStringId mRegion;


      std::vector<Effect> mEffects; // There will always be at least one of these

      bool operator<(const Entry& aRhs) { return GetId() < aRhs.GetId(); }
   };

private:
   void ApplyEffectIncrementP(double aSimTime, WsfPlatform* aOtherPlatformPtr, double aDamageAmount);

   bool IsVulnerableP(WsfPlatform* aOtherPlatformPtr);

   static void ReadPK_EnergyTable(UtInput& aInput, UtMatrixd* aTable);

   using EntryMap = UtStdMappedList<Entry, WsfStringId>;

   EntryMap mEntryMap;

   int          mSequence; // for multiple effects in a single event
   bool         mManageKills;
   bool         mUnharmedUntilKilled;
   const Entry* mEntryPtr;
};

#endif
