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

#include "P6DofTypeManager.hpp"

#include <algorithm> // std::find

#include "P6DofAtmosphere.hpp"
#include "P6DofGravity.hpp"
#include "P6DofIntegrator.hpp"
#include "P6DofJetEngineObject.hpp"
#include "P6DofPilotObject.hpp"
#include "P6DofRamjetEngineObject.hpp"
#include "P6DofRocketLiquidPropellantObject.hpp"
#include "P6DofRocketSolidPropellantObject.hpp"
#include "P6DofScenario.hpp"
#include "P6DofTerrain.hpp"
#include "P6DofThrustProducerObject.hpp"
#include "P6DofVehicle.hpp"
#include "P6DofVehicleType.hpp"
#include "P6DofWind.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"

const std::string P6DofTypeManager::mDefaultIntegratorName = "STANDARD_P6DOF_INTEGRATOR";

// =================================================================
P6DofTypeManager::P6DofTypeManager()
{
   mScenario = new P6DofScenario(this);

   // This is an example of how to add a type to a types map for a pilot object:
   // Define std::unordered_map<std::string, P6DofPilotObject*>  mPilotObjectTypesMap; in P6DofTypeManager.h
   // Then create an instance and add it to the map
   // P6DofPilotObject* controller = new P6DofPilotObject(mScenario);
   // mPilotObjectTypesMap["PilotController"] = controller;
}

P6DofTypeManager::~P6DofTypeManager()
{
   for (auto& i : mObjectTypeMap)
   {
      delete i.second;
   }
   for (auto& i : mThrustProducerTypeMap)
   {
      delete i.second;
   }
   for (auto& i : mIntegratorMap)
   {
      delete i.second;
   }

   delete mScenario;
}

P6DofTypeManager* P6DofTypeManager::Clone() const
{
   return new P6DofTypeManager(*this);
}

bool P6DofTypeManager::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "p6dof_object_type")
   {
      P6DofVehicleType* tempObjType = new P6DofVehicleType(mScenario);
      if (tempObjType->ProcessInput(aInput))
      {
         RegisterObjectType(tempObjType->GetName(), tempObjType);
      }
      else
      {
         myCommand = false;
      }
   }
   else if (command == "jet_engine_type")
   {
      P6DofJetEngineObject* tempObjType = new P6DofJetEngineObject(mScenario);
      if (tempObjType->ProcessInput(aInput))
      {
         RegisterThrustProducerObjectType(tempObjType);
      }
      else
      {
         myCommand = false;
      }
   }
   else if (command == "ramjet_engine_type")
   {
      P6DofRamjetEngineObject* tempObjType = new P6DofRamjetEngineObject(mScenario);
      if (tempObjType->ProcessInput(aInput))
      {
         RegisterThrustProducerObjectType(tempObjType);
      }
      else
      {
         myCommand = false;
      }
   }
   else if (command == "liquid_propellant_rocket_type")
   {
      P6DofRocketLiquidPropellantObject* tempObjType = new P6DofRocketLiquidPropellantObject(mScenario);
      if (tempObjType->ProcessInput(aInput))
      {
         RegisterThrustProducerObjectType(tempObjType);
      }
      else
      {
         myCommand = false;
      }
   }
   else if (command == "solid_propellant_rocket_type")
   {
      P6DofRocketSolidPropellantObject* tempObjType = new P6DofRocketSolidPropellantObject(mScenario);
      if (tempObjType->ProcessInput(aInput))
      {
         RegisterThrustProducerObjectType(tempObjType);
      }
      else
      {
         myCommand = false;
      }
   }
   else if (command == "p6dof_integrators")
   {
      std::string filename;
      aInput.ReadValue(filename);
      filename = aInput.LocateFile(filename);
      SetIntegrators(filename);
   }
   else if (command == "p6dof_atmosphere")
   {
      std::string filename;
      aInput.ReadValue(filename);
      filename = aInput.LocateFile(filename);
      mScenario->SetAtmosphere(new P6DofAtmosphere(filename));
   }
   else if (command == "p6dof_wind")
   {
      std::string filename;
      aInput.ReadValue(filename);
      mScenario->SetWind(new P6DofWind());
   }
   else if (command == "p6dof_gravity")
   {
      std::string filename;
      aInput.ReadValue(filename);
      mScenario->SetGravity(new P6DofGravity());
   }
   else if (command == "p6dof_terrain")
   {
      std::string filename;
      aInput.ReadValue(filename);
      filename = aInput.LocateFile(filename);
      mScenario->SetTerrain(new P6DofTerrain(filename));
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================
bool P6DofTypeManager::ProcessIntegratorsInput(UtInput& aInput)
{
   const std::string& command = aInput.GetCommand();

   if (command == "p6dof_integrators")
   {
      UtInputBlock block(aInput, "end_p6dof_integrators");

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();
         if (localCommand == "create_integrator")
         {
            std::string value;
            aInput.ReadValue(value);
            if (value == mDefaultIntegratorName)
            {
               // Note: This assumes the integratorType name is unique
               mIntegratorMap[mDefaultIntegratorName] = new P6DofIntegrator;
            }
            // Add other integrator types here -- if (integratorType == "INTEGRATOR_TYPE_NAME")
            else
            {
               auto out = ut::log::error()
                          << "Unrecognized integrator type within P6DofTypeManager::ProcessIntegratorsInput().";
               out.AddNote() << "Value: " << localCommand;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofTypeManager::ProcessIntegratorsInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return true;
}

// =================================================================
void P6DofTypeManager::SetIntegrators(const std::string& aFilename)
{
   try
   {
      UtInput inputObj;
      inputObj.PushInput(ut::make_unique<UtInputFile>(aFilename));
      ProcessIntegratorsInput(inputObj);
   }
   catch (UtInput::ExceptionBase& exception)
   {
      auto out = ut::log::error() << "Exception thrown while reading P6DOF integrator file.";
      out.AddNote() << "Message: " << exception.GetMessage();
      out.AddNote() << "Location: " << exception.GetLocation();
      out.AddNote() << "File: " << aFilename;
   }
   catch (UtInputFile::OpenError&)
   {
      auto out = ut::log::error() << "Unable to open P6DOF integrator file.";
      out.AddNote() << "File: " << aFilename;
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Exception in P6DOF integrator file.";
      out.AddNote() << "File: " << aFilename;
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
   }
}

const P6DofIntegrator* P6DofTypeManager::GetIntegratorByName(const std::string& aName) const
{
   std::string name = aName;
   // Set the default name, if appropriate
   if (name == "DEFAULT")
   {
      name = mDefaultIntegratorName;
   }

   const auto& iter = mIntegratorMap.find(name);
   if (iter != mIntegratorMap.end())
   {
      return iter->second;
   }

   return nullptr;
}

bool P6DofTypeManager::RegisterObjectType(const std::string& aTypeName, P6DofVehicleType* aObjectTypePtr)
{
   if (ObjectTypeNameIsUnique(aTypeName))
   {
      mObjectTypeMap[aTypeName] = aObjectTypePtr;
      return true;
   }
   return false;
}

void P6DofTypeManager::UnregisterObjectType(const std::string& aTypeName)
{
   auto iter = mObjectTypeMap.find(aTypeName);
   if (iter != mObjectTypeMap.end())
   {
      mObjectTypeMap.erase(iter);
   }
}

bool P6DofTypeManager::ObjectTypeExists(const std::string& aTypeName) const
{
   return mObjectTypeMap.find(aTypeName) != mObjectTypeMap.end();
}

bool P6DofTypeManager::ObjectTypeNameIsUnique(const std::string& aTypeName) const
{
   return !ObjectTypeExists(aTypeName);
}

std::string P6DofTypeManager::GetEngineBaseType(const std::string& aTypeName) const
{
   auto engineIter = mThrustProducerTypeMap.find(aTypeName);
   if (engineIter != mThrustProducerTypeMap.end())
   {
      return engineIter->second->GetClassType();
   }
   return "";
}

bool P6DofTypeManager::ThrustProducerObjectTypeExists(const std::string& aTypeName) const
{
   return (mThrustProducerTypeMap.find(aTypeName) != mThrustProducerTypeMap.end());
}


bool P6DofTypeManager::RegisterThrustProducerObjectType(P6DofThrustProducerObject* aObjectType)
{
   if (!ThrustProducerObjectTypeExists(aObjectType->GetName()))
   {
      mThrustProducerTypeMap[aObjectType->GetName()] = aObjectType;
      return true;
   }
   return false;
}

const P6DofVehicleType* P6DofTypeManager::GetObjectTypeByName(const std::string& aTypeName) const
{
   auto iter = mObjectTypeMap.find(aTypeName);
   if (iter != mObjectTypeMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

P6DofThrustProducerObject* P6DofTypeManager::GetThrustProducerObjectType(const std::string& aTypeName) const
{
   auto iter = mThrustProducerTypeMap.find(aTypeName);
   if (iter != mThrustProducerTypeMap.end())
   {
      return iter->second;
   }
   return nullptr;
}
