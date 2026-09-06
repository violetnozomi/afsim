// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "effects/WsfCyberEffect.hpp"

#include "UtInput.hpp"

namespace wsf
{
namespace cyber
{

// =================================================================================================
// virtual
bool Effect::Initialize(Engagement&)
{
   return true;
}

// =================================================================================================
// virtual
bool Effect::ProcessInput(UtInput& aInput)
{
   return WsfObject::ProcessInput(aInput);
}

} // namespace cyber
} // namespace wsf
