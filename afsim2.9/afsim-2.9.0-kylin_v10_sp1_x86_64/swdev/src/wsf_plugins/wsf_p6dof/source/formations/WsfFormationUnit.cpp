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

#include "WsfFormationUnit.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfFormationManager.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

WsfFormationUnit::WsfFormationUnit(WsfFormationManager& aManager, const std::string& aName)
   : WsfFormation(aManager, aName)
   , mPlatformName{}
{
}

WsfFormationUnit::WsfFormationUnit(const WsfFormationUnit& aOther,
                                   WsfFormationManager&    aManager,
                                   WsfFormation*           aParentPtr,
                                   const std::string&      aQualifiedName,
                                   const std::string&      aName)
   : WsfFormation(aOther, aManager, aParentPtr, aQualifiedName, aName)
   , mPlatformName{aOther.mPlatformName}
{
}

std::unique_ptr<WsfFormation> WsfFormationUnit::Duplicate(WsfFormationManager& aManager,
                                                          WsfFormation*        aParentPtr,
                                                          const std::string&   aQualifiedName,
                                                          const std::string&   aName) const
{
   return std::unique_ptr<WsfFormation>(new WsfFormationUnit(*this, aManager, aParentPtr, aQualifiedName, aName));
}

bool WsfFormationUnit::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command = aInput.GetCommand();
   if (command == "debug")
   {
      SetDebug(true);
   }
   else if (command == "member_platform")
   {
      // Note that this member will be validated to have a P6DOF mover by the formation manager.
      aInput.ReadCommand(mPlatformName);
   }
   else if (command == "offset")
   {
      WsfFormationOffset offset{};
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
WsfPlatform* WsfFormationUnit::GetMemberPlatform() const
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
WsfP6DOF_Mover* WsfFormationUnit::GetMemberMover() const
{
   WsfP6DOF_Mover* retval{nullptr};
   WsfPlatform*    platPtr = GetMemberPlatform();
   if (platPtr)
   {
      WsfMover* mvrPtr = platPtr->GetMover();
      // NOTE: We know this is a safe cast because we only allow members to be set that have
      // a P6DOF mover.
      retval = static_cast<WsfP6DOF_Mover*>(mvrPtr);
   }
   return retval;
}

//! Return if this platform has a member assigned to it.
bool WsfFormationUnit::HasMemberPlatform() const
{
   return !mPlatformName.empty();
}

//! Return the number of member platforms in this formation.
int WsfFormationUnit::GetNumMembers() const
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
bool WsfFormationUnit::AddChildFormation(WsfFormation*)
{
   return false;
}

//! Remove a child formation.
//!
//! For a unit formation, this does nothing, and will always return a nullptr.
//!
//! \returns - nullptr.
WsfFormation* WsfFormationUnit::RemoveChildFormation(const std::string&, bool)
{
   return nullptr;
}

//! Set the lead formation.
//!
//! For a unit formation, this does nothing, and will always return false;
//!
//! \returns     - false.
bool WsfFormationUnit::SetLead(const std::string&)
{
   return false;
}

//! Set the member platform name.
//!
//! This will verify that a platform with the given @p aPlatformName exists, and that
//! the mover on said platform is a WsfP6DOF_Mover before setting the member of this
//! formation.
//!
//! \param aPlatformName - the name of the member to set on this formation.
//! \returns             - true if such a platform exists and it has a WsfP6DOF_Mover; false otherwise.
bool WsfFormationUnit::SetMemberNameP(const std::string& aPlatformName)
{
   bool         retval{false};
   WsfPlatform* platPtr = GetManager().GetSimulation()->GetPlatformByName(aPlatformName);
   if (platPtr)
   {
      WsfP6DOF_Mover* mvrPtr = dynamic_cast<WsfP6DOF_Mover*>(platPtr->GetMover());
      if (mvrPtr)
      {
         mPlatformName = aPlatformName;
         retval        = true;
      }
      else
      {
         auto out = ut::log::warning() << "Formation member platform must have a WsfP6DOF_Mover.";
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
