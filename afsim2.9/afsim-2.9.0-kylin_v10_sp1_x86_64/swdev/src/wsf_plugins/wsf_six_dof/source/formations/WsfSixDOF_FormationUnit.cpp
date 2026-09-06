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

#include "WsfSixDOF_FormationUnit.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_FormationManager.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::FormationUnit::FormationUnit(FormationManager& aManager, const std::string& aName)
   : Formation(aManager, aName)
   , mPlatformName{}
{
}

wsf::six_dof::FormationUnit::FormationUnit(const FormationUnit& aOther,
                                           FormationManager&    aManager,
                                           Formation*           aParentPtr,
                                           const std::string&   aQualifiedName,
                                           const std::string&   aName)
   : Formation(aOther, aManager, aParentPtr, aQualifiedName, aName)
   , mPlatformName{aOther.mPlatformName}
{
}

std::unique_ptr<wsf::six_dof::Formation> wsf::six_dof::FormationUnit::Duplicate(FormationManager&  aManager,
                                                                                Formation*         aParentPtr,
                                                                                const std::string& aQualifiedName,
                                                                                const std::string& aName) const
{
   return std::unique_ptr<Formation>(new FormationUnit(*this, aManager, aParentPtr, aQualifiedName, aName));
}

bool wsf::six_dof::FormationUnit::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command = aInput.GetCommand();
   if (command == "debug")
   {
      SetDebug(true);
   }
   else if (command == "member_platform")
   {
      // Note that this member will be validated to have a SixDOF mover by the formation manager.
      aInput.ReadCommand(mPlatformName);
   }
   else if (command == "offset")
   {
      FormationOffset offset{};
      myCommand = offset.ProcessInput(aInput);
      SetOffset(offset);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

//! Return the member platform.
//!
//! This will return the member platform of this formation. This will operate
//! in one of two modes based on when in the application lifecycle this is
//! called. During initialization, before a simulation has begun, this will
//! return the input platform. After initialization, when a simulation has begun,
//! this will return the platform instance in that simulation. If there is
//! no such platform, or if that platform has been removed from the simulation,
//! this method will return nullptr.
//!
//! \returns - the platform, or nullptr.
WsfPlatform* wsf::six_dof::FormationUnit::GetMemberPlatform() const
{
   WsfPlatform* retval{nullptr};
   if (HasMemberPlatform())
   {
      WsfSimulation* simPtr = GetManager().GetSimulation();
      if (simPtr)
      {
         retval = simPtr->GetPlatformByName(mPlatformName);
      }
      else
      {
         WsfScenario& scen = GetManager().GetScenario();
         retval            = scen.GetInputPlatformByName(mPlatformName);
      }
   }
   return retval;
}

//! Get the mover associated with the member of the formation.
//!
//! If there is trouble getting the underlying member platform,
//! this method will return a null pointer.
//!
//! \returns - the mover for the member platform of this formation.
wsf::six_dof::Mover* wsf::six_dof::FormationUnit::GetMemberMover() const
{
   Mover*       retval{nullptr};
   WsfPlatform* platPtr = GetMemberPlatform();
   if (platPtr)
   {
      WsfMover* mvrPtr = platPtr->GetMover();
      // NOTE: We know this is a safe cast because we only allow members to be set that have
      // a valid mover.
      retval = static_cast<Mover*>(mvrPtr);
   }
   return retval;
}

//! Return if this platform has a member assigned to it.
bool wsf::six_dof::FormationUnit::HasMemberPlatform() const
{
   return !mPlatformName.empty();
}

//! Return the number of member platforms in this formation.
int wsf::six_dof::FormationUnit::GetNumMembers() const
{
   int retval{0};
   if (!mPlatformName.empty())
   {
      retval = 1;
   }
   return retval;
}

//! Add the given formation as a child with the given name.
//!
//! For a unit formation, this does nothing, and will always return false.
//!
//! \returns - false.
bool wsf::six_dof::FormationUnit::AddChildFormation(Formation*)
{
   return false;
}

//! Remove a child formation.
//!
//! For a unit formation, this does nothing, and will always return a nullptr.
//!
//! \returns - nullptr.
wsf::six_dof::Formation* wsf::six_dof::FormationUnit::RemoveChildFormation(const std::string&, bool)
{
   return nullptr;
}

//! Set the lead formation.
//!
//! For a unit formation, this does nothing, and will always return false;
//!
//! \returns     - false.
bool wsf::six_dof::FormationUnit::SetLead(const std::string&)
{
   return false;
}

//! Set the member platform name.
//!
//! This will verify that a platform with the given @p aPlatformName exists, and that
//! the mover on said platform is a Mover before setting the member of this
//! formation.
//!
//! \param aPlatformName - the name of the member to set on this formation.
//! \returns             - true if such a platform exists and it has a Mover; false otherwise.
bool wsf::six_dof::FormationUnit::SetMemberNameP(const std::string& aPlatformName)
{
   bool         retval{false};
   WsfPlatform* platPtr = GetManager().GetSimulation()->GetPlatformByName(aPlatformName);
   if (platPtr)
   {
      Mover* mvrPtr = dynamic_cast<Mover*>(platPtr->GetMover());
      if (mvrPtr)
      {
         mPlatformName = aPlatformName;
         retval        = true;
      }
      else
      {
         auto out = ut::log::warning() << "Formation member platform must have a Mover.";
         out.AddNote() << "Formation: " << GetQualifiedName();
         out.AddNote() << "Platform: " << aPlatformName;
      }
   }
   else
   {
      auto out = ut::log::warning() << "Cannot set member platform for formation. Platform not found.";
      out.AddNote() << "Formation: " << GetQualifiedName();
      out.AddNote() << "Platform: " << aPlatformName;
   }
   return retval;
}
