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

#ifndef SOSM_SENSORTARGET_HPP
#define SOSM_SENSORTARGET_HPP

#include "sosm_export.h"

#include <map>
#include <string>
#include <vector>

class SOSM_Interaction;
#include "SOSM_ScalarTable2D.hpp"
#include "SOSM_ScalarTable3D.hpp"
class SOSM_Sensor;
class SOSM_Target;

//! A sensor-target pair.
//!
//! There are two basic ways to perform sensor/target interactions:
//!
//! - Directly instantiating sensor and target objects and invoking the sensor detection routines.
//!   This will invoke the underlying sensor, target and atmospheric models to compute the detection
//!   result. This is the slower of the methods, but uses less storage.
//!
//! - Utilizing precomputed tables that are specifically computed for the sensor-type/target-type pair.
//!   This is the most efficient mechanism, but can require a LOT of memory if there are a great number
//!   sensor types and target types.
//!
//! The main function of this class is to isolate to calling application from knowing which method is
//! being used. The caller is simply responsible for instantiating and initializing one of these objects
//! for a given sensor/target pair.
//!
//! An application may choose to either maintain one of these objects for each sensor-type/target-type
//! pair or one for each sensor/target pair. The second method is more efficient (and thread-safe) because
//! the some of the models maintain a persistent state that works most efficiently when not changing between
//! targets. The first method is NOT thread-safe. The disadvantage is that each pairing takes about 100 bytes,
//! which should be no problem (unless has about 10000 bodies, all which have IR sensors detecting all targets!)
//!
//! @note Thread-safety only exists when each pair of interacting sensor and target objects has their own
//!       instance of this object.
class SOSM_EXPORT SOSM_SensorTarget
{
public:
   SOSM_SensorTarget(SOSM_Manager* aManagerPtr);
   virtual ~SOSM_SensorTarget();

   bool Initialize(const std::string& aSensorType, const std::string& aTargetType);

   float ComputeTargetIrradiance(SOSM_Interaction& aInteraction) const;

   float ComputeProbabilityOfDetection(float aTargetIrradiance, SOSM_Interaction& aInteraction) const;

   SOSM_Sensor* GetSensor() const { return mSensorPtr; }

   SOSM_Target* GetTarget() const { return mTargetPtr; }

   SOSM_Manager* GetManager() const { return mManagerPtr; }

protected:
   //! A sensor-atmosphere model pair.
   //! An instance of this exists for each sensor/atmosphere model pair.
   class SnrAtmPair
   {
   public:
      SnrAtmPair(SOSM_Manager* aManagerPtr);
      ~SnrAtmPair();
      SOSM_ScalarTable2D* mBackgroundRadiancePtr;
      SOSM_ScalarTable3D* mForegroundRadiancePtr;
      SOSM_ScalarTable3D* mTransmittancePtr;
   };

   //! A sensor-target model pair.
   //! An instance of this exists for each sensor/target-state pair.
   class SnrTgtPair
   {
   public:
      SnrTgtPair(SOSM_Target* aTargetPtr);
      ~SnrTgtPair();
      std::vector<SOSM_ScalarTable2D*> mProjectedAreaPtrs;
      std::vector<SOSM_ScalarTable2D*> mRadiantIntensityPtrs;
   };

   //! Copy constructor declared but not defined to prevent use.
   SOSM_SensorTarget(const SOSM_SensorTarget& aSrc) = delete;

   //! Assignment operator declared but not defined to prevent use.
   SOSM_SensorTarget operator=(const SOSM_SensorTarget& aRhs) = delete;

   SnrAtmPair* CreateOrLoadSensorAtmosphereTables();
   SnrTgtPair* CreateOrLoadSensorTargetTables();

   void LoadSensorTargetTables();

   SOSM_Manager* mManagerPtr;
   SOSM_Sensor*  mSensorPtr;
   SOSM_Target*  mTargetPtr;
   bool          mUsingTables;

   SnrAtmPair* mSnrAtmPtr;
   SnrTgtPair* mSnrTgtPtr;

   //! The key is '<snr-type>_<atm_type>'
   typedef std::map<std::string, SnrAtmPair*> SnrAtmTableMap;

   //! The key is '<snr-type>_<tgt_type>_<tgt_state>'
   typedef std::map<std::string, SnrTgtPair*> SnrTgtTableMap;

   static SnrAtmTableMap sSnrAtmTables;
   static SnrTgtTableMap sSnrTgtTables;
};

#endif
