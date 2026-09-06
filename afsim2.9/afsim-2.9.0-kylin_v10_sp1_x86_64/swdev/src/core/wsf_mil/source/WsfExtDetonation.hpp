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

#ifndef WSFEXTDETONATION_HPP
#define WSFEXTDETONATION_HPP

#include "wsf_mil_export.h"

#include <map>
#include <set>
#include <string>

class UtInput;
#include "UtVec3.hpp"
class WsfPlatform;
#include "WsfStringId.hpp"
#include "WsfWeaponEngagement.hpp"
class WsfExtInterface;
class WsfMilExtInterface;

//! Assists external interfaces with processing incoming detonations
class WSF_MIL_EXPORT WsfExtDetonation
{
public:
   //! Detonation result codes from IEEE 1278 SISO-REF-010 (aka 'The DIS Enumerations Document')
   enum DetonationResult
   {
      cOTHER                                          = 0,
      cENTITY_IMPACT                                  = 1,
      cENTITY_PROXIMATE_DETONATION                    = 2,
      cGROUND_IMPACT                                  = 3,
      cGROUND_PROXIMATE_DETONATION                    = 4,
      cDETONATION                                     = 5,
      cNONE                                           = 6,
      cHE_HIT_SMALL                                   = 7,
      cHE_HIT_MEDIUM                                  = 8,
      cHE_HIT_LARGE                                   = 9,
      cARMOR_PIERCING_HIT                             = 10,
      cDIRT_BLAST_SMALL                               = 11,
      cDIRT_BLAST_MEDIUM                              = 12,
      cDIRT_BLAST_LARGE                               = 13,
      cWATER_BLAST_SMALL                              = 14,
      cWATER_BLAST_MEDIUM                             = 15,
      cWATER_BLAST_LARGE                              = 16,
      cAIR_HIT                                        = 17,
      cBUILDING_HIT_SMALL                             = 18,
      cBUILDING_HIT_MEDIUM                            = 19,
      cBUILDING_HIT_LARGE                             = 20,
      cMINE_CLEARING_LINE_CHARGE                      = 21,
      cENVIRONMENT_OBJECT_IMPACT                      = 22,
      cENVIRONMENT_OBJECT_PROXIMATE_DETONATION        = 23,
      cWATER_IMPACT                                   = 24,
      cAIRBURST                                       = 25,
      cKILL_WITH_FRAGMENT_TYPE_1                      = 26,
      cKILL_WITH_FRAGMENT_TYPE_2                      = 27,
      cKILL_WITH_FRAGMENT_TYPE_3                      = 28,
      cKILL_WITH_FRAGMENT_TYPE_1_AFTER_FLYOUT_FAILURE = 29,
      cKILL_WITH_FRAGMENT_TYPE_2_AFTER_FLYOUT_FAILURE = 30,
      cMISS_DUE_TO_FLYOUT_FAILURE                     = 31,
      cMISS_DUE_TO_ENDGAME_FAILURE                    = 32,
      cMISS_DUE_TO_FLYOUT_AND_ENDGAME_FAILURE         = 33,
      cRESULT_CODE_COUNT
   };

   //! Contains the result of a detonation
   struct Result
   {
      Result()
         : mIgnored(false)
         , mGeometryResult(WsfWeaponEngagement::cIN_PROGRESS)
         , mMissDistance(0.0)
         , mDetonateLocationWCS(0.0, 0.0, 0.0)
         , mTargetLocationWCS(0.0, 0.0, 0.0)
         , mTargetPlatformPtr(nullptr)
      {
      }
      bool                                mIgnored;
      WsfWeaponEngagement::GeometryResult mGeometryResult;
      double                              mMissDistance;
      UtVec3d                             mDetonateLocationWCS;
      UtVec3d                             mTargetLocationWCS;
      WsfPlatform*                        mTargetPlatformPtr;
   };

   WsfExtDetonation(WsfExtInterface* aInterfacePtr);

   //! Set firing platform pointer
   void SetFiringEntity(WsfPlatform* aPlatformPtr) { mFiringEntityPtr = aPlatformPtr; }

   //! Set target platform pointer
   void SetTargetEntity(WsfPlatform* aPlatformPtr) { mTargetEntityPtr = aPlatformPtr; }

   void SetWeaponEntity(WsfPlatform* aPlatformPtr);

   //! Set the weapon platform type (used for implicit weapons)
   void SetWeaponEntityType(WsfStringId aWeaponTypeId) { mWeaponTypeId = aWeaponTypeId; }

   //! Set the expected result given by the external interface
   void SetResult(unsigned int aResult) { mResult = aResult; }

   std::string GetResultName() const;

   bool DebugEnabled();

   //! Sets the detonation location.  This is used to search for a target if the target entity is not set.
   void SetDetonateLocation(double aDetonateLocationWCS[3]) { mDetonateLocationWCS.Set(aDetonateLocationWCS); }
   void SetRelativeDetonationLocation(double aDetonationLocationECS[3])
   {
      mRelativeDetonationLocationECS.Set(aDetonationLocationECS);
   }

   //! Execute the detonation, destroying the target if successful.
   void Execute(double aSimTime, Result& aResult);

   std::string       GetDefaultWeaponEffectName();
   WsfWeaponEffects* GetDefaultWeaponEffect();

   WsfSimulation* GetSimulation() const;

private:
   void PrintDetonation(double aSimTime, Result& aResult);

   WsfPlatform* FindTargetPlatform(WsfPlatform* aReferencePlatformPtr);

   void PerformSimpleDetonation(double aSimTime, Result& aResult, WsfWeaponEngagement* aEngagementPtr);

   WsfStringId SelectWeaponEffect(WsfPlatform* aWeaponPlatformPtr, bool& aExcludeSimpleDetonation);

   // Detonation Inputs
   WsfExtInterface*    mInterfacePtr;
   WsfMilExtInterface* mMilInterfacePtr;
   WsfPlatform*        mFiringEntityPtr;
   WsfPlatform*        mTargetEntityPtr;
   WsfPlatform*        mWeaponEntityPtr;
   UtVec3d             mDetonateLocationWCS;
   UtVec3d             mRelativeDetonationLocationECS;
   unsigned int        mResult;
   WsfStringId         mWeaponTypeId;
};
#endif
