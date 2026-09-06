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

#ifndef WSFSIMULATIONINPUT_HPP
#define WSFSIMULATIONINPUT_HPP

#include "wsf_export.h"

#include <map>
#include <vector>

class UtInput;
class WsfDateTime;
class WsfPlatform;
class WsfScenario;
class WsfSimulation;
#include "WsfStringId.hpp"

//! Reads simulation configuration from the input file
//! May be extended by applications to override simulation input commands
class WSF_EXPORT WsfSimulationInput
{
public:
   friend class WsfSimulation;
   WsfSimulationInput(WsfScenario& aScenario);
   virtual ~WsfSimulationInput();

   virtual bool ProcessInput(UtInput& aInput);

   virtual void LoadComplete();

   //! Return the 'minimum mover timestep'.
   //! The minimum mover timestep is the amount of time that must elapse between mover updates on a
   //! platform before the update will actually be performed. If the elapsed time is less than this
   //! value then the update will be suppressed.
   //!
   //! A value of zero (or negative value) effectively allows all updates while a value greater than zero can
   //! result in suppressed updates.
   //!
   //! The constructor initializes this to a negative value to allow derived classes to detect if a value
   //! has been defined in the input file.
   double GetMinimumMoverTimestep() const { return mMinimumMoverTimestep; }


   //! Return whether the simulation should multi-thread were appropriate.
   bool MultiThreaded() const { return mMultiThreaded; }

   //! Get the defined end time of the simulation.
   double GetEndTime() const { return mEndTime; }

   const WsfDateTime& GetDateTime() const { return *mDateTimePtr; }

   //! Determine if the 'required Pd for detection' should be static or random.
   //! @see GetRequiredPd for more information.
   //! @returns 'true' then the constant 'required_pd' value from the sensor mode will be used (default of 0.5).
   //!
   //! @returns 'false' (the default value) then the 'required_pd' will be randomly determined for each detection
   //! attempt. (Computed as 1.0 - GetSimulation()->GetRandom().Uniform()).
   bool UseConstantRequiredPd() const { return mUseConstantRequiredPd; }

   //! Set the 'Use Required Pd' value to the desired value.
   //! @see GetRequiredPd for more information.
   void UseConstantRequiredPd(bool aUseConstantRequiredPd) { mUseConstantRequiredPd = aUseConstantRequiredPd; }
   //@}

   //! @name Determine the way multiple/alternate frequencies behave in sensors.
   //@{
   bool RandomizeFrequency() const { return mRandomizeFrequency; }
   void RandomizeFrequency(bool aRandomizeFrequency);

   bool UseDefaultFrequency() const { return mUseDefaultFrequency; }
   void UseDefaultFrequency(bool aUseDefaultFrequency);
   //@}

   //! Return whether the simulation should multi-thread were appropriate.
   void SetMultiThreaded(bool aMultiThreaded) { mMultiThreaded = aMultiThreaded; }

   //! Indicate if calculation shortcuts are allowed.
   //!
   //! This is a static method (applicable to ALL instances) which indicates if calculation
   //! 'shortcuts' are allowed. If 'true' (the default) a model may employ tests to shorten
   //! or bypass calculations. For instance, a model may check the current signal-to-noise
   //! and bypass calculation ifWsfSimulation the S/N is already too low.
   //!
   //! @note Shortcuts should not be allowed when creating a clutter map.
   void AllowClutterCalculationShortcuts(bool aAllowCalculationShortcuts)
   {
      mAllowClutterCalculationShortcuts = aAllowCalculationShortcuts;
   }

   //! Return the current status of allowing calculation shortcuts.
   bool AllowClutterCalculationShortcuts() const { return mAllowClutterCalculationShortcuts; }

   //! Indicate if calculation shortcuts are allowed.
   //!
   //! This is a static method (applicable to ALL instances) which indicates if calculation
   //! 'shortcuts' are allowed. If 'true' (the default) a model may employ tests to shorten
   //! or bypass calculations. For instance, a model may check the current signal-to-noise
   //! and bypass calculation if the S/N is already too low.
   //!
   //! @note Shortcuts should not be allowed when creating a clutter map.
   void AllowEM_PropagationCalculationShortcuts(bool aAllowCalculationShortcuts)
   {
      mAllowEM_PropagationCalculationShortcuts = aAllowCalculationShortcuts;
   }

   //! Return the current status of allowing calculation shortcuts.
   bool AllowEM_PropagationCalculationShortcuts() const { return mAllowEM_PropagationCalculationShortcuts; }

   WsfScenario* GetScenario() const { return mScenarioPtr; }

   //! Process priorities for windows
   enum ProcessPriority
   {
      cPP_BELOW_NORMAL,
      cPP_NORMAL,
      cPP_ABOVE_NORMAL,
      cPP_HIGH,
      cPP_REALTIME
   };

   //! Set the process priority class for windows
   void SetProcessPriority(ProcessPriority aProcessPriority) { mProcessPriority = aProcessPriority; }

protected:
   WsfScenario* mScenarioPtr;

   //! Whether this simulation is running real-time, or a multiple of real-time.
   bool mIsRealTime;

   //! Thread behavior
   bool mMultiThreaded;

   //! Timing method to use for UtWallClock
   int mWallClockTimingMethod;

   //! Defines the minimum amount of time that must elapse between updates before
   //! a mover update will be allowed.
   double mMinimumMoverTimestep;

   //! The end time of the simulation (seconds)
   double mEndTime;

   //! The ratio of simulation time to real world time
   double mClockRate;

   WsfDateTime* mDateTimePtr;

   //! 'true' if the Pd required for detection should be fixed to the sensor defined value
   //! rather than drawn from an uniform distribution.
   bool mUseConstantRequiredPd;

   //! Randomization flag for randomizing "frequency" for sensors with alternate frequency inputs in the simulation.
   bool mRandomizeFrequency;

   //! Force the use of the "frequency" input (default) for sensor's with alternate frequency inputs in the simulation.
   bool mUseDefaultFrequency;

   int             mNumberOfThreads;
   double          mBreakUpdateTime;
   bool            mDebugMultiThreading;
   ProcessPriority mProcessPriority;

   //! See documentation for AllowCalculationShortcuts.
   bool mAllowClutterCalculationShortcuts;

   //! See documentation for AllowCalculationShortcuts.
   bool mAllowEM_PropagationCalculationShortcuts;
};

//! Reads inputs specific to event-stepped simulations
class WSF_EXPORT WsfEventStepSimulationInput
{
public:
   WsfEventStepSimulationInput();
   virtual ~WsfEventStepSimulationInput() {}
   bool ProcessInput(UtInput& aInput, WsfSimulationInput& aSimInput);

   double mThreadUpdateInterval;
   int    mPlatformThreadUpdateMultiplier;
   int    mSensorThreadUpdateMultiplier;
};

//! Reads inputs specific to frame-stepped simulations
class WSF_EXPORT WsfFrameStepSimulationInput
{
public:
   WsfFrameStepSimulationInput();
   virtual ~WsfFrameStepSimulationInput() {}
   bool ProcessInput(UtInput& aInput, WsfSimulationInput& aSimInput);

   //! The time allotted to a frame (seconds)
   double mFrameTime;
};

//! Reads inputs for both WsfEventStepSimulation and WsfFrameStepSimulation
class WsfDefaultSimulationInput : public WsfSimulationInput, public WsfEventStepSimulationInput, public WsfFrameStepSimulationInput
{
public:
   WsfDefaultSimulationInput(WsfScenario& aScenario);
   ~WsfDefaultSimulationInput() override;
   bool ProcessInput(UtInput& aInput) override;
};

#endif
