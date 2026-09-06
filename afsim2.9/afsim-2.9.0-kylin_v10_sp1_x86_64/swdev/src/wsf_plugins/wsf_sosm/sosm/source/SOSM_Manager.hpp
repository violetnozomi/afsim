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

#ifndef SOSM_MANAGER_HPP
#define SOSM_MANAGER_HPP

#include "sosm_export.h"

#include <map>
#include <string>

class SOSM_Atmosphere;
class SOSM_CoreBase;
class SOSM_Sensor;
class SOSM_SensorTarget;
class SOSM_Target;
class SOSM_TestCommands;
#include "UtAtmosphere.hpp"
#include "UtCallback.hpp"
class UtInput;

//! A singleton class that manages SOSM data.
class SOSM_EXPORT SOSM_Manager
{
public:
   SOSM_Manager(const UtAtmosphere& aAtmosphere);
   ~SOSM_Manager();

   void Load(const std::string& aFileName);
   void Load(UtInput& aInput);
   bool ProcessInput(UtInput& aInput);

   UtAtmosphere& GetUtAtmosphere();

   SOSM_Atmosphere* AllocateDefaultAtmosphere();

   SOSM_SensorTarget* AllocateSensorTargetPair(const std::string& aSensorType, const std::string& aTargetType);

   SOSM_Atmosphere* AllocateAtmosphere(const std::string& aTypeName);
   SOSM_Sensor*     AllocateSensor(const std::string& aTypeName);
   SOSM_Target*     AllocateTarget(const std::string& aTypeName);

   SOSM_Atmosphere* FindAtmosphereType(const std::string& aTypeName);
   SOSM_Sensor*     FindSensorType(const std::string& aTypeName);
   SOSM_Target*     FindTargetType(const std::string& aTypeName);
   SOSM_Atmosphere* LoadAtmosphereType(const std::string& aTypeName, const std::string& aFileName);
   SOSM_Atmosphere* LoadAtmosphereType(const std::string& aTypeName, const std::string& aCachePrefix, UtInput& aInput);

   SOSM_Sensor* LoadSensorType(const std::string& aTypeName, const std::string& aFileName);
   SOSM_Sensor* LoadSensorType(const std::string& aTypeName, const std::string& aCachePrefix, UtInput& aInput);

   SOSM_Target* LoadTargetType(const std::string& aTypeName, const std::string& aFileName);
   SOSM_Target* LoadTargetType(const std::string& aTypeName, const std::string& aCachePrefix, UtInput& aInput);

   //! Return the name of the cache directory.
   std::string& CacheDirectory() { return mCacheDirectory; }

   //! Indicate if cached binary files should be ignored.
   bool IgnoreCacheFiles() { return mIgnoreCacheFiles; }

   //! Indicate if cached binary files should be created.
   bool WriteCacheFiles() { return mWriteCacheFiles; }

   //! Indicate if status messages should be shown during file loading.
   bool ShowStatus() { return mShowStatus; }

   //! Indicate if the 'fast detection mode' should be used.
   //! The 'fast detection model' uses pre-computed values which have already been spectrally integrated.
   bool UseFastDetectionMode() { return mUseFastDetectionMode; }

   //! Return the debug level (<= 0 if no debugging)
   int DebugLevel() { return mDebugLevel; }

   //! Return true if IRIPP comparison data is to be shown.
   bool ShowIRIPP_Data() { return mShowIRIPP_Data; }

   //! @name File referenced notification.
   //! The following exists to provide the controlling simulation a method to receive notification when
   //! the model references a file.
   //@{
   UtCallbackListN<void(const std::string&)> FileReferenced;
   //@}

   std::string CacheFileName(const std::string& aTextFileName, const std::string& aCachePrefix);

   //! @name Utility routines to assist is pre-allocation of fast-detection mode tables.
   //@{
   void GetSensorTypes(std::vector<std::string>& aTypes) const;
   void GetTargetTypes(std::vector<std::string>& aTypes) const;
   bool PreallocatePair(const std::string& aSensorType, const std::string& aTargetType);
   bool PreallocateAllPairs();
   //@}

protected:
   // Copy constructor declared but not defined to prevent use.
   SOSM_Manager(const SOSM_Manager& aSrc);

   // Assignment operator declared but not defined to prevent use.
   SOSM_Manager operator=(const SOSM_Manager& aRhs) = delete;

   std::string GetCachePrefix(const std::string& aFileName);

   void LoadFromStream(UtInput& aInput);

   void LoadType(SOSM_CoreBase* aTypePtr, UtInput& aInput);

   bool ReadCommand(UtInput& aInput, std::string& aCommand);

   void TestDriver(UtInput& aInput);

   typedef std::map<std::string, SOSM_Atmosphere*> AtmosphereTypes;
   typedef std::map<std::string, SOSM_Sensor*>     SensorTypes;
   typedef std::map<std::string, SOSM_Target*>     TargetTypes;
   UtAtmosphere                                    mUtAtmosphere;
   AtmosphereTypes                                 mAtmosphereTypes;
   SensorTypes                                     mSensorTypes;
   TargetTypes                                     mTargetTypes;
   std::string                                     mDefaultAtmosphereType;
   std::string                                     mCacheDirectory;
   int                                             mDebugLevel;
   bool                                            mIgnoreCacheFiles;
   bool                                            mWriteCacheFiles;
   bool                                            mUseFastDetectionMode;
   bool                                            mShowStatus;
   bool                                            mShowIRIPP_Data;
   SOSM_TestCommands*                              mTestCommandsPtr;
};

#endif
