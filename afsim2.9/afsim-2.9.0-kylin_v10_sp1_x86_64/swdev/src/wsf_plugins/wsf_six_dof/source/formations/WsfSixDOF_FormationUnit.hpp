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

#ifndef WSF_SIXDOF_FORMATIONUNIT_HPP
#define WSF_SIXDOF_FORMATIONUNIT_HPP

#include "wsf_six_dof_export.h"

#include <string>

#include "WsfSixDOF_Formation.hpp"

class UtInput;
class WsfPlatform;

namespace wsf
{
namespace six_dof
{
//! The unit formation is the leaf of the formation tree.
//! It cannot have sub-formations, and it can have a single member platform.
class WSF_SIX_DOF_EXPORT FormationUnit : public Formation
{
public:
   FormationUnit(FormationManager& aManager, const std::string& aName);
   ~FormationUnit() override = default;

   std::unique_ptr<Formation> Duplicate(FormationManager&  aManager,
                                        Formation*         aParentPtr,
                                        const std::string& aQualifiedName,
                                        const std::string& aName) const override;
   bool                       ProcessInput(UtInput& aInput) override;

   //! Return the member platform's name.
   std::string GetMemberPlatformName() const override { return mPlatformName; }

   WsfPlatform* GetMemberPlatform() const override;
   Mover*       GetMemberMover() const override;
   bool         HasMemberPlatform() const override;

   //! Return that this is a unit formation.
   bool IsUnitFormation() const override { return true; }

   int        GetNumMembers() const override;
   bool       AddChildFormation(Formation* aChildPtr) override;
   Formation* RemoveChildFormation(const std::string& aName, bool aRename = true) override;
   bool       SetLead(const std::string& aName) override;

protected:
   FormationUnit(const FormationUnit& aOther,
                 FormationManager&    aManager,
                 Formation*           aParentPtr,
                 const std::string&   aQualifiedName,
                 const std::string&   aName);

private:
   bool SetMemberNameP(const std::string& aPlatformName) override;

   std::string mPlatformName; //! The name of the platform in this unit formation.
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONUNIT_HPP
