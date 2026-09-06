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

#ifndef WSF_SIXDOF_FORMATIONUPDATESTATIONKEEPING_HPP
#define WSF_SIXDOF_FORMATIONUPDATESTATIONKEEPING_HPP

namespace wsf
{
namespace six_dof
{
class Formation;

class FormationUpdateStationKeeping
{
public:
   static void Update(Formation* aFormationPtr, double aSimTime, bool aJumpDetached = false);
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONUPDATESTATIONKEEPING_HPP
