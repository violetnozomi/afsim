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

#ifndef P6DOFTYPEMANAGER_H
#define P6DOFTYPEMANAGER_H

#include "p6dof_export.h"

#include <list>
#include <string>
#include <unordered_map>

class P6DofIntegrator;
class P6DofVehicleType;
class P6DofPilotObject;
class P6DofScenario;
class P6DofThrustProducerObject;
class UtInput;

// The P6DofTypeManager manages lists of object types.
class P6DOF_EXPORT P6DofTypeManager
{
public:
   P6DofTypeManager();
   ~P6DofTypeManager();

   P6DofTypeManager& operator=(const P6DofTypeManager& other) = delete;

   P6DofTypeManager* Clone() const;

   // TODO -- Determine if this could be handled better. Currently not
   // sure how to get WSF to initialize correctly without it
   P6DofScenario* GetP6DofScenario() const { return mScenario; }

   // This reads "p6dof_object_types" input blocks
   bool ProcessInput(UtInput& aInput);

   // SetIntegrators sets the P6DofIntegrator objects
   void SetIntegrators(const std::string& aFilename);

   // This registers a mover. All P6dofObjectType objects call this
   // when being created. It allows the manager to be aware of all
   // P6dofObjectType objects. It returns false if it fails.
   bool RegisterObjectType(const std::string& aTypeName, P6DofVehicleType* aObjectTypePtr);

   // This de-registers a mover. This should be called prior to
   // deleting a P6dofObjectType. It allows the manager to be aware
   // that the de-registered object is no longer available.
   void UnregisterObjectType(const std::string& aTypeName);

   // This returns a P6DofVehicleType based on the type name.
   // Null is returned if an object type with the desired name does not exist
   const P6DofVehicleType* GetObjectTypeByName(const std::string& aTypeName) const;

   P6DofThrustProducerObject* GetThrustProducerObjectType(const std::string& aTypeName) const;

   bool RegisterThrustProducerObjectType(P6DofThrustProducerObject* aObjectType);

   // Returns true if the type of object exists
   bool ObjectTypeExists(const std::string& aTypeName) const;

   // Returns true if the name is unique
   bool ObjectTypeNameIsUnique(const std::string& aTypeName) const;

   // Returns the string representing the engine base type (such as "Jet" or "Ramjet")
   std::string GetEngineBaseType(const std::string& aTypeName) const;

   // Returns true if the type of object exists
   bool ThrustProducerObjectTypeExists(const std::string& aTypeName) const;

   // This provides the number of object types currently in the list
   size_t GetNumberOfObjectTypesInList() const { return mObjectTypeMap.size(); }

   // This returns an integrator of the specified type/name or null if
   // no such integrator exists
   const P6DofIntegrator* GetIntegratorByName(const std::string& aTypeName) const;

   // This return the default integrator type name
   std::string GetDefaultIntegratorName() const { return mDefaultIntegratorName; }

protected:
   // Type Manager should only be instantiated once, don't allow accidental copies
   P6DofTypeManager(const P6DofTypeManager& /*aSrc*/)
      : mObjectTypeMap()
      , mThrustProducerTypeMap()
      , mScenario(nullptr)
      , mIntegratorMap()
   {
   }

   bool ProcessIntegratorsInput(UtInput& aInput);

   std::unordered_map<std::string, P6DofVehicleType*>          mObjectTypeMap;
   std::unordered_map<std::string, P6DofThrustProducerObject*> mThrustProducerTypeMap;

   P6DofScenario* mScenario;

   std::unordered_map<std::string, P6DofIntegrator*> mIntegratorMap;

   const static std::string mDefaultIntegratorName;
};

#endif
