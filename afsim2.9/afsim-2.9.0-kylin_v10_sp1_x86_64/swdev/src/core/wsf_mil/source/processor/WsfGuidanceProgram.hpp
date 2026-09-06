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

#ifndef WSFGUIDANCEPROGRAM_HPP
#define WSFGUIDANCEPROGRAM_HPP

#include "wsf_mil_export.h"

#include <string>

class UtEntity;
class UtInput;
#include "WsfGuidanceComputer.hpp"
class WsfGuidanceComputer;
class WsfGuidanceProgram;
#include "WsfObject.hpp"
class WsfScriptContext;

class WsfGuidanceProgramTypes
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfGuidanceProgramTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfGuidanceProgramTypes& Get(const WsfScenario& aScenario);
   //@}

   using FactoryPtr        = WsfGuidanceProgram* (*)(const std::string&);
   using ObjectFactoryList = std::list<FactoryPtr>;
   WsfGuidanceProgramTypes() { mObjectFactoryList.push_back(&WsfGuidanceProgramTypes::PredefinedObjectFactory); }
   WSF_MIL_EXPORT void                AddObjectFactory(FactoryPtr aFactoryPtr);
   WSF_MIL_EXPORT WsfGuidanceProgram* CreateInstance(const std::string& aTypeName) const;

protected:
   static WsfGuidanceProgram* PredefinedObjectFactory(const std::string& aTypeName);

   //! The list of object factories
   ObjectFactoryList mObjectFactoryList;
};

//! Base class for the guidance programs (maneuvers) used by WsfGuidanceComputer.
class WSF_MIL_EXPORT WsfGuidanceProgram : public WsfObject
{
public:
   // Create typedef's in our scope that parallel the same name in WsfGuidanceComputer.
   using State    = WsfGuidanceComputer::State;
   using Commands = WsfGuidanceComputer::Commands;
   using Phase    = WsfGuidanceComputer::Phase;

   //! The following are names of aux_data variables that are defined when the ORBIT_INSERTION_PROGRAM declares that
   //! orbit conditions have been satisfied. These are tested in weapon_tools...
   static const char* cGC_ORBIT_DECLARED;
   static const char* cGC_DELTA_V_REMAINING;
   static const char* cGC_BURN_TIME_REMAINING;
   static const char* cGC_FUEL_REMAINING;

   //!  The return status for the Execute method.
   enum Status
   {
      cCONTINUE, //!< The program should be continued to be called (remains in the active program list)
      cCOMPLETE  //!< The program is complete and no longer needs to be called (removed from the active program list)
   };

   enum Axis
   {
      cX_AXIS = 0,
      cY_AXIS = 1,
      cZ_AXIS = 2
   };

   WsfGuidanceProgram();
   WsfGuidanceProgram(const WsfGuidanceProgram& aSrc) = default;
   ~WsfGuidanceProgram() override                     = default;
   WsfGuidanceProgram& operator=(const WsfGuidanceProgram&) = delete;

   //! Virtual copy constructor.
   WsfGuidanceProgram* Clone() const override = 0;

   virtual bool Initialize(double aSimTime, WsfGuidanceComputer* aComputerPtr);

   bool ProcessInput(UtInput& aInput) override;

   virtual void Start(double aSimTime, double aCurTime);

   virtual void Terminate(double aSimTime);

   //! Execute the guidance program.
   //! @param aState    [input] The current state of the weapon and guidance target.
   //! @param aCommands [input/output] guidance program input/output.
   //! @returns the status of the program. 'cCONTINUE' means the program is not complete and should continue to be
   //! called. 'cCOMPLETE' means the program is complete and no longer needs to be called.
   virtual Status Execute(State& aState, Commands& aCommands) = 0;

   //! A convenience method for getting the pointer to the platform.
   WsfPlatform* GetPlatform() { return mComputerPtr->GetPlatform(); }

   WsfGuidanceComputer* GetComputer() { return mComputerPtr; }

protected:
   void AltitudeGuidance(State& aState, Phase& aPhase, double& aZ_Accel);

   void AngleOffsetGuidance(State& aState, Phase& aPhase, double& aY_Accel, double& aZ_Accel);

   static void ApplyGravityBias(UtEntity& aEntity, double aGravityBiasFactor, double& aY_Accel, double& aZ_Accel);

   static void ApplyLateralBias(UtEntity& aEntity, double aLateralBiasFactor, double& aY_Accel, double& aZ_Accel);

   static void ComputeGravitationalAcceleration(const double aLocWCS[3], double aGravAclWCS[3]);

   bool FlightPathAngleAchieved(const State& aState, double aCommandedFlightPathAngle) const;

   void FlightPathAngleGuidance(State& aState, Phase& aPhase, double aCommandedFlightPathAngle, double& aY_Accel, double& aZ_Accel);

   void ProNavGuidance(State& aState, double aProNavGain, bool aAugmented, double& aY_Accel, double& aZ_Accel);

   void PursuitGuidance(State& aState, double aPursuitGain, double& aY_Accel, double& aZ_Accel);

   WsfGuidanceComputer* mComputerPtr;
   double               mStartTime;    //!< Simulation time when this program was started.
   bool                 mDebugEnabled; //!< True if debug output is enabled.
};

#endif
