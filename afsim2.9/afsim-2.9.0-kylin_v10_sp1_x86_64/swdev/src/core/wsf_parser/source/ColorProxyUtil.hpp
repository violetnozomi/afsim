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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef COLORPROXYUTIL_HPP
#define COLORPROXYUTIL_HPP

#include "UtColor.hpp"
#include "UtOptional.hpp"
#include "WsfPProxyNode.hpp"

namespace ColorProxyUtil
{
ut::optional<UtColor> GetColor(const WsfPProxyNode& aColorNode);
}

#endif
