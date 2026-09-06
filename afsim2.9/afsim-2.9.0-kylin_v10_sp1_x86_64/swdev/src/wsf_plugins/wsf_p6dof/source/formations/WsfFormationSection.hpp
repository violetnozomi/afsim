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

#ifndef WSFFORMATIONSECTION_HPP
#define WSFFORMATIONSECTION_HPP

#include "wsf_p6dof_export.h"

#include "WsfFormation.hpp"

class UtInput;

//! A section is a formation that allows two unit sub-formations.
class WSF_P6DOF_EXPORT WsfFormationSection : public WsfFormation
{
public:
   WsfFormationSection(WsfFormationManager& aManager, const std::string& aName);
   ~WsfFormationSection() override = default;

   std::unique_ptr<WsfFormation> Duplicate(WsfFormationManager& aManager,
                                           WsfFormation*        aParentPtr,
                                           const std::string&   aQualifiedName,
                                           const std::string&   aName) const override;

   bool AddChildFormation(WsfFormation* aChildPtr) override;

protected:
   WsfFormationSection(const WsfFormationSection& aOther,
                       WsfFormationManager&       aManager,
                       WsfFormation*              aParentPtr,
                       const std::string&         aQualifiedName,
                       const std::string&         aName);
};

#endif // WSFFORMATIONSECTION_HPP
