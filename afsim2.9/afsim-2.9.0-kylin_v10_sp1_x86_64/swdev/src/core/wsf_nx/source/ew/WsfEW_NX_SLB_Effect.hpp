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

#ifndef WsfEW_NX_SLB_Effect_HPP
#define WsfEW_NX_SLB_Effect_HPP

#include <map>
#include <vector>

class UtInput;

class WsfEW_EA;
class WsfEW_EP;
#include "WsfEM_Interaction.hpp"
class WsfEM_Rcvr;
#include "ew/WsfEW_SLB_Effect.hpp"
class WsfAntennaPattern;


//! WsfEW_NX_SLB_Effect models and applies Sidelobe Blanker (SLB) effects.

class WsfEW_NX_SLB_Effect : public WsfEW_SLB_Effect
{
public:
   WsfEW_NX_SLB_Effect();
   WsfEW_NX_SLB_Effect& operator=(const WsfEW_NX_SLB_Effect&) = delete;
   ~WsfEW_NX_SLB_Effect() override                            = default;
   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   bool ProcessTargetBlankingInput(UtInput& aInput) override;

protected:
   WsfEW_NX_SLB_Effect(const WsfEW_NX_SLB_Effect& aSrc) = default;

private:
   unsigned int mNumberCellsBlanked; //!< The number of consecutive cells blocked when pulse jamming is perceived.
};

#endif
