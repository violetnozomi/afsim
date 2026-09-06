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

#ifndef WSF_SIXDOF_FORMATIONCHANGEOFFSETCOMMAND_HPP
#define WSF_SIXDOF_FORMATIONCHANGEOFFSETCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include <vector>

#include "WsfSixDOF_FormationCommand.hpp"
#include "WsfSixDOF_FormationOffset.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT FormationChangeOffsetCommand : public FormationCommand
{
public:
   FormationChangeOffsetCommand();
   FormationChangeOffsetCommand(const FormationChangeOffsetCommand& aOther) = default;
   ~FormationChangeOffsetCommand() override                                 = default;

   FormationChangeOffsetCommand* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfSixDOF_FormationChangeOffset"; }

   std::string Type() const override { return "CHANGE-OFFSET"; }

   bool IsInstantaneous() const override { return false; }
   bool IsSequence() const override { return false; }

   double                            Execute(double aSimTime) override;
   bool                              AcceptCommand(Formation* aFormationPtr) override;
   bool                              CanPropagate() const override { return false; }
   void                              ComputeCommonTransformation(Formation* aFormationPtr) override;
   std::unique_ptr<FormationCommand> ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr) override;

   // add and remove offset values
   void            AddOffset(FormationOffset& aOffset);
   FormationOffset GetOffset(int aOffsetIndex) { return mOffsets[aOffsetIndex + 1]; }
   size_t          GetOffsetCount() const { return mOffsets.size() - 1; }

   double GetTransitionTime() const { return mTransitionTime; }
   void   SetTransitionTime(double aTransitionTime) { mTransitionTime = aTransitionTime; }

private:
   // some kind of way to store the offsets on the path
   std::vector<FormationOffset> mOffsets;
   double                       mTransitionTime;
   double                       mInitialTime;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONCHANGEOFFSETCOMMAND_HPP
