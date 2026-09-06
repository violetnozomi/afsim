// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFSHADOWMOVER_HPP
#define WSFSHADOWMOVER_HPP

#include "wsf_export.h"

#include "WsfMover.hpp"
class UtInput;
class WsfPlatform;

/**
   A 'shadow mover' gets its position from some other platform.
*/

class WSF_EXPORT WsfShadowMover : public WsfMover
{
public:
   WsfShadowMover() = delete;
   WsfShadowMover(const WsfPlatform* aPlatformPtr);
   ~WsfShadowMover() override = default;

   WsfMover* Clone() const override;

   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime) override;

private:
   size_t mPlatformIndex;
};

#endif
