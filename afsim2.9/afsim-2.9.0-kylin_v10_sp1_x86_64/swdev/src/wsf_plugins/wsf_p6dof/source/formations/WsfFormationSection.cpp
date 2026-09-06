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

#include "WsfFormationSection.hpp"

#include "UtLog.hpp"

WsfFormationSection::WsfFormationSection(WsfFormationManager& aManager, const std::string& aName)
   : WsfFormation(aManager, aName)
{
}

WsfFormationSection::WsfFormationSection(const WsfFormationSection& aOther,
                                         WsfFormationManager&       aManager,
                                         WsfFormation*              aParentPtr,
                                         const std::string&         aQualifiedName,
                                         const std::string&         aName)
   : WsfFormation(aOther, aManager, aParentPtr, aQualifiedName, aName)
{
}

std::unique_ptr<WsfFormation> WsfFormationSection::Duplicate(WsfFormationManager& aManager,
                                                             WsfFormation*        aParentPtr,
                                                             const std::string&   aQualifiedName,
                                                             const std::string&   aName) const
{
   return std::unique_ptr<WsfFormation>(new WsfFormationSection(*this, aManager, aParentPtr, aQualifiedName, aName));
}

//! Add the given formation as a child with the given name.
//!
//! Add the formation @p aChildPtr to this formation as a child. The provided formation
//! must be a unit formation. If the formation does not currently have any children, this
//! will set the given formation as the leader.
//!
//! \param aChildPtr - The formation to add as a child.
//! \returns         - true if successfully added; false otherwise.
bool WsfFormationSection::AddChildFormation(WsfFormation* aChildPtr)
{
   bool retval{false};
   if (GetNumChildFormations() < 2)
   {
      if (aChildPtr->GetNumChildFormations() == 0 && aChildPtr->GetNumMembers() == 1)
      {
         retval = WsfFormation::AddChildFormation(aChildPtr);
      }
      else
      {
         auto out = ut::log::warning() << "Cannot add non-unit formation to section.";
         out.AddNote() << "Formation: " << aChildPtr->GetQualifiedName();
         out.AddNote() << "Section: " << GetQualifiedName();
      }
   }
   else
   {
      auto out = ut::log::warning() << "Cannot add third formation to section.";
      out.AddNote() << "Formation: " << aChildPtr->GetQualifiedName();
      out.AddNote() << "Section: " << GetQualifiedName();
   }
   return retval;
}
