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

#include "WsfFormationChangeOffsetCommand.hpp"

#include <algorithm>

#include "UtLog.hpp"
#include "WsfFormation.hpp"

WsfFormationChangeOffsetCommand::WsfFormationChangeOffsetCommand()
   : mOffsets(1)
   , mTransitionTime{0.0}
   , mInitialTime{-1.0}
{
}

WsfFormationChangeOffsetCommand* WsfFormationChangeOffsetCommand::Clone() const
{
   return new WsfFormationChangeOffsetCommand{*this};
}

double WsfFormationChangeOffsetCommand::Execute(double aSimTime)
{
   WsfFormation* formationPtr = GetFormation();
   if (mInitialTime < 0.0)
   {
      mInitialTime    = aSimTime;
      mOffsets[0]     = formationPtr->GetOffset();
      mTransitionTime = std::max(0.01, mTransitionTime);
   }

   double deltaT = aSimTime - mInitialTime;
   double retval{-1.0};
   if (deltaT < mTransitionTime)
   {
      double intervalDelta   = mTransitionTime / (mOffsets.size() - 1);
      int    interval        = static_cast<int>(floor(deltaT / intervalDelta));
      double deltaInInterval = deltaT - intervalDelta * interval;
      double blending        = deltaInInterval / intervalDelta;
      double blendComp       = 1.0 - blending;

      double ahead  = blendComp * mOffsets[interval].GetAhead() + blending * mOffsets[interval + 1].GetAhead();
      double right  = blendComp * mOffsets[interval].GetRight() + blending * mOffsets[interval + 1].GetRight();
      double stack  = blendComp * mOffsets[interval].GetStack() + blending * mOffsets[interval + 1].GetStack();
      bool   welded = mOffsets[interval + 1].IsWelded();

      WsfFormationOffset offset{};
      offset.SetAhead(ahead);
      offset.SetRight(right);
      offset.SetStack(stack);
      offset.SetWelded(welded);

      formationPtr->SetOffset(offset);

      // Make the transition in 100 steps, but update no more frequently than
      // 10 Hz.
      retval = aSimTime + std::max(0.1, mTransitionTime / 100.0);
   }
   else
   {
      formationPtr->SetOffset(mOffsets[mOffsets.size() - 1]);
   }

   return retval;
}

bool WsfFormationChangeOffsetCommand::AcceptCommand(WsfFormation* aFormationPtr)
{
   bool retval{true};
   if (aFormationPtr->IsRoot())
   {
      auto out = ut::log::warning() << "Changing the offset of a top-level formation has no effect.";
      out.AddNote() << "Command: " << Type();
      out.AddNote() << "This command will be ignored.";
      retval = false;
   }
   return retval;
}

void WsfFormationChangeOffsetCommand::ComputeCommonTransformation(WsfFormation* aFormationPtr) {}

std::unique_ptr<WsfFormationCommand> WsfFormationChangeOffsetCommand::ComputeTransformation(WsfFormation* aParentPtr,
                                                                                            WsfFormation* aChildPtr)
{
   return nullptr;
}

void WsfFormationChangeOffsetCommand::AddOffset(WsfFormationOffset& aOffset)
{
   mOffsets.push_back(aOffset);
}
