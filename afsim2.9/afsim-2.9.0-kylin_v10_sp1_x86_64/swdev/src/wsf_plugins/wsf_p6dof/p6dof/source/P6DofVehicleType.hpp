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

#ifndef P6DOFOBJECTTYPE_H
#define P6DOFOBJECTTYPE_H

#include "p6dof_export.h"

#include <list>
#include <vector>

#include "P6DofGraphicalSupport.hpp"
#include "P6DofKinematicState.hpp"
#include "P6DofMassProperties.hpp"
#include "P6DofObject.hpp"
#include "P6DofSizeFactor.hpp"
#include "UtInput.hpp"
#include "UtVec3dX.hpp"

class P6DofAeroCoreObject;
class P6DofAeroMovableObject;
class P6DofFlightControlSystem;
class P6DofLandingGear;
class P6DofPilotManager;
class P6DofPilotObject;
class P6DofPropulsionSystem;
class P6DofScenario;
class P6DofSequencer;

// The P6DofVehicleType class is used to define a "type" of P6DofVehicle. Each
// P6DofVehicle instance is derived, in part, from an instance of P6DofVehicleType.
class P6DOF_EXPORT P6DofVehicleType : public P6DofObject
{
   friend class P6DofVehicle;

public:
   explicit P6DofVehicleType(P6DofScenario* aScenario);
   ~P6DofVehicleType() override;

   P6DofVehicleType& operator=(const P6DofVehicleType& other) = delete;

   P6DofVehicleType* Clone() const;

   bool ProcessInput(UtInput& aInput);

   // Object 'Types' do not initialize, so they do not use Initialize() or Initialize2()

   // Name is the identifier for an object, the manager classes rely on the name not to change,
   //  thus there is no interface to change the name.
   std::string GetSubobjectBaseName() const;
   std::string GetDerivedFromName() const;

   void DeriveFrom(const P6DofVehicleType& aSrc);

   P6DofPilotObject*  PilotObject() const;
   P6DofPilotManager* PilotManager() const { return mPilotManagerPtr; }

protected:
   P6DofVehicleType(const P6DofVehicleType& aSrc);

   // This removes a sequencer (with specified name), from the sequencer list
   bool RemoveSequencer(const std::string& aName);

   std::string mSubobjectBaseName;
   std::string mDerivedFromType;

   // Mass properties
   P6DofMassProperties mMassProperties;

   // Kinematic (motion) state
   P6DofKinematicState mKinematicState;

   // Integrator type
   std::string mIntegratorTypeName;

   // Aero properties
   P6DofAeroCoreObject*               mAeroCoreObjPtr;
   std::list<P6DofAeroMovableObject*> mAeroPartList;

   // Propulsion system (engines, fuel tanks, etc)
   P6DofPropulsionSystem* mPropulsionObjPtr;

   // A pilot object is what controls the vehicle. This could a manual
   // pilot, synthetic pilot, a simulated hardware autopilot, or guidance.
   P6DofPilotManager* mPilotManagerPtr;

   // Flight controls determine the positions of flight control surfaces and
   // related objects
   P6DofFlightControlSystem* mFlightControlsPtr;

   // Landing gear is optional
   P6DofLandingGear* mLandingGearPtr;

   // List of sequencer objects
   std::vector<P6DofSequencer*> mSequencerList;

   // List of subobjects
   std::list<P6DofVehicleType*> mSubObjectList;

   // Relationship with parent object (if it exists)
   UtVec3dX mPosRelToParent_m;
   UtVec3dX mAngRelToParentYPR_rad;
   UtVec3dX mSeparationVel_fps;
   UtVec3dX mSeparationOmega_rps;

   // Graphical support -- provides support to define the current graphics
   // representation for a P6DofVehicle in graphical support tools.
   P6DofGraphicalSupport mGraphicalSupport;

   // The size factor allows effects (esp aerodynamic) based on size
   P6DofSizeFactor mSizeFactor;

   // Fixed object flag (used to denote no need for kinematics calculations)
   bool mFixedObject;

   // These parameters are used to generate internal tabular data.
   // Although the object can operate outside of these values, the
   // tabular data will be clamped to the highest value. For this reason,
   // accurate estimates of the operating envelope should be used.
   double mMaxMach;
   double mMaxAlpha_deg;
   double mMinAlpha_deg;
   double mMaxBeta_deg;
};

#endif
