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

#include "WsfSixDOF_Formation.hpp"

#include <cassert>
#include <numeric>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_FormationCommand.hpp"
#include "WsfSixDOF_FormationManager.hpp"
#include "WsfSixDOF_FormationOffset.hpp"
#include "WsfSixDOF_FormationUpdateStationKeeping.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::Formation::Formation(FormationManager& aManager, const std::string& aName)
   : UtScriptAccessible()
   , mQualifiedName{aName}
   , mName{aName}
   , mParentPtr{nullptr}
   , mOffset{}
   , mChildren{}
   , mLeadChild{}
   , mCurrCommandPtr{nullptr}
   , mManager(aManager)
   , mDebug{false}
   , mAttached{true}
{
}

wsf::six_dof::Formation::Formation(const Formation&   aOther,
                                   FormationManager&  aManager,
                                   Formation*         aParentPtr,
                                   const std::string& aQualifiedName,
                                   const std::string& aName)
   : UtScriptAccessible()
   , mQualifiedName{aQualifiedName}
   , mName{aName}
   , mParentPtr{aParentPtr}
   , mOffset{aOther.mOffset}
   , mChildren{}
   , mLeadChild{aOther.mLeadChild}
   , mCurrCommandPtr{nullptr}
   , mManager(aManager)
   , mDebug{aOther.mDebug}
   , mAttached{aOther.mAttached}
{
   for (auto& i : aOther.mChildren)
   {
      Formation* childPtr                    = mManager.DuplicateFormation(i.second, this, i.first);
      mChildren[childPtr->GetRelativeName()] = childPtr;
   }
}

wsf::six_dof::Formation::~Formation()
{
   if (mCurrCommandPtr && mCurrCommandPtr->IsRunning())
   {
      mCurrCommandPtr->Drop(*GetManager().GetSimulation());
      mCurrCommandPtr.release();
   }
}

//! Duplicate this formation.
//!
//! This will create a clone of this formation setting the @p aManager and @p aParentPtr
//! of the formation to the provided values. The formation is returned and should be
//! immediately claimed by a WsfSixDOF_FormationManager; this method should not be called
//! directly, instead WsfSixDOF_FormationManager::Duplicate should be used so that a formation
//! manager can immediately take ownership of the new formation.
//!
//! @param aManager       - The manager that will own this formation.
//! @param aParentPtr     - The parent formation of the newly created formation, may be nullptr.
//! @param aQualifiedName - The fully qualified name of this formation.
//! @param aName          - The relative name of this formation.
//! @returns              - The newly created formation.
std::unique_ptr<wsf::six_dof::Formation> wsf::six_dof::Formation::Duplicate(FormationManager&  aManager,
                                                                            Formation*         aParentPtr,
                                                                            const std::string& aQualifiedName,
                                                                            const std::string& aName) const
{
   return std::unique_ptr<Formation>(new Formation(*this, aManager, aParentPtr, aQualifiedName, aName));
}

bool wsf::six_dof::Formation::ProcessInput(UtInput& aInput)
{
   bool        myCommand{false};
   bool        isLead{false};
   std::string command = aInput.GetCommand();

   if (command == "debug")
   {
      SetDebug(true);
      myCommand = true;
   }
   else if (command == "offset")
   {
      FormationOffset offset{};
      myCommand = offset.ProcessInput(aInput);
      SetOffset(offset);
   }
   else if (command == "detached")
   {
      myCommand = true;
      SetAttached(false);
   }
   else if (command == "lead")
   {
      if (GetLeader().empty())
      {
         isLead = true;
         aInput.ReadCommand(command);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Can only set one child formation as lead.");
      }
   }

   if (mManager.HasType(command))
   {
      UtInputBlock inputBlock(aInput);
      myCommand = true;

      // Read in the formation's relative name
      std::string formName{};
      if (!inputBlock.ReadCommand())
      {
         throw UtInput::BadValue(aInput, "formations must be given a relative name.");
      }
      formName             = aInput.GetCommand();
      Formation* inputForm = mManager.CreateFormation(command, formName);
      if (!inputForm)
      {
         throw UtInput::BadValue(aInput, "Error creating formation.");
      }

      // Read in the formation
      while (inputBlock.ReadCommand())
      {
         if (!inputForm->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (isLead && !inputForm->GetOffset().IsZero())
      {
         auto out = ut::log::warning() << "Lead formation has non-zero offset in input.";
         out.AddNote() << "This offset will be ignored. (Set to zero.)";
         out.AddNote() << "Formation: " << inputForm->GetQualifiedName();
         inputForm->SetOffset(FormationOffset{});
      }

      if (!AddChildFormation(inputForm))
      {
         throw std::runtime_error("Error adding child formation during ProcessInput");
      }

      if (isLead)
      {
         SetLead(inputForm->GetRelativeName());
      }
   }
   else if (!myCommand)
   {
      if (isLead)
      {
         throw UtInput::BadValue(aInput, "'lead' must proceed a valid formation type.");
      }
   }

   return myCommand;
}

//! Return the number of child formations of this formation.
int wsf::six_dof::Formation::GetNumChildFormations() const
{
   return static_cast<int>(mChildren.size());
}

//! Get a child formation given its name.
//!
//! @param aName - the (unqualified) name of the child formation.
//! @returns     - the formation, or nullptr if there is no such child.
wsf::six_dof::Formation* wsf::six_dof::Formation::GetChildFormation(const std::string& aName) const
{
   Formation* retval{nullptr};
   auto       i = mChildren.find(aName);
   if (i != mChildren.end())
   {
      retval = i->second;
   }
   return retval;
}

//! Get the names of all formations descendant from this formation.
std::vector<std::string> wsf::six_dof::Formation::GetDescendantFormationNames() const
{
   std::vector<std::string> retval{};
   AddDescendantNames(retval);
   return retval;
}

//! Return the number of member platforms in this formation.
int wsf::six_dof::Formation::GetNumMembers() const
{
   int retval{0};
   for (auto& i : mChildren)
   {
      retval += i.second->GetNumMembers();
   }
   return retval;
}

//! Get the kinematic state of the member of this formation.
//!
//! If this formation has no member, this will throw std::runtime_error.
//!
//! @returns - the kinematic state of the member of this formation.
wsf::six_dof::RelativeManeuverKinematics wsf::six_dof::Formation::GetMemberKinematicState() const
{
   RelativeManeuverKinematics retval{};
   Mover*                     movPtr = GetMemberMover();
   if (movPtr)
   {
      WsfPlatform* memberPtr = movPtr->GetPlatform();

      // Get the data held by the platform
      memberPtr->GetLocationWCS(retval.mLocWCS.GetData());
      memberPtr->GetVelocityWCS(retval.mVelWCS.GetData());
      memberPtr->GetAccelerationWCS(retval.mAccWCS.GetData());
      double yaw, pitch, roll;
      memberPtr->GetOrientationNED(yaw, pitch, roll);
      retval.mAnglesNED.Set(yaw, pitch, roll);

      // Get the data held by the mover
      retval.mG_Load = movPtr->GetGLoad();
   }
   else
   {
      throw std::runtime_error("Unable to get kinematic state of a formation member.");
   }
   return retval;
}

wsf::six_dof::RelativeManeuverKinematics wsf::six_dof::Formation::GetMemberInitialKinematicState() const
{
   RelativeManeuverKinematics retval{};
   Mover*                     movPtr = GetMemberMover();
   if (movPtr)
   {
      // Note that this assumes that the craft is not experiencing any angular velocity, acceleration
      // or non-standard g-load.
      movPtr->GetInitialState(retval.mLocWCS.GetData(), retval.mVelWCS.GetData(), retval.mAnglesNED.GetData());
   }
   else
   {
      throw std::runtime_error("Unable to get kinematic state of a formation member.");
   }
   return retval;
}

//! Set the initial kinematic state of the member of this formation.
//!
//! This is used during initialization to bring the members of input formations into
//! their proper position in the formation. If this formation has no member this will
//! throw std::runtime_error.
//!
//! \param aState - the initial state to set for the member of this formation.
void wsf::six_dof::Formation::SetMemberInitialKinematicState(RelativeManeuverKinematics& aState) const
{
   Mover* mover = GetMemberMover();
   if (mover)
   {
      WsfGeoPoint pt(aState.mLocWCS.GetData());
      mover->SetInitialLatLon(pt.GetLat(), pt.GetLon());
      mover->SetInitialAlt(pt.GetAlt());
      mover->SetInitialNEDHeading(aState.mAnglesNED[0]);
      mover->SetInitialNEDPitch(aState.mAnglesNED[1]);
      mover->SetInitialNEDRoll(aState.mAnglesNED[2]);

      UtEntity temp{};
      temp.SetLocationWCS(aState.mLocWCS.GetData());
      temp.SetVelocityWCS(aState.mVelWCS.GetData());
      temp.SetOrientationNED(aState.mAnglesNED[0], aState.mAnglesNED[1], aState.mAnglesNED[2]);
      double velNED[3];
      temp.GetVelocityNED(velNED);
      mover->SetInitialNEDVelocity(velNED[0], velNED[1], velNED[2]);
   }
   else
   {
      throw std::runtime_error("Unable to set initial kinematic state on a non-existent formation member.");
   }
}

//! Return the overall lead sub-formation of this formation.
//!
//! By contrast to GetLeader, which returns the lead sub-formation among the children
//! of this formation, this method will return the lead formation among all the
//! descendants of this formation.
//!
//! \returns - the overall lead formation of this formation.
wsf::six_dof::Formation* wsf::six_dof::Formation::GetFormationLeader()
{
   Formation* retvalPtr{nullptr};
   if (IsLeaf())
   {
      retvalPtr = this;
   }
   else
   {
      Formation* leadForm = GetLeadSubFormation();
      if (leadForm)
      {
         retvalPtr = leadForm->GetFormationLeader();
      }
   }
   return retvalPtr;
}

//! Get the root of the attached subtree of which this formation is a part.
//!
//! This will return the first ancestor of this formation that is not attached
//! to its parent, or it will return this formation if this formation is the
//! overall root.
//!
//! \returns - the furthest ancestor of this formation to which it is attached.
wsf::six_dof::Formation* wsf::six_dof::Formation::GetAttachedRoot()
{
   Formation* retvalPtr{this};
   while (retvalPtr->IsAttached())
   {
      Formation* next = retvalPtr->GetParentFormation();
      if (next)
      {
         retvalPtr = next;
      }
      else
      {
         break;
      }
   }
   return retvalPtr;
}

wsf::six_dof::Formation* wsf::six_dof::Formation::GetKeepStationRoot()
{
   Formation* retval{nullptr};

   if (IsLeader())
   {
      // First go all the way the chain of leaders to first non-leader formation
      Formation* grandparentPtr = this;
      while (grandparentPtr->IsLeader() && grandparentPtr->IsAttached())
      {
         Formation* nextPtr = grandparentPtr->GetParentFormation();
         if (nextPtr == nullptr)
         {
            break;
         }
         grandparentPtr = nextPtr;
      }

      // then go one more if it is attached
      if (grandparentPtr && grandparentPtr->IsAttached())
      {
         Formation* nextPtr = grandparentPtr->GetParentFormation();
         if (nextPtr != nullptr)
         {
            grandparentPtr = nextPtr;
         }
      }
      retval = grandparentPtr;
   }
   else
   {
      if (IsAttached())
      {
         Formation* parentPtr = GetParentFormation();
         if (parentPtr != nullptr)
         {
            retval = parentPtr;
         }
      }
      else
      {
         retval = this;
      }
   }

   if (retval == nullptr)
   {
      retval = this;
   }

   return retval;
}

//! Return if this formation is the leader of its parent formation.
bool wsf::six_dof::Formation::IsLeader() const
{
   return mParentPtr && (mParentPtr->GetLeadSubFormation() == this);
}

//! Return what would be the qualified name of a child of this formation with the given relative name.
//!
//! This will return the qualified name of a child of this formation if @p aChildName was the
//! relative name of that child.
//!
//! \param aChildName - the relative name of the child.
//! \returns          - the qualified name of such a child of this formation.
std::string wsf::six_dof::Formation::GetChildQualifiedName(const std::string& aChildName) const
{
   return mQualifiedName + "." + aChildName;
}

//! Return if this formation has the given as an ancestor.
bool wsf::six_dof::Formation::HasAncestor(const Formation* aFormationPtr) const
{
   int              counter{0};
   const Formation* iter = this;
   while (counter < 10 && iter && iter != aFormationPtr)
   {
      iter = iter->GetParentFormation();
      ++counter;
   }
   return iter == aFormationPtr;
}

//! Get the total offset from the given formation to this formation.
//!
//! This can be used to get the total offset of this formation relative to
//! the given @p aFormation during initialization (when there is zero roll).
//! If this method is used outside the context of initialization, and the
//! platforms have non-zero roll, this could give incorrect results when
//! the offsets through the chain of ancestors are not all welded, or not
//! not all fixed-altitude.
//!
//! \param aFormation - the formation relative to which to compute the offset.
//! \returns          - the resulting offset.
wsf::six_dof::FormationOffset wsf::six_dof::Formation::GetOffsetFrom(Formation& aFormation) const
{
   FormationOffset retval{};
   if (HasAncestor(&aFormation))
   {
      const Formation* currPtr = this;
      while (currPtr != &aFormation)
      {
         retval += currPtr->GetOffset();
         currPtr = currPtr->GetParentFormation();
      }
   }
   else
   {
      throw std::runtime_error("unable to get offset from a formation that is not an ancestor of this formation.");
   }
   return retval;
}

//! Get the offset kinematics for this formation from the given formation.
//!
//! This will compute the offset kinematics for this formation relative to the given
//! @p aFormationPtr. This method will throw a std::runtime_error if the given formation
//! is not an ancestor of this formation.
//!
//! \param aFormationPtr - the formation relative to which the offset kinematics should be computed.
//! \returns             - the offset kinematics.
wsf::six_dof::RelativeManeuverKinematics wsf::six_dof::Formation::GetOffsetKinematicsFrom(Formation* aFormationPtr) const
{
   RelativeManeuverKinematics retval{};
   if (HasAncestor(aFormationPtr))
   {
      // walk up to ancestor saving off the offset along the way.
      std::vector<FormationOffset> offsets{};
      const Formation*             currPtr = this;
      while (currPtr != aFormationPtr)
      {
         offsets.push_back(currPtr->GetOffset());
         currPtr = currPtr->GetParentFormation();
      }

      // then add those offsets to the return value in the opposite order they were discovered.
      Formation* leaderPtr = aFormationPtr->GetFormationLeader();
      auto       state     = leaderPtr->GetMemberKinematicState();
      UtVec3d    omegaWCS  = state.GetTurnCircle().GetAngularVelocity();
      auto       func      = [&omegaWCS](const RelativeManeuverKinematics& aState, const FormationOffset& aOffset)
      { return Formation::ComputeOffsetKinematics(aState, aOffset, omegaWCS); };
      retval = std::accumulate(offsets.rbegin(), offsets.rend(), state, func);
   }
   else
   {
      throw std::runtime_error(
         "unable to get offset kinematics from a formation that is not an ancestor of this formation.");
   }
   return retval;
}

//! Get the formation manager that owns this formation.
wsf::six_dof::FormationManager& wsf::six_dof::Formation::GetManager() const
{
   return mManager;
}

//! Get the scenario in which the formation is a part.
WsfScenario& wsf::six_dof::Formation::GetScenario() const
{
   return mManager.GetScenario();
}

//! Set the parent formation.
//!
//! This will reset the parent of this formation. This will fail if this formation has the
//! @p aParentPtr as a child.
//!
//! \param aParentPtr - the new parent.
//! \param aRename    - true if the qualified names of this formation and its children should be updated.
bool wsf::six_dof::Formation::SetParentFormation(Formation* aParentPtr, bool aRename)
{
   bool retval{false};
   if (aParentPtr && aParentPtr->HasAncestor(this))
   {
      // Formations are trees.
      auto out = ut::log::warning() << "Cannot set parent formation. Parent would have child as an ancestor.";
      out.AddNote() << "Parent: " << aParentPtr->GetQualifiedName();
      out.AddNote() << "Child: " << this->GetQualifiedName();
   }
   else
   {
      mParentPtr = aParentPtr;
      if (aRename)
      {
         UpdateQualifiedName();
      }
      WsfSimulation* simPtr = GetManager().GetSimulation();
      if (simPtr != nullptr)
      {
         FormationUpdateStationKeeping::Update(this, simPtr->GetSimTime());
      }
      retval = true;
   }
   return retval;
}

//! Set (change) the relative name of the formation.
//!
//! This will change the relative name of this formation, and then update all the
//! qualified names of descendants of this formation. If this renaming cannot occur
//! without producing a name conflict in the set of fully qualified names, then
//! this will do nothing and return false.
//!
//! \param aRelativeName - the new relative name for this formation. Must not be empty.
//! \returns             - true if renaming is successful; false if the change would
//!                        produce a conflict.
bool wsf::six_dof::Formation::SetRelativeName(const std::string& aRelativeName)
{
   if (aRelativeName.empty())
   {
      return false;
   }

   bool       retval{false};
   Formation* parentPtr = GetParentFormation();
   if (parentPtr)
   {
      bool isLeader = IsLeader();
      if (parentPtr->UpdateChildRelativeName(GetRelativeName(), aRelativeName))
      {
         if (isLeader)
         {
            parentPtr->SetLead(aRelativeName);
         }
         retval = true;
      }
   }
   else
   {
      if (GetManager().GetFormation(aRelativeName))
      {
         auto out =
            ut::log::warning() << "Cannot change name of formation. There is already a formation with the new name.";
         out.AddNote() << "Old Name: " << GetQualifiedName();
         out.AddNote() << "New Name: " << aRelativeName;
      }
      else
      {
         mName = aRelativeName;
         UpdateQualifiedName();
         retval = true;
      }
   }

   if (retval)
   {
      WsfSimulation* simPtr = GetManager().GetSimulation();
      if (simPtr != nullptr)
      {
         FormationUpdateStationKeeping::Update(this, simPtr->GetSimTime());
      }
   }

   return retval;
}

//! Add the given formation as a child with the given name.
//!
//! Add the formation @p aChildPtr to this formation as a child. This method is allowed
//! to fail, which supports subclasses with more restrictive membership criteria. If the
//! formation does not currently have any children, this will set the given formation
//! as the leader.
//!
//! \param aChildPtr - The formation to add as a child.
//! \returns         - true if successfully added; false otherwise.
bool wsf::six_dof::Formation::AddChildFormation(Formation* aChildPtr)
{
   bool success{false};
   if (aChildPtr)
   {
      if (aChildPtr->GetParentFormation())
      {
         auto out = ut::log::warning() << "Cannot add child formation. Child already has a parent.";
         out.AddNote() << "Parent: " << GetQualifiedName();
         out.AddNote() << "Child: " << aChildPtr->GetQualifiedName();
      }
      else if (aChildPtr == this)
      {
         auto out = ut::log::warning() << "Cannot add formation as a child of itself.";
         out.AddNote() << "Formation: " << GetQualifiedName();
      }
      else if (mChildren.find(aChildPtr->GetRelativeName()) == mChildren.end())
      {
         success = aChildPtr->SetParentFormation(this);
         if (success)
         {
            mChildren[aChildPtr->GetRelativeName()] = aChildPtr;

            if (mChildren.size() == 1)
            {
               SetLead(aChildPtr->GetRelativeName());
            }
            // No need to redo station keeping update, it would have been handled in SetParentFormation.
         }
      }
      else
      {
         auto out =
            ut::log::warning() << "Cannot add child to formation. Parent already has a child with the same name.";
         out.AddNote() << "Parent: " << GetQualifiedName();
         out.AddNote() << "Child: " << aChildPtr->GetRelativeName();
      }
   }
   return success;
}

//! Remove a child formation.
//!
//! This will remove a direct child of this formation. Note that this does not destroy the
//! formation, this merely removes it as a child of this formation. This operation will not
//! succeed if the given @p aName indicates that the lead subformation should be removed,
//! unless the formation is being deleted.
//!
//! \param aName   - the name of the child formation to remove.
//! \param aRename - true if this should reset the parent. Set to false if this removal is right
//!                  before the formation is deleted.
//! \returns       - the formation removed, or nullptr.
wsf::six_dof::Formation* wsf::six_dof::Formation::RemoveChildFormation(const std::string& aName, bool aRename)
{
   Formation* retval{nullptr};

   if (GetManager().GetFormation(aName))
   {
      auto out =
         ut::log::warning() << "Cannot remove child formation. There is already a formation with the same name.";
      out.AddNote() << "Parent: " << GetQualifiedName();
      out.AddNote() << "Child: " << aName;
   }
   else if (GetLeader() == aName && aRename)
   {
      auto out = ut::log::warning() << "Cannot remove child formation. Child is the lead formation.";
      out.AddNote() << "Parent: " << GetQualifiedName();
      out.AddNote() << "Child: " << aName;
      out.AddNote() << "If you must remove it, change the formation's lead sub-formation first.";
   }
   else
   {
      auto i = mChildren.find(aName);
      if (i != mChildren.end())
      {
         retval = i->second;
         if (aRename)
         {
            retval->SetParentFormation(nullptr);
         }
         retval->SetAttached(false);
         mChildren.erase(i);

         if (mLeadChild == aName)
         {
            mLeadChild.clear();
         }
      }
      else
      {
         auto out = ut::log::warning() << "Cannot remove child formation. The child could not be found.";
         out.AddNote() << "Parent: " << GetQualifiedName();
         out.AddNote() << "Child: " << aName;
      }
   }
   return retval;
}

//! Set the lead formation.
//!
//! This will set the child with the given unqualified @p aName to be the formation leader.
//! This will also update the offsets of all children to take into account the
//! new leadership, and it will modify the offset of this formation relative to its
//! parent so keep the member platforms in the same location. If there is no child with
//! the given @p aName, this method does nothing and will return false.
//!
//! \param aName - the name of the child formation to set as Lead.
//! \returns     - true if successful; false otherwise.
bool wsf::six_dof::Formation::SetLead(const std::string& aName)
{
   bool       retval{false};
   Formation* childPtr = GetChildFormation(aName);
   if (childPtr)
   {
      mLeadChild = aName;

      FormationOffset delta = childPtr->GetOffset();

      // Update all offsets of children by subtracting the saved offset.
      for (auto& i : mChildren)
      {
         FormationOffset updatedOffset = i.second->GetOffset() -= delta;
         i.second->SetOffset(updatedOffset);
      }

      // Update offset of this formation relative to parent by the saved offset.
      mOffset += delta;
      WsfSimulation* simPtr = GetManager().GetSimulation();
      if (simPtr != nullptr)
      {
         // if there is a simulation, we are not initializing, so we will zero out the
         // overall offset.
         if (IsRoot())
         {
            mOffset = FormationOffset();
         }

         FormationUpdateStationKeeping::Update(this, simPtr->GetSimTime());
      }

      retval = true;
   }
   else
   {
      auto out = ut::log::warning() << "Cannot set new lead formation. The child could not be found.";
      out.AddNote() << "Parent: " << GetQualifiedName();
      out.AddNote() << "Child: " << aName;
   }
   return retval;
}

//! Set the offset for this formation relative to its parent.
void wsf::six_dof::Formation::SetOffset(const FormationOffset& aOffset)
{
   mOffset = aOffset;
}

//! Set the member platform name for this formation.
//!
//! In this implementation, this always fails, as only a unit formation can
//! set a member. Override this in derived classes to allow for members to
//! be set on a formation.
//!
//! \param aPlatformName - the name of the platform to set as member.
//! \returs              - true if successful; false otherwise.
bool wsf::six_dof::Formation::SetMemberName(const std::string& aPlatformName)
{
   bool       retval{false};
   Formation* formPtr = GetManager().GetFormationFromMember(aPlatformName);
   if (formPtr != nullptr)
   {
      auto out = ut::log::warning()
                 << "Cannot set formation member. The platform has already been assigned to another formation.";
      out.AddNote() << "Formation: " << GetQualifiedName();
      out.AddNote() << "Platform: " << aPlatformName;
      out.AddNote() << "Assigned Formation: " << formPtr->GetQualifiedName();
   }
   else
   {
      retval = SetMemberNameP(aPlatformName);
   }
   return retval;
}

//! Return the current command for this formation.
wsf::six_dof::FormationCommand* wsf::six_dof::Formation::GetCurrentCommand() const
{
   return mCurrCommandPtr.get();
}

//! Execute the given command on this formation.
//!
//! The given @p aCommandPtr will be executed on this formation. Note that this does
//! not necessarily mean that this formation will actually receive a new command.
//! Some commands will pass through to children and not require anything of the
//! parent formation.
//!
//! Note that this will return false if the command is not assigned to this formation,
//! even if that command is ultimately assigned to a child formation.
//!
//! \param aCommandPtr - the command to execute on this formation.
//! \param aSimTime    - the time at which to assign the command.
//! \returns           - true if the command was assigned to this formation; false otherwise.
bool wsf::six_dof::Formation::ExecuteCommand(std::unique_ptr<FormationCommand> aCommandPtr, double aSimTime)
{
   bool retval{false};
   if (aCommandPtr->Process(this, aSimTime))
   {
      if (mCurrCommandPtr && (mCurrCommandPtr->IsRunning()))
      {
         // The current command should have an outstanding event, so we cancel the
         // command and release the smart pointer's hold on the command so the next
         // update of that command will delete it.
         mCurrCommandPtr->Drop(*GetManager().GetSimulation());
         mCurrCommandPtr.release();
      }

      mCurrCommandPtr = std::move(aCommandPtr);
      mCurrCommandPtr->Assign(this, *GetManager().GetSimulation(), aSimTime);
      retval = true;
   }
   return retval;
}

//! Compute the kinematic state implied by an offset.
//!
//! This will compute the target kinematic state relative to a given set of @p aKinematics
//! for a given @p aOffset. The position and velocity of the return value will be translated
//! as appropriate based on the input kinematics. This does not modify the acceleration of
//! the input @p aKinematics.
//!
//! \param aKinematics - The kinematic state to compute the offset in position and velocity for.
//! \param aOffset     - The formation offset to use when computing the new kinematics.
//! \return            - The offset kinematics.
wsf::six_dof::RelativeManeuverKinematics
wsf::six_dof::Formation::ComputeOffsetKinematics(const RelativeManeuverKinematics& aKinematics,
                                                 const FormationOffset&            aOffset,
                                                 const UtVec3d&                    aOmega)
{
   static constexpr double cWELDED_MAX_RANGE = 100.0; // m

   RelativeManeuverKinematics retval{aKinematics};
   bool                       useWelded = aOffset.IsWelded() && aOffset.GetRange() < cWELDED_MAX_RANGE;

   // Set up a UtEntity to use the existing coordinate transformation utilities it provides.
   UtEntity utilEntity{};
   utilEntity.SetLocationWCS(aKinematics.mLocWCS.GetData());
   utilEntity.SetVelocityWCS(aKinematics.mVelWCS.GetData());
   if (useWelded)
   {
      utilEntity.SetOrientationNED(aKinematics.mAnglesNED[0], aKinematics.mAnglesNED[1], aKinematics.mAnglesNED[2]);
   }
   else
   {
      utilEntity.SetOrientationNED(aKinematics.mAnglesNED[0], 0.0, 0.0);
   }

   // Get the offset vector in WCS
   double deltaLocWCS[3];
   double offsetECS[3] = {aOffset.GetAhead(), aOffset.GetRight(), -aOffset.GetStack()};
   utilEntity.ConvertECSVectorToWCS(deltaLocWCS, offsetECS);

   // Add the delta to the position
   UtVec3d::Add(retval.mLocWCS.GetData(), aKinematics.mLocWCS.GetData(), deltaLocWCS);

   // Use omega vector and r vector to get velocity vector
   double deltaVelWCS[3];
   UtVec3d::CrossProduct(deltaVelWCS, aOmega.GetData(), deltaLocWCS);

   // Add the delta to the velocity
   UtVec3d::Add(retval.mVelWCS.GetData(), aKinematics.mVelWCS.GetData(), deltaVelWCS);

   return retval;
}

//! Update the qualified name of this formation from the parent.
//! This will also update all children of this formation, as well as informing the formation
//! manager that this formation is changing names.
void wsf::six_dof::Formation::UpdateQualifiedName()
{
   std::string updatedQualifiedName{};
   if (mParentPtr)
   {
      updatedQualifiedName = mParentPtr->GetChildQualifiedName(mName);
   }
   else
   {
      updatedQualifiedName = mName;
   }

   mManager.UpdateQualifiedName(mQualifiedName, updatedQualifiedName);
   mQualifiedName = updatedQualifiedName;

   for (auto& i : mChildren)
   {
      i.second->UpdateQualifiedName();
   }
}

//! Update the relative name of a child of this formation.
//!
//! This will also update the qualified names of the children of the modified formation, as well
//! as keeping the formation manager apprised of the modifications.
//!
//! \param aOldName - the old relative name of the child
//! \param aNewName - the new relative name of the child
//! \returns        - true on successful update; false otherwise.
bool wsf::six_dof::Formation::UpdateChildRelativeName(const std::string& aOldName, const std::string& aNewName)
{
   bool retval{false};
   if (aNewName.empty())
   {
      auto out = ut::log::warning() << "Cannot give child formation empty relative name.";
      out.AddNote() << "Parent: " << GetQualifiedName();
      out.AddNote() << "Child: " << aOldName;
   }
   else
   {
      auto i = mChildren.find(aOldName);
      auto j = mChildren.find(aNewName);
      if (i == mChildren.end())
      {
         auto out = ut::log::warning() << "Cannot rename child formation. The child could not be found.";
         out.AddNote() << "Parent: " << GetQualifiedName();
         out.AddNote() << "Child: " << aOldName;
      }
      else if (j != mChildren.end())
      {
         auto out = ut::log::warning() << "Cannot rename child formation. There is already a child with the new name.";
         out.AddNote() << "Parent: " << GetQualifiedName();
         out.AddNote() << "Old Name: " << aOldName;
         out.AddNote() << "New Name: " << aNewName;
      }
      else
      {
         mChildren[aNewName]        = mChildren[aOldName];
         mChildren[aNewName]->mName = aNewName;
         mChildren[aNewName]->UpdateQualifiedName();
         mChildren.erase(i);
         retval = true;
      }
   }
   return retval;
}

void wsf::six_dof::Formation::AddDescendantNames(std::vector<std::string>& aNames) const
{
   aNames.push_back(GetQualifiedName());
   for (auto& i : mChildren)
   {
      i.second->AddDescendantNames(aNames);
   }
}

bool wsf::six_dof::Formation::SetMemberNameP(const std::string& aPlatformName)
{
   return false;
}
