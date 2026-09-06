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

#include "SOSM_Manager.hpp"

#include <cstdlib>
#include <memory>

#include "SOSM_Sensor.hpp"
#include "SOSM_SensorTarget.hpp"
#include "SOSM_SimpleAtmosphere.hpp"
#include "SOSM_SimpleTarget.hpp"
#include "SOSM_TableAtmosphere.hpp"
#include "SOSM_TableTarget.hpp"
#include "SOSM_TestCommands.hpp"
#include "SOSM_Utility.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"

// =================================================================================================
// protected
SOSM_Manager::SOSM_Manager(const UtAtmosphere& aAtmosphere)
   : mUtAtmosphere(aAtmosphere)
   , mAtmosphereTypes()
   , mSensorTypes()
   , mTargetTypes()
   , mCacheDirectory("./sosm_cache")
   , mDebugLevel(0)
   , mIgnoreCacheFiles(false)
   , mWriteCacheFiles(true)
   , mUseFastDetectionMode(false)
   , mShowStatus(false)
   , mShowIRIPP_Data(false)
   , mTestCommandsPtr(nullptr) // new SOSM_TestCommands(this)) // Changed syntax to remove Windows compile warning
{
   mTestCommandsPtr = new SOSM_TestCommands(this); // Changed syntax to remove Windows compile warning
}

// =================================================================================================
// protected
SOSM_Manager::~SOSM_Manager()
{
   delete mTestCommandsPtr;
   for (AtmosphereTypes::iterator ati = mAtmosphereTypes.begin(); ati != mAtmosphereTypes.end(); ++ati)
   {
      delete (*ati).second;
   }
   for (SensorTypes::iterator sti = mSensorTypes.begin(); sti != mSensorTypes.end(); ++sti)
   {
      delete (*sti).second;
   }
   for (TargetTypes::iterator tti = mTargetTypes.begin(); tti != mTargetTypes.end(); ++tti)
   {
      delete (*tti).second;
   }
}

// =================================================================================================
//! Process a file containing SOSM commands.
//! @param aFileName The name of the file to be processed.
//! @throws Various exceptions if an input error occurs.
void SOSM_Manager::Load(const std::string& aFileName)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFileName);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open SOSM input file " + aFileName);
   }
   FileReferenced(aFileName); // Inform observers
   UtInput input;
   input.PushInput(std::move(inputPtr));
   Load(input);
}

// =================================================================================================
//! Process a stream containing SOSM commands.
//! @param aInput The input stream to be processed.
//! @throws Various exceptions if an input error occurs.
void SOSM_Manager::Load(UtInput& aInput)
{
   std::string command;
   while (ReadCommand(aInput, command))
   {
      try
      {
         if (!ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      catch (UtInput::ExceptionBase& e)
      {
         auto logError = ut::log::error() << "SOSM_Manager::Load - While processing Command!";
         logError.AddNote() << "Command: " << command;
         logError.AddNote() << "Input: " << aInput.GetLocation();
         logError.AddNote() << "Error Message: " << e.what();
         throw;
      }
      catch (std::exception& e)
      {
         auto logError = ut::log::error() << "SOSM_Manager::Load - While processing Command!";
         logError.AddNote() << "Command: " << command;
         logError.AddNote() << "Input: " << aInput.GetLocation();
         logError.AddNote() << "Error Message: " << e.what();
         throw;
      }
   }
}

// =================================================================================================
bool SOSM_Manager::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "load_atmosphere")
   {
      std::string typeName;
      aInput.ReadValue(typeName);
      std::string from;
      aInput.ReadValue(from);
      aInput.StringEqual(from, "from");
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName = aInput.LocateFile(fileName);
      LoadAtmosphereType(typeName, fileName);
   }
   else if (command == "load_sensor")
   {
      std::string typeName;
      aInput.ReadValue(typeName);
      std::string from;
      aInput.ReadValue(from);
      aInput.StringEqual(from, "from");
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName = aInput.LocateFile(fileName);
      LoadSensorType(typeName, fileName);
   }
   else if (command == "load_target")
   {
      std::string typeName;
      aInput.ReadValue(typeName);
      std::string from;
      aInput.ReadValue(from);
      aInput.StringEqual(from, "from");
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName = aInput.LocateFile(fileName);
      LoadTargetType(typeName, fileName);
   }
   else if (command == "cache_directory")
   {
      aInput.ReadValueQuoted(mCacheDirectory);
      mCacheDirectory                  = aInput.SubstitutePathVariables(mCacheDirectory);
      std::string::size_type endOffset = mCacheDirectory.size() - 1;
      if ((mCacheDirectory[endOffset] == '/') || (mCacheDirectory[endOffset] == '\\'))
      {
         mCacheDirectory = mCacheDirectory.substr(0, endOffset);
      }
   }
   else if (command == "default_atmosphere")
   {
      aInput.ReadValue(mDefaultAtmosphereType);
   }
   else if (command == "ignore_cache_files")
   {
      aInput.ReadValue(mIgnoreCacheFiles);
      // 'ignore_cache_files true' also sets 'writes_cache_files false'. If the user wants to ignore the
      // current cache and simply overwrite it then they must follow with a 'write_cache_files true'.
      if (mIgnoreCacheFiles)
      {
         mWriteCacheFiles = false;
      }
   }
   else if (command == "write_cache_files")
   {
      aInput.ReadValue(mWriteCacheFiles);
   }
   else if (command == "fast_detection_mode")
   {
      aInput.ReadValue(mUseFastDetectionMode);
   }
   else if (command == "show_status")
   {
      aInput.ReadValue(mShowStatus);
   }
   else if (command == "debug_level")
   {
      aInput.ReadValue(mDebugLevel);
   }
   else if (command == "debug")
   {
      mDebugLevel = 1;
   }
   else if (command == "show_iripp_data")
   {
      mShowIRIPP_Data = true;
   }
   else if (command == "spectral_print_format")
   {
      std::string format;
      aInput.ReadValue(format);
      if (format == "wavenumber")
      {
         SOSM_Utility::SetSpectralPrintFormat(SOSM_Utility::cSPF_WAVENUMBER);
      }
      else if (format == "wavelength")
      {
         SOSM_Utility::SetSpectralPrintFormat(SOSM_Utility::cSPF_WAVELENGTH);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   // The pre-allocation commands are executed immediately.
   // NOTE: They are only effective if fast_detection_mode is true at the time of processing.
   else if (command == "preallocate")
   {
      std::string sensorType;
      std::string targetType;
      aInput.ReadValue(sensorType);
      aInput.ReadValue(targetType);
      if (!PreallocatePair(sensorType, targetType))
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "preallocate_all")
   {
      if (!PreallocateAllPairs())
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (mTestCommandsPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Return a pointer to a new instance of the default atmosphere.
//! @returns A pointer to a new instance of the default atmosphere.
//! @note The return value will be 0 if the default atmosphere could not be created. This is
//! almost always a fatal error, but it is left to the caller to decide how to handle the situation.
SOSM_Atmosphere* SOSM_Manager::AllocateDefaultAtmosphere()
{
   SOSM_Atmosphere* atmospherePtr         = nullptr;
   std::string      defaultAtmosphereType = mDefaultAtmosphereType;
   if (!defaultAtmosphereType.empty())
   {
      atmospherePtr = AllocateAtmosphere(defaultAtmosphereType);
      if (atmospherePtr == nullptr)
      {
         auto logError = ut::log::error() << "Unable to create an instance of the Default Atmosphere!";
         logError.AddNote() << "Default Atmosphere: " << defaultAtmosphereType;
      }
   }
   else
   {
      ut::log::error() << "Default atmosphere not defined!";
   }
   return atmospherePtr;
}

// =================================================================================================
//! Create a sensor/target pairing object for the specified sensor and target types.
//! @param aSensorType The type of the sensor.
//! @param aTargetType The type of the target.
//! @returns A pointer to the sensor/target pairing object.
//! @note If either the sensor or target type has not been defined, the object will not be usable
//! for detection purposes. The caller is responsible for checking that the sensor and target pointers
//! are valid by ensuring that SOSM_SensorTarget::GetSensor() and SOSM_SensorTarget::GetTarget() both
//! return non-zero values.
SOSM_SensorTarget* SOSM_Manager::AllocateSensorTargetPair(const std::string& aSensorType, const std::string& aTargetType)
{
   SOSM_SensorTarget* pairPtr = new SOSM_SensorTarget(this);
   pairPtr->Initialize(aSensorType, aTargetType);
   return pairPtr;
}

// =================================================================================================
//! Allocate an instance of an atmosphere object.
//! @param aTypeName The name of the atmosphere type for which an instance is desired.
//! @returns A pointer to the instance or 0 if an instance could not be created.
SOSM_Atmosphere* SOSM_Manager::AllocateAtmosphere(const std::string& aTypeName)
{
   SOSM_Atmosphere* instancePtr = nullptr;
   SOSM_Atmosphere* typePtr     = FindAtmosphereType(aTypeName);
   if (typePtr != nullptr)
   {
      instancePtr = typePtr->Clone();
      if (!instancePtr->Initialize())
      {
         auto logError = ut::log::error() << "Failed to initialize new instance of Atmosphere Type!";
         logError.AddNote() << "Atmosphere Type: " << aTypeName;
         delete instancePtr;
         instancePtr = nullptr;
      }
   }
   return instancePtr;
}

// =================================================================================================
//! Allocate an instance of an sensor object.
//! @param aTypeName The name of the sensor type for which an instance is desired.
//! @returns A pointer to the instance or 0 if an instance could not be created.
SOSM_Sensor* SOSM_Manager::AllocateSensor(const std::string& aTypeName)
{
   SOSM_Sensor* instancePtr = nullptr;
   SOSM_Sensor* typePtr     = FindSensorType(aTypeName);
   if (typePtr != nullptr)
   {
      instancePtr = typePtr->Clone();
      if (!instancePtr->Initialize())
      {
         auto logError = ut::log::error() << "Failed to initialize new instance of Sensor Type!";
         logError.AddNote() << "Sensor Type: " << aTypeName;
         delete instancePtr;
         instancePtr = nullptr;
      }
   }
   return instancePtr;
}

// =================================================================================================
//! Allocate an instance of an target object.
//! @param aTypeName The name of the target type for which an instance is desired.
//! @returns A pointer to the instance or 0 if an instance could not be created.
SOSM_Target* SOSM_Manager::AllocateTarget(const std::string& aTypeName)
{
   SOSM_Target* instancePtr = nullptr;
   SOSM_Target* typePtr     = FindTargetType(aTypeName);
   if (typePtr != nullptr)
   {
      instancePtr = typePtr->Clone();
      if (!instancePtr->Initialize())
      {
         auto logError = ut::log::error() << "Failed to initialize new instance of Target Type!";
         logError.AddNote() << "Target Type: " << aTypeName;
         delete instancePtr;
         instancePtr = nullptr;
      }
   }
   return instancePtr;
}

// =================================================================================================
SOSM_Atmosphere* SOSM_Manager::FindAtmosphereType(const std::string& aTypeName)
{
   SOSM_Atmosphere*                typePtr = nullptr;
   AtmosphereTypes::const_iterator tli     = mAtmosphereTypes.find(aTypeName);
   if (tli != mAtmosphereTypes.end())
   {
      typePtr = (*tli).second;
   }
   return typePtr;
}

// =================================================================================================
SOSM_Sensor* SOSM_Manager::FindSensorType(const std::string& aTypeName)
{
   SOSM_Sensor*                typePtr = nullptr;
   SensorTypes::const_iterator tli     = mSensorTypes.find(aTypeName);
   if (tli != mSensorTypes.end())
   {
      typePtr = (*tli).second;
   }
   return typePtr;
}

// =================================================================================================
SOSM_Target* SOSM_Manager::FindTargetType(const std::string& aTypeName)
{
   SOSM_Target*                typePtr = nullptr;
   TargetTypes::const_iterator tli     = mTargetTypes.find(aTypeName);
   if (tli != mTargetTypes.end())
   {
      typePtr = (*tli).second;
   }
   return typePtr;
}

// =================================================================================================
SOSM_Atmosphere* SOSM_Manager::LoadAtmosphereType(const std::string& aTypeName, const std::string& aFileName)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFileName);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open atmosphere definition file " + aFileName);
   }
   FileReferenced(aFileName); // Inform observers
   UtInput input;
   input.PushInput(std::move(inputPtr));
   std::string      cachePrefix(GetCachePrefix(aFileName));
   SOSM_Atmosphere* typePtr = LoadAtmosphereType(aTypeName, cachePrefix, input);
   typePtr->UpdateModificationTime(SOSM_Utility::FileModificationTime(aFileName));
   return typePtr;
}

// =================================================================================================
SOSM_Atmosphere* SOSM_Manager::LoadAtmosphereType(const std::string& aTypeName, const std::string& aCachePrefix, UtInput& aInput)
{
   AtmosphereTypes::iterator tli = mAtmosphereTypes.find(aTypeName);
   if (tli != mAtmosphereTypes.end())
   {
      delete (*tli).second;
      mAtmosphereTypes.erase(tli);
   }

   std::string blockName;
   aInput.ReadCommand(blockName);
   aInput.StringEqual(blockName, "atmosphere_model");
   std::string modelName;
   aInput.ReadValue(modelName);

   SOSM_Atmosphere* typePtr = nullptr;
   if (modelName == "simple")
   {
      typePtr = new SOSM_SimpleAtmosphere(mUtAtmosphere);
   }
   else if (modelName == "table")
   {
      typePtr = new SOSM_TableAtmosphere(this);
   }
   else
   {
      throw UtInput::BadValue(aInput, "Unknown atmosphere_model selected: " + modelName);
   }

   typePtr->SetTypeName(aTypeName);
   typePtr->SetCachePrefix(aCachePrefix);
   LoadType(typePtr, aInput);

   // If this is the first atmosphere type and a default has not been defined, select it as the default.
   if (mDefaultAtmosphereType.empty() || mAtmosphereTypes.empty())
   {
      mDefaultAtmosphereType = aTypeName;
   }

   mAtmosphereTypes[aTypeName] = typePtr;
   return typePtr;
}

// =================================================================================================
SOSM_Sensor* SOSM_Manager::LoadSensorType(const std::string& aTypeName, const std::string& aFileName)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFileName);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open sensor definition file " + aFileName);
   }
   FileReferenced(aFileName); // Inform observers
   UtInput input;
   input.PushInput(std::move(inputPtr));
   std::string  cachePrefix(GetCachePrefix(aFileName));
   SOSM_Sensor* typePtr = LoadSensorType(aTypeName, cachePrefix, input);
   typePtr->UpdateModificationTime(SOSM_Utility::FileModificationTime(aFileName));
   return typePtr;
}

// =================================================================================================
SOSM_Sensor* SOSM_Manager::LoadSensorType(const std::string& aTypeName, const std::string& aCachePrefix, UtInput& aInput)
{
   SensorTypes::iterator tli = mSensorTypes.find(aTypeName);
   if (tli != mSensorTypes.end())
   {
      delete (*tli).second;
      mSensorTypes.erase(tli);
   }

   std::string blockName;
   aInput.ReadCommand(blockName);
   aInput.StringEqual(blockName, "sensor_model");
   std::string modelName;
   aInput.ReadValue(modelName);

   SOSM_Sensor* typePtr = nullptr;
   if (modelName == "default")
   {
      typePtr = new SOSM_Sensor(this);
   }
   else
   {
      throw UtInput::BadValue(aInput, "Unknown sensor_model selected: " + modelName);
   }

   typePtr->SetTypeName(aTypeName);
   typePtr->SetCachePrefix(aCachePrefix);
   LoadType(typePtr, aInput);
   mSensorTypes[aTypeName] = typePtr;
   return typePtr;
}

// =================================================================================================
SOSM_Target* SOSM_Manager::LoadTargetType(const std::string& aTypeName, const std::string& aFileName)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFileName);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open target definition " + aFileName);
   }
   FileReferenced(aFileName); // Inform observers
   UtInput input;
   input.PushInput(std::move(inputPtr));
   std::string  cachePrefix(GetCachePrefix(aFileName));
   SOSM_Target* typePtr = LoadTargetType(aTypeName, cachePrefix, input);
   typePtr->UpdateModificationTime(SOSM_Utility::FileModificationTime(aFileName));
   return typePtr;
}

// =================================================================================================
SOSM_Target* SOSM_Manager::LoadTargetType(const std::string& aTypeName, const std::string& aCachePrefix, UtInput& aInput)
{
   TargetTypes::iterator tli = mTargetTypes.find(aTypeName);
   if (tli != mTargetTypes.end())
   {
      delete (*tli).second;
      mTargetTypes.erase(tli);
   }

   std::string blockName;
   aInput.ReadCommand(blockName);
   aInput.StringEqual(blockName, "target_model");
   std::string modelName;
   aInput.ReadValue(modelName);

   SOSM_Target* typePtr = nullptr;
   if (modelName == "simple")
   {
      typePtr = new SOSM_SimpleTarget(this);
   }
   else if (modelName == "table")
   {
      typePtr = new SOSM_TableTarget(this);
   }
   else
   {
      throw UtInput::BadValue(aInput, "Unknown target_model selected: " + modelName);
   }

   typePtr->SetTypeName(aTypeName);
   typePtr->SetCachePrefix(aCachePrefix);
   LoadType(typePtr, aInput);
   mTargetTypes[aTypeName] = typePtr;
   return typePtr;
}

// =================================================================================================
// private static
std::string SOSM_Manager::GetCachePrefix(const std::string& aFileName)
{
   // Extract just the base name
   std::string cachePrefix(SOSM_Utility::BaseName(aFileName));

   // Remove the file extension if present
   std::string::size_type dotPos = cachePrefix.rfind('.');
   if (dotPos != std::string::npos)
   {
      cachePrefix = cachePrefix.substr(0, dotPos);
   }
   return cachePrefix;
}

// =================================================================================================
// private static
void SOSM_Manager::LoadType(SOSM_CoreBase* aTypePtr, UtInput& aInput)
{
   // The current command (atmosphere_model, sensor_model, target_model) is the model type.

   std::string  modelType(aInput.GetCommand());
   UtInputBlock inputBlock(aInput);

   std::unique_ptr<SOSM_CoreBase> autoTypePtr(aTypePtr); // Will delete object if exception thrown

   std::string command;
   try
   {
      while (inputBlock.ReadCommand(command))
      {
         if (!aTypePtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   catch (UtInput::ExceptionBase& e)
   {
      auto logError = ut::log::error() << "SOSM_Manager::LoadType - While processing Command!";
      logError.AddNote() << "Command: " << command;
      logError.AddNote() << "Input: " << aInput.GetLocation();
      logError.AddNote() << "Error Message: " << e.what();
      throw;
   }
   catch (std::exception& e)
   {
      auto logError = ut::log::error() << "SOSM_Manager::LoadType - While processing Command!";
      logError.AddNote() << "Command: " << command;
      logError.AddNote() << "Input: " << aInput.GetLocation();
      logError.AddNote() << "Error Message: " << e.what();
      throw;
   }

   // There shouldn't be anything else (other than comments) after the block terminator.

   if (aInput.TryReadCommand(command))
   {
      throw UtInput::BadValue(aInput, "Extraneous input after " + inputBlock.GetBlockTerminator());
   }

   try
   {
      aTypePtr->InputComplete();
   }
   catch (std::exception&)
   {
      auto logError = ut::log::error() << "Exception finalizing!";
      logError.AddNote() << "Model Type: " << modelType;
      logError.AddNote() << "SOSM_CoreBase Type: " << aTypePtr->GetTypeName();
      throw;
   }
   autoTypePtr.release(); // Prevent object from being deleted
}

// =================================================================================================
// private static
bool SOSM_Manager::ReadCommand(UtInput& aInput, std::string& aCommand)
{
   bool haveData = false;
   try
   {
      aInput.ReadCommand(aCommand);
      haveData = true;
   }
   catch (UtInput::EndOfData&)
   {
   }
   catch (std::exception& e)
   {
      auto logError = ut::log::error() << "Unexpected exception while reading a command!";
      logError.AddNote() << "Error Message: " << e.what();
      throw;
   }
   return haveData;
}

// =================================================================================================
//! Return the name of the cache file that is associated with an indicated text file.
//! This will also ensure the cache directory is created.
std::string SOSM_Manager::CacheFileName(const std::string& aTextFileName, const std::string& aCachePrefix)
{
   // Determine the file extension to be applied (based on machine type)
#if defined(_M_X64) || (_M_IX86) || defined(__i386) || defined(__x86_64) || defined(__amd64) || defined(__ia64) || \
   defined(__alpha)
   const std::string fileExtension(".bl4"); // Binary, little-endian, 4 byte floats
#elif defined(__host_mips) || defined(__sparc)
   const std::string fileExtension(".bb4"); // Binary, big-endian, 4 byte floats
#else
   const std::string fileExtension(".bin"); // Other
#endif

   // Apply the cache prefix (prefixes the file name, not the cache directory) if specified
   std::string cacheDirectory(CacheDirectory());
   if ((!aCachePrefix.empty()) && (aCachePrefix != "."))
   {
      cacheDirectory = cacheDirectory + '/' + aCachePrefix;
   }

   // First ensure the cache directory exists...
   if (!SOSM_Utility::MakePath(cacheDirectory))
   {
      throw UtException("Unable to create cache directory " + cacheDirectory);
   }

   // Form and return the file name
   return cacheDirectory + '/' + SOSM_Utility::BaseName(aTextFileName) + fileExtension;
}

// =================================================================================================
void SOSM_Manager::GetSensorTypes(std::vector<std::string>& aTypes) const
{
   aTypes.clear();
   for (SensorTypes::const_iterator tli = mSensorTypes.begin(); tli != mSensorTypes.end(); ++tli)
   {
      aTypes.push_back((*tli).first);
   }
}

// =================================================================================================
void SOSM_Manager::GetTargetTypes(std::vector<std::string>& aTypes) const
{
   aTypes.clear();
   for (TargetTypes::const_iterator tli = mTargetTypes.begin(); tli != mTargetTypes.end(); ++tli)
   {
      aTypes.push_back((*tli).first);
   }
}

// =================================================================================================
bool SOSM_Manager::PreallocatePair(const std::string& aSensorType, const std::string& aTargetType)
{
   if (!UseFastDetectionMode())
   {
      return true;
   }

   auto logInfo = ut::log::info() << "Pre-allocating fast detection tables for Sensor Type.";
   logInfo.AddNote() << "Sensor Type: " << aSensorType;
   logInfo.AddNote() << "Target Type: " << aTargetType;
   std::unique_ptr<SOSM_SensorTarget> pairPtr(new SOSM_SensorTarget(this));
   bool                               ok = pairPtr->Initialize(aSensorType, aTargetType);
   return ok;
}

// =================================================================================================
//! Pre-allocate all sensor/target fast detection mode tables (If fast detection mode enabled).
//! This is typically used to ensure all data is pre-computed for real-time operation.
bool SOSM_Manager::PreallocateAllPairs()
{
   if (!UseFastDetectionMode())
   {
      return true;
   }

   bool                     ok = true;
   std::vector<std::string> sensorTypes;
   std::vector<std::string> targetTypes;
   GetSensorTypes(sensorTypes);
   GetTargetTypes(targetTypes);
   for (size_t si = 0; si < sensorTypes.size(); ++si)
   {
      for (size_t ti = 0; ti < targetTypes.size(); ++ti)
      {
         ok &= PreallocatePair(sensorTypes[si], targetTypes[ti]);
      }
   }
   return ok;
}

// =================================================================================================
UtAtmosphere& SOSM_Manager::GetUtAtmosphere()
{
   return mUtAtmosphere;
}
