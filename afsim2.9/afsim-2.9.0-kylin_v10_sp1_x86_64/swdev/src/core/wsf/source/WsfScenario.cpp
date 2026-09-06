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

#include "WsfScenario.hpp"

#include <algorithm>
#include <memory>
#include <sstream>

#include "UtAtmosphere.hpp"
#include "UtBinder.hpp"
#include "UtDictionary.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtWallClock.hpp"
#include "WsfAdvancedBehaviorTreeNode.hpp"
#include "WsfAero.hpp"
#include "WsfAeroTypes.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfAntennaPatternTypes.hpp"
#include "WsfApplication.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfBehaviorTreeNode.hpp"
#include "WsfCallback.hpp"
#include "WsfCallbackTypes.hpp"
#include "WsfComm.hpp"
#include "WsfCommMediumTypes.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommNetworkTypes.hpp"
#include "WsfCommProtocolTypes.hpp"
#include "WsfCommRouterProtocolTypes.hpp"
#include "WsfCommRouterTypes.hpp"
#include "WsfCommTypes.hpp"
#include "WsfCommandChain.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfConsole.hpp"
#include "WsfCorrelationStrategyTypes.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfEM_AttenuationTypes.hpp"
#include "WsfEM_Clutter.hpp"
#include "WsfEM_ClutterTypes.hpp"
#include "WsfEM_PropagationTypes.hpp"
#include "WsfEarthGravityModel.hpp"
#include "WsfEnvironment.hpp"
#include "WsfException.hpp"
#include "WsfExclusionSensorComponent.hpp"
#include "WsfFilterTypes.hpp"
#include "WsfFuel.hpp"
#include "WsfFuelTypes.hpp"
#include "WsfFusionStrategyTypes.hpp"
#include "WsfGroup.hpp"
#include "WsfGroupTypes.hpp"
#include "WsfIFF_Manager.hpp"
#include "WsfIntersectMesh.hpp"
#include "WsfIntersectMeshTypes.hpp"
#include "WsfLOS_Manager.hpp"
#include "WsfMaskingPattern.hpp"
#include "WsfMaskingPatternTypes.hpp"
#include "WsfMessageTable.hpp"
#include "WsfMoonLOS_SensorComponent.hpp"
#include "WsfMover.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfNavigationErrors.hpp"
#include "WsfNoiseCloud.hpp"
#include "WsfPathFinder.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformAvailability.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfProcessor.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfRadarSignatureTypes.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteNetwork.hpp"
#include "WsfRouteNetworkTypes.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorErrorModelTypes.hpp"
#include "WsfSensorSignalProcessor.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfSignatureList.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfSolarElevationAtTargetComponent.hpp"
#include "WsfSolarIlluminationComponent.hpp"
#include "WsfStringId.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTerrain.hpp"
#include "WsfThermalSystem.hpp"
#include "WsfThermalSystemTypes.hpp"
#include "WsfTrackExtrapolationStrategyTypes.hpp"
#include "WsfTrackReportingStrategyTypes.hpp"
#include "WsfVersion.hpp"
#include "WsfVisualPartTypes.hpp"
#include "WsfZoneTypes.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"
#include "script/WsfScriptObserver.hpp"

// =================================================================================================
WsfScenario::WsfScenario(WsfApplication& aApplication)
   : mApplicationPtr(&aApplication)
   , mHadUnexpectedError(false)
   , mTestingForInputError(false)
   , mGotError(false)
   , mExpectInputError(false)
   , mExtensionsInOrder(false)
   , mLoadIsComplete(false)
   , mInputPtr(nullptr)
   , mDeferredInputPtr(ut::make_unique<WsfDeferredInput>())
   , mOpenConditionalBlocks(0)
   , mStringTable()
   , mSimulationInputPtr(nullptr)
   , mScriptTypesPtr(aApplication.GetScriptTypes())
   , mScriptEnvironmentPtr(ut::make_unique<UtScriptEnvironment>(aApplication.GetScriptTypes()))
   , mScriptManagerPtr(nullptr)
   , mEnvironmentPtr(nullptr)
   , mAtmospherePtr(ut::make_unique<UtAtmosphere>(UtAtmosphere::CreateNewAtmosphereTable()))
   , mMessageTablePtr(nullptr)
   , mPathFinderListPtr(nullptr)
   , mTerrainInterfacePtr(nullptr)
   , mIFF_ManagerPtr(nullptr)
   , mInputFiles()
   , mInputFunctions()
   , mUseQuantitativeTrackQuality(false)
   , mInitialRunNumber(1)
   , mFinalRunNumber(1)
   , mRunNumberIncrement(1)
   , mInitialRandomSeed(0)
   , mInitialSeed(0)
   , mRandomSeeds()
   , mInputPlatforms()
   , mPlatformAvailabilityPtr(ut::make_unique<WsfPlatformAvailability>())
   , mAllTypeLists()
   , mTypesListByKind()
   , mSignatureListPrototypePtr(ut::make_unique<WsfSignatureList>())
   , mExtensionListPtr(ut::make_unique<WsfExtensionList>())
   , mComponentFactoryList()
{
   // Create objects that must be created BEFORE type lists are created, that aren't already created.
   mScriptManagerPtr = ut::make_unique<WsfScriptManager>(this, mScriptEnvironmentPtr.get());

   // Then create the type lists
   CreateTypeLists(*this);

   mSimulationInputPtr  = ut::make_unique<WsfDefaultSimulationInput>(*this);
   mEnvironmentPtr      = ut::make_unique<WsfEnvironment>(*this);
   mPathFinderListPtr   = ut::make_unique<WsfPathFinderList>();
   mMessageTablePtr     = ut::make_unique<WsfMessageTable>();
   mTerrainInterfacePtr = ut::make_unique<wsf::TerrainInterface>(*this);
   mIFF_ManagerPtr      = ut::make_unique<WsfIFF_Manager>();

   RegisterExtension("comm_network_manager", ut::make_unique<wsf::comm::NetworkManagerExtension>());
   RegisterExtension("gravity_model", ut::make_unique<wsf::EarthGravityModelExtension>());
   RegisterExtension("los_manager", ut::make_unique<WsfLOS_ManagerExtension>());
   RegisterExtension("script_observer", ut::make_unique<WsfScriptObserverExtension>());

   // Create the main input object and attach the aux_data item used to contain the pointer back to the scenario.
   mInputPtr = ut::make_unique<UtInput>();
   ConfigureInput(*mInputPtr);

   // Inform the application the scenario has been created, which will cause scenario extensions to be registered.
   aApplication.ScenarioCreated(*this);
}

// =================================================================================================
//! Callback invoked by UtInput when opening a nested include file.
void WsfScenario::FileOpenCallback(const std::string& aFileName)
{
   GetSystemLog().WriteLogEntry("file " + aFileName);
}

// =================================================================================================
//! Load simulation input from the file with the specified name.
//! @param aFileName The name of a text file to be opened and loaded.
//! @throws  if errors are encountered.
void WsfScenario::LoadFromFile(const std::string& aFileName)
{
   mInputFiles.push_back(aFileName);
   try
   {
      mInputPtr->PushInput(ut::make_unique<UtInputFile>(aFileName));
      mInputPtr->SetFileOpenCallback([this](const std::string& aFileName) { FileOpenCallback(aFileName); });
      FileOpenCallback(aFileName);
      LoadFromStreamP(*mInputPtr);
   }
   catch (...)
   {
      mGotError = true;
      CheckErrorFailure();
      throw;
   }

   std::vector<WsfExtension*> extensions = mExtensionListPtr->GetExtensionsInOrder();
   for (auto& extension : extensions)
   {
      WsfScenarioExtension* extPtr = static_cast<WsfScenarioExtension*>(extension);
      extPtr->FileLoaded(aFileName);
   }
}

// =================================================================================================
//! Load simulation input from the file with the specified stream.
//! @param aInput Reference to the input stream.
//! @throws if errors are encountered.
void WsfScenario::LoadFromStream(UtInput& aInput)
{
   LoadFromStreamP(aInput);
   mDeferredInputPtr->Resolve(aInput);
}

// =================================================================================================
void WsfScenario::LoadFromStreamP(UtInput& aInput)
{
   ConfigureInput(aInput);

   std::string command;
   while (aInput.TryReadCommand(command))
   {
      // Use ProcessInputP instead of ProcessInput to avoid calling ConfigureInput so often
      if (ProcessInputP(aInput))
      {
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// =================================================================================================
bool WsfScenario::ProcessInput(UtInput& aInput)
{
   // Ensure the input object contains a pointer back to this scenario
   ConfigureInput(aInput);
   return ProcessInputP(aInput);
}

// =================================================================================================
bool WsfScenario::ProcessInputP(UtInput& aInput)
{
   std::string command = aInput.GetCommand();

   if (TypesProcessInput(aInput))
   {
   }
   else if (LoadPlatformInstance(aInput))
   {
   }
   else if (GetSystemLog().ProcessInput(aInput))
   {
   }
   else if (mSimulationInputPtr->ProcessInput(aInput))
   {
   }
   else if (mEnvironmentPtr->ProcessInput(aInput))
   {
   }
   else if (mAtmospherePtr->ProcessGlobalInput(aInput))
   {
   }
   else if (mMessageTablePtr->ProcessInput(aInput))
   {
   }
   else if (mScriptManagerPtr->ProcessInput(aInput))
   {
   }
   else if (ProcessExtensionInput(aInput))
   {
   }
   else if (mIFF_ManagerPtr->ProcessInput(aInput))
   {
   }
   else if (mPlatformAvailabilityPtr->ProcessInput(aInput))
   {
   }
   else if (command == "file_path")
   {
      std::string pathName;
      aInput.ReadValueQuoted(pathName);
      pathName = aInput.SubstitutePathVariables(pathName);
      // Get the path relative to the current file being processed
      UtPath currentFileDir(aInput.GetCurrentFileName());
      currentFileDir.Up();
      UtPath      path       = currentFileDir + pathName;
      std::string searchPath = path.GetNormalizedPath();
      if (searchPath.empty())
      {
         searchPath = ".";
      }
      aInput.AddToPathList(searchPath);
   }
   else if (command == "reset_file_path")
   {
      aInput.ResetPathList();
   }
   else if (command == "define_path_variable")
   {
      std::string name;
      std::string value;
      aInput.ReadValue(name);
      aInput.ReadValueQuoted(value);
      aInput.DefinePathVariable(name, value);
   }
   else if (command == "undefine_path_variable")
   {
      std::string name;
      aInput.ReadValue(name);
      aInput.UndefinePathVariable(name);
   }
   else if (command == "stream_debug_on")
   {
      // For use in debugging stream parsing problems where the offending keyword is not apparent.
      aInput.SetDebug(true);
   }
   else if (command == "stream_debug_off")
   {
      aInput.SetDebug(false);
   }
   else if (command == "enumerate")
   {
      ProcessEnumerateCommand(aInput);
   }
   else if (command == "initial_run_number")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueGreaterOrEqual(value, 1);
      SetInitialRunNumber(static_cast<unsigned int>(value));
   }
   else if ((command == "final_run_number") || (command == "number_of_runs"))
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueGreaterOrEqual(value, 1);
      SetFinalRunNumber(static_cast<unsigned int>(value));
   }
   else if (command == "run_number_increment")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueGreaterOrEqual(value, 1);
      SetRunNumberIncrement(static_cast<unsigned int>(value));
   }
   else if (command == "generate_random_seeds")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueGreater(value, 0);
      mInitialRandomSeed = value;
   }

   else if (command == "random_seed")
   {
      int randomSeed;
      aInput.ReadValue(randomSeed);
      aInput.ValueGreater(randomSeed, 0);
      mInitialSeed = static_cast<long int>(randomSeed);
   }
   else if (command == "random_seeds")
   {
      mRandomSeeds.clear();
      mInitialRandomSeed = 0; // disable generate_random_seeds
      UtInputBlock inputBlock(aInput);
      std::string  command2;
      while (inputBlock.ReadCommand(command2))
      {
         aInput.PushBack(command2);
         int randomSeed;
         aInput.ReadValue(randomSeed);
         aInput.ValueGreater(randomSeed, 0);
         mRandomSeeds.push_back(randomSeed);
      }
      // Providing an empty list is the same a reverting to use of the single random number established by random_seed
      // (or the default random seed).
      mInitialRunNumber = 1;
      mFinalRunNumber   = std::max(static_cast<int>(mRandomSeeds.size()), 1);
   }
   else if (command == "random_seed_time")
   {
      UtWallClock clock;
      long        timeMs = std::abs(static_cast<long>(1000 * clock.GetRawClock()));
      auto        out    = ut::log::info() << "Using time as random seed.";
      out.AddNote() << "Seed: " << timeMs;
      mInitialSeed = timeMs;
   }
   else if (command == "quantitative_track_quality")
   {
      aInput.ReadValue(mUseQuantitativeTrackQuality);
   }
   else if (command == "conditional_section")
   {
      bool passesConditionals = true;
      aInput.ReadCommand(command);
      if (command == "conditionals")
      {
         UtInputBlock block(aInput);
         std::string  conditionalCommand;
         while (block.ReadCommand(conditionalCommand))
         {
            if (conditionalCommand == "feature_present" || conditionalCommand == "feature_not_present")
            {
               bool        hasFeature = false;
               std::string feature;
               aInput.ReadValue(feature);
               const std::vector<WsfApplication::Feature>& registeredFeatures = GetApplication().GetRegisteredFeatures();
               for (const auto& registeredFeature : registeredFeatures)
               {
                  if (registeredFeature.mName == feature)
                  {
                     hasFeature = true;
                     break;
                  }
               }
               passesConditionals = passesConditionals && (hasFeature == (conditionalCommand == "feature_present"));
            }
            else if (conditionalCommand == "wsf_version")
            {
               std::string op, rhs;
               aInput.ReadValue(op);
               aInput.ReadValue(rhs);
               std::vector<std::string> tokens;
               UtStringUtil::Parse(rhs, tokens, ".");
               std::vector<int> compareVersion;
               for (const auto& token : tokens)
               {
                  compareVersion.push_back(UtStringUtil::ToInt(token));
               }
               bool             passes = false;
               std::vector<int> version{wsf::version::GetMajorVersion(),
                                        wsf::version::GetMinorVersion(),
                                        wsf::version::GetPatchVersion()};
               if (op == "<")
               {
                  passes = version < compareVersion;
               }
               else if (op == "<=")
               {
                  passes = version <= compareVersion;
               }
               else if (op == ">")
               {
                  passes = version > compareVersion;
               }
               else if (op == ">=")
               {
                  passes = version >= compareVersion;
               }
               else if (op == "=")
               {
                  passes = version == compareVersion;
               }
               else
               {
                  throw UtInput::BadValue(aInput, "Invalid Comparison operator");
               }
               passesConditionals = passesConditionals && passes;
            }
            else if (conditionalCommand == "type_present" || conditionalCommand == "type_not_present")
            {
               std::string typeName;
               aInput.ReadValue(typeName);
               const std::vector<WsfObjectTypeListBaseI*>& allTypeLists = GetTypeLists();
               bool                                        typePresent  = false;
               for (auto allTypeList : allTypeLists)
               {
                  if (allTypeList->Find(typeName) != nullptr)
                  {
                     typePresent = true;
                     break;
                  }
               }
               typePresent =
                  typePresent || FindExtension(typeName) != nullptr || WsfScriptManager::GetTypes().GetClass(typeName);

               bool passes        = typePresent == (conditionalCommand == "type_present");
               passesConditionals = passesConditionals && passes;
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Expected 'conditionals' command");
      }
      if (!passesConditionals)
      {
         int nestedBlocks = 1;
         while (nestedBlocks > 0)
         {
            aInput.ReadCommand(command);
            if (command == "conditional_section")
            {
               nestedBlocks++;
            }
            else if (command == "end_conditional_section")
            {
               --nestedBlocks;
            }
         }
      }
      else
      {
         ++mOpenConditionalBlocks;
      }
   }
   else if (command == "end_conditional_section")
   {
      if (mOpenConditionalBlocks == 0)
      {
         throw UtInput::UnknownCommand(aInput);
      }
      else
      {
         --mOpenConditionalBlocks;
      }
   }
   else if (command == "classification_levels")
   {
      if (!mClassificationsList.empty())
      {
         throw UtInput::BadValue(aInput, "Multiple classification_levels blocks are not allowed.");
      }

      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (command == "level")
         {
            std::string levelString;
            UtColor     classColor;
            aInput.ReadValueQuoted(levelString);
            if (FindClassificationLevel(levelString) != ut::npos)
            {
               throw UtInput::BadValue(aInput, "Duplicate level found in classification_levels block");
            }

            UtInputBlock levelBlock(aInput);
            while (levelBlock.ReadCommand(command))
            {
               if (command == "color")
               {
                  aInput.ReadValue(classColor);
               }
               else
               {
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            mClassificationsList.emplace_back(levelString, classColor);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "classification")
   {
      std::string levelString;

      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (command == "level")
         {
            aInput.ReadValueQuoted(levelString);
            size_t level = FindClassificationLevel(levelString);
            if (level != ut::npos)
            {
               mLevelIndex = (mLevelIndex == ut::npos) ? level : std::max(level, mLevelIndex);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Unknown classification: '" + levelString + "'");
            }
         }
         else if (command == "caveats")
         {
            std::string  caveat;
            UtInputBlock caveatsBlock(aInput);
            while (caveatsBlock.ReadCommand(caveat))
            {
               mCaveatsSet.insert(caveat);
            }
         }
         else if (command == "trigraphs")
         {
            std::string  trigraph;
            UtInputBlock trigraphsBlock(aInput);
            while (trigraphsBlock.ReadCommand(trigraph))
            {
               mTrigraphsSet.insert(trigraph);
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (levelString.empty())
      {
         throw UtInput::BadValue(aInput, "Classification blocks must contain a level command.");
      }
   }
   else if (command == "test_feature") // Do not document - used for automated tests
   {
      std::string feature;
      aInput.ReadValue(feature);
      bool                                        hasFeature         = false;
      const std::vector<WsfApplication::Feature>& registeredFeatures = GetApplication().GetRegisteredFeatures();
      for (const auto& registeredFeature : registeredFeatures)
      {
         if (registeredFeature.mName == feature)
         {
            hasFeature = true;
            break;
         }
      }
      if (!hasFeature)
      {
         // made this error due to it dealing with a termination
         auto out = ut::log::fatal() << "Feature not present.";
         out.AddNote() << "Missing Feature: " << feature;
         throw WsfApplication::FeatureNotPresent(mApplicationPtr->IsTestingEnabled());
      }
   }
   else if (command == "expect_input_error") // Do not document - used for automated tests
   {
      mTestingForInputError = true;
      bool value;
      aInput.ReadValue(value);
      ExpectError(value);
   }
   else if (command == "simulation_name")
   {
      aInput.ReadValueQuoted(mSimulationName);
   }
   else
   {
      bool myCommand = false;
      for (auto& mInputFunction : mInputFunctions)
      {
         if (mInputFunction(aInput))
         {
            myCommand = true;
         }
      }
      return myCommand;
   }
   return true;
}

// =================================================================================================
void WsfScenario::CompleteLoad()
{
   BuildClassificationString();

   if (!mExtensionsInOrder)
   {
      mExtensionListPtr->SortExtensions();
   }

   mTerrainInterfacePtr->Initialize();

   try
   {
      mDeferredInputPtr->Resolve(*mInputPtr);
   }
   catch (...)
   {
      mGotError = true;
      CheckErrorFailure();
      throw;
   }

   // all inputs are loaded;  'global const' values should be initialized here or earlier
   mMessageTablePtr->Initialize();

   GetZoneTypes().InitializeZones(*this);

   // Initialize the random seed vector if automatically generated seeds were requested.
   if ((mInitialRandomSeed != 0) && mRandomSeeds.empty())
   {
      ut::Random random;
      random.SetSeed(mInitialRandomSeed);
      mRandomSeeds.resize(mFinalRunNumber, 0);
      for (unsigned int i = 0; i < mFinalRunNumber; ++i)
      {
         while (mRandomSeeds[i] == 0)
         {
            long int seed = random.Uniform<long>();
            if ((seed != 0) && (seed != std::numeric_limits<long>::max()) &&
                (std::find(mRandomSeeds.begin(), mRandomSeeds.end(), seed) == mRandomSeeds.end()))
            {
               mRandomSeeds[i] = seed;
            }
         }
      }
   }

   // Call CompleteLoad() on all type lists
   for (auto& allTypeList : mAllTypeLists)
   {
      allTypeList->CompleteLoad();
   }

   // Call CompleteLoad() on all input platform
   for (auto& inputPlatform : mInputPlatforms)
   {
      inputPlatform->CompleteLoad(*this);
   }

   mSimulationInputPtr->LoadComplete();

   for (const auto& extStr : mExtensionListPtr->GetExtensionOrder())
   {
      FindExtension(extStr)->Complete();
   }

   mLoadIsComplete = true;

   for (const auto& extStr : mExtensionListPtr->GetExtensionOrder())
   {
      FindExtension(extStr)->Complete2();
   }
}

// =================================================================================================
//! Callback from a simulation to indicate that it has been created and is initializing.
//! This method is called by WsfSimulation::Initialize() immediately upon entry. Its main function
//! is to ensure that all extensions registered to the application and scenario are notified
//! of the creation of the simulation. The extensions
//!
//! @note This method should not be called directly by the user code.
void WsfScenario::SimulationCreated(WsfSimulation& aSimulation) const
{
   // Inform the application that the simulation has been created.
   // This will inform each of the application extensions
   GetApplication().SimulationCreated(aSimulation);

   // Inform each of the scenario extensions that the simulation has been created.
   for (const auto& extStr : mExtensionListPtr->GetExtensionOrder())
   {
      WsfScenarioExtension* extensionPtr = FindExtension(extStr);
      if (extensionPtr != nullptr)
      {
         extensionPtr->SimulationCreated(aSimulation);
      }
   }
}

// =================================================================================================
//! Find a type object of the specified kind and name.
//! @param aTypeKind The kind of type object that is to be found (e.g.: "sensor", "processor", etc.).
//!                  This is generally the 3rd argument to the constructor of WsfObjectTypeList<T>.
//! @param aTypeName The name of the type object that is to be found.
//! @returns The pointer to the requested object. This will be the null pointer if the requested object
//! could not be found.
WsfObject* WsfScenario::FindType(const std::string& aTypeKind, WsfStringId aTypeName) const
{
   WsfObjectTypeListBaseI* typeListPtr = GetTypes(aTypeKind);
   if (typeListPtr != nullptr)
   {
      return typeListPtr->Find(aTypeName);
   }
   return nullptr;
}

// =================================================================================================
//! Clone a type object of the specified kind and name.
//! @param aTypeKind The kind of type object that is to be cloned (e.g.: "sensor", "processor", etc.).
//!                  This is generally the 3rd argument to the constructor of WsfObjectTypeList<T>.
//! @param aTypeName The name of the type object that is to be cloned.
//! @returns The pointer to a copy of the requested object. This will be the null pointer if the
//! requested object could not be found.
WsfObject* WsfScenario::CloneType(const std::string& aTypeKind, WsfStringId aTypeName) const
{
   WsfObjectTypeListBaseI* typeListPtr = GetTypes(aTypeKind);
   if (typeListPtr != nullptr)
   {
      return typeListPtr->Clone(aTypeName);
   }
   return nullptr;
}

// =================================================================================================
//! Return pointer to the extension with the given name.
//! @returns null if the extension does not exist.
WsfScenarioExtension* WsfScenario::FindExtension(const std::string& aName) const
{
   return static_cast<WsfScenarioExtension*>(mExtensionListPtr->FindExtension(aName));
}

// =================================================================================================
//! Return the extension with the given name.
//! @throws UnknownExtension if the extension does not exist.
WsfScenarioExtension& WsfScenario::GetExtension(const std::string& aName) const
{
   return static_cast<WsfScenarioExtension&>(mExtensionListPtr->GetExtension(aName));
}

// ================================================================================================
//! Register an extension.
//!
//! @param aName [input] Name of the extension being registered
//! @param aExtensionPtr [input] A pointer to the extension instance to be registered.
//! Upon successful registration this class assumes ownership of the registered object and will
//! be responsible for its deletion.
//!
//! @throw DuplicateExtension if an extension of the same name already exists.
void WsfScenario::RegisterExtension(const std::string& aName, std::unique_ptr<WsfScenarioExtension> aExtensionPtr)
{
   auto* extensionPtr = aExtensionPtr.get();
   mExtensionListPtr->AddExtension(aName, std::move(aExtensionPtr));
   extensionPtr->AddedToScenarioP(aName, *this);
}

// ================================================================================================
//! Add a new extension dependency.
//! This guarantees that one extension is initialized before another. Because the initialization
//! order is inherited by the simulation extensions, the dependency only needs declared here.
//! This will override any dependencies defined in the application.
//! @param aExtensionName The name of the dependent extension
//! @param aDependsOnExtensionName The name of the extension to be initialized first
//! @param aRequired 'true' if the dependent extension cannot be used without the other
//!                  'false' if the dependent extension can still be used without the other
void WsfScenario::ExtensionDepends(const std::string& aExtensionName, const std::string& aDependsOnExtensionName, bool aRequired)
{
   mExtensionListPtr->AddDependency(aExtensionName, aDependsOnExtensionName, aRequired);
}

// =================================================================================================
void WsfScenario::RegisterComponentFactory(std::unique_ptr<WsfComponentFactoryBase> aFactoryPtr)
{
   aFactoryPtr->SetScenario(this);
   mComponentFactoryList.push_back(std::move(aFactoryPtr));
}

// =================================================================================================
bool WsfScenario::ProcessExtensionInput(UtInput& aInput)
{
   for (const auto& i : *mExtensionListPtr)
   {
      if (static_cast<WsfScenarioExtension*>(i.second.get())->ProcessInput(aInput))
      {
         return true;
      }
   }
   return false;
}

// =================================================================================================
void WsfScenario::AddInputProcessor(const InputFunction& aFunction)
{
   mInputFunctions.push_back(aFunction);
}

// =================================================================================================
WsfDeferredInput& WsfScenario::GetDeferredInput()
{
   return *mDeferredInputPtr;
}

// =================================================================================================
//! Returns the global script context.
//! @note This should only be used BEFORE the simulation is created.
//! The simulation will use a copy of this context!
WsfScriptContext* WsfScenario::GetScriptContext() const
{
   return &mScriptManagerPtr->GetContext();
}

// =================================================================================================
//! Return a reference to the system log object.
//! @note This is a convenience method that simply returns a reference to the object within the application.
WsfSystemLog& WsfScenario::GetSystemLog() const
{
   return mApplicationPtr->GetSystemLog();
}

// =================================================================================================
void WsfScenario::ScenarioChange()
{
   if (mLoadIsComplete)
   {
      throw UtException("Scenario modified after load complete");
   }
}

// =================================================================================================
//! Sets the simulation input object, replacing the existing input object.
//! Allows an application to override simulation inputs
//! @note Must be called before the first file is loaded
void WsfScenario::SetSimulationInput(WsfSimulationInput* aSimulationInput)
{
   assert(mSimulationInputPtr.get() != aSimulationInput);
   assert(mInputFiles.empty());
   mSimulationInputPtr.reset(aSimulationInput);
}

// =================================================================================================
//! Set the initial run number.
void WsfScenario::SetInitialRunNumber(unsigned int aInitialRunNumber)
{
   mInitialRunNumber         = aInitialRunNumber;
   unsigned int seedListSize = static_cast<unsigned int>(mRandomSeeds.size());
   if ((seedListSize != 0) && (mInitialRunNumber > seedListSize))
   {
      mInitialRunNumber = seedListSize;
      auto out          = ut::log::warning() << "Truncating initial_run_number to size of random_seeds list.";
      out.AddNote() << "New initial_run_number: " << mInitialRunNumber;
   }
}

// =================================================================================================
//! Set the final run number.
void WsfScenario::SetFinalRunNumber(unsigned int aFinalRunNumber)
{
   mFinalRunNumber           = aFinalRunNumber;
   unsigned int seedListSize = static_cast<unsigned int>(mRandomSeeds.size());
   if ((seedListSize != 0) && (mFinalRunNumber > seedListSize))
   {
      mFinalRunNumber = seedListSize;

      auto out = ut::log::warning() << "Truncating final_run_number to size of random_seeds list.";
      out.AddNote() << "New final_run_number: " << mFinalRunNumber;
   }
}

// =================================================================================================
//! Set the increment between run numbers.
void WsfScenario::SetRunNumberIncrement(unsigned int aRunNumberIncrement)
{
   mRunNumberIncrement = aRunNumberIncrement;
}

// =================================================================================================
void WsfScenario::CreateTypeLists(WsfScenario& aScenario)
{
   mAdvancedBehaviorTreeNodeTypesPtr   = CreateTypeList<WsfAdvancedBehaviorTreeNodeTypes>(aScenario);
   mAeroTypesPtr                       = CreateTypeList<WsfAeroTypes>(aScenario);
   mAntennaPatternTypesPtr             = CreateTypeList<WsfAntennaPatternTypes>(aScenario);
   mBehaviorTreeNodeTypesPtr           = CreateTypeList<WsfBehaviorTreeNodeTypes>(aScenario);
   mCallbackTypesPtr                   = CreateTypeList<WsfCallbackTypes>(aScenario);
   mCommTypesPtr                       = CreateTypeList<wsf::comm::Types>(aScenario);
   mCommProtocolTypesPtr               = CreateTypeList<wsf::comm::ProtocolTypes>(aScenario);
   mCorrelationTypesPtr                = CreateTypeList<WsfCorrelationStrategyTypes>(aScenario);
   mEM_AttenuationTypesPtr             = CreateTypeList<WsfEM_AttenuationTypes>(aScenario);
   mEM_ClutterTypesPtr                 = CreateTypeList<WsfEM_ClutterTypes>(aScenario);
   mEM_PropagationTypesPtr             = CreateTypeList<WsfEM_PropagationTypes>(aScenario);
   mFilterTypesPtr                     = CreateTypeList<WsfFilterTypes>(aScenario);
   mFuelTypesPtr                       = CreateTypeList<WsfFuelTypes>(aScenario);
   mFusionTypesPtr                     = CreateTypeList<WsfFusionStrategyTypes>(aScenario);
   mGroupTypesPtr                      = CreateTypeList<WsfGroupTypes>(aScenario);
   mIntersectMeshTypesPtr              = CreateTypeList<WsfIntersectMeshTypes>(aScenario);
   mMaskingPatternTypesPtr             = CreateTypeList<WsfMaskingPatternTypes>(aScenario);
   mMediumTypesPtr                     = CreateTypeList<wsf::comm::medium::Types>(aScenario);
   mMoverTypesPtr                      = CreateTypeList<WsfMoverTypes>(aScenario);
   mNetworkTypesPtr                    = CreateTypeList<wsf::comm::NetworkTypes>(aScenario);
   mNoiseCloudTypesPtr                 = CreateTypeList<WsfNoiseCloudTypes>(aScenario);
   mPathFinderTypesPtr                 = CreateTypeList<WsfPathFinderTypes>(aScenario);
   mPlatformTypesPtr                   = CreateTypeList<WsfPlatformTypes>(aScenario);
   mProcessorTypesPtr                  = CreateTypeList<WsfProcessorTypes>(aScenario);
   mRadarSignatureTypesPtr             = CreateTypeList<WsfRadarSignatureTypes>(aScenario);
   mRouteTypesPtr                      = CreateTypeList<WsfRouteTypes>(aScenario);
   mRouteNetworkTypesPtr               = CreateTypeList<WsfRouteNetworkTypes>(aScenario);
   mRouterTypesPtr                     = CreateTypeList<wsf::comm::router::Types>(aScenario);
   mRouterProtocolTypesPtr             = CreateTypeList<wsf::comm::router::ProtocolTypes>(aScenario);
   mSensorTypesPtr                     = CreateTypeList<WsfSensorTypes>(aScenario);
   mSensorErrorModelTypesPtr           = CreateTypeList<WsfSensorErrorModelTypes>(aScenario);
   mThermalSystemComponentTypesPtr     = CreateTypeList<WsfThermalSystemComponentTypes>(aScenario);
   mThermalSystemTypesPtr              = CreateTypeList<WsfThermalSystemTypes>(aScenario);
   mTrackReportingStrategyTypesPtr     = CreateTypeList<WsfTrackReportingStrategyTypes>(aScenario);
   mTrackExtrapolationStrategyTypesPtr = CreateTypeList<wsf::TrackExtrapolationStrategyTypes>(aScenario);
   mVisualPartTypesPtr                 = CreateTypeList<WsfVisualPartTypes>(aScenario);
   mZoneTypesPtr                       = CreateTypeList<WsfZoneTypes>(aScenario);

   for (const auto& typeList : mAllTypeLists)
   {
      assert(mTypesListByKind[typeList->GetBlockName()] == nullptr);
      mTypesListByKind[typeList->GetBlockName()] = typeList.get();
   }

   // Most type lists that represent things stored in the component list of WsfPlatform also
   // register a component factory. Register component factories for those things in the component
   // list that don't have factories.

   WsfCommandChain::RegisterComponentFactory(*this);
   WsfGeoPoint::RegisterComponentFactory(*this);
   WsfMoonLOS_SensorComponent::RegisterComponentFactory(*this);
   WsfNavigationErrors::RegisterComponentFactory(*this);
   WsfExclusionSensorComponent::RegisterComponentFactory(*this);
   WsfSolarIlluminationComponent::RegisterComponentFactory(*this);
   WsfSolarElevationAtTargetSensorComponent::RegisterComponentFactory(*this);
}

template<class T>
T* WsfScenario::CreateTypeList(WsfScenario& aScenario)
{
   auto  typeList  = ut::make_unique<T>(aScenario);
   auto* returnVal = typeList.get();
   mAllTypeLists.push_back(std::move(typeList));
   return returnVal;
}

// =================================================================================================
bool WsfScenario::TypesProcessInput(UtInput& aInput)
{
   for (auto& allTypeList : mAllTypeLists)
   {
      if (allTypeList->ProcessInput(aInput))
      {
         return true;
      }
   }
   if (mTerrainInterfacePtr->ProcessInput(aInput))
   {
   }
   else
   {
      return false;
   }
   return true;
}

// =================================================================================================
//! Returns the type list of the given name, or null if it does not exist
WsfObjectTypeListBaseI* WsfScenario::GetTypes(const std::string& aTypesKind) const
{
   auto it = mTypesListByKind.find(aTypesKind);
   if (it != mTypesListByKind.end())
   {
      return it->second;
   }
   return nullptr;
}

// =================================================================================================
long int WsfScenario::GetRandomSeed(unsigned int aRunNumber) const
{
   if (aRunNumber > 0 && mRandomSeeds.size() >= aRunNumber)
   {
      return mRandomSeeds[aRunNumber - 1];
   }
   else
   {
      ut::Random random;
      random.SetSeed(mInitialSeed);
      long int useSeed = mInitialSeed;
      for (unsigned int i = 1; i < aRunNumber; ++i)
      {
         //
         long int seed = random.Uniform<long>();
         if ((seed != 0) && (seed != std::numeric_limits<long>::max()))
         {
            useSeed = seed;
         }
         else
         {
            --i;
         }
      }
      return useSeed;
   }
}

// =================================================================================================
//! Return a specific entry from the list of input platforms.
//! @param aEntry The slot of the desired entry.  It must be valid, i.e.: 0 <= aEntry < GetInputPlatformCount().
//! @return Pointer to the requested input platform.
WsfPlatform* WsfScenario::GetInputPlatformEntry(unsigned int aEntry) const
{
   return mInputPlatforms[aEntry].get();
}

// =================================================================================================
//! Add or edit an instance of a 'type object' of this class on a platform.
//! @param aInput The input stream.
//! @returns 'true' if the command was processed or 'false' if not.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
bool WsfScenario::LoadPlatformInstance(UtInput& aInput)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "edit")
   {
      size_t      positionAfterEdit = aInput.GetCurrentInput()->GetOffset();
      std::string nextCmd;
      aInput.ReadValue(nextCmd);
      if (nextCmd != "platform")
      {
         aInput.PushBack(nextCmd);
      }
      else
      {
         myCommand                   = true;
         UtInput::StoredLocation loc = aInput.StoreLocation();

         UtInputBlock inputBlock(aInput, "end_platform");
         std::string  instanceName;
         aInput.ReadValue(instanceName);

         if (!GetDeferredInput().IsDelayLoading())
         {
            std::string cmd;
            while (inputBlock.ReadCommand(cmd))
            {
            }
            loc.mOffset = positionAfterEdit;
            GetDeferredInput().DelayLoad("edit_platform",
                                         instanceName,
                                         loc,
                                         UtStd::Bind(&WsfScenario::LoadPlatformInstance, this));
            return true;
         }
         GetDeferredInput().Requires("platform", instanceName);

         WsfPlatform* inputPlatformPtr = GetInputPlatformByName(instanceName);
         if (inputPlatformPtr != nullptr)
         {
            // Edit an existing platform instance.
            inputPlatformPtr->ProcessInputBlock(inputBlock, false);
         }
      }
   }
   if (aInput.GetCommand() == "platform")
   {
      UtInput::StoredLocation loc = aInput.StoreLocation();
      myCommand                   = true;
      UtInputBlock inputBlock(aInput);
      std::string  instanceName;
      aInput.ReadValue(instanceName);

      if (!GetDeferredInput().IsDelayLoading())
      {
         std::string cmd;
         while (inputBlock.ReadCommand(cmd))
         {
         }
         GetDeferredInput().DelayLoad("platform", instanceName, loc, UtStd::Bind(&WsfScenario::LoadPlatformInstance, this));
         return true;
      }

      // Determine if the platform with the same name already exists. If it does then we're editing the
      // existing instance. If it doesn't, or if the name is to be auto-generated (which would never
      // match an existing instance), then we simply create a new instance.

      WsfPlatform* inputPlatformPtr = nullptr;
      WsfStringId  instanceNameId   = instanceName;
      if (instanceName != "<default>")
      {
         inputPlatformPtr = GetInputPlatformByName(instanceNameId);
      }

      if (inputPlatformPtr != nullptr)
      {
         throw UtInput::BadValue(aInput,
                                 "Platform is already defined.  Use 'edit platform' to modify an existing platform.");
      }
      else
      {
         // Create a new platform instance of the specified type and populate with additional input.

         std::string baseType;
         aInput.ReadValue(baseType);

         if (nullptr == GetPlatformTypes().Find(baseType))
         {
            GetDeferredInput().Requires("platform_type", baseType);
         }
         std::unique_ptr<WsfPlatform> platformPtr(GetPlatformTypes().Clone(baseType));
         if (platformPtr != nullptr)
         {
            platformPtr->SetName(instanceNameId);
            platformPtr->ProcessInputBlock(inputBlock, false);
            AddInputPlatform(std::move(platformPtr));
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown platform type: " + baseType);
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Add an 'initial state' or 'input' platform.
//!
//! Add a platform that is to be considered part of the initial state of the simulation.
//! This method can only be used to add platforms prior to calling WsfSimulation::Initialize.
//! This is typically used by WsfSimulationLoader to add platforms defined in an input file.
//!
//! It is an error to invoke this after WsfSimulation::Initialize has been called.
//!
//! @param aPlatformPtr Pointer to the platform to be added.  This class assumes ownership
//!                     of the platform.
//! @return 'true' if the platform was added or 'false' if an not.
//!
//! @note The list of 'initial state' platforms is maintained separately from the list of
//! 'active' platforms.  The 'initial state' platforms are copied to the list of 'active'
//! platforms by WsfSimulation::Initialize.
bool WsfScenario::AddInputPlatform(std::unique_ptr<WsfPlatform> aPlatformPtr)
{
   bool ok = false;
   if (aPlatformPtr == nullptr)
   {
      ut::log::warning() << "WsfScenario: Attempting to add a null platform pointer.";
   }
   else if (find(mInputPlatforms.begin(), mInputPlatforms.end(), aPlatformPtr) != mInputPlatforms.end())
   {
      auto out = ut::log::warning() << "WsfScenario: Attempting to add the same platform twice.";
      out.AddNote() << "Name: " << aPlatformPtr->GetName();
      out.AddNote() << "Index: " << aPlatformPtr->GetIndex();
   }
   else if (aPlatformPtr->GetNameId() == nullptr)
   {
      auto out = ut::log::warning() << "WsfScenario: Attempting to add a platform that does not have a name.";
      out.AddNote() << "Pointer: " << aPlatformPtr.get();
   }
   else
   {
      // Duplicate name check is done when inserted in to the real simulation.
      ok = true;
      mInputPlatforms.push_back(std::move(aPlatformPtr));
   }
   return ok;
}

// =================================================================================================
//! Return a pointer to the 'input' platform given the string ID of the platform name.
//! @return The pointer to the platform or 0 if the platform does not exist.
WsfPlatform* WsfScenario::GetInputPlatformByName(WsfStringId aNameId)
{
   WsfPlatform* platformPtr = nullptr;
   for (auto& inputPlatform : mInputPlatforms)
   {
      if (inputPlatform->GetNameId() == aNameId)
      {
         platformPtr = inputPlatform.get();
         break;
      }
   }
   return platformPtr;
}

// =================================================================================================
//! Return a pointer to the 'input' platform given the string ID of the platform name.
//! @return The pointer to the platform or 0 if the platform does not exist.
const WsfPlatform* WsfScenario::GetInputPlatformByName(WsfStringId aNameId) const
{
   return const_cast<WsfScenario*>(this)->GetInputPlatformByName(aNameId);
}

// =================================================================================================
//! Return the number of platforms in the input platform list.
//! @return the number of platform in the input platform list.
unsigned int WsfScenario::GetInputPlatformCount() const
{
   return static_cast<unsigned int>(mInputPlatforms.size());
}

// =================================================================================================
//! Determine if an input platform defined in the scenario is to actually be added to a simulation.
//! A user may use the 'platform_availability' block to control what platforms in a scenario are
//! to actually be added to the simulation.
//! @param aPlatformPtr [input] Pointer to the platform being considered for addition.
//! @param aSimulation  [input] The simulation into which the platform will possibly be added.
//! @returns 'true' if the platform should be added to the simulation, or 'false' if not.
bool WsfScenario::PlatformIsAvailable(WsfPlatform* aPlatformPtr, WsfSimulation& aSimulation) const
{
   return mPlatformAvailabilityPtr->PlatformIsAvailable(aPlatformPtr, aSimulation);
}

// =================================================================================================
// Returns the scenario associated with the input
// throws WsfException if there is no scenario associated with the input
WsfScenario& WsfScenario::FromInput(UtInput& aInput)
{
   WsfScenario* scenarioPtr = static_cast<WsfScenario*>(aInput.GetAuxData(0));
   if (scenarioPtr == nullptr)
   {
      throw UtException("No WsfScenario associated with input");
   }
   return *scenarioPtr;
}

// =================================================================================================
// Configures a UtInput object for use by the scenario.
void WsfScenario::ConfigureInput(UtInput& aInput)
{
   // enables the use of WsfScenario::FromInput()
   aInput.SetAuxData(0, this);

   if (!mExtensionsInOrder)
   {
      SortExtensions();
   }
}

// =================================================================================================
// protected
void WsfScenario::SortExtensions()
{
   mExtensionListPtr->SortExtensions();
   mExtensionsInOrder = true;
}

// =================================================================================================
void WsfScenario::AddTypeList(std::unique_ptr<WsfObjectTypeListBaseI> aTypeListPtr)
{
   auto* ptr = aTypeListPtr.get();
   mAllTypeLists.push_back(std::move(aTypeListPtr));
   assert(mTypesListByKind[ptr->GetBlockName()] == nullptr);
   mTypesListByKind[ptr->GetBlockName()] = ptr;
}

// =================================================================================================
//! Declare if an error is expected.
// protected
void WsfScenario::ExpectError(bool aExpectError)
{
   CheckErrorFailure();
   mGotError         = false;
   mExpectInputError = aExpectError;
}

// =================================================================================================
//! Check to see if an unexpected error occurred.
// protected
void WsfScenario::CheckErrorFailure()
{
   if (mExpectInputError ^ mGotError)
   {
      mHadUnexpectedError = true;
      if (mTestingForInputError)
      {
         auto out = ut::log::info("test_fail") << "-FAIL- Error status mismatch.";
         out.AddNote() << "Actual: " << mGotError;
         out.AddNote() << "Expected: " << mExpectInputError;
      }
   }
}

// =================================================================================================
//! Process the 'enumerate' command.
// private
bool WsfScenario::ProcessEnumerateCommand(UtInput& aInput)
{
   UtInput::StoredLocation inputLocation(aInput.StoreLocation());

   std::string what;
   std::string to;
   std::string fileName;
   aInput.ReadCommand(what);
   aInput.ReadValue(to);
   aInput.StringEqual(to, "to");
   aInput.ReadValueQuoted(fileName);

   // We can't process the command until preceding deferred input has been completed.
   if (!GetDeferredInput().IsDelayLoading())
   {
      // A null string for the second argument indicates it is unnamed and there is no dependency check needed.
      GetDeferredInput().DelayLoad("enumerate", "", inputLocation, UtStd::Bind(&WsfScenario::ProcessEnumerateCommand, this));
      return true;
   }

   fileName = aInput.SubstitutePathVariables(fileName);

   std::ostream* outputFilePtr = &std::cout;
   std::ofstream outputFile;
   if (fileName != "STDOUT")
   {
      outputFile.open(fileName.c_str());
      if (!outputFile.is_open())
      {
         throw UtInput::BadValue(aInput, "Cannot open file: " + fileName);
      }
      outputFilePtr = &outputFile;
   }

   if (what == "dictionary")
   {
      (*outputFilePtr) << what << ":\n";
      for (int i = 1; i < WsfStringId::GetDictionary().GetCount(); ++i)
      {
         (*outputFilePtr) << i << "\t'" << i << "'\n";
      }
   }
   else
   {
      bool validCommand = false;

      // The list name must always with '_types' (e.g.: platform_types, sensor_types, etc.) as we are
      // enumerating types and not instance. Note that the names of the lists are keyed by their block
      // name (that is, the word that introduces a definition of that type in the input file - e.g.:
      // platform_type, sensor, comm, etc.). At the current time, platform_type is the only one that has
      // '_type' on the end. We must do a little special processing to make sure we catch all possibilities.

      size_t length    = what.size();
      size_t suffixLoc = what.rfind("_types");
      if ((suffixLoc != std::string::npos) && // "_types" was found
          ((suffixLoc + 6) == length))        // it was at the end of the string
      {
         // Try first without the '_types'. This will find the list named sensor when sensor_types is given.
         // (And similarly for most other type lists).
         std::map<std::string, WsfObjectTypeListBaseI*>::const_iterator tli =
            mTypesListByKind.find(what.substr(0, suffixLoc));
         if (tli == mTypesListByKind.end())
         {
            // Try without the trailing 's'. This will find the list named platform_type when platform_types is given.
            tli = mTypesListByKind.find(what.substr(0, length - 1));
         }
         if (tli != mTypesListByKind.end())
         {
            validCommand = true;
            (*outputFilePtr) << what << ":\n";

            std::vector<WsfStringId> typeIds;
            WsfObjectTypeListBaseI*  otlPtr(tli->second);
            otlPtr->GetTypeIds(typeIds);
            std::string typeName;
            for (auto& typeId : typeIds)
            {
               typeName = typeId;
               (*outputFilePtr) << typeId;
               WsfObject* typePtr = otlPtr->Find(typeName);
               if (typePtr == nullptr) // Should never happen
               {
                  (*outputFilePtr) << "\t'" << typeName << "'";
               }
               else
               {
                  const WsfObject::TypeList& derivedTypes = typePtr->GetTypeList();
                  for (auto derivedType : derivedTypes)
                  {
                     (*outputFilePtr) << "\t'" << derivedType << "'";
                  }
               }
               (*outputFilePtr) << '\n';
            }
         }
      }
      if (!validCommand)
      {
         throw UtInput::BadValue(aInput, "Unknown list: " + what);
      }
   }
   return true;
}

// =================================================================================================
//! Builds a string that holds the classification of a file along with its trigraphs and caveats
// protected
void WsfScenario::BuildClassificationString()
{
   if (mClassificationsList.size() > mLevelIndex)
   {
      mClassification = mClassificationsList[mLevelIndex];
   }

   for (const auto& trigraph : mTrigraphsSet)
   {
      mClassification.first += ("//" + trigraph);
   }

   if (!mCaveatsSet.empty())
   {
      mClassification.first += " - ";
      for (const auto& caveat : mCaveatsSet)
      {
         mClassification.first += caveat;
         // If it is not the last caveat, add the delimiter
         if (caveat != *mCaveatsSet.rbegin())
         {
            mClassification.first += "//";
         }
      }
   }
}

size_t WsfScenario::FindClassificationLevel(const std::string& aLevel) const
{
   auto iter = std::find_if(mClassificationsList.begin(),
                            mClassificationsList.end(),
                            [aLevel](const ClassificationLevel& level) { return level.first == aLevel; });
   if (iter != mClassificationsList.end())
   {
      return std::distance(mClassificationsList.begin(), iter);
   }
   return ut::npos;
}
