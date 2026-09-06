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

#ifndef WSFSIXDOFTYPEMANAGER_HPP
#define WSFSIXDOFTYPEMANAGER_HPP

#include "wsf_six_dof_export.h"

#include <unordered_map>

class UtInput;
#include "WsfScenarioExtension.hpp"
class WsfSimulation;
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_Integrator.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfSixDOF_ThrustProducerObject.hpp"

namespace wsf
{
namespace six_dof
{
class TypeManager : public WsfScenarioExtension
{
public:
   TypeManager()  = default;
   ~TypeManager() = default;

   // The extension has been added to the scenario. GetScenario() is now callable.
   // This adds "WSF_RIGID_BODY_SIX_DOF_MOVER" (WsfSixDOF_RigidBodyMover) and
   // "WSF_POINT_MASS_SIX_DOF_MOVER" (WsfSixDOF_PointMassMover),
   // "WSF_SIX_DOF_GUIDANCE_COMPUTER" (WsfSixDOF_GuidanceComputer),
   // "WSF_SIX_DOF_FUEL" (WsfSixDOF_Fuel), and "WSF_SIX_DOF_EXPLICIT_WEAPON"
   // (WsfSixDOF_ExplicitWeapon). It also registers ScriptTypes for
   // WsfScriptRigidBodySixDOF_MoverClass (for WsfSixDOF_RigidBodyMover) and
   // WsfScriptPointMassSixDOF_MoverClass (for WsfSixDOF_PointMassMover).
   void AddedToScenario() override;

   // This calls WsfSimulation::RegisterExtension() every time a new simulation is
   // created, giving it a new SixDOF object manager.
   void SimulationCreated(WsfSimulation& aSimulation) override;

   // This reads "six_dof_object_types" input blocks
   bool ProcessInput(UtInput& aInput) override;

   // This reads "subobject" input blocks
   bool ProcessSubObjectInput(UtInput& aInput, std::list<std::unique_ptr<Mover>>& aSubObjectList);

   // This creates a new vehicle instance to push to a subobject list
   std::unique_ptr<Mover> VehicleFactory(const Mover* aSubObjectType) const;

   // SetIntegrators sets the RigidBodyIntegrator objects
   void SetIntegrators(const std::string& aFilename);

   // This returns an integrator of the specified type/name or null if
   // no such integrator exists
   Integrator* GetIntegratorByName(const std::string& aTypeName) const;

   // This registers a mover. All ObjectType objects call this
   // when being created. It allows the manager to be aware of all
   // ObjectType objects. It returns false if it fails.
   bool RegisterObjectType(std::unique_ptr<Mover> aObjectTypePtr);

   // This de-registers a mover. This should be called prior to
   // deleting an ObjectType. It allows the manager to be aware
   // that the de-registered object is no longer available.
   void UnregisterObjectType(const std::string& aTypeName);

   // This returns a SixDOF Mover based on the type name.
   // Null is returned if an object type with the desired name does not exist
   const Mover* GetObjectTypeByName(const std::string& aTypeName) const;

   bool RegisterThrustProducerObjectType(std::unique_ptr<ThrustProducerObject> aObjectType);

   const ThrustProducerObject* GetThrustProducerObjectType(const std::string& aTypeName) const;

   // Returns true if the type of object exists
   bool ObjectTypeExists(const std::string& aTypeName) const;

   // Returns true if the name is unique
   bool ObjectTypeNameIsUnique(const std::string& aTypeName) const;

   // Returns the string representing the default integrator name
   const std::string GetDefaultPointMassSixDOF_IntegratorName() const
   {
      return cDefaultPointMassSixDOF_IntegratorName;
   };

   // Returns the string representing the default integrator name
   const std::string GetDefaultRigidBodySixDOF_IntegratorName() const
   {
      return cDefaultRigidBodySixDOF_IntegratorName;
   };

   // Returns true if the type of object exists
   bool ThrustProducerObjectTypeExists(const std::string& aTypeName) const;

   // This provides the number of object types currently in the list
   size_t GetNumberOfObjectTypesInList() const { return mObjectTypeMap.size(); }

   // This returns the platform type name for the specified SixDOF vehicle type. If no match is
   // found, an empty string will be returned.
   std::string GetVehiclePlatformTypeMatch(const std::string& aNameVehicleType) const;

protected:
   // This adds the specified SixDOF vehicle type and platform type matching to the master list,
   // as long as the vehicle type name is unique.
   void AddVehiclePlatformTypeMatch(const std::string& aNameVehicleType, const std::string& aNamePlatformType);

   bool ProcessIntegratorsInput(UtInput& aInput);

private:
   struct ObjectPlatformMatch
   {
      std::string nameVehicleType;
      std::string namePlatformType;
   };

   // This provides a list of vehicle to Platform matches
   std::list<ObjectPlatformMatch> mObjectPlatformMatchList;

   std::unordered_map<std::string, UtCloneablePtr<Mover>>                mObjectTypeMap;
   std::unordered_map<std::string, UtCloneablePtr<ThrustProducerObject>> mThrustProducerTypeMap;
   std::unordered_map<std::string, UtCloneablePtr<Integrator>>           mIntegratorMap;

   const static std::string cDefaultPointMassSixDOF_IntegratorName;
   const static std::string cDefaultRigidBodySixDOF_IntegratorName;
};
} // namespace six_dof
} // namespace wsf

#endif
