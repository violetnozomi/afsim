// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#ifndef WSFBRAWLERPROCESSOR_HPP
#define WSFBRAWLERPROCESSOR_HPP

#include "wsf_brawler_export.h"

#include "BrawlerPlatform.hpp"
#include "UtInput.hpp"
#include "UtTable.hpp"
#include "UtVec3.hpp"
#include "WsfBrawlerComponents.hpp"
class WsfDraw;
class WsfLocalTrack;
class WsfPerceptionProcessor;
class WsfPlatform;
#include "WsfQuantumTaskerProcessor.hpp"
class WsfScenario;
#include "WsfSimpleComponent.hpp"
#include "WsfStringId.hpp"
class WsfThreatProcessor;
class WsfTrack;

// A component that adds brawler related capability to the platform.
// This component should only be added to a platform if the platform has brawler attributes
class WSF_BRAWLER_EXPORT WsfBrawlerProcessor : public WsfQuantumTaskerProcessor, public BrawlerPlatform
// public WsfSimplePlatformComponent
{
public:
   // static void RegisterComponentFactory(WsfScenario& aScenario);
   static WsfBrawlerProcessor* Find(const WsfPlatform& aPlatform);

   WsfBrawlerProcessor(WsfScenario& aScenario);
   WsfBrawlerProcessor(const WsfBrawlerProcessor& aSrc);
   WsfBrawlerProcessor& operator=(const WsfBrawlerProcessor&) = delete;
   ~WsfBrawlerProcessor() override;

   const char* GetScriptClassName() const override { return "WsfBrawlerProcessor"; }

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   // virtual void          ComponentParentChanged(WsfPlatform* aPlatformPtr) { mPlatformPtr = aPlatformPtr; }
   //@}

   bool          Initialize(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;
   WsfProcessor* Clone() const override;

   // virtual void Update(double aSimTime);

   //! @name Brawler related methods
   //@{
   //! Get the last time to which the simulation was updated.
   double GetTime() override;

   //! Uses the task state to update the missile mode
   //! used in consciousness event timing calculations
   void UpdateMissileMode() override;

   //! @return true if platform has any weapon with a quantity remaining greater than zero
   bool HaveWeapons() override;

   //! @name Mover Location Setters
   //! Accessors for setting the mover location when using a WSF_BRAWLER_MOVER
   //!@{
   virtual void SetLocationWCS(const double aLocationWCS[3]);
   virtual void SetLocationECI(const double aLocationWCS[3]);
   virtual void SetLocationLLA(double aLat, double aLon, double aAlt);
   //!@}

   using BrawlerPlatform::GetSpeed; // ft/s

   //! Update BRAWLER specific "mind" state used to decide next event time
   //! @param aSimTime The simulation time (seconds)
   //! returns next consciousness event time
   double MindUpdate(double aSimTime) override;

   //! Accessor that returns the convenience pointer to the Perception Processor
   WsfPerceptionProcessor* GetPerceptionProcessor() const { return mPerceptionProcessorPtr; }
   //! Accessor that returns the convenience pointer to the Threat Processor
   WsfThreatProcessor* GetThreatProcessor() const { return mThreatProcessorPtr; }

   //! Returns the time in seconds to the next consciousness event.
   //! Used to schedule when the next regularly scheduled
   //! consciousness event will take place.
   //! This method is based on gcetim.f from Brawler v7.5
   //! @return The time in seconds to the next consciousness event
   double GetConsciousnessEventTime() override;

   //! @name Next Consciousness Event Time Accessors
   //! Accessors for setting/getting the simulation time (in seconds) the next event is scheduled to occur
   //! Set is used by WsfBrawlerConsciousneesEvent when scheduling new events and recording time of last event
   //!@{
   void SetNextConsciousnessEventTime(double aSimTime)
   {
      mLastConsciousnessEventTime = mNextConsciousnessEventTime;
      mNextConsciousnessEventTime = aSimTime;
   }
   double GetNextConsciousnessEventTime() { return mNextConsciousnessEventTime; }
   //!@}

   //! Adds a BRAWLER self called consciousness event to the event queue
   //! that is to take place at a specific time. Note that this time should be
   //! later than the current simulation time.
   //! @param aEventTime The simulation time (seconds) the event is to take place
   void AddSelfCalledConsciousnessEvent(double aEventTime);

   //! @name BRAWLER Alternative Evaluation.
   //! Project current position forward based on the alternative
   //! and evaluate the result of the projection
   //@{
   // brawler aero command type 1
   double EvaluateRates(UtVec3d& aAngularRates,                // Radians/sec
                        UtVec3d& aLongitudinalAccel) override; // Meters/sec^2

   // brawler aero command type 2
   double EvaluateHeadingSpeedAltitude(const double& aHeading,            // degrees
                                       const double& aSpeed,              // meters/sec
                                       const double& aAltitude,           // meters
                                       const double& aMaxGees,            // Gs
                                       const double& aMaxClimb) override; // meters/sec
   // brawler aero command type 3
   double EvaluateTurnInPlaneWithSpeed(UtVec3d&      aPlane,                 // World coordinates, meters
                                       const double& aGees,                  // Gs
                                       const double& aSpeed) override;       // meters/sec
   double EvaluateTurnInPlaneWithThrottle(UtVec3d&      aPlane,              // World coordinates, meters
                                          const double& aGees,               // Gs
                                          const double& aThrottle) override; //[0-3]
   // brawler aero command type 4
   double EvaluateVectorWithSpeed(UtVec3d&      aVelocityVector,        // Directional unit vector
                                  const double& aMaxGees,               // Gs
                                  const double& aSpeed) override;       // meters/sec
   double EvaluateVectorWithThrottle(UtVec3d&      aVelocityVector,     // Directional unit vector
                                     const double& aMaxGees,            // Gs
                                     const double& aThrottle) override; //[0-3]

   virtual double EvaluateAlternative(UtEntity& projectedState);
   //@}

   //! @name Sensor Coverage
   //! Methods to return the Az/El limits of the sensor
   //@{
   double SensorGimbalElevationLimit() override;
   double SensorGimbalAzimuthLimit() override;
   //@}

   //@}

   //! @name Aero Functions
   //! These methods provide access to the aero state and capability
   //! of the mover attached to the BRAWLER platform. Because BRAWLER
   //! uses different units than the framework standard in many places
   //! a BRAWLER convention of appending the return value units to the end
   //! of the method name was adopted.
   //@{
   // Forward Acceleration Functions
   double MaxForwardAcceleration_fps2() override;
   double MinForwardAcceleration_fps2() override;
   double MaxForwardAccelerationWithGravity_fps2() override;
   double MinForwardAccelerationWithGravity_fps2() override;
   // Maximum Performance Turn Functions
   double MaxInstantaneousGLoad_g() override;
   double MinInstantaneousTurnRadius_ft() override;
   double MaxInstantaneousTurnRate_rps() override;
   double MaxSustainedGLoad_g() override;
   double MinSustainedTurnRadius_ft() override;
   double MaxSustainedTurnRate_rps() override;
   // Comfortable Turn Functions
   double MaxTacticalGLoad_g() override;
   double MinTacticalTurnRadius_ft() override;
   double MaxTacticalTurnRate_rps() override;
   // Misc Performance Calculations
   double CorneringVelocity_fps() override;
   double SpecificEnergyRate_fps() override;
   double CalcTurnRate_rps(const double& aClMax,
                           const double& aSpeed_fps,
                           const double& aDynamicPressure_psf,
                           const double& aWingArea_ft2) override;
   // Weights
   double CurrentWeight_lbs() override;
   double EmptyWeight_lbs() override;
   double CurrentFuelWeight_lbs() override;
   double DesignWeightForMaxG_lbs() override;
   // Structural Limits
   double CurrentStructuralGLimit_g() override;
   double StructuralGLimitAtDesignWeight_g() override;
   double StructuralGLimitAboveDesignWeight_g() override;
   // Data Functions
   double DynamicPressure_psf() override;
   double Mach() override;
   double GetMaxCL() override;
   double GetMaxCL(const double& aMach) override;
   double GetMaxCL(const double& aAlt_ft, const double& aMach) override;
   double GetAlphaAtMaxCL_deg() override;
   double GetAlphaAtMaxCL_deg(const double& aMach) override;
   double GetAlphaAtMaxCL_deg(const double& aAlt_ft, const double& aMach) override;
   double MaxAngularRate_rps() override;

   virtual double Lift(const double& alpha,     // degrees (angle of attack)
                       const double& speed,     // meters/sec
                       const double& altitude); // meters

   virtual double Drag(const double& alpha,     // degrees (angle of attack)
                       const double& speed,     // meters/sec
                       const double& altitude); // meters

   virtual double Thrust(const double& throttle,  // setting [1-3]
                         const double& speed,     // meters/sec
                         const double& altitude); // meters

   virtual double Throttle(); // setting [1-3]

   // virtual double Weight(); //use: CurrentWeight_lbs()
   // virtual double DynamicPressure(); //use:       virtual double DynamicPressure_psf()
   virtual double Area();     // reference area or surface area (sq feet)
   virtual double Alpha();    // angle of attack, degrees
   virtual double AlphaCL0(); // alpha at zero lift (aircraft flying flat)
   //@}

protected:
   //! @name Target and Threat Access
   //! These methods override BrawlerPlatform parent class methods.
   //! They provide access to information on the current target and
   //! any threats.
   //@{
   WsfLocalTrack* GetTargetTrack();
   UtEntity*      GetTarget() override;                          // over-write
   double         FirstObservedTime(UtEntity* aTgtPtr) override; // over-write
   UtEntity*      GetThreat() override;                          // over-write
   int            GetNumberThreats() override;                   // over-write
   //@}

private:
   // WsfPlatform*                     mPlatformPtr;

   //! Convenience pointer to the perception processor part required by a BRAWLER platform
   WsfPerceptionProcessor* mPerceptionProcessorPtr;

   //! Convenience pointer to the WsfThreatProcessor part required by a BRAWLER platform
   WsfThreatProcessor* mThreatProcessorPtr;

   //! Flag that indicates platform in on the first CE. Used for scheduling
   bool mFirstCE;

   //! The simulation time in seconds the next consciousness event is scheduled to take place.
   //! Defaults to zero, updated each time a CE executes and schedules a new event.
   double mNextConsciousnessEventTime;

   //! The simulation time in seconds the previous consciousness event took place.
   //! Defaults to zero, updated each time a CE executes and schedules a new event.
   //! Equivalent to 'tlstce' of BRAWLER v7.5
   double mLastConsciousnessEventTime;

   //! User supplied consciousness event update time, if zero no user supplied input
   //! Equivalent to 'prcedt' defined in mindpr.fi of BRAWLER v7.5
   double mUserConsciousnessEventDT;

   //! @return the simulation time in seconds the previous consciousness event took place
   double LastConsciousnessEventTime() override { return mLastConsciousnessEventTime; } // replaces use of "tlstce"

   UtTable::Curve* mCLMaxVsMachTablePtr;        //!< Aero Data. CL Max vs Mach table
   UtTable::Curve* mAlphaAtCLMaxVsMachTablePtr; //!< Aero Data. Alpha at CL Max vs Mach table

   double mStructuralGLimitAboveDesignWeight_g; //!< Aircraft structural G limit above design weight in Gs

   bool     mDebug;                  //!< Debug flag, setting true enables additional output
   bool     mDebugDrawAlternatives;  //!< When true enables drawing of alternative projection and color coded score
   bool     mDebugDrawNominalStates; //!< When true enables drawing of nominal state
   WsfDraw* mDrawPtr;                //!< Pointer to draw class for drawing debug data

   double mAlternativeMinScore; //!< Used to track the lowest alternative score when evaluating alternatives
   double mAlternativeMaxScore; //!< Used to track the highest alternative score when evaluating alternatives

   //! Utility method that color code an alternative score
   //! @return a vector with 3 values for R,G,B
   UtVec3d GetEvaluationColor(double aScore);

   //! Utility method to fill the passed in UtEntity with data
   //! from the passed in track
   void     PopulateEntityFromTrack(UtEntity& aEntity, WsfTrack* aTrackPtr);
   UtEntity mTargetEntity; //!< Local storage for the state of the current target
   UtEntity mThreatEntity; //!< Local storage for the state of most threatening entity
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfBrawlerProcessor, cWSF_COMPONENT_BRAWLER_PROCESSOR)

#endif
