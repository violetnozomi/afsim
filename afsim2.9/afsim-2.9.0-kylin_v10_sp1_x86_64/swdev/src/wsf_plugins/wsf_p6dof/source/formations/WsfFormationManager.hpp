// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFFORMATIONMANAGER_HPP
#define WSFFORMATIONMANAGER_HPP

#include "wsf_p6dof_export.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "WsfFormation.hpp"
class WsfFormationOffset;
class WsfSimulation;
class WsfScenario;

//! Manage the set of defined formation types and the set of formation instances.
//! An instance of this object will be associated with each WsfScenario and each
//! WsfSimulation. In the former case, the manager is used to read in the initial
//! formations and the defined formation types from the input files. In the latter,
//! the object will be duplicated from this one and it will allow for creation and
//! destruction of formations during the simulation via script.
class WSF_P6DOF_EXPORT WsfFormationManager
{
public:
   WsfFormationManager(WsfScenario& aScenario, WsfSimulation* aSimulationPtr);
   WsfFormationManager(const WsfFormationManager& aOther, WsfSimulation* aSimulationPtr);
   WsfFormationManager(const WsfFormationManager& aOther) = delete;
   ~WsfFormationManager()                                 = default;

   //! Get the scenario for which this manager is managing formations.
   WsfScenario& GetScenario() const { return mScenario; }

   //! Get the simulation for which this manager is managing formation.
   //!
   //! This will only return a non-null pointer if this object was created
   //! for use in a simulation.
   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   bool                AddType(const std::string& aTypeName, std::unique_ptr<WsfFormation> aFormationTypePtr);
   bool                HasType(const std::string& aTypeName) const;
   const WsfFormation* GetType(const std::string& aTypeName) const;

   WsfFormation* CreateFormation(const std::string& aTypeName, const std::string& aFormationName);

   bool                       HasFormation(const std::string& aQualifiedName) const;
   WsfFormation*              GetFormation(const std::string& aQualifiedName) const;
   std::vector<WsfFormation*> GetTopLevelFormations() const;
   std::vector<WsfFormation*> GetAllFormations() const;
   bool                       DisbandFormation(const std::string& aQualifiedName);

   WsfFormation* DuplicateFormation(const WsfFormation* aPrototypePtr,
                                    WsfFormation*       aParentPtr,
                                    const std::string&  aFormationName);

   WsfFormation* GetFormationFromMember(const std::string& aMemberName) const;

   void UpdateQualifiedName(const std::string& aOldName, const std::string& aNewName);
   bool ValidateInitialFormations() const;
   void SetInitialMemberKinematics() const;
   void SetInitialMemberManeuvers() const;

private:
   void DuplicateFormationTypeTree(const WsfFormationManager& aOther,
                                   const WsfFormation*        aSourcePtr,
                                   WsfFormation*              aNewParentPtr);
   void DuplicateFormationInstanceTree(const WsfFormationManager& aOther,
                                       const WsfFormation*        aSourcePtr,
                                       WsfFormation*              aNewParentPtr);

   WsfScenario&   mScenario;
   WsfSimulation* mSimulationPtr;

   std::unordered_map<std::string, std::unique_ptr<WsfFormation>> mFormationTypes;
   std::unordered_map<std::string, std::unique_ptr<WsfFormation>> mFormations;
};

#endif // WSFFORMATIONMANAGER_HPP
