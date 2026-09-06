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

#ifndef WSFFORMATIONUNIT_HPP
#define WSFFORMATIONUNIT_HPP

#include "wsf_p6dof_export.h"

#include <string>

#include "WsfFormation.hpp"

class UtInput;
class WsfPlatform;

//! The unit formation is the leaf of the formation tree.
//! It cannot have sub-formations, and it can have a single member platform.
class WSF_P6DOF_EXPORT WsfFormationUnit : public WsfFormation
{
public:
   WsfFormationUnit(WsfFormationManager& aManager, const std::string& aName);
   ~WsfFormationUnit() override = default;

   std::unique_ptr<WsfFormation> Duplicate(WsfFormationManager& aManager,
                                           WsfFormation*        aParentPtr,
                                           const std::string&   aQualifiedName,
                                           const std::string&   aName) const override;
   bool                          ProcessInput(UtInput& aInput) override;

   //! Return the member platform's name.
   std::string GetMemberPlatformName() const override { return mPlatformName; }

   WsfPlatform*    GetMemberPlatform() const override;
   WsfP6DOF_Mover* GetMemberMover() const override;
   bool            HasMemberPlatform() const override;

   //! Return that this is a unit formation.
   bool IsUnitFormation() const override { return true; }

   int           GetNumMembers() const override;
   bool          AddChildFormation(WsfFormation* aChildPtr) override;
   WsfFormation* RemoveChildFormation(const std::string& aName, bool aRename = true) override;
   bool          SetLead(const std::string& aName) override;

protected:
   WsfFormationUnit(const WsfFormationUnit& aOther,
                    WsfFormationManager&    aManager,
                    WsfFormation*           aParentPtr,
                    const std::string&      aQualifiedName,
                    const std::string&      aName);

private:
   bool SetMemberNameP(const std::string& aPlatformName) override;

   std::string mPlatformName; //! The name of the platform in this unit formation.
};

#endif // WSFFORMATIONUNIT_HPP
