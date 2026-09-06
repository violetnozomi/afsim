// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfEW_NX_SLB_Effect.hpp"

#include <cfloat>
#include <map>
#include <string>
#include <vector>

#include "UtAzElLookup.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtVec2.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_PulseEffect.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSimulation.hpp"

WsfEW_NX_SLB_Effect::WsfEW_NX_SLB_Effect()
   : WsfEW_SLB_Effect()
   , mNumberCellsBlanked(1)
{
   mCoherencyMask = cEC_NONE | cEC_NONCOHERENT | cEC_COHERENT | cEC_NONCOHERENT_PULSE | cEC_COHERENT_PULSE;
   mBehavior      = cEB_JAMMER_POWER_EFFECT;
}

// virtual
WsfEW_Effect* WsfEW_NX_SLB_Effect::Clone() const
{
   return new WsfEW_NX_SLB_Effect(*this);
}

bool WsfEW_NX_SLB_Effect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = true;

   if (mTargetBlankingEffect == cTB_DUTYCYCLEALGORITHM)
   {
      double dutyCycle(0.0);
      while (dutyCycle <= 1.0)
      {
         double blankProb(0.0);
         for (unsigned int cell = 1; cell <= mNumberCellsBlanked; ++cell)
         {
            blankProb += dutyCycle * (1 - blankProb);
         }
         AddTargetBlankingProbability(dutyCycle, blankProb);
         dutyCycle += 0.01;
      }
   }

   ok &= WsfEW_SLB_Effect::Initialize(aSimulation);

   return ok;
}

bool WsfEW_NX_SLB_Effect::ProcessInput(UtInput& aInput)
{
   return WsfEW_SLB_Effect::ProcessInput(aInput);
}

bool WsfEW_NX_SLB_Effect::ProcessTargetBlankingInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "cell_blanking_probability_effect")
   {
      mTargetBlankingEffect = cTB_DUTYCYCLEALGORITHM;
      mTargetBlankingTable.clear();
   }
   else if (command == "number_cells_blanked")
   {
      if (mTargetBlankingEffect != cTB_DUTYCYCLEALGORITHM)
      {
         throw UtInput::BadValue(aInput,
                                 GetName() + " 'number_range_cells' only valid for cell_blanking_probability_effect.");
      }
      aInput.ReadValue(mNumberCellsBlanked);
      aInput.ValueGreaterOrEqual(mNumberCellsBlanked, 1U);
   }
   else
   {
      myCommand = WsfEW_SLB_Effect::ProcessTargetBlankingInput(aInput);
   }
   return myCommand;
}
