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

#include "WsfSixDOF_FormationSection.hpp"

#include "UtLog.hpp"

wsf::six_dof::FormationSection::FormationSection(FormationManager& aManager, const std::string& aName)
   : Formation(aManager, aName)
{
}

wsf::six_dof::FormationSection::FormationSection(const FormationSection& aOther,
                                                 FormationManager&       aManager,
                                                 Formation*              aParentPtr,
                                                 const std::string&      aQualifiedName,
                                                 const std::string&      aName)
   : Formation(aOther, aManager, aParentPtr, aQualifiedName, aName)
{
}

std::unique_ptr<wsf::six_dof::Formation> wsf::six_dof::FormationSection::Duplicate(FormationManager&  aManager,
                                                                                   Formation*         aParentPtr,
                                                                                   const std::string& aQualifiedName,
                                                                                   const std::string& aName) const
{
   return std::unique_ptr<Formation>(new FormationSection(*this, aManager, aParentPtr, aQualifiedName, aName));
}

//! Add the given formation as a child with the given name.
//!
//! Add the formation @p aChildPtr to this formation as a child. The provided formation
//! must be a unit formation. If the formation does not currently have any children, this
//! will set the given formation as the leader.
//!
//! \param aChildPtr - The formation to add as a child.
//! \returns         - true if successfully added; false otherwise.
bool wsf::six_dof::FormationSection::AddChildFormation(Formation* aChildPtr)
{
   bool retval{false};
   if (GetNumChildFormations() < 2)
   {
      if (aChildPtr->GetNumChildFormations() == 0 && aChildPtr->GetNumMembers() == 1)
      {
         retval = Formation::AddChildFormation(aChildPtr);
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
