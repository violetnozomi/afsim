// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSA_PREDICT_HPP
#define WSFSA_PREDICT_HPP

#include "wsf_air_combat_export.h"

#include "WsfSA_Module.hpp"
class WsfSA_Processor;

enum class TurnDirection
{
   cLEFT,
   cRIGHT
}; // TODO add one for closest turn direction?

/** This is the predict module of the SA Processor */
class WSF_AIR_COMBAT_EXPORT WsfSA_Predict : WsfSA_Module
{
public:
   friend class WsfSA_Processor;

   WsfSA_Predict(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr);
   WsfSA_Predict& operator=(const WsfSA_Predict&) = delete;
   ~WsfSA_Predict();

   /// Generic Prediction Methods
   // Starter functions, intended to be overridden in script by users with
   // strong preferences for how to capture assumptions about an entity's behavior.

   // Provides a best-guess for the entity's location (WCS) at some time in the future
   virtual const WsfGeoPoint ProjectPositionInTime(double aSimTime, const WsfSA_EntityPerception& aEntity);

   /// Prediction Primatives
   // Geometric approximations of where an entity might be if it carries out a particular maneuver.

   virtual const WsfGeoPoint ProjectPositionForward(double aSimTime, const WsfSA_EntityPerception& aEntity);

   virtual const WsfGeoPoint ProjectPositionLevelTurn(double                        aSimTime,
                                                      const WsfSA_EntityPerception& aEntity,
                                                      double                        aGLimit,
                                                      TurnDirection                 aDirection);

   virtual const WsfGeoPoint ProjectPositionTurnToHeading(double                        aSimTime,
                                                          const WsfSA_EntityPerception& aEntity,
                                                          double                        aHeading,
                                                          double                        aGLimit);

   virtual const WsfGeoPoint ProjectPositionGoToPoint(double                        aSimTime,
                                                      const WsfSA_EntityPerception& aEntity,
                                                      WsfGeoPoint                   aPointOfInterest,
                                                      double                        aGLimit);

   virtual const WsfGeoPoint ProjectPositionRollAndPull(double                        aSimTime,
                                                        const WsfSA_EntityPerception& aEntity,
                                                        double                        aHeading_deg,
                                                        double                        aRollAngle_deg,
                                                        double                        aGLimit);

   virtual const WsfGeoPoint ProjectPositionRollAndPull(double                        aSimTime,
                                                        const WsfSA_EntityPerception& aEntity,
                                                        double                        aRollAngle_deg,
                                                        double                        aGLimit);

   virtual const WsfGeoPoint ProjectPositionSplitS(double aSimTime, const WsfSA_EntityPerception& aEntity, double aGLimit);

protected:
   WsfSA_Predict(const WsfSA_Predict& aSrc);

   void Initialize(double aSimTime);

   bool GetPerceivedState(const WsfSA_EntityPerception& aEntity, UtVec3dX& aLocationWCS, UtVec3dX& aVelocityWCS);

   bool CheckRequestedTime(double aSimTime, const WsfSA_EntityPerception& aEntity);

private:
   // Script overrides for the prefab functionality

   UtScript* mGenericProjectionScriptPtr{nullptr};

   UtScript* mGoForwardScriptPtr{nullptr};
   UtScript* mLevelTurnLeftScriptPtr{nullptr};
   UtScript* mLevelTurnRightScriptPtr{nullptr};
   UtScript* mTurnToHeadingScriptPtr{nullptr};
   UtScript* mGoToPointScriptPtr{nullptr};
   UtScript* mRollAndPullScriptPtr{nullptr};
   UtScript* mRollAndPullToHeadingScriptPtr{nullptr};
   UtScript* mSplitEssScriptPtr{nullptr};
};

#endif
