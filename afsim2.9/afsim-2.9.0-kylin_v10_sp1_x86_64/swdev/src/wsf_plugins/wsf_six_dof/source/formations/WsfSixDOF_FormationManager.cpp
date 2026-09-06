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

#include "WsfSixDOF_FormationManager.hpp"

#include <algorithm>
#include <sstream>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_FormUpKeepStationManeuver.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationUnit.hpp"
#include "WsfSixDOF_FormationUpdateStationKeeping.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::FormationManager::FormationManager(WsfScenario& aScenario, WsfSimulation* aSimulationPtr)
   : mScenario(aScenario)
   , mSimulationPtr{aSimulationPtr}
   , mFormationTypes{}
   , mFormations{}
{
}

wsf::six_dof::FormationManager::FormationManager(const FormationManager& aOther, WsfSimulation* aSimulationPtr)
   : mScenario(aOther.mScenario)
   , mSimulationPtr{aSimulationPtr}
   , mFormationTypes{}
   , mFormations{}
{
   for (const auto& i : aOther.mFormationTypes)
   {
      if (i.second->IsRoot())
      {
         DuplicateFormationTypeTree(aOther, i.second.get(), nullptr);
      }
   }

   for (const auto& i : aOther.mFormations)
   {
      if (i.second->IsRoot())
      {
         DuplicateFormationInstanceTree(aOther, i.second.get(), nullptr);
      }
   }
}

//! Add a formation type to the manager.
//!
//! This will register the given formation type with this manager. This manager can then be used to create
//! formations of any registered type using CreateFormation. The @p aTypeName must be unique among all the
//! formation types registered with this manager.
//!
//! \param aTypeName         - a string giving the name of the formation type.
//! \param aFormationTypePtr - an instance of the formation type used as a prototype for creating other formations of
//! the same type. \returns                 - true on a successful addition of the type; false otherwise.
bool wsf::six_dof::FormationManager::AddType(const std::string& aTypeName, std::unique_ptr<Formation> aFormationTypePtr)
{
   bool retval{false};
   if (!HasType(aTypeName))
   {
      mFormationTypes[aTypeName] = std::move(aFormationTypePtr);
      retval                     = true;
   }
   else
   {
      auto out = ut::log::warning() << "Cannot create formation type. Type already exists.";
      out.AddNote() << "Type: " << aTypeName;
   }
   return retval;
}

//! Return if this manager has a type registered with the given name.
bool wsf::six_dof::FormationManager::HasType(const std::string& aTypeName) const
{
   return mFormationTypes.find(aTypeName) != mFormationTypes.end();
}

//! Get the type registered under the given name.
//!
//! This will return the type registered under the name @p aTypeName, or nullptr
//! if there is no such type registered.
//!
//! \param aTypeName - the type to retrieve.
//! \returns         - the prototype of the requested type, or nullptr.
const wsf::six_dof::Formation* wsf::six_dof::FormationManager::GetType(const std::string& aTypeName) const
{
   const Formation* retval{nullptr};
   if (HasType(aTypeName))
   {
      retval = mFormationTypes.at(aTypeName).get();
   }
   return retval;
}

//! Create a new formation of the given type.
//!
//! This will create a new formation of type @p aTypeName with the relative name @p aFormationName.
//! The created formation will be managed by this object. Note that this will create a
//! formation without a parent, so it will have an identical qualified and relative name. The
//! @p aTypeName must be a type registered with this manager, and there must not be a formation with
//! a qualified name equal to @p aFormationName, and the requested @p aFormationName must not be
//! empty, otherwise this will return a null pointer.
//!
//! \param aTypeName      - The type of formation to create.
//! \param aFormationName - The relative name of the formation to create.
//! \returns              - The newly created formation, or nullptr on an error.
wsf::six_dof::Formation* wsf::six_dof::FormationManager::CreateFormation(const std::string& aTypeName,
                                                                         const std::string& aFormationName)
{
   Formation* retval{nullptr};
   if (!HasType(aTypeName))
   {
      auto out = ut::log::warning() << "Cannot create formation. Formation type does not exist.";
      out.AddNote() << "Type: " << aTypeName;
   }
   else if (aFormationName.empty())
   {
      auto out = ut::log::warning() << "Cannot create formation. Formations must have non-empty names.";
      out.AddNote() << "Type: " << aTypeName;
   }
   else if (HasFormation(aFormationName))
   {
      auto out = ut::log::warning() << "Cannot create formation. A formation with this name already exists.";
      out.AddNote() << "Type: " << aTypeName;
      out.AddNote() << "Formation: " << aFormationName;
   }
   else
   {
      mFormations[aFormationName] = GetType(aTypeName)->Duplicate(*this, nullptr, aFormationName, aFormationName);
      retval                      = mFormations[aFormationName].get();
   }
   return retval;
}

//! Return if this object is managing a formation with the given qualified name.
bool wsf::six_dof::FormationManager::HasFormation(const std::string& aQualifiedName) const
{
   return mFormations.find(aQualifiedName) != mFormations.end();
}

//! Return the formation with the given qualified name.
wsf::six_dof::Formation* wsf::six_dof::FormationManager::GetFormation(const std::string& aQualifiedName) const
{
   Formation* retval{nullptr};
   if (HasFormation(aQualifiedName))
   {
      retval = mFormations.at(aQualifiedName).get();
   }
   return retval;
}

//! Return all the top-level formations currently managed by this object.
std::vector<wsf::six_dof::Formation*> wsf::six_dof::FormationManager::GetTopLevelFormations() const
{
   std::vector<Formation*> retval{};
   for (auto& i : mFormations)
   {
      if (i.second->IsRoot())
      {
         retval.push_back(i.second.get());
      }
   }
   return retval;
}

//! Return all formations currently managed by this object.
std::vector<wsf::six_dof::Formation*> wsf::six_dof::FormationManager::GetAllFormations() const
{
   std::vector<Formation*> retval{};
   for (auto& i : mFormations)
   {
      retval.push_back(i.second.get());
   }
   return retval;
}

//! Disband the formation with the given qualified name.
//!
//! This will remove the formation with the qualified name @p aQualifiedName if it exists.
//! This will also delete the formation. If there was such a formation and it was removed
//! successfully this will return true, otherwise it will return false.
//!
//! \param aQualifiedName - the qualified name of the formation to disband.
//! \returns              - true if successful; false otherwise.
bool wsf::six_dof::FormationManager::DisbandFormation(const std::string& aQualifiedName)
{
   bool       retval{false};
   Formation* form = GetFormation(aQualifiedName);
   if (form && form->IsRoot())
   {
      auto namesToRemove = form->GetDescendantFormationNames();
      for (auto& name : namesToRemove)
      {
         mFormations[name]->SetAttached(false);
      }
      FormationUpdateStationKeeping::Update(form, GetSimulation()->GetSimTime(), true);
      for (auto& name : namesToRemove)
      {
         mFormations.erase(name);
      }
      retval = true;
   }
   return retval;
}

//! Duplicate the given formation and claim management of the new formation.
//!
//! This will make a duplicate of the formation @p aPrototypePtr, assign the
//! new formation the relative name @p aFormationName, and give the new formation
//! the parent @p aParentPtr. This object will then take over management of that
//! formation.
//!
//! \param aPrototypePtr  - The formation to duplicate; need not be managed by this object.
//! \param aParentPtr     - A formation to set as the new formation's parent.
//! \param aFormationName - The relative name of the new formation, must not be empty.
//! \returns              - the formation; nullptr on error.
wsf::six_dof::Formation* wsf::six_dof::FormationManager::DuplicateFormation(const Formation*   aPrototypePtr,
                                                                            Formation*         aParentPtr,
                                                                            const std::string& aFormationName)
{
   Formation* retval{nullptr};
   if (aFormationName.empty())
   {
      ut::log::warning() << "Cannot duplicate formation. Formations must have non-empty names.";
   }
   else
   {
      std::string qualifiedName = aParentPtr->GetChildQualifiedName(aFormationName);
      if (!HasFormation(qualifiedName))
      {
         mFormations[qualifiedName] = aPrototypePtr->Duplicate(*this, aParentPtr, qualifiedName, aFormationName);
         retval                     = mFormations[qualifiedName].get();
      }
      else
      {
         auto out = ut::log::warning() << "Cannot duplicate formation. A formation with this name already exists.";
         out.AddNote() << "Formation: " << qualifiedName;
      }
   }
   return retval;
}

//! Return the formation to which the given platform is assigned.
//!
//! This will return the formation in which the platform with the given @p aMemberName is a
//! member. If there is no such platform, this will return a null pointer.
//!
//! \param aMemberName - the name of the platform of interest.
//! \returns           - the formation to which it belongs, or nullptr if it is not a member of a formation.
wsf::six_dof::Formation* wsf::six_dof::FormationManager::GetFormationFromMember(const std::string& aMemberName) const
{
   Formation* retval{nullptr};
   for (auto& i : mFormations)
   {
      if (i.second->IsUnitFormation() && (aMemberName == i.second->GetMemberPlatformName()))
      {
         retval = i.second.get();
         break;
      }
   }
   return retval;
}

//! Update the key of an existing formation.
//!
//! This will rekey the formation at @p aOldName with the new key @p aNewName. It is the caller's
//! responsibility to reset the qualified name on the formation.
void wsf::six_dof::FormationManager::UpdateQualifiedName(const std::string& aOldName, const std::string& aNewName)
{
   auto i = mFormations.find(aOldName);
   if (i == mFormations.end())
   {
      std::stringstream ss{};
      ss << "Unable to find formation '" << aOldName << "'.";
      throw std::runtime_error(ss.str().c_str());
   }

   auto j = mFormations.find(aNewName);
   if (j != mFormations.end())
   {
      std::stringstream ss{};
      ss << "There is already a formation with the name '" << aNewName << "'.";
      throw std::runtime_error(ss.str().c_str());
   }

   mFormations[aNewName] = std::move(i->second);
   if (1 != mFormations.erase(aOldName))
   {
      throw std::runtime_error("Somehow a formation no longer exists that should exist.");
   }
}

//! Perform initial validation checks on the defined formations.
//!
//! This will check that member platforms in a formation actually exist in the input,
//! and that each non-leaf formation has a lead sub-formation set.
//!
//! \returns - true if all checks pass; false otherwise.
bool wsf::six_dof::FormationManager::ValidateInitialFormations() const
{
   bool retval{true};

   std::vector<std::string> assignedPlatforms{};

   for (auto& i : mFormations)
   {
      if (i.second->IsUnitFormation())
      {
         WsfPlatform* platPtr = i.second->GetMemberPlatform();
         if (platPtr)
         {
            Mover* mvrPtr = dynamic_cast<Mover*>(platPtr->GetMover());
            if (!mvrPtr)
            {
               auto out = ut::log::error() << "Formation member platform must have a Mover.";
               out.AddNote() << "Formation: " << i.second->GetQualifiedName();
               out.AddNote() << "Platform: " << i.second->GetMemberPlatformName();
               retval = false;
            }

            if (assignedPlatforms.end() !=
                std::find(assignedPlatforms.begin(), assignedPlatforms.end(), platPtr->GetName()))
            {
               auto out = ut::log::error() << "A platform may only be assigned to one formation.";
               out.AddNote() << "Platform: " << platPtr->GetName();
               retval = false;
            }
            else
            {
               assignedPlatforms.push_back(platPtr->GetName());
            }
         }
         else
         {
            auto out = ut::log::error() << "Cannot find formation member platform.";
            out.AddNote() << "Formation: " << i.second->GetQualifiedName();
            out.AddNote() << "Platform: " << i.second->GetMemberPlatform();
            retval = false;
         }
      }
      else if (!i.second->IsLeaf() && !i.second->HasLeader())
      {
         auto out = ut::log::error() << "Formation does not have a lead sub-formation defined.";
         out.AddNote() << "Formation: " << i.second->GetQualifiedName();
         retval = false;
      }
   }
   return retval;
}

//! This will set the initial member kinematics of input formations.
//!
//! This will bring the member platforms in an input formation into the correct
//! kinematic state so that the platforms begin in formation.
void wsf::six_dof::FormationManager::SetInitialMemberKinematics() const
{
   for (auto& i : mFormations)
   {
      if (i.second->IsLeaf())
      {
         Formation* attachedRootPtr = i.second->GetAttachedRoot();
         if (attachedRootPtr != i.second.get())
         {
            const Formation* leadPtr = attachedRootPtr->GetFormationLeader();
            if (leadPtr != i.second.get())
            {
               RelativeManeuverKinematics initState = leadPtr->GetMemberInitialKinematicState();
               UtVec3d                    omegaWCS  = initState.GetTurnCircle().GetAngularVelocity();
               FormationOffset            offset    = i.second->GetOffsetFrom(*attachedRootPtr);
               RelativeManeuverKinematics state     = Formation::ComputeOffsetKinematics(initState, offset, omegaWCS);
               i.second->SetMemberInitialKinematicState(state);
            }
         }
      }
   }
}

//! This will set the initial station keeping maneuvers on input formations.
//!
//! This will execute station keeping maneuvers on all members of input
//! formations.
void wsf::six_dof::FormationManager::SetInitialMemberManeuvers() const
{
   for (auto& i : mFormations)
   {
      if (i.second->IsLeaf())
      {
         Formation* attachedRootPtr = i.second->GetAttachedRoot();
         if (attachedRootPtr != i.second.get())
         {
            const Formation* leadPtr = attachedRootPtr->GetFormationLeader();
            if (leadPtr != i.second.get())
            {
               Mover* moverPtr = i.second->GetMemberMover();
               auto   mvrPtr   = ut::make_unique<FormUpKeepStationManeuver>();
               mvrPtr->SetFormation(i.second.get()->GetQualifiedName());
               moverPtr->ExecuteManeuver(0.0, std::move(mvrPtr));
            }
         }
      }
   }
}

//! Duplicate a formation type, handing over management of the duplicate.
void wsf::six_dof::FormationManager::DuplicateFormationTypeTree(const FormationManager& aOther,
                                                                const Formation*        aSourcePtr,
                                                                Formation*              aNewParentPtr)
{
   mFormationTypes[aSourcePtr->GetQualifiedName()] =
      aSourcePtr->Duplicate(*this, aNewParentPtr, aSourcePtr->GetQualifiedName(), aSourcePtr->GetRelativeName());
}

//! Duplicate a formation instance, handing over management of the duplicate.
void wsf::six_dof::FormationManager::DuplicateFormationInstanceTree(const FormationManager& aOther,
                                                                    const Formation*        aSourcePtr,
                                                                    Formation*              aNewParentPtr)
{
   mFormations[aSourcePtr->GetQualifiedName()] =
      aSourcePtr->Duplicate(*this, aNewParentPtr, aSourcePtr->GetQualifiedName(), aSourcePtr->GetRelativeName());
}
