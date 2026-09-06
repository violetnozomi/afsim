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

#ifndef WSF_SIXDOF_FORMATIONMANAGER_HPP
#define WSF_SIXDOF_FORMATIONMANAGER_HPP

#include "wsf_six_dof_export.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "WsfSixDOF_Formation.hpp"

class WsfSimulation;
class WsfScenario;

namespace wsf
{
namespace six_dof
{
class FormationOffset;

//! Manage the set of defined formation types and the set of formation instances.
//! An instance of this object will be associated with each WsfScenario and each
//! WsfSimulation. In the former case, the manager is used to read in the initial
//! formations and the defined formation types from the input files. In the latter,
//! the object will be duplicated from this one and it will allow for creation and
//! destruction of formations during the simulation via script.
class WSF_SIX_DOF_EXPORT FormationManager
{
public:
   FormationManager(WsfScenario& aScenario, WsfSimulation* aSimulationPtr);
   FormationManager(const FormationManager& aOther, WsfSimulation* aSimulationPtr);
   FormationManager(const FormationManager& aOther) = delete;
   ~FormationManager()                              = default;

   //! Get the scenario for which this manager is managing formations.
   WsfScenario& GetScenario() const { return mScenario; }

   //! Get the simulation for which this manager is managing formation.
   //!
   //! This will only return a non-null pointer if this object was created
   //! for use in a simulation.
   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   bool             AddType(const std::string& aTypeName, std::unique_ptr<Formation> aFormationTypePtr);
   bool             HasType(const std::string& aTypeName) const;
   const Formation* GetType(const std::string& aTypeName) const;

   Formation* CreateFormation(const std::string& aTypeName, const std::string& aFormationName);

   bool                    HasFormation(const std::string& aQualifiedName) const;
   Formation*              GetFormation(const std::string& aQualifiedName) const;
   std::vector<Formation*> GetTopLevelFormations() const;
   std::vector<Formation*> GetAllFormations() const;
   bool                    DisbandFormation(const std::string& aQualifiedName);

   Formation* DuplicateFormation(const Formation* aPrototypePtr, Formation* aParentPtr, const std::string& aFormationName);

   Formation* GetFormationFromMember(const std::string& aMemberName) const;

   void UpdateQualifiedName(const std::string& aOldName, const std::string& aNewName);
   bool ValidateInitialFormations() const;
   void SetInitialMemberKinematics() const;
   void SetInitialMemberManeuvers() const;

private:
   void DuplicateFormationTypeTree(const FormationManager& aOther, const Formation* aSourcePtr, Formation* aNewParentPtr);

   void DuplicateFormationInstanceTree(const FormationManager& aOther, const Formation* aSourcePtr, Formation* aNewParentPtr);

   WsfScenario&   mScenario;
   WsfSimulation* mSimulationPtr;

   std::unordered_map<std::string, std::unique_ptr<Formation>> mFormationTypes;
   std::unordered_map<std::string, std::unique_ptr<Formation>> mFormations;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONMANAGER_HPP
