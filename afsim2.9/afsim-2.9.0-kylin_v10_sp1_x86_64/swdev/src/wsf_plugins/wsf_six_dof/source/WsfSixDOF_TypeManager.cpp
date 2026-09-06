// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_TypeManager.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfEventPipe.hpp"
#include "WsfFuelTypes.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfPlugin.hpp"
#include "WsfPointMassSixDOF_Integrator.hpp"
#include "WsfPointMassSixDOF_Mover.hpp"
#include "WsfPointMassSixDOF_ThrustProducerObject.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfRigidBodySixDOF_Integrator.hpp"
#include "WsfRigidBodySixDOF_Mover.hpp"
#include "WsfRigidBodySixDOF_ThrustProducerObject.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_EventPipe.hpp"
#include "WsfSixDOF_ExplicitWeapon.hpp"
#include "WsfSixDOF_Fuel.hpp"
#include "WsfSixDOF_GuidanceComputer.hpp"
#include "WsfSixDOF_JetEngine.hpp"
#include "WsfSixDOF_ObjectManager.hpp"
#include "WsfSixDOF_Observer.hpp"
#include "WsfSixDOF_RamjetEngine.hpp"
#include "WsfSixDOF_RocketLiquidPropellantEngine.hpp"
#include "WsfSixDOF_RocketSolidPropellantEngine.hpp"
#include "WsfSixDOF_Terrain.hpp"
#include "WsfWeaponTypes.hpp"

const std::string wsf::six_dof::TypeManager::cDefaultPointMassSixDOF_IntegratorName = "STANDARD_POINT_MASS_INTEGRATOR";
const std::string wsf::six_dof::TypeManager::cDefaultRigidBodySixDOF_IntegratorName = "STANDARD_RIGID_BODY_INTEGRATOR";

void wsf::six_dof::TypeManager::AddedToScenario()
{
   WsfScenario& scenario(GetScenario());
   WsfMoverTypes::Get(scenario).Add("WSF_RIGID_BODY_SIX_DOF_MOVER", ut::make_unique<RigidBodyMover>(scenario));
   WsfMoverTypes::Get(scenario).Add("WSF_POINT_MASS_SIX_DOF_MOVER", ut::make_unique<PointMassMover>(scenario));

   WsfProcessorTypes::Get(scenario).Add("WSF_SIX_DOF_GUIDANCE_COMPUTER", ut::make_unique<GuidanceComputer>(scenario));
   WsfFuelTypes::Get(scenario).Add("WSF_SIX_DOF_FUEL", ut::make_unique<Fuel>(scenario));
   WsfWeaponTypes::Get(scenario).Add("WSF_SIX_DOF_EXPLICIT_WEAPON", ut::make_unique<ExplicitWeapon>(scenario));

   // If the "event_pipe" extension has been defined then hook things up so our events will also be logged.
   WsfEventPipeExtension* eventPipePtr = WsfEventPipeExtension::Find(GetScenario());
   if (eventPipePtr)
   {
      wsf::six_dof::EventPipe::RegisterEvents(*eventPipePtr);
   }
}

bool wsf::six_dof::TypeManager::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();
   if (command == "six_dof_object_types")
   {
      myCommand = true;
      UtInputBlock block(aInput);

      std::string subCommand;
      while (block.ReadCommand(subCommand))
      {
         if (subCommand == "rigid_body_vehicle_type")
         {
            auto vehicleType = ut::make_unique<RigidBodyMover>(GetScenario());
            if (vehicleType->ProcessInput(aInput))
            {
               RegisterObjectType(std::move(vehicleType));
            }
            else
            {
               myCommand = false;
            }
         }
         else if (subCommand == "point_mass_vehicle_type")
         {
            auto vehicleType = ut::make_unique<PointMassMover>(GetScenario());
            if (vehicleType->ProcessInput(aInput))
            {
               RegisterObjectType(std::move(vehicleType));
            }
            else
            {
               myCommand = false;
            }
         }
         else if (subCommand == "rigid_body_engine_type")
         {
            auto tempObjType = ut::make_unique<RigidBodyThrustProducerObject>();
            if (tempObjType->ProcessInput(aInput, this))
            {
               RegisterThrustProducerObjectType(std::move(tempObjType));
            }
            else
            {
               myCommand = false;
            }
         }
         else if (subCommand == "point_mass_engine_type")
         {
            auto tempObjType = ut::make_unique<PointMassThrustProducerObject>();
            if (tempObjType->ProcessInput(aInput, this))
            {
               RegisterThrustProducerObjectType(std::move(tempObjType));
            }
            else
            {
               myCommand = false;
            }
         }
         else if (subCommand == "integrators")
         {
            std::string filename;
            aInput.ReadValue(filename);
            filename = aInput.LocateFile(filename);
            SetIntegrators(filename);
         }
         else if (subCommand == "map_vehicle_to_platform")
         {
            std::string nameObjType;
            std::string namePlatformType;
            aInput.ReadValue(nameObjType);
            aInput.ReadValue(namePlatformType);

            AddVehiclePlatformTypeMatch(nameObjType, namePlatformType);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within TypeManager::ProcessInput().";
            out.AddNote() << "Command: " << subCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

bool wsf::six_dof::TypeManager::ProcessSubObjectInput(UtInput& aInput, std::list<std::unique_ptr<Mover>>& aSubObjectList)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "subobject")
   {
      std::string subName;
      std::string subBaseType;
      aInput.ReadValue(subName);
      aInput.ReadValue(subBaseType);

      for (auto& iter : aSubObjectList)
      {
         if (iter->GetBaseName() == subName)
         {
            throw UtInput::BadValue(aInput, "Duplicate sub-object name (" + subName + ")");
         }
      }

      const Mover* derivedFromObject = GetObjectTypeByName(subBaseType);

      auto subobject = VehicleFactory(derivedFromObject);
      if (!subobject)
      {
         auto out = ut::log::error()
                    << "TypeManager is trying to create a subobject with an unknown type in TypeManager::ProcessInput.";
         out.AddNote() << "Subobject Type: " << subBaseType;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }

      subobject->SetName(subBaseType);
      subobject->mBaseName        = subName;
      subobject->mDerivedFromName = subBaseType;

      UtInputBlock subBlock(aInput, "end_subobject");

      while (subBlock.ReadCommand())
      {
         if (command == "parent_rel_x")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cLENGTH);
            double x, y, z;
            subobject->mPosRelToParent_m.Get(x, y, z);
            subobject->mPosRelToParent_m.Set(num, y, z);
         }
         else if (command == "parent_rel_y")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cLENGTH);
            double x, y, z;
            subobject->mPosRelToParent_m.Get(x, y, z);
            subobject->mPosRelToParent_m.Set(x, num, z);
         }
         else if (command == "parent_rel_z")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cLENGTH);
            double x, y, z;
            subobject->mPosRelToParent_m.Get(x, y, z);
            subobject->mPosRelToParent_m.Set(x, y, num);
         }
         else if (command == "parent_rel_yaw")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGLE);
            double y, p, r;
            subobject->mAngRelToParentYPR_rad.Get(y, p, r);
            subobject->mAngRelToParentYPR_rad.Set(num, p, r);
         }
         else if (command == "parent_rel_pitch")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGLE);
            double y, p, r;
            subobject->mAngRelToParentYPR_rad.Get(y, p, r);
            subobject->mAngRelToParentYPR_rad.Set(y, num, r);
         }
         else if (command == "parent_rel_roll")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGLE);
            double y, p, r;
            subobject->mAngRelToParentYPR_rad.Get(y, p, r);
            subobject->mAngRelToParentYPR_rad.Set(y, p, num);
         }
         else if (command == "separation_vx")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cSPEED);
            num *= UtMath::cFT_PER_M;
            double x, y, z;
            subobject->mSeparationVel_fps.Get(x, y, z);
            subobject->mSeparationVel_fps.Set(num, y, z);
         }
         else if (command == "separation_vy")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cSPEED);
            num *= UtMath::cFT_PER_M;
            double x, y, z;
            subobject->mSeparationVel_fps.Get(x, y, z);
            subobject->mSeparationVel_fps.Set(x, num, z);
         }
         else if (command == "separation_vz")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cSPEED);
            num *= UtMath::cFT_PER_M;
            double x, y, z;
            subobject->mSeparationVel_fps.Get(x, y, z);
            subobject->mSeparationVel_fps.Set(x, y, num);
         }
         else if (command == "separation_omega_x")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
            double x, y, z;
            subobject->mSeparationOmega_rps.Get(x, y, z);
            subobject->mSeparationOmega_rps.Set(num, y, z);
         }
         else if (command == "separation_omega_y")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
            double x, y, z;
            subobject->mSeparationOmega_rps.Get(x, y, z);
            subobject->mSeparationOmega_rps.Set(x, num, z);
         }
         else if (command == "separation_omega_z")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
            double x, y, z;
            subobject->mSeparationOmega_rps.Get(x, y, z);
            subobject->mSeparationOmega_rps.Set(x, y, num);
         }
         else if (command == "size_factor_radius")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cLENGTH);
            subobject->mSizeFactor.SetRefSizeFactorRadius_m(num);
         }
         else if (command == "size_factor_min")
         {
            double num;
            aInput.ReadValue(num);
            subobject->mSizeFactor.SetSizeFactorMin(num);
         }
         else if (command == "size_factor_max")
         {
            double num;
            aInput.ReadValue(num);
            subobject->mSizeFactor.SetSizeFactorMax(num);
         }
         else if (command == "size_factor_volume_rate_m3_per_sec")
         {
            double num;
            aInput.ReadValue(num);
            subobject->mSizeFactor.SetSizeFactorVolumeRate_m3_perSec(num);
         }
         else if (command == "size_factor_area_rate_m2_per_sec")
         {
            double num;
            aInput.ReadValue(num);
            subobject->mSizeFactor.SetSizeFactorAreaRate_m2_perSec(num);
         }
         else if (command == "size_factor_radius_rate_m_per_sec")
         {
            double num;
            aInput.ReadValue(num);
            subobject->mSizeFactor.SetSizeFactorRadiusRate_m_perSec(num);
         }
         else if (command == "fixed_object")
         {
            bool value;
            aInput.ReadValue(value);
            subobject->mFixedObject = value;
         }
         else if (command == "nominal_max_mach")
         {
            double value;
            aInput.ReadValue(value);
            subobject->mMaxMach = value;
         }
         else if (command == "nominal_max_alpha")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            subobject->mMaxAlpha_deg = value * UtMath::cDEG_PER_RAD;
         }
         else if (command == "nominal_min_alpha")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            subobject->mMinAlpha_deg = value * UtMath::cDEG_PER_RAD;
         }
         else if (command == "nominal_max_beta")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            subobject->mMaxBeta_deg = value * UtMath::cDEG_PER_RAD;
         }
         else if (command == "integrator")
         {
            std::string value;
            aInput.ReadValue(value);
            subobject->mIntegratorTypeName = value;
         }
         else
         {
            auto out =
               ut::log::error() << "Unrecognized command in subobject block within TypeManager::ProcessInput().";
            out.AddNote() << "Command: " << command;
            out.AddNote() << "Subobject: " << subName;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      aSubObjectList.push_back(std::move(subobject));
   }

   return myCommand;
}

std::unique_ptr<wsf::six_dof::Mover> wsf::six_dof::TypeManager::VehicleFactory(const Mover* aSubObjectDerivedFrom) const
{
   std::unique_ptr<Mover> newVehicle{nullptr};

   auto subObjectPointMassBase = dynamic_cast<const PointMassMover*>(aSubObjectDerivedFrom);
   if (subObjectPointMassBase != nullptr)
   {
      // Subobject is drawing from a point-mass vehicle, so build and return one
      auto subobject = ut::make_unique<PointMassMover>(GetScenario());
      subobject->DeriveFrom(*subObjectPointMassBase);

      newVehicle.reset(subobject.release());
   }

   auto subObjectRigidBodyBase = dynamic_cast<const RigidBodyMover*>(aSubObjectDerivedFrom);
   if (subObjectRigidBodyBase != nullptr)
   {
      // Subobject is drawing from a rigid-body vehicle, so build and return one
      auto subobject = ut::make_unique<RigidBodyMover>(GetScenario());
      subobject->DeriveFrom(*subObjectRigidBodyBase);

      newVehicle.reset(subobject.release());
   }

   return newVehicle;
}

bool wsf::six_dof::TypeManager::ProcessIntegratorsInput(UtInput& aInput)
{
   const std::string& command = aInput.GetCommand();

   if (command == "integrators")
   {
      UtInputBlock block(aInput, "end_integrators");

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();
         if (localCommand == "create_integrator")
         {
            std::string value;
            aInput.ReadValue(value);
            if (value == cDefaultRigidBodySixDOF_IntegratorName)
            {
               // Note: This assumes the integratorType name is unique
               mIntegratorMap[cDefaultRigidBodySixDOF_IntegratorName] = ut::make_unique<RigidBodyIntegrator>();
            }
            else if (value == cDefaultPointMassSixDOF_IntegratorName)
            {
               // Note: This assumes the integratorType name is unique
               mIntegratorMap[cDefaultPointMassSixDOF_IntegratorName] = ut::make_unique<PointMassIntegrator>();
            }
            // Add other integrator types here -- if (integratorType == "INTEGRATOR_TYPE_NAME")
            else
            {
               auto out = ut::log::error()
                          << "Unrecognized integrator type within TypeManager::ProcessIntegratorsInput().";
               out.AddNote() << "Value: " << localCommand;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within TypeManager::ProcessIntegratorsInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return true;
}

void wsf::six_dof::TypeManager::SetIntegrators(const std::string& aFilename)
{
   try
   {
      UtInput inputObj;
      inputObj.PushInput(ut::make_unique<UtInputFile>(aFilename));
      ProcessIntegratorsInput(inputObj);
   }
   catch (UtInput::ExceptionBase& exception)
   {
      auto out = ut::log::error() << "Exception thrown while reading SixDOF integrator file.";
#undef GetMessage
      out.AddNote() << "Message: " << exception.GetMessage();
      out.AddNote() << "Location: " << exception.GetLocation();
      out.AddNote() << "File: " << aFilename;
   }
   catch (UtInputFile::OpenError&)
   {
      auto out = ut::log::error() << "Unable to open SixDOF integrator file.";
      out.AddNote() << "File: " << aFilename;
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Exception in SixDOF integrator file.";
      out.AddNote() << "File: " << aFilename;
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
   }
}

wsf::six_dof::Integrator* wsf::six_dof::TypeManager::GetIntegratorByName(const std::string& aName) const
{
   std::string name = aName;
   // Set the default name, if appropriate
   // if (name == "DEFAULT")
   //{
   //   name = cDefaultIntegratorName;
   //}

   const auto& iter = mIntegratorMap.find(name);
   if (iter != mIntegratorMap.end())
   {
      return iter->second;
   }

   return nullptr;
}

bool wsf::six_dof::TypeManager::RegisterObjectType(std::unique_ptr<Mover> aObjectTypePtr)
{
   std::string objectName = aObjectTypePtr->GetName();

   if (ObjectTypeNameIsUnique(objectName))
   {
      mObjectTypeMap[objectName] = std::move(aObjectTypePtr);
      return true;
   }
   return false;
}

void wsf::six_dof::TypeManager::UnregisterObjectType(const std::string& aTypeName)
{
   auto iter = mObjectTypeMap.find(aTypeName);
   if (iter != mObjectTypeMap.end())
   {
      mObjectTypeMap.erase(iter);
   }
}

bool wsf::six_dof::TypeManager::ObjectTypeExists(const std::string& aTypeName) const
{
   return mObjectTypeMap.find(aTypeName) != mObjectTypeMap.end();
}

bool wsf::six_dof::TypeManager::ObjectTypeNameIsUnique(const std::string& aTypeName) const
{
   return !ObjectTypeExists(aTypeName);
}

bool wsf::six_dof::TypeManager::ThrustProducerObjectTypeExists(const std::string& aTypeName) const
{
   return (mThrustProducerTypeMap.find(aTypeName) != mThrustProducerTypeMap.end());
}

bool wsf::six_dof::TypeManager::RegisterThrustProducerObjectType(std::unique_ptr<ThrustProducerObject> aObjectType)
{
   auto name = aObjectType->GetName();
   if (!ThrustProducerObjectTypeExists(name))
   {
      mThrustProducerTypeMap[name] = std::move(aObjectType);
      return true;
   }
   return false;
}

const wsf::six_dof::Mover* wsf::six_dof::TypeManager::GetObjectTypeByName(const std::string& aTypeName) const
{
   auto iter = mObjectTypeMap.find(aTypeName);
   if (iter != mObjectTypeMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

const wsf::six_dof::ThrustProducerObject* wsf::six_dof::TypeManager::GetThrustProducerObjectType(const std::string& aTypeName) const
{
   auto iter = mThrustProducerTypeMap.find(aTypeName);
   if (iter != mThrustProducerTypeMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

void wsf::six_dof::TypeManager::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("wsf_six_dof_observer", ut::make_unique<Observer>());
   aSimulation.RegisterExtension("wsf_six_dof_eventpipe", ut::make_unique<EventPipeInterface>());
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<ObjectManager>(this));
}

void wsf::six_dof::TypeManager::AddVehiclePlatformTypeMatch(const std::string& aNameVehicleType,
                                                            const std::string& aNamePlatformType)
{
   bool addItem = true;
   // First, confirm there isn't already a match for this object type
   for (auto& object : mObjectPlatformMatchList)
   {
      if (object.nameVehicleType == aNameVehicleType)
      {
         // Already exists -- over-write the previous data
         object.namePlatformType = aNamePlatformType;
         addItem                 = false;
         break;
      }
   }

   if (addItem)
   {
      // Not in the list, so add it
      ObjectPlatformMatch item;
      item.nameVehicleType  = aNameVehicleType;
      item.namePlatformType = aNamePlatformType;
      mObjectPlatformMatchList.push_back(item);
   }
}

std::string wsf::six_dof::TypeManager::GetVehiclePlatformTypeMatch(const std::string& aNameVehicleType) const
{
   std::string pltfrmType = "";
   for (auto& match : mObjectPlatformMatchList)
   {
      if (match.nameVehicleType == aNameVehicleType)
      {
         // Match found
         pltfrmType = match.namePlatformType;
         break;
      }
   }
   return pltfrmType;
}
