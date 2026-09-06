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

#ifndef SOSM_TARGET_HPP
#define SOSM_TARGET_HPP

#include "sosm_export.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "SOSM_CoreBase.hpp"
class SOSM_Interaction;
class SOSM_Manager;
#include "SOSM_SpectralObject.hpp"
#include "UtAtmosphere.hpp"

//! Abstract base class that represents a target.
class SOSM_EXPORT SOSM_Target : public SOSM_CoreBase
{
public:
   //! A nested class that defines the conditions under which a particular signature state is to be used.
   //! This is a base class that is overriden by derived classes so they can attach additional data that
   //! is specific to the target model.
   class StateBase : public SOSM_SpectralObject
   {
   public:
      StateBase(const std::string& aStateName);
      ~StateBase() override;

      virtual void InputComplete(SOSM_Target& aTarget);

      virtual bool ProcessInput(UtInput& aInput, SOSM_Target& aTarget);

      std::string mStateName;
      float       mMinAltitude;
      float       mMaxAltitude;
      float       mMinSpeed;
      float       mMaxSpeed;
      float       mMinThrottle;
      float       mMaxThrottle;
      bool        mSpeedIsMach;
   };

   SOSM_Target(SOSM_Manager* aManagerPtr);
   SOSM_Target(const SOSM_Target& aSrc);
   SOSM_Target& operator=(const SOSM_Target&) = delete;

   //! A virtual copy constructor.
   virtual SOSM_Target* Clone() const = 0;

   bool Initialize() override;

   void InputComplete() override;

   bool ProcessInput(UtInput& aInput) override;

   //! Get the radiant intensity of this target.
   //! @param aRequestor        [input]  The spectral definition (origin/increment/count) of the sensor.
   //! @param aInteraction      [input]  The interaction object representing the sensor and target state.
   //! @param aBodyIntensity    [output] The radiant intensity of the body without the plume (W/sr/um).
   //!   Also known as I_AF.
   //! @param aBodyArea         [output] The presented area of the body without the plume (cm^2).
   //!   Also known as A_AF.
   //! @param aPlumeIntensity   [output] The radiant intensity of the plume (W/sr/um).
   //!   Also known as I_PL.
   //! @param aPlumeArea        [output] The presented area of the plume (cm^2).
   //!   Also known as A_PL
   virtual void ComputeRadiantIntensity(const SOSM_SpectralObject& aRequestor,
                                        SOSM_Interaction&          aInteraction,
                                        std::vector<float>&        aBodyIntensity,
                                        float&                     aBodyArea,
                                        std::vector<float>&        aPlumeIntensity,
                                        float&                     aPlumeArea) = 0;

   //! Get the sample points of the currently selected state (fast detection mode table generation).
   //! @param aInteraction [input] The interaction object for the current state.
   //! @param aAzValues [output] Azimuth sample points (radians).
   //! @param aElValues [output] Elevation sample points (radians).
   virtual void GetSamplePoints(SOSM_Interaction&   aInteraction,
                                std::vector<float>& aAzValues,
                                std::vector<float>& aElValues) = 0;

   //! Get the sample target state (altitude, speed, throttle) (for fast detection mode table generation).
   //! @param aInteraction [input] The interaction object for the current state.
   //! @param aAltitude [output] Altitude sample point (meters).
   //! @param aSpeed    [output] Speed sample point (meters/second).
   //! @param aThrottle [output] Throttle sample point (unitless [0..1]).
   virtual void GetSampleTargetState(SOSM_Interaction& aInteraction, float& aAltitude, float& aSpeed, float& aThrottle) = 0;

   //! Return the number of defined states.
   size_t StateCount() const { return mSharedDataPtr->mStates.size(); }

   //! Return the state of the specified index.
   //! @param aIndex The index of the desired state. This should be in the range [0 .. StateCount()-1].
   const StateBase* StateEntry(size_t aIndex) const { return mSharedDataPtr->mStates[aIndex]; }

   //! Select a state using the current target state from an interaction object.
   //! @returns the index of the selected state in the range [0 .. StateCount()-1].
   size_t SelectState(SOSM_Interaction& aInteraction) const;

   SOSM_Manager* GetManager() const { return mManagerPtr; }

protected:
   //! Class specific state allocator.
   //!
   //! This is a method that must be defined by the derived class which instantiates a new State object
   //! that is specific to the derived class.
   virtual StateBase* CreateState(const std::string& aStateName) const = 0;

   class SharedData
   {
   public:
      virtual ~SharedData();
      std::vector<StateBase*> mStates;
   };

   SOSM_Manager* mManagerPtr;

   //! A pointer to the shared state data.
   std::shared_ptr<SharedData> mSharedDataPtr;

   //! An atmosphere for converting speed to Mach number
   UtAtmosphere mAtmosphere;

   //! A mutex for locking access to the atmosphere object (UtAtmosphere is not thread-safe)
   mutable std::recursive_mutex mMutex;
};

#endif
