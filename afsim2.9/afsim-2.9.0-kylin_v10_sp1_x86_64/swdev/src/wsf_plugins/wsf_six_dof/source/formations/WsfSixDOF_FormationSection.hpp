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

#ifndef WSF_SIXDOF_FORMATIONSECTION_HPP
#define WSF_SIXDOF_FORMATIONSECTION_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Formation.hpp"

class UtInput;

namespace wsf
{
namespace six_dof
{
//! A section is a formation that allows two unit sub-formations.
class WSF_SIX_DOF_EXPORT FormationSection : public Formation
{
public:
   FormationSection(FormationManager& aManager, const std::string& aName);
   ~FormationSection() override = default;

   std::unique_ptr<Formation> Duplicate(FormationManager&  aManager,
                                        Formation*         aParentPtr,
                                        const std::string& aQualifiedName,
                                        const std::string& aName) const override;

   bool AddChildFormation(Formation* aChildPtr) override;

protected:
   FormationSection(const FormationSection& aOther,
                    FormationManager&       aManager,
                    Formation*              aParentPtr,
                    const std::string&      aQualifiedName,
                    const std::string&      aName);
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONSECTION_HPP
